/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003  
* All rights reserved.
* 
* Program : Simulate DES behavior and providing final DES driver interface
* Abstract : 
* $Id: desSim.c,v 1.1 2005/11/01 03:22:38 yjlou Exp $
* $Log: desSim.c,v $
* Revision 1.1  2005/11/01 03:22:38  yjlou
* *** empty log message ***
*
* Revision 1.3  2005/09/08 14:07:00  yjlou
* *: fixed the porting bugS of software DES in re865x/crypto: We always use Linux kernel's DES library.
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
* Revision 1.5  2003/09/25 10:00:13  jzchen
* Batch simulator
*
* Revision 1.4  2003/09/23 02:21:21  jzchen
* Provide generic API simulator
*
* Revision 1.3  2003/08/28 14:30:25  jzchen
* Change embedded IV function definition to meet ASIC behavior
*
* Revision 1.2  2003/08/28 02:51:11  jzchen
* Change DES_ENCRYPT and DES_DECRYPT define to TRUE and FALSE
*
* Revision 1.1  2003/08/22 11:52:03  jzchen
* Driver api simulation, include DES/3DES CBC/ECB and pass testing code
*
*
* --------------------------------------------------------------------
*/
#include <common.h>
#include "cryptoSim.h"

#include "hal_crypto.h"


// ------------- des.h --------------------
/* must be 32bit quantity */
#define DES_LONG uint32

typedef uint8 des_cblock[8];
typedef struct des_ks_struct
	{
	union	{
		des_cblock _;
		/* make sure things are correct size on machines with
		 * 8 byte longs */
		DES_LONG pad[2];
		} ks;
#undef _
#define _	ks._
	} des_key_schedule[16];

#define DES_KEY_SZ 	(sizeof(des_cblock))
#define DES_SCHEDULE_SZ (sizeof(des_key_schedule))

#define DES_ENCRYPT	1
#define DES_DECRYPT	0

#define DES_CBC_MODE	0
#define DES_PCBC_MODE	1

extern int32 des_check_key;	/* defaults to false */

char *des_options (void);
void des_ecb_encrypt (des_cblock *, des_cblock *,
	des_key_schedule, int32);
void des_encrypt (DES_LONG *, des_key_schedule, int32);
void des_encrypt2 (DES_LONG *, des_key_schedule, int32);

void des_set_odd_parity (uint8 *);
int des_is_weak_key (const u8 *);
int32 des_set_key (const u8* key, des_key_schedule);
int des_key_sched (u8 *, des_key_schedule);
void des_ecb3_encrypt( des_cblock *input, des_cblock *output,
		      des_key_schedule *ks1, des_key_schedule *ks2,
		      des_key_schedule *ks3,
	     int enc);
void des_ncbc_encrypt(const u8 *in, u8 *out, long length,
		     des_key_schedule *_schedule, des_cblock *ivec, int enc);
void des_ede3_cbc_encrypt(const unsigned char *input, unsigned char *output,
			  long length, des_key_schedule *ks1,
			  des_key_schedule *ks2, des_key_schedule *ks3,
			  des_cblock *ivec, int enc);
//void des_ede3_cbcm_encrypt(const unsigned char *in, unsigned char *out,
//             long length, des_key_schedule ks1, des_key_schedule ks2,
//             des_key_schedule ks3, des_cblock *ivec1, des_cblock *ivec2,
//             int enc);
//void des_xcbc_encrypt( des_cblock *_in,des_cblock *_out,
//                      long length, des_key_schedule schedule,
//                      des_cblock *ivec, des_cblock *inw,
//                      des_cblock *outw, int enc);

#define des_ecb2_encrypt(i,o,k1,k2,e) \
	des_ecb3_encrypt((i),(o),(k1),(k2),(k1),(e))



//-----------------des_locl.h------------

#undef DES_PTR

#ifdef __STDC__
#undef NOPROTO
#endif

#define ITERATIONS 16
#define HALF_ITERATIONS 8

/* used in des_read and des_write */
#define MAXWRITE	(1024*16)
#define BSIZE		(MAXWRITE+4)

#define c2l(c,l)	(l =((DES_LONG)(*((c)++)))    , \
			 l|=((DES_LONG)(*((c)++)))<< 8L, \
			 l|=((DES_LONG)(*((c)++)))<<16L, \
			 l|=((DES_LONG)(*((c)++)))<<24L)

/* NOTE - c is not incremented as per c2l */
#define c2ln(c,l1,l2,n)	{ \
			c+=n; \
			l1=l2=0; \
			switch (n) { \
			case 8: l2 =((DES_LONG)(*(--(c))))<<24L; \
			case 7: l2|=((DES_LONG)(*(--(c))))<<16L; \
			case 6: l2|=((DES_LONG)(*(--(c))))<< 8L; \
			case 5: l2|=((DES_LONG)(*(--(c))));     \
			case 4: l1 =((DES_LONG)(*(--(c))))<<24L; \
			case 3: l1|=((DES_LONG)(*(--(c))))<<16L; \
			case 2: l1|=((DES_LONG)(*(--(c))))<< 8L; \
			case 1: l1|=((DES_LONG)(*(--(c))));     \
				} \
			}

#define l2c(l,c)	(*((c)++)=(unsigned char)(((l)     )&0xff), \
			 *((c)++)=(unsigned char)(((l)>> 8L)&0xff), \
			 *((c)++)=(unsigned char)(((l)>>16L)&0xff), \
			 *((c)++)=(unsigned char)(((l)>>24L)&0xff))

/* replacements for htonl and ntohl since I have no idea what to do
 * when faced with machines with 8 byte longs. */
#define HDRSIZE 4

#define n2l(c,l)	(l =((DES_LONG)(*((c)++)))<<24L, \
			 l|=((DES_LONG)(*((c)++)))<<16L, \
			 l|=((DES_LONG)(*((c)++)))<< 8L, \
			 l|=((DES_LONG)(*((c)++))))

#define l2n(l,c)	(*((c)++)=(unsigned char)(((l)>>24L)&0xff), \
			 *((c)++)=(unsigned char)(((l)>>16L)&0xff), \
			 *((c)++)=(unsigned char)(((l)>> 8L)&0xff), \
			 *((c)++)=(unsigned char)(((l)     )&0xff))

/* NOTE - c is not incremented as per l2c */
#define l2cn(l1,l2,c,n)	{ \
			c+=n; \
			switch (n) { \
			case 8: *(--(c))=(unsigned char)(((l2)>>24L)&0xff); \
			case 7: *(--(c))=(unsigned char)(((l2)>>16L)&0xff); \
			case 6: *(--(c))=(unsigned char)(((l2)>> 8L)&0xff); \
			case 5: *(--(c))=(unsigned char)(((l2)     )&0xff); \
			case 4: *(--(c))=(unsigned char)(((l1)>>24L)&0xff); \
			case 3: *(--(c))=(unsigned char)(((l1)>>16L)&0xff); \
			case 2: *(--(c))=(unsigned char)(((l1)>> 8L)&0xff); \
			case 1: *(--(c))=(unsigned char)(((l1)     )&0xff); \
				} \
			}

#define	ROTATE(a,n)	(((a)>>(n))+((a)<<(32-(n))))

/* The changes to this macro may help or hinder, depending on the
 * compiler and the achitecture.  gcc2 always seems to do well :-).
 * Inspired by Dana How <how@isl.stanford.edu>
 * DO NOT use the alternative version on machines with 8 byte longs.
 * It does not seem to work on the Alpha, even when DES_LONG is 4
 * bytes, probably an issue of accessing non-word aligned objects :-( */
#ifdef DES_PTR

#define D_ENCRYPT(L,R,S) { \
	u=((R^s[S  ])<<2);	\
	t= R^s[S+1]; \
	t=ROTATE(t,2); \
	L^= (\
	*(DES_LONG *)((unsigned char *)des_SP+0x100+((t    )&0xfc))+ \
	*(DES_LONG *)((unsigned char *)des_SP+0x300+((t>> 8)&0xfc))+ \
	*(DES_LONG *)((unsigned char *)des_SP+0x500+((t>>16)&0xfc))+ \
	*(DES_LONG *)((unsigned char *)des_SP+0x700+((t>>24)&0xfc))+ \
	*(DES_LONG *)((unsigned char *)des_SP      +((u    )&0xfc))+ \
	*(DES_LONG *)((unsigned char *)des_SP+0x200+((u>> 8)&0xfc))+ \
	*(DES_LONG *)((unsigned char *)des_SP+0x400+((u>>16)&0xfc))+ \
	*(DES_LONG *)((unsigned char *)des_SP+0x600+((u>>24)&0xfc))); }
