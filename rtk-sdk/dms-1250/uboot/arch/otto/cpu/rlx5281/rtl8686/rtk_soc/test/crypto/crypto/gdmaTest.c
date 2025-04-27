/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Model code for pattern match
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: gdmaTest.c,v 1.43 2007/06/01 06:48:38 yjlou Exp $
*/

#if 0
#include "rtl_types.h"
#include "asicRegs.h"
#ifdef CONFIG_RTL865XC
#include "rtl865xC_tblAsicDrv.h"
#else
#include "rtl8651_tblAsicDrv.h"
#endif
#include "icExport.h"
#include "gdma.h"
#include "kmp.h"
#include "gdmaModel.h"
#include "virtualMac.h"
#include "rtl_glue.h"
#include "rtl_utils.h"
#include "icTest.h"
#include "gdmaTest.h"
#include "drvTest.h"
#ifdef RTL865X_MODEL_USER
#include "errno.h"
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif
#include "modelRandom.h"
#endif

#include "rtl_types.h"
#include "asicRegs.h"
#include "gdmaTest.h"
#include "crypto.h"

#define MAX_BLOCK_LENGTH ((1<<13)-1)
/* If defined, all the detailed test will be done.
 * If not defined, only partial test will be done, for example, destination length larger than 4 bytes.
 */
#define FULL_TEST


#if 0
	#define EXPECT_ACCEPT( s ) do {} while (0)
	#define EXPECT_NOT_ACCEPT( s ) do {} while (0)
#else
#define EXPECT_ACCEPT( state ) \
	do { \
		if ( ( state&STATE_ESCAPE ) != STATE_ESCAPE ) \
		{ \
			rtlglue_printf( "[%s():%d] Expect ACCEPT, but not accept (0x%06x)\n", __FUNCTION__, __LINE__, state ); \
			return FAILED; \
		} \
	} while (0)

#define EXPECT_NOT_ACCEPT( state ) \
	do { \
		if ( ( state&STATE_ESCAPE ) == STATE_ESCAPE ) \
		{ \
			rtlglue_printf( "[%s():%d] Expect NOT ACCEPT, but accepted (0x%06x)\n", __FUNCTION__, __LINE__, state ); \
			return FAILED; \
		} \
	} while (0)
#endif


#define HEAPSIZE (8*8*1024)
static uint8 _sb1[HEAPSIZE]; /* source heap (expected) */
static uint8 _db1[HEAPSIZE]; /* destination heap (expected) */
static uint8 _sb2[HEAPSIZE]; /* source heap (to be compared) */
static uint8 _db2[HEAPSIZE]; /* destination heap (to be compared) */
static uint8 *sb1, *db1, *sb2, *db2; /* Uncached address pointer for above heap */
/*	Enumerate data block buffers
 *	for model code and IC to compare results.
 */
typedef struct enumbuf_s
{
	uint8 *src;
	uint8 *dst;
} enumbuf_t;
static enumbuf_t enumbuf[2];


#define OPT_IGNCLR	 0x00000001 /* ignore clean buffer */
#define OPT_IGNSRC   0x00000002 /* ignore comparing source block */
#define OPT_CACHED1  0x00000004 /* Cached data block 1, including sb1 and db1 */
#define OPT_CACHED2  0x00000008 /* Cached data block 2, including sb2 and db2 */
#define OPT_ALL      0xFFFFFFFF /* all optimize */
static uint32 gdmaOptimize = 0;
void testGdmaOptimize( uint32 flags, uint32 enable )
{
	if ( enable )
		gdmaOptimize = gdmaOptimize | flags;
	else
		gdmaOptimize = gdmaOptimize & ~flags;

	if ( flags & OPT_CACHED1 )
	{
		if ( enable )
		{
			sb1 = (_sb1);
			db1 = (_db1);
			enumbuf[0].src = (_sb1);
			enumbuf[0].dst = (_db1);
		}
		else
		{
			sb1 = UNCACHED_ADDRESS(_sb1);
			db1 = UNCACHED_ADDRESS(_db1);
			enumbuf[0].src = UNCACHED_ADDRESS(_sb1);
			enumbuf[0].dst = UNCACHED_ADDRESS(_db1);
		}
	}
	if ( flags & OPT_CACHED2 )
	{
		if ( enable )
		{
			sb2 = (_sb2);
			db2 = (_db2);
			enumbuf[1].src = (_sb2);
			enumbuf[1].dst = (_db2);
		}
		else
		{
			sb2 = UNCACHED_ADDRESS(_sb2);
			db2 = UNCACHED_ADDRESS(_db2);
			enumbuf[1].src = UNCACHED_ADDRESS(_sb2);
			enumbuf[1].dst = UNCACHED_ADDRESS(_db2);
		}
	}
}


