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


/**************************************************************************
 * definition
 **************************************************************************/

//
#define CRYPTO_MAX_DESP 			16
#define CRYPTO_MAX_DIGEST_LENGTH	32  // SHA256 Digest length : 32
#define CRYPTO_MAX_KEY_LENGTH		32  // MAX  is  AES-256 : 32 byte,  3DES : 24 byte
#define CRYPTO_MAX_AUTH_KEY_LENGTH		64
//SHA-256, this is 512 bits(64byte), for SHA-384 and SHA-512, this is 1024 bits(128byte)
#define CRYPTO_PADSIZE 				64

#define CRYPTO_MD5_DIGEST_LENGTH 	16
#define CRYPTO_SHA1_DIGEST_LENGTH 	20
#define CRYPTO_SHA2_DIGEST_LENGTH 	32


//
#define _ERRNO_CRYPTO_DESC_NUM_SET_OutRange 		-2
#define _ERRNO_CRYPTO_BURST_NUM_SET_OutRange		-3
#define _ERRNO_CRYPTO_NULL_POINTER					-4
#define _ERRNO_CRYPTO_ENGINE_NOT_INIT				-5
#define _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned		-6
#define _ERRNO_CRYPTO_KEY_OutRange					-7
#define _ERRNO_CRYPTO_MSG_OutRange					-8
#define _ERRNO_CRYPTO_IV_OutRange					-9
#define _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH			-10
#define _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH 		-11
#define _ERRNO_CRYPTO_KEY_IV_LEN_DIFF				-12
#define _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned	-13



// Mode Select
#define CRYPTO_MS_TYPE_CIPHER	(0)
#define CRYPTO_MS_TYPE_AUTH     (1)
#define CRYPTO_MS_TYPE_MIX1_sshenc_espdec  	(2)
#define CRYPTO_MS_TYPE_MIX2_sshdec_espenc  	(3)
#define CRYPTO_MS_TYPE_MIX3_sslenc  	(4)


//
// AUTH Type
//

#define AUTH_TYPE_NO_AUTH 		((u32)-1)

#define AUTH_TYPE_MASK_FUNC		0x3	// bit 0, bit 1
#define AUTH_TYPE_MD5			0x2
#define AUTH_TYPE_SHA1			0x0
#define AUTH_TYPE_SHA2			0x1
	
#define AUTH_TYPE_MASK_HMAC 	0x4	// bit 2
#define AUTH_TYPE_HMAC_MD5 		(AUTH_TYPE_MD5 | AUTH_TYPE_MASK_HMAC)
#define AUTH_TYPE_HMAC_SHA1 	(AUTH_TYPE_SHA1 | AUTH_TYPE_MASK_HMAC)
#define AUTH_TYPE_HMAC_SHA2 	(AUTH_TYPE_SHA2 | AUTH_TYPE_MASK_HMAC)

// SHA2

#define AUTH_TYPE_MASK_SHA2 	0x30 // bit 3,4
#define AUTH_TYPE_SHA2_224		0x10
#define AUTH_TYPE_SHA2_256  	0x20
#define AUTH_TYPE_SHA2_384  	0x30 
#define AUTH_TYPE_SHA2_512  	0x00

#define SHA2_NONE	0
#define SHA2_224	(224/8)
#define SHA2_256	(256/8)
#define SHA2_384	(384/8)
#define SHA2_512	(512/8)



// 
// Cipher Type
//

#define CIPHER_TYPE_NO_CIPHER 		((u32)-1)


#define CIPHER_TYPE_MODE_ENCRYPT 	0x80

#define CIPHER_TYPE_MASK_FUNC 		0x30 	// 0x00 : DES, 0x10: 3DES, 0x20: AES
#define CIPHER_TYPE_FUNC_DES		0x00
#define CIPHER_TYPE_FUNC_3DES 		0x10
#define CIPHER_TYPE_FUNC_AES 		0x20
#define CIPHER_TYPE_FUNC_CHACHA 	0x30
	