#else /* original version */
#ifdef undef
#define D_ENCRYPT(L,R,S)	\
	U.l=R^s[S+1]; \
	T.s[0]=((U.s[0]>>4)|(U.s[1]<<12))&0x3f3f; \
	T.s[1]=((U.s[1]>>4)|(U.s[0]<<12))&0x3f3f; \
	U.l=(R^s[S  ])&0x3f3f3f3fL; \
	L^=	des_SPtrans[1][(T.c[0])]| \
		des_SPtrans[3][(T.c[1])]| \
		des_SPtrans[5][(T.c[2])]| \
		des_SPtrans[7][(T.c[3])]| \
		des_SPtrans[0][(U.c[0])]| \
		des_SPtrans[2][(U.c[1])]| \
		des_SPtrans[4][(U.c[2])]| \
		des_SPtrans[6][(U.c[3])];
#else
#define D_ENCRYPT(Q,R,S) {\
	u=(R^s[S  ]); \
	t=R^s[S+1]; \
	t=ROTATE(t,4); \
	Q^=	des_SPtrans[1][(t     )&0x3f]| \
		des_SPtrans[3][(t>> 8L)&0x3f]| \
		des_SPtrans[5][(t>>16L)&0x3f]| \
		des_SPtrans[7][(t>>24L)&0x3f]| \
		des_SPtrans[0][(u     )&0x3f]| \
		des_SPtrans[2][(u>> 8L)&0x3f]| \
		des_SPtrans[4][(u>>16L)&0x3f]| \
		des_SPtrans[6][(u>>24L)&0x3f]; }
#endif
#endif

	/* IP and FP
	 * The problem is more of a geometric problem that random bit fiddling.
	 0  1  2  3  4  5  6  7      62 54 46 38 30 22 14  6
	 8  9 10 11 12 13 14 15      60 52 44 36 28 20 12  4
	16 17 18 19 20 21 22 23      58 50 42 34 26 18 10  2
	24 25 26 27 28 29 30 31  to  56 48 40 32 24 16  8  0

	32 33 34 35 36 37 38 39      63 55 47 39 31 23 15  7
	40 41 42 43 44 45 46 47      61 53 45 37 29 21 13  5
	48 49 50 51 52 53 54 55      59 51 43 35 27 19 11  3
	56 57 58 59 60 61 62 63      57 49 41 33 25 17  9  1

	The output has been subject to swaps of the form
	0 1 -> 3 1 but the odd and even bits have been put into
	2 3    2 0
	different words.  The main trick is to remember that
	t=((l>>size)^r)&(mask);
	r^=t;
	l^=(t<<size);
	can be used to swap and move bits between words.

	So l =  0  1  2  3  r = 16 17 18 19
	        4  5  6  7      20 21 22 23
	        8  9 10 11      24 25 26 27
	       12 13 14 15      28 29 30 31
	becomes (for size == 2 and mask == 0x3333)
	   t =   2^16  3^17 -- --   l =  0  1 16 17  r =  2  3 18 19
		 6^20  7^21 -- --        4  5 20 21       6  7 22 23
		10^24 11^25 -- --        8  9 24 25      10 11 24 25
		14^28 15^29 -- --       12 13 28 29      14 15 28 29

	Thanks for hints from Richard Outerbridge - he told me IP&FP
	could be done in 15 xor, 10 shifts and 5 ands.
	When I finally started to think of the problem in 2D
	I first got ~42 operations without xors.  When I remembered
	how to use xors :-) I got it to its final state.
	*/
#define PERM_OP(a,b,t,n,m) ((t)=((((a)>>(n))^(b))&(m)),\
	(b)^=(t),\
	(a)^=((t)<<(n)))

#define IP(l,r) \
	{ \
	register DES_LONG tt; \
	PERM_OP(r,l,tt, 4,0x0f0f0f0fL); \
	PERM_OP(l,r,tt,16,0x0000ffffL); \
	PERM_OP(r,l,tt, 2,0x33333333L); \
	PERM_OP(l,r,tt, 8,0x00ff00ffL); \
	PERM_OP(r,l,tt, 1,0x55555555L); \
	}

#define FP(l,r) \
	{ \
	register DES_LONG tt; \
	PERM_OP(l,r,tt, 1,0x55555555L); \
	PERM_OP(r,l,tt, 8,0x00ff00ffL); \
	PERM_OP(l,r,tt, 2,0x33333333L); \
	PERM_OP(r,l,tt,16,0x0000ffffL); \
	PERM_OP(l,r,tt, 4,0x0f0f0f0fL); \
	}
 
// ----------------- podd.h --------------
static const uint8 odd_parity[256]={
  1,  1,  2,  2,  4,  4,  7,  7,  8,  8, 11, 11, 13, 13, 14, 14,
 16, 16, 19, 19, 21, 21, 22, 22, 25, 25, 26, 26, 28, 28, 31, 31,
 32, 32, 35, 35, 37, 37, 38, 38, 41, 41, 42, 42, 44, 44, 47, 47,
 49, 49, 50, 50, 52, 52, 55, 55, 56, 56, 59, 59, 61, 61, 62, 62,
 64, 64, 67, 67, 69, 69, 70, 70, 73, 73, 74, 74, 76, 76, 79, 79,
 81, 81, 82, 82, 84, 84, 87, 87, 88, 88, 91, 91, 93, 93, 94, 94,
 97, 97, 98, 98,100,100,103,103,104,104,107,107,109,109,110,110,
112,112,115,115,117,117,118,118,121,121,122,122,124,124,127,127,
128,128,131,131,133,133,134,134,137,137,138,138,140,140,143,143,
145,145,146,146,148,148,151,151,152,152,155,155,157,157,158,158,
161,161,162,162,164,164,167,167,168,168,171,171,173,173,174,174,
176,176,179,179,181,181,182,182,185,185,186,186,188,188,191,191,
193,193,194,194,196,196,199,199,200,200,203,203,205,205,206,206,
208,208,211,211,213,213,214,214,217,217,218,218,220,220,223,223,
224,224,227,227,229,229,230,230,233,233,234,234,236,236,239,239,
241,241,242,242,244,244,247,247,248,248,251,251,253,253,254,254};

