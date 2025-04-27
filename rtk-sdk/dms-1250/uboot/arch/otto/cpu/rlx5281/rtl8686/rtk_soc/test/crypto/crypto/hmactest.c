/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : 
* Abstract : Porting openssl testing code and use it to test both software hmac function and asic hmac co-processor
* $Id: hmactest.c,v 1.1 2005/11/01 03:22:38 yjlou Exp $
* $Log: hmactest.c,v $
* Revision 1.1  2005/11/01 03:22:38  yjlou
* *** empty log message ***
*
* Revision 1.2  2004/06/23 10:15:45  yjlou
* *: convert DOS format to UNIX format
*
* Revision 1.1  2004/06/23 09:18:57  yjlou
* +: support 865xB CRYPTO Engine
*   +: CONFIG_RTL865XB_EXP_CRYPTOENGINE
*   +: basic encry/decry functions (DES/3DES/SHA1/MAC)
*   +: old-fashion API (should be removed in next version)
*   +: batch functions (should be removed in next version)
*
* Revision 1.6  2003/09/23 02:20:25  jzchen
* Test generic simulation API
*
* Revision 1.5  2003/09/17 11:32:53  jzchen
* HMAC random test able to specify round number
*
* Revision 1.4  2003/09/09 02:52:04  jzchen
* Add random test to comare software HMAC and simulator HMAC
*
* Revision 1.3  2003/09/08 04:39:47  jzchen
* Add testing authentication simulator
*
* Revision 1.2  2003/09/02 14:59:27  jzchen
* Add RFC2202 test pattern
*
* --------------------------------------------------------------------
*/
/* crypto/hmac/hmactest.c */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 * 
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 * 
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from 
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 * 
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */
#include "md5.h"
#include "hmactest.h"
#include "sha1.h"
#include "hmac.h"
#include "authSim.h"
#include "crypto.h"

