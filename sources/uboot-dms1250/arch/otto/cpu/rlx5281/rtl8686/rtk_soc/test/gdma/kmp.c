/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : FSM generator for KMP
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: kmp.c,v 1.9 2006-07-13 16:03:07 chenyl Exp $
*/

#include "rtl_types.h"
#include "gdma_glue.h"
#include "asicregs.h"
#include "kmp.h"
#include "vsm.h"


#ifndef _KMP_DEBUG_
#define _KMP_DEBUG_ 0
#endif

/*
 *  This function returns the length of postfix of state+char that matches the prefix of the pattern.
 *  This is binary-safe function.
 *
 *  state+char --> n a n a n
 *                     n a n o n o <-- pattern
 *                     -----
 *                         ^--- maxL = 3
 *
 */
int32 partial( const uint8 *pattern, const int32 lenPattern,
               const uint8 *state_char, const int32 lenState_char )
{
	int l, i, maxL;

	maxL = 0;
	for( l = 1; l <= lenPattern && l <= lenState_char; l++ )
	{
		for( i = 0; i < l; i++ )
		{
			if ( state_char[lenState_char-l+i] != pattern[i] ) break;
		}
		if ( i == l )
			maxL = l;
	}

	return maxL;
}


/*
 *  Generate VSM from KMP algorithm.
 *  Given a pre-allocated pVsm and a string pattern with lenPattern bytes, this function will return a VSM that accept the pattern.
 *
 *  Note: This function is binary-safe for string pattern.
 */
int32 vsmGenFromKMP( pmVsm_t* pVsm, uint8* pattern, uint32 lenPattern )
{
	typedef uint32 state[256];
	state *nextState;
	uint8 *pszState_char;
	int i; /* index of pszState_char */
	int j; /* 0x00~0xff, for char .... */
	uint32 *cntRule; /* count of rule of each state */
	uint32 *stateToRuleNo; /* array that maps from state to rule No. */
	uint32 totalRuleNo = 0;
	int32 retval = FAILED;
	uint32 cntNewRule = 0;
	pmRule_t0 *pRule = NULL;
	pmRule_t0 *prevRule = NULL;

#if _KMP_DEBUG_ >= 1
	rtlglue_printf( "%s(pVsm=%p,pattern=\"%s\"):%d\n", __FUNCTION__, pVsm, pattern, __LINE__ );
#endif

	if ( pVsm==NULL )
		goto out0;

	pszState_char = (uint8*) rtlglue_malloc( lenPattern+1 );
	if ( pszState_char==NULL )
	{
		retval = FAILED;
		goto out0;
	}
	memset( pszState_char, 0, lenPattern+1 );

	nextState = (state*) rtlglue_malloc( sizeof(state)*(lenPattern+1) );
	if ( nextState==NULL )
	{
		retval = FAILED;
		goto out1;
	}

	cntRule = (uint32*) rtlglue_malloc( sizeof(*cntRule)*(lenPattern+1) );
	if ( cntRule==NULL )
	{
		retval = FAILED;
		goto out2;
	}
	memset( cntRule, 0, sizeof(*cntRule)*(lenPattern+1) );

	stateToRuleNo = (uint32*) rtlglue_malloc( sizeof(*stateToRuleNo)*(lenPattern+1) );
	if ( stateToRuleNo==NULL )
	{
		retval = FAILED;
		goto out3;
	}
	memset( stateToRuleNo, 0, sizeof(*stateToRuleNo)*(lenPattern+1) );

	/* call partial() to generate next state table */
	for( i = 0; i < lenPattern; i++ )
	{
		for( j = 0x00; j <= 0xff; j++ )
		{
			pszState_char[i] = j;
			nextState[i][j] = partial( pattern, lenPattern, pszState_char, i+1 );
		}

		pszState_char[i] = pattern[i];
	}

	/* statistic state to rule number mapping */
	for( i = 0; i < lenPattern; i++ )
	{
		cntRule[i]++; /* for default rule */
		for( j = 0x00; j <= 0xff; j++ )
		{
			if ( nextState[i][j]!=0 )
				cntRule[i]++;
		}
		totalRuleNo += cntRule[i];
	}

	/* convert to accumelated value */
	stateToRuleNo[0] = 0;
	for( i = 1; i < lenPattern; i++ )
	{
		stateToRuleNo[i] = stateToRuleNo[i-1] + cntRule[i-1];
	}

	/* rewrite next state table */
	for( i = 0; i < lenPattern; i++ )
	{
		for( j = 0x00; j <= 0xff; j++ )
		{
			nextState[i][j] = stateToRuleNo[nextState[i][j]];
		}
	}
	nextState[lenPattern-1][(uint8)pattern[lenPattern-1]] = STATE_ACCEPT;

	/* We need to allocate ONLY ONE stack entity.*/
	pVsm->cntPrefix = 1;
	pVsm->prefix = rtlglue_malloc( sizeof(*pVsm->prefix) );
	if ( pVsm->prefix==NULL )
	{
		retval = FAILED;
		goto out4;
	}
	memset( pVsm->prefix, 0, sizeof(*pVsm->prefix) );
	
	/*
	 * We need to allocate memory space for state machine now.
	 */
	pVsm->prefix[0].entityType = ENTYPE_STATE_MACHINE;
	pVsm->prefix[0].pStateMachine = rtlglue_malloc( sizeof( stateMachine_t ) );
	if ( pVsm->prefix[0].pStateMachine==NULL )
	{
		retval = FAILED;
		goto out5;
	}
	/* In IC/FPGA, we must use uncached address to to garentee data have been in SDRAM. */
	pVsm->prefix[0].pStateMachine = UNCACHED_ADDRESS( pVsm->prefix[0].pStateMachine );
	memset( pVsm->prefix[0].pStateMachine, 0, sizeof( stateMachine_t ) );

	/* output next state table to instruction */
	for( i = 0; i < lenPattern; i++ )
	{
		for( j = 0x00; j <= 0xff; j++ )
		{
			if ( nextState[i][j]!=0 )
			{
				/* If subsm is not yet allocated, allocate now. */
				if ( pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER]==NULL )
				{
					pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER] = rtlglue_malloc( sizeof(subStateMachine_t) );
					if ( pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER]==NULL ) goto out7;
					/* In IC/FPGA, we must use uncached address to to garentee data have been in SDRAM. */
					pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER] = UNCACHED_ADDRESS( pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER] );
					memset( pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER], 0, sizeof(subStateMachine_t) );
				}
				pRule = &pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER]->rules[cntNewRule&(INTERNAL_RULE_NUMBER-1)];

				pRule->not = 0;
				pRule->matchChar = j;
				pRule->jmpState = nextState[i][j];
