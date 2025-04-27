/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header file for pattern match
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: gdma.h,v 1.10 2006/07/13 16:03:07 chenyl Exp $
*/
#include "rtl_types.h"
#include <exports.h>
#include <config.h>

#ifndef _PATTERN_MATCH_
#define _PATTERN_MATCH_

/* _WITH_DEBUG_SYMBOL_
 *  If defined, struct pmRule_s will include debug symbol.
 */
#undef _WITH_DEBUG_SYMBOL_


/*
 *	Some bits in GDMACNR are only for internal used.
 *	However, driver needs to configure them.
 *
 *	burstSize[7:6] -- 00:4W, 01:8W, 10:16W, 11:32W.
 *	enough[5:4]    -- 00:>16, 01:>10, 10:>4, 00:>0
 *	dlow[3:2]      -- 00:>24, 01:>20. 10:>16, 11:>8
 *	slow[1:0]      -- 00:>24, 01:>20. 10:>16, 11:>8
 */
#define internalUsedGDMACNR (0x00000000)  /* BURST SIZE = 4 words */
//#define internalUsedGDMACNR (0x00000040) /* BURST SIZE = 8 words */
//#define internalUsedGDMACNR (0x00000080) /* BURST SIZE = 16 words */
//#define internalUsedGDMACNR (0x000000C0) /* BURST SIZE = 32 words */


#define rtlglue_malloc malloc
#define rtlglue_free free
#define rtlglue_printf printf
void rtlglue_srandom( uint32 seed );
uint32 rtlglue_random( void );
uint32 rtlglue_getmstime( uint32* pTime );
inline int32 rtlglue_flushDCache(uint32 start, uint32 size);
#if defined(CONFIG_RTL8196) || defined(CONFIG_RTL8652) || defined(CONFIG_RTL8672)
int timer_init(void);
#endif

#define STATE_MACHINE_ORDER 20
#define STATE_MACHINE_NUMBER (1<<STATE_MACHINE_ORDER)
#define INTERNAL_RULE_ORDER 8
#define INTERNAL_RULE_NUMBER (1<<INTERNAL_RULE_ORDER)
#define SUB_STATE_MACHINE_ORDER (STATE_MACHINE_ORDER-INTERNAL_RULE_ORDER)
#define SUB_STATE_MACHINE_NUMBER (1<<SUB_STATE_MACHINE_ORDER)
#define STATE_ESCAPE (0xF0000) /* If the next state is ranged from 0x1f0000 to 0x1ffffff, the FSM will stop, and return. */
#define STATE_ACCEPT (0xFFFFF) /* The special case of escape state. But for IC, it has no special meaning. */
#define STATE_DEAD (0xDEAD0) /* The special case of escape state. But for IC, it has no special meaning. */

#define MAX_BLOCK_LENGTH ((1<<13)-1)
#define ALL_BLOCK_LENGTH (MAX_BLOCK_LENGTH*8)
#define MAX_DEST_LENGTH ( MAX_BLOCK_LENGTH/*first*/ + (MAX_BLOCK_LENGTH&~0x3)*7 )


/***********************
 * Transition Rule 0   *
 ***********************/
struct pmRule_s0
{
#ifdef _LITTLE_ENDIAN
	uint32 jmpState:STATE_MACHINE_ORDER;
	uint32 cpu:1;
	uint32 not:1;
	uint32 type:2;
	uint32 matchChar:8;
#else
	uint32 matchChar:8;
	uint32 type:2;
	uint32 not:1;
	uint32 cpu:1;
	uint32 jmpState:STATE_MACHINE_ORDER;
#endif
#ifdef _WITH_DEBUG_SYMBOL_
	char* debugSymbol;
#endif
};
typedef struct pmRule_s0 pmRule_t0;


/***********************
 * Transition Rule 1   *
 ***********************/
struct pmRule_s1
{
#ifdef _LITTLE_ENDIAN
	uint32 Char0:8;
	uint32 type:2;
	int32 offset:6;
	uint32 Char1:8;
	uint32 Char2:8;
#else
	uint32 Char0:8;
	uint32 type:2;
	int32 offset:6;
	uint32 Char1:8;
	uint32 Char2:8;
#endif
#ifdef _WITH_DEBUG_SYMBOL_
	char* debugSymbol;
#endif
};
typedef struct pmRule_s1 pmRule_t1;


/***********************
 * Transition Rule 2   *
 ***********************/
struct pmRule_s2
{
#ifdef _LITTLE_ENDIAN
	int32 Else:11;
	int32 Then:11;
	uint32 type:2;
	uint32 Char:8;
#else
	uint32 Char:8;
	uint32 type:2;
	int32 Then:11;
	int32 Else:11;
#endif
#ifdef _WITH_DEBUG_SYMBOL_
	char* debugSymbol;
#endif
};
typedef struct pmRule_s2 pmRule_t2;


/***********************
 * Sub State Machine
 ***********************/
struct subStateMachine_s
{
	pmRule_t0 rules[INTERNAL_RULE_NUMBER];
};
typedef struct subStateMachine_s subStateMachine_t;

/***********************
 * Sub State Machine
 ***********************/
struct stateMachine_s
{
	subStateMachine_t* subsm[SUB_STATE_MACHINE_NUMBER];
	uint32 cntTotalRules;
};
typedef struct stateMachine_s stateMachine_t;


/* Pattern Match Rule Type */
#define GDMA_RULETYPE_0 0
#define GDMA_RULETYPE_1 1
#define GDMA_RULETYPE_2 2


/***********************
 * Block Descriptors
 ***********************/
struct descriptor_s
{
	uint8* pData;
#ifdef _LITTLE_ENDIAN
	uint32 length:31;
	uint32 ldb:1;
#else
	uint32 ldb:1;
	uint32 length:31;
#endif
};
typedef struct descriptor_s descriptor_t;


void arrangeDataRegs( descriptor_t *src, descriptor_t *dst, uint8 *in, int32 *pProcessed, uint8 *out, int32 *pOutputted );
int32 rtl865x_b64enc( uint8 *in, int32 inLen, uint8 *out, int32 outLen, int32 *pProcessed, int32 *pOutputted );
int32 rtl865x_b64dec( uint8 *in, int32 inLen, uint8 *out, int32 outLen, int32 *pProcessed, int32 *pOutputted );
int32 rtl865x_qpenc( uint8 *in, int32 inLen, uint8 *out, int32 outLen, int32 *pProcessed, int32 *pOutputted );
int32 rtl865x_qpdec( uint8 *in, int32 inLen, uint8 *out, int32 outLen, int32 *pProcessed, int32 *pOutputted );



#endif/*#ifndef _PATTERN_MATCH_*/

