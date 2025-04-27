/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File for AES Test
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: aesTest.h,v 1.3 2006/07/13 16:03:06 chenyl Exp $
*/

#include "rtl_types.h"

int32 runAes8651bGeneralApiTest(uint32 round, uint32 funStart, uint32 funEnd, uint32 lenStart, uint32 lenEnd, uint32 offsetStart, uint32 offsetEnd);
int32 runAes8651bGeneralApiRandTest(uint32 seed, uint32 round);

