/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __HAL_CRYPTO_RAM_H__
#define __HAL_CRYPTO_RAM_H__


#include "hal_crypto.h"
#include "diag.h"


#include "rtl_stdlib.h"



#undef printk
#define printk 	DiagPrintf

#undef memDump
#define memDump rtl_memDump


/**************************************************************************
 * Definitions for basic types in crypto
 **************************************************************************/


#ifndef SUCCESS
#define SUCCESS     0
#endif

#ifndef FAILED
#define FAILED -1
#endif

typedef unsigned long long  	uint64;
typedef long long       		int64;
typedef unsigned int    		uint32;
typedef int         			int32;
typedef unsigned short  		uint16;
typedef short           		int16;
typedef unsigned char   		uint8;
typedef char            		int8;

typedef unsigned int			size_t;


#define _LITTLE_ENDIAN

#ifndef assert
#define assert 
#endif



/**************************************************************************
 * Definition for the stdlib which crypto using
 **************************************************************************/



extern int rtl_memcmpb(const u8 *dst, const u8 *src, int bytes);


extern void rtl_srandom( u32 seed );
extern u32 rtl_random( void );


extern int rtl_align_to_be32(u32 *pData, int bytes4_num );

extern void rtl_memDump(const u8 *start, u32 size, char * strHeader);



/**************************************************************************
 * Definition for Parameters
 **************************************************************************/






/**************************************************************************
 * Type definition  for AES KEY
 **************************************************************************/


//
// For AES key calculation
// 

#define CIPHER_AES_MAXNR 14
#define CIPHER_AES_BLOCK_SIZE 16


enum _CIPHER_AES_KEYlEN
{
    _CIPHER_AES_KEYLEN_NONE = 0,
    _CIPHER_AES_KEYLEN_128BIT = 1,
    _CIPHER_AES_KEYLEN_192BIT = 2,
    _CIPHER_AES_KEYLEN_256BIT = 3,
};



struct rtl_sw_aes_key_st {
    u32 rd_key[4 *(CIPHER_AES_MAXNR + 1)] __attribute__ ((aligned(4)));
    int rounds;
};
typedef struct rtl_sw_aes_key_st RTL_SW_AES_KEY;



/**************************************************************************
 * Data Structure for Cipher Adapter
 **************************************************************************/




/**************************************************************************
 * Definition for value and type
 **************************************************************************/


//#define SYSTEM_BASE		0x40000000
//#define CRYPTO_BASE		(SYSTEM_BASE+0x70000)	/* 0x40070000*/
//#define IPSSDAR			(CRYPTO_BASE+0x00)	/* IPSec Source Descriptor Starting Address Register */
//#define IPSDDAR			(CRYPTO_BASE+0x04)	/* IPSec Destination Descriptor Starting Address Register */
//#define IPSCSR			(CRYPTO_BASE+0x08)	/* IPSec Command/Status Register */
//#define IPSCTR			(CRYPTO_BASE+0x0C)	/* IPSec Control Register */

/* IPSec Command/Status Register */
#define IPS_SDUEIP		(1<<15)				/* Source Descriptor Unavailable Error Interrupt Pending */
#define IPS_SDLEIP		(1<<14)				/* Source Descriptor Length Error Interrupt Pending */
#define IPS_DDUEIP		(1<<13)				/* Destination Descriptor Unavailable Error Interrupt Pending */
#define IPS_DDOKIP		(1<<12)				/* Destination Descriptor OK Interrupt Pending */
#define IPS_DABFIP		(1<<11)				/* Data Address Buffer Interrupt Pending */
#define IPS_POLL		(1<<1)				/* Descriptor Polling. Set 1 to kick crypto engine to fetch source descriptor. */
#define IPS_SRST		(1<<0)				/* Software reset, write 1 to reset */

/* IPSec Control Register */
//#define IPS_DDLEN_72	(7<<24)				/* Destination Descriptor Length : address offset to 72 : for SHA-512 */
#define IPS_DDLEN_20	(3<<24)				/* Destination Descriptor Length : Length is 20*DW : for SHA-1/MD5 */
#define IPS_DDLEN_16	(2<<24)				/* Destination Descriptor Length : Length is 16*DW : for SHA-1/MD5 */
#define IPS_DDLEN_12	(1<<24)				/* Destination Descriptor Length : Length is 12*DW : for SHA-1/MD5 */
#define IPS_DDLEN_8		(0<<24)				/* Destination Descriptor Length : Length is 8*DW : for SHA-1/MD5 */

