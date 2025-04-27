#ifndef _GDMA_TEST_H_
#define _GDMA_TEST_H_

#include <common.h>
#include "rtl_types.h"
#define CONFIG_RTL8686

#ifdef CONFIG_RTL8686
#define _cache_flush rlx5281_cache_flush_dcache
#endif

#ifdef CONFIG_RTL0371M
#define _cache_flush rlx5281_cache_flush_dcache
#endif

#ifdef CONFIG_RTL8198
#define _cache_flush rlx5281_cache_flush_dcache
#endif

#ifdef CONFIG_RTL0371S
#define _cache_flush rlx5181_cache_flush_dcache
#endif

#ifdef CONFIG_RTL8316
#define _cache_flush mips_cache_flush_all
#endif

#ifdef CONFIG_RTL8196
#define	_cache_flush rlx5181_cache_flush_dcache
#endif
typedef int32 (*CBBAL_T)( int32 *sl, int32 *sa, int32 *dl, int32 *da );

#define ENUMBAL_ALLOW_EQUAL (0x00000000) /* allow source length is equal to destination length */
#define ENUMBAL_ALLOW_SgD   (0x00000001) /* allow source length larger than destination length */
#define ENUMBAL_ALLOW_DgS   (0x00000002) /* allow destination length larger than source length */
#define ENUMBAL_IGNORE_SRC  (0x00000004) /* to tell enumBAL() the source descriptor blocks is no use, so please don't check it. */
#define ENUMBAL_IGNORE_DST  (0x00000008) /* to tell enumBAL() the destination descriptor blocks is no use, so please don't check it. */
#define ENUMBAL_FREE_SRC    (0x00000010) /* Source descriptor blocks are free. It means the length is determined by destination. It usually uses in the case we want to focus destination length and alignment. */
#define ENUMBAL_ALLOW_ALL   (ENUMBAL_ALLOW_SgD|ENUMBAL_ALLOW_DgS) /* allow all, no matter source and destination length */

int32 enumBAL( int32 maxBlockNum, int32 editBlock, int32 srcLen, int32 dstLen,
               int32* tsl, int32* tsa, int32* tdl, int32* tda, /* templates */
               int32 nsl, int32 nsa, int32 ndl, int32 nda, /* number of templates */
               int32* psl, int32* psa, int32* pdl, int32* pda, /* to be stored enumerated variables */
               CBBAL_T callback, uint32 flags );


/*
 *	For debug, to show the current source/destination length/alignment.
 */
extern uint64 enumbal_total_case_no;

#define DEBUG_BAL_INCNUM() \
		(++enumbal_total_case_no)
#define DEBUG_BAL_CASENUM() \
		rtlglue_printf("<%llu>", DEBUG_BAL_INCNUM() );
#define DEBUG_BAL_DETAILS() \
	do { \
		int i; \
		rtlglue_printf(" SRC[ "); \
		for( i = 0; i <= 8; i++ ) \
		{ \
			if ( sl[i]== 0 ) break; \
			rtlglue_printf( "%d:%d ", sl[i], sa[i] ); \
		} \
		rtlglue_printf("] DST[ "); \
		for( i = 0; i <= 8; i++ ) \
		{ \
			if ( dl[i]== 0 ) break; \
			rtlglue_printf( "%d:%d ", dl[i], da[i] ); \
		} \
		rtlglue_printf("]\n"); \
	} while( 0 )
		
#if 0 /* 1 for debug more, 0 for debug less */
#define DEBUG_BAL() \
	do { \
		DEBUG_BAL_CASENUM(); \
		DEBUG_BAL_DETAILS(); \
	} while( 0 )
#elif 0 /* 1 for debug, 0 for quiet */
#define DEBUG_BAL() \
	do { \
		DEBUG_BAL_CASENUM(); \
	} while( 0 )
#else
#define DEBUG_BAL() do { DEBUG_BAL_INCNUM(); } while(0)
#endif



int32 testGdmaPatternMatch1(uint32 caseNo);
int32 testGdmaPatternMatch2(uint32 caseNo);
int32 testGdmaPatternMatch3(uint32 caseNo);
int32 testGdmaPatternMatch4(uint32 caseNo);
int32 testGdmaPatternMatch5(uint32 caseNo);
int32 testGdmaPatternMatch6(uint32 caseNo);
int32 testGdmaPatternMatch7(uint32 caseNo);
int32 testGdmaPatternMatch8(uint32 caseNo);
int32 testGdmaPatternMatch9(uint32 caseNo);
int32 testGdmaPatternMatch10(uint32 caseNo);
int32 testGdmaPatternMatch11(uint32 caseNo);
int32 testGdmaPatternMatch12(uint32 caseNo);
int32 testGdmaPatternMatch13(uint32 caseNo);
int32 testGdmaPatternMatch14(uint32 caseNo);
int32 testGdmaPatternMatch15(uint32 caseNo);

int32 testGdmaMemory1(uint32 caseNo);
int32 testGdmaMemory2(uint32 caseNo);
int32 testGdmaMemory3(uint32 caseNo);
int32 testGdmaMemory4(uint32 caseNo);
int32 testGdmaMemory5(uint32 caseNo);
int32 testGdmaMemory6(uint32 caseNo);
int32 testGdmaMemory7(uint32 caseNo);
int32 testGdmaMemory8(uint32 caseNo);
int32 testGdmaMemory9(uint32 caseNo);
int32 testGdmaMemory10(uint32 caseNo);
int32 testGdmaMemory11(uint32 caseNo);
int32 testGdmaMemory12(uint32 caseNo);
int32 testGdmaMemory13(uint32 caseNo);
int32 testGdmaMemory14(uint32 caseNo);
int32 testGdmaMemory15(uint32 caseNo);
int32 testGdmaChecksum1(uint32 caseNo );
int32 testGdmaChecksum2(uint32 caseNo );
int32 testGdmaSequentialTCAM(uint32 caseNo );
int32 testGdmaSequentialTCAM2(uint32 caseNo );
int32 testGdmaWirelessMIC(uint32 caseNo );
int32 testGdmaWirelessMIC2(uint32 caseNo );
int32 testGdmaWirelessMIC3(uint32 caseNo );
int32 testGdmaBase64Encode1(uint32 caseNo);
int32 testGdmaBase64Encode2(uint32 caseNo);
int32 testGdmaBase64Encode3(uint32 caseNo);
int32 testGdmaBase64Decode1(uint32 caseNo);
int32 testGdmaBase64Decode2(uint32 caseNo);
int32 testGdmaBase64EncDec1(uint32 caseNo);
int32 testGdmaBase64EncDec2(uint32 caseNo);
int32 testGdmaQuotedPrintableEncode1(uint32 caseNo);
int32 testGdmaQuotedPrintableDecode1(uint32 caseNo);
int32 testGdmaQuotedPrintableDecode2(uint32 caseNo);
int32 testGdmaQuotedPrintableEncDec2(uint32 caseNo);
int32 testGdmaByteSwap1(uint32 caseNo);
int32 testGdmaByteSwap2(uint32 caseNo);
int32 testGdmaByteSwap3(uint32 caseNo);
int32 testGdmaByteSwap4(uint32 caseNo);
int32 testGdmaEnumBAL(uint32 caseNo);

#endif

