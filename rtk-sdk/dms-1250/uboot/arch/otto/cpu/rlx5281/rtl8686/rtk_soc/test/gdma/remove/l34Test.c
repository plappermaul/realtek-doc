/*
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
*
* Program : Test Bench for 865xC
* Abstract :
* Author : (rupert@realtek.com.tw)
* $Id: l34Test.c,v 1.51 2007-05-04 08:22:51 yjlou Exp $
*/

#include <rtl_types.h>
#include <rtl_glue.h>
#if defined(RTL865X_TEST)||defined(RTL865X_MODEL_USER)
#define  CONFIG_RTL865XB
#include "utility.h"
#include <cle_userglue.h>
#else
#include "utility.h"
#endif
#include <cle_struct.h>
#include "pktGen.h"
#include "asicRegs.h"
#include "icTest.h"
#include "l2Test.h"
#include "l34Test.h"
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
#include "l34Model.h"
#include "dump.h"
#define VID_0 0
#define WAN0_VID 1
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

/* define INT/EXT/REM hosts */
#define INT2_IP HOST2_IP
#define EXT0_IP WAN0_ISP_PUBLIC_IP
#define REM1_IP "120.168.1.3"


hsb_param_t Ghsb =
{
		spa: 3,			len: 64,
		vid: 0x0,			tagif: 0,
		pppoeif: 0,		sip:0,
		sprt: 0,			dip:0,
		dprt: 0,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_TCP,
		patmatch:0,		ethtype: 0x0800,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 0x2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
};


/*
 *  *ip is return with host-order.
 *
 *  IP               0   1   2   3
 *     BIG-endian: 31_24 .....  7_0
 *  Little-endian:  7_0  ..... 31_24
 */
uint32  strtoip(ipaddr_t *ip, int8 *str)
{
    int32 t1, t2, t3, t4;
    sscanf((char*)str, "%d.%d.%d.%d", &t1, &t2, &t3, &t4);
    *ip = (t1<<24)|(t2<<16)|(t3<<8)|t4;
	return *(uint32*)ip;
}
uint32 getL2TableRowFromMacStrFID(int8 *str,int fid)
{
    int32 t1, t2, t3, t4, t5, t6;
    ether_addr_t mac;
    sscanf((char*)str, "%x-%x-%x-%x-%x-%x", &t1, &t2, &t3, &t4, &t5, &t6);
    mac.octet[0]=t1; mac.octet[1]=t2; mac.octet[2]=t3; mac.octet[3]=t4; mac.octet[4]=t5; mac.octet[5]=t6;
    return rtl8651_filterDbIndex(&mac,fid);
}

uint32 getL2TableRowFromMacStr(int8 *str)
{
    int32 t1, t2, t3, t4, t5, t6;
    ether_addr_t mac;
    sscanf((char*)str, "%x-%x-%x-%x-%x-%x", &t1, &t2, &t3, &t4, &t5, &t6);
    mac.octet[0]=t1; mac.octet[1]=t2; mac.octet[2]=t3; mac.octet[3]=t4; mac.octet[4]=t5; mac.octet[5]=t6;
    return rtl8651_filterDbIndex(&mac,0);
}
int32   rtl865xc_netIfIndex(int vid)  
{
	uint16 i;
	rtl865xc_tblAsic_netifTable_t	 netif;
	for (i=0;i<RTL865XC_NETIFTBL_SIZE;i++)
	{
		_rtl8651_readAsicEntry(TYPE_NETINTERFACE_TABLE, i, &netif);
		if (netif.vid==vid)
			return i;
	}
	rtlglue_printf("Error  !!! not  foud network interface..... %s %d\n",__FILE__,__LINE__);
	return RTL865XC_NETIFTBL_SIZE;
}
static uint8 port_number_of_host[3][MAX_PORT_NUMBER] = {{0,1,2,3,4,5},{4,3,2,1,0,5},{1,0,3,2,4,5}};
static uint8 *hp;

int32  layer3Config( int routetIsPPPoE )
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
	rtl8651_setAsicOperationLayer(3);
	/* TTL control reg: enable TTL-1 operation */
	WRITE_MEM32(ALECR,READ_MEM32(ALECR)|(uint32)EN_TTL1);	
	WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_51B_CPU_REASON);
	/* VLAN tagging control reg: accept all frame */
	//WRITE_MEM32( VCR0, EN_ALL_PORT_VLAN_INGRESS_FILTER);	



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
	vlant.memberPortMask = vlant.untagPortMask= (1<<hp[5]);
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
	vlant.memberPortMask = vlant.untagPortMask = (1<<hp[4])|(1<<hp[0]);
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
	vlant.memberPortMask = 0x3f;//(1<<hp[3]);
	vlant.untagPortMask = 0; //tagged
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
	vlant.memberPortMask = vlant.untagPortMask = (1<<hp[2])|(1<<hp[1])|(1<<hp[6]);
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
	l2t.memberPortMask = (1<<hp[5]);
	l2t.isStatic = 1;
	l2t.nhFlag=TRUE;
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(WAN0_ISP_MAC), 0, &l2t);
	/* wan 1 host */
	strtomac(&l2t.macAddr, HOST4_MAC);
	l2t.memberPortMask = (1<<hp[4]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST4_MAC), 0, &l2t);
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
	if (routetIsPPPoE==0)
	{
		routet.nhStart = 0;
		routet.nhNum = 2; /*index 4~5*/
		routet.nhNxt = 0;
		routet.nhAlgo = 2; /*per-source*/
		routet.ipDomain = 0;
		routet.internal=1;
	}
	else
	{
		routet.nhStart = 30;
		routet.nhNum = 2; /*index 4~5*/
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
	nxthpt.pppoeIdx = 0; 
	nxthpt.dvid =1;
	nxthpt.extIntIpIdx = 0; 
	nxthpt.isPppoe = 1;
	rtl8651_setAsicNextHopTable(30, &nxthpt);       

	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 0;
	nxthpt.dvid =0;
	nxthpt.extIntIpIdx = 0; 
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(31, &nxthpt);       



	return 0;
	
}

/*
 *
 *          +----+----+----+----+----+----+
 *          | P0 | P1 | P2 | P3 | P4 | P5 |
 *          +----+----+----+----+----+----+
 *      PVID  11   11   11  967    9    1
 * tag/untag                      tag  tag
 *  internal      LAN1      LAN0  WAN1  WAN0 default route: PPPoE, 192.168.4.1(NPE) ----- HOST5:192.168.4.5
 *              1.1/29     2.1/29  3.1/29(NE)
 *                 |          |     |
 *                 |          |     \---- HOST4:192.168.3.1/29
 *                 |          |
 *                 |          \---- HOST3:192.168.2.2 (NPI)
 *                 |
 *                 \--- HOST1:192.168.1.2(NI)
 */
int32  layer4Config( void )
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
	rtl8651_setAsicPvid(hp[5],WAN0_VID);  /*host 5 pvid 1*/
	rtl8651_setAsicPvid(hp[4],9);   /*host 4 pvid 1*/
	rtl8651_setAsicPvid(hp[3],LAN0_VID);  /*host 3 pvid 0x3c7*/
	rtl8651_setAsicPvid(hp[2],11);  /*host 2 pvid 3*/
	rtl8651_setAsicPvid(hp[1],11);  /*host 1 pvid 3*/
	rtl8651_setAsicPvid(hp[0],11);  /*host 6 pvid 3*/
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
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
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
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = vlant.untagPortMask = (1<<hp[4]);
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
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = (1<<hp[3]);
	vlant.untagPortMask = 0; //tagged
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
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = vlant.untagPortMask = (1<<hp[2])|(1<<hp[1])|(1<<hp[0]);
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
	l2t.memberPortMask = (1<<hp[0]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST6_MAC), 0, &l2t);
	    
	/* config pppoe table */
	bzero((void*) &pppoet, sizeof(pppoet));
	pppoet.sessionId = WAN0_ISP_PPPOE_SID;
	rtl8651_setAsicPppoe(0, &pppoet);
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
	routet.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	routet.nhStart = 0;
	routet.nhNum = 2; //index 0-1
	routet.nhNxt = 0;
	routet.nhAlgo = 2; //per-source
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
	nxthpt.pppoeIdx = 0; /*WAN0_ISP0_PPPOE_SID*/
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0; /*WAN0_ISP0_NPE_IP*/
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(0, &nxthpt);       

	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0; /*WAN0_ISP0_PPPOE_SID*/
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0; /*WAN0_ISP0_NPE_IP*/
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(1, &nxthpt);       


	
	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 0; /*WAN0_ISP0_PPPOE_SID*/
	nxthpt.dvid = rtl865xc_netIfIndex(WAN1_VID);
	nxthpt.extIntIpIdx = 0; /*WAN0_ISP0_NPE_IP*/
	nxthpt.isPppoe = 1;
	rtl8651_setAsicNextHopTable(30, &nxthpt);       

	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 0; /*WAN0_ISP0_PPPOE_SID*/
	nxthpt.dvid = rtl865xc_netIfIndex(WAN1_VID);
	nxthpt.extIntIpIdx = 0; /*WAN0_ISP0_NPE_IP*/
	nxthpt.isPppoe = 1;
	rtl8651_setAsicNextHopTable(29, &nxthpt);       	
	nxthpt.nextHopRow = getL2TableRowFromMacStr(HOST4_MAC);
	nxthpt.pppoeIdx = 0; /*WAN0_ISP0_PPPOE_SID*/
	nxthpt.dvid = rtl865xc_netIfIndex(WAN1_VID);
	nxthpt.extIntIpIdx = 0; /*WAN0_ISP0_NPE_IP*/
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(31, &nxthpt);       


	
	/* config ip table */
	bzero((void*) &ipt, sizeof(ipt));
	/* wan 0 napt */
	strtoip(&ipt.intIpAddr, "0.0.0.0");
	strtoip(&ipt.extIpAddr, WAN0_ISP_PUBLIC_IP);
	rtl8651_setAsicExtIntIpTable(0, &ipt);
	/* wan 1 nat */
	strtoip(&ipt.intIpAddr, HOST1_IP);
	strtoip(&ipt.extIpAddr, WAN1_ISP_PUBLIC_IP);
	ipt.nat = 1;
	rtl8651_setAsicExtIntIpTable(1, &ipt);
	return 0;
	
}


int32  SPIConfig( void )
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
	rtl8651_setAsicPvid(hp[0],11);  /*host 6 pvid 3*/
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
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
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
	WRITE_MEM32( PCRP1, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP2, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP3, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP4, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP5, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP6, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP7, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	WRITE_MEM32( PCRP8, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|STP_PortST_FORWARDING );
	bzero( &vlant, sizeof(vlant) );
	vlant.memberPortMask = vlant.untagPortMask = (1<<hp[4]);
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
	vlant.memberPortMask = (1<<hp[3]);
	vlant.untagPortMask = 0; //tagged
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
	vlant.memberPortMask = vlant.untagPortMask = (1<<hp[2])|(1<<hp[1])|(1<<hp[0]);
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
	l2t.memberPortMask = (1<<hp[0]);
	rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST6_MAC), 0, &l2t);
	    
	/* config pppoe table */
	bzero((void*) &pppoet, sizeof(pppoet));
	pppoet.sessionId = WAN0_ISP_PPPOE_SID;
	rtl8651_setAsicPppoe(0, &pppoet);
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
	routet.internal=1;
	routet.DMZFlag=1;

 	rtl8651_setAsicRouting(0, &routet);
	
	/* route to lan 0: use arp table [8:15] */
	strtoip(&routet.ipAddr, GW_LAN0_IP);
	strtoip(&routet.ipMask, GW_LAN0_SUBNET_MASK);
	routet.process = PROCESS_INDIRECT;
	routet.vidx = rtl865xc_netIfIndex(LAN0_VID);
	routet.arpStart = 1;
	routet.arpEnd = 1;
	routet.internal=0;
	routet.DMZFlag=1;
	rtl8651_setAsicRouting(1, &routet);
	 /* route to lan 1: use arp table [16:23] */
	 strtoip(&routet.ipAddr, GW_LAN1_IP);
	strtoip(&routet.ipMask, GW_LAN1_SUBNET_MASK);
	routet.process = PROCESS_INDIRECT;
	routet.vidx = rtl865xc_netIfIndex(LAN1_VID);
	routet.arpStart = 2;
	routet.arpEnd = 2;
	routet.internal=1;
	routet.DMZFlag=0;
	rtl8651_setAsicRouting(2, &routet);
	/* default route to wan 0: direct to modem */
	 bzero((void*) &routet, sizeof(routet));
	 strtoip(&routet.ipAddr, "0.0.0.0");
	strtoip(&routet.ipMask, "0.0.0.0");
	routet.process = PROCESS_NXT_HOP;
	routet.vidx = rtl865xc_netIfIndex(WAN0_VID);
	routet.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	routet.nhStart = 0;
	routet.nhNum = 2; //index 0-1
	routet.nhNxt = 0;
	routet.nhAlgo = 2; //per-source
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
	nxthpt.pppoeIdx = 0; //WAN0_ISP0_PPPOE_SID
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0; //WAN0_ISP0_NPE_IP
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(0, &nxthpt);       

	nxthpt.nextHopRow = getL2TableRowFromMacStr(WAN0_ISP_MAC);
	nxthpt.pppoeIdx = 0; //WAN0_ISP0_PPPOE_SID
	nxthpt.dvid = rtl865xc_netIfIndex(WAN0_VID);
	nxthpt.extIntIpIdx = 0; //WAN0_ISP0_NPE_IP
	nxthpt.isPppoe = 0;
	rtl8651_setAsicNextHopTable(1, &nxthpt);       
	
	/* config ip table */
	bzero((void*) &ipt, sizeof(ipt));
	/* wan 0 napt */
	strtoip(&ipt.intIpAddr, "0.0.0.0");
	strtoip(&ipt.extIpAddr, WAN0_ISP_PUBLIC_IP);
	rtl8651_setAsicExtIntIpTable(0, &ipt);
	/* wan 1 nat */
	strtoip(&ipt.intIpAddr, HOST1_IP);
	strtoip(&ipt.extIpAddr, WAN1_ISP_PUBLIC_IP);
	ipt.nat = 1;
	rtl8651_setAsicExtIntIpTable(1, &ipt);
	return 0;
	
}


int32 testLayer3Routing(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,		len: 64,
		vid: 0x0,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("192.168.1.2")),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,	
		patmatch:0,		ethtype: 0x0800,
		ipfg:0,			ipfo0_n:0,
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
		
	};
	
	hsa_param_t hsa;
	int i;
	ether_addr_t mac;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		layer3Config(0);			
		/*********************************************************************		
		Expect : Forward
		*********************************************************************/
		WRITE_MEM32(SWTCR0, (READ_MEM32(SWTCR0)&~LIMDBC_MASK) | LIMDBC_VLAN);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.dip=ntohl(inet_addr(HOST5_IP));
		hsb.sip=ntohl(inet_addr("192.168.1.2")),
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );	
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );
		/*********************************************************************		
		PORT  BASE Routing
		Expect : Forward
		*********************************************************************/
		WRITE_MEM32(SWTCR0,(READ_MEM32(SWTCR0)&~LIMDBC_MASK) | LIMDBC_PORT);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_WAN0_MAC),sizeof(ether_addr_t));
		hsb.spa=PN_PORT0;
		hsb.dip=ntohl(inet_addr(HOST5_IP));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************		
		PORT  BASE Routing
		Expect : NOT MATCH AND Layer2 Forward
		*********************************************************************/
		rtl865xc_setPortBasedNetif(0,3);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_4, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************		
		PORT  BASE Routing
		Expect : Routing and Forward
		*********************************************************************/
		hsb.spa=PN_PORT1;

		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);

		
		WRITE_MEM32( VCR0,READ_MEM32(VCR0) &~ EN_ALL_PORT_VLAN_INGRESS_FILTER);	
		hsb.vid=WAN0_VID;
		hsb.tagif=1;

		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);

		hsb.tagif=0;
		rtl865xc_setPortBasedNetif(1,3);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_4|PM_PORT_2, __FUNCTION__, __LINE__ ,i);		
		
		/*********************************************************************		
		MAC  BASE Routing
		Expect : Forward
		*********************************************************************/		
		WRITE_MEM32(SWTCR0,(READ_MEM32(SWTCR0) & ~LIMDBC_MASK)| LIMDBC_MAC);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_WAN0_MAC),sizeof(ether_addr_t));
		hsb.spa=0;
		hsb.dip=ntohl(inet_addr(HOST5_IP));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);

		/*********************************************************************		
		MAC  BASE Routing
		Expect : Forward
		*********************************************************************/		
		hsb.spa=PN_PORT1;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************		
		MAC  BASE Routing
		Expect : Forward
		*********************************************************************/		
		hsb.spa=PN_PORT2;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************		
		MAC  BASE Routing
		Expect : Forward
		*********************************************************************/		
		hsb.spa=PN_PORT3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);

		/*********************************************************************		
		MAC  BASE Routing
		Expect : Forward
		*********************************************************************/		
		hsb.spa=PN_PORT3;
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );		
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************		
		MAC  BASE Routing
		Expect : Forward
		*********************************************************************/		
		hsb.spa=PN_PORT4;
		retval = virtualMacSetHsb( &hsb );		
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....",PM_PORT_1|PM_PORT_2,hsa.dp, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************
		Testing 8 GATEWAY MACs
		*********************************************************************/
		layer3Config(0);		
		bzero(&intf,sizeof(intf));			
		strtomac(&intf.macAddr, GW_LAN0_MAC);
		intf.macAddr.octet[5]&=0xf0;
		intf.vid = LAN0_VID;
		intf.inAclStart = 0;
		intf.inAclEnd = 0;
		intf.outAclStart = intf.outAclEnd = 0;
		intf.valid=1;
		intf.macAddrNumber=8;
		intf.mtu=1500;
		intf.enableRoute=1;	
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
		vlant.memberPortMask = (1<<hp[3]);
		vlant.untagPortMask = 0; //tagged
		rtl8651_setAsicVlan(intf.vid,&vlant);
		memcpy((void*)&hsb,(void*)&Ghsb,sizeof(hsb));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.da[5] =intf.macAddr.octet[5] |0x1;
		hsb.sip=ntohl(inet_addr("192.168.1.2")),
		hsb.dip=ntohl(inet_addr(HOST5_IP));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************		
		Expect : Forward
		*********************************************************************/
		hsb.dip=ntohl(inet_addr(HOST5_IP));
		hsb.da[5] |=0x2;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************		
		Expect : Forward
		*********************************************************************/
		hsb.da[5] |=0x3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************		
		Expect : Forward
		*********************************************************************/
		hsb.da[5] |=0x7;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************
		 VID 0 Routing 
		Expect : Piroirty TAG
		*********************************************************************/
		hsb.dip=ntohl(inet_addr(HOST4_IP));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_4, __FUNCTION__, __LINE__ ,i);


	}
	retval = SUCCESS;	
	return retval;
}
int32 testLayer3MTU(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,		len:1300,
		vid: 0x0,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("192.168.1.2")),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,	
		patmatch:0,		ethtype: 0x0800,
					ipfo0_n:0,
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst:2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
		
	};
	hsa_param_t hsa;
	int i;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	ether_addr_t mac;
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );		
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */


			layer3Config(0);				
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			bzero( &intf, sizeof(intf) );
			strtomac(&intf.macAddr, GW_WAN0_MAC);
			intf.macAddrNumber = 1;
			intf.vid = WAN0_VID;
			intf.mtu = 1200;
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
			vlant.memberPortMask = vlant.untagPortMask = (1<<hp[5]);
			rtl8651_setAsicVlan(intf.vid,&vlant);
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			IS_EQUAL_INT("Pakcet should be to CPU,but ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ );
			assert( retval == SUCCESS );
			

	}
	retval = SUCCESS;	
	return retval;
}

int32 testRandomServerPort(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip:0,
		sprt: 1020,		dip:0,
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,
		patmatch:0 ,		ethtype: 0x0800,	
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst:2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0, 		urlmch: 1,
		extspa: 0,		extl2: 0,
		pppoeid:0,
	};
	hsa_param_t hsa;
	int i,loop;
	rtl865x_tblAsicDrv_serverPortParam_t	 srvt;
	ether_addr_t mac;	
	int result[4],ipaddr;
	uint16 internalPrt,externalPrt;
	rtl865x_tblAsicDrv_extIntIpParam_t ipt;	

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for (loop=0;loop<10000000;loop++)
	{
#ifdef VERBOSE	
		rtlglue_printf("Run %d\n", loop);
#endif
		internalPrt=rtlglue_random()&0xffff;
		externalPrt=rtlglue_random()&0xffff;
		ipaddr=rtlglue_random();
#ifdef VERBOSE	
		rtlglue_printf("internalPrt= 0x%08x;\n", internalPrt);
		rtlglue_printf("externalPrt =0x%08x;\n", externalPrt);
		rtlglue_printf("ipaddr =0x%08x;\n", ipaddr);
#endif


		for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
		{
			retval = model_setTestTarget( i );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_WAN0_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,HOST2_MAC),sizeof(ether_addr_t));



			layer4Config();

			/* config ip table */
			bzero((void*) &ipt, sizeof(ipt));
			/* wan 0 napt */
			strtoip(&ipt.intIpAddr, "0.0.0.0");
			ipt.extIpAddr= ipaddr;
			rtl8651_setAsicExtIntIpTable(0, &ipt);
	

			bzero((void*) &srvt, sizeof(srvt));
			/* server port 1020 translated to wan 0 isp port 20 */
			strtoip(&srvt.intIpAddr, HOST2_IP);

			
			srvt.extIpAddr=ipaddr;
			srvt.extPort = externalPrt;
			srvt.intPort =  internalPrt;
			srvt.valid = 1;
			rtl8651_setAsicServerPortTable(0, &srvt);

 			hsb.dprt=externalPrt;
			hsb.dip= ipaddr;
			hsb.sip=ntohl(inet_addr("122.168.4.1"));
			hsb.spa=5;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );	

			result[i]=hsa.l3csdt;
			result[i+2]=hsa.l4csdt;
		}		
		if (hsa.dp==0x40) continue;
		IS_EQUAL_INT("l3csdt  should be the same ....  ", result[0],result[1], __FUNCTION__, __LINE__);
		IS_EQUAL_INT("l4csdt should be the same ....  ", result[2],result[3], __FUNCTION__, __LINE__);
		
		}
	retval = SUCCESS;	
	return retval;
}
int32 testServerPort(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip:0,
		sprt: 1020,		dip:0,
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,
		patmatch:0 ,		ethtype: 0x0800,	
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst:2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0, 		urlmch: 1,
		extspa: 0,		extl2: 0,
		pppoeid:0,
	};
	hsa_param_t hsa;
	int i,ipaddr;
	uint16 externalPrt,internalPrt;
	rtl865x_tblAsicDrv_serverPortParam_t	 srvt;
	ether_addr_t mac;	