#define IPS_SDUEIE		(1<<15)				/* Source Descriptor Unavailable Error Interrupt Enable */
#define IPS_SDLEIE		(1<<14)				/* Source Descriptor Length Error Interrupt Enable */
#define IPS_DDUEIE		(1<<13)				/* Destination Descriptor Unavailable Error Interrupt Enable */
#define IPS_DDOKIE		(1<<12)				/* Destination Descriptor OK Interrupt Enable */
#define IPS_DABFIE		(1<<11)				/* Data Address Buffer Interrupt Enable */
#define IPS_LXBIG		(1<<9)				/* Lx bus data endian*/
#define IPS_LBKM		(1<<8)				/* Loopback mode enable */
#define IPS_SAWB		(1<<7)				/* Source Address Write Back */
#define IPS_CKE			(1<<6)				/* Clock enable */
#define IPS_DMBS_MASK	(0x7<<3)			/* Mask for Destination DMA Maximum Burst Size */
#define IPS_DMBS_16		(0x0<<3)			/* 16 Bytes */
#define IPS_DMBS_32		(0x1<<3)			/* 32 Bytes */
#define IPS_DMBS_64		(0x2<<3)			/* 64 Bytes */
#define IPS_DMBS_128	(0x3<<3)			/* 128 Bytes */
#define IPS_SMBS_MASK	(0x7<<0)			/* Mask for SourceDMA Maximum Burst Size */
#define IPS_SMBS_16		(0x0<<0)			/* 16 Bytes */
#define IPS_SMBS_32		(0x1<<0)			/* 32 Bytes */
#define IPS_SMBS_64		(0x2<<0)			/* 64 Bytes */
#define IPS_SMBS_128	(0x3<<0)			/* 128 Bytes */



// modeCrypto
#define _MD_NOCRYPTO 			((u32)-1)
#define _MD_CBC					(u32)(0)
#define _MD_ECB					(u32)(2)
#define _MD_CTR					(u32)(3)
#define _MASK_CRYPTOECRYPT		(u32)(1<<7)
#define _MASK_CRYPTOTHENAUTH	(u32)(1<<6)
#define _MASK_CRYPTOAES			(u32)(1<<5)
#define _MASK_CRYPTO3DESDES		(u32)(1<<4)
#define _MASK_CBCECBCTR			((u32)((1<<0)|(1<<1)))
//#define _MASK_ECBCBC			(1<<1)

// modeAuth
#define _MD_NOAUTH				((u32)-1)
#define _MASK_AUTHSHA2			(1<<0)
#define _MASK_AUTHSHA1MD5		(1<<1)
#define _MASK_AUTHHMAC			(1<<2)
#define _MASK_AUTH_IOPAD_READY	(1<<3)

#define MAX_PKTLEN (1<<14)

//Bit 0: 0:DES 1:3DES
//Bit 1: 0:CBC 1:ECB
//Bit 2: 0:Decrypt 1:Encrypt
#define DECRYPT_CBC_DES					0x00
#define DECRYPT_CBC_3DES				0x01
#define DECRYPT_ECB_DES					0x02
#define DECRYPT_ECB_3DES				0x03
#define ENCRYPT_CBC_DES					0x04
#define ENCRYPT_CBC_3DES				0x05
#define ENCRYPT_ECB_DES					0x06
#define ENCRYPT_ECB_3DES				0x07
#define RTL8651_CRYPTO_NON_BLOCKING		0x08
#define RTL8651_CRYPTO_GENERIC_DMA		0x10
#define DECRYPT_CBC_AES					0x20
#define DECRYPT_ECB_AES					0x22
#define DECRYPT_CTR_AES					0x23
#define ENCRYPT_CBC_AES					0x24
#define ENCRYPT_ECB_AES					0x26
#define ENCRYPT_CTR_AES					0x27

//Bit 0: 0:MD5 1:SHA1
//Bit 1: 0:Hash 1:HMAC
#define HASH_MD5		0x00
#define HASH_SHA1		0x01
#define HMAC_MD5		0x02
#define HMAC_SHA1		0x03

#define REG32(reg)				(*((volatile u32 *)(reg)))
#define READ_MEM32(reg)			REG32(reg)
#define WRITE_MEM32(reg,val)	REG32(reg)=(val)

// address macro
#define KUSEG           0x00000000
#define KSEG0           0x80000000
#define KSEG1           0xa0000000
#define KSEG2           0xc0000000
#define KSEG3           0xe0000000

#define CPHYSADDR(a)        ((int)(a) & 0x1fffffff)
#define CKSEG0ADDR(a)       (CPHYSADDR(a) | KSEG0)
#define CKSEG1ADDR(a)       (CPHYSADDR(a) | KSEG1)
#define CKSEG2ADDR(a)       (CPHYSADDR(a) | KSEG2)
#define CKSEG3ADDR(a)       (CPHYSADDR(a) | KSEG3)



enum _MODE_SELECT
{
    _MS_CRYPTO = 0,
    _MS_AUTH = 1,
    _MS_AUTH_THEN_DECRYPT = 2,
    _MS_ENCRYPT_THEN_AUTH = 3,
};




//
// Authentication Functions
//


// MD5
extern int _rtl_crypto_md5_init(void *pIE);



// SHA1
extern int _rtl_crypto_sha1_init(void *pIE);



// SHA2
extern int _rtl_crypto_sha2_init(void *pIE, 
				IN const int sha2type);


// HMAC-MD5
extern int _rtl_crypto_hmac_md5_init(void *pIE, 
				IN const u8 *key, IN const u32 keylen);


// HMAC-SHA1
extern int _rtl_crypto_hmac_sha1_init(void *pIE, 
				IN const u8 *key, IN const u32 keylen);


// HMAC-SHA2
extern int _rtl_crypto_hmac_sha2_init(void *pIE, 
				IN const int sha2type, IN const u8 *key, IN const u32 keylen);




#endif  // __HAL_CRYPTO_RAM_H

