/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003  
* All rights reserved.
* 
* $Header: /home/cvsroot/uClinux-dist/linux-2.4.x/drivers/net/re865x/crypto/865xc/cryptoCmd.c,v 1.17 2007/06/04 18:12:54 michaelhuang Exp $
*
* Abstract: Crypto engine access command driver source code.
* $Author: michaelhuang $
* $id: $
* ---------------------------------------------------------------
*/

//#include <rtl_types.h>
#include "rtl_types.h"
//#include <rtl_cle.h> 
//#include <cryptoCmd.h>
//#include <authTest.h>
//#include <rtl_glue.h>
//#include <rtl_utils.h>
//#include <crypto.h>
#include "crypto.h"
#include "asicRegs.h"
#include "rtl865xc_authEngine.h"
#include "rtl865xc_cryptoEngine.h"
#include "rtl865xc_ipsecEngine.h"
//#include <aesTest.h>
//#include <authTest.h>
//#include <destest.h>
//#include <hmactest.h>
//#include <md5test.h>
//#include <sha1test.h>
//#include <desSim.h>
#include "desSim.h"
#include "aesSim.h"
//#include <authSim.h>
#include "aes.h"
//#include "aes_1.h"
//#include "aes_locl.h"
//#ifdef RTL865X_MODEL_KERNEL
//#include <gdmaTest.h>
#include "gdmaTest.h"
//#endif
#include "cryptoCmd.h"
#include "util.h"

#define BUFFER_LEN		(1<<14)

#define RTL865X_MODEL_KERNEL
/* use cachable address to speed up testing */
#define USE_CACHE

static uint32 BurstSize; /* Crypto DMA Burst size */
#ifdef RTL865X_MODEL_KERNEL
static uint64 initial_case_no = 0;
#endif

#if 0
#undef SUPPORT_AES_1 /* AES_1 is a slower implementation of AES */

int32 _rtl8651b_cryptoCmd(uint32 userId,  int32 argc,int8 **saved) {
	int32 size;
	int8 *nextToken;
	
	cle_getNextCmdToken(&nextToken,&size,saved); 

	if(!strcmp(nextToken, "init")){
		uint32 descNum;
		
		cle_getNextCmdToken(&nextToken,&size,saved); 
		descNum = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		BurstSize = U32_value(nextToken);	
		if( BurstSize == 64 ) 
		{	
			rtlglue_printf("Initialize crypto engine driver with %u descriptors working on 64-byte mode\n", descNum);
			return rtl8651b_cryptoEngine_init(descNum, 2);
		}
		else if( BurstSize == 32 ) 
		{	
			rtlglue_printf("Initialize crypto engine driver with %u descriptors working on 32-byte mode\n", descNum);
			return rtl8651b_cryptoEngine_init(descNum, 1);
		}
		else if( BurstSize == 16 )
		{	
			rtlglue_printf("Initialize crypto engine driver with %u descriptors working on 16-byte mode\n", descNum);
			return rtl8651b_cryptoEngine_init(descNum, 0);
		}
		else
			return FAILED;
		return SUCCESS;
	}
	else if(!strcmp(nextToken, "sim")){
		rtlglue_printf("Run des and des simulator test\n");
		destest();
		return SUCCESS;
	}
	else if(!strcmp(nextToken, "rand")){
		uint32 randSeed, roundLimit;
		cle_getNextCmdToken(&nextToken,&size,saved); 
		randSeed = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		roundLimit = U32_value(nextToken);	
		rtlglue_printf("Run DES random test seed %u and %u rounds\n", randSeed, roundLimit);
		return runDes8651bGeneralApiRandTest(randSeed, roundLimit);
	}
	else if(!strcmp(nextToken, "asic")){
		int32 retval;
		uint32 round, funStart, funEnd, lenStart, lenEnd, offsetStart, offsetEnd;
		rtlglue_printf("Run des simulator and 8651b crypto engine test\n");
		cle_getNextCmdToken(&nextToken,&size,saved); 
		round = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		funStart = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		funEnd = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		lenStart = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		lenEnd = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		offsetStart = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		offsetEnd = U32_value(nextToken);	
		rtlglue_printf("Round %u Function %u - %u Len %u - %u Offset %u - %u\n", round, funStart, funEnd, lenStart, lenEnd, offsetStart, offsetEnd);
//		des8651bTest(round, funStart, funEnd, lenStart, lenEnd, offsetStart, offsetEnd);
		retval = runDes8651bGeneralApiTest(round, funStart, funEnd, lenStart, lenEnd, offsetStart, offsetEnd);
#ifdef	CONFIG_RTL865X_MODEL_TEST_FT2
			if ( retval == SUCCESS ) {
				rtlglue_printf("Crypto runDes8651bGeneralApiTest Pass! {P902}\n");
			} else {
				rtlglue_printf("Crypto runDes8651bGeneralApiTest Fail! {F902}\n");
			}
#endif
		return retval;
	}
	else if(!strcmp(nextToken, "throughput")) {
		uint32 round, startMode, endMode, pktLen;
		cle_getNextCmdToken(&nextToken,&size,saved); 
		 if(!strcmp(nextToken, "asic")) {
			cle_getNextCmdToken(&nextToken,&size,saved); 
			round = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			startMode = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			endMode = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			pktLen = U32_value(nextToken);	
		 	des8651bAsicThroughput(round, startMode, endMode, pktLen);
			return SUCCESS;
		 }
		 else if(!strcmp(nextToken, "sw")) {
			cle_getNextCmdToken(&nextToken,&size,saved); 
			round = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			startMode = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			endMode = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			pktLen = U32_value(nextToken);	
		 	des8651bSwThroughput(round, startMode, endMode, pktLen);
			return SUCCESS;
		 }
	}

	return FAILED;
}

int32 _rtl8651b_authenticationCmd(uint32 userId,  int32 argc,int8 **saved) {
	int32 size;
	int8 *nextToken;
	
	cle_getNextCmdToken(&nextToken,&size,saved); 

	if(!strcmp(nextToken, "init")){
		uint32 descNum;
		cle_getNextCmdToken(&nextToken,&size,saved); 
		descNum = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		BurstSize = U32_value(nextToken);	
		if(BurstSize == 64) 
		{	
			rtlglue_printf("Initialize authentication engine driver with %u descriptors working on 64-byte mode\n", descNum);
			return rtl8651b_authEngine_init(descNum, 2);
		}
		else if(BurstSize == 32) 
		{	
			rtlglue_printf("Initialize authentication engine driver with %u descriptors working on 32-byte mode\n", descNum);
			return rtl8651b_authEngine_init(descNum, 1);
		}
		else 
		{	
			rtlglue_printf("Initialize authentication engine driver with %u descriptors working on 16-byte mode\n", descNum);
			return rtl8651b_authEngine_init(descNum, 0);
		}
		return SUCCESS;
	}
	else if(!strcmp(nextToken, "sim")){
		rtlglue_printf("Run MD5 and MD5 simulator test\n");
		md5test();
		rtlglue_printf("Run SHA-1 and SHA-1 simulator test\n");
		sha1test();
		rtlglue_printf("Run HMAC MD5 and HMAC MD5 simulator test\n");
		hmactest();
		rtlglue_printf("Run HMAC random test\n");
		hmacSimRandTest(2);
		return SUCCESS;
	}
	else if(!strcmp(nextToken, "rand")){
		uint32 randSeed, roundLimit;
		cle_getNextCmdToken(&nextToken,&size,saved); 
		randSeed = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		roundLimit = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		rtlglue_printf("Run %s random test seed %u for %u rounds\n",nextToken, randSeed, roundLimit);
		if(!strcmp(nextToken, "asic"))
			return runAuth8651bGeneralApiRandTest(randSeed, roundLimit);
		else
			return hmacSimRandTest(roundLimit);
	}
	else if(!strcmp(nextToken, "asic")) {
		int32 retval;
		uint32 round, funStart, funEnd, lenStart, lenEnd, keyLenStart, keyLenEnd, offsetStart, offsetEnd;
		rtlglue_printf("Run auth simulator and 8651b auth engine test\n");
		cle_getNextCmdToken(&nextToken,&size,saved); 
		round = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		funStart = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		funEnd = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		lenStart = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		lenEnd = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		keyLenStart = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		keyLenEnd = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		offsetStart = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		offsetEnd = U32_value(nextToken);	
		rtlglue_printf("Round %u Function %u - %u Len %u - %u keyLen %u - %u Offset %u - %u \n", round, funStart, funEnd, lenStart, lenEnd, keyLenStart, keyLenEnd, offsetStart, offsetEnd);
		retval = runAuth8651bGeneralApiTest(round, funStart, funEnd, lenStart, lenEnd, keyLenStart, keyLenEnd, offsetStart, offsetEnd);
#ifdef	CONFIG_RTL865X_MODEL_TEST_FT2
			if ( retval == SUCCESS ) {
				rtlglue_printf("Crypto runAuth8651bGeneralApiTest Pass! {P903}\n");
			} else {
				rtlglue_printf("Crypto runAuth8651bGeneralApiTest Fail! {F903}\n");
			}
#endif
		return retval;
	}
	else if(!strcmp(nextToken, "throughput")) {
		uint32 round, startMode, endMode, pktLen;
		cle_getNextCmdToken(&nextToken,&size,saved); 
		 if(!strcmp(nextToken, "asic")) {
			cle_getNextCmdToken(&nextToken,&size,saved); 
			round = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			startMode = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			endMode = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			pktLen = U32_value(nextToken);	
		 	auth8651bAsicThroughput(round, startMode, endMode, pktLen);
			return SUCCESS;
		 }
		 else if(!strcmp(nextToken, "sw")) {
			cle_getNextCmdToken(&nextToken,&size,saved); 
			round = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			startMode = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			endMode = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			pktLen = U32_value(nextToken);	
		 	auth8651bSwThroughput(round, startMode, endMode, pktLen);
			return SUCCESS;
		 }
	}
	return FAILED;
}



enum ENUM_AES_TYPE
{
	CBC_AES = 0x20,
	ECB_AES = 0x22,
	CTR_AES = 0x23,
};

typedef struct AES_test_pattern_s 
{
	enum ENUM_AES_TYPE type;
	int32	keyLen;
	uint8	*key;
	uint32	ivLen;
	uint8	*iv;
	uint32	dataLen;
	uint8	*plain;
	uint8	*cipher;
} AES_test_pattern_t;

uint8 *buffer1, *buffer2, *plainText;
//static uint8 buffer1[BUFFER_LEN], buffer2[BUFFER_LEN];
//static uint8 plainText[BUFFER_LEN];