#ifdef _WITH_DEBUG_SYMBOL_
				pRule->debugSymbol = rtlglue_malloc( i + 3 );
				pRule->debugSymbol[0] = '\"';
				if ( i > 0 ) memcpy( pRule->debugSymbol+1, pattern, i );
				pRule->debugSymbol[i+1] = '\"';
				pRule->debugSymbol[i+2] = '\0';
#endif
				pRule->cpu = nextState[i][j]==STATE_ACCEPT?1:0;
				prevRule = pRule;
				cntNewRule++;
			}
		}
				
		/* default rule */
		/* If subsm is not yet allocated, allocate now. */
		if ( pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER]==NULL )
		{
			pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER] = rtlglue_malloc( sizeof(subStateMachine_t) );
			if ( pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER]==NULL ) goto out7;
			/* In IC/FPGA, we must use uncached address to to garentee data have been in SDRAM. */
			pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER] = UNCACHED_ADDRESS( pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER] );
			memset( pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER], 0, sizeof(subStateMachine_t) );
		}
		pRule = &pVsm->prefix[0].pStateMachine->subsm[cntNewRule>>INTERNAL_RULE_ORDER]->rules[cntNewRule&(INTERNAL_RULE_NUMBER-1)];
		
		pRule->not = 1;
		pRule->matchChar = prevRule->matchChar;
		pRule->jmpState = 0;
#ifdef _WITH_DEBUG_SYMBOL_
		pRule->debugSymbol = rtlglue_malloc( i + 3 );
		pRule->debugSymbol[0] = '\"';
		if ( i > 0 ) memcpy( pRule->debugSymbol+1, pattern, i );
		pRule->debugSymbol[i+1] = '\"';
		pRule->debugSymbol[i+2] = '\0';
#endif
		pRule->cpu = 0;
		prevRule = pRule;
		cntNewRule++;
	}

#if 0
	if ( prevRule )
		prevRule->endTR = 1;
#endif
	
	pVsm->prefix[0].pStateMachine->cntTotalRules = cntNewRule;