#define CIPHER_TYPE_MASK_BLOCK 		0xF 	// 0x0 : ECB, 0x1: CBC, 0x2: CFB , 0x3 : OFB , 0x4 : CTR, 0x5 : GCTR, 0x6: GMAC, 0x7: GHASH, 0x8: GCM
#define CIPHER_TYPE_BLOCK_ECB 		0x0
#define CIPHER_TYPE_BLOCK_CBC 		0x1
#define CIPHER_TYPE_BLOCK_CFB 		0x2
#define CIPHER_TYPE_BLOCK_OFB 		0x3
#define CIPHER_TYPE_BLOCK_CTR 		0x4 
#define CIPHER_TYPE_BLOCK_GCTR 		0x5
#define CIPHER_TYPE_BLOCK_GMAC 		0x6
#define CIPHER_TYPE_BLOCK_GHASH 	0x7
#define CIPHER_TYPE_BLOCK_GCM 		0x8

#define CIPHER_TYPE_BLOCK_CHACHA 	0x1



/**************************************************************************
 * typedef structures
 **************************************************************************/





typedef struct rtl_crypto_cl_struct_s {
    // offset 0 : 
    u32 cipher_mode:4;
    u32 cipher_eng_sel:2;
	u32 rsvd1:1;
    u32 cipher_encrypt:1;
    u32 aes_key_sel:2;
	u32 des3_en:1;
	u32 des3_type:1;
	u32 ckbs:1;
	u32 hmac_en:1;
	u32 hmac_mode:3;
	u32 hmac_seq_hash_last:1;
	u32 engine_mode:3;
	u32 hmac_seq_hash_first:1;
	u32 hmac_seq_hash:1;
	u32 hmac_seq_hash_no_wb:1;
	u32 icv_total_length:8;

	// offset 4
	u32 aad_last_data_size:4;
	u32 enc_last_data_size:4;
	u32 pad_last_data_size:3;
	u32 ckws:1;
	u32 enc_pad_last_data_size:3;
	u32 hsibs:1;
	u32 caws:1;
	u32 cabs:1;
	u32 ciws:1;
	u32 cibs:1;
	u32 cows:1;
	u32 cobs:1;
	u32 codws:1;
	u32 cidws:1;
	u32 haws:1;
	u32 habs:1;
	u32 hiws:1;
	u32 hibs:1;
	u32 hows:1;
	u32 hobs:1;
	u32 hkws:1;
	u32 hkbs:1;

	// offset 8
	u32 hash_pad_len:8;
	u32 header_total_len:6;
	u32 ssl_apl:2;
	u32 enl:16;	

	// offset 
	u32 ap0;
	u32 ap1;
	u32 ap2;
	u32 ap3;
}rtl_crypto_cl_t;







typedef struct hal_crypto_adapter_s
{
    u8 isInit;  // 0: not init, 1: init

    u8 dma_burst_num; // 0 ~ 32
    
    u8 desc_num;  // total number of source/destination description
    
    u8 mode_select;
	
    u32 cipher_type;
	u8 des;
	u8 trides;
	u8 aes;
	u8 chacha;
	u8 isDecrypt;
	
    u32 auth_type;
    u8 isHMAC;
    u8 isMD5;
	u8 isSHA1;
	u8 isSHA2;
	u8 sha2type;

	// hash
	u32 enc_last_data_size;
	u32 aad_last_data_size;
	u32 pad_last_data_size;

	//ssl
	u32 data_padding_len;
	u32 crypto_block_size;
	u32 enc_pad_last_data_size;
	
    u32 lenAuthKey;
    const u8*  pAuthKey;
	u8  digest_data[CRYPTO_MAX_DIGEST_LENGTH];
	u32 digestlen;

	// sequential hash
	u8 hmac_seq_hash_first;
	u8 hmac_seq_hash_last;
	u32 hmac_seq_hash_total_len;
	
    u32 	lenCipherKey;
    const u8*  	pCipherKey;

	u32 a2eo, apl_aad, enl, apl, hash_apl; 


    u8 g_IOPAD[CRYPTO_PADSIZE*2+32];		
    u8 *ipad;
	u8 *opad;

	u8 g_authPadding[128*2];

#if 0    
    // Source Descriptor
    u8 idx_srcdesc;
    rtl_crypto_source_t *g_ipssdar;	
	rtl_crypto_source_t g_src_buf[CRYPTO_MAX_DESP];

    // Destination Descriptor
    u8 idx_dstdesc;
    rtl_crypto_dest_t *g_ipsddar;	
	rtl_crypto_dest_t g_dst_buf[CRYPTO_MAX_DESP];
#endif    
} hal_crypto_adapter_t, *phal_crypto_adapter_t ;