static void AES_printit(AES_test_pattern_t *pt, uint8 *en, uint8 *de) {
	int32 n;

	rtlglue_printf("\nKey:        ");
	for(n=0; n<(pt->keyLen); n++)
		rtlglue_printf("%02x", pt->key[n]);
	rtlglue_printf("\nIV:         ");
	for(n=0; n<(pt->ivLen); n++)
		rtlglue_printf("%02x", pt->iv[n]);
	rtlglue_printf("\nPlain Text: ");
	for(n=0; n<pt->dataLen; n++)
		rtlglue_printf("%02x", pt->plain[n]);
	rtlglue_printf("\nCipher:     ");
	for(n=0; n<pt->dataLen; n++)
		rtlglue_printf("%02x", pt->cipher[n]);

	/* Make sure the correct result */	
	for(n=0; n<pt->dataLen; n++)
		if (pt->cipher[n] != en[n]) {
			rtlglue_printf("\nAES_ENCRYPT: failure!!!");
			rtlglue_printf("\nEncode:     ");
			for(n=0; n<pt->dataLen; n++)
				rtlglue_printf("%02x", en[n]);
		}
	for(n=0; n<pt->dataLen; n++)
		if (pt->plain[n] != de[n]) {
			rtlglue_printf("\nAES_DECRYPT: failure!!!");
			rtlglue_printf("\nDecode:     ");
			for(n=0; n<pt->dataLen; n++)
				rtlglue_printf("%02x", de[n]);
		}
	rtlglue_printf("\n");
}


enum ENUM_SEL 
{
	SEL_AES_0 = 1,
	SEL_AES_1 = 2,
	SEL_INIT = 3,
	SEL_RAND = 4,
	SEL_ASIC = 5,
} sel;


static void AES_timeTrial(int32 sel, int8 *mode, int32 keylen, int32 blkSize, int32 c) {
	uint8 key[] =  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b ,0x1c, 0x1d, 0x1e, 0x1f };
	uint8 ivec[16];
	AES_KEY aesKey; /* for AES-0 */
	uint32 p_time, l_time;
	uint32 enRate, deRate, encryptRate, decryptRate, encyptKeyExpand, decyptKeyExpand;
	uint32 en_dif, de_dif, dif;
	int32 i;

	enRate = deRate = encyptKeyExpand = decyptKeyExpand = -1;
	buffer1 = (uint8 *)rtlglue_malloc(BUFFER_LEN);
	buffer2 = (uint8 *)rtlglue_malloc(BUFFER_LEN);
	plainText = (uint8 *)rtlglue_malloc(BUFFER_LEN);
	assert(buffer1 && buffer2 && plainText);
	
	if (blkSize > BUFFER_LEN || (blkSize%16)!=0)
		return;
	for(i=0; i<BUFFER_LEN; i++)
		plainText[i] = i&0xff;
	for(i=0; i<16; i++)
		ivec[i] = (((i*100 + 4)<<2) & 0xff);
	
	if(!strcmp(mode, "ebc")) {
#if 0	
		/* (1) Estimate the key expansion time for encryption */
		if (sel == SEL_AES_0) {
			rtlglue_getmstime(&p_time);
			for(i=0; i<10000; i++)
				AES_set_encrypt_key(key, keylen, &aesKey);
			rtlglue_getmstime(&l_time);
		} else {
			gen_tabs();
			rtlglue_getmstime(&p_time);
			for(i=0; i<10000; i++)
				aes_encrypt_key(key, keylen, &aes_ctx);
			rtlglue_getmstime(&l_time);
		}
		encyptKeyExpand = (l_time - p_time +1 );
#endif
		/* (2) Estimate Encryption time not include key expansion time */
		if (sel == SEL_AES_0) {
			AES_set_encrypt_key(key, keylen, &aesKey);
			memset(&(aesKey.rd_key), 0, 240);
			rtlglue_printf("Encrypt=>> round: %d\n", aesKey.rounds);
			rtlglue_getmstime(&p_time);
			for(i=0; i<50000; i++) 
				AES_ecb_encrypt(plainText, buffer1, &aesKey, AES_ENCRYPT);
			rtlglue_getmstime(&l_time);
		} else {
#ifdef SUPPORT_AES_1
			rtlglue_getmstime(&p_time);
			for(i=0; i<50000; i++)
				aes_encrypt(plainText, buffer1, &aes_ctx);
			rtlglue_getmstime(&l_time);
#endif
		}
		en_dif = l_time-p_time; 
		encryptRate = (50000*128)/(en_dif*10);

#if 0		
		/* (3) Estimate the key expansion time for decryption */
		if (sel == SEL_AES_0) {
			rtlglue_getmstime(&p_time);
			for(i=0; i<10000; i++)
				AES_set_decrypt_key(key, keylen, &aesKey);
			rtlglue_getmstime(&l_time);
		} else {
			rtlglue_getmstime(&p_time);
			for(i=0; i<10000; i++)
				aes_decrypt_key(key, keylen, &aes_ctx);
			rtlglue_getmstime(&l_time);
		}
		decyptKeyExpand = (l_time - p_time +1 );
#endif

		/* (4) Estimate Decryption time not include key expansion time */
		if (sel == SEL_AES_0) {
			AES_set_decrypt_key(key, keylen, &aesKey);
			memset(&(aesKey.rd_key), 0, 240);
			rtlglue_printf("Decrypt=>> round: %d\n", aesKey.rounds);
			rtlglue_getmstime(&p_time);
			for(i=0; i<50000; i++)
				AES_ecb_encrypt(plainText, buffer1, &aesKey, AES_DECRYPT);
			rtlglue_getmstime(&l_time);
		} else  {
#ifdef SUPPORT_AES_1
			rtlglue_getmstime(&p_time);
			for(i=0; i<50000; i++)
				aes_decrypt(plainText, buffer1, &aes_ctx);
			rtlglue_getmstime(&l_time);
#endif
		}
		de_dif = l_time - p_time;
		decryptRate = (50000*128)/(de_dif*10);

#if 0				
		/* (5) Estimate the time for AES encryption */
		if (sel == SEL_AES_0) {
			rtlglue_getmstime(&p_time);
			for(i=0; i<c; i++) {
				AES_set_encrypt_key(key, keylen, &aesKey);
				for(n=blkSize>>4,p=plainText, p1=buffer1; n>=1; n--, p+=16, p1+=16)
					AES_ecb_encrypt(p, p1, &aesKey, AES_ENCRYPT);
			}
			rtlglue_getmstime(&l_time);
		} else {
			rtlglue_getmstime(&p_time);
			for(i=0; i<c; i++) {
				aes_encrypt_key(key, keylen, &aes_ctx);
				for(n=blkSize>>4,p=plainText, p1=buffer1; n>=1; n--, p+=16, p1+=16)
					aes_encrypt(p, p1, &aes_ctx);
			}
			rtlglue_getmstime(&l_time);
		}
		dif = l_time - p_time;
		enRate = (c*blkSize*8)/(dif*10);
		
		/* (6) Estimate the time for AES decryption */
		if (sel == SEL_AES_0) {
			rtlglue_getmstime(&p_time);
			for(i=0; i<c; i++) {
				AES_set_decrypt_key(key, keylen, &aesKey);
				for(n=blkSize>>4,p=buffer1, p1=buffer2; n>=1; n--, p+=16, p1+=16) 
					AES_ecb_encrypt(p, p1, &aesKey, AES_DECRYPT);
			}
			rtlglue_getmstime(&l_time);
		} else {
			rtlglue_getmstime(&p_time);
			for(i=0; i<c; i++) {
				aes_decrypt_key(key, keylen, &aes_ctx);
				for(n=blkSize>>4,p=buffer1, p1=buffer2; n>=1; n--, p+=16, p1+=16) 
					aes_decrypt(p, p1, &aes_ctx);
			}
			rtlglue_getmstime(&l_time);
		}
		dif = l_time - p_time;
		deRate = (c*blkSize*8)/(dif*10);	
#endif		
		rtlglue_printf("Encrypt Only: %8u Kbit(s)/sec, %4u(ms)\n", encryptRate, en_dif*10);
		rtlglue_printf("Decrypt Only: %8u Kbit(s)/sec, %4u(ms)\n", decryptRate, de_dif*10);
		rtlglue_printf("Encryption Key Expansion : Decryption Key Expansion <=> %u : %u\n\n", encyptKeyExpand , decyptKeyExpand);

	} else if (!strcmp(mode, "cbc")) {
		/* CBC Mode encryption */
		if (sel == SEL_AES_0) {
			rtlglue_getmstime(&p_time);
			for(i=0; i<c; i++) {
				AES_set_encrypt_key(key, keylen, &aesKey);
				soft_AES_cbc_encrypt(plainText, buffer1, blkSize, &aesKey, ivec, AES_ENCRYPT);
			}
			rtlglue_getmstime(&l_time);
		} else {
#ifdef SUPPORT_AES_1
			rtlglue_getmstime(&p_time);
			for(i=0; i<c; i++) {
				aes_encrypt_key(key, keylen, &aes_ctx);
				aes_cbc_encrypt(plainText, buffer1, blkSize, &aes_ctx, ivec, AES_ENCRYPT);
			}
			rtlglue_getmstime(&l_time);
#endif
		}
		dif = l_time - p_time;
		enRate = (c*blkSize*8)/(dif*10);
		
		for(i=0; i<16; i++)
			ivec[i] = (((i*100 + 4)<<2) & 0xff);
		/* CBC Mode decryption */
		if (sel == SEL_AES_0) {
			rtlglue_getmstime(&p_time);
			for(i=0; i<c; i++) {
				AES_set_decrypt_key(key, keylen, &aesKey);
				soft_AES_cbc_encrypt(buffer1, buffer2, blkSize, &aesKey, ivec, AES_DECRYPT);
			}		
			rtlglue_getmstime(&l_time);
		} else {
#ifdef SUPPORT_AES_1
			rtlglue_getmstime(&p_time);
			for(i=0; i<c; i++) {
				aes_decrypt_key(key, keylen, &aes_ctx);
				aes_cbc_encrypt(plainText, buffer1, blkSize, &aes_ctx, ivec, AES_DECRYPT);
			}
			rtlglue_getmstime(&l_time);
#endif
		}
		dif = l_time - p_time;
		deRate = (c*blkSize*8)/(dif* 10);
		
		/* gen correct results */
		for(i=0; i<16; i++)
			ivec[i] = (((i*100 + 4)<<2) & 0xff);
		if (sel == SEL_AES_0) {
			AES_set_encrypt_key(key, keylen, &aesKey);
			soft_AES_cbc_encrypt(plainText, buffer1, blkSize, &aesKey, ivec, AES_ENCRYPT);
		} else {
#ifdef SUPPORT_AES_1
			aes_encrypt_key(key, keylen, &aes_ctx);
			aes_cbc_encrypt(plainText, buffer1, blkSize, &aes_ctx, ivec, AES_ENCRYPT);
#endif
		}
		for(i=0; i<16; i++)
			ivec[i] = (((i*100 + 4)<<2) & 0xff);
		if (sel == SEL_AES_0) {
			AES_set_decrypt_key(key, keylen, &aesKey);
			soft_AES_cbc_encrypt(buffer1, buffer2, blkSize, &aesKey, ivec, AES_DECRYPT);
		} else {
#ifdef SUPPORT_AES_1
			aes_decrypt_key(key, keylen, &aes_ctx);
			aes_cbc_encrypt(buffer1, buffer2, blkSize, &aes_ctx, ivec, AES_DECRYPT);
#endif
		}
		
	}
	rtlglue_free(buffer1); rtlglue_free(buffer2); rtlglue_free(plainText);
	rtlglue_printf("Encryption: %8u Kbit(s)/sec\n", enRate);
	rtlglue_printf("Decryption: %8u Kbit(s)/sec\n", deRate);
	for(i=0; i<blkSize; i++) {
		if (buffer2[i] != plainText[i]) {
			rtlglue_printf("failure......\n");
			return;
		}
	}
}



