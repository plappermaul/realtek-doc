
/*
* Copyright c                  Realtek Semiconductor Corporation, 2005
* All rights reserved.
* 
* Program : Model code for packet parser
* Abstract : 
* Author : Tony Tzong-yn Su (tysu@realtek.com.tw)               
* $Id: parserTest.c,v 1.73 2007-02-13 11:55:31 yjlou Exp $
*/

#include "rtl_types.h"
#include "asicRegs.h"
#ifdef CONFIG_RTL865XC
#include "rtl865xC_tblAsicDrv.h"
#include "rtl865xc_asicregs.h"
#else
#include "rtl8651_tblAsicDrv.h"
#include "rtl865xb_asicregs.h"
#endif
#include "gdma.h"
#include "kmp.h"
#include "gdmaModel.h"
#include "virtualMac.h"
#include "rtl_glue.h"
#include "rtl_utils.h"
#include "drvTest.h"
#include "icTest.h"
#include "icModel.h"
#include "swNic2.h"
#include "nicTest.h"
#include "vsv_conn.h"
#include "parserTest.h"
#include "l2Test.h"
#define RTL8651_TBLDRV_LOCAL_H
#include "rtl8651_aclLocal.h"
#include "rtl8651_layer2local.h"
#include "rtl8651_tblDrvFwd.h"


#define CHECK(x) do{ \
	if((x)==FAILED)  \
	{ rtlglue_printf("FAILED at %s:%d\n",__FUNCTION__,__LINE__); \
	return FAILED; } \
} while(0);





#include "l2Test.h"
#include "l34Test.h"

#define MAX_QUEUE_ENTRY 16

#define VID_0 0
#define WAN0_VID 3099
#define GW_WAN0_MAC "00-00-00-00-88-88"
#define WAN1_VID 9
#define GW_WAN1_MAC "00-00-00-00-88-89"
#define GW_WAN1_SUBNET_MASK "255.255.255.248"
#define LAN0_VID 0x3C7
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
#define WAN0_ISP_PUBLIC_IP "192.168.4.1"
#define WAN3_ISP_PUBLIC_IP "192.168.4.3"
/* isp 1 */
#define WAN1_ISP_PUBLIC_IP 	"192.168.3.1"
#define GW_WAN1_IP 			"192.168.3.1"
#define GW_WAN1_SUBNET_MASK "255.255.255.248"

/* wan 0 hosts */
#define HOST5_MAC "00-00-00-00-00-10"
#define HOST5_IP "192.168.3.5"
/* wan 1 hosts */
#define HOST4_MAC "00-00-00-00-00-20"
#define HOST4_IP "192.168.3.1"

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



static uint8 port_number_of_host[3][MAX_PORT_NUMBER] = {{3,5,2,0,4,1},{4,3,2,1,0,5},{1,0,3,2,4,5}};

static int32  RoutingConfig( void )
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
	uint8 *hp;
	int32 retval;

	hp = port_number_of_host[0];

	rtl8651_clearRegister();
	WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);	
	rtl8651_clearAsicAllTable();
	rtl8651_clearAsicPvid();
	rtl8651_setAsicPvid(hp[5],WAN0_VID);  /*host 5 pvid 0*/
	rtl8651_setAsicPvid(hp[4],9);   /*host 4 pvid 1*/
	rtl8651_setAsicPvid(hp[3],LAN0_VID);  /*host 3 pvid 2*/
	rtl8651_setAsicPvid(hp[2],11);  /*host 2 pvid 3*/
	rtl8651_setAsicPvid(hp[1],11);  /*host 1 pvid 3*/
	rtl8651_setAsicPvid(hp[6],11);  /*host 6 pvid 3*/
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
	retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	ASSERT( retval==SUCCESS );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP1)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP2)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP3)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP4)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP5)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP6)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP7)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP8)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = vlant.untagPortMask= 0x3f;/*(1<<hp[5]); */
	retval = rtl8651_setAsicVlan(intf.vid,&vlant);
	ASSERT( retval==SUCCESS );

	 /* ingress filter gateway mac */
	 memset((void*)&aclt, 0, sizeof(aclt));
	aclt.ruleType_ = RTL8651_ACL_MAC;
	aclt.actionType_ = RTL8651_ACL_PERMIT;
	aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
	retval = rtl8651_setAsicAclRule(0, &aclt);
	ASSERT( retval==SUCCESS );
	 /* wan 1 */
	strtomac(&intf.macAddr, GW_WAN1_MAC);
	intf.vid = VID_0;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	ASSERT( retval==SUCCESS );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP1)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP2)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP3)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP4)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP5)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP6)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP7)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP8)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = vlant.untagPortMask = (1<<hp[4])|(1<<hp[0]);
	retval = rtl8651_setAsicVlan(intf.vid,&vlant);
	ASSERT( retval==SUCCESS );
	/* ingress filter gateway mac */
	aclt.ruleType_ = RTL8651_ACL_MAC;
	memcpy(aclt.srcMac_.octet, intf.macAddr.octet, 6);
	aclt.actionType_ = RTL8651_ACL_CPU;
	aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
	retval = rtl8651_setAsicAclRule(4, &aclt);
	ASSERT( retval==SUCCESS );
	/* lan 0 */
	strtomac(&intf.macAddr, GW_LAN0_MAC);
	intf.vid = LAN0_VID;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	ASSERT( retval==SUCCESS );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP1)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP2)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP3)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP4)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP5)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP6)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP7)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP8)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = 0x3f;/*(1<<hp[3]);*/
	vlant.untagPortMask = 0; /*tagged */
	retval = rtl8651_setAsicVlan(intf.vid,&vlant);
	ASSERT( retval==SUCCESS );
	/* ingress filter gateway mac */
	aclt.ruleType_ = RTL8651_ACL_MAC;
	memcpy(aclt.srcMac_.octet, intf.macAddr.octet, 6);
	aclt.actionType_ = RTL8651_ACL_CPU;
	aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
	retval = rtl8651_setAsicAclRule(8, &aclt);
	ASSERT( retval==SUCCESS );
	/* lan 1 */
	strtomac(&intf.macAddr, GW_LAN1_MAC);
	intf.vid = LAN1_VID;
	intf.inAclStart = 0;
	intf.inAclEnd = 0;
	intf.outAclStart = intf.outAclEnd = 0;
	retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
	ASSERT( retval==SUCCESS );
	
	WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP1)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP2)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP3)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP4)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP5)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP6)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP7)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP8)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = vlant.untagPortMask = (1<<hp[2])|(1<<hp[1])|(1<<hp[6]);
	retval = rtl8651_setAsicVlan(intf.vid,&vlant);
	ASSERT( retval==SUCCESS );
	 /* ingress filter gateway mac */
	aclt.ruleType_ = RTL8651_ACL_MAC;
	memcpy(aclt.srcMac_.octet, intf.macAddr.octet, 6);
	aclt.actionType_ = RTL8651_ACL_CPU;
	aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
	retval = rtl8651_setAsicAclRule(12, &aclt);    
	ASSERT( retval==SUCCESS );
	/* config l2 table */
	bzero((void*) &l2t, sizeof(l2t));
	/* wan 0 isp modem */
	strtomac(&l2t.macAddr, WAN0_ISP_MAC);
	l2t.memberPortMask = (1<<hp[5]);
	l2t.isStatic = 1;
	l2t.nhFlag=TRUE;
	retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr(WAN0_ISP_MAC), 0, &l2t);
	ASSERT( retval==SUCCESS );
	/* wan 1 host */
	strtomac(&l2t.macAddr, HOST4_MAC);
	l2t.memberPortMask = (1<<hp[4]);
	retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST4_MAC), 0, &l2t);
	ASSERT( retval==SUCCESS );

	/* wan 2 host */
	strtomac(&l2t.macAddr, HOST5_MAC);
	l2t.memberPortMask =(1<<hp[5]);
	retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST5_MAC), 0, &l2t);
	ASSERT( retval==SUCCESS );

	
	/* lan 0 host */
	strtomac(&l2t.macAddr, HOST3_MAC);
	l2t.memberPortMask = (1<<hp[3]);
	retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST3_MAC), 0, &l2t);
	ASSERT( retval==SUCCESS );
	/* lan 1 host 1 */
	strtomac(&l2t.macAddr, HOST2_MAC);
	l2t.memberPortMask = (1<<hp[2]);
	retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST2_MAC), 0, &l2t);
	ASSERT( retval==SUCCESS );
	/* lan 1 host 0 */
	strtomac(&l2t.macAddr, HOST1_MAC);
	l2t.memberPortMask = (1<<hp[1]);
	retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST1_MAC), 0, &l2t);
	ASSERT( retval==SUCCESS );
	/* lan 1 host 5 */
	strtomac(&l2t.macAddr, HOST6_MAC);
	l2t.memberPortMask = (1<<hp[6]);
	retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST6_MAC), 0, &l2t);
	ASSERT( retval==SUCCESS );

	    
	/* config pppoe table */
	bzero((void*) &pppoet, sizeof(pppoet));
	pppoet.sessionId = WAN0_ISP_PPPOE_SID;
	retval = rtl8651_setAsicPppoe(0, &pppoet);
	ASSERT( retval==SUCCESS );
	/* config arp table */
	bzero((void*) &arpt, sizeof(arpt));
	/* wan 1 host */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	strtoip(&ip32, HOST4_IP);
	retval = rtl8651_setAsicArp(ip32 & 0x7, &arpt);
	ASSERT( retval==SUCCESS );

	/* lan 0 host */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST3_MAC);
	strtoip(&ip32, HOST3_IP);
	retval = rtl8651_setAsicArp(8 + (ip32 & 0x7), &arpt);
	ASSERT( retval==SUCCESS );
    
	/* lan 1 host 1 */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST2_MAC);
	strtoip(&ip32, HOST2_IP);
	rtl8651_setAsicArp(16 + (ip32 & 0xff), &arpt);	
	/* lan 1 host 0 */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST1_MAC);
	strtoip(&ip32, HOST1_IP);
	retval = rtl8651_setAsicArp(16 + (ip32 & 0xff), &arpt);
	ASSERT( retval==SUCCESS );
	 /* lan 1 host 5 */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST6_MAC);
	strtoip(&ip32, HOST6_IP);
	retval = rtl8651_setAsicArp(16 + (ip32 & 0xff), &arpt);
	ASSERT( retval==SUCCESS );
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
 	retval = rtl8651_setAsicRouting(0, &routet);
	ASSERT( retval==SUCCESS );
	
	/* route to lan 0: use arp table [8:15] */
	strtoip(&routet.ipAddr, GW_LAN0_IP);
	strtoip(&routet.ipMask, GW_LAN0_SUBNET_MASK);
	routet.process = PROCESS_INDIRECT;
	routet.vidx = rtl865xc_netIfIndex(LAN0_VID);
	routet.arpStart = 1;
	routet.arpEnd = 1;
	routet.internal=1;
	retval = rtl8651_setAsicRouting(1, &routet);
	ASSERT( retval==SUCCESS );
	 /* route to lan 1: use arp table [16:23] */
	 strtoip(&routet.ipAddr, GW_LAN1_IP);
	strtoip(&routet.ipMask, GW_LAN1_SUBNET_MASK);
	routet.process = PROCESS_INDIRECT;
	routet.vidx = rtl865xc_netIfIndex(LAN1_VID);
	routet.arpStart = 2;
	routet.arpEnd = 2;
	routet.internal=1;
	retval = rtl8651_setAsicRouting(6, &routet);
	ASSERT( retval==SUCCESS );
	/* default route to wan 0: direct to modem */
	 bzero((void*) &routet, sizeof(routet));
	 strtoip(&routet.ipAddr, "0.0.0.0");
	strtoip(&routet.ipMask, "0.0.0.0");
	routet.process = PROCESS_NXT_HOP;
	routet.vidx = rtl865xc_netIfIndex(WAN0_VID);
	routet.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
#if 1
	routet.nhStart = 0;
	routet.nhNum = 2; /*index 4~5*/
	routet.nhNxt = 0;
	routet.nhAlgo = 2; /*per-source */
	routet.ipDomain = 0;
	routet.internal=1;
#else
	routet.nhStart = 30;
	routet.nhNum = 2; /*index 4~5 */
	routet.nhNxt = 0;
	routet.nhAlgo = 2; /*per-source */
	routet.ipDomain = 0;
	routet.internal=1;

