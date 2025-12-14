/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"

#include "rtl_stdlib.h"

#include "hal_crypto.h"
#include "hal_crypto_ram.h"
#include "rtl8195a_crypto_rom.h"

#undef printk
#define printk DiagPrintf


//
// stdlib for crypto
//


int rtl_memcmpb(const u8 *dst, const u8 *src, int bytes)
{
	return __rtl_memcmpb(dst, src, bytes);
}

#ifdef __ICCARM__
#pragma data_alignment=4
#endif
static u32 _rtl_seed
#ifdef __GNUC__
__attribute__ ((aligned (4)))
#endif
;
#ifdef __ICCARM__
#pragma data_alignment=1
#endif


void rtl_srandom( u32 seed )
{
    _rtl_seed = seed;
}

u32 rtl_random( void )
{
	_rtl_seed = __rtl_random(_rtl_seed);
    return ( _rtl_seed );
}


int rtl_align_to_be32(u32 *pData, int bytes4_num )
{
	if ( pData == NULL ) return FAIL; 
	if ( ((u32)(pData) & 0x3) != 0 ) return FAIL ; 
	__rtl_align_to_be32(pData, bytes4_num);
	return SUCCESS;
}

int rtl_memsetw(u32 *pData, u32 data, int bytes4_num )
{
	if ( pData == NULL ) return FAIL ;
	if ( ((u32)(pData) & 0x3) != 0 ) return FAIL ; 
	__rtl_memsetw(pData, data, bytes4_num);
	return SUCCESS; 
}

int rtl_memsetb(u8 *pData, u8 data, int bytes )
{
	if ( pData == NULL ) return FAIL ;
	__rtl_memsetb(pData, data, bytes);
	return SUCCESS; 
}


int rtl_memcpyw(u32 *dst, u32 *src, int bytes4_num)
{
	if ( src == NULL ) return FAIL;
	if ( dst == NULL ) return FAIL; 
	if ( ((u32)(src) & 0x3) != 0 ) return FAIL ; 
	if ( ((u32)(dst) & 0x3) != 0 ) return FAIL ; 
	__rtl_memcpyw(dst, src, bytes4_num);
	return SUCCESS;
}

int rtl_memcpyb(u8 *dst, const u8 *src, int bytes)
{
	if ( src == NULL ) return FAIL;
	if ( dst == NULL ) return FAIL; 
	__rtl_memcpyb(dst, src, bytes);
	return SUCCESS;
}

void rtl_memDump(const u8 *start, u32 size, char * strHeader)
{
	__rtl_memDump(start, size, strHeader);
}


//
// HAL crypto functions
//

#ifdef __ICCARM__
#pragma data_alignment=32
#endif
static HAL_CRYPTO_ADAPTER g_rtl_cipherEngine
#ifdef __GNUC__
__attribute__ ((aligned (32)))
#endif
;
#ifdef __ICCARM__
#pragma data_alignment=1
#endif




void rtl_cryptoEngine_info(void)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;
	
	_rtl_cryptoEngine_info(pIE);
}



int rtl_cryptoEngine_init(void)
{

	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;

	pIE->desc_num = CRYPTO_MAX_DESP;
	pIE->dma_mode = 0; // 16 byte 
	//pIE->dma_mode = 1; // 32 byte 
	//pIE->dma_mode = 2; // 64 byte 

	
	DBG_INFO_MSG_OFF(_DBG_CRYPTO_); // default : turn off debug message 
	
	return _rtl_cryptoEngine_init(pIE);

}



// MD5


int rtl_crypto_md5_init(void)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;

	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	return _rtl_crypto_md5_init(pIE);
}


const static u8 md5_null_msg_result[16] = {
	0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
    0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E 
};

int rtl_crypto_md5_process(
			IN const u8* message, IN const u32 msglen, 
			OUT u8* pDigest)
{

	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	if ( message == NULL ) {
		if ( msglen > 0 ) {
		    return _ERRNO_CRYPTO_NULL_POINTER;
		} else {
			rtl_memcpyb(pDigest, md5_null_msg_result, 16);
		}
	}
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

	//if ( (u32)(pDigest) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignement


	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	return _rtl_crypto_auth_process(pIE, message, msglen, pDigest);
	
}


