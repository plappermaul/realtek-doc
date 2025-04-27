/*
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
*
* Program : File for 865xC L34 Packet Test
* Abstract :
* Author : (rupert@realtek.com.tw)
* $Id: l34PacketTest.c,v 1.38 2007-05-10 10:27:30 yjlou Exp $
*/

#include <rtl_types.h>
#include <rtl_glue.h>
#include <rtl_utils.h>
#if defined(RTL865X_TEST)||defined(RTL865X_MODEL_USER)
#define  CONFIG_RTL865XB
#include "utility.h"
#include <cle_userglue.h>
#else
#include "utility.h"
#endif
#include <cle_struct.h>
#include "pktGen.h"
#include  "pktParse.h"
#include "asicRegs.h"
#include "icTest.h"
#include "hsModel.h"
#ifdef CONFIG_RTL865XC
#include "rtl865xC_tblAsicDrv.h"
#else
#include "rtl8651_tblAsicDrv.h"
#endif
#define RTL8651_TBLDRV_LOCAL_H
#include "rtl8651_aclLocal.h"
#include "rtl8651_layer2local.h"
#include "drvTest.h"
#include "fwdTest.h"
#include "rtl8651_tblDrvFwd.h"
#include "virtualMac.h"
#include "PktProc/pktGen.h"
#include "l2Test.h"
#include "l34Test.h"
#include "l34Model.h"
#include "l34PacketTest.h"
#include "dump.h"
#include "nicTest.h"
#include "swNic2.h"

#define PPPOE_PAYLOADLEN_OFFSET  (0x12)
#define PPPOE_PAYLOADLEN_OFFSET_INCLUDE_VLANTAG (PPPOE_PAYLOADLEN_OFFSET+4)
#define 	ETHERTYPE_II	0x0700 /* Must higher than 0x600*/
#define 	LLC_OTHER		0xFFFF
#define   RFC_1042		0x1122
#if defined(CONFIG_RTL865X_MODEL_TEST_FT2)
#define   RANDOM_RUN_NUM 5
#else
#define   RANDOM_RUN_NUM 10000
#endif
#define IPX_ETHERTYPE	0xE0E0
#define PPPOECONTROL_ETHERTYPE 0x8863
#define PPPOESESSION_ETHERTYPE 0x8864
#if defined(CONFIG_RTL865X_MODEL_TEST_FT2)
#undef	VERBOSE
#else
#define VERBOSE 1
#endif
#define VID_0 0
#define WAN0_VID 3099
#define GW_WAN0_MAC "00-00-00-00-88-88"
#define WAN1_VID 9
#define GW_WAN1_MAC "00-00-00-00-88-89"
#define GW_WAN1_SUBNET_MASK "255.255.255.248"
#define LAN0_VID  88
#define GW_LAN0_MAC "00-00-00-00-88-8a"
#define GW_LAN0_IP "192.168.2.1"
#define GW_LAN0_SUBNET_MASK "255.255.255.248"
#define LAN1_VID 11
#define GW_LAN1_MAC "00-00-00-00-88-8b"
#define GW_LAN1_IP "192.168.1.1"
#define GW_LAN1_SUBNET_MASK "255.255.255.248"
/* isp 0 */
#define WAN0_ISP_MAC "00-00-00-00-99-9c"
#define WAN0_ISP_PPPOE_SID 0x55
#define WAN1_ISP_PPPOE_SID 0x56
#define WAN0_ISP_PUBLIC_IP "192.168.4.1"
#define WAN3_ISP_PUBLIC_IP "192.168.4.3"
/* isp 1 */
#define WAN1_ISP_PUBLIC_IP 	"192.168.3.1"
#define GW_WAN1_IP 			"192.168.3.1"
#define GW_WAN1_SUBNET_MASK "255.255.255.248"

/* wan 0 hosts */
#define HOST5_MAC "00-00-00-00-00-10"
#define HOST5_IP "192.168.4.5"
/* wan 1 hosts */
#define HOST4_MAC "00-00-00-00-00-20"
#define HOST4_IP "192.168.3.2"

/* lan 0 hosts */
#define HOST3_MAC "00-00-00-00-00-30"
#define HOST3_IP "192.168.2.2"
/* lan 1 hosts */
#define HOST1_MAC "00-00-00-00-00-40"
#define HOST1_IP "192.168.1.2"
#define HOST2_MAC "00-00-00-00-00-50"
#define HOST2_IP "192.168.1.3"
#define HOST6_MAC "00-00-00-00-00-60"
#define HOST6_IP "192.168.1.4"
#define MULTICAST_MAC "01-00-5e-00-00-03"
#define EXTERNAL_PORT 20
#define INTERNAL_PORT 30
#define LOWER_PORT	    80
#define UPPER_PORT	    180
#define MORE_FRAG	2




void stringToConfMac(uint8 *mac, int8 *str)
{
    uint32  t1, t2, t3, t4, t5, t6;
    sscanf((char*)str, "%x-%x-%x-%x-%x-%x", &t1, &t2, &t3, &t4, &t5, &t6);
    mac[0]=t1; mac[1]=t2; mac[2]=t3; mac[3]=t4; mac[4]=t5; mac[5]=t6;
}

void dumpPacket2TextPcap(char*pktptr,int len) {
	int i,j;
	unsigned char *pkt=pktptr;
	rtlglue_printf("PCAP Text Format  Len:%d \n",len);
	for (i=0;i<60;i+=16)
	{
		rtlglue_printf("%04x   ",i);
		for (j=0;j<8;j++) rtlglue_printf("%02X ",*(pkt+i+j));
		rtlglue_printf("   ");
		for (j=8;j<16;j++) rtlglue_printf("%02X ",*(pkt+i+j));
		rtlglue_printf("\n");
	}
	rtlglue_printf("PCAP_END \n");
}

/*  Output format of different packet content
 *    If defined, output the format for 'text2pcap' tools.
 *    If not defined, output the different part with color.
 */
#undef TEXT_PCAP
#ifdef TEXT_PCAP
#define  PKT_CMP(pktBuf, pktEBuf,pktLen,pktELen, func , line,  mode) \
do{\
	int ret;\
	if (pktBuf &&pktEBuf )\
	{\
		if ((ret=memcmp((void*)pktBuf,(void*)pktEBuf,pktELen)))\
		{\
			rtlglue_printf("\t%s(%u):%s  {UP:RESULT, DOWN:EXPECT}\n",func,line,mode==IC_TYPE_REAL?"IC MODE":"MODEL MODE");\
			dumpPacket2TextPcap(pktBuf,pktLen);\
			dumpPacket2TextPcap(pktEBuf,pktELen);\
			return FAILED;\
		}\
	}\
} while(0)
#else
#define  PKT_CMP(pktBuf, pktEBuf,pktLen,pktELen, func , line,  mode) \
do {\
	int ret;\
	if (pktBuf &&pktEBuf )\
	{\
		if ((ret=memcmp((void*)pktBuf,(void*)pktEBuf,pktELen)))\
		{\
			rtlglue_printf("\t%s(%u):%s  {UP:RESULT, DOWN:EXPECT}\n",func,line,mode==IC_TYPE_REAL?"IC MODE":"MODEL MODE");\
			memComp( pktBuf, pktEBuf, max(pktLen,pktELen), "" );\
			return FAILED;\
		}\
	}\
} while(0)

#endif


static uint8 memberPortList[3][MAX_PORT_NUMBER] = {{0,1,2,3,4,5},{0,1,2,3,4,5},{1,0,3,2,4,5}};
static uint8 untagList[3][MAX_PORT_NUMBER] = {{1,1,1,1,1,1},{0,0,0,0,0,0},{1,1,1,1,1,1}};
static uint8 dstData[2048]={0x21,0x81,0x88,0x0b,0x00,0x00,0x02,0x01,0x00,0x00,0x00,0x16};			


static int32  RoutingConfig( int configId ,int routetIsPPPoE)
{
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_l2Param_t l2t;
	rtl865x_tblAsicDrv_pppoeParam_t pppoet;
	rtl865x_tblAsicDrv_arpParam_t arpt;
	rtl865x_tblAsicDrv_routingParam_t routet;
	rtl865x_tblAsicDrv_nextHopParam_t nxthpt;
	ipaddr_t ip32;
	uint8 *memberP;
	uint8  *untagIf;
	
	memberP = memberPortList[configId];
	untagIf = untagList[configId];
	rtl8651_clearRegister();
	rtl8651_clearAsicAllTable();
	rtl8651_clearAsicPvid();
	rtl8651_setAsicPvid(memberP[5],WAN0_VID);  /*host 5 pvid 0*/
	rtl8651_setAsicPvid(memberP[4],9);   /*host 4 pvid 1*/
	rtl8651_setAsicPvid(memberP[3],LAN0_VID);  /*host 3 pvid 2*/
	rtl8651_setAsicPvid(memberP[2],11);  /*host 2 pvid 3*/
	rtl8651_setAsicPvid(memberP[1],11);  /*host 1 pvid 3*/
	/* module switch control reg: enable L2/L3/L4/STP, trap 802.1D/GARP to CPU */
	rtl8651_setAsicOperationLayer(3);
	/* TTL control reg: enable TTL-1 operation */
	WRITE_MEM32(ALECR,READ_MEM32(ALECR)|(uint32)EN_TTL1);	
	WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_51B_CPU_REASON);
	/* VLAN tagging control reg: accept all frame */
	 /* config vlan table */
	 bzero((void*) &intf, sizeof(intf));
	 bzero((void*) &aclt, sizeof(aclt));
	/* wan 0 */
	strtomac(&intf.macAddr, GW_WAN0_MAC);
	intf.macAddrNumber = 1;
	intf.vid = WAN0_VID;
	intf.mtu = 1500;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	intf.enableRoute = 1;
	intf.valid = 1;
	rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = 1<<memberP[5];
	if (untagIf[5])
		vlant.untagPortMask =  (1<<memberP[5]);
	rtl8651_setAsicVlan(intf.vid,&vlant);

	 /* ingress filter gateway mac */
	 memset((void*)&aclt, 0, sizeof(aclt));
	aclt.ruleType_ = RTL8651_ACL_MAC;
	aclt.actionType_ = RTL8651_ACL_PERMIT;
	aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
	rtl8651_setAsicAclRule(0, &aclt);
	 /* wan 1 */
	strtomac(&intf.macAddr, GW_WAN1_MAC);
	intf.vid = VID_0;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask =  (1<<memberP[4])|(1<<memberP[0]);
	if (untagIf[4])
		vlant.untagPortMask =  (1<<memberP[4]);
	if (untagIf[0])
		vlant.untagPortMask |=  (1<<memberP[0]);

	rtl8651_setAsicVlan(intf.vid,&vlant);
	/* ingress filter gateway mac */
	aclt.ruleType_ = RTL8651_ACL_MAC;
	memcpy(aclt.srcMac_.octet, intf.macAddr.octet, 6);
	aclt.actionType_ = RTL8651_ACL_CPU;
	aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
	rtl8651_setAsicAclRule(4, &aclt);
	/* lan 0 */
	strtomac(&intf.macAddr, GW_LAN0_MAC);
	intf.vid = LAN0_VID;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = (1<<memberP[3]);
	if (untagIf[3])
		vlant.untagPortMask =  (1<<memberP[3]);

	vlant.untagPortMask = 0; /*tagged*/
	rtl8651_setAsicVlan(intf.vid,&vlant);
	/* ingress filter gateway mac */
	aclt.ruleType_ = RTL8651_ACL_MAC;
	memcpy(aclt.srcMac_.octet, intf.macAddr.octet, 6);
	aclt.actionType_ = RTL8651_ACL_CPU;
	aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
	rtl8651_setAsicAclRule(8, &aclt);
	/* lan 1 */
	strtomac(&intf.macAddr, GW_LAN1_MAC);
	intf.vid = LAN1_VID;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask =  (1<<memberP[2])|(1<<memberP[1]);
	if (untagIf[2])
		vlant.untagPortMask =  (1<<memberP[2]);
	if (untagIf[1])
		vlant.untagPortMask |=  (1<<memberP[1]);


	rtl8651_setAsicVlan(intf.vid,&vlant);
	 /* ingress filter gateway mac */
	aclt.ruleType_ = RTL8651_ACL_MAC;
	memcpy(aclt.srcMac_.octet, intf.macAddr.octet, 6);
	aclt.actionType_ = RTL8651_ACL_CPU;
	aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
	rtl8651_setAsicAclRule(12, &aclt);    
	/* config l2 table */
	bzero((void*) &l2t, sizeof(l2t));
	/* wan 0 isp modem */
	strtomac(&l2t.macAddr, WAN0_ISP_MAC);
	l2t.memberPortMask = (1<<memberP[5]);
	l2t.isStatic = 1;
	l2t.nhFlag=TRUE;
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(WAN0_ISP_MAC), 0, &l2t);
	/* wan 1 host */
	strtomac(&l2t.macAddr, HOST4_MAC);
	l2t.memberPortMask = (1<<memberP[4]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST4_MAC), 0, &l2t);
	/* lan 0 host */
	strtomac(&l2t.macAddr, HOST3_MAC);
	l2t.memberPortMask = (1<<memberP[3]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST3_MAC), 0, &l2t);
	/* lan 1 host 1 */
	strtomac(&l2t.macAddr, HOST2_MAC);
	l2t.memberPortMask = (1<<memberP[2]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST2_MAC), 0, &l2t);
	/* lan 1 host 0 */
	strtomac(&l2t.macAddr, HOST1_MAC);
	l2t.memberPortMask = (1<<memberP[1]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST1_MAC), 0, &l2t);
	/* lan 1 host 5 */
	strtomac(&l2t.macAddr, HOST6_MAC);
	l2t.memberPortMask = 1<<memberP[2];
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST6_MAC), 0, &l2t);
	    
	/* config pppoe table */
	bzero((void*) &pppoet, sizeof(pppoet));
	pppoet.sessionId = WAN0_ISP_PPPOE_SID;
	rtl8651_setAsicPppoe(0, &pppoet);
	/* config pppoe table */
	bzero((void*) &pppoet, sizeof(pppoet));
	pppoet.sessionId = WAN0_ISP_PPPOE_SID+1;
	rtl8651_setAsicPppoe(1, &pppoet);

	/* config arp table */
	bzero((void*) &arpt, sizeof(arpt));
	/* wan 1 host */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	strtoip(&ip32, HOST4_IP);
	rtl8651_setAsicArp(ip32 & 0x7, &arpt);
	/* lan 0 host */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST3_MAC);
	strtoip(&ip32, HOST3_IP);
	rtl8651_setAsicArp(8 + (ip32 & 0x7), &arpt);
    
	/* lan 1 host 1 */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST2_MAC);
	strtoip(&ip32, HOST2_IP);
	rtl8651_setAsicArp(16 + (ip32 & 0xff), &arpt);	
	/* lan 1 host 0 */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST1_MAC);
	strtoip(&ip32, HOST1_IP);
	rtl8651_setAsicArp(16 + (ip32 & 0xff), &arpt);
	 /* lan 1 host 5 */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST6_MAC);
	strtoip(&ip32, HOST6_IP);
	rtl8651_setAsicArp(16 + (ip32 & 0xff), &arpt);
	/* config routing table */
	bzero((void*) &routet, sizeof(routet));
	/* route to wan 1: use arp table [0:7] */
	strtoip(&routet.ipAddr, GW_WAN1_IP);
	strtoip(&routet.ipMask, GW_WAN1_SUBNET_MASK);
	routet.process = PROCESS_INDIRECT;
	routet.vidx = rtl865xc_netIfIndex(VID_0);
	routet.arpStart = 0;
	routet.arpEnd = 0;
	routet.internal=1;
 	rtl8651_setAsicRouting(0, &routet);
	
	/* route to lan 0: use arp table [8:15] */
	strtoip(&routet.ipAddr, GW_LAN0_IP);
	strtoip(&routet.ipMask, GW_LAN0_SUBNET_MASK);
	routet.process = PROCESS_INDIRECT;
	routet.vidx = rtl865xc_netIfIndex(LAN0_VID);
	routet.arpStart = 1;
	routet.arpEnd = 1;
	routet.internal=1;
	rtl8651_setAsicRouting(1, &routet);
	 /* route to lan 1: use arp table [16:23] */
	 strtoip(&routet.ipAddr, GW_LAN1_IP);
	strtoip(&routet.ipMask, GW_LAN1_SUBNET_MASK);
	routet.process = PROCESS_INDIRECT;
	routet.vidx = rtl865xc_netIfIndex(LAN1_VID);
	routet.arpStart = 2;
	routet.arpEnd = 2;
	routet.internal=1;
	rtl8651_setAsicRouting(6, &routet);
	/* default route to wan 0: direct to modem */
	 bzero((void*) &routet, sizeof(routet));
	 strtoip(&routet.ipAddr, "0.0.0.0");
	strtoip(&routet.ipMask, "0.0.0.0");
	routet.process = PROCESS_NXT_HOP;
	routet.vidx = rtl865xc_netIfIndex(WAN0_VID);
	routet.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);

	if (routetIsPPPoE)
	{
		routet.nhStart = 30;
		routet.nhNum = 2; /*index 4~5*/
		routet.nhNxt = 0;
		routet.nhAlgo = 2; /* per-source*/
		routet.ipDomain = 0;
		routet.internal=1;
	}
	else
	{
		routet.nhStart = 0;
		routet.nhNum = 2; 
		routet.nhNxt = 0;
		routet.nhAlgo = 2; 
		routet.ipDomain = 0;
		routet.internal=1;
	}
	rtl8651_setAsicRouting(7, &routet);
	 /* config next hop table */
	 bzero((void*) &nxthpt, sizeof(nxthpt));
	 /* wan 0: load balance traffic through isp0 & isp1 as 3:1 */
	/* isp 0 occupies 3 entries */
	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0; 
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0;
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(0, &nxthpt);       

	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0;
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0;
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(1, &nxthpt);       


	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 1; /*WAN0_ISP0_PPPOE_SID*/
	nxthpt.dvid =1;/* rtl865xc_netIfIndex(WAN1_VID);*/
	nxthpt.extIntIpIdx = 0; /*WAN0_ISP0_NPE_IP*/
	nxthpt.isPppoe = 1;
	rtl8651_setAsicNextHopTable(30, &nxthpt);       

	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 1;/*WAN0_ISP0_PPPOE_SID*/
	nxthpt.dvid =0;/* rtl865xc_netIfIndex(WAN1_VID);*/
	nxthpt.extIntIpIdx = 0; /*WAN0_ISP0_NPE_IP*/
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(31, &nxthpt);       
	return 0;
	
}
static int32  RoutingConfig1( int configId ,int routetIsPPPoE)
{
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_l2Param_t l2t;
	rtl865x_tblAsicDrv_pppoeParam_t pppoet;
	rtl865x_tblAsicDrv_arpParam_t arpt;
	rtl865x_tblAsicDrv_routingParam_t routet;
	rtl865x_tblAsicDrv_nextHopParam_t nxthpt;
	ipaddr_t ip32;
	uint8 *memberP;
	uint8  *untagIf;
	
	memberP = memberPortList[configId];
	untagIf = untagList[configId];
	rtl8651_clearRegister();
	rtl8651_clearAsicAllTable();
	rtl8651_clearAsicPvid();
	rtl8651_setAsicPvid(memberP[5],WAN0_VID);  /*host 5 pvid 0*/
	rtl8651_setAsicPvid(memberP[4],9);   /*host 4 pvid 1*/
	rtl8651_setAsicPvid(memberP[3],LAN0_VID);  /*host 3 pvid 2*/
	rtl8651_setAsicPvid(memberP[2],11);  /*host 2 pvid 3*/
	rtl8651_setAsicPvid(memberP[1],11);  /*host 1 pvid 3*/
	/* module switch control reg: enable L2/L3/L4/STP, trap 802.1D/GARP to CPU */
	rtl8651_setAsicOperationLayer(3);
	/* TTL control reg: enable TTL-1 operation */
	WRITE_MEM32(ALECR,READ_MEM32(ALECR)|(uint32)EN_TTL1);	
	WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_51B_CPU_REASON);
	/* VLAN tagging control reg: accept all frame */
	 /* config vlan table */
	 bzero((void*) &intf, sizeof(intf));
	 bzero((void*) &aclt, sizeof(aclt));
	/* wan 0 */
	strtomac(&intf.macAddr, GW_WAN0_MAC);
	intf.macAddrNumber = 1;
	intf.vid = WAN0_VID;
	intf.mtu = 1500;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	intf.enableRoute = 1;
	intf.valid = 1;
	rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = 1<<memberP[5];
	if (untagIf[5])
		vlant.untagPortMask =  (1<<memberP[5]);
	rtl8651_setAsicVlan(intf.vid,&vlant);

	 /* ingress filter gateway mac */
	 memset((void*)&aclt, 0, sizeof(aclt));
	aclt.ruleType_ = RTL8651_ACL_MAC;
	aclt.actionType_ = RTL8651_ACL_PERMIT;
	aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
	rtl8651_setAsicAclRule(0, &aclt);
	 /* wan 1 */
	strtomac(&intf.macAddr, GW_WAN1_MAC);
	intf.vid = VID_0;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask =  (1<<memberP[4])|(1<<memberP[0]);
	if (untagIf[4])
		vlant.untagPortMask =  (1<<memberP[4]);
	if (untagIf[0])
		vlant.untagPortMask |=  (1<<memberP[0]);

	rtl8651_setAsicVlan(intf.vid,&vlant);
	/* ingress filter gateway mac */
	aclt.ruleType_ = RTL8651_ACL_MAC;
	memcpy(aclt.srcMac_.octet, intf.macAddr.octet, 6);
	aclt.actionType_ = RTL8651_ACL_CPU;
	aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
	rtl8651_setAsicAclRule(4, &aclt);
	/* lan 0 */
	strtomac(&intf.macAddr, GW_LAN0_MAC);
	intf.vid = LAN0_VID;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = (1<<memberP[3]);
	if (untagIf[3])
		vlant.untagPortMask =  (1<<memberP[3]);

	vlant.untagPortMask = 0; /*tagged*/
	rtl8651_setAsicVlan(intf.vid,&vlant);
	/* ingress filter gateway mac */
	aclt.ruleType_ = RTL8651_ACL_MAC;
	memcpy(aclt.srcMac_.octet, intf.macAddr.octet, 6);
	aclt.actionType_ = RTL8651_ACL_CPU;
	aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
	rtl8651_setAsicAclRule(8, &aclt);
	/* lan 1 */
	strtomac(&intf.macAddr, GW_LAN1_MAC);
	intf.vid = LAN1_VID;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask =  (1<<memberP[2])|(1<<memberP[1]);
	if (untagIf[2])
		vlant.untagPortMask =  (1<<memberP[2]);
	if (untagIf[1])
		vlant.untagPortMask |=  (1<<memberP[1]);
	rtl8651_setAsicVlan(intf.vid,&vlant);
	 /* ingress filter gateway mac */
	aclt.ruleType_ = RTL8651_ACL_MAC;
	memcpy(aclt.srcMac_.octet, intf.macAddr.octet, 6);
	aclt.actionType_ = RTL8651_ACL_CPU;
	aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
	rtl8651_setAsicAclRule(12, &aclt);    
	/* config l2 table */
	bzero((void*) &l2t, sizeof(l2t));
	/* wan 0 isp modem */
	strtomac(&l2t.macAddr, WAN0_ISP_MAC);
	l2t.memberPortMask = (1<<memberP[5]);
	l2t.isStatic = 1;
	l2t.nhFlag=TRUE;
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(WAN0_ISP_MAC), 0, &l2t);
	/* wan 1 host */
	strtomac(&l2t.macAddr, HOST4_MAC);
	l2t.memberPortMask = (1<<memberP[4]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST4_MAC), 0, &l2t);
	/* lan 0 host */
	strtomac(&l2t.macAddr, HOST3_MAC);
	l2t.memberPortMask = (1<<memberP[3]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST3_MAC), 0, &l2t);
	/* lan 1 host 1 */
	strtomac(&l2t.macAddr, HOST2_MAC);
	l2t.memberPortMask = (1<<memberP[2]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST2_MAC), 0, &l2t);
	/* lan 1 host 0 */
	strtomac(&l2t.macAddr, HOST1_MAC);
	l2t.memberPortMask = (1<<memberP[1]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST1_MAC), 0, &l2t);
	/* lan 1 host 5 */
	strtomac(&l2t.macAddr, HOST6_MAC);
	l2t.memberPortMask = 1<<memberP[2];
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST6_MAC), 0, &l2t);
	    
	/* config pppoe table */
	bzero((void*) &pppoet, sizeof(pppoet));
	pppoet.sessionId = WAN0_ISP_PPPOE_SID;
	rtl8651_setAsicPppoe(0, &pppoet);
	/* config pppoe table */
	bzero((void*) &pppoet, sizeof(pppoet));
	pppoet.sessionId = WAN0_ISP_PPPOE_SID+1;
	rtl8651_setAsicPppoe(1, &pppoet);

	bzero((void*) &arpt, sizeof(arpt));
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	strtoip(&ip32, "192.168.3.24");
	rtl8651_setAsicArp((ip32 & 0xff), &arpt);	

	

	/* config routing table */
	bzero((void*) &routet, sizeof(routet));
	/* route to wan 1: use arp table [0:7] */
	strtoip(&routet.ipAddr, GW_WAN1_IP);
	strtoip(&routet.ipMask, "255.255.255.0");
	routet.process = PROCESS_INDIRECT;
	routet.vidx = rtl865xc_netIfIndex(VID_0);
	routet.arpStart = 0;
	routet.arpEnd = 4;
	routet.internal=1;
 	rtl8651_setAsicRouting(0, &routet);
	
	/* default route to wan 0: direct to modem */
	 bzero((void*) &routet, sizeof(routet));
	 strtoip(&routet.ipAddr, "0.0.0.0");
	strtoip(&routet.ipMask, "0.0.0.0");
	routet.process = PROCESS_NXT_HOP;
	routet.vidx = rtl865xc_netIfIndex(WAN0_VID);
	routet.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);

	if (routetIsPPPoE)
	{
		routet.nhStart = 30;
		routet.nhNum = 2; /*index 4~5*/
		routet.nhNxt = 0;
		routet.nhAlgo = 2; /* per-source*/
		routet.ipDomain = 0;
		routet.internal=1;
	}
	else
	{
		routet.nhStart = 0;
		routet.nhNum = 2; 
		routet.nhNxt = 0;
		routet.nhAlgo = 2; 
		routet.ipDomain = 0;
		routet.internal=1;
	}
	rtl8651_setAsicRouting(7, &routet);
	 /* config next hop table */
	 bzero((void*) &nxthpt, sizeof(nxthpt));
	 /* wan 0: load balance traffic through isp0 & isp1 as 3:1 */
	/* isp 0 occupies 3 entries */
	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0; 
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0;
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(0, &nxthpt);       

	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0;
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0;
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(1, &nxthpt);       


	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 1; /*WAN0_ISP0_PPPOE_SID*/
	nxthpt.dvid =1;/* rtl865xc_netIfIndex(WAN1_VID);*/
	nxthpt.extIntIpIdx = 0; /*WAN0_ISP0_NPE_IP*/
	nxthpt.isPppoe = 1;
	rtl8651_setAsicNextHopTable(30, &nxthpt);       

	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 1;/*WAN0_ISP0_PPPOE_SID*/
	nxthpt.dvid =0;/* rtl865xc_netIfIndex(WAN1_VID);*/
	nxthpt.extIntIpIdx = 0; /*WAN0_ISP0_NPE_IP*/
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(31, &nxthpt);       
	return 0;
	
}

