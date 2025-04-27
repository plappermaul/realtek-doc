/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File for Model Trace Function
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: modelTrace.h,v 1.3 2006-07-13 15:58:51 chenyl Exp $
*/

#ifndef _MODEL_TRACE_
#define _MODEL_TRACE_

#include "rtl_types.h"


#define MT_WATCH_MASK        0x0001
#define MT_DROP_MASK         0x0002
#define MT_TOCPU_MASK        0x0004

extern uint32 mt_trace_mask;

/*
 * The MT_DEBUG_* macro set is used to trace the packet processing in model code.
 * You can insert them into your model code.
 *
 *  MT_WATCH() -- Show function name and line number when packet traversing.
 *  MT_DROP() -- Show message when ASIC decides to drop the packet.
 *  MT_TOCPU() -- Show message when ASIC decides to trap the packet to CPU.
 */
#define MT_WATCH( comment ) \
	if ( mt_trace_mask & MT_WATCH_MASK ) \
		rtlglue_printf( "[TRACE,WATCH] %s @%s():%d\n", comment, __FUNCTION__, __LINE__ );
	
#define MT_DROP( comment ) \
	if ( mt_trace_mask & MT_DROP_MASK ) \
		rtlglue_printf( "[TRACE,DROP] %s @%s():%d\n", comment, __FUNCTION__, __LINE__ );

#define MT_TOCPU( comment ) \
	if ( mt_trace_mask & MT_TOCPU_MASK ) \
		rtlglue_printf( "[TRACE,TOCPU] %s @%s():%d\n", comment, __FUNCTION__, __LINE__ );


int32 runModelTrace(uint32 userId,  int32 argc,int8 **saved);


#endif