#endif
	retval = rtl8651_setAsicRouting(7, &routet);
	ASSERT( retval==SUCCESS );
	 /* config next hop table */
	 bzero((void*) &nxthpt, sizeof(nxthpt));
	 /* wan 0: load balance traffic through isp0 & isp1 as 3:1 */
	/* isp 0 occupies 3 entries */
	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0; /*WAN0_ISP0_PPPOE_SID */
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0; /*WAN0_ISP0_NPE_IP */
	nxthpt.isPppoe = 0;
	retval = rtl8651_setAsicNextHopTable(0, &nxthpt);       
	ASSERT( retval==SUCCESS );

	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0; /*WAN0_ISP0_PPPOE_SID */
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0; /*WAN0_ISP0_NPE_IP */
	nxthpt.isPppoe = 0;
	retval = rtl8651_setAsicNextHopTable(1, &nxthpt);       
	ASSERT( retval==SUCCESS );


	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 1; /* WAN0_ISP0_PPPOE_SID */
	nxthpt.dvid =1;/*  rtl865xc_netIfIndex(WAN1_VID); */
	nxthpt.extIntIpIdx = 0; /* WAN0_ISP0_NPE_IP */
	nxthpt.isPppoe = 1;
	retval = rtl8651_setAsicNextHopTable(30, &nxthpt);       
	ASSERT( retval==SUCCESS );

	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 1;/*0; WAN0_ISP0_PPPOE_SID */
	nxthpt.dvid =0;/* rtl865xc_netIfIndex(WAN1_VID); */
	nxthpt.extIntIpIdx = 0; /* WAN0_ISP0_NPE_IP */
	nxthpt.isPppoe = 0;
	retval = rtl8651_setAsicNextHopTable(31, &nxthpt);       
	ASSERT( retval==SUCCESS );





	return 0;
	
}


void parserInit(int icType)
{


/*	rtl8651_clearAsicAllTable();  */
	WRITE_MEM32(CCR,L4ChkSErrAllow|L3ChkSErrAllow|L2CRCErrAllow ); /* skip CRC error */
	/* WRITE_MEM32(TMCR,0xfcfc0000); virtualMacInit() will handle */
	WRITE_MEM32(SSIR,TRXRDY); /*  TX, RX, Ready */
	WRITE_MEM32(PCRP0,EnForceMode|ForceLink|ForceSpeed1000M|ForceDuplex|STP_PortST_FORWARDING|AcptMaxLen_1536|EnablePHYIf);
	WRITE_MEM32(PCRP1,EnForceMode|ForceLink|ForceSpeed1000M|ForceDuplex|STP_PortST_FORWARDING|AcptMaxLen_1536|EnablePHYIf);
	WRITE_MEM32(PCRP2,EnForceMode|ForceLink|ForceSpeed1000M|ForceDuplex|STP_PortST_FORWARDING|AcptMaxLen_1536|EnablePHYIf);
	WRITE_MEM32(PCRP3,EnForceMode|ForceLink|ForceSpeed1000M|ForceDuplex|STP_PortST_FORWARDING|AcptMaxLen_1536|EnablePHYIf);
	WRITE_MEM32(PCRP4,EnForceMode|ForceLink|ForceSpeed1000M|ForceDuplex|STP_PortST_FORWARDING|AcptMaxLen_1536|EnablePHYIf);
	WRITE_MEM32(PCRP5,EnForceMode|ForceLink|ForceSpeed1000M|ForceDuplex|STP_PortST_FORWARDING|AcptMaxLen_1536|EnablePHYIf);

	{ /* delay 1/5 sec for link REALLY up (only IC need delay) */
		virtualMacWaitCycle( 250000*200 ); /* about 200ms */
	}
	

#if 0
	WRITE_MEM32(GMIITM_RXR0,0);
	WRITE_MEM32(GMIITM_RXR0,0);	
	WRITE_MEM32(GMIITM_RXR0,0);
	WRITE_MEM32(GMIITM_RXR0,0);
	WRITE_MEM32(GMIITM_RXR0,0);
	WRITE_MEM32(GMIITM_RXR0,0);
	WRITE_MEM32(GMIITM_RXR0,0);
	WRITE_MEM32(GMIITM_RXR0,0);
#endif	


}


void dumpHSB(hsb_param_t *hsb)
{
	rtlglue_printf("hsb->spa=0x%x;\n",hsb->spa);  
	rtlglue_printf("hsb->len=%d;\n",hsb->len);	
	rtlglue_printf("hsb->vid=0x%x;\n",hsb->vid);
	rtlglue_printf("hsb->tagif=0x%x;\n",hsb->tagif);
	rtlglue_printf("hsb->pppoeif=0x%x;\n",hsb->pppoeif);
	rtlglue_printf("hsb->sip=0x%x;\n",hsb->sip);
	rtlglue_printf("hsb->sprt=0x%x;\n",hsb->sprt);
	rtlglue_printf("hsb->dip=0x%x;\n",hsb->dip);	
	rtlglue_printf("hsb->dprt=0x%x;\n",hsb->dprt);
	rtlglue_printf("hsb->ipptl=0x%x;\n",hsb->ipptl);
	rtlglue_printf("hsb->ipfg=0x%x;\n",hsb->ipfg);
	rtlglue_printf("hsb->iptos=0x%x;\n",hsb->iptos);
	rtlglue_printf("hsb->tcpfg=0x%x;\n",hsb->tcpfg);
	rtlglue_printf("hsb->type=0x%x;\n",hsb->type);
	rtlglue_printf("hsb->patmatch=0x%x;\n",hsb->patmatch);
	rtlglue_printf("hsb->ethtype=0x%x;\n",hsb->ethtype);
	rtlglue_printf("hsb->da0=0x%x; ",hsb->da[0]);
	rtlglue_printf("hsb->da1=0x%x; ",hsb->da[1]);	
	rtlglue_printf("hsb->da2=0x%x; ",hsb->da[2]);	
	rtlglue_printf("hsb->da3=0x%x; ",hsb->da[3]);	
	rtlglue_printf("hsb->da4=0x%x; ",hsb->da[4]);	
	rtlglue_printf("hsb->da5=0x%x;\n",hsb->da[5]);	
	rtlglue_printf("hsb->sa0=0x%x; ",hsb->sa[0]);
	rtlglue_printf("hsb->sa1=0x%x; ",hsb->sa[1]);	
	rtlglue_printf("hsb->sa2=0x%x; ",hsb->sa[2]);	
	rtlglue_printf("hsb->sa3=0x%x; ",hsb->sa[3]);	
	rtlglue_printf("hsb->sa4=0x%x; ",hsb->sa[4]);	
	rtlglue_printf("hsb->sa5=0x%x;\n",hsb->sa[5]);	
	rtlglue_printf("hsb->hiprior=0x%x;\n",hsb->hiprior);		
	rtlglue_printf("hsb->snap=0x%x;\n",hsb->snap);
	rtlglue_printf("hsb->udpnocs=0x%x;\n",hsb->udpnocs);
	rtlglue_printf("hsb->ttlst=0x%x;\n",hsb->ttlst);
	rtlglue_printf("hsb->dirtx=0x%x;\n",hsb->dirtx);
	rtlglue_printf("hsb->l3csok=0x%x;\n",hsb->l3csok);	
	rtlglue_printf("hsb->l4csok=0x%x;\n",hsb->l4csok);	
	rtlglue_printf("hsb->ipfo0_n=0x%x;\n",hsb->ipfo0_n);		
	rtlglue_printf("hsb->llcothr=0x%x;\n",hsb->llcothr);	
	rtlglue_printf("hsb->urlmch=0x%x;\n",hsb->urlmch);		
	rtlglue_printf("hsb->extspa=0x%x;\n",hsb->extspa);		
	rtlglue_printf("hsb->extl2=0x%x;\n",hsb->extl2);	
	rtlglue_printf("hsb->linkid=0x%x;\n",hsb->linkid);		
	rtlglue_printf("hsb->pppoeid=0x%x;\n",hsb->pppoeid);			
}

int compareHsb(hsb_param_t *hsb1,hsb_param_t *hsb2)
{
	int ret=SUCCESS;
	if(hsb1->spa!=hsb2->spa)
		{rtlglue_printf("hsb1->spa=0x%x; hsb2->spa=0x%x;\n",hsb1->spa,hsb2->spa);  ret=FAILED;}
	if(hsb1->len!=hsb2->len)	
		{rtlglue_printf("hsb1->len=%d; hsb2->len=%d;\n;",hsb1->len,hsb2->len);  ret=FAILED;}	
	if(hsb1->vid!=hsb2->vid)	
		{rtlglue_printf("hsb1->vid=0x%x; hsb2->vid=0x%x;\n",hsb1->vid,hsb2->vid);  ret=FAILED;}
	if(hsb1->tagif!=hsb2->tagif)		
		{rtlglue_printf("hsb1->tagif=0x%x; hsb2->tagif=0x%x;\n",hsb1->tagif,hsb2->tagif);  ret=FAILED;}
	if(hsb1->pppoeif!=hsb2->pppoeif)	
		{rtlglue_printf("hsb1->pppoeif=0x%x; hsb2->pppoeif=0x%x;\n",hsb1->pppoeif,hsb2->pppoeif);  ret=FAILED;}
	if(hsb1->sip!=hsb2->sip)	
		{rtlglue_printf("hsb1->sip=0x%x; hsb2->sip=0x%x;\n",hsb1->sip,hsb2->sip);  ret=FAILED;}
	if(hsb1->sprt!=hsb2->sprt)	
		{rtlglue_printf("hsb1->sprt=0x%x; hsb2->sprt=0x%x;\n",hsb1->sprt,hsb2->sprt);  ret=FAILED;}
	if(hsb1->dip!=hsb2->dip)	
		{rtlglue_printf("hsb1->dip=0x%x; hsb2->dip=0x%x;\n",hsb1->dip,hsb2->dip);  ret=FAILED;}	
	if(hsb1->dprt!=hsb2->dprt)	
		{rtlglue_printf("hsb1->dprt=0x%x; hsb2->dprt=0x%x;\n",hsb1->dprt,hsb2->dprt);  ret=FAILED;}
	if(hsb1->ipptl!=hsb2->ipptl)	
		{rtlglue_printf("hsb1->ipptl=0x%x; hsb2->ipptl=0x%x;\n",hsb1->ipptl,hsb2->ipptl);  ret=FAILED;}
	if(hsb1->ipfg!=hsb2->ipfg)	
		{rtlglue_printf("hsb1->ipfg=0x%x; hsb2->ipfg=0x%x;\n",hsb1->ipfg,hsb2->ipfg);  ret=FAILED;}
	if(hsb1->iptos!=hsb2->iptos)	
		{rtlglue_printf("hsb1->iptos=0x%x; hsb2->iptos=0x%x;\n",hsb1->iptos,hsb2->iptos);  ret=FAILED;}
	if(hsb1->tcpfg!=hsb2->tcpfg)	
		{rtlglue_printf("hsb1->tcpfg=0x%x; hsb2->tcpfg=0x%x;\n",hsb1->tcpfg,hsb2->tcpfg);  ret=FAILED;}
	if(hsb1->type!=hsb2->type)	
		{rtlglue_printf("hsb1->type=0x%x; hsb2->type=0x%x;\n",hsb1->type,hsb2->type);  ret=FAILED;}
	if(hsb1->patmatch!=hsb2->patmatch)
		{rtlglue_printf("hsb1->patmatch=0x%x; hsb2->patmatch=0x%x;\n",hsb1->patmatch,hsb2->patmatch);  ret=FAILED;}
	if(hsb1->ethtype!=hsb2->ethtype)
		{rtlglue_printf("hsb1->ethtype=0x%x; hsb2->ethtype=0x%x;\n",hsb1->ethtype,hsb2->ethtype);  ret=FAILED;}
	
	if(memcmp(&hsb1->da[0],&hsb2->da[0],6)!=0)
	{		
		rtlglue_printf("hsb1->da[]=%02x:%02x:%02x:%02x:%02x:%02x; hsb2->da[]=%02x:%02x:%02x:%02x:%02x:%02x;\n"
			,hsb1->da[0],hsb1->da[1],hsb1->da[2],hsb1->da[3],hsb1->da[4],hsb1->da[5]
			,hsb2->da[0],hsb2->da[1],hsb2->da[2],hsb2->da[3],hsb2->da[4],hsb2->da[5]);
		ret=FAILED;
	}

	if(memcmp(&hsb1->sa[0],&hsb2->sa[0],6)!=0)
	{		
		rtlglue_printf("hsb1->sa[]=%02x:%02x:%02x:%02x:%02x:%02x; hsb2->sa[]=%02x:%02x:%02x:%02x:%02x:%02x;\n"
			,hsb1->sa[0],hsb1->sa[1],hsb1->sa[2],hsb1->sa[3],hsb1->sa[4],hsb1->sa[5]
			,hsb2->sa[0],hsb2->sa[1],hsb2->sa[2],hsb2->sa[3],hsb2->sa[4],hsb2->sa[5]);
		ret=FAILED;
	}

	if(hsb1->hiprior!=hsb2->hiprior)	
		{rtlglue_printf("hsb1->hiprior=0x%x; hsb2->hiprior=0x%x;\n",hsb1->hiprior,hsb2->hiprior);  ret=FAILED;}
	if(hsb1->snap!=hsb2->snap)	
		{rtlglue_printf("hsb1->snap=0x%x; hsb2->snap=0x%x;\n",hsb1->snap,hsb2->snap);  ret=FAILED;}
	if(hsb1->udpnocs!=hsb2->udpnocs)	
		{rtlglue_printf("hsb1->udpnocs=0x%x; hsb2->udpnocs=0x%x;\n",hsb1->udpnocs,hsb2->udpnocs);  ret=FAILED;}
	if(hsb1->ttlst!=hsb2->ttlst)	
		{rtlglue_printf("hsb1->ttlst=0x%x; hsb2->ttlst=0x%x;\n",hsb1->ttlst,hsb2->ttlst);  ret=FAILED;}
	if(hsb1->dirtx!=hsb2->dirtx)	
		{rtlglue_printf("hsb1->dirtx=0x%x; hsb2->dirtx=0x%x;\n",hsb1->dirtx,hsb2->dirtx);  ret=FAILED;}
	if(hsb1->l3csok!=hsb2->l3csok)	
		{rtlglue_printf("hsb1->l3csok=0x%x; hsb2->l3csok=0x%x;\n",hsb1->l3csok,hsb2->l3csok);  ret=FAILED;}	
	if(hsb1->l4csok!=hsb2->l4csok)	
		{rtlglue_printf("hsb1->l4csok=0x%x; hsb2->l4csok=0x%x;\n",hsb1->l4csok,hsb2->l4csok);  ret=FAILED;}	
	if(hsb1->ipfo0_n!=hsb2->ipfo0_n)	
		{rtlglue_printf("hsb1->ipfo0_n=0x%x; hsb2->ipfo0_n=0x%x;\n",hsb1->ipfo0_n,hsb2->ipfo0_n);  ret=FAILED;}		
	if(hsb1->llcothr!=hsb2->llcothr)	
		{rtlglue_printf("hsb1->llcothr=0x%x; hsb2->llcothr=0x%x;\n",hsb1->llcothr,hsb2->llcothr);  ret=FAILED;}	
	if(hsb1->urlmch!=hsb2->urlmch)	
		{rtlglue_printf("hsb1->urlmch=0x%x; hsb2->urlmch=0x%x;\n",hsb1->urlmch,hsb2->urlmch);  ret=FAILED;}		
	if(hsb1->extspa!=hsb2->extspa)	
		{rtlglue_printf("hsb1->extspa=0x%x; hsb2->extspa=0x%x;\n",hsb1->extspa,hsb2->extspa);  ret=FAILED;}		
	if(hsb1->extl2!=hsb2->extl2)	
		{rtlglue_printf("hsb1->extl2=0x%x; hsb2->extl2=0x%x;\n",hsb1->extl2,hsb2->extl2);  ret=FAILED;}	
	if(hsb1->linkid!=hsb2->linkid)	
		{rtlglue_printf("hsb1->linkid=0x%x;hsb2->linkid=0x%x;\n",hsb1->linkid,hsb2->linkid);  ret=FAILED;}		
	if(hsb1->pppoeid!=hsb2->pppoeid)	
		{rtlglue_printf("hsb1->pppoeid=0x%x;hsb2->pppoeid=0x%x;\n",hsb1->pppoeid,hsb2->pppoeid);  ret=FAILED;}
	return ret;
	
}


