/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : 
* Abstract : Porting openssl testing code and use it to test both software des function and asic des co-processor
* $Id: destest.c,v 1.6 2006/02/13 12:36:09 yjlou Exp $
*/
/* crypto/des/destest.c */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 * 
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 * 
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from 
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 * 
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#include "des.h"
#include "destest.h"
#include "desSim.h"
#include "rtl865xc_cryptoEngine.h"
#include "asicRegs.h"
//#include "rtl_types.h"
//#include <rtl_glue.h>

/* tisk tisk - the test keys don't all have odd parity :-( */
/* test data */
#define NUM_TESTS 34
static int8 key_data[NUM_TESTS][8]={
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
	{0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11},
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF},
	{0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11},
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10},
	{0x7C,0xA1,0x10,0x45,0x4A,0x1A,0x6E,0x57},
	{0x01,0x31,0xD9,0x61,0x9D,0xC1,0x37,0x6E},
	{0x07,0xA1,0x13,0x3E,0x4A,0x0B,0x26,0x86},
	{0x38,0x49,0x67,0x4C,0x26,0x02,0x31,0x9E},
	{0x04,0xB9,0x15,0xBA,0x43,0xFE,0xB5,0xB6},
	{0x01,0x13,0xB9,0x70,0xFD,0x34,0xF2,0xCE},
	{0x01,0x70,0xF1,0x75,0x46,0x8F,0xB5,0xE6},
	{0x43,0x29,0x7F,0xAD,0x38,0xE3,0x73,0xFE},
	{0x07,0xA7,0x13,0x70,0x45,0xDA,0x2A,0x16},
	{0x04,0x68,0x91,0x04,0xC2,0xFD,0x3B,0x2F},
	{0x37,0xD0,0x6B,0xB5,0x16,0xCB,0x75,0x46},
	{0x1F,0x08,0x26,0x0D,0x1A,0xC2,0x46,0x5E},
	{0x58,0x40,0x23,0x64,0x1A,0xBA,0x61,0x76},
	{0x02,0x58,0x16,0x16,0x46,0x29,0xB0,0x07},
	{0x49,0x79,0x3E,0xBC,0x79,0xB3,0x25,0x8F},
	{0x4F,0xB0,0x5E,0x15,0x15,0xAB,0x73,0xA7},
	{0x49,0xE9,0x5D,0x6D,0x4C,0xA2,0x29,0xBF},
	{0x01,0x83,0x10,0xDC,0x40,0x9B,0x26,0xD6},
	{0x1C,0x58,0x7F,0x1C,0x13,0x92,0x4F,0xEF},
	{0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01},
	{0x1F,0x1F,0x1F,0x1F,0x0E,0x0E,0x0E,0x0E},
	{0xE0,0xFE,0xE0,0xFE,0xF1,0xFE,0xF1,0xFE},
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF},
	{0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10}};

static int8 plain_data[NUM_TESTS][8]={
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
	{0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
	{0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11},
	{0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11},
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF},
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF},
	{0x01,0xA1,0xD6,0xD0,0x39,0x77,0x67,0x42},
	{0x5C,0xD5,0x4C,0xA8,0x3D,0xEF,0x57,0xDA},
	{0x02,0x48,0xD4,0x38,0x06,0xF6,0x71,0x72},
	{0x51,0x45,0x4B,0x58,0x2D,0xDF,0x44,0x0A},
	{0x42,0xFD,0x44,0x30,0x59,0x57,0x7F,0xA2},
	{0x05,0x9B,0x5E,0x08,0x51,0xCF,0x14,0x3A},
	{0x07,0x56,0xD8,0xE0,0x77,0x47,0x61,0xD2},
	{0x76,0x25,0x14,0xB8,0x29,0xBF,0x48,0x6A},
	{0x3B,0xDD,0x11,0x90,0x49,0x37,0x28,0x02},
	{0x26,0x95,0x5F,0x68,0x35,0xAF,0x60,0x9A},
	{0x16,0x4D,0x5E,0x40,0x4F,0x27,0x52,0x32},
	{0x6B,0x05,0x6E,0x18,0x75,0x9F,0x5C,0xCA},
	{0x00,0x4B,0xD6,0xEF,0x09,0x17,0x60,0x62},
	{0x48,0x0D,0x39,0x00,0x6E,0xE7,0x62,0xF2},
	{0x43,0x75,0x40,0xC8,0x69,0x8F,0x3C,0xFA},
	{0x07,0x2D,0x43,0xA0,0x77,0x07,0x52,0x92},
	{0x02,0xFE,0x55,0x77,0x81,0x17,0xF1,0x2A},
	{0x1D,0x9D,0x5C,0x50,0x18,0xF7,0x28,0xC2},
	{0x30,0x55,0x32,0x28,0x6D,0x6F,0x29,0x5A},
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF},
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF},
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF},
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}};

static int8 cipher_data[NUM_TESTS][8]={
	{0x8C,0xA6,0x4D,0xE9,0xC1,0xB1,0x23,0xA7},
	{0x73,0x59,0xB2,0x16,0x3E,0x4E,0xDC,0x58},
	{0x95,0x8E,0x6E,0x62,0x7A,0x05,0x55,0x7B},
	{0xF4,0x03,0x79,0xAB,0x9E,0x0E,0xC5,0x33},
	{0x17,0x66,0x8D,0xFC,0x72,0x92,0x53,0x2D},
	{0x8A,0x5A,0xE1,0xF8,0x1A,0xB8,0xF2,0xDD},
	{0x8C,0xA6,0x4D,0xE9,0xC1,0xB1,0x23,0xA7},
	{0xED,0x39,0xD9,0x50,0xFA,0x74,0xBC,0xC4},
	{0x69,0x0F,0x5B,0x0D,0x9A,0x26,0x93,0x9B},
	{0x7A,0x38,0x9D,0x10,0x35,0x4B,0xD2,0x71},
	{0x86,0x8E,0xBB,0x51,0xCA,0xB4,0x59,0x9A},
	{0x71,0x78,0x87,0x6E,0x01,0xF1,0x9B,0x2A},
	{0xAF,0x37,0xFB,0x42,0x1F,0x8C,0x40,0x95},
	{0x86,0xA5,0x60,0xF1,0x0E,0xC6,0xD8,0x5B},
	{0x0C,0xD3,0xDA,0x02,0x00,0x21,0xDC,0x09},
	{0xEA,0x67,0x6B,0x2C,0xB7,0xDB,0x2B,0x7A},
	{0xDF,0xD6,0x4A,0x81,0x5C,0xAF,0x1A,0x0F},
	{0x5C,0x51,0x3C,0x9C,0x48,0x86,0xC0,0x88},
	{0x0A,0x2A,0xEE,0xAE,0x3F,0xF4,0xAB,0x77},
	{0xEF,0x1B,0xF0,0x3E,0x5D,0xFA,0x57,0x5A},
	{0x88,0xBF,0x0D,0xB6,0xD7,0x0D,0xEE,0x56},
	{0xA1,0xF9,0x91,0x55,0x41,0x02,0x0B,0x56},
	{0x6F,0xBF,0x1C,0xAF,0xCF,0xFD,0x05,0x56},
	{0x2F,0x22,0xE4,0x9B,0xAB,0x7C,0xA1,0xAC},
	{0x5A,0x6B,0x61,0x2C,0xC2,0x6C,0xCE,0x4A},
	{0x5F,0x4C,0x03,0x8E,0xD1,0x2B,0x2E,0x41},
	{0x63,0xFA,0xC0,0xD0,0x34,0xD9,0xF7,0x93},
	{0x61,0x7B,0x3A,0x0C,0xE8,0xF0,0x71,0x00},
	{0xDB,0x95,0x86,0x05,0xF8,0xC8,0xC6,0x06},
	{0xED,0xBF,0xD1,0xC6,0x6C,0x29,0xCC,0xC7},
	{0x35,0x55,0x50,0xB2,0x15,0x0E,0x24,0x51},
	{0xCA,0xAA,0xAF,0x4D,0xEA,0xF1,0xDB,0xAE},
	{0xD5,0xD4,0x4F,0xF7,0x20,0x68,0x3D,0x0D},
	{0x2A,0x2B,0xB0,0x08,0xDF,0x97,0xC2,0xF2}};