static int32  NaptConfig( int configId,int32 mtu,int routetIsPPPoE,int algo)
{

	rtl865x_tblAsicDrv_extIntIpParam_t ipt;	
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_l2Param_t l2t;
	rtl865x_tblAsicDrv_pppoeParam_t pppoet;
	rtl865x_tblAsicDrv_arpParam_t arpt;
	rtl865x_tblAsicDrv_routingParam_t routet;
	rtl865x_tblAsicDrv_nextHopParam_t nxthpt;
	ipaddr_t ip32;
	uint8 *memberP;
	uint8  *untagIf;

	memberP = memberPortList[configId];
	untagIf = untagList[configId];
	rtl8651_clearRegister();
	rtl8651_clearAsicAllTable();
	rtl8651_clearAsicPvid();
	rtl8651_setAsicPvid(memberP[5],WAN0_VID);  /*host 5 pvid 0*/
	rtl8651_setAsicPvid(memberP[4],9);   /*host 4 pvid 1*/
	rtl8651_setAsicPvid(memberP[3],LAN0_VID);  /*host 3 pvid 2*/
	rtl8651_setAsicPvid(memberP[2],11);  /*host 2 pvid 3*/
	rtl8651_setAsicPvid(memberP[1],11);  /*host 1 pvid 3*/
	/* module switch control reg: enable L2/L3/L4/STP, trap 802.1D/GARP to CPU */
	if (mtu>1500)
	{
		WRITE_MEM32(PCRP0, (READ_MEM32(PCRP0)&~6)|0x3<<1 ); /* enforce first, to speed up */
		WRITE_MEM32(PCRP1, READ_MEM32(PCRP1)|0x3<<1 ); /* enforce first, to speed up */
		WRITE_MEM32(PCRP2, READ_MEM32(PCRP2)|0x3<<1 ); /* enforce first, to speed up */
		WRITE_MEM32(PCRP3, (READ_MEM32(PCRP3)&~6)|0x3<<1 ); /* enforce first, to speed up */
		WRITE_MEM32(PCRP4, READ_MEM32(PCRP4)|0x3<<1 ); /* enforce first, to speed up */
		WRITE_MEM32(PCRP5, (READ_MEM32(PCRP5)&~6)|0x3<<1 ); /* enforce first, to speed up */	
	}
	rtl8651_setAsicOperationLayer(4);
	/* TTL control reg: enable TTL-1 operation */
	WRITE_MEM32(ALECR,READ_MEM32(ALECR)|(uint32)EN_TTL1);	
	WRITE_MEM32(TEACR,0xffffffff);
	WRITE_MEM32(TEATCR,0xffffffff);

	WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_51B_CPU_REASON);

	 /* config vlan table */
	 bzero((void*) &intf, sizeof(intf));
	 bzero((void*) &aclt, sizeof(aclt));
	/* wan 0 */
	strtomac(&intf.macAddr, GW_WAN0_MAC);
	intf.macAddrNumber = 1;
	intf.vid = WAN0_VID;
	intf.mtu = mtu;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	intf.enableRoute = 1;
	intf.valid = 1;
	rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask =  (1<<memberP[5]);
	if (untagIf[5])
		vlant.untagPortMask =  (1<<memberP[5]);
	rtl8651_setAsicVlan(intf.vid,&vlant);
	 /* ingress filter gateway mac */
	 memset((void*)&aclt, 0, sizeof(aclt));
	aclt.ruleType_ = RTL8651_ACL_MAC;
	aclt.actionType_ = RTL8651_ACL_PERMIT;
	aclt.pktOpApp    = RTL865XC_ACLTBL_ALL_LAYER;
	rtl8651_setAsicAclRule(0, &aclt);
	 /* wan 1 */
	strtomac(&intf.macAddr, GW_WAN1_MAC);
	intf.vid = WAN1_VID;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = (1<<memberP[4]);
	if (untagIf[4])
		vlant.untagPortMask =  (1<<memberP[4]);
	rtl8651_setAsicVlan(intf.vid,&vlant);

	/* ingress filter gateway mac */
	aclt.ruleType_ = RTL8651_ACL_MAC;
	memcpy(aclt.srcMac_.octet, intf.macAddr.octet, 6);
	aclt.actionType_ = RTL8651_ACL_CPU;
	aclt.pktOpApp			= RTL865XC_ACLTBL_ALL_LAYER;
	rtl8651_setAsicAclRule(4, &aclt);
	/* lan 0 */
	strtomac(&intf.macAddr, GW_LAN0_MAC);
	intf.vid = LAN0_VID;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = (1<<memberP[3]);
	if (untagIf[3])
		vlant.untagPortMask =  (1<<memberP[3]);

	rtl8651_setAsicVlan(intf.vid,&vlant);
	
	/* ingress filter gateway mac */
	aclt.ruleType_ = RTL8651_ACL_MAC;
	memcpy(aclt.srcMac_.octet, intf.macAddr.octet, 6);
	aclt.actionType_ = RTL8651_ACL_CPU;
	aclt.pktOpApp			= RTL865XC_ACLTBL_ALL_LAYER;	
	rtl8651_setAsicAclRule(8, &aclt);
	/* lan 1 */
	strtomac(&intf.macAddr, GW_LAN1_MAC);
	intf.vid = LAN1_VID;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask   = (1<<memberP[2])|(1<<memberP[1]);
	if (untagIf[2])
		vlant.untagPortMask =  (1<<memberP[2]);
	if (untagIf[1])
		vlant.untagPortMask |=  (1<<memberP[1]);
	rtl8651_setAsicVlan(intf.vid,&vlant);
	 /* ingress filter gateway mac */
	aclt.ruleType_ = RTL8651_ACL_MAC;
	memcpy(aclt.srcMac_.octet, intf.macAddr.octet, 6);
	aclt.actionType_ = RTL8651_ACL_CPU;
	aclt.pktOpApp			= RTL865XC_ACLTBL_ALL_LAYER;
	rtl8651_setAsicAclRule(12, &aclt);    
	/* config l2 table */
	bzero((void*) &l2t, sizeof(l2t));
	/* wan 0 isp modem */
	strtomac(&l2t.macAddr, WAN0_ISP_MAC);
	l2t.memberPortMask = (1<<memberP[5]);
	l2t.isStatic = 1;
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(WAN0_ISP_MAC), 0, &l2t);
	/* wan 1 host */
	strtomac(&l2t.macAddr, HOST4_MAC);
	l2t.memberPortMask = (1<<memberP[4]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST4_MAC), 0, &l2t);
	/* lan 0 host */
	strtomac(&l2t.macAddr, HOST3_MAC);
	l2t.memberPortMask = (1<<memberP[3]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST3_MAC), 0, &l2t);
	/* lan 1 host 1 */
	strtomac(&l2t.macAddr, HOST2_MAC);
	l2t.memberPortMask = (1<<memberP[2]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST2_MAC), 0, &l2t);
	/* lan 1 host 0 */
	strtomac(&l2t.macAddr, HOST1_MAC);
	l2t.memberPortMask = (1<<memberP[1]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST1_MAC), 0, &l2t);
	/* lan 1 host 5 */
	strtomac(&l2t.macAddr, HOST6_MAC);
	l2t.memberPortMask = (1<<memberP[5]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST6_MAC), 0, &l2t);
	    
	/* config pppoe table */
	bzero((void*) &pppoet, sizeof(pppoet));
	pppoet.sessionId = WAN0_ISP_PPPOE_SID;
	rtl8651_setAsicPppoe(0, &pppoet);
	/* config pppoe table */
	bzero((void*) &pppoet, sizeof(pppoet));
	pppoet.sessionId = WAN1_ISP_PPPOE_SID;
	rtl8651_setAsicPppoe(1, &pppoet);

	/* config arp table */
	bzero((void*) &arpt, sizeof(arpt));
	/* wan 1 host */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	strtoip(&ip32, HOST4_IP);
	rtl8651_setAsicArp(ip32 & 0x7, &arpt);
	/* lan 0 host */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST3_MAC);
	strtoip(&ip32, HOST3_IP);
	rtl8651_setAsicArp(8 + (ip32 & 0x7), &arpt);
    
	/* lan 1 host 1 */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST2_MAC);
	strtoip(&ip32, HOST2_IP);
	rtl8651_setAsicArp(16 + (ip32 & 0xff), &arpt);		
	/* lan 1 host 0 */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST1_MAC);
	strtoip(&ip32, HOST1_IP);
	rtl8651_setAsicArp(16 + (ip32 & 0xff), &arpt);
	 /* lan 1 host 5 */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST6_MAC);
	strtoip(&ip32, HOST6_IP);
	rtl8651_setAsicArp(16 + (ip32 & 0xff), &arpt);
	/* config routing table */
	bzero((void*) &routet, sizeof(routet));
	/* route to wan 1: use arp table [0:7] */
	strtoip(&routet.ipAddr, GW_WAN1_IP);
	strtoip(&routet.ipMask, GW_WAN1_SUBNET_MASK);
	routet.process = PROCESS_INDIRECT;
	routet.vidx = rtl865xc_netIfIndex(WAN1_VID);
	routet.arpStart = 0;
	routet.arpEnd = 0;
	routet.internal=0;
 	rtl8651_setAsicRouting(0, &routet);
	
	/* route to lan 0: use arp table [8:15] */
	strtoip(&routet.ipAddr, GW_LAN0_IP);
	strtoip(&routet.ipMask, GW_LAN0_SUBNET_MASK);
	routet.process = PROCESS_INDIRECT;
	routet.vidx = rtl865xc_netIfIndex(LAN0_VID);
	routet.arpStart = 1;
	routet.arpEnd = 1;
	routet.internal=1;
	rtl8651_setAsicRouting(1, &routet);
	 /* route to lan 1: use arp table [16:23] */
	 strtoip(&routet.ipAddr, GW_LAN1_IP);
	strtoip(&routet.ipMask, GW_LAN1_SUBNET_MASK);
	routet.process = PROCESS_INDIRECT;
	routet.vidx = rtl865xc_netIfIndex(LAN1_VID);
	routet.arpStart = 2;
	routet.arpEnd = 2;
	routet.internal=1;
	rtl8651_setAsicRouting(2, &routet);
	/* default route to wan 0: direct to modem */
	 bzero((void*) &routet, sizeof(routet));
	 strtoip(&routet.ipAddr, "0.0.0.0");
	strtoip(&routet.ipMask, "0.0.0.0");
	routet.process = PROCESS_NXT_HOP;
	routet.vidx = rtl865xc_netIfIndex(WAN0_VID);
	routet.internal=0;
	routet.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	if (routetIsPPPoE)
	{
		routet.nhStart = 30;
		routet.nhNum = 2; /*index 4~5*/
		routet.nhNxt = 0;
		/* Load Balance Algorithm
			0 per-packet roud roubin
			1 per-session
			2 per-source
		*/
		routet.nhAlgo = algo; 
		routet.ipDomain = 0;
		routet.internal=0;
	}
	else
	{
		routet.nhStart = 0;
		routet.nhNum = 2; 
		routet.nhNxt = 0;
		routet.nhAlgo = algo; 
		routet.ipDomain = 0;
		routet.internal=0;
	}
	rtl8651_setAsicRouting(7, &routet);
	 /* config next hop table */
	 bzero((void*) &nxthpt, sizeof(nxthpt));
	 /* wan 0: load balance traffic through isp0 & isp1 as 3:1 */
	/* isp 0 occupies 3 entries */
	 bzero((void*) &nxthpt, sizeof(nxthpt));
	 /* wan 0: load balance traffic through isp0 & isp1 as 3:1 */
	/* isp 0 occupies 3 entries */
	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0; 
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0;
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(0, &nxthpt);       

	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0; 
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0; 
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(1, &nxthpt);       	
	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 0; 
	nxthpt.dvid = rtl865xc_netIfIndex(WAN1_VID);
	nxthpt.extIntIpIdx = 0;
	nxthpt.isPppoe = 1;
	rtl8651_setAsicNextHopTable(30, &nxthpt);     
	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 0;
	nxthpt.dvid = rtl865xc_netIfIndex(WAN1_VID);
	nxthpt.extIntIpIdx = 0; 
	nxthpt.isPppoe = 1;
	rtl8651_setAsicNextHopTable(29, &nxthpt);       	
	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 1; 
	nxthpt.dvid = rtl865xc_netIfIndex(WAN1_VID);
	nxthpt.extIntIpIdx = 0;
	nxthpt.isPppoe = 1;
	rtl8651_setAsicNextHopTable(31, &nxthpt);       	
	/* config ip table */
	bzero((void*) &ipt, sizeof(ipt));
	/* wan 0 napt */
	strtoip(&ipt.intIpAddr, "0.0.0.0");
	strtoip(&ipt.extIpAddr, WAN0_ISP_PUBLIC_IP);
	rtl8651_setAsicExtIntIpTable(0, &ipt);
	return 0;
	
}