/*	 rtl865x_tblAsicDrv_extIntIpParam_t ipt;	*/

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
			layer4Config();				
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,HOST2_MAC),sizeof(ether_addr_t));
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );		
			
			/* config server port table */
			bzero((void*) &srvt, sizeof(srvt));
			/* server port 1020 translated to wan 0 isp port 20 */
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

 			hsb.sprt=INTERNAL_PORT;
			hsb.sip= ntohl(inet_addr("192.168.1.3")),
			hsb.dip=ntohl(inet_addr("122.168.4.1"));
			hsb.spa=3;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );	

			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("PORT should be to ....", hsa.port,EXTERNAL_PORT, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("l3csdt should be to ....", hsa.l3csdt,64769, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("l4csdt should be to ....", hsa.l4csdt,64779, __FUNCTION__, __LINE__,i );

			strtoip(&srvt.intIpAddr, HOST2_IP);
			strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
			srvt.extPort = EXTERNAL_PORT+10;
			srvt.intPort = INTERNAL_PORT;
			srvt.portRange=0;
			srvt.valid = 1;
			rtl8651_setAsicServerPortTable(0, &srvt);
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );	
			assert( retval == SUCCESS );	
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.l3csdt,64769, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.l4csdt,64769, __FUNCTION__, __LINE__,i );



			strtoip(&srvt.intIpAddr, HOST2_IP);
			strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
			srvt.extPort = EXTERNAL_PORT+20;
			srvt.intPort = INTERNAL_PORT;
			srvt.valid = 1;
			rtl8651_setAsicServerPortTable(0, &srvt);
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );			
			assert( retval == SUCCESS );	
			
			IS_EQUAL_INT_DETAIL("l3csdt  should be to ....", hsa.l3csdt,64769, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("l4csdt  should be to ....", hsa.l4csdt,64759, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );


			strtoip(&srvt.intIpAddr, HOST2_IP);
			strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
			srvt.extPort = EXTERNAL_PORT+30;
			srvt.intPort = INTERNAL_PORT;
			srvt.valid = 1;
			rtl8651_setAsicServerPortTable(0, &srvt);
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );			
			assert( retval == SUCCESS );	
			
			IS_EQUAL_INT_DETAIL("l3csdt  should be to ....", hsa.l3csdt,64769, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("l4csdt  should be to ....", hsa.l4csdt,64749, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );



			strtoip(&srvt.intIpAddr, HOST2_IP);
			strtoip(&srvt.extIpAddr, "10.0.0.1");
			srvt.extPort = EXTERNAL_PORT+30;
			srvt.intPort = INTERNAL_PORT;
			srvt.valid = 1;
			rtl8651_setAsicServerPortTable(0, &srvt);
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );			
			assert( retval == SUCCESS );	
			
			IS_EQUAL_INT_DETAIL("l3csdt  should be to ....", hsa.l3csdt,47018, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("l4csdt  should be to ....", hsa.l4csdt,46998, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );




			internalPrt= 0x0000e784;
			externalPrt =0x0000019c;
			ipaddr =0xe33af41c;

			bzero((void*) &srvt, sizeof(srvt));
			/* server port 1020 translated to wan 0 isp port 20 */
			strtoip(&srvt.intIpAddr, HOST2_IP);
			srvt.extIpAddr=ipaddr;
			srvt.extPort = externalPrt;
			srvt.intPort =  internalPrt;
			srvt.valid = 1;
			rtl8651_setAsicServerPortTable(0, &srvt);
 			hsb.sprt=internalPrt;
			hsb.sip= ntohl(inet_addr("192.168.1.3")),
			hsb.dip=ntohl(inet_addr("122.168.4.1"));
			hsb.spa=3;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			IS_EQUAL_INT_DETAIL("l3csdt  should be to ....", hsa.l3csdt,59987, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("l4csdt  should be to ....", hsa.l4csdt,53308, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );




			internalPrt= 0x00004f68;
			externalPrt =0x0000a75c;
			ipaddr =0x00532827;


			bzero((void*) &srvt, sizeof(srvt));
			/* server port 1020 translated to wan 0 isp port 20 */
			strtoip(&srvt.intIpAddr, HOST2_IP);

			srvt.extIpAddr=ipaddr;
			srvt.extPort = externalPrt;
			srvt.intPort =  internalPrt;
			srvt.valid = 1;
			rtl8651_setAsicServerPortTable(0, &srvt);
 			hsb.sprt=internalPrt;
			hsb.sip= ntohl(inet_addr("192.168.1.3")),
			hsb.dip=ntohl(inet_addr("122.168.4.1"));
			hsb.spa=3;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			IS_EQUAL_INT_DETAIL("l3csdt  should be to ....", hsa.l3csdt,39217, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("l4csdt  should be to ....", hsa.l4csdt,16701, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );



			


	
			


			/* config server port table */
			bzero((void*) &srvt, sizeof(srvt));
			strtoip(&srvt.intIpAddr, HOST2_IP);
			strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
			srvt.extPort = EXTERNAL_PORT;
			srvt.intPort = INTERNAL_PORT;
			srvt.protocol=SERVERP_PROTTCP;
			srvt.valid = 1;
			rtl8651_setAsicServerPortTable(0, &srvt);
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );	
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i );


			/*********************************************************************
			From  LAN  to WAN
			Port Bouncing
			Expect to CPU
 			*********************************************************************/
			hsb.dip=ntohl(inet_addr("192.168.4.1"));
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );	
			retval = virtualMacGetHsa( &hsa );			
			IS_EQUAL_INT_DETAIL("Pakcet should be to cpu ,but ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
			assert( retval == SUCCESS );	
			/*********************************************************************
	 	       From WAN to LAN 
			Protcol TCP
			Expect forward
			*********************************************************************/
      			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_WAN0_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-00-00-99-9c"),sizeof(ether_addr_t));
			hsb.dip= ntohl(inet_addr("192.168.4.1")),
			hsb.sip=ntohl(inet_addr("122.168.4.1"));
			hsb.dprt=EXTERNAL_PORT;
			hsb.type=HSB_TYPE_TCP;
			hsb.spa=5;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dp,PM_PORT_2, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("l3csdt should be to ....", hsa.l3csdt,766, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("l4csdt should be to ....", hsa.l4csdt,756, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("PORT should be to ....", hsa.port,INTERNAL_PORT, __FUNCTION__, __LINE__,i );
			bzero((void*) &srvt, sizeof(srvt));
			rtl8651_getAsicServerPortTable(0,&srvt);
			IS_EQUAL_INT_DETAIL("SERVER PORT Traffic Bit should be on  ....",srvt.traffic,1, __FUNCTION__, __LINE__,i );
			/*********************************************************************
			Testing Traffic bit
			*********************************************************************/			
			srvt.traffic=0;			
			rtl8651_setAsicServerPortTable(0,&srvt);
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );			
			bzero((void*) &srvt, sizeof(srvt));			
			rtl8651_getAsicServerPortTable(0,&srvt);
			IS_EQUAL_INT_DETAIL("SERVER PORT Traffic Bit should be on  ....", srvt.traffic,1, __FUNCTION__, __LINE__,i );			
			/*********************************************************************
		       From WAN to LAN 
			Protcol UDP
			Expect to CPU  due to Not MATCH
			*********************************************************************/
      			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_WAN0_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-00-00-99-9c"),sizeof(ether_addr_t));
			hsb.dip= ntohl(inet_addr("192.168.4.1")),
			hsb.sip=ntohl(inet_addr("122.168.4.1"));
			hsb.dprt=EXTERNAL_PORT;
			hsb.type=HSB_TYPE_UDP;
			hsb.spa=5;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );			
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i );
			/*********************************************************************
		       From WAN to LAN 
			Protcol UDP
			Expect to CPU  due to Not MATCH
			*********************************************************************/
      			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_WAN0_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-00-00-99-9c"),sizeof(ether_addr_t));
			hsb.dip= ntohl(inet_addr("192.168.4.1")),
			hsb.sip=ntohl(inet_addr("122.168.4.1"));
			hsb.dprt=EXTERNAL_PORT;
			hsb.type=HSB_TYPE_UDP;
			hsb.spa=5;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );			
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i );
			/*********************************************************************
		       From LAN to WAN
		       Server is Port Range
			Expect to Foward
			*********************************************************************/
      			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-00-00-99-9c"),sizeof(ether_addr_t));
			hsb.dip= ntohl(inet_addr("192.168.3.2")),
			hsb.sip=ntohl(inet_addr("192.168.1.3"));
			hsb.sprt=LOWER_PORT;
			hsb.type=HSB_TYPE_UDP;
			hsb.spa=3;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			IS_EQUAL_INT_DETAIL("l3csdt should be to ....", hsa.l3csdt,65025, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("l4csdt should be to ....", hsa.l4csdt,65025, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dp,PM_PORT_4, __FUNCTION__, __LINE__,i );
			#if 0
			/*********************************************************************
		       From LAN  to LAN 
		       MATCH Server Port AND NAT 		       
			Protcol TCP
			Expect Undefined to match server port  tableor nat table prioirty
			*********************************************************************/			
			/* wan 1 nat */
			strtoip(&ipt.intIpAddr, HOST2_IP);
			strtoip(&ipt.extIpAddr, WAN1_ISP_PUBLIC_IP);
			ipt.nat = 1;
			rtl8651_setAsicExtIntIpTable(1, &ipt);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,HOST2_MAC),sizeof(ether_addr_t));
			hsb.sip= ntohl(inet_addr(HOST2_IP));
			hsb.dip= ntohl(inet_addr(HOST5_IP)),
			hsb.spa=3;
			hsb.vid++;
			hsb.type=HSB_TYPE_TCP;
			hsb.sprt=INTERNAL_PORT;
			retval = virtualMacSetHsb( &hsb );			
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			IS_EQUAL_INT_DETAIL("Pakcet HSA.svid  ....", hsa.svid,hsb.vid, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.trip,ntohl(inet_addr(WAN1_ISP_PUBLIC_IP)), __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Port should be traslated   ....", hsa.port,INTERNAL_PORT, __FUNCTION__, __LINE__,i );
			#endif



	}
	retval = SUCCESS;	
	return retval;
}
int32 testALG(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("192.168.1.3")),
		sprt: 80,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,
		patmatch:0 ,		ethtype: 0x0800,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 0x2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	ether_addr_t mac;
	int i;
	rtl865x_tblAsicDrv_algParam_t algt;
	rtl865x_tblAsicDrv_serverPortParam_t	 srvt;
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		layer4Config();	
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		/* config server port table */
		bzero((void*) &srvt, sizeof(srvt));
		/* server port  */
		strtoip(&srvt.intIpAddr, HOST2_IP);
		strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		srvt.extPort = 20;
		srvt.intPort = 80;
		srvt.valid = 1;
		rtl8651_setAsicServerPortTable(0, &srvt);		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );		
		IS_EQUAL_INT_DETAIL("Pakcet should be to CPU but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );

		/* config alg table */
		bzero((void*) &algt, sizeof(algt));
		algt.port = 80; //trap http
		algt.protocol=ALG_PROTOCOL_BOTH;
		algt.direction=ALG_DIRECTION_WANTOLAN;
		rtl8651_setAsicAlg(0, &algt);
		hsb.dprt=20;
		hsb.sprt=80;		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be to CPU but ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);


		/* config alg table */
		bzero((void*) &algt, sizeof(algt));
		algt.port = 80; //trap http
		algt.protocol=ALG_PROTOCOL_BOTH;
		algt.direction=ALG_DIRECTION_LANTOWAN;
		rtl8651_setAsicAlg(0, &algt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be to forward but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );


		bzero((void*) &algt, sizeof(algt));
		algt.port = 80; //trap http
		algt.protocol=ALG_PROTOCOL_UDP;
		algt.direction=ALG_DIRECTION_BOTH;		
		rtl8651_setAsicAlg(0, &algt);
		hsb.type=HSB_TYPE_UDP;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be to forward but ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);


		hsb.type=HSB_TYPE_TCP;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be to forward but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );
		assert( retval == SUCCESS );	


		bzero((void*) &algt, sizeof(algt));
		algt.port = 20;
		algt.protocol=ALG_PROTOCOL_TCP1;
		algt.direction=ALG_DIRECTION_LANTOWAN;
		rtl8651_setAsicAlg(0, &algt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forwarded to CPU (hsa.dp) ....", hsa.dp,PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forwarded to CPU (hsa.dpext) ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);

		bzero((void*) &algt, sizeof(algt));
		algt.port = 20;
		algt.protocol=ALG_PROTOCOL_TCP1;
		algt.direction=ALG_DIRECTION_INVALID;
		rtl8651_setAsicAlg(0, &algt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forwarded to Port 5 ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );




		
		bzero((void*) &algt, sizeof(algt));
		algt.port = 20; //trap http
		algt.protocol=ALG_PROTOCOL_BOTH;
		algt.direction=ALG_DIRECTION_WANTOLAN;
		rtl8651_setAsicAlg(0, &algt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		IS_EQUAL_INT_DETAIL("Pakcet should be to forward but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		assert( retval == SUCCESS );	


	}
	retval = SUCCESS;	
	return retval;
}
int testLocalPublicIP(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr(WAN3_ISP_PUBLIC_IP)),
		sprt: 20,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,
		patmatch:0 ,		ethtype: 0x0800,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 0x2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i;
	rtl865x_tblAsicDrv_extIntIpParam_t ipt;
	ether_addr_t mac;
	rtl865x_tblAsicDrv_algParam_t algt;
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
			layer4Config();				
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			bzero((void*)&ipt, sizeof(rtl865x_tblAsicDrv_extIntIpParam_t));
			strtoip(&ipt.extIpAddr, WAN3_ISP_PUBLIC_IP);
			strtoip(&ipt.intIpAddr,WAN3_ISP_PUBLIC_IP);
			ipt.localPublic	= TRUE;
			ipt.nat			= FALSE;
			ipt.nhIndex		= 0;
			rtl8651_setAsicExtIntIpTable(2, &ipt);

			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_INT("Pakcet should be to ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ );
			
			bzero((void*) &algt, sizeof(algt));
			algt.port = 80; //trap http
			algt.protocol=ALG_PROTOCOL_BOTH;
			algt.direction=ALG_DIRECTION_LANTOWAN;
			rtl8651_setAsicAlg(0, &algt);

			bzero((void*) &algt, sizeof(algt));
			algt.port = 80; //trap http
			algt.protocol=ALG_PROTOCOL_BOTH;
			algt.direction=ALG_DIRECTION_WANTOLAN;
			rtl8651_setAsicAlg(1, &algt);

			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_INT("Pakcet should be to ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ );

	}
	retval = SUCCESS;	
	return retval;


}
int testACLIPRule(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr(WAN3_ISP_PUBLIC_IP)),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,
		patmatch:0 ,		ethtype: 0x0800,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 0x2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i;
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	rtl865x_tblAsicDrv_serverPortParam_t	 srvt;
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		layer4Config();						
		/* wan 0 */
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
		vlant.memberPortMask = vlant.untagPortMask = (1<<hp[5]);
		rtl8651_setAsicVlan(intf.vid,&vlant);
		/* LAN */
		bzero((void*) &intf, sizeof(intf));
	 	strtomac(&intf.macAddr, GW_LAN0_MAC);
		intf.vid = LAN0_VID;
		intf.mtu = 1500;
		intf.inAclStart = 0;
		intf.inAclEnd = 2;
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
		vlant.memberPortMask = (1<<3);
		vlant.untagPortMask = 0; /*tagged*/
		rtl8651_setAsicVlan(intf.vid,&vlant);
		/* config server port table */
		bzero((void*) &srvt, sizeof(srvt));
		/* server port 1020 translated to wan 0 isp port 20 */
		strtoip(&srvt.intIpAddr, HOST2_IP);
		strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		srvt.extPort = EXTERNAL_PORT;
		srvt.intPort = INTERNAL_PORT;
		srvt.valid = 1;
		rtl8651_setAsicServerPortTable(0, &srvt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(5, &aclt);
		rtl8651_setAsicAclRule(2, &aclt);
		/*********************************************************************
		Set A IP Rule : PERMIT 
		Default : DROP
		Expect MATCH and Forwrd
		***********************************************************************/		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.tos_				= 0x00;
		aclt.tosMask_			= 0x0;
		aclt.srcIpAddr_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrMask_	= 0xFFFFFF00;
		aclt.ruleType_			= RTL8651_ACL_IP;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_DROP;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.sprt=INTERNAL_PORT;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************
		Set A  IP RANGE Rule : PERMIT  ( distinguish  IP Rule) 
		Default is DROP
		Expect NOT MATCH and PACKET DROP
		***********************************************************************/		
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddr_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrMask_	= 0xFFFFFF00;
		aclt.ruleType_			= RTL8652_ACL_IP_RANGE;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__ ,i);		
		/*********************************************************************
		Set A  IP RANGE Rule : PERMIT
		Default is DROP
		Expect  MATCH and PACKET Forward
		***********************************************************************/		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrLB_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;		
		aclt.ruleType_			= RTL8652_ACL_IP_RANGE;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);		
		/*********************************************************************
		 Set A  IPRANGE DROP  ACL Rule  and pkOpApp= Layer 4 
		 Default is PERMIT
		 Expect :  Match  IP RANGE Rule AND DROP 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_IP_RANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__,i );		
		/**********************************************************************		
		 Set A  IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 2 and Layer 3
		 Default is PERMIT
		 Expect :  No match and Forward 
 		 **********************************************************************/
 		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_IP_RANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L2_AND_L3;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );	
				/**********************************************************************
		Set A  IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 3  and Layer 4
		Default is PERMIT
		Expect :  DROP 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_IP_RANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,0, __FUNCTION__, __LINE__,i );		
		/**********************************************************************
		Set A  IP RANGE DROP  ACL Rule  and pkOpApp= Only Layer 2 
		Default is PERMIT
		Expect :  FORWARD 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_IP_RANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i);		
		/**********************************************************************
		Set A  IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 2  and Layer3
		Default is PERMIT
		Expect :  FORWARD
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_IP_RANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L2_AND_L3;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );		
		/**********************************************************************
		Set A  IPRANGE DROP  ACL Rule  and pkOpApp= Layer 3 
		Default is PERMIT
		Expect :  FORWARD
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_IP_RANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L3;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);				

	}
	retval=SUCCESS;
	return retval;
}
int32 testACLIcmpRule(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr(WAN3_ISP_PUBLIC_IP)),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_ICMP,
		patmatch:0 ,		ethtype: 0x0800,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 0x2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i;
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		layer4Config();						
		/* wan 0 */
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
		vlant.memberPortMask = vlant.untagPortMask = (1<<hp[5]);
		rtl8651_setAsicVlan(intf.vid,&vlant);
		/* LAN */
		bzero((void*) &intf, sizeof(intf));
	 	strtomac(&intf.macAddr, GW_LAN0_MAC);
		intf.vid = LAN0_VID;
		intf.mtu = 1500;
		intf.inAclStart = 0;
		intf.inAclEnd = 2;
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
		vlant.memberPortMask = (1<<3);
		vlant.untagPortMask = 0; //tagged
		rtl8651_setAsicVlan(intf.vid,&vlant);
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(5, &aclt);
		rtl8651_setAsicAclRule(2, &aclt);
		/*********************************************************************
		Set A IP Rule : PERMIT 
		Default : DROP
		Expect MATCH and Forwrd
		***********************************************************************/		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.tos_				= 0x00;
		aclt.tosMask_			= 0x0;
		aclt.srcIpAddr_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrMask_	= 0xFFFFFF00;
		aclt.ruleType_			= RTL8651_ACL_ICMP;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_DROP;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		hsb.sip= ntohl(inet_addr(HOST1_IP));
		hsb.type=HSB_TYPE_ICMP;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************
		Set A  ICMP IP RANGE Rule : PERMIT  ( distinguish  IP Rule) 
		Default is DROP
		Expect NOT MATCH and PACKET DROP
		***********************************************************************/		
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddr_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrMask_	= 0xFFFFFF00;
		aclt.ruleType_			= RTL8652_ACL_ICMP_IPRANGE;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__ ,i);		
		/*********************************************************************
		Set A  ICMP IP RANGE Rule : PERMIT
		Default is DROP
		Expect  MATCH and PACKET Forward
		***********************************************************************/		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrLB_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;		
		aclt.ruleType_			= RTL8652_ACL_ICMP_IPRANGE;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);		
		/*********************************************************************
		 Set A  ICMP IP RANGE DROP  ACL Rule  and pkOpApp= Layer 4 
		 Default is PERMIT
		 Expect :  Match ICMP  IP RANGE Rule AND DROP 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_ICMP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__,i );		
		/**********************************************************************		
		 Set A ICMP IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 2 and Layer 3
		 Default is PERMIT
		 Expect :  No match and Forward 
 		 **********************************************************************/
 		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_ICMP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L2_AND_L3;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );	
		/**********************************************************************
		Set A  ICMP IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 3  and Layer 4
		Default is PERMIT
		Expect :  DROP 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_ICMP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,0, __FUNCTION__, __LINE__,i );		
		/**********************************************************************
		Set A  ICMP IP RANGE DROP  ACL Rule  and pkOpApp= Only Layer 2 
		Default is PERMIT
		Expect :  FORWARD 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_ICMP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i);		
		/**********************************************************************
		Set A  ICMP IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 2  and Layer3
		Default is PERMIT
		Expect :  FORWARD
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_ICMP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L2_AND_L3;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );		
		/**********************************************************************
		Set A  ICMP IP RANGE DROP  ACL Rule  and pkOpApp= Layer 3 
		Default is PERMIT
		Expect :  FORWARD
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_ICMP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L3;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);				

	}
	retval=SUCCESS;
	return retval;

}
int32 testAclIgmpRule(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr(WAN3_ISP_PUBLIC_IP)),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_IGMP,
		patmatch:0 ,		ethtype: 0x0800,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 0x2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i;
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		layer4Config();						
		/* wan 0 */
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
		vlant.memberPortMask = vlant.untagPortMask = (1<<hp[5]);
		rtl8651_setAsicVlan(intf.vid,&vlant);
		/* LAN */
		bzero((void*) &intf, sizeof(intf));
	 	strtomac(&intf.macAddr, GW_LAN0_MAC);
		intf.vid = LAN0_VID;
		intf.mtu = 1500;
		intf.inAclStart = 0;
		intf.inAclEnd = 2;
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
		vlant.memberPortMask = (1<<3);
		vlant.untagPortMask = 0; //tagged
		rtl8651_setAsicVlan(intf.vid,&vlant);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(5, &aclt);
		rtl8651_setAsicAclRule(2, &aclt);
		/*********************************************************************
		Set A IGMP  Rule : PERMIT 
		Default : DROP
		Expect MATCH and Forwrd
		***********************************************************************/		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddr_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrMask_	= 0xFFFFFF00;
		aclt.ruleType_			= RTL8651_ACL_IGMP;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_DROP;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		hsb.pppoeif=1;
		hsb.sip= ntohl(inet_addr(HOST1_IP));
		hsb.dip= ntohl(inet_addr("144.0.0.22"));
		hsb.type=HSB_TYPE_IGMP;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************
		Set A  IGMP IP RANGE Rule : PERMIT  ( distinguish  IP Rule) 
		Default is DROP
		Expect NOT MATCH and PACKET DROP
		***********************************************************************/		
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddr_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrMask_	= 0xFFFFFF00;
		aclt.ruleType_			= RTL8652_ACL_IGMP_IPRANGE;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__ ,i);		
		/*********************************************************************
		Set A  ICMP IP RANGE Rule : PERMIT
		Default is DROP
		Expect  MATCH and PACKET Forward
		***********************************************************************/		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrLB_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;		
		aclt.ruleType_			= RTL8652_ACL_IGMP_IPRANGE;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);		
		/*********************************************************************
		 Set A  ICMP IP RANGE DROP  ACL Rule  and pkOpApp= Layer 4 
		 Default is PERMIT
		 Expect :  Match ICMP  IP RANGE Rule AND DROP 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_IGMP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__,i );		
		/**********************************************************************		
		 Set A ICMP IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 2 and Layer 3
		 Default is PERMIT
		 Expect :  No match and Forward 
 		 **********************************************************************/
 		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_IGMP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L2_AND_L3;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );	
		/**********************************************************************
		Set A  ICMP IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 3  and Layer 4
		Default is PERMIT
		Expect :  DROP 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_IGMP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,0, __FUNCTION__, __LINE__,i );		
		/**********************************************************************
		Set A  ICMP IP RANGE DROP  ACL Rule  and pkOpApp= Only Layer 2 
		Default is PERMIT
		Expect :  FORWARD 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_IGMP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i);		
		/**********************************************************************
		Set A  ICMP IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 2  and Layer3
		Default is PERMIT
		Expect :  FORWARD
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_IGMP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L2_AND_L3;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );		
		/**********************************************************************
		Set A  IGMP IP RANGE DROP  ACL Rule  and pkOpApp= Layer 3 
		Default is PERMIT
		Expect :  FORWARD
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.ruleType_			= RTL8652_ACL_IGMP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L3;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);				

	}
	retval=SUCCESS;
	return retval;

}



int testACLUDPRule(uint32 caseNo)
{	
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr(WAN3_ISP_PUBLIC_IP)),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,
		patmatch:0 ,		ethtype: 0x0800,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 0x2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i;
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	rtl865x_tblAsicDrv_serverPortParam_t	 srvt;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		layer4Config();						
		/* wan 0 */
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
		vlant.memberPortMask = vlant.untagPortMask = (1<<hp[5]);
		rtl8651_setAsicVlan(intf.vid,&vlant);
		/* LAN */
		bzero((void*) &intf, sizeof(intf));
	 	strtomac(&intf.macAddr, GW_LAN0_MAC);
		intf.vid = LAN0_VID;
		intf.mtu = 1500;
		intf.inAclStart = 0;
		intf.inAclEnd = 2;
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
		vlant.memberPortMask = (1<<3);
		vlant.untagPortMask = 0; //tagged
		rtl8651_setAsicVlan(intf.vid,&vlant);
		/* config server port table */
		bzero((void*) &srvt, sizeof(srvt));
		/* server port 1020 translated to wan 0 isp port 20 */
		strtoip(&srvt.intIpAddr, HOST2_IP);
		strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		srvt.extPort = EXTERNAL_PORT;
		srvt.intPort = INTERNAL_PORT;
		srvt.valid = 1;
		rtl8651_setAsicServerPortTable(0, &srvt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(5, &aclt);
		rtl8651_setAsicAclRule(2, &aclt);

		/*********************************************************************
		Set A TCP Rule : PERMIT 
		Default : DROP
		Expect MATCH and Forwrd
		***********************************************************************/		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.tos_				= 0x00;
		aclt.tosMask_			= 0x0;
		aclt.srcIpAddr_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrMask_	= 0xFFFFFF00;
		aclt.udpSrcPortUB_		=0xFFFF;
		aclt.udpDstPortUB_		=0xFFFF;
		aclt.ruleType_ = RTL8651_ACL_UDP;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_DROP;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.sprt=INTERNAL_PORT;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************
		Set A UDP IP RANGE Rule : PERMIT  ( distinguish  IP Rule) 
		Default is DROP
		Expect NOT MATCH and PACKET DROP
		***********************************************************************/		
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddr_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrMask_	= 0xFFFFFF00;
		aclt.udpSrcPortUB_		= 0xFFFF;
		aclt.udpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_UDP_IPRANGE;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__ ,i);		
		/*********************************************************************
		Set A TCP IP RANGE Rule : PERMIT
		Default is DROP
		Expect  MATCH and PACKET Forward
		***********************************************************************/		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrLB_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;		
		aclt.udpSrcPortUB_		= 0xFFFF;
		aclt.udpDstPortUB_		= 0xFFFF;
		aclt.ruleType_ 		= RTL8652_ACL_UDP_IPRANGE;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);		
		/*********************************************************************
		 Set A UDP IPRANGE DROP  ACL Rule  and pkOpApp= Layer 4 
		 Default is PERMIT
		 Expect :  Match TCP IP RANGE Rule AND DROP 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.udpSrcPortUB_		=0xFFFF;
		aclt.udpDstPortUB_		=0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_UDP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__,i );		
		/**********************************************************************		
		 Set A UDP IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 2 and Layer 3
		 Default is PERMIT
		 Expect :  No match and Forward 
 		 **********************************************************************/
 		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_UDP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L2_AND_L3;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );	
				/**********************************************************************
		Set A UDP IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 3  and Layer 4
		Default is PERMIT
		Expect :  DROP 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.udpSrcPortUB_		= 0xFFFF;
		aclt.udpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_UDP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,0, __FUNCTION__, __LINE__,i );		
		/**********************************************************************
		Set A TCP IP RANGE DROP  ACL Rule  and pkOpApp= Only Layer 2 
		Default is PERMIT
		Expect :  FORWARD 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.udpSrcPortUB_		=0xFFFF;
		aclt.udpDstPortUB_		=0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_UDP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i);		
		/**********************************************************************
		Set A UDP IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 2  and Layer3
		Default is PERMIT
		Expect :  FORWARD
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.udpSrcPortUB_		= 0xFFFF;
		aclt.udpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_UDP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L2_AND_L3;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );		
		/**********************************************************************
		Set A UDP IPRANGE DROP  ACL Rule  and pkOpApp= Layer 3 
		Default is PERMIT
		Expect :  FORWARD
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.udpSrcPortUB_		= 0xFFFF;
		aclt.udpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_UDP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L3;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);				



	}
	retval=SUCCESS;
	return retval;

}
int testACLTCPRule(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsa_param_t hsa;
	hsb_param_t hsb;
	int i;
	ether_addr_t mac;
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	rtl865x_tblAsicDrv_serverPortParam_t	 srvt;
	rtl865x_tblAsicDrv_l2Param_t l2t;
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		layer4Config();						
		memcpy((void*)&hsb,(void*)&Ghsb,sizeof(hsb));
		/*********************************************************************
		override VLAN Setting
		*********************************************************************/
		/* wan 0 */
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
		vlant.memberPortMask = vlant.untagPortMask = (1<<hp[5])|0x1FF;
		rtl8651_setAsicVlan(intf.vid,&vlant);
		
		/* wan 1 */
		strtomac(&intf.macAddr, GW_WAN1_MAC);
		intf.macAddrNumber = 1;
		intf.vid = WAN1_VID;
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
		vlant.memberPortMask = vlant.untagPortMask = (1<<hp[4])|0x1FF;
		rtl8651_setAsicVlan(intf.vid,&vlant);

		/* LAN0 */
		 bzero((void*) &intf, sizeof(intf));
	 	strtomac(&intf.macAddr, GW_LAN0_MAC);
		intf.vid = LAN0_VID;
		intf.mtu = 1500;
		intf.inAclStart = 0;
		intf.inAclEnd = 2;
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
		vlant.memberPortMask = (1<<hp[3])|(1<<hp[2])|(1<<hp[1]);
		vlant.untagPortMask = 0; //tagged
		rtl8651_setAsicVlan(intf.vid,&vlant);
		
		/* LAN1 */
		bzero((void*) &intf, sizeof(intf));
	 	strtomac(&intf.macAddr, GW_LAN1_MAC);
		intf.vid = LAN1_VID;
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
		vlant.memberPortMask = vlant.untagPortMask = (1<<hp[0]);
		rtl8651_setAsicVlan(intf.vid,&vlant);
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_OUT_ACL));		

		bzero(&l2t,sizeof(l2t));
		strtomac(&l2t.macAddr, "00-FF-FF-00-00-00");
		l2t.memberPortMask = (1<<hp[4]);
		l2t.isStatic = 1;
		l2t.nhFlag=TRUE;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-FF-FF-00-00-00"), 0, &l2t);

		/* config server port table */
		bzero((void*) &srvt, sizeof(srvt));
		/* server port 1020 translated to wan 0 isp port 20 */
		strtoip(&srvt.intIpAddr, HOST2_IP);
		strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		srvt.extPort = EXTERNAL_PORT;
		srvt.intPort = INTERNAL_PORT;
		srvt.valid = 1;
		rtl8651_setAsicServerPortTable(0, &srvt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(5, &aclt);
		rtl8651_setAsicAclRule(2, &aclt);
		/*********************************************************************
		Set A TCP Rule : PERMIT 
		Default : DROP
		Expect MATCH and Forwrd
		***********************************************************************/		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.tos_				= 0x00;
		aclt.tosMask_			= 0x0;
		aclt.srcIpAddr_		= ntohl(inet_addr(HOST2_IP)) & 0xFFFFFF00;
		aclt.srcIpAddrMask_	= 0xFFFFFF00;
		aclt.dstIpAddr_		= 0x00000000;
		aclt.dstIpAddrMask_	= 0x00000000;			 
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_ 		= RTL8651_ACL_TCP;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/*********************************************************************
		Set A TCP IP RANGE Rule : PERMIT  ( distinguish  IP Rule) 
		Default is DROP
		Expect NOT MATCH and PACKET DROP
		***********************************************************************/		
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddr_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrMask_	= 0xFFFFFF00;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_ 		= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__ ,i);		
		/*********************************************************************
		Set A TCP IP RANGE Rule : PERMIT
		Default is DROP
		Expect  MATCH and PACKET Forward
		***********************************************************************/		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrLB_		= ntohl(inet_addr(HOST2_IP)) &0xFFFFFF00;
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;		
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);		
		/*********************************************************************
		 Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp= Layer 4 
		 Default is PERMIT
		 Expect :  Match TCP IP RANGE Rule AND DROP 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__,i );		
		/**********************************************************************		
		 Set A TCP IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 2 and Layer 3
		 Default is PERMIT
		 Expect :  No match and Forward 
 		 **********************************************************************/
 		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L2_AND_L3;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );	

		/**********************************************************************		
		 Set A TCP POLICY  ACL Rule  and pkOpApp=  Layer 3 and Layer 4
		 Default is PERMIT
		 Expect :  Redirect
		**********************************************************************/

 		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8651_ACL_TCP;
		aclt.actionType_		= RTL8651_ACL_POLICY;
		aclt.nhIndex		 = 30;
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_4, __FUNCTION__, __LINE__,i );	


 		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_REDIRECT;
		aclt.nextHop_			 = getL2TableRowFromMacStr(HOST1_MAC)<<2;			
		aclt.dvid_				= 0x6;
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_1, __FUNCTION__, __LINE__,i );	

		
		/**********************************************************************
		Set A TCP IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 3  and Layer 4
		Default is PERMIT
		Expect :  DROP 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,0, __FUNCTION__, __LINE__,i );		
		/**********************************************************************
		Set A TCP IP RANGE DROP  ACL Rule  and pkOpApp= Only Layer 2 
		Default is PERMIT
		Expect :  FORWARD 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i);		
		/**********************************************************************
		Set A TCP IP RANGE DROP  ACL Rule  and pkOpApp=  Layer 2  and Layer3
		Default is PERMIT
		Expect :  Forward 
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;		
		aclt.pktOpApp	 		= RTL8651_ACLTBL_L2_AND_L3;
		rtl8651_setAsicAclRule(0, &aclt);			
		retval = virtualMacSetHsb( &hsb);
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );		
		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp= Layer 3 
		Default is PERMIT
		Expect :  Foorward
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L3;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);				

		/**********************************************************************

								Fragment Packet


		
		*********************************************************************/		
		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp= Layer 3 
		Default is PERMIT
		Packet is not  First Fragment Packet
		Expect :  Foorward
		 **********************************************************************/

		
		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp= Layer3 and Layer4  
		Default is PERMIT
		Packet is the first Fragment Packet
		Expect : match and  Drop
		 **********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1,&aclt);
	 	WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_FRAG_TO_ACLPT);
		hsb.ipfo0_n=0;
		hsb.ipfg=MORE_FRAG;
		hsb.sip=ntohl(inet_addr(HOST1_IP));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp= Only Layer3
		Default is PERMIT
		Packet is the first fragment Packet
		Expect :  NOT MATCH and Forward
		 **********************************************************************/
 		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L3;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp= ALL Layer
		Default is PERMIT
		Packet is the first fragment packet
		Expect : match and  Drop
		 **********************************************************************/
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be Drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp= ALL Layer
		Default is PERMIT
		Packet is the not  first fragment packet
		Expect : not match and to cpu
		 **********************************************************************/
  		memset((void*)&aclt, 0, sizeof(aclt));				
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L2;		
		hsb.ipfo0_n=1;
		hsb.ipfg=MORE_FRAG;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be CPU, but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);		


		/**********************************************************************
		CHECK HSB ipfo0_n (fragment BIT)
		**********************************************************************/
  		memset((void*)&aclt, 0, sizeof(aclt));				
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xF;
		aclt.tcpDstPortUB_		= 0xF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
		hsb.ipfo0_n			= 0;
		hsb.ipfg				= MORE_FRAG;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);		
		IS_EQUAL_INT_DETAIL("Pakcet should be froward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);		
  		memset((void*)&aclt, 0, sizeof(aclt));				
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xF;
		aclt.tcpDstPortUB_		= 0xF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
		hsb.ipfo0_n			= 1;
		hsb.ipfg				= MORE_FRAG;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);		
		IS_EQUAL_INT_DETAIL("Pakcet should be CPU, but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************		
			Succesful ipfo0_n

		**********************************************************************/


		/**********************************************************************		
		When ACL doesn't contain L4 information, when PORT is 0 to 0xffff implicit imply 
		layer4 inforation is not needed.
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp= ALL Layer
		Default is PERMIT
		Packet is the not  first fragment packet
		Expect : match and drop
		**********************************************************************/
		memset((void*)&aclt, 0, sizeof(aclt));				
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
		hsb.ipfo0_n			= 1;
		hsb.ipfg				= MORE_FRAG;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);		
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__ ,i);		

		/**********************************************************************		
		When ACL  contain L4 information, the rule is  a TCP IPRANGE DROP  Rule  and pkOpApp= Layer3 and Layer4. 
		Default is PERMIT.
		Packet is the not  first fragment packet.
		Expect : NOT MATCH and TO CPU
		**********************************************************************/
  		memset((void*)&aclt, 0, sizeof(aclt));				
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xF;
		aclt.tcpDstPortUB_		= 0xF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
		hsb.ipfo0_n			= 1;
		hsb.ipfg				= MORE_FRAG;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);		
		IS_EQUAL_INT_DETAIL("Pakcet should be Froward, but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************		
		When ACL  contain L4 information, the rule is  a TCP IPRANGE DROP  Rule  and pkOpApp all Layer
		Default is PERMIT.
		Packet is the not  first fragment packet and is layer2 packet.
		Expect : NOT MATCH and TO CPU
		**********************************************************************/	
  		memset((void*)&aclt, 0, sizeof(aclt));				
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xF;
		aclt.tcpDstPortUB_		= 0xF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL865XC_ACLTBL_ALL_LAYER;
		hsb.ipfo0_n			= 1;
		hsb.ipfg				= MORE_FRAG;
		hsb.da[5]+=3;
		rtl8651_setAsicAclRule(0, &aclt);		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		IS_EQUAL_INT_DETAIL("Pakcet should be Drop, but .......",hsa.dp,PM_PORT_2|PM_PORT_1, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp= ALL Layer
		Default is PERMIT
		Packet is the not  first fragment packet
		Expect : match and DROP
		 **********************************************************************/
  		memset((void*)&aclt, 0, sizeof(aclt));				
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL865XC_ACLTBL_ALL_LAYER;
		hsb.ipfo0_n=1;
		hsb.ipfg=MORE_FRAG;
		rtl8651_setAsicAclRule(0, &aclt);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be Drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp= ONLY Layer 4
		Default is PERMIT
		Packet is the first fragment Packet and DA IS NOT GATEWAY
		Expect :  FORWARD
		 **********************************************************************/		 
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_DROP;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		hsb.ipfo0_n=0;
		hsb.ipfg=MORE_FRAG;		
		hsb.da[5]+=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_2|PM_PORT_1, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp=  Layer 3 and Layer 4
		Default is DROP
		Packet is the first fragment packet
		Expect :  Forward
		 **********************************************************************/		 
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp=  Layer 3 and Layer 4
		Default is DROP
		Packet is the First Fragment Packet
		Expect :  Forward
		 **********************************************************************/		 
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		hsb.ipfg=MORE_FRAG;
		hsb.ipfo0_n=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		Set A TCP IPRANGE PERMIT  ACL Rule  and pkOpApp=  Layer 3 and Layer 4
		Default is DROP
		Packet is not  the first fragment packet
		Expect :  TO CPU
		 **********************************************************************/		 
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		hsb.ipfg=MORE_FRAG;
		hsb.ipfo0_n=1;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);		
		
		/**********************************************************************
		Set A TCP IPRANGE PERMIT  ACL Rule AND TCPFLAG = 0xF  and pkOpApp=  Layer 3 and Layer 4	 
		Default is DROP
		Packet is not the first fragment packet
		Expect :  TO CPU
		 **********************************************************************/		 
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFF00;
		aclt.tcpDstPortUB_		= 0xFF00;
		aclt.tcpFlag_			= 0xF;
		aclt.tcpFlagMask_		= 0xF;		
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		hsb.ipfg=2;
		hsb.ipfo0_n=1;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be to CPU , but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		Set A TCP IPRANGE PERMIT  ACL Rule including layer4 information  and pkOpApp=  Layer 3 and Layer 4	 
		Default is DROP
		Packet is not the first fragment packet
		Expect : NOT MATCH and TO CPU
		 **********************************************************************/		 
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFF00;
		aclt.tcpDstPortUB_		= 0xFF00;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		hsb.ipfg=MORE_FRAG;
		hsb.ipfo0_n=1;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be to CPU , but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		Set A TCP IPRANGE PERMIT  ACL Rule including layer4 information  and pkOpApp=  Layer 3 and Layer 4	 
		Default is DROP
		Packet is  the first fragment packet and should  include Layer4 information.
		Expect :  DROP
		 **********************************************************************/		 
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFF;
		aclt.tcpDstPortUB_		= 0xFF;
		aclt.tcpFlag_			= 0xF;
		aclt.tcpFlagMask_		= 0xF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		hsb.ipfg=MORE_FRAG;
		hsb.ipfo0_n=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be to DROP, but .......",hsa.dp,0, __FUNCTION__, __LINE__ ,i);		

		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp=  Layer 2
		Packet is not  the first fragment packet and pure Layer 2
		Default is DROP
		Expect :  FORWARD
		 **********************************************************************/		 
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFF;
		aclt.tcpDstPortUB_		= 0xFF;
		aclt.tcpFlag_			= 0xF;
		aclt.tcpFlagMask_		= 0xF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.ipfg=2;
		hsb.ipfo0_n=1;
		hsb.da[5]+=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be FORWARD, but .......",hsa.dp,PM_PORT_1|PM_PORT_2, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp=  Layer 2
		Packet is not  the first fragment packet and pure Layer 2
		Default is DROP
		Expect :  FORWARD
		 **********************************************************************/		 
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFF;
		aclt.tcpDstPortUB_		= 0xFF;
		aclt.tcpFlag_			= 0xF;
		aclt.tcpFlagMask_		= 0xF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.ipfg=2;
		hsb.ipfo0_n=1;
		hsb.da[5]+=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be FORWARD, but .......",hsa.dp,PM_PORT_1|PM_PORT_2, __FUNCTION__, __LINE__ ,i);		

		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp=  Layer 2
		Packet is not  the first fragment packet and pure Layer 2
		Default is DROP
		Expect :  FORWARD
		 **********************************************************************/		 
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.tcpFlag_			= 0xF;
		aclt.tcpFlagMask_		= 0xF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.ipfg=2;
		hsb.ipfo0_n=1;
		hsb.da[5]+=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be FORWARD, but .......",hsa.dp,PM_PORT_1|PM_PORT_2, __FUNCTION__, __LINE__ ,i);		

		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp=  Layer 2
		Packet is not  the first fragment packet and pure Layer 2
		Expect :  TO PM_PORT_2|PM_PORT_1
		 **********************************************************************/		 
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 		= RTL8651_ACL_MAC;
		aclt.actionType_ 		= RTL8651_ACL_DROP;
		aclt.pktOpApp	 		= RTL865XC_ACLTBL_ALL_LAYER;
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.ipfg=2;
		hsb.ipfo0_n=1;
		hsb.da[5]+=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,PM_PORT_2|PM_PORT_1, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		Set A TCP IPRANGE DROP  ACL Rule  and pkOpApp=  Layer 3 and Layer 4
		Default is DROP
		Packet is not  the first fragment packet and pure Layer 2
		Expect :  TO Drop
		 **********************************************************************/		 
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcIpAddrUB_		= 0xFFFFFFFF;
		aclt.dstIpAddrUB_		= 0xFFFFFFFF;
		aclt.tcpSrcPortUB_		= 0xFFFF;
		aclt.tcpDstPortUB_		= 0xFFFF;
		aclt.ruleType_			= RTL8652_ACL_TCP_IPRANGE;
		aclt.actionType_		= RTL8651_ACL_PERMIT;
		aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.ipfg=2;
		hsb.ipfo0_n=1;
		hsb.da[5]+=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert(retval==SUCCESS);
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but .......",hsa.dp,0, __FUNCTION__, __LINE__ ,i);		

	}
	retval = SUCCESS;	
	return retval;
}
int32 testACLSrcRule(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb;
	rtl865x_tblAsicDrv_serverPortParam_t	 srvt;	
	hsa_param_t hsa;
	int32 i;
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	ether_addr_t mac;
	
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		layer4Config();			
		memcpy((void*)&hsb,(void*)&Ghsb,sizeof(hsb));
		/*********************************************************************
		override VLAN Setting
		*********************************************************************/
		/* wan 0 */
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
		vlant.memberPortMask = vlant.untagPortMask = (1<<hp[5]);
		rtl8651_setAsicVlan(intf.vid,&vlant);
		/* LAN */
		bzero((void*) &intf, sizeof(intf));
	 	strtomac(&intf.macAddr, GW_LAN0_MAC);
		intf.vid = LAN0_VID;
		intf.mtu = 1500;
		intf.inAclStart = 0;
		intf.inAclEnd = 2;
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
		vlant.memberPortMask = (1<<hp[3])|(1<<hp[2])|(1<<hp[1]);
		vlant.untagPortMask = 0; //tagged
		rtl8651_setAsicVlan(intf.vid,&vlant);
		bzero((void*) &intf, sizeof(intf));
	 	strtomac(&intf.macAddr, GW_LAN1_MAC);
		intf.vid = LAN1_VID;
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
		vlant.memberPortMask = vlant.untagPortMask = (1<<hp[4]);
		rtl8651_setAsicVlan(intf.vid,&vlant);
		/* config server port table */
		bzero((void*) &srvt, sizeof(srvt));
		/* server port 1020 translated to wan 0 isp port 20 */
		strtoip(&srvt.intIpAddr, HOST2_IP);
		strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		srvt.extPort = EXTERNAL_PORT;
		srvt.intPort = INTERNAL_PORT;
		srvt.valid = 1;
		rtl8651_setAsicServerPortTable(0, &srvt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 	= RTL8651_ACL_MAC;
		aclt.actionType_ 	= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 	= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(5, &aclt);
		rtl8651_setAsicAclRule(2, &aclt);
		/*********************************************************************
		Set A SRC Rule : PERMIT 
		Default : DROP
		Expect MATCH and Forwrd
		***********************************************************************/		
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[1]	= 0xFF;
		aclt.srcFilterMac_.octet[2]	= 0xFF;
		aclt.srcFilterIpAddr_		= 0xFFFFFFFF;
		aclt.ruleType_				= RTL8651_ACL_SRCFILTER;
		aclt.actionType_			= RTL8651_ACL_PERMIT;
		aclt.pktOpApp				= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 			= RTL8651_ACL_MAC;
		aclt.actionType_ 			= RTL8651_ACL_CPU;
		aclt.pktOpApp	 			= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
	
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[0]	= hsb.sa[0];
		aclt.srcFilterMac_.octet[1]	= hsb.sa[1];
		aclt.srcFilterMac_.octet[2]	= hsb.sa[2];
		aclt.srcFilterMac_.octet[3]	= hsb.sa[3];
		aclt.srcFilterMac_.octet[4]	= hsb.sa[4];
		aclt.srcFilterMac_.octet[5]	= hsb.sa[5];		
		aclt.srcFilterMacMask_.octet[0] =0xff;
		aclt.srcFilterMacMask_.octet[1] =0xff;
		aclt.srcFilterMacMask_.octet[2] =0xff;
		aclt.srcFilterMacMask_.octet[3] =0xff;
		aclt.srcFilterMacMask_.octet[4] =0xff;
		aclt.srcFilterMacMask_.octet[5] =0xff;
		aclt.srcFilterPort_ =0xf;		
		aclt.srcFilterPortUpperBound_ =0xffff;
		aclt.ruleType_					= RTL8651_ACL_SRCFILTER;
		aclt.actionType_			=	 RTL8651_ACL_PERMIT;
		aclt.pktOpApp				= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);



  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[0]	= hsb.sa[0];
		aclt.srcFilterMac_.octet[1]	= hsb.sa[1];
		aclt.srcFilterMac_.octet[2]	= hsb.sa[2];
		aclt.srcFilterMac_.octet[3]	= hsb.sa[3];
		aclt.srcFilterMac_.octet[4]	= hsb.sa[4];
		aclt.srcFilterMac_.octet[5]	= hsb.sa[5];		
		aclt.srcFilterMacMask_.octet[0] =0xff;
		aclt.srcFilterMacMask_.octet[1] =0xff;
		aclt.srcFilterMacMask_.octet[2] =0xff;
		aclt.srcFilterMacMask_.octet[3] =0xff;
		aclt.srcFilterMacMask_.octet[4] =0xff;
		aclt.srcFilterMacMask_.octet[5] =0xff;
		aclt.srcFilterVlanId_ =0x13;
		aclt.srcFilterPort_ =0xf;
		aclt.srcFilterVlanIdMask_=0xff;
		aclt.srcFilterPortUpperBound_ =0xffff;
		aclt.ruleType_					= RTL8651_ACL_SRCFILTER;
		aclt.actionType_			=	 RTL8651_ACL_PERMIT;
		aclt.pktOpApp				= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		PACKET TAG VID 10		
		Default is TOCPU
		Expect :  Forward
		 **********************************************************************/		   		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[0]		= hsb.sa[0];
		aclt.srcFilterMac_.octet[1]		= hsb.sa[1];
		aclt.srcFilterMac_.octet[2]		= hsb.sa[2];
		aclt.srcFilterMac_.octet[3]		= hsb.sa[3];
		aclt.srcFilterMac_.octet[4]		= hsb.sa[4];
		aclt.srcFilterMac_.octet[5]		= hsb.sa[5];		
		aclt.srcFilterMacMask_.octet[0] =0xff;
		aclt.srcFilterMacMask_.octet[1] =0xff;
		aclt.srcFilterMacMask_.octet[2] =0xff;
		aclt.srcFilterMacMask_.octet[3] =0xff;
		aclt.srcFilterMacMask_.octet[4] =0xff;
		aclt.srcFilterMacMask_.octet[5] =0xff;
		aclt.srcFilterPort_ 				= 0xf;		
		aclt.srcFilterVlanId_ 			= 0xA;		
		aclt.srcFilterVlanIdMask_ 		= 0x0;		
		aclt.srcFilterPortUpperBound_ =0xffff;
		aclt.ruleType_					= RTL8651_ACL_SRCFILTER;
		aclt.actionType_			=	 RTL8651_ACL_PERMIT;
		aclt.pktOpApp				= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.tagif=1;
		hsb.vid=LAN0_VID;
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		PACKET NO TAG
		aclt.srcFilterVlanId_ 			= 0x0;		
		aclt.srcFilterVlanIdMask_ 		= 0xFFF;		
		Default is TOCPU
		Expect :  CPU
		 **********************************************************************/		   		
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[0]		= hsb.sa[0];
		aclt.srcFilterMac_.octet[1]		= hsb.sa[1];
		aclt.srcFilterMac_.octet[2]		= hsb.sa[2];
		aclt.srcFilterMac_.octet[3]		= hsb.sa[3];
		aclt.srcFilterMac_.octet[4]		= hsb.sa[4];
		aclt.srcFilterMac_.octet[5]		= hsb.sa[5];		
		aclt.srcFilterMacMask_.octet[0] =0xff;
		aclt.srcFilterMacMask_.octet[1] =0xff;
		aclt.srcFilterMacMask_.octet[2] =0xff;
		aclt.srcFilterMacMask_.octet[3] =0xff;
		aclt.srcFilterMacMask_.octet[4] =0xff;
		aclt.srcFilterMacMask_.octet[5] =0xff;
		aclt.srcFilterPort_ 				= 0xF;		
		aclt.srcFilterVlanId_ 			= 0x0;		
		aclt.srcFilterVlanIdMask_ 		= 0xFFF;		
		aclt.srcFilterPortUpperBound_ 	= 0xFFFF;
		aclt.ruleType_					= RTL8651_ACL_SRCFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);

		/**********************************************************************
		PACKET NO TAG
		Default is TOCPU
		Expect :  Forward
		 **********************************************************************/		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[0]		= hsb.sa[0];
		aclt.srcFilterMac_.octet[1]		= hsb.sa[1];
		aclt.srcFilterMac_.octet[2]		= hsb.sa[2];
		aclt.srcFilterMac_.octet[3]		= hsb.sa[3];
		aclt.srcFilterMac_.octet[4]		= hsb.sa[4];
		aclt.srcFilterMac_.octet[5]		= hsb.sa[5];		
		aclt.srcFilterMacMask_.octet[0] =0xff;
		aclt.srcFilterMacMask_.octet[1] =0xff;
		aclt.srcFilterMacMask_.octet[2] =0xff;
		aclt.srcFilterMacMask_.octet[3] =0xff;
		aclt.srcFilterMacMask_.octet[4] =0xff;
		aclt.srcFilterMacMask_.octet[5] =0xff;
		aclt.srcFilterPort_ 				= 0xF;		
		aclt.srcFilterVlanId_ 			= LAN0_VID;		
		aclt.srcFilterVlanIdMask_ 		= 0xFFF;		
		aclt.srcFilterPortUpperBound_ 	= 0xFFFF;
		aclt.ruleType_					= RTL8651_ACL_SRCFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.tagif=0;
		hsb.vid=0xB;
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		PACKET NO TAG
		MAC  MASK 
		Default is TOCPU
		Expect :  CPU
		 **********************************************************************/		   		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[0]		= hsb.sa[0];
		aclt.srcFilterMac_.octet[1]		= hsb.sa[1];
		aclt.srcFilterMac_.octet[2]		= hsb.sa[2];
		aclt.srcFilterMac_.octet[3]		= hsb.sa[3];
		aclt.srcFilterMac_.octet[4]		= hsb.sa[4];
		aclt.srcFilterMac_.octet[5]		= 0xF0;
		aclt.srcFilterMacMask_.octet[0] =0xff;
		aclt.srcFilterMacMask_.octet[1] =0xff;
		aclt.srcFilterMacMask_.octet[2] =0xff;
		aclt.srcFilterMacMask_.octet[3] =0xff;
		aclt.srcFilterMacMask_.octet[4] =0xff;
		aclt.srcFilterMacMask_.octet[5] =0xfe;
		aclt.srcFilterPort_ 				= 0xF;		
		aclt.srcFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8651_ACL_SRCFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sa[5]=0xF3;		
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);



		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[0]		= hsb.sa[0];
		aclt.srcFilterMac_.octet[1]		= hsb.sa[1];
		aclt.srcFilterMac_.octet[2]		= hsb.sa[2];
		aclt.srcFilterMac_.octet[3]		= hsb.sa[3];
		aclt.srcFilterMac_.octet[4]		= hsb.sa[4];
		aclt.srcFilterMac_.octet[5]		= 0xF0;
		aclt.srcFilterMacMask_.octet[0] =0xff;
		aclt.srcFilterMacMask_.octet[1] =0xff;
		aclt.srcFilterMacMask_.octet[2] =0xff;
		aclt.srcFilterMacMask_.octet[3] =0xff;
		aclt.srcFilterMacMask_.octet[4] =0xff;
		aclt.srcFilterMacMask_.octet[5] =0xf8;
		aclt.srcFilterPort_ 				= 0xF;		
		aclt.srcFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8651_ACL_SRCFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sa[5]=0xF7;		
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);


		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[5]		= 0x3;
		aclt.srcFilterMacMask_.octet[0] =0xff;
		aclt.srcFilterMacMask_.octet[1] =0xff;
		aclt.srcFilterMacMask_.octet[2] =0xff;
		aclt.srcFilterMacMask_.octet[3] =0xff;
		aclt.srcFilterMacMask_.octet[4] =0xff;
		aclt.srcFilterMacMask_.octet[5] =0xf3;
		aclt.srcFilterPort_ 				= 0xF;		
		aclt.srcFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8651_ACL_SRCFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sa[5]=0xF7;		
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);



		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[0]		= hsb.sa[0];
		aclt.srcFilterMac_.octet[1]		= hsb.sa[1];
		aclt.srcFilterMac_.octet[2]		= hsb.sa[2];
		aclt.srcFilterMac_.octet[3]		= hsb.sa[3];
		aclt.srcFilterMac_.octet[4]		= hsb.sa[4];
		aclt.srcFilterMac_.octet[5]		= 0xE3;
		aclt.srcFilterMacMask_.octet[0] =0xff;
		aclt.srcFilterMacMask_.octet[1] =0xff;
		aclt.srcFilterMacMask_.octet[2] =0xff;
		aclt.srcFilterMacMask_.octet[3] =0xff;
		aclt.srcFilterMacMask_.octet[4] =0xff;
		aclt.srcFilterMacMask_.octet[5] =0xf3;
		aclt.srcFilterPort_ 				= 0xF;		
		aclt.srcFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8651_ACL_SRCFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sa[5]=0xF4;		
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);


		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[0]		= 0;
		aclt.srcFilterMac_.octet[1]		= 0;
		aclt.srcFilterMac_.octet[2]		= hsb.sa[2];
		aclt.srcFilterMac_.octet[3]		= hsb.sa[3];
		aclt.srcFilterMac_.octet[4]		= hsb.sa[4];
		aclt.srcFilterMac_.octet[5]		= 0xE7;
		aclt.srcFilterMacMask_.octet[0] =0xff;
		aclt.srcFilterMacMask_.octet[1] =0xff;
		aclt.srcFilterMacMask_.octet[2] =0xff;
		aclt.srcFilterMacMask_.octet[3] =0xff;
		aclt.srcFilterMacMask_.octet[4] =0xff;
		aclt.srcFilterMacMask_.octet[5] =0xf4;
		aclt.srcFilterPort_ 				= 0xF;		
		aclt.srcFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8651_ACL_SRCFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sa[5]=0xF4;		
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);


		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[0]		= hsb.sa[0];
		aclt.srcFilterMac_.octet[1]		= hsb.sa[1];
		aclt.srcFilterMac_.octet[2]		= hsb.sa[2];
		aclt.srcFilterMac_.octet[3]		= hsb.sa[3];
		aclt.srcFilterMac_.octet[4]		= hsb.sa[4];
		aclt.srcFilterMac_.octet[5]		= 0xE3;
		aclt.srcFilterMacMask_.octet[0] =0xff;
		aclt.srcFilterMacMask_.octet[1] =0xff;
		aclt.srcFilterMacMask_.octet[2] =0xff;
		aclt.srcFilterMacMask_.octet[3] =0xff;
		aclt.srcFilterMacMask_.octet[4] =0xff;
		aclt.srcFilterMacMask_.octet[5] =0xf3;
		aclt.srcFilterPort_ 				= 0xF;		
		aclt.srcFilterIpAddr_ 			= 0xFFFF;
		aclt.srcFilterPortUpperBound_ 	= 0xFFFF;		

		aclt.ruleType_					= RTL8651_ACL_SRCFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sa[5]=0xF4;		
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		PACKET NO TAG
		aclt.srcFilterIgnoreL3L4 = 1
		Default is TOCPU
		Expect :  FORWARD
		 **********************************************************************/		   		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[0]		= hsb.sa[0];
		aclt.srcFilterMac_.octet[1]		= hsb.sa[1];
		aclt.srcFilterMac_.octet[2]		= hsb.sa[2];
		aclt.srcFilterMac_.octet[3]		= hsb.sa[3];
		aclt.srcFilterMac_.octet[4]		= hsb.sa[4];
		aclt.srcFilterMac_.octet[5]		= hsb.sa[5];
		aclt.srcFilterMacMask_.octet[0] =0xff;
		aclt.srcFilterMacMask_.octet[1] =0xff;
		aclt.srcFilterMacMask_.octet[2] =0xff;
		aclt.srcFilterMacMask_.octet[3] =0xff;
		aclt.srcFilterMacMask_.octet[4] =0xff;
		aclt.srcFilterMacMask_.octet[5] =0xff;
		aclt.srcFilterPort_ 				= 0xF;		
		aclt.srcFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8651_ACL_SRCFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);

		/**********************************************************************
 	     	Set A SRC IP RANGE Filter
		Default is TOCPU
		Expect :  Not MATCH and to CPU
		 **********************************************************************/		   		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterPort_ 				= 0xF;		
		aclt.srcFilterIpAddrUB_			= ntohl(inet_addr(HOST2_IP));
		aclt.srcFilterIpAddrLB_			=  0xFFFFFF00;
		aclt.srcFilterIgnoreL4_			= 1;
		aclt.ruleType_					= RTL8652_ACL_SRCFILTER_IPRANGE;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
		
			
	}
	retval = SUCCESS;	
	return retval;		
}

