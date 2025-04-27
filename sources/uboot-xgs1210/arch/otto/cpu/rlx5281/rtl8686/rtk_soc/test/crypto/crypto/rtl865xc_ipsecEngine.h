/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003  
* All rights reserved.
* 
* Program : 8651B crypto engine driver header
* Abstract : 
* $Id: rtl865xc_ipsecEngine.h,v 1.4 2007/03/16 07:31:37 jiucai_wang Exp $
*/
#ifndef RTL865XC_IPSECENGINE_H
#define RTL865XC_IPSECENGINE_H

#include "rtl_types.h"
#include "crypto.h"

/*MICRO definition*/
#define _MD_NOCRYPTO 	((uint32)-1)
#define _MD_CBC			0
#define _MD_ECB			(1<<1)
#define _MD_CTR			((1<<0)|(1<<1))
#define _MASK_CRYPTOTHENAUTH		(1<<2)
#define _MASK_CRYPTOAES			(1<<5)
#define _MASK_CRYPTODECRYPTO		(1<<2)
#define _MASK_CRYPTO3DESDES		(1<<0)
#define _MASK_CBCECBCTR			((1<<0)|(1<<1))
#define _MASK_ECBCBC				(1<<1)

#define _MD_NOAUTH		((uint32)-1)
#define _MASK_AUTHSHA1MD5			(1<<0)
#define _MASK_AUTHHMAC				(1<<1)

/*
 *  From 865xC, crypto engine supports scatter list: your data can be stored in several segments those are not continuous.
 *  Each scatter points to one segment of data.
 */
struct rtl865x_cryptoScatter_s
{
	int32 len;
	void* ptr;
};
typedef struct rtl865x_cryptoScatter_s rtl865x_cryptoScatter_t;


enum IPSEC_OPTION
{
	IPSOPT_LBKM, /* loopback mode */
	IPSOPT_SAWB, /* Source Address Write Back */
	IPSOPT_DMBS, /* Dest Max Burst Size */
	IPSOPT_SMBS, /* Source Max Burst Size */
};

int32 rtl865xb_ipsecSetOption( enum IPSEC_OPTION option, uint32 value );
int32 rtl865xb_ipsecGetOption( enum IPSEC_OPTION option, uint32* value );

int32 rtl865x_ipsecEngine( uint32 modeCrypto, uint32 modeAuth, 
                           uint32 cntScatter, rtl865x_cryptoScatter_t *scatter, 
                           uint32 lenCryptoKey, void* pCryptoKey, 
                           uint32 lenAuthKey, void* pAuthKey, 
                           void* pIv, void* pPad, void* pDigest,
                           uint32 a2eo, uint32 enl );
#endif