static int32 _crypto_aesThoughput( void ) {
	int32 bksize, keylen;

	for(keylen=192; keylen<=256; keylen+=64) {
		for(bksize=16; bksize<1600; bksize+=48) {
			rtlglue_printf("%d-bit key length, %d-byte block size:\n", keylen, bksize);
			AES_timeTrial(1, "cbc", keylen, bksize, 20000);
			rtlglue_printf("\n");
		}
	}

	return SUCCESS;
}


static int32 _crypto_aesTestCmd(uint32 userId, int32 argc, int8 **saved) {
	int8 *nextToken;
	int32 size, i, errCount = 0;
	static AES_KEY aesKey; /* for AES-0 */
	static AES_test_pattern_t AES_pattern[] = 
{
	/* Testing Patterns from "A Specification for The AES Algorithm" */
	{ 	type: ECB_AES,
		keyLen: 128/8, 
		key: "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c",
		ivLen: 0,
		iv: NULL,
		dataLen: 16,
		plain: "\x32\x43\xf6\xa8\x88\x5a\x30\x8d\x31\x31\x98\xa2\xe0\x37\x07\x34",
		cipher: "\x39\x25\x84\x1d\x02\xdc\x09\xfb\xdc\x11\x85\x97\x19\x6a\x0b\x32",
	},
	{ 	type: ECB_AES,
		keyLen: 192/8, 
		key: "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c\x76\x2e\x71\x60\xf3\x8b\x4d\xa5",
		ivLen: 0,
		iv: NULL,
		dataLen: 16,
		plain: "\x32\x43\xf6\xa8\x88\x5a\x30\x8d\x31\x31\x98\xa2\xe0\x37\x07\x34",
		cipher: "\xf9\xfb\x29\xae\xfc\x38\x4a\x25\x03\x40\xd8\x33\xb8\x7e\xbc\x00",
	},
	{ 	type: ECB_AES,
		keyLen: 256/8, 
		key: "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c\x76\x2e\x71\x60\xf3\x8b\x4d\xa5\x6a\x78\x4d\x90\x45\x19\x0c\xfe",
		ivLen: 0,
		iv: NULL,
		dataLen: 16,
		plain: "\x32\x43\xf6\xa8\x88\x5a\x30\x8d\x31\x31\x98\xa2\xe0\x37\x07\x34",
		cipher: "\x1a\x6e\x6c\x2c\x66\x2e\x7d\xa6\x50\x1f\xfb\x62\xbc\x9e\x93\xf3",
	},

	/* Testing Patterns from chhuang */
	{	type: ECB_AES,
		keyLen: 128/8,
		key: "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
		ivLen: 0,
		iv: NULL,
		dataLen: 16,
		plain: "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff",
		cipher: "\x69\xc4\xe0\xd8\x6a\x7b\x04\x30\xd8\xcd\xb7\x80\x70\xb4\xc5\x5a",
	},
	{	type: ECB_AES,
		keyLen: 192/8,
		key: "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17",
		ivLen: 0,
		iv: NULL,
		dataLen: 16,
		plain: "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff",
		cipher: "\xdd\xa9\x7c\xa4\x86\x4c\xdf\xe0\x6e\xaf\x70\xa0\xec\x0d\x71\x91",
	},
	{	type: ECB_AES,
		keyLen: 256/8,
		key: "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f",
		ivLen: 0,
		iv: NULL,
		dataLen: 16,
		plain: "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff",
		cipher: "\x8e\xa2\xb7\xca\x51\x67\x45\xbf\xea\xfc\x49\x90\x4b\x49\x60\x89",
	},
	
	/* Testing Patterns from RFC 3602 */
	{	type: CBC_AES,
		keyLen: 128/8,
		key: "\x06\xa9\x21\x40\x36\xb8\xa1\x5b\x51\x2e\x03\xd5\x34\x12\x00\x06",
		ivLen: 16,
		iv: "\x3d\xaf\xba\x42\x9d\x9e\xb4\x30\xb4\x22\xda\x80\x2c\x9f\xac\x41",
		dataLen: 16,
		plain: "Single block msg",
		cipher: "\xe3\x53\x77\x9c\x10\x79\xae\xb8\x27\x08\x94\x2d\xbe\x77\x18\x1a",
	},
	{	type: CBC_AES,
		keyLen: 128/8,
		key: "\xc2\x86\x69\x6d\x88\x7c\x9a\xa0\x61\x1b\xbb\x3e\x20\x25\xa4\x5a",
		ivLen: 16,
		iv: "\x56\x2e\x17\x99\x6d\x09\x3d\x28\xdd\xb3\xba\x69\x5a\x2e\x6f\x58",
		dataLen: 32,
		plain: "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f",
		cipher: "\xd2\x96\xcd\x94\xc2\xcc\xcf\x8a\x3a\x86\x30\x28\xb5\xe1\xdc\x0a\x75\x86\x60\x2d\x25\x3c\xff\xf9\x1b\x82\x66\xbe\xa6\xd6\x1a\xb1",
	},
	{	type: CBC_AES,
		keyLen: 128/8,
		key: "\x6c\x3e\xa0\x47\x76\x30\xce\x21\xa2\xce\x33\x4a\xa7\x46\xc2\xcd",
		ivLen: 16,
		iv: "\xc7\x82\xdc\x4c\x09\x8c\x66\xcb\xd9\xcd\x27\xd8\x25\x68\x2c\x81",
		dataLen: 48,
		plain: "This is a 48-byte message (exactly 3 AES blocks)",
		cipher: "\xd0\xa0\x2b\x38\x36\x45\x17\x53\xd4\x93\x66\x5d\x33\xf0\xe8\x86\x2d\xea\x54\xcd\xb2\x93\xab\xc7\x50\x69\x39\x27\x67\x72\xf8\xd5\x02\x1c\x19\x21\x6b\xad\x52\x5c\x85\x79\x69\x5d\x83\xba\x26\x84",
	},
	{	type: CBC_AES,
		keyLen: 128/8,
		key: "\x56\xe4\x7a\x38\xc5\x59\x89\x74\xbc\x46\x90\x3d\xba\x29\x03\x49",
		ivLen: 16,
		iv: "\x8c\xe8\x2e\xef\xbe\xa0\xda\x3c\x44\x69\x9e\xd7\xdb\x51\xb7\xd9",
		dataLen: 64,
		plain: "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf",
		cipher: "\xc3\x0e\x32\xff\xed\xc0\x77\x4e\x6a\xff\x6a\xf0\x86\x9f\x71\xaa\x0f\x3a\xf0\x7a\x9a\x31\xa9\xc6\x84\xdb\x20\x7e\xb0\xef\x8e\x4e\x35\x90\x7a\xa6\x32\xc3\xff\xdf\x86\x8b\xb7\xb2\x9d\x3d\x46\xad\x83\xce\x9f\x9a\x10\x2e\xe9\x9d\x49\xa5\x3e\x87\xf4\xc3\xda\x55",
	},

	/* Testing Patterns from RFC 3686 */
	{	type: CTR_AES,
		keyLen: 128/8,
		key: "\xAE\x68\x52\xF8\x12\x10\x67\xCC\x4B\xF7\xA5\x76\x55\x77\xF3\x9E",
		ivLen: 16,
		iv: "\x00\x00\x00\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
		dataLen: 16,
		plain: "\x53\x69\x6E\x67\x6C\x65\x20\x62\x6C\x6F\x63\x6B\x20\x6D\x73\x67",
		cipher: "\xE4\x09\x5D\x4F\xB7\xA7\xB3\x79\x2D\x61\x75\xA3\x26\x13\x11\xB8",
	},
	{	type: CTR_AES,
		keyLen: 128/8,
		key: "\x7E\x24\x06\x78\x17\xFA\xE0\xD7\x43\xD6\xCE\x1F\x32\x53\x91\x63",
		ivLen: 16,
		iv: "\x00\x6C\xB6\xDB\xC0\x54\x3B\x59\xDA\x48\xD9\x0B\x00\x00\x00\x01",
		dataLen: 32,
		plain: "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F",
		cipher: "\x51\x04\xA1\x06\x16\x8A\x72\xD9\x79\x0D\x41\xEE\x8E\xDA\xD3\x88\xEB\x2E\x1E\xFC\x46\xDA\x57\xC8\xFC\xE6\x30\xDF\x91\x41\xBE\x28",
	},
	{	type: CTR_AES,
		keyLen: 128/8,
		key: "\x76\x91\xBE\x03\x5E\x50\x20\xA8\xAC\x6E\x61\x85\x29\xF9\xA0\xDC",
		ivLen: 16,
		iv: "\x00\xE0\x01\x7B\x27\x77\x7F\x3F\x4A\x17\x86\xF0\x00\x00\x00\x01",
		dataLen: 36,
		plain: "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20\x21\x22\x23",
		cipher: "\xC1\xCF\x48\xA8\x9F\x2F\xFD\xD9\xCF\x46\x52\xE9\xEF\xDB\x72\xD7\x45\x40\xA4\x2B\xDE\x6D\x78\x36\xD5\x9A\x5C\xEA\xAE\xF3\x10\x53\x25\xB2\x07\x2F",
	},
	{	type: CTR_AES,
		keyLen: 192/8,
		key: "\x16\xAF\x5B\x14\x5F\xC9\xF5\x79\xC1\x75\xF9\x3E\x3B\xFB\x0E\xED\x86\x3D\x06\xCC\xFD\xB7\x85\x15",
		ivLen: 16,
		iv: "\x00\x00\x00\x48\x36\x73\x3C\x14\x7D\x6D\x93\xCB\x00\x00\x00\x01",
		dataLen: 16,
		plain: "Single block msg",
		cipher: "\x4B\x55\x38\x4F\xE2\x59\xC9\xC8\x4E\x79\x35\xA0\x03\xCB\xE9\x28",
	},
	{	type: CTR_AES,
		keyLen: 192/8,
		key: "\x7C\x5C\xB2\x40\x1B\x3D\xC3\x3C\x19\xE7\x34\x08\x19\xE0\xF6\x9C\x67\x8C\x3D\xB8\xE6\xF6\xA9\x1A",
		ivLen: 16,
		iv: "\x00\x96\xB0\x3B\x02\x0C\x6E\xAD\xC2\xCB\x50\x0D\x00\x00\x00\x01",
		dataLen: 32,
		plain: "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F",
		cipher: "\x45\x32\x43\xFC\x60\x9B\x23\x32\x7E\xDF\xAA\xFA\x71\x31\xCD\x9F\x84\x90\x70\x1C\x5A\xD4\xA7\x9C\xFC\x1F\xE0\xFF\x42\xF4\xFB\x00",
	},
	{	type: CTR_AES,
		keyLen: 192/8,
		key: "\x02\xBF\x39\x1E\xE8\xEC\xB1\x59\xB9\x59\x61\x7B\x09\x65\x27\x9B\xF5\x9B\x60\xA7\x86\xD3\xE0\xFE",
		ivLen: 16,
		iv: "\x00\x07\xBD\xFD\x5C\xBD\x60\x27\x8D\xCC\x09\x12\x00\x00\x00\x01",
		dataLen: 36,
		plain: "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20\x21\x22\x23",
		cipher: "\x96\x89\x3F\xC5\x5E\x5C\x72\x2F\x54\x0B\x7D\xD1\xDD\xF7\xE7\x58\xD2\x88\xBC\x95\xC6\x91\x65\x88\x45\x36\xC8\x11\x66\x2F\x21\x88\xAB\xEE\x09\x35",
	},
	{	type: CTR_AES,
		keyLen: 256/8,
		key: "\x77\x6B\xEF\xF2\x85\x1D\xB0\x6F\x4C\x8A\x05\x42\xC8\x69\x6F\x6C\x6A\x81\xAF\x1E\xEC\x96\xB4\xD3\x7F\xC1\xD6\x89\xE6\xC1\xC1\x04",
		ivLen: 16,
		iv: "\x00\x00\x00\x60\xDB\x56\x72\xC9\x7A\xA8\xF0\xB2\x00\x00\x00\x01",
		dataLen: 16,
		plain: "Single block msg",
		cipher: "\x14\x5A\xD0\x1D\xBF\x82\x4E\xC7\x56\x08\x63\xDC\x71\xE3\xE0\xC0",
	},
	{	type: CTR_AES,
		keyLen: 256/8,
		key: "\xF6\xD6\x6D\x6B\xD5\x2D\x59\xBB\x07\x96\x36\x58\x79\xEF\xF8\x86\xC6\x6D\xD5\x1A\x5B\x6A\x99\x74\x4B\x50\x59\x0C\x87\xA2\x38\x84",
		ivLen: 16,
		iv: "\x00\xFA\xAC\x24\xC1\x58\x5E\xF1\x5A\x43\xD8\x75\x00\x00\x00\x01",
		dataLen: 32,
		plain: "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F",
		cipher: "\xF0\x5E\x23\x1B\x38\x94\x61\x2C\x49\xEE\x00\x0B\x80\x4E\xB2\xA9\xB8\x30\x6B\x50\x8F\x83\x9D\x6A\x55\x30\x83\x1D\x93\x44\xAF\x1C",
	},
	{	type: CTR_AES,
		keyLen: 256/8,
		key: "\xFF\x7A\x61\x7C\xE6\x91\x48\xE4\xF1\x72\x6E\x2F\x43\x58\x1D\xE2\xAA\x62\xD9\xF8\x05\x53\x2E\xDF\xF1\xEE\xD6\x87\xFB\x54\x15\x3D",
		ivLen: 16,
		iv: "\x00\x1C\xC5\xB7\x51\xA5\x1D\x70\xA1\xC1\x11\x48\x00\x00\x00\x01",
		dataLen: 36,
		plain: "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20\x21\x22\x23",
		cipher: "\xEB\x6C\x52\x82\x1D\x0B\xBB\xF7\xCE\x75\x94\x46\x2A\xCA\x4F\xAA\xB4\x07\xDF\x86\x65\x69\xFD\x07\xF4\x8C\xC0\xB5\x83\xD6\x07\x1F\x1E\xC0\xE6\xB8",
	},
	
#if 0 /* Template */
	{	type: -1,
		keyLen: -1,
		key: NULL,
		ivLen: -1,
		iv: NULL,
		dataLen: -1,
		plain: NULL,
		cipher: NULL,
	},
#endif
};

#define NUM_PATTERN		(sizeof(AES_pattern)/sizeof(struct AES_test_pattern_s))
	
	cle_getNextCmdToken(&nextToken,&size,saved); 
	if (!strcmp("aes-0", nextToken))
		sel = SEL_AES_0;
	else if (!strcmp("aes-1", nextToken))
		sel = SEL_AES_1;
	else if (!strcmp("init", nextToken))
		sel = SEL_INIT;
	else if (!strcmp("rand", nextToken))
		sel = SEL_RAND;
	else if (!strcmp("asic", nextToken))
		sel = SEL_ASIC;
	else return FAILED;

	if ( sel==SEL_AES_0 || sel==SEL_AES_1 )
	{
		cle_getNextCmdToken(&nextToken,&size,saved); 
		if (!strcmp("verify", nextToken)) {

			buffer1 = rtlglue_malloc(BUFFER_LEN);
			buffer2 = rtlglue_malloc(BUFFER_LEN);
			plainText = rtlglue_malloc(BUFFER_LEN);
			assert(buffer1 && buffer2 && plainText);
			buffer1 = UNCACHED_ADDRESS( buffer1 );
			buffer2 = UNCACHED_ADDRESS( buffer2 );
			plainText = UNCACHED_ADDRESS( plainText );
		
			for(i=0; i<NUM_PATTERN; i++) 
			{
				rtlglue_printf("\r[%03d,%03d] %s:", i, errCount,
				                               AES_pattern[i].type==ECB_AES?"ECB_AES":AES_pattern[i].type==CBC_AES?"CBC_AES":"CTR_AES" );
				if (sel==SEL_AES_0)
				{
					/* Encode */
					aesSim_aes( AES_pattern[i].type|4/*enc*/, AES_pattern[i].plain, buffer1, AES_pattern[i].dataLen, 
					            AES_pattern[i].keyLen, AES_pattern[i].key, AES_pattern[i].iv );

					/* Decode */
					aesSim_aes( AES_pattern[i].type, AES_pattern[i].cipher, buffer2, AES_pattern[i].dataLen, 
					            AES_pattern[i].keyLen, AES_pattern[i].key, AES_pattern[i].iv );
					
					if ( memcmp( AES_pattern[i].cipher, buffer1, AES_pattern[i].dataLen ) != 0 ||
					     memcmp( AES_pattern[i].plain, buffer2, AES_pattern[i].dataLen ) != 0 )
					{
						AES_printit( &AES_pattern[i], buffer1, buffer2 );
						errCount++;
					}
				}
				else if (sel == SEL_AES_1)
				{
#ifdef SUPPORT_AES_1
					if (AES_pattern[i].type==ECB_AES) 
					{
						gen_tabs();
						aes_encrypt_key(AES_pattern[i].key, AES_pattern[i].keyLen*8, &aes_ctx);
						aes_encrypt(AES_pattern[i].plain, buffer1, &aes_ctx);
						aes_decrypt_key(AES_pattern[i].key, AES_pattern[i].keyLen*8, &aes_ctx);
						aes_decrypt(buffer1, buffer2, &aes_ctx);
					}
					else if (AES_pattern[i].type==CBC_AES)
					{
						return FAILED;
					}
					else if (AES_pattern[i].type==CTR_AES)
					{
						return FAILED;
					}
#else
					return FAILED;
#endif
				}
			}		

			rtlglue_free(CACHED_ADDRESS(buffer1)); 
			rtlglue_free(CACHED_ADDRESS(buffer2)); 
			rtlglue_free(CACHED_ADDRESS(plainText));

		} else if (!strcmp("time-trial", nextToken)) {
			int32 blkSize, repeat, keylen;
			int8 mode[4] = { 0x00, 0x00, 0x00, 0x00 };

			cle_getNextCmdToken(&nextToken,&size,saved); 
			mode[0] = nextToken[0]; mode[1] = nextToken[1]; mode[2] = nextToken[2];
			cle_getNextCmdToken(&nextToken,&size,saved); 
			keylen = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			blkSize = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			repeat = U32_value(nextToken);	
			AES_timeTrial(sel, mode, keylen, blkSize, repeat);
		} else if (!strcmp("throughput", nextToken)) {
			_crypto_aesThoughput();
		}
	}
	else if ( sel==SEL_INIT )
	{
		/* 'cr aes init' */
		uint32 descNum;
		
		cle_getNextCmdToken(&nextToken,&size,saved); 
		descNum = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		BurstSize = U32_value(nextToken);	
		if( BurstSize == 64 ) 
		{	
			rtlglue_printf("Initialize crypto engine driver with %u descriptors working on 64-byte mode\n", descNum);
			return rtl8651b_cryptoEngine_init(descNum, 2);
		}
		else if( BurstSize == 32 ) 
		{	
			rtlglue_printf("Initialize crypto engine driver with %u descriptors working on 32-byte mode\n", descNum);
			return rtl8651b_cryptoEngine_init(descNum, 1);
		}
		else if( BurstSize == 16 )
		{	
			rtlglue_printf("Initialize crypto engine driver with %u descriptors working on 16-byte mode\n", descNum);
			return rtl8651b_cryptoEngine_init(descNum, 0);
		}
		else
			return FAILED;
		return SUCCESS;
		
	}
	else if ( sel==SEL_RAND )
	{
		/* 'crypto aes rand' */
		uint32 randSeed, roundLimit;
		
		cle_getNextCmdToken(&nextToken,&size,saved); 
		randSeed = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		roundLimit = U32_value(nextToken);	
		rtlglue_printf("Run AES random test seed %u and %u rounds\n", randSeed, roundLimit);
		return runAes8651bGeneralApiRandTest(randSeed, roundLimit);
	}
	else if ( sel==SEL_ASIC )
	{
		/* 'crypto aes asic' */
		uint32 round, funStart, funEnd, lenStart, lenEnd, offsetStart, offsetEnd;
		
		rtlglue_printf("Run AES simulator and 8651b crypto engine test\n");
		cle_getNextCmdToken(&nextToken,&size,saved); 
		if ( !strcmp( nextToken, "verify" ) )
		{
			/* Test pattern is ECB_AES */
			buffer1 = (uint8 *)rtlglue_malloc(BUFFER_LEN);
			buffer2 = (uint8 *)rtlglue_malloc(BUFFER_LEN);
			plainText = (uint8 *)rtlglue_malloc(BUFFER_LEN);
			assert(buffer1 && buffer2 && plainText);
			buffer1 = UNCACHED_ADDRESS( buffer1 );
			buffer2 = UNCACHED_ADDRESS( buffer2 );
			plainText = UNCACHED_ADDRESS( plainText );
			
			for(i=0; i<NUM_PATTERN; i++) 
			{
				/* Currently, we only support 128-bit encryption. */
				if ( AES_pattern[i].keyLen != 128/8 ) continue;
			
				rtlglue_printf("\r[%03d,%03d] %s:", i, errCount,
				                               AES_pattern[i].type==ECB_AES?"ECB_AES":AES_pattern[i].type==CBC_AES?"CBC_AES":"CTR_AES" );

				/* Encrypt */
				memcpy( buffer1, AES_pattern[i].plain, AES_pattern[i].dataLen );
				if ( rtl8651b_cryptoEngine_des( AES_pattern[i].type|4/*enc*/, buffer1, (AES_pattern[i].dataLen+15)&~15, AES_pattern[i].key, AES_pattern[i].iv ) == FAILED )
				{
					rtlglue_printf( "ASIC Encryption FAILED\n" );
					return FAILED;
				}

				/* Decrypt */
				if ( AES_pattern[i].type==CTR_AES )
				{
					memcpy( buffer2, AES_pattern[i].cipher, AES_pattern[i].dataLen );
					if ( rtl8651b_cryptoEngine_des( AES_pattern[i].type|4/*enc*/, buffer2, (AES_pattern[i].dataLen+15)&~15, AES_pattern[i].key, AES_pattern[i].iv ) == FAILED )
					{
						rtlglue_printf( "ASIC Encryption FAILED\n" );
						return FAILED;
					}
				}
				else
				{
					memcpy( buffer2, AES_pattern[i].cipher, AES_pattern[i].dataLen );
					/* Software must prepare decrypt key of first round. And stored in 'plaintext'. */
					AES_set_decrypt_key(AES_pattern[i].key, AES_pattern[i].keyLen*8, &aesKey);
					memcpy( plainText, &aesKey.rd_key[0], 128/8 );
					if ( rtl8651b_cryptoEngine_des( AES_pattern[i].type/*dec*/, buffer2, (AES_pattern[i].dataLen+15)&~15, plainText, AES_pattern[i].iv ) == FAILED )
					{
						rtlglue_printf( "ASIC Decryption FAILED\n" );
						return FAILED;
					}
				}

				if ( memcmp( AES_pattern[i].cipher, buffer1, AES_pattern[i].dataLen ) != 0 ||
				     memcmp( AES_pattern[i].plain, buffer2, AES_pattern[i].dataLen ) != 0 )
				{
					AES_printit( &AES_pattern[i], buffer1, buffer2 );
					errCount++;
				}
			}

			rtlglue_free(CACHED_ADDRESS(buffer1)); 
			rtlglue_free(CACHED_ADDRESS(buffer2)); 
			rtlglue_free(CACHED_ADDRESS(plainText));
			
		}
		else
		{
			int32 retval;
			
			round = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			funStart = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			funEnd = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			lenStart = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			lenEnd = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			offsetStart = U32_value(nextToken);	
			cle_getNextCmdToken(&nextToken,&size,saved); 
			offsetEnd = U32_value(nextToken);	
			rtlglue_printf("(X)Round %u Function %u - %u Len %u - %u Offset %u - %u\n", round, funStart, funEnd, lenStart, lenEnd, offsetStart, offsetEnd);
			retval = runAes8651bGeneralApiTest(round, funStart, funEnd, lenStart, lenEnd, offsetStart, offsetEnd);
#ifdef	CONFIG_RTL865X_MODEL_TEST_FT2
			if ( retval == SUCCESS ) {
				rtlglue_printf("Crypto runAes8651bGeneralApiTest Pass! {P901}\n");
			} else {
				rtlglue_printf("Crypto runAes8651bGeneralApiTest Fail! {F901}\n");
			}
#endif
			return retval;
		}
	}
	
	if ( errCount != 0 ) 
		return FAILED;
	else
		return SUCCESS;
}