int32 testACLDstRule(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb;
	rtl865x_tblAsicDrv_serverPortParam_t	 srvt;	
	hsa_param_t hsa;
	int32 i;
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	ether_addr_t mac;
	
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		layer4Config();			
		memcpy((void*)&hsb,(void*)&Ghsb,sizeof(hsb));
		/*********************************************************************
		override VLAN Setting
		*********************************************************************/
		/* wan 0 */
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
		/* LAN */
		bzero((void*) &intf, sizeof(intf));
	 	strtomac(&intf.macAddr, GW_LAN0_MAC);
		intf.vid = LAN0_VID;
		intf.mtu = 1500;
		intf.inAclStart = 0;
		intf.inAclEnd = 2;
		intf.outAclStart = intf.outAclEnd = 5;
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
		vlant.memberPortMask = (1<<hp[3])|(1<<hp[2])|(1<<hp[1]);
		vlant.untagPortMask = 0; //tagged
		rtl8651_setAsicVlan(intf.vid,&vlant);
		bzero((void*) &intf, sizeof(intf));
	 	strtomac(&intf.macAddr, GW_LAN1_MAC);
		intf.vid = LAN1_VID;
		intf.mtu = 1500;
		intf.inAclStart = 5;
		intf.inAclEnd = 5;
		intf.outAclStart = intf.outAclEnd = 5;
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
		vlant.memberPortMask = vlant.untagPortMask = (1<<hp[4]);
		rtl8651_setAsicVlan(intf.vid,&vlant);
		/* config server port table */
		bzero((void*) &srvt, sizeof(srvt));
		/* server port 1020 translated to wan 0 isp port 20 */
		strtoip(&srvt.intIpAddr, HOST2_IP);
		strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		srvt.extPort = EXTERNAL_PORT;
		srvt.intPort = INTERNAL_PORT;
		srvt.valid = 1;
		rtl8651_setAsicServerPortTable(0, &srvt);
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 	= RTL8651_ACL_MAC;
		aclt.actionType_ 	= RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 	= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(5, &aclt);
		rtl8651_setAsicAclRule(2, &aclt);
		/*********************************************************************
		Set A SRC Rule : PERMIT 
		Default : CPU
		Expect MATCH and Forwrd
		***********************************************************************/		
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterIpAddr_		= 0xFFFFFFFF;
		aclt.ruleType_				= RTL8651_ACL_DSTFILTER;
		aclt.actionType_			= RTL8651_ACL_PERMIT;
		aclt.pktOpApp				= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 			= RTL8651_ACL_MAC;
		aclt.actionType_ 			= RTL8651_ACL_CPU;
		aclt.pktOpApp	 			= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1, &aclt);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
  		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.dstFilterMac_.octet[0]		= hsb.da[0];
		aclt.dstFilterMac_.octet[1]		= hsb.da[1];
		aclt.dstFilterMac_.octet[2]		= hsb.da[2];
		aclt.dstFilterMac_.octet[3]		= hsb.da[3];
		aclt.dstFilterMac_.octet[4]		= hsb.da[4];
		aclt.dstFilterMac_.octet[5]		= hsb.da[5];		
		aclt.dstFilterMacMask_.octet[5] 	= 0xF;
		aclt.dstFilterPortUpperBound_ 	= 0xFFFF;
		aclt.ruleType_					= RTL8651_ACL_DSTFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			

		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);


		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
  		memset((void*)&aclt, 0, sizeof(aclt));
		memcpy((void*)&aclt.dstFilterMac_.octet[0],(void*)&hsb.da[0],sizeof(ether_addr_t));
		aclt.dstFilterMacMask_.octet[5] 	= 0xF;
		aclt.dstFilterVlanId_ 		  	= 0x13;
		aclt.dstFilterVlanIdMask_		= 0xFF;
		aclt.dstFilterPortUpperBound_ 	= 0xFFFF;
		aclt.ruleType_					= RTL8651_ACL_DSTFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			

		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		PACKET TAG VID 10		
		Default is TO CPU
		Expect :  Forward
		 **********************************************************************/		   				 
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		memset((void*)&aclt, 0, sizeof(aclt));
		memcpy((void*)&aclt.dstFilterMac_.octet[0],(void*)&hsb.da[0],sizeof(ether_addr_t));		
		aclt.dstFilterMacMask_.octet[5]     = 0xF;
		aclt.dstFilterVlanId_ 			= 0xF00;
		aclt.dstFilterVlanIdMask_ 		= 0x0;		
		aclt.dstFilterPortUpperBound_ 	= 0xFFFF;
		aclt.ruleType_					= RTL8651_ACL_DSTFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			

		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.tagif=1;
		hsb.vid=LAN0_VID;
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);

		/**********************************************************************
		PACKET NO TAG
		aclt.srcFilterVlanId_ 			= 0x0;		
		aclt.srcFilterVlanIdMask_ 		= 0xFFF;		
		Default is TOCPU
		Expect :  CPU
		 **********************************************************************/		   		
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.srcFilterMac_.octet[0]		= hsb.sa[0];
		aclt.srcFilterMac_.octet[1]		= hsb.sa[1];
		aclt.srcFilterMac_.octet[2]		= hsb.sa[2];
		aclt.srcFilterMac_.octet[3]		= hsb.sa[3];
		aclt.srcFilterMac_.octet[4]		= hsb.sa[4];
		aclt.srcFilterMac_.octet[5]		= hsb.sa[5];		
		aclt.srcFilterMacMask_.octet[5] 	= 0xF;
		aclt.srcFilterPort_ 				= 0xF;		
		aclt.srcFilterVlanId_ 			= 0x0;		
		aclt.srcFilterVlanIdMask_ 		= 0xFFF;		
		aclt.srcFilterPortUpperBound_ 	= 0xFFFF;
		aclt.ruleType_					= RTL8651_ACL_DSTFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);

		/**********************************************************************
		PACKET NO TAG
		Default is TOCPU
		Expect :  Forward
		 **********************************************************************/		
 		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		memset((void*)&aclt, 0, sizeof(aclt));
		memcpy((void*)&aclt.dstFilterMac_.octet[0],(void*)&hsb.da[0],sizeof(ether_addr_t));
		aclt.dstFilterMacMask_.octet[5] 	= 0xF;
		aclt.dstFilterVlanId_ 			= LAN0_VID;		
		aclt.dstFilterVlanIdMask_ 		= 0xFFF;		
		aclt.dstFilterPortUpperBound_ 	= 0xFFFF;
		aclt.ruleType_					= RTL8651_ACL_DSTFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			

		hsb.tagif=0;
		hsb.vid=0xB;
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		PACKET NO TAG
		MAC  MASK 
		Default is TOCPU
		Expect :  CPU
		 **********************************************************************/		   		
 		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		memset((void*)&aclt, 0, sizeof(aclt));
		memcpy((void*)&aclt.dstFilterMac_.octet[0],(void*)&hsb.da[0],sizeof(ether_addr_t));
		aclt.dstFilterMac_.octet[5]		= 0xF0;
		aclt.dstFilterMacMask_.octet[5] 	= 0xE;

		aclt.dstFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8651_ACL_DSTFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			


		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);


		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		memset((void*)&aclt, 0, sizeof(aclt));
		memcpy((void*)&aclt.dstFilterMac_.octet[0],(void*)&hsb.da[0],sizeof(ether_addr_t));
		aclt.dstFilterMac_.octet[5]		= 0xF0;
		aclt.dstFilterMacMask_.octet[5] 	= 0x08;
		aclt.dstFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8651_ACL_DSTFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);			

		hsb.da[5]=0xF7;		
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=PN_PORT3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_1|PM_PORT_2, __FUNCTION__, __LINE__ ,i);


		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.dstFilterMac_.octet[5]		= 0x3;
		aclt.dstFilterMacMask_.octet[5] 	= 0x3;
		aclt.dstFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8651_ACL_DSTFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.da[5]=0xF7;		
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_1|PM_PORT_2, __FUNCTION__, __LINE__ ,i);


		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.dstFilterMac_.octet[5]		= 0xE3;
		aclt.dstFilterMacMask_.octet[5] 	= 0x03;
		aclt.dstFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8651_ACL_DSTFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.da[5]=0xF4;		
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);


		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.dstFilterMac_.octet[0]		= 0;
		aclt.dstFilterMac_.octet[1]		= 0;
		aclt.dstFilterMac_.octet[2]		= hsb.da[2];
		aclt.dstFilterMac_.octet[3]		= hsb.da[3];
		aclt.dstFilterMac_.octet[4]		= hsb.da[4];
		aclt.dstFilterMac_.octet[5]		= 0xE7;
		aclt.dstFilterMacMask_.octet[5] 	= 0x04;
		aclt.dstFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8651_ACL_DSTFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_ONLY_L2;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.da[5]=0xF4;		
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_1|PM_PORT_2, __FUNCTION__, __LINE__ ,i);


		
		memset((void*)&aclt, 0, sizeof(aclt));
		memcpy((void*)&aclt.dstFilterMac_.octet[0],(void*)&hsb.da[0],sizeof(ether_addr_t));
		aclt.dstFilterMac_.octet[5]		= 0xE3;
		aclt.dstFilterMacMask_.octet[5] 	= 0x03;
		aclt.dstFilterIpAddr_ 			= 0xFFFF;
		aclt.dstFilterPortUpperBound_ 	= 0xFFFF;		

		aclt.ruleType_					= RTL8651_ACL_DSTFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sa[5]=0xF4;		
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		PACKET NO TAG
		aclt.dstFilterIgnoreL3L4 = 1
		Default is TOCPU
		Expect :  FORWARD
		 **********************************************************************/		   		
		memset((void*)&aclt, 0, sizeof(aclt));
		memcpy((void*)&aclt.dstFilterMac_.octet[0],(void*)&hsb.da[0],sizeof(ether_addr_t));
		aclt.dstFilterMacMask_.octet[5] 	= 0xF;
		aclt.dstFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8651_ACL_DSTFILTER;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);

		/**********************************************************************
 	     	Set A DST  IP RANGE Filter
		Default is TOCPU
		Expect :  Not MATCH and to CPU
		 **********************************************************************/		   		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.dstFilterIpAddrUB_			= 0xFFFFFFFF;
		aclt.dstFilterIpAddrLB_			= 0xFFFFFF00;
		aclt.dstFilterIgnoreL3L4_		= 1;
		aclt.ruleType_					= RTL8652_ACL_DSTFILTER_IPRANGE;
		aclt.actionType_				= RTL8651_ACL_PERMIT;
		aclt.pktOpApp					= RTL8651_ACLTBL_L3_AND_L4;
		rtl8651_setAsicAclRule(0, &aclt);			
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.sip= ntohl(inet_addr(HOST2_IP));
		hsb.dip= ntohl(inet_addr(HOST5_IP));
		hsb.sprt=INTERNAL_PORT;		
		hsb.spa=3;
		hsb.tagif=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be cpu , but ....",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		
			
	}
	retval = SUCCESS;	
	return retval;		
}