/**
  *  @brief	rtl_crypto_md5()  
  * 			 :  Authentication function : MD5
  *
  *				Output = MD5(input buffer)
  *
  *  @param 	message : input buffer
  *  @param    msglen 	: input buffer length
  *  @param    pDigest	: the result of MD5 function 
  *  @return     0        	: SUCCESS <br>
  *                  others 	: fail, refer to ERRNO 
  */

int rtl_crypto_md5(IN const u8* message, IN const u32 msglen, OUT u8* pDigest)
{
	int ret;


	ret = rtl_crypto_md5_init();
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_md5_process(message, msglen, pDigest);

	return ret;
}


// SHA1


int rtl_crypto_sha1_init(void)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;

	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	return _rtl_crypto_sha1_init(pIE);
}



int rtl_crypto_sha1_process(
			IN const u8* message, 	IN const u32 msglen, 
			OUT u8* pDigest)
{

	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pDigest) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignement


	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	return _rtl_crypto_auth_process(pIE, message, msglen, pDigest);
}




int rtl_crypto_sha1(IN const u8* message, IN const u32 msglen, OUT u8* pDigest)
{
	int ret;


	ret = rtl_crypto_sha1_init();
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_sha1_process(message, msglen, pDigest);

	return ret;
}


// SHA2


int rtl_crypto_sha2_init(IN const SHA2_TYPE sha2type)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;

	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	return _rtl_crypto_sha2_init(pIE, sha2type);
}



int rtl_crypto_sha2_process(
			IN const u8* message, IN const u32 msglen, 
			OUT u8* pDigest)
{

	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pDigest) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignement


	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	return _rtl_crypto_auth_process(pIE, message, msglen, pDigest);
}




int rtl_crypto_sha2(
	IN const SHA2_TYPE sha2type, IN const u8* message, IN const u32 msglen, 
	OUT u8* pDigest)
{
	int ret;


	ret = rtl_crypto_sha2_init(sha2type);
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_sha2_process(message, msglen, pDigest);

	return ret;
}


// HMAC-MD5


int rtl_crypto_hmac_md5_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;

	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4 byte alignment
	if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
	if ( keylen > CRYPTO_AUTH_PADDING ) return _ERRNO_CRYPTO_KEY_OutRange;


	return _rtl_crypto_hmac_md5_init(pIE, key, keylen);
}



int rtl_crypto_hmac_md5_process(
			IN const u8* message, 	IN const u32 msglen, 
			OUT u8* pDigest)
{

	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pDigest) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignement


	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	return _rtl_crypto_auth_process(pIE, message, msglen, pDigest);
}




int rtl_crypto_hmac_md5(
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* key, 		IN const u32 keylen, 
			OUT u8* pDigest)
{
	int ret;


	ret = rtl_crypto_hmac_md5_init(key, keylen);
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_hmac_md5_process(message, msglen, pDigest);

	return ret;
}


// HMAC-SHA1


int rtl_crypto_hmac_sha1_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;

	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4 byte alignment
	if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
	if ( keylen > CRYPTO_AUTH_PADDING ) return _ERRNO_CRYPTO_KEY_OutRange;


	return _rtl_crypto_hmac_sha1_init(pIE, key, keylen);
}



int rtl_crypto_hmac_sha1_process(
			IN const u8* message, 	IN const u32 msglen, 
			OUT u8* pDigest)
{

	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pDigest) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignement


	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	return _rtl_crypto_auth_process(pIE, message, msglen, pDigest);
}




int rtl_crypto_hmac_sha1(
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* key, 		IN const u32 keylen, 
			OUT u8* pDigest)
{
	int ret;


	ret = rtl_crypto_hmac_sha1_init(key, keylen);
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_hmac_sha1_process(message, msglen, pDigest);

	return ret;
}


// HMAC-SHA2