/*
 *  flags:
 */
#define CLEAN_SB1 0x00000001
#define CLEAN_SB2 0x00000002
#define CLEAN_DB1 0x00000004
#define CLEAN_DB2 0x00000008
#define CLEAN_ALL (CLEAN_SB1|CLEAN_SB2|CLEAN_DB1|CLEAN_DB2)
#define CLEAN_OVERALL 0x80000000 /* for gdmaInit() use */
void clearGdmaBuffer( uint32 flags )
{
	uint32 *su1, *su2, *du1, *du2;
	int i;

	if ( (flags&CLEAN_OVERALL)==0 )
	{
		if ( gdmaOptimize & OPT_IGNCLR )
		{
			/* Only one situation we will return to do nothing: optimization is enabled, and is not called by gdmaInit(). */
			return;
		}
	}

	su1 = (uint32*)&sb1[0];
	su2 = (uint32*)&sb2[0];
	du1 = (uint32*)&db1[0];
	du2 = (uint32*)&db2[0];
	if ( flags & CLEAN_SB1 )
	{
		for( i = 0; i < HEAPSIZE/4; i++ )
		{
			*su1 = 0;
			su1++;
		}
	}
	if ( flags & CLEAN_SB2 )
	{
		for( i = 0; i < HEAPSIZE/4; i++ )
		{
			*su2 = 0;
			su2++;
		}
	}
	if ( flags & CLEAN_DB1 )
	{
		for( i = 0; i < HEAPSIZE/4; i++ )
		{
			*du1 = 0;
			du1++;
		}
	}
	if ( flags & CLEAN_DB2 )
	{
		for( i = 0; i < HEAPSIZE/4; i++ )
		{
			*du2 = 0;
			du2++;
		}
	}
}

#if 0
/* initial routine for test GDMA */
int32 testGdmaInit( void )
{
#if defined(RTL865X_MODEL_KERNEL)
	testGdmaOptimize( OPT_ALL, FALSE ); /* turn off all optimize */
	testGdmaOptimize( OPT_IGNCLR|OPT_IGNSRC, TRUE ); /* then, enable IGNCLR and IGNSRC */
#elif defined(RTL865X_MODEL_USER)
	sb1 = _sb1;
	db1 = _db1;
	sb2 = _sb2;
	db2 = _db2;
	enumbuf[0].src = _sb1;
	enumbuf[0].dst = _db1;
	enumbuf[1].src = _sb2;
	enumbuf[1].dst = _db2;
#else
#error neither RTL865X_MODEL_KERNEL nor RTL865X_MODEL_USER is defined
#endif
	enumbal_total_case_no = 0;

	clearGdmaBuffer( CLEAN_OVERALL|CLEAN_ALL );
	
	return SUCCESS;
}
#endif
/*
 *	To compare {sb1,db1} and {sb2,db2}
 */
