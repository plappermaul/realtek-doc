/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "platform_autoconf.h"
#include "diag.h"
//#include "rtl8195a.h"


//#include "basic_types.h"
//#include "diag.h"
#include <common.h>
#include "cryptoSim.h"

#include "rtk_crypto_test.h"
#include "rtk_crypto_rom.h"
//#include "section_config.h"
//#include "FreeRTOS.h"
//#include "hal_timer.h"
#include <asm/mipsregs.h>
#define  CAUSEB_DC              27
#define  CAUSEF_DC              (_ULCAST_(1)   << 27)

#define DISABLE_AMEBA_NEW_ARCH 1
#define CPU_CACHE_LINE_SIZE 32

//unit Mhz
//#define CPU_CLOCK_RATE	40
#define CPU_CLOCK_RATE	800
//#define CPU_CLOCK_RATE	1000
#define CPU_CP0_COUNT_RATE (CPU_CLOCK_RATE/2)

//#define CRYPTO_TEST_SRAM 1
//#define CRYPTO_TEST_NOT_ALIGN 0 //select 0~3
//#define CRYPTO_TEST_DMA_MODE 2
int dbg_level = 3;
//#define CRYPTO_ENABLE_TIMER_MEASUER 1

//#define CRYPTO_FORCE_MODE 1
#define FORCE_modeCrypto 1
#define FORCE_modeAuth 3
#define FORCE_lenA2eo 208
#define FORCE_pktLen 216

int tstc(void);
int getc(void);
unsigned int get_rand_seed(void)
{
	return 0xabcd1234;
}

void rtlglue_srandom( u32 seed )
{

	rtl_srandom( seed );

}

u32 rtlglue_random( void )
{

	return rtl_random( );

}

#ifndef DISABLE_AMEBA_NEW_ARCH
extern HAL_TIMER_OP HalTimerOp;
#endif

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



u8 _asic_authkey[1024] 								__attribute__ ((aligned (CPU_CACHE_LINE_SIZE)));


u8 _asic_cryptkey[1024] 							__attribute__ ((aligned (CPU_CACHE_LINE_SIZE)));


u8 _asic_iv[1024]									__attribute__ ((aligned (CPU_CACHE_LINE_SIZE)));


u8 _asic_orig[32 + CRYPTO_MAX_MSG_LENGTH + 8 + 32] __attribute__ ((aligned (CPU_CACHE_LINE_SIZE)));


u8 _asic_enc[32 + CRYPTO_MAX_MSG_LENGTH + 32] 		__attribute__ ((aligned (CPU_CACHE_LINE_SIZE)));


u8 _sw_orig[CRYPTO_MAX_MSG_LENGTH + 32] 					__attribute__ ((aligned (CPU_CACHE_LINE_SIZE)));


u8 _sw_enc[32 + CRYPTO_MAX_MSG_LENGTH + 32]			__attribute__ ((aligned (CPU_CACHE_LINE_SIZE)));


u8 _asic_dec[32 + CRYPTO_MAX_MSG_LENGTH + 32]		__attribute__ ((aligned (CPU_CACHE_LINE_SIZE)));



u8 _sw_dec[32 + CRYPTO_MAX_MSG_LENGTH + 32]			__attribute__ ((aligned (CPU_CACHE_LINE_SIZE)));


u8 _AsicDigest[32+CRYPTO_SHA2_DIGEST_LENGTH+32] 	__attribute__ ((aligned (4)));


u8 sw_digest[CRYPTO_SHA2_DIGEST_LENGTH]				__attribute__ ((aligned (4)));

u8 crypt_keylen_sel=2;

/******************************************************************************
 * External Function
 ******************************************************************************/

extern u32 
Strtoul(
    IN  const u8 *nptr,
    IN  u8 **endptr,
    IN  u32 base
);


/******************************************************************************
 * Functions
 ******************************************************************************/



int32 ipsecSim(uint32 modeCrypto, uint32 modeAuth,
	uint8 *data, uint8 *pCryptResult,
	uint32 lenCryptoKey, uint8 *pCryptoKey, 
	uint32 lenAuthKey, uint8 *pAuthKey, 
	uint8 *pIv, uint8 *pPad, uint8 *pDigest,
	uint32 lenA2eo, uint32 lenEnl)
{
	uint32 digestlen=0;

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
			digestlen = CRYPTO_MD5_DIGEST_LENGTH;
			//return FAILED;
	}
//printf("(%s-%d)\n", __func__, __LINE__);
	
	if ((modeAuth != (uint32) -1) &&
		(modeCrypto != (uint32) -1))
	{
		//printf("(%s-%d)\n", __func__, __LINE__);

		if (modeCrypto & 0x80) // encrypt then auth
		{
	#ifdef CRYPTO_LOOPBACK_DEBUG
			memcpy(pCryptResult, data, lenA2eo + lenEnl);
	#else
			if (modeCrypto & 0x20) // aes 
				aesSim_aes(modeCrypto, data + lenA2eo, lenEnl, lenCryptoKey,
					pCryptoKey, pIv, pCryptResult + lenA2eo);
			else // des
				desSim_des(modeCrypto, data + lenA2eo, lenEnl,
						pCryptoKey, pIv, pCryptResult + lenA2eo);
//printf("(%s-%d)\n", __func__, __LINE__);

			memcpy(pCryptResult, data, lenA2eo);
	#endif
//printf("(%s-%d)\n", __func__, __LINE__);

			authSim((modeAuth&7), pCryptResult, lenA2eo + lenEnl,
				pAuthKey, lenAuthKey, pDigest, digestlen);
		}
		else // auth then decrypt
		{
			authSim(modeAuth&7, data, lenA2eo + lenEnl,
				pAuthKey, lenAuthKey, pDigest, digestlen);
		
	#ifdef CRYPTO_LOOPBACK_DEBUG
			memcpy(pCryptResult, data, lenA2eo + lenEnl);
	#else
			if (modeCrypto & 0x20) // aes
				aesSim_aes(modeCrypto, data + lenA2eo, lenEnl, lenCryptoKey,
					 pCryptoKey, pIv, pCryptResult + lenA2eo);
			else // des
				desSim_des(modeCrypto, data + lenA2eo, lenEnl,
						pCryptoKey, pIv, pCryptResult + lenA2eo);

			memcpy(pCryptResult, data, lenA2eo);
	#endif
		}
	}
	else if (modeAuth != (uint32) -1) // auth only
	{
//		printf("(%s-%d)\n", __func__, __LINE__);

		authSim(modeAuth&7, data, lenA2eo + lenEnl,
			pAuthKey, lenAuthKey, pDigest, digestlen);
			
	}
	else if (modeCrypto != (uint32) -1) // crypto only
	{
//		printf("(%s-%d)\n", __func__, __LINE__);
	#ifdef CRYPTO_LOOPBACK_DEBUG
			memcpy(pCryptResult, data, lenA2eo + lenEnl);
	#else

		if (modeCrypto & 0x20) // aes
			aesSim_aes(modeCrypto, data + lenA2eo, lenEnl, lenCryptoKey,
				 pCryptoKey, pIv, pCryptResult + lenA2eo);
		else // des
			desSim_des(modeCrypto, data + lenA2eo, lenEnl,
				pCryptoKey, pIv, pCryptResult + lenA2eo);
	#endif
	}
	else
	{
		return FAILED;
	}

	return SUCCESS;
}


int rtl_crypto_auth_sim(const u32 authtype, 
	const u8* pKey, const u32 keylen, const u8* pText, const u32 textlen, u8* pDigest, const u32 digestlen)
{
    int     ret;

 
	ret = authSim(authtype, pText, textlen, pKey, keylen, pDigest, digestlen);
            

    return ret;
}



int rtl_crypto_esp_sim(const u32 modeCrypto, 
	const u8* pKey, const u32 keylen, const u8* pIv, const u32 ivLen, const u8* pText, const u32 textlen, u8* pResult)
{
    int     ret = SUCCESS;

    

	dbg_printk("modeCrypto=0x%x, pText=%p, pResult=%p, keylen=%d, pKey=%p, pIv=%p, ivLen=%d, pText=%p textlen=%d\n", 
		modeCrypto, pText, pResult, keylen, pKey, pIv, ivLen, pText, textlen);

	dbg_mem_dump(pText, textlen, "Text :");
	dbg_mem_dump(pKey, keylen, "Key :");
	dbg_mem_dump(pIv, ivLen, "IV :");

	#ifdef CRYPTO_LOOPBACK_DEBUG
	memcpy(pResult, pText, textlen);
	#else
	if ( (modeCrypto & 0x30) == 0x30 ) { // chacha20
#ifdef NEW_CRYPTO
		CRYPTO_sim_chacha_20(pResult, pText, textlen, pKey, pIv, 0);
#endif
	} else if ( (modeCrypto & 0x20) == 0x20 ) { // aes
		ret = aesSim_aes(modeCrypto, pText,
			textlen, keylen, pKey, pIv, pResult);
	} else { // 3des / des
		ret = desSim_des(modeCrypto, pText,
			textlen, pKey, pIv, pResult);
	}
	#endif

    return ret;
}