int rtl_crypto_hmac_sha2_init(
			IN const SHA2_TYPE sha2type, 	
			IN const u8* key, 	IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;

	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4 byte alignment
	if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
	if ( keylen > CRYPTO_AUTH_PADDING ) return _ERRNO_CRYPTO_KEY_OutRange;


	return _rtl_crypto_hmac_sha2_init(pIE, sha2type, key, keylen);
}



int rtl_crypto_hmac_sha2_process(
			IN const u8* message, 	IN const u32 msglen, 
			OUT u8* pDigest)
{

	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pDigest) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignement


	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	return _rtl_crypto_auth_process(pIE, message, msglen, pDigest);
}




int rtl_crypto_hmac_sha2(
			IN const SHA2_TYPE sha2type, 
			IN const u8* message, 			IN const u32 msglen, 
			IN const u8* key, 				IN const u32 keylen,
			OUT u8* pDigest)
{
	int ret;


	ret = rtl_crypto_hmac_sha2_init(sha2type, key, keylen);
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_hmac_sha2_process(message, msglen, pDigest);

	return ret;
}



// 3DES length 8 byte alignment


int rtl_crypto_aes_cbc_init(IN const u8* key, 	IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( (keylen != 128/8) && (keylen != 192/8) && (keylen != 256/8) ) return _ERRNO_CRYPTO_KEY_OutRange;

	return _rtl_crypto_aes_cbc_init(pIE, key, keylen);
}


int rtl_crypto_aes_cbc_encrypt(
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* iv, 		IN const u32 ivlen, 
			OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen != 16 ) return _ERRNO_CRYPTO_IV_OutRange;
	

	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	

	return _rtl_crypto_aes_cbc_encrypt(pIE, message, msglen, iv, ivlen, pResult);
}



int rtl_crypto_aes_cbc_decrypt(
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* iv, 		IN const u32 ivlen, 
			OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;	
	if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen != 16 ) return _ERRNO_CRYPTO_IV_OutRange;

	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	

	return _rtl_crypto_aes_cbc_decrypt(pIE, message, msglen, iv, ivlen, pResult);
}



int rtl_crypto_aes_ecb_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( (keylen != 128/8) && (keylen != 192/8) && (keylen != 256/8) ) return _ERRNO_CRYPTO_KEY_OutRange;

	return _rtl_crypto_aes_ecb_init(pIE, key, keylen);
}


int rtl_crypto_aes_ecb_encrypt(
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* iv, 		IN const u32 ivlen, 
			OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;	
	if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
	
	if ( ivlen != 0 ) return _ERRNO_CRYPTO_IV_OutRange; // ECB mode : ivlen = 0
	

	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	
	return _rtl_crypto_aes_ecb_encrypt(pIE, message, msglen, iv, ivlen, pResult);
}



int rtl_crypto_aes_ecb_decrypt(
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;	
	if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
	
	if ( ivlen != 0 ) return _ERRNO_CRYPTO_IV_OutRange; // ECB mode : ivlen = 0

	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return _rtl_crypto_aes_ecb_decrypt(pIE, message, msglen, iv, ivlen, pResult);
}



int rtl_crypto_aes_ctr_init(IN const u8* key, 	IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( (keylen != 128/8) && (keylen != 192/8) && (keylen != 256/8) ) return _ERRNO_CRYPTO_KEY_OutRange;

	return _rtl_crypto_aes_ctr_init(pIE, key, keylen);
}



int rtl_crypto_aes_ctr_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
	
	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	

	return _rtl_crypto_aes_ctr_encrypt(pIE, message, msglen, iv, ivlen, pResult);
}



int rtl_crypto_aes_ctr_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;



	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;

	
	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


	return _rtl_crypto_aes_ctr_decrypt(pIE, message, msglen, iv, ivlen, pResult);
}


//
// 3DES-CBC
//

int rtl_crypto_3des_cbc_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
	if ( keylen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return _rtl_crypto_3des_cbc_init(pIE, key, keylen);
}


int rtl_crypto_3des_cbc_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen < 0 ) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
	
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pResult) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	

	return _rtl_crypto_3des_cbc_encrypt(pIE, message, msglen, iv, ivlen, pResult);
}