static struct testMd5_st
	{
	unsigned char key[80];
	int key_len;
	unsigned char data[128];
	int data_len;
	unsigned char *digest;
	} testMd5[8]={
	{	"",
		0,
		"More text test vectors to stuff up EBCDIC machines :-)",
		54,
		(unsigned char *)"e9139d1e6ee064ef8cf514fc7dc83e86",
	},{	{0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,
		 0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,},
		16,
		"Hi There",
		8,
		(unsigned char *)"9294727a3638bb1c13f48ef8158bfc9d",
	},{	"Jefe",
		4,
		"what do ya want for nothing?",
		28,
		(unsigned char *)"750c783e6ab0b503eaa86e310a5db738",
	},{
		{0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
		 0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,},
		16,
		{0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,
		 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,
		 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,
		 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,
		 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,
		 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,
		 0xdd,0xdd},
		50,
		(unsigned char *)"56be34521d144c88dbb8c733f0e8b3f6",
	},{
		{	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
			0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 
			0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19},
		25,
		{0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
		 0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
		 0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
		 0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
		 0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
		 0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
		 0xcd,0xcd},
		50,
		(unsigned char *)"697eaf0aca3a3aea3a75164746ffaa79",
	},{
		{	0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 
			0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c},
		16,
		"Test With Truncation",
		20,
		(unsigned char *)"56461ef2342edc00f9bab995690efd4c",
	},{
		{	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
		80,
		"Test Using Larger Than Block-Size Key - Hash Key First",
		54,
		(unsigned char *)"6b1ab7fe4bd7bf8f0b62e6ce61b9d0cd",
	},{
		{	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
		80,
		"Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data",
		73,
		(unsigned char *)"6f630fad67cda0ee1fb1f562db3aa53e",
	},
};

static struct testSha1_st
	{
	unsigned char key[80];
	int key_len;
	unsigned char data[128];
	int data_len;
	unsigned char *digest;
	} testSha1[7]={
	{	{	0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,
		 	0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,
		 	0x0b,0x0b,0x0b,0x0b,},
		20,
		"Hi There",
		8,
		(unsigned char *)"b617318655057264e28bc0b6fb378c8ef146be00",
	},{	"Jefe",
		4,
		"what do ya want for nothing?",
		28,
		(unsigned char *)"effcdf6ae5eb2fa2d27416d5f184df9c259a7c79",
	},{
		{	0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
		 	0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
		 	0xaa,0xaa,0xaa,0xaa,},
		20,
		{0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,
		 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,
		 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,
		 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,
		 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,
		 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,
		 0xdd,0xdd},
		50,
		(unsigned char *)"125d7342b9ac11cd91a39af48aa17b4f63f175d3",
	},{
		{	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
			0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 
			0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19},
		25,
		{0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
		 0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
		 0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
		 0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
		 0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
		 0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
		 0xcd,0xcd},
		50,
		(unsigned char *)"4c9007f4026250c6bc8414f9bf50c86c2d7235da",
	},{
		{	0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 
			0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 
			0x0c, 0x0c, 0x0c, 0x0c},
		20,
		"Test With Truncation",
		20,
		(unsigned char *)"4c1a03424b55e07fe7f27be1d58bb9324a9a5a04",
	},{
		{	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
		80,
		"Test Using Larger Than Block-Size Key - Hash Key First",
		54,
		(unsigned char *)"aa4ae5e15272d00e95705637ce8a3b55ed402112",
	},{
		{	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
			0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
		80,
		"Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data",
		73,
		(unsigned char *)"e8e99d0f45237d786d6bbaa7965c7808bbff1a91",
	},
};
 static uint8 *ptMd5(uint8 *md) {
	int i;
	static uint8 buf[80];

	for (i=0; i<MD5_DIGEST_LENGTH; i++)
		sprintf(&(buf[i*2]),"%02x",md[i]);
	return(buf);
}

static uint8 *ptSha1(uint8 *md) {
	int i;
	static uint8 buf[80];

	for (i=0; i<SHA_DIGEST_LENGTH; i++)
		sprintf(&(buf[i*2]),"%02x",md[i]);
	return(buf);
}

static uint8 data[2048];

int32 hmactest(void)	{
	int err=0;
	uint32 i;
	uint8 *p, digest[SHA_DIGEST_LENGTH];

	for (i=0; i<8; i++) {
		if(HMACMD5(testMd5[i].data, testMd5[i].data_len, testMd5[i].key, testMd5[i].key_len, digest) == FAILED)
			rtlglue_printf("HMAC MD5 execuition failed\n");
		p = ptMd5(digest);
		if (strncmp(p,(char *)testMd5[i].digest, MD5_DIGEST_LENGTH<<1) != 0)
			{
			rtlglue_printf("error calculating HMAC MD5 on %d entry'\n",i);
			rtlglue_printf("got %s instead of %s\n",p,testMd5[i].digest);
			err++;
			}
		else
			rtlglue_printf("test HMAC MD5 %d ok\n",i);

		memcpy(data, testMd5[i].data, testMd5[i].data_len);
		if(authSim_hmacMd5(data, testMd5[i].data_len, testMd5[i].key, testMd5[i].key_len, digest) == FAILED)
			rtlglue_printf("HMAC MD5 simulation failed\n");
		p = ptMd5(digest);
		if (strncmp(p,(char *)testMd5[i].digest, MD5_DIGEST_LENGTH<<1) != 0) {
			rtlglue_printf("auth simulator error calculating HMAC MD5 on %d entry'\n",i);
			rtlglue_printf("got %s instead of %s\n",p,testMd5[i].digest);
			err++;
		}
		else
			rtlglue_printf("test HMAC MD5 simulator %d ok\n",i);

		memcpy(data, testMd5[i].data, testMd5[i].data_len);
		if(authSim(SWHMAC_MD5, data, testMd5[i].data_len, testMd5[i].key, testMd5[i].key_len, digest) == FAILED)
			rtlglue_printf("HMAC MD5 simulation 2 failed\n");
		p = ptMd5(digest);
		if (strncmp(p,(char *)testMd5[i].digest, MD5_DIGEST_LENGTH<<1) != 0) {
			rtlglue_printf("auth simulator 2 error calculating HMAC MD5 on %d entry'\n",i);
			rtlglue_printf("got %s instead of %s\n",p,testMd5[i].digest);
			err++;
		}
		else
			rtlglue_printf("test HMAC MD5 simulator %d ok\n",i);
	}
	for (i=0; i<7; i++) {
		if(HMACSHA1(testSha1[i].data, testSha1[i].data_len, testSha1[i].key, testSha1[i].key_len, digest) == FAILED)
			rtlglue_printf("HMAC SHA1 execuition failed\n");
		p = ptSha1(digest);

		if (strncmp(p,(char *)testSha1[i].digest, SHA_DIGEST_LENGTH<<1) != 0)
			{
			rtlglue_printf("error calculating HMAC SHA1 on %d entry'\n",i);
			rtlglue_printf("got %s instead of %s\n",p,testSha1[i].digest);
			err++;
			}
		else
			rtlglue_printf("test HMAC SHA1 %d ok\n",i);

		memcpy(data, testSha1[i].data, testSha1[i].data_len);
		if(authSim_hmacSha1(data, testSha1[i].data_len, testSha1[i].key, testSha1[i].key_len, digest) == FAILED)
			rtlglue_printf("HMAC SHA1 simulation failed\n");
		p = ptSha1(digest);
		if (strncmp(p,(char *)testSha1[i].digest, SHA_DIGEST_LENGTH<<1) != 0) {
			rtlglue_printf("auth simulator error calculating HMAC SHA1 on %d entry'\n",i);
			rtlglue_printf("got %s instead of %s\n",p,testSha1[i].digest);
			err++;
		}
		else
			rtlglue_printf("test HMAC SHA1 simulator %d ok\n",i);

		memcpy(data, testSha1[i].data, testSha1[i].data_len);
		if(authSim(SWHMAC_SHA1, data, testSha1[i].data_len, testSha1[i].key, testSha1[i].key_len, digest) == FAILED)
			rtlglue_printf("HMAC SHA1 simulation 2 failed\n");
		p = ptSha1(digest);
		if (strncmp(p,(char *)testSha1[i].digest, SHA_DIGEST_LENGTH<<1) != 0) {
			rtlglue_printf("auth simulator 2 error calculating HMAC SHA1 on %d entry'\n",i);
			rtlglue_printf("got %s instead of %s\n",p,testSha1[i].digest);
			err++;
		}
		else
			rtlglue_printf("test HMAC SHA1 simulator %d ok\n",i);
	}
	return(0);
}

uint8 simRandTestData[2048], simRandTestKey[1024];
int32 hmacSimRandTest(uint32 roundLimit) {
	uint32 i, dataLen, keyLen, round;
	uint8 swDigest[SHA_DIGEST_LENGTH], simDigest[SHA_DIGEST_LENGTH];
	
	rtlglue_srandom(19);
	for(round = 0; round < roundLimit; round++) {
		rtlglue_printf("\rRound %5u", round);
		dataLen = rtlglue_random()%2040;
		keyLen = rtlglue_random()%1024;
		for(i=0; i<dataLen; i++)
			simRandTestData[i] = (uint8)rtlglue_random()%256;
		for(i=0; i<keyLen; i++)
			simRandTestKey[i] = (uint8)rtlglue_random()%256;
		HMACMD5(simRandTestData, dataLen, simRandTestKey, keyLen, swDigest);
		authSim_hmacMd5(simRandTestData, dataLen, simRandTestKey, keyLen, simDigest);
		if(memcmp(swDigest, simDigest, MD5_DIGEST_LENGTH)) {
			rtlglue_printf("\nHMAC MD5 Mismatch! Data length %u Key length %u\nSW Digest:\n", dataLen, keyLen);
			for(i=0; i<MD5_DIGEST_LENGTH; i++) {
				rtlglue_printf("%02x ", swDigest[i]&0xFF);
				if(i%8==7 || i==(MD5_DIGEST_LENGTH-1))
					rtlglue_printf("\n");
			}
			rtlglue_printf("Sim Digest:\n");
			for(i=0; i<MD5_DIGEST_LENGTH; i++) {
				rtlglue_printf("%02x ", simDigest[i]&0xFF);
				if(i%8==7 || i==(MD5_DIGEST_LENGTH-1))
					rtlglue_printf("\n");
			}
			return FAILED;
		}
		authSim(SWHMAC_MD5, simRandTestData, dataLen, simRandTestKey, keyLen, simDigest);
		if(memcmp(swDigest, simDigest, MD5_DIGEST_LENGTH)) {
			rtlglue_printf("\nHMAC MD5 Mismatch! Data length %u Key length %u\nSW Digest:\n", dataLen, keyLen);
			for(i=0; i<MD5_DIGEST_LENGTH; i++) {
				rtlglue_printf("%02x ", swDigest[i]&0xFF);
				if(i%8==7 || i==(MD5_DIGEST_LENGTH-1))
					rtlglue_printf("\n");
			}
			rtlglue_printf("Sim 2 Digest:\n");
			for(i=0; i<MD5_DIGEST_LENGTH; i++) {
				rtlglue_printf("%02x ", simDigest[i]&0xFF);
				if(i%8==7 || i==(MD5_DIGEST_LENGTH-1))
					rtlglue_printf("\n");
			}
			return FAILED;
		}
		HMACSHA1(simRandTestData, dataLen, simRandTestKey, keyLen, swDigest);
		authSim_hmacSha1(simRandTestData, dataLen, simRandTestKey, keyLen, simDigest);
		if(memcmp(swDigest, simDigest, SHA_DIGEST_LENGTH)) {
			rtlglue_printf("\nHMAC SHA1 Mismatch! Data length %u Key length %u\nSW Digest:\n", dataLen, keyLen);
			for(i=0; i<SHA_DIGEST_LENGTH; i++) {
				rtlglue_printf("%02x ", swDigest[i]&0xFF);
				if(i%8==7 || i==(SHA_DIGEST_LENGTH-1))
					rtlglue_printf("\n");
			}
			rtlglue_printf("Sim Digest:\n");
			for(i=0; i<SHA_DIGEST_LENGTH; i++) {
				rtlglue_printf("%02x ", simDigest[i]&0xFF);
				if(i%8==7 || i==(SHA_DIGEST_LENGTH-1))
					rtlglue_printf("\n");
			}
			return FAILED;
		}
		authSim(SWHMAC_SHA1, simRandTestData, dataLen, simRandTestKey, keyLen, simDigest);
		if(memcmp(swDigest, simDigest, SHA_DIGEST_LENGTH)) {
			rtlglue_printf("\nHMAC SHA1 Mismatch! Data length %u Key length %u\nSW Digest:\n", dataLen, keyLen);
			for(i=0; i<SHA_DIGEST_LENGTH; i++) {
				rtlglue_printf("%02x ", swDigest[i]&0xFF);
				if(i%8==7 || i==(SHA_DIGEST_LENGTH-1))
					rtlglue_printf("\n");
			}
			rtlglue_printf("Sim 2 Digest:\n");
			for(i=0; i<SHA_DIGEST_LENGTH; i++) {
				rtlglue_printf("%02x ", simDigest[i]&0xFF);
				if(i%8==7 || i==(SHA_DIGEST_LENGTH-1))
					rtlglue_printf("\n");
			}
			return FAILED;
		}
	}
	rtlglue_printf("\nHMAC Random test PASS\n");
	return SUCCESS;
}

