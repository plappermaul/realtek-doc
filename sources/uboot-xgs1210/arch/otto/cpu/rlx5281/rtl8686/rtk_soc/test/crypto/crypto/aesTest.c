/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : 
* Abstract : Porting openssl testing code and use it to test both software des function and asic des co-processor
* $Id: aesTest.c,v 1.7 2006/07/13 16:03:06 chenyl Exp $
*/
#include "aes.h"
#include "aesTest.h"
#include "aesSim.h"
#include "rtl865xc_cryptoEngine.h"
#include "asicRegs.h"
//#include <rtl_glue.h>



static int8 *cryptGenOrg, *cryptGenEncrypt, *cryptGenDecrypt, *cryptGenAsic;
static int8 _cryptGenKey[32], _cryptGenIv[16];
static int8 *cryptGenKey, *cryptGenIv;
static int8 * aesOpModeString[] = {
	"CBC_AES ",
	"NONE",
	"ECB_AES ",
	"AES_CTR",
};
static void displayAesEncryptMismatch(int8 * modeTitle, int8 * title, int8 * org, int8 * data1, int8 * data2, uint32 pktLen) {
	uint32 i, j;
	
	rtlglue_printf("\n%s %s [%p] [%p]\n", modeTitle, title, data1, data2);
	for(i=0;i<pktLen;i+=8) {
		if(memcmp(&data1[i], &data2[i], 8) != 0) {
			rtlglue_printf("%04u:", i);
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", org[i+j]&0xff);
			rtlglue_printf("->[%p]",&data1[i]); 
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", data1[i+j]&0xff);
			rtlglue_printf("!=[%p]",&data2[i]);
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", data2[i+j]&0xff);
			rtlglue_printf("\n");
		}
	}

}

static void displayAesDecryptMismatch(int8 * modeTitle, int8 * title, int8 * org, int8 * data, uint32 pktLen) {
	uint32 i, j;
	
	rtlglue_printf("\n%s %s [%p]\n", modeTitle, title, data);
	for(i=0;i<pktLen;i+=8) {
		if(memcmp(&org[i], &data[i], 8) != 0) {
			rtlglue_printf("%04u: ", i);
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", org[i+j]&0xff);
			rtlglue_printf(" ->> [%p]",&data[i]); 
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", data[i+j]&0xff);
			rtlglue_printf("\n");
		}
	}

}

/********************************************************************
 * To provide general API for 'cr aes as' and 'cr aes rand' commands. *
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
 ********************************************************************/