int32 testPktRemarking(uint32 caseNo)
{
	int i,retval,len,j;
	enum PORT_MASK toPort;	
	enum PORT_MASK rxPort;	
	int32 pktLen,pktExpectLen;
	uint8 ttl;
   	rtl8651_PktConf_t conf;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	int8  *pktExpect,*pktBuf,*pktData;
	uint8 *memberP;


	pktExpect = sharedPkt[0];
	pktBuf = sharedPkt[1];
	pktData =  sharedPkt[2];
	for (i=1;i<1400;i++) pktData[i]=i;

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{

		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		RoutingConfig(0,0);
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		memberP = memberPortList[0];		
		
		WRITE_MEM32(RMCR1P,0xffffffff);
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));		
		bzero(&vlan, sizeof(vlan));
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1;
		rtl8651_setAsicVlan(1024,&vlan);
		for(j=0;j<=1  ;j++)
		{
			WRITE_MEM32(DSCPRM0,0xffffffff);
			WRITE_MEM32(DSCPRM1,0xffffffff);
			WRITE_MEM32(RLRC,0xFFFFFFFF);
	
			for (len=1;len<1;len++)
			{
#if VERBOSE		
				rtlglue_printf("len %d\n",len);
#endif			
				RESET_CONF(&conf);    
				conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
				conf.ip.id							= rtlglue_random();
				conf.ip.ttl							= 254;
				conf.conf_sport					= 100;
				conf.conf_dport					= 0;
				conf.payload.length				= len;
				conf.ip.headerLen					= 28;
				conf.payload.content				= pktData;
				conf.pppoe.type 					= 0x8864;
				conf.pppoe.session 				= 0;
				conf.vlan.vid						=1024;
				conf.vlan.prio						= 1;
				conf.l2Flag						=L2_PPPoE||L2_VLAN;;
				strtoip(&conf.conf_sip, HOST1_IP);
				strtoip(&conf.conf_dip,"240.240.240.240");
				stringToConfMac(conf.conf_dmac,"00-00-00-00-88-99");
				stringToConfMac(conf.conf_smac,HOST1_MAC);
				pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */		
				toPort = memberP[0];
				virtualMacInput(toPort,pktBuf, pktLen+4);
				rxPort = 1<<memberP[1];
				memset(pktBuf,0,pktLen);
				pktLen = _PKT_LEN;
				retval=virtualMacOutput(&rxPort,pktBuf, &pktLen);
				ASSERT(retval==SUCCESS);
			

			/* Expect Packet */
			conf.ip.tos						= 0xfc;
			conf.vlan.prio						= 7;
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

		}
		ttl = 254;
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_IP;					/* udp packet */
		conf.ip.id							= rtlglue_random(); 
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.ip.protocol					= 0x2F; /*GRE*/
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 12;
		conf.payload.content				= dstData; 
		conf.pppoe.type 					= 0x8864;
		conf.pppoe.session 				= 123;
		conf.vlan.prio					= 1;
		strtoip(&conf.conf_sip, "192.168.1.2");
		strtoip(&conf.conf_dip,"140.0.0.3");
		stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf,pktBuf);				/* generate pkt in buff */
		toPort = memberP[3];
		virtualMacInput(toPort,pktBuf, pktLen+4);		
		rxPort = 1<<memberP[5];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT(retval==SUCCESS);
		stringToConfMac(conf.conf_smac,GW_WAN0_MAC);
		stringToConfMac(conf.conf_dmac,WAN0_ISP_MAC);
		conf.ip.ttl						= ttl-1;
		conf.ip.tos						= 0xfc;
		conf.vlan.prio						= 7;
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

		WRITE_MEM32(RLRC,RMLC_DSCP_L4|RMLC_DSCP_L3||RMLC_8021P_L4|RMLC_8021P_L3);
		ttl = 254;
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0x99; 
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 12;
		conf.payload.content				= dstData; 
		conf.pppoe.type 					= 0x8864;
		conf.pppoe.session 				= 123;
		conf.vlan.prio					= 1;
		strtoip(&conf.conf_sip, "192.168.1.2");
		strtoip(&conf.conf_dip,"140.0.0.3");
		stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf,pktBuf);				/* generate pkt in buff */
		toPort = memberP[3];
		virtualMacInput(toPort,pktBuf, pktLen+4);		
		rxPort = 1<<memberP[5];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT(retval==SUCCESS);
		stringToConfMac(conf.conf_smac,GW_WAN0_MAC);
		stringToConfMac(conf.conf_dmac,WAN0_ISP_MAC);
		conf.ip.ttl						= ttl-1;
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			


		}
	}
	return SUCCESS;


}


/*
 *  In this case, we will randomize the following condition:
 *    1. RMCR1P for each port enable/disable
 *    2. RMCR1P.newPriority for every system parsed priority
 *    3. DSCPCR for each port enable/disable
 *    4. DSCPRCR.newDSCPvalue for system parsed priority
 */
int32 testPktRemarkingRandom(uint32 caseNo)
{
	int i,retval;
	enum PORT_NUM inPort, port;	
	enum PORT_MASK outPortMask[2];
	int32 pktLen[2];
	int32 isPktOut[2]; /* to store the output result */
   	rtl8651_PktConf_t conf;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	int32 loop;

	/* Since we need to run random test, the both mode shoul be support. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE )
		{
			rtlglue_printf( "\n[%s] mode is not supported, ignore this case.\n\n", i==IC_TYPE_REAL?"REAL":"MODEL" );
			return SUCCESS; /* Not support, we don't compare. */	
		}

		retval = virtualMacInit();
		ASSERT( retval==SUCCESS );
	}
	
	for( loop = 0; loop < RANDOM_RUN_NUM; loop++ )
	{
		uint32 rmcr1p;
		uint32 dscprm0, dscprm1;
		uint32 rlrc;
		uint32 prio;
		uint32 orgTos; /* random original ToS value to test if IC remarks ip.tos[7:2], instead of ECN field. */
		uint32 opLayer; /* 0-L2 Switching, 1-L3 Switching, 2-L4 Switching */

		rmcr1p = rtlglue_random();
		dscprm0 = rtlglue_random();
		dscprm1 = rtlglue_random();
		rlrc = rtlglue_random();
		prio = rtlglue_random()&0x7;
		orgTos = rtlglue_random()&0xff;
		opLayer = rtlglue_random()%3;

		/* generate random pcket */
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0x99; 
		conf.ip.ttl							= 64;
		conf.ip.tos							= orgTos;
		conf.conf_dport						= EXTERNAL_PORT;
		conf.conf_sport						= EXTERNAL_PORT+1;
		conf.payload.length					= 12;
		conf.payload.content				= dstData; 
		conf.pppoe.type 					= 0x8864;
		conf.pppoe.session 					= 123;
		conf.l2Flag							= L2_VLAN;
		conf.vlan.prio						= prio;
		switch( opLayer )
		{
			case 0: /* L2 Switching */
				inPort = rtlglue_random()%3; /* Physical port 0~2 in LAN1 */
				conf.vlan.vid						= 0; /* no VID specified */
				strtoip(&conf.conf_sip, "192.168.1.2");
				strtoip(&conf.conf_dip,"140.0.0.3");
				stringToConfMac(conf.conf_dmac,"00-00-10-11-12-20");
				stringToConfMac(conf.conf_smac,"00-00-10-11-12-30");
				break;
			case 1: /* L3 Switching - Routing */
				inPort = 3;
				conf.vlan.vid						= 0x3c7; /* LAN0 VID for NPI */
				strtoip(&conf.conf_sip, HOST3_IP);
				strtoip(&conf.conf_dip, HOST1_IP);
				stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
				stringToConfMac(conf.conf_smac,HOST1_MAC);
				break;
			case 2: /* L4 Switching - NAT */
			default:
				inPort = 1;
				conf.vlan.vid						= 11; /* LAN1 VID for NI */
				strtoip(&conf.conf_sip, HOST1_IP);
				strtoip(&conf.conf_dip,"140.0.0.3");
				stringToConfMac(conf.conf_dmac,GW_LAN1_MAC);
				stringToConfMac(conf.conf_smac,HOST1_MAC);
				break;
		}
		pktLen[0] = pktLen[1] = pktGen( &conf, sharedPkt[0] );				/* generate pkt in buff */
		memcpy( sharedPkt[1], sharedPkt[0], pktLen[0] ); /* copy to pkt1 */

		rtlglue_printf( "[loop=%d]-------------------------------\n", loop );
#if VERBOSE
		rtlglue_printf( "Input Port: %d opLayer=%s prio=%d  orgTos=0x%08x\n", inPort, opLayer==0?"L2":opLayer==1?"L3":"L4", prio, orgTos );
		rtlglue_printf( "rmcr1p=0x%08x  rlrc=0x%08x\n", rmcr1p, rlrc );
		rtlglue_printf( "dscprm0=0x%08x dscprm1=0x%08x\n", dscprm0, dscprm1 );
#endif

		isPktOut[0] = isPktOut[1] = FALSE;
		for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
		{
			retval = model_setTestTarget( i );
			ASSERT( retval == SUCCESS );

			retval = virtualMacInit();
			ASSERT(retval==SUCCESS);
			retval = layer4Config();
			ASSERT(retval==SUCCESS);
			retval = configNicInit();
			ASSERT(retval==SUCCESS);
			
			WRITE_MEM32(RMCR1P,rmcr1p);
			WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));		
			bzero(&vlan, sizeof(vlan));
			vlan.memberPortMask = PM_PORT_0|PM_PORT_1;
			rtl8651_setAsicVlan(1024,&vlan);

			WRITE_MEM32(DSCPRM0,dscprm0);
			WRITE_MEM32(DSCPRM1,dscprm1);
			WRITE_MEM32(RLRC,rlrc);
		
			WRITE_MEM32(LPTM8021Q, 0x00000000 ); /* default value */
			WRITE_MEM32(PBPCR, 0x00000000 ); /* default value */
			WRITE_MEM32(DSCPCR0, 0x00000000 ); /* default value */
			WRITE_MEM32(DSCPCR1, 0x00000000 ); /* default value */
			WRITE_MEM32(DSCPCR2, 0x00000000 ); /* default value */
			WRITE_MEM32(DSCPCR3, 0x00000000 ); /* default value */
			WRITE_MEM32(DSCPCR4, 0x00000000 ); /* default value */
			WRITE_MEM32(DSCPCR5, 0x00000000 ); /* default value */
			WRITE_MEM32(DSCPCR6, 0x00000000 ); /* default value */
			WRITE_MEM32(QIDDPCR, (1<<PBP_PRI_OFFSET)|(1<<BP8021Q_PRI_OFFSET)|(1<<DSCP_PRI_OFFSET)|(1<<ACL_PRI_OFFSET)|(1<<NAPT_PRI_OFFSET) ); /* default value */
			WRITE_MEM32(UPTCMCR0, 0x00000000 ); /* default value */
			WRITE_MEM32(UPTCMCR1, 0x00b6d000 ); /* default value */
			WRITE_MEM32(UPTCMCR2, 0x00b49000 ); /* default value */
			WRITE_MEM32(UPTCMCR3, 0x00b52201 ); /* default value */
			WRITE_MEM32(UPTCMCR4, 0x00b5a201 ); /* default value */
			WRITE_MEM32(UPTCMCR5, 0x00b63401 ); /* default value */
			WRITE_MEM32(QNUMCR, P0QNum_1|P1QNum_1|P2QNum_1|P3QNum_1|P4QNum_1|P5QNum_1);

			retval = virtualMacInput( inPort, sharedPkt[i], pktLen[i]+4 );		
			ASSERT( retval==SUCCESS );
		}

		/* Since packet may be forwaorded to multiple ports, we need to poll every port */
		for( port = PN_PORT0; port <= PN_PORT5; port++ )
		{
			for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
			{
				enum PORT_MASK opm;
				
				retval = model_setTestTarget( i );
				ASSERT( retval == SUCCESS );
				
				pktLen[i] = _PKT_LEN;
				opm = 1<<port;
				retval = virtualMacOutput( &opm, sharedPkt[i], &pktLen[i] );
				if ( retval==SUCCESS )
					isPktOut[i] = TRUE;
				else
					isPktOut[i] = FALSE;
			}
			
			IS_EQUAL_INT( "Output result is not the same.", isPktOut[0], isPktOut[1], __FUNCTION__, __LINE__ );
			PKT_CMP( sharedPkt[0], sharedPkt[1], pktLen[0], pktLen[1], __FUNCTION__, __LINE__, i );
			IS_EQUAL_INT( "Output pktLen is not the same.", pktLen[0], pktLen[1], __FUNCTION__, __LINE__ );
		}
		

		/* polling all remained packets */
		for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
		{
			retval = model_setTestTarget( i );
			ASSERT( retval == SUCCESS );

			while (1)
			{
				outPortMask[i] = PM_PORT_ALL;
				pktLen[i] = _PKT_LEN;
				retval=virtualMacOutput( &outPortMask[i], sharedPkt[i], &pktLen[i] );
				if ( retval!=SUCCESS ) break;
			}
		}
	}
	return SUCCESS;
}