int32 testACL(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb_napt =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("192.168.1.3")),
		sprt: 20,			dip:ntohl(inet_addr("120.167.4.5")),
		dprt: 8800,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,
		patmatch:0,		ethtype: 0x0800,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x00,0x00,0x00,0x0,0x40},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 0x2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0, 		urlmch: 1,
		extspa: 0,		extl2: 0,
	};

	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr(WAN3_ISP_PUBLIC_IP)),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,
		patmatch:0 ,		ethtype: 0x0800,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 0x2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i;
	rtl865x_tblAsicDrv_extIntIpParam_t ipt;
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	ether_addr_t mac;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			layer4Config();					
			/* wan 0 */
			strtomac(&intf.macAddr, GW_WAN0_MAC);
			intf.macAddrNumber = 1;
			intf.vid = WAN0_VID;
			intf.mtu = 1500;
			intf.inAclStart = 1;
			intf.inAclEnd = 1;
			intf.outAclStart = intf.outAclEnd = 1;
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

			/* LAN */
			bzero((void*) &intf, sizeof(intf));
		 	strtomac(&intf.macAddr, GW_LAN0_MAC);
			intf.vid = LAN0_VID;
			intf.mtu = 1500;
			intf.inAclStart = 0;
			intf.inAclEnd = 1;
			intf.outAclStart = intf.outAclEnd = 1;
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
			vlant.memberPortMask = (1<<3);
			vlant.untagPortMask = 0; //tagged
			rtl8651_setAsicVlan(intf.vid,&vlant);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb_napt.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			bzero((void*)&ipt, sizeof(rtl865x_tblAsicDrv_extIntIpParam_t));
			strtoip(&ipt.extIpAddr, WAN3_ISP_PUBLIC_IP);
			strtoip(&ipt.intIpAddr,WAN3_ISP_PUBLIC_IP);
			ipt.localPublic	= TRUE;
			ipt.nat			= FALSE;
			ipt.nhIndex		= 0;
			//rtl8651_setAsicExtIntIpTable(2, &ipt);

			/* wan 0 */
			strtomac(&intf.macAddr, GW_WAN0_MAC);
			intf.macAddrNumber = 1;
			intf.vid = WAN0_VID;
			intf.mtu = 1500;
			intf.inAclStart = 1;
			intf.inAclEnd = 1;
			intf.outAclStart = intf.outAclEnd = 1;
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

			/* LAN */
			bzero((void*) &intf, sizeof(intf));
		 	strtomac(&intf.macAddr, GW_LAN0_MAC);
			intf.vid = LAN0_VID;
			intf.mtu = 1500;
			intf.inAclStart = 0;
			intf.inAclEnd = 1;
			intf.outAclStart = intf.outAclEnd = 1;
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
			vlant.memberPortMask = (1<<3);
			vlant.untagPortMask = 0; /*tagged*/
			rtl8651_setAsicVlan(intf.vid,&vlant);


			/* case 0*/
			/* Set A IP Rule */
			/* Expect MATCH and DROP*/
			 memset((void*)&aclt, 0, sizeof(aclt));
			aclt.tos_				= 0x00;
			aclt.tosMask_			= 0x0;
			aclt.srcIpAddr_		= 0xc0a80400;
			aclt.srcIpAddrMask_	= 0xffffff00;
			aclt.dstIpAddr_		= 0x00000000;
			aclt.dstIpAddrMask_	= 0x00000000;			 
			aclt.ruleType_ = RTL8651_ACL_IP;
			aclt.actionType_ = RTL8651_ACL_PERMIT;
			aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
			rtl8651_setAsicAclRule(0, &aclt);			
			
			 memset((void*)&aclt, 0, sizeof(aclt));
			aclt.ruleType_ = RTL8651_ACL_MAC;
			aclt.actionType_ = RTL8651_ACL_PERMIT;
			aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
			rtl8651_setAsicAclRule(1, &aclt);

				

			/* case 1*/
			/* Set A IP Rule */
			/* Expect MATCH and DROP*/
			 memset((void*)&aclt, 0, sizeof(aclt));
			aclt.tos_				= 0x00;
			aclt.tosMask_			= 0x0;
			aclt.srcIpAddr_		= 0xc0a80400;
			aclt.srcIpAddrMask_	= 0xffffff00;
			aclt.dstIpAddr_		= 0x00000000;
			aclt.dstIpAddrMask_	= 0x00000000;			 
			aclt.ruleType_ = RTL8651_ACL_IP;
			aclt.actionType_ = RTL8651_ACL_DROP;
			aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
			rtl8651_setAsicAclRule(0, &aclt);			
			
			 memset((void*)&aclt, 0, sizeof(aclt));
			aclt.ruleType_ = RTL8651_ACL_MAC;
			aclt.actionType_ = RTL8651_ACL_PERMIT;
			aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
			rtl8651_setAsicAclRule(1, &aclt);

				
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			IS_EQUAL_INT("Pakcet should be dropped, but forward....",hsa.dp,0, __FUNCTION__, __LINE__ );
			assert( retval == SUCCESS );


			/* case 2*/
			/* Set A IP RANGE Rule to distinguish  IP Rule */
			/* Expect NOT MATCH and PACKET FORWARD*/
			 memset((void*)&aclt, 0, sizeof(aclt));
			aclt.srcIpAddr_		= 0xc0a80400;
			aclt.srcIpAddrMask_	= 0xffffff00;
			aclt.dstIpAddr_		= 0x00000000;
			aclt.dstIpAddrMask_	= 0x00000000;				 
			aclt.ruleType_ = RTL8652_ACL_IP_RANGE;
			aclt.actionType_ = RTL8651_ACL_DROP;
			aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
			rtl8651_setAsicAclRule(0, &aclt);
			
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert(retval==SUCCESS);
			IS_NOT_EQUAL_INT("Pakcet should be forward, but .......",hsa.dp,0, __FUNCTION__, __LINE__ );		

			/*case 4*/	
			/* Set A TCP IPRANGE DROP  ACL Rule  */
			/* Expect : Don't match TCP IP RANGE Rule AND Forward */
			 memset((void*)&aclt, 0, sizeof(aclt));
			aclt.srcIpAddrUB_		= 0xffffff00;
			aclt.srcIpAddrLB_		= 0xc0a80400;
			aclt.dstIpAddrUB_		= 0xffffffff;
			aclt.tcpSrcPortUB_		=0xffff;
			aclt.tcpDstPortUB_		=0xffff;
			aclt.ruleType_			=RTL8652_ACL_TCP_IPRANGE;
			aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
			aclt.actionType_		= RTL8651_ACL_DROP;
			rtl8651_setAsicAclRule(0, &aclt);
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_NOT_EQUAL_INT("Pakcet should be forward, but .......",hsa.dp,0, __FUNCTION__, __LINE__ );		




			/*case 6*/	
			/* Set A TCP IP RANGE DROP  ACL Rule  */
			/* Expect :  Match TCP IP RANGE Rule AND DROP */
			hsb.type=HSB_TYPE_TCP;
			 memset((void*)&aclt, 0, sizeof(aclt));
			aclt.srcIpAddrUB_		= 0xffffff00;
			aclt.srcIpAddrLB_		= 0xc0a80400;
			aclt.dstIpAddrUB_		= 0xffffffff;
			aclt.tcpSrcPortUB_		=0xffff;
			aclt.tcpDstPortUB_		=0xffff;
			aclt.ruleType_			=RTL8652_ACL_TCP_IPRANGE;
			aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
			aclt.actionType_		= RTL8651_ACL_DROP;
			rtl8651_setAsicAclRule(0, &aclt);
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			IS_EQUAL_INT("Pakcet should be dropped, but forward....",hsa.dp,0, __FUNCTION__, __LINE__ );

	}
	retval = SUCCESS;	
	return retval;
}

