/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header file for VSM of pattern match
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: vsm.h,v 1.3 2006-07-13 16:03:07 chenyl Exp $
*/

#ifndef _PATTERN_MATCH_VSM_
#define _PATTERN_MATCH_VSM_

#include "gdma.h"



enum ENTITY_TYPE
{
	ENTYPE_STATE_MACHINE /* OPERAND */,
	ENTYPE_OR,
	ENTYPE_AND,
	ENTYPE_NOT,
	ENTYPE_FOLLOW,
};


struct exprEntity_s
{
	enum ENTITY_TYPE entityType;
	stateMachine_t* pStateMachine;
};
typedef struct exprEntity_s exprEntity_t;


struct pmVsm_s
{
	uint32 cntPrefix;
	exprEntity_t* prefix;
	uint32 result1;
	uint32 result2;
};
typedef struct pmVsm_s pmVsm_t;



int32 vsmInit( pmVsm_t* pVsm );
int32 vsmRun( pmVsm_t* pVsm, uint8* pkt, uint32 lenPkt );
int32 vsmFree( pmVsm_t* pVsm );
int32 vsmGenDOT( pmVsm_t* pVsm, char** ppDot );

#endif