// ------------------ sk.h ---------------
static const DES_LONG des_skb[8][64]={
{
/* for C bits (numbered as per FIPS 46) 1 2 3 4 5 6 */
0x00000000L,0x00000010L,0x20000000L,0x20000010L,
0x00010000L,0x00010010L,0x20010000L,0x20010010L,
0x00000800L,0x00000810L,0x20000800L,0x20000810L,
0x00010800L,0x00010810L,0x20010800L,0x20010810L,
0x00000020L,0x00000030L,0x20000020L,0x20000030L,
0x00010020L,0x00010030L,0x20010020L,0x20010030L,
0x00000820L,0x00000830L,0x20000820L,0x20000830L,
0x00010820L,0x00010830L,0x20010820L,0x20010830L,
0x00080000L,0x00080010L,0x20080000L,0x20080010L,
0x00090000L,0x00090010L,0x20090000L,0x20090010L,
0x00080800L,0x00080810L,0x20080800L,0x20080810L,
0x00090800L,0x00090810L,0x20090800L,0x20090810L,
0x00080020L,0x00080030L,0x20080020L,0x20080030L,
0x00090020L,0x00090030L,0x20090020L,0x20090030L,
0x00080820L,0x00080830L,0x20080820L,0x20080830L,
0x00090820L,0x00090830L,0x20090820L,0x20090830L,
},{
/* for C bits (numbered as per FIPS 46) 7 8 10 11 12 13 */
0x00000000L,0x02000000L,0x00002000L,0x02002000L,
0x00200000L,0x02200000L,0x00202000L,0x02202000L,
0x00000004L,0x02000004L,0x00002004L,0x02002004L,
0x00200004L,0x02200004L,0x00202004L,0x02202004L,
0x00000400L,0x02000400L,0x00002400L,0x02002400L,
0x00200400L,0x02200400L,0x00202400L,0x02202400L,
0x00000404L,0x02000404L,0x00002404L,0x02002404L,
0x00200404L,0x02200404L,0x00202404L,0x02202404L,
0x10000000L,0x12000000L,0x10002000L,0x12002000L,
0x10200000L,0x12200000L,0x10202000L,0x12202000L,
0x10000004L,0x12000004L,0x10002004L,0x12002004L,
0x10200004L,0x12200004L,0x10202004L,0x12202004L,
0x10000400L,0x12000400L,0x10002400L,0x12002400L,
0x10200400L,0x12200400L,0x10202400L,0x12202400L,
0x10000404L,0x12000404L,0x10002404L,0x12002404L,
0x10200404L,0x12200404L,0x10202404L,0x12202404L,
},{
/* for C bits (numbered as per FIPS 46) 14 15 16 17 19 20 */
0x00000000L,0x00000001L,0x00040000L,0x00040001L,
0x01000000L,0x01000001L,0x01040000L,0x01040001L,
0x00000002L,0x00000003L,0x00040002L,0x00040003L,
0x01000002L,0x01000003L,0x01040002L,0x01040003L,
0x00000200L,0x00000201L,0x00040200L,0x00040201L,
0x01000200L,0x01000201L,0x01040200L,0x01040201L,
0x00000202L,0x00000203L,0x00040202L,0x00040203L,
0x01000202L,0x01000203L,0x01040202L,0x01040203L,
0x08000000L,0x08000001L,0x08040000L,0x08040001L,
0x09000000L,0x09000001L,0x09040000L,0x09040001L,
0x08000002L,0x08000003L,0x08040002L,0x08040003L,
0x09000002L,0x09000003L,0x09040002L,0x09040003L,
0x08000200L,0x08000201L,0x08040200L,0x08040201L,
0x09000200L,0x09000201L,0x09040200L,0x09040201L,
0x08000202L,0x08000203L,0x08040202L,0x08040203L,
0x09000202L,0x09000203L,0x09040202L,0x09040203L,
},{
/* for C bits (numbered as per FIPS 46) 21 23 24 26 27 28 */
0x00000000L,0x00100000L,0x00000100L,0x00100100L,
0x00000008L,0x00100008L,0x00000108L,0x00100108L,
0x00001000L,0x00101000L,0x00001100L,0x00101100L,
0x00001008L,0x00101008L,0x00001108L,0x00101108L,
0x04000000L,0x04100000L,0x04000100L,0x04100100L,
0x04000008L,0x04100008L,0x04000108L,0x04100108L,
0x04001000L,0x04101000L,0x04001100L,0x04101100L,
0x04001008L,0x04101008L,0x04001108L,0x04101108L,
0x00020000L,0x00120000L,0x00020100L,0x00120100L,
0x00020008L,0x00120008L,0x00020108L,0x00120108L,
0x00021000L,0x00121000L,0x00021100L,0x00121100L,
0x00021008L,0x00121008L,0x00021108L,0x00121108L,
0x04020000L,0x04120000L,0x04020100L,0x04120100L,
0x04020008L,0x04120008L,0x04020108L,0x04120108L,
0x04021000L,0x04121000L,0x04021100L,0x04121100L,
0x04021008L,0x04121008L,0x04021108L,0x04121108L,
},{
/* for D bits (numbered as per FIPS 46) 1 2 3 4 5 6 */
0x00000000L,0x10000000L,0x00010000L,0x10010000L,
0x00000004L,0x10000004L,0x00010004L,0x10010004L,
0x20000000L,0x30000000L,0x20010000L,0x30010000L,
0x20000004L,0x30000004L,0x20010004L,0x30010004L,
0x00100000L,0x10100000L,0x00110000L,0x10110000L,
0x00100004L,0x10100004L,0x00110004L,0x10110004L,
0x20100000L,0x30100000L,0x20110000L,0x30110000L,
0x20100004L,0x30100004L,0x20110004L,0x30110004L,
0x00001000L,0x10001000L,0x00011000L,0x10011000L,
0x00001004L,0x10001004L,0x00011004L,0x10011004L,
0x20001000L,0x30001000L,0x20011000L,0x30011000L,
0x20001004L,0x30001004L,0x20011004L,0x30011004L,
0x00101000L,0x10101000L,0x00111000L,0x10111000L,
0x00101004L,0x10101004L,0x00111004L,0x10111004L,
0x20101000L,0x30101000L,0x20111000L,0x30111000L,
0x20101004L,0x30101004L,0x20111004L,0x30111004L,
},{
/* for D bits (numbered as per FIPS 46) 8 9 11 12 13 14 */
0x00000000L,0x08000000L,0x00000008L,0x08000008L,
0x00000400L,0x08000400L,0x00000408L,0x08000408L,
0x00020000L,0x08020000L,0x00020008L,0x08020008L,
0x00020400L,0x08020400L,0x00020408L,0x08020408L,
0x00000001L,0x08000001L,0x00000009L,0x08000009L,
0x00000401L,0x08000401L,0x00000409L,0x08000409L,
0x00020001L,0x08020001L,0x00020009L,0x08020009L,
0x00020401L,0x08020401L,0x00020409L,0x08020409L,
0x02000000L,0x0A000000L,0x02000008L,0x0A000008L,
0x02000400L,0x0A000400L,0x02000408L,0x0A000408L,
0x02020000L,0x0A020000L,0x02020008L,0x0A020008L,
0x02020400L,0x0A020400L,0x02020408L,0x0A020408L,
0x02000001L,0x0A000001L,0x02000009L,0x0A000009L,
0x02000401L,0x0A000401L,0x02000409L,0x0A000409L,
0x02020001L,0x0A020001L,0x02020009L,0x0A020009L,
0x02020401L,0x0A020401L,0x02020409L,0x0A020409L,
},{
/* for D bits (numbered as per FIPS 46) 16 17 18 19 20 21 */
0x00000000L,0x00000100L,0x00080000L,0x00080100L,
0x01000000L,0x01000100L,0x01080000L,0x01080100L,
0x00000010L,0x00000110L,0x00080010L,0x00080110L,
0x01000010L,0x01000110L,0x01080010L,0x01080110L,
0x00200000L,0x00200100L,0x00280000L,0x00280100L,
0x01200000L,0x01200100L,0x01280000L,0x01280100L,
0x00200010L,0x00200110L,0x00280010L,0x00280110L,
0x01200010L,0x01200110L,0x01280010L,0x01280110L,
0x00000200L,0x00000300L,0x00080200L,0x00080300L,
0x01000200L,0x01000300L,0x01080200L,0x01080300L,
0x00000210L,0x00000310L,0x00080210L,0x00080310L,
0x01000210L,0x01000310L,0x01080210L,0x01080310L,
0x00200200L,0x00200300L,0x00280200L,0x00280300L,
0x01200200L,0x01200300L,0x01280200L,0x01280300L,
0x00200210L,0x00200310L,0x00280210L,0x00280310L,
0x01200210L,0x01200310L,0x01280210L,0x01280310L,
},{
/* for D bits (numbered as per FIPS 46) 22 23 24 25 27 28 */
0x00000000L,0x04000000L,0x00040000L,0x04040000L,
0x00000002L,0x04000002L,0x00040002L,0x04040002L,
0x00002000L,0x04002000L,0x00042000L,0x04042000L,
0x00002002L,0x04002002L,0x00042002L,0x04042002L,
0x00000020L,0x04000020L,0x00040020L,0x04040020L,
0x00000022L,0x04000022L,0x00040022L,0x04040022L,
0x00002020L,0x04002020L,0x00042020L,0x04042020L,
0x00002022L,0x04002022L,0x00042022L,0x04042022L,
0x00000800L,0x04000800L,0x00040800L,0x04040800L,
0x00000802L,0x04000802L,0x00040802L,0x04040802L,
0x00002800L,0x04002800L,0x00042800L,0x04042800L,
0x00002802L,0x04002802L,0x00042802L,0x04042802L,
0x00000820L,0x04000820L,0x00040820L,0x04040820L,
0x00000822L,0x04000822L,0x00040822L,0x04040822L,
0x00002820L,0x04002820L,0x00042820L,0x04042820L,
0x00002822L,0x04002822L,0x00042822L,0x04042822L,
}};

// -------------------------- spr.h ---------------