#endif

/**************************************************************
 * Mix
 **************************************************************/
#ifdef RTL865X_MODEL_KERNEL
/*  Debug Level for cbMix()
 *  0 -- NO DEBUG INFO
 *  1 -- not defined
 *  2 -- print only when [sa,sl] accepted
 *  3 -- print no matter accepted or not
 *  4 -- print verbose
 *  5 -- print ALL
 */
#define _CBMIX_DEBUG_ 2
#define _DEBUG_MSG_INTERVAL_ 113 /* To speed up, we will skip some debug messages according this value (only when _CBMIX_DEBUG_==2). */
#if _CBMIX_DEBUG_==2
static uint32 debugIntervalCount = 0;
#endif

static int32 modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo; /* Unit: byte */

int32 cbMix( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int i;
	rtl865x_cryptoScatter_t src[8];
	uint32 cntScatter = 0;
	int32 lenValid = 0; /* data length (not include gaps) */
	int32 lenAll = 0; /* data length and gap length */
	int32 lenEnl;
	int32 retval;
	uint8 *cur;
	static uint8 bufAsic[MAX_PKTLEN];
	static uint8 bufSw[MAX_PKTLEN];
	static uint8 _cryptoKey[256];
	static uint8 _authKey[256];
	static uint8 _iv[32]; /* 256-bits */
	static uint8 _pad[128]; /* ipad + opad */
	static uint8 _AsicDigest[20]; /* max digest length */
	static uint8 _SwDigest[20]; /* max digest length */
#ifdef USE_CACHE
	uint8 *pCryptoKey = ( _cryptoKey );
	uint8 *pDecryptoKey;
	uint8 *pAuthKey = ( _authKey );
	uint8 *pIv = ( _iv );
	uint8 *pPad = ( _pad );
	uint8 *pAsicDigest = _AsicDigest;
	uint8 *pSwDigest = _SwDigest;
#else
	uint8 *pCryptoKey = UNCACHED_ADDRESS( _cryptoKey );
	uint8 *pDecryptoKey;
	uint8 *pAuthKey = UNCACHED_ADDRESS( _authKey );
	uint8 *pIv = UNCACHED_ADDRESS( _iv );
	uint8 *pPad = UNCACHED_ADDRESS( _pad );
	uint8 *pAsicDigest = _AsicDigest;
	uint8 *pSwDigest = _SwDigest;
#endif

	DEBUG_BAL();
	WRITE_MEM32( WDTCNR, READ_MEM32(WDTCNR)|WDTCLR );
	if ( enumbal_total_case_no < initial_case_no ) goto pass; /* ignore until initial case number. */

	/* create scatter list */
#ifdef USE_CACHE
	cur = ( &bufAsic[0] );
#else
	cur = UNCACHED_ADDRESS( &bufAsic[0] );
#endif
	for( i = 0; i < 8; i++, cntScatter++ )
	{
		if ( sl[i]==0 )
			break;

		#if _CBMIX_DEBUG_>=3
		rtlglue_printf( "[%d:%d]", sa[i], sl[i] );
		#endif

		cur += sa[i];
		src[i].ptr = cur;
		src[i].len = sl[i];
		lenValid += sl[i];
		lenAll += sl[i] + sa[i];
		cur += sl[i];

		#if _CBMIX_DEBUG_>=4
		rtlglue_printf( "{%p:%d} ", src[i].ptr, src[i].len );
		#endif
	}

	/*  <--lenValid-->
	 *  | A2EO | ENL | APL
	 */
	lenEnl = lenValid-lenA2eo;
	#if _CBMIX_DEBUG_>=3
	rtlglue_printf( "ENL=%d ", lenEnl );
	#endif

	if ( lenEnl <= 0 )
	{
		#if _CBMIX_DEBUG_>=3
		rtlglue_printf( "[IGNORE,lenEnl<=0]\n" );
		#endif
		goto ignore;
	}

	if ( modeCrypto!=(uint32)-1 )
	{
		if ( modeCrypto&0x20 )
		{ /* AES, ENL is the times of 16 bytes. */
			if ( lenEnl&0xf )
			{
				#if _CBMIX_DEBUG_>=3
				rtlglue_printf( "[IGNORE,ENL=0x%04x]\n", lenEnl );
				#endif
				goto ignore;
			}
		}
		else
		{ /* DES, ENL is the times of 8 bytes. */
			if ( lenEnl&0x7 )
			{
				#if _CBMIX_DEBUG_>=3
				rtlglue_printf( "[IGNORE,ENL=0x%04x]\n", lenEnl );
				#endif
				goto ignore;
			}
		}
	}

	if ( modeAuth!=(uint32)-1 )
	{
		if ( lenValid&3 )
		{
			/* Since A2EO and APL must be 4-byte times (ENL is 8/16-byte), lenValid must be 4-byte times.
			 * Otherwise, APL will be non-4-byte times. */
			#if _CBMIX_DEBUG_>=3
			rtlglue_printf( "[IGNORE,Valid=0x%04x]\n", lenValid );
			#endif
			goto ignore;
		}
	}

#if 0 /* to speed debug, you can assign the BUG case here. */
	if ( sl[0]!=8 || sl[1]!=8 || sl[2]!=8 || sl[3]!=8 || sl[4]!=16 || sl[5]!=128 ) goto ignore;
#endif

	/* Since IC only implement 8 descriptors FIFO, we should check if the caller provides too many descriptor. */
	{
		uint32 needToEnsure; /* the number of descriptor to summary */
		uint32 sumLen; /* the length of the first 'sumNum' descriptos */
		uint32 candidateAdd; /* 1 descriptor for auth */
		uint32 j; /* start index for check */
		
		if ( modeAuth==(uint32)-1 )
		{ /* Crypto only, we need to ensure if cntScatter is LE 8. */
			needToEnsure = 8;
			candidateAdd = 0;
		}
		else
		{ /* Crypto+Auth or AuthOnly, since APL will be added by driver, the threshold is 7 descriptors. */
			needToEnsure = 7;
			candidateAdd = 1;
		}

		/* If the count of scatter is larger than or equal to 'needToEnsure', we need to check the length.
		 *   Else, it is always OK. */
		if ( cntScatter >= needToEnsure )
		{
			/* try all possibility of combination. */
			for( j = 0; j <= (cntScatter-7); j++ )
			{
				sumLen = 0; /* calculate the continuous 7 descriptors */
				for( i = 0; i < 7; i++ )
				{
					sumLen += sl[j+i];
				}
				
				if ( sumLen<BurstSize /* It is OK for equal case. */ &&
				     (cntScatter+candidateAdd)>(j+7) /* more descriptor behind */ )
				{
					/* IC limitation, no need to verify */
					#if _CBMIX_DEBUG_>=3
					rtlglue_printf( "[IGNORE,icLimit]\n" );
					#endif
					goto ignore;
				}
			}
		}
	}

	#if _CBMIX_DEBUG_>=3
	rtlglue_printf( "\n" );
	#endif
	
	#if _CBMIX_DEBUG_==2
	if ( (debugIntervalCount%_DEBUG_MSG_INTERVAL_)==0 )
	{
		rtlglue_printf( "<%lu>", enumbal_total_case_no );
		for( i = 0; i < cntScatter; i++ )
			rtlglue_printf( "[%d:%d]", sa[i], sl[i] );
	}
	#endif

#if 1 /* More complex pattern */
	/* fill the test data to software buffer */
	cur = &bufSw[0];
	for( i = 0; i < lenAll; i++ )
	{
		*cur = (i+0x23)^(i*4-0x79);
		cur++;
	}

	/* build keys and iv */
	for( i = 0; i < lenCryptoKey; i++ )
		pCryptoKey[i] = (i+0x27)^(i*6-0xcf);
	for( i = 0; i < lenAuthKey; i++ )
		pAuthKey[i] = (i+0xcf)^(i*6-0x27);
	for( i = 0; i < 16; i++ )
		pIv[i] = (i+0xcf)^(i*12-0x27);
	for( i = 0; i < 128; i++ )
		pPad[i] = ((i+0xcf)^(i*2-0x26));
#else
	/* Simple pattern for debug */

	/* fill the test data to software buffer */
	cur = &bufSw[0];
	for( i = 0; i < lenAll; i++ )
	{
		*cur = i;
		cur++;
	}

	/* build keys and iv */
	for( i = 0; i < lenCryptoKey; i++ )
		pCryptoKey[i] = i;
	for( i = 0; i < lenAuthKey; i++ )
		pAuthKey[i] = i;
	for( i = 0; i < 16; i++ )
		pIv[i] = i;
	for( i = 0; i < 128; i++ )
		pPad[i] = i;
#endif

	/* copy scatter buffer from software buffer (continuous) to asic buffer */
	cur = &bufSw[0];
	for( i = 0; i < cntScatter; i++ )
	{
		memcpy( src[i].ptr, cur, src[i].len );
		cur += src[i].len;
	}

	#if _CBMIX_DEBUG_>=5
	memDump( bufSw, lenEnl, "Software bufSw" );
	for( i = 0; i < cntScatter; i++ )
	{
		memDump( src[i].ptr, src[i].len, "ASIC srcScatter" );
	}
	#endif

	/*---------------------------------------------------------------------
	 *                                 ASIC ENC/Auth
	 *---------------------------------------------------------------------*/
	/* encrypt */
#ifdef USE_CACHE
	/* flush cache, before run IPSEC Engine */
	rtlglue_flushDCache((uint32)bufAsic, sizeof(bufAsic));
	rtlglue_flushDCache((uint32)_AsicDigest, sizeof(_AsicDigest));
	rtlglue_flushDCache((uint32)_cryptoKey, sizeof(_cryptoKey));
	rtlglue_flushDCache((uint32)_authKey, sizeof(_authKey));
	rtlglue_flushDCache((uint32)_iv, sizeof(_iv));
	rtlglue_flushDCache((uint32)_pad, sizeof(_pad));
#endif
	retval = rtl865x_ipsecEngine( modeCrypto|4/*enc*/, modeAuth, cntScatter, src, 
	                              lenCryptoKey, pCryptoKey, lenAuthKey, pAuthKey, pIv, pPad, pAsicDigest, 
	                              lenA2eo, lenEnl );
	if ( retval != SUCCESS )
	{
		rtlglue_printf( "%s():%d rtl865x_cryptoEngine() returns %d, crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n", __FUNCTION__, __LINE__, retval, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo );
		rtlglue_printf( "<%lu>", enumbal_total_case_no );
		for( i = 0; i < cntScatter; i++ )
			rtlglue_printf( "[%d:%d]", sa[i], sl[i] );
		goto out;
	}
	
	/*---------------------------------------------------------------------
	 *                               Software ENC/Auth
	 *---------------------------------------------------------------------*/
	if ( modeCrypto!=(uint32)-1 )
	{
		if ( modeCrypto&0x20 )
		{ /* AES */
			aesSim_aes( modeCrypto|4/*enc*/, bufSw+lenA2eo, bufSw+lenA2eo, lenEnl, lenCryptoKey, pCryptoKey, pIv );
		}
		else
		{ /* DES */
			desSim_des( modeCrypto|4/*enc*/, bufSw+lenA2eo, bufSw+lenA2eo, lenEnl, pCryptoKey, pIv );
		}

		/* compare with Crypto Result */
		cur = &bufSw[0];
		retval = SUCCESS;
		for( i = 0; i < cntScatter; i++ )
		{
			if ( memcmp( cur, src[i].ptr, src[i].len )!= 0 )
			{
				int j;

				if ( retval==SUCCESS ) /* print once */
				{
					rtlglue_printf( "ENC DIFF![%d]crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n", i, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo );
					rtlglue_printf( "<%lu>", enumbal_total_case_no );
					for( j = 0; j < cntScatter; j++ )
						rtlglue_printf( "[%d:%d]", sa[j], sl[j] );
				}
				memDump( cur, src[i].len, "Software" );
				memDump( src[i].ptr, src[i].len, "ASIC" );
				retval = FAILED;
			}
			cur += src[i].len;
		}
		if ( retval!=SUCCESS ) goto failed; 
	}

	/* compare with Auth Result */
	if ( modeAuth!=(uint32)-1 )
	{
		authSim( modeAuth, bufSw, lenValid, pAuthKey, lenAuthKey, pSwDigest );

		if ( memcmp( pSwDigest, pAsicDigest, modeAuth&1?20/*SHA1*/:16/*MD5*/ )!=0 )
		{
			rtlglue_printf( "ENC DIGEST! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n", modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo );
			rtlglue_printf( "<%lu>", enumbal_total_case_no );
			for( i = 0; i < cntScatter; i++ )
				rtlglue_printf( "[%d:%d]", sa[i], sl[i] );
			memDump( pSwDigest, modeAuth&1?20/*SHA1*/:16/*MD5*/, "Software" );
			memDump( pAsicDigest, modeAuth&1?20/*SHA1*/:16/*MD5*/, "ASIC" );
			goto failed;
		}
	}

	/* Since no crypto employed, no need to hash again. */
	if ( modeCrypto==(uint32)-1 )
		goto success;

	/* AES_CTR mode need not verify. */
	if ( modeCrypto==0x23/*AES_CTR*/ )
		goto success;

	/*---------------------------------------------------------------------
	 *                                 ASIC DEC
	 *---------------------------------------------------------------------*/
	/* decrypt */
	if ( modeCrypto==0x20/*AES_CBC*/ || modeCrypto==0x22/*AES_ECB*/ )
	{
		AES_KEY aes_key;
		static uint8 key_sch[256/8];

#if 1
		/* IC accept the key in reverse order. */
		AES_set_encrypt_key( pCryptoKey, lenCryptoKey*8, &aes_key );
		switch( lenCryptoKey )
		{
			case 128/8:
#ifdef USE_CACHE
				memcpy( (key_sch), &aes_key.rd_key[4*10], 16 );
#else
				memcpy( UNCACHED_ADDRESS(key_sch), &aes_key.rd_key[4*10], 16 );
#endif
				break;
			case 192/8:
#ifdef USE_CACHE
				memcpy( (key_sch), &aes_key.rd_key[4*12], 16 );
				memcpy( (&key_sch[16]), &aes_key.rd_key[4*11+2], 8 );
#else
				memcpy( UNCACHED_ADDRESS(key_sch), &aes_key.rd_key[4*12], 16 );
				memcpy( UNCACHED_ADDRESS(&key_sch[16]), &aes_key.rd_key[4*11+2], 8 );
#endif
				break;
			case 256/8:
#ifdef USE_CACHE
				memcpy( (key_sch), &aes_key.rd_key[4*14], 16 );
				memcpy( (&key_sch[16]), &aes_key.rd_key[4*13], 16 );
#else
				memcpy( UNCACHED_ADDRESS(key_sch), &aes_key.rd_key[4*14], 16 );
				memcpy( UNCACHED_ADDRESS(&key_sch[16]), &aes_key.rd_key[4*13], 16 );
#endif
				break;
			default:
				goto failed;
		}
#ifdef USE_CACHE
		pDecryptoKey = (key_sch);
#else
		pDecryptoKey = UNCACHED_ADDRESS(key_sch);
#endif
		/*memDump( pDecryptoKey, lenCryptoKey, "pDecryptoKey" );*/
#else
		/* IC only accept the final round key in decryption. */
		AES_set_decrypt_key( pCryptoKey, lenCryptoKey*8, &aes_key );
		memcpy( UNCACHED_ADDRESS(key_sch), &aes_key.rd_key[0], lenCryptoKey );
		memDump(&aes_key.rd_key[0], 16*15, "&aes_key.rd_key[0]" );
		pDecryptoKey = UNCACHED_ADDRESS(key_sch);
#endif

#ifdef USE_CACHE
		/* flush cache */
		rtlglue_flushDCache((uint32)key_sch, sizeof(key_sch));
#endif
	}
	else
		pDecryptoKey = pCryptoKey;

#ifdef USE_CACHE
	/* flush cache, before run IPSEC Engine */
	rtlglue_flushDCache((uint32)bufAsic, sizeof(bufAsic));
	rtlglue_flushDCache((uint32)_AsicDigest, sizeof(_AsicDigest));
	rtlglue_flushDCache((uint32)_cryptoKey, sizeof(_cryptoKey));
	rtlglue_flushDCache((uint32)_authKey, sizeof(_authKey));
	rtlglue_flushDCache((uint32)_iv, sizeof(_iv));
	rtlglue_flushDCache((uint32)_pad, sizeof(_pad));
#endif
	retval = rtl865x_ipsecEngine( modeCrypto/*dec*/,  modeAuth, cntScatter, src, 
	                              lenCryptoKey, pDecryptoKey, lenAuthKey, pAuthKey, pIv, pPad, pAsicDigest, 
	                              lenA2eo, lenEnl );
	if ( retval != SUCCESS )
	{
		rtlglue_printf( "%s():%d rtl865x_cryptoEngine() returns %d, crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n", __FUNCTION__, __LINE__, retval, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo );
		rtlglue_printf( "<%lu>", enumbal_total_case_no );
		for( i = 0; i < cntScatter; i++ )
			rtlglue_printf( "[%d:%d]", sa[i], sl[i] );
		goto out;
	}
	
	/*---------------------------------------------------------------------
	 *                               Software DEC
	 *---------------------------------------------------------------------*/
	if ( modeAuth!=(uint32)-1 )
	{
		authSim( modeAuth, bufSw, lenValid, pAuthKey, lenAuthKey, pSwDigest );

		if ( memcmp( pSwDigest, pAsicDigest, modeAuth&1?20/*SHA1*/:16/*MD5*/ )!=0 )
		{
			rtlglue_printf( "DEC DIGEST! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n", modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo );
			rtlglue_printf( "<%lu>", enumbal_total_case_no );
			for( i = 0; i < cntScatter; i++ )
				rtlglue_printf( "[%d:%d]", sa[i], sl[i] );
			memDump( pSwDigest, modeAuth&1?20/*SHA1*/:16/*MD5*/, "Software" );
			memDump( pAsicDigest, modeAuth&1?20/*SHA1*/:16/*MD5*/, "ASIC" );
			goto failed;
		}
	}
	
	if ( modeCrypto!=(uint32)-1 )
	{
		if ( modeCrypto&0x20 )
		{ /* AES */
			aesSim_aes( modeCrypto/*dec*/, bufSw+lenA2eo, bufSw+lenA2eo, lenValid-lenA2eo, lenCryptoKey, pCryptoKey, pIv );
		}
		else
		{ /* DES */
			desSim_des( modeCrypto/*dec*/, bufSw+lenA2eo, bufSw+lenA2eo, lenValid-lenA2eo, pCryptoKey, pIv );
		}

		/* compare with ASIC and SW */
		cur = &bufSw[0];
		retval = SUCCESS;
		for( i = 0; i < cntScatter; i++ )
		{
			if ( memcmp( cur, src[i].ptr, src[i].len )!= 0 )
			{
				int j;

				if ( retval==SUCCESS ) /* print once */
				{
					rtlglue_printf( "DEC DIFF![%d]crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n", i, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo );
					rtlglue_printf( "<%lu>", enumbal_total_case_no );
					for( j = 0; j < cntScatter; j++ )
						rtlglue_printf( "[%d:%d]", sa[j], sl[j] );
				}
				memDump( cur, src[i].len, "Software" );
				memDump( src[i].ptr, src[i].len, "ASIC" );
				retval = FAILED;
			}
			cur += src[i].len;
		}
		if ( retval!=SUCCESS ) goto failed; 
	}

success:
	#if _CBMIX_DEBUG_==2
	if ( (debugIntervalCount%_DEBUG_MSG_INTERVAL_)==0 )
	{
		rtlglue_printf( "\r" );
	}
	debugIntervalCount++;
	#endif
ignore:
	do {} while(0);
pass:
	retval = SUCCESS;
	goto out;
	
failed:

	retval = FAILED;

out:
	return retval;
}
#endif

