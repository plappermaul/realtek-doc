/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File of Model Code for 865xC
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: l2Test.h,v 1.14 2006-08-29 12:59:59 chenyl Exp $
*/

#ifndef _L2_TEST_
#define _L2_TEST_

#include "hsModel.h"



/* Since we have the following modes for test, we list the corresponding 
 *   setting in TMCR register.
 *
 *   Case A (pure ALE):
 *        HSB --> ALE --> HSA
 *
 *        TMCR.TxTMEnPM = [ignore];
 *        TMCR.RxTMEnPM = [ignore];
 *        TMCR.EnHSBtestMode = 1;
 *
 *   Case B (VERA:Mii-Like):
 *        MiiLikeRx --> Parser -> HSB -> ALE -> HSA -> Translator -> MiiLikeTx
 *
 *        TMCR.TxTMEnPM = 0xfc;
 *        TMCR.RxTMEnPM = 0xfc;
 *        TMCR.EnHSBtestMode = 0;
 *
 *   Case C (VSV):
 *        vsv_write_pkt -->Parser->HSB->ALE->HSA->Translator-> vsv_readPkt
 *
 *        TMCR.TxTMEnPM = [ignore];
 *        TMCR.RxTMEnPM = [ignore];
 *        TMCR.EnHSBtestMode = 0;
 *
 */
#if defined(VSV)
	#define TESTING_MODE		do { } while (0)
	#define NORMAL_MODE 		{WRITE_MEM32(TMCR,~ENHSBTESTMODE&READ_MEM32(TMCR));}		
#elif defined(MIILIKE)
	#define TESTING_MODE 		{WRITE_MEM32(TMCR,ENHSBTESTMODE|READ_MEM32(TMCR));}		
	#define NORMAL_MODE 		{WRITE_MEM32(TMCR,~ENHSBTESTMODE&READ_MEM32(TMCR));}		
#else
	#define TESTING_MODE 		{WRITE_MEM32(TMCR,ENHSBTESTMODE|READ_MEM32(TMCR));}		
	#define NORMAL_MODE 		{WRITE_MEM32(TMCR,~ENHSBTESTMODE&READ_MEM32(TMCR));}		
#endif


void modelCPUport(hsb_param_t*,hsa_param_t* );
void  AllPortForward(int state);

int32 testLayer2FIDLearning(uint32 caseNo);
int32 testLayer2TableReadWrite(uint32 caseNo);
int32 testLayer2UniLearning(uint32);
int32 testLayer2UniLRULearning(uint32);
int32 testLayer2UnwareVlan(uint32);
int32 testLayer2VlanNoExist(uint32);
int32 testLayer2UnkonwUniCast(uint32);
int32 testLayer2Broadcast(uint32);
int32 testLayer2Multicast(uint32);
int32 testLayer28021XPortBase(uint32);
int32 testLayer28021XMacBase(uint32);
int32 testLayer28021XGuestVlan(uint32);
int32 testRandom8021X(uint32);
int32 testRandom8021Q(uint32);
int32 testPrioritySelection(uint32);
int32 testProtocolBaseVlan(uint32);
int32 testSpanningTree(uint32);
int32 testLayer2UnKnownMACForward(uint32);
int32 testLayer2Forward(uint32);
int32 testRandomPrioritySelection(uint32 caseNo);
int32 testLayer2PriorityTag(uint32);
int32 testPureLayer2FID(uint32 caseNo);
int32 testLayer2Aging(uint32);
int32 testReservedMulticast(uint32);
int32 testPatternMatch(uint32);
int32 testLayer2TrapToCpu(uint32);
int32 testRandomSpanningTree(uint32);


#define _PKT_LEN (16*1024+64) /* packet length for sharedPkt */
extern uint8 sharedPkt[3][_PKT_LEN];


#endif