static const DES_LONG des_SPtrans[8][64]={
{
/* nibble 0 */
0x00820200L, 0x00020000L, 0x80800000L, 0x80820200L,
0x00800000L, 0x80020200L, 0x80020000L, 0x80800000L,
0x80020200L, 0x00820200L, 0x00820000L, 0x80000200L,
0x80800200L, 0x00800000L, 0x00000000L, 0x80020000L,
0x00020000L, 0x80000000L, 0x00800200L, 0x00020200L,
0x80820200L, 0x00820000L, 0x80000200L, 0x00800200L,
0x80000000L, 0x00000200L, 0x00020200L, 0x80820000L,
0x00000200L, 0x80800200L, 0x80820000L, 0x00000000L,
0x00000000L, 0x80820200L, 0x00800200L, 0x80020000L,
0x00820200L, 0x00020000L, 0x80000200L, 0x00800200L,
0x80820000L, 0x00000200L, 0x00020200L, 0x80800000L,
0x80020200L, 0x80000000L, 0x80800000L, 0x00820000L,
0x80820200L, 0x00020200L, 0x00820000L, 0x80800200L,
0x00800000L, 0x80000200L, 0x80020000L, 0x00000000L,
0x00020000L, 0x00800000L, 0x80800200L, 0x00820200L,
0x80000000L, 0x80820000L, 0x00000200L, 0x80020200L,
},{
/* nibble 1 */
0x10042004L, 0x00000000L, 0x00042000L, 0x10040000L,
0x10000004L, 0x00002004L, 0x10002000L, 0x00042000L,
0x00002000L, 0x10040004L, 0x00000004L, 0x10002000L,
0x00040004L, 0x10042000L, 0x10040000L, 0x00000004L,
0x00040000L, 0x10002004L, 0x10040004L, 0x00002000L,
0x00042004L, 0x10000000L, 0x00000000L, 0x00040004L,
0x10002004L, 0x00042004L, 0x10042000L, 0x10000004L,
0x10000000L, 0x00040000L, 0x00002004L, 0x10042004L,
0x00040004L, 0x10042000L, 0x10002000L, 0x00042004L,
0x10042004L, 0x00040004L, 0x10000004L, 0x00000000L,
0x10000000L, 0x00002004L, 0x00040000L, 0x10040004L,
0x00002000L, 0x10000000L, 0x00042004L, 0x10002004L,
0x10042000L, 0x00002000L, 0x00000000L, 0x10000004L,
0x00000004L, 0x10042004L, 0x00042000L, 0x10040000L,
0x10040004L, 0x00040000L, 0x00002004L, 0x10002000L,
0x10002004L, 0x00000004L, 0x10040000L, 0x00042000L,
},{
/* nibble 2 */
0x41000000L, 0x01010040L, 0x00000040L, 0x41000040L,
0x40010000L, 0x01000000L, 0x41000040L, 0x00010040L,
0x01000040L, 0x00010000L, 0x01010000L, 0x40000000L,
0x41010040L, 0x40000040L, 0x40000000L, 0x41010000L,
0x00000000L, 0x40010000L, 0x01010040L, 0x00000040L,
0x40000040L, 0x41010040L, 0x00010000L, 0x41000000L,
0x41010000L, 0x01000040L, 0x40010040L, 0x01010000L,
0x00010040L, 0x00000000L, 0x01000000L, 0x40010040L,
0x01010040L, 0x00000040L, 0x40000000L, 0x00010000L,
0x40000040L, 0x40010000L, 0x01010000L, 0x41000040L,
0x00000000L, 0x01010040L, 0x00010040L, 0x41010000L,
0x40010000L, 0x01000000L, 0x41010040L, 0x40000000L,
0x40010040L, 0x41000000L, 0x01000000L, 0x41010040L,
0x00010000L, 0x01000040L, 0x41000040L, 0x00010040L,
0x01000040L, 0x00000000L, 0x41010000L, 0x40000040L,
0x41000000L, 0x40010040L, 0x00000040L, 0x01010000L,
},{
/* nibble 3 */
0x00100402L, 0x04000400L, 0x00000002L, 0x04100402L,
0x00000000L, 0x04100000L, 0x04000402L, 0x00100002L,
0x04100400L, 0x04000002L, 0x04000000L, 0x00000402L,
0x04000002L, 0x00100402L, 0x00100000L, 0x04000000L,
0x04100002L, 0x00100400L, 0x00000400L, 0x00000002L,
0x00100400L, 0x04000402L, 0x04100000L, 0x00000400L,
0x00000402L, 0x00000000L, 0x00100002L, 0x04100400L,
0x04000400L, 0x04100002L, 0x04100402L, 0x00100000L,
0x04100002L, 0x00000402L, 0x00100000L, 0x04000002L,
0x00100400L, 0x04000400L, 0x00000002L, 0x04100000L,
0x04000402L, 0x00000000L, 0x00000400L, 0x00100002L,
0x00000000L, 0x04100002L, 0x04100400L, 0x00000400L,
0x04000000L, 0x04100402L, 0x00100402L, 0x00100000L,
0x04100402L, 0x00000002L, 0x04000400L, 0x00100402L,
0x00100002L, 0x00100400L, 0x04100000L, 0x04000402L,
0x00000402L, 0x04000000L, 0x04000002L, 0x04100400L,
},{
/* nibble 4 */
0x02000000L, 0x00004000L, 0x00000100L, 0x02004108L,
0x02004008L, 0x02000100L, 0x00004108L, 0x02004000L,
0x00004000L, 0x00000008L, 0x02000008L, 0x00004100L,
0x02000108L, 0x02004008L, 0x02004100L, 0x00000000L,
0x00004100L, 0x02000000L, 0x00004008L, 0x00000108L,
0x02000100L, 0x00004108L, 0x00000000L, 0x02000008L,
0x00000008L, 0x02000108L, 0x02004108L, 0x00004008L,
0x02004000L, 0x00000100L, 0x00000108L, 0x02004100L,
0x02004100L, 0x02000108L, 0x00004008L, 0x02004000L,
0x00004000L, 0x00000008L, 0x02000008L, 0x02000100L,
0x02000000L, 0x00004100L, 0x02004108L, 0x00000000L,
0x00004108L, 0x02000000L, 0x00000100L, 0x00004008L,
0x02000108L, 0x00000100L, 0x00000000L, 0x02004108L,
0x02004008L, 0x02004100L, 0x00000108L, 0x00004000L,
0x00004100L, 0x02004008L, 0x02000100L, 0x00000108L,
0x00000008L, 0x00004108L, 0x02004000L, 0x02000008L,
},{
/* nibble 5 */
0x20000010L, 0x00080010L, 0x00000000L, 0x20080800L,
0x00080010L, 0x00000800L, 0x20000810L, 0x00080000L,
0x00000810L, 0x20080810L, 0x00080800L, 0x20000000L,
0x20000800L, 0x20000010L, 0x20080000L, 0x00080810L,
0x00080000L, 0x20000810L, 0x20080010L, 0x00000000L,
0x00000800L, 0x00000010L, 0x20080800L, 0x20080010L,
0x20080810L, 0x20080000L, 0x20000000L, 0x00000810L,
0x00000010L, 0x00080800L, 0x00080810L, 0x20000800L,
0x00000810L, 0x20000000L, 0x20000800L, 0x00080810L,
0x20080800L, 0x00080010L, 0x00000000L, 0x20000800L,
0x20000000L, 0x00000800L, 0x20080010L, 0x00080000L,
0x00080010L, 0x20080810L, 0x00080800L, 0x00000010L,
0x20080810L, 0x00080800L, 0x00080000L, 0x20000810L,
0x20000010L, 0x20080000L, 0x00080810L, 0x00000000L,
0x00000800L, 0x20000010L, 0x20000810L, 0x20080800L,
0x20080000L, 0x00000810L, 0x00000010L, 0x20080010L,
},{
/* nibble 6 */
0x00001000L, 0x00000080L, 0x00400080L, 0x00400001L,
0x00401081L, 0x00001001L, 0x00001080L, 0x00000000L,
0x00400000L, 0x00400081L, 0x00000081L, 0x00401000L,
0x00000001L, 0x00401080L, 0x00401000L, 0x00000081L,
0x00400081L, 0x00001000L, 0x00001001L, 0x00401081L,
0x00000000L, 0x00400080L, 0x00400001L, 0x00001080L,
0x00401001L, 0x00001081L, 0x00401080L, 0x00000001L,
0x00001081L, 0x00401001L, 0x00000080L, 0x00400000L,
0x00001081L, 0x00401000L, 0x00401001L, 0x00000081L,
0x00001000L, 0x00000080L, 0x00400000L, 0x00401001L,
0x00400081L, 0x00001081L, 0x00001080L, 0x00000000L,
0x00000080L, 0x00400001L, 0x00000001L, 0x00400080L,
0x00000000L, 0x00400081L, 0x00400080L, 0x00001080L,
0x00000081L, 0x00001000L, 0x00401081L, 0x00400000L,
0x00401080L, 0x00000001L, 0x00001001L, 0x00401081L,
0x00400001L, 0x00401080L, 0x00401000L, 0x00001001L,
},{
/* nibble 7 */
0x08200020L, 0x08208000L, 0x00008020L, 0x00000000L,
0x08008000L, 0x00200020L, 0x08200000L, 0x08208020L,
0x00000020L, 0x08000000L, 0x00208000L, 0x00008020L,
0x00208020L, 0x08008020L, 0x08000020L, 0x08200000L,
0x00008000L, 0x00208020L, 0x00200020L, 0x08008000L,
0x08208020L, 0x08000020L, 0x00000000L, 0x00208000L,
0x08000000L, 0x00200000L, 0x08008020L, 0x08200020L,
0x00200000L, 0x00008000L, 0x08208000L, 0x00000020L,
0x00200000L, 0x00008000L, 0x08000020L, 0x08208020L,
0x00008020L, 0x08000000L, 0x00000000L, 0x00208000L,
0x08200020L, 0x08008020L, 0x08008000L, 0x00200020L,
0x08208000L, 0x00000020L, 0x00200020L, 0x08008000L,
0x08208020L, 0x00200000L, 0x08200000L, 0x08000020L,
0x00208000L, 0x00008020L, 0x08008020L, 0x08200000L,
0x00000020L, 0x08208000L, 0x00208020L, 0x00000000L,
0x08000000L, 0x08200020L, 0x00008000L, 0x00208020L,
}};