int rtl_crypto_3des_cbc_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;



	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen < 0 ) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;

	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pResult) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	

	return _rtl_crypto_3des_cbc_decrypt(pIE, message, msglen, iv, ivlen, pResult);
}


//
// 3DES-ECB
//

int rtl_crypto_3des_ecb_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
	if ( keylen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;


	return _rtl_crypto_3des_ecb_init(pIE, key, keylen);
}

int rtl_crypto_3des_ecb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;



	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen < 0 ) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;

	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pResult) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	

	return _rtl_crypto_3des_ecb_encrypt(pIE, message, msglen, iv, ivlen, pResult);
}

int rtl_crypto_3des_ecb_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;



	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen < 0 ) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;

	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pResult) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	

	return _rtl_crypto_3des_ecb_decrypt(pIE, message, msglen, iv, ivlen, pResult);
}

//
// DES-CBC
//
int rtl_crypto_des_cbc_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;

	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
	if ( keylen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;


	return _rtl_crypto_des_cbc_init(pIE, key, keylen);
}

int rtl_crypto_des_cbc_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;



	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen < 0 ) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen != pIE->lenCipherKey ) return _ERRNO_CRYPTO_KEY_IV_LEN_DIFF;

	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pResult) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	

	return _rtl_crypto_des_cbc_encrypt(pIE, message, msglen, iv, ivlen, pResult);
}

int rtl_crypto_des_cbc_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;



	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen < 0 ) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen != pIE->lenCipherKey ) return _ERRNO_CRYPTO_KEY_IV_LEN_DIFF;

	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pResult) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	

	return _rtl_crypto_des_cbc_decrypt(pIE, message, msglen, iv, ivlen, pResult);
}


//
// DES-ECB
//

int rtl_crypto_des_ecb_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;


	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
	if ( keylen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;


	return _rtl_crypto_des_ecb_init(pIE, key, keylen);
}

int rtl_crypto_des_ecb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{

	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;

	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen < 0 ) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen != pIE->lenCipherKey ) return _ERRNO_CRYPTO_KEY_IV_LEN_DIFF;

	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pResult) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	

	return _rtl_crypto_des_ecb_encrypt(pIE, message, msglen, iv, ivlen, pResult);
}


int rtl_crypto_des_ecb_decrypt(
	IN const u8* message, 		IN const u32 msglen, 
	IN const u8* iv, 			IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &g_rtl_cipherEngine;



	if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(message) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;
	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen < 0 ) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen != pIE->lenCipherKey ) return _ERRNO_CRYPTO_KEY_IV_LEN_DIFF;

	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	//if ( (u32)(pResult) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	

	return _rtl_crypto_des_ecb_decrypt(pIE, message, msglen, iv, ivlen, pResult);
}

//
// HAL CRYPTO RAM Wrapper functions 
//

// TODO: 
// will modify to see how to do assertion and print out in the future
#define rtl_assert(x) 





/**************************************************************************
 * External Functions in RAM
 **************************************************************************/



void _rtl_cryptoEngine_srcDesc_show(RTL_CRYPTO_SOURCE_T *pSrcDesc)
{
	rtl_assert(pSrcDesc != NULL);

	printk(" OWN: %1x, ", pSrcDesc->own);
	printk(" EOR: %1x, ", pSrcDesc->eor);
	printk(" FS: %1x, ", pSrcDesc->fs);
	printk(" MS: %1x, ", pSrcDesc->ms);
	printk(" KAM: %1x, ", pSrcDesc->kam);
	printk(" AESKL: %1x, ", pSrcDesc->aeskl);
	printk(" 3DES: %1x, ", pSrcDesc->trides);
	printk(" CBC: %1x, ", pSrcDesc->cbc);
	printk(" CTR: %1x, ", pSrcDesc->ctr);
	printk(" HMAC: %1x, ", pSrcDesc->hmac);
	printk(" MD5: %1x, ", (pSrcDesc->md5<<1)|(pSrcDesc->isSHA2));
	printk(" SBL: %4x, ", pSrcDesc->sbl);
	printk(" SHA2: %1x, ", pSrcDesc->sha2);
	printk(" A2EO: %4x, ", pSrcDesc->a2eo);
	printk(" ENL: %4x, ", pSrcDesc->enl);
	printk(" APL: %4x, ", pSrcDesc->apl);
	printk(" SDBP: %p\n", pSrcDesc->sdbp);
	
}


