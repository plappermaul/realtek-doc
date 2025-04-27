/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File of Model Code for l34Test.h
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: l34Test.h,v 1.8 2006-08-09 06:41:35 yjlou Exp $
*/

#ifndef _L34_TEST_
#define _L34_TEST_

#include "hsModel.h"


uint8* strtomac(ether_addr_t *mac,int8 *str);
uint32 strtoip(ipaddr_t *ip, int8 *str);
int32 layer3Config(int);
int32 layer4Config(void);
uint32 getL2TableRowFromMacStr(int8 *str);
uint32 getL2TableRowFromMacStrFID(int8 *str,int);
int32 testLayer2UniLRULearning(uint32);
int32 testLayer2UnwareVlan(uint32);
int32 testLayer2VlanNoExist(uint32);
int32 testLayer2UnkonwUniCast(uint32);
int32 testLayer2Multicast(uint32);
int32 testLayer28021XPortBase(uint32);
int32 testLayer28021XMacBase(uint32);
int32 testLayer28021XGuestVlan(uint32);
int32 testPPTP(uint32);
int32 testRandom8021X(uint32);
int32 testRandom8021Q(uint32);
int32 testPrioritySelection(uint32);
int32 testProtocolBaseVlan(uint32);
int32 testSpanningTree(uint32);
int32 testLayer2Forward(uint32);
int32 testRandomPrioritySelection(uint32 caseNo);
int32 testLayer2PriorityTag(uint32);
int32 testPureLayer2FID(uint32 caseNo);
int32 testLayer2Aging(uint32);
int32 testReservedMulticast(uint32);
int32 testPatternMatch(uint32);
int32 testLayer3Routing(uint32);
int32  rtl865xc_netIfIndex(int32);
int32 testArpAging(uint32);
int32 testNawtThruEth(uint32);
int32 testLayer3MTU(uint32);
int32 testSPI(uint32);
int32 testServerPort(uint32);
int32 testLayer4Priority(uint32);
int32 testACLIPRule(uint32);
int32 testACLIcmpRule(uint32);
int32 testAclIgmpRule(uint32);
int32 testACLTCPRule(uint32);
int32 testACLSrcRule(uint32);
int32 testACLDstRule(uint32);
int32 testACLUDPRule(uint32);
int32 testACL(uint32);
int32 testUdpNAPTAging(uint32);
int32 testACLIPFrag(uint32);
int32 testIPMulticast(uint32);
int32 testLayer3GuestVLAN(uint32);
int32 testNatThruEth(uint32);
int32 testALG(uint32);
int32 testLocalPublic(uint32);
int32 testLocalPublicIP(uint32);
int32 testRandomServerPort(uint32);
int32 testGREAging(uint32 caseNo);
int32 testIPMulticastAging(uint32);
int32 testPPPoEAging(uint32);
int32 testEnhancedGRE(uint32);
int32 testLayer3RoutingToCpu(uint32);

#endif
