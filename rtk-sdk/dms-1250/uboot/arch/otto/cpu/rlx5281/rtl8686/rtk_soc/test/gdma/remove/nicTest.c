/*
* Copyright c                  Realtek Semiconductor Corporation, 2005
* All rights reserved.
* 
* Program : Model code for NIC
* Abstract : 
* Author : Tony Tzong-yn Su (tysu@realtek.com.tw)               
* $Id: nicTest.c,v 1.91 2007-05-03 13:09:24 tony Exp $
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
#include "l2Test.h"
#include "l34Test.h"
#include "vsv_conn.h"
#include "parserTest.h"
#define RTL8651_TBLDRV_LOCAL_H
#include "rtl8651_aclLocal.h"
#include "rtl8651_layer2local.h"

#include "rtl8651_tblDrvFwd.h"
#include "utility.h" /* for inet_addr */

static uint32 countRX=0;
static int PriorityChecksum=0;
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

uint8 *extData=NULL;

extern int32 drrenable;
extern uint32	rxPkthdrRingRefillByDRR[RTL865X_SWNIC_RXRING_MAX_PKTDESC];
extern uint32 QueuePriority[MAX_QUEUE_ENTRY];
extern uint8   QueuePriorityIndex;
int firstTime=TRUE;
__IRAM int32 swNic_isrReclaim(uint32 rxDescIdx,uint32 rxMbufIdx, struct rtl_pktHdr*pPkthdr,struct rtl_mBuf *pMbuf);


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
//	WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);	
	WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)&(~EXCLUDE_CRC));	
	rtl8651_clearAsicAllTable();
	rtl8651_clearAsicPvid();

	while(1)
	{
		int i;
		WRITE_MEM32(SSIR,FULL_RST);
		for(i=0;i<10000;i++) rtlglue_printf("\r");
		WRITE_MEM32(SSIR,TRXRDY);
		virtualMacWaitCycle(10000);							
		if((READ_MEM32(SSIR)&TRXRDY)==TRXRDY) break;
	}		

	WRITE_MEM32(PCRP0, (6<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf |STP_PortST_FORWARDING); /* Jumbo Frame */
	WRITE_MEM32(PCRP1, (1<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf |STP_PortST_FORWARDING ); /* Jumbo Frame */
	WRITE_MEM32(PCRP2, (2<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf |STP_PortST_FORWARDING ); /* Jumbo Frame */
	WRITE_MEM32(PCRP3, (3<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf |STP_PortST_FORWARDING); /* Jumbo Frame */
	WRITE_MEM32(PCRP4, (4<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf |STP_PortST_FORWARDING); /* Jumbo Frame */
	WRITE_MEM32(PCRP5, (5<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf |STP_PortST_FORWARDING); /* Jumbo Frame */
	WRITE_MEM32(PCRP6, (7<<ExtPHYID_OFFSET)|AcptMaxLen_16K|EnablePHYIf |STP_PortST_FORWARDING); /* Jumbo Frame */		

	/* PHY patch */
	{
		uint32 port, status;
		
		WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( 6 << PHYADD_OFFSET ) | ( 9 << REGADD_OFFSET ) |0x0505 );
		while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );
		WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( 6 << PHYADD_OFFSET ) | ( 4 << REGADD_OFFSET ) |0x1F10 );
		while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );
		WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( 6 << PHYADD_OFFSET ) | ( 0 << REGADD_OFFSET ) |0x1200 );
		while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );

		for ( port = 0; port < 5; port++ )	/* Embedded phy id is the same as port id. */
		{
			/* Read */
			WRITE_MEM32( MDCIOCR, COMMAND_READ | ( port << PHYADD_OFFSET ) | ( 0 << REGADD_OFFSET ) );
			do
			{
				status = READ_MEM32( MDCIOSR );

			} while ( ( status & STATUS ) != 0 );

			status &= 0xffff;

			WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( port << PHYADD_OFFSET ) | ( 0 << REGADD_OFFSET ) | 0x200 | status);
			while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );
		}
		
	}		
	

	
	rtl8651_setAsicPvid(hp[5],WAN0_VID);  /*host 5 pvid 0*/
	rtl8651_setAsicPvid(hp[4],9);   /*host 4 pvid 1*/
	rtl8651_setAsicPvid(hp[3],LAN0_VID);  /*host 3 pvid 2*/
	rtl8651_setAsicPvid(hp[2],11);  /*host 2 pvid 3*/
	rtl8651_setAsicPvid(hp[1],11);  /*host 1 pvid 3*/

	
//	rtl8651_setAsicPvid(hp[6],11);  /*host 6 pvid 3*/
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

	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = vlant.untagPortMask= 0x3f;/* (1<<hp[5]); */
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


	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = 0x3f;/* (1<<hp[3]); */
	vlant.untagPortMask = 0; /* tagged */
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
	routet.nhNum = 2; /* index 4~5 */
	routet.nhNxt = 0;
	routet.nhAlgo = 2; /* per-source */
	routet.ipDomain = 0;
	routet.internal=1;
#else
	routet.nhStart = 30;
	routet.nhNum = 2; /* index 4~5 */
	routet.nhNxt = 0;
	routet.nhAlgo = 2; /* per-source */
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
	nxthpt.pppoeIdx = 0; /* WAN0_ISP0_PPPOE_SID */
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0; /* WAN0_ISP0_NPE_IP */
	nxthpt.isPppoe = 0;
	retval = rtl8651_setAsicNextHopTable(0, &nxthpt);       
	ASSERT( retval==SUCCESS );

	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0; /* WAN0_ISP0_PPPOE_SID */
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0; /* WAN0_ISP0_NPE_IP */
	nxthpt.isPppoe = 0;
	retval = rtl8651_setAsicNextHopTable(1, &nxthpt);       
	ASSERT( retval==SUCCESS );


	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 1; /* WAN0_ISP0_PPPOE_SID */
	nxthpt.dvid =1;/* rtl865xc_netIfIndex(WAN1_VID); */
	nxthpt.extIntIpIdx = 0; /*WAN0_ISP0_NPE_IP */
	nxthpt.isPppoe = 1;
	retval = rtl8651_setAsicNextHopTable(30, &nxthpt);       
	ASSERT( retval==SUCCESS );

	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 1;/* 0;  WAN0_ISP0_PPPOE_SID  */
	nxthpt.dvid =0;/* rtl865xc_netIfIndex(WAN1_VID); */
	nxthpt.extIntIpIdx = 0; /* WAN0_ISP0_NPE_IP */
	nxthpt.isPppoe = 0;
	retval = rtl8651_setAsicNextHopTable(31, &nxthpt);       
	ASSERT( retval==SUCCESS );

	return 0;
	
}


static uint8 *hp;

int32  Niclayer4Config( void )
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
	hp = port_number_of_host[0];
	rtl8651_clearRegister();
	rtl8651_clearAsicAllTable();
	rtl8651_clearAsicPvid();

	rtl8651_setAsicPvid(hp[5],WAN0_VID);  /*host 5 pvid 0*/
	rtl8651_setAsicPvid(hp[4],9);   /*host 4 pvid 1*/
	rtl8651_setAsicPvid(hp[3],LAN0_VID);  /*host 3 pvid 2*/
	rtl8651_setAsicPvid(hp[2],11);  /*host 2 pvid 3*/
	rtl8651_setAsicPvid(hp[1],11);  /*host 1 pvid 3*/
	rtl8651_setAsicPvid(hp[6],11);  /*host 6 pvid 3*/
	/* module switch control reg: enable L2/L3/L4/STP, trap 802.1D/GARP to CPU */
	rtl8651_setAsicOperationLayer(4);
	/* TTL control reg: enable TTL-1 operation */
	WRITE_MEM32(ALECR,READ_MEM32(ALECR)|(uint32)EN_TTL1);	
	WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_51B_CPU_REASON);

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
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP1)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP2)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP3)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP4)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP5)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP6)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP7)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP8)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = vlant.untagPortMask = (1<<hp[5]);

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
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP1)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP2)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP3)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP4)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP5)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP6)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP7)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP8)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
/*	vlant.memberPortMask = vlant.untagPortMask = (1<<hp[4])|(1<<6)|(1<<8); */

	/* set this value for test case : testNicRxPkthdr - loop 1 , to EXT2 only */
	vlant.memberPortMask = vlant.untagPortMask = (1<<8);
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
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP1)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP2)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP3)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP4)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP5)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP6)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP7)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP8)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = (1<<hp[3]);
	vlant.untagPortMask = 0; /* tagged */
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
	l2t.memberPortMask = (1<<hp[5]);
	l2t.isStatic = 1;
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(WAN0_ISP_MAC), 0, &l2t);
	/* wan 1 host */
	strtomac(&l2t.macAddr, HOST4_MAC);
	l2t.memberPortMask = (1<<hp[4]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST4_MAC), 0, &l2t);

	/* wan 2 host */
	strtomac(&l2t.macAddr, HOST5_MAC);
	l2t.memberPortMask = (1<<hp[4])|(1<<5)|(1<<6)|(1<<7)|(1<<8);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST5_MAC), 0, &l2t);
	
	/* lan 0 host */
	strtomac(&l2t.macAddr, HOST3_MAC);
	l2t.memberPortMask = (1<<hp[3]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST3_MAC), 0, &l2t);
	/* lan 1 host 1 */
	strtomac(&l2t.macAddr, HOST2_MAC);
	l2t.memberPortMask = (1<<hp[2]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST2_MAC), 0, &l2t);
	/* lan 1 host 0 */
	strtomac(&l2t.macAddr, HOST1_MAC);
	l2t.memberPortMask = (1<<hp[1]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST1_MAC), 0, &l2t);
	/* lan 1 host 5 */
	strtomac(&l2t.macAddr, HOST6_MAC);
	l2t.memberPortMask = (1<<hp[6]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST6_MAC), 0, &l2t);
	    
	/* config pppoe table */
	bzero((void*) &pppoet, sizeof(pppoet));
	pppoet.sessionId = WAN0_ISP_PPPOE_SID;
	rtl8651_setAsicPppoe(0, &pppoet);
	/* config arp table */
	bzero((void*) &arpt, sizeof(arpt));
	/* wan 1 host */
/* 	arpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC); */
/*	strtoip(&ip32, HOST4_IP); */
/*	rtl8651_setAsicArp(ip32 & 0x7, &arpt); */

	/* wan 2 host */
	arpt.nextHopRow = getL2TableRowFromMacStr(HOST5_MAC);
	strtoip(&ip32, HOST5_IP);
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
	routet.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	routet.nhStart = 0;
	routet.nhNum = 2; /* index 0-1 */
	routet.nhNxt = 0;
	routet.nhAlgo = 2; /* per-source */
	routet.ipDomain = 0;
	routet.internal=0;
	rtl8651_setAsicRouting(7, &routet);
	 /* config next hop table */
	 bzero((void*) &nxthpt, sizeof(nxthpt));
	 /* wan 0: load balance traffic through isp0 & isp1 as 3:1 */
	/* isp 0 occupies 3 entries */
	 bzero((void*) &nxthpt, sizeof(nxthpt));
	 /* wan 0: load balance traffic through isp0 & isp1 as 3:1 */
	/* isp 0 occupies 3 entries */
	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0; /* WAN0_ISP0_PPPOE_SID */
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0; /* WAN0_ISP0_NPE_IP */
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(0, &nxthpt);       

	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0; /* WAN0_ISP0_PPPOE_SID */
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0; /* WAN0_ISP0_NPE_IP */
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(1, &nxthpt);       


	
	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 0; /* WAN0_ISP0_PPPOE_SID */
	nxthpt.dvid = rtl865xc_netIfIndex(WAN1_VID);
	nxthpt.extIntIpIdx = 0; /* WAN0_ISP0_NPE_IP */
	nxthpt.isPppoe = 1;
	rtl8651_setAsicNextHopTable(30, &nxthpt);       

	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 0; /*WAN0_ISP0_PPPOE_SID  */
	nxthpt.dvid = rtl865xc_netIfIndex(WAN1_VID);
	nxthpt.extIntIpIdx = 0; /* WAN0_ISP0_NPE_IP */
	nxthpt.isPppoe = 1;
	rtl8651_setAsicNextHopTable(29, &nxthpt);       	
	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 0; /* WAN0_ISP0_PPPOE_SID */
	nxthpt.dvid = rtl865xc_netIfIndex(WAN1_VID);
	nxthpt.extIntIpIdx = 0; /* WAN0_ISP0_NPE_IP  */
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(31, &nxthpt);       


	
	/* config ip table */
	bzero((void*) &ipt, sizeof(ipt));
	/* wan 0 napt */
	strtoip(&ipt.intIpAddr, "0.0.0.0");
	strtoip(&ipt.extIpAddr, WAN1_ISP_PUBLIC_IP);
	rtl8651_setAsicExtIntIpTable(0, &ipt);
	
	/* wan 1 nat */
/*	strtoip(&ipt.intIpAddr, HOST1_IP); */
/*	strtoip(&ipt.extIpAddr, WAN1_ISP_PUBLIC_IP); */
/*	ipt.nat = 1; */
/*	rtl8651_setAsicExtIntIpTable(1, &ipt); */
	return 0;
	
}




struct rtl_pktHdr rxPkthdr;
struct rtl_mBuf rxMbuf;
uint8 rxMdata[2048]={0};

#undef FASTRX_DEBUG

long  rtl865x_fastRx(struct rtl_pktHdr* pPkt)
{

	struct rtl_mBuf *mbuf;

	/* =================================================
		1. Store information of packet
		2. Update counter
		3. Update checksum of packet receiving
	     ================================================= */
	mbuf = pPkt->ph_mbuf;
	memcpy(&rxPkthdr, pPkt, sizeof(rxPkthdr));
	memcpy(&rxMbuf, mbuf, sizeof(rxMbuf));
	memcpy(&rxMdata, mbuf->m_data, mbuf->m_len>2048?2048:mbuf->m_len);

	countRX ++;

	/* Calculate the checksum of RX descriptor ring receiving sequence */
	//printf("==>countRX:%d, descIndex:%d\n",countRX,pPkt->ph_rxPkthdrDescIdx);
	PriorityChecksum += (countRX * (pPkt->ph_rxPkthdrDescIdx + 1));

#ifdef FASTRX_DEBUG
	rtlglue_printf(	"[%s-%d] Receive packet (0x%p) from RX-ring [%d] Length : %d\n",
				__FUNCTION__,
				__LINE__,
				pPkt,
				pPkt->ph_rxPkthdrDescIdx,
				pPkt->ph_len);

	rtlglue_printf(	"\tReason: 0x%x\n",
				pPkt->ph_reason);

	rtlglue_printf(	"\tCountRX: %d, PriorityChecksum: %d\n",
				countRX,
				PriorityChecksum);
#endif

	{
		struct rtl_mBuf* mbuf_p = mbuf;

		while (mbuf_p)
		{
		rtlglue_printf(	"\tCountRX: %d, PriorityChecksum: %d\n",
				countRX,
				PriorityChecksum);
			rtlglue_printf("\tMBUF: 0x%p -- Length %d\n", mbuf_p, mbuf_p->m_len);
			mbuf_p = mbuf_p->m_next;
		}
	}

	swNic_isrReclaim(	pPkt->ph_rxPkthdrDescIdx,
						pPkt->ph_rxdesc,
						pPkt,
						mbuf);
	return 0;
}


long  rtl865x_fastRx_loopback(struct rtl_pktHdr* pPkt)
{
     
	struct rtl_mBuf *mbuf;

	mbuf = pPkt->ph_mbuf;

	countRX ++;
//	rtlglue_printf("rx=%d\n",countRX);

	/*
		Show the address of "countRX" for ICE access
	*/
	if (countRX == 1) 
	{
		rtlglue_printf("countRX at 0x%x\n", (uint32)&countRX);
	}


#if 0

	{
		int ret;
		/* uint32 cnt2=0; */

		struct rtl_pktHdr	*pktHdrTx;
		struct rtl_mBuf	*MblkTx;

		/* memcpy(extData,pPkt->ph_mbuf->m_data,pPkt->ph_mbuf->m_len); */

		MblkTx=mBuf_attachHeader((void*)extData,(uint32)0,2048, 1500,0);
		ASSERT(MblkTx!=NULL);
		pktHdrTx=MblkTx->m_pkthdr;		
		pktHdrTx->ph_portlist=0x1;

		ret=swNic_write(pktHdrTx,1);
		assert(ret==SUCCESS);	

		/*
		{
			uint8 *c=pPkt->ph_mbuf->m_data;				
			if(*c==0xee) countRX=0xf0000000;
		}
		*/

		while(1)
		{
			if((READ_MEM32(CPUIISR)&(TX_DONE_IE1))==(TX_DONE_IE1))
			{				
				break;	
			}
#if 0
			cnt2++;

			if(cnt2>10000)
			{
				rtlglue_printf("wait for TX_DONE_IE. CPUIISR=%x countRX=%x\n",READ_MEM32(CPUIISR),countRX);
				countRX=0xf0000000; 
				break;
			}
#endif
		}

		WRITE_MEM32(CPUIISR, TX_DONE_IE1);
	}
#else
#if 1//2006/11/21
	{
		int ret;
		struct rtl_pktHdr *pktHdrTx;
		struct rtl_mBuf *MblkTx;

		MblkTx = mBuf_attachHeader(	(void*)pPkt->ph_mbuf->m_data,
									(uint32)0,
									2048,
									pPkt->ph_len,
									0);
		ASSERT(MblkTx != NULL);			

		pktHdrTx = MblkTx->m_pkthdr;
		pktHdrTx->ph_portlist = 0x1;
	

		ret = swNic_write(pktHdrTx, 1);
		assert(ret == SUCCESS);

#if 0
		memDump(pktHdrTx,32,"TXpkt");
		memDump(MblkTx,32,"TXmbuf");
		rtlglue_printf("%s %d RX=%d pPkt->ph_len=%d \n",__FUNCTION__,__LINE__,countRX,pPkt->ph_len);
		rtlglue_printf("%s %d RX=%d\n",__FUNCTION__,__LINE__,countRX);
#endif		

		while (1)
		{
			if ((READ_MEM32(CPUIISR)&(TX_DONE_IE1)) == (TX_DONE_IE1))
			{
				break;
			}
		}
		WRITE_MEM32(CPUIISR, TX_DONE_IE1);
	}
#endif
#endif

	swNic_isrReclaim(	pPkt->ph_rxPkthdrDescIdx,
						pPkt->ph_rxdesc,
						pPkt,
						mbuf);

	return 0;
}

uint32 *firstMbufPtr = NULL;

/* ysleu: To add DRR queue, backward compatiable.*/
int configNicExtInit(void)
{
	int retval;

	retval = configNicInit();
	/* enable DRR queue ability */
	drrenable = TRUE;

	return retval;
}

int configNicInit(void)
{
	int retval;

	virtualMacInit();
	if(firstTime==TRUE)
	{
		firstTime=FALSE;				

		retval=mBuf_init(512,  0/* use external cluster pool */, 128, 2048, 0);
		if (retval!=SUCCESS)
		{
			rtlglue_printf("mBuf_init failed...\n");
			return FAILED;
		}
		{
			int32 rxRingSize[6] = {8, 8, 8, 8, 8, 8};
			int32 mbufRingSize = 256;
			int32 txRingSize[2] = {16, 16};

			if (0 != swNic_init(	rxRingSize,
								mbufRingSize,
								txRingSize,
								2048,
								(proc_input_pkt_funcptr_t)rtl865x_fastRx,
								NULL,
								NULL))
			{				
				rtlglue_printf("swNic_init failed........\n");
				return FAILED;
			}
		}

		firstMbufPtr=(uint32*)READ_MEM32(CPURMDCR0);
		

		
	}
	else
	{
		if (swNic_hwSetup() != SUCCESS)
		{
			rtlglue_printf("NIC hardware setup failed");
			return FAILED;
		}

		swNic_reset();
		swNic_txRxSwitch(TRUE,TRUE);		
		swNic_enableSwNicWriteTxFN(TRUE);		

	}
	return SUCCESS;
}

#if 0
/*
 *  This function is to simulate NIC Rx of model code. It is done by tysu before yjlou complete the model code part.
 */
struct rtl_pktHdr*  simulateRX(int QueueID,int to, char *data,uint16 datalen)
{

	struct rtl_pktHdr* pPkt=NULL;
	struct rtl_mBuf* pMbuf;
	int descriptRing=0;

	if(to==0) /* to CPU */
		descriptRing=(READ_VIR16(CPUQDM0+QueueID*2)>>12)&0x7;
	else if(to==1) /* to Ext1 */
		descriptRing=(READ_VIR16(CPUQDM0+QueueID*2)>>8)&0x7;
	else if(to==2) /* to Ext2 */
		descriptRing=(READ_VIR16(CPUQDM0+QueueID*2)>>4)&0x7;
	else if(to==3) /* to Ext3 */
		descriptRing=READ_VIR16(CPUQDM0+QueueID*2)&0x7;
			
	WRITE_VIR32(CPUIIMR,READ_MEM32(CPUIIMR)|RX_DONE_IE_ALL);

	
	switch(descriptRing)
	{
	case 0: 			
		WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|RX_DONE_IE0);
		