int32 compare_data_block( void )
{
	int32 retval = SUCCESS;
	int i;
#if 1
	uint32 *du1, *du2;
	uint32 DU1, DU2;

	if ( gdmaOptimize&OPT_IGNSRC )
	{
		/* do nothing, if we need to ignore comparing source block. */
	}
	else
	{
		uint32 *su1, *su2;
		uint32 SU1, SU2;

		su1 = (uint32*)&sb1[0];
		su2 = (uint32*)&sb2[0];
		for( i = 0; i < HEAPSIZE/4; i++ )
		{
			if ( (SU1=*su1) != (SU2=*su2) )
			{
				rtlglue_printf( "sb1(%p) sb2(%p) is diff since %drd bytes sb1(0x%02x), sb2(0x%02x).\n", sb1, sb2, i*4, SU1, SU2 );
				if ( ( SU1 != *su1 ) || ( SU2 != *su2 ) ) rtlglue_printf( "strange! *su1=0x%08x, *su2=0x%08x; however, SU1=0x%08x, SU2=0x%08x\n", *su1, *su2, SU1, SU2 );
				retval = FAILED;
				break;
			}
			su1++;
			su2++;
		}
	}

	du1 = (uint32*)&db1[0];
	du2 = (uint32*)&db2[0];
	for( i = 0; i < HEAPSIZE/4; i++ )
	{
		if ( (DU1=*du1) != (DU2=*du2) )
		{
			rtlglue_printf( "db1(%p) db2(%p) is diff since %drd bytes db1(0x%02x), db2(0x%02x).\n", db1, db2, i*4, DU1, DU2 );
			if ( ( DU1 != *du1 ) || ( DU2 != *du2 ) ) rtlglue_printf( "strange! *du1=0x%08x, *du2=0x%08x; however, DU1=0x%08x, DU2=0x%08x\n", *du1, *du2, DU1, DU2 );
			retval = FAILED;
			break;
		}
		du1++;
		du2++;
	}
	
#elif 0
	int i;

	for( i = 0; i < HEAPSIZE; i++ )
	{
		if ( sb1[i] != sb2[i] )
		{
			rtlglue_printf( "sb1(%p) sb2(%p) is diff since %drd bytes sb1(0x%02x), sb2(0x%02x).\n", sb1, sb2, i, sb1[i], sb2[i] );
			retval = FAILED;
			break;
		}
	}
	for( i = 0; i < HEAPSIZE; i++ )
	{
		if ( db1[i] != db2[i] )
		{
			rtlglue_printf( "db1(%p) db2(%p) is diff since %drd bytes db1(0x%02x), db2(0x%02x).\n", db1, db2, i, db1[i], db2[i] );
			retval = FAILED;
			break;
		}
	}
#else
	if ( memcmp( sb1, sb2, HEAPSIZE ) != 0 )
	{
		rtlglue_printf( "sb1(%p) sb2(%p) is different.\n", sb1, sb2 );
		return FAILED;
	}
	if ( memcmp( db1, db2, HEAPSIZE ) != 0 )
	{
		rtlglue_printf( "db1(%p) db2(%p) is different.\n", db1, db2 );
		return FAILED;
	}
	return SUCCESS;
#endif

	return retval;
}



/***************************************************************************************************
 ***************************************************************************************************
 **	These functions will enumulate block alignment and length.                                    **
 **	Results are stored in either source block pointer/length or destination block pointer/length. **
 **	(4BA: 4 bytes aligned)                                                                        **
 **		pointer: 4BA, 		length: 4BA                                                           **
 **		pointer: 4BA+1,		length: 4BA                                                           **
 **		pointer: 4BA+2,		length: 4BA                                                           **
 **		pointer: 4BA+3,		length: 4BA                                                           **
 **	#block = { 1, 2, 3, 4, 5, 6, 7, 8 };                                                          **
 **	pointer = { 4BA+0, 4BA+1, 4BA+2, 4BA+3, 16BA+3, FIFOBoundary, Bank+1, ChipBoundary };         **
 **	length = { 4BA+0, 4BA+1, 4BA+2, 4BA+3, FIFOSize, BankSize,  };                                **
 ***************************************************************************************************
 ***************************************************************************************************/
/* block length combination:
 *              |   src>dst
 *              |  NA  |Allow
 *         -----+------+------
 *           NA |  ==  | S>D
 * dst<src -----+------+------
 *         Allow|  D<S | All
 */
/*
 *	Given 'seq' and 'totolLen', this function will return corresponding 'align' and 'length'.
 *	The current indexes of 'align' and 'length' are stored in seq[].
 */
