/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : VSM routine
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: vsm.c,v 1.8 2006-07-13 16:03:07 chenyl Exp $
*/

#include "rtl_types.h"
#include "rtl_glue.h"
#include "asicRegs.h"
#include "gdma.h"
#include "vsm.h"


int32 vsmInit( pmVsm_t* pVsm )
{
	return SUCCESS;
}


int32 vsmRun( pmVsm_t* pVsm, uint8* pkt, uint32 lenPkt )
{
	return SUCCESS;
}


int32 vsmFree( pmVsm_t* pVsm )
{
	int idxPrefix;

	for( idxPrefix = 0; idxPrefix < pVsm->cntPrefix; idxPrefix++ )
	{
		int i;
		for( i = 0; 
		     i<sizeof(pVsm->prefix[idxPrefix].pStateMachine->subsm)/sizeof(pVsm->prefix[idxPrefix].pStateMachine->subsm[idxPrefix]);
		     i++ )
		{
			if ( pVsm->prefix[idxPrefix].pStateMachine->subsm[i] )
			{
#ifdef _WITH_DEBUG_SYMBOL_
				int j;
				for( j = 0;
				     j < INTERNAL_RULE_NUMBER;
				     j++ )
				{
					if ( pVsm->prefix[idxPrefix].pStateMachine->subsm[i]->rules[j].debugSymbol )
						rtlglue_free( pVsm->prefix[idxPrefix].pStateMachine->subsm[i]->rules[j].debugSymbol );
					else
						break;
				}
#endif

#if defined(RTL865X_MODEL_KERNEL)
				pVsm->prefix[idxPrefix].pStateMachine->subsm[i] = CACHED_ADDRESS( pVsm->prefix[idxPrefix].pStateMachine->subsm[i] );
#endif
				rtlglue_free( pVsm->prefix[idxPrefix].pStateMachine->subsm[i] );
			}
			else
				break;
		}
		
#if defined(RTL865X_MODEL_KERNEL)
		pVsm->prefix[idxPrefix].pStateMachine = CACHED_ADDRESS( pVsm->prefix[idxPrefix].pStateMachine );
#endif
		rtlglue_free( pVsm->prefix[idxPrefix].pStateMachine );
	}

	return SUCCESS;
}


#define DOT_SIZE (128*1024) /* 128KB */
/* sub-function of vsmGenDOT() */
static int32 safe_append( char* chBuf, char* pDot, int32* pOutSize )
{
	int32 bufLen;
	
	bufLen = strlen(chBuf);
	if ( *pOutSize + bufLen > DOT_SIZE-1 )
		return FAILED;

	memcpy( &pDot[*pOutSize], chBuf, bufLen+1 );
	*pOutSize += bufLen;

	return SUCCESS;
}

/* sub-function of vsmGenDOT() */
static char* printable( uint32 ch )
{
	static char buf[16];

	if ( ( ch >= '#' && ch <= ']' ) ||
	     ( ch >= '[' && ch <= 'z' ) )
		sprintf( buf, "%c", ch );
	else
		sprintf( buf, "0x%02x", ch );

	return buf;
}

/* sub-function of vsmGenDOT() */
static char* state( uint32 state, char* buf )
{
	if ( state < STATE_ESCAPE )
		sprintf( buf, "%d", state );
	else
		sprintf( buf, "{ESC_0x%X}", state );

	return buf;
}

/*
@func int32 | vsmGenDOT | Generate DOT file from a VSM
@parm pmVsm_t* | Pointer to VSM
@parm char** | Pointer to store new-allocated string
@rvalue SUCCESS | Success API call.
@comm

http://www.graphviz.org/
 */