		if((*(uint32*)(READ_MEM32(CPURPDCR0))&DESC_SWCORE_OWNED)==0)
		{
			WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|PKTHDR_DESC_RUNOUT_IE0);
			return NULL;
		}

		*(uint32*)READ_MEM32(CPURPDCR0) &=(~DESC_SWCORE_OWNED);
		pPkt=(struct rtl_pktHdr *)((*(uint32*)READ_MEM32(CPURPDCR0))&(~(DESC_OWNED_BIT|DESC_WRAP)));	


		if((*(uint32*)(READ_MEM32(CPURPDCR0))&DESC_WRAP)!=0) 
		{
			WRITE_VIR32(CPURPDCR0,(uint32)&RxPkthdrRing0[0]);	
		}
		else
		{
			WRITE_VIR32(CPURPDCR0,READ_MEM32(CPURPDCR0)+4);										
		}			
		break;	
	case 1: 			
		WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|RX_DONE_IE1);
		
		if((*(uint32*)(READ_MEM32(CPURPDCR1))&DESC_SWCORE_OWNED)==0)
		{
			WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|PKTHDR_DESC_RUNOUT_IE1);
			return NULL;
		}

		*(uint32*)READ_MEM32(CPURPDCR1) &=(~DESC_SWCORE_OWNED);
		pPkt=(struct rtl_pktHdr *)((*(uint32*)READ_MEM32(CPURPDCR1))&(~(DESC_OWNED_BIT|DESC_WRAP)));	


		if((*(uint32*)(READ_MEM32(CPURPDCR1))&DESC_WRAP)!=0) 
		{
			WRITE_VIR32(CPURPDCR1,(uint32)&RxPkthdrRing1[0]);	
		}
		else
		{
			WRITE_VIR32(CPURPDCR1,READ_MEM32(CPURPDCR1)+4);										
		}			
		break;
	case 2: 			
		WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|RX_DONE_IE2);
		
		if((*(uint32*)(READ_MEM32(CPURPDCR2))&DESC_SWCORE_OWNED)==0)
		{
			WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|PKTHDR_DESC_RUNOUT_IE2);
			return NULL;
		}

		*(uint32*)READ_MEM32(CPURPDCR2) &=(~DESC_SWCORE_OWNED);
		pPkt=(struct rtl_pktHdr *)((*(uint32*)READ_MEM32(CPURPDCR2))&(~(DESC_OWNED_BIT|DESC_WRAP)));	


		if((*(uint32*)(READ_MEM32(CPURPDCR2))&DESC_WRAP)!=0) 
		{
			WRITE_VIR32(CPURPDCR2,(uint32)&RxPkthdrRing2[0]);	
		}
		else
		{
			WRITE_VIR32(CPURPDCR2,READ_MEM32(CPURPDCR2)+4);										
		}			
		break;
	case 3: 			
		WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|RX_DONE_IE3);
		
		if((*(uint32*)(READ_MEM32(CPURPDCR3))&DESC_SWCORE_OWNED)==0)
		{
			WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|PKTHDR_DESC_RUNOUT_IE3);
			return NULL;
		}

		*(uint32*)READ_MEM32(CPURPDCR3) &=(~DESC_SWCORE_OWNED);
		pPkt=(struct rtl_pktHdr *)((*(uint32*)READ_MEM32(CPURPDCR3))&(~(DESC_OWNED_BIT|DESC_WRAP)));	


		if((*(uint32*)(READ_MEM32(CPURPDCR3))&DESC_WRAP)!=0) 
		{
			WRITE_VIR32(CPURPDCR3,(uint32)&RxPkthdrRing3[0]);	
		}
		else
		{
			WRITE_VIR32(CPURPDCR3,READ_MEM32(CPURPDCR3)+4);										
		}			
		break;
	case 4: 			
		WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|RX_DONE_IE4);
		
		if((*(uint32*)(READ_MEM32(CPURPDCR4))&DESC_SWCORE_OWNED)==0)
		{
			WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|PKTHDR_DESC_RUNOUT_IE4);
			return NULL;
		}

		*(uint32*)READ_MEM32(CPURPDCR4) &=(~DESC_SWCORE_OWNED);
		pPkt=(struct rtl_pktHdr *)((*(uint32*)READ_MEM32(CPURPDCR4))&(~(DESC_OWNED_BIT|DESC_WRAP)));	


		if((*(uint32*)(READ_MEM32(CPURPDCR4))&DESC_WRAP)!=0) 
		{
			WRITE_VIR32(CPURPDCR4,(uint32)&RxPkthdrRing4[0]);	
		}
		else
		{
			WRITE_VIR32(CPURPDCR4,READ_MEM32(CPURPDCR4)+4);										
		}			
		break;	
	case 5: 			
		WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|RX_DONE_IE5);
		
		if((*(uint32*)(READ_MEM32(CPURPDCR5))&DESC_SWCORE_OWNED)==0)
		{
			WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|PKTHDR_DESC_RUNOUT_IE5);
			return NULL;
		}

		*(uint32*)READ_MEM32(CPURPDCR5) &=(~DESC_SWCORE_OWNED);
		pPkt=(struct rtl_pktHdr *)((*(uint32*)READ_MEM32(CPURPDCR5))&(~(DESC_OWNED_BIT|DESC_WRAP)));	

		if((*(uint32*)(READ_MEM32(CPURPDCR5))&DESC_WRAP)!=0) 
		{
			WRITE_VIR32(CPURPDCR5,(uint32)&RxPkthdrRing5[0]);	
		}
		else
		{
			WRITE_VIR32(CPURPDCR5,READ_MEM32(CPURPDCR5)+4);										
		}			
		break;		
	}


	/*  handle Mbuf */
	if((*(uint32*)(READ_MEM32(CPURMDCR0))&DESC_SWCORE_OWNED)==0)
	{
		WRITE_VIR32(CPUIISR,READ_MEM32(CPUIISR)|MBUF_DESC_RUNOUT_IE0);
		return NULL;
	}

	*(uint32*)READ_MEM32(CPURMDCR0) &=(~DESC_SWCORE_OWNED);
	
	pMbuf=(struct rtl_mBuf *)((*(uint32*)READ_MEM32(CPURMDCR0))&(~(DESC_OWNED_BIT|DESC_WRAP)));

	if((*(uint32*)(READ_MEM32(CPURMDCR0))&DESC_WRAP)!=0) 
	{
		WRITE_VIR32(CPURMDCR0,(uint32)&RxMbufRing0[0]);				
	}
	else
	{
		WRITE_VIR32(CPURMDCR0,READ_MEM32(CPURMDCR0)+4);								
	}



	/*  DMA data	*/
	pPkt->ph_mbuf=pMbuf;
	pPkt->ph_len=datalen;
	pPkt->ph_flags=0x8000; /* if flags have PKTHDR_DRIVERHOLD , will become rx runout. */


	pPkt->ph_portlist=0x1;
	pPkt->ph_type=0;	
	pPkt->ph_queueId = QueueID;

	
	pMbuf->m_pkthdr=pPkt;
	pMbuf->m_next=NULL;
	pMbuf->m_data=pMbuf->m_extbuf=(uint8*)data;
	pMbuf->m_len=datalen;


	return pPkt;

}
#endif

int32 testNicRegs(uint32 caseNo)
{
	int i;
	int retval;
	uint32 orgMemICR;
	uint32 orgMemIMR;
	uint32 orgMemISR;
	uint32 orgMemCPUQDM01;
	uint32 orgMemCPUQDM23;
	uint32 orgMemCPUQDM45;
	return SUCCESS;


	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		return SUCCESS;
					
		retval = model_setTestTarget( i );

		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		
		orgMemICR=READ_MEM32(CPUICR);
		orgMemIMR=READ_MEM32(CPUIIMR);
		orgMemISR=READ_MEM32(CPUIISR);
		orgMemCPUQDM01=READ_MEM32(CPUQDM0);	
		orgMemCPUQDM23=READ_MEM32(CPUQDM2);	
		orgMemCPUQDM45=READ_MEM32(CPUQDM4);	

	
		WRITE_MEM32(CPUICR,0xffffffff);
		if(READ_MEM32(CPUICR)!=0) 
		{
			retval=FAILED;
			goto out;
		}
			
		WRITE_MEM32(CPUICR,0xff00ffff);
		if(READ_MEM32(CPUICR)!=0xf70000ff) 
		{
			retval=FAILED;
			goto out;
		}
		

		WRITE_MEM32(CPUICR,0xffa0ffff);
		if(READ_MEM32(CPUICR)!=0xf70000ff) 
		{
			retval=FAILED;
			goto out;
		}
		

		WRITE_MEM32(CPUICR,0xff10ffff);
		if(READ_MEM32(CPUICR)!=0xf70000ff) 
		{
			retval=FAILED;
			goto out;
		}


		WRITE_MEM32(CPUIIMR,0xffffffff);

		if(READ_MEM32(CPUIIMR)!=0x807e0ffe) 
		{
			retval=FAILED;
			goto out;
		}

		if(i==IC_TYPE_MODEL)
		{
			WRITE_VIR32(CPUIISR,0xffff07fe);
			WRITE_MEM32(CPUIISR,0x80800000);
			if(READ_MEM32(CPUIISR)!=0x7f7f07fe) 
			{
				retval=FAILED;
				goto out;
			}
	

			WRITE_VIR32(CPUIISR,0xffff07fe);
			WRITE_MEM32(CPUIISR,0xffffffff);
			
			if(READ_MEM32(CPUIISR)!=0) 
			{
				retval=FAILED;
				goto out;
			}			
		}
		
		retval=SUCCESS;
		goto out;

	}

out:
		WRITE_MEM32(CPUICR,orgMemICR);
		WRITE_MEM32(CPUIIMR,orgMemIMR);
		WRITE_MEM32(CPUIISR,orgMemISR);
		WRITE_MEM32(CPUQDM0,orgMemCPUQDM01);	
		WRITE_MEM32(CPUQDM2,orgMemCPUQDM23);
		WRITE_MEM32(CPUQDM4,orgMemCPUQDM45);
	
	return retval;
	
}

int32 testNicRx(uint32 caseNo)
{

	int i,j,len=0;
	char *data;
	int retval;

	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{
		countRX=0;	
		retval = model_setTestTarget( j );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		data=rtlglue_malloc(2048);
		ASSERT(data!=NULL);
#ifdef RTL865X_MODEL_KERNEL		
		data=(char *)(UNCACHE_MASK|(uint32)data);		
#endif
		memset(data,0,2048);

		RoutingConfig();

		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		
		parserInit(j);

		for(i=0;i<6;i++)
			WRITE_MEM16(CPUQDM0+i*2,(i<<12));

		for(i=0;i<150;i++)
		{
			int retval;
			
			rtlglue_printf("i=%d\n",i);
			/* Switch Core Operation */
			/* simulate an interrupt (turn  CPUIISR RX_DONE_IE on) */
			
			{
				parserInit(j);
				len=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP)+4;
				data[0]=0xff;
				data[1]=0xff;
				data[2]=0xff;
				data[3]=0xff;
				data[4]=0xff;
				data[5]=0xff;
				data[14]=0x46;
				/* memDump(data,len,"rx"); */
				retval=virtualMacInput(PN_PORT0,data,len);

				if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);

	
				{
					hsb_param_t rhsb;
					virtualMacWaitCycle(15000);
					virtualMacGetHsb(&rhsb);
					/* dumpHSB(&rhsb); */
				}		

			}

			while(1)
			{
				retval=swNic_intHandler(&i);
				if(retval!=1/*TRUE*/) break;
				swNic_rxThread(i);
			}

			


			if(countRX!=(i+1)) 
			{
#ifdef RTL865X_MODEL_KERNEL					
				data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
				rtlglue_free(data);			
				rtlglue_printf("Failed when %dth packet RX!\n",countRX);
				return FAILED;
			}

			/* mBuf_freeMbufChain(pMbuf); */

			
		}

#ifdef RTL865X_MODEL_KERNEL		
		data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
		rtlglue_free(data);		
		return SUCCESS;
	}

	return SUCCESS;
}



int32 testNicTx(uint32 caseNo)
{

	struct rtl_pktHdr	*pktHdr;
	struct rtl_mBuf	*Mblk;

/*

The following packet is a HTTP Request;
===================================================================================
GET / HTTP/1.1
Accept: * / *
Accept-Language: zh-tw
Accept-Encoding: gzip, deflate
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; .NET CLR 1.1.4322)
Host: www.google.com.tw
Connection: Keep-Alive
Cookie: PREF=ID=0c6c50c80799b799:TB=2:LD=zh-TW:TM=1116899852:LM=1119873714:GM=1:S=lPKPB0qpAAJvXssT
===================================================================================

*/

	char org_data[] = {


	0x00, 0x00, 0xe8, 0x66, 0x46, 0x85,   /* DMAC */
	0x00, 0x0c, 0x6e, 0x3a, 0x5d, 0x66,   /* SMAC */

/*	0x05,0xff,0xaa,0xaa,0x03,0,0,0,  */ /* for SNAP */

	
	0x08, 0x00,   /* Ether Type */

	/* IP header */

	0x45, 0x00, 0x01, 0x67, 0xa4, 0xd2, 0x40, 0x00, 
	0x80, 0x06, 0xaf, 0xde, 0xc0, 0xa8, 0x9a, 0x6e, 
	0x42, 0x66, 0x07, 0x63, 


	/* TCP header */

	0x10, 0x46, 0x00, 0x50, 0x92, 0x11, 
	0xa2, 0xdf, 0x4c, 0xa5, 0x5c, 0xdd, 0x50, 0x18,
	0x44, 0x70, 0xbc, 0x2c, 0x00, 0x00,


	/*  Payload */

	0x47, 0x45, 0x54, 0x20, 0x2f, 0x20, 0x48, 0x54, 	0x54, 0x50, 0x2f, 0x31, 0x2e, 0x31, 0x0d, 0x0a, 
	0x41, 0x63, 0x63, 0x65, 0x70, 0x74, 0x3a, 0x20, 	0x2a, 0x2f, 0x2a, 0x0d, 0x0a, 0x41, 0x63, 0x63, 
	0x65, 0x70, 0x74, 0x2d, 0x4c, 0x61, 0x6e, 0x67, 	0x75, 0x61, 0x67, 0x65, 0x3a, 0x20, 0x7a, 0x68, 
	0x2d, 0x74, 0x77, 0x0d, 0x0a, 0x41, 0x63, 0x63, 	0x65, 0x70, 0x74, 0x2d, 0x45, 0x6e, 0x63, 0x6f, 
	0x64, 0x69, 0x6e, 0x67, 0x3a, 0x20, 0x67, 0x7a, 	0x69, 0x70, 0x2c, 0x20, 0x64, 0x65, 0x66, 0x6c, 
	0x61, 0x74, 0x65, 0x0d, 0x0a, 0x55, 0x73, 0x65, 	0x72, 0x2d, 0x41, 0x67, 0x65, 0x6e, 0x74, 0x3a, 
	0x20, 0x4d, 0x6f, 0x7a, 0x69, 0x6c, 0x6c, 0x61, 	0x2f, 0x34, 0x2e, 0x30, 0x20, 0x28, 0x63, 0x6f, 
	0x6d, 0x70, 0x61, 0x74, 0x69, 0x62, 0x6c, 0x65, 	0x3b, 0x20, 0x4d, 0x53, 0x49, 0x45, 0x20, 0x36, 
	0x2e, 0x30, 0x3b, 0x20, 0x57, 0x69, 0x6e, 0x64, 	0x6f, 0x77, 0x73, 0x20, 0x4e, 0x54, 0x20, 0x35, 
	0x2e, 0x30, 0x3b, 0x20, 0x2e, 0x4e, 0x45, 0x54, 	0x20, 0x43, 0x4c, 0x52, 0x20, 0x31, 0x2e, 0x31, 
	0x2e, 0x34, 0x33, 0x32, 0x32, 0x29, 0x0d, 0x0a, 	0x48, 0x6f, 0x73, 0x74, 0x3a, 0x20, 0x77, 0x77, 
	0x77, 0x2e, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 	0x2e, 0x63, 0x6f, 0x6d, 0x2e, 0x74, 0x77, 0x0d, 
	0x0a, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 	0x69, 0x6f, 0x6e, 0x3a, 0x20, 0x4b, 0x65, 0x65, 
	0x70, 0x2d, 0x41, 0x6c, 0x69, 0x76, 0x65, 0x0d, 	0x0a, 0x43, 0x6f, 0x6f, 0x6b, 0x69, 0x65, 0x3a, 
	0x20, 0x50, 0x52, 0x45, 0x46, 0x3d, 0x49, 0x44, 	0x3d, 0x30, 0x63, 0x36, 0x63, 0x35, 0x30, 0x63, 
	0x38, 0x30, 0x37, 0x39, 0x39, 0x62, 0x37, 0x39, 	0x39, 0x3a, 0x54, 0x42, 0x3d, 0x32, 0x3a, 0x4c, 
	0x44, 0x3d, 0x7a, 0x68, 0x2d, 0x54, 0x57, 0x3a, 	0x54, 0x4d, 0x3d, 0x31, 0x31, 0x31, 0x36, 0x38, 
	0x39, 0x39, 0x38, 0x35, 0x32, 0x3a, 0x4c, 0x4d, 	0x3d, 0x31, 0x31, 0x31, 0x39, 0x38, 0x37, 0x33, 
	0x37, 0x31, 0x34, 0x3a, 0x47, 0x4d, 0x3d, 0x31, 	0x3a, 0x53, 0x3d, 0x6c, 0x50, 0x4b, 0x50, 0x42, 
	0x30, 0x71, 0x70, 0x41, 0x41, 0x4a, 0x76, 0x58, 	0x73, 0x73, 0x54, 0x0d, 0x0a, 0x0d, 0x0a };


	int i,j;

	int retval;


	

	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{
		retval = model_setTestTarget( j );		
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();
		rtl8651_clearAsicPvid();
	
		/* module switch control reg: enable L2/L3/L4/STP, trap 802.1D/GARP to CPU */
		rtl8651_setAsicOperationLayer(3);

		retval=configNicInit();
		ASSERT(retval==SUCCESS);		
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		
		/* parserInit(j);*/

		WRITE_MEM32(CPUIIMR,READ_MEM32(CPUIIMR)|TX_DONE_IE1);

		for(i=0;i<72;i++)
		{
			int ret;
			char *data;
			enum PORT_MASK loop, mask;
			enum PORT_NUM port;
			int len;

#if 0
			/* full test for all port combination */
			for( loop = PM_PORT_0; loop<=(PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5); loop++ )
#else
			/* simple test, only one port */
			for( loop = PM_PORT_0; loop<=PM_PORT_0; loop++ )
#endif
			{
				rtlglue_printf("i=%d loop=0x%x\n", i, loop );
			
				data=rtlglue_malloc(sizeof(org_data));
				ASSERT(data!=NULL);
#ifdef RTL865X_MODEL_KERNEL						
				data=(char *)(UNCACHE_MASK|(uint32)data);	
#endif
				memcpy(data,org_data,sizeof(org_data));
				
				Mblk=mBuf_attachHeader((void*)data,(uint32)0,2048, sizeof(org_data),0);
				ASSERT(Mblk!=NULL);

				
				pktHdr=Mblk->m_pkthdr;
				pktHdr->ph_portlist = loop;


				ret=swNic_write(pktHdr,1); /* send with high priority */

				rtlglue_printf("pkthdr=%p mbuf=%p data=%p mbuf->m_len=%d\n",pktHdr,Mblk,Mblk->m_data,Mblk->m_len);
				/* memDump(Mblk->m_data,ntohs(Mblk->m_len),"data"); */


				while(1)
				{
					if(READ_MEM32(CPUIISR)&TX_DONE_IP1)
					{
						WRITE_MEM32(CPUIISR,TX_DONE_IP1|TX_ALL_DONE_IP1|TX_ALL_DONE_IP0);
						break;					
						
					}

					virtualMacWaitCycle(2000);
					
				}			

				/* polling all ports output */
				for( port = PN_PORT0; port <= PN_PORT5; port++ )
				{
					if ( loop&(1<<port) )
					{
						mask = loop;
						len=_PKT_LEN;
						retval = virtualMacOutput( &mask, sharedPkt[0], &len );

						ASSERT( retval == SUCCESS );
					}
				}

				/*
				{
					hsb_param_t rhsb;				
					vsv_waitCycle(conn_client,1000);				
					virtualMacGetHsb(&rhsb);
					dumpHSB(&rhsb);
				}
				*/

							
				/* mBuf_freeMbufChain(Mblk); */
			
				ASSERT(ret==SUCCESS);
#ifdef RTL865X_MODEL_KERNEL						
				data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
				rtlglue_free(data);
				
			}
		}
		
		return SUCCESS;
	}

	return SUCCESS;

}


/*
 *  In this case, we will generate 4 packets into NIC Tx.
 *
 *    0. DMAC --:00:--:--:--:--  ==> Low queue
 *    1. DMAC --:01:--:--:--:--  ==> High queue
 *    2. DMAC --:02:--:--:--:--  ==> Low queue
 *    3. DMAC --:03:--:--:--:--  ==> High queue
 *
 *  We expect the output sequence is :
 *    1 -> 3 -> 0 -> 2
 */