//---------------------------------------

/* Weak and semi week keys as take from
 * %A D.W. Davies
 * %A W.L. Price
 * %T Security for Computer Networks
 * %I John Wiley & Sons
 * %D 1984
 * Many thanks to smb@ulysses.att.com (Steven Bellovin) for the reference
 * (and actual cblock values).
 */
#define NUM_WEAK_KEY    16
static int8 weak_keys[NUM_WEAK_KEY][8]={
    /* weak keys */
    {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01},
    {0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE},
    {0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F},
    {0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0},
    /* semi-weak keys */
    {0x01,0xFE,0x01,0xFE,0x01,0xFE,0x01,0xFE},
    {0xFE,0x01,0xFE,0x01,0xFE,0x01,0xFE,0x01},
    {0x1F,0xE0,0x1F,0xE0,0x0E,0xF1,0x0E,0xF1},
    {0xE0,0x1F,0xE0,0x1F,0xF1,0x0E,0xF1,0x0E},
    {0x01,0xE0,0x01,0xE0,0x01,0xF1,0x01,0xF1},
    {0xE0,0x01,0xE0,0x01,0xF1,0x01,0xF1,0x01},
    {0x1F,0xFE,0x1F,0xFE,0x0E,0xFE,0x0E,0xFE},
    {0xFE,0x1F,0xFE,0x1F,0xFE,0x0E,0xFE,0x0E},
    {0x01,0x1F,0x01,0x1F,0x01,0x0E,0x01,0x0E},
    {0x1F,0x01,0x1F,0x01,0x0E,0x01,0x0E,0x01},
    {0xE0,0xFE,0xE0,0xFE,0xF1,0xFE,0xF1,0xFE},
    {0xFE,0xE0,0xFE,0xE0,0xFE,0xF1,0xFE,0xF1}};


int32 des_check_key=0;

void des_set_odd_parity(uint8 *key) {
    int32 i;

    for (i=0; i<DES_KEY_SZ; i++)
        key[i]=odd_parity[key[i]];
}

static int32 check_parity(const uint8 *key) {
    int32 i;

    for (i=0; i<DES_KEY_SZ; i++) {
        if (key[i] != odd_parity[key[i]])
            return(0);
    }
    return(1);
}


int des_is_weak_key(const u8 *key) {
    int32 i;

    for (i=0; i<NUM_WEAK_KEY; i++)
        /* Added == 0 to comparision, I obviously don't run
         * this section very often :-(, thanks to
         * engineering@MorningStar.Com for the fix
         * eay 93/06/29 */
        if ( rtl_memcmpb((const u8*)(weak_keys[i]),key,sizeof(*key)) == 0) return(1);
    return(0);
}

/* NOW DEFINED IN des_local.h
 * See ecb_encrypt.c for a pseudo description of these macros.
 * #define PERM_OP(a,b,t,n,m) ((t)=((((a)>>(n))^(b))&(m)),\
 *  (b)^=(t),\
 *  (a)=((a)^((t)<<(n))))
 */

#define HPERM_OP(a,t,n,m) ((t)=((((a)<<(16-(n)))^(a))&(m)),\
    (a)=(a)^(t)^(t>>(16-(n))))

/* return 0 if key parity is odd (correct),
 * return -1 if key parity error,
 * return -2 if illegal weak key.
 */
int des_set_key(const u8 * key, des_key_schedule schedule) {
    static int32 shifts2[16]={0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0};
    DES_LONG c,d,t,s;
    uint8 *in;
    DES_LONG *k;
    int32 i;

    if (des_check_key)
        {
        if (!check_parity(key))
            return(-1);

        if (des_is_weak_key(key))
            return(-2);
        }

    k=(DES_LONG *)schedule;
    in=(uint8 *)key;

    c2l(in,c);
    c2l(in,d);

    /* do PC1 in 60 simple operations */
/*  PERM_OP(d,c,t,4,0x0f0f0f0fL);
    HPERM_OP(c,t,-2, 0xcccc0000L);
    HPERM_OP(c,t,-1, 0xaaaa0000L);
    HPERM_OP(c,t, 8, 0x00ff0000L);
    HPERM_OP(c,t,-1, 0xaaaa0000L);
    HPERM_OP(d,t,-8, 0xff000000L);
    HPERM_OP(d,t, 8, 0x00ff0000L);
    HPERM_OP(d,t, 2, 0x33330000L);
    d=((d&0x00aa00aaL)<<7L)|((d&0x55005500L)>>7L)|(d&0xaa55aa55L);
    d=(d>>8)|((c&0xf0000000L)>>4);
    c&=0x0fffffffL; */

    /* I now do it in 47 simple operations :-)
     * Thanks to John Fletcher (john_fletcher@lccmail.ocf.llnl.gov)
     * for the inspiration. :-) */
    PERM_OP (d,c,t,4,0x0f0f0f0fL);
    HPERM_OP(c,t,-2,0xcccc0000L);
    HPERM_OP(d,t,-2,0xcccc0000L);
    PERM_OP (d,c,t,1,0x55555555L);
    PERM_OP (c,d,t,8,0x00ff00ffL);
    PERM_OP (d,c,t,1,0x55555555L);
    d=  (((d&0x000000ffL)<<16L)| (d&0x0000ff00L)     |
         ((d&0x00ff0000L)>>16L)|((c&0xf0000000L)>>4L));
    c&=0x0fffffffL;

    for (i=0; i<ITERATIONS; i++)
        {
        if (shifts2[i])
            { c=((c>>2L)|(c<<26L)); d=((d>>2L)|(d<<26L)); }
        else
            { c=((c>>1L)|(c<<27L)); d=((d>>1L)|(d<<27L)); }
        c&=0x0fffffffL;
        d&=0x0fffffffL;
        /* could be a few less shifts but I am to lazy at this
         * point in time to investigate */
        s=  des_skb[0][ (c    )&0x3f                ]|
            des_skb[1][((c>> 6)&0x03)|((c>> 7L)&0x3c)]|
            des_skb[2][((c>>13)&0x0f)|((c>>14L)&0x30)]|
            des_skb[3][((c>>20)&0x01)|((c>>21L)&0x06) |
                          ((c>>22L)&0x38)];
        t=  des_skb[4][ (d    )&0x3f                ]|
            des_skb[5][((d>> 7L)&0x03)|((d>> 8L)&0x3c)]|
            des_skb[6][ (d>>15L)&0x3f                ]|
            des_skb[7][((d>>21L)&0x0f)|((d>>22L)&0x30)];

        /* table contained 0213 4657 */
        *(k++)=((t<<16L)|(s&0x0000ffffL))&0xffffffffL;
        s=     ((s>>16L)|(t&0xffff0000L));
        
        s=(s<<4L)|(s>>28L);
        *(k++)=s&0xffffffffL;
        }
    return(0);
    }

int des_key_sched(u8 * key, des_key_schedule schedule) {
    return(des_set_key(key,schedule));
}


// ---------------------------------------------------

int8 *libdes_version="libdes v 3.24 - 20-Apr-1996 - eay";
int8 *DES_version="DES part of SSLeay 0.6.4 30-Aug-1996";

int8 *des_options()
    {
#ifdef DES_PTR
    if (sizeof(DES_LONG) != sizeof(long))
        return("des(ptr,int32)");
    else
        return("des(ptr,long)");
#else
    if (sizeof(DES_LONG) != sizeof(long))
        return("des(idx,int32)");
    else
        return("des(idx,long)");
#endif
    }
        