struct rtl_mBuf * _genPktHdr(void *buffer, uint32 id, uint32 bufsize,uint32 datalen, uint16 align){

	struct rtl_mBuf *m=NULL;
	struct rtl_pktHdr *ph=NULL;	

	m=(struct rtl_mBuf *)rtlglue_malloc(sizeof(struct rtl_mBuf));
	ph =(struct rtl_pktHdr *)rtlglue_malloc(sizeof(struct rtl_pktHdr));
	memset(m,0,sizeof(struct rtl_mBuf));
	memset(ph,0,sizeof(struct rtl_pktHdr));

	ph->ph_mbuf = m;
	ph->ph_len = (datalen);
	ph->ph_flags= (PKTHDR_USED);
	/* Louis: what is this ?  *((uint16 *)((uint8 *)ph)+8)=0;*/

	ph->ph_iphdrOffset=0;
	ph->ph_pppeTagged = 0;
	ph->ph_LLCTagged = 0;
	ph->ph_vlanTagged = 0;

	ph->ph_rxdesc = PH_RXDESC_INDRV;
	ph->ph_rxPkthdrDescIdx = PH_RXPKTHDRDESC_INDRV;
	m->m_next = NULL;
	m->m_pkthdr = ph;
	m->m_len=(datalen);
	m->m_flags=(MBUF_USED|MBUF_EXT|MBUF_PKTHDR);
	m->m_data = ((uint8 *)buffer)+align;
	m->m_extbuf=buffer;
	m->m_extClusterId = (id);
	m->m_extsize= (bufsize);

	return m;
 }



	char l4data[] = {

	/* The following packet is a HTTP Request;
	===================================================================================
	GET / HTTP/1.1
	Accept: * / *
	Accept-Language: zh-tw
	Accept-Encoding: gzip, deflate
	User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; .NET CLR 1.1.4322)
	Host: www.google.com.tw
	Connection: Keep-Alive
	Cookie: PREF=ID=0c6c50c80799b799:TB=2:LD=zh-TW:TM=1116899852:LM=1119873714:GM=1:S=lPKPB0qpAAJvXssT
	===================================================================================*/

	/* TCP header */
	0x10, 0x46, 0x00, 0x50, 0x92, 0x11, 
	0xa2, 0xdf, 0x4c, 0xa5, 0x5c, 0xdd, 0x50, 0x18,
	0x44, 0x70, 0xbc, 0x2c, 0x00, 0x00,

	/* Payload */
	0x47, 0x45, 0x54, 0x20, 0x2f, 0x20, 0x48, 0x54, 	0x54, 0x50, 0x2f, 0x31, 0x2e, 0x31, 0x0d, 0x0a, 	0x41, 0x63, 0x63, 0x65, 0x70, 0x74, 0x3a, 0x20, 	
	0x2a, 0x2f, 0x2a, 0x0d, 0x0a, 0x41, 0x63, 0x63, 	0x65, 0x70, 0x74, 0x2d, 0x4c, 0x61, 0x6e, 0x67, 	0x75, 0x61, 0x67, 0x65, 0x3a, 0x20, 0x7a, 0x68, 	
	0x2d, 0x74, 0x77, 0x0d, 0x0a, 0x41, 0x63, 0x63, 	0x65, 0x70, 0x74, 0x2d, 0x45, 0x6e, 0x63, 0x6f, 	0x64, 0x69, 0x6e, 0x67, 0x3a, 0x20, 0x67, 0x7a, 	
	0x69, 0x70, 0x2c, 0x20, 0x64, 0x65, 0x66, 0x6c, 	0x61, 0x74, 0x65, 0x0d, 0x0a, 0x55, 0x73, 0x65, 	0x72, 0x2d, 0x41, 0x67, 0x65, 0x6e, 0x74, 0x3a, 	
	0x20, 0x4d, 0x6f, 0x7a, 0x69, 0x6c, 0x6c, 0x61, 	0x2f, 0x34, 0x2e, 0x30, 0x20, 0x28, 0x63, 0x6f, 	0x6d, 0x70, 0x61, 0x74, 0x69, 0x62, 0x6c, 0x65, 	
	0x3b, 0x20, 0x4d, 0x53, 0x49, 0x45, 0x20, 0x36, 	0x2e, 0x30, 0x3b, 0x20, 0x57, 0x69, 0x6e, 0x64, 	0x6f, 0x77, 0x73, 0x20, 0x4e, 0x54, 0x20, 0x35, 	
	0x2e, 0x30, 0x3b, 0x20, 0x2e, 0x4e, 0x45, 0x54, 	0x20, 0x43, 0x4c, 0x52, 0x20, 0x31, 0x2e, 0x31, 	0x2e, 0x34, 0x33, 0x32, 0x32, 0x29, 0x0d, 0x0a, 	
	0x48, 0x6f, 0x73, 0x74, 0x3a, 0x20, 0x77, 0x77, 	0x77, 0x2e, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 	0x2e, 0x63, 0x6f, 0x6d, 0x2e, 0x74, 0x77, 0x0d, 	
	0x0a, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 	0x69, 0x6f, 0x6e, 0x3a, 0x20, 0x4b, 0x65, 0x65, 	0x70, 0x2d, 0x41, 0x6c, 0x69, 0x76, 0x65, 0x0d, 	
	0x0a, 0x43, 0x6f, 0x6f, 0x6b, 0x69, 0x65, 0x3a, 	0x20, 0x50, 0x52, 0x45, 0x46, 0x3d, 0x49, 0x44, 	0x3d, 0x30, 0x63, 0x36, 0x63, 0x35, 0x30, 0x63, 	
	0x38, 0x30, 0x37, 0x39, 0x39, 0x62, 0x37, 0x39, 	0x39, 0x3a, 0x54, 0x42, 0x3d, 0x32, 0x3a, 0x4c, 	0x44, 0x3d, 0x7a, 0x68, 0x2d, 0x54, 0x57, 0x3a, 	
	0x54, 0x4d, 0x3d, 0x31, 0x31, 0x31, 0x36, 0x38, 	0x39, 0x39, 0x38, 0x35, 0x32, 0x3a, 0x4c, 0x4d, 	0x3d, 0x31, 0x31, 0x31, 0x39, 0x38, 0x37, 0x33, 	
	0x37, 0x31, 0x34, 0x3a, 0x47, 0x4d, 0x3d, 0x31, 	0x3a, 0x53, 0x3d, 0x6c, 0x50, 0x4b, 0x50, 0x42, 	0x30, 0x71, 0x70, 0x41, 0x41, 0x4a, 0x76, 0x58, 	
	0x73, 0x73, 0x54, 0x0d, 0x0a, 0x0d, 0x0a 

	};


	char l4udpdata[] = {

	/* UDP header */
	0x10, 0x46,  /* source port */
	0x00, 0x50,  /* dest port */
	0x01, 0x3f,  /* length */
	0x00, 0x00,  /* checksum */

	/* Payload */
	0x47, 0x45, 0x54, 0x20, 0x2f, 0x20, 0x48, 0x54, 	0x54, 0x50, 0x2f, 0x31, 0x2e, 0x31, 0x0d, 0x0a, 	0x41, 0x63, 0x63, 0x65, 0x70, 0x74, 0x3a, 0x20, 	
	0x2a, 0x2f, 0x2a, 0x0d, 0x0a, 0x41, 0x63, 0x63, 	0x65, 0x70, 0x74, 0x2d, 0x4c, 0x61, 0x6e, 0x67, 	0x75, 0x61, 0x67, 0x65, 0x3a, 0x20, 0x7a, 0x68, 	
	0x2d, 0x74, 0x77, 0x0d, 0x0a, 0x41, 0x63, 0x63, 	0x65, 0x70, 0x74, 0x2d, 0x45, 0x6e, 0x63, 0x6f, 	0x64, 0x69, 0x6e, 0x67, 0x3a, 0x20, 0x67, 0x7a, 	
	0x69, 0x70, 0x2c, 0x20, 0x64, 0x65, 0x66, 0x6c, 	0x61, 0x74, 0x65, 0x0d, 0x0a, 0x55, 0x73, 0x65, 	0x72, 0x2d, 0x41, 0x67, 0x65, 0x6e, 0x74, 0x3a, 	
	0x20, 0x4d, 0x6f, 0x7a, 0x69, 0x6c, 0x6c, 0x61, 	0x2f, 0x34, 0x2e, 0x30, 0x20, 0x28, 0x63, 0x6f, 	0x6d, 0x70, 0x61, 0x74, 0x69, 0x62, 0x6c, 0x65, 	
	0x3b, 0x20, 0x4d, 0x53, 0x49, 0x45, 0x20, 0x36, 	0x2e, 0x30, 0x3b, 0x20, 0x57, 0x69, 0x6e, 0x64, 	0x6f, 0x77, 0x73, 0x20, 0x4e, 0x54, 0x20, 0x35, 	
	0x2e, 0x30, 0x3b, 0x20, 0x2e, 0x4e, 0x45, 0x54, 	0x20, 0x43, 0x4c, 0x52, 0x20, 0x31, 0x2e, 0x31, 	0x2e, 0x34, 0x33, 0x32, 0x32, 0x29, 0x0d, 0x0a, 	
	0x48, 0x6f, 0x73, 0x74, 0x3a, 0x20, 0x77, 0x77, 	0x77, 0x2e, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 	0x2e, 0x63, 0x6f, 0x6d, 0x2e, 0x74, 0x77, 0x0d, 	
	0x0a, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 	0x69, 0x6f, 0x6e, 0x3a, 0x20, 0x4b, 0x65, 0x65, 	0x70, 0x2d, 0x41, 0x6c, 0x69, 0x76, 0x65, 0x0d, 	
	0x0a, 0x43, 0x6f, 0x6f, 0x6b, 0x69, 0x65, 0x3a, 	0x20, 0x50, 0x52, 0x45, 0x46, 0x3d, 0x49, 0x44, 	0x3d, 0x30, 0x63, 0x36, 0x63, 0x35, 0x30, 0x63, 	
	0x38, 0x30, 0x37, 0x39, 0x39, 0x62, 0x37, 0x39, 	0x39, 0x3a, 0x54, 0x42, 0x3d, 0x32, 0x3a, 0x4c, 	0x44, 0x3d, 0x7a, 0x68, 0x2d, 0x54, 0x57, 0x3a, 	
	0x54, 0x4d, 0x3d, 0x31, 0x31, 0x31, 0x36, 0x38, 	0x39, 0x39, 0x38, 0x35, 0x32, 0x3a, 0x4c, 0x4d, 	0x3d, 0x31, 0x31, 0x31, 0x39, 0x38, 0x37, 0x33, 	
	0x37, 0x31, 0x34, 0x3a, 0x47, 0x4d, 0x3d, 0x31, 	0x3a, 0x53, 0x3d, 0x6c, 0x50, 0x4b, 0x50, 0x42, 	0x30, 0x71, 0x70, 0x41, 0x41, 0x4a, 0x76, 0x58, 	
	0x73, 0x73, 0x54, 0x0d, 0x0a, 0x0d, 0x0a 

	};


	char l4icmpdata[] = {

	/* ICMP header */
	0x08,  /* TYPE */
	0x00,  /* CODE */
	0x3e, 0x5c,  /* checksum */
	0x04, 0x00,  /* ID */
	0x0b, 0x00,  /* sequence */

	/* Payload */
	0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,      0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
	0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61,      0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 
	
	};	


	char l4igmpdata[] = {

	/* IGMP header */
	0x01,  /* TYPE */
	0x00,  /* group */
	0xfe, 0xf9,  /* checksum */
	0x00, 0x00,0x00, 0x01,  /* ID */
	0x00, 0x00,0x00, 0x02,  /* Multicast address */
	0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x03,  /* Access key */	
	0x00,  /* TYPE */
	

	/* Payload */
	0,0,0,0,0,0,0,0, 	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0
	
	};	

