/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __HAL_CRYPTO_ROM_H__
#define __HAL_CRYPTO_ROM_H__


#include "hal_crypto_ram.h"


#undef __printk
#define __printk DiagPrintf


// Function mapping
#define __rtl_memcmpb			__rtl_memcmpb_v1_00
#define __rtl_random			__rtl_random_v1_00
#define __rtl_align_to_be32 	__rtl_align_to_be32_v1_00
#define __rtl_memsetw 			__rtl_memsetw_v1_00
#define __rtl_memsetb 			__rtl_memsetb_v1_00
#define __rtl_memcpyw			__rtl_memcpyw_v1_00
#define __rtl_memcpyb			__rtl_memcpyb_v1_00
#define __rtl_memDump			__rtl_memDump_v1_00


// static inline function / macros

extern _LONG_CALL_ 
int __rtl_memcmpb_v1_00(const u8 *dst, const u8 *src, int bytes);


extern _LONG_CALL_ 
u32 __rtl_random_v1_00(u32 rtl_seed);


static inline
u32 __rtl_cpu_to_be32(u32 val)
{
	asm volatile(
		"rev %0, %0"
	  : "=r" (val) 
	  : "0" (val));
		
	return val;
}


// Extern API functions with versions

extern _LONG_CALL_ void __rtl_align_to_be32_v1_00(u32 *pData, int bytes4_num );
extern _LONG_CALL_ void __rtl_memsetw_v1_00(u32 *pData, u32 data, int bytes4_num );
extern _LONG_CALL_ void __rtl_memsetb_v1_00(u8 *pData, u8 data, int bytes );
extern _LONG_CALL_ void __rtl_memcpyw_v1_00(u32 *dst, u32 *src, int bytes4_num);
extern _LONG_CALL_ void __rtl_memcpyb_v1_00(u8 *dst, const u8 *src, int bytes);
extern _LONG_CALL_ void __rtl_memDump_v1_00(const u8 *start, u32 size, char * strHeader);





//
// ROM global variables
extern const u32 __AES_Te4[];
extern const u32 __AES_rcon[];


//
// ROM Function mapping
//

#define __rtl_cryptoEngine_init						__rtl_cryptoEngine_init_v1_00
#define __rtl_cryptoEngine_exit						__rtl_cryptoEngine_exit_v1_00
#define __rtl_cryptoEngine_reset					__rtl_cryptoEngine_reset_v1_00


#define __rtl_cryptoEngine_set_security_mode		__rtl_cryptoEngine_set_security_mode_v1_00
#define __rtl_cryptoEngine							__rtl_cryptoEngine_v1_00

#define __rtl_crypto_cipher_init					__rtl_crypto_cipher_init_v1_00
#define __rtl_crypto_cipher_encrypt					__rtl_crypto_cipher_encrypt_v1_00
#define __rtl_crypto_cipher_decrypt					__rtl_crypto_cipher_decrypt_v1_00


//
// ROM functions
//


extern _LONG_CALL_ int __rtl_cryptoEngine_init_v1_00(HAL_CRYPTO_ADAPTER *pIE);

extern _LONG_CALL_ int __rtl_cryptoEngine_exit_v1_00(HAL_CRYPTO_ADAPTER *pIE);

extern _LONG_CALL_ int __rtl_cryptoEngine_reset_v1_00(HAL_CRYPTO_ADAPTER *pIE);



extern _LONG_CALL_ int __rtl_cryptoEngine_set_security_mode_v1_00(HAL_CRYPTO_ADAPTER *pIE, 
        IN const u32 mode_select, IN const u32 cipher_type, IN const u32 auth_type,
		IN const void* pCipherKey, IN const u32 lenCipherKey, 
        IN const void* pAuthKey, IN const u32 lenAuthKey
        );


extern _LONG_CALL_ int __rtl_cryptoEngine_v1_00(HAL_CRYPTO_ADAPTER *pIE, 
    IN const u8 *message, 	IN const u32 msglen, 
    IN const u8 *pIv, 		IN const u32 ivlen,
    IN const u32 a2eo, 		OUT void *pResult);


extern _LONG_CALL_ int __rtl_crypto_cipher_init_v1_00(HAL_CRYPTO_ADAPTER *pIE, 
				IN const u32 cipher_type, 
				IN const u8* key, IN const u32 keylen);

extern _LONG_CALL_ int __rtl_crypto_cipher_encrypt_v1_00(HAL_CRYPTO_ADAPTER *pIE, 
			IN const u32 cipher_type, 	
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* piv, 		IN const u32 ivlen, 
			OUT u8* pResult);


extern _LONG_CALL_ int __rtl_crypto_cipher_decrypt_v1_00(HAL_CRYPTO_ADAPTER *pIE, 
			IN const u32 cipher_type, 	IN const u8* message, 	IN const u32 msglen, 
			IN const u8* piv, 			IN const u32 ivlen, 	
			OUT u8* pResult );


#endif  // __HAL_CRYPTO_ROM_H__