int32 testUdpNAPTAging(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb_napt =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("192.168.1.3")),
		sprt: 20,			dip:ntohl(inet_addr("120.167.4.5")),
		dprt: 8800,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,
		patmatch:0,		ethtype: 0x0800,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x00,0x00,0x00,0x0,0x40},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 0x2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0, 		urlmch: 1,
		extspa: 0,		extl2: 0,
	};

	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr(WAN3_ISP_PUBLIC_IP)),
		sprt: 20,			dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,
		patmatch:0 ,		ethtype: 0x0800,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 0x2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i;
	rtl865x_tblAsicDrv_extIntIpParam_t ipt;
	ether_addr_t mac;
   	rtl865x_tblAsicDrv_naptTcpUdpParam_t napt;
	
	

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			layer4Config();				

			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb_napt.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			bzero((void*)&ipt, sizeof(rtl865x_tblAsicDrv_extIntIpParam_t));
			bzero((void*) &napt, sizeof(napt));	
			bzero(&napt,sizeof(napt));
			napt.insideLocalIpAddr=hsb_napt.sip;
			napt.insideLocalPort = hsb_napt.sprt;
			napt.isValid = 1;
	   	       napt.isDedicated = 0		;
		       napt.isTcp = 0;
		       napt.isStatic = 1;
		       napt.selExtIPIdx = 0;
  		 	napt.selEIdx=0;
	     	       napt.offset=0;
			hsb_napt.type=HSB_TYPE_UDP;
			rtl8651_setAsicNaptTcpUdpTable(1,rtl8651_naptTcpUdpTableIndex(0,hsb_napt.sip, hsb_napt.sprt,hsb_napt.dip,hsb_napt.dprt), &napt);
			WRITE_MEM32(TEATCR,5<<UDP_TH_OFFSET);			
			retval = virtualMacSetHsb( &hsb_napt );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			dump_tcpudp();
			hsb_napt.sip=ntohl(inet_addr("192.168.1.5")),
			hsb_napt.sprt=0xfe23;
			hsb_napt.sprt=0xfe23;
			hsb_napt.dip=0xca234398;
			hsb_napt.dprt=0x4398;
			napt.insideLocalIpAddr=hsb_napt.sip;
			napt.insideLocalPort = hsb_napt.sprt;
			napt.isValid = 1;
	   	       napt.isDedicated = 1;	
		       napt.isTcp = 0;
		       napt.isStatic = 1;
		       napt.selExtIPIdx = 0;
  		 	napt.selEIdx=0;
	     	       napt.offset=0;
			hsb_napt.type=HSB_TYPE_UDP;
			rtl8651_setAsicNaptTcpUdpTable(1,rtl8651_naptTcpUdpTableIndex(0,hsb_napt.sip, hsb_napt.sprt,0,0), &napt);
			WRITE_MEM32(TEATCR,7<<UDP_TH_OFFSET);			
			retval = virtualMacSetHsb( &hsb_napt );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );

		}
	return SUCCESS;

}

/*
 *  Test Multicast packet, the combination will include the following conditions:
 *
 *  + if exceed MTU (consider VLAN tagged/untagged)
 *  + if source VID matched
 *  + if source MAC matched
 *  + if source Port matched 
 *  + if destinated to Extension Port
 *  + if ACL opLayer matched
 *  + disable Multicast for pure L2 operation
 *  + disable netif l34 routing
 *  + Enable 1Q_UNWARE
 *  + WAN to LAN
 */