static int8 cipher_ecb2[NUM_TESTS-1][8]={
	{0x92,0x95,0xB5,0x9B,0xB3,0x84,0x73,0x6E},
	{0x19,0x9E,0x9D,0x6D,0xF3,0x9A,0xA8,0x16},
	{0x2A,0x4B,0x4D,0x24,0x52,0x43,0x84,0x27},
	{0x35,0x84,0x3C,0x01,0x9D,0x18,0xC5,0xB6},
	{0x4A,0x5B,0x2F,0x42,0xAA,0x77,0x19,0x25},
	{0xA0,0x6B,0xA9,0xB8,0xCA,0x5B,0x17,0x8A},
	{0xAB,0x9D,0xB7,0xFB,0xED,0x95,0xF2,0x74},
	{0x3D,0x25,0x6C,0x23,0xA7,0x25,0x2F,0xD6},
	{0xB7,0x6F,0xAB,0x4F,0xBD,0xBD,0xB7,0x67},
	{0x8F,0x68,0x27,0xD6,0x9C,0xF4,0x1A,0x10},
	{0x82,0x57,0xA1,0xD6,0x50,0x5E,0x81,0x85},
	{0xA2,0x0F,0x0A,0xCD,0x80,0x89,0x7D,0xFA},
	{0xCD,0x2A,0x53,0x3A,0xDB,0x0D,0x7E,0xF3},
	{0xD2,0xC2,0xBE,0x27,0xE8,0x1B,0x68,0xE3},
	{0xE9,0x24,0xCF,0x4F,0x89,0x3C,0x5B,0x0A},
	{0xA7,0x18,0xC3,0x9F,0xFA,0x9F,0xD7,0x69},
	{0x77,0x2C,0x79,0xB1,0xD2,0x31,0x7E,0xB1},
	{0x49,0xAB,0x92,0x7F,0xD0,0x22,0x00,0xB7},
	{0xCE,0x1C,0x6C,0x7D,0x85,0xE3,0x4A,0x6F},
	{0xBE,0x91,0xD6,0xE1,0x27,0xB2,0xE9,0x87},
	{0x70,0x28,0xAE,0x8F,0xD1,0xF5,0x74,0x1A},
	{0xAA,0x37,0x80,0xBB,0xF3,0x22,0x1D,0xDE},
	{0xA6,0xC4,0xD2,0x5E,0x28,0x93,0xAC,0xB3},
	{0x22,0x07,0x81,0x5A,0xE4,0xB7,0x1A,0xAD},
	{0xDC,0xCE,0x05,0xE7,0x07,0xBD,0xF5,0x84},
	{0x26,0x1D,0x39,0x2C,0xB3,0xBA,0xA5,0x85},
	{0xB4,0xF7,0x0F,0x72,0xFB,0x04,0xF0,0xDC},
	{0x95,0xBA,0xA9,0x4E,0x87,0x36,0xF2,0x89},
	{0xD4,0x07,0x3A,0xF1,0x5A,0x17,0x82,0x0E},
	{0xEF,0x6F,0xAF,0xA7,0x66,0x1A,0x7E,0x89},
	{0xC1,0x97,0xF5,0x58,0x74,0x8A,0x20,0xE7},
	{0x43,0x34,0xCF,0xDA,0x22,0xC4,0x86,0xC8},
	{0x08,0xD7,0xB4,0xFB,0x62,0x9D,0x08,0x85}};