#if 0
int32 _crypto_mixCmd(uint32 userId,  int32 argc,int8 **saved) 
{
	int32 retval;
	int32 size;
	int8 *nextToken;
	
	cle_getNextCmdToken(&nextToken,&size,saved); 

	if (!strcmp(nextToken, "init"))
	{
		uint32 descNum;
		uint32 mode32Bytes;
		
		cle_getNextCmdToken(&nextToken,&size,saved); 
		descNum = U32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		BurstSize = U32_value(nextToken);

		if ( BurstSize == 64 ) mode32Bytes = 2;
		else if ( BurstSize == 32 ) mode32Bytes = 1;
		else if ( BurstSize == 16 ) mode32Bytes = 0;
		else return FAILED;
		
#ifdef	CONFIG_RTL865X_MODEL_TEST_FT2
		if ( ( retval = rtl8651b_cryptoEngine_init(descNum, mode32Bytes) ) != SUCCESS )
		{
			rtlglue_printf("Crypto Init Fail! {F900}\n");
			return retval;
		}
		if ( ( retval = rtl8651b_authEngine_init(descNum, mode32Bytes) ) != SUCCESS )
		{
			rtlglue_printf("Crypto Init Fail! {F900}\n");
			return retval;
		}

		rtlglue_printf("Crypto Init OK! {P900}\n");

#else
		if ( ( retval = rtl8651b_cryptoEngine_init(descNum, mode32Bytes) ) != SUCCESS )
			return retval;
		if ( ( retval = rtl8651b_authEngine_init(descNum, mode32Bytes) ) != SUCCESS )
			return retval;
#endif
		
	}
	else if (!strcmp(nextToken, "rand"))
	{
	}
	else if (!strcmp(nextToken, "asic"))
	{
#endif
//#ifdef RTL865X_MODEL_KERNEL
int32 _crypto_mixCmdAsic(int32 round, int32 cryptoStart, int32 cryptoEnd, int32 authStart, int32 authEnd, int32 lenCryptoKeyStart, int32 lenCryptoKeyEnd, int32 lenAuthKeyStart, int32 lenAuthKeyEnd, int32  a2eoStart, int32 a2eoEnd, uint32 enumBalMask)
{
//		int32 round, cryptoStart, cryptoEnd, authStart, authEnd, lenCryptoKeyStart, lenCryptoKeyEnd, lenAuthKeyStart, lenAuthKeyEnd, a2eoStart, a2eoEnd;
//		uint32 enumBalMask;
		int32 retval;
		int32 i;
		uint32 bCKeyFix; /* TRUE for 3DES/DES (key length is fixed) */
		int32 sa[8];
		int32 sl[8];
		int32 da[8];
		int32 dl[8];
#if 0
		cle_getNextCmdToken(&nextToken,&size,saved); 
		round = I32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		cryptoStart = I32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		cryptoEnd = I32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		authStart = I32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		authEnd = I32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		lenCryptoKeyStart = I32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		lenCryptoKeyEnd = I32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		lenAuthKeyStart = I32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		lenAuthKeyEnd = I32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		a2eoStart = I32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		a2eoEnd = I32_value(nextToken);	
		cle_getNextCmdToken(&nextToken,&size,saved); 
		enumBalMask = U32_value(nextToken);	
		if ( cle_getNextCmdToken(&nextToken,&size,saved)==SUCCESS )
			initial_case_no = U32_value(nextToken);
		else 
			initial_case_no = 0;
#endif
		initial_case_no = 0;
		rtlglue_printf("Round %u Crypto(%x-%x) Auth(%x-%x) CKey %d-%d AKey %d-%d a2eo %d-%d mask=0x%x\n", round, cryptoStart, cryptoEnd, authStart, authEnd, lenCryptoKeyStart, lenCryptoKeyEnd, lenAuthKeyStart, lenAuthKeyEnd, a2eoStart, a2eoEnd, enumBalMask );

		enumbal_total_case_no = 0;
		for( i = 0; i < round; i++ )
		{
			for( modeCrypto = cryptoStart; modeCrypto <= cryptoEnd; modeCrypto++ )
			{
				/* valid crypto mode: 0x00~0x03, 0x20, 0x22, 0x23 */
				if ( modeCrypto==-1 ||
				     modeCrypto==0x00 ||
				     modeCrypto==0x01 ||
				     modeCrypto==0x02 ||
				     modeCrypto==0x03 ||
				     modeCrypto==0x20 ||
				     modeCrypto==0x22 ||
				     modeCrypto==0x23 )
				{
					/* valid mode, go on */
				}
				else
				{
					continue; /* not valid */
				}
				
				for( modeAuth = authStart; modeAuth <= authEnd; modeAuth++ )
				{
					if ( modeCrypto==0/*DES_CBC*/ || modeCrypto==2/*DES_ECB*/ ) { bCKeyFix=TRUE; lenCryptoKey = 8; goto CKeyFix; } else { bCKeyFix = FALSE; }
					if ( modeCrypto==1/*3DES_CBC*/ || modeCrypto==3/*3DES_ECB*/ ) { bCKeyFix=TRUE; lenCryptoKey = 24; goto CKeyFix; } else { bCKeyFix = FALSE; }
					
					for( lenCryptoKey = lenCryptoKeyStart;  lenCryptoKey <= lenCryptoKeyEnd; lenCryptoKey+=8 )
					{
						if ( ( modeCrypto==0x20/*AES_CBC*/ || modeCrypto==0x22/*AES_ECB*/ || modeCrypto==0x23/*AES_CTR*/ ) &&
						     ( lenCryptoKey < 16 ) )
							continue;
						
						CKeyFix:
						for( lenAuthKey = lenAuthKeyStart;  lenAuthKey <= lenAuthKeyEnd; lenAuthKey+=4 )
						{
							for( lenA2eo = a2eoStart; lenA2eo <= a2eoEnd; lenA2eo+=4 )
							{
								#if _CBMIX_DEBUG_ >= 2
								rtlglue_printf( "Crypto:0x%x Auth:0x%x CKey:%d AKey:%d A2EO:%d\n", modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo );
								#endif
								
								if ( enumBalMask & 0x01 )
								{
									int32 tmplSrcLen[] = { 1, 2, 3, 4, 5, 6, 7 };
									int32 tmplSrcAlign[] = { 0 };
									int32 tmplDstLen[] = { 0 };
									int32 tmplDstAlign[] = { 0 };
									memset( sa, 0, sizeof(sa) );
									memset( sl, 0, sizeof(sl) );
									memset( da, 0, sizeof(da) );
									memset( dl, 0, sizeof(dl) );

									retval = enumBAL( 5, 0, 16376, 16376, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
									                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), 0 ,0 ,
									                  sl, sa, dl, da, cbMix, ENUMBAL_IGNORE_DST );
									if ( retval != SUCCESS ) return retval;
								}
								if ( enumBalMask & 0x02 )
								{
									int32 tmplSrcLen[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };
									int32 tmplSrcAlign[] = { 0, 1, 2, 3 };
									int32 tmplDstLen[] = { 0 };
									int32 tmplDstAlign[] = { 0 };
									memset( sa, 0, sizeof(sa) );
									memset( sl, 0, sizeof(sl) );
									memset( da, 0, sizeof(da) );
									memset( dl, 0, sizeof(dl) );

									retval = enumBAL( 5, 0, 16376, 16376, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
									                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), 0 ,0 ,
									                  sl, sa, dl, da, cbMix, ENUMBAL_IGNORE_DST );
									if ( retval != SUCCESS ) return retval;
								}
								if ( enumBalMask & 0x04 )
								{
									int32 tmplSrcLen[] = { 8, 16, 24, 32, 64, 128 };
									int32 tmplSrcAlign[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
									int32 tmplDstLen[] = { 0 };
									int32 tmplDstAlign[] = { 0 };
									memset( sa, 0, sizeof(sa) );
									memset( sl, 0, sizeof(sl) );
									memset( da, 0, sizeof(da) );
									memset( dl, 0, sizeof(dl) );

									retval = enumBAL( 6, 0, 16376, 16376, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
									                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), 0 ,0 ,
									                  sl, sa, dl, da, cbMix, ENUMBAL_IGNORE_DST );
									if ( retval != SUCCESS ) return retval;
								}
								if ( enumBalMask & 0x08 )
								{
									int32 tmplSrcLen[] = { 1, 2, 3, 4 };
									int32 tmplSrcAlign[] = { 1, 2, 3 };
									int32 tmplDstLen[] = { 0 };
									int32 tmplDstAlign[] = { 0 };
									memset( sa, 0, sizeof(sa) );
									memset( sl, 0, sizeof(sl) );
									memset( da, 0, sizeof(da) );
									memset( dl, 0, sizeof(dl) );

									retval = enumBAL( 7, 0, 16376, 16376, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
									                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), 0 ,0 ,
									                  sl, sa, dl, da, cbMix, ENUMBAL_IGNORE_DST );
									if ( retval != SUCCESS ) return retval;
								}
								if ( enumBalMask & 0x10 )
								{
									int32 tmplSrcLen[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };
									int32 tmplSrcAlign[] = { 0 };
									int32 tmplDstLen[] = { 0 };
									int32 tmplDstAlign[] = { 0 };
									memset( sa, 0, sizeof(sa) );
									memset( sl, 0, sizeof(sl) );
									memset( da, 0, sizeof(da) );
									memset( dl, 0, sizeof(dl) );

									retval = enumBAL( 7, 0, 16376, 16376, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
									                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), 0 ,0 ,
									                  sl, sa, dl, da, cbMix, ENUMBAL_IGNORE_DST );
									if ( retval != SUCCESS ) return retval;
								}
								if ( enumBalMask & 0x20 )
								{
									int32 tmplSrcLen[] = { 1, 2, 3, 4, 5, 6, 7, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62 };
									int32 tmplSrcAlign[] = { 0 };
									int32 tmplDstLen[] = { 0 };
									int32 tmplDstAlign[] = { 0 };
									memset( sa, 0, sizeof(sa) );
									memset( sl, 0, sizeof(sl) );
									memset( da, 0, sizeof(da) );
									memset( dl, 0, sizeof(dl) );

									retval = enumBAL( 3, 0, 16376, 16376, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
									                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), 0, 0,
									                  sl, sa, dl, da, cbMix, ENUMBAL_IGNORE_DST );
									if ( retval != SUCCESS ) return retval;
								}
								if ( enumBalMask & 0x40 ) /* designed for AES and A2EO!=0 */
								{
									int32 tmplSrcLen[] = { 40, 64, 128, 2048 };
									int32 tmplSrcAlign[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
									int32 tmplDstLen[] = { 0 };
									int32 tmplDstAlign[] = { 0 };
									memset( sa, 0, sizeof(sa) );
									memset( sl, 0, sizeof(sl) );
									memset( da, 0, sizeof(da) );
									memset( dl, 0, sizeof(dl) );

									retval = enumBAL( 6, 0, 16376, 16376, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
									                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), 0 ,0 ,
									                  sl, sa, dl, da, cbMix, ENUMBAL_IGNORE_DST );
									if ( retval != SUCCESS ) return retval;
								}
								if ( enumBalMask & 0x80 ) /* designed for MS=10/11 and single data block mode */
								{
									int32 lenData; /* the length of data (included A2EO, ENL, APL) */
									int32 tmplSrcLen[] = { -1 };
									int32 tmplSrcAlign[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
									int32 tmplDstLen[] = { 0 };
									int32 tmplDstAlign[] = { 0 };
									memset( sa, 0, sizeof(sa) );
									memset( sl, 0, sizeof(sl) );
									memset( da, 0, sizeof(da) );
									memset( dl, 0, sizeof(dl) );

									for( lenData = 8; lenData < 16240; lenData+=8 )
									{
										tmplSrcLen[0] = lenData;
										retval = enumBAL( 1, 0, lenData, lenData, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
										                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), 0 ,0 ,
										                  sl, sa, dl, da, cbMix, ENUMBAL_IGNORE_DST );
										if ( retval != SUCCESS ) return retval;
									}
								}
							}
						}
						if ( bCKeyFix==TRUE ) break; /* no need to try other length */
					}
				}
			}
		}

		rtlglue_printf( "\n" );

		return SUCCESS;
}
//#else
//		rtlglue_printf( "\nplease enable RTL865X_MODEL_KERNEL in menuconfig.\n\n" );
//		return FAILED;
//#endif
#if 0
	}
	else
		return FAILED;

	return SUCCESS;
}
#endif
 