int32 testIPMulticast(uint32 caseNo)
{

	int i;
	hsa_param_t hsa;
	int32 retval = SUCCESS;
	rtl865x_tblAsicDrv_multiCastParam_t mcast; 
	hsb_param_t hsb ;
	uint32 multi_mtu;
	ether_addr_t mac;
	rtl865x_tblAsicDrv_l2Param_t l2t;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	rtl865x_tblAsicDrv_intfParam_t intf;
	_rtl8651_tblDrvAclRule_t aclt;

	memcpy((void*)&hsb,(void*)&Ghsb,sizeof(hsb));

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
			layer4Config();				
			/*strtomac(&intf.macAddr, GW_LAN0_MAC);*/
			vlant.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
			vlant.untagPortMask = 0; //tagged
			rtl8651_setAsicVlan(LAN0_VID,&vlant);

			/**********************************************************************
			0. Config: IP Multicast Table
			   SVID:10, sport=3, mbr=P0,P1,P2,P3
			**********************************************************************/
			multi_mtu=1500;
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
			1. IP Multicast Packet 224.0.0.3
			   expect hsa.trip WAN0_ISP_PUBLIC_IP
			**********************************************************************/
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=PN_PORT3;
			hsb.tagif=0;
			hsb.pppoeif=0;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("IP should be TRIP, but .......",hsa.trip,ntohl(inet_addr(WAN0_ISP_PUBLIC_IP)), __FUNCTION__, __LINE__,i) ;
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			2. IP Multicast Packet 224.0.0.3
			   Payload length(46B) is just match to MTU
			   Expect Forward
			**********************************************************************/
			multi_mtu=46;	
			rtl8651_setAsicMulticastMTU(multi_mtu);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=PN_PORT3;
			hsb.tagif=0;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Packet should be forward, but .......",hsa.dp,mcast.mbr, __FUNCTION__, __LINE__,i) ;
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			3. IP Multicast Packet 224.0.0.3
			   Payload length(46B) exceeds MTU(45B)
			   Expect to CPU
			**********************************************************************/
			multi_mtu=45;	
			rtl8651_setAsicMulticastMTU(multi_mtu);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=PN_PORT3;
			hsb.tagif=0;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("hsa.dpc should be 1", hsa.dpc, 1, __FUNCTION__, __LINE__,i) ;
			IS_EQUAL_INT_DETAIL("Packet should be forwarded to CPU", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i) ;
			IS_EQUAL_INT_DETAIL("Packet should be forwarded to CPU", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__,i) ;
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			IS_EQUAL_INT_DETAIL("Bit[4:1] of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1e, 0x4<<1/*ACL filtered, but before DMAC==GMAC*/, __FUNCTION__, __LINE__,i );
			
			/**********************************************************************
			4. IP Multicast Packet 224.0.0.3
			   Payload length(42B, -4[vlan]) under MTU(45B)
			   Expect to forward
			**********************************************************************/
			multi_mtu=45;	
			rtl8651_setAsicMulticastMTU(multi_mtu);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=PN_PORT3;
			hsb.type=HSB_TYPE_UDP;
			hsb.tagif=1;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			IS_EQUAL_INT_DETAIL("Packet should be forward, but .......",hsa.dp,mcast.mbr, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			5. IP Multicast Packet 224.0.0.3
			   expect hsa.trip WAN0_ISP_PUBLIC_IP
			   Payload length(42B) exceeds MTU (41B)
			   Expect to CPU
			**********************************************************************/
			multi_mtu=41;	
			rtl8651_setAsicMulticastMTU(multi_mtu);
			bzero((void*) &mcast,sizeof(mcast));
			mcast.dip= ntohl(inet_addr("224.0.0.3"));
			mcast.sip=ntohl(inet_addr("192.168.1.1"));
			mcast.mbr=PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
			mcast.svid=LAN0_VID;
			mcast.port=PN_PORT3;
 		 	rtl8651_setAsicIpMulticastTable(&mcast);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=PN_PORT3;
			hsb.type=HSB_TYPE_UDP;
			hsb.tagif=1;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Packet should be CPU, but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i) ;
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			6. IP Multicast Packet 224.0.0.3
			   expect hsa.trip WAN0_ISP_PUBLIC_IP
			   MTU exceed but disable multicast (pure layer2 operation)
			   Expect :forward
			**********************************************************************/
			rtl8651_setAsicMulticastEnable(FALSE);

			bzero((void*) &mcast,sizeof(mcast));
			mcast.dip= ntohl(inet_addr("224.0.0.3"));
			mcast.sip=ntohl(inet_addr("192.168.1.1"));
			mcast.mbr=PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
			mcast.svid=LAN0_VID;
			mcast.port=PN_PORT3;
 		 	rtl8651_setAsicIpMulticastTable(&mcast);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=PN_PORT3;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Packet should be CPU, but .......",hsa.dp,PM_PORT_0|PM_PORT_1|PM_PORT_2, __FUNCTION__, __LINE__,i) ;
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			7. IP Multicast Packet 224.0.0.4
			   DMAC:01-00-5e-00-00-02 is unknown MAC (Multicast is enabled, MTU=1500)
			   expect to CPU
			**********************************************************************/			
			multi_mtu=1500;	
			rtl8651_setAsicMulticastMTU(multi_mtu);
			rtl8651_setAsicMulticastEnable(TRUE);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-02"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.5"));
			hsb.spa=PN_PORT3;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i );
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			IS_EQUAL_INT_DETAIL("Bit[4:1] of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1e, 0x4<<1/*ACL filtered, but before DMAC==GMAC*/, __FUNCTION__, __LINE__,i );
			
			/**********************************************************************
			8. IP Multicast Packet 224.0.0.3 
			   TAG Packet (normal forward, just like 1 and 4)
			   expect hsa.trip WAN0_ISP_PUBLIC_IP
			**********************************************************************/
			rtl8651_setAsicMulticastEnable(TRUE);

			bzero((void*) &mcast,sizeof(mcast));
			mcast.dip= ntohl(inet_addr("224.0.0.3"));
			mcast.sip=ntohl(inet_addr("192.168.1.1"));
			mcast.mbr=PM_PORT_1|PM_PORT_2|PM_PORT_3;
			mcast.svid=LAN0_VID;
			mcast.port=PN_PORT3;
 		 	rtl8651_setAsicIpMulticastTable(&mcast);
 		 	
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=PN_PORT3;
			hsb.tagif=1;
			hsb.vid=LAN0_VID;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("IP should be translated, but .......",hsa.trip,ntohl(inet_addr(WAN0_ISP_PUBLIC_IP)), __FUNCTION__, __LINE__,i) ;
			IS_EQUAL_INT_DETAIL("Packet  should be forward , but .......",hsa.dp,mcast.mbr, __FUNCTION__, __LINE__,i) ;
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			9. IP Multicast Packet 224.0.0.3 
			   ACL[5]: ONLY_L4 --> trap to CPU
			   ACL[6]: ONLY_L3 --> PERMIT
			   expect forward and hsa.trip=WAN0_ISP_PUBLIC_IP
			**********************************************************************/
			strtomac(&intf.macAddr, GW_LAN0_MAC);
			intf.vid = LAN0_VID;
			intf.outAclStart = intf.outAclEnd = 0;
			intf.mtu = 1500;
			intf.inAclStart =5;
			intf.inAclEnd = 6;
			intf.outAclStart = intf.outAclEnd = 0;
			intf.enableRoute = 1;
			intf.macAddrNumber=1;
			intf.valid = 1;
			retval=rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
			
			memset((void*)&aclt, 0, sizeof(aclt));			
			aclt.ruleType_ 		= RTL8651_ACL_MAC;
			aclt.actionType_ 		= RTL8651_ACL_CPU;
			aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L4;
			rtl8651_setAsicAclRule(5, &aclt);	
			
			memset((void*)&aclt, 0, sizeof(aclt));
			aclt.ruleType_ 		= RTL8651_ACL_MAC;
			aclt.actionType_ 		= RTL8651_ACL_PERMIT;
			aclt.pktOpApp			= RTL8651_ACLTBL_ONLY_L3;
			rtl8651_setAsicAclRule(6, &aclt);			

			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=PN_PORT3;
			hsb.tagif=1;
			hsb.vid=LAN0_VID;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("IP should be translated, but .......",hsa.trip,ntohl(inet_addr(WAN0_ISP_PUBLIC_IP)), __FUNCTION__, __LINE__,i) ;
			IS_EQUAL_INT_DETAIL("Packet  should be forward , but .......",hsa.dp,mcast.mbr, __FUNCTION__, __LINE__,i) ;
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			10. IP Multicast Packet 224.0.0.3 
			    Disable netif l34 routing
			    expect hsa.trip WAN0_ISP_PUBLIC_IP
			    expect to layer2 operatoin
			**********************************************************************/			
			strtomac(&intf.macAddr, GW_LAN0_MAC);
			intf.vid = LAN0_VID;
			intf.enableRoute=0;
			intf.inAclStart = 0;
			intf.inAclEnd = 0;
			intf.outAclStart = intf.outAclEnd = 0;
			intf.mtu = 1500;
			intf.inAclStart = 0;
			intf.inAclEnd = 0;
			intf.outAclStart = intf.outAclEnd = 0;
			intf.enableRoute = 0;
			intf.macAddrNumber=1;
			intf.valid = 1;
			retval=rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
			
			ASSERT(retval==SUCCESS);
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );			
			IS_EQUAL_INT_DETAIL("Packet  should be forward , but .......",hsa.dp,PM_PORT_0|PM_PORT_1|PM_PORT_2, __FUNCTION__, __LINE__,i) ;
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			11. Expect layer2 operation due to l3/4 routing disable
			    Seems the same as 10.
			**********************************************************************/
			strtomac(&intf.macAddr, GW_LAN0_MAC);
			intf.vid = LAN0_VID;
			intf.enableRoute=0;
			intf.inAclStart = 0;
			intf.inAclEnd = 0;
			intf.outAclStart = intf.outAclEnd = 0;
			intf.mtu = 1500;
			intf.inAclStart =0;
			intf.inAclEnd = 0;
			intf.outAclStart = intf.outAclEnd = 0;
			intf.enableRoute = 0;
			intf.macAddrNumber=1;
			intf.valid = 1;
			retval=rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );

			ASSERT(retval==SUCCESS);
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );			
			IS_EQUAL_INT_DETAIL("Packet  should be forward , but .......",hsa.dp,PM_PORT_0|PM_PORT_1|PM_PORT_2, __FUNCTION__, __LINE__,i) ;
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;

			/**********************************************************************
			12. ACL[5] not include L3/L4 when ip mulicast is layer2 operation 
			    Expect: apply layer2   
			**********************************************************************/
			strtomac(&intf.macAddr, GW_LAN0_MAC);
			intf.vid = LAN0_VID;
			intf.enableRoute=0;
			intf.inAclStart = 0;
			intf.inAclEnd = 0;
			intf.outAclStart = intf.outAclEnd = 0;
			intf.mtu = 1500;
			intf.inAclStart =5;
			intf.inAclEnd = 6;
			intf.outAclStart = intf.outAclEnd = 0;
			intf.enableRoute = 0;
			intf.macAddrNumber=1;
			intf.valid = 1;
			retval=rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
			memset((void*)&aclt, 0, sizeof(aclt));			
			aclt.ruleType_ 		= RTL8651_ACL_MAC;
			aclt.actionType_ 		= RTL8651_ACL_CPU;
			aclt.pktOpApp			= RTL8651_ACLTBL_L3_AND_L4;
			rtl8651_setAsicAclRule(5, &aclt);	
			
			memset((void*)&aclt, 0, sizeof(aclt));
			aclt.ruleType_ 		= RTL8651_ACL_MAC;
			aclt.actionType_ 		= RTL8651_ACL_PERMIT;
			aclt.pktOpApp			= RTL865XC_ACLTBL_ALL_LAYER;
			rtl8651_setAsicAclRule(6, &aclt);			
			
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-06"),sizeof(ether_addr_t));
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );			
			IS_EQUAL_INT_DETAIL("Packet  should be forward , but .......",hsa.dp,PM_PORT_0|PM_PORT_1|PM_PORT_2, __FUNCTION__, __LINE__,i) ;
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;

			/**********************************************************************
			13. Egress ACL[20]: all ==> trap to CPU
			    Expect TO CPU
			**********************************************************************/
			intf.outAclStart=intf.outAclEnd = 20;
			retval=rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
			ASSERT(retval==SUCCESS);
	 		memset((void*)&aclt, 0, sizeof(aclt));
			aclt.ruleType_ = RTL8651_ACL_MAC;
			aclt.actionType_ = RTL8651_ACL_CPU;
			aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
			retval=rtl8651_setAsicAclRule(20, &aclt);
			ASSERT( retval == SUCCESS );
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );			
			IS_EQUAL_INT_DETAIL("Packet  should be CPU , but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i) ;
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			 * restore netif setting (netif.enableRoute=FALSE)
			 **********************************************************************/
			bzero( &vlant, sizeof(vlant) );
			vlant.memberPortMask = (1<<3);
			vlant.untagPortMask = 0; //tagged	
			/* ingress filter gateway mac */
			strtomac(&intf.macAddr, GW_LAN0_MAC);
			intf.vid = LAN0_VID;
			intf.enableRoute=0;
			intf.inAclStart = 0;
			intf.inAclEnd = 0;
			intf.outAclStart = intf.outAclEnd = 0;
			intf.mtu = 1500;
			intf.inAclStart = 0;
			intf.inAclEnd = 0;
			intf.outAclStart = intf.outAclEnd = 0;
			intf.enableRoute = 1;
			intf.macAddrNumber=1;
			intf.valid = 1;
			retval=rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
			ASSERT(retval==SUCCESS);

			/**********************************************************************
			14. Source Port Mismatch
			    expect DROP
			**********************************************************************/
 		 	rtl8651_setAsicIpMulticastTable(&mcast);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=2;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Packet  should be drop",hsa.dp,0, __FUNCTION__, __LINE__,i) ;
			IS_EQUAL_INT_DETAIL("hsa.dpc should be equal to 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;

			/**********************************************************************
			15. SVID Mismatch
			    expect : drop
			**********************************************************************/
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=3;
			hsb.vid=LAN1_VID;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Packet  should be drop , but .......",hsa.dp,0, __FUNCTION__, __LINE__,i) ;
			IS_EQUAL_INT_DETAIL("hsa.dpc should be equal to 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			16. Enable 1Q_UNWARE. So even SVID is dismatch, ASIC can forward this packet based on port-based VID.
			    expect hsa.trip WAN0_ISP_PUBLIC_IP
			**********************************************************************/
			WRITE_MEM32( VCR0,READ_MEM32(VCR0)| EN_1QTAGVIDIGNORE);			
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("IP should be translated",hsa.trip,ntohl(inet_addr(WAN0_ISP_PUBLIC_IP)), __FUNCTION__, __LINE__,i) ;
			IS_EQUAL_INT_DETAIL("Packet should be forward",hsa.dp,mcast.mbr, __FUNCTION__, __LINE__,i) ;
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			17. Wan2Lan
			    PPPoE unicast Packet 
			    Downstream (Only )
			    IP Multicast Packet 224.0.0.3 
			    TAG Packet 
			**********************************************************************/
			bzero((void*) &mcast,sizeof(mcast));
			mcast.dip= ntohl(inet_addr("224.0.0.3"));
			mcast.sip=ntohl(inet_addr("142.168.1.1"));
			mcast.mbr=PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
			mcast.svid=LAN0_VID;
			mcast.port=3;
 		 	rtl8651_setAsicIpMulticastTable(&mcast);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("142.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=3;
			hsb.tagif=1;
			hsb.vid=LAN0_VID;
			hsb.type=HSB_TYPE_IGMP;
			hsb.pppoeif=1;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			strtomac(&mac,"01-00-5e-00-00-03");
			IS_EQUAL_MAC_DETAIL(" MAC should be translated ....",hsa.nhmac,mac, __FUNCTION__,__LINE__,i);
			IS_EQUAL_INT_DETAIL("Pakcet should be drop , but .......",hsa.dp,PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__ ,i);
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			18. LAN to WAN
			    DMAC:01-00-5e-00-00-03
			    MAC and IP Dismatch
			    expect to CPU
			**********************************************************************/
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.4"));
			hsb.spa=3;
			hsb.tagif=0;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Pakcet should be drop , but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			19. IP Multicast Packet IP  224.0.0.3
			    DMAC:01-00-5e-00-00-03
			    Multicast mask = P0,P1,P2,P3
			    L2 mask = P0
			    L2 table entry can't override IP Multicast Table
			    Expect Forward
			**********************************************************************/
			bzero((void*) &mcast,sizeof(mcast));
			mcast.dip= ntohl(inet_addr("224.0.0.3"));
			mcast.sip=ntohl(inet_addr("192.168.1.1"));
			mcast.mbr=PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
			mcast.svid=LAN0_VID;
			mcast.port=3;
 		 	retval=rtl8651_setAsicIpMulticastTable(&mcast);
			ASSERT(retval==SUCCESS);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			bzero(&l2t,sizeof(l2t));
			strtomac(&l2t.macAddr, " 01-00-5e-00-00-03");
			l2t.memberPortMask = PM_PORT_0;
			l2t.isStatic=1;			
			rtl8651_setAsicL2Table(getL2TableRowFromMacStr("01-00-5e-00-00-03"), 0, &l2t);
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_MAC(" MAC should be translated ....",hsa.nhmac,mac, __FUNCTION__,__LINE__);
			IS_EQUAL_INT_DETAIL("Pakcet should be forward , but .......",hsa.dp,PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__ ,i);
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;

			/**********************************************************************
			20. Wan2Lan
			    Member Two Extension Ports
			    PPPoE unicast Packet 
			    Expect to Ext0 and Ext2
			**********************************************************************/
			bzero((void*) &mcast,sizeof(mcast));
			mcast.dip= ntohl(inet_addr("224.0.0.3"));
			mcast.sip=ntohl(inet_addr("142.168.1.1"));
			mcast.mbr=PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|(PM_PORT_EXT0<<RTL8651_PORT_NUMBER)|(PM_PORT_EXT2<<RTL8651_PORT_NUMBER);
			mcast.svid=LAN0_VID;
			mcast.port=3;
 		 	rtl8651_setAsicIpMulticastTable(&mcast);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("142.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=3;
			hsb.tagif=1;
			hsb.vid=LAN0_VID;
			hsb.type=HSB_TYPE_IGMP;
			hsb.pppoeif=1;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			strtomac(&mac,"01-00-5e-00-00-03");
			IS_EQUAL_MAC(" MAC should be translated ....",hsa.nhmac,mac, __FUNCTION__,__LINE__);
			IS_EQUAL_INT_DETAIL("hsa.dp",hsa.dp,PM_PORT_NOTPHY|PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("hsa.dpext",hsa.dpext,PM_PORT_EXT0|PM_PORT_EXT2, __FUNCTION__, __LINE__,i );
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;

			/**********************************************************************
			21. Wan2Lan
			    FFCR : IP Multicast To CPU
			    Expect to CPU
			**********************************************************************/
			WRITE_MEM32(FFCR,(READ_MEM32(FFCR) & ~IPMltCstCtrl_MASK)|IPMltCstCtrl_TrapToCpu);
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			strtomac(&mac,"01-00-5e-00-00-03");
			IS_EQUAL_INT_DETAIL("hsa.dp",hsa.dp,PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i );
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			    Change extension port member port mask
			**********************************************************************/
			WRITE_MEM32(FFCR,(READ_MEM32(FFCR) & ~IPMltCstCtrl_MASK)|IPMltCstCtrl_Enable);
			bzero((void*) &mcast,sizeof(mcast));
			mcast.dip= ntohl(inet_addr("224.0.0.3"));
			mcast.sip=ntohl(inet_addr("142.168.1.1"));
			mcast.mbr=PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|(PM_PORT_EXT1<<RTL8651_PORT_NUMBER)|(PM_PORT_EXT2<<RTL8651_PORT_NUMBER);
			mcast.svid=LAN0_VID;
			mcast.port=3;
 		 	rtl8651_setAsicIpMulticastTable(&mcast);
 		 	
			/**********************************************************************
			22. Wan2Lan
			    Payload length(38B, minused PPPoE header) just makes MTU(38B)
			    Expect forward
			**********************************************************************/
			multi_mtu=38;	
			rtl8651_setAsicMulticastMTU(multi_mtu);
			hsb.pppoeif = 1;
			hsb.tagif = 0;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			strtomac(&mac,"01-00-5e-00-00-03");
			IS_EQUAL_INT_DETAIL("hsa.dp",hsa.dp, PM_PORT_NOTPHY|PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dpext,PM_PORT_EXT1|PM_PORT_EXT2, __FUNCTION__, __LINE__,i );
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			23. Wan2Lan
			    Payload length(38B) exceeds MTU(37B)
			    Expect to CPU
			**********************************************************************/
			multi_mtu=37;	
			rtl8651_setAsicMulticastMTU(multi_mtu);
			hsb.tagif = 0;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			strtomac(&mac,"01-00-5e-00-00-03");
			IS_EQUAL_INT_DETAIL("hsa.dp",hsa.dp,PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i );
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			24. Wan2Lan
			    Payload length(34B, minused VLAN header) just make MTU(34B)
			    Expect forward
			**********************************************************************/
			multi_mtu=34;	
			rtl8651_setAsicMulticastMTU(multi_mtu);
			hsb.tagif = 1;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			strtomac(&mac,"01-00-5e-00-00-03");
			IS_EQUAL_INT_DETAIL("hsa.dp",hsa.dp, PM_PORT_NOTPHY|PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dpext,PM_PORT_EXT1|PM_PORT_EXT2, __FUNCTION__, __LINE__,i );
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			
			/**********************************************************************
			25. Wan2Lan
			    Payload length(34B) exceeds MTU(33B)
			    Expect to CPU
			**********************************************************************/
			multi_mtu=33;	
			rtl8651_setAsicMulticastMTU(multi_mtu);
			hsb.tagif = 1;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			strtomac(&mac,"01-00-5e-00-00-03");
			IS_EQUAL_INT_DETAIL("hsa.dp",hsa.dp,PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i );
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;

			/**********************************************************************
			26. Wan2Lan (trap to CPU)
			    Payload length(34B, minused VLAN header) just make MTU(34B)
			    Expect forward to physical ports and to CPU
			**********************************************************************/
 			{ /* set cpu bit */
				rtl865xc_tblAsic_ipMulticastTable_t entry;
				uint32 index;

				index = rtl8651_ipMulticastTableIndex( mcast.dip, mcast.sip );
				retval = _rtl8651_readAsicEntry( TYPE_MULTICAST_TABLE, index, &entry );
				ASSERT( retval==SUCCESS );
				entry.toCPU = TRUE;
				retval = _rtl8651_forceAddAsicEntry( TYPE_MULTICAST_TABLE, index, &entry );
				ASSERT( retval==SUCCESS );
			}
			multi_mtu=34;	
			rtl8651_setAsicMulticastMTU(multi_mtu);
			hsb.tagif = 1;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			strtomac(&mac,"01-00-5e-00-00-03");
			IS_EQUAL_INT_DETAIL("hsa.dp",hsa.dp, PM_PORT_NOTPHY|PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dpext,PM_PORT_CPU|PM_PORT_EXT1|PM_PORT_EXT2, __FUNCTION__, __LINE__,i );
			IS_NOT_EQUAL_INT_DETAIL("hsa.dpc should be more than 0", hsa.dpc, 0, __FUNCTION__, __LINE__,i) ;
			IS_EQUAL_INT_DETAIL("Bit[4:1] of CPU Reason is wrong (real, expect)", hsa.why2cpu&0x1e, 0x4<<1/*ACL filtered, but before DMAC==GMAC*/, __FUNCTION__, __LINE__,i );
 			{ /* recover */
				rtl865xc_tblAsic_ipMulticastTable_t entry;
				uint32 index;

				index = rtl8651_ipMulticastTableIndex( mcast.dip, mcast.sip );
				retval = _rtl8651_readAsicEntry( TYPE_MULTICAST_TABLE, index, &entry );
				ASSERT( retval==SUCCESS );
				entry.toCPU = FALSE;
				retval = _rtl8651_forceAddAsicEntry( TYPE_MULTICAST_TABLE, index, &entry );
				ASSERT( retval==SUCCESS );
			}

			
	}
	retval = SUCCESS;	

	return retval;		
}


int32 testLayer3GuestVLAN(uint32 caseNo)
{
	int i;
	hsa_param_t hsa;
	uint16 guestVid;
	ether_addr_t mac;
	int32 retval = SUCCESS;
	rtl865x_tblAsicDrv_multiCastParam_t mcast; 
	rtl865x_tblAsicDrv_l2Param_t l2t;
	hsb_param_t hsb ;
	uint32 multi_mtu;
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			layer3Config(0);			
			rtl8651_setAsicOperationLayer(3);
			guestVid=13;
			memcpy((void*)&hsb,(void*)&Ghsb,sizeof(hsb));
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			/**********************************************************************
			IP Multicast Packet 224.0.0.3 
			TAG Packet 
			expect : Drop
			**********************************************************************/
			WRITE_MEM32(DOT1XMACCR, EN_8021X_PORT3_MAC_CONTROL);
			multi_mtu=1500;	
			rtl8651_setAsicMulticastMTU(multi_mtu);
			rtl8651_setAsicMulticastEnable(TRUE);
			bzero((void*) &mcast,sizeof(mcast));
			mcast.dip= ntohl(inet_addr("224.0.0.3"));
			mcast.sip=ntohl(inet_addr("192.168.1.1"));
			mcast.mbr=PM_PORT_1|PM_PORT_2|PM_PORT_3;
			mcast.svid=LAN0_VID;
			mcast.port=3;
 		 	rtl8651_setAsicIpMulticastTable(&mcast);
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=3;
			hsb.tagif=1;
			hsb.vid=LAN0_VID;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Packet  should be DROP , but .......",hsa.dp,0, __FUNCTION__, __LINE__,i) ;
			/**********************************************************************
			IP Multicast Packet 224.0.0.3 
			TAG Packet 
			expect : forward lookup ip multicast table
			**********************************************************************/
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-5e-00-00-03"),sizeof(ether_addr_t));
			bzero((void*) &l2t,sizeof(l2t));
			strtomac(&l2t.macAddr,"00-00-5e-00-00-03");
			l2t.memberPortMask = (1<<3);
			l2t.auth=1;
			retval=rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-00-5e-00-00-03"), 0, &l2t);
			ASSERT( retval == SUCCESS );
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Packet  should be to forward , but .......",hsa.dp,mcast.mbr, __FUNCTION__, __LINE__,i) ;									
			/**********************************************************************
			IP Multicast Packet 224.0.0.3 
			TAG Packet 
			expect : forward , because ... disable multicast table
			**********************************************************************/
			rtl8651_setAsicMulticastEnable(FALSE);

			WRITE_MEM32(DOT1XMACCR, EN_8021X_PORT3_MAC_CONTROL|EN_8021X_PORT2_MAC_CONTROL);
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Packet  should be to forward , but .......",hsa.dp,PM_PORT_0|PM_PORT_1|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__,i) ;									
			/**********************************************************************
			IP Multicast Packet 224.0.0.3 
			TAG Packet 
			enable port authentication			
			expect : forward

			**********************************************************************/
			rtl8651_setAsicMulticastEnable(TRUE);

			WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0|EN_DOT1XPB_P1);							
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Packet  should be to forward , but .......",hsa.dp,mcast.mbr, __FUNCTION__, __LINE__,i) ;												
			/**********************************************************************
			IP Multicast Packet 224.0.0.3 
			TAG Packet 
			expect : CPU
			**********************************************************************/
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			bzero((void*) &l2t,sizeof(l2t));
			strtomac(&l2t.macAddr,"01-00-5e-00-00-03");
			l2t.memberPortMask = (1<<3);
			l2t.isStatic =1;
			l2t.auth=1;
			retval=rtl8651_setAsicL2Table(getL2TableRowFromMacStr("01-00-5e-00-00-03"), 0, &l2t);				
			ASSERT( retval == SUCCESS );
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Packet  should be to forward , but .......",hsa.dp,mcast.mbr, __FUNCTION__, __LINE__,i) ;						

			/*********************************************************************
			authenticated node --> unauthenticated node 
			DA is Gateway MAC
			Expect  :Forward
			**********************************************************************/
			WRITE_MEM32(DOT1XMACCR, EN_8021X_PORT3_MAC_CONTROL);			
			bzero((void*) &l2t,sizeof(l2t));
			strtomac(&l2t.macAddr, HOST3_MAC);
			l2t.memberPortMask = (1<<3);
			l2t.isStatic =0;
			l2t.auth=1;
			rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST3_MAC), 0, &l2t);				
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,HOST3_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			hsb.dip= ntohl(inet_addr("124.0.0.3"));
			hsb.spa=3;
			hsb.type=HSB_TYPE_TCP;
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );
			/*********************************************************************
			authenticated node --> unauthenticated node 
			enable port authenicatioin
			DA is Gateway MAC
			Expect  :drop
			**********************************************************************/
			WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0|EN_DOT1XPB_P5);				
			bzero((void*) &l2t,sizeof(l2t));
			strtomac(&l2t.macAddr, HOST3_MAC);
			l2t.memberPortMask = (1<<3);
			l2t.isStatic =0;
			l2t.auth=1;
			rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST3_MAC), 0, &l2t);				
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,HOST3_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			hsb.dip= ntohl(inet_addr("124.0.0.3"));
			hsb.spa=3;
			hsb.type=HSB_TYPE_TCP;
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,0, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Finish  Lookup Working .......",hsa.hwfwrd,1, __FUNCTION__, __LINE__,i );
			/*********************************************************************
			authenticated node --> unauthenticated node 
			enable port authenicatioin
			DA is Gateway MAC
			Expect  :forward
			**********************************************************************/
			WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0|EN_DOT1XPB_P5|EN_DOT1XPB_P5_OPINDIR);				
			bzero((void*) &l2t,sizeof(l2t));
			strtomac(&l2t.macAddr, HOST3_MAC);
			l2t.memberPortMask = (1<<3);
			l2t.isStatic =0;
			l2t.auth=1;
			rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST3_MAC), 0, &l2t);				
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,HOST3_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			hsb.dip= ntohl(inet_addr("124.0.0.3"));
			hsb.spa=3;
			hsb.type=HSB_TYPE_TCP;
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Finish  Lookup Working .......",hsa.hwfwrd,1, __FUNCTION__, __LINE__,i );
			/*********************************************************************
			authenticated node --> unauthenticated node 
			enable port authenicatioin and MAC 1X
			DA is Gateway MAC
			Expect  :forward
			**********************************************************************/
			WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0|EN_DOT1XPB_P5|EN_DOT1XPB_P5_OPINDIR);				
			WRITE_MEM32(DOT1XMACCR, EN_8021X_PORT3_MAC_CONTROL|EN_8021X_PORT5_MAC_CONTROL);			
			bzero((void*) &l2t,sizeof(l2t));
			strtomac(&l2t.macAddr, HOST3_MAC);
			l2t.memberPortMask = (1<<3);
			l2t.isStatic =0;
			l2t.auth=1;
			rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST3_MAC), 0, &l2t);				
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,HOST3_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			hsb.dip= ntohl(inet_addr("124.0.0.3"));
			hsb.spa=3;
			hsb.type=HSB_TYPE_TCP;
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Pakcet should be forward, but .......",hsa.dp,0, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("Finish  Lookup Working .......",hsa.hwfwrd,1, __FUNCTION__, __LINE__,i );			
			/*********************************************************************
			unauthenticated node --> unauthenticated node 
			DA is Gateway MAC
			Entry is dynamic
			Expect  :Drop
			**********************************************************************/
			bzero((void*) &l2t,sizeof(l2t));
			strtomac(&l2t.macAddr, HOST3_MAC);
			l2t.memberPortMask = (1<<3);
			l2t.isStatic =0;
			l2t.auth=0;
			rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST3_MAC), 0, &l2t);				
			memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,HOST3_MAC),sizeof(ether_addr_t));
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
			hsb.spa=3;
			hsb.type=HSB_TYPE_TCP;
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("Pakcet should be Drop, but .......",hsa.dp,0x0, __FUNCTION__, __LINE__,i );	
			/*********************************************************************
			Enable Guest VLAN  
			unauthenticated node --> unauthenticated node 
 			DA is Gateway MAC		
 			Entry is static
			Expect Drop
			*********************************************************************/			 
			WRITE_MEM32(GVGCR,guestVid|EN_8021X_TOGVLAN);
			strtomac(&l2t.macAddr, HOST3_MAC);
			l2t.memberPortMask = (1<<3);
			l2t.isStatic = 1;
			l2t.auth=0;
			rtl8651_setAsicL2Table(getL2TableRowFromMacStr(HOST3_MAC), 0, &l2t);							
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL(" Pakcet should be Drop, but .......",hsa.dp,0x0, __FUNCTION__, __LINE__,i );
			/*********************************************************************
			Enable Guest VLAN  layer3 ToCPU 
			unauthenticated node --> unauthenticated node 
 			DA is Gateway MAC			 
			Expect to CPU
			*********************************************************************/		
			WRITE_MEM32(GVGCR,guestVid|EN_8021X_TOGVLAN|EN_DOT1X_GVR2CPU);
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL(" Pakcet should be CPU, but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i );
			/*********************************************************************
			Enable Guest VLAN  layer3 ToCPU 
			unauthenticated node --> unauthenticated node 
 			DA is Gateway MAC			 
			Expect to CPU
			*********************************************************************/		
			strtomac(&l2t.macAddr, GW_LAN0_MAC);
			l2t.memberPortMask = (1<<3);
			l2t.isStatic = 0;
			l2t.auth=1;			
			rtl8651_setAsicL2Table(getL2TableRowFromMacStr(GW_LAN0_MAC), 0, &l2t);
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL(" Pakcet should be CPU, but .......",hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i );

			
	}
	
	retval=SUCCESS;
	return retval;
}
int32 testArpAging(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 4,		len: 64,
		vid: 0x0,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("192.168.1.2")),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,	
		patmatch:0,		ethtype: 0x0800,
					ipfo0_n:0,
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
		
	};
	hsa_param_t hsa;
	int i,j;
	uint32 ip32;
	ether_addr_t mac;	
	rtl865x_tblAsicDrv_arpParam_t arpt,arptNow;
	uint32 time,timeout;
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
	  	if (i!=IC_TYPE_REAL) continue;
		retval = model_setTestTarget( i );		
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		layer3Config(0);			
		/*********************************************************************		
		Expect : Forward
		*********************************************************************/
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN1_MAC),sizeof(ether_addr_t));
		hsb.dip=ntohl(inet_addr(HOST4_IP));
		hsb.sip=ntohl(inet_addr("192.168.1.2")),
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_4, __FUNCTION__, __LINE__,i );
		strtoip(&ip32, HOST4_IP);
		bzero((void*) &arpt, sizeof(arpt));
		rtl8651_getAsicArp(ip32 & 0x7, &arpt);		
		IS_NOT_EQUAL_INT_DETAIL("ARP age  should not be zero , ....", arpt.aging,0, __FUNCTION__, __LINE__,i );
		rtl8651_getAsicArp(ip32 & 0x7, &arptNow);		
		rtlglue_getmstime(&time);
		while ( rtlglue_getmstime(&timeout)==time);/*sync Time*/
		rtlglue_printf("Wait ARP change \n");
		do
		{
			rtlglue_getmstime(&timeout);
			rtl8651_getAsicArp(ip32 & 0x7, &arptNow);		
			if ((timeout-time) >(1600*1000))
			{
				rtlglue_printf("\nWait Arp Entry Aging Change Too Long. (%d second Wait) \n",(timeout-time)/1000);			
					return FAILED;
			}else if ((timeout-time)%1000==0)
			{
				rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);
			}
			
		}while((arptNow.aging-arpt.aging)==0);
		rtl8651_getAsicArp(ip32 & 0x7, &arpt);		
		rtlglue_printf("\nARP Entry Age is %d(Time Difference(31  downto 30 sec):  %d (ms) \n",arpt.aging,timeout-time);			


		for (j=30;j>0;j--)
		{
			rtlglue_getmstime(&time);
			while ( rtlglue_getmstime(&timeout)==time);/*sync Time*/
			rtlglue_printf("Wait ARP change \n");
			do
			{
				rtlglue_getmstime(&timeout);
				rtl8651_getAsicArp(ip32 & 0x7, &arptNow);		
				if ((timeout-time) >(1600*1000))
				{
					rtlglue_printf("\nWait Arp Entry Aging Change Too Long. (%d second Wait) \n",(timeout-time)/1000);			
						return FAILED;
				}else if ((timeout-time)%1000==0)
				{
					rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);
				}
				
			}while((arptNow.aging-arpt.aging)==0);
			rtl8651_getAsicArp(ip32 & 0x7, &arpt);				
			rtlglue_printf("\nARP Entry Age is %d(Time Difference(%d  downto %d sec):  %d (ms) \n",arpt.aging,j,j-1,timeout-time);			
		}
		
	}
	retval = SUCCESS;	
	return retval;
}


