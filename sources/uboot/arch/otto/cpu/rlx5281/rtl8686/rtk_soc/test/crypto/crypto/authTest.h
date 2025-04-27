/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File for AUTH Test
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: authTest.h,v 1.1 2005/11/01 03:22:38 yjlou Exp $
*/

#ifndef AUTHTEST_H
#define AUTHTEST_H

#include "rtl_types.h"

int32 authTest_hashTest(void);
int32 authTest_hmacTest(void);
int32 runAuth8651bGeneralApiTest(uint32 round, uint32 funStart, uint32 funEnd, uint32 lenStart, uint32 lenEnd, uint32 keyLenStart, uint32 keyLenEnd, uint32 offsetStart, uint32 offsetEnd);
int32 runAuth8651bBatchApiTest(uint32 round, uint32 funStart, uint32 funEnd, uint32 lenStart, uint32 lenEnd, uint32 keyLenStart, uint32 keyLenEnd, uint32 offsetStart, uint32 offsetEnd, uint32 batchStart, uint32 batchEnd);
int32 runAuth8651bGeneralApiRandTest(uint32 seed, uint32 round);
int32 auth8651bAsicThroughput(uint32 round, uint32 startMode, uint32 endMode, uint32 pktLen);
int32 auth8651bSwThroughput(uint32 round, uint32 startMode, uint32 endMode, uint32 pktLen);

#endif