#if 0		
int32 _crypto_optionCmd(uint32 userId,  int32 argc,int8 **saved) {
	int32 size;
	int8 *nextToken;
	enum IPSEC_OPTION option;
	uint32 value;
	
	cle_getNextCmdToken(&nextToken,&size,saved);
	if(!strcmp(nextToken, "show"))
	{
		rtlglue_printf( "\n==== IPSEC Engine Control Register ====\n\n" );
		
		rtl865xb_ipsecGetOption( IPSOPT_LBKM, &value );
		if ( value==TRUE ) rtlglue_printf( "  Loopback Mode enabled\n" );

		rtl865xb_ipsecGetOption( IPSOPT_SAWB, &value );
		if ( value==TRUE ) rtlglue_printf( "  SAWB Mode enabled\n" );
		
		rtl865xb_ipsecGetOption( IPSOPT_DMBS, &value );
		rtlglue_printf( "  Dest MAX Burst Size : %d Bytes\n", value );

		rtl865xb_ipsecGetOption( IPSOPT_SMBS, &value );
		rtlglue_printf( "  Source MAX Burst Size : %d Bytes\n", value );

		rtlglue_printf( "\n\n" );
		return SUCCESS;
	}
	else if(!strcmp(nextToken, "loopback"))
	{
		option = IPSOPT_LBKM;
		cle_getNextCmdToken(&nextToken,&size,saved);
		if (!strcmp(nextToken, "enable"))
			value = 1;
		else if (!strcmp(nextToken, "disable"))
			value = 0;
		else
			return FAILED;
	}
	else if(!strcmp(nextToken, "sawb"))
	{
		option = IPSOPT_SAWB;
		cle_getNextCmdToken(&nextToken,&size,saved);
		if (!strcmp(nextToken, "enable"))
			value = 1;
		else if (!strcmp(nextToken, "disable"))
			value = 0;
		else
			return FAILED;
	}
	else if(!strcmp(nextToken, "dmbs"))
	{
		option = IPSOPT_DMBS;
		cle_getNextCmdToken(&nextToken,&size,saved); 
		value = BurstSize = U32_value(nextToken);	
		if ( BurstSize == 64 ) 
			rtlglue_printf("set DMBS 64-byte mode\n");
		else if( BurstSize == 32 ) 
			rtlglue_printf("set DMBS 32-byte mode\n");
		else if( BurstSize == 16 ) 
			rtlglue_printf("set DMBS 16-byte mode\n");
		else return FAILED;
	}
	else if(!strcmp(nextToken, "smbs"))
	{
		option = IPSOPT_SMBS;
		cle_getNextCmdToken(&nextToken,&size,saved); 
		value = BurstSize = U32_value(nextToken);	
		if ( BurstSize == 64 ) 
			rtlglue_printf("set SMBS 64-byte mode\n");
		else if( BurstSize == 32 ) 
			rtlglue_printf("set SMBS 32-byte mode\n");
		else if( BurstSize == 16 ) 
			rtlglue_printf("set SMBS 16-byte mode\n");
		else return FAILED;
	}
	else
		return FAILED;

	return rtl865xb_ipsecSetOption( option, value );
}