//
// For external functions
// 

int _rtl_cryptoEngine_init(HAL_CRYPTO_ADAPTER* pIE)
{
	return __rtl_cryptoEngine_init(pIE);
}



int _rtl_cryptoEngine(HAL_CRYPTO_ADAPTER *pIE, 
    IN const u8 *message, IN const u32 msglen, IN const u8 *pIv, IN const u32 ivlen,
    IN const u32 a2eo, OUT void *pResult) 
{


	return __rtl_cryptoEngine(pIE, message, msglen, pIv, ivlen, a2eo, pResult);
}



int _rtl_cryptoEngine_set_security_mode(HAL_CRYPTO_ADAPTER *pIE, 
        IN const u32 mode_select, IN const u32 cipher_type, IN const u32 auth_type,
		IN const void* pCipherKey, IN const u32 lenCipherKey, 
        IN const void* pAuthKey, IN const u32 lenAuthKey
        )
{
//	int (*fp)(HAL_CRYPTO_ADAPTER*, const u32, const u32, const u32, const void*, const u32, const void*, const u32);

//	fp = (void*)0x0000427d;
	
//	return fp(pIE, mode_select, cipher_type, auth_type, pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);
	return __rtl_cryptoEngine_set_security_mode(pIE, mode_select, cipher_type, auth_type, 
					pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);	
}



int _rtl_crypto_cipher_init(HAL_CRYPTO_ADAPTER *pIE, 
				IN const u32 cipher_type, 
				IN const u8* key, IN const u32 keylen)
{

	return __rtl_crypto_cipher_init(pIE, cipher_type, key, keylen);
}



int _rtl_crypto_cipher_encrypt(HAL_CRYPTO_ADAPTER *pIE, 
			IN const u32 cipher_type, 	
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* piv, 		IN const u32 ivlen, 
			OUT u8* pResult)
{

	return __rtl_crypto_cipher_encrypt(pIE, cipher_type, message, msglen, piv, ivlen, pResult);
}


int _rtl_crypto_cipher_decrypt(HAL_CRYPTO_ADAPTER *pIE, 
			IN const u32 cipher_type, 	IN const u8* message, 	IN const u32 msglen, 
			IN const u8* piv, 			IN const u32 ivlen, 	
			OUT u8* pResult )
{
	return __rtl_crypto_cipher_decrypt(pIE, cipher_type, message, msglen, piv, ivlen, pResult);
}


void _rtl_cryptoEngine_info(HAL_CRYPTO_ADAPTER* pIE)
{
    int i;
//    char buffer[256];
    u32 desc_num;
    RTL_CRYPTO_SOURCE_T *g_ipssdar;
    RTL_CRYPTO_DEST_T     *g_ipsddar;

    rtl_assert(pIE != NULL);
    
    desc_num = pIE->desc_num;
    g_ipssdar = pIE->g_ipssdar;
    g_ipsddar = pIE->g_ipsddar;

    if ( pIE->isInit == _TRUE ) {
        printk("Crypto Engine has been initialized\n");
    } else {
        printk("Crypto Engine is NOT initialized\n");
    }

    printk("=========================================\n");
    printk("Crypto Engine Registers\n");
    printk("=========================================\n");

    printk(" IPSSDAR : %08x\n", HAL_CRYPTO_READ32(REG_IPSSDAR));
    printk(" IPSDDAR : %08x\n", HAL_CRYPTO_READ32(REG_IPSDDAR));
    printk(" IPSCSR : %08x\n",  HAL_CRYPTO_READ32(REG_IPSCSR));
    printk(" IPSCTR : %08x\n",  HAL_CRYPTO_READ32(REG_IPSCTR));

    printk(" desc_num : %d\n", desc_num);

    for (i=0; i<desc_num; i++)
    {
        printk("ipssdar[%d] : ", i);
        memDump((u8*)(&g_ipssdar[i]), sizeof(g_ipssdar[0]), NULL);
    }

    for (i=0; i<desc_num; i++)
    {
        printk("ipsddar[%d] : ", i);
        memDump((u8*)(&g_ipsddar[i]), sizeof(g_ipsddar[0]), 0);
    }
}