int32 testNicTxQueuePriority(uint32 caseNo)
{

/*
	uint32 QueuePrioritySample[4]={0};
	uint8 QueuePrioritySampleIndex=0;
*/	
	char org_data[] = {

	0x00, 0x00, 0xe8, 0x66, 0x46, 0x85,   /* DMAC */
	0x00, 0x0c, 0x6e, 0x3a, 0x5d, 0x66,   /* SMAC */

/*	0x05,0xff,0xaa,0xaa,0x03,0,0,0, */ /* for SNAP  */
	
/*	0x08, 0x00,     Ether Type */	
	0x08, 0x01,  /* Ether Type */


	/* IP header */
	0x45, 0x00, 0x01, 0x67, 0xa4, 0xd2, 0x40, 0x00, 
	0x80, 0x06, 0xaf, 0xde, 0xc0, 0xa8, 0x9a, 0x6e, 
	0x42, 0x66, 0x07, 0x63, 

	/* TCP header */

	0x10, 0x46, 0x00, 0x50, 0x92, 0x11, 
	0xa2, 0xdf, 0x4c, 0xa5, 0x5c, 0xdd, 0x50, 0x18,
	0x44, 0x70, 0xbc, 0x2c, 0x00, 0x00,


	/* Payload */
	

	0x47, 0x45, 0x54, 0x20, 0x2f, 0x20, 0x48, 0x54, 	0x54, 0x50, 0x2f, 0x31, 0x2e, 0x31, 0x0d, 0x0a, 
	0x41, 0x63, 0x63, 0x65, 0x70, 0x74, 0x3a, 0x20, 	0x2a, 0x2f, 0x2a, 0x0d, 0x0a, 0x41, 0x63, 0x63, 
	0x65, 0x70, 0x74, 0x2d, 0x4c, 0x61, 0x6e, 0x67, 	0x75, 0x61, 0x67, 0x65, 0x3a, 0x20, 0x7a, 0x68, 
	0x2d, 0x74, 0x77, 0x0d, 0x0a, 0x41, 0x63, 0x63, 	0x65, 0x70, 0x74, 0x2d, 0x45, 0x6e, 0x63, 0x6f, 
	0x64, 0x69, 0x6e, 0x67, 0x3a, 0x20, 0x67, 0x7a, 	0x69, 0x70, 0x2c, 0x20, 0x64, 0x65, 0x66, 0x6c, 
	0x61, 0x74, 0x65, 0x0d, 0x0a, 0x55, 0x73, 0x65, 	0x72, 0x2d, 0x41, 0x67, 0x65, 0x6e, 0x74, 0x3a, 
	0x20, 0x4d, 0x6f, 0x7a, 0x69, 0x6c, 0x6c, 0x61, 	0x2f, 0x34, 0x2e, 0x30, 0x20, 0x28, 0x63, 0x6f, 
	0x6d, 0x70, 0x61, 0x74, 0x69, 0x62, 0x6c, 0x65, 	0x3b, 0x20, 0x4d, 0x53, 0x49, 0x45, 0x20, 0x36, 
	0x2e, 0x30, 0x3b, 0x20, 0x57, 0x69, 0x6e, 0x64, 	0x6f, 0x77, 0x73, 0x20, 0x4e, 0x54, 0x20, 0x35, 
	0x2e, 0x30, 0x3b, 0x20, 0x2e, 0x4e, 0x45, 0x54, 	0x20, 0x43, 0x4c, 0x52, 0x20, 0x31, 0x2e, 0x31, 
	0x2e, 0x34, 0x33, 0x32, 0x32, 0x29, 0x0d, 0x0a, 	0x48, 0x6f, 0x73, 0x74, 0x3a, 0x20, 0x77, 0x77, 
	0x77, 0x2e, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 	0x2e, 0x63, 0x6f, 0x6d, 0x2e, 0x74, 0x77, 0x0d, 
	0x0a, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 	0x69, 0x6f, 0x6e, 0x3a, 0x20, 0x4b, 0x65, 0x65, 
	0x70, 0x2d, 0x41, 0x6c, 0x69, 0x76, 0x65, 0x0d, 	0x0a, 0x43, 0x6f, 0x6f, 0x6b, 0x69, 0x65, 0x3a, 
	0x20, 0x50, 0x52, 0x45, 0x46, 0x3d, 0x49, 0x44, 	0x3d, 0x30, 0x63, 0x36, 0x63, 0x35, 0x30, 0x63, 
	0x38, 0x30, 0x37, 0x39, 0x39, 0x62, 0x37, 0x39, 	0x39, 0x3a, 0x54, 0x42, 0x3d, 0x32, 0x3a, 0x4c, 
	0x44, 0x3d, 0x7a, 0x68, 0x2d, 0x54, 0x57, 0x3a, 	0x54, 0x4d, 0x3d, 0x31, 0x31, 0x31, 0x36, 0x38, 
	0x39, 0x39, 0x38, 0x35, 0x32, 0x3a, 0x4c, 0x4d, 	0x3d, 0x31, 0x31, 0x31, 0x39, 0x38, 0x37, 0x33, 
	0x37, 0x31, 0x34, 0x3a, 0x47, 0x4d, 0x3d, 0x31, 	0x3a, 0x53, 0x3d, 0x6c, 0x50, 0x4b, 0x50, 0x42, 
	0x30, 0x71, 0x70, 0x41, 0x41, 0x4a, 0x76, 0x58, 	0x73, 0x73, 0x54, 0x0d, 0x0a, 0x0d, 0x0a 

	};

	int i,j,k;

	int retval;


	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{
		rtl865x_tblAsicDrv_vlanParam_t vlant;
		rtl865x_tblAsicDrv_intfParam_t intf;

		retval = model_setTestTarget( j );		
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */


		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();
		rtl8651_clearAsicPvid();
		/* module switch control reg: enable L2/L3/L4/STP, trap 802.1D/GARP to CPU */
		rtl8651_setAsicOperationLayer(3);

		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		
/*		parserInit(j);	*/

		retval = rtl8651_setAsicOperationLayer(2);
		ASSERT(retval==SUCCESS);		

		 /* config vlan table */
		 bzero((void*) &intf, sizeof(intf));
		/* wan 0 */
		strtomac(&intf.macAddr, GW_WAN0_MAC);
		intf.macAddrNumber = 1;
		intf.vid = 0;
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
		vlant.memberPortMask = vlant.untagPortMask= 0x3f;
		retval = rtl8651_setAsicVlan(intf.vid,&vlant);
		ASSERT( retval==SUCCESS );


#if 0 /* old code to skip translator model */			
		QueuePriorityIndex=0;
#endif

		for(i=0;i<4;i++)
		{
			int ret;
#ifdef _LITTLE_ENDIAN		
			int z;
#endif
			char *data;
			struct rtl_pktHdr	*pktHdr;
			struct rtl_mBuf	*Mblk;			
			data=rtlglue_malloc(sizeof(org_data));			
			ASSERT(data!=NULL);
#ifdef RTL865X_MODEL_KERNEL					
			data=(char *)(UNCACHE_MASK|(uint32)data);	
#endif
			memcpy(data,org_data,sizeof(org_data));
			
			data[1]=i;

#ifdef _LITTLE_ENDIAN
			for(z=0;z<sizeof(org_data);z+=4)
			{
				uint32 *p,q;
				p=(uint32*)&data[z];
				q=*p;
				*p=ntohl(q);
			}
#endif			
			
			Mblk=mBuf_attachHeader((void*)data,(uint32)0,2048, sizeof(org_data),0);
			ASSERT(Mblk!=NULL);			
			pktHdr=Mblk->m_pkthdr;
			
			pktHdr->ph_portlist=0x1;
#if 0 /* old code to skip translator model */			
			QueuePrioritySample[QueuePrioritySampleIndex++]=(uint32)pktHdr;
#endif 

			swNic_enableSwNicWriteTxFN(FALSE);
			if(i%2==1)
			{				
				ret=swNic_write(pktHdr,1);			
			}
			else
			{
				ret=swNic_write(pktHdr,0);			
			}
			swNic_enableSwNicWriteTxFN(TRUE);

			/* mBuf_freeMbufChain(Mblk); */


			ASSERT(ret==SUCCESS);
		}

		WRITE_MEM32(CPUICR , READ_MEM32(CPUICR) | TXFD );

#if 0 /* old code to skip translator model */
		if(j==IC_TYPE_MODEL)
		{
			if(QueuePriority[0]!=QueuePrioritySample[1]) return FAILED;
			if(QueuePriority[1]!=QueuePrioritySample[3]) return FAILED;
			if(QueuePriority[2]!=QueuePrioritySample[0]) return FAILED;
			if(QueuePriority[3]!=QueuePrioritySample[2]) return FAILED;
		}
		else
#endif
		{
			uint8 data[2048]={0};
			enum PORT_MASK port=PM_PORT_0;
			int len=2048;
			int retval=0;

			while(1)
			{
				if((READ_MEM32(CPUIISR)&(TX_ALL_DONE_IE1|TX_ALL_DONE_IE0))==(TX_ALL_DONE_IE1|TX_ALL_DONE_IE0))
				{
					WRITE_MEM32(CPUIISR,TX_ALL_DONE_IE1);
					WRITE_MEM32(CPUIISR,TX_ALL_DONE_IE0);
					break;					
					
				}
/*				rtlglue_printf("READ_MEM32(CPUIISR)=%x\n",READ_MEM32(CPUIISR));	*/
			}

			virtualMacWaitCycle( 10000 ); /* try and error value */
			
			k=0;
			while(1)
			{
				port=PM_PORT_0;
				len=2048;
				retval=virtualMacOutput(&port, data, &len);
				if(retval==SUCCESS)
				{
/*
					rtlglue_printf("len=%d k=%d - date[1] : %d\n",len,k, data[1]);
					memDump(data,len,"data");
*/
					switch(k)
					{
						case 0:
							ASSERT(data[1]==1);
							break;
						case 1:
							ASSERT(data[1]==3);
							break;
						case 2:
							ASSERT(data[1]==0);
							break;
						case 3:
							ASSERT(data[1]==2);
							return SUCCESS;
							break;
					
					}
					k++;
				}
				else
				{
					rtlglue_printf("virtualMacOutput FAILED .......\n");
					return FAILED;
				}
			}
		}
		return SUCCESS;
		
	}


	return SUCCESS;

}



int32 testNicRxRunout(uint32 caseNo)
{

	int i,j,k;

	int retval;
	countRX=0;


	
	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{
		int len=0;
		char data[2048]={0};
		retval = model_setTestTarget( j );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */


		RoutingConfig();
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		
		parserInit(j);

		for(i=0;i<6;i++)
			WRITE_MEM16(CPUQDM0+i*2,(i<<12));

#if 0 /* for old test case */
		if(j==IC_TYPE_MODEL)
		{
			len=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP)+4;
			
			for(k=0;k<=15;k++)
			{
				if(k<=10)
					simulateRX(0,TO_CPU,data,len); /* after 8th runout */
				
				if(k==11) simulateRX(5,TO_CPU,data,len);	/* 9th */
				if(k==12) simulateRX(3,TO_CPU,data,len);	 /* 10th */
				if(k==13) simulateRX(4,TO_CPU,data,len);	 /* 11th */
				if(k==14) simulateRX(2,TO_CPU,data,len);	 /* 12th */
				if(k==15) simulateRX(5,TO_CPU,data,len);	 /* 13th */

				if((k==10)||(k==15))
				{
					/* Swich Core interrupt to notify the RISC that packets is ready	 */
					while(1)
					{
						retval=swNic_intHandler(&i);
						if(retval!=1) break;
						swNic_rxThread(i);
						virtualMacWaitCycle(8000); /* waitting for pkthdr change to SWITCH Owned */
						/* when driver clear the runout bit, hardware will read descript ring immediately */
					}

					/* if k==10 , cpu receive packet and set descript ring to switch owned fixed runout issue */
					/* if k==15 , cpu receive all other descript ring make sure NIC RX work correctly after runout */
				}
			}

			
		}
		else
#endif			
		{
			hsb_param_t rhsb;
			parserInit(j);
			
			WRITE_MEM32(QIDDPCR,0x10|(1<<PBP_PRI_OFFSET)); /* QueueID decision priority control register, set 802.1Q to first priority */
			/* WRITE_MEM32(QNUMCR,0x1b6db6); */ /* set all Port to 6 queue */
			WRITE_MEM32(QNUMCR,0x189249); /* set CPU Port to 6 queue */
			
			for(k=0x28;k<=0x78;k+=4)
			{
				/* set QDBFCRP0G0 ~  QDBFCRP6G2 */
				WRITE_MEM32(SBFCTR+k,0x00000006);
			}
			
			for(k=0x7c;k<=0xcc;k+=4)
			{
				WRITE_MEM32(SBFCTR+k,0x00000002); /* per port per group setting for packet threshold */
			}
			WRITE_MEM32(PQPLGR,0x1); /* per-class-queue physical length gap register */
			WRITE_MEM32(UPTCMCR5,0xb6c688); /* set Priority to QueueID */
			WRITE_MEM32(CPUQIDMCR0,0x55543210); /* set DP to QueueID mapping */
			WRITE_MEM32(LPTM8021Q,0xb6c688); /* linear mapping for 802.1Q priority */
			WRITE_MEM32(QRR,1); /* write one to apply setting value */
			/* FIXME: waitting for apply ok here */
			

			for(k=0;k<=15;k++)
			{
				len=_createSourcePacket(data,L2_VLAN_IP,20,IPPROTO_TCP)+4;
				data[0]=0xff;
				data[1]=0xff;
				data[2]=0xff;
				data[3]=0xff;
				data[4]=0xff;
				data[5]=0xff;

				if(k<=10)
				{
					data[14]=0<<5; /* Priority , CFI , vlanID high bits */
				}
				else
				{
					if(k==11) data[14]=5<<5;
					if(k==12) data[14]=3<<5;
					if(k==13) data[14]=4<<5;
					if(k==14) data[14]=2<<5;
					if(k==15) data[14]=5<<5;
				}
				
				data[15]=0; /* vlan ID low bits */
				data[18]=0x46; /* parse failed, direct TX to CPU */
				/* memDump(data,32,"rx"); */
				retval=virtualMacInput(PN_PORT0,data,len);

				if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);
				
				
				virtualMacGetHsb(&rhsb);
				/* dumpHSB(&rhsb); */
				rtlglue_printf("hsb.hiprior=%d\n",rhsb.hiprior);



				if((k==10)||(k==15))
				{
					virtualMacWaitCycle(250000);
				
					while(1)
					{
						/* Swich Core interrupt to notify the RISC that packets is ready	 */
						retval=swNic_intHandler(&i);
						if(retval!=1) break;
						swNic_rxThread(i);

					}

						/* if k==10 , cpu receive packet and set descript ring to switch owned fixed runout issue */
						/* if k==15 , cpu receive all other descript ring make sure NIC RX work correctly after runout */
					
				}				
			}
		}

		if(!((countRX==8+5+3) || (countRX==8+5) ))
		{
			rtlglue_printf("Failed when %dth packet RX!\n",countRX);
			return FAILED;
		}

		return SUCCESS;

		/* mBuf_freeMbufChain(pMbuf); */
		
	}

	return SUCCESS;
}


int32 testNicRxPriority(uint32 caseNo)
{

	int i,j,k,len=0;
	char data[2048]={0};

	int retval;
	countRX=0;
	PriorityChecksum=0;

	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{
		retval = model_setTestTarget( j );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		RoutingConfig();		
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		
		parserInit(j);

		for(i=0;i<6;i+=2)
		{
			uint32 value;
			value=(i<<28)|((i+1)<<12);
			rtlglue_printf("i<<28=%x (i+1)<<12=%x value=%x htonl(v)=%x\n",i<<28,(i+1)<<12,value,htonl(value));
			/* WRITE_MEM16(CPUQDM0+i*2,(i<<12)); */
			WRITE_MEM32(CPUQDM0+i*2,value);
		}


#if 0
		if(j==IC_TYPE_MODEL)
		{
			len=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP)+4;
			simulateRX(0,TO_CPU,data,len);			
			simulateRX(5,TO_CPU,data,len);	
			simulateRX(3,TO_CPU,data,len);	
			simulateRX(4,TO_CPU,data,len);	
			simulateRX(2,TO_CPU,data,len);	
			simulateRX(5,TO_CPU,data,len);	
			simulateRX(1,TO_CPU,data,len);			
		}
		else			
#endif			
		{
			hsb_param_t rhsb;
			parserInit(j);




			WRITE_MEM32(QIDDPCR,0x10|(1<<PBP_PRI_OFFSET)); /* QueueID decision priority control register, set 802.1Q to first priority */
			/* WRITE_MEM32(QNUMCR,0x1b6db6); */ /* set all Port to 6 queue */
			WRITE_MEM32(QNUMCR,0x189249); /* set CPU Port to 6 queue */
			
			for(k=0x28;k<=0x78;k+=4)
			{
				/* set QDBFCRP0G0 ~  QDBFCRP6G2 */
				WRITE_MEM32(SBFCTR+k,0x00000006);
			}
			
			for(k=0x7c;k<=0xcc;k+=4)
			{
				WRITE_MEM32(SBFCTR+k,0x00000002); /* per port per group setting for packet threshold */
			}
			WRITE_MEM32(PQPLGR,0x1); /* per-class-queue physical length gap register */
			WRITE_MEM32(UPTCMCR5,0xb6c688); /* set Priority to QueueID */
			WRITE_MEM32(CPUQIDMCR0,0x55543210); /* set DP to QueueID mapping */
			WRITE_MEM32(LPTM8021Q,0xb6c688); /* linear mapping for 802.1Q priority */
			WRITE_MEM32(QRR,1); /* write one to apply setting value */

			/* FIXME: waitting for apply ok here */

			for(k=0;k<7;k++)
			{
				len=_createSourcePacket(data,L2_VLAN_IP,20,IPPROTO_TCP)+4;
				data[0]=0xff;
				data[1]=0xff;
				data[2]=0xff;
				data[3]=0xff;
				data[4]=0xff;
				data[5]=0xff;

				switch(k)
				{
					case 0:
						 data[14]=0;
						 break;
					case 1:
						 data[14]=5<<5;
						 break;						
					case 2:
						 data[14]=3<<5;
						 break;						
					case 3:
						 data[14]=4<<5;
						 break;
					case 4:
						 data[14]=2<<5;
						 break;
					case 5:
						 data[14]=5<<5;
						 break;
					case 6:						
						 data[14]=1<<5;
						 break;
				}

				
				data[15]=0; /* vlan ID low bits */
				data[18]=0x46; /* parse failed, direct TX to CPU */

				rtlglue_printf("k=%d\n",k);
				/* memDump(data,32,"rx"); */
				retval=virtualMacInput(PN_PORT0,data,len);
				if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);				
				virtualMacWaitCycle(15000);
				virtualMacGetHsb(&rhsb);
				/* dumpHSB(&rhsb); */
				rtlglue_printf("hsb.hiprior=%d\n",rhsb.hiprior);
				
			}		

		}

/*
swNic_dumpPkthdrDescRing();
swNic_dumpMbufDescRing();	
*/		

		/* Swich Core interrupt to notify the RISC that packets is ready	 */
		while(1)
		{
			retval=swNic_intHandler(&i);			
			if(retval!=1) break;
			swNic_rxThread(i);
		}
		

/*
swNic_dumpPkthdrDescRing();
swNic_dumpMbufDescRing();	
*/	


		if(PriorityChecksum!=83)
		{
			rtlglue_printf("pkt receive priority failed!\n");
			return FAILED;
		}


		if(countRX!=7) 
		{
			rtlglue_printf("Failed when %dth packet RX!\n",countRX);
			return FAILED;
		}

		return SUCCESS;


		/* mBuf_freeMbufChain(pMbuf); */
		
	}

	return SUCCESS;
}


int32 testNicRxQueueidDescringMapping(uint32 caseNo)
{

	int i,j,k,len=0;
	char data[2048]={0};

	int retval;
	countRX=0;
	PriorityChecksum=0;

	
	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{
		retval = model_setTestTarget( j );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		RoutingConfig();
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		
		parserInit(j);		

		for(i=0;i<6;i++)
			WRITE_MEM16(CPUQDM0+i*2,((i/2)<<12));

#if 0
		if(j==IC_TYPE_MODEL)
		{		
			len=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP)+4;
			simulateRX(0,TO_CPU,data,len);	/* to Desc0 */
			simulateRX(5,TO_CPU,data,len);	/* to Desc2 */
			simulateRX(3,TO_CPU,data,len);	/* to Desc1 */
			simulateRX(4,TO_CPU,data,len);	/* to Desc2 */
			simulateRX(2,TO_CPU,data,len);	/* to Desc1 */
			simulateRX(5,TO_CPU,data,len);	/* to Desc2 */	
			simulateRX(1,TO_CPU,data,len);	/* to Desc0 */
		}
		else
#endif					
		{
			hsb_param_t rhsb;
			parserInit(j);
			
			WRITE_MEM32(QIDDPCR,0x10|(1<<PBP_PRI_OFFSET)); /* QueueID decision priority control register, set 802.1Q to first priority */
			/* WRITE_MEM32(QNUMCR,0x1b6db6); */ /* set all Port to 6 queue */
			WRITE_MEM32(QNUMCR,0x189249); /* set CPU Port to 6 queue */
			
			for(k=0x28;k<=0x78;k+=4)
			{
				/* set QDBFCRP0G0 ~  QDBFCRP6G2 */
				WRITE_MEM32(SBFCTR+k,0x00000006);
			}
			
			for(k=0x7c;k<=0xcc;k+=4)
			{
				WRITE_MEM32(SBFCTR+k,0x00000002); /* per port per group setting for packet threshold */
			}
			WRITE_MEM32(PQPLGR,0x1); /* per-class-queue physical length gap register */
			WRITE_MEM32(UPTCMCR5,0xb6c688); /* set Priority to QueueID */
			WRITE_MEM32(CPUQIDMCR0,0x55543210); /* set DP to QueueID mapping */
			WRITE_MEM32(LPTM8021Q,0xb6c688); /* linear mapping for 802.1Q priority */
			WRITE_MEM32(QRR,1); /* write one to apply setting value */
			
			/* FIXME: waitting for apply ok here */
			

			for(k=0;k<7;k++)
			{
				len=_createSourcePacket(data,L2_VLAN_IP,20,IPPROTO_TCP)+4;
				data[0]=0xff;
				data[1]=0xff;
				data[2]=0xff;
				data[3]=0xff;
				data[4]=0xff;
				data[5]=0xff;

				switch(k)
				{
					case 0:
						 data[14]=0<<5;
						 break;
					case 1:
						 data[14]=5<<5;
						 break;						
					case 2:
						 data[14]=3<<5;
						 break;						
					case 3:
						 data[14]=4<<5;
						 break;
					case 4:
						 data[14]=2<<5;
						 break;
					case 5:
						 data[14]=5<<5;
						 break;
					case 6:						
						 data[14]=1<<5;
						 break;
				}
				
				data[15]=0; /* vlan ID low bits */
				data[18]=0x46; /* parse failed, direct TX to CPU */

				rtlglue_printf("k=%d\n",k);
				/* memDump(data,32,"rx"); */
				retval=virtualMacInput(PN_PORT0,data,len);

				if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);
				
				virtualMacWaitCycle(15000);
				virtualMacGetHsb(&rhsb);
				/*dumpHSB(&rhsb); */
				rtlglue_printf("hsb.hiprior=%d\n",rhsb.hiprior);

			}		

		}

		/* Swich Core interrupt to notify the RISC that packets is ready	 */
		while(1)
		{
			retval=swNic_intHandler(&i);			
			if(retval!=1) break;
			swNic_rxThread(i);
		}

		if(PriorityChecksum!=49)
		{
			rtlglue_printf("pkt receive priority failed!\n");
			return FAILED;
		}


		if(countRX!=7) 
		{
			rtlglue_printf("Failed when %dth packet RX!\n",countRX);
			return FAILED;
		}

		return SUCCESS;


		/* mBuf_freeMbufChain(pMbuf); */
		
	}

	return SUCCESS;
}



