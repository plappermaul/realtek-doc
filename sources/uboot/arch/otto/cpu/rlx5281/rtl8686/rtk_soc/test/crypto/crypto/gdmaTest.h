#ifndef _GDMA_TEST_H_
#define _GDMA_TEST_H_

#include "rtl_types.h"

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
//extern uint64 enumbal_total_case_no;
static uint32 enumbal_total_case_no;

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

#endif