int32 testPktProtocolBase(uint32 caseNo)
{
	int i,retval,len;
	enum PORT_MASK toPort;	
	enum PORT_MASK rxPort;	
	int32 pktLen,pktExpectLen;
   	rtl8651_PktConf_t conf;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	int8  *pktExpect,*pktBuf,*pktData;
	uint8 *memberP;
	pktExpect = sharedPkt[0];
	pktBuf = sharedPkt[1];
	pktData =  sharedPkt[2];
	ASSERT(pktData);
	for (i=1;i<1400;i++) pktData[i]=i;

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{

		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		RoutingConfig(0,0);
		memberP = memberPortList[0];		
		WRITE_MEM32(SWTCR0,READ_MEM32(SWTCR0) | LIMDBC_MAC);
		WRITE_MEM32(PLITIMR,6<<9);
		rtl8651_setProtocolBasedVLAN(RTL8651_PBV_RULE_PPPOE_CONTROL, PN_PORT0, TRUE, 224 );	
		rtl8651_setProtocolBasedVLAN(RTL8651_PBV_RULE_PPPOE_SESSION, PN_PORT0, TRUE, 1024 );	
		for (len=1;len<0x23;len++)
		{		
			bzero(&vlan, sizeof(vlan));
			vlan.memberPortMask = PM_PORT_0|PM_PORT_1;
			rtl8651_setAsicVlan(1024,&vlan);
			/**********************************************************************
			LAN to LAN
			Include PPPoE Session Header
			Expect : ADD VLAN TAG 
			**********************************************************************/					
			RESET_CONF(&conf);    
			conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
			conf.ip.id							= 0x100;
			conf.ip.ttl							= 254;
			conf.conf_sport					= 100;
			conf.conf_dport					= 0;
			conf.payload.length				= len;
			conf.payload.content				= pktData;
			conf.pppoe.type 					= PPPOESESSION_ETHERTYPE;
			conf.pppoe.session 				= 0;
			conf.l2Flag						=L2_PPPoE;
			strtoip(&conf.conf_sip, HOST1_IP);
			strtoip(&conf.conf_dip,"240.240.240.240");
			stringToConfMac(conf.conf_dmac,"00-00-00-00-88-99");
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */		
			toPort = memberP[0];
			virtualMacInput(toPort,pktBuf, pktLen+4);
			rxPort = 1<<memberP[1];
			memset(pktBuf,0,pktLen);
			pktLen = _PKT_LEN;
			retval=virtualMacOutput(&rxPort,pktBuf, &pktLen);
			ASSERT(retval==SUCCESS);
			/* Expect Packet */
			conf.vlan.vid						=1024;
			conf.l2Flag						|= L2_VLAN;
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
			/**********************************************************************
			LAN to LAN
			Include PPPoE Session Header
			Expect : Forward
			**********************************************************************/			
			bzero(&vlan, sizeof(vlan));
			vlan.memberPortMask = vlan.untagPortMask=PM_PORT_0|PM_PORT_1;
			rtl8651_setAsicVlan(1024,&vlan);
			RESET_CONF(&conf);    
			conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
			conf.ip.id							= 0x101;
			conf.ip.ttl							= 254;
			conf.conf_sport					= 100;
			conf.conf_dport					= 0;
			conf.payload.length				= len;
			conf.payload.content				= pktData;
			conf.pppoe.type 					= PPPOESESSION_ETHERTYPE;
			conf.pppoe.session 				= len^len;
			conf.l2Flag						= L2_PPPoE;
			strtoip(&conf.conf_sip, HOST1_IP);
			strtoip(&conf.conf_dip,"240.240.240.240");
			stringToConfMac(conf.conf_dmac,"00-00-00-00-88-99");
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */		
			toPort = memberP[0];
			virtualMacInput(toPort,pktBuf, pktLen+4);
			rxPort = 1<<memberP[1];
			memset(pktBuf,0,pktLen);
			pktLen = _PKT_LEN;
			retval=virtualMacOutput(&rxPort,pktBuf, &pktLen);
			ASSERT(retval==SUCCESS);
			/* Expect Packet */
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);	
			/**********************************************************************
			LAN to LAN
			Include PPPoE Control Header
			Expect : ADD VLAN TAG 
			**********************************************************************/			
			bzero(&vlan, sizeof(vlan));
			vlan.memberPortMask = PM_PORT_0|PM_PORT_1;
			rtl8651_setAsicVlan(224,&vlan);
			RESET_CONF(&conf);    
			conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
			conf.ip.id							= 0x210;
			conf.ip.ttl							= 254;
			conf.conf_sport					= 100;
			conf.conf_dport					= rtlglue_random()&0xffff;
			conf.payload.length				= len;
			conf.payload.content				= pktData;
			conf.pppoe.type					= PPPOECONTROL_ETHERTYPE;
			conf.l2Flag						= L2_PPPoE;
			strtoip(&conf.conf_sip, HOST1_IP);
			strtoip(&conf.conf_dip,"240.240.240.240");
			stringToConfMac(conf.conf_dmac,"00-00-00-00-88-99");
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */		
			toPort = memberP[0];
			virtualMacInput(toPort,pktBuf, pktLen+4);
			rxPort = 1<<memberP[1];
			memset(pktBuf,0,pktLen);
			pktLen = _PKT_LEN;
			retval=virtualMacOutput(&rxPort,pktBuf, &pktLen);
			ASSERT(retval==SUCCESS);
			/* Expect Packet */
			conf.vlan.vid						=224;
			conf.l2Flag						|= L2_VLAN;
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
		}

	}
	return SUCCESS;
}

int32 testPktNapt(uint32 caseNo)
{
	int i,retval;
	enum PORT_MASK toPort;	
	enum PORT_MASK rxPort;	
	int32 pktLen,pktExpectLen;
   	rtl8651_PktConf_t conf;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	int8  *pktExpect,*pktBuf,*pktData;
	uint8 *memberP;

	pktExpect = sharedPkt[0];
	pktBuf = sharedPkt[1];
	pktData =  sharedPkt[2];
	for (i=1;i<512;i++) pktData[i]=i;

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{

		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		NaptConfig(0,0,0,ALGO_SOURCE_BASE);
		memberP = memberPortList[0];		

		strtomac(&intf.macAddr, GW_WAN0_MAC);
		intf.macAddrNumber = 1;
		intf.vid = WAN0_VID;
		intf.mtu = 1500;
		intf.inAclStart = 5;
		intf.inAclEnd = 5;
		intf.outAclStart = intf.outAclEnd = 5;
		intf.enableRoute = 1;
		intf.valid = 1;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );		
		WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
		WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
		WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
		WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
		WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
		WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
		WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
		WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
		WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
		bzero( &vlant, sizeof(vlant) );
		vlant.memberPortMask = vlant.untagPortMask =PM_PORT_5;
		rtl8651_setAsicVlan(intf.vid,&vlant);


		toPort = PN_PORT3;
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xa;
		conf.ip.ttl							= 254;
		conf.conf_sport					= 100;
		conf.conf_dport					= 200;
		conf.payload.length				= 15;
		conf.vlan.vid						=LAN0_VID;
		conf.pppoe.type = 0x8864;
		conf.pppoe.session = 0;
		conf.l2Flag						=  L2_PPPoE;
		strtoip(&conf.conf_sip, HOST1_IP);
		strtoip(&conf.conf_dip,"240.240.240.240");
		stringToConfMac(conf.conf_dmac,"00-00-00-00-88-8A");
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
		virtualMacInput(toPort,pktBuf, pktLen+4);		
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput(&rxPort,pktBuf, &pktLen);		
		ASSERT(retval==SUCCESS);
		
		/* Expect Packet */
		conf.l2Flag						|=  L2_VLAN;

		strtoip(&conf.conf_sip, HOST1_IP);
		strtoip(&conf.conf_dip,"240.240.240.240");
		stringToConfMac(conf.conf_dmac,"00-00-00-00-99-9C");
		stringToConfMac(conf.conf_smac,"00-00-00-00-88-88");

		
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */



	}
	return SUCCESS;
}

int32 testPktPPPoE(uint32 caseNo)
{
	int i,j,retval;
	enum PORT_MASK rxPort;	
	enum PORT_MASK toPort;	
	int32 pktLen,pktExpectLen;
   	rtl8651_PktConf_t conf;
	rtl865x_tblAsicDrv_serverPortParam_t	 srvt;
	int8 *pktExpect,*pktBuf,*pktData;
	uint8 ttl;
	uint16 len,headerLen;
	hsa_param_t hsa;
	int8 *memberP;
	

	pktExpect = sharedPkt[0];
	pktBuf = sharedPkt[1];
	pktData =  sharedPkt[2];
	for (j=1;j<512;j++) pktData[j]=j;	
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{

		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		NaptConfig(0,1500,1,ALGO_SOURCE_BASE);
		memberP = memberPortList[0];		
		ttl=254;	
		WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1));
		/* config server port table */
		bzero((void*) &srvt, sizeof(srvt));
		strtoip(&srvt.intIpAddr, HOST2_IP);
		strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		srvt.extPort = EXTERNAL_PORT;
		srvt.intPort = INTERNAL_PORT;
		srvt.valid = 1;
		rtl8651_setAsicServerPortTable(0, &srvt);

		strtoip(&srvt.intIpAddr, HOST2_IP);
		strtoip(&srvt.extIpAddr, WAN1_ISP_PUBLIC_IP);
		srvt.extPort = UPPER_PORT;
		srvt.intPort = LOWER_PORT;
		srvt.portRange=1;
		srvt.valid = 1;
		rtl8651_setAsicServerPortTable(15, &srvt);
#if defined(CONFIG_RTL865X_MODEL_TEST_FT2)
		for (len=9;len<10;len++)
#else
		for (len=5;len<10;len++)
#endif
		{

			WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)&~EN_RTL8650B);
			/**********************************************************************
			WAN to LAN 
			Include PPPoE Header
			**********************************************************************/	
			RESET_CONF(&conf);    
			conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
			conf.ip.id							= 0xAA;
			conf.ip.ttl							= ttl;
			conf.conf_dport					= EXTERNAL_PORT;
			conf.pppoe.type 					= 0x8864;
			conf.pppoe.session 				= 0x55;
			conf.l2Flag						= L2_PPPoE;
			conf.conf_sport					= EXTERNAL_PORT+1;
			conf.payload.length				= len;
			conf.payload.content				= pktData;
			strtoip(&conf.conf_sip, "240.240.240.240");
			strtoip(&conf.conf_dip,"192.168.4.1");
			stringToConfMac(conf.conf_dmac,GW_WAN0_MAC);
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
			toPort =memberP[5];
			virtualMacInput(toPort,pktBuf, pktLen+4);		
			virtualMacGetHsa(&hsa);
			rxPort  =1<<memberP[2];
			memset(pktBuf,0,pktLen);
			pktLen = _PKT_LEN;
			retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
			ASSERT(retval==SUCCESS);
			conf.l2Flag						= 0;
			conf.ip.ttl							= ttl-1;
			stringToConfMac(conf.conf_smac,GW_LAN1_MAC);
			stringToConfMac(conf.conf_dmac,HOST2_MAC);
			strtoip(&conf.conf_dip,HOST2_IP);
			conf.conf_dport					= INTERNAL_PORT;
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);				
			/**********************************************************************
			WAN to WAN 
			Include PPPoE Header
			**********************************************************************/
			RESET_CONF(&conf);    
			conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
			conf.ip.id							= 0xBB;
			conf.ip.ttl							= ttl;
			conf.conf_dport					= EXTERNAL_PORT;
			conf.pppoe.type 					= 0x8864;
			conf.pppoe.session					= 0x56;
			conf.l2Flag						= L2_PPPoE;
			conf.conf_sport					= EXTERNAL_PORT+1;
			conf.payload.length				= len;
			conf.payload.content				= pktData;
			strtoip(&conf.conf_sip, "240.240.240.240");
			strtoip(&conf.conf_dip,"192.168.4.4");
			WRITE_MEM32(ALECR,READ_MEM32(ALECR)|(uint32)EN_TTL1);	
			stringToConfMac(conf.conf_dmac,GW_WAN0_MAC);
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
			toPort =memberP[5];
			virtualMacInput(toPort,pktBuf, pktLen+4);		
			virtualMacGetHsa(&hsa);
			rxPort  =1<<memberP[4];
			memset(pktBuf,0,pktLen);
			pktLen = _PKT_LEN;
			retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
			ASSERT(retval==SUCCESS);
			conf.ip.ttl							= ttl-1;
			conf.pppoe.session 				= 0x55;
			stringToConfMac(conf.conf_smac,GW_WAN1_MAC);
			stringToConfMac(conf.conf_dmac,HOST4_MAC);
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */					
			headerLen=*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET];
			if (ntohs(headerLen)<0x28)
			{
				*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET]=htons(0x28);
			}
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);				
			/**********************************************************************
			WAN to WAN 
			Include PPPoE Header and VLAN TAG
			Expect : Remove VLAN TAG
			**********************************************************************/
			RESET_CONF(&conf);    
			conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
			conf.ip.id							= 0xBB;
			conf.ip.ttl							= ttl;
			conf.conf_dport					= EXTERNAL_PORT;
			conf.pppoe.type 					= 0x8864;
			conf.pppoe.session					= 0x56;
			conf.l2Flag						= L2_PPPoE|L2_VLAN;
			conf.vlan.vid						= WAN0_VID;
			conf.conf_sport					= EXTERNAL_PORT+1;
			conf.payload.length				= len;
			conf.payload.content				= pktData;
			strtoip(&conf.conf_sip, "240.240.240.240");
			strtoip(&conf.conf_dip,"192.168.4.4");
			WRITE_MEM32(ALECR,READ_MEM32(ALECR)|(uint32)EN_TTL1);	
			stringToConfMac(conf.conf_dmac,GW_WAN0_MAC);
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
			toPort =memberP[5];
			virtualMacInput(toPort,pktBuf, pktLen+4);		
			virtualMacGetHsa(&hsa);
			rxPort  =1<<memberP[4];
			memset(pktBuf,0,pktLen);
			pktLen = _PKT_LEN;
			retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
			ASSERT(retval==SUCCESS);
			conf.ip.ttl							= ttl-1;
			conf.pppoe.session 				= 0x55;
			conf.l2Flag						= L2_PPPoE;
			stringToConfMac(conf.conf_smac,GW_WAN1_MAC);
			stringToConfMac(conf.conf_dmac,HOST4_MAC);
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */					
			headerLen=*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET];
			if (ntohs(headerLen)<0x24)
			{
				if (pktExpectLen>60)
					*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET]=htons(0x24+pktExpectLen-60);					
				else
					*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET]=htons(0x24);
			}

			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);				
			/**********************************************************************
			LAN to WAN 
			Include PPPoE Header
			**********************************************************************/
			RESET_CONF(&conf);    
			conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
			conf.ip.id							= 0xCC;
			conf.ip.ttl							= ttl;
			conf.conf_dport					= EXTERNAL_PORT;
			conf.pppoe.type					= 0x8864;
			conf.pppoe.session 				= 0x54;
			conf.l2Flag						= L2_PPPoE;
			conf.conf_sport					= INTERNAL_PORT; 
			conf.payload.length				= len;
			conf.payload.content				= pktData;
			strtoip(&conf.conf_dip, "240.240.240.240");
			strtoip(&conf.conf_sip,"192.168.1.3");
			stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
			toPort = memberP[3];
			virtualMacInput(toPort,pktBuf, pktLen+4);		
			virtualMacGetHsa(&hsa);
			IS_EQUAL_INT_DETAIL("HSA  should be the same....",hsa.dp,PM_PORT_4, __FUNCTION__,__LINE__,i);
			rxPort  = 1<< memberP[4];
			memset(pktBuf,0,pktLen);
			pktLen = _PKT_LEN;
			retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
			ASSERT(retval==SUCCESS);

			stringToConfMac(conf.conf_smac,GW_WAN1_MAC);
			stringToConfMac(conf.conf_dmac,HOST4_MAC);
			strtoip(&conf.conf_sip,"192.168.4.1");
			conf.pppoe.session 				= WAN1_ISP_PPPOE_SID;
			conf.conf_sport					= EXTERNAL_PORT; 
			conf.ip.ttl							= ttl-1;	
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */
			headerLen=*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET];
			if (ntohs(headerLen)<0x28)
			{
				*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET]=htons(0x28);
			}
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);	
			/**********************************************************************
			LAN to WAN 
			INCLUDE VLAN TAG
			**********************************************************************/
			RESET_CONF(&conf);    
			conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
			conf.ip.id							= 0xDD;
			conf.conf_sport					= INTERNAL_PORT; 
			conf.ip.ttl							= ttl;
			conf.conf_dport					= EXTERNAL_PORT;
			conf.payload.length				= len;
			conf.payload.content				= pktData;
			conf.vlan.vid						= LAN0_VID;			
			conf.l2Flag						= L2_VLAN;
			strtoip(&conf.conf_dip, "240.240.240.240");
			strtoip(&conf.conf_sip,"192.168.1.3");
			stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
			toPort = memberP[3];
			virtualMacInput(toPort,pktBuf, pktLen+4);		
			virtualMacGetHsa(&hsa);
			IS_EQUAL_INT_DETAIL("HSA  should be the same....",hsa.dp,PM_PORT_4, __FUNCTION__,__LINE__,i);
			rxPort  = 1<< memberP[4];
			memset(pktBuf,0,pktLen);
			pktLen = _PKT_LEN;
			retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
			ASSERT(retval==SUCCESS);

			stringToConfMac(conf.conf_smac,GW_WAN1_MAC);
			stringToConfMac(conf.conf_dmac,HOST4_MAC);
			conf.pppoe.session 				= WAN1_ISP_PPPOE_SID;
			conf.ip.ttl							= ttl-1;
			conf.pppoe.type					= PPPOESESSION_ETHERTYPE;;
			conf.pppoe.session 				= 0x56;
			conf.l2Flag						= L2_PPPoE;
			conf.conf_sport					= EXTERNAL_PORT; 
			strtoip(&conf.conf_sip,"192.168.4.1");
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */
			headerLen=*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET];
			if (ntohs(headerLen)<44)  /*the smallest  size is 60-16 (DA/SA/VLANTAG)=44*/
			{
				if (pktExpectLen>60)
					*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET]=htons(44+pktExpectLen-60);
				else
					*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET]=htons(44);
			}
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);					
			/**********************************************************************
			LAN to WAN 
			**********************************************************************/
			RESET_CONF(&conf);    
			conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
			conf.ip.id							= 0xDD;
			conf.conf_sport					= INTERNAL_PORT; 
			conf.ip.ttl							= ttl;
			conf.conf_dport					= EXTERNAL_PORT;
			conf.pppoe.type					= 0x8864;
			conf.pppoe.session					= 0x54;
			conf.payload.length				= len;
			conf.payload.content				= pktData;
			strtoip(&conf.conf_dip, "240.240.240.240");
			strtoip(&conf.conf_sip,"192.168.1.3");
			stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
			toPort = memberP[3];
			virtualMacInput(toPort,pktBuf, pktLen+4);		
			virtualMacGetHsa(&hsa);
			IS_EQUAL_INT_DETAIL("HSA  should be the same....",hsa.dp,PM_PORT_4, __FUNCTION__,__LINE__,i);
			rxPort  = 1<< memberP[4];
			memset(pktBuf,0,pktLen);
			pktLen = _PKT_LEN;
			retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
			ASSERT(retval==SUCCESS);

			stringToConfMac(conf.conf_smac,GW_WAN1_MAC);
			stringToConfMac(conf.conf_dmac,HOST4_MAC);
			conf.pppoe.session 				= WAN1_ISP_PPPOE_SID;
			conf.ip.ttl							= ttl-1;
			conf.l2Flag						= L2_PPPoE;
			conf.conf_sport					= EXTERNAL_PORT; 
			strtoip(&conf.conf_sip,"192.168.4.1");
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */
			headerLen=*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET];
			if (ntohs(headerLen)<48)  /*the smallest  size is 60-12 (DA/SA)=48*/
			{
				*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET]=htons(48);
			}
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);	
			WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_RTL8650B);
		}

		NaptConfig(1,1500,1,ALGO_SOURCE_BASE);
		WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1));
		/* config server port table */
		bzero((void*) &srvt, sizeof(srvt));
		strtoip(&srvt.intIpAddr, HOST2_IP);
		strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		srvt.extPort = EXTERNAL_PORT;
		srvt.intPort = INTERNAL_PORT;
		srvt.valid = 1;
		rtl8651_setAsicServerPortTable(0, &srvt);

		strtoip(&srvt.intIpAddr, HOST2_IP);
		strtoip(&srvt.extIpAddr, WAN1_ISP_PUBLIC_IP);
		srvt.extPort = UPPER_PORT;
		srvt.intPort = LOWER_PORT;
		srvt.portRange=1;
		srvt.valid = 1;
		rtl8651_setAsicServerPortTable(15, &srvt);
		memberP = memberPortList[1];		