#if 0 /* We disabled the optimize because we want to use 'not' bit to draw state machine. */
	/* Interpreter Optimization */
	if ( pVsm->prefix[0].pStateMachine->cntTotalRules >= 2 )
	{
		/*  Consider the typical case:
		 *    rule 0:     match 'A', jump to rule 2
		 *    rule 1: NOT macth 'A', jump to rule 0
		 *    rule 2:     blahblah 
		 *
		 *  However, the most case matches the rule 1, and the rule 0 is usually not matched.
		 *  So we can swap rule 0 and rule 1 to optimize the most case.
		 */
		pmRule_t0 tmp;
		tmp = pVsm->prefix[0].pStateMachine->subsm[0]->rules[0];
		pVsm->prefix[0].pStateMachine->subsm[0]->rules[0] = pVsm->prefix[0].pStateMachine->subsm[0]->rules[1];
		pVsm->prefix[0].pStateMachine->subsm[0]->rules[1] = tmp;
		pVsm->prefix[0].pStateMachine->subsm[0]->rules[0].not = 1;
		pVsm->prefix[0].pStateMachine->subsm[0]->rules[1].not = 0;
	}
#endif

#if _KMP_DEBUG_ >= 2
	/* debug output */
	{
		#define OUT_START 'a'
		#define OUT_END 'z'
		
		for( i = 0; i <lenPattern; i++ ) rtlglue_printf("=");
		rtlglue_printf( "===========" );
		for( j = OUT_START; j <= OUT_END; j++ )
			rtlglue_printf( "====" );
		rtlglue_printf( "\n" );
		
		rtlglue_printf( "%3s %*s %3s | ", "Idx", lenPattern, "", "cnt" );
		for( j = OUT_START; j <= OUT_END; j++ )
			rtlglue_printf( "  %c ", j );
		rtlglue_printf( "\n" );
		
		for( i = 0; i <lenPattern; i++ ) rtlglue_printf("-");
		rtlglue_printf( "---------+-" );
		for( j = OUT_START; j <= OUT_END; j++ )
			rtlglue_printf( "----" );
		rtlglue_printf( "\n" );
		
		for( i = 0; i < lenPattern; i++ )
		{
			rtlglue_printf( "%03x %*.*s %03x | ", stateToRuleNo[i], lenPattern, i, pattern, cntRule[i] );
			for( j = OUT_START; j <= OUT_END; j++ )
			{
				if ( (nextState[i][j]&STATE_ESCAPE)==STATE_ESCAPE )
					rtlglue_printf( "%04X", nextState[i][j]&(STATE_ESCAPE^STATE_ACCEPT) );
				else
					rtlglue_printf( "%3x ", nextState[i][j] );
			}
			rtlglue_printf( "\n" );
		}
		
		for( i = 0; i <lenPattern; i++ ) rtlglue_printf("-");
		rtlglue_printf( "-----------" );
		for( j = OUT_START; j <= OUT_END; j++ )
			rtlglue_printf( "----" );
		rtlglue_printf( "\n" );
#endif

#if _KMP_DEBUG_ >= 1
		rtlglue_printf( "Total Rule Number of this FSM = %d\n", totalRuleNo );
#endif
		
#if _KMP_DEBUG_ >= 2
		for( i = 0; i <lenPattern; i++ ) rtlglue_printf("=");
		rtlglue_printf( "===========" );
		for( j = OUT_START; j <= OUT_END; j++ )
			rtlglue_printf( "====" );
		rtlglue_printf( "\n" );
	}
#endif

	retval = SUCCESS;
	/* pattern generated successfully, free temp data */
	goto out4;
	
out7:
	{
		int i;
		for( i = 0; i<sizeof(pVsm->prefix[0].pStateMachine->subsm)/sizeof(pVsm->prefix[0].pStateMachine->subsm[0]); i++ )
		{
			if ( pVsm->prefix[0].pStateMachine->subsm[i] )
				rtlglue_free( pVsm->prefix[0].pStateMachine->subsm[i] );
			else
				break;
		}
	}
/*out6: nobody uses this symbol */
	pVsm->prefix[0].pStateMachine = CACHED_ADDRESS( pVsm->prefix[0].pStateMachine );
	rtlglue_free( pVsm->prefix[0].pStateMachine );
out5:
	rtlglue_free( pVsm->prefix );
out4:
	rtlglue_free( stateToRuleNo );
out3:
	rtlglue_free( cntRule );
out2:
	rtlglue_free( nextState );
out1:
	rtlglue_free( pszState_char );
out0:

	return retval;
}