int32 testLayer4Priority(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip:0,
		sprt: 1020,		dip:0,
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,
		patmatch:0 ,		ethtype: 0x0800,	
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst:2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0, 		urlmch: 1,
		extspa: 0,		extl2: 0,
		pppoeid:0,
	};
	hsa_param_t hsa;
	int i;
	rtl865x_tblAsicDrv_serverPortParam_t	 srvt;
	ether_addr_t mac;	
	int pbp, svrp; /* Port-Based Priority and NAPT priority (Server Port) */
	uint8 szBuf[128];

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		layer4Config();				
		WRITE_MEM32(QIDDPCR,READ_MEM32(QIDDPCR)|(1<<NAPT_PRI_OFFSET)|(1<<PBP_PRI_OFFSET));
		WRITE_MEM32(QIDDPCR,(1<<NAPT_PRI_OFFSET)|(1<<PBP_PRI_OFFSET));
		WRITE_MEM32(PBPCR, 0x00000000 ); /* default value */

		for( pbp = 0; pbp<=7; pbp++ )
		{
			for( svrp = 0; svrp<=7; svrp++ )
			{
				WRITE_MEM32( PBPCR, pbp<<PBPRI_P3_OFFSET );
				
				memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
				memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,HOST2_MAC),sizeof(ether_addr_t));
				retval = virtualMacSetHsb( &hsb );
				assert( retval == SUCCESS );
				retval = virtualMacGetHsa( &hsa );		
				
				/* config server port table */
				bzero((void*) &srvt, sizeof(srvt));
				/* server port 1020 translated to wan 0 isp port 20 */
				strtoip(&srvt.intIpAddr, HOST2_IP);
				strtoip(&srvt.extIpAddr, WAN0_ISP_PUBLIC_IP);
				srvt.extPort = EXTERNAL_PORT;
				srvt.intPort = INTERNAL_PORT;
				srvt.valid = 1;
				srvt.pvaild=1;
				srvt.pid=svrp;
				rtl8651_setAsicServerPortTable(0, &srvt);

				strtoip(&srvt.intIpAddr, HOST2_IP);
				strtoip(&srvt.extIpAddr, WAN1_ISP_PUBLIC_IP);
				srvt.extPort = UPPER_PORT;
				srvt.intPort = LOWER_PORT;
				srvt.portRange=1;
				srvt.valid = 1;
				srvt.pid=svrp;
				rtl8651_setAsicServerPortTable(15, &srvt);

	 			hsb.sprt=INTERNAL_PORT;
				hsb.dprt=EXTERNAL_PORT;
				hsb.sip= ntohl(inet_addr("192.168.1.3")),
				hsb.dip=ntohl(inet_addr("122.168.4.1"));
				hsb.spa=3;
				hsb.type=HSB_TYPE_UDP;
				retval = virtualMacSetHsb( &hsb );
				assert( retval == SUCCESS );
				retval = virtualMacGetHsa( &hsa );
				assert( retval == SUCCESS );	
				IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );
				IS_EQUAL_INT_DETAIL("PORT should be to ....", hsa.port,EXTERNAL_PORT, __FUNCTION__, __LINE__,i );
				if ( svrp > pbp ) /* ALE will choose the bigger one. */
				{
					snprintf( szBuf, sizeof(szBuf), "hsa.priority(%d) is not matched with svrp(%d). pbp=(%d)", hsa.priority, svrp, pbp );
					IS_EQUAL_INT_DETAIL( szBuf, hsa.priority, svrp , __FUNCTION__, __LINE__,i);
				}
				else
				{
					snprintf( szBuf, sizeof(szBuf), "hsa.priority(%d) is not matched with pbp(%d). svrp=(%d)", hsa.priority, pbp, svrp );
					IS_EQUAL_INT_DETAIL( szBuf, hsa.priority, pbp , __FUNCTION__, __LINE__,i);
				}

				/*********************************************************************
				From  LAN  to WAN
				Port Bouncing
				Expect to CPU
	 			*********************************************************************/
				hsb.dip=ntohl(inet_addr("192.168.4.1"));
				retval = virtualMacSetHsb( &hsb );
				assert( retval == SUCCESS );	
				retval = virtualMacGetHsa( &hsa );
				IS_EQUAL_INT_DETAIL("Pakcet should be to cpu ,but ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
				{ /* Since NAPT-based is not matched, ALE will always choose Port-Based. */
					snprintf( szBuf, sizeof(szBuf), "hsa.priority(%d) is not matched with pbp(%d). naptp=(%d)", hsa.priority, pbp, svrp );
					IS_EQUAL_INT_DETAIL( szBuf, hsa.priority, pbp , __FUNCTION__, __LINE__,i);
				}
			}
		}
	}
	retval = SUCCESS;	
	return retval;
}

/******************************************************************************
If a packet is not in TCP/UDP/ICMP format and is routed to an interface 
that requires NAPT operations (internal to external packets):
0: Drop the packet1: Trap the packet to the CPU
*****************************************************************************/

int32 testNaptF2CPU(uint32 caseNo)
{
	return SUCCESS;
}

int32 testSPI(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 4,		len: 64,
		vid: 0x0,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("192.168.1.2")),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,	
		patmatch:0,		ethtype: 0x0800,
					ipfo0_n:0,
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
		
	};

	 rtl865x_tblAsicDrv_naptTcpUdpParam_t napt;
	hsa_param_t hsa;
	int i;
	ether_addr_t mac;	

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		/*if (i!=IC_TYPE_REAL) continue;*/
		retval = model_setTestTarget( i );		
		WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_RTL8650B);
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		SPIConfig();	
		/***********************************************************************
		WAN to WAN
		***********************************************************************/		
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_WAN0_MAC),sizeof(ether_addr_t));
		hsb.dip=ntohl(inet_addr("132.168.2.2"));
		hsb.sip=ntohl(inet_addr("140.8.2.2")),
		hsb.sprt=100;
		hsb.dprt=200;
		hsb.spa=PN_PORT5;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );		
		/***********************************************************************
		WAN to DMZ
		***********************************************************************/		
		hsb.dip=ntohl(inet_addr("192.168.2.2"));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_3, __FUNCTION__, __LINE__,i );


		hsb.type=HSB_TYPE_UDP;
		WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_SPI4_WAN_DMZ);		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i ); 
		memset(&napt,0,sizeof(napt));
		napt.insideLocalPort = hsb.dprt;
		napt.insideLocalIpAddr=hsb.dip;
		napt.isValid = 0;
		napt.isDedicated = 1;
		napt.isTcp = 0;
		napt.selExtIPIdx = 0;
		rtl8651_setAsicNaptTcpUdpTable(napt.isTcp,rtl8651_naptTcpUdpTableIndex(napt.isTcp, hsb.dip, hsb.dprt, hsb.sip, hsb.sprt),&napt);				
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_3, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("l34trip should be 0 ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,PM_PORT_3, __FUNCTION__, __LINE__,i );

		
		hsb.type=HSB_TYPE_TCP;		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );		
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i ); 


		napt.isTcp=1;
		rtl8651_setAsicNaptTcpUdpTable(napt.isTcp,rtl8651_naptTcpUdpTableIndex(napt.isTcp, hsb.dip, hsb.dprt, hsb.sip, hsb.sprt),&napt);				
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );		
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_3, __FUNCTION__, __LINE__,i );		
			IS_EQUAL_INT_DETAIL("l34trip should be 0 ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,PM_PORT_3, __FUNCTION__, __LINE__,i );

		/***********************************************************************
		WAN to LAN
		***********************************************************************/		
		hsb.dip=ntohl(inet_addr("192.168.1.2"));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be drop ,but ....", hsa.dp,0, __FUNCTION__, __LINE__,i );		
		/***********************************************************************
		WAN to RLAN
		***********************************************************************/		
		hsb.dip=ntohl(inet_addr("192.168.3.2"));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_4, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("l34trip should be 0 ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,PM_PORT_4, __FUNCTION__, __LINE__,i );
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		WRITE_MEM32(SWTCR1,EN_51B_CPU_REASON);
		/***********************************************************************
		DMZ to WAN
		***********************************************************************/
		hsb.spa=PN_PORT3;
		hsb.type=HSB_TYPE_UDP;		
		hsb.sip=ntohl(inet_addr("192.168.2.2"));
		hsb.dip=ntohl(inet_addr("140.8.2.2"));
		hsb.sprt=300;
		hsb.dprt=400;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("l34trip should be 0 ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,PM_PORT_5, __FUNCTION__, __LINE__,i );
		
		/***********************************************************************
		DMZ to DMZ
		***********************************************************************/		
		hsb.sip=ntohl(inet_addr("192.168.2.1")),
		hsb.dip=ntohl(inet_addr("192.168.2.2")),
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_3 ,__FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("l34trip should be 0 ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,PM_PORT_3, __FUNCTION__, __LINE__,i );
		
		/***********************************************************************
		DMZ to LAN
		***********************************************************************/		
		hsb.dip=ntohl(inet_addr("192.168.1.2")),
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("l34trip should be forward ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_1, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,PM_PORT_1, __FUNCTION__, __LINE__,i );

		/***********************************************************************
		DMZ to  RLAN
		***********************************************************************/
		hsb.dip=ntohl(inet_addr("192.168.3.2"));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_4, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("l34trip should be 0 ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,PM_PORT_4, __FUNCTION__, __LINE__,i );




		WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_SPI3_DMZ_RLAN);		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i );
			IS_EQUAL_INT_DETAIL("l34trip should be 0 ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,0, __FUNCTION__, __LINE__,i );





		napt.insideLocalPort = hsb.dprt;
		napt.insideLocalIpAddr=hsb.dip;
		napt.isValid = 0;
		napt.isDedicated = 1;
		napt.isTcp = 0;
		napt.selExtIPIdx = 0;
		rtl8651_setAsicNaptTcpUdpTable(napt.isTcp,rtl8651_naptTcpUdpTableIndex(napt.isTcp, hsb.dip, hsb.dprt, hsb.sip, hsb.sprt),&napt);				
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_4, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("l34trip should be 0 ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,PM_PORT_4, __FUNCTION__, __LINE__,i );



		WRITE_MEM32(SWTCR1,EN_51B_CPU_REASON);
		/***********************************************************************
		LAN to WAN
		***********************************************************************/		
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.spa=PN_PORT3;
		hsb.sip=ntohl(inet_addr("192.168.1.1"));
		hsb.dip=ntohl(inet_addr("140.168.2.2")),
		hsb.sprt=500;
		hsb.dprt=600;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i );		
		/***********************************************************************
		LAN to DMZ
		***********************************************************************/		
		hsb.dip=ntohl(inet_addr("192.168.2.2")),
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_3, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("l34trip should be 0 ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,PM_PORT_3, __FUNCTION__, __LINE__,i );

		/***********************************************************************
		LAN to LAN
		***********************************************************************/		
		hsb.dip=ntohl(inet_addr("192.168.1.2")),
 		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_1, __FUNCTION__, __LINE__,i );
		/***********************************************************************
		LAN to RLAN
		***********************************************************************/		
		hsb.dip=ntohl(inet_addr("192.168.3.2")),
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_4, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("l34trip should be 0 ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,PM_PORT_4, __FUNCTION__, __LINE__,i );



		WRITE_MEM32(SWTCR1,EN_51B_CPU_REASON);
		/***********************************************************************
		RLAN to  WAN
		***********************************************************************/
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.spa=PN_PORT3;
		hsb.sip=ntohl(inet_addr("192.168.3.1"));
		hsb.dip=ntohl(inet_addr("140.140.140.140"));
		hsb.sprt=700;
		hsb.dprt=800;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("l34trip should be 0 ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,PM_PORT_5, __FUNCTION__, __LINE__,i );


/*		WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_SPI3_DMZ_RLAN);		
		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_4, __FUNCTION__, __LINE__,i );
*/
		/***********************************************************************
		RLAN to DMZ
		***********************************************************************/		
		hsb.dip=ntohl(inet_addr("192.168.2.2"));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_3, __FUNCTION__, __LINE__,i );
		/***********************************************************************
		RLAN to LAN
		***********************************************************************/		
		hsb.dip=ntohl(inet_addr("192.168.1.2"));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_1, __FUNCTION__, __LINE__,i );
		/***********************************************************************
		RLAN to RLAN
		***********************************************************************/		
		hsb.dip=ntohl(inet_addr("192.168.3.2"));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_4, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("l34trip should be 0 ,but ....", hsa.l34tr,0, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("ttl  should be 0 ,but ....", hsa.ttl_1if,PM_PORT_4, __FUNCTION__, __LINE__,i );


	}
	retval = SUCCESS;	
	return retval;
}



int32 testPPTP(uint32 caseNo)
{
	rtl865x_tblAsicDrv_naptIcmpParam_t pptpAsicEntry;
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 124,
		vid: 0x00,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0xa0,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_PPTP,
		patmatch:0,		ethtype: 0,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x21,0x11,0x00,0x00},
		hiprior:3,			snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch:0,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i;
	ether_addr_t  mac;
	uint16 outboundGlobalId;
	uint32 tblIdx;
		
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		layer4Config();
		outboundGlobalId=0x1143;

		memset(&pptpAsicEntry, 0, sizeof(rtl865x_tblAsicDrv_naptIcmpParam_t));
		pptpAsicEntry.insideLocalId = 0xa0;
		pptpAsicEntry.insideLocalIpAddr =ntohl(inet_addr("192.168.1.1"));
		pptpAsicEntry.offset = 0x1123;
		pptpAsicEntry.isCollision = 1;
		pptpAsicEntry.isStatic = 1;
		pptpAsicEntry.isValid = 1;
		pptpAsicEntry.isPptp = 1;
		pptpAsicEntry.isSpi = 0;
		rtl8651_naptIcmpTableIndex(pptpAsicEntry.insideLocalIpAddr,pptpAsicEntry.insideLocalId,ntohl(inet_addr("100.168.4.1")),&tblIdx); 
		rtl8651_setAsicNaptIcmpTable(TRUE, tblIdx, &pptpAsicEntry);
		hsb.spa=3;
		hsb.sprt=pptpAsicEntry.insideLocalId ;
		hsb.sip=pptpAsicEntry.insideLocalIpAddr;
		hsb.dip=ntohl(inet_addr("100.168.4.1"));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );



		hsb.sip=ntohl(inet_addr("100.168.4.1"));
		hsb.dip=ntohl(inet_addr("192.168.4.1"));
		hsb.sprt=pptpAsicEntry.offset;
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i );


	}
	retval = SUCCESS;	
	return retval;

}
#define GRE_TIMEOUT (10*8000)