#if defined(CONFIG_RTL865X_MODEL_TEST_FT2)
		for (len=14;len<15;len++)
#else
		for (len=1;len<15;len++)
#endif
		{
			/**********************************************************************
			WAN to WAN 
			Include PPPoE Header
			EXPECT : VLAN TAG+PPPoE Header
			**********************************************************************/
			RESET_CONF(&conf);    
			conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
			conf.ip.id							= 0xEE;
			conf.ip.ttl							= ttl;
			conf.conf_dport					= EXTERNAL_PORT;
			conf.pppoe.type 					= PPPOESESSION_ETHERTYPE;
			conf.pppoe.session					= 0x56;
			conf.l2Flag						= L2_PPPoE;
			conf.conf_sport					= EXTERNAL_PORT+1;
			conf.payload.length				= len;
			conf.payload.content				= pktData;
			strtoip(&conf.conf_sip, "240.240.240.240");
			strtoip(&conf.conf_dip,"192.168.4.4");
			WRITE_MEM32(ALECR,READ_MEM32(ALECR)|(uint32)EN_TTL1);	
			stringToConfMac(conf.conf_dmac,GW_WAN0_MAC);
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
			toPort =memberP[5];
			virtualMacInput(toPort,pktBuf, pktLen+4);		
			virtualMacGetHsa(&hsa);
			rxPort  =1<<memberP[4];
			memset(pktBuf,0,pktLen);
			pktLen = _PKT_LEN;
			retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
			ASSERT(retval==SUCCESS);
			conf.ip.ttl							= ttl-1;
			conf.vlan.vid						= WAN1_VID;			
			conf.pppoe.session 				= WAN0_ISP_PPPOE_SID;
			conf.l2Flag						= L2_PPPoE|L2_VLAN;
			stringToConfMac(conf.conf_smac,GW_WAN1_MAC);
			stringToConfMac(conf.conf_dmac,HOST4_MAC);
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */					
			headerLen=*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET_INCLUDE_VLANTAG];
			if (ntohs(headerLen)<0x28)
			{
				*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET_INCLUDE_VLANTAG]=htons(0x28);
			}
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);						
			/**********************************************************************
			LAN to WAN 
			INCLUDE VLAN TAG
			**********************************************************************/
			RESET_CONF(&conf);    
			conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
			conf.ip.id							= 0xFF;
			conf.conf_sport					= INTERNAL_PORT; 
			conf.ip.ttl							= ttl;
			conf.conf_dport					= EXTERNAL_PORT;
			conf.vlan.vid						= LAN0_VID;			
			conf.payload.length				= len;
			conf.payload.content				= pktData;
			conf.l2Flag						= L2_VLAN;
			strtoip(&conf.conf_dip, "240.240.240.240");
			strtoip(&conf.conf_sip,"192.168.1.3");
			stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
			toPort = memberP[3];
			virtualMacInput(toPort,pktBuf, pktLen+4);		
			virtualMacGetHsa(&hsa);
			IS_EQUAL_INT_DETAIL("HSA  should be the same....",hsa.dp,PM_PORT_4, __FUNCTION__,__LINE__,i);
			rxPort  = 1<< memberP[4];
			memset(pktBuf,0,pktLen);
			pktLen = _PKT_LEN;
			retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);				
			ASSERT(retval==SUCCESS);
			stringToConfMac(conf.conf_smac,GW_WAN1_MAC);
			stringToConfMac(conf.conf_dmac,HOST4_MAC);
			conf.ip.ttl							= ttl-1;
			conf.pppoe.type					= PPPOESESSION_ETHERTYPE;
			conf.pppoe.session 				= WAN1_ISP_PPPOE_SID;
			conf.l2Flag						= L2_PPPoE|L2_VLAN;
			conf.conf_sport					= EXTERNAL_PORT; 
			conf.vlan.vid						= WAN1_VID;			
			strtoip(&conf.conf_sip,"192.168.4.1");
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */
			headerLen=*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET_INCLUDE_VLANTAG];
			if (ntohs(headerLen)<44)  /*the smallest  size is 60-12 (DA/SA)-4(vlan)=44*/
			{
				*(uint16*)&pktExpect[PPPOE_PAYLOADLEN_OFFSET_INCLUDE_VLANTAG]=htons(44);
			}
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);		
		}

	}

	return SUCCESS;
}

int32 testPktChecksumZero(uint32 caseNo)
{
	int32 pktLen,pktExpectLen;
   	rtl8651_PktConf_t conf;
	int i,retval;
	enum PORT_MASK rxPort;	
	enum PORT_MASK toPort;	
	uint8 ttl;
	int8 *pktExpect,*pktBuf,*pktData;
	uint8 *memberP;
	
	pktExpect = sharedPkt[0];
	pktBuf = sharedPkt[1];
	pktData =  sharedPkt[2];
	for (i=1;i<512;i++) pktData[i]=i;

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{

		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		RoutingConfig(0,0);
		memberP = memberPortList[0];
		WRITE_MEM32(CSCR,ALLOW_L2_CHKSUM_ERR);
		ttl=0xFD;
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_TCP;					/* tcp packet */
		conf.ip.id							= 0xa;
		conf.ip.ttl							= ttl;
		conf.conf_sport					= 100;
		conf.conf_dport					= 200;
		strtoip(&conf.conf_sip, HOST1_IP);
		strtoip(&conf.conf_dip,"240.240.240.240");
		stringToConfMac(conf.conf_dmac,"00-00-00-00-88-8A");
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, pktBuf);				/* generate pkt in buff */
		pktBuf[0x32]=0;
		pktBuf[0x33]=0;
		toPort = memberP[0];
		dumpPacket2TextPcap(pktBuf,pktLen);
		virtualMacInput(toPort,pktBuf, pktLen+4);
		rxPort  =1<memberP[4];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
		ASSERT(retval==SUCCESS);
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			


		WRITE_MEM32(CSCR,ALLOW_L2_CHKSUM_ERR);
		ttl=0xFD;
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* tcp packet */
		conf.ip.id							= 0xa;
		conf.ip.ttl							= ttl;
		conf.conf_sport					= 100;
		conf.conf_dport					= 200;
		strtoip(&conf.conf_sip, HOST1_IP);
		strtoip(&conf.conf_dip,"240.240.240.240");
		stringToConfMac(conf.conf_dmac,"00-00-00-00-88-8A");
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
		pktBuf[40]=0;
		pktBuf[41]=0;
		toPort = memberP[3];
		dumpPacket2TextPcap(pktBuf,pktLen);
		retval=virtualMacInput(toPort,pktBuf, pktLen+4);
		ASSERT(retval==SUCCESS);
		rxPort  =1<<memberP[4];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
		ASSERT(retval==SUCCESS);
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

	}
	return SUCCESS;
}


int32 testPktRouting(uint32 caseNo)
{
	int32 pktLen,pktExpectLen;
   	rtl8651_PktConf_t conf;
	int i,retval;
	pktParse_t  result;
	enum PORT_MASK rxPort;	
	enum PORT_MASK toPort;	
	ether_addr_t mac;
	uint8 ttl;
	int8 *pktExpect,*pktBuf,*pktData;
	uint8 *memberP;
	

	pktExpect = sharedPkt[0];
	pktBuf = sharedPkt[1];
	pktData =  sharedPkt[2];
	for (i=1;i<512;i++) pktData[i]=i;

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{

		retval = model_setTestTarget( i );

		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		retval=configNicInit();
		RoutingConfig1(1,0);

		memberP = memberPortList[1];
		ttl=0xFD;
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_TCP;					/* tcp packet */
		conf.ip.id							= 0xa;
		conf.ip.ttl							= ttl;
		conf.conf_sport					= 100;
		conf.conf_dport					= 200;
		conf.conf_tcp_seq					= 0xb;
		conf.conf_tcp_ack					= 0xc;
		conf.conf_tcp_flag					= TH_ACK;
		conf.conf_tcp_win					= 0xfffe;
		strtoip(&conf.conf_dip, "192.168.3.24");
		strtoip(&conf.conf_sip,"240.240.240.240");
		stringToConfMac(conf.conf_dmac,"00-00-00-00-88-8A");
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */

		toPort =memberP[3] ;
		retval=virtualMacInput(toPort,pktBuf, pktLen+4);
		ASSERT(retval==SUCCESS);
		rxPort  = 1<<memberP[5];
		memset(pktBuf,0,pktLen);
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
		ASSERT(retval==SUCCESS);
		
		/* Expect Packet */
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_TCP;		
		conf.ip.id							= 0xa;
		conf.ip.ttl							= ttl-1;
		conf.conf_sport					= 100;
		conf.conf_dport					= 200;
		conf.conf_tcp_seq					= 0xb;
		conf.conf_tcp_ack					= 0xc;
		conf.conf_tcp_flag					= TH_ACK;
		conf.conf_tcp_win					= 0xfffe;

		strtoip(&conf.conf_sip, HOST1_IP);
		strtoip(&conf.conf_dip,"240.240.240.240");
		stringToConfMac(conf.conf_dmac,"00-00-00-00-99-9C");
		stringToConfMac(conf.conf_smac,"00-00-00-00-88-88");
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */

		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
		

		
		RoutingConfig(1,0);
		memberP = memberPortList[1];
		ttl=0xFD;
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_TCP;					/* tcp packet */
		conf.ip.id							= 0xa;
		conf.ip.ttl							= ttl;
		conf.conf_sport					= 100;
		conf.conf_dport					= 200;
		conf.conf_tcp_seq					= 0xb;
		conf.conf_tcp_ack					= 0xc;
		conf.conf_tcp_flag					= TH_ACK;
		conf.conf_tcp_win					= 0xfffe;
		strtoip(&conf.conf_sip, HOST1_IP);
		strtoip(&conf.conf_dip,"240.240.240.240");
		stringToConfMac(conf.conf_dmac,"00-00-00-00-88-8A");
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */

		toPort =memberP[3] ;
		retval=virtualMacInput(toPort,pktBuf, pktLen+4);
		ASSERT(retval==SUCCESS);
		rxPort  = 1<<memberP[5];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
		ASSERT(retval==SUCCESS);
		
		/* Expect Packet */
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_TCP;		
		conf.ip.id							= 0xa;
		conf.ip.ttl							= ttl-1;
		conf.conf_sport					= 100;
		conf.conf_dport					= 200;
		conf.conf_tcp_seq					= 0xb;
		conf.conf_tcp_ack					= 0xc;
		conf.conf_tcp_flag					= TH_ACK;
		conf.conf_tcp_win					= 0xfffe;

		strtoip(&conf.conf_sip, HOST1_IP);
		strtoip(&conf.conf_dip,"240.240.240.240");
		stringToConfMac(conf.conf_dmac,"00-00-00-00-99-9C");
		stringToConfMac(conf.conf_smac,"00-00-00-00-88-88");
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */

		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
		/*********************************************************************		
		PKT TYPE : UDP
		*********************************************************************/
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */

		toPort = memberP[3];
		virtualMacInput(toPort,pktBuf, pktLen+4);
		rxPort  = 1<<memberP[4];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
		ASSERT(retval==SUCCESS);
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

		/*********************************************************************		
		DISABLE TTL
		*********************************************************************/
		WRITE_MEM32(ALECR,READ_MEM32(ALECR)&~(uint32)EN_TTL1);	
		toPort=memberP[3];
		virtualMacInput(toPort,pktBuf, pktLen+4);		
		rxPort  = 1<<memberP[5];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);		
		ASSERT(retval==SUCCESS);
		pktParse(pktBuf, &result,pktLen );
		strtomac(&mac,"00-00-00-00-99-9c");		
		strtomac(&mac,"00-00-00-00-88-88");		
		IS_EQUAL_INT_DETAIL(" TTL should be the same....",result.ip->ip_ttl,ttl, __FUNCTION__,__LINE__,i);



		/* VLAN Unware*/

		WRITE_MEM32( VCR0,READ_MEM32(VCR0)| EN_1QTAGVIDIGNORE);	
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xa;
		conf.ip.ttl							= 254;
		
		conf.conf_sport					= 100;
		conf.conf_dport					= 200;
		conf.payload.length				= 15;

		conf.vlan.vid						=LAN0_VID;
		conf.pppoe.type = 0x8864;
		conf.pppoe.session = 0;
		conf.l2Flag						=L2_VLAN;
		strtoip(&conf.conf_sip, HOST1_IP);
		strtoip(&conf.conf_dip,"240.240.240.240");
		stringToConfMac(conf.conf_dmac,"00-00-00-00-88-8A");
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */

		toPort = memberP[3];
		retval=virtualMacInput(toPort,pktBuf, pktLen+4);
		ASSERT(retval==SUCCESS);
		rxPort = 1<<memberP[5];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT(retval==SUCCESS);


		/* IP Version != 4 */
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xa;
		conf.ip.ttl							= 254;
		conf.ip.version					= 4;		
		conf.ip.headerLen					= 24;
		conf.conf_sport					= 100;
		conf.conf_dport					= 200;
		conf.payload.length				= 15;
		WRITE_MEM32(CSCR,ALLOW_L2_CHKSUM_ERR |ALLOW_L3_CHKSUM_ERR|ALLOW_L4_CHKSUM_ERR);
		strtoip(&conf.conf_sip, HOST1_IP);
		strtoip(&conf.conf_dip,"240.240.240.240");
		stringToConfMac(conf.conf_dmac,"00-00-00-00-88-8A");
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
		toPort = memberP[2];
		virtualMacInput(toPort,pktBuf, pktLen+4);
		rxPort = 1<<memberP[5];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT(retval==SUCCESS);
	
		

	}
	return SUCCESS;
}