//
// Public Functions
//


int _rtl_crypto_md5_init(HAL_CRYPTO_ADAPTER *pIE)
{
	const u8* pCipherKey 	= NULL;
	const u32 lenCipherKey 	= 0;
	const u8* pAuthKey 		= NULL;
	const u32 lenAuthKey 	= 0;

	rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );
	
	return _rtl_cryptoEngine_set_security_mode(pIE, 
		_MS_TYPE_AUTH, CIPHER_TYPE_NO_CIPHER, AUTH_TYPE_MD5,
			pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);

}


int _rtl_crypto_auth_process(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		OUT u8* pDigest)
{
	int ret;
	const u8* pIV 	= NULL;
	const u32 ivlen = 0;
	const u32 a2eo 	= 0;

	rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );
	
	ret = _rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, a2eo, pDigest);	


	return ret;
}





int _rtl_crypto_sha1_init(HAL_CRYPTO_ADAPTER *pIE)
{
	const u8* pCipherKey 	= NULL;
	const u32 lenCipherKey 	= 0;
	const u8* pAuthKey 		= NULL;
	const u32 lenAuthKey 	= 0;

	rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );
	
	return _rtl_cryptoEngine_set_security_mode(pIE, _MS_TYPE_AUTH, CIPHER_TYPE_NO_CIPHER, AUTH_TYPE_SHA1,
		pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);

}




int _rtl_crypto_sha2_init(HAL_CRYPTO_ADAPTER *pIE, IN const SHA2_TYPE sha2type)
{
	int auth_type;
	const u8* pCipherKey 	= NULL;
	const u32 lenCipherKey 	= 0;
	const u8* pAuthKey 		= NULL;
	const u32 lenAuthKey 	= 0;

	rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );

	auth_type = AUTH_TYPE_SHA2;
	
	switch (sha2type) {
		case SHA2_224 :
			auth_type |= AUTH_TYPE_SHA2_224;
			break;
		case SHA2_256 : 
			auth_type |= AUTH_TYPE_SHA2_256;
			break;
		case SHA2_384 : 
		case SHA2_512 : 
		default: 
			return FAIL;
	}

	return _rtl_cryptoEngine_set_security_mode(pIE, 
			_MS_TYPE_AUTH, CIPHER_TYPE_NO_CIPHER, auth_type,
			pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);

}




int _rtl_crypto_hmac_md5_init(HAL_CRYPTO_ADAPTER *pIE, 
			IN const u8 *key, 	IN const u32 keylen)
{
	u8* pCipherKey = NULL;
	u32 lenCipherKey = 0;


	rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );
		

	return _rtl_cryptoEngine_set_security_mode(pIE, _MS_TYPE_AUTH, CIPHER_TYPE_NO_CIPHER, AUTH_TYPE_HMAC_MD5,
		pCipherKey, lenCipherKey, key, keylen);
		
}




int _rtl_crypto_hmac_sha1_init(HAL_CRYPTO_ADAPTER *pIE, 
			IN const u8 *key, 	IN const u32 keylen)
{
	u8* pCipherKey = NULL;
	u32 lenCipherKey = 0;


	rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );
		

	return _rtl_cryptoEngine_set_security_mode(pIE, _MS_TYPE_AUTH, CIPHER_TYPE_NO_CIPHER, AUTH_TYPE_HMAC_SHA1,
		pCipherKey, lenCipherKey, key, keylen);
		
}





