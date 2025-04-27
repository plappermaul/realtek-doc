/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : 
* Abstract :                                                           
* $Id: sha1.h,v 1.1 2005/11/01 03:22:38 yjlou Exp $
* $Log: sha1.h,v $
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
* Revision 1.2  2003/09/01 03:51:07  jzchen
* Change sha1_result parameter definition and constant definition
*
* Revision 1.1  2002/11/11 11:10:33  hiwu
* initial version
*
* --------------------------------------------------------------------
*/
/*	$FreeBSD: src/sys/crypto/sha1.h,v 1.3.2.3 2000/10/12 18:59:31 archie Exp $	*/
/*	$KAME: sha1.h,v 1.5 2000/03/27 04:36:23 sumikawa Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 * FIPS pub 180-1: Secure Hash Algorithm (SHA-1)
 * based on: http://csrc.nist.gov/fips/fip180-1.txt
 * implemented by Jun-ichiro itojun Itoh <itojun@itojun.org>
 */

#ifndef _CRYPTO_SHA1_H_
#define _CRYPTO_SHA1_H_

#include "rtl_types.h"
#include "crypto.h"


struct sha1_ctxt {
	union {
		uint8	b8[20];
		uint32	b32[5];
	} h;
	union {
		uint8	b8[8];
		uint64	b64[1];
	} c;
	union {
		uint8	b8[64];
		uint32	b32[16];
	} m;
	uint8	count;
};

extern void sha1_init (struct sha1_ctxt *);
extern void sha1_pad (struct sha1_ctxt *);
extern void sha1_loop (struct sha1_ctxt *, const uint8 *, uint32);
extern void sha1_result (struct sha1_ctxt *, uint8*);

/* compatibilty with other SHA1 source codes */
typedef struct sha1_ctxt SHA1_CTX;
#define SHA1Init(x)		sha1_init((x))
#define SHA1Update(x, y, z)	sha1_loop((x), (y), (z))
#define SHA1Final(x, y)		sha1_result((y), (x))

#define	SHA1_RESULTLEN	(160/8)
#define SHA_DIGEST_LENGTH 20

#endif /*_CRYPTO_SHA1_H_*/