void rtl_crypto_usage(char *type)
{

    if (type == NULL)
    {
        //printk("Usage: crypto <init|rand|throughput|asic|info|debug|vector|config>\n");
        printk("Usage: crypto <init|vector|test|rand|interrupt>\n");
    }
    else if (strcmp(type, "init") == 0)
    {        
        printk("Usage:" Crypto_CMD "init\n");
        printk("  max_desc:  default: 8 descriptors\n");
        printk("  dma_burst: default: 2 (64 bytes)\n");
    }
    else if (strcmp(type, "debug") == 0)
    {
        printk("Usage:" Crypto_CMD "debug [mode]\n");
        printk("  mode:  0: turn off, 1: turn on \n");
    }
    else if (strcmp(type, "vector") == 0)
    {
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
    }
    else if (strcmp(type, "test") == 0)
    {
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
    else if (strcmp(type, "rand") == 0)
    {
        printk("Usage: crypto rand [<seed> <round>] [<crypto_mode> <auth_mode>] [cntScatter]\n");
        printk("  seed: random seed\n");
        printk("  round: test round, default: 100\n");
        printk("  crypto_mode: 0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES \n");
        printk("             : 0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES\n");
        printk("             : 0xffffffff: no crypto, default: random \n");
        printk("  auth_mode: 0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1\n");
        printk("             : 0xffffffff: no auth, default: random\n");
        printk("  cntScatter: 1..MAX_SCATTER: count scatter, -1: random, default: -1\n");
    }
#if 0
    else if (strcmp(type, "throughput") == 0)
    {
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
    }
    else if (strcmp(type, "asic") == 0)
    {
        printk("Usage:" Crypto_CMD "asic <round> <len_start> <len_end>\n");
        printk("                   <off_start> <off_end>\n");
        printk("                   <crypto_start> <crypto_end>\n");
        printk("                   <auth_start> <auth_end>\n");
        printk("                   <ckeylen_start> <ckeylen_end>\n");
        printk("                   <akeylen_start> <akeylen_start>\n");
        printk("                   <a2eo_start> <a2eo_end>\n");
        printk("                   <cntScatter>\n");
    }
    else if (strcmp(type, "debug") == 0)
    {
        printk("Usage:" Crypto_CMD " debug <level>\n");
        printk("  level: 0: show error, 1: show success, 2: show skip, 3: show all, default: 0\n");
    }
    else if (strcmp(type, "config") == 0)
    {
        printk("Usage:" Crypto_CMD "config <config_name> <config_value>\n");
        printk("  config_name: sawb, config_value: 0 = disabled, 1 = enabled\n");
        printk("  config_name: fr, config_value: 0 = failed exit, 1 = do failed reset\n");
        printk("  config_name: seed, config_value: seed_value\n");
    }
#endif
}

int32 mix8651bAsicThroughput(int32 round, int32 cryptoStart, int32 cryptoEnd, int32 authStart, int32 authEnd,
	int32 CryptoKeyLen, int32 AuthKeyLen, int32 pktLen, int32 a2eoLen)
{
	uint32 testRound, i, bps;
	uint32 sTime, eTime;
	int32 cryptoIdx, authIdx;
	uint8 *cur;
	int32 lenEnl;
	//uint32 cntScatter = 0;
	//rtl_ipsecScatter_t src[8];
	#if 0//!defined(CRYPTOTEST_USE_UNCACHED_MALLOC)
	static uint8 bufAsic[32+MAX_PKTLEN+32];
	static uint8 _cryptoKey[32+256+32];
	static uint8 _authKey[32+MAX_AUTH_KEY+32];
	static uint8 _iv[32+32+32]; /* 256-bits */
	static uint8 _pad[32+128+32]; /* ipad + opad */
	static uint8 _AsicDigest[32+20+32]; /* max digest length */
	static uint8 _AsicCrypt[32+MAX_PKTLEN+32];
	uint8 *pCryptoKey = (void *) UNCACHED_ADDRESS( &_cryptoKey[32] );
	uint8 *pAuthKey = (void *) UNCACHED_ADDRESS( &_authKey[32] );
	uint8 *pIv = (void *) UNCACHED_ADDRESS( &_iv[32] );
	uint8 *pPad = (void *) UNCACHED_ADDRESS( &_pad[32] );
	uint8 *pAsicDigest = &_AsicDigest[32];
	uint8 *pAsicCrypt = (void *) UNCACHED_ADDRESS(&_AsicCrypt[32]);
	#else
	uint8 *pCryptoKey = (void *) UNCACHED_ADDRESS(&_asic_cryptkey[32]);
	uint8 *pAuthKey = (void *) UNCACHED_ADDRESS(&_asic_authkey[32]);
	uint8 *pIv = (void *) UNCACHED_ADDRESS(&_asic_iv[32]);
	//uint8 *pPad = pad; 
	uint8 *pAsicDigest = (void *) UNCACHED_ADDRESS(&_AsicDigest[32]);
	uint8 *pAsicCrypt = (void *) UNCACHED_ADDRESS(&_asic_enc[32]);
	#endif	


	uint32 mode_select;
	uint32 ivlen;
	uint32 digestlen=0;
	
#if 1//bruce
#define TEST_ROUND 1000

	if (round > TEST_ROUND) 
#endif
	{
	printf("Evaluate 8651b throughput(round=%d,cryptoStart=%d,cryptoEnd=%d,authStart=%d,authEnd=%d,"
		"CryptoKeyLen=%d,AuthKeyLen=%d,pktLen=%d,a2eoLen=%d)\n", round, cryptoStart, cryptoEnd, authStart, authEnd,
		CryptoKeyLen, AuthKeyLen, pktLen, a2eoLen);
	}

	/*  <--lenValid-->
	 *  | A2EO | ENL | APL
	 */
	lenEnl = pktLen - a2eoLen;

	if ( lenEnl <= 0 )
	{
		printf( "[IGNORE,lenEnl<=0]\n" );
		return FAILED;
	}
	
	//SMP_LOCK_IPSEC;
	#if defined(CRYPTOTEST_USE_UNCACHED_MALLOC)
	//rtl_ipsecTest_Init(); 
	#endif
	/* Simple pattern for debug */
	/* build keys and iv */
	for( i = 0; i < CryptoKeyLen; i++ )
		pCryptoKey[i] = i;
	for( i = 0; i < AuthKeyLen; i++ )
		pAuthKey[i] = i;
	for( i = 0; i < 16; i++ )
		pIv[i] = i;
	//for( i = 0; i < 128; i++ )
	//	pPad[i] = i;

	/* fill the test data */
	#if !defined(CRYPTOTEST_USE_UNCACHED_MALLOC)
	//cur = (void *) UNCACHED_ADDRESS( &bufAsic[0+32] );
	#else
	//cur = asic_orig;
	#endif
	cur = (void *) UNCACHED_ADDRESS(&_asic_orig[32]);
	for( i = 0; i < pktLen; i++ )
	{
		cur[i] = 0xff;
	}

#if 0//0104
	src[0].ptr = cur;
	src[0].len = pktLen/2;
	cntScatter = 2;

	src[1].ptr = cur+pktLen/2;
	src[1].len = pktLen/2;
#else
	//src[0].ptr = cur;
	//src[0].len = pktLen;
	//cntScatter = 1;
#endif

	for (cryptoIdx=cryptoStart; cryptoIdx<=cryptoEnd; cryptoIdx++) 
	{	
		if ( cryptoIdx==-1 || // none
		     cryptoIdx==0x00 || cryptoIdx==0x01 || cryptoIdx==0x02 || cryptoIdx==0x03 || // des/3des decode
		     cryptoIdx==0x80 || cryptoIdx==0x81 || cryptoIdx==0x82 || cryptoIdx==0x83 || // des/3des encode
		     cryptoIdx==0x20 || cryptoIdx==0x22 || cryptoIdx==0x23 || // aes decode
		     cryptoIdx==0xa0 || cryptoIdx==0xa2 || cryptoIdx==0xa3) // aes encode
		{
			/* valid mode, go on */
		}
		else
			continue;

		if ( cryptoIdx!=(uint32)-1 )
		{
			if ( cryptoIdx&0x20 )
			{ /* AES, ENL is the times of 16 bytes. */
				if ( lenEnl&0xf )
				{
					;
					printf( "[IGNORE,ENL=0x%04x]\n", lenEnl );
					return FAILED;
				}
			}
			else
			{ /* DES, ENL is the times of 8 bytes. */
				if ( lenEnl&0x7 )
				{
					;
					printf( "[IGNORE,ENL=0x%04x]\n", lenEnl );
					return FAILED;
				}
			}

	switch (cryptoIdx) {
		case 0:
		case 2:
		case 0x10:
		case 0x12:
		case 0x80:
		case 0x82:
		case 0x90:
		case 0x92:
			ivlen = 8;
			break;
		case 0x20:
		case 0xa0:
			ivlen = 16;
			break;
		case 0x22:
		case 0xa2:
			ivlen = 0;
			break;
		case 0x23:
		case 0xa3:
			ivlen = 8;
			break;
		default:
			ivlen = 0;
	}

		}


		if (cryptoIdx == 0x00 || cryptoIdx == 0x02 || cryptoIdx == 0x80 || cryptoIdx == 0x82)
			CryptoKeyLen = 8; // DES
		else if (cryptoIdx == 0x01 || cryptoIdx == 0x03 || cryptoIdx == 0x81 || cryptoIdx == 0x83)
			CryptoKeyLen = 24; // 3DES

		for (authIdx=authStart; authIdx<=authEnd; authIdx++) 
		{
			
			if ( authIdx!=(uint32)-1 )
			{
				if ( pktLen&3 )
				{
					/* Since A2EO and APL must be 4-byte times (ENL is 8/16-byte), lenValid must be 4-byte times.
					 * Otherwise, APL will be non-4-byte times. */
					;
					printf( "[IGNORE,Valid=0x%04x]\n", pktLen );
					return FAILED;
				}
			}

			if (authIdx != (uint32)-1 && cryptoIdx != (uint32)-1)
			{
				mode_select = _MS_TYPE_CRYPT_THEN_AUTH;
				
			} else {
				if (authIdx != (uint32)-1)
					mode_select = _MS_TYPE_AUTH;
				else
					mode_select = _MS_TYPE_CRYPT;
				
			}

	switch (authIdx) {
		case _MD_NOAUTH:
			digestlen = 0;
			break;
	    case 2:
	    case 6:
	        digestlen = 16;
	        break;
	    case 0:
	    case 4:
	        digestlen = 20;
	        break;
		case 1:			
		case 5:
			digestlen = 32;
			break;
		case 7: // poly1305
			digestlen = 16;
			break;
			
	    default:
	        err_printk("authtype(%d) : no this type\n", authIdx);
	        return FAIL;
	}
			
			//rtlglue_getmstime(&sTime);
			for(testRound=0; testRound<=round; testRound++) 
			{
				//udelay(3);//bruce
#if 0
				if (rtl_ipsecEngine(cryptoIdx, authIdx, cntScatter, src, pAsicCrypt,
					CryptoKeyLen, pCryptoKey, AuthKeyLen, pAuthKey, pIv, pPad, pAsicDigest, 
					a2eoLen, lenEnl) != SUCCESS)
				{
					SMP_UNLOCK_IPSEC;
					rtlglue_printf("testRound=%d, rtl8651x_ipsecEngine(modeIdx=%d,authIdx=%d) failed... pktlen=%d\n",
						testRound, cryptoIdx, authIdx, pktLen);
					return FAILED;
				}
#else
				if (rtl_crypto_mix(mode_select, cryptoIdx, authIdx, cur, pktLen, 
					CryptoKeyLen, pCryptoKey, AuthKeyLen, pAuthKey, pIv, ivlen,  
					a2eoLen, pAsicCrypt, pAsicDigest) != SUCCESS)
				{
					
					printf("testRound=%d, rtl8651x_ipsecEngine(modeIdx=%d,authIdx=%d) failed... pktlen=%d\n",
						testRound, cryptoIdx, authIdx, pktLen);
					return FAILED;
				}
				//printf("b");
#endif
			}
#if 0
			rtlglue_getmstime(&eTime);

			if ( eTime - sTime == 0 )
			{
				SMP_UNLOCK_IPSEC;
			#if 1//bruce
				if (round > TEST_ROUND) 
			#endif
				{
				rtlglue_printf("round is too small to measure throughput, try larger round number!\n");
				}
				return FAILED;
			}
			else
			{
			#if 1//bruce
				if (round > TEST_ROUND) 
			#endif
				{
				bps = pktLen*8*1000/((uint32)(eTime - sTime))*round;
				if(bps>1000000)
					rtlglue_printf("%s round %u len %u time %u throughput %u.%02u mbps\n",
						mixModeString(cryptoIdx, authIdx),
						round, pktLen, (uint32)(eTime - sTime),
						bps/1000000, (bps%1000000)/10000);
				else if(bps>1000)
					rtlglue_printf("%s round %u len %u time %u throughput %u.%02u kbps\n",
						mixModeString(cryptoIdx, authIdx),
						round, pktLen, (uint32)(eTime - sTime),
						bps/1000, (bps%1000)/10);
				else
					rtlglue_printf("%s round %u len %u time %u throughput %u bps\n",
						mixModeString(cryptoIdx, authIdx),
						round, pktLen, (uint32)(eTime - sTime),
						bps);			
				}
			}
#endif
		}
	}
	;
	return SUCCESS;
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
	void *pIv, void *pPad, void *pDigest,
	uint32 lenA2eo, uint32 lenEnl)
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
	uint32 mode_select;
	uint32 ivlen;
	uint32 digestlen=0;
	uint32 temp;
	uint8 *pu8CryptResult = pCryptResult;
	static uint8 *asic_dec;

	asic_dec = (void *) UNCACHED_ADDRESS(&_asic_dec[32]);

	sawb = 0;//rtl_ipsecEngine_sawb();
	if (!sawb && pCryptResult == NULL)
	{
		printk("no pCryptResult && no sawb!\n");
		return FAILED;
	}

	if (modeAuth == (uint32)-1 && modeCrypto == (uint32)-1)
	{
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
	if (modeAuth != (uint32)-1)
	{
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
	}

	// -1:NONE
	// 0x00:CBC_DES,0x10:CBC_3DES,0x02:ECB_DES,0x12:ECB_3DES
	// 0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES
	if (!(modeCrypto == (uint32)-1 ||
		modeCrypto==0x00 ||
		modeCrypto==0x10 ||
		modeCrypto==0x02 ||
		modeCrypto==0x12 ||
		modeCrypto==0x20 ||
		modeCrypto==0x22 ||
		modeCrypto==0x23))
	{
		// crypto type check failed
		return 3;
	}
	switch (modeCrypto) {
		case 0:
		case 2:
		case 0x10:
		case 0x12:
			ivlen = 8;
			break;
		case 0x20:
			ivlen = 16;
			break;
		case 0x22:
			ivlen = 0;
			break;
		case 0x23:
			ivlen = 8;
			break;
		default:
			ivlen = 0;
	}

	// 0,2: DES_CBC or DES_ECB
	// 10,12: 3DES_CBC or 3DES_ECB 
	// 20,22,23: AES_CBC or AES_ECB or AES_CTR
	if (((modeCrypto == 0 || modeCrypto == 2) && lenCryptoKey != 8) ||
		((modeCrypto == 0x10 || modeCrypto == 0x12) && lenCryptoKey != 24) ||
		((modeCrypto == 0x20 || modeCrypto == 0x22 || modeCrypto == 0x23) &&
			(lenCryptoKey < 16 || lenCryptoKey > 32)))
	{
		// crypto length check failed
		return 4;
	}

	// mix mode check
	if (modeCrypto != (uint32)-1 && (!(modeAuth&0x4)))
	{
		// crypto + hash is not support
		return 5;
	}

	// enl length check
	if (modeCrypto != (uint32)-1)
	{
		if (modeCrypto & 0x20) /* AES, ENL is the times of 16 bytes. */
		{
			if (lenEnl & 0xf)
				return 6;
		}
		else /* DES, ENL is the times of 8 bytes. */
		{
			if (lenEnl & 0x7)
				return 7;
		}

		if ((int) lenEnl <= 0)
			return 8;
	}

	// a2eo check
	if (lenA2eo)
	{
		if ( lenA2eo & 3 )
		{
			/* A2EO must be 4-byte times */
			return 9;
		}
		else if (modeAuth == (uint32)-1)
		{
			/* A2EO not support if no AUTH enable */
			return 10;
		}
	}

	if (msglen == 0) // pkt len = 0
		return 11;
//printf("(%s-%d)\n", __func__, __LINE__);

	// Mix Mode issue
	if (modeAuth != (uint32)-1 && modeCrypto != (uint32)-1)
	{
		int apl, authPadSpace;
		mode_select = _MS_TYPE_CRYPT_THEN_AUTH;

		// lenA2eo limit
		//if ((cntScatter == 1) ||
		//	(!rtl_ipsecEngine_sawb()))
		if (1)
		{

			int max_lenA2eo;

			switch (CRYPTO_TEST_DMA_MODE/*rtl_ipsecEngine_dma_mode()*/)
			{
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

		}
		else // multi-desc && use sawb
		{
			// 201303: A2EO limit if Multi-Desc & SAWB: it cause encryption failed
			if (lenA2eo > 136) // 136 = 152 - 16
				return 13;
		}

		// 201303: APL lenEnl limit -> encryption ok but hmac failed
		authPadSpace = 64 - ((lenA2eo + lenEnl) & 0x3f);
		apl = authPadSpace > 8 ? authPadSpace : authPadSpace + 64;

#ifndef CRYPTO_FORCE_MODE  //jwsyu 20150323 disable check
		if (apl == 68 && lenEnl < 64)
			return 14; 
#endif
		// 201303: Multi-Desc lenEnl limit -> 'sometimes' timeout if multi-desc
		if ((0/*cntScatter > 1*/) && (lenEnl <= 48))
			return 15;
	} else {
		if (modeAuth != (uint32)-1)
			mode_select = _MS_TYPE_AUTH;
		else
			mode_select = _MS_TYPE_CRYPT;

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
	for (i = 0; i < cntScatter; i++)
	{
		if (scatter[i].ptr == NULL)
		{
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

#ifndef DISABLE_SW_CHECK
//printf("(%s-%d)\n", __func__, __LINE__);

#if 1
	retval = ipsecSim(modeCrypto|0x80/*enc*/, modeAuth, _sw_orig, _sw_enc,
		lenCryptoKey, pCryptoKey, lenAuthKey, pAuthKey,
		pIv, pPad, sw_digest, lenA2eo, lenEnl);
#else

#endif

	if (retval != SUCCESS)
	{
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
		pIv, ivlen, lenA2eo, pCryptResult, pDigest);
#endif
//printf("(%s-%d)\n", __func__, __LINE__);

	if (retval != SUCCESS)
	{
		return FAILED; // IPSec Engine unable to encrypt is mandatory testing failed
	}
	
	/*---------------------------------------------------------------------
	 *                                 Compare ENC/Auth
	 *---------------------------------------------------------------------*/
#ifndef DISABLE_SW_CHECK
	if (modeCrypto != (uint32) -1)
	{
		if (0/*sawb*/) // use sawb, ASIC write back to scatter buffer
		{
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
		}
		else // not use sawb, ASIC write to continuous buffer pCryptResult
		{
			if (memcmp(_sw_enc + lenA2eo, pCryptResult + lenA2eo, lenEnl) != 0)
			{
				printk("ENC DIFF![%d]crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
					i, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

				for (temp=4 ; temp<lenA2eo; temp+=4) {
					if (_sw_enc[lenA2eo] == pu8CryptResult[lenA2eo-temp]) {
						if (_sw_enc[lenA2eo+1] == pu8CryptResult[lenA2eo-temp+1])
							break;
					}
				}
				printk("shift=%d", temp);
				if (memcmp(_sw_enc + lenA2eo, pCryptResult + lenA2eo - temp, lenEnl) == 0) {
					printk("recheck = OK");
					memDump(_sw_enc + lenA2eo, lenEnl, "Software");
					memDump(pCryptResult + lenA2eo, lenEnl, "ASIC");
					memDump(_sw_enc , lenA2eo, "Software-lenA2eo");
					memDump(pCryptResult , lenA2eo, "ASIC-lenA2eo");
					if (modeAuth != (uint32) -1) {
						memDump(sw_digest, digestlen, "Software_ICV");
						memDump(pDigest, digestlen, "ASIC_ICV");
						memDump(pAuthKey, lenAuthKey, "auth key");
					}

				}
				if (0/*g_failed_reset*/)
				{
					// no need more debug if auto reset
					return FAILED;
				}
				else
				{
					memDump(_sw_enc + lenA2eo, lenEnl, "Software");
					memDump(pCryptResult + lenA2eo, lenEnl, "ASIC");
					memDump(_sw_enc , lenA2eo, "Software-lenA2eo");
					memDump(pCryptResult , lenA2eo, "ASIC-lenA2eo");
					if (modeAuth != (uint32) -1) {
						memDump(sw_digest, digestlen, "Software_ICV");
						memDump(pDigest, digestlen, "ASIC_ICV");
						memDump(pAuthKey, lenAuthKey, "auth key");
					}
					rtl_cryptoEngine_info();
					return FAILED;
				}
			}
		}
	}

	if (modeAuth != (uint32) -1)
	{
		//if (memcmp(sw_digest, pDigest, modeAuth&1?20/*SHA1*/:16/*MD5*/ ) != 0) 
		if (memcmp(sw_digest, pDigest, digestlen ) != 0)
		{
			printk("ENC DIGEST! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
				modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

			if (0/*g_failed_reset*/)
			{
				// no need more debug if auto reset
				return FAILED;
			}
			else
			{
				memDump(sw_digest, digestlen, "Software");
				memDump(pDigest, digestlen, "ASIC");

				memDump(_sw_enc, lenA2eo + lenEnl, "sw_enc");
				memDump(pCryptResult, lenA2eo + lenEnl, "asic_enc");
				memDump(pAuthKey, lenAuthKey, "auth key");
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

	/*---------------------------------------------------------------------
	 *                                 SW Auth/DEC
	 *---------------------------------------------------------------------*/
#ifndef DISABLE_SW_CHECK
#if 1
	retval = ipsecSim(modeCrypto/*dec*/, modeAuth, _sw_enc, _sw_dec,
		lenCryptoKey, pCryptoKey, lenAuthKey, pAuthKey,
		pIv, pPad, sw_digest, lenA2eo, lenEnl);
#else
#endif

	if (retval != SUCCESS)
	{
		return FAILED; // IPSec Simulator Engine unable to decrypt is mandatory testing failed
	}
#endif

	/*---------------------------------------------------------------------
	 *                                 ASIC Auth/DEC
	 *---------------------------------------------------------------------*/
	// Mix Mode change mode_select
	if (modeAuth != (uint32)-1 && modeCrypto != (uint32)-1)
	{
		mode_select = _MS_TYPE_AUTH_THEN_CRYPT;
	}
	// create de-key for ASIC
#if 0 // 20150331 neo new api using pCryptoKey for input.
	if (modeCrypto == 0x20/*AES_CBC*/ || modeCrypto == 0x22/*AES_ECB*/)
	{
		AES_KEY aes_key;
		static uint8 __key_sch[32 + 256/8 + 32];
		static uint8 *key_sch = &__key_sch[32];

		/* IC accept the de-key in reverse order. */
		AES_set_encrypt_key(pCryptoKey, lenCryptoKey*8, &aes_key);
		switch (lenCryptoKey)
		{
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
	}
	else
	{
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
		pCryptResult, msglen,
		lenCryptoKey, pCryptoKey, lenAuthKey, pAuthKey,
		pIv, ivlen, lenA2eo, asic_dec, pDigest);

#endif

	if (retval != SUCCESS)
	{
		return FAILED; // IPSec Engine unable to encrypt is mandatory testing failed
	}
	
	/*---------------------------------------------------------------------
	 *                                 Compare Auth/DEC
	 *---------------------------------------------------------------------*/
#ifdef DISABLE_SW_CHECK
	if (modeAuth != (uint32) -1)
	{
		if (0 /*sawb*/) // use sawb, ASIC write back to scatter buffer
		{
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
		}
		else // not use sawb, ASIC write to continuous buffer pCryptResult
		{
			if (memcmp(_sw_orig + lenA2eo, pCryptResult + lenA2eo, lenEnl) != 0)
			{
				printk("DEC DIFF! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
					modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);
				return FAILED;
			}
		}
	}
#else
	if (modeAuth != (uint32) -1)
	{
		if (memcmp(sw_digest, pDigest, digestlen) != 0)
		{
			printk("DEC DIGEST! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
				modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

			if (0/*g_failed_reset*/)
			{
				// no need more debug if auto reset
				return FAILED;
			}
			else
			{
				memDump(sw_digest, digestlen, "Orig");
				memDump(pDigest, digestlen, "Software");
					rtl_cryptoEngine_info();
				return FAILED;
			}
		}
	}

	if (modeCrypto != (uint32) -1)
	{
		if (memcmp(_sw_orig + lenA2eo, _sw_dec + lenA2eo, lenEnl) != 0)
		{
			printk("DEC DIFF-2! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
				modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

			if (0/*g_failed_reset*/)
			{
				// no need more debug if auto reset
				return FAILED;
			}
			else
			{
				memDump(_sw_orig + lenA2eo, lenEnl, "Orig");
				memDump(_sw_dec + lenA2eo, lenEnl, "Software");
				return FAILED;
			}
		}

		if (0/*sawb*/) // use sawb, ASIC write back to scatter buffer
		{
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
		}
		else // not use sawb, ASIC write to continuous buffer asic_dec
		{
			if (memcmp(_sw_dec + lenA2eo, asic_dec + lenA2eo, lenEnl) != 0)
			{
				printk("DEC DIFF-4! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
					modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

				if (0/*g_failed_reset*/)
				{
					// no need more debug if auto reset
					return FAILED;
				}
				else
				{
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
static const unsigned char des3_test_keys[24] =
{
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
    0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23
};



__attribute__ ((aligned (4)))
static const unsigned char des3_test_iv[8] =
{
    0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
};


//__attribute__ ((aligned (4)))
static const unsigned char des3_test_buf[64] =
{
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
static const unsigned char des3_test_buf_encoded[8] =
{
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
#if 1 //jwsyu test new pattern
static const u8 _sha1_plaintext[] 	= "The quick brown fox jumps over the lazy dog";
#else
  #if 0
static const u8 _sha1_plaintext[]       = {0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
                                           0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
                                           0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
                                           0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
                                           0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
                                           0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
                                           0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78 };
  #elif 1
static const u8 _sha1_plaintext[]       = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                           0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                           0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                           0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                           0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                           0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    #if 1
                                           0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa };
	#else
                                           0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa };
	#endif
  #else
static const u8 _sha1_plaintext[]       = {0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12,
                                           0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12,
                                           0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12,
                                           0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12,
                                           0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12,
                                           0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12,
                                           0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12 };
  #endif
#endif


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


//__attribute__ ((aligned (4)))
static const u8 _hmac_md5_plaintext[] 	= "The quick brown fox jumps over the lazy dog";


//__attribute__ ((aligned (4)))
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
// http://en.wikipedia.org/wiki/SHA-2
//
// HMAC_SHA256("", "") = 0xb613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad
//
// HMAC_SHA256("key", "The quick brown fox jumps over the lazy dog") = 
//    0xf7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8
//

__attribute__ ((aligned (4)))
static const u8 _hmac_sha256_key[] 		= "key";


//__attribute__ ((aligned (4)))
static const u8 _hmac_sha256_plaintext[] 	= "The quick brown fox jumps over the lazy dog";


//__attribute__ ((aligned (4)))
static const u8 _hmac_sha256_digest[] 		=  "\xf7\xbc\x83\xf4\x30\x53\x84\x24"
							 			"\xb1\x32\x98\xe6\xaa\x6f\xb1\x43"
							 			"\xef\x4d\x59\xa1\x49\x46\x17\x59"
							 			"\x97\x47\x9d\xbc\x2d\x1a\x3c\xd8";


//
// http://tools.ietf.org/html/draft-nir-cfrg-chacha20-poly1305-06 
// Poly1305 vector : Test vector #2


__attribute__ ((aligned (4)))
static const u8 poly1305_key[32] 		= {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x36, 0xe5, 0xf6, 0xb5, 0xc5, 0xe0, 0x60, 0x70,
	0xf0, 0xef, 0xca, 0x96, 0x22, 0x7a, 0x86, 0x3e
};

//__attribute__ ((aligned (4)))
static const u8 poly1305_plaintext[] 	= 
	"Any submission to the IETF intended by the Contributor for publication "
	"as all or part of an IETF Internet-Draft or RFC and any statement made "
	"within the context of an IETF activity is considered an \"IETF "
	"Contribution\". Such statements include oral statements in IETF "
	"sessions, as well as written and electronic communications made at "
	"any time or place, which are addressed to";


//__attribute__ ((aligned (4)))
static const u8 poly1305_digest[] 		=  
	{ 0x36, 0xe5, 0xf6, 0xb5, 0xc5, 0xe0, 0x60, 0x70, 
	  0xf0, 0xef, 0xca, 0x96, 0x22, 0x7a, 0x86, 0x3e };


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
    //u8 *pAsicDigest = &_AsicDigest[32];
	// force not 4 byte alignment
    u8 *pAsicDigest = &_AsicDigest[33];
	int digestlen = 0;
#ifdef CRYPTO_TEST_SRAM
	message = (u8 *) (0xbfe00000+CRYPTO_TEST_NOT_ALIGN);
	*((u32 *) (0xbfe00000)) = 0;
#else
	// force not 4-byte alignment
	message = &_asic_orig[CRYPTO_TEST_NOT_ALIGN];
	_asic_orig[0]=0;
	_asic_orig[1]=0;
	_asic_orig[2]=0;
#endif
	
	switch (authtype) {
   		case 0: // SHA1
			key = NULL;
			keylen = 0;
			rtl_memcpyb(message, _sha1_plaintext, sizeof(_sha1_plaintext));
			plaintext = message;
			textlen = strlen(plaintext);
			digest = _sha1_digest;
			digestlen = 20;
			ret = rtl_crypto_sha1(plaintext, strlen(plaintext), pAsicDigest);
			break;
			
		case 1: // SHA2
			key = NULL;
			keylen = 0;
			rtl_memcpyb(message, _sha2_plaintext, sizeof(_sha2_plaintext));
			plaintext = message;
			textlen = strlen(plaintext);
			digest = _sha2_digest;
			digestlen = 32;
			ret = rtl_crypto_sha2(SHA2_256, plaintext, strlen(plaintext), pAsicDigest);
			break;
			
        case 2: // MD5
            key = NULL;
			keylen = 0;
			rtl_memcpyb(message, _md5_plaintext, sizeof(_md5_plaintext));
			plaintext = message;
			textlen = strlen(plaintext);
            digest = _md5_digest;            
			digestlen = 16;
			ret = rtl_crypto_md5(plaintext, strlen(plaintext), pAsicDigest);
            break;
			
		case 4: // HMAC-SHA1
			key = _hmac_sha1_key;
			keylen = strlen(_hmac_sha1_key);
			rtl_memcpyb(message, _hmac_sha1_plaintext, sizeof(_hmac_sha1_plaintext));
			plaintext = message;
			textlen = strlen(_hmac_sha1_plaintext);
			digest = _hmac_sha1_digest;
			digestlen = 20;
			ret = rtl_crypto_hmac_sha1(plaintext, textlen, key, keylen, pAsicDigest);
			break;
			
		case 5: // HMAC-SHA256
			//digestlen = SHA2_DIGEST_LENGTH;
			key = _hmac_sha256_key;
			keylen = strlen(key);
			rtl_memcpyb(message, _hmac_sha256_plaintext, sizeof(_hmac_sha256_plaintext));
			plaintext = message;
			textlen = strlen(plaintext);
			digest = _hmac_sha256_digest;
			digestlen = 32;
			ret = rtl_crypto_hmac_sha2(SHA2_256, plaintext, textlen, key, keylen, pAsicDigest);
			break;
		
        case 6: // HMAC-MD5
            key = _hmac_md5_key;
			keylen = strlen(key);
			rtl_memcpyb(message, _hmac_md5_plaintext, sizeof(_hmac_md5_plaintext));
			plaintext = message;
			textlen = strlen(plaintext);
            digest = _hmac_md5_digest;
			digestlen = 16;
			ret = rtl_crypto_hmac_md5(plaintext, strlen(plaintext), key, keylen, pAsicDigest);
            break;

		case 7: 
			key = poly1305_key;
			keylen = sizeof(poly1305_key);
			plaintext = poly1305_plaintext;
			textlen = strlen(plaintext);
			digest = poly1305_digest;
			digestlen = 16;
			ret = SUCCESS;
			break;
			
        default:
            err_printk("authtype(%d) : no this type\n", authtype);
            return FAIL;
  }

   
	if ( rtl_memcmpb(pAsicDigest, digest, digestlen) == 0)
	{
			dbg_mem_dump(pAsicDigest, digestlen, "test OK - digest:");
	    return SUCCESS;
	} else {
	    err_printk("test failed!\n");
	    dbg_mem_dump(pAsicDigest, digestlen, "asic digest");
	}

    ret = rtl_crypto_auth_sim(authtype, key, keylen, plaintext, textlen, sw_digest, digestlen);
    if (ret != SUCCESS)
    {
        err_printk("ipsecSim failed!\n");
        return ret;
    }

    dbg_mem_dump(sw_digest, digestlen, "sw digest");

    if ( rtl_memcmpb(sw_digest, digest, digestlen) == 0)
    {
        printk("the same as sw_digest\n");
    }
    else
    {
        printk("IPSec failed!totally different\n");
        dbg_mem_dump(pAsicDigest, digestlen, "asic digest");
		dbg_mem_dump(sw_digest, digestlen, "sw digest");
		dbg_mem_dump(digest, digestlen, "correct digest");		
    }
	
    return FAIL;
}



int vector_test_auth_rand(void)
{

    int ret = SUCCESS;

    u8 *pAsicDigest = &_AsicDigest[32];

    u32 authtype;
    u32 keylen;
    u8 *key;
    u32 msglen;
    u8 *message;
    int i;

	u32 this_rtlseed;
//	HAL_CIPHER_ADAPTER *pIE;
	u32 digestlen = 0;

	this_rtlseed = rtl_random();
	//dbg_printk("seed=0x%x\n", this_rtlseed);


//	pIE = &g_rtl_IPSec_Engine;
	
	// 0 : SHA1, 1: SHA2, 2:MD5, 4:HMAC-SHA1, 5: HMAC-SHA2, 6:HMAC-MD5
	authtype = ( (rtl_random() % 2 )== 1)?
				 (rtl_random()%3) :
				 (rtl_random()%3)+4 ;

	// Suppose keylen = 0~ BLOCK(64)
	if ( authtype < 3 ) { // no HMAC
		keylen = 0;
		key = NULL;
	} else {
	  keylen = ( rtl_random() % 63 )+1;
	  key = &_asic_authkey[0];
	  for (i=0; i<keylen; i++)
	      key[i] = rtl_random() & 0xFF;
	  key[i] = '\0';
	}


	// Suppose msglen = 1~16000
	msglen = (rtl_random() % 16000)+1;

	//message = &_asic_orig[32];
	// force not 4byte alignment
	message = &_asic_orig[33];

	for (i=0; i<msglen; i++)
	  message[i] = rtl_random() & 0xFF;
	message[i] = '\0';

flush_dcache_range((u32)message, ((u32)message)+msglen+64);

	switch (authtype) {
	case 2: 			
			digestlen = CRYPTO_MD5_DIGEST_LENGTH;
			ret = rtl_crypto_md5(message, msglen, pAsicDigest);
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				return ret; 				
			}
			
		break;
	case 6:
			digestlen = CRYPTO_MD5_DIGEST_LENGTH;
			ret = rtl_crypto_hmac_md5(message, msglen, key, keylen, pAsicDigest);
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				return ret; 				
			}
		break;
	case 0:
			digestlen = CRYPTO_SHA1_DIGEST_LENGTH;
			ret = rtl_crypto_sha1(message, msglen, pAsicDigest);
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				return ret; 				
			}
			
		break;
	case 4:
			digestlen = CRYPTO_SHA1_DIGEST_LENGTH;
			ret = rtl_crypto_hmac_sha1(message, msglen, key, keylen, pAsicDigest);
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				return ret; 				
			}
			
		break;
	case 1: 		
			digestlen = CRYPTO_SHA2_DIGEST_LENGTH;
			ret = rtl_crypto_sha2(SHA2_256, message, msglen, pAsicDigest);
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				return ret; 				
			}
		break;
	case 5:
			digestlen = CRYPTO_SHA2_DIGEST_LENGTH;
			ret = rtl_crypto_hmac_sha2(SHA2_256, message, msglen, key, keylen, pAsicDigest);
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				return ret; 				
			}
			
		break;
	default:
		err_printk("authtype(%d) : no this type\n", authtype);
		ret = FAIL;
	}



	ret = rtl_crypto_auth_sim(authtype, key, keylen, message, msglen, sw_digest, digestlen);
	if (ret != SUCCESS)
	{
	    err_printk("ipsecSim failed!\n");
	    ret = FAIL;
	    goto ret_vector_test_auth_rand;
	}

	if ( rtl_memcmpb(pAsicDigest, sw_digest, digestlen) == 0)
	{
	    dbg_printk("IPSec ok\n");
	    ret = SUCCESS;
	}
	else
	{
	    ret = FAIL;
	}

ret_vector_test_auth_rand:

	if ( ret == FAIL ) {
	    printk("IPSec failed!\n");
		err_printk("rtl_seed = 0x%x\n", this_rtlseed);
		err_printk("auth_type = 0x%x\n", authtype);
		err_printk("keylen = 0x%x\n", keylen);
		err_printk("msglen = 0x%x\n", msglen);
		err_printk("digestlen = 0x%x\n", digestlen);

	    memDump(key, keylen, "key: ");
	    memDump(message, msglen, "message: ");
	    memDump(pAsicDigest, digestlen, "asic digest: ");
	    memDump(sw_digest, digestlen, "sw digest: ");
	}

	return ret;
}



int vector_test_cipher_rand(void)
{
    int ret = FAIL;
	
	u32 cipher_type;
    u32 keylen, ivlen;
    u8 *key, *pIv;
    u32 msglen;
    u8 *message;
	u8 *message_sw;

    int i;

	u8 *pResult_asic;
	u8 *pResult_sw;

	u32 enl;
	u32 this_rtlseed;

	
	this_rtlseed = rtl_random();
//	this_rtlseed = 0xaa9b6229;
//	rtl_srandom(this_rtlseed);
	//	dbg_printk("seed=0x%x\n", this_rtlseed);

	switch (rtl_random() % 7) {
	    case 0:
			cipher_type = 0x00; // DES-CBC
			keylen = 8;			
			ivlen = keylen;
			break;
		case 1:
			cipher_type = 0x02; // DES-ECB
			keylen = 8;			
			ivlen = keylen;
			break;
		case 2:
			cipher_type = 0x10; // 3DES-CBC
			keylen = 24;
			ivlen = 8;
			break;
		case 3:
			cipher_type = 0x12; // 3DES-ECB
			keylen = 24;
			ivlen  = 8;
			break;
		case 4:
			cipher_type = 0x20; // AES-CBC, 128, 192, 256
			keylen = ((rtl_random() %3)+2)*8;
			ivlen = 16;
			break;
		case 5:
			cipher_type = 0x22; // AES-ECB, 128, 192, 256
			keylen = ((rtl_random() %3)+2)*8;
			ivlen = 0;
			break;
		case 6:
			cipher_type = 0x23; // AES-CTR 128, 192, 256
			keylen = ((rtl_random() %3)+2)*8;
			ivlen = 8;
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
flush_dcache_range((u32)key, ((u32)key)+keylen+64);
	

	// Suppose msglen = 1~16000
	msglen = ((rtl_random() % 1000)+1)*16;

	//message = &_asic_orig[32];
	// force not 4-byte alignment
	message = &_asic_orig[33];
	
	//memcpy(message, des3_test_buf, msglen);
	for (i=0; i<msglen; i++)
	  message[i] = rtl_random() & 0xFF;
	message[msglen] = '\0';
flush_dcache_range((u32)message, ((u32)message)+msglen+64);

	message_sw = _sw_orig;
	rtl_memcpyb(message_sw, message, msglen);
	message_sw[msglen] = '\0';

	pIv = _asic_iv;
	//memcpy(pIv, des3_test_iv, 8);
	for (i=0; i<ivlen; i++)
		pIv[i] = rtl_random() & 0xFF;
flush_dcache_range((u32)pIv, ((u32)pIv)+ivlen+64);

	//pResult_asic = _asic_enc;		  
	// force result not 4-byte alignment	
	//pResult_asic = &_asic_enc[1];		
	pResult_asic = &_asic_enc[32+1];
	// force result is message
//	pResult_asic = message;		  
	pResult_sw	 = _sw_enc;
flush_dcache_range((u32)pResult_asic, ((u32)pResult_asic)+msglen+64);

   
	// a2eo = 0;
	enl = msglen;

	switch ( cipher_type) {
		case 0x0: // DES-CBC
			ret = rtl_crypto_des_cbc_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			ret = rtl_crypto_des_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		
		case 0x2: // DES-ECB
			ret = rtl_crypto_des_ecb_init(key,keylen);
			if ( ret != SUCCESS ) return ret;

			ret = rtl_crypto_des_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		case 0x10: // 3DES-CBC
			ret = rtl_crypto_3des_cbc_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			
			ret = rtl_crypto_3des_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		
		case 0x12: // 3DES-ECB
			ret = rtl_crypto_3des_ecb_init(key,keylen);
			if ( ret != SUCCESS ) return ret;

			ret = rtl_crypto_3des_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		case 0x20: // AES-CBC
			ret = rtl_crypto_aes_cbc_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			
			ret = rtl_crypto_aes_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			
		case 0x22: // AES-ECB
			ret = rtl_crypto_aes_ecb_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			
			ret = rtl_crypto_aes_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		case 0x23: // AES-CTR
			ret = rtl_crypto_aes_ctr_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			
			ret = rtl_crypto_aes_ctr_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			

		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			break;
	}

	if (ret != SUCCESS)
	{
	   err_printk("ipsec encrypt failed!, ret=%d\n", ret);
	   ret = FAIL;
	   goto ret_vector_test_esp_rand;
	}
   
	ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, pIv, ivlen, message_sw, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
	   err_printk("ipsecSim encrypt failed!\n");
	   memDump(pResult_asic, msglen, "ASIC enc msg: ");
	   ret = FAIL;
	   goto ret_vector_test_esp_rand;
	}
	   
	if ( rtl_memcmpb(pResult_asic, pResult_sw, enl) == 0 )
	{
	   dbg_mem_dump(pResult_asic, msglen, "test OK - enc:");
	} else {
	   err_printk("test failed!\n");
	   memDump(pResult_asic, msglen, "ASIC enc msg: ");
	   memDump(pResult_sw, msglen, "SW enc msg: ");
	   ret = FAIL;
	   goto ret_vector_test_esp_rand;
	}
   
	// Decryption

	message = pResult_asic;
flush_dcache_range((u32)message, ((u32)message)+msglen+64);
	
	rtl_memcpyb(message_sw, message, msglen);
	message_sw[msglen] = '\0';
flush_dcache_range((u32)message_sw, ((u32)message_sw)+msglen+64);

	//pResult_asic = _asic_dec;		 
	// force result not 4-byte alignment
	//pResult_asic = &_asic_dec[1];		 
	pResult_asic = &_asic_dec[32+1];
	// force result is message
	//pResult_asic = message;
flush_dcache_range((u32)pResult_asic, ((u32)pResult_asic)+msglen+64);
	
	pResult_sw	= _sw_dec;

	switch ( cipher_type) {
		case 0x0: // DES-CBC
			ret = rtl_crypto_des_cbc_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		
		case 0x2: // DES-ECB
			ret = rtl_crypto_des_ecb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		case 0x10: // 3DES-CBC
			ret = rtl_crypto_3des_cbc_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		
		case 0x12: // 3DES-ECB
			ret = rtl_crypto_3des_ecb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		case 0x20: // AES-CBC
			ret = rtl_crypto_aes_cbc_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			
		case 0x22: // AES-ECB
			ret = rtl_crypto_aes_ecb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		case 0x23: // AES-CTR
			ret = rtl_crypto_aes_ctr_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			

		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			break;
	}
		
	if (ret != SUCCESS)
	{
	   err_printk("ipsec decrypt failed!\n");
	   goto ret_vector_test_esp_rand;
	}


	ret = rtl_crypto_esp_sim(cipher_type, key, keylen, pIv, ivlen, message_sw, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
	   err_printk("ipsecSim decrypt failed!\n");
	   memDump(pResult_asic, msglen, "ASIC dec msg: ");
	   goto ret_vector_test_esp_rand;
	}

	if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 )
	{
	   dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
	   ret = SUCCESS;
	} else {
	   err_printk("test failed!\n");
	   
	   memDump(pResult_asic, msglen, "ASIC dec msg: ");
	   
	   ret = FAIL;
	   goto ret_vector_test_esp_rand;
	}

	return SUCCESS;

ret_vector_test_esp_rand: 
	
	err_printk("message (%d) : \n", msglen);
	memDump(message, msglen, "MSG:");
	
	err_printk("this seed=0x%x\n", this_rtlseed);
	err_printk("cipher_type=0x%x\n", cipher_type);
	
	err_printk("key (%d) : \n", keylen);
	memDump(key, keylen, "key:");
	
	err_printk("IV (%d) : \n", ivlen);
	memDump(pIv, ivlen, "IV:");

	return ret;
}


/****************************************
 * By 'crypto mix rand' command
 *   random key/iv/mode/len/offset/data
 ****************************************/
int32 crypto_GeneralApiRandTest(uint32 seed, uint32 round,
        int32 mode_crypto, int32 mode_auth, int32 maxScatter)
{
#define LEN_CRTPTO_VALS (sizeof(modeCrypto_val)/sizeof(modeCrypto_val[0]))
#define LEN_AUTH_VALS (sizeof(modeAuth_val)/sizeof(modeAuth_val[0]))
#define LEN_CRYPTOKEY_VALS (sizeof(CryptoKey_val)/sizeof(CryptoKey_val[0]))
	int i, j, k;
	int32 roundIdx;
	int32 pktLen, offset;
	int32 modeCrypto, modeAuth;
	int32 lenCryptoKey, lenAuthKey;
	int32 lenA2eo, lenEnl;
	int32 ret = SUCCESS;
	int32 modeCrypto_val[8] = {-1, 0x00, 0x10, 0x02, 0x12, 0x20, 0x22, 0x23};
	int32 modeAuth_val[9] = {-1, 0, 2, 4, 6, 0x11, 0x21, 0x15, 0x25};
	int32 CryptoKey_val[4] = {8, 16, 24, 32};
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
	uint8 *asic_buf;
	int cnt_success, cnt_failed, cnt_ignore, cnt_scatters;
	int32 stat_tests[LEN_CRTPTO_VALS][LEN_AUTH_VALS][LEN_CRYPTOKEY_VALS];
	int idxCrypto, idxAuth, idxKeyLen, cnt;

	int stop_run_keyin;

	cryptoKey = (void *) UNCACHED_ADDRESS(&_asic_cryptkey[32]);
	cryptoIv = (void *) UNCACHED_ADDRESS(&_asic_iv[32]);
	authKey = (void *) UNCACHED_ADDRESS(&_asic_authkey[32]);
	//pad = (void *) UNCACHED_ADDRESS(&_pad[32]);
	asic_orig = (void *) UNCACHED_ADDRESS(&_asic_orig[32]);
	asic_enc = (void *) UNCACHED_ADDRESS(&_asic_enc[32]);
	asic_digest = (void *) UNCACHED_ADDRESS(&_AsicDigest[32]);

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

	for (roundIdx=0; roundIdx<round; roundIdx++) 
	{
		printf("============>  start TEST seed=%x <=================\n", seed);
		if (tstc()) {
			stop_run_keyin = getc();
			printf("stop_run_keyin=%c", stop_run_keyin);
			if (stop_run_keyin == 's') {
				printf("stop test\n");
				break;
			}
		}
		rtlglue_srandom(seed++);
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
			idxCrypto = rtlglue_random() % LEN_CRTPTO_VALS;
#ifdef CRYPTO_FORCE_MODE
	idxCrypto = FORCE_modeCrypto;
#endif

		modeCrypto = modeCrypto_val[idxCrypto];

		if (mode_auth == 0xff)
			idxAuth = rtlglue_random() % LEN_AUTH_VALS;
#ifdef CRYPTO_FORCE_MODE
	idxAuth = FORCE_modeAuth;
#endif

		modeAuth = modeAuth_val[idxAuth];

		if (modeCrypto == 0x00 || modeCrypto == 0x02)
			idxKeyLen = 0; // DES
		else if (modeCrypto == 0x10 || modeCrypto == 0x12)
			idxKeyLen = 2; // 3DES
		else
			idxKeyLen = (rtlglue_random() % 3) + 1; // AES 128,192,256

		lenCryptoKey = CryptoKey_val[idxKeyLen];

		stat_tests[idxCrypto][idxAuth][idxKeyLen]++;

		//lenAuthKey = rtlglue_random() & 0x3ff; // 0~1023
		//if (lenAuthKey > MAX_AUTH_KEY)
		//	lenAuthKey = MAX_AUTH_KEY;
		lenAuthKey = (rtlglue_random() & 0x3f)+1; // 1~64

		for(i=0; i<24; i++)
			cryptoKey[i] = rtlglue_random() & 0xFF;
		for(i=0; i<8; i++)
			cryptoIv[i] = rtlglue_random() & 0xFF;

		//if (randScatter)
		//{
		//	i = MAX_SCATTER < 7 ? MAX_SCATTER : 7;
		//	if (rtl_ipsecEngine_sawb()) // max scatter is 7 if enable sawb
		//		maxScatter = (rtlglue_random() % i) + 1;
		//	else
		//		maxScatter = (rtlglue_random() % MAX_SCATTER) + 1;
		//}

		if (modeCrypto == -1 && modeAuth == -1)
		{
			cnt_ignore++;
			continue;
		}
		else if (modeCrypto == -1) // auth only
		{
			lenA2eo = rtlglue_random() & 0xFC; // a2eo must 4 byte aligna, 0..252

			//if (roundIdx % 10 > 1) // 80% case use 1..1518
			if (0) //jwsyu pktLen random
				pktLen = rtlglue_random() % 1518 + 1;
			else if (modeAuth & 0x4) // HMAC
				pktLen = rtlglue_random() % (0x3FC0 - 9 - HMAC_MAX_MD_CBLOCK) + 1; // 1..16247
			else // HASH only
				pktLen = rtlglue_random() % (0x3FC0 - 9) + 1; // 1..16311
		}
		else if (modeAuth == -1) // crypto only
		{
			lenA2eo = 0; // lenA2eo must 0 if crypto only

			//if (roundIdx % 10 > 1) // 80% case use 1..1518
			if (0) //jwsyu pktLen random
			{
				pktLen = rtlglue_random() % 1518 + 1;
				pktLen = pktLen & ((modeCrypto & 0x20) ? 0x3FF0 : 0x3FF8);
			}
			else if (modeCrypto & 0x20) // aes
				pktLen = rtlglue_random() & 0x3FF0; /* 16 ~ 16368, 0 will be skipped later */
			else // des
				pktLen = rtlglue_random() & 0x3FF8; /* 8 ~ 16376, 0 will be skipped later */
		} 
		else // mix mode
		{
			int max_a2eo;

			modeAuth |= 0x4; // support hmac only if mix mode

			if ((1 /*cntScatter == 1*/) || (!0/*rtl_ipsecEngine_sawb()*/))
			{
#if 1
				int dma_bytes[4] = {16, 32, 64, 64};
				max_a2eo = 152 + dma_bytes[CRYPTO_TEST_DMA_MODE/*rtl_ipsecEngine_dma_mode()*/] - 16;
#else
				max_a2eo = 152 - 4;
#endif
			}
			else 
			{
				max_a2eo = 152 - 16;
			}
			//jwsyu limit max a2eo 20160108,
			#ifdef MAX_A2EO_LIMIT
			max_a2eo=132;
			#endif
			lenA2eo = (rtlglue_random() % max_a2eo) & 0xFC; // a2eo must 4 byte aligna, 0..252

			//if (roundIdx % 10 > 1) // 80% case use 1..1518
			if (0) //jwsyu pktLen random
				lenEnl = rtlglue_random() % (1518 - lenA2eo) + 1;
			else
				lenEnl = rtlglue_random() % (0x3FC0 - 9 - HMAC_MAX_MD_CBLOCK - lenA2eo) + 1; /* 1..16247 - a2eo*/

			if (modeCrypto & 0x20)	// aes
			{
				lenEnl = lenEnl & 0x3FF0;
				if (lenEnl <= 48)
					lenEnl = 64;
			}
			else // des
			{
				lenEnl = lenEnl & 0x3FF8;
				if (lenEnl <= 48)
					lenEnl = 56;
			}

			pktLen = lenEnl + lenA2eo;
		}
#ifdef CRYPTO_FORCE_MODE
	lenEnl = FORCE_pktLen - FORCE_lenA2eo;
	lenA2eo = FORCE_lenA2eo;
	pktLen = FORCE_pktLen;
#endif
		offset = rtlglue_random() & 0x7;
//offset = 0;
		for(i=0; i<pktLen; i++) //8-byte for IV and 4-byte for DMA_addr (0-3) byte offset test
			asic_orig[i+offset] = rtlglue_random() & 0xFF;

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
			lenA2eo, pktLen - lenA2eo);

		if (ret == FAILED)
		{
			printk("SAWB:%d DMA:%d Len:%d Off:%d Enc:0x%x Auth:0x%x CK:%d AK:%d A2EO:%d [FAILED-%d]\n",
				0/*rtl_ipsecEngine_sawb()*/, CRYPTO_TEST_DMA_MODE/*rtl_ipsecEngine_dma_mode()*/,
				pktLen, offset, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo , __LINE__
				);

			if (0/*g_failed_reset*/)
			{
				//rtl_ipsecEngine_reset();
				cnt_failed++;
			}
			else
				return FAILED;
		}
		else if (ret > 0)
		{
			if (dbg_level >= 2)
				printk("SAWB:%d DMA:%d Len:%d Off:%d Enc:0x%x Auth:0x%x CK:%d AK:%d A2EO:%d [SKIP %d]\n",
					0/*rtl_ipsecEngine_sawb()*/, CRYPTO_TEST_DMA_MODE/*rtl_ipsecEngine_dma_mode()*/,
					pktLen, offset, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo, 
					ret);

			cnt_ignore++;
		}
		else
		{
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

	for (i=0; i<LEN_CRTPTO_VALS; i++)
	{
		for (j=0; j<LEN_AUTH_VALS; j++)
		{
			switch (modeCrypto_val[i])
			{
			case -1: // no crypto
				cnt = 0;
				for (k=0; k<LEN_CRYPTOKEY_VALS; k++)
					cnt += stat_tests[i][j][k];

				printk("no crypto: stat(0x%x, 0x%x) = %d\n",
					modeCrypto_val[i], modeAuth_val[j], cnt);
				break;
			case 0x00: // des cbc
			case 0x02: // des ecb
				for (k=0; k<LEN_CRYPTOKEY_VALS; k++)
				{
					if (k == 0)
					{
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
			case 0x10: // 3des cbc
			case 0x12: // 3des ecb
				for (k=0; k<LEN_CRYPTOKEY_VALS; k++)
				{
					if (k == 2)
					{
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
				for (k=0; k<LEN_CRYPTOKEY_VALS; k++)
				{
					if (k)
					{
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
	//const u8 *pResult_correct;
    int ret;
	u32 msglen, enl;
	//	u32 a2eo;
	//	u32 modeSelect;
	//	HAL_CIPHER_ADAPTER *pIE;


	
	// Encryption

	key = des3_test_keys;
	keylen = 24;
#ifdef CRYPTO_TEST_SRAM
	message = (u8 *) (0xbfe00000+CRYPTO_TEST_NOT_ALIGN);
	*((u32 *) (0xbfe00000)) = 0;
#else
	//message = &_asic_orig[32];
	// force not 4-byte alignmnet
	message = &_asic_orig[32+CRYPTO_TEST_NOT_ALIGN];
#endif
	msglen = sizeof(des3_test_buf);
	rtl_memcpyb(message, des3_test_buf, msglen);
    pIv = des3_test_iv;
	ivlen = 8;

	// force not 4-byte alignment
	//pResult_asic = _asic_enc;		  
	pResult_asic = &_asic_enc[1];		  
	pResult_sw   = _sw_enc;
	//pResult_correct = des3_test_buf_encoded;

	//modeSelect = _MS_CRYPTO;

//   Note: a2eo is only for athentication to encryption, can not set a2eo in only crypto
//	a2eo = 32;
//   message = _asic_orig;
	//a2eo = 0;
	enl = msglen;


 	switch ( cipher_type) {
		case 0x10: // 3DES-CBC
			rtl_crypto_3des_cbc_init(key,keylen);
			ret = rtl_crypto_3des_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		
		case 0x12 : // 3DES-ECB
			rtl_crypto_3des_ecb_init(key,keylen);
			ret = rtl_crypto_3des_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			

		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			ret = _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH;
			break;
	}

	if (ret != SUCCESS)
	{
		err_printk("ipsec encrypt failed!\n");
		return ret;
	}
 

 	ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, pIv, ivlen, message, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
		err_printk("ipsecSim encrypt failed!\n");
		return ret;
	}
 	
	if ( rtl_memcmpb(pResult_asic, pResult_sw, enl) == 0 )
	{
		memDump(pResult_asic, msglen, "test OK - enc:");
	} else {
		err_printk("test failed!\n");
		dbg_mem_dump(pResult_asic, msglen, "ASIC enc msg: ");
		dbg_mem_dump(pResult_sw, msglen, "SW enc msg: ");
		return FAIL;
	}

	// Decryption

    message = pResult_asic;
flush_dcache_range((u32)message, ((u32)message)+msglen+64);

	//pResult_asic = _asic_dec;		  
	// force not 4-byte alignment
	pResult_asic = &_asic_dec[1];		  
	pResult_sw   = _sw_dec;
	//pResult_correct = des3_test_buf;

flush_dcache_range((u32)pResult_asic, ((u32)pResult_asic)+msglen+64);
	switch ( cipher_type) {
		case 0x10: // 3DES-CBC
			ret = rtl_crypto_3des_cbc_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;

		
		case 0x12 : // 3DES-ECB
			ret = rtl_crypto_3des_ecb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			

		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			break;
	}

	if (ret != SUCCESS)
	{
		err_printk("ipsec encrypt failed!\n");
		return ret;
	}

	ret = rtl_crypto_esp_sim(cipher_type, key, keylen, pIv, ivlen, message, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
		err_printk("ipsecSim encrypt failed!\n");
		return ret;
	}
	
	if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 )
	{
		dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
		return SUCCESS;
	} else {
		err_printk("test failed!\n");
		dbg_mem_dump(pResult_asic, msglen, "ASIC dec msg: ");
		dbg_mem_dump(pResult_sw, msglen, "SW dec msg: ");
		return FAIL;
	}


}


//
// http://www.inconteam.com/software-development/41-encryption/55-aes-test-vectors
// 

#if 1
// 128 bit - CBC

__attribute__ ((aligned (4)))
static const unsigned char aes_test_key[16] =
{
	0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
	0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
} ;
#elif 0
__attribute__ ((aligned (4)))
static const unsigned char aes_test_key[16] =
{
	0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
	0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
} ;
#elif 1 // linux-3.10/crypto/testmgr.h test vector aes_ctr_enc_tv_template
__attribute__ ((aligned (4)))
static const unsigned char aes_test_key[32] =
{
	0xc9, 0x83, 0xa6, 0xc9, 0xec, 0x0f, 0x32, 0x55,
	0x0f, 0x32, 0x55, 0x78, 0x9b, 0xbe, 0x78, 0x9b,
	0xbe, 0xe1, 0x04, 0x27, 0xe1, 0x04, 0x27, 0x4a,
	0x6d, 0x90, 0x4a, 0x6d, 0x90, 0xb3, 0xd6, 0xf9
} ;
#else



#endif

__attribute__ ((aligned (4)))
static const unsigned char aes_test_iv_1[16] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};


#if 1
__attribute__ ((aligned (4)))
static const unsigned char aes_test_buf[16] =
{
	0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};
#elif 0
__attribute__ ((aligned (4)))
static const unsigned char aes_test_buf[64] =
{
	0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
	0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
	0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
	0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
	0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
	0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
	0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};
#else
__attribute__ ((aligned (4)))
static const unsigned char aes_test_buf[80] =
{
	0x50, 0xb9, 0x22, 0xae, 0x17, 0x80, 0x0c, 0x75,
	0xde, 0x47, 0xd3, 0x3c, 0xa5, 0x0e, 0x9a, 0x03,
	0x6c, 0xf8, 0x61, 0xca, 0x33, 0xbf, 0x28, 0x91,
	0x1d, 0x86, 0xef, 0x58, 0xe4, 0x4d, 0xb6, 0x1f,
	0xab, 0x14, 0x7d, 0x09, 0x72, 0xdb, 0x44, 0xd0,
	0x39, 0xa2, 0x0b, 0x97, 0x00, 0x69, 0xf5, 0x5e,
	0xc7, 0x30, 0xbc, 0x25, 0x8e, 0x1a, 0x83, 0xec,
	0x55, 0xe1, 0x4a, 0xb3, 0x1c, 0xa8, 0x11, 0x7a,
	0x06, 0x6f, 0xd8, 0x41, 0xcd, 0x36, 0x9f, 0x08,
	0x94, 0xfd, 0x66, 0xf2, 0x5b, 0xc4, 0x2d, 0xb9,
};

#endif



__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_128[16] =
{
	0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
	0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d
};


// 192 bit - CBC 

__attribute__ ((aligned (4)))
static const unsigned char aes_test_key_192[24] =
{
	0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
	0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
	0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
};



__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_192_cbc[16] =
{
	0x4f, 0x02, 0x1d, 0xb2, 0x43, 0xbc, 0x63, 0x3d,
	0x71, 0x78, 0x18, 0x3a, 0x9f, 0xa0, 0x71, 0xe8
};


// 256 bit - CBC

__attribute__ ((aligned (4)))
static const unsigned char aes_test_key_256[32] =
{
	0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
	0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
	0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
	0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};



__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_256_cbc[16] =
{
	0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba,
	0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6
};


// ECB

__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_128_ECB[16] =
{
	0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
};


__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_192_ECB[16] =
{
	0xbd, 0x33, 0x4f, 0x1d, 0x6e, 0x45, 0xf2, 0x5f,
	0xf7, 0x12, 0xa2, 0x14, 0x57, 0x1f, 0xa5, 0xcc
};


__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_256_ECB[16] =
{
	0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c,
	0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8
};



// CTR
#if 1
__attribute__ ((aligned (4)))
static const unsigned char aes_test_iv[16] =
{
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};
#elif 0
__attribute__ ((aligned (4)))
static const unsigned char aes_test_iv[16] =
{
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd
};
#elif 0
__attribute__ ((aligned (4)))
static const unsigned char aes_test_iv[16] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd
};
#else
__attribute__ ((aligned (4)))
static const unsigned char aes_test_iv[16] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xfd
};

#endif

__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_128_CTR[16] =
{
	0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26,
	0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce
};


__attribute__ ((aligned (4)))
static const unsigned char aes_test_res_192_CTR[16] =
{
	0x1a, 0xbc, 0x93, 0x24, 0x17, 0x52, 0x1c, 0xa2,
	0x4f, 0x2b, 0x04, 0x59, 0xfe, 0x7e, 0x6e, 0x0b
};



int vector_test_cipher_AES(u32 cipher_type)
{
    const u8 *key, *pIv;
	u32 keylen= 0;
	u32 ivlen = 0;
    u8 *message;
    u8 *pResult_asic;
	u8 *pResult_sw;
	//const u8 *pResult_correct = aes_test_res_128;
    int ret;
	u32 msglen, enl;
	//u32 a2eo
//	u32 modeSelect;
//	HAL_CIPHER_ADAPTER *pIE;

//	pIE = &g_rtl_IPSec_Engine;
	
	// Encryption

	key = aes_test_key;
	keylen = sizeof(aes_test_key);
#ifdef CRYPTO_TEST_SRAM
	message = (u8 *) (0xbfe00000+CRYPTO_TEST_NOT_ALIGN);
	*((u32 *) (0xbfe00000)) = 0;
#else
	//message = &_asic_orig[32];
	// force message not 4-byte alignment
	message = &_asic_orig[32+CRYPTO_TEST_NOT_ALIGN];
#endif
	msglen = sizeof(aes_test_buf);
	//msglen = 12;//sizeof(aes_test_buf);
	rtl_memcpyb(message, aes_test_buf, msglen);
    pIv = aes_test_iv;
//	ivlen = ( (cipher_type & _MASK_CBCECBCTR) == 0x2 ) ? 0 : sizeof(aes_test_iv);
	

	//pResult_asic = _asic_enc;		  
	// force not 4-byte alignment
	pResult_asic = &_asic_enc[1];		  
	pResult_sw   = _sw_enc;

//	modeSelect = _MS_CRYPTO;

//	a2eo = 32;
//   message = _asic_orig;
	//a2eo = 0;
	enl = msglen;



	switch ( cipher_type) {
		//case CIPHER_TYPE_3DES_CBC: // 3DES-CBC
		//case CIPHER_TYPE_3DES_ECB : // 3DES-ECB
			//ret = vector_test_esp_3DES(crypto_type);
			//break;
			
		case 0x20: // AES-CBC
			pIv = aes_test_iv_1;
			ivlen = sizeof(aes_test_iv);
			ret = rtl_crypto_aes_cbc_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			ret = rtl_crypto_aes_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			//pResult_correct = aes_test_res_128;
			break;
			
		case 0x22 : // AES-ECB
			ret = rtl_crypto_aes_ecb_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			ivlen = 0;
			ret = rtl_crypto_aes_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			//pResult_correct = aes_test_res_128_ECB;
			break;

		case 0x23 : // AES-CTR
			ret = rtl_crypto_aes_ctr_init(key,keylen);
			if ( ret != SUCCESS ) return ret;
			ivlen = sizeof(aes_test_iv);
			ret = rtl_crypto_aes_ctr_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			//pResult_correct = aes_test_res_128_CTR;
			break;

		case 0x24 : // AES-CFB 
			//ret = rtl_crypto_aes_cfb_init(key,keylen);
			//if ( ret != SUCCESS ) return ret;
			pIv = aes_test_iv_1;
			ivlen = sizeof(aes_test_iv_1);
//			ret = rtl_crypto_aes_cfb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
			ret = SUCCESS;
			// AES-CFB
			// enc: 3b 3f d9 2e b7 2d ad 20   33 34 49 f8 e8 3c fb 4a 
			break;

		case 0x25 : // AES-OFB
			pIv = aes_test_iv_1;
			ivlen = sizeof(aes_test_iv_1);
			ret = SUCCESS;
			// AES-CFB
			// enc: 3b 3f d9 2e b7 2d ad 20   33 34 49 f8 e8 3c fb 4a 
			// AES-OFB
			// enc: 3b 3f d9 2e b7 2d ad 20   33 34 49 f8 e8 3c fb 4a 
			break;

			
		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			ret = _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH;
			break;
	}
	
	if (ret != SUCCESS)
	{
		err_printk("ipsec encrypt failed!, ret=%d\n", ret);
		return ret;
	}


#if 1
	ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, pIv, ivlen, message, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
		err_printk("ipsecSim encrypt failed!\n");
		return ret;
	}
#endif
	
	if ( rtl_memcmpb(pResult_asic, pResult_sw, enl) == 0 )
	{
		dbg_mem_dump(pResult_asic, msglen, "test OK - enc:");
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
		//case CIPHER_TYPE_3DES_CBC: // 3DES-CBC
		//case CIPHER_TYPE_3DES_ECB : // 3DES-ECB
			//ret = vector_test_esp_3DES(crypto_type);
			//break;
			
		case 0x20: // AES-CBC
			ret = rtl_crypto_aes_cbc_decrypt(message, msglen, pIv,ivlen, pResult_asic);
			break;
			
		case 0x22 : // AES-ECB
			ret = rtl_crypto_aes_ecb_decrypt(message, msglen, pIv,ivlen, pResult_asic);
			break;

		case 0x23 : // AES-CTR
			ret = rtl_crypto_aes_ctr_decrypt(message, msglen, pIv,ivlen, pResult_asic);
			break;

		case 0x24 : // AES-CFB 
//			ret = rtl_crypto_aes_cfb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
			break;
			
		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			break;
	}

	if (ret != SUCCESS)
	{
		err_printk("ipsec decrypt failed!\n");
		return ret;
	}


	

#if 1
	ret = rtl_crypto_esp_sim(cipher_type, key, keylen, pIv, ivlen, message, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
		err_printk("ipsecSim encrypt failed!\n");
		return ret;
	}
#endif
	
	if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 )
	{
		dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
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


#ifdef NEW_CRYPTO

__attribute__ ((aligned (4)))
static const unsigned char chacha_test_key[32] = { 0 };
static const unsigned char chacha_test_buf[64] = { 0 };
static const unsigned char chacha_test_iv[8] = { 0 };

static const unsigned char chacha_test_res[64] ={

0x76, 0xb8, 0xe0, 0xad, 0xa0, 0xf1, 0x3d, 0x90,
0x40, 0x5d, 0x6a, 0xe5, 0x53, 0x86, 0xbd, 0x28, 
0xbd, 0xd2, 0x19, 0xb8, 0xa0, 0x8d, 0xed, 0x1a,
0xa8, 0x36, 0xef, 0xcc, 0x8b, 0x77, 0x0d, 0xc7,

0xda, 0x41, 0x59, 0x7c, 0x51, 0x57, 0x48, 0x8d,
0x77, 0x24, 0xe0, 0x3f, 0xb8, 0xd8, 0x4a, 0x37,
0x6a, 0x43, 0xb8, 0xf4, 0x15, 0x18, 0xa1, 0x1c,
0xc3, 0x87, 0xb6, 0x69, 0xb2, 0xee, 0x65, 0x86 };


int vector_test_cipher_chacha(u32 cipher_type)
{
    const u8 *key, *pIv;
//	u32 keylen= 0;
//	u32 ivlen = 0;
    u8 *message;
//    u8 *pResult_asic;
	u8 *pResult_sw;
    int ret;
	u32 msglen, enl;
	
	// Encryption

	key = chacha_test_key;
//	keylen = sizeof(chacha_test_key);
	message = &_asic_orig[32];
	msglen = sizeof(chacha_test_buf);
	rtl_memcpyb(message, chacha_test_buf, msglen);
    pIv = chacha_test_iv;
//	ivlen = sizeof(chacha_test_iv);	

//	pResult_asic = _asic_enc;		  
	pResult_sw   = _sw_enc;

	enl = msglen;

	switch ( cipher_type) {
		case 0x30: // chacha
			ret = SUCCESS;
			// chacha
			break;

			
		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			ret = _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH;
			break;
	}
	
	if (ret != SUCCESS)
	{
		err_printk("ipsec encrypt failed!, ret=%d\n", ret);
		return ret;
	}


#if 0
	CRYPTO_sim_chacha_20(pResult_sw, message, msglen, key, pIv, 0);

	//ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, pIv, ivlen, message, msglen, pResult_sw);
#endif
	
	if ( rtl_memcmpb(chacha_test_res, pResult_sw, enl) == 0 )
	{
		printk("test OK\n");
		dbg_mem_dump(pResult_sw, msglen, "test OK - enc:");
		ret = SUCCESS;
	} else {
		err_printk("test failed!\n");
		dbg_mem_dump(pResult_sw, msglen, "SW enc msg: ");
		return FAIL;
	}

	// Decryption

#if 0
    message = pResult_asic;

	pResult_asic = _asic_dec;		  
	pResult_sw   = _sw_dec;
	//pResult_correct = aes_test_buf;


	switch ( cipher_type) {

		case 0x30 : // chacha 
			ret = SUCCESS;
			break;
			
		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			break;
	}

	if (ret != SUCCESS)
	{
		err_printk("ipsec decrypt failed!\n");
		return ret;
	}


	

#if 1
	ret = rtl_crypto_esp_sim(cipher_type, key, keylen, pIv, ivlen, message, msglen, pResult_sw);
	if (ret != SUCCESS)
	{
		err_printk("ipsecSim encrypt failed!\n");
		return ret;
	}
#endif
	
	if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 )
	{
		dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
		return SUCCESS;
	} else {
		err_printk("test failed!\n");
		dbg_mem_dump(pResult_asic, msglen, "ASIC dec msg: ");
		dbg_mem_dump(pResult_sw, msglen, "SW dec msg: ");
		return FAIL;
	}
#endif

	return ret;

}
#endif

// DES-CBC   : 0x00
// DES-ECB   :  0x02
// 3DES-CBC : 0x10
// 3DES-ECB  : 0x12
//

int vector_test_cipher(u32 cipher_type)
{
	int ret = FAIL;

	switch ( cipher_type) {
		case 0x10: // 3DES-CBC
		case 0x12 : // 3DES-ECB
			ret = vector_test_cipher_3DES(cipher_type);
			break;
		case 0x20: // AES-CBC
		case 0x22 : // AES-ECB
		case 0x23 : // AES-CTR
		case 0x24 : // AES-CFB
		case 0x25 : // AES-OFB
			ret = vector_test_cipher_AES(cipher_type);
			break;
		case 0x30 : // Chacha
#if 0
			ret = vector_test_cipher_chacha(cipher_type);
#endif
			break;		
		default : 
			err_printk("no this crypto_type=0x%x\n", cipher_type);
			break;
	}

	return ret;
		
}


#if 0
static rtl_ipsecScatter_t scatter_th[1];
static u8 _cryptoKey_th[32 + 32 + 32]; // 32 for AES-256

//		throughput 0 0 -1 -> DES-CBC : 0x0
//		throughput 0 2 -1 -> DES-ECB : 0x2
//		throughput 0 16 -1 -> 3DES-CBC : 0x10
//		throughput 0 18 -1 -> 3DES-ECB : 0x12 
//		throughput 0 32 -1 -> AES-CBC :   0x20
//		throughput 0 34 -1 -> AES-ECB  :  0x22
//		throughput 0 35 -1 -> AES-CTR  : 0x23

#endif 

int throughput_test_cipher(u32 cipher_type, 
    void* key, u32 keylen, u8* iv, int ivlen, 
    u32 digestlen, u8 *message, int msglen, 
    void *pResult, u32 resultLen, int loops) 
{
	int loop, ret;
//	u32 start_ticks, end_ticks, spend_ticks, spend_time, throughput;
    u32 start_count, current_count, spend_time, spend_ticks, throughput;
	
	printk("%s start, msglen=%d, cpu_count_rate=%dMhz\n", __FUNCTION__, msglen, CPU_CP0_COUNT_RATE);
	//start_ticks = xTaskGetTickCount();
#ifndef DISABLE_AMEBA_NEW_ARCH
	start_count = HalTimerOp.HalTimerReadCount(1);
#else
	start_count = 0;
	set_c0_cause(CAUSEF_DC);
	write_c0_count(0);
	clear_c0_cause(CAUSEF_DC);
#endif

	switch (cipher_type) {
		case 0x0: // DES-CBC				
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_des_cbc_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_des_cbc_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed-%d!\n", ret);
					return ret;					
				}
			}
			break;
			
		case 0x2: // DES-ECB
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_des_ecb_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_des_ecb_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;

		case 0x10: // 3DES-CBC
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_3des_cbc_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_3des_cbc_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;
			
		case 0x12: // 3DES-ECB
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_3des_ecb_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_3des_ecb_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;


		case 0x20: // AES-CBC
			
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_aes_cbc_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_aes_cbc_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;
			
		case 0x22: // AES-ECB
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_aes_ecb_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_aes_ecb_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed! ret=%d\n", ret);
					return ret;					
				}
			}
			break;

		case 0x23: // AES-CTR
			for (loop=0; loop<loops; loop++) {
				ret = rtl_crypto_aes_ctr_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				ret = rtl_crypto_aes_ctr_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;

		case 0x24 : // AES-CFB 
			for (loop=0; loop<loops; loop++) {
				//ret = rtl_crypto_aes_cfb_init(key, keylen); 
				if (ret != SUCCESS)
				{
					err_printk("init failed!\n");
					return ret; 				
				}

				//ret = rtl_crypto_aes_cfb_encrypt(message, msglen, iv, ivlen, pResult);
				if (ret != SUCCESS)
				{
					err_printk("failed!\n");
					return ret;					
				}
			}
			break;

		case 0x25 : // AES-OFB
		case 0x40 : // CHACHA20
			ret = SUCCESS;
			break;

		
		default:
			err_printk("authtype(%d) : no this type\n", cipher_type);
			ret = FAIL;
	}

		
	//end_ticks = xTaskGetTickCount();
#ifndef DISABLE_AMEBA_NEW_ARCH
	current_count = HalTimerOp.HalTimerReadCount(1);	
#else
	set_c0_cause(CAUSEF_DC);
	
	current_count = read_c0_count();
#endif
	//if ( start_count > current_count) {
        //spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
    //}
    //else {
    //    spend_ticks = start_count - current_count;            
    //}
    spend_ticks = current_count;

	
	//spend_ticks = end_ticks - start_ticks;
	//spend_time = spend_ticks * portTICK_RATE_MS;
#ifndef DISABLE_AMEBA_NEW_ARCH
	spend_time = spend_ticks * 30 / loops;
#else
	spend_time = spend_ticks / (CPU_CP0_COUNT_RATE) / loops;
#endif
	if (spend_time == 0)
		throughput = 999;
	else
		throughput = (msglen*8*1000) / spend_time;
	printk("HW spend time each:%d, ticks:%d, throughput: %d kbps\n", 
		spend_time, spend_ticks, throughput);

	//
	// run SW simulation
	//
	//start_ticks = xTaskGetTickCount();
#ifndef DISABLE_AMEBA_NEW_ARCH
	start_count = HalTimerOp.HalTimerReadCount(1);
#else
	start_count = 0;
	set_c0_cause(CAUSEF_DC);
	write_c0_count(0);
	clear_c0_cause(CAUSEF_DC);
#endif

	for (loop=0; loop<loops; loop++) {
		ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, iv, ivlen, message, msglen, pResult);
	}
	
	//end_ticks = xTaskGetTickCount();
#ifndef DISABLE_AMEBA_NEW_ARCH
	current_count = HalTimerOp.HalTimerReadCount(1);	
#else
	set_c0_cause(CAUSEF_DC);
	current_count = read_c0_count();
#endif
//	if ( start_count > current_count) {
//        spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
//    }
//    else {
//        spend_ticks = start_count - current_count;            
//    }
	spend_ticks = current_count;
	//spend_ticks = end_ticks - start_ticks;
	//spend_time = spend_ticks * portTICK_RATE_MS;

#ifndef DISABLE_AMEBA_NEW_ARCH
	spend_time = spend_ticks * 30 / loops;
#else
	spend_time = spend_ticks / (CPU_CP0_COUNT_RATE) / loops;
#endif
	if (spend_time == 0)
		throughput = 999;
	else
		throughput = (msglen*8*1000) / spend_time;
	printk("SW spend time each:%d, ticks:%d, throughput: %d kbps\n", 
		spend_time, spend_ticks, throughput);

	return ret;
}


int throughput_test_auth(u32 authtype, 
    void* key, u32 keylen, 
    u32 digestlen, u8 *message, int msglen, 
    void *pResult, u32 resultLen, int loops) 
{
	int loop, ret = SUCCESS;
	//u32 start_ticks, end_ticks, spend_ticks, spend_time, throughput;
    u32 start_count, current_count, spend_time, spend_ticks, throughput;

	
	printk("%s start, msglen=%d, cpu_count_rate=%dMhz\n", __FUNCTION__, msglen, CPU_CP0_COUNT_RATE);
	//start_ticks = xTaskGetTickCount();

#ifndef DISABLE_AMEBA_NEW_ARCH
	start_count = HalTimerOp.HalTimerReadCount(1);
#else
	start_count = 0;
	set_c0_cause(CAUSEF_DC);
	write_c0_count(0);
	clear_c0_cause(CAUSEF_DC);
#endif

		switch (authtype) {
			case 2:				
				for (loop=0; loop<loops; loop++) {
					ret = rtl_crypto_md5_init();
					if (ret != SUCCESS)
					{
						err_printk("init failed!\n");
						return ret; 				
					}
					

					ret = rtl_crypto_md5_process(message, msglen, pResult);
					if (ret != SUCCESS)
					{
						err_printk("failed!\n");
						return ret;					
					}
				}
				break;
				
			case 6:
				for (loop=0; loop<loops; loop++) {
					ret = rtl_crypto_hmac_md5_init(key, keylen);
					if (ret != SUCCESS)
					{
						err_printk("init failed!, ret=%d\n", ret);
						return ret; 				
					}
					

					ret = rtl_crypto_hmac_md5_process(message, msglen, pResult);
					if (ret != SUCCESS)
					{
						err_printk("failed!, ret = %d\n", ret);
						return ret;					
					}
				}
				break;

			case 0:
				
				for (loop=0; loop<loops; loop++) {
					ret = rtl_crypto_sha1_init();
					if (ret != SUCCESS)
					{
						err_printk("init failed! ret=%d\n", ret);
						return ret; 				
					}

					ret = rtl_crypto_sha1_process(message, msglen, pResult);
					if (ret != SUCCESS)
					{
						err_printk("failed!, ret=%d\n", ret);
						return ret;					
					}
				}
				break;
				
			case 4:
				
				for (loop=0; loop<loops; loop++) {
					ret = rtl_crypto_hmac_sha1_init(key, keylen);
					if (ret != SUCCESS)
					{
						err_printk("init failed!\n");
						return ret; 				
					}
					
					ret = rtl_crypto_hmac_sha1_process(message, msglen, pResult);
					if (ret != SUCCESS)
					{
						err_printk("failed!\n");
						return ret;					
					}
				}
				break;
				
			case 1: 		
				for (loop=0; loop<loops; loop++) {
					ret = rtl_crypto_sha2_init(SHA2_256);
					if (ret != SUCCESS)
					{
						err_printk("init failed!\n");
						return ret; 				
					}
					
					ret = rtl_crypto_sha2_process(message, msglen, pResult);
					if (ret != SUCCESS)
					{
						err_printk("failed!\n");
						return ret;					
					}
				}
				break;
				
			case 5:
				for (loop=0; loop<loops; loop++) {
					ret = rtl_crypto_hmac_sha2_init(SHA2_256, key, keylen);
					if (ret != SUCCESS)
					{
						err_printk("init failed!\n");
						return ret; 				
					}
					
					
					ret = rtl_crypto_hmac_sha2_process(message, msglen, pResult);
					if (ret != SUCCESS)
					{
						err_printk("failed!\n");
						return ret;					
					}
				}
				break;

			case 7 : //poly1305
				ret = SUCCESS;
				break;
				
			default:
				err_printk("authtype(%d) : no this type\n", authtype);
				ret = FAIL;
		}
		
	//end_ticks = xTaskGetTickCount();
#ifndef DISABLE_AMEBA_NEW_ARCH
	current_count = HalTimerOp.HalTimerReadCount(1);	
#else
	set_c0_cause(CAUSEF_DC);
	current_count = read_c0_count();
#endif
	//if ( start_count > current_count) {
        //spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
    //}
    //else {
    //    spend_ticks = start_count - current_count;            
    //}
	spend_ticks = current_count;
	
	//spend_ticks = end_ticks - start_ticks;
	//spend_time = spend_ticks * portTICK_RATE_MS;

#ifndef DISABLE_AMEBA_NEW_ARCH
	spend_time = spend_ticks * 30 / loops;
#else
	spend_time = spend_ticks / (CPU_CP0_COUNT_RATE) / loops;
#endif
	if (spend_time == 0)
		throughput = 999;
	else
		throughput = (msglen*8*1000) / spend_time;
	printk("HW spend time each:%d, ticks:%d, throughput: %d kbps\n", 
		spend_time, spend_ticks, throughput);
	

	//
	// test sofware 
	//

	//start_ticks = xTaskGetTickCount();
#ifndef DISABLE_AMEBA_NEW_ARCH
	start_count = HalTimerOp.HalTimerReadCount(1);
#else
	start_count = 0;
	set_c0_cause(CAUSEF_DC);
	write_c0_count(0);
	clear_c0_cause(CAUSEF_DC);
#endif
	
	for (loop=0; loop<loops; loop++) {
	    ret = rtl_crypto_auth_sim(authtype, key, keylen, message, msglen, pResult, digestlen);
		if (ret != SUCCESS)
		{
			err_printk("failed!\n");
			return ret; 				
		}
	}
	//end_ticks = xTaskGetTickCount();
#ifndef DISABLE_AMEBA_NEW_ARCH
	current_count = HalTimerOp.HalTimerReadCount(1);	
#else
	set_c0_cause(CAUSEF_DC);
	current_count = read_c0_count();
#endif
//	if ( start_count > current_count) {
        //spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
    //}
    //else {
    //    spend_ticks = start_count - current_count;            
    //}
	spend_ticks = current_count;
	
	//spend_ticks = end_ticks - start_ticks;
	//spend_time = spend_ticks * portTICK_RATE_MS;
#ifndef DISABLE_AMEBA_NEW_ARCH
	spend_time = spend_ticks * 30 / loops;
#else
	spend_time = spend_ticks / (CPU_CP0_COUNT_RATE) / loops;
#endif
	if (spend_time == 0)
		throughput = 999;
	else
		throughput = (msglen*8*1000) / spend_time;
	printk("SW spend time each:%d, ticks:%d, throughput: %d kbps\n", 
		spend_time, spend_ticks, throughput);


	return ret;
}


int throughput_test(u32 modetype, u32 cipher_type, u32 authtype, u32 loops)
{
//	u8 *pAsicDigest = &_AsicDigest[32];
	u32 digestlen;

	u32 auth_keylen, crypt_keylen, ivlen;
	u8 *auth_key, *crypt_key, *pIv;
	u32 msglen;
	u8 *message;
	int i;
	int ret = FAIL;

	u8 *pResult_asic;
//	u8 *pResult_sw;

	//int loop;
	//u32 start_ticks, end_ticks, spend_ticks, spend_time, throughput;
	//u32 this_rtlseed;

	//u32 a2eo, enl;

	

	dbg_printk("modetype=0x%x, cipher_type=0x%x, authtype=0x%x\n", 
		modetype, cipher_type, authtype);


	ivlen = 8;
	switch (cipher_type) {
		case _MD_NOCRYPTO:
			crypt_keylen = 0;
			break;
	    case 0x00:
			crypt_keylen = 8;
			break;
		case 0x02:
			crypt_keylen = 8;
			break;
		case 0x10:
			crypt_keylen = 24;
			break;
		case 0x12:
			crypt_keylen = 24;
			break;
		case 0x20:
		case 0x24:
		case 0x25:
			//crypt_keylen = 32;
			crypt_keylen = 16+8*crypt_keylen_sel;
			ivlen = 16;
			break;
		case 0x22:
			//crypt_keylen = 32;
			crypt_keylen = 16+8*crypt_keylen_sel;
			ivlen = 0;
			break;
		case 0x23:
			//crypt_keylen = 32; 
			crypt_keylen = 16+8*crypt_keylen_sel;
			ivlen = 8;
			break;
		case 0x30: // chacha20
			crypt_keylen = 32; 
			ivlen = 8;
			break;
	    default:
	        err_printk("no this cipher type\n");
			return FAIL;
	}

	switch (authtype) {
		case _MD_NOAUTH:
			digestlen = 0;
			break;
	    case 2:
	    case 6:
	        digestlen = 16;
	        break;
	    case 0:
	    case 4:
	        digestlen = 20;
	        break;
		case 1:			
		case 5:
			digestlen = 32;
			break;
		case 7: // poly1305
			digestlen = 16;
			break;
			
	    default:
	        err_printk("authtype(%d) : no this type\n", authtype);
	        return FAIL;
	}


#ifdef CRYPTO_TEST_SRAM
	crypt_key = (u8 *) (0x9fe00000+1024*4);
#else
	crypt_key = &_asic_cryptkey[0];
#endif
	for (i=0; i<crypt_keylen; i++)
	      crypt_key[i] = rtl_random() & 0xFF;
	crypt_key[i] = '\0';



	// Suppose keylen = 0~ BLOCK(64)
	if ( (digestlen == 0) || (authtype < 3) ) { // no HMAC
		auth_keylen = 0;
		auth_key = NULL;
	} else {
	  auth_keylen = ( rtl_random() % 63 )+1;
	  auth_key = &_asic_authkey[0];
	  for (i=0; i<auth_keylen; i++)
	      auth_key[i] = rtl_random() & 0xFF;
	  auth_key[i] = '\0';
	}

	if ( cipher_type != _MD_NOCRYPTO ) {

#ifdef CRYPTO_TEST_SRAM
		pIv = (u8 *) (0x9fe00000+1024*4+64);
#else
		pIv = _asic_iv;
#endif
		for (i=0; i<ivlen; i++)
			pIv[i] = rtl_random() & 0xFF;
	} else {
		pIv = NULL;
		ivlen = 0;
	}
	

	// 32 byte * 50 = 1600
	msglen = 1600;
	//msglen = 1504;//16 byte * 94

#ifdef CRYPTO_TEST_SRAM
	message = (u8 *) (0x9fe00000);
	*((u32 *) (0xbfe00000)) = 0;
#else
	message = &_asic_orig[32];
#endif

	for (i=0; i<msglen; i++)
	  message[i] = rtl_random() & 0xFF;
	message[i] = '\0';


	//a2eo = 0;
	//enl = msglen;

#ifdef CRYPTO_TEST_SRAM
	pResult_asic = (u8 *) (0x9fe00000+2048);
	*((u32 *) (0xbfe00000+2048)) = 0;
#else
	pResult_asic = _asic_enc;
#endif

#if 0

	//
	// verify function first;
	// 

	scatter_th[0].len = msglen;
	scatter_th[0].ptr = message;

	pResult_sw	 = _sw_enc;


	ret = rtl_ipsecEngine(&g_rtl_IPSec_Engine, modetype, cipher_type|0x80, authtype, 
	   crypt_key, crypt_keylen, 
	   auth_key,auth_keylen,
	   pIv, ivlen, digestlen,
	   1, scatter_th, 
	   a2eo, enl, 
	   pResult_asic, sizeof(_asic_enc));

	rtl_assert( ret == SUCCESS );


	ret = rtl_ipsec_mixed_sim(modetype, cipher_type|0x80, authtype, 
		crypt_key, crypt_keylen, auth_key, auth_keylen, pIv, ivlen, 
		message, a2eo+enl, pResult_sw, a2eo, enl, digestlen);
	if (ret != SUCCESS)
	{
	    err_printk("ipsec_mixed_sim failed!\n");
	    goto ret_throughput_test;
	}

	if (memcmp(pResult_asic, pResult_sw, a2eo+enl) == 0)
	{
	    dbg_printk("IPSec ok\n");
	    ret = SUCCESS;
	}
	else
	{
	    ret = FAIL;
		goto ret_throughput_test;
	}
#endif

	if ( modetype == _MS_TYPE_AUTH ) {
		
			throughput_test_auth(authtype,
				auth_key, auth_keylen, digestlen, message, msglen, pResult_asic, sizeof(_asic_enc), loops);

			
	} 
	else {

			throughput_test_cipher(cipher_type,
				crypt_key, crypt_keylen, pIv, ivlen, digestlen, message, msglen, pResult_asic, sizeof(_asic_enc), loops);

	}

#if 0 
		//
		// 
		// 
		printk("Start looping to calculate HW throughput\n");
		start_ticks = xTaskGetTickCount();
		for (loop=0; loop<loops; loop++) {

			ret = rtl_ipsecEngine(&g_rtl_IPSec_Engine, modetype, cipher_type|0x80, authtype, 
			   crypt_key, crypt_keylen, 
			   auth_key,auth_keylen,
			   pIv, ivlen, digestlen,
			   1, scatter_th, 
			   a2eo, enl, 
			   pResult_asic, sizeof(_asic_enc));
			
			if (ret != SUCCESS)
			{
				err_printk("failed!\n");
				goto ret_throughput_test;
			}
		}
		end_ticks = xTaskGetTickCount();
		spend_ticks = end_ticks - start_ticks;
		spend_time = spend_ticks * portTICK_RATE_MS;
		throughput = (loops*(msglen*8)) / (spend_time*1000);
		printk("HW spend time:%d, ms:%d, throughput: %d Mbps, idea : %d MPS\n", 
			spend_ticks, spend_time, throughput, throughput * 40000000/SYSTEM_CLK);
	//
	//
	//
	printk("Start looping to calculate SW throughput\n");
	start_ticks = xTaskGetTickCount();
	for (loop=0; loop<loops; loop++) {

		ret = rtl_ipsec_mixed_sim(modetype, cipher_type|0x80, authtype, 
			crypt_key, crypt_keylen, auth_key, auth_keylen, pIv, ivlen, 
			message, a2eo+enl, pResult_sw, a2eo, enl, digestlen);
		
		if (ret != SUCCESS)
		{
			err_printk("failed!\n");
			goto ret_throughput_test;
		}
	}
	end_ticks = xTaskGetTickCount();
	spend_ticks = end_ticks - start_ticks;
	spend_time = spend_ticks * portTICK_RATE_MS;
	throughput = (loops*(msglen*8)) / (spend_time*1000);
	printk("SW spend time:%d, ms:%d, throughput: %d Mbps, idea : %d MPS\n", 
		spend_ticks, spend_time, throughput, throughput * 40000000/SYSTEM_CLK);

#endif
	
	ret = SUCCESS;
	

	return ret;


}


extern int rtl_crypto_test(IN u16 argc, IN u8 *argv[]);
extern void Start_crypto_int(int interrupt);

int rtl_crypto_test(u16 argc, u8 *argv[])
{
	int ret;
	
	dbg_printk("argc=%d\n", argc);
	dbg_printk("argv[0]=%s\n", argv[0]);
    if (argc < 1)
    {
        rtl_crypto_usage(NULL);
        return _FALSE;
    }

    if (strcmp(argv[0], "init") == 0)
    {
		dbg_printk("doing init\n");
		ret = rtl_cryptoEngine_init();
		if(  ret == SUCCESS ) {
			printk("Crypto Engine Init Success!!!\n");
			return _TRUE;
		} else {
			printk("Crypto Engine Init FAILED, ret=%d\n", ret);
			return _FALSE;
		}
    }
    else if (strcmp(argv[0], "info") == 0)
    {
		dbg_printk("Hello info\n");
        rtl_cryptoEngine_info();
    }
    else if (strcmp(argv[0], "test") == 0)
    {
        int loop=1;
	//int loop=1373;
	int stop_run_keyin;
        //int loop=1300;//800;//200;
        //int loop=3200;//1800;//1500;

		int ret; 

		
		printk("Hello test \n");
		rtl_srandom(0xA0A0A0A0);
		//rtl_srandom(0x7fcd4df5);
		//rtl_srandom(0xa3e814f9);//0x1c041653);//0xd2c99c96);
		//rtl_srandom(0xead4d9d9);//0xead4d9d9);//0x1c041653);
        //int loop=6841;
	//rtl_srandom(0x8961b0f0);
        for (;;) {
			if ( (loop % 100) == 0 )  
 			    printk("Round %d, _rtl_seed=%x \n", loop, rtl_grandom() );
			if (tstc()) {
				stop_run_keyin = getc();
				printf("stop_run_keyin=%c", stop_run_keyin);
				if (stop_run_keyin == 's') {
					printf("stop test\n");
					printk("Round %d, _rtl_seed=%x \n", loop, rtl_grandom() );
					break;
				}
			}
			if ( (loop % 2) == 0 ) 
				ret = vector_test_cipher_rand();
			else 
				ret = vector_test_auth_rand();
            if ( ret != SUCCESS ) {
				printk("failed\n");
				printk("Round %d\n", loop);
				break;
            }
            loop++;
        }
        
    }
    else if (strcmp(argv[0], "rand") == 0)
    {
        static int seed = 0;
        int round;
        int mode_crypto;
        int mode_auth;
        int cntScatter;

        // Random test
        if (argc == 1)
        {
            crypto_GeneralApiRandTest(seed++, 100, 0xff, 0xff, 0xff); // 0xff for random
        }
        else if (argc == 3)
        {
            seed = Strtoul((const char*)(argv[1]), (char **)NULL, 10);
            round = Strtoul((const char*)(argv[2]), (char **)NULL, 10);
            crypto_GeneralApiRandTest(seed, round, 0xff, 0xff, 0xff); // 0xff for random
        }
        else if (argc == 5 || argc ==6)
        {
            //todo
            // CRYPTO:
            //   0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES
            //   0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES
            // AUTH:
            //   0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1

        }
        else
        {
            rtl_crypto_usage("rand");
            return;
        }
    }
    else if (strcmp(argv[0], "debug") == 0)
    {
    	u8 mode_select;
		
        if (argc != 2)
        {
            rtl_crypto_usage("debug");
            return _FALSE;
        }

		mode_select = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
		 //issue_test1();

		switch ( mode_select ) {
			case 0 : 
#ifndef DISABLE_AMEBA_NEW_ARCH
				DBG_INFO_MSG_OFF(_DBG_CRYPTO_);
#else
				ConfigDebugInfo &= (~_DBG_CRYPTO_);
#endif
				printk("Crypto Debug Info turn OFF\n");
				break;

			case 1 : 
#ifndef DISABLE_AMEBA_NEW_ARCH
				DBG_INFO_MSG_ON(_DBG_CRYPTO_);
#else
				ConfigDebugInfo |= (_DBG_CRYPTO_);
#endif
				printk("Crypto Debug Info turn ON\n");
				break;

			default : 
				rtl_crypto_usage("debug");
				break;
		}
    	
    }
    else if (strcmp(argv[0], "interrupt") == 0)
    {
        int mode_select;
        mode_select = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
	printf("abc\n");
        Start_crypto_int(mode_select);
    }
	
#if 1

//
//	Usage : 
//
//
//		throughput 0 0 -1 -> DES-CBC : 0x0
//		throughput 0 2 -1 -> DES-ECB : 0x2
//		throughput 0 16 -1 -> 3DES-CBC : 0x10
//		throughput 0 18 -1 -> 3DES-ECB : 0x12 
//		throughput 0 32 -1 -> AES-CBC :   0x20
//		throughput 0 34 -1 -> AES-ECB  :  0x22
//		throughput 0 35 -1 -> AES-CTR  : 0x23
//		throughput 0 36 -1 -> AES-CFB  : 0x24
//		throughput 0 37 -1 -> AES-OFB  : 0x25
//
//		throughput 1 -1 0 -> SHA1
//		throughput 1 -1 1 -> SHA2
//		throughput 1 -1 2 -> MD5
//		throughput 1 -1 4 -> HMAC-SHA1
//		throughput 1 -1 5 -> HMAC-SHA2
//		throughput 1 -1 6 -> HMAC-MD5
//
//		throughput 2 0 0 -> aes key 128bit
//		throughput 2 1 0 -> aes key 192bit
//		throughput 2 2 0 -> aes key 256bit
//
//		throughput 3 0 0 -> apteam mix-mode throughput

    
	else if (strcmp(argv[0], "throughput") == 0)
	{
		u32 modetype, crypttype, authtype, loops;


        modetype = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
        //crypttype = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
        //authtype = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);
        //loops = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 10);

		if (modetype == 2) {
			crypt_keylen_sel = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
			printk("crypt_keylen_sel=%d \n", crypt_keylen_sel);
		} else if (modetype ==3) {
			int i=0;
			printf("==> %s:%d, start test\n", __func__, __LINE__);
			for (i=0;i<0xfffffff;i++) {
				//mix8651bAsicThroughput(4, 0xa0, 0xa0, 3, 3, 32, 8, 1408, 16);
				mix8651bAsicThroughput(100000, 0x20, 0x20, 4, 4, 32, 8, 1408, 16);
				//if (i%0xffff == 0x1ffe) {
					printf("==> %s:%d, i=0x%x\n", __func__, __LINE__, i);
				//}
				//if (i%0xfff == 0x2fe) {
				//	printf(".", __func__, __LINE__, i);
				//}
				//printf("-");
			}
			
		} else {
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
	}	
#endif // temp
//
//	Usage : 
//
//		vector 0 16 -1 -> 3DES-CBC : 0x10
//		vector 0 18 -1 -> 3DES-ECB : 0x12 
//		vector 0 32 -1 -> AES-CBC :   0x20
//		vector 0 34 -1 -> AES-ECB  :  0x22
//		vector 0 35 -1 -> AES-CTR  : 0x23
//          vector 0 36 -1 -> AES-CFB   : 0x24 
//          vector 0 37 -1 -> AES-OFB   : 0x25
//		vector 0 48 -1 -> Chacha20 : 0x30
//
//		vector 1 -1 0 -> SHA1
//		vector 1 -1 1 -> SHA2
//		vector 1 -1 2 -> MD5
//		vector 1 -1 4 -> HMAC-SHA1
//		vector 1 -1 5 -> HMAC-SHA2
//		vector 1 -1 6 -> HMAC-MD5
//	       vector 1 -1 7 -> Poly1305
//
//		vector 2 16 0 -> 3DES-CCB-SHA1 : MtE (MAC-then-Encrypt, for SSL)
//		MtE : HMAC(message) ->> Encrypt(message + HMAC )
//
    else if (strcmp(argv[0], "vector") == 0)
    {
        int ret; 
		u32 mode_select, cipher_type, auth_type;

        if (argc != 4)
        {
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
			default:
				err_printk("No this mode_select type:%d\n", mode_select);
				ret = _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;
				break;
		}

        printk("vector test done. ret=%d\n", ret);
        return _TRUE;
    }
    else
    {
        rtl_crypto_usage(NULL);
		return _FALSE;
    }

	return _TRUE;
}

#if 0 // fips trng test jwsyu 2016
extern int rtl_crypto_trng_test(IN u16 argc, IN u8 *argv[]);
extern void Start_trng_test(int test);

int rtl_crypto_trng_test(u16 argc, u8 *argv[])
{
	int test;
	
	test = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);

	printf("test=%d", test);
	Start_trng_test(test);

	return _TRUE;
}


extern int rtl_crypto_trng_throughput_test(IN u16 argc, IN u8 *argv[]);
extern void Start_trng_test(int test);

int rtl_crypto_trng_throughput_test(u16 argc, u8 *argv[])
{
	//int test;
	
	//test = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);

	//printf("test=%d", test);
	Start_trng_throughput_test();

	return _TRUE;
}
#endif