int32 _createSourcePacket(char *data_org,int flags,int8 iphlen,uint8 ip_proto)
{

	uint16 pktsize=0;
	struct ip *ip=NULL;
	struct tcphdr *tc=NULL;
	struct udphdr *ud=NULL;
	struct icmp *ic=NULL;	
	struct igmp *ig=NULL;	
/*	struct udphdr *ud=NULL;	*/

	
	char *data=data_org;
	char mac[] = {
	0x00, 0x00, 0xe8, 0x66, 0x46, 0x85,   /* DMAC */
	0x00, 0x0c, 0x6e, 0x3a, 0x5d, 0x66,   /* SMAC */
	};

	char type_ip[] = {
	0x08, 0x00  /* Ether Type */
	};

	char type_other[] = {
	0x88, 0x08  /* Ether Type */
	};	

	char type_pppoe63[] = {
	0x88, 0x63,  /* PPPoE Type */
	0x11,0,1,2,3,4,  /* PPPoE Header , seesion id=0x0102, length=0x0304  */
	0,0x21 /* ppp (IP Type) */
	};		

	char type_pppoe64[] = {
	0x88, 0x64,  /* PPPoE Type */
	0x11,0,1,2,3,4,  /* PPPoE Header , seesion id=0x0102, length=0x0304 */
	0,0x21 /* ppp (IP Type) */
	};		

	char snap[] = {
	0x05,0xff,0xaa,0xaa,0x03,0,0,0, /* for SNAP */
	};	

	char type_vlan[] = {
	0x81,0x00, /* VLAN Tag */
	0xaa,0xaa, /* Priority=5, CFI=0, VID=2730 */
	};	

	char llc[] ={
	0x05,0xff,0xff,0xff /* IPX */
	};


	char l3data[] = {
	/* IP header */
	0x45, 0x00, 0x01, 0x67, 0xa4, 0xd2, 0x40, 0x00, 
	0x80, 0x06, 0xaf, 0xde, 0xc0, 0xa8, 0x9a, 0x6e, 
	0x42, 0x66, 0x07, 0x63
	};

	memset(data_org,0,64);

	l3data[9]=ip_proto; /* set layer 3 protocol */

	

	switch(flags)
	{
		case L2_IP:
		case L2_VLAN_IP:
		case L2_SNAP_IP:
		case L2_VLAN_SNAP_IP:
			memcpy(data,mac,sizeof(mac));
			data+=sizeof(mac);
			if((flags==L2_VLAN_IP)||(flags==L2_VLAN_SNAP_IP))
			{
				memcpy(data,type_vlan,sizeof(type_vlan));
				data+=sizeof(type_vlan);
			}
			if((flags==L2_SNAP_IP)||(flags==L2_VLAN_SNAP_IP))
			{
				memcpy(data,snap,sizeof(snap));
				data+=sizeof(snap);
			}
			memcpy(data,type_ip,sizeof(type_ip));
			data+=sizeof(type_ip);

			ip=(struct ip *)data;

			{
				int ipvhl=0x40;				
				ipvhl+=(iphlen>>2)&0xff; /* if iphlen > 60, it is able to create the case : IP header != ipv4 */
				l3data[0]=ipvhl;
				
				memcpy(data,l3data,sizeof(l3data));	
				data+=sizeof(l3data);


				if(ipvhl>=0x45)
				{
					memset(data,0,(ipvhl-0x45)<<2);
					data+=((ipvhl-0x45)<<2);
					l3data[3]+=((ipvhl-0x45)<<2);
				}
				else
				{
					data+=((ipvhl-0x45)*4);
				}				
			}

			ip->ip_sum=0;
			switch(ip_proto)
			{
				case IPPROTO_TCP:
					tc=(struct tcphdr *)data;
					memcpy(data,l4data,sizeof(l4data));	
					data += sizeof(l4data);
					ip->ip_len = htons(sizeof(l4data)+iphlen);					
					tc->th_sum = 0;			
					tc->th_sum = model_tcpChecksum(ip);
					break;
				case IPPROTO_UDP:
					ud=(struct udphdr *)data;
					memcpy(data,l4udpdata,sizeof(l4udpdata));	
					data += sizeof(l4udpdata);
					ip->ip_len = htons(sizeof(l4udpdata)+iphlen);
					ud->uh_sum = 0;			
					ud->uh_sum = model_tcpChecksum(ip);
					break;			
				case IPPROTO_ICMP:
					ic=(struct icmp *)data;
					memcpy(data,l4icmpdata,sizeof(l4icmpdata));
					data += sizeof(l4icmpdata);
					ip->ip_len = htons(sizeof(l4icmpdata)+iphlen);
					ic->icmp_cksum = 0;
					ic->icmp_cksum = model_icmpChecksum(ip);
					break;
				case IPPROTO_IGMP:
					ig=(struct igmp *)data;
					memcpy(data,l4igmpdata,sizeof(l4igmpdata));
					data += sizeof(l4igmpdata);
					ip->ip_len = htons(sizeof(l4igmpdata)+iphlen);
					ig->igmp_cksum = 0;
					ig->igmp_cksum = model_icmpChecksum(ip);
					break;
					
			}
			
			ip->ip_sum = model_ipChecksum(ip);
			
			break;


		case L2_PPPOE63:
		case L2_VLAN_PPPOE63:			
		case L2_SNAP_PPPOE63:			
		case L2_VLAN_SNAP_PPPOE63:						
			memcpy(data,mac,sizeof(mac));
			data+=sizeof(mac);
			if((flags==L2_VLAN_PPPOE63)||(flags==L2_VLAN_SNAP_PPPOE63))
			{
				memcpy(data,type_vlan,sizeof(type_vlan));
				data+=sizeof(type_vlan);
			}
			if((flags==L2_SNAP_PPPOE63)||(flags==L2_VLAN_SNAP_PPPOE63))
			{
				memcpy(data,snap,sizeof(snap));
				data+=sizeof(snap);
			}			
			memcpy(data,type_pppoe63,sizeof(type_pppoe63));
			data+=sizeof(type_pppoe63);

			break;

			
		case L2_PPPOE64_IP:
		case L2_VLAN_PPPOE64_IP:			
		case L2_SNAP_PPPOE64_IP:			
		case L2_VLAN_SNAP_PPPOE64_IP:						
			memcpy(data,mac,sizeof(mac));
			data+=sizeof(mac);
			if((flags==L2_VLAN_PPPOE64_IP)||(flags==L2_VLAN_SNAP_PPPOE64_IP))
			{
				memcpy(data,type_vlan,sizeof(type_vlan));
				data+=sizeof(type_vlan);
			}
			if((flags==L2_SNAP_PPPOE64_IP)||(flags==L2_VLAN_SNAP_PPPOE64_IP))
			{
				memcpy(data,snap,sizeof(snap));
				data+=sizeof(snap);
			}			
			memcpy(data,type_pppoe64,sizeof(type_pppoe64));
			data+=sizeof(type_pppoe64);

			ip=(struct ip *)data;

			{
				int ipvhl=0x40;
				ipvhl|=(iphlen>>2)&0xf;
				l3data[0]=ipvhl;
				
				memcpy(data,l3data,sizeof(l3data));	
				data+=sizeof(l3data);

				if(ipvhl>=0x45)
				{
					memset(data,0,(ipvhl-0x45)<<2);
					data+=((ipvhl-0x45)<<2);
					l3data[3]+=((ipvhl-0x45)<<2);
				}
				else
				{
					data+=((ipvhl-0x45)*4);
				}
			}

			ip->ip_sum=0;
			
			switch(ip_proto)
			{
				case IPPROTO_TCP:			
					tc=(struct tcphdr *)data;					
					memcpy(data,l4data,sizeof(l4data));	
					data += sizeof(l4data);
					ip->ip_len = htons(sizeof(l4data)+sizeof(l3data));
					tc->th_sum = 0;			
					tc->th_sum = model_tcpChecksum(ip);
					break;
				case IPPROTO_UDP:
					ud=(struct udphdr *)data;
					memcpy(data,l4udpdata,sizeof(l4udpdata));	
					data += sizeof(l4udpdata);
					ip->ip_len = htons(sizeof(l4udpdata)+sizeof(l3data));
					ud->uh_sum = 0;			
					ud->uh_sum = model_tcpChecksum(ip);
					break;
				case IPPROTO_ICMP:
					ic=(struct icmp *)data;
					memcpy(data,l4icmpdata,sizeof(l4icmpdata));	
					data += sizeof(l4icmpdata);					
					ip->ip_len = htons(sizeof(l4icmpdata)+iphlen);				
					ic->icmp_cksum = 0;
					ic->icmp_cksum = model_icmpChecksum(ip);
					break;
				case IPPROTO_IGMP:
					ig=(struct igmp *)data;
					memcpy(data,l4igmpdata,sizeof(l4igmpdata));
					data += sizeof(l4igmpdata);
					ip->ip_len = htons(sizeof(l4igmpdata)+iphlen);
					ig->igmp_cksum = 0;
					ig->igmp_cksum = model_icmpChecksum(ip);
					break;					
			}			


			ip->ip_sum = model_ipChecksum(ip);
			
			break;
			
			
		case L2_OTHER:
		case L2_VLAN_OTHER:
		case L2_SNAP_OTHER:			
		case L2_VLAN_SNAP_OTHER:						
			memcpy(data,mac,sizeof(mac));
			data+=sizeof(mac);
			if((flags==L2_VLAN_OTHER)||(flags==L2_VLAN_SNAP_OTHER))
			{
				memcpy(data,type_vlan,sizeof(type_vlan));
				data+=sizeof(type_vlan);
			}
			if((flags==L2_SNAP_OTHER)||(flags==L2_VLAN_SNAP_OTHER))
			{
				memcpy(data,snap,sizeof(snap));
				data+=sizeof(snap);
			}				
			memcpy(data,type_other,sizeof(type_other));
			data+=sizeof(type_other);

			memcpy(data,l3data,sizeof(l3data));	
			data+=sizeof(l3data);

			switch(ip_proto)
			{
				case IPPROTO_TCP:			
					memcpy(data,l4data,sizeof(l4data));	
					data+=sizeof(l4data);
					break;
				case IPPROTO_UDP:			
					memcpy(data,l4udpdata,sizeof(l4udpdata));	
					data+=sizeof(l4udpdata);
					break;
				case IPPROTO_ICMP:			
					memcpy(data,l4icmpdata,sizeof(l4icmpdata));	
					data+=sizeof(l4icmpdata);
					break;
				case IPPROTO_IGMP:			
					memcpy(data,l4igmpdata,sizeof(l4igmpdata));	
					data+=sizeof(l4igmpdata);
					break;					
			}
			

			break;
			
		case L2_LLC:
		case L2_VLAN_LLC:			
			memcpy(data,mac,sizeof(mac));
			data+=sizeof(mac);
			if(flags==L2_VLAN_LLC)
			{
				memcpy(data,type_vlan,sizeof(type_vlan));
				data+=sizeof(type_vlan);
			}			
			memcpy(data,llc,sizeof(llc));
			data+=sizeof(llc);

			/* padding size large than 64 bytes */
			memset(data,0,60);
			data+=60;
			break;
	}

	pktsize= (int)data-(int)data_org;
	if(pktsize<60) pktsize=60;


	return pktsize;
}

