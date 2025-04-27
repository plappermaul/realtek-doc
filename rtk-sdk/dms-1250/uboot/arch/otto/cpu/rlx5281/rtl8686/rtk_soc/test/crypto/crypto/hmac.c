/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : HMAC MD5 and HMAC SHA-1 code
* Abstract : Porting HMACMD5 from RFC2104
* $Id: hmac.c,v 1.1 2005/11/01 03:22:38 yjlou Exp $
* $Log: hmac.c,v $
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
* Revision 1.2  2003/09/02 14:58:36  jzchen
* HMAC emulation API
*
* --------------------------------------------------------------------
*/
#include "md5.h"
#include "hmac.h"
#include "sha1.h"

int32 HMACMD5(uint8 * data, uint32 dataLen, uint8 * key, uint32 keyLen, uint8 * digest) {
	MD5_CTX context;
	uint8 k_ipad[HMAC_MAX_MD_CBLOCK+1], k_opad[HMAC_MAX_MD_CBLOCK+1], tk[MD5_DIGEST_LENGTH];
	uint32 i;
	
	if(keyLen > HMAC_MAX_MD_CBLOCK) {
		MD5Init(&context);
		MD5Update(&context, key, keyLen);
		MD5Final(tk, &context);
		key = tk;
		keyLen = MD5_DIGEST_LENGTH;
	}

	memset(k_ipad, 0, sizeof k_ipad);
	memset(k_opad, 0, sizeof k_opad);
	memcpy(k_ipad, key, keyLen);
	memcpy(k_opad, key, keyLen);

	for(i=0; i<HMAC_MAX_MD_CBLOCK; i++) {
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}

	//Inner MD5
	MD5Init(&context);
	MD5Update(&context, k_ipad, HMAC_MAX_MD_CBLOCK);
	MD5Update(&context, data, dataLen);
	MD5Final(digest, &context);
	//Outer MD5
	MD5Init(&context);
	MD5Update(&context, k_opad, HMAC_MAX_MD_CBLOCK);
	MD5Update(&context, digest, MD5_DIGEST_LENGTH);
	MD5Final(digest, &context);

	return SUCCESS;
}

int32 HMACSHA1(uint8 * data, uint32 dataLen, uint8 * key, uint32 keyLen, uint8 * digest) {
	SHA1_CTX context;
	uint8 k_ipad[HMAC_MAX_MD_CBLOCK+1], k_opad[HMAC_MAX_MD_CBLOCK+1], tk[SHA_DIGEST_LENGTH];
	uint32 i;
	
	if(keyLen > HMAC_MAX_MD_CBLOCK) {
		SHA1Init(&context);
		SHA1Update(&context, key, keyLen);
		SHA1Final(tk, &context);
		key = tk;
		keyLen = SHA_DIGEST_LENGTH;
	}

	memset(k_ipad, 0, sizeof k_ipad);
	memset(k_opad, 0, sizeof k_opad);
	memcpy(k_ipad, key, keyLen);
	memcpy(k_opad, key, keyLen);

	for(i=0; i<HMAC_MAX_MD_CBLOCK; i++) {
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}

	//Inner MD5
	SHA1Init(&context);
	SHA1Update(&context, k_ipad, HMAC_MAX_MD_CBLOCK);
	SHA1Update(&context, data, dataLen);
	SHA1Final(digest, &context);
	//Outer MD5
	SHA1Init(&context);
	SHA1Update(&context, k_opad, HMAC_MAX_MD_CBLOCK);
	SHA1Update(&context, digest, SHA_DIGEST_LENGTH);
	SHA1Final(digest, &context);
	
	return SUCCESS;
}