int32 testPktServerPort(uint32 caseNo)
{
	int i,j,retval;
	int ipaddr;
	enum PORT_MASK rxPort;	
	enum PORT_MASK toPort;	
	int32 pktLen,pktExpectLen;
   	rtl8651_PktConf_t conf;
	rtl865x_tblAsicDrv_serverPortParam_t	 srvt;
	int8 *pktExpect,*pktBuf,*pktData;
	uint8 ttl;
	rtl865x_tblAsicDrv_extIntIpParam_t ipt;		
	uint16  externalPrt;
	hsa_param_t hsa;
	uint8 *memberP;
		
	pktExpect = sharedPkt[0];
	pktBuf = sharedPkt[1];
	pktData =  sharedPkt[2];
	for (i=1;i<1512;i++) pktData[i]=i;
	
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{

		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		NaptConfig(0,1500,0,ALGO_SOURCE_BASE);
		memberP =  memberPortList[0];
		WRITE_MEM32(CSCR,ALLOW_L2_CHKSUM_ERR |ALLOW_L3_CHKSUM_ERR|ALLOW_L4_CHKSUM_ERR);

		/* config server port table */
		bzero((void*) &srvt, sizeof(srvt));
		strtoip(&srvt.intIpAddr, HOST2_IP);
		strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		srvt.extPort = EXTERNAL_PORT;
		srvt.intPort = INTERNAL_PORT;
		srvt.pvaild=1;
		srvt.pid=2;
		srvt.valid = 1;
		rtl8651_setAsicServerPortTable(0, &srvt);

		strtoip(&srvt.intIpAddr, HOST2_IP);
		strtoip(&srvt.extIpAddr, WAN1_ISP_PUBLIC_IP);
		srvt.extPort = UPPER_PORT;
		srvt.intPort = LOWER_PORT;
		srvt.portRange=1;
		srvt.valid = 1;
		rtl8651_setAsicServerPortTable(15, &srvt);

		/**********************************************************************
		WAN to LAN
		**********************************************************************/
		ttl = 254;
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xa;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;

		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 25;
		conf.payload.content				= pktData;
		conf.vlan.vid						=LAN0_VID;
		strtoip(&conf.conf_sip, "240.240.240.240");
		strtoip(&conf.conf_dip,"192.168.4.1");

		stringToConfMac(conf.conf_dmac,GW_WAN0_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf,pktBuf);				/* generate pkt in buff */
		rxPort = PN_PORT5;
		virtualMacInput(rxPort,pktBuf, pktLen+4);		
		toPort = PM_PORT_2;
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &toPort,pktBuf, &pktLen);
		ASSERT(retval==SUCCESS);


		stringToConfMac(conf.conf_smac,GW_LAN1_MAC);
		stringToConfMac(conf.conf_dmac,HOST2_MAC);
		strtoip(&conf.conf_dip,HOST2_IP);
		conf.ip.ttl							= ttl-1;
		conf.conf_dport				= INTERNAL_PORT;
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

		/**********************************************************************
		WAN to LAN 
		Include PPPoE Header
		**********************************************************************/
	
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xa;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.pppoe.type = 0x8864;
		conf.pppoe.session = 0;
		conf.l2Flag						=L2_PPPoE;
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 7;
		conf.payload.content				= pktData;
		conf.vlan.vid						=LAN0_VID;
		strtoip(&conf.conf_sip, "240.240.240.240");
		strtoip(&conf.conf_dip,"192.168.4.1");

		stringToConfMac(conf.conf_dmac,GW_WAN0_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
		rxPort = PN_PORT5;
		virtualMacInput(rxPort,pktBuf, pktLen+4);		
		toPort = PM_PORT_2;
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &toPort,pktBuf, &pktLen);
		ASSERT(retval==SUCCESS);


		conf.l2Flag						=0;
		conf.ip.ttl							= ttl-1;
		stringToConfMac(conf.conf_smac,GW_LAN1_MAC);
		stringToConfMac(conf.conf_dmac,HOST2_MAC);
		strtoip(&conf.conf_dip,HOST2_IP);
		conf.conf_dport				= INTERNAL_PORT;
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

		/**********************************************************************
		LAN to WAN 
		**********************************************************************/
		for (j=0;j<RANDOM_RUN_NUM;j++)
		{
		#if VERBOSE
			rtlglue_printf("Run %d L%d\n",j,__LINE__);
		#endif
			ipaddr = rtlglue_random();	
			ttl =rtlglue_random()&0xff;	
			externalPrt=rtlglue_random()&0xffff;		 
			bzero((void*) &ipt, sizeof(ipt));
			/* wan 0 napt */
			strtoip(&ipt.intIpAddr, "0.0.0.0");
			ipt.extIpAddr= ipaddr;
			rtl8651_setAsicExtIntIpTable(0, &ipt);

			/* config server port table */
			bzero((void*) &srvt, sizeof(srvt));
			strtoip(&srvt.intIpAddr, HOST2_IP);
			srvt.extIpAddr=ipaddr;
			srvt.extPort = externalPrt;
			srvt.intPort = INTERNAL_PORT;
			srvt.valid = 1;
			rtl8651_setAsicServerPortTable(0, &srvt);
			WRITE_MEM32(ALECR,READ_MEM32(ALECR)&~EN_TTL1);	


			RESET_CONF(&conf);    
			conf.pktType						= (rtlglue_random()&0x1)+_PKT_TYPE_TCP;					/* udp packet */
			conf.ip.id							= 0xa;
			conf.ip.ttl							= ttl;
			conf.conf_dport					= EXTERNAL_PORT ^j;
			conf.conf_sport					= INTERNAL_PORT;
			conf.payload.length				= rtlglue_random()&0x1ff;
			conf.payload.content				= pktData;		
			strtoip(&conf.conf_sip, HOST2_IP);
			conf.conf_dip=ipaddr+1;
			stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */

			toPort = memberP[3];
			retval = virtualMacInput(toPort,pktBuf, pktLen+4);		
			ASSERT(retval==SUCCESS);
			retval = virtualMacGetHsa(&hsa);
			ASSERT( retval==SUCCESS );
			if( hsa.dp& PM_PORT_NOTPHY)
			{
#if defined(CONFIG_RTL865X_MODEL_TEST_FT2)
				/* Do not show any thing in FT2 mode. */
#else
				dump_hs();

				do
				{
					uint32 param;
					retval=swNic_intHandler(&param);
					if(retval!=1/*TRUE*/) break;
					swNic_rxThread(param);
				} while(0);
#endif
				continue;
			}
			rxPort = 1<<memberP[5];
			memset(pktBuf,0,pktLen);
			pktLen = _PKT_LEN;
			retval = virtualMacOutput( &rxPort,pktBuf, &pktLen);
			if (retval!=SUCCESS)
			{
				rtlglue_printf( "%s():%d virtualMacOutput() returns %d. ERROR.\n", __FUNCTION__, __LINE__, retval );
				dump_hs();
				rxPort = PM_PORT_ALL;
				pktLen = _PKT_LEN;
				retval = virtualMacOutput( &rxPort,pktBuf, &pktLen);
				rtlglue_printf( "%s():%d retval=%d rxPort=0x%x pktLen=%d\n", __FUNCTION__, __LINE__, retval, rxPort, pktLen );
				if (retval==SUCCESS)
					memDump( pktBuf, pktLen, "pktBuf" );
				return FAILED;
			}
			
			stringToConfMac(conf.conf_smac,GW_WAN0_MAC);
			stringToConfMac(conf.conf_dmac,"00-00-00-00-99-9C");
			conf.conf_sip					= ipaddr;
			conf.conf_sport				= externalPrt;
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
		}


		/**********************************************************************
		LAN to WAN   JumBo 
		Test MAX Data Input and Output
		WAN is  PPPoE 
 		 **********************************************************************/
		for (j=0;j<RANDOM_RUN_NUM;j++)
		{
		#if VERBOSE
			rtlglue_printf("Run %d L%d\n",j,__LINE__);
		#endif
			NaptConfig(1,((1<<14)-1),1,ALGO_SOURCE_BASE);
			memberP =  memberPortList[1];
			ipaddr=0x90909090;
			ttl =rtlglue_random()&0xff;	
			externalPrt=rtlglue_random()&0xffff;		 
			bzero((void*) &ipt, sizeof(ipt));
			/* wan 0 napt */
			strtoip(&ipt.intIpAddr, "0.0.0.0");
			rtl8651_setAsicExtIntIpTable(0, &ipt);

			/* config server port table */
			bzero((void*) &srvt, sizeof(srvt));
			strtoip(&srvt.intIpAddr, HOST2_IP);
			srvt.extIpAddr=ipaddr;
			srvt.extPort = externalPrt;
			srvt.intPort = INTERNAL_PORT;
			srvt.valid = 1;
			srvt.pvaild=1;
			srvt.pid=rtlglue_random();
			rtl8651_setAsicServerPortTable(0, &srvt);
			WRITE_MEM32(ALECR,READ_MEM32(ALECR)&~EN_TTL1);	


			RESET_CONF(&conf);    
			conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
			conf.ip.id							= 0xa;
			conf.ip.ttl							= ttl;
			conf.conf_dport					= EXTERNAL_PORT ^j;
			conf.conf_sport					= INTERNAL_PORT;
			conf.vlan.vid				 	= LAN0_VID;
			conf.pppoe.type					= 0x8864;
			conf.pppoe.session 				= WAN1_ISP_PPPOE_SID;
			conf.payload.length				= 16326-4/*vlan*/-8/*pppoe*/;
			conf.payload.content				= pktData;		
			strtoip(&conf.conf_sip, HOST2_IP);
			conf.conf_dip=ipaddr+1;
			stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, pktBuf);				/* generate pkt in buff */
			toPort = memberP[3];			
			retval = virtualMacInput(toPort,pktBuf, pktLen+4);		
			ASSERT( retval==SUCCESS );
			retval = virtualMacGetHsa(&hsa);
			ASSERT( retval==SUCCESS );
			if( hsa.dp& PM_PORT_NOTPHY)
			{
#if defined(CONFIG_RTL865X_MODEL_TEST_FT2)
				/* Do not show any thing in FT2 mode. */
#else
				dump_hs();

				do
				{
					uint32 param;
					retval=swNic_intHandler(&param);
					if(retval!=1/*TRUE*/) break;
					swNic_rxThread(param);
				} while(0);
#endif
				continue;
			}
			rxPort =1<<memberP[4];
			memset(pktBuf,0,pktLen);			
			pktLen = _PKT_LEN;
			retval = virtualMacOutput( &rxPort,pktBuf, &pktLen);
			if (retval!=SUCCESS)
			{
				rtlglue_printf( "%s():%d virtualMacOutput() returns %d. ERROR.\n", __FUNCTION__, __LINE__, retval );
				dump_hs();
				rxPort = PM_PORT_ALL;
				pktLen = _PKT_LEN;
				retval = virtualMacOutput( &rxPort,pktBuf, &pktLen);
				rtlglue_printf( "%s():%d retval=%d rxPort=0x%x pktLen=%d\n", __FUNCTION__, __LINE__, retval, rxPort, pktLen );
				if (retval==SUCCESS)
					memDump( pktBuf, pktLen, "pktBuf" );
				return FAILED;
			}
		
			conf.vlan.vid						= 0x9;
			conf.l2Flag						= L2_VLAN|L2_PPPoE;
			stringToConfMac(conf.conf_smac,"00-00-00-00-88-89");
			stringToConfMac(conf.conf_dmac,"00-00-00-00-00-20");
			conf.conf_sip					= ipaddr;
			conf.conf_sport				= externalPrt;
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen+4, __FUNCTION__,__LINE__,i);			
		}
		

		/**********************************************************************
		LAN to WAN   JumBo
		**********************************************************************/
		for (j=0;j<RANDOM_RUN_NUM;j++)
		{
		#if VERBOSE
			rtlglue_printf("Run %d L%d\n",j,__LINE__);
		#endif
			NaptConfig(0,3000,0,ALGO_SOURCE_BASE);
			memberP =  memberPortList[0];
			ipaddr = rtlglue_random();	
			ttl =rtlglue_random()&0xff;	
			externalPrt=rtlglue_random()&0xffff;		 
			bzero((void*) &ipt, sizeof(ipt));
			/* wan 0 napt */
			strtoip(&ipt.intIpAddr, "0.0.0.0");
			rtl8651_setAsicExtIntIpTable(0, &ipt);

			/* config server port table */
			bzero((void*) &srvt, sizeof(srvt));
			strtoip(&srvt.intIpAddr, HOST2_IP);
			srvt.extIpAddr=ipaddr;
			srvt.extPort = externalPrt;
			srvt.intPort = INTERNAL_PORT;
			srvt.valid = 1;
			srvt.pvaild=1;
			srvt.pid=rtlglue_random();
			rtl8651_setAsicServerPortTable(0, &srvt);
			WRITE_MEM32(ALECR,READ_MEM32(ALECR)&~EN_TTL1);	


			RESET_CONF(&conf);    
			conf.pktType						= (rtlglue_random()&0x1)+_PKT_TYPE_TCP;					/* udp packet */
			conf.ip.id							= 0xa;
			conf.ip.ttl							= ttl;
			conf.conf_dport					= EXTERNAL_PORT ^j;
			conf.conf_sport					= INTERNAL_PORT;
			conf.vlan.vid						= LAN0_VID;
			conf.vlan.prio						= 7;
			conf.pppoe.type					= 0x8864;
			conf.pppoe.session 				= 877;
			conf.payload.length				= 2500;
			conf.l2Flag						= L2_VLAN|L2_PPPoE;
			conf.payload.content				= pktData;		
			strtoip(&conf.conf_sip, HOST2_IP);
			conf.conf_dip=ipaddr+1;
			stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
			stringToConfMac(conf.conf_smac,HOST1_MAC);
			pktLen = pktGen(&conf, pktBuf);				/* generate pkt in buff */
			toPort = memberP[3];			
			retval = virtualMacInput(toPort,pktBuf, pktLen+4);		
			ASSERT( retval==SUCCESS );
			retval = virtualMacGetHsa(&hsa);
			ASSERT( retval==SUCCESS );
			if( hsa.dp& PM_PORT_NOTPHY)
			{
#if defined(CONFIG_RTL865X_MODEL_TEST_FT2)
				/* Do not show any thing in FT2 mode. */
#else
				dump_hs();
				do
				{
					uint32 param;
					retval=swNic_intHandler(&param);
					if(retval!=1/*TRUE*/) break;
					swNic_rxThread(param);
				} while(0);
#endif
				continue;
			}
			rxPort =1<<memberP[5];
			memset(pktBuf,0,pktLen);			
			pktLen = _PKT_LEN;
			retval = virtualMacOutput( &rxPort,pktBuf, &pktLen);
			if (retval!=SUCCESS)
			{
				rtlglue_printf( "%s():%d virtualMacOutput() returns %d. ERROR.\n", __FUNCTION__, __LINE__, retval );
				dump_hs();
				rxPort = PM_PORT_ALL;
				pktLen = _PKT_LEN;
				retval = virtualMacOutput( &rxPort,pktBuf, &pktLen);
				rtlglue_printf( "%s():%d retval=%d rxPort=0x%x pktLen=%d\n", __FUNCTION__, __LINE__, retval, rxPort, pktLen );
				if (retval==SUCCESS)
					memDump( pktBuf, pktLen, "pktBuf" );
				return FAILED;
			}
		
			conf.vlan.vid						= WAN0_VID;
			conf.l2Flag						= 0;
			stringToConfMac(conf.conf_smac,GW_WAN0_MAC);
			stringToConfMac(conf.conf_dmac,WAN0_ISP_MAC);
			conf.conf_sip					= ipaddr;
			conf.conf_sport				= externalPrt;
			pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
			PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen+4, __FUNCTION__,__LINE__,i);			

		}
		
	}

	return SUCCESS;
}
int32 testPktIPMulticast(uint32 caseNo)
{
	int i,retval;

	enum PORT_MASK rxPort;	
	enum PORT_MASK toPort;	
	int32 pktLen,pktExpectLen;
   	rtl8651_PktConf_t conf;
	int8 *pktExpect,*pktBuf,*pktData;
	uint8 ttl;
	uint32 multi_mtu;
	rtl865x_tblAsicDrv_multiCastParam_t mcast; 
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	uint8 *memberP;

	pktExpect = sharedPkt[0];
	pktBuf = sharedPkt[1];
	pktData =  sharedPkt[2];
	for (i=1;i<512;i++) pktData[i]=i;
	ttl=0x23;
	
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{

		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		NaptConfig(0,3000,0,ALGO_SOURCE_BASE);
		memberP=memberPortList[0];
		multi_mtu=1500;
		WRITE_MEM32(CSCR,0);
		WRITE_MEM32(SWTCR0,(PM_PORT_3<<5)|READ_MEM32(SWTCR0));
		/**********************************************************************
		IP Multicast Packet 224.0.0.3
		expect hsa.trip WAN0_ISP_PUBLIC_IP
		**********************************************************************/
		rtl8651_setAsicMulticastMTU(multi_mtu);
		rtl8651_setAsicMulticastEnable(TRUE);

		bzero((void*) &mcast,sizeof(mcast));
		mcast.sip= ntohl(inet_addr("192.168.1.1"));
		mcast.dip=ntohl(inet_addr("224.0.0.3"));
		mcast.mbr=PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4;
		mcast.svid=LAN1_VID;
		mcast.port=PN_PORT1;
		WRITE_MEM32(ALECR,READ_MEM32(ALECR)&~(uint32)EN_TTL1);	
 	 	rtl8651_setAsicIpMulticastTable(&mcast);

		/**********************************************************************
		LAN to WAN
		**********************************************************************/

		ttl = 254;
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xa;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;

		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 25;
		conf.payload.content				= pktData;
		strtoip(&conf.conf_sip, "192.168.1.1");
		strtoip(&conf.conf_dip,"224.0.0.3");
		stringToConfMac(conf.conf_dmac," 01-00-5e-00-00-03");
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf,pktBuf);				/* generate pkt in buff */		
		toPort = memberP[1];
		retval = virtualMacInput(toPort,pktBuf, pktLen+4);		
		ASSERT( retval==SUCCESS );
		/**********************************************************************
		Expect : Layer2 operation (bacause no source filter)
		**********************************************************************/
		rxPort =1<< memberP[1];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
		/**********************************************************************
		Expect :  Layer2  operation
		**********************************************************************/
		rxPort =1<< memberP[2];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
		/**********************************************************************
		Expect :  Layer3 operation
		**********************************************************************/
		rxPort =1<< memberP[4];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		stringToConfMac(conf.conf_smac,GW_LAN1_MAC);
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
		/**********************************************************************
		Expect :  NAT 
		**********************************************************************/
		rxPort =1<< memberP[3];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		stringToConfMac(conf.conf_smac,GW_LAN1_MAC);
		strtoip(&conf.conf_sip,WAN0_ISP_PUBLIC_IP);
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

		/**********************************************************************
		Remarking
		**********************************************************************/
		WRITE_MEM32(ALECR,READ_MEM32(ALECR)|EN_TTL1);	
		ttl = 250;
		WRITE_MEM32(RMCR1P,0xffffffff);
		WRITE_MEM32(DSCPRM0,0xffffffff);
		WRITE_MEM32(DSCPRM1,0xffffffff);
		WRITE_MEM32(RLRC,RMLC_DSCP_L4|RMLC_DSCP_L3|RMLC_DSCP_L2|RMLC_8021P_L4|RMLC_8021P_L3|RMLC_8021P_L2);
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xa;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 25;
		conf.payload.content				= pktData;
		strtoip(&conf.conf_sip, "192.168.1.1");
		strtoip(&conf.conf_dip,"224.0.0.3");
		stringToConfMac(conf.conf_dmac," 01-00-5e-00-00-03");
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf,pktBuf);				/* generate pkt in buff */		
		toPort = memberP[1];
		retval = virtualMacInput(toPort,pktBuf, pktLen+4);		
		ASSERT( retval==SUCCESS );
		/**********************************************************************
		Expect : Layer2 operation (bacause no source filter)
		**********************************************************************/
		rxPort =1<< memberP[1];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		conf.ip.tos						=0xfc;
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
		/**********************************************************************
		Expect :  Layer2  operation
		**********************************************************************/
		rxPort =1<< memberP[2];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
		/**********************************************************************
		Expect :  Layer3 operation
		**********************************************************************/
		rxPort =1<< memberP[4];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		stringToConfMac(conf.conf_smac,GW_LAN1_MAC);
		conf.ip.ttl						=ttl-1;
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

		/**********************************************************************
		Expect :  NAT 
		**********************************************************************/
		rxPort =1<< memberP[3];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		stringToConfMac(conf.conf_smac,GW_LAN1_MAC);
		strtoip(&conf.conf_sip,WAN0_ISP_PUBLIC_IP);
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			


		/**********************************************************************
		checksum is zero	
		**********************************************************************/
		pktBuf[40]=0;
		pktBuf[41]=0;

		/**********************************************************************
		WAN to LAN 
		Include PPPoE Header
		**********************************************************************/
		WRITE_MEM32(RLRC,0);
		bzero((void*) &mcast,sizeof(mcast));
		mcast.sip= ntohl(inet_addr("192.168.4.4"));
		mcast.dip=ntohl(inet_addr("224.0.0.3"));
		mcast.mbr=PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4;
		mcast.svid=LAN0_VID;
		mcast.port=PN_PORT3;
		WRITE_MEM32(ALECR,READ_MEM32(ALECR)|(uint32)EN_TTL1);	
 	 	rtl8651_setAsicIpMulticastTable(&mcast);

		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xb;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.pppoe.type 					= PPPOESESSION_ETHERTYPE;
		conf.pppoe.session 				= 0;
		conf.l2Flag						= L2_PPPoE|L2_VLAN;
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 7;
		conf.payload.content				= pktData;
		conf.vlan.vid						=LAN0_VID;
		strtoip(&conf.conf_dip, "224.0.0.3");
		strtoip(&conf.conf_sip,"192.168.4.4");

		stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
		toPort = memberP[3];
		retval = virtualMacInput(toPort,pktBuf, pktLen+4);		
		ASSERT( retval==SUCCESS );
		rxPort = 1<<memberP[1];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		conf.l2Flag						=0;
		conf.ip.ttl							= ttl-1;
		stringToConfMac(conf.conf_smac,GW_LAN0_MAC);
		stringToConfMac(conf.conf_dmac," 01-00-5e-00-00-03");
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

		rxPort = 1<<memberP[0];
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );

		rxPort = 1<<memberP[2];
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
	
		rxPort = 1<<memberP[3];
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );

		rxPort = 1<<memberP[4];
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );

		/**********************************************************************
		WAN to LAN 
		Include PPPoE Header
		Ignore VLAN TAG	
		**********************************************************************/
		WRITE_MEM32( VCR0,READ_MEM32(VCR0)| EN_1QTAGVIDIGNORE);	
		bzero((void*) &mcast,sizeof(mcast));
		mcast.sip= ntohl(inet_addr("192.168.4.4"));
		mcast.dip=ntohl(inet_addr("224.0.0.3"));
		mcast.mbr=PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4;
		mcast.svid=LAN0_VID;
		mcast.port=PN_PORT3;
		WRITE_MEM32(ALECR,READ_MEM32(ALECR)|(uint32)EN_TTL1);	
 	 	rtl8651_setAsicIpMulticastTable(&mcast);

		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xc;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.pppoe.type 					= PPPOESESSION_ETHERTYPE;
		conf.pppoe.session 				= 0;
		conf.l2Flag						= L2_PPPoE|L2_VLAN;
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 7;
		conf.payload.content				= pktData;
		conf.vlan.vid						= LAN0_VID;
		strtoip(&conf.conf_dip, "224.0.0.3");
		strtoip(&conf.conf_sip,"192.168.4.4");

		stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
		toPort = memberP[3];
		retval = virtualMacInput(toPort,pktBuf, pktLen+4);		
		ASSERT( retval==SUCCESS );
		rxPort = 1<<memberP[1];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		conf.l2Flag						= L2_VLAN;
		conf.ip.ttl							= ttl-1;
		stringToConfMac(conf.conf_smac,GW_LAN0_MAC);
		stringToConfMac(conf.conf_dmac," 01-00-5e-00-00-03");
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

		rxPort = 1<<memberP[0];
		pktLen = _PKT_LEN;
		memset(pktBuf,0,pktLen);
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );

		rxPort = 1<<memberP[2];
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
	
		rxPort = 1<<memberP[3];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );

		rxPort = 1<<memberP[4];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		
		/**********************************************************************
		WAN to LAN 
		Include PPPoE Header
		Remarking
		**********************************************************************/
		WRITE_MEM32(RMCR1P,0xffffffff);
		WRITE_MEM32(DSCPRM0,0xffffffff);
		WRITE_MEM32(DSCPRM1,0xffffffff);
		WRITE_MEM32(RLRC,RMLC_DSCP_L4|RMLC_DSCP_L3|RMLC_DSCP_L2|RMLC_8021P_L4|RMLC_8021P_L3|RMLC_8021P_L2);

		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xb;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.pppoe.type 					= PPPOESESSION_ETHERTYPE;
		conf.pppoe.session 				= 0;
		conf.l2Flag						= L2_PPPoE|L2_VLAN;
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 7;
		conf.ip.tos						= 0x2;
		conf.payload.content				= pktData;
		conf.vlan.vid						= LAN0_VID;
		strtoip(&conf.conf_dip, "224.0.0.3");
		strtoip(&conf.conf_sip,"192.168.4.4");
		stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
		
		toPort = memberP[3];
		retval = virtualMacInput(toPort,pktBuf, pktLen+4);		
		ASSERT( retval==SUCCESS );
		rxPort = 1<<memberP[1];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		
		conf.l2Flag						= L2_VLAN;
		conf.vlan.prio						= 0x7;
		conf.ip.tos						= 0xfe;
		conf.ip.ttl							= ttl-1;
		stringToConfMac(conf.conf_smac,GW_LAN0_MAC);
		stringToConfMac(conf.conf_dmac," 01-00-5e-00-00-03");
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

		rxPort = 1<<memberP[0];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );

		rxPort = 1<<memberP[2];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
	
		rxPort = 1<<memberP[3];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );

		rxPort = 1<<memberP[4];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);	
		/**********************************************************************
		WAN to LAN 
		Include PPPoE Header
              Layer3  Remarking Disable
              Expect  No Remarking
		**********************************************************************/
		WRITE_MEM32(RMCR1P,0xffffffff);
		WRITE_MEM32(DSCPRM0,0xffffffff);
		WRITE_MEM32(DSCPRM1,0xffffffff);
		WRITE_MEM32(RLRC,(RMLC_DSCP_L4|RMLC_DSCP_L2));
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xb;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.pppoe.type 					= PPPOESESSION_ETHERTYPE;
		conf.pppoe.session 				= 0;
		conf.l2Flag						= L2_PPPoE|L2_VLAN;
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 7;
		conf.ip.tos						= 0x2;
		conf.payload.content				= pktData;
		conf.vlan.vid						= LAN0_VID;
		strtoip(&conf.conf_dip, "224.0.0.3");
		strtoip(&conf.conf_sip,"192.168.4.4");
		stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
		
		toPort = memberP[3];
		virtualMacInput(toPort,pktBuf, pktLen+4);		
		rxPort = 1<<memberP[1];
		pktLen = _PKT_LEN;
		memset(pktBuf,0,pktLen);
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		
		conf.l2Flag						= L2_VLAN;
		conf.ip.ttl							= ttl-1;
		stringToConfMac(conf.conf_smac,GW_LAN0_MAC);
		stringToConfMac(conf.conf_dmac," 01-00-5e-00-00-03");
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

		rxPort = 1<<memberP[0];
		pktLen = _PKT_LEN;
		memset(pktBuf,0,pktLen);
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);

		rxPort = 1<<memberP[2];
		pktLen = _PKT_LEN;
		memset(pktBuf,0,pktLen);
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
	
		rxPort = 1<<memberP[3];
		pktLen = _PKT_LEN;
		memset(pktBuf,0,pktLen);
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);

		rxPort = 1<<memberP[4];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );

	
		
		/**********************************************************************
		IP Multicast Remarking Session 8864
		*********************************************************************/
		rtl8651_setProtocolBasedVLAN(RTL8651_PBV_RULE_PPPOE_SESSION, memberP[3], TRUE, 333 );	
		bzero(&vlan, sizeof(vlan));
		vlan.memberPortMask = (1<<memberP[3])|(1<<memberP[2]);
		rtl8651_setAsicVlan(333,&vlan);
		WRITE_MEM32( VCR0,READ_MEM32(VCR0)&~EN_1QTAGVIDIGNORE);	
		WRITE_MEM32(RMCR1P,0xffffffff);
		WRITE_MEM32(DSCPRM0,0xffffffff);
		WRITE_MEM32(DSCPRM1,0xffffffff);
		WRITE_MEM32(RLRC,RMLC_DSCP_L4|RMLC_DSCP_L3|RMLC_DSCP_L2|RMLC_8021P_L4|RMLC_8021P_L3|RMLC_8021P_L2);
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xd;
		conf.ip.tos						= 0x1;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.pppoe.type					 = PPPOESESSION_ETHERTYPE;
		conf.pppoe.session 				= 0x55;
		conf.l2Flag						= L2_PPPoE;
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 7;
		conf.payload.content				= pktData;
		strtoip(&conf.conf_dip, "224.0.0.3");
		strtoip(&conf.conf_sip,"192.168.4.4");
		stringToConfMac(conf.conf_dmac,HOST2_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
		toPort = memberP[3];
		retval=virtualMacInput(toPort,pktBuf, pktLen+4);		
		ASSERT(retval==SUCCESS);
		rxPort = 1<<memberP[2];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		conf.l2Flag						= L2_PPPoE|L2_VLAN;
		conf.vlan.vid						= 333;
		conf.ip.tos						= 0xfd;
		conf.vlan.prio						= 0x7;
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
		/**********************************************************************
		IP Multicast Remarking Session 8863
		*********************************************************************/
		rtl8651_setProtocolBasedVLAN(RTL8651_PBV_RULE_PPPOE_CONTROL, memberP[3], TRUE, 635 );	
		bzero(&vlan, sizeof(vlan));
		vlan.memberPortMask =(1<<memberP[3])|(1<<memberP[2]);
		rtl8651_setAsicVlan(635,&vlan);
		WRITE_MEM32(RMCR1P,0xffffffff);
		WRITE_MEM32(DSCPRM0,0xffffffff);
		WRITE_MEM32(DSCPRM1,0xffffffff);
		WRITE_MEM32(RLRC,0xFFFFFFFF);
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xe;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.pppoe.type					= PPPOECONTROL_ETHERTYPE;
		conf.pppoe.session					= 0x56;
		conf.l2Flag						= L2_PPPoE;
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 7;
		conf.payload.content				= pktData;
		strtoip(&conf.conf_dip, "224.0.0.3");
		strtoip(&conf.conf_sip,"192.168.4.4");
		stringToConfMac(conf.conf_dmac,HOST2_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
		toPort = memberP[3];
		retval=virtualMacInput(toPort,pktBuf, pktLen+4);		
		ASSERT(retval==SUCCESS);
		rxPort = 1<<memberP[2];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT( retval==SUCCESS );
		conf.l2Flag						= L2_PPPoE|L2_VLAN;
		conf.vlan.vid						= 635;
		conf.vlan.prio						= 0x7;
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			


		

	}
	return SUCCESS;
}



	