void dump_hs(void);
void _initHsb(hsb_param_t *hsb)
{
	memset(hsb,0,sizeof(hsb_param_t));

	
	hsb->spa=PN_PORT0;
	hsb->vid=0x0;
	hsb->tagif=0x0;
	hsb->pppoeif=0x0;
	hsb->sip=0xc0a89a6e;
	hsb->sprt=4166;
	hsb->dprt=80;
	hsb->dip=0x42660763;
	hsb->ipptl=0x6;
	hsb->ipfg=0x2;
	hsb->iptos=0x0;
	hsb->tcpfg=0x18;
	hsb->type=0x5;
	hsb->patmatch=0x0;
	hsb->ethtype=0x0800;
	hsb->da[0]=0x0; hsb->da[1]=0x0; hsb->da[2]=0xe8; hsb->da[3]=0x66; hsb->da[4]=0x46; hsb->da[5]=0x85;
	hsb->sa[0]=0x0; hsb->sa[1]=0xc; hsb->sa[2]=0x6e; hsb->sa[3]=0x3a; hsb->sa[4]=0x5d; hsb->sa[5]=0x66;
	hsb->hiprior=0x0;
	hsb->snap=0x0;
	hsb->llcothr=0x0;
	hsb->udpnocs=0x0;
	hsb->ttlst=0x2;
	hsb->dirtx=0x0;
	hsb->l3csok=0x1;
	hsb->l4csok=0x1;
	hsb->ipfo0_n=0x0;
	hsb->urlmch=0x1;
	hsb->extspa=0x0;
	hsb->extl2=0x0;
	hsb->linkid=0x0;
	hsb->pppoeid=0x0;	
}

int32 parserTagForDirectTXfromCPU(uint32 caseNo)
{
	int k;

	for( k = IC_TYPE_MIN; k < IC_TYPE_MAX; k++ )
	{

		struct rtl_pktHdr	*pktHdr=NULL;
		struct rtl_mBuf	*Mblk=NULL;
		hsb_param_t hsb,rhsb;	
		char *data;
		int len;
		int i=0,j,m,n;	
		int maxloop;
		int retval;
		int errcnt=0;
		enum PORT_MASK port;

		retval = model_setTestTarget( k );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */



		data=rtlglue_malloc(2048);

		ASSERT(data!=NULL);


#ifdef RTL865X_MODEL_KERNEL	
		data=(char *)(UNCACHE_MASK|(uint32)data);
#endif
	
		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();
		rtl8651_clearAsicPvid();


		/* module switch control reg: enable L2/L3/L4/STP, trap 802.1D/GARP to CPU */
		rtl8651_setAsicOperationLayer(3);

		retval=configNicInit();
		ASSERT(retval==SUCCESS);		
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		
		/* parserInit(j); */

		WRITE_MEM32(CPUIIMR,READ_MEM32(CPUIIMR)|TX_DONE_IE1);

		
		/* set Pattern Match */
		{
			union {
				char pat[4];
				uint32 pattern;
			}u;

			u.pat[0]='g';
			u.pat[1]='z';
			u.pat[2]='i';
			u.pat[3]='p';

			for(i=0; i<RTL8651_PORT_NUMBER; i++) {
				int32 retval;
				retval = rtl8651_setAsicPortPatternMatch(i,htonl(u.pattern), htonl(0xffffffff), 0x2);
				assert(retval == SUCCESS);
			}
		}	

		if(k==IC_TYPE_MODEL)
		{
			maxloop=1;
		}
		else
		{
			maxloop=1;
		}

		for(i=0;i<maxloop;i++)
		{
			for(j=0;j<L2_TYPE_MAX;j++)
			{
				for(n=0;n<4;n++)
				{
					int ip_proto=IPPROTO_TCP;
					switch(n)
					{
						case 0:
							ip_proto=IPPROTO_TCP;
							break;
						case 1:
							ip_proto=IPPROTO_UDP;
							break;
						case 2:								
							ip_proto=IPPROTO_ICMP;
							break;
						case 3:								
							ip_proto=IPPROTO_IGMP;
							break;							
					}
			
				
					for(m=0;m<=68;m+=4) /* case 64,68 for ip not version 4 */
					{
						if(m==28) m=56;
						rtlglue_printf("loop=%d l2=%d iphdr_len=%d type=%d\n",i,j,m,n);
						memset(&hsb,0,sizeof(hsb_param_t));	

						len=_createSourcePacket(data,j,m,ip_proto);

						if(k==IC_TYPE_MODEL)
						{
							Mblk=_genPktHdr((void*)data,(uint32)0,2048, len,0);
						}
						else
						{
							Mblk=mBuf_attachHeader((void*)data,(uint32)0,2048, len,0);						
						}

						pktHdr=Mblk->m_pkthdr;
						pktHdr->ph_srcExtPortNum=0;  /* from CPU */
						

						/*  those field copy from realtek packet header */
						/* bit offset 5 */
						hsb.len=pktHdr->ph_len+4;				

						pktHdr->ph_vlanId=rtlglue_random()&0xffff;
						/* bit offset 20 */
						hsb.vid=pktHdr->ph_vlanId&0xfff; /*  low bit [11-0]: for vlanID */
						/* bit offset 32 */

						switch(j)
						{
							case L2_VLAN_IP:
							case L2_VLAN_PPPOE63:
							case L2_VLAN_PPPOE64_IP:
							case L2_VLAN_OTHER:
							case L2_VLAN_LLC:
							case L2_VLAN_SNAP_IP:
							case L2_VLAN_SNAP_PPPOE63:
							case L2_VLAN_SNAP_PPPOE64_IP:
							case L2_VLAN_SNAP_OTHER:
								pktHdr->ph_vlanTagged=1;
								break;
							default:
								pktHdr->ph_vlanTagged=0;		
								break;
						}
						hsb.tagif=pktHdr->ph_vlanTagged;
		
						/* bit offset 33 */

						switch(j)
						{
							case L2_PPPOE64_IP:
							case L2_SNAP_PPPOE64_IP:
							case L2_VLAN_PPPOE64_IP:
							case L2_VLAN_SNAP_PPPOE64_IP:
								pktHdr->ph_pppeTagged=1;
								break;
							default:
								pktHdr->ph_pppeTagged=0;						
								break;
						}

						hsb.pppoeif=pktHdr->ph_pppeTagged;
						
						/* bit offset 66 */
						pktHdr->ph_flags&=(~CSUM_IP);				
						if(rtlglue_random()&0x1) pktHdr->ph_flags|=(CSUM_IP);

						/* bit offset 114 */
						pktHdr->ph_flags&=(~CSUM_L4);				
						if(rtlglue_random()&0x1) pktHdr->ph_flags|=(CSUM_L4);

pktHdr->ph_flags|=(CSUM_IP);
pktHdr->ph_flags|=(CSUM_L4);



						/* bit offset 157 */
						switch(j)
						{
							case L2_IP:
							case L2_PPPOE64_IP:
							case L2_SNAP_IP:
							case L2_SNAP_PPPOE64_IP:
							case L2_VLAN_IP:
							case L2_VLAN_PPPOE64_IP:
							case L2_VLAN_SNAP_IP:
							case L2_VLAN_SNAP_PPPOE64_IP:
								switch(ip_proto)
								{
									case IPPROTO_TCP:
										pktHdr->ph_type=PKTHDR_TCP;
										break;
									case IPPROTO_UDP:
										pktHdr->ph_type=PKTHDR_UDP;
										break;
									case IPPROTO_ICMP:
										pktHdr->ph_type=PKTHDR_ICMP;
										break;
									case IPPROTO_IGMP:
										pktHdr->ph_type=PKTHDR_IGMP;
										break;

								}
								break;
							case L2_PPPOE63:
							case L2_OTHER:
							case L2_LLC:
							case L2_SNAP_PPPOE63:
							case L2_SNAP_OTHER:
							case L2_VLAN_PPPOE63:
							case L2_VLAN_OTHER:
							case L2_VLAN_LLC:
							case L2_VLAN_SNAP_PPPOE63:
							case L2_VLAN_SNAP_OTHER:
								pktHdr->ph_type=PKTHDR_ETHERNET;
								break;	
						}


						hsb.type = pktHdr->ph_type; 
						/* bit offset 161 */
						pktHdr->ph_flags2=(rtlglue_random()&0xffff); /*  bit[5:0]:  Per Port Tag mask setting for TX. */
						/*pktHdr->ph_portlist=rtlglue_random()&0x1f; */ /*  bit[5:0]: for Physical Port mask */
						/* pktHdr->ph_pppoeIdx=rtlglue_random()&0x7;  */
						pktHdr->ph_portlist=PM_PORT_0; /*  bit[5:0]: for Physical Port mask */

						hsb.ethtype=(((pktHdr->ph_flags2)&0x3f)<<10)|((pktHdr->ph_portlist&0x3f)<<4)|((((pktHdr->ph_flags)>>2)&1)<<3)|(pktHdr->ph_pppoeIdx);
						/* bit offset 273 */
						hsb.hiprior=((pktHdr->ph_vlanId)>>12)&0x7; /*  bit[14-12]: for Direct TX Priority */
						/* bit offset 276 */

						switch(j)
						{
							case L2_SNAP_IP:
							case L2_SNAP_PPPOE63:
							case L2_SNAP_PPPOE64_IP:
							case L2_SNAP_OTHER:
							case L2_VLAN_SNAP_IP:
							case L2_VLAN_SNAP_PPPOE63:
							case L2_VLAN_SNAP_PPPOE64_IP:
							case L2_VLAN_SNAP_OTHER:
								pktHdr->ph_LLCTagged=1; /*  SNAP tag */
								break;
							default:
								pktHdr->ph_LLCTagged=0; /*  SNAP tag */
								break;
						}
						
						hsb.snap=pktHdr->ph_LLCTagged;
						/* bit offset 280 */
						pktHdr->ph_flags&=(~PKTHDR_HWLOOKUP);				
						hsb.dirtx=(pktHdr->ph_flags&PKTHDR_HWLOOKUP)?0:1;
						if(rtlglue_random()&0x1)	hsb.dirtx|=PKTHDR_HWLOOKUP;				
						
						/* DirectTX not parse those field */
						/*  Ether Type Parser  */
						hsb.spa = 7;					
						hsb.trigpkt=0;
						hsb.sip=0;
						hsb.dip=0;
						hsb.ipptl=0;	
						hsb.ipfg=0;
						hsb.iptos=0;
						hsb.tcpfg=0;
						hsb.patmatch=0;
//						hsb.da[0]=0x0; hsb.da[1]=0x0; hsb.da[2]=0x0; hsb.da[3]=0x0; hsb.da[4]=0x0; hsb.da[5]=0x0;
						hsb.da[0]=0x0; hsb.da[1]=0x0; hsb.da[2]=0xe8; hsb.da[3]=0x66; hsb.da[4]=0x46; hsb.da[5]=0x85;
						hsb.sa[0]=0x0; hsb.sa[1]=0x0; hsb.sa[2]=0x0; hsb.sa[3]=0x0; hsb.sa[4]=0x0; hsb.sa[5]=0x0;	
						hsb.udpnocs=0;				
						hsb.ttlst=0;
						hsb.l3csok=1;	
						hsb.l4csok=1;	
						hsb.ipfo0_n=0;
						hsb.llcothr=0;
						hsb.urlmch=0;
						hsb.extspa=3;
						hsb.extl2=0;
						hsb.linkid=0;
						hsb.pppoeid=0;

						hsb.sprt=0;
						hsb.dprt=0;				

						switch(j) /*  by Ethernet Type */
						{
							/*  with layer 4 header */
							case L2_IP:
							case L2_VLAN_IP:
							case L2_SNAP_IP:
							case L2_VLAN_SNAP_IP:
							case L2_PPPOE64_IP:
							case L2_VLAN_PPPOE64_IP:			
							case L2_SNAP_PPPOE64_IP:			
							case L2_VLAN_SNAP_PPPOE64_IP:

								if(pktHdr->ph_flags&(CSUM_IP))
								{
								
									if((m>=20)&&(m<=60))
									{
										/* memDump(data,128,"dat"); */
										switch(ip_proto)
										{
											case IPPROTO_TCP:
												hsb.sprt=((data[hsb.len-sizeof(l4data)-m+6])<<8)&0xff00;		
												hsb.sprt|=data[hsb.len-sizeof(l4data)-m+7]&0xff;								
												break;
											case IPPROTO_UDP:
												hsb.sprt=((data[hsb.len-sizeof(l4udpdata)-m+6])<<8)&0xff00;		
												hsb.sprt|=data[hsb.len-sizeof(l4udpdata)-m+7]&0xff;								
												break;
											case IPPROTO_ICMP:
												hsb.sprt=((data[hsb.len-sizeof(l4icmpdata)-m+6])<<8)&0xff00;		
												hsb.sprt|=data[hsb.len-sizeof(l4icmpdata)-m+7]&0xff;	
												break;
											case IPPROTO_IGMP:
												hsb.sprt=((data[hsb.len-sizeof(l4igmpdata)-m+6])<<8)&0xff00;		
												hsb.sprt|=data[hsb.len-sizeof(l4igmpdata)-m+7]&0xff;
												break;												
										}
												
										
									}
								}
								
								if(pktHdr->ph_flags&(CSUM_L4))
								{
									if((m>=20)&&(m<=60))
									{
										switch(ip_proto)
										{
											case IPPROTO_TCP:
												hsb.dprt=((data[len-sizeof(l4data)+16])<<8)&0xff00;		
												hsb.dprt|=data[len-sizeof(l4data)+17]&0xff;								

												break;
											case IPPROTO_UDP:
												hsb.dprt=((data[len-sizeof(l4udpdata)+6])<<8)&0xff00;		
												hsb.dprt|=data[len-sizeof(l4udpdata)+7]&0xff;

												break;
											case IPPROTO_ICMP:
												hsb.dprt=((data[len-sizeof(l4icmpdata)+2])<<8)&0xff00;
												hsb.dprt|=data[len-sizeof(l4icmpdata)+3]&0xff;

												break;
											case IPPROTO_IGMP:
												hsb.dprt=((data[len-sizeof(l4igmpdata)+2])<<8)&0xff00;
												hsb.dprt|=data[len-sizeof(l4igmpdata)+3]&0xff;

												break;	
										}								
									}
								}
							
		
								break;

							
						}

#if 0
						if(k==IC_TYPE_MODEL)
						{
/*							 memDump(data,hsb.len-4,"data");  */

							modelPktParser(&rhsb,data,7,hsb.len,pktHdr);
							/* dumpHSB(&rhsb); */
							rtlglue_free(Mblk);
							rtlglue_free(pktHdr);

						}
						else
#endif
						{


#ifdef _LITTLE_ENDIAN						
							int z;
							for(z=0;z<pktHdr->ph_len;z+=4)
							{
								uint32 *p,q;
								p=(uint32*)&data[z];
								q=*p;
								*p=ntohl(q);
							}
#endif
						
							retval=swNic_write(pktHdr,1); /* send with high priority */						

							while(1)
							{
								if(READ_MEM32(CPUIISR)&TX_DONE_IE1)
								{
									WRITE_MEM32(CPUIISR,TX_DONE_IE1);
									break;					
								}
								rtlglue_printf("%s %d  CPUIISR=%x\n",__FUNCTION__,__LINE__,READ_MEM32(CPUIISR));
							}	
							
					
						virtualMacWaitCycle(10000);
			
						}
					
						virtualMacGetHsb(&rhsb);
						/* dumpHSB(&rhsb); */

						retval=compareHsb(&hsb,&rhsb);
						if(retval==FAILED) 
						{
							rtlglue_printf("pkt=%x mblk=%x data=%x\n",(uint32)pktHdr,(uint32)Mblk,(uint32)data);					
							memDump(pktHdr,32,"pkt");
							memDump(Mblk,32,"mbuf");
							memDump(data,hsb.len,"data");
							errcnt++;
						}
				
						port=PM_PORT_0;
						len=2048;
						retval=virtualMacOutput(&port, data, &len);					
						ASSERT(retval==SUCCESS);
						
						
						/* CHECK(compareHsb(&hsb,&rhsb));	 */
					}
				}
			}
		}
		if(errcnt!=0) return FAILED;

#ifdef RTL865X_MODEL_KERNEL	
		data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
		rtlglue_free(data);
		
		return SUCCESS;
	}
	return SUCCESS;

}