void des_ecb_encrypt(input, output, ks, encrypt)
des_cblock (*input);
des_cblock (*output);
des_key_schedule ks;
int32 encrypt;
    {
    DES_LONG l;
    uint8 *in,*out;
    DES_LONG ll[2];

    in=(uint8 *)input;
    out=(uint8 *)output;
    c2l(in,l); ll[0]=l;
    c2l(in,l); ll[1]=l;
    des_encrypt(ll,ks,encrypt);
    l=ll[0]; l2c(l,out);
    l=ll[1]; l2c(l,out);
    l=ll[0]=ll[1]=0;
    }

void des_encrypt(data, ks, encrypt)
DES_LONG *data;
des_key_schedule ks;
int32 encrypt;
    {
    DES_LONG l,r,t,u;
#ifdef DES_PTR
    uint8 *des_SP=(uint8 *)des_SPtrans;
#endif
#ifdef undef
    union fudge {
        DES_LONG  l;
        uint16 s[2];
        uint8  c[4];
        } U,T;
#endif
    int32 i;
    DES_LONG *s;

    u=data[0];
    r=data[1];

    IP(u,r);
    /* Things have been modified so that the initial rotate is
     * done outside the loop.  This required the
     * des_SPtrans values in sp.h to be rotated 1 bit to the right.
     * One perl script later and things have a 5% speed up on a sparc2.
     * Thanks to Richard Outerbridge <71755.204@CompuServe.COM>
     * for pointing this out. */
    l=(r<<1)|(r>>31);
    r=(u<<1)|(u>>31);

    /* clear the top bits on machines with 8byte longs */
    l&=0xffffffffL;
    r&=0xffffffffL;

    s=(DES_LONG *)ks;
    /* I don't know if it is worth the effort of loop unrolling the
     * inner loop
     */
    if (encrypt)
        {
        for (i=0; i<32; i+=8)
            {
            D_ENCRYPT(l,r,i+0); /*  1 */
            D_ENCRYPT(r,l,i+2); /*  2 */
            D_ENCRYPT(l,r,i+4); /*  3 */
            D_ENCRYPT(r,l,i+6); /*  4 */
            }
        }
    else
        {
        for (i=30; i>0; i-=8)
            {
            D_ENCRYPT(l,r,i-0); /* 16 */
            D_ENCRYPT(r,l,i-2); /* 15 */
            D_ENCRYPT(l,r,i-4); /* 14 */
            D_ENCRYPT(r,l,i-6); /* 13 */
            }
        }
    l=(l>>1)|(l<<31);
    r=(r>>1)|(r<<31);
    /* clear the top bits on machines with 8byte longs */
    l&=0xffffffffL;
    r&=0xffffffffL;

    FP(r,l);
    data[0]=l;
    data[1]=r;
    l=r=t=u=0;
    }

void des_encrypt2(data, ks, encrypt)
DES_LONG *data;
des_key_schedule ks;
int32 encrypt;
    {
    DES_LONG l,r,t,u;
#ifdef DES_PTR
    uint8 *des_SP=(uint8 *)des_SPtrans;
#endif
#ifdef undef
    union fudge {
        DES_LONG  l;
        uint16 s[2];
        uint8  c[4];
        } U,T;
#endif
    int32 i;
    DES_LONG *s;

    u=data[0];
    r=data[1];

    /* Things have been modified so that the initial rotate is
     * done outside the loop.  This required the
     * des_SPtrans values in sp.h to be rotated 1 bit to the right.
     * One perl script later and things have a 5% speed up on a sparc2.
     * Thanks to Richard Outerbridge <71755.204@CompuServe.COM>
     * for pointing this out. */
    l=(r<<1)|(r>>31);
    r=(u<<1)|(u>>31);

    /* clear the top bits on machines with 8byte longs */
    l&=0xffffffffL;
    r&=0xffffffffL;

    s=(DES_LONG *)ks;
    /* I don't know if it is worth the effort of loop unrolling the
     * inner loop */
    if (encrypt)
        {
        for (i=0; i<32; i+=8)
            {
            D_ENCRYPT(l,r,i+0); /*  1 */
            D_ENCRYPT(r,l,i+2); /*  2 */
            D_ENCRYPT(l,r,i+4); /*  3 */
            D_ENCRYPT(r,l,i+6); /*  4 */
            }
        }
    else
        {
        for (i=30; i>0; i-=8)
            {
            D_ENCRYPT(l,r,i-0); /* 16 */
            D_ENCRYPT(r,l,i-2); /* 15 */
            D_ENCRYPT(l,r,i-4); /* 14 */
            D_ENCRYPT(r,l,i-6); /* 13 */
            }
        }
    l=(l>>1)|(l<<31);
    r=(r>>1)|(r<<31);
    /* clear the top bits on machines with 8byte longs */
    l&=0xffffffffL;
    r&=0xffffffffL;

    data[0]=l;
    data[1]=r;
    l=r=t=u=0;
    }

void DES_encrypt3(DES_LONG *data, des_key_schedule *ks1,
          des_key_schedule *ks2, des_key_schedule *ks3)
    {
    register DES_LONG l,r;

    l=data[0];
    r=data[1];
    IP(l,r);
    data[0]=l;
    data[1]=r;
    des_encrypt2((DES_LONG *)data,*ks1,DES_ENCRYPT);
    des_encrypt2((DES_LONG *)data,*ks2,DES_DECRYPT);
    des_encrypt2((DES_LONG *)data,*ks3,DES_ENCRYPT);
    l=data[0];
    r=data[1];
    FP(r,l);
    data[0]=l;
    data[1]=r;
}

void DES_decrypt3(DES_LONG *data, des_key_schedule *ks1,
          des_key_schedule *ks2, des_key_schedule *ks3)
    {
    register DES_LONG l,r;

    l=data[0];
    r=data[1];
    IP(l,r);
    data[0]=l;
    data[1]=r;
    des_encrypt2((DES_LONG *)data,*ks3,DES_DECRYPT);
    des_encrypt2((DES_LONG *)data,*ks2,DES_ENCRYPT);
    des_encrypt2((DES_LONG *)data,*ks1,DES_DECRYPT);
    l=data[0];
    r=data[1];
    FP(r,l);
    data[0]=l;
    data[1]=r;
}

void des_ecb3_encrypt(des_cblock *input, des_cblock *output,
              des_key_schedule *ks1, des_key_schedule *ks2,
              des_key_schedule *ks3,
         int enc)
    {
    register DES_LONG l0,l1;
    DES_LONG ll[2];
    const unsigned char *in = &(*input)[0];
    unsigned char *out = &(*output)[0];

    c2l(in,l0);
    c2l(in,l1);
    ll[0]=l0;
    ll[1]=l1;
    if (enc)
        DES_encrypt3(ll,ks1,ks2,ks3);
    else
        DES_decrypt3(ll,ks1,ks2,ks3);
    l0=ll[0];
    l1=ll[1];
    l2c(l0,out);
    l2c(l1,out);
}