int32 testPktGRE(uint32 caseNo)
{
	int i,retval;
	enum PORT_MASK rxPort;	
	enum PORT_MASK toPort;	
	int32 pktLen,pktExpectLen;
   	rtl8651_PktConf_t conf;
	int8 *pktExpect,*pktBuf,*pktData;
	uint8 ttl;
	uint32 tblIdx;
	rtl865x_tblAsicDrv_naptIcmpParam_t pptpAsicEntry;
	int8 *memberP;
	hsa_param_t hsa;

	pktExpect = sharedPkt[0];
	pktBuf = sharedPkt[1];
	pktData =  sharedPkt[2];
	ASSERT(pktData!=NULL);
	for (i=1;i<512;i++) pktData[i]=i;
	
#if defined(RTL865X_MODEL_USER) /* Since NIC only can be initialized once, we always run in one mode. */
	for( i = IC_TYPE_MODEL; i <= IC_TYPE_MODEL; i++ )
#else
	for( i = IC_TYPE_REAL; i <= IC_TYPE_REAL; i++ )
#endif
	{

		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		

        rtl8651_clearRegister();
        rtl8651_clearAsicAllTable();
        rtl8651_clearAsicPvid();

        /* module switch control reg: enable L2/L3/L4/STP, trap 802.1D/GARP to CPU */
        rtl8651_setAsicOperationLayer(3);

		NaptConfig(0,1500,0,ALGO_SOURCE_BASE);
		retval=configNicInit();
		ASSERT(retval==SUCCESS);		
		memberP = memberPortList[0];		
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));		
		WRITE_MEM32(SWTCR0,READ_MEM32(SWTCR0)|NAPTF2CPU);
		memset(&pptpAsicEntry, 0, sizeof(rtl865x_tblAsicDrv_naptIcmpParam_t));
		pptpAsicEntry.insideLocalId =0x201;
		pptpAsicEntry.insideLocalIpAddr =ntohl(inet_addr("192.168.1.2"));
		pptpAsicEntry.offset =0xffff;
		pptpAsicEntry.isCollision = 1;
		pptpAsicEntry.isStatic = 1;
		pptpAsicEntry.isValid = 1;
		pptpAsicEntry.isPptp = 1;
		pptpAsicEntry.isSpi = 0;
		pptpAsicEntry.direction = 0;
		rtl8651_naptIcmpTableIndex(pptpAsicEntry.insideLocalIpAddr,pptpAsicEntry.insideLocalId,ntohl(inet_addr("140.0.0.3")),&tblIdx); 
		rtl8651_setAsicNaptIcmpTable(TRUE, tblIdx, &pptpAsicEntry);
		/**********************************************************************
		expect hsa.trip WAN0_ISP_PUBLIC_IP
		**********************************************************************/
		/**********************************************************************
		LAN to WAN
		**********************************************************************/
		ttl = 254;
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_PPTP;					/* udp packet */
		conf.ip.id							= 0x111;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.pptp.callid					= pptpAsicEntry.insideLocalId;
		conf.payload.length				= 28;
		conf.payload.content				= dstData; 
		strtoip(&conf.conf_sip, "192.168.1.2");
		strtoip(&conf.conf_dip,"140.0.0.3");
		stringToConfMac(conf.conf_dmac,GW_LAN0_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf,pktBuf);				/* generate pkt in buff */
		toPort = memberP[3];
		virtualMacInput(toPort,pktBuf, pktLen+4);		
		rxPort =1<<memberP[5];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT(retval==SUCCESS);
		stringToConfMac(conf.conf_dmac,WAN0_ISP_MAC);
		stringToConfMac(conf.conf_smac,GW_WAN0_MAC);
		conf.ip.ttl							= ttl-1;
		strtoip(&conf.conf_sip, "192.168.4.1");
		conf.pptp.callid					= 	pptpAsicEntry.offset;
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
		/**********************************************************************
		WAN to LAN 
		Include PPPoE Header
		expect to CPU
		**********************************************************************/
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_PPTP;					/* udp packet */
		conf.ip.id							= 0x112;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.pptp.callid					= pptpAsicEntry.offset;
		conf.payload.length				= 28;
		conf.payload.content				= dstData; 
		strtoip(&conf.conf_sip, "140.0.0.3");
		strtoip(&conf.conf_dip,"192.168.4.1");
		stringToConfMac(conf.conf_dmac,GW_WAN0_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
		toPort =memberP[5];
		virtualMacInput(toPort,pktBuf, pktLen+4);		
		virtualMacGetHsa(&hsa);
		IS_EQUAL_INT("Pakcet should be to CPU,but ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ );
		/**********************************************************************
		WAN to LAN 
		Include PPPoE Header
		expect to forward
		**********************************************************************/
		memset(&pptpAsicEntry, 0, sizeof(rtl865x_tblAsicDrv_naptIcmpParam_t));
		pptpAsicEntry.insideLocalId =0x23;
		pptpAsicEntry.insideLocalIpAddr =ntohl(inet_addr("192.168.1.2"));
		pptpAsicEntry.offset =0xffff;
		pptpAsicEntry.isCollision = 1;
		pptpAsicEntry.isStatic = 1;
		pptpAsicEntry.isValid = 1;
		pptpAsicEntry.isPptp = 1;
		pptpAsicEntry.isSpi = 0;
		pptpAsicEntry.direction = 0;
		rtl8651_naptIcmpTableIndex(ntohl(inet_addr("140.0.0.3")),pptpAsicEntry.offset,ntohl(inet_addr("192.168.4.1")),&tblIdx); 
		rtl8651_setAsicNaptIcmpTable(TRUE, tblIdx, &pptpAsicEntry);
		toPort =memberP[5];
		virtualMacInput(toPort,pktBuf, pktLen+4);		
		rxPort =1<<memberP[1];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT(retval==SUCCESS);
		conf.l2Flag						= 0;
		conf.ip.ttl							= ttl-1;
		conf.pptp.callid					= pptpAsicEntry.insideLocalId;
		strtoip(&conf.conf_dip,"192.168.1.2");
		stringToConfMac(conf.conf_smac,GW_LAN1_MAC);
		stringToConfMac(conf.conf_dmac,"00-00-00-00-00-40");

		conf.conf_dport				= INTERNAL_PORT;
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			
		/**********************************************************************
		Remarking
		**********************************************************************/
		
	}
	return SUCCESS;
}