int32 testGREAging(uint32 caseNo)
{
	rtl865x_tblAsicDrv_naptIcmpParam_t pptpAsicEntry;
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 124,
		vid: 0x00,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0xa0,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_PPTP,
		patmatch:0,		ethtype: 0,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x21,0x11,0x00,0x00},
		hiprior:3,			snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch:0,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i;
	ether_addr_t  mac;
	uint32 tblIdx;
	rtl865x_tblAsicDrv_naptIcmpParam_t naptIcmp,naptIcmpNow;
	uint32 time,timeout;
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		if (i!=IC_TYPE_REAL) continue;
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		layer4Config();

		WRITE_MEM32(TEATCR,5<<ICMP_TH_OFFSET);
		memset(&pptpAsicEntry, 0, sizeof(rtl865x_tblAsicDrv_naptIcmpParam_t));
		pptpAsicEntry.insideLocalId = 0xa0;
		pptpAsicEntry.insideLocalIpAddr =ntohl(inet_addr("192.168.1.1"));
		pptpAsicEntry.offset = 0x1123;
		pptpAsicEntry.isCollision = 1;
		pptpAsicEntry.isStatic = 1;
		pptpAsicEntry.isValid = 1;
		pptpAsicEntry.isPptp = 1;
		pptpAsicEntry.isSpi = 0;
		rtl8651_naptIcmpTableIndex(pptpAsicEntry.insideLocalIpAddr,pptpAsicEntry.insideLocalId,ntohl(inet_addr("100.168.4.1")),&tblIdx); 
		rtl8651_setAsicNaptIcmpTable(TRUE, tblIdx, &pptpAsicEntry);
		hsb.spa=3;
		hsb.sprt=pptpAsicEntry.insideLocalId ;
		hsb.sip=pptpAsicEntry.insideLocalIpAddr;
		hsb.dip=ntohl(inet_addr("100.168.4.1"));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );

		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &naptIcmp);
		ASSERT( retval == SUCCESS );
		rtlglue_getmstime(&time);
		while ( rtlglue_getmstime(&timeout)==time);/*sync Time*/
		rtlglue_printf("Wait Age change \n");
		do
		{
			rtlglue_getmstime(&timeout);
			retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &naptIcmpNow);
			ASSERT( retval == SUCCESS );
			if ((timeout-time) >(GRE_TIMEOUT))
			{
				rtlglue_printf("\nWait GRE Entry Aging Change Too Long. (%d second Wait) \n",(timeout-time)/1000);			
					return FAILED;
			}else if ((timeout-time)%1000==0)
			{
				rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);
			}
			
		}while(naptIcmpNow.ageSec!=0);
		rtlglue_printf("\nGRE Entry Age is %d(Time Difference  %d (ms) \n",naptIcmpNow.ageSec,timeout-time);			

		WRITE_MEM32(TEATCR,16<<ICMP_TH_OFFSET);
		memset(&pptpAsicEntry, 0, sizeof(rtl865x_tblAsicDrv_naptIcmpParam_t));
		pptpAsicEntry.insideLocalId = 0xa0;
		pptpAsicEntry.insideLocalIpAddr =ntohl(inet_addr("192.168.1.1"));
		pptpAsicEntry.offset = 0x1123;
		pptpAsicEntry.isCollision = 1;
		pptpAsicEntry.isStatic = 1;
		pptpAsicEntry.isValid = 1;
		pptpAsicEntry.isPptp = 1;
		pptpAsicEntry.isSpi = 0;
		rtl8651_naptIcmpTableIndex(pptpAsicEntry.insideLocalIpAddr,pptpAsicEntry.insideLocalId,ntohl(inet_addr("100.168.4.1")),&tblIdx); 
		rtl8651_setAsicNaptIcmpTable(TRUE, tblIdx, &pptpAsicEntry);
		hsb.spa=3;
		hsb.sprt=pptpAsicEntry.insideLocalId ;
		hsb.sip=pptpAsicEntry.insideLocalIpAddr;
		hsb.dip=ntohl(inet_addr("100.168.4.1"));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );

		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &naptIcmp);
		ASSERT( retval == SUCCESS );
		rtlglue_getmstime(&time);
		while ( rtlglue_getmstime(&timeout)==time);/*sync Time*/
		rtlglue_printf("Wait Age change \n");
		do
		{
			rtlglue_getmstime(&timeout);
			retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &naptIcmpNow);
			ASSERT( retval == SUCCESS );
			if ((timeout-time) >(80*GRE_TIMEOUT))
			{
				rtlglue_printf("\nWait Arp Entry Aging Change Too Long. (%d second Wait) \n",(timeout-time)/1000);			
					return FAILED;
			}else if ((timeout-time)%1000==0)
			{
				rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);
			}
			
		}while(naptIcmpNow.ageSec!=0);
		rtlglue_printf("\nGRE Entry Age is %d(Time Difference  %d (ms) \n",naptIcmpNow.ageSec,timeout-time);			

		

	}
	retval = SUCCESS;	
	return retval;

}
int32 testPPPoEAging(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,		len: 64,
		vid: 0x0,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("192.168.1.2")),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,	
		patmatch:0,		ethtype: 0x0800,
		ipfg:0,			ipfo0_n:0,
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
		
	};
	
	hsa_param_t hsa;
	int i,j;
	rtl865x_tblAsicDrv_pppoeParam_t pppoet,pppoetNow;
	ether_addr_t mac;
	uint32 time,timeout;

	

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		if (i!=IC_TYPE_REAL) continue;
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		layer3Config(1);			
		/*********************************************************************		
		Expect : Forward
		*********************************************************************/
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.dip=ntohl(inet_addr(HOST5_IP));
		hsb.sip=ntohl(inet_addr("192.168.1.2")),
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );	
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__,i );
		
		rtl8651_getAsicPppoe(0, &pppoet);		
		rtlglue_getmstime(&time);
		while ( rtlglue_getmstime(&timeout)==time);/*sync Time*/
		rtlglue_printf("Wait PPPoE change \n");
		do
		{
			rtlglue_getmstime(&timeout);
			rtl8651_getAsicPppoe(0, &pppoetNow);		
			if ((timeout-time) >(1600*1000))
			{
				rtlglue_printf("\nWait Arp Entry Aging Change Too Long. (%d second Wait) \n",(timeout-time)/1000);			
					return FAILED;
			}else if ((timeout-time)%1000==0)
			{
				rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);
			}
			
		}while((pppoetNow.age-pppoet.age)==0);
		rtl8651_getAsicPppoe(0, &pppoet);		
		rtlglue_printf("\nARP Entry Age is %d(Time Difference(31  downto 30 sec):  %d (ms) \n",pppoet.age,timeout-time);			


		for (j=5;j>0;j--)
		{
			rtlglue_getmstime(&time);
			while ( rtlglue_getmstime(&timeout)==time);/*sync Time*/
			rtlglue_printf("Wait PPPOE Age change \n");
			do
			{
				rtlglue_getmstime(&timeout);
				rtl8651_getAsicPppoe(0, &pppoetNow);		
				if ((timeout-time) >(1600*1000))
				{
					rtlglue_printf("\nWait Arp Entry Aging Change Too Long. (%d second Wait) \n",(timeout-time)/1000);			
						return FAILED;
				}else if ((timeout-time)%1000==0)
				{
					rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);
				}
				
			}while((pppoetNow.age-pppoet.age)==0);
			rtl8651_getAsicPppoe(0, &pppoet);		
			rtlglue_printf("\nPPPoE Entry Age is %d(Time Difference(%d  downto %d sec):  %d (ms) \n",pppoet.age,j,j-1,timeout-time);			
		}
		
	}
	retval = SUCCESS;	
	return retval;
}
#define MCAST_TIMEOUT  (5*8000)
int32 testIPMulticastAging(uint32 caseNo)
{
	int i,idx,j;
	hsa_param_t hsa;
	int32 retval = SUCCESS;
	rtl865x_tblAsicDrv_multiCastParam_t mcast,mcastNow; 
	hsb_param_t hsb ;
	uint32 multi_mtu;
	ether_addr_t mac;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	uint32 time,timeout;
	

	memcpy((void*)&hsb,(void*)&Ghsb,sizeof(hsb));

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		if (i!=IC_TYPE_REAL) continue;
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		layer4Config();				
		/*strtomac(&intf.macAddr, GW_LAN0_MAC);*/
		vlant.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
		vlant.untagPortMask = 0; //tagged
		rtl8651_setAsicVlan(LAN0_VID,&vlant);

		multi_mtu=1500;
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
		idx=rtl8651_ipMulticastTableIndex(mcast.sip, mcast.dip);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
		hsb.sip=ntohl(inet_addr("192.168.1.1"));
		hsb.dip=ntohl(inet_addr("224.0.0.3"));
		hsb.spa=3;
		hsb.type=HSB_TYPE_UDP;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		IS_EQUAL_INT_DETAIL("IP should be TRIP, but .......",hsa.trip,ntohl(inet_addr(WAN0_ISP_PUBLIC_IP)), __FUNCTION__, __LINE__,i) ;
		rtl8651_getAsicIpMulticastTable( idx, &mcast);		
		rtlglue_getmstime(&time);
		while ( rtlglue_getmstime(&timeout)==time);/*sync Time*/
		rtlglue_printf("Wait Multicast Age change \n");
		do
		{
			rtlglue_getmstime(&timeout);
			rtl8651_getAsicIpMulticastTable(idx, &mcastNow);		
			if ((timeout-time) >(MCAST_TIMEOUT))
			{
				rtlglue_printf("\nWait Multicast  Entry Aging Change Too Long. (%d second Wait) \n",(timeout-time)/1000);			
					return FAILED;
			}else if ((timeout-time)%1000==0)
			{
				rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);
			}
			
		}while((mcastNow.age-mcast.age)==0);
		rtl8651_getAsicIpMulticastTable( idx, &mcast);		
		rtlglue_printf("\nMulticast  Entry Age is %d(Time Difference(31  downto 30 sec):  %d (ms) \n",mcast.age,timeout-time);			


		for (j=5;j>0;j--)
		{
			rtlglue_getmstime(&time);
			while ( rtlglue_getmstime(&timeout)==time);/*sync Time*/
			rtlglue_printf("Wait Multicast change \n");
			do
			{
				rtlglue_getmstime(&timeout);
				rtl8651_getAsicIpMulticastTable(idx, &mcastNow);		
				if ((timeout-time) >(MCAST_TIMEOUT))
				{
					rtlglue_printf("\nWait Multicast Entry Aging Change Too Long. (%d second Wait) \n",(timeout-time)/1000);			
						return FAILED;
				}else if ((timeout-time)%1000==0)
				{
					rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);
				}
				
			}while((mcastNow.age-mcast.age)==0);
			rtl8651_getAsicIpMulticastTable(idx, &mcast);		
			rtlglue_printf("\n Multicast Entry Age is %d(Time Difference(%d  downto %d sec):  %d (ms) \n",mcast.age,j,j-1,timeout-time);			
		}
		
	}
	retval = SUCCESS;	
	return retval;
}
int32 testIRateLimit(uint32 caseNo)
{
	int i;
	hsa_param_t hsa;
	int32 retval = SUCCESS;
	rtl865x_tblAsicDrv_multiCastParam_t mcast; 
	hsb_param_t hsb ;
	uint32 multi_mtu;
	ether_addr_t mac;
	rtl865x_tblAsicDrv_vlanParam_t vlant;
	
	

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		if (i!=IC_TYPE_REAL) continue;
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
			layer4Config();				
			/*strtomac(&intf.macAddr, GW_LAN0_MAC);*/
			vlant.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
			vlant.untagPortMask = 0; //tagged
			rtl8651_setAsicVlan(LAN0_VID,&vlant);

			multi_mtu=1500;
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
			memcpy((void*)&hsb.da[0],(void*)strtomac(&mac," 01-00-5e-00-00-03"),sizeof(ether_addr_t));
			hsb.sip=ntohl(inet_addr("192.168.1.1"));
			hsb.dip=ntohl(inet_addr("224.0.0.3"));
			hsb.spa=3;
			hsb.type=HSB_TYPE_UDP;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );
			IS_EQUAL_INT_DETAIL("IP should be TRIP, but .......",hsa.trip,ntohl(inet_addr(WAN0_ISP_PUBLIC_IP)), __FUNCTION__, __LINE__,i) ;
	}
	retval = SUCCESS;	
	return retval;
}


/*
 *  Enhanced GRE supports any Call ID
 *
 *  1. add an outbound entry into ASIC
 *  2. test an outbound packet with matched CallID ==> expect hsa.port is changed
 *  3. test an outbound packet with dismatched CallID ==> expect to CPU
 *  4. test an outbound packet with matched CallID, but different source IP address ==> expect to CPU
 *  5. add an inbound entry into ASIC
 *  6. test an inbound packet with matched CallID ==> expect hsa.port is changed
 *  7. test an inbound packet with dismatched CallID ==> expect to CPU
 *  8. test an inbound packet with matched CallID, but different source IP address ==> expect to CPU
 *  9. add an bi-direction entry into ASIC
 *  10. test an outbound packet with matched CallID ==> expect hsa.port is changed
 *  11. test an outbound packet with dismatched CallID ==> expect to CPU
 *  12. test an outbound packet with matched CallID, but different source IP address ==> expect to CPU
 *  13. test an inbound packet with matched CallID ==> expect hsa.port is changed
 *  14. test an inbound packet with dismatched CallID ==> expect to CPU
 *  15. test an inbound packet with matched CallID, but different source IP address ==> expect to CPU
 */
int32 testEnhancedGRE(uint32 caseNo)
{
	rtl865x_tblAsicDrv_naptIcmpParam_t pptpAsicEntry;
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 124,
		vid: 0x00,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0xa0,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_PPTP,
		patmatch:0,		ethtype: 0,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x21,0x11,0x00,0x00},
		hiprior:3,			snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch:0,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i;
	ether_addr_t  mac;
	uint32 tblIdx;
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	/* for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ ) */
	for( i = IC_TYPE_REAL; i <= IC_TYPE_REAL; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		/* ----------------------------------------- */
		retval = layer4Config();
		ASSERT( retval==SUCCESS );
		WRITE_MEM32(TEATCR,5<<ICMP_TH_OFFSET);

		/* --------------------------------------------------------------- */
		/* 1. add an outbound entry into ASIC                              */
		memset(&pptpAsicEntry, 0, sizeof(rtl865x_tblAsicDrv_naptIcmpParam_t));
		pptpAsicEntry.insideLocalId = 0xa0;
		pptpAsicEntry.insideLocalIpAddr = ntohl(inet_addr(INT2_IP));
		pptpAsicEntry.offset = 0xa569;
		pptpAsicEntry.isCollision = 1;
		pptpAsicEntry.isStatic = 1;
		pptpAsicEntry.isValid = 1;
		pptpAsicEntry.isPptp = 1;
		pptpAsicEntry.isSpi = 0;
		pptpAsicEntry.direction = 1;
		rtl8651_naptIcmpTableIndex(ntohl(inet_addr(INT2_IP)),pptpAsicEntry.insideLocalId,ntohl(inet_addr(REM1_IP)),&tblIdx); 
		retval = rtl8651_setAsicNaptIcmpTable(TRUE, tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );

		/* --------------------------------------------------------------- */
		/* 2. test an outbound packet with matched CallID ==> expect hsa.port is changed */
		hsb.spa = 3;
		hsb.sprt = 0xa0;
		hsb.sip = ntohl(inet_addr(INT2_IP));
		hsb.dip = ntohl(inet_addr(REM1_IP));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp, PM_PORT_5, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("IP should be translated", hsa.trip, ntohl(inet_addr(EXT0_IP)), __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("CallID should be translated", hsa.port, 0xa569, __FUNCTION__, __LINE__,i );

		/* --------------------------------------------------------------- */
		/* 3. test an outbound packet with dismatched CallID ==> expect to CPU */
		hsb.spa = 3;
		hsb.sprt = 0xa1;
		hsb.sip = ntohl(inet_addr(INT2_IP));
		hsb.dip = ntohl(inet_addr(REM1_IP));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );

		/* --------------------------------------------------------------- */
 		/*  4. test an outbound packet with matched CallID, but different source IP address ==> expect to CPU */
		hsb.spa = 3;
		hsb.sprt = 0xa0;
		hsb.sip = ntohl(inet_addr(INT2_IP))+1;
		hsb.dip = ntohl(inet_addr(REM1_IP));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );

 		/* --------------------------------------------------------------- */
		/* 5. add an inbound entry into ASIC                               */
		memset(&pptpAsicEntry, 0, sizeof(rtl865x_tblAsicDrv_naptIcmpParam_t));
		pptpAsicEntry.insideLocalId = 0xa0;
		pptpAsicEntry.insideLocalIpAddr = ntohl(inet_addr(INT2_IP));
		pptpAsicEntry.offset = 0xa569;
		pptpAsicEntry.isCollision = 1;
		pptpAsicEntry.isStatic = 1;
		pptpAsicEntry.isValid = 1;
		pptpAsicEntry.isPptp = 1;
		pptpAsicEntry.isSpi = 0;
		pptpAsicEntry.direction = 2;
		rtl8651_naptIcmpTableIndex(ntohl(inet_addr(REM1_IP)),pptpAsicEntry.insideLocalId,ntohl(inet_addr(EXT0_IP)),&tblIdx); 
		retval = rtl8651_setAsicNaptIcmpTable(TRUE, tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );

		/* --------------------------------------------------------------- */
		/*  6. test an inbound packet with matched CallID ==> expect hsa.port is changed */
		hsb.spa = 3;
		hsb.sprt = 0xa0;
		hsb.sip = ntohl(inet_addr(INT2_IP));
		hsb.dip = ntohl(inet_addr(REM1_IP));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp, PM_PORT_5, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("IP should be translated", hsa.trip, ntohl(inet_addr(EXT0_IP)), __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("CallID should be translated", hsa.port, 0xa569, __FUNCTION__, __LINE__,i );

		/* --------------------------------------------------------------- */
		/* 7. test an inbound packet with dismatched CallID ==> expect to CPU */
		hsb.spa = 3;
		hsb.sprt = 0xa1;
		hsb.sip = ntohl(inet_addr(INT2_IP));
		hsb.dip = ntohl(inet_addr(REM1_IP));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );

		/* --------------------------------------------------------------- */
 		/* 8. test an inbound packet with matched CallID, but different source IP address ==> expect to CPU */
		hsb.spa = 3;
		hsb.sprt = 0xa0;
		hsb.sip = ntohl(inet_addr(INT2_IP))+1;
		hsb.dip = ntohl(inet_addr(REM1_IP));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );

 
		/* --------------------------------------------------------------- */
		/*  9. add an bi-direction entry into ASIC                         */
		memset(&pptpAsicEntry, 0, sizeof(rtl865x_tblAsicDrv_naptIcmpParam_t));
		pptpAsicEntry.insideLocalId = 0xa0;
		pptpAsicEntry.insideLocalIpAddr = ntohl(inet_addr(INT2_IP));
		pptpAsicEntry.offset = 0xa569;
		pptpAsicEntry.isCollision = 1;
		pptpAsicEntry.isStatic = 1;
		pptpAsicEntry.isValid = 1;
		pptpAsicEntry.isPptp = 1;
		pptpAsicEntry.isSpi = 0;
		pptpAsicEntry.direction = 3;
		rtl8651_naptIcmpTableIndex(ntohl(inet_addr(INT2_IP)),pptpAsicEntry.insideLocalId,ntohl(inet_addr(REM1_IP)),&tblIdx); 
		retval = rtl8651_setAsicNaptIcmpTable(TRUE, tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );

		/* --------------------------------------------------------------- */
		/* 10. test an outbound packet with matched CallID ==> expect hsa.port is changed */
		hsb.spa = 3;
		hsb.sprt = 0xa0;
		hsb.sip = ntohl(inet_addr(INT2_IP));
		hsb.dip = ntohl(inet_addr(REM1_IP));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp, PM_PORT_5, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("IP should be translated", hsa.trip, ntohl(inet_addr(EXT0_IP)), __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("CallID should be translated", hsa.port, 0xa569, __FUNCTION__, __LINE__,i );

		/* --------------------------------------------------------------- */
		/* 11. test an outbound packet with dismatched CallID ==> expect to CPU */
		hsb.spa = 3;
		hsb.sprt = 0xa1;
		hsb.sip = ntohl(inet_addr(INT2_IP));
		hsb.dip = ntohl(inet_addr(REM1_IP));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );

		/* --------------------------------------------------------------- */
 		/* 12. test an outbound packet with matched CallID, but different source IP address ==> expect to CPU*/
		hsb.spa = 3;
		hsb.sprt = 0xa0;
		hsb.sip = ntohl(inet_addr(INT2_IP))+1;
		hsb.dip = ntohl(inet_addr(REM1_IP));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );

		/* --------------------------------------------------------------- */
		/* 13. test an inbound packet with matched CallID ==> expect hsa.port is changed */
		hsb.spa = 3;
		hsb.sprt = 0xa0;
		hsb.sip = ntohl(inet_addr(INT2_IP));
		hsb.dip = ntohl(inet_addr(REM1_IP));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp, PM_PORT_5, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("IP should be translated", hsa.trip, ntohl(inet_addr(EXT0_IP)), __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("CallID should be translated", hsa.port, 0xa569, __FUNCTION__, __LINE__,i );

		/* --------------------------------------------------------------- */
		/* 14. test an inbound packet with dismatched CallID ==> expect to CPU */
		hsb.spa = 3;
		hsb.sprt = 0xa1;
		hsb.sip = ntohl(inet_addr(INT2_IP));
		hsb.dip = ntohl(inet_addr(REM1_IP));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );

		/* --------------------------------------------------------------- */
 		/* 15. test an inbound packet with matched CallID, but different source IP address ==> expect to CPU */
		hsb.spa = 3;
		hsb.sprt = 0xa0;
		hsb.sip = ntohl(inet_addr(INT2_IP))+1;
		hsb.dip = ntohl(inet_addr(REM1_IP));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		retval=rtl8651_getAsicNaptIcmpTable(tblIdx, &pptpAsicEntry);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ,but ....", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i );
	}
	retval = SUCCESS;	
	return retval;
}


/*
 * Test L3 Routing Process to CPU (for IP unnumberred host more than 16)
 */
int32 testLayer3RoutingToCpu(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,		len: 64,
		vid: 0x0,		tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("192.168.1.2")),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 80,			ipptl: 0xdd,
		iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,	
		patmatch:0,		ethtype: 0x0800,
		ipfg:0,			ipfo0_n:0,
		sa:{0x00,0x80,0xc0,0x00,0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
		
	};
	rtl865x_tblAsicDrv_routingParam_t routet;
	hsa_param_t hsa;
	int i;
	ether_addr_t mac;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	/* for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ ) */
	for( i = IC_TYPE_REAL; i <= IC_TYPE_REAL; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		

		/*********************************************************************		
		 * Config L3 (including a entry with process to cpu)
		 *********************************************************************/
		layer3Config(0);
		/* route to special net: process to CPU */
		strtoip(&routet.ipAddr, "222.222.222.222");
		strtoip(&routet.ipMask, "255.255.255.128");
		routet.process = PROCESS_S_CPU;
		routet.internal = 1;
		rtl8651_setAsicRouting(2, &routet);

		/*********************************************************************		
		Expect : Forward
		*********************************************************************/
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,GW_LAN0_MAC),sizeof(ether_addr_t));
		hsb.dip=ntohl(inet_addr("222.222.222.234"));
		hsb.sip=ntohl(inet_addr("192.168.1.2")),
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );	
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("hsa.dpc should be 1", hsa.dpc, 1, __FUNCTION__, __LINE__, i );
		IS_EQUAL_INT_DETAIL("Pakcet should be trapped to CPU", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__, i );
		IS_EQUAL_INT_DETAIL("Packet should be forwarded to CPU", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__,i) ;
		IS_EQUAL_INT_DETAIL("CPU Reason[4:1] should be L34 Action Required (real, expect)", (hsa.why2cpu>>1)&0xf, 5/*L34 Action Required*/, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("CPU Reason[9:5] should be NPI(SIP) (real, expect)", (hsa.why2cpu>>5)&0x1f, 0x10/*NPI*/, __FUNCTION__, __LINE__,i );
		IS_EQUAL_INT_DETAIL("CPU Reason[14:10] should be NPI(DIP) (real, expect)", (hsa.why2cpu>>10)&0x1f, 0x10/*NPI*/, __FUNCTION__, __LINE__,i );


	}
	retval = SUCCESS;	
	return retval;
}
