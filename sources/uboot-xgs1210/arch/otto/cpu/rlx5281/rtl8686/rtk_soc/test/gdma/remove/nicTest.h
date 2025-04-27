/*
* Copyright c                  Realtek Semiconductor Corporation, 2005
* All rights reserved.
* 
* Program : Model code for NIC
* Abstract : 
* Author : Tony Tzong-yn Su (tysu@realtek.com.tw)               
* $Id: nicTest.h,v 1.17 2006-11-06 13:45:49 ysleu Exp $
*/


#ifndef _NIC_MODEL_CODE_
#define _NIC_MODEL_CODE_

#include "rtl_types.h"
int configNicInit(void);
int configNicExtInit(void);


int32 testNicTx(uint32 caseNo);
int32 testNicTxQueuePriority(uint32 caseNo);
int32 testNicRx(uint32 caseNo);
int32 testNicRegs(uint32 caseNo);
int32 testNicRxRunout(uint32 caseNo);
int32 testNicRxPriority(uint32 caseNo);
int32 testNicRxQueueidDescringMapping(uint32 caseNo);
int32 testNicTxPppoeTagOffload(uint32 caseNo);
int32 testNicTxCrcCheck(uint32 caseNo);
int32 testNicTxJumboFrame(uint32 caseNo);
int32 testNicRxJumboFrame(uint32 caseNo);
int32 testNicRxPkthdr(uint32 caseNo);
int32 testNicRxMbufNewSpec(uint32 caseNo);
int32 testNicLoopback(uint32 caseNo);
int32 testNicTxTagAddRemoveModify(uint32 caseNo);
int32 testNicTxGreChecksumOffload(uint32 caseNo);
int32 testNicTxMibCounter(uint32 caseNo);
int32 testNicRxDRR(uint32 caseNo);

int32 testNicCpuReason(uint32 caseNo);

#endif

enum toWhichDescRing
{
	TO_CPU=0,
	TO_EXT1,
	TO_EXT2,
	TO_EXT3
};