int32 testPktIGMP(uint32 caseNo)
{
	int i,retval;

	enum PORT_MASK rxPort;	
	enum PORT_MASK toPort;	
	int32 pktLen,pktExpectLen;
   	rtl8651_PktConf_t conf;
	ipaddr_t mcast_1 ;
	int8 *pktExpect,*pktBuf,*pktData;
	uint8 ttl;
	uint32 multi_mtu;
	rtl865x_tblAsicDrv_multiCastParam_t mcast; 
	hsa_param_t hsa;
	int8 *memberP;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	
	mcast_1 = ntohl(inet_addr("234.1.1.1"));
	multi_mtu=1500;
	pktExpect = rtlglue_malloc(2048);
	assert(pktExpect==0);
	pktBuf = rtlglue_malloc(2048);
	assert(pktBuf==0);
	pktData = rtlglue_malloc(512);
	assert(pktData==0);
	for (i=1;i<512;i++) pktData[i]=i;
	
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{

		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		NaptConfig(0,3000,0,ALGO_SOURCE_BASE);
		memberP=memberPortList[0];
		/**********************************************************************
		IP Multicast Packet 224.0.0.3
		expect hsa.trip WAN0_ISP_PUBLIC_IP
		**********************************************************************/
		rtl8651_setAsicMulticastMTU(multi_mtu);
		rtl8651_setAsicMulticastEnable(TRUE);
		bzero((void*) &mcast,sizeof(mcast));
		mcast.dip= ntohl(inet_addr("224.0.0.3"));
		mcast.sip=ntohl(inet_addr("192.168.1.1"));
		mcast.mbr=PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
		mcast.svid=LAN0_VID;
		mcast.port=PN_PORT3;
 	 	rtl8651_setAsicIpMulticastTable(&mcast);
		/**********************************************************************
		LAN to WAN
		**********************************************************************/
		ttl = 254;
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_IGMP;					/* igmp packet */
		conf.l2Flag						= 0;
		conf.ErrFlag						= 0;
		conf.conf_Flags					|= (CSUM_IP|CSUM_L4);
		conf.ip.id							= 0xa;
		conf.ip.mf						= 0;
		conf.ip.offset						= 0;
		conf.ip.ttl							= 1;		/* igmp ttl */
		conf.igmp.ver						= 2;
		conf.igmp.type					= IGMP_V2_LEAVE;
		conf.igmp.respTime				= 0;
		conf.igmp.gaddr					= mcast_1;
		conf.payload.content				= NULL;
		conf.payload.length				= 0;
		conf.conf_vlanIdx					= 2;
		conf.conf_pppoeIdx				= 0;
		strtoip(&conf.conf_sip, "192.168.1.1");
		strtoip(&conf.conf_dip,"224.0.0.3");
		stringToConfMac(conf.conf_dmac," 01-00-5e-00-00-03");
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf,pktBuf);				/* generate pkt in buff */
		toPort = memberP[3];
		virtualMacInput(toPort,pktBuf, pktLen+4);		
		virtualMacGetHsa(&hsa);
		IS_EQUAL_INT("Pakcet should be to CPU,but ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ );	
		/**********************************************************************
		Unicast  
		Remarking
		TOS	 0x1F;
		Expect : TOS 0xFF
		**********************************************************************/	
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));		
		rtl8651_setProtocolBasedVLAN(RTL8651_PBV_RULE_PPPOE_SESSION, memberP[3], TRUE, 1024 );	
		bzero(&vlan, sizeof(vlan));
		vlan.memberPortMask=PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4;
		rtl8651_setAsicVlan(1024,&vlan);
		WRITE_MEM32(RLRC,0xFFFFFFFF);
		WRITE_MEM32(DSCPRM0,0xffffffff);
		WRITE_MEM32(DSCPRM1,0xffffffff);		
		ttl = 254;
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_IGMP;					/* igmp packet */
		conf.l2Flag						= 0;
		conf.ErrFlag						= 0;
		conf.ip.tos						= 0x1f;
		conf.conf_Flags					|= (CSUM_IP|CSUM_L4);
		conf.ip.id							= 0xa;
		conf.ip.mf						= 0;
		conf.ip.offset						= 0;
		conf.ip.ttl							= 1;		/* igmp ttl */
		conf.igmp.ver						= 2;
		conf.igmp.type					= IGMP_V2_LEAVE;
		conf.igmp.respTime				= 0;
		conf.igmp.gaddr					= mcast_1;
		conf.payload.content				= NULL;
		conf.payload.length				= 0;
		conf.pppoe.type 					= PPPOESESSION_ETHERTYPE;
		conf.pppoe.session 				= 0;
		conf.l2Flag						= L2_PPPoE;
		strtoip(&conf.conf_sip, "192.168.1.1");
		strtoip(&conf.conf_dip,"224.0.0.3");
		stringToConfMac(conf.conf_dmac,HOST2_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf,pktBuf);				/* generate pkt in buff */
		toPort = memberP[3];
		retval=virtualMacInput(toPort,pktBuf, pktLen+4);		
		ASSERT(retval==SUCCESS);
		rxPort =  1<<memberP[2];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		conf.vlan.vid						= 1024;
		conf.l2Flag						= L2_PPPoE|L2_VLAN;
		conf.ip.tos						= 0xff;
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);						
		/**********************************************************************
		WAN to LAN 
		Include PPPoE Header
		**********************************************************************/	
		RESET_CONF(&conf);    
		conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
		conf.ip.id							= 0xa;
		conf.ip.ttl							= ttl;
		conf.conf_dport					= EXTERNAL_PORT;
		conf.pppoe.type 					= PPPOESESSION_ETHERTYPE;
		conf.pppoe.session					= 0;
		conf.l2Flag						= L2_PPPoE;
		conf.conf_sport					= EXTERNAL_PORT+1;
		conf.payload.length				= 7;
		conf.payload.content				= pktData;
		conf.vlan.vid						= LAN0_VID;
		strtoip(&conf.conf_sip, "240.240.240.240");
		strtoip(&conf.conf_dip,"192.168.4.1");
		stringToConfMac(conf.conf_dmac,GW_WAN0_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf, (int8*)pktBuf);				/* generate pkt in buff */
		toPort = memberP[5];
		retval=virtualMacInput(toPort,pktBuf, pktLen+4);		
		ASSERT(retval==SUCCESS);
		rxPort = 1<<memberP[2];
		memset(pktBuf,0,pktLen);
		pktLen = _PKT_LEN;
		retval=virtualMacOutput( &rxPort,pktBuf, &pktLen);
		ASSERT(retval==SUCCESS);
		conf.l2Flag						= 0;
		conf.ip.ttl							= ttl-1;
		stringToConfMac(conf.conf_smac,GW_LAN1_MAC);
		stringToConfMac(conf.conf_dmac,HOST2_MAC);
		strtoip(&conf.conf_dip,HOST2_IP);
		conf.conf_dport					= INTERNAL_PORT;
		pktExpectLen = pktGen(&conf, pktExpect);				/* generate pkt in buff */		
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);			

		
	}
	return SUCCESS;
}


/*
 *  Test the IC behavior if the IP->protocol is NOT UDP/TCP/ICMP/IGMP.
 *  1. Outbound unsupported protocol and not NAT, expect to CPU
 *  2. Outbound unsupported protocol but is from NAT host(NI), expect to Port 5
 *  3. Inbound unsupported protocol and not NAT, expect to CPU
 *  4. Inbound unsupported protocol but is to NAT host(NE), expect to Port 1 (Host1).
 */
int32 testIpOther(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb;
	hsa_param_t hsa;
	int i;
   	rtl8651_PktConf_t conf;
   	uint32 pktLen, pktExpectLen;
   	enum PORT_NUM fromPort;
   	enum PORT_MASK toPort;
   	uint8 *pktBuf = sharedPkt[0];
   	uint8 *pktExpect = sharedPkt[1];
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	/* for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ ) */
	for( i = IC_TYPE_REAL; i < IC_TYPE_MODEL; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		/* --------------------------------------------------------------- */
		/* 0. Initialization */
		retval = layer4Config();
		ASSERT(retval==SUCCESS);
		retval = configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32( SWTCR0, READ_MEM32(SWTCR0)|NAPTF2CPU ); /* to trap unknown outbound traffic */
		WRITE_MEM32( SWTCR0, READ_MEM32(SWTCR0)|EnNAPTRNotFoundDrop ); /* to trap unknown inbound trraffic */

		/* --------------------------------------------------------------- */
		/* 1. Unsupported protocol and not NAT, expect to CPU */
		RESET_CONF(&conf);    
		conf.pktType					= _PKT_TYPE_IP;					/* ip packet */
		conf.l2Flag						= 0;
		conf.ErrFlag					= 0;
		conf.conf_Flags					|= (CSUM_IP|CSUM_L4);
		conf.ip.protocol                = 78;
		conf.ip.id						= 0xa;
		conf.ip.mf						= 0;
		conf.ip.offset					= 0;
		conf.ip.ttl						= 1;		/* igmp ttl */
		conf.payload.content			= NULL;
		conf.payload.length				= 0;
		conf.conf_vlanIdx				= 2;
		conf.conf_pppoeIdx				= 0;
		strtoip(&conf.conf_sip,HOST2_IP);
		strtoip(&conf.conf_dip,"222.222.222.222");
		stringToConfMac(conf.conf_dmac,GW_LAN1_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf,pktBuf);				/* generate pkt in buff */
		fromPort = PN_PORT2;
		retval = virtualMacInput(fromPort,pktBuf, pktLen+4);
		ASSERT( retval == SUCCESS );
		
		/* --------------------------------------------------------------- */
		/* Compare HSB/HSA */
		retval = virtualMacGetHsb( &hsb );
		ASSERT( retval==SUCCESS );
		IS_EQUAL_INT_DETAIL("hsb.type should be IP type.", hsb.type, HSB_TYPE_IP, __FUNCTION__, __LINE__,i );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval==SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be trapped to CPU", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );

		/* pump remain packet */
		retval = virtualMacPumpPackets (PM_PORT_ALL );
		ASSERT( retval==SUCCESS );

		/* --------------------------------------------------------------- */
		/* 2. Unsupported protocol but is from NAT host(NI), expect to Port 5 */
		RESET_CONF(&conf);    
		conf.pktType					= _PKT_TYPE_IP;					/* ip packet */
		conf.l2Flag						= 0;
		conf.ErrFlag					= 0;
		conf.conf_Flags					|= (CSUM_IP|CSUM_L4);
		conf.ip.protocol                = 79;
		conf.ip.id						= 0xa;
		conf.ip.mf						= 0;
		conf.ip.offset					= 0;
		conf.ip.ttl						= 2;		/* ttl */
		conf.payload.content			= NULL;
		conf.payload.length				= 0;
		conf.conf_vlanIdx				= 2;
		conf.conf_pppoeIdx				= 0;
		strtoip(&conf.conf_sip,HOST1_IP);
		strtoip(&conf.conf_dip,"222.222.222.222");
		stringToConfMac(conf.conf_dmac,GW_LAN1_MAC);
		stringToConfMac(conf.conf_smac,HOST1_MAC);
		pktLen = pktGen(&conf,pktBuf);				/* generate pkt in buff */
		fromPort = PN_PORT1;
		retval = virtualMacInput(fromPort,pktBuf, pktLen+4);
		ASSERT( retval == SUCCESS );

		/* --------------------------------------------------------------- */
		/* Compare HSB/HSA */
		retval = virtualMacGetHsb( &hsb );
		ASSERT( retval==SUCCESS );
		IS_EQUAL_INT_DETAIL("hsb.type should be IP type.", hsb.type, HSB_TYPE_IP, __FUNCTION__, __LINE__,i );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval==SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forwarded to PORT5", hsa.dp, PM_PORT_5, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("L2 should be translated", hsa.l2tr, TRUE, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("L34 should be translated", hsa.l34tr, TRUE, __FUNCTION__, __LINE__,i );
		
		/* --------------------------------------------------------------- */
		/* Expect output packet */
		toPort = PM_PORT_ALL;
		pktLen = _PKT_LEN;
		retval = virtualMacOutput( &toPort, pktBuf, &pktLen );
		ASSERT( retval==SUCCESS );
		RESET_CONF(&conf);    
		conf.pktType					= _PKT_TYPE_IP;					/* ip packet */
		conf.l2Flag						= 0;
		conf.ErrFlag					= 0;
		conf.conf_Flags					|= (CSUM_IP|CSUM_L4);
		conf.ip.protocol                = 79;
		conf.ip.id						= 0xa;
		conf.ip.mf						= 0;
		conf.ip.offset					= 0;
		conf.ip.ttl						= 1;		/* ttl */
		conf.payload.content			= NULL;
		conf.payload.length				= 0;
		conf.conf_vlanIdx				= 2;
		conf.conf_pppoeIdx				= 0;
		strtoip(&conf.conf_sip,WAN1_ISP_PUBLIC_IP);
		strtoip(&conf.conf_dip,"222.222.222.222");
		stringToConfMac(conf.conf_dmac,WAN0_ISP_MAC);
		stringToConfMac(conf.conf_smac,GW_WAN0_MAC);
		pktExpectLen = pktGen(&conf,pktExpect);				/* generate pkt in buff */
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);

		/* pump remain packet */
		retval = virtualMacPumpPackets (PM_PORT_ALL );
		ASSERT( retval==SUCCESS );

		/* --------------------------------------------------------------- */
		/* 3. Inbound unsupported protocol and not NAT, expect to CPU */
		RESET_CONF(&conf);    
		conf.pktType					= _PKT_TYPE_IP;					/* ip packet */
		conf.l2Flag						= 0;
		conf.ErrFlag					= 0;
		conf.conf_Flags					|= (CSUM_IP|CSUM_L4);
		conf.ip.protocol                = 80;
		conf.ip.id						= 0xa;
		conf.ip.mf						= 0;
		conf.ip.offset					= 0;
		conf.ip.ttl						= 1;		/* igmp ttl */
		conf.payload.content			= NULL;
		conf.payload.length				= 0;
		conf.conf_vlanIdx				= 2;
		conf.conf_pppoeIdx				= 0;
		strtoip(&conf.conf_sip,"222.222.222.222");
		strtoip(&conf.conf_dip,WAN0_ISP_PUBLIC_IP);
		stringToConfMac(conf.conf_dmac,GW_WAN0_MAC);
		stringToConfMac(conf.conf_smac,HOST5_MAC);
		pktLen = pktGen(&conf,pktBuf);				/* generate pkt in buff */
		fromPort = PN_PORT5;
		retval = virtualMacInput(fromPort,pktBuf, pktLen+4);
		ASSERT( retval == SUCCESS );
		
		/* --------------------------------------------------------------- */
		/* Compare HSB/HSA */
		retval = virtualMacGetHsb( &hsb );
		ASSERT( retval==SUCCESS );
		IS_EQUAL_INT_DETAIL("hsb.type should be IP type.", hsb.type, HSB_TYPE_IP, __FUNCTION__, __LINE__,i );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval==SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be trapped to CPU", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );

		/* pump remain packet */
		retval = virtualMacPumpPackets (PM_PORT_ALL );
		ASSERT( retval==SUCCESS );

		/* --------------------------------------------------------------- */
		/*  4. Inbound unsupported protocol but is to NAT host(NE), expect to Port 1 (Host1). */
		RESET_CONF(&conf);    
		conf.pktType					= _PKT_TYPE_IP;					/* ip packet */
		conf.l2Flag						= 0;
		conf.ErrFlag					= 0;
		conf.conf_Flags					|= (CSUM_IP|CSUM_L4);
		conf.ip.protocol                = 81;
		conf.ip.id						= 0xa;
		conf.ip.mf						= 0;
		conf.ip.offset					= 0;
		conf.ip.ttl						= 2;		/* ttl */
		conf.payload.content			= NULL;
		conf.payload.length				= 0;
		conf.conf_vlanIdx				= 2;
		conf.conf_pppoeIdx				= 0;
		strtoip(&conf.conf_sip,"222.222.222.222");
		strtoip(&conf.conf_dip,WAN1_ISP_PUBLIC_IP);
		stringToConfMac(conf.conf_dmac,GW_WAN1_MAC);
		stringToConfMac(conf.conf_smac,HOST4_MAC);
		pktLen = pktGen(&conf,pktBuf);				/* generate pkt in buff */
		fromPort = PN_PORT4;
		retval = virtualMacInput(fromPort,pktBuf, pktLen+4);
		ASSERT( retval == SUCCESS );

		/* --------------------------------------------------------------- */
		/* Compare HSB/HSA */
		retval = virtualMacGetHsb( &hsb );
		ASSERT( retval==SUCCESS );
		IS_EQUAL_INT_DETAIL("hsb.type should be IP type.", hsb.type, HSB_TYPE_IP, __FUNCTION__, __LINE__,i );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval==SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forwarded to PORT1", hsa.dp, PM_PORT_1, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("L2 should be translated", hsa.l2tr, TRUE, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("L34 should be translated", hsa.l34tr, TRUE, __FUNCTION__, __LINE__,i );
		
		/* --------------------------------------------------------------- */
		/* Expect output packet */
		toPort = PM_PORT_ALL;
		pktLen = _PKT_LEN;
		retval = virtualMacOutput( &toPort, pktBuf, &pktLen );
		ASSERT( retval==SUCCESS );
		RESET_CONF(&conf);    
		conf.pktType					= _PKT_TYPE_IP;					/* ip packet */
		conf.l2Flag						= 0;
		conf.ErrFlag					= 0;
		conf.conf_Flags					|= (CSUM_IP|CSUM_L4);
		conf.ip.protocol                = 81;
		conf.ip.id						= 0xa;
		conf.ip.mf						= 0;
		conf.ip.offset					= 0;
		conf.ip.ttl						= 1;		/* ttl */
		conf.payload.content			= NULL;
		conf.payload.length				= 0;
		conf.conf_vlanIdx				= 2;
		conf.conf_pppoeIdx				= 0;
		strtoip(&conf.conf_sip,"222.222.222.222");
		strtoip(&conf.conf_dip,HOST1_IP);
		stringToConfMac(conf.conf_dmac,HOST1_MAC);
		stringToConfMac(conf.conf_smac,GW_LAN1_MAC);
		pktExpectLen = pktGen(&conf,pktExpect);				/* generate pkt in buff */
		PKT_CMP(pktBuf,pktExpect,pktLen,pktExpectLen, __FUNCTION__,__LINE__,i);

		/* pump remain packet */
		retval = virtualMacPumpPackets (PM_PORT_ALL );
		ASSERT( retval==SUCCESS );

	}
	retval = SUCCESS;	
	return retval;
}


	



