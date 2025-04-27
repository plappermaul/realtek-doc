/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header file for crypto model code
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: cryptoModel.h,v 1.2 2006-07-13 15:58:51 chenyl Exp $
*/

#include "rtl_types.h"

#ifndef _CRYPTO_MODEL_
#define _CRYPTO_MODEL_


struct src_crypto_desc_s
{
	/* Word 0 */
#ifndef _LITTLE_ENDIAN
	uint32 own:1;	/* 1-Crypto own. */
	uint32 eor:1;	/* End Of Ring */
	uint32 fs:1;	/* First Segment */
	uint32 resv:1;
	uint32 mode:2;	/* mode select
	                 * 00-crypto only
	                 * 01-hash only
	                 * 10-hash, then crypto
	                 * 11-crypto, then hash
	                 */
	uint32 kam:3;	/* Key Application Management
	                 * 000-Descrypt with K1,K2,K3
	                 * 010-Descrypt with K1, encrypt with K2, decrypt with K3
	                 * 101-Encrypt with K1, descrypt with K2, encrypt with K3
	                 * 111-Encrypt with K1,K2,K3
	                 */
	uint32 aeskl:2;	/* AES Key Length
	                 * 00-Disable AES
	                 * 01-128 bits
	                 * 10-192 bits
	                 * 11-256 bits
	                 */
	uint32 triDES:1;/* 3DES */
	uint32 cbc:1;	/* CBC mode */
	uint32 ctr:1;	/* Counter mode */
	uint32 hmac:1;	/* HMAC */
	uint32 md5:1;	/* MD5 */
	uint32 resv2:2;
	uint32 sbl:14;	/* Source Buffer Length */
#else
	uint32 sbl:14;
	uint32 resv2:2;
	uint32 md5:1;
	uint32 hmac:1;
	uint32 ctr:1;
	uint32 cbc:1;
	uint32 triDES:1;
	uint32 aeskl:2;
	uint32 kam:3;
	uint32 mode:2;
	uint32 resv:1;
	uint32 fs:1;
	uint32 eor:1;
	uint32 own:1;
#endif

	/* Word 1 */
#ifndef _LITTLE_ENDIAN
	uint32 resv3:8;
	uint32 a2eo:8;	/* Authentication to encryption offset */
	uint32 resv4:2;
	uint32 enl:14;	/* encryption data length */
#else
	uint32 enl:14;
	uint32 resv4:2;
	uint32 a2eo:8;
	uint32 resv3:8;
#endif

	/* Word 2 */
#ifndef _LITTLE_ENDIAN
	uint32 resv5:8;
	uint32 apl:8;	/* Authentication padding length */
	uint32 resv6:16;
#else
	uint32 resv6:16;
	uint32 apl:8;
	uint32 resv5:8;
#endif

	/* Word 3 */
	void *sdbp;		/* Source Data Buffer Pointer */
};
typedef struct src_crypto_desc_s src_crypto_desc_t;


struct dst_crypto_desc_s
{
	/* Word 0 */
#ifndef _LITTLE_ENDIAN
	uint32 own:1;	/* 1-crypto own */
	uint32 eor:1;	/* end of ring */
	uint32 resv1:16;
	uint32 dbl:14;	/* Data Buffer Length */
#else
	uint32 dbl:14;
	uint32 resv1:16;
	uint32 eor:1;
	uint32 own:1;
#endif

	/* Word 1 */
	void *ddbp;		/* Destination Data Buffer Pointer */

	/* Word 2 */
	uint32 resv2:32;

	/* Word 3,4,5,6,7 */
	uint32 ICV[5];
};
typedef struct dst_crypto_desc_s dst_crypto_desc_t;


#endif/*_CRYPTO_MODEL_*/