static int32 aes8651bGeneralApiTestItem(uint32 mode, int8 * input, uint32 pktLen, uint32 lenKey, int8 * key, int8 * iv) {
	int32 ret = SUCCESS;
	AES_KEY aes_key;
	uint8 key_sch[128/8];

	/* We have no mode for 0x21. */
	if ( (mode&~4)==0x21) return SUCCESS;
	
	memcpy(cryptGenOrg, input, pktLen);//Backup original data for final comparision

	aesSim_aes( mode|4/*enc*/, cryptGenOrg, cryptGenEncrypt, pktLen, 
	            lenKey, key, iv );
	if(rtl8651b_cryptoEngine_des(mode|4/*encrypt*/, input, pktLen, key, iv) == FAILED)
	{
		rtlglue_printf("ASIC Encrypt FAILED\n");
		return FAILED;//Crypto engine unable to encrypt is mandatory testing failed
	}
	if(memcmp(input, cryptGenEncrypt, pktLen) != 0) 
	{
		displayAesEncryptMismatch(aesOpModeString[mode&3], "Encrypt ASIC != simulator", cryptGenOrg, input, cryptGenEncrypt, pktLen);
		ret = FAILED;
	}

	aesSim_aes( mode, cryptGenEncrypt, cryptGenDecrypt, pktLen, 
	            lenKey, key, iv );
	if ( mode==0x23 )
	{ /* CTR_AES */
		if(rtl8651b_cryptoEngine_des(mode|4/*enc*/, input, pktLen, key, iv) == FAILED)
		{
			rtlglue_printf("ASIC Decrypt FAILED\n");
			return FAILED;//Crypto engine unable to encrypt is mandatory testing failed
		}
	}
	else
	{ /* ECB_AES, CBC_AES */
		AES_set_decrypt_key(key, 128, &aes_key );
#ifdef CONFIG_RTL8316S
mips_cache_flush_all();
#else
_cache_flush_all();
#endif
		memcpy( UNCACHED_ADDRESS(key_sch), &aes_key.rd_key[0], 128/8 );

		if(rtl8651b_cryptoEngine_des(mode&~4/*decrypt*/, input, pktLen, UNCACHED_ADDRESS(key_sch), iv) == FAILED)
		{
			rtlglue_printf("ASIC Decrypt FAILED\n");
			return FAILED;//Crypto engine unable to encrypt is mandatory testing failed
		}
	}
	
	if(memcmp(input, cryptGenDecrypt, pktLen) != 0) 
	{
		if(memcmp(cryptGenDecrypt, cryptGenOrg, pktLen))
		{
			displayAesDecryptMismatch(aesOpModeString[mode&3], "Decrypt Simulator != original", cryptGenOrg, cryptGenDecrypt, pktLen);
			ret = FAILED;
		}
		else 
		{
			displayAesDecryptMismatch(aesOpModeString[mode&3], "Decrypt ASIC != original", cryptGenOrg, input, pktLen);
			ret = FAILED;
		}
	}
	return ret;
}

/***********************************
 * By 'crypto aes asic' command *
 *   fixed key/iv/data             *
 ***********************************/
int32 runAes8651bGeneralApiTest(uint32 round, uint32 funStart, uint32 funEnd, uint32 lenStart, uint32 lenEnd, uint32 offsetStart, uint32 offsetEnd) {
	uint32 roundIdx, modeIdx, pktLen,  i, offset, errCount;
	uint32 doneCounter, allDoneCounter;
	int32 ret = SUCCESS;

/* Since UNCACHED_MALLOC() is un-safe if allocation fails, we need to rewrite the following code. */
#define UNCACHED_MALLOC(x) UNCACHED_ADDRESS(rtlglue_malloc(x))
#define UNCACHED_FREE(x) rtlglue_free((void*)CACHED_ADDRESS(x))
	cryptGenKey = UNCACHED_ADDRESS( _cryptGenKey );
	cryptGenIv = UNCACHED_ADDRESS( _cryptGenIv );
	cryptGenOrg = (int8 *) rtlglue_malloc(MAX_PKTLEN+8/*for alignment issue*/);
	cryptGenEncrypt = (int8 *) rtlglue_malloc(MAX_PKTLEN+8/*for alignment issue*/);
	cryptGenDecrypt = (int8 *) rtlglue_malloc(MAX_PKTLEN+8/*for alignment issue*/);
	cryptGenAsic = (int8 *) UNCACHED_MALLOC(MAX_PKTLEN+8/*for alignment issue*/);

#ifdef CONFIG_RTL8316S
mips_cache_flush_all();
#else
_cache_flush_all();
#endif

	for(i=0; i<24; i++)
		cryptGenKey[i] = 0x01;
	for(i=0; i<8; i++)
		cryptGenIv[i] = 0x01;
	errCount = 0;
	for(roundIdx = 0; roundIdx<round; roundIdx++)
		for(modeIdx=funStart; modeIdx<=funEnd; modeIdx++)
			for(pktLen=lenStart; pktLen<=lenEnd; pktLen+=16) 
				for(offset=offsetStart; offset<=offsetEnd; offset++) {
					for(i=0; i<pktLen; i++)//8-byte for IV and 4-byte for DMA_addr (0-3) byte offset test
						cryptGenAsic[i+offset] = (i&0xFF);
					rtl8651b_cryptoEngineGetIntCounter(&doneCounter, &allDoneCounter);
					rtlglue_printf("\r[%05u, %03u]Mode: %s Packet length %04u Offset %u (%u, %u)", roundIdx, errCount, aesOpModeString[modeIdx&0x3], pktLen, offset, doneCounter, allDoneCounter);
					if(aes8651bGeneralApiTestItem((modeIdx&0x3)|0x20, &cryptGenAsic[offset], pktLen, 128/8, cryptGenKey, cryptGenIv) == FAILED) 
						errCount++;
				}
	UNCACHED_FREE(cryptGenAsic);
	rtlglue_free(cryptGenDecrypt);
	rtlglue_free(cryptGenEncrypt);
	rtlglue_free(cryptGenOrg);
	rtlglue_printf("\nGeneral API Test Finished. errCount=%d\n", errCount );
	if ( errCount > 0 ) ret = FAILED;
	return ret;	
}