int32 enumBAL( int32 maxBlockNum, int32 editBlock, int32 srcLen, int32 dstLen,
               int32* tsl, int32* tsa, int32* tdl, int32* tda, /* templates */
               int32 nsl, int32 nsa, int32 ndl, int32 nda, /* number of templates */
               int32* psl, int32* psa, int32* pdl, int32* pda, /* to be stored enumerated variables */
               CBBAL_T callback, uint32 flags )
{
	int sl, sa, dl, da; /* loop variables for srcLen/srcAlign/dstLen/dstAlign */
	int32 retval;
	if ( editBlock >= maxBlockNum ) /* no more deep */
		return SUCCESS;
	if ( ( !(flags&ENUMBAL_IGNORE_SRC) && srcLen<=0 ) ||
	     ( !(flags&ENUMBAL_IGNORE_DST) && dstLen<=0 ) )
	{
		/*rtlglue_printf( "srcLen=%d dstLen=%d\n", srcLen, dstLen );*/
		return SUCCESS;
	}

	if ( flags & ENUMBAL_IGNORE_SRC )
	{
		sl = 0;
		sa = 0;
		nsl = 0;
		nsa = 0;
		flags |= ENUMBAL_ALLOW_DgS;
		goto escape_src_loop;
	}
	if ( flags & ENUMBAL_FREE_SRC )
	{
		sl = 0;
		sa = 0;
		goto escape_src_loop;
	}
	for( sl = 0; sl < nsl; sl++ )
	{
		psl[editBlock] = tsl[sl];
		for( sa = 0; sa < nsa; sa++ )
		{
			psa[editBlock] = tsa[sa];
			/* to indicate no more block after */
			if ( editBlock+1 < maxBlockNum )
				psl[editBlock+1] = 0;
				
			if ( flags & ENUMBAL_IGNORE_DST )
			{
				dl = 0;
				da = 0;
				ndl = 0;
				nda = 0;
				flags |= ENUMBAL_ALLOW_SgD;
				goto escape_dst_loop;
			}
			
escape_src_loop:
			for( dl = 0; dl < ndl; dl++ )
			{
				pdl[editBlock] = tdl[dl];
				for( da = 0; da < nda; da++ )
				{
					int i, src, dst; /* for compute total block length */
					
escape_dst_loop:
					pda[editBlock] = tda[da];
					
					/* to indicate no more block after */
					if ( editBlock+1 < maxBlockNum )
						pdl[editBlock+1] = 0;

					for( i = 0, src = 0; i <= editBlock; i++ ) src += psl[i];
					for( i = 0, dst = 0; i <= editBlock; i++ ) dst += pdl[i];

					/* build source blocks according to destination blocks. */
					if ( flags & ENUMBAL_FREE_SRC )
					{
						int _src = dst;
						nsl = 0;
						nsa = 0;
						for( i = 0; i < 8; i++ )
						{
							if ( _src > MAX_BLOCK_LENGTH )
								psl[i] = MAX_BLOCK_LENGTH;
							else
								psl[i] = _src;
							_src -= psl[i];
							psa[i] = 0;
							nsl++;
							nsa++;
							if ( _src <= 0 ) break;
						}
						/* to indicate no more block after */
						if ( i != 8 )
							psl[i+1] = 0;

						src = dst;
					}

					/* before calling callback function, we must check if it fits criterions. */
					if ( ( src==dst ) ||
					     ( src>dst && (flags&ENUMBAL_ALLOW_SgD) ) ||
					     ( dst>src && (flags&ENUMBAL_ALLOW_DgS) ) )
					{
						/* source/destination block alignment and length have been ready, call the provided function to test. */
						if ( ( retval = callback( psl, psa, pdl, pda ) ) != SUCCESS )
							return retval;
					}

					/* recursive to enumerate next level */
					if ( ( editBlock+1 < maxBlockNum ) /* to speed up, we will check editBlock before calling self-recursive function. */ &&
					     ( retval = enumBAL( maxBlockNum, editBlock+1, srcLen-psl[editBlock], dstLen-pdl[editBlock], 
					                         tsl, tsa, tdl, tda, nsl, nsa, ndl, nda,
					                         psl, psa, pdl, pda, callback, flags ) ) != SUCCESS )
						return retval;
				}
			}
			
			if ( flags & ENUMBAL_FREE_SRC )
				goto escape_src_loop2;
		}
	}

escape_src_loop2: do {} while (0);

	return SUCCESS;
}

#if 0
/*
 *	To test enumBAL().
 *
 *	Assume sl=4, sa=4, dl=4, da=4, and ENUMBAL_ALLOW_EQUAL. 
 *	Block number is changed from 1 to 8:
 *
 *    Total Loop   ALLOW_EQUAL     ALLOW_ALL
 *		Block No
 *		       1            64           256
 *		       2         11328         65792
 *		       3       2387008      16843008
 *		       4     532704320    4311810304
 *		       5  122486287424
 *		       6              
 *		       7              
 *		       8              
 *
 *	Total estimated time is about 7 minutes for block no 2 and ALLOW_EQUAL.
 */
int32 cbEnumBAL( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	DEBUG_BAL();
	
	return SUCCESS;
}
int32 testGdmaEnumBAL(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	static int32 tmplSrcLen[] = { 1, 2, 3, 4 };
	static int32 tmplSrcAlign[] = { 0, 1, 2, 3 };
	static int32 tmplDstLen[] = { 1, 2, 3, 4 };
	static int32 tmplDstAlign[] = { 0, 1, 2, 3 };
	int32 retval;

	testGdmaInit();

	memset( sa, 0, sizeof(sa) );
	memset( sl, 0, sizeof(sl) );
	memset( da, 0, sizeof(da) );
	memset( dl, 0, sizeof(dl) );

	retval = enumBAL( 2, 0, 99, 99, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
	                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
	                  sl, sa, dl, da, cbEnumBAL, ENUMBAL_ALLOW_EQUAL );
	rtlglue_printf("enumbal_total_case_no=%llu\n", enumbal_total_case_no );
	return retval;
}

#endif