int32 parserPatternMatch(uint32 caseNo)
{

	int k;
	


	for( k = IC_TYPE_MIN; k < IC_TYPE_MAX; k++ )
	{
		hsb_param_t hsb,rhsb;	
		char data[2048]={0};	
		int i=0;
		int retval;

		retval = model_setTestTarget(k );
			
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */


		rtl8651_clearRegister();
		parserInit(k);		
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC); 	
		
		/* set Pattern Match */
		{
			union {
				char pat[4];
				uint32 pattern;
			}u;

			u.pat[0]='T';
			u.pat[1]='T';
			u.pat[2]='P';
			u.pat[3]='/';

			for(i=0; i<RTL8651_PORT_NUMBER; i++) {
				int32 retval;
				retval = rtl8651_setAsicPortPatternMatch(i,htonl(u.pattern), htonl(0xffffffff), 0x2);
				assert(retval == SUCCESS);
			}
		}


		_initHsb(&hsb);

		hsb.spa=PN_PORT0;
		hsb.sprt=4166;
		hsb.ipfg=2;
		hsb.urlmch=1;
		hsb.linkid=0;
		hsb.dprt=80;
		hsb.patmatch=0x1;	
		hsb.ethtype=0x0800;
		hsb.dirtx=0x0;	
		hsb.l3csok=0x1;
		hsb.l4csok=0x1;
		hsb.pppoeid=0;	

		/*  Ether Type Parser  */
		hsb.len=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP)+4;

		if(data[14]!=0x45)
		{
			hsb.dirtx=1;
			hsb.tcpfg=2;
			if((data[14]&0x40)==0x40)
			{
				/*  is ipv4 header */
				hsb.tcpfg|= ((data[14]&0xf)<<2); /*  ipv4 header length */
				hsb.tcpfg|=1<<6;   /* ipv4 packet indication */
			}
		}


		if(k==IC_TYPE_MODEL)
		{
			/* rtlglue_printf("hsb.len=%d\n",hsb.len); */
			/* memDump(data,hsb.len,"test"); */
			
			modelPktParser(&rhsb,data,PN_PORT0,hsb.len,NULL);
		}
		else
		{
			rtl8651_clearAsicAllTable();
			parserInit(k);
			retval=virtualMacInput(PN_PORT0,data,hsb.len);
			if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);
		}
		
		retval=virtualMacGetHsb(&rhsb);
		ASSERT(retval == SUCCESS);
		CHECK(compareHsb(&hsb,&rhsb));

		return SUCCESS;
	}

	return SUCCESS;
}

int32 parserChecksumCheck(uint32 caseNo)
{
	
	int k;

	for( k = IC_TYPE_MIN; k < IC_TYPE_MAX; k++ )
	{
	
		/* 	struct rtl_pktHdr	*pktHdr; */
		/* 	struct rtl_mBuf	*Mblk; */
		hsb_param_t hsb,rhsb;	
		char data[2048]={0};	
		int i=0;
		int retval;

		retval = model_setTestTarget(k );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		rtl8651_clearRegister();
		parserInit(k);		
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC); 			

		/* set Pattern Match (Disable) */
		{
			union {
				char pat[4];
				uint32 pattern;
			}u;

			u.pat[0]='g';
			u.pat[1]='z';
			u.pat[2]='i';
			u.pat[3]='p';

			for(i=0; i<RTL8651_PORT_NUMBER; i++) {
				int32 retval;
				retval = rtl8651_setAsicPortPatternMatch(i,htonl(u.pattern), htonl(0), 0x2);
				assert(retval == SUCCESS);
			}
		}



		_initHsb(&hsb);

		hsb.spa=PN_PORT0;
		hsb.l3csok=0x1;
		hsb.l4csok=0x1;
		hsb.pppoeid=0;
		hsb.patmatch=0;

		/*  Ether Type Parser (L34 OK) */
		hsb.len=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP)+4;


		if(k==IC_TYPE_MODEL)
		{
			modelPktParser(&rhsb,data,0,hsb.len,NULL);			
		}
		else
		{
			rtl8651_clearAsicAllTable();
			parserInit(k);
			retval=virtualMacInput(PN_PORT0,data,hsb.len);
			if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);
			
		}
		virtualMacGetHsb(&rhsb);	
		CHECK(compareHsb(&hsb,&rhsb));	


		/*  Ether Type Parser (L3 OK,L4 Checksum Error) */
		hsb.l3csok=0x1;
		hsb.l4csok=0x0;

		hsb.len=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP)+4;

		data[hsb.len-5]=0x0b; /* modify layer 4 content last byte */

		if(k==IC_TYPE_MODEL)
		{
			modelPktParser(&rhsb,data,0,hsb.len,NULL);
		}
		else
		{
			parserInit(k);		
			retval=virtualMacInput(PN_PORT0,data,hsb.len);
			if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);
		}
		virtualMacGetHsb(&rhsb);	
		CHECK(compareHsb(&hsb,&rhsb));	

		/*  Ether Type Parser (L3 L4 Checksum Error) */
		hsb.l3csok=0x0;
		hsb.l4csok=0x0;	
		hsb.len=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP)+4;
		data[26]=1; /* modify the IP address of IP packet */
		hsb.sip=0x01a89a6e;

		if(k==IC_TYPE_MODEL)
		{
			modelPktParser(&rhsb,data,0,hsb.len,NULL);
		}
		else
		{
			parserInit(k);		
			retval=virtualMacInput(PN_PORT0,data,hsb.len);
			if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);
		}
		virtualMacGetHsb(&rhsb);	
		CHECK(compareHsb(&hsb,&rhsb));	

		
		hsb.len=_createSourcePacket(data,L2_IP,20,IPPROTO_UDP)+4;

		hsb.l3csok=0x1;
		hsb.l4csok=0x0;
		hsb.udpnocs=0x1;
		hsb.sip=0xc0a89a6e;
		hsb.ipptl=0x11;
		hsb.tcpfg=0;
		hsb.type=6;
		data[40]=0;
		data[41]=0;	

		if(k==IC_TYPE_MODEL)
		{
			memDump(data,hsb.len,"data");
			modelPktParser(&rhsb,data,0,hsb.len,NULL);
		}
		else
		{
			parserInit(k);		
			retval=virtualMacInput(PN_PORT0,data,hsb.len);
			if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);
		}
		virtualMacGetHsb(&rhsb);	
		CHECK(compareHsb(&hsb,&rhsb));	
		

		return SUCCESS;	
	}
	return SUCCESS;
}

int32 parserUrlTrap(uint32 caseNo)
{

	/* not be implemented */
	
	return SUCCESS;
}

