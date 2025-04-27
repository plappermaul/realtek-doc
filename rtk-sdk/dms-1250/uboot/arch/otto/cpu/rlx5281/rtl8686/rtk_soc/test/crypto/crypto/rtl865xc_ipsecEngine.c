/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003  
* All rights reserved.
* 
* Program : 865xC crypto engine driver code
* Abstract : 
* $Id: rtl865xc_ipsecEngine.c,v 1.9 2007/03/16 07:30:33 jiucai_wang Exp $
*
* --------------------------------------------------------------------
*/
#include "rtl_types.h"
#include "rtl865xc_ipsecEngine.h"
#include "rtl865xc_authEngine.h"
#include "crypto.h"
#include "md5.h"
#include "sha1.h"
#include "hmac.h"
#ifdef CONFIG_RTL865X_MODEL_KERNEL
//#include "modelTrace.h"
//#include "icExport.h"
//#include "virtualMac.h"
#endif
//#include <linux/autoconf.h>
//#include <asicRegs.h>
#include "asicRegs.h"
//#include "rtl_glue.h"
#include "crypto.h"
//#include <rtl_utils.h>
#include "assert.h"
#include "util.h"

/*  IPSEC Engine debug level*/
#define _IPSEC_ENGINE_DEBUG_ 1

/* Max Scatter List Number*/
#define MAX_SCATTER 8


static uint8 IOPAD[RTL8651B_MAX_MD_CBLOCK*2]; /* buffer for rtl865xc_authEngineInputPad and rtl865xc_authEngineOutputPad */
static uint8 *rtl865xc_authEngineInputPad, *rtl865xc_authEngineOutputPad;
static uint8 _authPadding[HMAC_MAX_MD_CBLOCK*2]; /* padding of packet tail */
static uint8 _rtl865xc_authEngineMd5TempKey[RTL8651B_MD5_DIGEST_LENGTH], _rtl865xc_authEngineSha1TempKey[RTL8651B_SHA1_DIGEST_LENGTH]; /* buffer for *rtl8651b_authEngineMd5TempKey, *rtl8651b_authEngineSha1TempKey */
static uint8 *rtl865xc_authEngineMd5TempKey, *rtl865xc_authEngineSha1TempKey;
static uint8 *pAuthPadding;

enum _MODE_SELECT
{
	_MS_CRYPTO = 0,
	_MS_AUTH = 1,
	_MS_AUTH_THEN_DECRYPT = 2,
	_MS_ENCRYPT_THEN_AUTH = 3,
};

enum _AES_KEYlEN
{
	_AES_KEYLEN_NONE = 0,
	_AES_KEYLEN_128BIT = 1,
	_AES_KEYLEN_192BIT = 2,
	_AES_KEYLEN_256BIT = 3,
};


int32 rtl865xb_ipsecSetOption( enum IPSEC_OPTION option, uint32 value )
{
	switch ( option )
	{
		case IPSOPT_LBKM:
			if ( value == TRUE )
				WRITE_MEM32( IPSCTR, READ_MEM32(IPSCTR)|IPS_LBKM );
			else
				WRITE_MEM32( IPSCTR, READ_MEM32(IPSCTR)&~IPS_LBKM );
			break;
			
		case IPSOPT_SAWB:
			if ( value == TRUE )
				WRITE_MEM32( IPSCTR, READ_MEM32(IPSCTR)|IPS_SAWB);
			else
				WRITE_MEM32( IPSCTR, READ_MEM32(IPSCTR)&~IPS_SAWB );
			break;
			
		case IPSOPT_DMBS:
			if ( value == 16 )
				WRITE_MEM32( IPSCTR, (READ_MEM32(IPSCTR)&~IPS_DMBS_MASK)|IPS_DMBS_16 );
			else if ( value == 32 )
				WRITE_MEM32( IPSCTR, (READ_MEM32(IPSCTR)&~IPS_DMBS_MASK)|IPS_DMBS_32 );
			else if ( value == 64 )
				WRITE_MEM32( IPSCTR, (READ_MEM32(IPSCTR)&~IPS_DMBS_MASK)|IPS_DMBS_64 );
			else
				return FAILED;
			break;
			
		case IPSOPT_SMBS:
			if ( value == 16 )
				WRITE_MEM32( IPSCTR, (READ_MEM32(IPSCTR)&~IPS_SMBS_MASK)|IPS_SMBS_16 );
			else if ( value == 32 )
				WRITE_MEM32( IPSCTR, (READ_MEM32(IPSCTR)&~IPS_SMBS_MASK)|IPS_SMBS_32 );
			else if ( value == 64 )
				WRITE_MEM32( IPSCTR, (READ_MEM32(IPSCTR)&~IPS_SMBS_MASK)|IPS_SMBS_64 );
			else
				return FAILED;
			break;

		default:
			return FAILED;
	}

	return SUCCESS;
}