int _rtl_crypto_hmac_sha2_init(HAL_CRYPTO_ADAPTER *pIE, 
			IN const SHA2_TYPE sha2type, 	IN const u8 *key, 	IN const u32 keylen)
{
	int auth_type;
	u8* pCipherKey = NULL;
	u32 lenCipherKey = 0;

	rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );

	auth_type = AUTH_TYPE_HMAC_SHA2;
	
	switch (sha2type) {
		case SHA2_224 :
			auth_type |= AUTH_TYPE_SHA2_224;
			break;
		case SHA2_256 : 
			auth_type |= AUTH_TYPE_SHA2_256;
			break;
		case SHA2_384 : 
		case SHA2_512 : 
		default: 
			return FAIL;
	}

	return _rtl_cryptoEngine_set_security_mode(pIE, 
			_MS_TYPE_AUTH, CIPHER_TYPE_NO_CIPHER, auth_type,
			pCipherKey, lenCipherKey, key, keylen);

}



//
// Cipher Functions
//


int _rtl_crypto_aes_cbc_init(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* key, 	IN const u32 keylen)
{
	return _rtl_crypto_cipher_init(pIE, CIPHER_TYPE_AES_CBC, key, keylen);
}


int _rtl_crypto_aes_cbc_encrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_AES_CBC, message, msglen, iv, ivlen, pResult);
}


int _rtl_crypto_aes_cbc_decrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_AES_CBC, message, msglen, iv, ivlen, pResult);	
}



int _rtl_crypto_aes_ecb_init(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* key, 		IN const u32 keylen)
{
	return _rtl_crypto_cipher_init(pIE, CIPHER_TYPE_AES_ECB, key, keylen);
}


int _rtl_crypto_aes_ecb_encrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_AES_ECB, message, msglen, iv, ivlen, pResult);
}



int _rtl_crypto_aes_ecb_decrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_AES_ECB, message, msglen, iv, ivlen, pResult);	
}



int _rtl_crypto_aes_ctr_init(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* key, 		IN const u32 keylen)
{
	return _rtl_crypto_cipher_init(pIE, CIPHER_TYPE_AES_CTR, key, keylen);
}


int _rtl_crypto_aes_ctr_encrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_AES_CTR, message, msglen, iv, ivlen, pResult);
}



int _rtl_crypto_aes_ctr_decrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	// AES CTR : decrypt is the same as encrypt
	return _rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_AES_CTR, message, msglen, iv, ivlen, pResult);
}


//
// 3DES-CBC
//

int _rtl_crypto_3des_cbc_init(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* key, 		IN const u32 keylen)
{
	return _rtl_crypto_cipher_init(pIE, CIPHER_TYPE_3DES_CBC, key, keylen);
}


int _rtl_crypto_3des_cbc_encrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_3DES_CBC, message, msglen, iv, ivlen, pResult);
}


int _rtl_crypto_3des_cbc_decrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_3DES_CBC, message, msglen, iv, ivlen, pResult);
}


//
// 3DES-ECB
//

int _rtl_crypto_3des_ecb_init(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* key, 		IN const u32 keylen)
{
	return _rtl_crypto_cipher_init(pIE, CIPHER_TYPE_3DES_ECB, key, keylen);
}


int _rtl_crypto_3des_ecb_encrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_3DES_ECB, message, msglen, iv, ivlen, pResult);
}


int _rtl_crypto_3des_ecb_decrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_3DES_ECB, message, msglen, iv, ivlen, pResult);
}

//
// DES-CBC
//

int _rtl_crypto_des_cbc_init(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* key, 		IN const u32 keylen)
{
	return _rtl_crypto_cipher_init(pIE, CIPHER_TYPE_DES_CBC, key, keylen);
}



int _rtl_crypto_des_cbc_encrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_DES_CBC, message, msglen, iv, ivlen, pResult);
}



int _rtl_crypto_des_cbc_decrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_DES_CBC, message, msglen, iv, ivlen, pResult);
}


//
// DES-ECB
//

int _rtl_crypto_des_ecb_init(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* key, 		IN const u32 keylen)
{
	return _rtl_crypto_cipher_init(pIE, CIPHER_TYPE_DES_ECB, key, keylen);
}



int _rtl_crypto_des_ecb_encrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_DES_ECB, message, msglen, iv, ivlen, pResult);
}



int _rtl_crypto_des_ecb_decrypt(HAL_CRYPTO_ADAPTER *pIE, 
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	return _rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_DES_ECB, message, msglen, iv, ivlen, pResult);
}