int32 parserRxFromExtPort(uint32 caseNo)
{

	int k;
	

	for( k = IC_TYPE_MIN; k < IC_TYPE_MAX; k++ )
	{
		struct rtl_pktHdr	*pktHdr;
		struct rtl_mBuf	*Mblk;
		hsb_param_t hsb,rhsb;	
		char *data;	
		int i=0,j,m=0,n;
		int retval;
		int errcnt=0;
		int len;

		retval = model_setTestTarget( k );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */	

		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable(); /*  keep packet continuing send. */		
		RoutingConfig();
		
		parserInit(k);		
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC); 		
		

		data=rtlglue_malloc(2048);
		ASSERT(data!=NULL) ;

#ifdef RTL865X_MODEL_KERNEL	
		data=(char *)(UNCACHE_MASK|(uint32)data);
#endif
		
		/* trap all pkt to CPU */
		{
			_rtl8651_tblDrvAclRule_t aclt;
			bzero((void*) &aclt, sizeof(aclt));
			aclt.ruleType_ = RTL8651_ACL_MAC;
			aclt.actionType_ = RTL8651_ACL_CPU;
			aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
			retval = rtl8651_setAsicAclRule(0, &aclt);
			ASSERT( retval==SUCCESS );
			retval = rtl8651_setAsicAclRule(4, &aclt);
			ASSERT( retval==SUCCESS );
			retval = rtl8651_setAsicAclRule(8, &aclt);
			ASSERT( retval==SUCCESS );
			retval = rtl8651_setAsicAclRule(12, &aclt);
			ASSERT( retval==SUCCESS );			
		}


		/* set Pattern Match */
		{
			union {
				char pat[4];
				uint32 pattern;
			}u;

			u.pat[0]='g';
			u.pat[1]='z';
			u.pat[2]='i';
			u.pat[3]='p';

			for(i=0; i<RTL8651_PORT_NUMBER; i++) {
				int32 retval;
				retval = rtl8651_setAsicPortPatternMatch(i,htonl(u.pattern), htonl(0xffffffff), 0x2);
				assert(retval == SUCCESS);
			}
		}	

		


		for(j=0;j<L2_TYPE_MAX;j++)
		{
			for(n=0;n<4;n++)
			{

				int ip_proto=IPPROTO_TCP;
				switch(n)
				{
					case 0:
						ip_proto=IPPROTO_TCP;
						break;
					case 1:
						ip_proto=IPPROTO_UDP;
						break;
					case 2:								
						ip_proto=IPPROTO_ICMP;
						break;
					case 3:								
						ip_proto=IPPROTO_IGMP;
						break;						
				}

				for(m=0;m<=68;m+=4)
				{
					if(m==28) m=56;
					rtlglue_printf("j=%d type=%d hdr=%d\n",j,n,m);
					memset(&hsb,0,sizeof(hsb_param_t));	
					hsb.spa=0x7;		
					len=_createSourcePacket(data,j,m,ip_proto);	
/*					memDump(data,len,"data"); */

					if(k==IC_TYPE_MODEL)
					{
						Mblk=_genPktHdr((void*)data,(uint32)0,2048, len,0);
					}
					else
					{
						Mblk=mBuf_attachHeader((void*)data,(uint32)0,2048, len,0);						
					}
					
					pktHdr=Mblk->m_pkthdr;
					hsb.len=pktHdr->ph_len+4;			
					
					hsb.da[0]=0x0; hsb.da[1]=0x0; hsb.da[2]=0xe8; hsb.da[3]=0x66; hsb.da[4]=0x46; hsb.da[5]=0x85;
					hsb.sa[0]=0x0; hsb.sa[1]=0xc; hsb.sa[2]=0x6e; hsb.sa[3]=0x3a; hsb.sa[4]=0x5d; hsb.sa[5]=0x66;
					hsb.udpnocs=0x0;	
					hsb.dirtx=0x0;

					pktHdr->ph_srcExtPortNum=rtlglue_random()%3+1;
					pktHdr->ph_flags|=(0x60); /*  Enable: HWLOOKUP & BRIDGING */
					pktHdr->ph_linkID=(0x7f);
					
					hsb.extspa=pktHdr->ph_srcExtPortNum-1;
					hsb.extl2=((pktHdr->ph_flags)>>6)&1;
					hsb.linkid=pktHdr->ph_linkID;

					
					switch(j) /*  by Ethernet Type */
					{
						case L2_IP:
						case L2_VLAN_IP:
						case L2_SNAP_IP:
						case L2_VLAN_SNAP_IP:
							hsb.llcothr=0x0;
							hsb.ethtype=0x0800;
							hsb.pppoeif=0;
							hsb.ipptl=ip_proto;

							/* from IP */
							switch(ip_proto)
							{
								case IPPROTO_TCP:
									hsb.type=0x5;
									hsb.tcpfg=0x18;
									hsb.sprt=4166;
									hsb.dprt=80;	
									hsb.urlmch=0x1;
									hsb.patmatch=0x0;
									break;
								case IPPROTO_UDP:
									hsb.type=0x6;
									hsb.sprt=4166;
									hsb.dprt=80;									
									hsb.urlmch=0x1;
									hsb.patmatch=0x0;
									break;
								case IPPROTO_ICMP:
									hsb.type=0x3;
									hsb.sprt=0x400;
									hsb.ipptl=0x8;
									hsb.urlmch=0x0;
									hsb.patmatch=0x0; 									

									break;
								case IPPROTO_IGMP:
									hsb.type=0x4;
									hsb.sprt=0x0;
									hsb.ipptl=0x1;
									hsb.urlmch=0x0;
									hsb.patmatch=0x0; 										
									break;
								case IPPROTO_GRE:
									hsb.type=0x1;
									break;							
							}
					
											
							hsb.sip=0xc0a89a6e;
							hsb.dip=0x42660763;	
							hsb.l3csok=0x1;
							hsb.l4csok=0x1;
							hsb.ipfg=0x2;
							hsb.iptos=0x0;
							hsb.ttlst=0x2;	
							hsb.ipfo0_n=0x0;
							hsb.pppoeid=0;


							if(m!=20)
							{
								hsb.dirtx=1;
								hsb.tcpfg=(1<<1)|(1<<6)|((m>>2)<<2);
								if((m<20)||(m>60))
								{
									hsb.sip=0;
									hsb.sprt=0;
									hsb.dip=0;
									hsb.dprt=0;
									hsb.ipptl=0;
									hsb.ipfg=0;
									hsb.type=0;
									hsb.ttlst=0;
									hsb.l3csok=0;
									hsb.l4csok=0;
									hsb.tcpfg&=(~(uint32)(1<<6));
								}
							}

							
							break;


						case L2_PPPOE63:
						case L2_VLAN_PPPOE63:			
						case L2_SNAP_PPPOE63:			
						case L2_VLAN_SNAP_PPPOE63:
							hsb.llcothr=0x0;
							hsb.ethtype=0x08863;
							hsb.pppoeif=0;
							hsb.pppoeid=0;
							hsb.patmatch=0x0; 
							break;

							
							
						case L2_PPPOE64_IP:
						case L2_VLAN_PPPOE64_IP:			
						case L2_SNAP_PPPOE64_IP:			
						case L2_VLAN_SNAP_PPPOE64_IP:
							hsb.llcothr=0x0;
							hsb.ethtype=0x08864;
							hsb.pppoeif=1;
							hsb.pppoeid=0x0102;
							hsb.ipptl=ip_proto;							

							/* from IP */
							switch(ip_proto)
							{
								case IPPROTO_TCP:
									hsb.type=0x5;
									hsb.tcpfg=0x18;
									hsb.sprt=4166;
									hsb.dprt=80;									
									hsb.urlmch=0x1;							
									break;
								case IPPROTO_UDP:
									hsb.type=0x6;
									hsb.sprt=4166;
									hsb.dprt=80;
									hsb.urlmch=0x1;
									break;
								case IPPROTO_ICMP:
									hsb.type=0x3;
									hsb.sprt=0x400;
									hsb.ipptl=0x8;							
									hsb.urlmch=0x0;							
									
									break;
								case IPPROTO_IGMP:
									hsb.type=0x4;
									hsb.sprt=0x0;
									hsb.ipptl=0x1;							
									hsb.urlmch=0x0;										
									break;
								case IPPROTO_GRE:
									hsb.type=0x1;
									break;							
							}							

							hsb.sip=0xc0a89a6e;
							hsb.dip=0x42660763;	
							hsb.l3csok=0x1;
							hsb.l4csok=0x1;				

							hsb.ipfg=0x2;
							hsb.iptos=0x0;				

							hsb.ttlst=0x2;		
							hsb.ipfo0_n=0x0; /* not tested */
							
							hsb.patmatch=0x0;


							if(m!=20)
							{
								hsb.dirtx=1;
								hsb.tcpfg=(1<<1)|(1<<6)|((m>>2)<<2);
								if((m<20)||(m>60))
								{
									hsb.sip=0;
									hsb.sprt=0;
									hsb.dip=0;
									hsb.dprt=0;
									hsb.ipptl=0;
									hsb.ipfg=0;
									hsb.type=0;
									hsb.ttlst=0;
									hsb.l3csok=0;
									hsb.l4csok=0;
									hsb.tcpfg&=(~(uint32)(1<<6));
								}
							}
							
							break;
							
						case L2_OTHER:
						case L2_VLAN_OTHER:
						case L2_SNAP_OTHER:			
						case L2_VLAN_SNAP_OTHER:						
							hsb.llcothr=0x0;
							hsb.ethtype=0x8808;
							hsb.pppoeif=0;
							hsb.pppoeid=0;
							switch(ip_proto)
							{
								case IPPROTO_TCP:
								case IPPROTO_UDP:
									hsb.urlmch=0x1;										
									break;
								case IPPROTO_ICMP:
								case IPPROTO_IGMP:
									hsb.urlmch=0x0;										
									break;
									
									
							}

							hsb.patmatch=0x0; 
							break;
							
						case L2_LLC:
						case L2_VLAN_LLC:
							hsb.llcothr=0x1;
							hsb.ethtype=0xffff;
							hsb.pppoeif=0;
							hsb.patmatch=0x0; 
							break;
					}

					switch(j) /*  by VLAN Tag */
					{
						case L2_VLAN_IP:
						case L2_VLAN_SNAP_IP:
						case L2_VLAN_PPPOE63:			
						case L2_VLAN_SNAP_PPPOE63:						
						case L2_VLAN_PPPOE64_IP:			
						case L2_VLAN_SNAP_PPPOE64_IP:						
						case L2_VLAN_OTHER:
						case L2_VLAN_SNAP_OTHER:						
						case L2_VLAN_LLC:			
							hsb.vid=2730;
							hsb.hiprior=5;
							hsb.tagif=1;				
							break;
						default:
							hsb.vid=0;
							hsb.hiprior=0;				
							hsb.tagif=0;	
							break;
					}		

					switch(j) /*  by SNAP */
					{
						case L2_SNAP_IP:
						case L2_VLAN_SNAP_IP:
						case L2_SNAP_PPPOE63:			
						case L2_VLAN_SNAP_PPPOE63:					
						case L2_SNAP_PPPOE64_IP:			
						case L2_VLAN_SNAP_PPPOE64_IP:						
						case L2_SNAP_OTHER:			
						case L2_VLAN_SNAP_OTHER:						
							hsb.snap=0x1;
							break;
						default:
							hsb.snap=0x0;
							break;
					}


/*
		rtlglue_printf("pkt=%x mblk=%x data=%x\n",(uint32)pktHdr,(uint32)Mblk,(uint32)data);					
		memDump(pktHdr,32,"pkt");
		memDump(Mblk,32,"mbuf");
		memDump(data,hsb.len,"data");
*/		

#if 0
					if(k==IC_TYPE_MODEL)
					{
						modelPktParser(&rhsb,data,7,hsb.len,pktHdr);
						rtlglue_free(Mblk);
						rtlglue_free(pktHdr);				
					}
					else
#endif						
					{

#ifdef _LITTLE_ENDIAN
						int z;
						for(z=0;z<pktHdr->ph_len;z+=4)
						{
							uint32 *p,q;
							p=(uint32*)&data[z];
							q=*p;
							*p=ntohl(q);
						}
#endif						


						retval=swNic_write(pktHdr,1); /* send with high priority */

						while(1)
						{
							if(READ_MEM32(CPUIISR)&TX_DONE_IE1)
							{
								WRITE_MEM32(CPUIISR,TX_DONE_IE1);
								break;					
							}
						}	
						
					virtualMacWaitCycle(20000);

					}
					virtualMacGetHsb(&rhsb);

				
					if(compareHsb(&hsb,&rhsb)==FAILED) 
					{
						errcnt++;
						rtlglue_printf("pkt=%x mblk=%x data=%x\n",(uint32)pktHdr,(uint32)Mblk,(uint32)data);
						memDump(pktHdr,32,"pkt");
						memDump(Mblk,32,"mbuf");
						memDump(data,hsb.len,"data");
						rtlglue_printf("Error Case!!");
						goto out; 
					}	

					
					while(1)
					{
						retval=swNic_intHandler(&i);			
						if(retval!=1) break;						
						swNic_rxThread(i);
					}								
				}
			}
		}

 out: 
#ifdef RTL865X_MODEL_KERNEL	
		data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
		rtlglue_free(data);


		if(errcnt==0) 	return SUCCESS;
		else return FAILED;
	}
	return SUCCESS;
}