static int8 cbc_key [8]={0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
static int8 cbc2_key[8]={0xf1,0xe0,0xd3,0xc2,0xb5,0xa4,0x97,0x86};
static int8 cbc3_key[8]={0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
static int8 cbc_iv  [8]={0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
/* Changed the following text constant to binary so it will work on ebcdic
 * machines :-) */
/* static char cbc_data[40]="7654321 Now is the time for \0001"; */
static int8 cbc_data[40]={
	0x37,0x36,0x35,0x34,0x33,0x32,0x31,0x20,
	0x4E,0x6F,0x77,0x20,0x69,0x73,0x20,0x74,
	0x68,0x65,0x20,0x74,0x69,0x6D,0x65,0x20,
	0x66,0x6F,0x72,0x20,0x00,0x31,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	};

static int8 cbc_ok[32]={
	0xcc,0xd1,0x73,0xff,0xab,0x20,0x39,0xf4,
	0xac,0xd8,0xae,0xfd,0xdf,0xd8,0xa1,0xeb,
	0x46,0x8e,0x91,0x15,0x78,0x88,0xba,0x68,
	0x1d,0x26,0x93,0x97,0xf7,0xfe,0x62,0xb4};

static int8 xcbc_ok[32]={
	0x84,0x6B,0x29,0x14,0x85,0x1E,0x9A,0x29,
	0x54,0x73,0x2F,0x8A,0xA0,0xA6,0x11,0xC1,
	0x15,0xCD,0xC2,0xD7,0x95,0x1B,0x10,0x53,
	0xA6,0x3C,0x5E,0x03,0xB2,0x1A,0xA3,0xC4,
	};

static int8 cbc3_ok[32]={
	0x3F,0xE3,0x01,0xC9,0x62,0xAC,0x01,0xD0,
	0x22,0x13,0x76,0x3C,0x1C,0xBD,0x4C,0xDC,
	0x79,0x96,0x57,0xC0,0x64,0xEC,0xF5,0xD4,
	0x1C,0x67,0x38,0x12,0xCF,0xDE,0x96,0x75};

static char *pt(char *p);

int32 destest(void) {
	int i,j,err=0;
	des_cblock in,out,outin,iv3,iv2;
	des_key_schedule ks,ks2,ks3;
	char cbc_in[40], simCbc_in[40], simEmbIVCbc_in[64];
	char cbc_out[40], simCbc_out[40], simEmbIVCbc_out[64];
	int8 desSimCipherData[8], desSimPlainData[8], key[24];

	rtlglue_printf("Doing ecb\n");
	for (i=0; i<NUM_TESTS; i++) {
		int8 desSimCipherData[8], desSimPlainData[8];
		
		des_set_key((des_cblock*)key_data[i], ks);
		memcpy(in, plain_data[i],8);
		memset(out,0,8);
		memset(outin,0,8);
		des_ecb_encrypt(&in,&out,ks,DES_ENCRYPT);
		des_ecb_encrypt(&out,&outin,ks,DES_DECRYPT);
		
		if(desSim_ecb_encrypt(plain_data[i], &desSimCipherData[0], 8, key_data[i], TRUE) != SUCCESS)
			rtlglue_printf("desSimulator ecb encrypt failed\n");
		if(desSim_ecb_encrypt(cipher_data[i], &desSimPlainData[0], 8, key_data[i], FALSE) != SUCCESS)
			rtlglue_printf("desSimulator ecb decrypt failed\n");
			
		if (memcmp(out,cipher_data[i],8) != 0) {
			rtlglue_printf("Encryption error %2d\nk=%s p=%s o=%s act=%s\n",
				i+1,pt(key_data[i]),pt((char *)in),pt(cipher_data[i]),
				pt((char *)out));
			err=1;
		}
		
		if (memcmp(in,outin,8) != 0) {
			rtlglue_printf("Decryption error %2d\nk=%s p=%s o=%s act=%s\n",
				i+1,pt(key_data[i]),pt((char *)out),pt((char *)in),pt((char *)outin));
			err=1;
		}

		if(memcmp(&desSimCipherData[0], &cipher_data[i], 8) != 0) {
			rtlglue_printf("desSim ecb Encryption error %2d\nk=%s plain=%s Expect cipher=%s crypt result=%s\n",
				i+1,pt(key_data[i]),pt(plain_data[8]),pt(cipher_data[i]), pt(&desSimCipherData[0]));
			err=1;
		}

		if(memcmp(&desSimPlainData[0], plain_data[i], 8) != 0) {
			rtlglue_printf("desSim ecb Decryption error %2d\nk=%s Crypted data=%s Expect plain=%s decrypt result=%s\n",
				i+1,pt(key_data[i]),pt(cipher_data[i]),pt(plain_data[i]), pt(&desSimPlainData[0]));
			err=1;
		}
	}

	rtlglue_printf("Doing ede ecb\n");
	for (i=0; i<(NUM_TESTS-1); i++)	{

		des_set_key((des_cblock*)&key_data[i], ks);
		des_set_key((des_cblock*)&key_data[i+1], ks2);
//		des_set_key(&key_data[i+2], ks3);	//This code is useless...legacy?
		memcpy(in,plain_data[i],8);
		memset(out,0,8);
		memset(outin,0,8);
		des_ecb2_encrypt(&in,&out,&ks,&ks2,(int)DES_ENCRYPT);
		des_ecb2_encrypt(&out,&outin,&ks,&ks2,(int)DES_DECRYPT);

		memcpy(&key[0], &key_data[i], 8);
		memcpy(&key[8], &key_data[i+1], 8);
		memcpy(&key[16], &key_data[i], 8);
		if(desSim_ede_ecb_encrypt(plain_data[i], &desSimCipherData[0], 8, &key[0], TRUE) != SUCCESS)
			rtlglue_printf("desSimulator ede ecb encrypt failed\n");
		if(desSim_ede_ecb_encrypt(cipher_ecb2[i], &desSimPlainData[0], 8, &key[0], FALSE) != SUCCESS)
			rtlglue_printf("desSimulator ede ecb decrypt failed\n");
			
		if (memcmp(out,cipher_ecb2[i],8) != 0)
			{
			rtlglue_printf("Encryption error %2d\nk=%s %s p=%s o=%s act=%s\n",
				i+1,pt(key_data[i]),pt(key_data[i+1]),pt(in),pt(cipher_ecb2[i]),
				pt(out));
			err=1;
			}
		if (memcmp(in,outin,8) != 0)
			{
			rtlglue_printf("Decryption error %2d\nk=%s %s p=%s o=%s act=%s\n",
				i+1,pt(key_data[i]),pt(key_data[i+1]),pt(out),pt(in),pt(outin));
			err=1;
			}
		if(memcmp(&desSimCipherData[0], &cipher_ecb2[i], 8) != 0) {
			rtlglue_printf("desSim cbc ecb encryption error %2d\nk=%s plain=%s Expect cipher=%s crypt result=%s\n",
				i+1,pt(key_data[i]),pt(plain_data[8]),pt(cipher_ecb2[i]), pt(&desSimCipherData[0]));
			err=1;
		}

		if(memcmp(&desSimPlainData[0], plain_data[i], 8) != 0) {
			rtlglue_printf("desSim cbc ecb decryption error %2d\nk=%s Crypted data=%s Expect plain=%s decrypt result=%s\n",
				i+1,pt(key_data[i]),pt(cipher_ecb2[i]),pt(plain_data[i]), pt(&desSimPlainData[0]));
			err=1;
		}
	}

	rtlglue_printf("Doing cbc\n");
	if ((j=des_set_key((des_cblock*)cbc_key, ks)) != 0)
		rtlglue_printf("Key error %d\n",j);

	memset(cbc_out,0,40);
	memset(cbc_in,0,40);
	memcpy(iv3,cbc_iv,sizeof(cbc_iv));
	des_ncbc_encrypt( (des_cblock *)cbc_data, (des_cblock *)cbc_out, strlen((char *)cbc_data)+1, &ks,  &iv3, TRUE);
	if (memcmp(cbc_out,cbc_ok,32) != 0)
		rtlglue_printf("cbc_encrypt encrypt error\n");

	memcpy(iv3,cbc_iv,sizeof(cbc_iv));
	des_ncbc_encrypt((des_cblock *)cbc_out,(des_cblock *)cbc_in,strlen((char *)cbc_data)+1,&ks, &iv3, FALSE);
	if (memcmp(cbc_in,cbc_data,strlen((char *)cbc_data)) != 0)
		rtlglue_printf("cbc_encrypt decrypt error\n");

	if(desSim_cbc_encrypt(cbc_data, simCbc_out, strlen((char *)cbc_data)+1, &cbc_key[0], &cbc_iv[0], TRUE) != SUCCESS)
		rtlglue_printf("desSim cbc encryption error\n");
	if (memcmp(simCbc_out,cbc_ok,32) != 0)
		rtlglue_printf("desSim cbc encrypt result error\n");

	memcpy(&simEmbIVCbc_in[0], &cbc_iv[0], 8);
	memcpy(&simEmbIVCbc_in[8], &cbc_data[0], 40);
	if(desSim_cbc_encryptEmbIV(&simEmbIVCbc_in[0], &simEmbIVCbc_out[0], strlen((char *)cbc_data)+9, &cbc_key[0], TRUE) != SUCCESS)
		rtlglue_printf("desSim cbc embed IV encrypt failed\n");
	if (memcmp(&simEmbIVCbc_out[8], cbc_ok,32) != 0)
		rtlglue_printf("desSim cbc embed IV encrypt result error\n");
	
	//CBC Decrypt
	if(desSim_cbc_encrypt(simCbc_out, simCbc_in, strlen((char *)cbc_data)+1, &cbc_key[0], &cbc_iv[0], FALSE) != SUCCESS) 
		rtlglue_printf("desSim cbc decryption error\n");
	if (memcmp(simCbc_in,cbc_data,strlen((char *)cbc_data)) != 0)
		rtlglue_printf("desSim cbc decrypt error\n");

	memcpy(&simEmbIVCbc_out[0], &cbc_iv[0], 8);
	if(desSim_cbc_encryptEmbIV(&simEmbIVCbc_out[0], &simEmbIVCbc_in[0], strlen((char *)cbc_data)+9, &cbc_key[0], FALSE) != SUCCESS)
		rtlglue_printf("desSim cbc embed IV decrypt failed\n");
	if (memcmp(&simEmbIVCbc_in[8], cbc_data,strlen((char *)cbc_data)) != 0)
		rtlglue_printf("desSim cbc embed IV decrypt error\n");
	
	
	rtlglue_printf("Doing ede cbc\n");
	if ((j=des_set_key((des_cblock*)cbc_key, ks)) != 0)
		rtlglue_printf("Key1 error %d\n",j);
	if ((j=des_set_key((des_cblock*)cbc2_key, ks2)) != 0)
		rtlglue_printf("Key2 error %d\n",j);
	if ((j=des_set_key((des_cblock*)cbc3_key, ks3)) != 0)
		rtlglue_printf("Key3 error %d\n",j);
	memset(cbc_out,0,40);
	memset(cbc_in,0,40);
	i=strlen((char *)cbc_data)+1;
	/* i=((i+7)/8)*8; */
	memcpy(iv3,cbc_iv,sizeof(cbc_iv));

	des_ede3_cbc_encrypt((des_cblock *)cbc_data, (des_cblock *)cbc_out,i,&ks,&ks2,&ks3,&iv3, DES_ENCRYPT);
	if (memcmp(cbc_out,cbc3_ok,
		(unsigned int)(strlen((char *)cbc_data)+1+7)/8*8) != 0)
		{
		int n;

		rtlglue_printf("des_ede3_cbc_encrypt encrypt error\n");
		for(n=0 ; n < i ; ++n)
		    rtlglue_printf(" %02x",cbc_out[n]);
		rtlglue_printf("\n");
		for(n=0 ; n < i ; ++n)
		    rtlglue_printf(" %02x",cbc3_ok[n]);
		rtlglue_printf("\n");
		err=1;
		}

	memcpy(&key[0], &cbc_key[0], 8);
	memcpy(&key[8], &cbc2_key[0], 8);
	memcpy(&key[16], &cbc3_key[0], 8);
	if(desSim_ede_cbc_encrypt(&cbc_data[0], &simCbc_out[0], strlen((char *)cbc_data)+1, &key[0], &cbc_iv[0], TRUE) != SUCCESS)
		rtlglue_printf("desSim ede cbc encrypt failed\n");
	if(memcmp(simCbc_out, cbc3_ok, (unsigned int)(strlen((char *)cbc_data)+1+7)/8*8) != 0)
		rtlglue_printf("desSim ede cbc encrypt result error\n");

	memcpy(&simEmbIVCbc_in[0], &cbc_iv[0], 8);
	memcpy(&simEmbIVCbc_in[8], &cbc_data[0], 40);
	if(desSim_ede_cbc_encryptEmbIV(&simEmbIVCbc_in[0], &simEmbIVCbc_out[0], strlen((char *)cbc_data)+1+8, &key[0], TRUE) != SUCCESS)
		rtlglue_printf("desSim ede cbc embed IV encrypt failed\n");
	if(memcmp(&simEmbIVCbc_out[8], cbc3_ok, (unsigned int)(strlen((char *)cbc_data)+1+7)/8*8) != 0)
		rtlglue_printf("desSim ede cbc embed IV encrypt result error\n");
	
	
	memcpy(iv3,cbc_iv,sizeof(cbc_iv));
	des_ede3_cbc_encrypt( (des_cblock *)cbc_out, (des_cblock *)cbc_in,i,&ks, &ks2,&ks3,&iv3,FALSE);
	if (memcmp(cbc_in,cbc_data,strlen((char *)cbc_data)+1) != 0) {
		int n;

		rtlglue_printf("des_ede3_cbc_encrypt decrypt error\n");
		for(n=0 ; n < i ; ++n)
		    rtlglue_printf(" %02x",cbc_data[n]);
		rtlglue_printf("\n");
		for(n=0 ; n < i ; ++n)
		    rtlglue_printf(" %02x",cbc_in[n]);
		rtlglue_printf("\n");
		err=1;
	}

	if(desSim_ede_cbc_encrypt(&simCbc_out[0], &simCbc_in[0], i, &key[0], &cbc_iv[0], FALSE) != SUCCESS)
		rtlglue_printf("desSim ede cbc decrypt failed\n");
	if(memcmp(simCbc_in, cbc_data, strlen((char *)cbc_data)+1) != 0) {
		int n;
		rtlglue_printf("desSim ede cbc decrypt result error\n");
		for(n=0 ; n < i ; ++n)
		    rtlglue_printf(" %02x",cbc_data[n]);
		rtlglue_printf("\n");
		for(n=0 ; n < i ; ++n)
		    rtlglue_printf(" %02x",simCbc_in[n]);
		rtlglue_printf("\n");
	}

	memcpy(&simEmbIVCbc_out[0], &cbc_iv[0], 8);
	if(desSim_ede_cbc_encryptEmbIV(&simEmbIVCbc_out[0], &simEmbIVCbc_in[0], i+8, &key[0], FALSE) != SUCCESS)
		rtlglue_printf("desSim ede cbc embed IV decrypt failed\n");
	if(memcmp(&simEmbIVCbc_in[8], cbc_data, strlen((char *)cbc_data)+1) != 0) {
		int n;
		rtlglue_printf("desSim ede cbc embed IV decrypt result error\n");
		for(n=0 ; n < i ; ++n)
		    rtlglue_printf(" %02x",cbc_data[n]);
		rtlglue_printf("\n");
		for(n=0 ; n < i ; ++n)
		    rtlglue_printf(" %02x",simEmbIVCbc_in[n+8]);
		rtlglue_printf("\n");
	}

	/*Crypto engine not implemented part*/
	rtlglue_printf("No crypto engine verification part\n");
	rtlglue_printf("Doing cbcm\n");
	if ((j=des_set_key((des_cblock*)cbc_key, ks)) != 0)
		{
		rtlglue_printf("Key error %d\n",j);
		err=1;
		}
	if ((j=des_set_key((des_cblock*)cbc2_key, ks2)) != 0)
		{
		rtlglue_printf("Key error %d\n",j);
		err=1;
		}
	if ((j=des_set_key((des_cblock*)cbc3_key, ks3)) != 0)
		{
		rtlglue_printf("Key error %d\n",j);
		err=1;
		}
	memset(cbc_out,0,40);
	memset(cbc_in,0,40);
	i=strlen((char *)cbc_data)+1;
	/* i=((i+7)/8)*8; */
	memcpy(iv3,cbc_iv,sizeof(cbc_iv));
	memset(iv2,'\0',sizeof iv2);

	des_ede3_cbcm_encrypt(cbc_data,cbc_out,16L,ks,ks2,ks3,&iv3,&iv2,
			      DES_ENCRYPT);
	des_ede3_cbcm_encrypt(&cbc_data[16],&cbc_out[16],i-16,ks,ks2,ks3,
			      &iv3,&iv2,DES_ENCRYPT);
	memcpy(iv3,cbc_iv,sizeof(cbc_iv));
	memset(iv2,'\0',sizeof iv2);
	des_ede3_cbcm_encrypt(cbc_out,cbc_in,i,ks,ks2,ks3,&iv3,&iv2,DES_DECRYPT);
	if (memcmp(cbc_in,cbc_data,strlen((char *)cbc_data)+1) != 0) {
		int n;

		rtlglue_printf("des_ede3_cbcm_encrypt decrypt error\n");
		for(n=0 ; n < i ; ++n)
		    rtlglue_printf(" %02x",cbc_data[n]);
		rtlglue_printf("\n");
		for(n=0 ; n < i ; ++n)
		    rtlglue_printf(" %02x",cbc_in[n]);
		rtlglue_printf("\n");
		err=1;
	} 
	rtlglue_printf("Doing desx cbc\n");
	if ((j=des_set_key((des_cblock *)cbc_key, ks)) != 0) {
		rtlglue_printf("Key error %d\n",j);
		err=1;
	}
	memset(cbc_out,0,40);
	memset(cbc_in,0,40);
	memcpy(iv3,cbc_iv,sizeof(cbc_iv));
	des_xcbc_encrypt((des_cblock*)cbc_data,(des_cblock*)cbc_out,strlen((char *)cbc_data)+1,ks,
			 &iv3,(des_cblock*)cbc2_key,(des_cblock*)cbc3_key, DES_ENCRYPT);
	if (memcmp(cbc_out,xcbc_ok,32) != 0) {
		rtlglue_printf("des_xcbc_encrypt encrypt error\n");
		err=1;
	}
	memcpy(iv3,cbc_iv,sizeof(cbc_iv));
	des_xcbc_encrypt((des_cblock*)cbc_out,(des_cblock*)cbc_in,strlen((char *)cbc_data)+1,ks,
			 &iv3,(des_cblock*)cbc2_key,(des_cblock*)cbc3_key, DES_DECRYPT);
	if (memcmp(cbc_in,cbc_data,strlen((char *)cbc_data)+1) != 0) {
		rtlglue_printf("des_xcbc_encrypt decrypt error\n");
		err=1;
	}

	return(err);
}

static char *pt(char *p)
	{
	static char bufs[10][20];
	static int bnum=0;
	char *ret;
	int i;
	static char *f="0123456789ABCDEF";

	ret= &(bufs[bnum++][0]);
	bnum%=10;
	for (i=0; i<8; i++)
		{
		ret[i*2]=f[(p[i]>>4)&0xf];
		ret[i*2+1]=f[p[i]&0xf];
		}
	ret[16]='\0';
	return(ret);
}

/*
int32 desSim_ecb_encrypt(int8 *input, int8 * output, uint32 len, int8 * ks, int8 encrypt);
int32 desSim_ede_ecb_encrypt(int8 *input, int8 * output, uint32 len, int8 * key, int8 encrypt);
int32 desSim_cbc_encrypt(int8 *input, int8 * output, uint32 len, int8 * key, int8 * iv, int8 encrypt);
int32 desSim_ede_cbc_encrypt(int8 *input, int8 * output, uint32 len, int8 * key, int8 * iv, int8 encrypt);
int32 desSim_cbc_encryptEmbIV(int8 *input, int8 * output, uint32 len, int8 * key, int8 encrypt) ;
int32 desSim_ede_cbc_encryptEmbIV(int8 *input, int8 * output, uint32 len, int8 * key, int8 encrypt);
*/
/*
int32 rtl8651b_cryptoEngine_ecb_encrypt(int8 *input, uint32 len, int8 * ks, int8 encrypt);
int32 rtl8651b_cryptoEngine_3des_ecb_encrypt(int8 *input, uint32 len, int8 * key, int8 encrypt);
int32 rtl8651b_cryptoEngine_cbc_encrypt(int8 *input, uint32 len, int8 * key, int8 * iv, int8 encrypt);
int32 rtl8651b_cryptoEngine_3des_cbc_encrypt(int8 *input, uint32 len, int8 * key, int8 * iv, int8 encrypt);
int32 rtl8651b_cryptoEngine_cbc_encryptEmbIV(int8 *input, uint32 len, int8 * key, int8 encrypt) ;
int32 rtl8651b_cryptoEngine_3des_cbc_encryptEmbIV(int8 *input, uint32 len, int8 * key, int8 encrypt);
*/
int8 allenKey[8] = {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
};

int8 allenIV[8] = {
	0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef	
};

int8 allenPlaintext[96] = {
	0x4e, 0x6f, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74, 0x5c, 0x5b, 0x21, 0x58, 0xf9, 0xd8, 0xed, 0x9b,
	0xe5, 0xc7, 0xcd, 0xde, 0x87, 0x2b, 0xf2, 0x7c, 0xe5, 0xc7, 0xcd, 0xde, 0x87, 0x2b, 0xf2, 0x7c, 
	0x68, 0x65, 0x20, 0x74, 0x69, 0x6d, 0x65, 0x20, 0x8d, 0xa2, 0xed, 0xaa, 0xee, 0x46, 0x97, 0x5c,
	0x43, 0xe9, 0x34, 0x00, 0x8c, 0x38, 0x9c, 0x0f, 0x43, 0xe9, 0x34, 0x00, 0x8c, 0x38, 0x9c, 0x0f, 
	0x66, 0x6f, 0x72, 0x20, 0x61, 0x6c, 0x6c, 0x20, 0x25, 0x86, 0x46, 0x20, 0xed, 0x54, 0xf0, 0x2f, 
	0x68, 0x37, 0x88, 0x49, 0x9a, 0x7c, 0x05, 0xf6, 0x68, 0x37, 0x88, 0x49, 0x9a, 0x7c, 0x05, 0xf6,
};

int8 allenCiphertext[96] = {
	0xe5, 0xc7, 0xcd, 0xde, 0x87, 0x2b, 0xf2, 0x7c, 0xe5, 0xc7, 0xcd, 0xde, 0x87, 0x2b, 0xf2, 0x7c,
	0x5c, 0x5b, 0x21, 0x58, 0xf9, 0xd8, 0xed, 0x9b, 0x4e, 0x6f, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74, 
	0x43, 0xe9, 0x34, 0x00, 0x8c, 0x38, 0x9c, 0x0f, 0x43, 0xe9, 0x34, 0x00, 0x8c, 0x38, 0x9c, 0x0f,
	0x8d, 0xa2, 0xed, 0xaa, 0xee, 0x46, 0x97, 0x5c, 0x68, 0x65, 0x20, 0x74, 0x69, 0x6d, 0x65, 0x20, 
	0x68, 0x37, 0x88, 0x49, 0x9a, 0x7c, 0x05, 0xf6, 0x68, 0x37, 0x88, 0x49, 0x9a, 0x7c, 0x05, 0xf6,
	0x25, 0x86, 0x46, 0x20, 0xed, 0x54, 0xf0, 0x2f, 0x66, 0x6f, 0x72, 0x20, 0x61, 0x6c, 0x6c, 0x20,
};

int8 *cryptOrg, *cryptAsic, *cryptEncrypt, *cryptDecrypt;
int8 *cryptKey, *cryptIv;

static void displayEncryptMismatch(int8 * title, int8 * org, int8 * data1, int8 * data2, uint32 pktLen) {
	uint32 i, j;
	
	rtlglue_printf("\n%s [%p] [%p]\n", title, data1, data2);
	for(i=0;i<pktLen;i+=8) {
		if(memcmp(&data1[i], &data2[i], 8) != 0) {
			rtlglue_printf("%04u:", i);
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", org[i+j]&0xff);
			rtlglue_printf("->[%p]",&data1[i]); 
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", data1[i+j]&0xff);
			rtlglue_printf("!=[%p]",&data2[i]);
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", data2[i+j]&0xff);
			rtlglue_printf("\n");
		}
	}

}

static void displayDecryptMismatch(int8 * title, int8 * org, int8 * data, uint32 pktLen) {
	uint32 i, j;
	
	rtlglue_printf("\n%s [%p]\n", title, data);
	for(i=0;i<pktLen;i+=8) {
		if(memcmp(&org[i], &data[i], 8) != 0) {
			rtlglue_printf("%04u: ", i);
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", org[i+j]&0xff);
			rtlglue_printf(" ->> [%p]",&data[i]); 
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", data[i+j]&0xff);
			rtlglue_printf("\n");
		}
	}

}

static int32 runDes8651TestRoundDesEcb(uint32 offset, uint32 pktLen) {
	int32 ret = SUCCESS;
	
	if(desSim_ecb_encrypt(cryptOrg+offset, cryptEncrypt+offset, pktLen, cryptKey, TRUE) == FAILED) {
		rtlglue_printf("desSim DES ECB encrypt failed\n");
		ret = FAILED;
	}
	if(desSim_ecb_encrypt(cryptEncrypt+offset, cryptDecrypt+offset, pktLen, cryptKey, FALSE) == FAILED) {
		rtlglue_printf("desSim DES ECB decrypt failed\n");
		ret = FAILED;
	}
	if(memcmp(cryptOrg+offset, cryptDecrypt+offset, pktLen) != 0) {
		displayDecryptMismatch("desSim DES ECB encrypt->decrypt failed", &cryptOrg[offset], &cryptDecrypt[offset], pktLen);
		ret = FAILED;
	}

	memcpy(cryptAsic+offset, cryptOrg+offset, pktLen);
	if(rtl8651b_cryptoEngine_ecb_encrypt(cryptAsic+offset, pktLen, cryptKey, TRUE) == FAILED) {
		rtlglue_printf("cryptoEngine DES ECB encrypt failed\n");
		ret = FAILED;
	}
	if(memcmp(cryptAsic+offset, cryptEncrypt+offset, pktLen) != 0) {
		displayEncryptMismatch("DES ECB SW and HW results are different", &cryptOrg[offset], &cryptAsic[offset], &cryptEncrypt[offset], pktLen);
		if(memcmp(cryptAsic+offset, cryptOrg+offset, pktLen)==0)
			rtlglue_printf("cryptoEngine DES ECB do nothing\n");
		ret = FAILED;
	}

	if(rtl8651b_cryptoEngine_ecb_encrypt(cryptAsic+offset, pktLen, cryptKey, FALSE) == FAILED) {
		rtlglue_printf("cryptoEngine DES ECB decrypt failed\n");
		ret = FAILED;
	}
	if(memcmp(cryptAsic+offset, cryptOrg+offset, pktLen) != 0) {
		displayDecryptMismatch("cryptoEngine DES ECB encrypt->decrypt failed", &cryptOrg[offset], &cryptAsic[offset], pktLen);
		ret = FAILED;
	}
	return ret;
}

static int32 runDes8651TestRound3DesEcb(uint32 offset, uint32 pktLen) {
	int32 ret = SUCCESS;
	
	if(desSim_ede_ecb_encrypt(cryptOrg+offset, cryptEncrypt+offset, pktLen, cryptKey, TRUE) == FAILED) {
		rtlglue_printf("desSim ECB encrypt failed\n");
		ret = FAILED;
	}
	if(desSim_ede_ecb_encrypt(cryptEncrypt+offset, cryptDecrypt+offset, pktLen, cryptKey, FALSE) == FAILED) {
		rtlglue_printf("desSim ECB decrypt failed\n");
		ret = FAILED;
	}
	if(memcmp(cryptOrg+offset, cryptDecrypt+offset, pktLen) != 0) {
		displayDecryptMismatch("desSim ECB encrypt->decrypt failed", &cryptOrg[offset], &cryptDecrypt[offset], pktLen);
		ret = FAILED;
	}

	memcpy(cryptAsic+offset, cryptOrg+offset, pktLen);
	if(rtl8651b_cryptoEngine_3des_ecb_encrypt(cryptAsic+offset, pktLen, cryptKey, TRUE) == FAILED) {
		rtlglue_printf("cryptoEngine 3DES ECB encrypt failed\n");
		ret = FAILED;
	}
	if(memcmp(cryptAsic+offset, cryptEncrypt+offset, pktLen) != 0) {
		displayEncryptMismatch("3DES ECB SW and HW results are different", &cryptOrg[offset], &cryptAsic[offset], &cryptEncrypt[offset], pktLen);
		if(memcmp(cryptAsic+offset, cryptOrg+offset, pktLen)==0)
			rtlglue_printf("cryptoEngine 3DES ECB do nothing\n");
		ret = FAILED;
	}
	if(rtl8651b_cryptoEngine_3des_ecb_encrypt(cryptAsic+offset, pktLen, cryptKey, FALSE) == FAILED) {
		rtlglue_printf("cryptoEngine 3DES ECB decrypt failed\n");
		ret = FAILED;
	}
	if(memcmp(cryptAsic+offset, cryptOrg+offset, pktLen) != 0) {
		displayDecryptMismatch("cryptoEngine 3DES ECB encrypt->decrypt failed", &cryptOrg[offset], &cryptAsic[offset], pktLen);
		ret = FAILED;
	}
	return ret;
}

static int32 runDes8651TestRoundDesCbc(uint32 offset, uint32 pktLen) {
	int32 ret = SUCCESS;
	
	if(desSim_cbc_encrypt(cryptOrg+offset, cryptEncrypt+offset, pktLen, cryptKey, cryptIv, TRUE) == FAILED) {
		rtlglue_printf("desSim DES CBC encrypt failed\n");
		ret = FAILED;
	}
	if(desSim_cbc_encrypt(cryptEncrypt+offset, cryptDecrypt+offset, pktLen, cryptKey, cryptIv, FALSE) == FAILED) {
		rtlglue_printf("desSim DES CBC decrypt failed\n");
		ret = FAILED;
	}
	if(memcmp(cryptOrg+offset, cryptDecrypt+offset, pktLen) != 0) {
		displayDecryptMismatch("desSim DES CBC encrypt->decrypt failed", &cryptOrg[offset], &cryptDecrypt[offset], pktLen);
		ret = FAILED;
	}

	memcpy(cryptAsic+offset, cryptOrg+offset, pktLen);
	if(rtl8651b_cryptoEngine_cbc_encrypt(cryptAsic+offset, pktLen, cryptKey, cryptIv, TRUE) == FAILED) {
		rtlglue_printf("cryptoEngine DES CBC encryption failed\n");
		ret = FAILED;
	}
	if(memcmp(cryptAsic+offset, cryptEncrypt+offset, pktLen) != 0) {
		displayEncryptMismatch("DES CBC SW and HW results are different", &cryptOrg[offset], &cryptAsic[offset], &cryptEncrypt[offset], pktLen);
		if(memcmp(cryptAsic+offset, cryptOrg+offset, pktLen) == 0)
			rtlglue_printf("cryptoEngine DES CBC do nothing\n");
		ret = FAILED;
	}
	if(rtl8651b_cryptoEngine_cbc_encrypt(cryptAsic+offset, pktLen, cryptKey, cryptIv, FALSE) == FAILED) {
		rtlglue_printf("cryptoEngine DES CBC decryption failed\n");
		ret = FAILED;
	}
	if(memcmp(cryptAsic+offset, cryptOrg+offset, pktLen) != 0) {
		displayDecryptMismatch("cryptoEngine DES CBC encrypt->decrypt failed", &cryptOrg[offset], &cryptAsic[offset], pktLen);
		ret = FAILED;
	}
	return ret;
}

static int32 runDes8651TestRound3DesCbc(uint32 offset, uint32 pktLen) {
	int32 ret = SUCCESS;
	
	if(desSim_ede_cbc_encrypt(cryptOrg+offset, cryptEncrypt+offset, pktLen, cryptKey, cryptIv, TRUE) == FAILED) {
		rtlglue_printf("desSim 3DES CBC encrypt failed\n");
		ret = FAILED;
	}
	if(desSim_ede_cbc_encrypt(cryptEncrypt+offset, cryptDecrypt+offset, pktLen, cryptKey, cryptIv, FALSE) == FAILED) {
		rtlglue_printf("desSim 3DES CBC decrypt failed\n");
		ret = FAILED;
	}
	if(memcmp(cryptOrg+offset, cryptDecrypt+offset, pktLen) != 0) {
		displayDecryptMismatch("desSim 3DES CBC encrypt->decrypt failed", &cryptOrg[offset], &cryptDecrypt[offset], pktLen);
		ret = FAILED;
	}

	memcpy(cryptAsic+offset, cryptOrg+offset, pktLen);
	if(rtl8651b_cryptoEngine_3des_cbc_encrypt(cryptAsic+offset, pktLen, cryptKey, cryptIv, TRUE) == FAILED) {
		rtlglue_printf("cryptoEngine 3DES CBC encryption failed\n");
		ret = FAILED;
	}
	if(memcmp(cryptAsic+offset, cryptEncrypt+offset, pktLen) != 0) {
		displayEncryptMismatch("3DES CBC SW and HW results are different", &cryptOrg[offset], &cryptAsic[offset], &cryptEncrypt[offset], pktLen);
		if(memcmp(cryptAsic+offset, cryptOrg+offset, pktLen) == 0)
			rtlglue_printf("cryptoEngine 3DES CBC do nothing\n");
		ret = FAILED;
	}
	if(rtl8651b_cryptoEngine_3des_cbc_encrypt(cryptAsic+offset, pktLen, cryptKey, cryptIv, FALSE) == FAILED) {
		rtlglue_printf("cryptoEngine 3DES CBC decryption failed\n");
		ret = FAILED;
	}
	if(memcmp(cryptAsic+offset, cryptOrg+offset, pktLen) != 0) {
		displayDecryptMismatch("cryptoEngine 3DES CBC encrypt->decrypt failed", &cryptOrg[offset], &cryptAsic[offset], pktLen);
		ret = FAILED;
	}
	return ret;	
}


typedef struct testFunc_s {
	int8 * funcTitle;
	int32 (*func)(uint32, uint32);
} testFunc_t;

static testFunc_t testDesItem[6] = {
	{"DES ECB",  runDes8651TestRoundDesEcb},
	{"3DES ECB",  runDes8651TestRound3DesEcb},
	{"DES CBC",  runDes8651TestRoundDesCbc},
	{"3DES CBC",  runDes8651TestRound3DesCbc},
};

int32 des8651bTest(uint32 round, uint32 funStart, uint32 funEnd, uint32 lenStart, uint32 lenEnd, uint32 offsetStart, uint32 offsetEnd) {
	uint32 i, pktLen, offset, funIdx, dupRound;
	
	rtlglue_srandom(19);

/* Since UNCACHED_MALLOC() is un-safe if allocation fails, we need to rewrite the following code. */
#define UNCACHED_MALLOC(x) UNCACHED_ADDRESS(rtlglue_malloc(x))
#define UNCACHED_FREE(x) rtlglue_free((void*)CACHED_ADDRESS(x))
	cryptOrg = (int8 *) UNCACHED_MALLOC(4352);
	cryptAsic = (int8 *) UNCACHED_MALLOC(4352);
	cryptEncrypt = (int8 *) UNCACHED_MALLOC(4352);
	cryptDecrypt = (int8 *) UNCACHED_MALLOC(4352);
	cryptKey = (int8 *) UNCACHED_MALLOC(24);
	cryptIv = (int8 *) UNCACHED_MALLOC(8);
	
	for(i=0; i<24; i++)
		cryptKey[i] = 0x01;
	for(i=0; i<8; i++)
		cryptIv[i] = 0x01;
	for(i=0; i<2064; i++)/*8-byte for IV and 4-byte for DMA_addr (0-3) byte offset test*/
		cryptOrg[i] = 0xff;

#if 0
	REG32(0xbd01200c) &= 0x0fffffff;
	REG32(0xbd012010) |= 0xf0000000;
	REG32(0xbd012014) &= 0x0fffffff;
#endif

	for(dupRound=0; dupRound<=round; dupRound++) {
		for(funIdx=funStart; funIdx<=funEnd; funIdx++) {
			for(pktLen = (lenStart&0xFFFFFFF8); pktLen <= (lenEnd&0xFFFFFFF8); pktLen+=8) {
				for(offset = offsetStart; offset<=offsetEnd; offset++) {
					rtlglue_printf("\rRound %4d %s Offset %2d PktLen %4d", dupRound, testDesItem[funIdx].funcTitle, offset, pktLen);
					if(testDesItem[funIdx].func(offset, pktLen)==FAILED) {
						rtlglue_printf("\nOffset %2d PktLen %4d %s Failed\n", offset, pktLen, testDesItem[funIdx].funcTitle);
#if 0
						REG32(0xbd012014) |= 0xf0000000;
#endif
						goto FINISHED;
					}
				}
			}
		}
	}	

FINISHED:	
	UNCACHED_FREE(cryptOrg);
	UNCACHED_FREE(cryptAsic);
	UNCACHED_FREE(cryptEncrypt);
	UNCACHED_FREE(cryptDecrypt);
	UNCACHED_FREE(cryptKey);
	UNCACHED_FREE(cryptIv);

	return SUCCESS;
}

static int8 * desModeString[] = {
	"DECRYPT_CBC_DES ",
	"DECRYPT_CBC_3DES",
	"DECRYPT_ECB_DES ",
	"DECRYPT_ECB_3DES",
	"ENCRYPT_CBC_DES ",
	"ENCRYPT_CBC_3DES",
	"ENCRYPT_ECB_DES ",
	"ENCRYPT_ECB_3DES",
};

int32 des8651bAsicThroughput(uint32 round, uint32 startMode, uint32 endMode, uint32 pktLen) {
	uint32 testRound, modeIdx, i, bps;
	uint32 sTime, eTime;
	
	cryptAsic = (int8 *) UNCACHED_MALLOC(4352);
	cryptKey = (int8 *) UNCACHED_MALLOC(24);
	cryptIv = (int8 *) UNCACHED_MALLOC(8);

	for(i=0; i<24; i++)
		cryptKey[i] = 0x01;
	for(i=0; i<8; i++)
		cryptIv[i] = 0x01;
	for(i=0; i<pktLen; i++)/*8-byte for IV and 4-byte for DMA_addr (0-3) byte offset test*/
		cryptAsic[i] = 0xff;
	rtlglue_printf("Evaluate 8651b throughput(round=%d,startMode=%d,endMode=%d,pktLen=%d)\n",round,startMode,endMode,pktLen);
	for(modeIdx=startMode;modeIdx<=endMode;modeIdx++) {
		rtlglue_getmstime(&sTime);
		for(testRound=0; testRound<=round; testRound++) 
		{
			if ( rtl8651b_cryptoEngine_des(modeIdx,  cryptAsic, pktLen, cryptKey, cryptIv) != SUCCESS )
			{
				rtlglue_printf("testRound=%d, rtl8651b_cryptoEngine_des(modeIdx=%d) failed...\n", testRound, modeIdx );
				return FAILED;
			}
		}
		rtlglue_getmstime(&eTime);

		if ( eTime - sTime == 0 )
		{
			rtlglue_printf("round is too small to measure throughput, try larger round number!\n");
			return FAILED;
		}
		else
		{
			bps = pktLen*8*1000/((uint32)(eTime - sTime))*round;
			if(bps>1000000)
				rtlglue_printf("%s round %u len %u time %u throughput %u.%02u mbps\n", desModeString[modeIdx], round, pktLen, (uint32)(eTime - sTime), bps/1000000, (bps%1000000)/10000);
			else if(bps>1000)
				rtlglue_printf("%s round %u len %u time %u throughput %u.%02u kbps\n", desModeString[modeIdx], round, pktLen, (uint32)(eTime - sTime), bps/1000, (bps%1000)/10);
			else
				rtlglue_printf("%s round %u len %u time %u throughput %u bps\n", desModeString[modeIdx], round, pktLen, (uint32)(eTime - sTime), bps);			
		}
	}
	UNCACHED_FREE(cryptAsic);
	UNCACHED_FREE(cryptKey);
	UNCACHED_FREE(cryptIv);
	
	return SUCCESS;
}

int32 des8651bSwThroughput(uint32 round, uint32 startMode, uint32 endMode, uint32 pktLen) {
	uint32 testRound, modeIdx, i, bps;
	uint32 sTime, eTime;
	
	cryptOrg = (int8 *) UNCACHED_MALLOC(4352);
	cryptEncrypt = (int8 *) UNCACHED_MALLOC(4352);
	cryptKey = (int8 *) UNCACHED_MALLOC(24);
	cryptIv = (int8 *) UNCACHED_MALLOC(8);

	for(i=0; i<24; i++)
		cryptKey[i] = 0x01;
	for(i=0; i<8; i++)
		cryptIv[i] = 0x01;
	for(i=0; i<pktLen; i++)//8-byte for IV and 4-byte for DMA_addr (0-3) byte offset test
		cryptOrg[i] = 0xff;
	rtlglue_printf("Evaluate software simulation throughput\n");
	for(modeIdx=startMode;modeIdx<=endMode;modeIdx++) {
		rtlglue_getmstime(&sTime);
		for(testRound=0; testRound<=round; testRound++) 
		{
			if ( desSim_des(modeIdx,  cryptOrg, cryptEncrypt, pktLen, cryptKey, cryptIv) != SUCCESS )
			{
				rtlglue_printf("testRound=%d, desSim_des(modeIdx=%d) failed...\n", testRound, modeIdx );
				return FAILED;
			}
		}
		rtlglue_getmstime(&eTime);

		if ( eTime - sTime == 0 )
		{
			rtlglue_printf("round is too small to measure throughput, try larger round number!\n");
			return FAILED;
		}
		else
		{
			bps = pktLen*8*1000/((uint32)(eTime - sTime))*round;
			if(bps>1000000)
				rtlglue_printf("%s round %u len %u time %u throughput %u.%02u mbps\n", desModeString[modeIdx], round, pktLen, (uint32)(eTime - sTime), bps/1000000, (bps%1000000)/10000);
			else if(bps>1000)
				rtlglue_printf("%s round %u len %u time %u throughput %u.%02u kbps\n", desModeString[modeIdx], round, pktLen, (uint32)(eTime - sTime), bps/1000, (bps%1000)/10);
			else
				rtlglue_printf("%s round %u len %u time %u throughput %u bps\n", desModeString[modeIdx], round, pktLen, (uint32)(eTime - sTime), bps);			
		}
	}
	UNCACHED_FREE(cryptAsic);
	UNCACHED_FREE(cryptKey);
	UNCACHED_FREE(cryptIv);
	
	return SUCCESS;
}

static int8 *cryptGenOrg, *cryptGenEncrypt, *cryptGenDecrypt, *cryptGenAsic;
static int8 _cryptGenKey[32], _cryptGenIv[16];
static int8 *cryptGenKey, *cryptGenIv;
static int8 * desOpModeString[] = {
	"CBC_DES ",
	"CBC_3DES",
	"ECB_DES ",
	"ECB_3DES",
};
static void displayDesEncryptMismatch(int8 * modeTitle, int8 * title, int8 * org, int8 * data1, int8 * data2, uint32 pktLen) {
	uint32 i, j;
	uint32 k;
	rtlglue_printf("\n%s %s [%p] [%p]\n", modeTitle, title, data1, data2);
	for(i=0;i<pktLen;i+=8) {
		if(memcmp(&data1[i], &data2[i], 8) != 0) {
			rtlglue_printf("%04u:", i);
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", org[i+j]&0xff);
for(j=0;j<100000;j++);
			rtlglue_printf("->[%p]",&data1[i]); 
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", data1[i+j]&0xff);
for(j=0;j<100000;j++);
			rtlglue_printf("!=[%p]",&data2[i]);
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", data2[i+j]&0xff);
for(j=0;j<100000;j++);
			rtlglue_printf("\n");
		}
	}

}

static void displayDesDecryptMismatch(int8 * modeTitle, int8 * title, int8 * org, int8 * data, uint32 pktLen) {
	uint32 i, j;
	
	rtlglue_printf("\n%s %s [%p]\n", modeTitle, title, data);
	for(i=0;i<pktLen;i+=8) {
		if(memcmp(&org[i], &data[i], 8) != 0) {
			rtlglue_printf("%04u: ", i);
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", org[i+j]&0xff);
			rtlglue_printf(" ->> [%p]",&data[i]); 
			for(j=0; j<8; j++)
				rtlglue_printf("%02x", data[i+j]&0xff);
			rtlglue_printf("\n");
		}
	}

}

/********************************************************************
 * To provide general API for 'cr cr as' and 'cr cr rand' commands. *
 *          input
 *         |     |
 *         v     v
 *       SWenc HWenc
 *         |     |
 *         Compare
 *         |     |
 *         v     v
 *       SWdec HWdec
 *         |     |
 *         Compare
 ********************************************************************/
static int32 des8651bGeneralApiTestItem(uint32 mode, int8 * input, uint32 pktLen, int8 * key, int8 * iv) {
	int32 ret = SUCCESS;
uint32 *p_w;
int32 i;	
int32 j;	
	memcpy(cryptGenOrg, input, pktLen);//Backup original data for final comparision
	mode = (mode & 0x3) | 0x4;//Keep algorithm and operation mode, but force to be encrypt
	if(desSim_des(mode, cryptGenOrg, cryptGenEncrypt, pktLen, key, iv) == FAILED)
	{
		return FAILED;//Crypto simulator unable to encrypt is mandatory testing failed
	}

//#define DUMP_CRYPTO_DATA
#ifdef DUMP_CRYPTO_DATA
printf("GenOrg = 0x%x, odata_sw = 0x%x, odata = 0x%x, data size = 0x%x\n", cryptGenOrg, cryptGenEncrypt, input, pktLen);
printf("odata_sw = 0x%x, odata = 0x%x, data size = 0x%x\n", cryptGenEncrypt, input, pktLen);
for(j=0;j<100000;j++);
printf("correct_data:\n");
p_w = (uint32*)cryptGenEncrypt;
for(i=0;i<((pktLen/4));i++){
printf("\t.word 0x%08x\n", *p_w);
for(j=0;j<100000;j++);
p_w++;
}
printf("correct_data_end:\n");
for(j=0;j<1000;j++);
for(i=0; i<1000; i++);
#endif
	if(rtl8651b_cryptoEngine_des(mode, input, pktLen, key, iv) == FAILED)
	{
		return FAILED;//Crypto engine unable to encrypt is mandatory testing failed
	}
	if(memcmp(input, cryptGenEncrypt, pktLen) != 0) 
	{
		displayDesEncryptMismatch(desModeString[mode], "Encrypt ASIC != simulator", cryptGenOrg, input, cryptGenEncrypt, pktLen);
		ret = FAILED;
	}
	mode = mode & 0x3;//Keep algorithm and operation mode, but force to be decrypt
	if(desSim_des(mode, cryptGenEncrypt, cryptGenDecrypt, pktLen, key, iv) == FAILED)
	{
		return FAILED;//Crypto simulator unable to decrypt is mandatory testing failed
	}
#ifdef DUMP_CRYPTO_DATA
if(pktLen == 7544){
printf("odata_sw = 0x%x, odata = 0x%x, data size = 0x%x\n", cryptGenEncrypt, input, pktLen);
for(j=0;j<1000000;j++);
printf("correct_data:\n");
p_w = (uint32*)cryptGenDecrypt;
for(i=0;i<((pktLen/4)+1);i++){
printf("\t.word 0x%08x\n", *p_w);
for(j=0;j<1000000;j++);
p_w++;
}
printf("correct_data_end:\n");
for(j=0;j<1000;j++);
}
for(i=0; i<1000; i++);
#endif
	if(rtl8651b_cryptoEngine_des(mode, input, pktLen, key, iv) == FAILED)
	{
		return FAILED;//Crypto engine unable to encrypt is mandatory testing failed
	}
	if(memcmp(input, cryptGenDecrypt, pktLen) != 0) 
	{
		if(memcmp(cryptGenDecrypt, cryptGenOrg, pktLen))
		{
			displayDesDecryptMismatch(desModeString[mode], "Decrypt Simulator != original", cryptGenOrg, cryptGenDecrypt, pktLen);
			ret = FAILED;
		}
		else 
		{
			displayDesDecryptMismatch(desModeString[mode], "Decrypt ASIC != original", cryptGenOrg, input, pktLen);
			ret = FAILED;
		}
	}
	return ret;
}

/***********************************
 * By 'crypto crypto asic' command *
 *   fixed key/iv/data             *
 ***********************************/
int32 runDes8651bGeneralApiTest(uint32 round, uint32 funStart, uint32 funEnd, uint32 lenStart, uint32 lenEnd, uint32 offsetStart, uint32 offsetEnd) {
	uint32 roundIdx, modeIdx, pktLen,  i, offset, errCount;
	uint32 doneCounter, allDoneCounter;
	int32 ret = SUCCESS;
uint32 k;
	cryptGenKey = UNCACHED_ADDRESS( _cryptGenKey );
	cryptGenIv = UNCACHED_ADDRESS( _cryptGenIv );
	cryptGenOrg = (int8 *) rtlglue_malloc(MAX_PKTLEN+8/*for alignment issue*/);
	cryptGenEncrypt = (int8 *) rtlglue_malloc(MAX_PKTLEN+8/*for alignment issue*/);
	cryptGenDecrypt = (int8 *) rtlglue_malloc(MAX_PKTLEN+8/*for alignment issue*/);
	cryptGenAsic = (int8 *) UNCACHED_MALLOC(MAX_PKTLEN+8/*for alignment issue*/);
#ifdef CONFIG_RTL8316S
mips_cache_flush_all();
#else
_cache_flush_all();
#endif
//rtlglue_printf("cryptGenAsic = 0x%x\n", cryptGenAsic);
	for(i=0; i<24; i++)
		cryptGenKey[i] = 0x01;
	for(i=0; i<8; i++)
		cryptGenIv[i] = 0x01;
	errCount = 0;
#if 0 	
	for(roundIdx = 0; roundIdx<round; roundIdx++)
		for(modeIdx=funStart; modeIdx<=funEnd; modeIdx++)
			for(pktLen=lenStart; pktLen<=lenEnd; pktLen+=8) 
				for(offset=offsetStart; offset<=offsetEnd; offset++) {
					for(i=0; i<pktLen; i++)//8-byte for IV and 4-byte for DMA_addr (0-3) byte offset test
						cryptGenAsic[i+offset] = 0xff;//(int8)taurand(256);
					rtlglue_printf("\r[%05u, %03u]Mode: %s Packet length %04u Offset %u[%08X]", roundIdx, errCount, desModeString[modeIdx], pktLen, offset, &cryptGenAsic[offset]);
					
					if(des8651bGeneralApiTestItem(modeIdx, &cryptGenAsic[offset], pktLen, cryptGenKey, cryptGenIv) == FAILED) 
						errCount++;
				}
#endif
	for(roundIdx = 0; roundIdx<round; roundIdx++)
	{
		for(modeIdx=funStart; modeIdx<=funEnd; modeIdx++)
		{
			for(pktLen=lenStart; pktLen<=lenEnd; pktLen+=8) 
			{
				for(offset=offsetStart; offset<=offsetEnd; offset++) 
				{
					for(i=0; i<pktLen; i++)//8-byte for IV and 4-byte for DMA_addr (0-3) byte offset test
						cryptGenAsic[i+offset] = (i&0xFF);
					rtl8651b_cryptoEngineGetIntCounter(&doneCounter, &allDoneCounter);
					rtlglue_printf("\r[%05u, %03u]Mode: %s Packet length %04u Offset %u (%u, %u)", roundIdx, errCount, desOpModeString[modeIdx&0x3], pktLen, offset, doneCounter, allDoneCounter);
#if 0
		printf("cryptGenKey: ");
	for(k=0; k<24; k++)
		printf("%02x",cryptGenKey[k]);
printf("\n");
		printf("cryptGenIv:");
	for(k=0; k<8; k++) 
		printf("%02x",cryptGenIv[k]);
printf("\n");
#endif
					if(des8651bGeneralApiTestItem(modeIdx&0x3, &cryptGenAsic[offset], pktLen, cryptGenKey, cryptGenIv) == FAILED) 
						errCount++;
				}
			}
		}
	}
				
	UNCACHED_FREE(cryptGenAsic);
	rtlglue_free(cryptGenDecrypt);
	rtlglue_free(cryptGenEncrypt);
	rtlglue_free(cryptGenOrg);
	rtlglue_printf("\nGeneral API Test Finished. errCount=%d\n", errCount );
	if ( errCount > 0 ) ret = FAILED;
	return ret;	
}

/****************************************
 * By 'crypto crypto rand' command      *
 *   random key/iv/mode/len/offset/data *
 ****************************************/
int32 runDes8651bGeneralApiRandTest(uint32 seed, uint32 round) {
	uint32 roundIdx, modeIdx, pktLen,  i, offset, errCount;
	uint32 doneCounter, allDoneCounter;
	int32 ret = SUCCESS;

	cryptGenKey = UNCACHED_ADDRESS( _cryptGenKey );
	cryptGenIv = UNCACHED_ADDRESS( _cryptGenIv );
	cryptGenOrg = (int8 *) rtlglue_malloc(MAX_PKTLEN+8/*for alignment issue*/);
	cryptGenEncrypt = (int8 *) rtlglue_malloc(MAX_PKTLEN+8/*for alignment issue*/);
	cryptGenDecrypt = (int8 *) rtlglue_malloc(MAX_PKTLEN+8/*for alignment issue*/);
	cryptGenAsic = (int8 *) UNCACHED_MALLOC(MAX_PKTLEN+8/*for alignment issue*/);
	rtlglue_srandom(seed);
	errCount = 0;

#ifdef CONFIG_RTL8316S
mips_cache_flush_all();
#else
_cache_flush_all();
#endif

	for(roundIdx = 0; roundIdx<round; roundIdx++) {
		for(i=0; i<24; i++)
			cryptGenKey[i] = rtlglue_random()&0xFF;
		for(i=0; i<8; i++)
			cryptGenIv[i] = rtlglue_random()&0xFF;
		modeIdx = rtlglue_random()&0x3;
		pktLen = ((rtlglue_random()%(MAX_PKTLEN-16))&0x3FF8) + 8; /* 8 ~ 16376 */
		offset = rtlglue_random()&0x7;
		for(i=0; i<pktLen; i++)//8-byte for IV and 4-byte for DMA_addr (0-3) byte offset test
			cryptGenAsic[i+offset] = rtlglue_random()&0xFF;
		rtl8651b_cryptoEngineGetIntCounter(&doneCounter, &allDoneCounter);
		rtlglue_printf("\r[%05u, %03u]Mode: %s Packet length %04u Offset %u (%u, %u)", roundIdx, errCount, desOpModeString[modeIdx&0x3], pktLen, offset, doneCounter, allDoneCounter);
		if(des8651bGeneralApiTestItem(modeIdx&0x3, &cryptGenAsic[offset], pktLen, cryptGenKey, cryptGenIv) == FAILED) 
			errCount++;
	}
	UNCACHED_FREE(cryptGenAsic);
	rtlglue_free(cryptGenDecrypt);
	rtlglue_free(cryptGenEncrypt);
	rtlglue_free(cryptGenOrg);
	rtlglue_printf("\nGeneral API Test Finished. errCount=%d\n", errCount );
	if ( errCount > 0 ) ret = FAILED;
	return ret;	
}