void des_ncbc_encrypt(const u8 *in, unsigned char *out, long length,
             des_key_schedule *_schedule, des_cblock *ivec, int enc)
    {
    register DES_LONG tin0,tin1;
    register DES_LONG tout0,tout1,xor0,xor1;
    register long l=length;
    DES_LONG tin[2];
    unsigned char *iv;

    iv = &(*ivec)[0];

    if (enc)
        {
        c2l(iv,tout0);
        c2l(iv,tout1);
        for (l-=8; l>=0; l-=8)
            {
            c2l(in,tin0);
            c2l(in,tin1);
            tin0^=tout0; tin[0]=tin0;
            tin1^=tout1; tin[1]=tin1;
            des_encrypt((DES_LONG *)tin,*_schedule,DES_ENCRYPT);
            tout0=tin[0]; l2c(tout0,out);
            tout1=tin[1]; l2c(tout1,out);
            }
        if (l != -8)
            {
            c2ln(in,tin0,tin1,l+8);
            tin0^=tout0; tin[0]=tin0;
            tin1^=tout1; tin[1]=tin1;
            des_encrypt((DES_LONG *)tin,*_schedule,DES_ENCRYPT);
            tout0=tin[0]; l2c(tout0,out);
            tout1=tin[1]; l2c(tout1,out);
            }
        iv = &(*ivec)[0];
        l2c(tout0,iv);
        l2c(tout1,iv);
        }
    else
        {
        c2l(iv,xor0);
        c2l(iv,xor1);
        for (l-=8; l>=0; l-=8)
            {
            c2l(in,tin0); tin[0]=tin0;
            c2l(in,tin1); tin[1]=tin1;
            des_encrypt((DES_LONG *)tin,*_schedule,DES_DECRYPT);
            tout0=tin[0]^xor0;
            tout1=tin[1]^xor1;
            l2c(tout0,out);
            l2c(tout1,out);
            xor0=tin0;
            xor1=tin1;
            }
        if (l != -8)
            {
            c2l(in,tin0); tin[0]=tin0;
            c2l(in,tin1); tin[1]=tin1;
            des_encrypt((DES_LONG *)tin,*_schedule,DES_DECRYPT);
            tout0=tin[0]^xor0;
            tout1=tin[1]^xor1;
            l2cn(tout0,tout1,out,l+8);
            xor0=tin0;
            xor1=tin1;
            }
        iv = &(*ivec)[0];
        l2c(xor0,iv);
        l2c(xor1,iv);
        }
    tin0=tin1=tout0=tout1=xor0=xor1=0;
    tin[0]=tin[1]=0;
}

 
unsigned char 
desx_white_in2out[256]={
	0xBD,0x56,0xEA,0xF2,0xA2,0xF1,0xAC,0x2A,0xB0,0x93,0xD1,0x9C,0x1B,0x33,0xFD,0xD0,
	0x30,0x04,0xB6,0xDC,0x7D,0xDF,0x32,0x4B,0xF7,0xCB,0x45,0x9B,0x31,0xBB,0x21,0x5A,
	0x41,0x9F,0xE1,0xD9,0x4A,0x4D,0x9E,0xDA,0xA0,0x68,0x2C,0xC3,0x27,0x5F,0x80,0x36,
	0x3E,0xEE,0xFB,0x95,0x1A,0xFE,0xCE,0xA8,0x34,0xA9,0x13,0xF0,0xA6,0x3F,0xD8,0x0C,
	0x78,0x24,0xAF,0x23,0x52,0xC1,0x67,0x17,0xF5,0x66,0x90,0xE7,0xE8,0x07,0xB8,0x60,
	0x48,0xE6,0x1E,0x53,0xF3,0x92,0xA4,0x72,0x8C,0x08,0x15,0x6E,0x86,0x00,0x84,0xFA,
	0xF4,0x7F,0x8A,0x42,0x19,0xF6,0xDB,0xCD,0x14,0x8D,0x50,0x12,0xBA,0x3C,0x06,0x4E,
	0xEC,0xB3,0x35,0x11,0xA1,0x88,0x8E,0x2B,0x94,0x99,0xB7,0x71,0x74,0xD3,0xE4,0xBF,
	0x3A,0xDE,0x96,0x0E,0xBC,0x0A,0xED,0x77,0xFC,0x37,0x6B,0x03,0x79,0x89,0x62,0xC6,
	0xD7,0xC0,0xD2,0x7C,0x6A,0x8B,0x22,0xA3,0x5B,0x05,0x5D,0x02,0x75,0xD5,0x61,0xE3,
	0x18,0x8F,0x55,0x51,0xAD,0x1F,0x0B,0x5E,0x85,0xE5,0xC2,0x57,0x63,0xCA,0x3D,0x6C,
	0xB4,0xC5,0xCC,0x70,0xB2,0x91,0x59,0x0D,0x47,0x20,0xC8,0x4F,0x58,0xE0,0x01,0xE2,
	0x16,0x38,0xC4,0x6F,0x3B,0x0F,0x65,0x46,0xBE,0x7E,0x2D,0x7B,0x82,0xF9,0x40,0xB5,
	0x1D,0x73,0xF8,0xEB,0x26,0xC7,0x87,0x97,0x25,0x54,0xB1,0x28,0xAA,0x98,0x9D,0xA5,
	0x64,0x6D,0x7A,0xD4,0x10,0x81,0x44,0xEF,0x49,0xD6,0xAE,0x2E,0xDD,0x76,0x5C,0x2F,
	0xA7,0x1C,0xC9,0x09,0x69,0x9A,0x83,0xCF,0x29,0x39,0xB9,0xE9,0x4C,0xFF,0x43,0xAB,
};

void des_xwhite_in2out(const des_cblock *des_key, const des_cblock *in_white,
         des_cblock *out_white)
    {
    int out0,out1;
    int i;
    const unsigned char *key = &(*des_key)[0];
    const unsigned char *in = &(*in_white)[0];
    unsigned char *out = &(*out_white)[0];

    out[0]=out[1]=out[2]=out[3]=out[4]=out[5]=out[6]=out[7]=0;
    out0=out1=0;
    for (i=0; i<8; i++)
        {
        out[i]=key[i]^desx_white_in2out[out0^out1];
        out0=out1;
        out1=(int)out[i&0x07];
        }

    out0=out[0];
    out1=out[i];
    for (i=0; i<8; i++)
        {
        out[i]=in[i]^desx_white_in2out[out0^out1];
        out0=out1;
        out1=(int)out[i&0x07];
        }
    }

#if 0
void des_xcbc_encrypt(des_cblock *_in, des_cblock *_out,
              long length, des_key_schedule schedule,
              des_cblock *ivec, des_cblock *inw,
              des_cblock *outw, int enc)
    {
    unsigned char *in = (unsigned char *) _in;
    unsigned char *out = (unsigned char *) _out;
    register DES_LONG tin0,tin1;
    register DES_LONG tout0,tout1,xor0,xor1;
    register DES_LONG inW0,inW1,outW0,outW1;
    register const unsigned char *in2;
    register long l=length;
    DES_LONG tin[2];
    unsigned char *iv;

    in2 = &(*inw)[0];
    c2l(in2,inW0);
    c2l(in2,inW1);
    in2 = &(*outw)[0];
    c2l(in2,outW0);
    c2l(in2,outW1);

    iv = &(*ivec)[0];

    if (enc)
        {
        c2l(iv,tout0);
        c2l(iv,tout1);
        for (l-=8; l>=0; l-=8)
            {
            c2l(in,tin0);
            c2l(in,tin1);
            tin0^=tout0^inW0; tin[0]=tin0;
            tin1^=tout1^inW1; tin[1]=tin1;
            des_encrypt(tin,schedule,DES_ENCRYPT);
            tout0=tin[0]^outW0; l2c(tout0, out);
            tout1=tin[1]^outW1; l2c(tout1, out);
            }
        if (l != -8)
            {
            c2ln(in,tin0,tin1,l+8);
            tin0^=tout0^inW0; tin[0]=tin0;
            tin1^=tout1^inW1; tin[1]=tin1;
            des_encrypt(tin,schedule,DES_ENCRYPT);
            tout0=tin[0]^outW0; l2c(tout0,out);
            tout1=tin[1]^outW1; l2c(tout1,out);
            }
        iv = &(*ivec)[0];
        l2c(tout0,iv);
        l2c(tout1,iv);
        }
    else
        {
        c2l(iv,xor0);
        c2l(iv,xor1);
        for (l-=8; l>0; l-=8)
            {
            c2l(in,tin0); tin[0]=tin0^outW0;
            c2l(in,tin1); tin[1]=tin1^outW1;
            des_encrypt(tin,schedule,DES_DECRYPT);
            tout0=tin[0]^xor0^inW0;
            tout1=tin[1]^xor1^inW1;
            l2c(tout0,out);
            l2c(tout1,out);
            xor0=tin0;
            xor1=tin1;
            }
        if (l != -8)
            {
            c2l(in,tin0); tin[0]=tin0^outW0;
            c2l(in,tin1); tin[1]=tin1^outW1;
            des_encrypt(tin,schedule,DES_DECRYPT);
            tout0=tin[0]^xor0^inW0;
            tout1=tin[1]^xor1^inW1;
            l2cn(tout0,tout1,out,l+8);
            xor0=tin0;
            xor1=tin1;
            }

        iv = &(*ivec)[0];
        l2c(xor0,iv);
        l2c(xor1,iv);
        }
    tin0=tin1=tout0=tout1=xor0=xor1=0;
    inW0=inW1=outW0=outW1=0;
    tin[0]=tin[1]=0;
    }
#endif

