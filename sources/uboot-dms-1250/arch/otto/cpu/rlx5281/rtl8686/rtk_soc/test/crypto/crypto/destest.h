/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File for DES Test
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: destest.h,v 1.4 2006/07/13 16:03:07 chenyl Exp $
*/

#include "rtl_types.h"

int32 destest(void);
int32 des8651bTest(uint32 round, uint32 funStart, uint32 funEnd, uint32 lenStart, uint32 lenEnd, uint32 offsetStart, uint32 offsetEnd);
int32 des8651bAsicThroughput(uint32 round, uint32 startMode, uint32 endMode, uint32 pktLen);
int32 des8651bSwThroughput(uint32 round, uint32 startMode, uint32 endMode, uint32 pktLen);
int32 runDes8651bGeneralApiTest(uint32 round, uint32 funStart, uint32 funEnd, uint32 lenStart, uint32 lenEnd, uint32 offsetStart, uint32 offsetEnd);
int32 runDes8651bGeneralApiRandTest(uint32 seed, uint32 round);