int32 testNicTxPppoeTagOffload(uint32 caseNo)
{
	int i,j;

	int retval;


	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{

		int ret;
#ifdef _LITTLE_ENDIAN		
		int z;
#endif
		char *data;
		int datalen=0;
		struct rtl_pktHdr	*pktHdr;
		struct rtl_mBuf	*Mblk;	
		
		retval = model_setTestTarget( j );		
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		data=rtlglue_malloc(2048);
		ASSERT(data!=NULL);
#ifdef RTL865X_MODEL_KERNEL				
		data=(char *)(UNCACHE_MASK|(uint32)data);		
#endif
		memset(data,0,2048);	

		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();
		rtl8651_clearAsicPvid();
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		

		for(i=0;i<8;i++)
		{
			datalen=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP); /* without CRC */

#ifdef _LITTLE_ENDIAN
			for(z=0;z<datalen;z+=4)
			{
				uint32 *p,q;
				p=(uint32*)&data[z];
				q=*p;
				*p=ntohl(q);
			}
#endif			
			
			Mblk=mBuf_attachHeader((void*)data,(uint32)0,2048, datalen,0);
			ASSERT(Mblk!=NULL);			
			pktHdr=Mblk->m_pkthdr;
				
			pktHdr->ph_portlist=0x1; /* to PORT 0 */
			pktHdr->ph_flags |= PKTHDR_ORIGINAL_PPPOE_TAGGED;			
			pktHdr->ph_type = i; /* set each type for test */
			
			
			ret=swNic_write(pktHdr,1);	
			/* mBuf_freeMbufChain(Mblk); */
			ASSERT(ret==SUCCESS);

			
#if 0
			if(j==IC_TYPE_MODEL)
			{
				rtlglue_printf("Not Implemented!\n");
				return FAILED;
			}
			else
#endif
			{
				uint8 data2[2048]={0};
				enum PORT_MASK port=PM_PORT_0;
				int len=2048;
				int retval=0;

				while(1)
				{
					if((READ_MEM32(CPUIISR)&(TX_ALL_DONE_IE1))==(TX_ALL_DONE_IE1))
					{
						WRITE_MEM32(CPUIISR,TX_ALL_DONE_IE1);
						break;					
					}
/*					rtlglue_printf("READ_MEM32(CPUIISR)=%x\n",READ_MEM32(CPUIISR));	*/
				}

				virtualMacWaitCycle( 10000 ); /* try and error value */

				retval=virtualMacOutput(&port, data2, &len);
				if(retval==SUCCESS)
				{
		
					rtlglue_printf("len=%d i=%d\n",len,i);
					memDump(data2,len,"data");  
					switch(i)
					{
						case 0: /* ether type */
							ASSERT(data2[14]==0x45);
							break;
						case 1: /* PPTP */
						case 2: /* IP */
						case 3: /* ICMP */
						case 4: /* IGMP */
						case 5: /* TCP */
						case 6: /* UDP */
							ASSERT((data2[20]==0)&&(data2[21]==0x21));
							break;
						case 7: /* IPv6 */
							ASSERT((data2[20]==0)&&(data2[21]==0x57));
							break;						
					}

				}
				else
				{
					rtlglue_printf("virtualMacOutput FAILED........\n");
#ifdef RTL865X_MODEL_KERNEL							
					data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
					rtlglue_free(data);					
					return FAILED;
				}
	
			} /* end else */
		} /* end for */

#ifdef RTL865X_MODEL_KERNEL		
		data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
		rtlglue_free(data);	
		return SUCCESS;
	}

	return SUCCESS;

}




int32 testNicTxCrcCheck(uint32 caseNo)
{
	int j;

	int retval;


	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{

		int ret;
#ifdef _LITTLE_ENDIAN		
		int z;
#endif
		char *data;
		int datalen=0;
		struct rtl_pktHdr	*pktHdr;
		struct rtl_mBuf	*Mblk;	
		
		retval = model_setTestTarget( j );		
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		data=rtlglue_malloc(2048);
		ASSERT(data!=NULL);
#ifdef RTL865X_MODEL_KERNEL				
		data=(char *)(UNCACHE_MASK|(uint32)data);		
#endif
		memset(data,0,2048);
		
		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();
		rtl8651_clearAsicPvid();
		
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		



		datalen=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP)+4; /* length with CRC */
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)&(~EXCLUDE_CRC)); /* length with CRC */
		WRITE_MEM32(PCRP0,READ_MEM32(PCRP0)|BYPASS_TCRC); 
		rtlglue_printf("MACCR=%x\n",READ_MEM32(MACCR));						
		data[datalen-4]=0x01;
		data[datalen-3]=0x02;		
		data[datalen-2]=0x03;		
		data[datalen-1]=0x04;		

#ifdef _LITTLE_ENDIAN		
		for(z=0;z<datalen;z+=4)
		{
			uint32 *p,q;
			p=(uint32*)&data[z];
			q=*p;
			*p=ntohl(q);
		}
#endif		
		
		Mblk=mBuf_attachHeader((void*)data,(uint32)0,2048, datalen,0);
		ASSERT(Mblk!=NULL);		
		pktHdr=Mblk->m_pkthdr;
			
		pktHdr->ph_portlist=0x1; /* to PORT 0 */


		ret=swNic_write(pktHdr,1);			

		/* mBuf_freeMbufChain(Mblk); */
		ASSERT( ret==SUCCESS );	


#if 0
		if(j==IC_TYPE_MODEL)
		{
			rtlglue_printf("Not Implemented!\n");
			return FAILED;
		}
		else
#endif
		{
			uint8 data2[2048]={0};
			enum PORT_MASK port=PM_PORT_0;
			int len=2048;
			int retval=0;

			while(1)
			{
				if((READ_MEM32(CPUIISR)&(TX_ALL_DONE_IE1))==(TX_ALL_DONE_IE1))
				{
					WRITE_MEM32(CPUIISR,TX_ALL_DONE_IE1);
					break;					
				}
/*					rtlglue_printf("READ_MEM32(CPUIISR)=%x\n",READ_MEM32(CPUIISR));	*/
			}

			virtualMacWaitCycle( 10000 ); /* try and error value */

			retval=virtualMacOutput(&port, data2, &len);


			WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC); /* length without CRC */
			WRITE_MEM32(PCRP0,READ_MEM32(PCRP0)&(~BYPASS_TCRC)); /* not by pass CRC offload */
			
			if(retval==SUCCESS)
			{
				/* memDump(data2,len,"data2"); */
				ASSERT( data2[len-4]==0x1 );
				ASSERT( data2[len-3]==0x2 );
				ASSERT( data2[len-2]==0x3 );
				ASSERT( data2[len-1]==0x4 );
#ifdef RTL865X_MODEL_KERNEL						
				data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
				rtlglue_free(data);				
				return SUCCESS;
			}
			else
			{
				
				rtlglue_printf("virtualMacOutput FAILED........\n");  /* maybe CRC error , MAC drop the packet */
#ifdef RTL865X_MODEL_KERNEL		
				data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
				rtlglue_free(data);				
				return FAILED;
			}

		} /* end else */

#ifdef RTL865X_MODEL_KERNEL		
		data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
		rtlglue_free(data);
		
		return SUCCESS;
	}

	return SUCCESS;

}


int32 testNicTxJumboFrame(uint32 caseNo)
{
	int j;

	int retval;

	char org_data[] = {


	0x00, 0x00, 0xe8, 0x66, 0x46, 0x85,   /* DMAC */
	0x00, 0x0c, 0x6e, 0x3a, 0x5d, 0x66,   /* SMAC */

/*	0x05,0xff,0xaa,0xaa,0x03,0,0,0,  */ /* for SNAP */

	
	0x08, 0x00,   /* Ether Type */

	/* IP header */

	0x45, 0x00, 0x01, 0x67, 0xa4, 0xd2, 0x40, 0x00, 
	0x80, 0x06, 0xaf, 0xde, 0xc0, 0xa8, 0x9a, 0x6e, 
	0x42, 0x66, 0x07, 0x63, 


	/* TCP header */

	0x10, 0x46, 0x00, 0x50, 0x92, 0x11, 
	0xa2, 0xdf, 0x4c, 0xa5, 0x5c, 0xdd, 0x50, 0x18,
	0x44, 0x70, 0xbc, 0x2c, 0x00, 0x00,


	/*  Payload */
};


	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{

		int ret;
		char *data;
		int datalen=0;
		struct rtl_pktHdr	*pktHdr;
		struct rtl_mBuf	*Mblk,*m;
		struct ip *iph;
		struct tcphdr *tc;
		int i;
		
		retval = model_setTestTarget( j );		
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */


		rtl8651_clearRegister();
		retval=configNicInit();
		WRITE_MEM32(CCR,L4ChkSErrAllow|L3ChkSErrAllow); /* skip CRC error */
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		

		data=rtlglue_malloc(17000); 
		ASSERT(data!=NULL);
#ifdef RTL865X_MODEL_KERNEL				
		data=(char *)(UNCACHE_MASK|(uint32)data);				
#endif


		/* set CPU port jumbo frame size */
		WRITE_MEM32(PCRP6,READ_MEM32(PCRP6)|AcptMaxLen_16K);
		WRITE_MEM32(PCRP7,READ_MEM32(PCRP7)|AcptMaxLen_16K);
		WRITE_MEM32(PCRP8,READ_MEM32(PCRP8)|AcptMaxLen_16K);		
		WRITE_MEM32(PCRP0,READ_MEM32(PCRP0)|AcptMaxLen_16K); /* set jumbo packet to 16k-2 bytes */


		memset(data,0,17000);
		
		for(i=0;i<17;i++)
		{
			memset(data+i*1000,i,1000);
		}
		
		memcpy(data,org_data,sizeof(org_data));
		datalen=16370-4;	 /* 4 bytes for CRC */
		iph=(struct ip *)&data[14];
		tc=(struct tcphdr *)&data[34];
		iph->ip_len =htons(datalen-14); 

/*
		memDump(org_data,sizeof(org_data),"12");
		memDump(data,128,"data"); */

#ifdef _LITTLE_ENDIAN
		for(i=0;i<datalen;i+=4)
		{
			uint32 *p,q;
			p=(uint32*)&data[i];
			q=*p;
			*p=ntohl(q);
		}
#endif
		
/*		Mblk=mBuf_attachHeader((void*)data,(uint32)0,2048, datalen,0); */

		Mblk=mBuf_attachHeaderJumbo((void*)data,(uint32)0,2048, datalen);
		ASSERT(Mblk!=NULL);
		
		m=Mblk;

		while(1)
		{
			/* memDump(m->m_data,32,"data");  */
			if(m->m_next==NULL) break;	
			m=m->m_next;
		}

		pktHdr=Mblk->m_pkthdr;
		pktHdr->ph_portlist=0x1; /* to PORT 0 */

		ret=swNic_write(pktHdr,1);			
		/* mBuf_freeMbufChain(Mblk); */
		ASSERT( ret==SUCCESS );	

		{
			uint8 *data2;
			enum PORT_MASK port=PM_PORT_0;
			int len=2048;
			int retval=0;


			data2=rtlglue_malloc(17000);
			ASSERT(data2!=NULL);
#ifdef RTL865X_MODEL_KERNEL					
			data2=(char *)(UNCACHE_MASK|(uint32)data2);	
#endif
			

			while(1)
			{

				if((READ_MEM32(CPUIISR)&(TX_ALL_DONE_IE1))==(TX_ALL_DONE_IE1))
				{
					rtlglue_printf("READ_MEM32(CPUIISR)=%x\n",READ_MEM32(CPUIISR));

					WRITE_MEM32(CPUIISR,TX_ALL_DONE_IE1);

					break;					
				}
					rtlglue_printf("READ_MEM32(CPUIISR)=%x\n",READ_MEM32(CPUIISR));	
			}

			virtualMacWaitCycle( 50000 ); /* try and error value */

			retval=virtualMacOutput(&port, data2, &len);


#ifdef _LITTLE_ENDIAN
			for(i=0;i<datalen;i+=4)
			{
				uint32 *p,q;
				p=(uint32*)&data[i];
				q=*p;
				*p=ntohl(q);
			}
#endif
			
			if(retval==SUCCESS)
			{

				if(memcmp(data,data2,len-4)!=0)
				{
					memDump(data,len,"data");
					memDump(data2,len,"data2");

				}
				ASSERT(memcmp(data,data2,len-4)==0);
#ifdef RTL865X_MODEL_KERNEL						
				data=(char *)((uint32)data&(~UNCACHE_MASK));
				data2=(char *)((uint32)data2&(~UNCACHE_MASK));				
#endif				
				rtlglue_free(data2);
				rtlglue_free(data);
/*				retval=mBuf_freeMbufChain(Mblk);	*/
/*				mBuf_freePkthdr(pktHdr);			*/
				return SUCCESS;				
			}
			else
			{
				rtlglue_printf("virtualMacOutput FAILED........\n");  /* maybe CRC error , MAC drop the packet */
#ifdef RTL865X_MODEL_KERNEL						
				data=(char *)((uint32)data&(~UNCACHE_MASK));
				data2=(char *)((uint32)data2&(~UNCACHE_MASK));				
#endif				
				rtlglue_free(data2);
				rtlglue_free(data);				
				return FAILED;
			}

		} /* end else */

		return SUCCESS;
	}

	return SUCCESS;

}

int32 testNicRxJumboFrame(uint32 caseNo)
{

	int i,j,len=0;


	int retval;
	countRX=0;
	PriorityChecksum=0;
	
	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{
		char *data;
/*		
		struct rtl_pktHdr	*pktHdr;
		struct rtl_mBuf	*Mblk,*m;
*/
		struct ip *iph;
		struct tcphdr *tc;	
		hsb_param_t rhsb;
		
		retval = model_setTestTarget( j );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		RoutingConfig();
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		
		parserInit(j);

		data=rtlglue_malloc(17000);		
		ASSERT(data!=NULL);
#ifdef RTL865X_MODEL_KERNEL				
		data=(char *)(UNCACHE_MASK|(uint32)data);		
#endif

		for(i=0;i<17;i++)
		{
			memset(data+i*1000,i,1000);
		}



		for(i=0;i<6;i+=2)
		{
			uint32 value;
			value=(i<<28)|((i+1)<<12);
			rtlglue_printf("i<<28=%x (i+1)<<12=%x value=%x htonl(v)=%x\n",i<<28,(i+1)<<12,value,htonl(value));
			/* WRITE_MEM16(CPUQDM0+i*2,(i<<12)); */
			WRITE_MEM32(CPUQDM0+i*2,value);
		}
		
			
		parserInit(j);
		WRITE_MEM32(PCRP0,READ_MEM32(PCRP0)|AcptMaxLen_16K); /* set jumbo packet to 16k-2-4-8 bytes */
		_createSourcePacket(data,L2_IP,20,IPPROTO_TCP);			
		len=16370-4; /* length without CRC */


		/* let this packet parse failed, direct TX to CPU */	
		data[0]=0xff;
		data[1]=0xff;
		data[2]=0xff;
		data[3]=0xff;
		data[4]=0xff;
		data[5]=0xff;			
		data[14]=0x46; 

		
		iph=(struct ip *)&data[14];
		tc=(struct tcphdr *)&data[34];
		iph->ip_len =htons(len-14);
		tc->th_sum=0;
		iph->ip_sum=0;
		tc->th_sum=model_tcpChecksum(iph);
		iph->ip_sum=model_ipChecksum(iph);

/*		memDump(data,len,"rx"); */
		retval=virtualMacInput(PN_PORT0,data,len+4);
		if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);				
		virtualMacWaitCycle(20000);
		virtualMacGetHsb(&rhsb);
		dumpHSB(&rhsb);

		

		/* Swich Core interrupt to notify the RISC that packets is ready	 */
		while(1)
		{
			retval=swNic_intHandler(&i);			
			if(retval!=1) break;
			swNic_rxThread(i);
		}
		


		if(countRX!=1) 
		{
			rtlglue_printf("Failed when %dth packet RX!\n",countRX);
#ifdef RTL865X_MODEL_KERNEL		
			data=(char *)((uint32)data&(~UNCACHE_MASK));			
#endif
			rtlglue_free(data);			
			return FAILED;
		}

#ifdef RTL865X_MODEL_KERNEL		
		data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
		rtlglue_free(data);
		return SUCCESS;


		/* mBuf_freeMbufChain(pMbuf); */
		
	}

	return SUCCESS;
}


