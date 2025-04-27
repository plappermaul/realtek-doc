/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File of Model Code for 865xC
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: icTest.h,v 1.34 2007-09-06 08:39:55 stevewei Exp $
*/

#ifndef _MODEL_TEST_
#define _MODEL_TEST_

#include "gdma_glue.h"
//#include "hsModel.h"
#include "virtualMac.h"


#define IS_NOT_EQUAL_INT_DETAIL(str, v1, v2, _f_, _l_,i) \
	do{if ((v1) == (v2)) { \
		rtlglue_printf("\t%s(%u): %s\n\t%s: %d(0x%08x), %d(0x%08x)\n", _f_, _l_,i==IC_TYPE_REAL?"IC MODE":"MODEL MODE", str, v1, v1, v2, v2); \
		return FAILED; \
	}}while(0)

#define IS_EQUAL_INT_DETAIL(str, v1, v2, _f_, _l_,i) \
	do{if ((v1) != (v2)) { \
		rtlglue_printf("\t%s(%u): %s\n\t%s: %d(0x%08x), %d(0x%08x)", _f_, _l_, i==IC_TYPE_REAL?"IC MODE":"MODEL MODE",str, v1, v1,v2,v2); \
		return FAILED; \
	}}while(0)


#define IS_EQUAL_MAC_DETAIL(str, v1, v2, _f_, _l_,i) \
do { \
	if (memcmp((int8 *)&(v1), (int8 *)&(v2), 6)) { \
		rtlglue_printf("\t%s(%u):%s\n\t %s: %02x:%02x:%02x:%02x:%02x:%02x, %02x:%02x:%02x:%02x:%02x:%02x\n", _f_, _l_,i==IC_TYPE_REAL?"IC MODE":"MODEL MODE", str, \
		*( (uint8*)&v1+0),*( (uint8*)&v1+1),*( (uint8*)&v1+2),*( (uint8*)&v1+3),*( (uint8*)&v1+4),*( (uint8*)&v1+5),\
		*( (uint8*)&v2+0),*( (uint8*)&v2+1),*( (uint8*)&v2+2),*( (uint8*)&v2+3),*( (uint8*)&v2+4),*( (uint8*)&v2+5));\
		return FAILED; \
	} \
} while(0)

//int32 runModelTest(uint32 userId,  int32 argc,int8 **saved);


/* Bitmask for test code and model export */
#define AUTO_CASE_NO 0 /* auto-increased Case No */
#define GRP_NONE            0x80000000
#define GRP_ALL             0x00000001
#define GRP_PKT_PARSER      0x00000002
#define GRP_PKT_TRANSLATOR  0x00000004
#define GRP_PRE_CORE        0x00000008
#define GRP_L2              0x00000010
#define GRP_L34             0x00000020
#define GRP_POST_CORE       0x00000040
#define GRP_GDMA			0x00000080
#define GRP_NIC			0x00000100
#define GRP_NAPT		0x00000200
#define GRP_FT2		0x00000400

#define GROUP_TBLDRV	0x00000800

//int32 runModelTest(uint32 userId,  int32 argc,int8 **saved);
//int32 compHsa( hsa_param_t* hsa1, hsa_param_t* hsa2 );
//int32 compHsb( hsb_param_t* hsb1, hsb_param_t* hsb2 );
//int32 _rtl8651_setTarget(uint32 userId,  int32 argc,int8 **saved);
//extern void mbufList_init(void);

#endif
