/*
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
*
* Program : Model Trace Function
* Abstract :
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)
* $Id: modelTrace.c,v 1.4 2006-12-01 07:28:05 qy_wang Exp $
*/

#include "rtl_types.h"
#include "gdma_glue.h"
#include "modelTrace.h"

uint32 mt_trace_mask = 0;

#if 0
int32 runModelTrace(uint32 userId,  int32 argc,int8 **saved)
{
	int8 *nextToken;
	int32 size;
	uint32 mask = 0;
#ifndef RTL865XC_MODEL_USE
	cle_getNextCmdToken(&nextToken,&size,saved);
#endif
	if ( !strcmp( nextToken, "all" ) )
		mask = 0xffffffff;
	else if ( !strcmp( nextToken, "watch" ) )
		mask = MT_WATCH_MASK;
	else if ( !strcmp( nextToken, "drop" ) )
		mask = MT_DROP_MASK;
	else if ( !strcmp( nextToken, "tocpu" ) )
		mask = MT_TOCPU_MASK;
	else
		return FAILED;
	
#ifndef RTL865XC_MODEL_USE
	cle_getNextCmdToken(&nextToken,&size,saved);
#endif

	if ( !strcmp( nextToken, "on" ) )
		mt_trace_mask |= mask;	
	else
		mt_trace_mask &= ~mask;

	rtlglue_printf( "mt_trace_mask=0x%08x\n", mt_trace_mask );

	return SUCCESS;
}
#endif