int32 testNicRxPkthdr(uint32 caseNo)
{

	int i,j,len=0;


	int retval;
	countRX=0;
	PriorityChecksum=0;
	
	/*(for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )*/
	for( j = IC_TYPE_REAL;j <= IC_TYPE_REAL; j++ )
	{
		char *data;
		hsb_param_t rhsb;
		hsa_param_t rhsa;
		int errnum=0;
		struct ip *iph;
		struct tcphdr *tc;
		struct udphdr *ud;
		
		retval = model_setTestTarget( j );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		Niclayer4Config();
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		parserInit(j);		
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);
		
		data=rtlglue_malloc(2048);		
		ASSERT(data!=NULL);
#ifdef RTL865X_MODEL_KERNEL				
		data=(char *)(UNCACHE_MASK|(uint32)data);		
#endif


		/* set queue ID to descript ring mapping  */
		/* 
			CPU Port Queue ID 0 ==> Descript Ring 0
			CPU Port Queue ID 1 ==> Descript Ring 1
			CPU Port Queue ID 2 ==> Descript Ring 2
			CPU Port Queue ID 3 ==> Descript Ring 3
			CPU Port Queue ID 4 ==> Descript Ring 4
			CPU Port Queue ID 5 ==> Descript Ring 5
			Ext Port 1  All Queue ID ==> Descript Ring 3
			Ext Port 2  All Queue ID ==> Descript Ring 3
			Ext Port 3  All Queue ID ==> Descript Ring 3			
		*/
			
		for(i=0;i<6;i+=2)
		{
			uint32 value;
			value=(i<<28)|((i+1)<<12)|(3)|(3<<4)|(3<<8)|(3<<16)|(3<<20)|(3<<24);
			/* rtlglue_printf("i<<28=%x (i+1)<<12=%x value=%x htonl(v)=%x\n",i<<28,(i+1)<<12,value,htonl(value)); */
			/* WRITE_MEM16(CPUQDM0+i*2,(i<<12)); */
			WRITE_MEM32(CPUQDM0+i*2,value);
		}
			
		parserInit(j);



/* FIXME: to skip a sw_core bug */
		for(i=0;i<7;i++)
		{		
			rtl865x_tblAsicDrv_extIntIpParam_t ipt;
/*			ether_addr_t mac; */
		   	rtl865x_tblAsicDrv_naptTcpUdpParam_t napt;
/*			rtl865x_tblAsicDrv_arpParam_t arpt; */

		
			countRX=0;
			/* set to vlan 3099 */


			switch(i)
			{

				case 0: /* set DP have EXT2 , */

					len=_createSourcePacket(data,L2_VLAN_IP,20,IPPROTO_TCP);

					{
						rtl865x_tblAsicDrv_vlanParam_t vlant;
						bzero( &vlant, sizeof(vlant) );

						/* change DP mask here for test case 0 */
						vlant.memberPortMask = vlant.untagPortMask= 0x180;/*(1<<hp[5]); */
						retval = rtl8651_setAsicVlan(3099,&vlant); /* VLAN 3099 have member port 7 & 8 */
						ASSERT( retval==SUCCESS );
					}				
					data[14]=0xc;
					data[15]=0x1b;	
					
					break;

					
				case 1:
					len=_createSourcePacket(data,L2_VLAN_IP,20,IPPROTO_TCP);				
					data[14]=0xc|0x10; /* SET CFI = 1 */
					data[15]=0x1b;
					break;
					
				case 2: /* set DP only have EXT port  */


					/* the Niclayer4Config  */
					len=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP);

					data[0]=0;
					data[1]=0;
					data[2]=0;
					data[3]=0;
					data[4]=0x88;
					data[5]=0x8a;

					data[6]=0;
					data[7]=0;
					data[8]=0;
					data[9]=0;
					data[10]=0;
					data[11]=0x40;

					bzero((void*)&ipt, sizeof(rtl865x_tblAsicDrv_extIntIpParam_t));
					bzero((void*) &napt, sizeof(napt));	
					bzero(&napt,sizeof(napt));
					napt.insideLocalIpAddr= 0xc0a80103;
					napt.insideLocalPort = 20;
					napt.isValid = 1;
			   	       napt.isDedicated = 0;
				       napt.isTcp = 1;
				       napt.isStatic = 1;
				       napt.selExtIPIdx = 0;
		  		 	napt.selEIdx=0;
			     	       napt.offset=0;
					rtl8651_setAsicNaptTcpUdpTable(1,rtl8651_naptTcpUdpTableIndex(1,0xc0a80103, 20,0xc0a80305,8800), &napt);


					iph=(struct ip *)&data[14];
					tc=(struct tcphdr *)&data[34];			
					iph->ip_len =htons(len-14);

					iph->ip_src.s_addr=htonl(0xc0a80103);
					iph->ip_dst.s_addr=htonl(0xc0a80305);
					tc->th_sport=htons(20);
					tc->th_dport=htons(8800);			
					
					tc->th_sum=0;
					iph->ip_sum=0;
					tc->th_sum=model_tcpChecksum(iph);
					iph->ip_sum=model_ipChecksum(iph);

		
					break;					
					
				case 3: /* mismatch NAPT trap to CPU */


					/* the Niclayer4Config  */
					len=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP);

					data[0]=0;
					data[1]=0;
					data[2]=0;
					data[3]=0;
					data[4]=0x88;
					data[5]=0x8a;

					data[6]=0;
					data[7]=0;
					data[8]=0;
					data[9]=0;
					data[10]=0;
					data[11]=0x40;

					iph=(struct ip *)&data[14];
					tc=(struct tcphdr *)&data[34];			
					iph->ip_len =htons(len-14);

					iph->ip_src.s_addr=htonl(0xc0a80104);
					iph->ip_dst.s_addr=htonl(0xc0a80306);
					tc->th_sport=htons(30);
					tc->th_dport=htons(8800);			
					
					tc->th_sum=0;
					iph->ip_sum=0;
					tc->th_sum=model_tcpChecksum(iph);
					iph->ip_sum=model_ipChecksum(iph);

		
					break;

				case 4: /* multicast to CPU */
					{
					rtl865x_tblAsicDrv_multiCastParam_t mcast; 

					rtl8651_setAsicMulticastMTU(45);
					rtl8651_setAsicMulticastEnable(TRUE);

					bzero((void*) &mcast,sizeof(mcast));
					mcast.dip=0xe0000003;
					mcast.sip=0xc0a80101;
					mcast.mbr=PM_PORT_0;
					mcast.svid=LAN0_VID;
					mcast.port=PN_PORT0;
		 		 	rtl8651_setAsicIpMulticastTable(&mcast);

					len=_createSourcePacket(data,L2_IP,20,IPPROTO_UDP);

					data[0]=0x01;
					data[1]=0x00;
					data[2]=0x5e;
					data[3]=0x00;
					data[4]=0x00;
					data[5]=0x03;

					iph=(struct ip *)&data[14];
					ud=(struct udphdr *)&data[34];
					iph->ip_len =htons(len-14);

					iph->ip_src.s_addr=htonl(0xc0a80101);
					iph->ip_dst.s_addr=htonl(0xe0000003);
					ud->uh_sport=htons(30);
					ud->uh_dport=htons(8800);			
					
					ud->uh_sum=0;
					iph->ip_sum=0;
					ud->uh_sum=model_tcpChecksum(iph);
					iph->ip_sum=model_ipChecksum(iph);

					}
					break;

				case 5: /* multicast forward */
					{

					rtl8651_setAsicMulticastMTU(1500);
					len=_createSourcePacket(data,L2_IP,20,IPPROTO_UDP);
					data[0]=0x01;
					data[1]=0x00;
					data[2]=0x5e;
					data[3]=0x00;
					data[4]=0x00;
					data[5]=0x03;

					iph=(struct ip *)&data[14];
					ud=(struct udphdr *)&data[34];
					iph->ip_len =htons(len-14);

					iph->ip_src.s_addr=htonl(0xc0a80101);
					iph->ip_dst.s_addr=htonl(0xe0000003);
					ud->uh_sport=htons(30);
					ud->uh_dport=htons(8800);			
					
					ud->uh_sum=0;
					iph->ip_sum=0;
					ud->uh_sum=model_tcpChecksum(iph);
					iph->ip_sum=model_ipChecksum(iph);
					}
					break;
				case 6: /* multicast forward to ext port */
					{
					rtl865x_tblAsicDrv_multiCastParam_t mcast; 

					rtl8651_setAsicMulticastMTU(1500);
					rtl8651_setAsicMulticastEnable(TRUE);

					bzero((void*) &mcast,sizeof(mcast));
					mcast.dip=0xe0000003;
					mcast.sip=0xc0a80101;
					mcast.mbr=PM_PORT_0|(PM_PORT_EXT0<<RTL8651_PORT_NUMBER);
					mcast.svid=LAN0_VID;
					mcast.port=PN_PORT0;
					mcast.cpu=1;
					
		 		 	rtl8651_setAsicIpMulticastTable(&mcast);

					len=_createSourcePacket(data,L2_IP,20,IPPROTO_UDP);

					data[0]=0x01;
					data[1]=0x00;
					data[2]=0x5e;
					data[3]=0x00;
					data[4]=0x00;
					data[5]=0x03;

					iph=(struct ip *)&data[14];
					ud=(struct udphdr *)&data[34];
					iph->ip_len =htons(len-14);

					iph->ip_src.s_addr=htonl(0xc0a80101);
					iph->ip_dst.s_addr=htonl(0xe0000003);
					ud->uh_sport=htons(30);
					ud->uh_dport=htons(8800);			
					
					ud->uh_sum=0;
					iph->ip_sum=0;
					ud->uh_sum=model_tcpChecksum(iph);
					iph->ip_sum=model_ipChecksum(iph);

					}
					break;
					
			}

			/* memDump(data,len,"rx"); */
			retval=virtualMacInput(PN_PORT0,data,len+4);
			if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);				
			virtualMacWaitCycle(5000);
			virtualMacGetHsb(&rhsb);
			virtualMacGetHsa(&rhsa);
			dumpHSB(&rhsb);

			switch(i)
			{

				case 0: /* set vlan 3099, member port EXT1/EXT2 ,  except to CPU queue id to desc ring mapping - ring 0 */
					ASSERT((READ_MEM32(CPUIISR)&RX_DONE_IP0)==RX_DONE_IP0);
					break;
				case 1:
					ASSERT(rhsb.dirtx==1);
					break;					
				case 2: /* from EXT2, expect to EXT2 queue id to desc ring mapping -  ring 3*/
					ASSERT((READ_MEM32(CPUIISR)&RX_DONE_IP3)==RX_DONE_IP3);
					break;
				case 3:
					/* assert to CPU port only */
					ASSERT(rhsa.dpext==8);
					ASSERT(rhsa.dp==0x40); 
					ASSERT(rhsa.dpc==1);
					break;
				case 4:
					/* assert to CPU port only */
					ASSERT(rhsa.dpext==8);
					ASSERT(rhsa.dp==0x40);
					ASSERT(rhsa.dpc==1);
					break;
				case 5:
					/* assert forward to port 0 */
					ASSERT(rhsa.dpext==0);
					ASSERT(rhsa.dp==0x1);
					ASSERT(rhsa.dpc==1);
					ASSERT(rhsa.hwfwrd==1);
					ASSERT(rhsa.l2tr==0);
					ASSERT(rhsa.l34tr==0);
					break;					
				case 6:
					/* assert to CPU port only */
					ASSERT(rhsa.dpext==1);
					ASSERT(rhsa.dp==0x41);
					ASSERT(rhsa.dpc==2);
					break;
								
			}

			/* Swich Core interrupt to notify the RISC that packets is ready	 */
			while(1)
			{
				retval=swNic_intHandler(&i);			
				if(retval!=1) break;
				swNic_rxThread(i);
			}
			


			if((countRX<1) &&(i!=5)) /* case 5: do not trap to cpu , just forward */
			{
				rtlglue_printf("Failed when %dth packet RX!\n",countRX);
				errnum++;
				goto out;
			}


			rtlglue_printf("pkthdr->isorg=%d\n", rxPkthdr.ph_isOriginal);		
			rtlglue_printf("pkthdr->hwfwd=%d\n", rxPkthdr.ph_hwFwd);
			rtlglue_printf("pkthdr->l2Trans=%d\n", rxPkthdr.ph_l2Trans);		


			switch(i)
			{

				case 0: /* set vlan 3099, member port EXT1/EXT2 , */
					if((rxPkthdr.ph_isOriginal!=1)||
					   (rxPkthdr.ph_l2Trans!=0)||
					   (rxPkthdr.ph_hwFwd!=1))
					{
						errnum++;
						rtlglue_printf("FAILED at case %d...\n",i);
					}
					break;
				case 1:
					memDump(&rxPkthdr,32,"pkthdr");
					ASSERT(rxPkthdr.ph_reason==0xaa0); /* IPv4, length=20, iphdr=0x45, CFI=1 */
					break;					
				case 2:
					if((rxPkthdr.ph_isOriginal!=0)||
					   (rxPkthdr.ph_hwFwd!=1)||
					   (rxPkthdr.ph_l2Trans!=1))

					{
						errnum++;
						rtlglue_printf("FAILED at case %d....\n",i);
					}
					break;
				case 3:
					/* ASSERT DMAC, SMAC is original */
					ASSERT(memcmp(data,rxMdata,12)==0);					
					break;
				case 4:
					/* assert to CPU, not hardware forward, l2act =1 */
					if((rxPkthdr.ph_isOriginal!=1)||
					   (rxPkthdr.ph_hwFwd!=0)||
					   (rxPkthdr.ph_l2Trans!=0)) /* if multicast to cpu only, the l2trans always be zero.  */
					{
						errnum++;
						rtlglue_printf("FAILED at case %d....\n",i);
					}
					break;					
				case 5:
					break;
				case 6:
					/* assert to CPU, not hardware forward, l2act =1 */
					if((rxPkthdr.ph_isOriginal!=0)||
					   (rxPkthdr.ph_hwFwd!=1)||
					   (rxPkthdr.ph_l2Trans!=1)) /* because multicast member port not in the source vlan */
					{
						errnum++;
						rtlglue_printf("FAILED at case %d....\n",i);
					}
					break;					

			}
			

		}

out:		
#ifdef RTL865X_MODEL_KERNEL		
		data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
		rtlglue_free(data);
		if(errnum==0) return SUCCESS;
		else return FAILED;


		/* mBuf_freeMbufChain(pMbuf); */
		
	}

	return SUCCESS;
}


int32 testNicRxMbufNewSpec(uint32 caseNo)
{

	int i,j,k,len=0;
	char data[2048]={0};
	

	int retval;
/*	hsb_param_t rhsb; */
	int idx=0;
	countRX=0;
	PriorityChecksum=0;
	
	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{
		uint32 orgMbufPtr[256]={0};
		int lastRXcnt=0;
		
		retval = model_setTestTarget( j );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		RoutingConfig();
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		
		parserInit(j);

		len=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP)+4;
		data[0]=0xff;
		data[1]=0xff;
		data[2]=0xff;
		data[3]=0xff;
		data[4]=0xff;
		data[5]=0xff;	
		data[14]=0x46; /* parse failed, direct TX to CPU */
		countRX=0;

		for(k=0;k<10;k++)
		{
			uint32 *ring;

			rtlglue_printf("k=%d\n",k);
/*			swNic_dumpPkthdrDescRing(); */
			if(k==0)
			{
				
				ring=(uint32*)READ_MEM32(CPURMDCR0);			
				if(firstMbufPtr!=ring)
				{
					swNic_reset();
					ring=(uint32*)READ_MEM32(CPURMDCR0);
				}

				ASSERT(ring==firstMbufPtr);


				while(1)
				{
					firstMbufPtr[idx]=firstMbufPtr[idx]|(1);
					if(firstMbufPtr[idx]&2) break;
					idx++;
				}

				for(i=0;i<=idx;i++)
				{
					if(i==idx-8)
						firstMbufPtr[i]|=(1);
					else
						firstMbufPtr[i]&=(~1);
				}
/*				swNic_dumpMbufDescRing();	*/

			}			

			if(k==1)
			{
				firstMbufPtr[idx-8]|=(1);
/*				swNic_dumpMbufDescRing();	*/
			}

			if(k==2)
			{
				firstMbufPtr[idx]|=(1);
				firstMbufPtr[4]|=(1);
/*				swNic_dumpMbufDescRing();	*/
			}
			if(k==3)
			{
				firstMbufPtr[idx-14]|=(1);
				firstMbufPtr[idx-12]|=(1);
				firstMbufPtr[idx-10]|=(1);
				firstMbufPtr[idx-9]|=(1);
/*				swNic_dumpMbufDescRing();	*/
			}
			if(k==8)
			{				
				firstMbufPtr[idx-8]|=(1);
				firstMbufPtr[idx]|=(1);
/*				swNic_dumpMbufDescRing();	*/
			}
			
			
			
			retval=virtualMacInput(PN_PORT0,data,len);
			if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);				
			virtualMacWaitCycle(15000);
			/* virtualMacGetHsb(&rhsb); */
			/* dumpHSB(&rhsb); */



			ring=(uint32*)READ_MEM32(CPURMDCR0);

					
			rtlglue_printf("k=%d ring-firstMbufPtr=%d idx=%d\n",k,ring-firstMbufPtr,idx);
			
			if(k<2)
			{			
				ASSERT((*(firstMbufPtr+idx-8)&1)==0);
			}
			else if(k==2)
			{			
				ASSERT((*(firstMbufPtr+idx)&1)==0);
			}
			else if(k==3)
			{
				ASSERT((*(firstMbufPtr+idx-4)&1)==0);
			}			
			else if(k==4)
			{
				ASSERT((*(firstMbufPtr+idx-14)&1)==0);
			}
			else if(k==5)
			{
				ASSERT((*(firstMbufPtr+idx-12)&1)==0);
			}			
			else if(k==6)
			{
				ASSERT((*(firstMbufPtr+idx-10)&1)==0);
			}
			else if(k==7)
			{
				ASSERT((*(firstMbufPtr+idx-9)&1)==0);
			}
			else if(k==8)
			{
				ASSERT((*(firstMbufPtr+idx-8)&1)==0);
			}
			else if(k==9)
			{
				ASSERT((*(firstMbufPtr+idx)&1)==0);
			}

		
			/* backup all mbuf own info */
			for(i=0;i<idx;i++)
				orgMbufPtr[i]=firstMbufPtr[i];

			/* receive packets to CPU, and change the mbuf of free pkt to switch own */
			while(1)
			{
				retval=swNic_intHandler(&i);			
				if(retval!=1) break;
				swNic_rxThread(i);
			}	

			if(lastRXcnt==countRX)
			{
/*			
				swNic_dumpPkthdrDescRing();
				swNic_dumpMbufDescRing();
*/				
				goto out;
			}
			
			lastRXcnt=countRX;

			/* restore all mbuf own info */
			for(i=0;i<idx;i++)
				firstMbufPtr[i]=orgMbufPtr[i];

	
	
		}		

out:
		swNic_reset();

		for(i=0;i<=idx;i++)
			firstMbufPtr[i]|=(1);

/*		swNic_dumpMbufDescRing(); */
		
		return SUCCESS;


		/* mBuf_freeMbufChain(pMbuf); */
		
	}

	return SUCCESS;
}


int32 testNicLoopback(uint32 caseNo)
{

	int i,j;
	int retval;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	_rtl8651_tblDrvAclRule_t aclt;

	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{
		countRX=0;	
		retval = model_setTestTarget( j );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();
		rtl8651_clearAsicPvid();

//		WRITE_MEM32(0xbb804600,READ_MEM32(0xbb804600)&(~0xffffffef)); /* disable CPU port link */
		
		while(1)
		{
			 WRITE_MEM32(SSIR,FULL_RST);
			for(i=0;i<10000;i++) rtlglue_printf("\r"); /* 2006/11/23:  this delay time must enough, else cause hardware other issues! */

			WRITE_MEM32(SSIR,TRXRDY);
			virtualMacWaitCycle(10000);	
			for(i=0;i<10000;i++) rtlglue_printf("\r"); /* 2006/11/23:  this delay time must enough, else cause hardware other issues! */
			
			if((READ_MEM32(SSIR)&TRXRDY)==TRXRDY) break;
		}		

		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)&(~EXCLUDE_CRC));	
		
//		WRITE_MEM32(PCRP0, (0<<ExtPHYID_OFFSET)|EnablePHYIf |STP_PortST_FORWARDING); /* Jumbo Frame */


		// for B-cut / C-cut patch
		WRITE_MEM32(PCRP0, (6<<ExtPHYID_OFFSET)|EnablePHYIf |STP_PortST_FORWARDING|ForceDuplex|ForceSpeed100M|EnForceMode|ForceLink); /* Jumbo Frame */
//		WRITE_MEM32(PCRP0, ((6<<ExtPHYID_OFFSET)|EnablePHYIf |STP_PortST_FORWARDING |EnForceMode) & (~ForceLink)); /* Jumbo Frame */

		WRITE_MEM32(PCRP1, ((1<<ExtPHYID_OFFSET)|EnablePHYIf |STP_PortST_FORWARDING |EnForceMode) & (~ForceLink)); /* Jumbo Frame */
		WRITE_MEM32(PCRP2, ((2<<ExtPHYID_OFFSET)|EnablePHYIf |STP_PortST_FORWARDING  |EnForceMode) & (~ForceLink)); /* Jumbo Frame */
		WRITE_MEM32(PCRP3, ((3<<ExtPHYID_OFFSET)|EnablePHYIf |STP_PortST_FORWARDING |EnForceMode) & (~ForceLink)); /* Jumbo Frame */
		WRITE_MEM32(PCRP4, ((4<<ExtPHYID_OFFSET)|EnablePHYIf |STP_PortST_FORWARDING |EnForceMode) & (~ForceLink)); /* Jumbo Frame */
		WRITE_MEM32(PCRP5, ((5<<ExtPHYID_OFFSET)|EnablePHYIf |STP_PortST_FORWARDING |EnForceMode) & (~ForceLink)); /* Jumbo Frame */
		WRITE_MEM32(PCRP6, (7<<ExtPHYID_OFFSET)|EnablePHYIf |STP_PortST_FORWARDING ); /* Jumbo Frame */		
		WRITE_MEM32(PCRP7, ((8<<ExtPHYID_OFFSET)|EnablePHYIf |STP_PortST_FORWARDING |EnForceMode )& (~ForceLink)); /* Jumbo Frame */		
		WRITE_MEM32(PCRP8, ((9<<ExtPHYID_OFFSET)|EnablePHYIf |STP_PortST_FORWARDING |EnForceMode) & (~ForceLink)); /* Jumbo Frame */				

		/* PHY patch */
		{
			uint32 port, status;
			
			WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( 6 << PHYADD_OFFSET ) | ( 9 << REGADD_OFFSET ) |0x0505 );
			while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );
			WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( 6 << PHYADD_OFFSET ) | ( 4 << REGADD_OFFSET ) |0x1F10 );
			while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );
			WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( 6 << PHYADD_OFFSET ) | ( 0 << REGADD_OFFSET ) |0x1200 );
			while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );

			for ( port = 0; port < 5; port++ )	/* Embedded phy id is the same as port id. */
			{
				/* Read */
				WRITE_MEM32( MDCIOCR, COMMAND_READ | ( port << PHYADD_OFFSET ) | ( 0 << REGADD_OFFSET ) );
				do
				{
					status = READ_MEM32( MDCIOSR );

				} while ( ( status & STATUS ) != 0 );

				status &= 0xffff;

				WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( port << PHYADD_OFFSET ) | ( 0 << REGADD_OFFSET ) | 0x200 | status);
				while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );
			}		
		}		
		

		rtl8651_setAsicPvid(0,LAN0_VID);
		rtl8651_setAsicPvid(1,LAN0_VID);
		rtl8651_setAsicPvid(2,LAN0_VID);
		rtl8651_setAsicPvid(3,LAN0_VID);
		rtl8651_setAsicPvid(4,LAN0_VID);
		

		/* module switch control reg: enable L2/L3/L4/STP, trap 802.1D/GARP to CPU */
		rtl8651_setAsicOperationLayer(2);
		/* TTL control reg: enable TTL-1 operation */