int32 rtl865xb_ipsecGetOption( enum IPSEC_OPTION option, uint32* value )
{
	switch ( option )
	{
		case IPSOPT_LBKM:
			if ( READ_MEM32(IPSCTR)&IPS_LBKM )
				*value = TRUE;
			else
				*value = FALSE;
			break;

		case IPSOPT_SAWB:
			if ( READ_MEM32(IPSCTR)&IPS_SAWB )
				*value = TRUE ;
			else
				*value = FALSE;
			break;

		case IPSOPT_DMBS:
			if ( (READ_MEM32(IPSCTR)&IPS_DMBS_MASK)==IPS_DMBS_16 )
				*value = 16;
			else if ( (READ_MEM32(IPSCTR)&IPS_DMBS_MASK)==IPS_DMBS_32 )
				*value = 32;
			else if ( (READ_MEM32(IPSCTR)&IPS_DMBS_MASK)==IPS_DMBS_64 )
				*value = 64;
			else
				return FAILED;
			break;

		case IPSOPT_SMBS:
			if ( (READ_MEM32(IPSCTR)&IPS_SMBS_MASK)==IPS_SMBS_16 )
				*value = 16;
			else if ( (READ_MEM32(IPSCTR)&IPS_SMBS_MASK)==IPS_SMBS_32 )
				*value = 32;
			else if ( (READ_MEM32(IPSCTR)&IPS_SMBS_MASK)==IPS_SMBS_64 )
				*value = 64;
			else
				return FAILED;
			break;

		default:
			return FAILED;
	}

	return SUCCESS;
}


/*************************************************************************
 *  To provide the following new features of 865xC, we add the new crypto API.
 *    1. scatter list
 *    2. encryption, then hash
 *    3. hash, then decryption.
 *    4. variant key length for AES
 *
 *  scatter[n].ptr, pKey, pIv and pDigest does NOT have 4-byte alignment limitatiuon.
 *  The unit of lenKey is 'byte'.
 *  modeCrypto:
 *          0x00   0x20
 *          DES  / AES
 *   0x01:  3DES   none
 *   0x02:  ECB    ECB
 *   0x04:  Enc    Enc
 *   0x08:  nBlk   nBlk
 *   0x10:  DMA    DMA
 *
 *   CBC_AES:0x20
 *   ECB_AES:0x22
 *   CTR_AES:0x23
 *************************************************************************/
