/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File of naptTest.c
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: naptTest.h,v 1.5 2006-10-25 06:17:20 evinlien Exp $
*/

#ifndef _NAPT_TEST_H_
#define _NAPT_TEST_H_

#define ASSERT(value)\
do {\
	if (!((int32)(value))) {\
		rtlglue_printf("\t%s(%d): errno=%d, fail, so abort!\n", __FUNCTION__, __LINE__, 0);\
		return FAILED; \
	}\
}while (0)

#define MBUF_CMPandFREE(m1, m2) \
do { \
	struct rtl_mBuf *m3; \
	struct rtl_mBuf *m4; \
	m3 = m1;\
	m4 = m2;\
	if (m3 && m4)\
	{ MBUF_CMP(m3, m4); }\
	if (m3) { freeMbuf(m3); } else {printf ("MBUF_CMPandFREE: (FAIL) mbuf 1 is NULL\n");}\
	if (m4) { freeMbuf(m4); } else {printf ("MBUF_CMPandFREE: (FAIL) mbuf 2 is NULL\n");}\
}while(0)

#define MBUF_CMP(mbuf1, mbuf2)		if (mbuf_cmp(mbuf1, mbuf2, (int8 *)__FUNCTION__, __LINE__) == FAILED) return FAILED;

#define FWDENG_INPUT(retval, hdr) \
do { \
	if (!hdr) { printf("FWDENG_INPUT : (FAIL) hdr == NULL\n"); retval = FWDENG_DROP; break;} \
	mbufList_Reinit(); \
	log_reinit(); \
	retval = rtl8651_fwdEngineInput((void *)hdr); \
}while (0)
	
#define START_TEST_CASE()	rtlglue_printf("@Running testing case %d(%s)......\n", caseNo, __FUNCTION__);
#define TCPUDP_ENTRY_CMP(entry1, entry2)	if (tcpudp_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define VALUE_CMP(val1, val2)		if( value_cmp((int32)val1, (int32)val2, (int8 *)__FUNCTION__, __LINE__) == FAILED) return FAILED;


extern int32 mbuf_cmp(struct rtl_mBuf* m1, struct rtl_mBuf* m2, int8* _fun_, uint32 _line_);
extern int32 rtl8651_log_init(void);


int32 testNaptLanToWan(uint32);
int32 testNaptLanToWan2(uint32);
int32 testNaptLanToWanPriority(uint32);
int32 testNaptEnhancedHash1(uint32);

//Tests for tblDriver
int32 AddNaptConnectionErrors(uint32);
int32 NaptTypicalCase(uint32);
int32 NaptTypicalCaseWith4Way(uint32);
int32 NaptTestL04(uint32);
int32 NaptTestL04UdpForCDRouter(uint32);
int32 testNapt4WayHash(uint32);
int32 DMZHostUsingHash1(uint32);
int32 ServerPortUsingOneHash2Entry(uint32);
int32 ServerPortUsingTwoHash2Entries(uint32);
int32 ServerPortAndNewHash1(uint32);
int32 TestUpdateNaptConnectionHybrid1(uint32);

int32 NaptEnhanceHash1TypicalCase(uint32);
int32 PacketCase(uint32);

#endif