//		WRITE_MEM32(ALECR,READ_MEM32(ALECR)|(uint32)EN_TTL1);	
//		WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_51B_CPU_REASON);
		/* VLAN tagging control reg: accept all frame */



		 /* config vlan table */
		 bzero((void*) &intf, sizeof(intf));
		 bzero((void*) &aclt, sizeof(aclt));

		/* wan 0 */
		strtomac(&intf.macAddr, GW_LAN0_MAC);	
		intf.macAddrNumber = 1;
		intf.vid = LAN0_VID;
		intf.mtu = 1500;
		intf.inAclStart = 0;
		intf.inAclEnd = 0;
		intf.outAclStart = 0;
		intf.outAclEnd = 0;
//		intf.enableRoute = 1;
		intf.valid = 1;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval==SUCCESS );

		bzero( &vlant, sizeof(vlant) );
		vlant.memberPortMask = vlant.untagPortMask= 0x3f;
		retval = rtl8651_setAsicVlan(intf.vid,&vlant);
		ASSERT( retval==SUCCESS );


		 /* ingress filter gateway mac */
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_CPU;
//		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		retval = rtl8651_setAsicAclRule(0, &aclt);
		ASSERT( retval==SUCCESS );



		if(firstTime==FALSE)
		{
			assert("This case must run individual\n");
			return SUCCESS;
		}

		firstTime=FALSE;				
		retval=mBuf_init(2048,  0/* use external cluster pool */, 1024, 2048, 0);

		if (retval != SUCCESS)
		{
			rtlglue_printf("mBuf_init failed...\n"); return FAILED;
		}
		
		{
			int32 rxRingSize[6] = {128, 128, 128, 128, 128, 128};
			int32 mbufRingSize = 1024;
			int32 txRingSize[2] = {16, 16};

			if (0 != swNic_init (	rxRingSize,
								mbufRingSize,
								txRingSize,
								2048,
								(proc_input_pkt_funcptr_t) rtl865x_fastRx_loopback,
								NULL,
								NULL))
			{				
				rtlglue_printf("swNic_init failed........\n");
				return FAILED;
			}
		}
	
//		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)&(~EXCLUDE_CRC)); /* length with CRC */

		for(i=0;i<6;i++)
			WRITE_MEM16(CPUQDM0+i*2,(i<<12));

		if(extData==NULL)
		{
			extData=rtlglue_malloc(2048);
			ASSERT(extData!=NULL);
#ifdef RTL865X_MODEL_KERNEL				
			extData=(char *)(UNCACHE_MASK|(uint32)extData);		
#endif
			memset(extData,0,2048);
		}

	
		/* WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)&(~EXCLUDE_CRC)); */ /* length with CRC */
		/* WRITE_MEM32(MACCR,READ_MEM32(MACCR)|BYPASS_TCRC); */ /* by pass CRC offload */

		while(1) /* polling - waitting packet come in */
		{

			/* Swich Core interrupt to notify the RISC that packets is ready	 */
			retval=swNic_intHandler(&i);
			swNic_rxThread(i);
			if(countRX>=0xf0000000) break;

		}

{
	uint32 port0out=0;

	WRITE_MEM32(0xbb801000,0xffffffff);

		
	i=0;
	
			while(1)
			{
				int ret;
				int datalen;
				int cnt2=0;
				

				struct rtl_pktHdr	*pktHdrTx;
				struct rtl_mBuf	*MblkTx;
				i++;

				datalen=_createSourcePacket(extData,L2_IP,20,IPPROTO_ICMP); /* length with CRC */
		
				MblkTx=mBuf_attachHeader((void*)extData,(uint32)0,2048, datalen,0);
				ASSERT(MblkTx!=NULL);			
				pktHdrTx=MblkTx->m_pkthdr;
				
				pktHdrTx->ph_portlist=0x1;

#if 0
				memDump(pktHdrTx,32,"pktHdrTx");
				memDump(MblkTx,32,"MblkTx");
				if(MblkTx->m_len>32)
					memDump(MblkTx->m_data,32,"data in TX");			
				else
					memDump(MblkTx->m_data,MblkTx->m_len,"data in TX");

/*				rtlglue_printf("TX test %d...\n",i); */
#endif

#if 0
				while(READ_MEM32(0xbb801808)!=READ_MEM32(0xbb80144c))
				{
				
					rtlglue_printf("i=%d desc 144c=%x , 1808=%x bb806100=%x\n",i,READ_MEM32(0xbb80144c),READ_MEM32(0xbb801808),READ_MEM32(0xbb806100));
					break;
				}
#endif				

			
				ret=swNic_write(pktHdrTx,1);

				
				ASSERT(ret==SUCCESS);		


				while(1)
				{
					if((READ_MEM32(CPUIISR)&(TX_DONE_IE1))==(TX_DONE_IE1))
					{				
						break;	
					}
					cnt2++;

					if(cnt2>10000)
					{
						rtlglue_printf("[start] wait for TX_DONE_IE. CPUIISR=%x countRX=%x\n",READ_MEM32(CPUIISR),i);		
						i=5000;
						break;
					}
					
					/*WRITE_MEM32(CPUICR , READ_MEM32(CPUICR) | TXFD );			*/
				}

				/*rtlglue_printf("READ_MEM32(CPUIISR) in fastRX=%x\n",READ_MEM32(CPUIISR));	*/
				WRITE_MEM32(CPUIISR,TX_DONE_IE1);
				/*rtlglue_printf("after clear READ_MEM32(CPUIISR) in fastRX=%x\n",READ_MEM32(CPUIISR));	*/

				port0out++;				
				if(port0out>2000000) break;
				

/*				port0out=READ_MEM32(0xbb801808); */
				
				
			}
			


}
		

		return SUCCESS;

	}
	return SUCCESS;
}


int32 testNicTxTagAddRemoveModify(uint32 caseNo)
{
	int i,j,k,l,m;

	int retval;


	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{

		int ret;
#ifdef _LITTLE_ENDIAN		
		int z;
#endif
		char *data,*data3;
		int datalen=0;
		int datalen3=0;
		struct rtl_pktHdr	*pktHdr;
		struct rtl_mBuf	*Mblk;	
		
		retval = model_setTestTarget( j );		
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		data=rtlglue_malloc(2048);
		ASSERT(data!=NULL);
		data3=rtlglue_malloc(2048);
		ASSERT(data3!=NULL);
		
#ifdef RTL865X_MODEL_KERNEL				
		data=(char *)(UNCACHE_MASK|(uint32)data);		
		data3=(char *)(UNCACHE_MASK|(uint32)data3);		
#endif

		memset(data,0,2048);	
		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();
		rtl8651_clearAsicPvid();
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);


		datalen3=_createSourcePacket(data3,L2_IP,20,IPPROTO_TCP); 


		for(m=0;m<2;m++) /* llc tag remove */
		{
			/*
				snap tag: sw_core don't have add/remove/modify capabilities.
				the sw_core only modify the snap length 
			*/
			for(l=0;l<4;l++)  /*  vlan tag add/remove/modify */
			{
				for(k=0;k<4;k++) /* do pppoe tag offload or not */
				{
					for(i=0;i<8;i++) /* translator to different type */
					{

						if(m==0)
						{
							switch(l)
							{
								case 0: /* do nothing */
								case 1: /* add vlan tag */							
									switch(k)
									{
										case 0: /* do nothing */
										case 1: /* add pppoe tag */
											datalen=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP); /* without CRC */
											break;							
										case 2: /* remove pppoe tag */
										case 3:	/* modify pppoe tag */		
											datalen=_createSourcePacket(data,L2_PPPOE64_IP,20,IPPROTO_TCP); /* without CRC */
											break;
									}
									break;
									
								case 2: /* remove vlan tag */
								case 3: /* modify vlan tag */
									switch(k)
									{
										case 0: /* do nothing */
										case 1: /* add pppoe tag */
											datalen=_createSourcePacket(data,L2_VLAN_IP,20,IPPROTO_TCP); /* without CRC */
											break;							
										case 2: /* remove pppoe tag */
										case 3:	/* modify pppoe tag */		
											datalen=_createSourcePacket(data,L2_VLAN_PPPOE64_IP,20,IPPROTO_TCP); /* without CRC */
											break;
									}
									break;
							}
						}	
						else
						{
							switch(l)
							{
								case 0: /* do nothing */
								case 1: /* add vlan tag */							
									switch(k)
									{
										case 0: /* do nothing */
										case 1: /* add pppoe tag */
											datalen=_createSourcePacket(data,L2_SNAP_IP,20,IPPROTO_TCP); /* without CRC */
											break;							
										case 2: /* remove pppoe tag */
										case 3:	/* modify pppoe tag */		
											datalen=_createSourcePacket(data,L2_SNAP_PPPOE64_IP,20,IPPROTO_TCP); /* without CRC */
											break;
									}
									break;
									
								case 2: /* remove vlan tag */
								case 3: /* modify vlan tag */
									switch(k)
									{
										case 0: /* do nothing */
										case 1: /* add pppoe tag */
											datalen=_createSourcePacket(data,L2_VLAN_SNAP_IP,20,IPPROTO_TCP); /* without CRC */
											break;							
										case 2: /* remove pppoe tag */
										case 3:	/* modify pppoe tag */		
											datalen=_createSourcePacket(data,L2_VLAN_SNAP_PPPOE64_IP,20,IPPROTO_TCP); /* without CRC */
											break;
									}
									break;
							}
						}
						
/*						memDump(data,datalen,"input"); */
			
						
						Mblk=mBuf_attachHeader((void*)data,(uint32)0,2048, datalen,0);
						ASSERT(Mblk!=NULL);
						pktHdr=Mblk->m_pkthdr;
						pktHdr->ph_portlist=0x1; /* to PORT 0 */
						pktHdr->ph_type = i; /* set each type for test */
						pktHdr->ph_flags|= CSUM_IP|CSUM_L4;


						if(m==1)
						{
							pktHdr->ph_LLCTagged=1;
						}

						switch(l)
						{
							case 0: /* do nothing */
								pktHdr->ph_vlanTagged = 0;
								pktHdr->ph_txCVlanTagAutoAdd &= (~PKTHDR_VLAN_P0_AUTOADD);	
								break;
							case 1: /* add vlan tag */
								pktHdr->ph_vlanTagged = 0;
								pktHdr->ph_txCVlanTagAutoAdd |= PKTHDR_VLAN_P0_AUTOADD;
								break;
							case 2: /* remove vlan tag */
								pktHdr->ph_vlanTagged = 1;
								pktHdr->ph_txCVlanTagAutoAdd &= (~PKTHDR_VLAN_P0_AUTOADD);	
								break;
							case 3:	/* modify vlan tag */
								pktHdr->ph_vlanTagged = 1;
								pktHdr->ph_txCVlanTagAutoAdd |= PKTHDR_VLAN_P0_AUTOADD;
								break;
						}

						switch(k)
						{
							case 0: /* do nothing */
								pktHdr->ph_pppeTagged=0;
								pktHdr->ph_flags &= (~PKTHDR_ORIGINAL_PPPOE_TAGGED);	
								break;
							case 1: /* add pppoe tag */
								pktHdr->ph_pppeTagged=0;
								pktHdr->ph_flags |= PKTHDR_ORIGINAL_PPPOE_TAGGED;
								break;
							case 2: /* remove pppoe tag */
								pktHdr->ph_pppeTagged=1;
								pktHdr->ph_flags &= (~PKTHDR_ORIGINAL_PPPOE_TAGGED);	
								break;
							case 3:	/* modify pppoe tag */
								pktHdr->ph_pppeTagged=1;
								pktHdr->ph_flags |= PKTHDR_ORIGINAL_PPPOE_TAGGED;
								break;
						}

/*						memDump(pktHdr,32,"pkthdr"); */

#ifdef _LITTLE_ENDIAN
						for(z=0;z<datalen;z+=4)
						{
							uint32 *p,q;
							p=(uint32*)&data[z];
							q=*p;
							*p=ntohl(q);
						}
#endif						
							

						ret=swNic_write(pktHdr,1);	
						/* mBuf_freeMbufChain(Mblk); */
						ASSERT(ret==SUCCESS);


						{
							uint8 data2[2048]={0};
							enum PORT_MASK port=PM_PORT_0;
							int len=2048;
							int retval=0;

							while(1)
							{
								if((READ_MEM32(CPUIISR)&(TX_ALL_DONE_IE1))==(TX_ALL_DONE_IE1))
								{
									WRITE_MEM32(CPUIISR,TX_ALL_DONE_IE1);
									break;					
								}
			/*					rtlglue_printf("READ_MEM32(CPUIISR)=%x\n",READ_MEM32(CPUIISR));	*/
							}

							virtualMacWaitCycle( 10000 ); /* try and error value */

							retval=virtualMacOutput(&port, data2, &len);
							if(retval==SUCCESS)
							{
					
								rtlglue_printf("len=%d snap=%d vlan=%d pppoe=%d type=%d\n",len,m,l,k,i);
/*								memDump(data2,len,"data"); */


								switch(l) /*  vlan tag add/remove/modify */
								{
									case 0:  /* vlan do nothing */
									case 2: /* remove vlan tag */
										switch(k) 
										{

											case 0: /* pppoe do nothing */
											case 2: /* remove pppoe tag */
												ASSERT(data2[14+8*m]==0x45);
												if(i==5) 	ASSERT(memcmp(&data2[14+8*m],&data3[14],datalen3-14)==0);
												break;

											case 1: /* add pppoe tag */
											case 3: /* modify pppoe tag */
												switch(i)
												{
													case 0: /* ether type */
														ASSERT(data2[14+8*m]==0x45);
														if(i==5) 	ASSERT(memcmp(&data2[14+8*m],&data3[14],datalen3-14)==0);
														break;
													case 1: /* PPTP */
													case 2: /* IP */
													case 3: /* ICMP */
													case 4: /* IGMP */
													case 5: /* TCP */
													case 6: /* UDP */
														ASSERT((data2[20+8*m]==0)&&(data2[21+8*m]==0x21));
														if(i==5) 	ASSERT(memcmp(&data2[22+8*m],&data3[14],datalen3-14)==0);
														break;
													case 7: /* IPv6 */
														/* 
															tony:
															this is a special case: when pppoe tag do modification , ppp type will not be changed 
															spec is disccues with cw_yang , chenyl 
														*/
														if(k==3)
															ASSERT((data2[20+8*m]==0)&&(data2[21+8*m]==0x21));
														else
															ASSERT((data2[20+8*m]==0)&&(data2[21+8*m]==0x57));
														if(i==5) 	ASSERT(memcmp(&data2[22+8*m],&data3[14],datalen3-14)==0);
														break;
												}
												break;
										}
										break;
										
									case 1: /* add vlan tag */
									case 3: /* modify vlan tag */
										switch(k)
										{

											case 0: /* pppoe do nothing */
											case 2: /* remove pppoe tag */
												ASSERT(data2[18+8*m]==0x45);
												if(i==5) 	ASSERT(memcmp(&data2[18+8*m],&data3[14],datalen3-14)==0);
												break;

											case 1: /* add pppoe tag */
											case 3: /* modify pppoe tag */
												switch(i)
												{
													case 0: /* ether type */
														ASSERT(data2[18+8*m]==0x45);
														if(i==5) 	ASSERT(memcmp(&data2[18+8*m],&data3[14],datalen3-14)==0);
														break;
													case 1: /* PPTP */
													case 2: /* IP */
													case 3: /* ICMP */
													case 4: /* IGMP */
													case 5: /* TCP */
													case 6: /* UDP */
														ASSERT((data2[24+8*m]==0)&&(data2[25+8*m]==0x21));
														if(i==5) 	ASSERT(memcmp(&data2[26+8*m],&data3[14],datalen3-14)==0);
														break;
													case 7: /* IPv6 */
														/* 
															tony:
															this is a special case: when pppoe tag do modification , ppp type will not be changed 
															spec is disccues with cw_yang , chenyl 
														*/														
														if(k==3)
															ASSERT((data2[24+8*m]==0)&&(data2[25+8*m]==0x21));
														else
															ASSERT((data2[24+8*m]==0)&&(data2[25+8*m]==0x57));
														break;
														if(i==5) 	ASSERT(memcmp(&data2[26+8*m],&data3[14],datalen3-14)==0);
												}
												break;
										}
										break;
										
								}


								

							}
							else
							{
								rtlglue_printf("virtualMacOutput FAILED........\n");
#ifdef RTL865X_MODEL_KERNEL							
								data=(char *)((uint32)data&(~UNCACHE_MASK));
								data3=(char *)((uint32)data3&(~UNCACHE_MASK));
#endif
								rtlglue_free(data);		
								rtlglue_free(data3);	
								return FAILED;
							}
				
						} /* end else */
					} /* end for */
				} /* end for k */
			} /* end for l */
		}/* end for m */
#ifdef RTL865X_MODEL_KERNEL		
		data=(char *)((uint32)data&(~UNCACHE_MASK));
#endif
		rtlglue_free(data);	
		return SUCCESS;
	}

	return SUCCESS;

}



int32 testNicTxGreChecksumOffload(uint32 caseNo)
{
	int j;

	int retval;
	char data[] = {


	0x00, 0x00, 0xe8, 0x66, 0x46, 0x85,   /* DMAC */
	0x00, 0x0c, 0x6e, 0x3a, 0x5d, 0x66,   /* SMAC */
	
	0x08, 0x00,   /* Ether Type */

	/* IP header */
	0x45, 0x00, 0x00, 0x5d, 0x52, 0x72, 0x00, 0x00, 
	0x80, 0x2f, 0x31, 0x42, 0xc0, 0xa8, 0x9a, 0x6e, 
	0xc0, 0xa8, 0x9a, 0xfe,

	/* GRE header */
	0x30, 0x01, 0x88, 0x0b, 0x00, 0x3d, 0x03, 0x00,
	0x00, 0x00, 0x00, 0x33, 0x21, 

	/* IP header */
	0x45, 0x00, 0x00, 0x3c, 0x52, 0x71, 0x00, 0x00,
	0x80, 0x01, 0x6a, 0xf9, 0xc0, 0xaa, 0xfe, 0x01, 
	0xc0, 0xa8, 0xfe, 0x01, 0x08, 0x00, 0x45, 0x5c,
	0x06, 0x00, 0x02, 0x00,

	/* ICMP header */
	0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
	0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61,
	0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,	0x69	

	};


	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{

		int ret;
#ifdef _LITTLE_ENDIAN		
		int z;
#endif

		int datalen=0;
		struct rtl_pktHdr	*pktHdr;
		struct rtl_mBuf	*Mblk;	
		
		retval = model_setTestTarget( j );		
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */


		ASSERT(data!=NULL);

		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();
		rtl8651_clearAsicPvid();
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		


		datalen= sizeof(data); /* without CRC */

		
		
		Mblk=mBuf_attachHeader((void*)data,(uint32)0,2048, datalen,0);
		ASSERT(Mblk!=NULL);			
		pktHdr=Mblk->m_pkthdr;
			
		pktHdr->ph_portlist=0x1; /* to PORT 0 */
		pktHdr->ph_type = 1; /* set type = GRE */
		pktHdr->ph_flags|= CSUM_IP|CSUM_L4;
		

		pktHdr->ph_vlanTagged=0;
		pktHdr->ph_txCVlanTagAutoAdd &= (~PKTHDR_VLAN_P0_AUTOADD);
		pktHdr->ph_pppeTagged=0;
		pktHdr->ph_flags &= (~PKTHDR_ORIGINAL_PPPOE_TAGGED);	

		memDump(data,datalen,"org");
		
#ifdef _LITTLE_ENDIAN
		for(z=0;z<datalen;z+=4)
		{
			uint32 *p,q;
			p=(uint32*)&data[z];
			q=*p;
			*p=ntohl(q);
		}
#endif	
		
		ret=swNic_write(pktHdr,1);	
		/* mBuf_freeMbufChain(Mblk); */
		ASSERT(ret==SUCCESS);

		{
			uint8 data2[2048]={0};
			enum PORT_MASK port=PM_PORT_0;
			int len=2048;
			int retval=0;

			while(1)
			{
				if((READ_MEM32(CPUIISR)&(TX_ALL_DONE_IE1))==(TX_ALL_DONE_IE1))
				{
					WRITE_MEM32(CPUIISR,TX_ALL_DONE_IE1);
					break;					
				}
/*					rtlglue_printf("READ_MEM32(CPUIISR)=%x\n",READ_MEM32(CPUIISR));	*/
			}

			virtualMacWaitCycle( 10000 ); /* try and error value */

			retval=virtualMacOutput(&port, data2, &len);
			if(retval==SUCCESS)
			{
#ifdef _LITTLE_ENDIAN
		for(z=0;z<datalen;z+=4)
		{
			uint32 *p,q;
			p=(uint32*)&data[z];
			q=*p;
			*p=ntohl(q);
		}
#endif				
				memDump(data2,len,"data");  
				ASSERT(memcmp(data,data2,datalen)==0);
		
			}
			else
			{
				rtlglue_printf("virtualMacOutput FAILED........\n");		
				return FAILED;
			}

		} /* end else */

		return SUCCESS;
	}

	return SUCCESS;

}