/****************************************
 * By 'crypto aes rand' command      *
 *   random key/iv/mode/len/offset/data *
 ****************************************/
int32 runAes8651bGeneralApiRandTest(uint32 seed, uint32 round) {
	uint32 roundIdx, modeIdx, pktLen,  i, offset, errCount;
	uint32 doneCounter, allDoneCounter;
	int32 ret = SUCCESS;

	cryptGenKey = UNCACHED_ADDRESS( _cryptGenKey );
	cryptGenIv = UNCACHED_ADDRESS( _cryptGenIv );
	cryptGenOrg = (int8 *) rtlglue_malloc(MAX_PKTLEN+8/*for alignment issue*/);
	cryptGenEncrypt = (int8 *) rtlglue_malloc(MAX_PKTLEN+8/*for alignment issue*/);
	cryptGenDecrypt = (int8 *) rtlglue_malloc(MAX_PKTLEN+8/*for alignment issue*/);
	cryptGenAsic = (int8 *) UNCACHED_MALLOC(MAX_PKTLEN+8/*for alignment issue*/);
	rtlglue_srandom(seed);
	errCount = 0;

#ifdef CONFIG_RTL8316S
mips_cache_flush_all();
#else
_cache_flush_all();
#endif

	for(roundIdx = 0; roundIdx<round; roundIdx++) {
		for(i=0; i<24; i++)
			cryptGenKey[i] = rtlglue_random()&0xFF;
		for(i=0; i<8; i++)
			cryptGenIv[i] = rtlglue_random()&0xFF;
		modeIdx = rtlglue_random()&0x3;
		pktLen = ((rtlglue_random()%(MAX_PKTLEN-16))&0x3FF0) + 16; /* 16 ~ 16368 */
		offset = rtlglue_random()&0x7;
		for(i=0; i<pktLen; i++)//8-byte for IV and 4-byte for DMA_addr (0-3) byte offset test
			cryptGenAsic[i+offset] = rtlglue_random()&0xFF;
		rtl8651b_cryptoEngineGetIntCounter(&doneCounter, &allDoneCounter);
		rtlglue_printf("\r[%05u, %03u]Mode: %s Packet length %04u Offset %u (%u, %u)", roundIdx, errCount, aesOpModeString[modeIdx&0x3], pktLen, offset, doneCounter, allDoneCounter);
		if(aes8651bGeneralApiTestItem((modeIdx&0x3)|0x20, &cryptGenAsic[offset], pktLen, 128/8, cryptGenKey, cryptGenIv) == FAILED) 
			errCount++;
	}
	UNCACHED_FREE(cryptGenAsic);
	rtlglue_free(cryptGenDecrypt);
	rtlglue_free(cryptGenEncrypt);
	rtlglue_free(cryptGenOrg);
	rtlglue_printf("\nGeneral API Test Finished. errCount=%d\n", errCount );
	if ( errCount > 0 ) ret = FAILED;
	return ret;	
}