cle_exec_t rtl865x_crypt_cmds[] = 
{
	{   "crypto",
		"Crypto engine commands",
		" { init %d'descNum' %d'burst-size' | sim |"
		"   rand'random test' %d'seed' %d'round' |"
		"   asic'compare SW and ASIC' %d'round' %d'funStart' %d'funEnd' %d'lenStart' %d'lenEnd' %d'offsetStart' %d'offsetEnd' |"
		"   batch %d'round' %d'funStart' %d'funEnd' %d'lenStart' %d'lenEnd' %d'offsetStart' %d'offsetEnd' %d'batchStart' %d'batchEnd' |"
		"   throughput { asic | sw } %d'round' %d'StartMode' %d'EndMode' %d'PktLen' }",
		_rtl8651b_cryptoCmd,
		CLE_USECISCOCMDPARSER,
		0,
		NULL
	},
	{   "auth",
		"Auth engine commands",
		" { init %d'descNum' %d'burst-size' | sim |"
		"   rand'random test to compare the results of ASIC and SW.' %d'seed' %d'round' { asic | sw } |"
		"   asic'use ASIC encrypt a random data, and expect ASIC can decrypt to original data.' %d'round' %d'funStart' %d'funEnd' %d'lenStart' %d'lenEnd' %d'keyStart' %d'keyEnd' %d'offsetStart' %d'offsetEnd' |"
		"   batch %d'round' %d'funStart' %d'funEnd' %d'lenStart' %d'lenEnd' %d'keyStart' %d'keyEnd' %d'offsetStart' %d'offsetEnd' %d'batchStart' %d'batchEnd' |"
		"   throughput { asic | sw } %d'round' %d'StartMode' %d'EndMode' %d'PktLen' }",
		_rtl8651b_authenticationCmd,
		CLE_USECISCOCMDPARSER,
		0,
		NULL
	},
	{	"aes",
		"AES algorithm.",
		" { { { aes-0'AES implemented by openSSL.' | aes-1'AES implemented by Brain Gladman.' } { verify'Testing pattern' | throughput'Throughput' | time-trial'Performance measure.'  { ebc | cbc } %d'Key length' %d'block size(byte)' %d'repeat count' } } | "
		"   { init %d'descNum' %d'burst-size' } | "
		"   { rand'random test' %d'seed' %d'round' } | "
		"   { asic'compare SW and ASIC' { { %d'round' %d'funStart (0:CBC_AES,2:ECB_AES,3:CTR_AES)' %d'funEnd (0:CBC_AES,2:ECB_AES,3:CTR_AES)' "
		"                                   %d'lenStart (>=16)' %d'lenEnd(<=16368)' %d'offsetStart' %d'offsetEnd' } | "
		"                                 { verify'Testing pattern' } } } } ",
		_crypto_aesTestCmd,
		CLE_USECISCOCMDPARSER,
		0,
		NULL
	},
	{	"mix",
		"Test all algorithm together",
		" { { init %d'descNum' %d'burst-size' } | "
		"   { rand'random test' %d'seed' %d'round' } | "
		"   { asic'compare SW and ASIC' { %d'round' %d'cryptoStart (-1:NONE,0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES,0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES)' %d'cryptoEnd (0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES,0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES)' "
		"                                 %d'authStart (-1:NONE,0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1)' %d'authEnd (0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1)'"
		"                                 %d'lenCryptoKeyStart (for AES: 16,24,32)' %d'lenCryptoKeyEnd (for AES: 16,24,32)' "
		"                                 %d'lenAuthKeyStart' %d'lenAuthKeyEnd' "
		"                                 %d'a2eoStart (4-bytes)' %d'a2eoEnd (4-bytes)' "
		"                                 %d'enumBAL mask' "
		"                                 [ %d'enum initial caseno' ] "
		"                               } } } ",
		_crypto_mixCmd,
		CLE_USECISCOCMDPARSER,
		0,
		NULL
	},
	{	"option",
		"Set option of 865xc ipsec engine",
		" { "
		"   { show'Show settings' } | "
		"   { loopback'loopback mode' { enable'enable' | disable'disable' } } | "
		"   { sawb'Source Address Write Back' { enable'enable' | disable'disable' } } | "
		"   { dmbs'Dest DMA Max Burst Size' %d'16/32/64 bytes' } | "
		"   { smbs'Source DMA Max Burst Size' %d'16/32/64 bytes' } "
		" } ",
		_crypto_optionCmd,
		CLE_USECISCOCMDPARSER,
		0,
		NULL
	},
};
#endif
