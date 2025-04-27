/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : 
* Abstract :                                                           
* $Id: md5.h,v 1.1 2005/11/01 03:22:38 yjlou Exp $
* $Log: md5.h,v $
* Revision 1.1  2005/11/01 03:22:38  yjlou
* *** empty log message ***
*
* Revision 1.2  2005/09/09 15:04:14  yjlou
* *: Fixed for Kernel Model Code. Including:
*    change printf() to rtlglue_printf()
*    unused variables
*    uninitialized variables
*    blah blah
*
* Revision 1.1  2004/02/25 14:26:33  chhuang
* *** empty log message ***
*
* Revision 1.2  2004/02/25 14:24:52  chhuang
* *** empty log message ***
*
* Revision 1.4  2004/02/18 13:54:37  chhuang
* *** empty log message ***
*
* Revision 1.3  2003/09/30 06:07:50  orlando
* check in RTL8651BLDRV_V20_20030930
*
* Revision 1.4  2003/09/01 03:51:22  jzchen
* Change constant definition
*
* Revision 1.3  2003/01/09 15:01:50  hiwu
* change md5.h
*
* Revision 1.2  2002/12/17 07:22:52  hiwu
* add MD5StringHash()
*
* Revision 1.1  2002/11/11 11:10:33  hiwu
* initial version
*
* --------------------------------------------------------------------
*/
/*	$FreeBSD: src/sys/crypto/md5.h,v 1.1.2.1 2000/07/15 07:14:18 kris Exp $	*/
/*	$KAME: md5.h,v 1.4 2000/03/27 04:36:22 sumikawa Exp $	*/

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

#ifndef _CRYPTO_MD5_H_
#define _CRYPTO_MD5_H_

#include "crypto.h"
#include "rtl_types.h"


#define MD5_BUFLEN	64
#define MD5_DIGEST_LENGTH 16

typedef struct {
	union { 
		uint32	md5_state32[4];
		uint8	md5_state8[16];
	} md5_st;
#define md5_sta		md5_st.md5_state32[0]
#define md5_stb		md5_st.md5_state32[1]
#define md5_stc		md5_st.md5_state32[2]
#define md5_std		md5_st.md5_state32[3]
#define md5_st8		md5_st.md5_state8

	union { 
		uint64_t md5_count64;
		uint8_t	md5_count8[8];
	} md5_count;

#define md5_n	md5_count.md5_count64
#define md5_n8	md5_count.md5_count8

	uint32	md5_i;
	uint8	md5_buf[MD5_BUFLEN];
} md5_ctxt;

extern void md5_init (md5_ctxt *);
extern void md5_loop (md5_ctxt *, uint8 *, uint32);
extern void md5_pad (md5_ctxt *);
extern void md5_result (uint8 *, md5_ctxt *);

/* compatibility */
#define MD5_CTX		md5_ctxt
#define MD5Init(x)	md5_init((x))
#define MD5Update(x, y, z)	md5_loop((x), (y), (z))
#define MD5Final(x, y) \
do {				\
	md5_pad((y));		\
	md5_result((x), (y));	\
} while (0)

void MD5StringHash (uint8* digest, const int8* str);

#endif /* ! _CRYPTO_MD5_H_*/