#undef __printk
#define __printk DiagPrintf




// static inline function / macros

extern _LONG_CALL_ 
int __rtl_memcmpb(const u8 *dst, const u8 *src, int bytes);


extern _LONG_CALL_ 
u32 __rtl_random(u32 rtl_seed);


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

extern _LONG_CALL_ void __rtl_align_to_be32(u32 *pData, int bytes4_num );
extern _LONG_CALL_ void __rtl_memDump(const u8 *start, u32 size, char * strHeader);






//
// ROM functions
//


extern _LONG_CALL_ int __rtl_cryptoEngine_init(hal_crypto_adapter_t *pIE);

extern _LONG_CALL_ int __rtl_cryptoEngine_exit(hal_crypto_adapter_t *pIE);

extern _LONG_CALL_ int __rtl_cryptoEngine_reset(hal_crypto_adapter_t *pIE);



extern _LONG_CALL_ int __rtl_cryptoEngine_set_security_mode(hal_crypto_adapter_t *pIE, 
        IN const u32 mode_select, IN const u32 cipher_type, IN const u32 auth_type,
		IN const void* pCipherKey, IN const u32 lenCipherKey, 
        IN const void* pAuthKey, IN const u32 lenAuthKey
        );


extern _LONG_CALL_ int __rtl_cryptoEngine(hal_crypto_adapter_t *pIE, 
    IN const u8 *message, 	IN const u32 msglen, 
    IN const u8 *pIv, 		IN const u32 ivlen,
    IN const u8 *paad,		IN const u32 a2eo, 
    OUT void *pResult, OUT void *pTag);


extern _LONG_CALL_ int __rtl_crypto_cipher_init(hal_crypto_adapter_t *pIE, 
				IN const u32 cipher_type, 
				IN const u8* key, IN const u32 keylen);

extern _LONG_CALL_ int __rtl_crypto_cipher_encrypt(hal_crypto_adapter_t *pIE, 
			IN const u32 cipher_type, 	
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* piv, 		IN const u32 ivlen, 
			IN const u8* paad,		IN const u32 aadlen, 
			OUT u8* pResult, OUT u8* pTag);


extern _LONG_CALL_ int __rtl_crypto_cipher_decrypt(hal_crypto_adapter_t *pIE, 
			IN const u32 cipher_type, 	IN const u8* message, 	IN const u32 msglen, 
			IN const u8* piv, 			IN const u32 ivlen, 	
			IN const u8* paad,			IN const u32 aadlen, 	
			OUT u8* pResult, OUT u8* pTag );


extern _LONG_CALL_ int __rtl_crypto_crc32(hal_crypto_adapter_t *pIE, 
	IN const u8* message,	IN const u32 msglen, 
	OUT u32* pCrc);

extern _LONG_CALL_
int __rtl_crypto_crc_setting(
	hal_crypto_adapter_t *pIE, 
	IN int order, IN u32 polynom, IN u8 direct, IN u32 crcinit, IN u32 crcxor, IN u32 refin, IN u32 refout);

extern _LONG_CALL_
int __rtl_crypto_crc(
	hal_crypto_adapter_t *pIE, 
	IN const u8* message,	IN const u32 msglen, 
	OUT u32* pCrc);


#endif  // __HAL_CRYPTO_ROM_H__