void des_ede3_cbc_encrypt(const unsigned char *input, unsigned char *output,
              long length, des_key_schedule *ks1,
              des_key_schedule *ks2, des_key_schedule *ks3,
              des_cblock *ivec, int enc)
    {
    register DES_LONG tin0,tin1;
    register DES_LONG tout0,tout1,xor0,xor1;
    register const unsigned char *in;
    unsigned char *out;
    register long l=length;
    DES_LONG tin[2];
    unsigned char *iv;


    in=input;
    out=output;
    iv = &(*ivec)[0];


    if (enc)
        {
        c2l(iv,tout0);
        c2l(iv,tout1);
        for (l-=8; l>=0; l-=8)
            {
            c2l(in,tin0);
            c2l(in,tin1);
            tin0^=tout0;
            tin1^=tout1;

            tin[0]=tin0;
            tin[1]=tin1;
            DES_encrypt3((DES_LONG *)tin,ks1,ks2,ks3);
            tout0=tin[0];
            tout1=tin[1];

            l2c(tout0,out);
            l2c(tout1,out);
            }
        if (l != -8)
            {
            c2ln(in,tin0,tin1,l+8);
            tin0^=tout0;
            tin1^=tout1;

            tin[0]=tin0;
            tin[1]=tin1;
            DES_encrypt3((DES_LONG *)tin,ks1,ks2,ks3);
            tout0=tin[0];
            tout1=tin[1];

            l2c(tout0,out);
            l2c(tout1,out);
            }
        iv = &(*ivec)[0];
        l2c(tout0,iv);
        l2c(tout1,iv);
        }
    else
        {
        register DES_LONG t0,t1;

        c2l(iv,xor0);
        c2l(iv,xor1);
        for (l-=8; l>=0; l-=8)
            {
            c2l(in,tin0);
            c2l(in,tin1);

            t0=tin0;
            t1=tin1;

            tin[0]=tin0;
            tin[1]=tin1;
            DES_decrypt3((DES_LONG *)tin,ks1,ks2,ks3);
            tout0=tin[0];
            tout1=tin[1];

            tout0^=xor0;
            tout1^=xor1;
            l2c(tout0,out);
            l2c(tout1,out);
            xor0=t0;
            xor1=t1;
            }
        if (l != -8)
            {
            c2l(in,tin0);
            c2l(in,tin1);
            
            t0=tin0;
            t1=tin1;

            tin[0]=tin0;
            tin[1]=tin1;
            DES_decrypt3((DES_LONG *)tin,ks1,ks2,ks3);
            tout0=tin[0];
            tout1=tin[1];
        
            tout0^=xor0;
            tout1^=xor1;
            l2cn(tout0,tout1,out,l+8);
            xor0=t0;
            xor1=t1;
            }

        iv = &(*ivec)[0];
        l2c(xor0,iv);
        l2c(xor1,iv);
        }
    tin0=tin1=tout0=tout1=xor0=xor1=0;
    tin[0]=tin[1]=0;
    }

#if 0
void des_ede3_cbcm_encrypt(const unsigned char *in, unsigned char *out,
         long length, des_key_schedule ks1, des_key_schedule ks2,
         des_key_schedule ks3, des_cblock *ivec1, des_cblock *ivec2,
         int enc)
    {
    register DES_LONG tin0,tin1;
    register DES_LONG tout0,tout1,xor0,xor1,m0,m1;
    register long l=length;
    DES_LONG tin[2];
    unsigned char *iv1,*iv2;

    iv1 = &(*ivec1)[0];
    iv2 = &(*ivec2)[0];

    if (enc)
    {
    c2l(iv1,m0);
    c2l(iv1,m1);
    c2l(iv2,tout0);
    c2l(iv2,tout1);
    for (l-=8; l>=-7; l-=8)
        {
        tin[0]=m0;
        tin[1]=m1;
        des_encrypt(tin,ks3,1);
        m0=tin[0];
        m1=tin[1];

        if(l < 0)
        {
        c2ln(in,tin0,tin1,l+8);
        }
        else
        {
        c2l(in,tin0);
        c2l(in,tin1);
        }
        tin0^=tout0;
        tin1^=tout1;

        tin[0]=tin0;
        tin[1]=tin1;
        des_encrypt(tin,ks1,1);
        tin[0]^=m0;
        tin[1]^=m1;
        des_encrypt(tin,ks2,0);
        tin[0]^=m0;
        tin[1]^=m1;
        des_encrypt(tin,ks1,1);
        tout0=tin[0];
        tout1=tin[1];

        l2c(tout0,out);
        l2c(tout1,out);
        }
    iv1=&(*ivec1)[0];
    l2c(m0,iv1);
    l2c(m1,iv1);

    iv2=&(*ivec2)[0];
    l2c(tout0,iv2);
    l2c(tout1,iv2);
    }
    else
    {
    register DES_LONG t0,t1;

    c2l(iv1,m0);
    c2l(iv1,m1);
    c2l(iv2,xor0);
    c2l(iv2,xor1);
    for (l-=8; l>=-7; l-=8)
        {
        tin[0]=m0;
        tin[1]=m1;
        des_encrypt(tin,ks3,1);
        m0=tin[0];
        m1=tin[1];

        c2l(in,tin0);
        c2l(in,tin1);

        t0=tin0;
        t1=tin1;

        tin[0]=tin0;
        tin[1]=tin1;
        des_encrypt(tin,ks1,0);
        tin[0]^=m0;
        tin[1]^=m1;
        des_encrypt(tin,ks2,1);
        tin[0]^=m0;
        tin[1]^=m1;
        des_encrypt(tin,ks1,0);
        tout0=tin[0];
        tout1=tin[1];

        tout0^=xor0;
        tout1^=xor1;
        if(l < 0)
        {
        l2cn(tout0,tout1,out,l+8);
        }
        else
        {
        l2c(tout0,out);
        l2c(tout1,out);
        }
        xor0=t0;
        xor1=t1;
        }

    iv1=&(*ivec1)[0];
    l2c(m0,iv1);
    l2c(m1,iv1);

    iv2=&(*ivec2)[0];
    l2c(xor0,iv2);
    l2c(xor1,iv2);
    }
    tin0=tin1=tout0=tout1=xor0=xor1=0;
    tin[0]=tin[1]=0;
}

#endif


// --------------------------------------------------


int desSim_ecb_encrypt(const u8 *input, u8 * output, const u32 len, const u8 * key, const u8 encrypt) {
    u32 idx;
    des_cblock in,out;
    des_key_schedule ks;

    if(encrypt != TRUE && encrypt != FALSE) //Only support either encrypt or decrypt operation
        return FAILED;
    
    for(idx = 0; idx<len; idx+=8) {
        des_set_key(key, ks);
        rtl_memcpyb((u8*)in, (const u8*)(&input[idx]), 8);
        rtl_memsetb((u8*)out, 0,8);
        des_ecb_encrypt(&in,&out,ks,encrypt==TRUE? DES_ENCRYPT : DES_DECRYPT);
        rtl_memcpyb((u8*)(&output[idx]), out, 8);
    }
    return SUCCESS;
}

int desSim_ede_ecb_encrypt(const u8 *input, u8 * output, const u32 len, const u8 * key, const u8 encrypt) {
    u32 idx;
    des_cblock in,out;
    des_key_schedule ks, ks2, ks3;

    if(encrypt != TRUE && encrypt != FALSE) //Only support either encrypt or decrypt operation
        return FAILED;

    for(idx = 0; idx<len; idx+=8) {
        if (des_set_key(&key[0], ks) != 0 || des_set_key(&key[8], ks2) != 0 || des_set_key(&key[16], ks3) != 0)
            return FAILED;
    
        rtl_memcpyb((u8*)in, &input[idx], 8);
        rtl_memsetb((u8*)out, 0, 8);
        des_ecb3_encrypt((des_cblock *)&in,&out,&ks,&ks2, &ks3, encrypt==TRUE? DES_ENCRYPT : DES_DECRYPT);
        rtl_memcpyb((u8*)(&output[idx]), out, 8);
    }
    return SUCCESS;
}

int desSim_cbc_encrypt(const u8 *input, u8 * output, const u32 len, const u8 * key, const u8 * iv, const u8 encrypt) {
    des_cblock iv3;
    des_key_schedule ks;

    if(encrypt != TRUE && encrypt != FALSE) //Only support either encrypt or decrypt operation
        return FAILED;

    if (des_set_key(&key[0], ks) != 0)
        return FAILED;
    
    rtl_memcpyb((u8*)iv3, iv, 8);
    des_ncbc_encrypt(input, output, len, &ks, &iv3, encrypt==TRUE? DES_ENCRYPT : DES_DECRYPT);
    
    return SUCCESS;
}

int desSim_ede_cbc_encrypt(const u8 *input, u8 * output, const u32 len, const u8 * key, const u8 * iv, const u8 encrypt) {
    des_cblock iv3;
    des_key_schedule ks, ks2, ks3;

    if(encrypt != TRUE && encrypt != FALSE) //Only support either encrypt or decrypt operation
        return FAILED;

    if (des_set_key(&key[0], ks) != 0 || des_set_key(&key[8], ks2) != 0 || des_set_key(&key[16], ks3) != 0)
        return FAILED;

    rtl_memcpyb((u8*)iv3, iv, 8);
	
    des_ede3_cbc_encrypt(input, output, len, &ks, &ks2, &ks3, &iv3, encrypt==TRUE? DES_ENCRYPT : DES_DECRYPT);
    
    return SUCCESS;
}

 