int32 rtl865x_ipsecEngine( uint32 modeCrypto, uint32 modeAuth, 
                           uint32 cntScatter, rtl865x_cryptoScatter_t *scatter, 
                           uint32 lenCryptoKey, void* pCryptoKey, 
                           uint32 lenAuthKey, void* pAuthKey, 
                           void* pIv, void* pPad, void* pDigest,
                           uint32 a2eo, uint32 enl )
{
	static rtl865xc_crypto_source_t prepSrc[3+MAX_SCATTER]; /* MS=10,11: KeyArray+IvArray+PAD+DataSegs */
	static rtl865xc_crypto_dest_t prepDst[1];
	MD5_CTX md5Context;
	SHA1_CTX sha1Context;
	uint8 keyPresent = 0;
	uint8 ivPresent = 0;
	uint8 padPresent = 0;
	uint8 aplPresent = 0;
	uint32 totalLen = 0;
	uint64 len64;
	uint8 *uint8Ptr = (uint8 *)&len64;
	uint8 *ipad, *opad;
	uint32 apl;
	uint32 hmac = 0; /* for FS=1 crypto key array descriptor */
	uint32 md5 = 0; /* for FS=1 crypto key array descriptor */
	uint32 authPadSpace;
	int i;
	enum _MODE_SELECT modeSelect;
	enum _AES_KEYlEN aesKeyLen;

	memset( prepSrc, 0, sizeof(prepSrc) );
	memset( prepDst, 0, sizeof(prepDst) );

	/* check scatter list count */
	if ( cntScatter == 0 ||cntScatter > MAX_SCATTER )
	{
		#if _IPSEC_ENGINE_DEBUG_>=1
		rtlglue_printf("%s():%d Invalid scatter count.\n", __FUNCTION__, __LINE__ );	
		#endif
		return FAILED;
	}

	/* statistic total length in the scatter list */
	for( i = 0; i < cntScatter; i++ )
	{
		totalLen += scatter[i].len;
		if ( scatter[i].ptr==NULL )
		{
			#if _IPSEC_ENGINE_DEBUG_>=1
			rtlglue_printf("%s():%d Invalid scatter pointer: %p\n", __FUNCTION__, __LINE__, scatter[i].ptr );	
			#endif
			return FAILED;
		}
	}

	if ( a2eo & 3 )
	{
		#if _IPSEC_ENGINE_DEBUG_>=1
		rtlglue_printf("%s():%d A2EO(%u) should be 4-bytes alignment.\n", __FUNCTION__, __LINE__, a2eo );	
		#endif
		return FAILED;
	}

	if( totalLen != a2eo+enl)
	{
		#if _IPSEC_ENGINE_DEBUG_>=1
		rtlglue_printf("%s():%d  Parameters error:totalLen(%d) != a2eo+enl (%d).\n", __FUNCTION__, __LINE__,totalLen, a2eo+enl );	
		#endif
		return FAILED;
	}

	/***************************************
	prepare source data descriptor:

		 ----------
		|Key array   |
		 ----------
		|IV array     |
		 ----------
		|PAD array  |
		 ----------
		|Data 1       |
		|	.          |
		|	.          |
		|	.          |
		|Data N       |
		 ----------
		|HMAC APL  |
		 ----------
	
	*****************************************/

	/* calculate APL */
	if ( modeAuth == _MD_NOAUTH)
	{
		apl = 0;
	}
	else
	{
		authPadSpace = ((totalLen + HMAC_MAX_MD_CBLOCK -1) &(~(HMAC_MAX_MD_CBLOCK -1))) - totalLen;
		if ( authPadSpace > 8 )
		{	/* Yes, we have enough space to store 0x80 and dmaLen */
			apl = authPadSpace;
		}
		else
		{	/* We don't have enough space to store 0x80 and dmaLen. Allocate another 64 bytes. */
			apl = authPadSpace+HMAC_MAX_MD_CBLOCK;
		}
	}

	/* set  authenticate type flag */
	if ( modeCrypto == _MD_NOCRYPTO)
	{
		modeSelect = _MS_AUTH; /* auth only */
	}
	else
	{
		if ( modeAuth == _MD_NOAUTH)
		{
			modeSelect = _MS_CRYPTO; /* Crypto only */
		}
		else
		{
			if ( modeCrypto & _MASK_CRYPTOTHENAUTH )
			{
				modeSelect = _MS_ENCRYPT_THEN_AUTH; /* encrypt, then auth */				
			}
			else
			{
				modeSelect = _MS_AUTH_THEN_DECRYPT; /* auth, then decrypt */
			}
		
			if( modeAuth & _MASK_AUTHHMAC) 
			{	/*HMAC*/
				hmac = 1;
				if( (modeAuth & _MASK_AUTHSHA1MD5)==0 ) 
					md5 = 1; /* MD5 */
			}
		}
	}

/* step 1: prepare Key & IV array: */
	
	if ( modeCrypto != _MD_NOCRYPTO)
	{
		keyPresent = 1;
		ivPresent = 1;
		
		if ( modeCrypto & _MASK_CRYPTOAES)
		{ /* AES */
			if( totalLen == 0 ||
			    totalLen > 16368 ||
			    enl & 0xf)
			{
				#if _IPSEC_ENGINE_DEBUG_>=1
				rtlglue_printf("%s():%d Unable to process non-16-byte aligned data (%d bytes)\n", __FUNCTION__, __LINE__, totalLen-a2eo );
				#endif
				return FAILED;
			}

			switch(lenCryptoKey)
			{
				case 128/8:
					aesKeyLen = _AES_KEYLEN_128BIT;
					break;
				case 192/8:
					aesKeyLen = _AES_KEYLEN_192BIT;
					break;
				case 256/8:
					aesKeyLen = _AES_KEYLEN_256BIT;
					break;
				default:
					#if _IPSEC_ENGINE_DEBUG_>=1
						rtlglue_printf("%s():%d Invalid AES key length: %d bytes.\n", __FUNCTION__, __LINE__, lenCryptoKey );	
					#endif
					return FAILED;
					break;						
			}

			/* AES */
			prepSrc[0].own = 1; /* Own by crypto */
			prepSrc[0].ms = modeSelect; /* Mode Select = AES/DES/3DES */
			prepSrc[0].aeskl = aesKeyLen;
			switch ( modeCrypto & _MASK_CBCECBCTR)
			{
				case _MD_CBC: /* CBC mode */
					prepSrc[0].cbc = 1;
					break;
				case _MD_ECB: /* ECB mode */
					prepSrc[0].cbc = 0;
					break;
				case _MD_CTR: /* Counter mode */
					prepSrc[0].ctr = 1;
					break;
				default:
					#if _IPSEC_ENGINE_DEBUG_>=1
					rtlglue_printf("%s():%d Unsupported crypto mode: %02x\n", __FUNCTION__, __LINE__, modeCrypto );	
					#endif
					return FAILED;
					break;
			}
			if ( modeCrypto & _MASK_CRYPTODECRYPTO)
			{
				prepSrc[0].kam = 7; /* Key Application Mechanism: Encryption */
			}
			else
			{
				prepSrc[0].kam = 0; /* Key Application Mechanism: Decryption */
			}
			prepSrc[0].hmac = hmac;
			prepSrc[0].md5 = md5;

			/* first descriptor: Key Array */
			prepSrc[0].fs = 1;
			prepSrc[0].sbl = lenCryptoKey; /* Key Array Length */
			prepSrc[0].a2eo = a2eo;
			prepSrc[0].enl = enl;
			prepSrc[0].apl = apl;
			prepSrc[0].sdbp = PHYSICAL_ADDRESS( pCryptoKey );

			/* second descriptor: IV Array */
			prepSrc[1].own = 1; /* Own by crypto */
			prepSrc[1].fs = 0;
			prepSrc[1].sbl = 128/8; /* IV Array Length */
			prepSrc[1].a2eo = a2eo;
			prepSrc[1].enl = enl;
			prepSrc[1].apl = apl;
			prepSrc[1].sdbp = PHYSICAL_ADDRESS( pIv );
		}
		else
		{ /* DES/3DES */
			if( totalLen == 0 ||
			    totalLen > 16376 ||
			    enl & 0x7)
			{
				#if _IPSEC_ENGINE_DEBUG_>=1
				rtlglue_printf("%s():%d Unable to process non-8-byte aligned data (%d bytes).\n", __FUNCTION__, __LINE__, totalLen );	
				#endif
				return FAILED;/**/
			}

			/* DES/3DES */
			prepSrc[0].own = 1; /* Own by crypto */
			prepSrc[0].ms = modeSelect; /* Mode Select = AES/DES/3DES */
			prepSrc[0].aeskl = 0; /* DES/3DES */
			if ( modeCrypto & 1 )
			{
				/* 3DES */
				if ( lenCryptoKey != 64/8*3 )
				{
					#if _IPSEC_ENGINE_DEBUG_>=1
					rtlglue_printf("%s():%d Invalid 3DES key length: %d bytes.\n", __FUNCTION__, __LINE__, lenCryptoKey );	
					#endif
					return FAILED;
				}
			
				prepSrc[0].trides = 1;
				prepSrc[0].sbl = 24; /* Key Array Length */
				if ( modeCrypto & _MASK_CRYPTODECRYPTO)
					prepSrc[0].kam = 5; /* Encryption */
				else
				{
					static uint8 _swapped[24];
					uint8* swapped = UNCACHED_ADDRESS( _swapped );
					uint8* pk = pCryptoKey; /* for compiler */

					prepSrc[0].kam = 2; /* Decryption */
					/* for descryption, we need to swap K1 and K3 (K1,K2,K3)==>(K3,K2,K1). */
					memcpy( &swapped[16], &pk[0], 8 );
					memcpy( &swapped[8], &pk[8], 8 );
					memcpy( &swapped[0], &pk[16], 8 );
					pCryptoKey = swapped; /* re-pointer to new key */
				}
			}
			else
			{
				/* DES */
				if ( lenCryptoKey != 64/8 )
				{
					#if _IPSEC_ENGINE_DEBUG_>=1
					rtlglue_printf("%s():%d Invalid DES key length: %d bytes.\n", __FUNCTION__, __LINE__, lenCryptoKey );	
					#endif
					return FAILED;
				}
				
				prepSrc[0].trides = 0;
				prepSrc[0].sbl = 8; /* Key Array Length */
				if ( modeCrypto & _MASK_CRYPTODECRYPTO )
					prepSrc[0].kam = 7; /* Encryption */
				else
					prepSrc[0].kam = 0; /* Decryption */
			}
			
			if ( modeCrypto & _MASK_ECBCBC)
				prepSrc[0].cbc = 0; /* ECB */
			else
				prepSrc[0].cbc = 1; /* CBC */
			prepSrc[0].hmac = hmac;
			prepSrc[0].md5 = md5;
				
			/* first descriptor: Key Array */
			prepSrc[0].fs = 1;
			prepSrc[0].a2eo = a2eo;
			prepSrc[0].enl = enl;
			prepSrc[0].apl = apl;
			prepSrc[0].sdbp = PHYSICAL_ADDRESS( pCryptoKey );
			
			/* second descriptor: IV Array */
			prepSrc[1].own = 1;
			prepSrc[1].fs = 0;
			prepSrc[1].sbl = 8; /* IV Array Length */
			prepSrc[1].a2eo = a2eo;
			prepSrc[1].enl = enl;
			prepSrc[1].apl = apl;
			prepSrc[1].sdbp = PHYSICAL_ADDRESS( pIv );
		}
	}

/* step 2: prepare PAD array: */
	if ( modeAuth!= _MD_NOAUTH)
	{
		/*Calculating ipad and opad*/
		if( modeAuth & _MASK_AUTHHMAC) 
		{	/*HMAC*/
			padPresent = 1;
			
			if( lenAuthKey > HMAC_MAX_MD_CBLOCK) 
			{
				if( modeAuth & _MASK_AUTHSHA1MD5) 
				{ /* SHA1 */
					rtl865xc_authEngineSha1TempKey = UNCACHED_ADDRESS( _rtl865xc_authEngineSha1TempKey );
					
					SHA1Init(&sha1Context);
					SHA1Update(&sha1Context, pAuthKey, lenAuthKey);
					SHA1Final(rtl865xc_authEngineSha1TempKey, &sha1Context);
					pAuthKey = rtl865xc_authEngineSha1TempKey;
					lenAuthKey = SHA_DIGEST_LENGTH;
				}
				else 
				{ /* MD5 */
					rtl865xc_authEngineMd5TempKey = UNCACHED_ADDRESS( _rtl865xc_authEngineMd5TempKey );
					
					MD5Init(&md5Context);
					MD5Update(&md5Context, pAuthKey, lenAuthKey);
					MD5Final(rtl865xc_authEngineMd5TempKey, &md5Context);
					pAuthKey = rtl865xc_authEngineMd5TempKey;
					lenAuthKey = MD5_DIGEST_LENGTH;
				}
			}
			
			if ( modeAuth & RTL8651_AUTH_IOPAD_READY )
			{
				/* ipad and opad is pre-computed. */
				ipad = pPad;
				opad = pPad + HMAC_MAX_MD_CBLOCK;
			}
			else
			{
				rtl865xc_authEngineInputPad = UNCACHED_ADDRESS( &IOPAD[0] );
				rtl865xc_authEngineOutputPad = UNCACHED_ADDRESS( &IOPAD[HMAC_MAX_MD_CBLOCK] );
				memset(rtl865xc_authEngineInputPad, 0x36, HMAC_MAX_MD_CBLOCK);
				memset(rtl865xc_authEngineOutputPad, 0x5c, HMAC_MAX_MD_CBLOCK);
				for(i=0; i<lenAuthKey; i++) 
				{
					rtl865xc_authEngineInputPad[i] ^= ((uint8*)pAuthKey)[i];
					rtl865xc_authEngineOutputPad[i] ^= ((uint8*)pAuthKey)[i];
				}
				ipad = rtl865xc_authEngineInputPad;
				opad = rtl865xc_authEngineOutputPad;
			}

		}
		else
		{	/* HashOnly */
			padPresent = 0;
			ipad = NULL;
			opad = NULL;
		}

		if ( modeSelect==_MS_AUTH ) /* Only when AuthOnly mode, this block is the first block */
			prepSrc[keyPresent+ivPresent].fs = 1;
		else
			prepSrc[keyPresent+ivPresent].fs = 0;
		
		prepSrc[keyPresent+ivPresent].own = 1; /* Own by crypto */
		prepSrc[keyPresent+ivPresent].ms = modeSelect; /* Mode Select = SHA1 or MD5 */

		if ( ( modeAuth & _MASK_AUTHHMAC) == 0 )
			prepSrc[keyPresent+ivPresent].hmac = 0; /* Hash only */
		else
			prepSrc[keyPresent+ivPresent].hmac = 1; /* HMAC */

		if ( ( modeAuth & _MASK_AUTHSHA1MD5) == 0 )
			prepSrc[keyPresent+ivPresent].md5 = 1; /* MD5 */
		else
			prepSrc[keyPresent+ivPresent].md5 = 0; /* SHA1 */

		prepSrc[keyPresent+ivPresent].sbl = 128; /* PAD size */
		prepSrc[keyPresent+ivPresent].a2eo = a2eo;
		prepSrc[keyPresent+ivPresent].enl = enl;
		prepSrc[keyPresent+ivPresent].apl = apl;
		prepSrc[keyPresent+ivPresent].sdbp = PHYSICAL_ADDRESS( ipad );
	}

/* step 3: prepare Data1 ~ DataN */
	for( i = 0; i < cntScatter; i++ )
	{
		prepSrc[keyPresent+ivPresent+padPresent+i].own = 1;
		prepSrc[keyPresent+ivPresent+padPresent+i].fs = 0;
		prepSrc[keyPresent+ivPresent+padPresent+i].sbl = scatter[i].len; /* Data Length */
		prepSrc[keyPresent+ivPresent+padPresent+i].a2eo = a2eo;
		prepSrc[keyPresent+ivPresent+padPresent+i].enl = enl;
		prepSrc[keyPresent+ivPresent+padPresent+i].apl = apl;
		prepSrc[keyPresent+ivPresent+padPresent+i].sdbp = PHYSICAL_ADDRESS( scatter[i].ptr );
	}

/* step 4: prepare padding for MD5 and SHA-1 */
	if(modeAuth != _MD_NOAUTH)
	{	
		aplPresent = 1;
		
		if(modeAuth&_MASK_AUTHHMAC) 
		{	/*HMAC*/
			if( (totalLen+apl) >= 0x3FC0) /* 2**14, MAX_PKTLEN */
			{
				#if _AUTH_DEBUG_ >= 1
				rtlglue_printf("%s():%d dmaLen=%d too long.\n", __FUNCTION__, __LINE__, dmaLen );
				#endif
				return FAILED;/*DMA length larger than 8651b can support*/
			}
		}
		else
		{	/* HashOnly */
			if( (totalLen+apl) > 0x3FFF) /* 2**14, MAX_PKTLEN */
			{
				#if _AUTH_DEBUG_ >= 1
				rtlglue_printf("%s():%d dmaLen=%d too long.\n", __FUNCTION__, __LINE__, dmaLen );
				#endif
				return FAILED;/*DMA length larger than 8651b can support*/
			}
		}

		/* build padding pattern */
		pAuthPadding = UNCACHED_ADDRESS( _authPadding );
		pAuthPadding[0] = 0x80; /* the first byte */
		for( i = 1; i < apl-8; i++ )
			pAuthPadding[i] = 0; /* zero bytes */

		/* final, length bytes */
		if(modeAuth&_MASK_AUTHHMAC) /*HMAC*/
			len64 = (totalLen+64)<<3;/*First padding length is*/
		else
			len64 = totalLen<<3;/*First padding length is*/
		for(i=0; i<8; i++) 
		{
			if(modeAuth&_MASK_AUTHSHA1MD5) /*SHA-1*/
				pAuthPadding[apl-8+i] = uint8Ptr[i];
			else
				pAuthPadding[apl-i-1] = uint8Ptr[i];
		}

		/* append to the last descriptor */
		prepSrc[keyPresent+ivPresent+padPresent+cntScatter].own = 1; /* Own by crypto */
		prepSrc[keyPresent+ivPresent+padPresent+cntScatter].ms = modeSelect; /* Mode Select = SHA1 or MD5 */

		if ( ( modeAuth & _MASK_AUTHHMAC ) == 0 )
			prepSrc[keyPresent+ivPresent+padPresent+cntScatter].hmac = 0; /* Hash only */
		else
			prepSrc[keyPresent+ivPresent+padPresent+cntScatter].hmac = 1; /* HMAC */

		if ( ( modeAuth & _MASK_AUTHSHA1MD5 ) == 0 )
			prepSrc[keyPresent+ivPresent+padPresent+cntScatter].md5 = 1; /* MD5 */
		else
			prepSrc[keyPresent+ivPresent+padPresent+cntScatter].md5 = 0; /* SHA1 */

		prepSrc[keyPresent+ivPresent+padPresent+cntScatter].fs = 0;
		prepSrc[keyPresent+ivPresent+padPresent+cntScatter].sbl = apl; /* PAD size */
		prepSrc[keyPresent+ivPresent+padPresent+cntScatter].a2eo = a2eo;
		prepSrc[keyPresent+ivPresent+padPresent+cntScatter].enl = enl;
		prepSrc[keyPresent+ivPresent+padPresent+cntScatter].apl = apl;
		prepSrc[keyPresent+ivPresent+padPresent+cntScatter].sdbp = PHYSICAL_ADDRESS( pAuthPadding );
	}

/* step 5: write software parameters to ASIC */

	/* We assume the CPU and ASIC are pointed to the same descriptor.
	 * However, in async mode, this assumption is invalid.
	 */
	if ( ( ( modeCrypto!=(uint32)-1) && ( modeCrypto&0x08 ) ) ||
	     ( ( modeAuth!=(uint32)-1) && ( modeAuth&0x08 ) ) )
	{
		/* non-blocking mode, we cannot expect where ASIC is pointing to. */
	}
	else
	{
		/* blocking mode */
		idxAsicSrc = ((rtl865xc_crypto_source_t*)KSEG1_ADDRESS(READ_MEM32( IPSSDAR )))-ipssdar;
		assert( idxAsicSrc == idxCpuSrc );
		idxAsicDst = ((rtl865xc_crypto_dest_t*)KSEG1_ADDRESS(READ_MEM32( IPSDDAR )))-ipsddar;
		assert( idxAsicDst == idxCpuDst );
	}

	/* prepare destination descriptor. */
	prepDst[0].own = 1; /* set owned by ASIC */
	prepDst[0].eor = (idxCpuDst==(numDst-1)); /* If this is the final descriptor, set EOR bit. */
	prepDst[0].dbl = totalLen+apl; /* destination data length */
	prepDst[0].ddbp = PHYSICAL_ADDRESS( scatter[0].ptr ); /* destination data block pointer: the scatter[0].ptr is a continuous buffer if SAWB is disabled. */

	/* write prepared descriptors into ASIC */
	/* 1.destination first */
	memcpy( ((char*)&ipsddar[idxCpuDst])+4, ((char*)&prepDst[0])+4, sizeof(ipsddar[0])-4 ); /* skip first word. */
	*(uint32*)&ipsddar[idxCpuDst] = *(uint32*)&prepDst[0]; /* copy the first word. */
	idxCpuDst = (idxCpuDst+1)%numDst;
	
	/* 2.then source */
	for( i = 0; i < keyPresent+ivPresent+padPresent+cntScatter+aplPresent; i++ )
	{
		if ( ipssdar[(idxCpuSrc+i)%numSrc].own == 0 )
		{
			/* Owned by CPU, overwrite it ! */
			prepSrc[i].eor = (((idxCpuSrc+i)%numSrc)==(numSrc-1)); /* If this is the final descriptor, set EOR bit. */
			memcpy( ((char*)&ipssdar[(idxCpuSrc+i)%numSrc])+4, ((char*)&prepSrc[i])+4, sizeof(ipssdar[0])-4 ); /* skip first word. */
			*(uint32*)&ipssdar[(idxCpuSrc+i)%numSrc] = *(uint32*)&prepSrc[i]; /* copy the first word. */
		}
		else
		{
			/* Owned by ASIC.
			 * Currently, we do not support this situation. 
			 * This means one of following:
			 *  1. numSrc < prepNum
			 *  2. async operation is fired
			 */
			#if _IPSEC_ENGINE_DEBUG_>=1
			rtlglue_printf("%s():%d\n", __FUNCTION__, __LINE__ );	
			#endif
			return FAILED;
		}
	}
	idxCpuSrc = (idxCpuSrc+keyPresent+ivPresent+padPresent+cntScatter+aplPresent)%numSrc;
	
#if _IPSEC_ENGINE_DEBUG_ >= 5
	rtlglue_printf( "numSrc=%d prepNum=%d idxCpuSrc=%d(%d) idxCpuDst=%d(%d)\n", numSrc, keyPresent+ivPresent+padPresent+cntScatter+aplPresent, idxCpuSrc, idxAsicSrc, idxCpuDst, idxAsicDst );
	memDump( (void*)IPSSDAR, 0x10, "Crypto Engine Registers" );
	for( i = 0; i < keyPresent+ivPresent+padPresent+cntScatter+aplPresent; i++ ) memDump( &ipssdar[(idxCpuSrc+numSrc-(keyPresent+ivPresent+padPresent+cntScatter+aplPresent)+i)%numSrc], sizeof(prepSrc[i]), "ipssdar" );
	memDump( &ipsddar[(idxCpuDst+numDst-1)%numDst], sizeof(prepDst), "ipsddar" );
#endif

#ifdef CONFIG_RTL865X_MODEL_KERNEL
	model_setTestTarget( IC_TYPE_REAL );
	modelExportSetIcType( IC_TYPE_REAL );
	modelExportSetOutputForm( EXPORT_ICEMON );
	modelExportCryptoRegisters();
#endif

	/* Clear OK flag */
	WRITE_MEM32( IPSCSR, READ_MEM32( IPSCSR ) | (IPS_SDUEIP|IPS_SDLEIP|IPS_DDUEIP|IPS_DDOKIP|IPS_DABFIP) );

	/* kick off crypto engine */
	WRITE_MEM32( IPSCSR, READ_MEM32(IPSCSR)|IPS_POLL );

	if ( ( ( modeCrypto!=(uint32)-1) && ( modeCrypto&0x08 ) ) ||
	     ( ( modeAuth!=(uint32)-1) && ( modeAuth&0x08 ) ) )
	{
		/* non-blocking mode */
		return SUCCESS;
	}
	else
	{
		/* blocking mode */
		int32 loopWait;

		/* wait until crypto engine stop */
		loopWait = 1000000; /* hope long enough */
		while( ( READ_MEM32( IPSCSR ) & (IPS_SDUEIP|IPS_SDLEIP|IPS_DDUEIP|IPS_DDOKIP|IPS_DABFIP) ) == 0 )
		{
			loopWait--;
			if ( loopWait==0 )
			{
				#if _IPSEC_ENGINE_DEBUG_>=1
				rtlglue_printf("%s():%d Wait Timeout. READ_MEM32(IPSCSR)=0x%08x.\n", __FUNCTION__, __LINE__, READ_MEM32( IPSCSR ) );
				#endif
				return FAILED; /* error occurs */
			}
		}
		assert( ipsddar[(idxCpuDst+numDst-1)%numDst].own==0 ); 

		if ( READ_MEM32( IPSCSR ) & (IPS_SDLEIP|IPS_DABFIP) )
		{
			#if _IPSEC_ENGINE_DEBUG_>=1
			rtlglue_printf("%s():%d IPS_SDLEIP or IPS_DABFI is ON. READ_MEM32(IPSCSR)=0x%08x.\n", __FUNCTION__, __LINE__, READ_MEM32( IPSCSR ) );
			#endif
			return FAILED; /* error occurs */
		}
	}

	if ( modeAuth!=(uint32)-1 )
	{
		#if _IPSEC_ENGINE_DEBUG_ >= 3
		memDump( ipsddar[(idxCpuDst+numDst-1)%numDst].icv, 16, "Digest: ipsddar[(idxCpuDst+numDst-1)%numDst].icv" );
		#endif
		if( modeAuth &0x1 )
			memcpy( pDigest, ipsddar[(idxCpuDst+numDst-1)%numDst].icv, RTL8651B_SHA1_DIGEST_LENGTH);/*Avoid 4-byte alignment limitation*/
		else
			memcpy( pDigest, ipsddar[(idxCpuDst+numDst-1)%numDst].icv, RTL8651B_MD5_DIGEST_LENGTH);/*Avoid 4-byte alignment limitation*/
	}
		
	return SUCCESS;
}