void dumpMibCounter(void)
{
	int i,j;

	rtlglue_printf("\nIn Port 0\n");
	for(i=0x100,j=0;i<=0x158;i+=4,j++)
	{
		if(j%4==0)
		{
			rtlglue_printf("%08x: ",0xbb801000+i);

		}
		rtlglue_printf("%08x ",(READ_MEM32(0xbb801000+i)));
		if(j%4==3)
		{
			rtlglue_printf("\n");

		}
	}
	
	rtlglue_printf("\nOut Port 0\n");
	for(i=0x800,j=0;i<=0x858;i+=4,j++)
	{
		if(j%4==0)
		{
			rtlglue_printf("%08x: ",0xbb801000+i);

		}
		rtlglue_printf("%08x ",(READ_MEM32(0xbb801000+i)));
		if(j%4==3)
		{
			rtlglue_printf("\n");

		}
	}

	rtlglue_printf("\nIn Port CPU\n");
	for(i=0x400,j=0;i<=0x458;i+=4,j++)
	{
		if(j%4==0)
		{
			rtlglue_printf("%08x: ",0xbb801000+i);

		}
		rtlglue_printf("%08x ",(READ_MEM32(0xbb801000+i)));
		if(j%4==3)
		{
			rtlglue_printf("\n");

		}
	}
	
	rtlglue_printf("\nOut Port CPU\n");
	for(i=0xb00,j=0;i<=0xb58;i+=4,j++)
	{
		if(j%4==0)
		{
			rtlglue_printf("%08x: ",0xbb801000+i);

		}
		rtlglue_printf("%08x ",(READ_MEM32(0xbb801000+i)));
		if(j%4==3)
		{
			rtlglue_printf("\n");

		}
	}	
	rtlglue_printf("\n");

}

int32 compareMibCounter(uint32 inOffset,uint32 *mibIn,uint32 outOffset,uint32* mibOut)
{
	int i;
	
	/* compare in port */
	for(i=0;i<88;i+=4)
	{
		if(mibIn[i>>2]!=(READ_MEM32(0xbb801000+inOffset+i))) 
		{
			rtlglue_printf("MIB address %x[%x,%x] is different.\n",0xbb801000+inOffset+i,mibIn[i>>2],(READ_MEM32(0xbb801000+inOffset+i)));
			return FAILED;
		}
	}

	/* compare out port */
	for(i=0;i<52;i+=4)
	{
		if(mibOut[i>>2]!=(READ_MEM32(0xbb801000+outOffset+i))) 
		{
			rtlglue_printf("MIB address %x[%x,%x] is different.\n",0xbb801000+outOffset+i,mibOut[i>>2],(READ_MEM32(0xbb801000+outOffset+i)));		
			return FAILED;
		}
	}	

	return SUCCESS;

}


int32 testNicTxMibCounter(uint32 caseNo)
{
	int i,j,k;

	int retval;

	char org_data[] = {


	0x00, 0x00, 0xe8, 0x66, 0x46, 0x85,   /* DMAC */
	0x00, 0x0c, 0x6e, 0x3a, 0x5d, 0x66,   /* SMAC */

/*	0x05,0xff,0xaa,0xaa,0x03,0,0,0,  */ /* for SNAP */

	
	0x08, 0x00,   /* Ether Type */

	/* IP header */

	0x45, 0x00, 0x01, 0x67, 0xa4, 0xd2, 0x40, 0x00, 
	0x80, 0x06, 0xaf, 0xde, 0xc0, 0xa8, 0x9a, 0x6e, 
	0x42, 0x66, 0x07, 0x63, 


	/* TCP header */

	0x10, 0x46, 0x00, 0x50, 0x92, 0x11, 
	0xa2, 0xdf, 0x4c, 0xa5, 0x5c, 0xdd, 0x50, 0x18,
	0x44, 0x70, 0xbc, 0x2c, 0x00, 0x00,


	/*  Payload */
};

	uint32 mibIn[22]={0};
	uint32 mibOut[13]={0};


	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{

		int ret;
		char *data;
		int datalen=0;
		struct rtl_pktHdr	*pktHdr;
		struct rtl_mBuf	*Mblk,*m;
		struct ip *iph;
		struct tcphdr *tc;
		uint8 *data2;


		
		retval = model_setTestTarget( j );		
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		if(j==IC_TYPE_MODEL) 
		{
			rtlglue_printf("Not Implemented!\n");
			return SUCCESS;
		}

		RoutingConfig();			
		retval=configNicInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		
		WRITE_MEM32(CCR,0); /* l2/l3/l4 checksum error not allow, ale will drop the packet */

		data=rtlglue_malloc(17000); 
		ASSERT(data!=NULL);
		data2=rtlglue_malloc(17000);
		ASSERT(data2!=NULL);
		
#ifdef RTL865X_MODEL_KERNEL				
		data=(char *)(UNCACHE_MASK|(uint32)data);			
		data2=(char *)(UNCACHE_MASK|(uint32)data2);
#endif


		/* set CPU port jumbo frame size */
		WRITE_MEM32(PCRP6,READ_MEM32(PCRP6)|AcptMaxLen_16K);
		WRITE_MEM32(PCRP7,READ_MEM32(PCRP7)|AcptMaxLen_16K);
		WRITE_MEM32(PCRP8,READ_MEM32(PCRP8)|AcptMaxLen_16K);		
		WRITE_MEM32(PCRP0,READ_MEM32(PCRP0)|AcptMaxLen_16K); /* set jumbo packet to 16370 bytes */

	/* trap all packet to CPU */
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



		for(i=0;i<10;i++)
		{

			memset(data,0,17000);			
			for(k=0;k<17;k++)
			{
				memset(data+k*1000,k,1000);
			}			
			memcpy(data,org_data,sizeof(org_data));
		

			/* reset mib counter */
			WRITE_MEM32(0xbb801000,0xffffffff);
			memset((void *)mibIn,0,88);
			memset((void *)mibOut,0,52);

		
			switch(i)				
			{
				case 0:
					datalen=64-4;
					break;
				case 1:
					datalen=127-4;
					break;							
				case 2:
					datalen=128-4;
					break;					
				case 3:
					datalen=256-4;
					break;					
				case 4:
					datalen=512-4;
					break;					
				case 5:
					datalen=1024-4;
					break;					
				case 6:
					datalen=2048-4;
					break;					
				case 7:
					datalen=32-4;
					break;
				case 8:
					datalen=64-4;
					data[0]=0xff;
					data[1]=0xff;
					data[2]=0xff;
					data[3]=0xff;
					data[4]=0xff;
					data[5]=0xff;
					break;
				case 9:
					datalen=64-4;
					data[0]=0x01;
					data[1]=0x02;
					data[2]=0x03;
					data[3]=0x04;
					data[4]=0x05;
					data[5]=0x06;
					break;
				case 10: /* pause frame will be drop in MAC RX parser , so we skip this test case  */
					datalen=64-4;
					memset(data,0,64);
					data[0]=0x01; /* DA: 01-80-C2-00-00-01 */
					data[1]=0x80;
					data[2]=0xc2;
					data[3]=0x00;
					data[4]=0x00;
					data[5]=0x01;
					data[12]=0x88; /* ether type : 0x8808 */
					data[13]=0x08;
					data[14]=0x00; /* opcode 1 */
					data[15]=0x01; /* opcode 2 */
					data[16]=0x00; /* params */
					data[17]=0x00;
					break;					
					
			}

						
			iph=(struct ip *)&data[14];
			tc=(struct tcphdr *)&data[34];
			iph->ip_len =htons(datalen-14); 
	/*
			memDump(org_data,sizeof(org_data),"12");
			memDump(data,128,"data"); */
			rtlglue_printf("before len=%d %d\n",datalen+4,i);


			

#ifdef _LITTLE_ENDIAN
			for(k=0;k<datalen;k+=4)
			{
				uint32 *p,q;
				p=(uint32*)&data[k];
				q=*p;
				*p=ntohl(q);
			}
#endif
			
	/*		Mblk=mBuf_attachHeader((void*)data,(uint32)0,2048, datalen,0); */

			Mblk=mBuf_attachHeaderJumbo((void*)data,(uint32)0,2048, datalen);
			ASSERT(Mblk!=NULL);


			
			m=Mblk;

			while(1)
			{
				/* memDump(m->m_data,32,"data");  */
				if(m->m_next==NULL) break;	
				m=m->m_next;
			}

			pktHdr=Mblk->m_pkthdr;
			pktHdr->ph_portlist=0x1; /* to PORT 0 */


			swNic_enableSwNicWriteTxFN(FALSE);
			ret=swNic_write(pktHdr,1);	

			if(i==7)
			{
				Mblk->m_len=32-4;
				pktHdr->ph_len=32-4;				
			}
			swNic_enableSwNicWriteTxFN(TRUE);

			WRITE_MEM32(CPUICR , READ_MEM32(CPUICR) | TXFD );

			
			/* mBuf_freeMbufChain(Mblk); */
			ASSERT( ret==SUCCESS );	

			{
				
				enum PORT_MASK port=PM_PORT_0;
				int len=2048;
				int retval=0;


				

				while(1)
				{
					if((READ_MEM32(CPUIISR)&(TX_ALL_DONE_IE1))==(TX_ALL_DONE_IE1))
					{
						WRITE_MEM32(CPUIISR,TX_ALL_DONE_IE1);
						break;					
					}
						rtlglue_printf("READ_MEM32(CPUIISR)=%x\n",READ_MEM32(CPUIISR));	
				}

				virtualMacWaitCycle( 5000 ); /* try and error value */

				retval=virtualMacOutput(&port, data2, &len);


#ifdef _LITTLE_ENDIAN
				for(k=0;k<datalen;k+=4)
				{
					uint32 *p,q;
					p=(uint32*)&data[k];
					q=*p;
					*p=ntohl(q);
				}
#endif
				
				if((retval==SUCCESS)||(i==7))
				{
					rtlglue_printf("after len=%d\n",len);
					memDump(data2,len,"out");
					dumpMibCounter();

					switch(i)				
					{
						case 0:
							mibIn[0x0c>>2]=64;
							mibIn[0x1c>>2]=1;
							mibIn[0x4c>>2]=1;
							mibOut[0x0>>2]=64;
							mibOut[0x8>>2]=1;
							break;
						case 1:
							mibIn[0x0c>>2]=127;
							mibIn[0x20>>2]=1;
							mibIn[0x4c>>2]=1;
							mibOut[0x0>>2]=127;
							mibOut[0x8>>2]=1;							
							break;							
						case 2:
							mibIn[0x0c>>2]=128;
							mibIn[0x24>>2]=1;
							mibIn[0x4c>>2]=1;
							mibOut[0x0>>2]=128;
							mibOut[0x8>>2]=1;								
							break;					
						case 3:
							mibIn[0x0c>>2]=256;
							mibIn[0x28>>2]=1;
							mibIn[0x4c>>2]=1;	
							mibOut[0x0>>2]=256;
							mibOut[0x8>>2]=1;	
							break;					
						case 4:
							mibIn[0x0c>>2]=512;
							mibIn[0x2c>>2]=1;
							mibIn[0x4c>>2]=1;	
							mibOut[0x0>>2]=512;
							mibOut[0x8>>2]=1;	
							break;					
						case 5:
							mibIn[0x0c>>2]=1024;
							mibIn[0x30>>2]=1;
							mibIn[0x4c>>2]=1;
							mibOut[0x0>>2]=1024;
							mibOut[0x8>>2]=1;								
							break;					
						case 6:
							mibIn[0x0c>>2]=2048;
							mibIn[0x38>>2]=1;
							mibIn[0x4c>>2]=1;
							mibOut[0x0>>2]=2048;
							mibOut[0x8>>2]=1;								
							break;					
						case 7:
							mibIn[0x0c>>2]=32;
							mibIn[0x18>>2]=1;
							mibIn[0x4c>>2]=1;						
							break;
						case 8:
							mibIn[0x0c>>2]=64;
							mibIn[0x1c>>2]=1;
							mibIn[0x4c>>2]=1;
							mibOut[0x0>>2]=64;
							mibOut[0x10>>2]=1;
							break;
						case 9:
							mibIn[0x0c>>2]=64;
							mibIn[0x1c>>2]=1;
							mibIn[0x4c>>2]=1;
							mibOut[0x0>>2]=64;
							mibOut[0xc>>2]=1;
							break;
						case 10:
							mibIn[0x0c>>2]=64;
							mibIn[0x1c>>2]=1;
							mibIn[0x4c>>2]=1;
							mibOut[0x0>>2]=64;
							mibOut[0x2c>>2]=1;
							break;
					}


					
					if(i!=7) ASSERT(memcmp(data,data2,len-4)==0);
					ASSERT(compareMibCounter(0x400,mibIn,0x800,mibOut)==SUCCESS);

					if(i==7)
					{
						len=32;
						memcpy(data2,data,32);
					}



					/* reset mib counter */
					WRITE_MEM32(0xbb801000,0xffffffff);
					memset((void *)mibIn,0,88);
					memset((void *)mibOut,0,52);
rtlglue_printf("from physical port %d\n",i);					
					
					retval=virtualMacInput(PN_PORT0,data2,len);
					if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);				
					virtualMacWaitCycle(5000);

					countRX=0;

					while(1)
					{
						retval=swNic_intHandler(&i);
						if(retval!=1/*TRUE*/) break;
						swNic_rxThread(i);
					}


					
					dumpMibCounter();

					switch(i)				
					{
						case 0:
							mibIn[0x08>>2]=1;
							mibIn[0x00>>2]=64;
							mibIn[0x0c>>2]=64;
							mibIn[0x1c>>2]=1;
							mibIn[0x44>>2]=1;
							break;
						case 1:
							mibIn[0x08>>2]=1;
							mibIn[0x00>>2]=127;
							mibIn[0x0c>>2]=127;							
							mibIn[0x20>>2]=1;
							mibIn[0x44>>2]=1;							

							break;							
						case 2:
							mibIn[0x08>>2]=1;
							mibIn[0x00>>2]=128;
							mibIn[0x0c>>2]=128;							
							mibIn[0x24>>2]=1;
							mibIn[0x44>>2]=1;							

							break;					
						case 3:
							mibIn[0x08>>2]=1;
							mibIn[0x00>>2]=256;
							mibIn[0x0c>>2]=256;							
							mibIn[0x28>>2]=1;
							mibIn[0x44>>2]=1;							

							break;					
						case 4:
							mibIn[0x08>>2]=1;
							mibIn[0x00>>2]=512;
							mibIn[0x0c>>2]=512;							
							mibIn[0x2c>>2]=1;
							mibIn[0x44>>2]=1;							

							break;					
						case 5:
							mibIn[0x08>>2]=1;
							mibIn[0x00>>2]=1024;
							mibIn[0x0c>>2]=1024;							
							mibIn[0x30>>2]=1;
							mibIn[0x44>>2]=1;							

							break;					
						case 6:
							mibIn[0x08>>2]=1;
							mibIn[0x00>>2]=2048;
							mibIn[0x0c>>2]=2048;							
							mibIn[0x34>>2]=1;
							mibIn[0x44>>2]=1;							

							break;					
						case 7:
							mibIn[0x0c>>2]=32;
							mibIn[0x18>>2]=1;
							mibIn[0x4c>>2]=1;
							break;
						case 8:
							mibIn[0x40>>2]=1;
							mibIn[0x00>>2]=64;
							mibIn[0x0c>>2]=64;
							mibIn[0x1c>>2]=1;
							mibIn[0x44>>2]=1;
							break;							
						case 9:
							mibIn[0x3c>>2]=1;
							mibIn[0x00>>2]=64;
							mibIn[0x0c>>2]=64;
							mibIn[0x1c>>2]=1;
							mibIn[0x44>>2]=1;
							break;
						case 10:
							mibIn[0x58>>2]=1;
							mibIn[0x00>>2]=64;
							mibIn[0x0c>>2]=64;
							mibIn[0x1c>>2]=1;
							mibIn[0x44>>2]=1;
							break;							
							
					}

					
for(k=0;k<88;k+=4)
{
	rtlglue_printf("mibIn[0x%x]=%d\n",k,mibIn[k>>2]);
}
for(k=0;k<52;k+=4)
{
	rtlglue_printf("mibOut[0x%x]=%d\n",k,mibOut[k>>2]);
}

					
					ASSERT(compareMibCounter(0x100,mibIn,0xb00,mibOut)==SUCCESS);


					
				}
				else
				{
					rtlglue_printf("virtualMacOutput FAILED........\n");  /* maybe CRC error , MAC drop the packet */
#ifdef RTL865X_MODEL_KERNEL						
					data=(char *)((uint32)data&(~UNCACHE_MASK));
					data2=(char *)((uint32)data2&(~UNCACHE_MASK));				
#endif				
					rtlglue_free(data2);
					rtlglue_free(data);				
					return FAILED;
				}

			} 
		}
#ifdef RTL865X_MODEL_KERNEL						
		data=(char *)((uint32)data&(~UNCACHE_MASK));
		data2=(char *)((uint32)data2&(~UNCACHE_MASK));				
#endif				
		rtlglue_free(data2);
		rtlglue_free(data);		
		return SUCCESS;
	}

	return SUCCESS;

}


/*
 *  Test CPU Reason of ALE Processing
 *
 *  In this case, we will test if ALE can generate correct CPU reason.
 *  We will input packet from HSB, and we expect HSA.why2cpu as we wish.
 *  The test sequence is the same as NIC Spec descripted.
 *
 *  1. Patterm Match Mirror to CPU (also source mirroring to Ext2)
 *  2. 802.1x Guest VLAN routing packet trap to CPU
 *  3. pre-ACL filtering trap to CPU (L3/L4 disabled, but DMAC==GMAC)
 *  4. pre-ACL filtering trap to CPU (DMAC==GMAC, but ether type is NOT IP)
 *  5. pre-ACL filtering trap to CPU (DMAC==01-80-c2-00-00-XX)
 *  6. pre-ACL filtering trap to CPU (Realtek Proprietary Ether Type)
 *  7. pre-ACL filtering trap to CPU (ACL enabled, fragment IP packet)
 *  8. ACL matched to CPU
 *  9. ACL matched to CPU for logging
 * 10. ACL passed, but before checking DMAC==GMAC (IP multicast table entry trap to CPU)
 * 11. ACL passed, but before checking DMAC==GMAC (multicast IP packet size exceeds outgoing VLAN MTU)
 * 12. ACL passed, but before checking DMAC==GMAC (IP multicast table lookup miss)
 * 13. L3/4 Action Required (TTL=0, SIP=RP, DIP=NE)
 *   . ALG trap (SIP=DMZ, DIP=DMZ)
 *   . NAPT/SPI/ICMP(SIP=NPI, DIP=RP)
 *   . pre-egress filter trap (SIP=LP, DIP=LP)
 *   . egress filter rule match to CPU
 *   . egress filter rule log to CPU
 *   . post egress filter trap
 *   . to single extension port (not to CPU) ==> The last matched ACL index is for WMM purpose.
 *   . to multiple extension ports (not to CPU) ==> The last matched ACL index is for WMM purpose.
 */
int32 testNicCpuReason(uint32 caseNo)
{
	enum IC_TYPE i;
	int32 retval;
	hsb_param_t _hsb = /* original setting, for recover */
	{
		spa: 0,			len: 124,
		vid: 11,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0xa0,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_IP,
		patmatch:0,		ethtype: 0x0800/*IP*/,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x21,0x11,0x00,0x00},
		hiprior:3,			snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch:0,
		extspa: 0,		extl2: 0
	};
	hsb_param_t hsb; /* for test */
	hsa_param_t hsa;
	ether_addr_t mac;
	rtl865x_tblAsicDrv_intfParam_t intf;
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	/* for( i = IC_TYPE_MODEL; i <= IC_TYPE_MODEL; i++ ) */
	/* for( i = IC_TYPE_REAL; i <= IC_TYPE_REAL; i++ ) */
	{


		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

//rtlglue_printf("TMCR=0x%08x\n", READ_MEM32(TMCR) );
TESTING_MODE;
		
		retval = layer4Config();
		ASSERT( retval==SUCCESS );
		WRITE_MEM32( SWTCR1, READ_MEM32(SWTCR1)|SelCpuReason); /* 865xB CPU Reason */

		memcpy( &hsb, &_hsb, sizeof(_hsb) );
		/**************************************************************
		 *  1. Patterm Match Mirror to CPU (also source mirroring to Ext2)
		 **************************************************************/
#if 0 /* We ignore mirror port test until model code comletes the mirror port function. */
		WRITE_MEM32( PMCR, (1<<(MirrorPortMsk_OFFSET+RTL8651_MAC_NUMBER+PN_PORT_EXT2)) |
		                   (PM_PORT_1<<MirrorRxPrtMsk_OFFSET) );
#endif
		WRITE_MEM32( PPMAR, EnPatternMatch_P1 | MatchOpRx1_MTCPU );
		hsb.spa = 1;
		hsb.patmatch = TRUE;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should not be 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dp)", hsa.dp, PM_PORT_NOTPHY|PM_PORT_0|PM_PORT_2, __FUNCTION__, __LINE__,i );