int32 vsmGenDOT( pmVsm_t* pVsm, char** ppDot )
{
	char* pDot;
	int32 outSize;
	int idxPrefix;
	uint32 idxState;
	int32 ret = FAILED;
	static char chBuf[256];
	
	outSize = 0;
	idxState = 0;
	pDot = rtlglue_malloc( DOT_SIZE );
	if ( pDot == NULL ) goto out;
	pDot[outSize] = '\0'; /* Initialize */
	pDot[DOT_SIZE-1] = '\0'; /* for safe */
	*ppDot = pDot;
	chBuf[255] = '\0';

	snprintf( chBuf, sizeof(chBuf)-1, "\
digraph G \n\r\
{ \n\r\
	node [shape=ellipse, color=black, fillcolor=blue, fontcolor=white, style=filled]; \n\r\
	overlap=false; \n\r\
	sep=0.5; \n\r\
	rankdir=\"LR\"; \n\r\
	bgcolor=transparent; \n\r\n\r" );
	if ( ( ret = safe_append( chBuf, pDot, &outSize ) ) != SUCCESS )
		goto out;

	for( idxPrefix = 0; idxPrefix < pVsm->cntPrefix; )
	{
		int idxSubSM;
		exprEntity_t *pEntity;

		pEntity = &pVsm->prefix[idxPrefix];
		for( idxSubSM = 0; 
		     idxSubSM<sizeof(pEntity->pStateMachine->subsm)/sizeof(pEntity->pStateMachine->subsm[idxPrefix]);
		     idxSubSM++ )
		{
			subStateMachine_t *pSubSM;

			pSubSM = pEntity->pStateMachine->subsm[idxSubSM];
			if ( pSubSM )
			{
				int idxTranRule;

				for( idxTranRule = 0;
				     idxTranRule < sizeof(pSubSM->rules)/sizeof(pSubSM->rules[0]);
				     idxTranRule++ )
				{
					pmRule_t0 *pRule;
					uint32 ruleNo;
					char _strCurIdx[32];
					char _strNxtIdx[32];
					char *pszCurIdx;
					char *pszNxtIdx;

					ruleNo = idxSubSM*(sizeof(pSubSM->rules)/sizeof(pSubSM->rules[0]))+idxTranRule;
					if ( ruleNo >= pEntity->pStateMachine->cntTotalRules )
					{
						/* reach end of state machine */
						goto tail;
					}

					pRule = &pSubSM->rules[idxTranRule];
#ifdef _WITH_DEBUG_SYMBOL_
					{
						pmRule_t0 *pJump;
						if ( pRule->debugSymbol )
							pszCurIdx = pRule->debugSymbol;
						else
						{
							state( idxState, _strCurIdx );
							pszCurIdx = _strCurIdx;
						}

						if ( (pRule->jmpState&STATE_ESCAPE)==STATE_ESCAPE )
						{
							state( pRule->jmpState, _strNxtIdx );
							pszNxtIdx = _strNxtIdx;
						}
						else
						{
							pJump = &pEntity->pStateMachine->subsm[pRule->jmpState>>INTERNAL_RULE_ORDER]->rules[pRule->jmpState&(INTERNAL_RULE_NUMBER-1)];
							if ( pJump->debugSymbol )
							{
								pszNxtIdx = pJump->debugSymbol;
							}
							else
							{
								state( pRule->jmpState, _strNxtIdx );
								pszNxtIdx = _strNxtIdx;
							}
						}
					}
#else
					state( idxState, _strCurIdx );
					state( pRule->jmpState, _strNxtIdx );
					pszCurIdx = _strCurIdx;
					pszNxtIdx = _strNxtIdx;
#endif
					snprintf( chBuf, sizeof(chBuf)-1,
					          "\t%s -> %s [ color=green, fontcolor=red, label=\"%s%s\" ];\n\r",
					          pszCurIdx, pszNxtIdx, pRule->not?"!":" ", printable( pRule->matchChar ) );
					if ( ( ret = safe_append( chBuf, pDot, &outSize ) ) != SUCCESS )
						goto out;
					
					if ( pRule->not )
					{
						/* Yes, this is the last rule of this state. 
						   Update state index according to next rule index. */
						idxState = ruleNo + 1;
					}
				}
			}
			else
			{
				/* end of SM */
				goto nextPrefix;
			}
		}
nextPrefix:
		idxPrefix++;
	}

tail:
	snprintf( chBuf, sizeof(chBuf)-1,
	          "}\n\r" );
	if ( ( ret = safe_append( chBuf, pDot, &outSize ) ) != SUCCESS )
		goto out;

	/* add tailing \0 for snprintf() */
	ret = SUCCESS;
out:
	return ret;
}