int32 parserRxFromPhysicalPort(uint32 caseNo)
{

	int k,n;
	

	for( k = IC_TYPE_MIN; k < IC_TYPE_MAX; k++ )
	{
		hsb_param_t hsb,rhsb;	
		char data[2048]={0};	
		int i=0,j,m=0;
		int retval;
		int errnum=0;

		retval = model_setTestTarget( k);
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */	

		rtl8651_clearRegister();
		parserInit(k);		
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC); 			

		/* set Pattern Match */
		{
			union {
				char pat[4];
				uint32 pattern;
			}u;

			u.pat[0]='g';
			u.pat[1]='z';
			u.pat[2]='i';
			u.pat[3]='p';

			for(i=0; i<RTL8651_PORT_NUMBER; i++) {
				int32 retval;
				retval = rtl8651_setAsicPortPatternMatch(i,htonl(u.pattern), htonl(0xffffffff), 0x2);
				assert(retval == SUCCESS);
			}
		}

		/* drop all by SW core */
		{
			_rtl8651_tblDrvAclRule_t aclt;
			bzero((void*) &aclt, sizeof(aclt));
			aclt.ruleType_ = RTL8651_ACL_MAC;
			aclt.actionType_ = RTL8651_ACL_CPU;
			aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
			retval = rtl8651_setAsicAclRule(0, &aclt);
			ASSERT( retval==SUCCESS );
			retval = rtl8651_setAsicAclRule(4, &aclt);
			ASSERT( retval==SUCCESS );
			retval = rtl8651_setAsicAclRule(8, &aclt);
			ASSERT( retval==SUCCESS );
			retval = rtl8651_setAsicAclRule(12, &aclt);
			ASSERT( retval==SUCCESS );			
		}			


		for(j=0;j<L2_TYPE_MAX;j++)
		{		
			memset(&hsb,0,sizeof(hsb_param_t));	

			hsb.spa=PN_PORT0;


			for(n=0;n<4;n++)
			{
				for(m=0;m<=68;m+=4) /* case 64,68  == ip is not version 4 */
				{
					int ip_proto=IPPROTO_TCP;
					if(m==28) m=56;

					switch(n)
					{
						case 0:
							ip_proto=IPPROTO_TCP;
							break;
						case 1:								
							ip_proto=IPPROTO_UDP;
							break;
						case 2:								
							ip_proto=IPPROTO_ICMP;
							break;
						case 3:								
							ip_proto=IPPROTO_IGMP;
							break;								
					}
					
					hsb.len=_createSourcePacket(data,j,m,ip_proto)+4;	
								
								
					rtlglue_printf("j=%d m=%d n=%d\n",j,m,n);
/*					memDump(data,hsb.len-4,"data"); */
					
					hsb.da[0]=0x0; hsb.da[1]=0x0; hsb.da[2]=0xe8; hsb.da[3]=0x66; hsb.da[4]=0x46; hsb.da[5]=0x85;
					hsb.sa[0]=0x0; hsb.sa[1]=0xc; hsb.sa[2]=0x6e; hsb.sa[3]=0x3a; hsb.sa[4]=0x5d; hsb.sa[5]=0x66;
					hsb.udpnocs=0x0;	
					hsb.dirtx=0x0;		
					hsb.extspa=0x0; /* default value in ASIC */
					hsb.extl2=0x0;
					hsb.linkid=0;
					hsb.tcpfg=0;
					hsb.dirtx=0;					
					
					switch(j) /*   by Ethernet Type */
					{
						case L2_IP:
						case L2_VLAN_IP:
						case L2_SNAP_IP:
						case L2_VLAN_SNAP_IP:
							hsb.llcothr=0x0;
							hsb.ethtype=0x0800;
							hsb.pppoeif=0;

							hsb.pppoeid=0;
							hsb.ipptl=ip_proto;

							/*  from IP */
							switch(ip_proto)
							{

							
								case IPPROTO_TCP:
									hsb.type=0x5;
									hsb.tcpfg=0x18;
									hsb.patmatch=0x1;
									hsb.urlmch=0x1;
									hsb.sprt=4166;
									hsb.dprt=80;									
									break;
								case IPPROTO_UDP:
									hsb.type=0x6;
									hsb.patmatch=0x1;
									hsb.urlmch=0x1;
									hsb.sprt=4166;
									hsb.dprt=80;									
									break;
								case IPPROTO_ICMP:
									hsb.patmatch=0x0;
									hsb.urlmch=0x0;
									hsb.type=0x3;
									hsb.sprt=0x400;
									hsb.ipptl=0x8;
									break;
								case IPPROTO_IGMP:
									hsb.type=0x4;
									hsb.patmatch=0x0;
									hsb.urlmch=0x0;
									hsb.sprt=0x0;
									hsb.ipptl=0x1;
									break;
								case IPPROTO_GRE:
									hsb.type=0x1;
									break;
									
							}
							hsb.sip=0xc0a89a6e;
							hsb.dip=0x42660763;	
							hsb.l3csok=0x1;
							hsb.l4csok=0x1;				
							
							hsb.ipfg=0x2;
							hsb.iptos=0x0;				
						
							hsb.ttlst=0x2;	
							hsb.ipfo0_n=0x0;						
							

							if(m!=20)
							{
								hsb.dirtx=1;
								hsb.tcpfg=(1<<1)|(1<<6)|((m>>2)<<2);
								if((m<20)||(m>60))
								{
									hsb.sip=0;
									hsb.sprt=0;
									hsb.dip=0;
									hsb.dprt=0;
									hsb.ipptl=0;
									hsb.ipfg=0;
									hsb.type=0;
									hsb.ttlst=0;
									hsb.l3csok=0;
									hsb.l4csok=0;
									hsb.tcpfg&=(~(uint32)(1<<6));

								}
							}

							break;


						case L2_PPPOE63:
						case L2_VLAN_PPPOE63:			
						case L2_SNAP_PPPOE63:			
						case L2_VLAN_SNAP_PPPOE63:
							hsb.llcothr=0x0;
							hsb.ethtype=0x08863;
							hsb.pppoeif=0;
							hsb.pppoeid=0;
							break;
							
							
						case L2_PPPOE64_IP:
						case L2_VLAN_PPPOE64_IP:			
						case L2_SNAP_PPPOE64_IP:			
						case L2_VLAN_SNAP_PPPOE64_IP:
							hsb.llcothr=0x0;
							hsb.ethtype=0x08864;
							hsb.pppoeif=1;
							hsb.pppoeid=0x0102;
							hsb.ipptl=ip_proto;

							/*  from IP */
							switch(ip_proto)
							{
								case IPPROTO_TCP:
									hsb.type=0x5;
									hsb.tcpfg=0x18;
									hsb.urlmch=0x1;
									hsb.patmatch=0x1;
									hsb.sprt=4166;
									hsb.dprt=80;									
									break;
								case IPPROTO_UDP:
									hsb.type=0x6;
									hsb.urlmch=0x1;
									hsb.patmatch=0x1;									
									hsb.sprt=4166;
									hsb.dprt=80;									
									
									break;
								case IPPROTO_ICMP:
									hsb.type=0x3;
									hsb.urlmch=0x0;
									hsb.patmatch=0x0;									
									hsb.sprt=0x400;
									hsb.ipptl=0x8;
									break;
								case IPPROTO_IGMP:
									hsb.type=0x4;
									hsb.patmatch=0x0;
									hsb.urlmch=0x0;
									hsb.sprt=0x0;
									hsb.ipptl=0x1;
									break;
								case IPPROTO_GRE:
									hsb.type=0x1;
									break;							
							}				
							hsb.sip=0xc0a89a6e;
							hsb.dip=0x42660763;	
							hsb.l3csok=0x1;
							hsb.l4csok=0x1;				
							
							hsb.ipfg=0x2;
							hsb.iptos=0x0;				
						
							hsb.ttlst=0x2;		
							hsb.ipfo0_n=0x0; /*  not tested */
							

							if(m!=20)
							{
								hsb.dirtx=1;
								hsb.tcpfg=(1<<1)|(1<<6)|((m>>2)<<2);
								if((m<20)||(m>60))
								{
									hsb.sip=0;
									hsb.sprt=0;
									hsb.dip=0;
									hsb.dprt=0;
									hsb.ipptl=0;
									hsb.ipfg=0;
									hsb.type=0;
									hsb.ttlst=0;
									hsb.l3csok=0;
									hsb.l4csok=0;
									hsb.tcpfg&=(~(uint32)(1<<6));
								}							
							}
							break;
							
						case L2_OTHER:
						case L2_VLAN_OTHER:
						case L2_SNAP_OTHER:			
						case L2_VLAN_SNAP_OTHER:						
							hsb.llcothr=0x0;
							hsb.ethtype=0x8808;
							hsb.pppoeif=0;




							switch(ip_proto)
							{
								case IPPROTO_TCP:
									hsb.urlmch=0x1;
									hsb.patmatch=0x1;									
									break;
								case IPPROTO_UDP:
							
									hsb.urlmch=0x1;
									hsb.patmatch=0x1;									
									break;
								case IPPROTO_ICMP:
									hsb.urlmch=0x0;
									hsb.patmatch=0x0;									
									break;
								case IPPROTO_IGMP:
									hsb.urlmch=0x0;
									hsb.patmatch=0x0;										
									break;
								case IPPROTO_GRE:							
									break;							
							}			

							
							hsb.pppoeid=0;
							break;
							
						case L2_LLC:
						case L2_VLAN_LLC:
							hsb.llcothr=0x1;
							hsb.ethtype=0xffff;
							hsb.pppoeif=0;
							break;
					}

					switch(j) /*   by VLAN Tag */
					{
						case L2_VLAN_IP:
						case L2_VLAN_SNAP_IP:
						case L2_VLAN_PPPOE63:			
						case L2_VLAN_SNAP_PPPOE63:					
						case L2_VLAN_PPPOE64_IP:			
						case L2_VLAN_SNAP_PPPOE64_IP:						
						case L2_VLAN_OTHER:
						case L2_VLAN_SNAP_OTHER:						
						case L2_VLAN_LLC:			
							hsb.vid=2730;
							hsb.hiprior=5;
							hsb.tagif=1;				
							break;
						default:
							hsb.vid=0;
							hsb.hiprior=0;				
							hsb.tagif=0;	
							break;
					}		

					switch(j) /*  by SNAP */
					{
						case L2_SNAP_IP:
						case L2_VLAN_SNAP_IP:
						case L2_SNAP_PPPOE63:			
						case L2_VLAN_SNAP_PPPOE63:						
						case L2_SNAP_PPPOE64_IP:			
						case L2_VLAN_SNAP_PPPOE64_IP:						
						case L2_SNAP_OTHER:			
						case L2_VLAN_SNAP_OTHER:						
							hsb.snap=0x1;
							break;
						default:
							hsb.snap=0x0;
							break;
					}			

					if(k==IC_TYPE_MODEL)
					{
						modelPktParser(&rhsb,data,0,hsb.len,NULL);
						/* dumpHSB(&rhsb); */
					}
					else
					{
						hsb.len+=4;
						rtl8651_clearAsicAllTable();
						parserInit(k);			
						retval=virtualMacInput(PN_PORT0,data,hsb.len);
						ASSERT(retval==SUCCESS);
					}

					virtualMacGetHsb(&rhsb);	

					while(1)
					{
						retval=swNic_intHandler(&i);			
						if(retval!=1) break;
						swNic_rxThread(i);
					}
					
					/*  CHECK(compareHsb(&hsb,&rhsb)); */
					/*  rtlglue_printf("j=%d\n",j); */
					if(compareHsb(&hsb,&rhsb)==FAILED) 
					{
						errnum++;
						rtlglue_printf("FAILED!!\n");
						return FAILED;
					}

				}
			}
		}

		rtlglue_printf("errnum=%d\n",errnum);
		ASSERT(errnum==0);
		
	}
	return SUCCESS;
}