#if 0 /* We ignore mirror port test until model code comletes the mirror port function. */
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dpext)", hsa.dpext, PM_PORT_CPU|PM_PORT_EXT2, __FUNCTION__, __LINE__,i );
#else
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dpext)", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__,i );
#endif
		IS_EQUAL_INT_DETAIL("Bit 0 of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1, 0x1, __FUNCTION__, __LINE__,i );
		WRITE_MEM32( PMCR, 0 ); /* recover the default setting */
		WRITE_MEM32( PPMAR, 0 ); /* recover the default setting */
		hsb.patmatch = FALSE; /* recover the default setting */
		
		/**************************************************************
		 *  2. 802.1x Guest VLAN routing packet trap to CPU
		 **************************************************************/
		WRITE_MEM32( DOT1XPORTCR, Dot1XPB_P1En ); /* Enable Port1, but un-authorized */
		WRITE_MEM32( DOT1XMACCR, 0 );
		WRITE_MEM32( GVGCR, Dot1xUNAUTHBH_TOCPU );
		hsb.spa = 1;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should not be 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dp) (real, expect)", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dpext) (real, expect)", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit 0 of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1, 0x1, __FUNCTION__, __LINE__,i );
		WRITE_MEM32( DOT1XPORTCR, 0 ); /* recover the default setting */
		WRITE_MEM32( DOT1XMACCR, 0 ); /* recover the default setting */
		WRITE_MEM32( GVGCR, 0 ); /* recover the default setting */

		/**************************************************************
		 *  3. pre-ACL filtering trap to CPU (L3/L4 disabled, but DMAC==GMAC)
		 **************************************************************/
		WRITE_MEM32( MSCR, READ_MEM32(MSCR)&~(EN_L3|EN_L4) ); /* disable L3/L4 */
		hsb.spa = 1;
		memcpy( (void*)&hsb.da[0], (void*)strtomac(&mac, GW_LAN1_MAC), sizeof(mac) );
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should not be 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dp) (real, expect)", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dpext) (real, expect)", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit[4:1] of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1e, 0x1<<1, __FUNCTION__, __LINE__,i );
		WRITE_MEM32( MSCR, READ_MEM32(MSCR)|(EN_L3|EN_L4) ); /* recover value */

		/**************************************************************
		 *  4. pre-ACL filtering trap to CPU (DMAC==GMAC, but ether type is NOT IP)
		 **************************************************************/
		hsb.spa = 1;
		memcpy( (void*)&hsb.da[0], (void*)strtomac(&mac, GW_LAN1_MAC), sizeof(mac) );
		hsb.type = HSB_TYPE_ETHERNET;
		hsb.ethtype = 0x0806; /* ARP */
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should not be 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dp) (real, expect)", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dpext) (real, expect)", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit[4:1] of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1e, 0x1<<1, __FUNCTION__, __LINE__,i );
		memcpy( (void*)&hsb.da[0], (void*)strtomac(&mac, "ff-ff-ff-ff-ff-ff"), sizeof(mac) ); /* recover */
		hsb.type = HSB_TYPE_IP; /* recover */
		hsb.ethtype = 0x0800; /* IP, recover */

		/**************************************************************
		 *  5. pre-ACL filtering trap to CPU (DMAC==01-80-c2-00-00-XX)
		 **************************************************************/
		WRITE_MEM32( RMACR, MADDR20_2F|MADDR00_10|MADDR21|MADDR20|MADDR10|MADDR0E|MADDR03|MADDR02|MADDR00 );
		hsb.spa = 1;
		hsb.type = HSB_TYPE_ETHERNET;
		memcpy( (void*)&hsb.da[0], (void*)strtomac(&mac, "01-80-c2-00-00-01"), sizeof(mac) );
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should not be 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dp) (real, expect)", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dpext) (real, expect)", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit[4:1] of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1e, 0x1<<1, __FUNCTION__, __LINE__,i );
		hsb.type = HSB_TYPE_IP; /* recover */
		WRITE_MEM32( RMACR, 0 ); /* recover */
		memcpy( (void*)&hsb.da[0], (void*)strtomac(&mac, GW_LAN1_MAC), sizeof(mac) ); /* recover */

		/**************************************************************
		 *  6. pre-ACL filtering trap to CPU (Realtek Proprietary Ether Type)
		 **************************************************************/
		WRITE_MEM32( MSCR, READ_MEM32(MSCR)|EnRRCP2CPU );
		hsb.spa = 1;
		hsb.type = HSB_TYPE_ETHERNET;
		hsb.ethtype = 0x8899; /* Realtek Proprietary Protocol */
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should not be 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dp) (real, expect)", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dpext) (real, expect)", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit[4:1] of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1e, 0x1<<1, __FUNCTION__, __LINE__,i );
		hsb.type = HSB_TYPE_IP; /* recover */
		hsb.ethtype = 0x0800; /* recover */
		WRITE_MEM32( MSCR, READ_MEM32(MSCR)&~EnRRCP2CPU ); /* recover */

		/**************************************************************
		 *  7. pre-ACL filtering trap to CPU (ACL enabled, fragment IP packet)
		 **************************************************************/
		WRITE_MEM32( ALECR, READ_MEM32(ALECR)|Fragment2CPU );
		hsb.spa = 1;
		hsb.ipfo0_n = TRUE; /* IP Offset is not zero */
		memcpy( (void*)&hsb.da[0], (void*)strtomac(&mac, "01-80-c2-00-00-01"), sizeof(mac) );
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should not be 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dp) (real, expect)", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dpext) (real, expect)", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit[4:1] of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1e, 0x1<<1, __FUNCTION__, __LINE__,i );
		WRITE_MEM32( ALECR, READ_MEM32(ALECR)&~Fragment2CPU ); /* recover */
		hsb.ipfo0_n = FALSE; /* recover */
		memcpy( (void*)&hsb.da[0], (void*)strtomac(&mac, GW_LAN1_MAC), sizeof(mac) ); /* recover */

		/**************************************************************
		 *  8. ACL matched to CPU
		 **************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_CPU;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		retval = rtl8651_setAsicAclRule(78, &aclt);
		ASSERT( retval==SUCCESS );
	 	/* set ACL rule of LAN1 */
		intf.vid = LAN1_VID;
		retval = rtl8651_getAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf ); /* read original value */
		intf.inAclStart = 78;
		intf.inAclEnd = 78;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval==SUCCESS );
		/* input the packet */
		hsb.spa = 1;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should not be 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dp) (real, expect)", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dpext) (real, expect)", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit[4:1] of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1e, 0x2<<1, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit[11:5] of CPU Reason should be matched ACL index (real, expect)", hsa.why2cpu&0xfe0, 78<<5, __FUNCTION__, __LINE__,i );
		/* recover */
		intf.vid = LAN1_VID;
		retval = rtl8651_getAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf ); /* read original value */
		intf.inAclStart = 0;
		intf.inAclEnd = 0;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval==SUCCESS );

		/**************************************************************
		 *  9. ACL matched to CPU for logging
		 **************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_DROP_NOTIFY;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		retval = rtl8651_setAsicAclRule(56, &aclt);
		ASSERT( retval==SUCCESS );
	 	/* set ACL rule of LAN1 */
		intf.vid = LAN1_VID;
		retval = rtl8651_getAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf ); /* read original value */
		intf.inAclStart = 56;
		intf.inAclEnd = 56;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval==SUCCESS );
		/* input the packet */
		hsb.spa = 1;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should not be 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dp) (real, expect)", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU and Ext2 (hsa.dpext) (real, expect)", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit[4:1] of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1e, 0x3<<1, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit[11:5] of CPU Reason should be matched ACL index (real, expect)", hsa.why2cpu&0xfe0, 56<<5, __FUNCTION__, __LINE__,i );
		/* recover */
		intf.vid = LAN1_VID;
		retval = rtl8651_getAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf ); /* read original value */
		intf.inAclStart = 0;
		intf.inAclEnd = 0;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval==SUCCESS );

		/**************************************************************
		 * 10. ACL passed, but before checking DMAC==GMAC (IP multicast table entry trap to CPU)
		 *     This test is done in testIPMulticast().
		 **********************************************************************/
		/**************************************************************
		 * 11. ACL passed, but before checking DMAC==GMAC (multicast IP packet size exceeds outgoing VLAN MTU)
		 *     This test is done in testIPMulticast().
		 **********************************************************************/
		/**************************************************************
		 * 12. ACL passed, but before checking DMAC==GMAC (IP multicast table lookup miss)
		 *     This test is done in testIPMulticast().
		 **********************************************************************/

		/**************************************************************
		 * 13. L3/4 Action Required (TTL=0, SIP=RP, DIP=NE)
		 **********************************************************************/
		hsb.ttlst = 0; /* TTL=0 */
		hsb.vid = 9;
		hsb.spa = 4;
		hsb.sip = ntohl(inet_addr(HOST4_IP));
		hsb.dip = ntohl(inet_addr(GW_WAN1_IP));
		hsb.sprt = 5566;
		hsb.dprt = 7788;
		memcpy( (void*)&hsb.da[0], (void*)strtomac(&mac, GW_WAN1_MAC), sizeof(mac) );
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should not be 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU (hsa.dp) (real, expect)", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to CPU (hsa.dpext) (real, expect)", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit[4:1] of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1e, 0x5<<1, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit[9:5] of CPU Reason should be SIP=RP (real, expect)", (hsa.why2cpu>>5)&0x1f, 0x13/*RP*/, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Bit[14:10] of CPU Reason should be DIP=RP (real, expect)", (hsa.why2cpu>>10)&0x1f, 0x01/*NE,idx=1*/, __FUNCTION__, __LINE__,i );
		memcpy( &hsb, &_hsb, sizeof(_hsb) ); /* recover */

		/**************************************************************
		 *   . NAPT/SPI/ICMP(SIP=NPI, DIP=RP)
		 *     This case is done in item4, testNaptEnhancedHash1().
		 **********************************************************************/

		/**************************************************************
		 *   . to single extension port (not to CPU) ==> The last matched ACL index is for WMM purpose.
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt)); /* In Ingress ACL */
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		retval = rtl8651_setAsicAclRule(12, &aclt);
		ASSERT( retval==SUCCESS );
		memset((void*)&aclt, 0, sizeof(aclt)); /* for Egress ACL */
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		retval = rtl8651_setAsicAclRule(34, &aclt);
		ASSERT( retval==SUCCESS );
	 	/* set ACL rule of LAN1 */
		intf.vid = LAN1_VID;
		retval = rtl8651_getAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf ); /* read original value */
		intf.inAclStart = 12;
		intf.inAclEnd = 12;
		intf.outAclStart = 34;
		intf.outAclEnd = 34;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval==SUCCESS );
		/* add extension port for LAN1 */
		rtl8651_getAsicVlan( intf.vid, &vlant );
		vlant.memberPortMask |= (1<<6/*Ext0*/);
		vlant.untagPortMask |= (1<<6/*Ext0*/);
		rtl8651_setAsicVlan( intf.vid, &vlant );
		/* input the packet */
		hsb.spa = 2;
		memcpy( (void*)&hsb.da[0], (void*)strtomac(&mac, "00-11-12-00-00-20"/* uniknown unicast*/), sizeof(mac) );
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should not be 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to Ext0 (hsa.dp) (real, expect)", hsa.dp, PM_PORT_NOTPHY|PM_PORT_0|PM_PORT_1, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to Ext0 (hsa.dpext) (real, expect)", hsa.dpext, PM_PORT_EXT0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("CPU Reason[6:0] should be permited ingress ACL index (real, expect)", hsa.why2cpu&0x7f, 12, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("CPU Reason[13:7] should be permited ingress ACL index (real, expect)", (hsa.why2cpu>>7)&0x7f, 34, __FUNCTION__, __LINE__,i );
		/* recover */
		intf.vid = LAN1_VID;
		retval = rtl8651_getAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf ); /* read original value */
		intf.inAclStart = 0;
		intf.inAclEnd = 0;
		intf.outAclStart = 0;
		intf.outAclEnd = 0;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval==SUCCESS );

		/**************************************************************
		 *   . to multiple extension ports (not to CPU) ==> The last matched ACL index is for WMM purpose.
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt)); /* In Ingress ACL */
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		retval = rtl8651_setAsicAclRule(12, &aclt);
		ASSERT( retval==SUCCESS );
		memset((void*)&aclt, 0, sizeof(aclt)); /* for Egress ACL */
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		retval = rtl8651_setAsicAclRule(34, &aclt);
		ASSERT( retval==SUCCESS );
	 	/* set ACL rule of LAN1 */
		intf.vid = LAN1_VID;
		retval = rtl8651_getAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf ); /* read original value */
		intf.inAclStart = 12;
		intf.inAclEnd = 12;
		intf.outAclStart = 34;
		intf.outAclEnd = 34;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval==SUCCESS );
		/* add extension port for LAN1 */
		rtl8651_getAsicVlan( intf.vid, &vlant );
		vlant.memberPortMask |= (7<<6/*EXT0,EXT1,EXT2*/);
		vlant.untagPortMask |= (7<<6/*EXT0,EXT1,EXT2*/);
		rtl8651_setAsicVlan( intf.vid, &vlant );
		/* input the packet */
		hsb.spa = 2;
		memcpy( (void*)&hsb.da[0], (void*)strtomac(&mac, "00-11-12-00-00-20"/* uniknown unicast*/), sizeof(mac) );
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should not be 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to Ext0 (hsa.dp) (real, expect)", hsa.dp, PM_PORT_NOTPHY|PM_PORT_0|PM_PORT_1, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to Ext0 (hsa.dpext) (real, expect)", hsa.dpext, PM_PORT_EXT0|PM_PORT_EXT1|PM_PORT_EXT2, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("CPU Reason[6:0] should be permited ingress ACL index (real, expect)", hsa.why2cpu&0x7f, 12, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("CPU Reason[13:7] should be permited ingress ACL index (real, expect)", (hsa.why2cpu>>7)&0x7f, 34, __FUNCTION__, __LINE__,i );
		/* recover */
		intf.vid = LAN1_VID;
		retval = rtl8651_getAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf ); /* read original value */
		intf.inAclStart = 0;
		intf.inAclEnd = 0;
		intf.outAclStart = 0;
		intf.outAclEnd = 0;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval==SUCCESS );

	}

	return SUCCESS;
}

int32 testNicRxDRR(uint32 caseNo)
{

	int i,j,k,m,len=0;
	char data[2048]={0};
	int rxpktNum = 8;
	int rxpktRingIdx[rxpktNum];
	int rxpktSize[rxpktNum];
	int rxpktQtmAccumulate[] = {0,0,0,0,0,0};
	int rxpktOrder[rxpktNum];
	int refillMask = 0;
	int testRound = 3;
	int testcountRX = 0;
	int totalrxpktNum = rxpktNum * testRound;
	int retval;
	int curPriorityChecksum = PriorityChecksum =0;

	countRX=0;
	rxPkthdrRingRefillByDRR[0] = 600;
	rxPkthdrRingRefillByDRR[1] = 500;
	rxPkthdrRingRefillByDRR[2] = 400;
	rxPkthdrRingRefillByDRR[3] = 300;
	rxPkthdrRingRefillByDRR[4] = 200;
	rxPkthdrRingRefillByDRR[5] = 100;

	for( j = IC_TYPE_MIN;j < IC_TYPE_MAX; j++ )
	{
		retval = model_setTestTarget( j );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		RoutingConfig();		
		retval=configNicExtInit();
		ASSERT(retval==SUCCESS);
		WRITE_MEM32(CPUICR,READ_MEM32(CPUICR)|EXCLUDE_CRC);		
		parserInit(j);

		for(i=0;i<6;i+=2)
		{
			uint32 value;
			value=(i<<28)|((i+1)<<12);
//			rtlglue_printf("i<<28=%x (i+1)<<12=%x value=%x htonl(v)=%x\n",i<<28,(i+1)<<12,value,htonl(value));
			/* WRITE_MEM16(CPUQDM0+i*2,(i<<12)); */
			WRITE_MEM32(CPUQDM0+i*2,value);
		}


#if 0
		if(j==IC_TYPE_MODEL)
		{
			len=_createSourcePacket(data,L2_IP,20,IPPROTO_TCP)+4;
			simulateRX(0,TO_CPU,data,len);			
			simulateRX(5,TO_CPU,data,len);	
			simulateRX(3,TO_CPU,data,len);	
			simulateRX(4,TO_CPU,data,len);	
			simulateRX(2,TO_CPU,data,len);	
			simulateRX(5,TO_CPU,data,len);	
			simulateRX(1,TO_CPU,data,len);			
		}
		else			
#endif			
		{
			hsb_param_t rhsb;
			parserInit(j);




			WRITE_MEM32(QIDDPCR,0x10|(1<<PBP_PRI_OFFSET)); /* QueueID decision priority control register, set 802.1Q to first priority */
			/* WRITE_MEM32(QNUMCR,0x1b6db6); */ /* set all Port to 6 queue */
			WRITE_MEM32(QNUMCR,0x189249); /* set CPU Port to 6 queue */
			
			for(k=0x28;k<=0x78;k+=4)
			{
				/* set QDBFCRP0G0 ~  QDBFCRP6G2 */
				WRITE_MEM32(SBFCTR+k,0x00000006);
			}
			
			for(k=0x7c;k<=0xcc;k+=4)
			{
				WRITE_MEM32(SBFCTR+k,0x00000002); /* per port per group setting for packet threshold */
			}
			WRITE_MEM32(PQPLGR,0x1); /* per-class-queue physical length gap register */
			WRITE_MEM32(UPTCMCR5,0xb6c688); /* set Priority to QueueID */
			WRITE_MEM32(CPUQIDMCR0,0x55543210); /* set DP to QueueID mapping */
			WRITE_MEM32(LPTM8021Q,0xb6c688); /* linear mapping for 802.1Q priority */
			WRITE_MEM32(QRR,1); /* write one to apply setting value */

			/* FIXME: waitting for apply ok here */
	nextRound:
			for(k=0;k<rxpktNum;k++)
			{
				len=_createSourcePacket(data,L2_VLAN_IP,20,IPPROTO_TCP)+4;
				data[0]=0xff;
				data[1]=0xff;
				data[2]=0xff;
				data[3]=0xff;
				data[4]=0xff;
				data[5]=0xff;

				rxpktRingIdx[k] = rtlglue_random()%6;
				rxpktSize[k] = len;
				rxpktOrder[k] = -1;

//				printf("# %d: package size = %d\n",rxpktRingIdx[k],len);
				data[14] = rxpktRingIdx[k] << 5; /*vlan priority */
				data[15]=0; /* vlan ID low bits */
				data[18]=0x46; /* parse failed, direct TX to CPU */

				//rtlglue_printf("k=%d\n",k);
				/* memDump(data,32,"rx"); */
				retval=virtualMacInput(PN_PORT0,data,len);
				if(retval!=SUCCESS) rtlglue_printf("virtualMacInput FAILED! return value=%d\n",retval);				
				virtualMacWaitCycle(15000);
				virtualMacGetHsb(&rhsb);
				/* dumpHSB(&rhsb); */
				rtlglue_printf("hsb.hiprior=%d\n",rhsb.hiprior);
				
			}		
		
			i = 0;
			m = 0;
			while(i < rxpktNum)
			{
				k = 0;
				m++;
				while(k < rxpktNum)
				{
					if(rxpktSize[k]>0)
					{
						refillMask |= (1<<rxpktRingIdx[k]);
					}
					
					if((rxpktQtmAccumulate[rxpktRingIdx[k]] > rxpktSize[k]) & (rxpktOrder[k] < 0))
					{
						//printf("index:%d,Quantum:%d,pktsize:%d,round:%d\n",rxpktRingIdx[k],rxpktQtmAccumulate[rxpktRingIdx[k]],rxpktSize[k],m);
						rxpktQtmAccumulate[rxpktRingIdx[k]] -= rxpktSize[k];
						rxpktSize[k] = 0;
						rxpktOrder[k] = m;
						i++;
					}
					k++;
				}

				if(i==rxpktNum) break;

				for(j = 0; j < RTL865X_SWNIC_RXRING_MAX_PKTDESC; j++)
				{
					if(refillMask & (1<<j))
					{
						rxpktQtmAccumulate[j] += rxPkthdrRingRefillByDRR[j];
						refillMask &= ~(1<<j);
						//printf("#refill index:%d,Quantum:%d\n",j,rxpktQtmAccumulate[j]);
					}
				}
			}

			/* calculate priority checksum */
			j = 0;
			i = 0;
			while(i<rxpktNum)
			{
				for(m=5;m>=0;m--)
				{
					for(k=0;k<rxpktNum;k++)
					{
						if((rxpktOrder[k] == j) && (rxpktRingIdx[k] == m))
						{
							curPriorityChecksum += ((testcountRX+1)*(rxpktRingIdx[k]+1));
							//printf("CountRx:%d,Ring:%d,Quantum:%d,round:%d\n",testcountRX,rxpktRingIdx[k]+1,rxpktQtmAccumulate[rxpktRingIdx[k]],rxpktOrder[k]);
//							rtlglue_printf("#Num: %d Ring:%d, PriorityChecksum = %d, Order= %d\n",i,m,curPriorityChecksum,rxpktOrder[k]);
							i++;testcountRX++;
						}
					}
				}
				j++;
			}
		
		}
/*
swNic_dumpPkthdrDescRing();
swNic_dumpMbufDescRing();	
*/		

		/* Swich Core interrupt to notify the RISC that packets is ready	 */
		while(1)
		{
			retval=swNic_intHandler(&i);			
			if(retval!=1) break;
			swNic_rxThread(i);
		}
		

/*
swNic_dumpPkthdrDescRing();
swNic_dumpMbufDescRing();	
*/	

		testRound--;
		if(testRound > 0)	goto nextRound;

		if(PriorityChecksum!=curPriorityChecksum)
		{
			//rtlglue_printf("PriorityChecksum = %d::curPriorityChecksum=%d\n",PriorityChecksum,curPriorityChecksum);
			rtlglue_printf("pkt receive priority failed!\n");
			return FAILED;
		}

		if(countRX!=totalrxpktNum) 
		{
			rtlglue_printf("Failed when %dth packet RX!\n",countRX);
			return FAILED;
		}
		return SUCCESS;


		/* mBuf_freeMbufChain(pMbuf); */
	}

	return SUCCESS;
}

