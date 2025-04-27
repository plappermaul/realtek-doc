/*
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
*
* Program : Test Code for 865xC NAPT
* Abstract :
* Author : (rupert@realtek.com.tw)
* $Id: naptTest.c,v 1.27 2007-02-08 09:59:04 yjlou Exp $
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
#include "l34Test.h"
#include "hsModel.h"
#ifdef CONFIG_RTL865XC
#include "rtl865xC_tblAsicDrv.h"
#else
#include "rtl8651_tblAsicDrv.h"
#endif
#include "rtl8651_tblDrvLocal.h"
//#define RTL8651_TBLDRV_LOCAL_H
#include "rtl8651_aclLocal.h"
#include "rtl8651_layer2local.h"
#include "rtl8651_layer3local.h"
#include "rtl8651_layer4local.h"
#include "fwdTest.h"
#include "rtl8651_tblDrvFwd.h"
#include "virtualMac.h"
#include "rtl8651_multicast.h"
#include "rtl8651_dos.h"
#include "mbufGen.h"
#include "naptTest.h"

#define WAN0_VID 8
#define GW_WAN0_MAC "00-00-00-00-88-88"
#define WAN1_VID 9
#define GW_WAN1_MAC "00-00-00-00-88-89"
#define GW_WAN1_SUBNET_MASK "255.255.255.248"
#define LAN0_VID 10
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
#define WAN1_ISP_PUBLIC_IP "192.168.3.1"
#define GW_WAN1_IP "192.168.3.1"
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

/* define INT/EXT/REM hosts */
#define INT2_IP HOST2_IP
#define EXT0_IP WAN0_ISP_PUBLIC_IP
#define REM1_IP "120.168.1.3"

#define NAPT_INTF_RTK0 (ntohl(inet_addr("192.168.1.254"))) /*IP address of network interface rtk0*/
#define NAPT_INTF_RTK1 (ntohl(inet_addr("140.113.214.102"))) /*IP address of network interface rtk1*/
#define NAPT_INTF_RTK1_2 (ntohl(inet_addr("140.113.214.117"))) /*IP address of network interface rtk1 (Local Public)*/
#define NAPT_INTF_RTK2 (ntohl(inet_addr("140.113.131.53"))) /*IP address of network interface rtk2*/



int8 ConfigNaptTestEnvironment(void)
{
	uint32 ret;
	ether_addr_t ether0 = { { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 } };
	ether_addr_t ether1 = { { 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f } };
	ether_addr_t ether2 = { { 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f } };

	/*internal interface*/
	ret = rtl8651_addNetworkIntf( "rtk0" );
	IS_EQUAL_INT( "addNetworkIntf(rtk0) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	ret = rtl8651_assignVlanMacAddress( 1, &ether0, 1 );
	IS_EQUAL_INT( "assignVlanMacAddress(rtk0) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	ret = rtl8651_specifyNetworkIntfLinkLayerType( "rtk0", RTL8651_LL_VLAN, 1 );
	IS_EQUAL_INT( "specifyNetworkIntfLinkLayerType(rtk0) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	
	ret = rtl8651_addIpIntf( "rtk0", NAPT_INTF_RTK0, ntohl( inet_addr("255.255.255.0") ) );
	IS_EQUAL_INT( "addIpIntf(rtk0) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/*external interface 1*/
	ret = rtl8651_addVlan( 2 );
	IS_EQUAL_INT( "addVlan(rtk1) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	ret = rtl8651_addNetworkIntf( "rtk1" );
	IS_EQUAL_INT( "addNetworkIntf(rtk1) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	ret = rtl8651_assignVlanMacAddress( 2, &ether1, 1 );
	IS_EQUAL_INT( "assignVlanMacAddress(rtk1) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	ret = rtl8651_specifyNetworkIntfLinkLayerType( "rtk1", RTL8651_LL_VLAN, 2 );
	IS_EQUAL_INT( "specifyNetworkIntfLinkLayerType(rtk1) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	ret = rtl8651_addExtNetworkInterface( "rtk1" );
	IS_EQUAL_INT( "addExtNetworkInterface(rtk1) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	
	ret = rtl8651_addIpIntf( "rtk1", NAPT_INTF_RTK1, ntohl( inet_addr("255.255.255.0") ) );
	IS_EQUAL_INT( "addIpIntf(rtk1) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	ret = rtl8651_addNaptMapping( NAPT_INTF_RTK1 );
	IS_EQUAL_INT( "addNaptMappingExt(rtk1) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/*external interface 2*/
	ret = rtl8651_addVlan( 3 );
	IS_EQUAL_INT( "addVlan(rtk1) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	
	ret = rtl8651_addNetworkIntf( "rtk2" );
	IS_EQUAL_INT( "addNetworkIntf(rtk2) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	ret = rtl8651_assignVlanMacAddress( 3, &ether2, 1 );
	IS_EQUAL_INT( "assignVlanMacAddress(rtk2) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	ret = rtl8651_specifyNetworkIntfLinkLayerType( "rtk2", RTL8651_LL_VLAN, 3 );
	IS_EQUAL_INT( "specifyNetworkIntfLinkLayerType(rtk2) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	ret = rtl8651_addExtNetworkInterface( "rtk2" );
	IS_EQUAL_INT( "addExtNetworkInterface(rtk2) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	
	ret = rtl8651_addIpIntf( "rtk2", NAPT_INTF_RTK2, ntohl( inet_addr("255.255.255.0") ) );
	IS_EQUAL_INT( "addIpIntf(rtk2) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	ret = rtl8651_addDriverNaptMapping( NAPT_INTF_RTK2 );
	IS_EQUAL_INT( "addNaptMappingExt(rtk2) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/*add default route*/
	ret = rtl8651_addRoute( ntohl( inet_addr( "0.0.0.0" ) ), ntohl( inet_addr( "0.0.0.0" ) ),
	                        "rtk1", ntohl( inet_addr( "140.113.214.254" ) ) );
	IS_EQUAL_INT( "addRoute(DefaultRoute) failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/*set port range*/
	ret = rtl8651_setNaptTcpUdpPortRange( 1024, 20480-1 );
	IS_EQUAL_INT( "rtl8651_setNaptTcpUdpPortRange() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/*add local public*/
	ret = rtl8651_addLocalServer( NAPT_INTF_RTK1_2 );
	IS_EQUAL_INT( "rtl8651_addLocalServer() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/* multicast related setting*/
	{
		rtl8651_tblDrvMCastPara_t para;

		para.MCastUpstream_Timeout	= 0xffffffff;
		para.MCastMember_ExpireTime	= 0xffffffff;
		para.igmp_sqi					= 0xffffffff;
		para.igmp_sqc				= 0;	/*disable startup query*/
		para.igmp_response_tolerance_delay	= 0xffffffff;
		para.igmp_qri					= 0xffffffff;
		para.igmp_qi					= 0xffffffff;
		para.igmp_oqpi				= 0xffffffff;
		para.igmp_lmqi				= 0xffffffff;
		para.igmp_lmqc				= 0;
		para.igmp_group_query_interval	= 0xffffffff;
		ASSERT(rtl8651_multicastSetPara(&para) == SUCCESS);
		rtl8651_multicastPeriodicQuery(FALSE);		/*disable periodically query*/
	}
	
	return FAILED;
}





int32 testNaptLanToWan(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,	tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("192.168.1.3")),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 8800,		ipptl: 0x11,
		ipfg: 0x0,		iptos: 0x65,
		tcpfg: 0x00,	type: HSB_TYPE_UDP,
		patmatch: 1,	ethtype: 0x7788,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x00,0x00,0x00,0x00,0x40},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 0x3,
		dirtx: 0,		l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0, 	urlmch: 1,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i;
	rtl865x_tblAsicDrv_extIntIpParam_t  ipt;
	rtl865x_tblAsicDrv_naptTcpUdpParam_t napt;
	int idx1;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		layer4Config();
		/* Set UDP timeout to 5566 sec */
		WRITE_MEM32( TEATCR, (READ_MEM32(TEATCR)&~UDPT_MASK)|(_rtl8651_NaptAgingToUnit(5566)<<UDPT_OFFSET) ); 
		WRITE_MEM32(PBPCR, 0x00000000 ); /* default value */

		/* config server port table */
		bzero((void*) &ipt, sizeof(ipt));
		bzero((void*) &napt, sizeof(napt));

		/* internal port 1020 translated to wan 0 isp port 20 */
		strtoip(&ipt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		strtoip(&napt.insideLocalIpAddr, HOST2_IP);
		napt.insideLocalPort = 1020;
		napt.isValid = 1;
		napt.isDedicated = 0;
		napt.isTcp = 0;
		napt.selExtIPIdx = 2;

/*
		   rtlglue_printf("HSB.SIP: %x\n", hsb.sip);
		   rtlglue_printf("HSB.SPORT: %d\n", hsb.sprt);
		   rtlglue_printf("HSB.DIP: %x\n", hsb.dip);
		   rtlglue_printf("HSB.DPORT: %d\n", hsb.dprt);
*/

		retval = rtl8651_setAsicExtIntIpTable(2, &ipt);
		ASSERT( retval == SUCCESS );
		idx1 = rtl8651_naptTcpUdpTableIndex(0, hsb.sip, hsb.sprt, hsb.dip, hsb.dprt);
		retval = rtl8651_setAsicNaptTcpUdpTable(1, idx1, &napt);
		ASSERT( retval == SUCCESS );

		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.trip, ntohl(inet_addr(WAN0_ISP_PUBLIC_IP)), __FUNCTION__, __LINE__, i );

		/* Expect the NAPT age is updated */
		bzero((void*) &napt, sizeof(napt));
		retval = rtl8651_getAsicNaptTcpUdpTable( idx1, &napt);
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("NAPT Age should be updated 5566 secs", _rtl8651_NaptAgingToUnit(napt.ageSec), _rtl8651_NaptAgingToUnit(5566), __FUNCTION__, __LINE__, i );
	}
	retval = SUCCESS;	
	return retval;
}

int32 testNaptLanToWan2(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,	tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("192.168.1.3")),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 8800,		ipptl: 0x11,
		ipfg: 0x0,		iptos: 0x65,
		tcpfg: 0x00,	type: HSB_TYPE_UDP,
		patmatch: 1,	ethtype: 0x7788,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x00,0x00,0x00,0x00,0x40},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 0x3,
		dirtx: 0,		l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0, 	urlmch: 1,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i;
	rtl865x_tblAsicDrv_extIntIpParam_t  ipt;
	rtl865x_tblAsicDrv_naptTcpUdpParam_t napt;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		layer4Config();
		WRITE_MEM32(PBPCR, 0x00000000 ); /* default value */

		/* ----------------------------------------- */
		/* Test bi-direction                         */
		/* config server port table */
		bzero((void*) &ipt, sizeof(ipt));
		bzero((void*) &napt, sizeof(napt));

		/* internal port 1020 translated to wan 0 isp port 20 */
		strtoip(&ipt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		strtoip(&napt.insideLocalIpAddr, HOST2_IP);
		napt.insideLocalPort = 1020;
		napt.isValid = 1;
		napt.isDedicated = 1;
		napt.isTcp = 0;
		napt.selExtIPIdx = 2;
		napt.tcpFlag = 0;
		napt.selEIdx = 1011;
		napt.offset = 0x1b;

		retval = rtl8651_setAsicExtIntIpTable(2, &ipt);
		ASSERT( retval == SUCCESS );
		retval = rtl8651_setAsicNaptTcpUdpTable(1, rtl8651_naptTcpUdpTableIndex(0, hsb.sip, hsb.sprt, 0, 0), &napt);
		ASSERT( retval == SUCCESS );

		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.trip, ntohl(inet_addr(WAN0_ISP_PUBLIC_IP)), __FUNCTION__, __LINE__, i );
		IS_EQUAL_INT_DETAIL("Pakcet translated port should be ....", hsa.port, (napt.offset<<10) + napt.selEIdx, __FUNCTION__, __LINE__, i );

		/* ----------------------------------------- */
		/* Test bi-direction (II)                    */
		/* config server port table */
		bzero((void*) &ipt, sizeof(ipt));
		bzero((void*) &napt, sizeof(napt));

		/* internal port 1020 translated to wan 0 isp port 20 */
		strtoip(&ipt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		strtoip(&napt.insideLocalIpAddr, HOST2_IP);
		napt.insideLocalPort = 1020;
		napt.isValid = 1;
		napt.isDedicated = 1;
		napt.isTcp = 0;
		napt.selExtIPIdx = 2;
		napt.tcpFlag = 1;
		napt.selEIdx = 1111;

		retval = rtl8651_setAsicExtIntIpTable(2, &ipt);
		ASSERT( retval == SUCCESS );
		retval = rtl8651_setAsicNaptTcpUdpTable(1, rtl8651_naptTcpUdpTableIndex(0, hsb.sip, hsb.sprt, 0, 0), &napt);
		ASSERT( retval == SUCCESS );

		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.trip, ntohl(inet_addr(WAN0_ISP_PUBLIC_IP)), __FUNCTION__, __LINE__, i );

		/* ----------------------------------------- */
		/* Test outbound only                        */
		/* config server port table */
		bzero((void*) &ipt, sizeof(ipt));
		bzero((void*) &napt, sizeof(napt));

		/* internal port 1020 translated to wan 0 isp port 20 */
		strtoip(&ipt.extIpAddr, WAN0_ISP_PUBLIC_IP);
		strtoip(&napt.insideLocalIpAddr, HOST2_IP);
		napt.insideLocalPort = 1020;
		napt.isValid = 1;
		napt.isDedicated = 1;
		napt.isTcp = 0;
		napt.selExtIPIdx = 2;
		napt.tcpFlag = 3;
		napt.selEIdx = 1111;

		retval = rtl8651_setAsicExtIntIpTable(2, &ipt);
		ASSERT( retval == SUCCESS );
		retval = rtl8651_setAsicNaptTcpUdpTable(1, rtl8651_naptTcpUdpTableIndex(0, hsb.sip, hsb.sprt, 0, 0), &napt);
		ASSERT( retval == SUCCESS );

		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.trip, ntohl(inet_addr(WAN0_ISP_PUBLIC_IP)), __FUNCTION__, __LINE__, i );

	}
	retval = SUCCESS;	
	return retval;
}
int32 testNaptLanToWanPriority(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,	tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("192.168.1.3")),
		sprt: 1020,		dip:ntohl(inet_addr("120.168.1.3")),
		dprt: 8800,		ipptl: 0x11,
		ipfg: 0x0,		iptos: 0x65,
		tcpfg: 0x00,	type: HSB_TYPE_UDP,
		patmatch: 1,	ethtype: 0x7788,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x00,0x00,0x00,0x00,0x40},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 0x3,
		dirtx: 0,		l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0, 	urlmch: 1,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i;
	rtl865x_tblAsicDrv_extIntIpParam_t  ipt;
	rtl865x_tblAsicDrv_naptTcpUdpParam_t napt;
	int pbp, naptp; /* Port-Based Priority and NAPT priority */
	uint8 szBuf[128];

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		layer4Config();
		WRITE_MEM32(QIDDPCR,(1<<NAPT_PRI_OFFSET)|(1<<PBP_PRI_OFFSET));
		WRITE_MEM32(PBPCR, 0x00000000 ); /* default value */

		for( pbp = 0; pbp<=7; pbp++ )
		{
			for( naptp = 0; naptp<=7; naptp++ )
			{
				WRITE_MEM32( PBPCR, pbp<<PBPRI_P3_OFFSET );
				
				/* ----------------------------------------- */
				/* Test bi-direction                         */
				/* config server port table */
				bzero((void*) &ipt, sizeof(ipt));
				bzero((void*) &napt, sizeof(napt));

				/* internal port 1020 translated to wan 0 isp port 20 */
				strtoip(&ipt.extIpAddr, WAN0_ISP_PUBLIC_IP);
				strtoip(&napt.insideLocalIpAddr, HOST2_IP);
				napt.insideLocalPort = 1020;
				napt.isValid = 1;
				napt.isDedicated = 1;
				napt.isTcp = 0;
				napt.selExtIPIdx = 2;
				napt.tcpFlag = 0;
				napt.selEIdx = 1111;
				napt.priValid=1;
				napt.priority=naptp;
				retval = rtl8651_setAsicExtIntIpTable(2, &ipt);
				ASSERT( retval == SUCCESS );
				retval = rtl8651_setAsicNaptTcpUdpTable(1, rtl8651_naptTcpUdpTableIndex(0, hsb.sip, hsb.sprt, 0, 0), &napt);
				ASSERT( retval == SUCCESS );

				retval = virtualMacSetHsb( &hsb );
				ASSERT( retval == SUCCESS );
				retval = virtualMacGetHsa( &hsa );
				ASSERT( retval == SUCCESS );
				IS_EQUAL_INT_DETAIL("Pakcet should be to ....", hsa.trip, ntohl(inet_addr(WAN0_ISP_PUBLIC_IP)), __FUNCTION__, __LINE__, i );
				if ( naptp > pbp ) /* ALE will choose the bigger one. */
				{
					snprintf( szBuf, sizeof(szBuf), "hsa.priority(%d) is not matched with naptp(%d). pbp=(%d)", hsa.priority, naptp, pbp );
					IS_EQUAL_INT_DETAIL( szBuf, hsa.priority, naptp , __FUNCTION__, __LINE__,i);
				}
				else
				{
					snprintf( szBuf, sizeof(szBuf), "hsa.priority(%d) is not matched with pbp(%d). naptp=(%d)", hsa.priority, pbp, naptp );
					IS_EQUAL_INT_DETAIL( szBuf, hsa.priority, pbp , __FUNCTION__, __LINE__,i);
				}
			}
		}
	}
	retval = SUCCESS;	
	return retval;
}

/*
 *  test Enhanced Hash1
 *
 *  1. enable Enhanced Hash1
 *  2. add an entry to outbound hash1 location
 *  3. give an outbound flow, expect SPort is changed as any number.
 *  4. give another outbound flow, expect to CPU
 *  5. add an entry to inbound hash1 location
 *  6. give an inbound entry, expect DPort is changed.
 *  7. give another inbound entry, expect to CPU
 */
int32 testNaptEnhancedHash1(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 3,			len: 64,
		vid: 0x0234,	tagif: 0,
		pppoeif: 0,		sip: ntohl(inet_addr("0.0.0.0")),
		sprt: 0,		dip:ntohl(inet_addr("0.0.0.0")),
		dprt: 0,		ipptl: 0x11,
		ipfg: 0x0,		iptos: 0x65,
		tcpfg: 0x00,	type: HSB_TYPE_IP,
		patmatch: 1,	ethtype: 0x7788,
		da:{0x00,0x00,0x00,0x00,0x88,0x8a},
		sa:{0x00,0x00,0x00,0x00,0x00,0x40},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 0x3,
		dirtx: 0,		l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0, 	urlmch: 1,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i;
	rtl865x_tblAsicDrv_extIntIpParam_t  ipt;
	rtl865x_tblAsicDrv_naptTcpUdpParam_t napt;
	uint32 index1;
	int isTcp;
	int tcpLongTimeout = 7788;
	int udpTimeout = 7799;
	int pbp, naptp; /* Port-Based Priority and NAPT priority */
	uint8 szBuf[128];

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	/* for( i = IC_TYPE_REAL; i <= IC_TYPE_REAL; i++ ) */
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		/* ----------------------------------------- */
		/* 1. Enable Enhanced Hash1                  */
		layer4Config();
		WRITE_MEM32(QIDDPCR,(1<<NAPT_PRI_OFFSET)|(1<<PBP_PRI_OFFSET));
		WRITE_MEM32( SWTCR1, READ_MEM32(SWTCR1)|L4EnHash1 );
		/* Set TCP long timeout */
		WRITE_MEM32( TEATCR, (READ_MEM32(TEATCR)&~TCPLT_MASK&~UDPT_MASK)|(_rtl8651_NaptAgingToUnit(tcpLongTimeout)<<TCPLT_OFFSET)|
		                                                                 (_rtl8651_NaptAgingToUnit(udpTimeout)<<UDPT_OFFSET) ); 
		WRITE_MEM32(QIDDPCR,(1<<NAPT_PRI_OFFSET)|(1<<PBP_PRI_OFFSET));
		WRITE_MEM32(PBPCR, 0x00000000 ); /* default value */

		for( pbp = 0; pbp<=7; pbp++ )
		{
			for( naptp = 0; naptp<=7; naptp++ )
			{
				WRITE_MEM32( PBPCR, pbp<<PBPRI_P3_OFFSET );

				for( isTcp=FALSE; isTcp<=TRUE; isTcp++ )
				{
					/* ----------------------------------------- */
					/* 2. add an entry to outbound hash1 location*/
					/*    config napt table                      */
					bzero((void*) &ipt, sizeof(ipt));
					bzero((void*) &napt, sizeof(napt));
					index1 = rtl8651_naptTcpUdpTableIndex(isTcp, ntohl(inet_addr(INT2_IP)), 1020, ntohl(inet_addr(REM1_IP)), 8866);
					if ( isTcp==TRUE )
						hsb.type = HSB_TYPE_TCP;
					else
						hsb.type = HSB_TYPE_UDP;
			
					/* internal port 1020 translated to wan 0 isp port 42405(0xa5a5) */
					strtoip(&ipt.extIpAddr, EXT0_IP);
					strtoip(&napt.insideLocalIpAddr, INT2_IP);
					napt.insideLocalPort = 1020;
					napt.offset = 0x29; /* hsb.dprt[15:10] of 0xa5a5 */
					napt.selEIdx = 0x1a5; /* hsb.dprt[9:0] of 0xa5a5 */
					napt.isValid = 1;
					napt.isDedicated = 0;
					napt.isTcp = isTcp;
					napt.tcpFlag = 1/*outbound*/;
					napt.selExtIPIdx = 4/*no use in outbound*/;
					napt.ageSec = 2266;
					napt.isCollision = 0;
					napt.isCollision2 = 0;
					napt.isStatic = 0;
					napt.priValid = 1;
					napt.priority = naptp;
					retval = rtl8651_setAsicExtIntIpTable(2, &ipt);
					ASSERT( retval == SUCCESS );
					retval = rtl8651_setAsicNaptTcpUdpTable(1, index1, &napt);
					ASSERT( retval == SUCCESS );
			
					/* ----------------------------------------- */
					/* 3. give an outbound flow, expect SPort is changed as any number. */
					hsb.sip = ntohl(inet_addr(INT2_IP));
					hsb.sprt = 1020;
					hsb.dip = ntohl(inet_addr(REM1_IP));
					hsb.dprt = 8866;
					retval = virtualMacSetHsb( &hsb );
					ASSERT( retval == SUCCESS );
					retval = virtualMacGetHsa( &hsa );
					ASSERT( retval == SUCCESS );
					IS_NOT_EQUAL_INT_DETAIL("Packet should be forwarded by ASIC", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__, i );
					IS_EQUAL_INT_DETAIL("trip wrong", hsa.trip, ntohl(inet_addr(EXT0_IP)), __FUNCTION__, __LINE__, i );
					IS_EQUAL_INT_DETAIL("l4 port wrong", hsa.port, 0xa5a5, __FUNCTION__, __LINE__, i );
					if ( naptp > pbp ) /* ALE will choose the bigger one. */
					{
						snprintf( szBuf, sizeof(szBuf), "hsa.priority(%d) is not matched with naptp(%d). pbp=(%d)", hsa.priority, naptp, pbp );
						IS_EQUAL_INT_DETAIL( szBuf, hsa.priority, naptp , __FUNCTION__, __LINE__,i);
					}
					else
					{
						snprintf( szBuf, sizeof(szBuf), "hsa.priority(%d) is not matched with pbp(%d). naptp=(%d)", hsa.priority, pbp, naptp );
						IS_EQUAL_INT_DETAIL( szBuf, hsa.priority, pbp , __FUNCTION__, __LINE__,i);
					}
			
					/* Expect the NAPT age is updated */
					bzero((void*) &napt, sizeof(napt));
					retval = rtl8651_getAsicNaptTcpUdpTable( index1, &napt);
					ASSERT( retval == SUCCESS );
					if ( isTcp==TRUE )
						IS_EQUAL_INT_DETAIL("NAPT Age should be updated to long timeout", _rtl8651_NaptAgingToUnit(napt.ageSec), _rtl8651_NaptAgingToUnit(tcpLongTimeout), __FUNCTION__, __LINE__, i );
					else
						IS_EQUAL_INT_DETAIL("NAPT Age should be updated to UDP timeout", _rtl8651_NaptAgingToUnit(napt.ageSec), _rtl8651_NaptAgingToUnit(udpTimeout), __FUNCTION__, __LINE__, i );

					/* ----------------------------------------- */
					/* 4. give another outbound flow, expect to CPU*/
					hsb.sip = ntohl(inet_addr(INT2_IP));
					hsb.sprt = 1021; /* differs */
					hsb.dip = ntohl(inet_addr(REM1_IP));
					hsb.dprt = 8866;
					retval = virtualMacSetHsb( &hsb );
					ASSERT( retval == SUCCESS );
					retval = virtualMacGetHsa( &hsa );
					ASSERT( retval == SUCCESS );
					IS_EQUAL_INT_DETAIL("trip wrong", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__, i );
					IS_EQUAL_INT_DETAIL("CPU Reason[4:1] should be NAPT process (real, expect)", (hsa.why2cpu>>1)&0xf, 7/*NAPT process*/, __FUNCTION__, __LINE__,i );
					IS_EQUAL_INT_DETAIL("CPU Reason[9:5] should be NPI index (SIP) (real, expect)", (hsa.why2cpu>>5)&0x1f, 0x10/*NPI*/, __FUNCTION__, __LINE__,i );
					IS_EQUAL_INT_DETAIL("CPU Reason[14:10] should be RP(DIP) (real, expect)", (hsa.why2cpu>>10)&0x1f, 0x13/*RP*/, __FUNCTION__, __LINE__,i );
					{ /* Since NAPT-based is not matched, ALE will always choose Port-Based. */
						snprintf( szBuf, sizeof(szBuf), "hsa.priority(%d) is not matched with pbp(%d)", hsa.priority, pbp );
						IS_EQUAL_INT_DETAIL( szBuf, hsa.priority, pbp , __FUNCTION__, __LINE__,i);
					}
				
			
					/* ----------------------------------------- */
					/* 5. add an entry to inbound hash1 location */
					/*    config napt table                      */
					bzero((void*) &ipt, sizeof(ipt));
					bzero((void*) &napt, sizeof(napt));
			
					/* external port 42405(0xa5a5) translated to internal port 1020 */
					index1 = rtl8651_naptTcpUdpTableIndex(isTcp, ntohl(inet_addr(REM1_IP)), 8866, ntohl(inet_addr(EXT0_IP)), 0xa5a5);
					strtoip(&ipt.extIpAddr, EXT0_IP);
					strtoip(&napt.insideLocalIpAddr, INT2_IP);
					napt.insideLocalPort = 1020;
					napt.offset = 0x25/* hsb.dprt[5:0] of 0xa5a5 */;
					napt.selExtIPIdx = 6/* hsb.dprt[9:6] of 0xa5a5 */;
					napt.selEIdx = rtl8651_naptTcpUdpTableIndex(HASH_FOR_VERI|isTcp, ntohl(inet_addr(REM1_IP)), 8866, 0, 0); /* veri */
					napt.isValid = 1;
					napt.isDedicated = 0;
					napt.isTcp = isTcp;
					napt.tcpFlag = 0/*inbound*/;
					napt.ageSec = 2266;
					napt.isCollision = 0;
					napt.isCollision2 = 0;
					napt.isStatic = 0;
					napt.priValid = 1;
					napt.priority = naptp;
					retval = rtl8651_setAsicExtIntIpTable(2, &ipt);
					ASSERT( retval == SUCCESS );
					retval = rtl8651_setAsicNaptTcpUdpTable(1, index1, &napt);
					ASSERT( retval == SUCCESS );
			
					/* ----------------------------------------- */
					/*  6. give an inbound entry, expect DPort is changed. */
					hsb.sip = ntohl(inet_addr(REM1_IP));
					hsb.sprt = 8866;
					hsb.dip = ntohl(inet_addr(EXT0_IP));
					hsb.dprt = 0xa5a5;
					retval = virtualMacSetHsb( &hsb );
					ASSERT( retval == SUCCESS );
					retval = virtualMacGetHsa( &hsa );
					ASSERT( retval == SUCCESS );
					IS_NOT_EQUAL_INT_DETAIL("Packet should be forwarded by ASIC", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__, i );
					IS_EQUAL_INT_DETAIL("trip wrong", hsa.trip, ntohl(inet_addr(INT2_IP)), __FUNCTION__, __LINE__, i );
					IS_EQUAL_INT_DETAIL("l4 port wrong", hsa.port, 1020, __FUNCTION__, __LINE__, i );
					if ( naptp > pbp ) /* ALE will choose the bigger one. */
					{
						snprintf( szBuf, sizeof(szBuf), "hsa.priority(%d) is not matched with naptp(%d). pbp=(%d)", hsa.priority, naptp, pbp );
						IS_EQUAL_INT_DETAIL( szBuf, hsa.priority, naptp , __FUNCTION__, __LINE__,i);
					}
					else
					{
						snprintf( szBuf, sizeof(szBuf), "hsa.priority(%d) is not matched with pbp(%d). naptp=(%d)", hsa.priority, pbp, naptp );
						IS_EQUAL_INT_DETAIL( szBuf, hsa.priority, pbp , __FUNCTION__, __LINE__,i);
					}
			
					/* Expect the NAPT age is updated */
					bzero((void*) &napt, sizeof(napt));
					retval = rtl8651_getAsicNaptTcpUdpTable( index1, &napt);
					ASSERT( retval == SUCCESS );
					if ( isTcp==TRUE )
						IS_EQUAL_INT_DETAIL("NAPT Age should be updated to long timeout", _rtl8651_NaptAgingToUnit(napt.ageSec), _rtl8651_NaptAgingToUnit(tcpLongTimeout), __FUNCTION__, __LINE__, i );
					else
						IS_EQUAL_INT_DETAIL("NAPT Age should be updated to UDP timeout", _rtl8651_NaptAgingToUnit(napt.ageSec), _rtl8651_NaptAgingToUnit(udpTimeout), __FUNCTION__, __LINE__, i );

					/* ----------------------------------------- */
					/*  7. give another inbound entry, expect to CPU */
					hsb.sip = ntohl(inet_addr(REM1_IP));
					hsb.sprt = 8867;
					hsb.dip = ntohl(inet_addr(EXT0_IP));
					hsb.dprt = 0xa5a5;
					retval = virtualMacSetHsb( &hsb );
					ASSERT( retval == SUCCESS );
					retval = virtualMacGetHsa( &hsa );
					ASSERT( retval == SUCCESS );
					IS_EQUAL_INT_DETAIL("trip wrong", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__, i );
					{ /* Since NAPT-based is not matched, ALE will always choose Port-Based. */
						snprintf( szBuf, sizeof(szBuf), "hsa.priority(%d) is not matched with pbp(%d)", hsa.priority, pbp );
						IS_EQUAL_INT_DETAIL( szBuf, hsa.priority, pbp , __FUNCTION__, __LINE__,i);
					}
				}
			}
		}	
	}
	retval = SUCCESS;	
	return retval;
}


int32 _rtl8651_getOutboundNaptFlow( int8, ipaddr_t, uint16, ipaddr_t, uint16, struct rtl8651_tblDrv_naptTcpUdpFlowEntry_s** );
int32 tcpudp_entry_cmp(rtl865x_tblAsicDrv_naptTcpUdpParam_t *entry1, rtl865x_tblAsicDrv_naptTcpUdpParam_t *entry2, int8 *_fun_, uint32 _line_)
{
	IS_EQUAL_IP("insideLocalIpAddr", entry1->insideLocalIpAddr, entry2->insideLocalIpAddr, _fun_, _line_);
	IS_EQUAL_INT("insideLocalPort", entry1->insideLocalPort, entry2->insideLocalPort, _fun_, _line_);
	IS_EQUAL_INT("tcpFlag", entry1->tcpFlag, entry2->tcpFlag, _fun_, _line_);
	IS_EQUAL_INT("offset", entry1->offset, entry2->offset, _fun_, _line_);
	IS_EQUAL_INT("selExtIPIdx", entry1->selExtIPIdx, entry2->selExtIPIdx, _fun_, _line_);
	IS_EQUAL_INT("selEIdx", entry1->selEIdx, entry2->selEIdx, _fun_, _line_);
	IS_EQUAL_BOOL("isTcp", entry1->isTcp, entry2->isTcp, _fun_, _line_);
	IS_EQUAL_BOOL("isCollision", entry1->isCollision, entry2->isCollision, _fun_, _line_);
	IS_EQUAL_BOOL("isCollision2", entry1->isCollision2, entry2->isCollision2, _fun_, _line_);
	IS_EQUAL_BOOL("isStatic", entry1->isStatic, entry2->isStatic, _fun_, _line_);
	IS_EQUAL_BOOL("isValid", entry1->isValid, entry2->isValid, _fun_, _line_);
	IS_EQUAL_BOOL("isDedicated", entry1->isDedicated, entry2->isDedicated, _fun_, _line_);
	IS_EQUAL_INT("ageSec",
				_rtl8651_NaptAgingToSec(_rtl8651_NaptAgingToUnit(entry1->ageSec)),
				entry2->ageSec, _fun_, _line_
	);
	return SUCCESS;
}


#define TCPUDPTBL_BITSMSK		0x3ff
#define TCPUDPTBL_INBOUND_PORTOFFSET	6
#define TCPUDPTBL_INBOUND_PORTMSK		0x3f
#define TCPUDPTBL_INBOUND_PORTOFFSETMSK 0xf



/*These cases are used to test if the addNaptConnection() can detect the error.*/
int32 AddNaptConnectionErrors(uint32 caseNo) 
{
	int32 ret;
	uint32 insideLocalIpAddr = ntohl( inet_addr("192.168.222.222") );
	uint16 insideLocalPort = 80;
	ipaddr_t insideGlobalIpAddr = ntohl( inet_addr("140.113.222.222") );
	uint16 insideGlobalPort = 80;
	ipaddr_t remotePublicIpAddr = ntohl( inet_addr("140.113.222.222") );
	uint16 remotePublicPort = 1234;

	START_TEST_CASE();
	ConfigNaptTestEnvironment();

	/*Not Internal IP*/
	ret = rtl8651_addNaptConnection( TRUE, TCP_ESTABLISHED_FLOW, insideLocalIpAddr, insideLocalPort, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 remotePublicIpAddr, remotePublicPort );
	IS_EQUAL_INT( "addNaptConnection() should fail", ret, TBLDRV_ENOTINTIP, __FUNCTION__, __LINE__ );

	/*Not External IP*/
	insideLocalIpAddr = ntohl( inet_addr("192.168.1.1") );
	ret = rtl8651_addNaptConnection( TRUE, TCP_ESTABLISHED_FLOW, insideLocalIpAddr, insideLocalPort, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 remotePublicIpAddr, remotePublicPort );
	IS_EQUAL_INT( "addNaptConnection() should fail", ret, TBLDRV_EINVEXTIP, __FUNCTION__, __LINE__ );

	/*invalid port number*/
	insideGlobalIpAddr = ntohl( inet_addr("140.113.214.102") );
	remotePublicPort = 0;
	ret = rtl8651_addNaptConnection( TRUE, TCP_ESTABLISHED_FLOW, insideLocalIpAddr, insideLocalPort, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 remotePublicIpAddr, remotePublicPort );
	IS_EQUAL_INT( "addNaptConnection() should fail", ret, TBLDRV_EINVL4PORTNUM, __FUNCTION__, __LINE__ );

	remotePublicPort = 1234;
	/*insert correct connection (Hash1)*/
	insideGlobalIpAddr = ntohl( inet_addr("140.113.214.102") );
	ret = rtl8651_addNaptConnection( FALSE, TCP_ESTABLISHED_FLOW, insideLocalIpAddr, insideLocalPort, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 remotePublicIpAddr, remotePublicPort );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/*insert duplicate connection*/
	ret = rtl8651_addNaptConnection( FALSE, TCP_ESTABLISHED_FLOW, insideLocalIpAddr, insideLocalPort, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 remotePublicIpAddr, remotePublicPort );
	IS_EQUAL_INT( "addNaptConnection() should fail", ret, TBLDRV_EDRVNAPTEXIST, __FUNCTION__, __LINE__ );
	
	/*insert duplicate connection, but different internal IP:port.*/
	ret = rtl8651_addNaptConnection( FALSE, TCP_ESTABLISHED_FLOW, ntohl( inet_addr("192.168.1.100") ), 2543, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 remotePublicIpAddr, remotePublicPort );
	IS_EQUAL_INT( "addNaptConnection() fail", ret, SUCCESS, __FUNCTION__, __LINE__ );
	
	/*insert duplicate intIp/intPort, but remoteIp:remotePort is 0.0.0.0:0. */
	ret = rtl8651_addNaptConnection( FALSE, TCP_ESTABLISHED_FLOW, insideLocalIpAddr, insideLocalPort, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 0, 0 );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, FAILED, __FUNCTION__, __LINE__ );/*UPNP is not supported in 865xC*/

	/*remove it for Hash2 testing*/
	ret = rtl8651_delNaptConnection( TRUE, insideLocalIpAddr, insideLocalPort, 
	                                 remotePublicIpAddr, remotePublicPort );
	IS_EQUAL_INT( "delNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/*remove it for Hash2 testing*/
	ret = rtl8651_delNaptConnection( TRUE, ntohl( inet_addr("192.168.1.100") ), 2543,
	                                 remotePublicIpAddr, remotePublicPort );
	IS_EQUAL_INT( "delNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	
	/*insert correct connection (Hash2)*/
	insideGlobalPort = 80;
	ret = rtl8651_addNaptConnection( TRUE, TCP_ESTABLISHED_FLOW, insideLocalIpAddr, insideLocalPort, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 0, 0 );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/*insert duplicate connection*/
	ret = rtl8651_addNaptConnection( TRUE, TCP_ESTABLISHED_FLOW, insideLocalIpAddr, insideLocalPort, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 0, 0 );
	IS_EQUAL_INT( "addNaptConnection() should fail", ret, TBLDRV_EDRVNAPTEXIST, __FUNCTION__, __LINE__ );

{
	uint16 insideGlobalPort2 = 78;
	ret = rtl8651_addNaptConnection( TRUE, TCP_ESTABLISHED_FLOW, insideLocalIpAddr, insideLocalPort, 
	                                 &insideGlobalIpAddr, &insideGlobalPort2, 
	                                 0, 0 );
	IS_EQUAL_INT( "addNaptConnection() fail", ret, SUCCESS, __FUNCTION__, __LINE__ );
}
	
	/*insert duplicate connection, but different internal IP:Port.*/
	ret = rtl8651_addNaptConnection( TRUE, TCP_ESTABLISHED_FLOW, ntohl( inet_addr( "192.168.1.200" ) ), 3678, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 0, 0 );
	IS_EQUAL_INT( "addNaptConnection() should fail", ret, TBLDRV_EINVALIDEXTPORT, __FUNCTION__, __LINE__ );
	
	/*insert duplicate connection, but remoteIp:remotePort is NOT 0.0.0.0:0.*/
	ret = rtl8651_addNaptConnection( TRUE, TCP_ESTABLISHED_FLOW, insideLocalIpAddr, insideLocalPort, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 remotePublicIpAddr, remotePublicPort );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/*insert duplicate gport, but different internal IP:Port ( remote is the same as above ).*/
	ret = rtl8651_addNaptConnection( TRUE, TCP_ESTABLISHED_FLOW, ntohl( inet_addr("192.168.1.1") ), 9999, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 remotePublicIpAddr, remotePublicPort );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, TBLDRV_EINVALIDEXTPORT, __FUNCTION__, __LINE__ );

	/*insert duplicate gport, but different internal IP:Port ( remote is the same as above ).*/
	ret = rtl8651_addNaptConnection( TRUE, TCP_ESTABLISHED_FLOW, ntohl( inet_addr("192.168.1.1") ), 9999, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, 
	                                 ntohl( inet_addr("140.113.17.5") ), 53 );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/*remove it*/
	ret = rtl8651_delNaptConnection( TRUE, insideLocalIpAddr, insideLocalPort, 
	                                 0, 0 );
	IS_EQUAL_INT( "delNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/*remove it*/
	ret = rtl8651_delNaptConnection( TRUE, insideLocalIpAddr, insideLocalPort, 
	                                 remotePublicIpAddr, remotePublicPort );
	IS_EQUAL_INT( "delNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/*remove it*/
	ret = rtl8651_delNaptConnection( TRUE, ntohl( inet_addr("192.168.1.1") ), 9999, 
	                                 ntohl( inet_addr("140.113.17.5") ), 53 );
	IS_EQUAL_INT( "delNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	return SUCCESS;
}


/*
@func int32		| NaptTypicalCase	| NAPT typical case
@rvalue SUCCESS	| test OK
@comm 
This is the typical case of 8651: Hash 1 hardware accelerated.
Add a new connection and two entries would be write to ASIC.
*/
int32 NaptTypicalCase(uint32 caseNo) 
{
	rtl865x_tblAsicDrv_naptTcpUdpParam_t Entry0, Entry1;
	uint16 outIdx, inIdx;
	uint32 intIp = ntohl( inet_addr("192.168.1.1") );
	uint16 intPort = 1025;
	uint32 remIp = ntohl( inet_addr("140.113.1.1") );
	uint16 remPort = 80;
	ipaddr_t extIp; /*out*/
	uint16 extPort;     /*out*/
	int32 ret;

	outIdx = rtl8651_naptTcpUdpTableIndex(1, intIp, intPort, remIp, remPort);
	
	START_TEST_CASE();
	ConfigNaptTestEnvironment();

	ret = rtl8651_addNaptConnection( FALSE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW,
	                                 intIp, intPort, 
	                                 &extIp, &extPort,
	                                 remIp, remPort );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "insideGlobalIpAddr should be assigned properly.", extIp, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "insideGlobalPort should be assigned properly.", extPort & (RTL8651_TCPUDPTBL_SIZE-1), outIdx, __FUNCTION__, __LINE__ );

	/* Expected TCPUDP Entry */
	Entry1.insideLocalIpAddr	= intIp;
	Entry1.insideLocalPort		= intPort;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
	Entry1.tcpFlag				= 3;
	Entry1.offset				= extPort >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( extIp, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= extPort & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0;

	/* Read TCPUDP Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( outIdx, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

	/*chk inbound entry...*/
	Entry1.tcpFlag				= 2;
	Entry1.offset 				= extPort & TCPUDPTBL_INBOUND_PORTMSK;
	Entry1.selExtIPIdx 			= (extPort >> TCPUDPTBL_INBOUND_PORTOFFSET) & TCPUDPTBL_INBOUND_PORTOFFSETMSK;
	Entry1.selEIdx				= rtl8651_naptTcpUdpTableIndex(1, remIp, remPort, 0, 0);
	
	inIdx = rtl8651_naptTcpUdpTableIndex(1, remIp, remPort, NAPT_INTF_RTK1, extPort);
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( inIdx, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

	return SUCCESS;
}


//
// This is the typical case : Hash 1 hardware accelerated.
// Additionally, we enable 4-way hash for Hash 1.
// Expect: 4 connections that conllide to the same index will group together.
//         [69] Conn1
//         [70] Conn2
//         [71] Conn3
//         [68] Conn4
//
int32 NaptTypicalCaseWith4Way(uint32 caseNo) 
{
	rtl865x_tblAsicDrv_naptTcpUdpParam_t Entry0, Entry1;
	uint32 intIp = ntohl( inet_addr("192.168.1.4") );
	uint16 intPort = 1058;
	uint32 remIp = ntohl( inet_addr("140.113.1.1") );
	uint16 remPort = 80;
	ipaddr_t extIp; // out
	uint16 extPort[4];     // out
	int32 ret, i;
	int Ports[4] = { 1058, 2081, 3104, 4135 }; // Their idx1 is 69.
	uint16 idx1 = rtl8651_naptTcpUdpTableIndex( 1, intIp, intPort, 
	                                               remIp, remPort );

	START_TEST_CASE();
	ConfigNaptTestEnvironment();
	rtl8651_enable4WayHash( TRUE );

	// Add 4 connections
	for( i = 0; i < 4; i++ )
	{
		intPort = Ports[i];
		ret = rtl8651_addNaptConnection( FALSE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW,
		                                 intIp, intPort, 
		                                 &extIp, &extPort[i],
		                                 remIp, remPort );
		IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT( "insideGlobalIpAddr should be assigned properly.", extIp, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT( "insideGlobalPort should be assigned properly.", extPort[i] & (RTL8651_TCPUDPTBL_SIZE-1), WRAPIN_4WAY( idx1, i ), __FUNCTION__, __LINE__ );
	}

	// Expected connections.
	for( i = 0; i < 4; i++ )
	{
		intPort = Ports[i];
		
		/* Expected TCPUDP Entry */
		Entry1.insideLocalIpAddr	= intIp;
		Entry1.insideLocalPort		= intPort;
		Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
		Entry1.tcpFlag				= 3;
		Entry1.offset				= extPort[i] >> RTL8651_TCPUDPTBL_BITS;
		Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( extIp, 0, IP_ATTR_NAPT ) );
		Entry1.selEIdx				= extPort[i] & (RTL8651_TCPUDPTBL_SIZE-1);
		Entry1.isTcp				= TRUE;
		Entry1.isCollision			= 1;
		Entry1.isCollision2			= 1;
		Entry1.isStatic				= 1;
		Entry1.isValid				= 1;
		Entry1.isDedicated			= 0;

		/* Read TCPUDP Entry */
		memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
		ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1, &Entry0 ) == SUCCESS);
		TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
		
		idx1 = WRAPIN_4WAY( idx1, 1 );
	}

	// test getOutboundNaptFlow()
	for( i = 0; i < 4; i++ )
	{
		struct rtl8651_tblDrv_naptTcpUdpFlowEntry_s Entry;
		struct rtl8651_tblDrv_naptTcpUdpFlowEntry_s *pEntry = &Entry;
		intPort = Ports[i];
		ret = _rtl8651_getOutboundNaptFlow( TRUE, intIp, intPort,
		                                          remIp, remPort,
		                                          &pEntry );
		IS_EQUAL_INT( "getOutboundNaptFlow() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT( "insideLocalPort should be the same.", intPort, pEntry->insideLocalPort, __FUNCTION__, __LINE__ );
	}
	
	// test rtl8651_delNaptConnection()
	for( i = 0; i < 4; i++ )
	{
		intPort = Ports[i];
		ret = rtl8651_delNaptConnection( TRUE, intIp, intPort,
		                                       remIp, remPort );
		IS_EQUAL_INT( "delNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	}

	// disable 4-way hash
	rtl8651_enable4WayHash( FALSE );
	
	return SUCCESS;
}


//
// This is function is used to test the L04 branch of addNaptConnection() function.
// At first, this function add l4NaptFindEmptyAsicRetry + 1 connections.
//
int32 NaptTestL04(uint32 caseNo) 
{
	rtl865x_tblAsicDrv_naptTcpUdpParam_t Entry0, Entry1;
	uint32 IntIp = ntohl( inet_addr("192.168.1.1") );
	uint16 IntPort = 1028;
	uint32 RemIp = ntohl( inet_addr("140.113.1.1") );
	uint16 RemPort = 80;
	ipaddr_t ExtIp; // out
	uint16 ExtPort;     // out
	int32 ret, i;
	uint16 idx1;

	START_TEST_CASE();
	ConfigNaptTestEnvironment();

	// we assume the 1024 connections will occupy the whole 1024 ASIC entries.
	for( i = 0; i < 8/*rtl8651GlobalControl.l4NaptFindEmptyAsicRetry*/; i++, IntPort++ )
	{
		idx1 = rtl8651_naptTcpUdpTableIndex( TRUE, IntIp, IntPort, 
		                                            RemIp, RemPort );

		ret = rtl8651_addNaptConnection( FALSE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW,
		                                 IntIp, IntPort, 
		                                 &ExtIp, &ExtPort,
		                                 RemIp, RemPort );
		IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT( "insideGlobalIpAddr should be assigned properly.", ExtIp, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT( "insideGlobalPort should be assigned properly.", ExtPort& (RTL8651_TCPUDPTBL_SIZE-1), idx1, __FUNCTION__, __LINE__ );

		/* Expected TCPUDP Entry */
		Entry1.insideLocalIpAddr	= IntIp;
		Entry1.insideLocalPort		= IntPort;
		Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout) );
		Entry1.tcpFlag				= 3;
		Entry1.offset				= ExtPort >> RTL8651_TCPUDPTBL_BITS;
		Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( ExtIp, 0, IP_ATTR_NAPT ) );
		Entry1.selEIdx				= ExtPort & (RTL8651_TCPUDPTBL_SIZE-1);
		Entry1.isTcp				= TRUE;
		Entry1.isCollision			= 1;
		Entry1.isCollision2			= 1;
		Entry1.isStatic				= 1;
		Entry1.isValid				= 1;
		Entry1.isDedicated			= 0;

		/* Read TCPUDP Entry */
		memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
		ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1, &Entry0 ) == SUCCESS);
		TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
	}

	// Since all ASIC entries are full, the last line will be appended to candidate list.
	idx1 = rtl8651_naptTcpUdpTableIndex( 1, IntIp, IntPort, 
	                                        RemIp, RemPort );

	ret = rtl8651_addNaptConnection( FALSE, TCP_ESTABLISHED_FLOW,
	                                 IntIp, IntPort, 
	                                 &ExtIp, &ExtPort,
	                                 RemIp, RemPort );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "insideGlobalIpAddr should be assigned properly.", ExtIp, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "insideGlobalPort should be assigned properly.", ExtPort& (RTL8651_TCPUDPTBL_SIZE-1), idx1, __FUNCTION__, __LINE__ );

	return SUCCESS;
}


//
// This is function is used to test the L04 branch of addNaptConnection() function.
// For CDRouter, we dont write HASH1 and HASH2 UDP connection tp ASIC.
// This case is designed for this prupose.
// At first, this function add l4NaptFindEmptyAsicRetry + 1 connections.
//
int32 NaptTestL04UdpForCDRouter(uint32 caseNo) 
{
	rtl865x_tblAsicDrv_naptTcpUdpParam_t Entry0, Entry1;
	uint32 IntIp = ntohl( inet_addr("192.168.1.1") );
	uint16 IntPort = 1028;
	uint32 RemIp = ntohl( inet_addr("140.113.1.1") );
	uint16 RemPort = 80;
	ipaddr_t ExtIp; // out
	uint16 ExtPort;     // out
	int32 ret, i;
	uint16 idx1;

	START_TEST_CASE();
	ConfigNaptTestEnvironment();

	// we assume the 1024 connections will occupy the whole 1024 ASIC entries.
	for( i = 0; i < 8; i++, IntPort++ )
	{
		idx1 = rtl8651_naptTcpUdpTableIndex( FALSE, IntIp, IntPort, 
		                                            RemIp, RemPort );

		ret = rtl8651_addNaptConnection( FALSE, UDP_FLOW,
		                                 IntIp, IntPort, 
		                                 &ExtIp, &ExtPort,
		                                 RemIp, RemPort );
		IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT( "insideGlobalIpAddr should be assigned properly.", ExtIp, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT( "insideGlobalPort should be assigned properly.", ExtPort& (RTL8651_TCPUDPTBL_SIZE-1), idx1, __FUNCTION__, __LINE__ );

		/* Expected TCPUDP Entry */
		memset((int8 *)&Entry1, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
		Entry1.isCollision			= 1;
		Entry1.isCollision2			= 1;

		/* Read TCPUDP Entry */
		memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
		ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1, &Entry0 ) == SUCCESS);
		TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
	}

	// Since all ASIC entries are full, the last line will be appended to candidate list.
	idx1 = rtl8651_naptTcpUdpTableIndex( FALSE, IntIp, IntPort, 
	                                            RemIp, RemPort );

	ret = rtl8651_addNaptConnection( FALSE, UDP_FLOW,
	                                 IntIp, IntPort, 
	                                 &ExtIp, &ExtPort,
	                                 RemIp, RemPort );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "insideGlobalIpAddr should be assigned properly.", ExtIp, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "insideGlobalPort should be assigned properly.", ExtPort& (RTL8651_TCPUDPTBL_SIZE-1), idx1, __FUNCTION__, __LINE__ );

	return SUCCESS;
}


//
// This is the case for 4-way hash1.
// Louis wonder if the 4-way hash1 is OK.
// We expect these 4 connections use the same group of ASIC entry. 
//
int32 testNapt4WayHash(uint32 caseNo) 
{
	rtl865x_tblAsicDrv_naptTcpUdpParam_t Entry0, Entry1;
	int32 ret;
	uint32 intIp = ntohl( inet_addr("192.168.1.1") );
	uint16 intPort[4] = { 1025, 2050, 3075, 4100 };
	ipaddr_t extIp;
	uint16 extPort[4];
	ipaddr_t remIp = ntohl( inet_addr("140.113.1.1") );
	uint16 remPort = 80;
	struct rtl8651_tblDrv_naptTcpUdpFlowEntry_s *pEntry;
	uint16 idx1 = rtl8651_naptTcpUdpTableIndex( 1, intIp, intPort[0], 
	                                               remIp, remPort );

	START_TEST_CASE();
	ConfigNaptTestEnvironment();
	rtl8651_enable4WayHash( TRUE );

	//
	// Conn1
	//
	ret = rtl8651_addNaptConnection( FALSE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, intIp, intPort[0], 
	                                 &extIp, &extPort[0], remIp, remPort );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp should be assigned properly.", extIp, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort should be assigned properly.", extPort[0]&(RTL8651_TCPUDPTBL_SIZE-1), idx1, __FUNCTION__, __LINE__ );

	/* Expected TCPUDP Entry (Conn1) */
	Entry1.insideLocalIpAddr	= intIp;
	Entry1.insideLocalPort		= intPort[0];
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
	Entry1.tcpFlag				= 3;
	Entry1.offset				= extPort[0] >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( extIp, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= extPort[0] & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0; /* HASH1 */
	/* Read TCPUDP Entry (Conn1) */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

	ASSERT(_rtl8651_getOutboundNaptFlow( TRUE, intIp, intPort[0], remIp, remPort, &pEntry )==SUCCESS);
	IS_EQUAL_INT( "intIp is not the same", pEntry->insideLocalIpAddr, intIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "intPort is not the same", pEntry->insideLocalPort, intPort[0], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp is not the same", pEntry->insideGlobalIpAddr, extIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort is not the same", pEntry->insideGlobalPort, extPort[0], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remIp is not the 0", pEntry->dstIpAddr, remIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remPort is not the 0", pEntry->dstPort, remPort, __FUNCTION__, __LINE__ );

	ASSERT(_rtl8651_getInboundNaptFlow( TRUE, extIp, extPort[0], remIp, remPort, &pEntry )==SUCCESS);
	IS_EQUAL_INT( "intIp is not the same", pEntry->insideLocalIpAddr, intIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "intPort is not the same", pEntry->insideLocalPort, intPort[0], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp is not the same", pEntry->insideGlobalIpAddr, extIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort is not the same", pEntry->insideGlobalPort, extPort[0], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remIp is not the 0", pEntry->dstIpAddr, remIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remPort is not the 0", pEntry->dstPort, remPort, __FUNCTION__, __LINE__ );


	//
	// Conn12
	//
	ret = rtl8651_addNaptConnection( FALSE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, intIp, intPort[1], 
	                                 &extIp, &extPort[1], remIp, remPort );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp should be assigned properly.", extIp, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort should be assigned properly.", extPort[1]&(RTL8651_TCPUDPTBL_SIZE-1), idx1+1, __FUNCTION__, __LINE__ );

	/* Expected TCPUDP Entry (Conn1) */
	Entry1.insideLocalIpAddr	= intIp;
	Entry1.insideLocalPort		= intPort[1];
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
	Entry1.tcpFlag				= 3;
	Entry1.offset				= extPort[1] >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( extIp, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= extPort[1] & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0; /* HASH1 */
	/* Read TCPUDP Entry (Conn1) */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1+1, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

	ASSERT(_rtl8651_getOutboundNaptFlow( TRUE, intIp, intPort[1], remIp, remPort, &pEntry )==SUCCESS);
	IS_EQUAL_INT( "intIp is not the same", pEntry->insideLocalIpAddr, intIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "intPort is not the same", pEntry->insideLocalPort, intPort[1], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp is not the same", pEntry->insideGlobalIpAddr, extIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort is not the same", pEntry->insideGlobalPort, extPort[1], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remIp is not the 0", pEntry->dstIpAddr, remIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remPort is not the 0", pEntry->dstPort, remPort, __FUNCTION__, __LINE__ );

	ASSERT(_rtl8651_getInboundNaptFlow( TRUE, extIp, extPort[1], remIp, remPort, &pEntry )==SUCCESS);
	IS_EQUAL_INT( "intIp is not the same", pEntry->insideLocalIpAddr, intIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "intPort is not the same", pEntry->insideLocalPort, intPort[1], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp is not the same", pEntry->insideGlobalIpAddr, extIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort is not the same", pEntry->insideGlobalPort, extPort[1], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remIp is not the 0", pEntry->dstIpAddr, remIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remPort is not the 0", pEntry->dstPort, remPort, __FUNCTION__, __LINE__ );


	//
	// Conn13
	//
	ret = rtl8651_addNaptConnection( FALSE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, intIp, intPort[2], 
	                                 &extIp, &extPort[2], remIp, remPort );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp should be assigned properly.", extIp, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort should be assigned properly.", extPort[2]&(RTL8651_TCPUDPTBL_SIZE-1), idx1+2, __FUNCTION__, __LINE__ );

	/* Expected TCPUDP Entry (Conn1) */
	Entry1.insideLocalIpAddr	= intIp;
	Entry1.insideLocalPort		= intPort[2];
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
	Entry1.tcpFlag				= 3;
	Entry1.offset				= extPort[2] >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( extIp, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= extPort[2] & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0; /* HASH1 */
	/* Read TCPUDP Entry (Conn1) */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1+2, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

	ASSERT(_rtl8651_getOutboundNaptFlow( TRUE, intIp, intPort[2], remIp, remPort, &pEntry )==SUCCESS);
	IS_EQUAL_INT( "intIp is not the same", pEntry->insideLocalIpAddr, intIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "intPort is not the same", pEntry->insideLocalPort, intPort[2], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp is not the same", pEntry->insideGlobalIpAddr, extIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort is not the same", pEntry->insideGlobalPort, extPort[2], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remIp is not the 0", pEntry->dstIpAddr, remIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remPort is not the 0", pEntry->dstPort, remPort, __FUNCTION__, __LINE__ );

	ASSERT(_rtl8651_getInboundNaptFlow( TRUE, extIp, extPort[2], remIp, remPort, &pEntry )==SUCCESS);
	IS_EQUAL_INT( "intIp is not the same", pEntry->insideLocalIpAddr, intIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "intPort is not the same", pEntry->insideLocalPort, intPort[2], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp is not the same", pEntry->insideGlobalIpAddr, extIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort is not the same", pEntry->insideGlobalPort, extPort[2], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remIp is not the 0", pEntry->dstIpAddr, remIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remPort is not the 0", pEntry->dstPort, remPort, __FUNCTION__, __LINE__ );


	//
	// Conn14
	//
	ret = rtl8651_addNaptConnection( FALSE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, intIp, intPort[3], 
	                                 &extIp, &extPort[3], remIp, remPort );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp should be assigned properly.", extIp, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort should be assigned properly.", extPort[3]&(RTL8651_TCPUDPTBL_SIZE-1), idx1+3, __FUNCTION__, __LINE__ );

	/* Expected TCPUDP Entry (Conn1) */
	Entry1.insideLocalIpAddr	= intIp;
	Entry1.insideLocalPort		= intPort[3];
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
	Entry1.tcpFlag				= 3;
	Entry1.offset				= extPort[3] >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( extIp, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= extPort[3] & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0; /* HASH1 */
	/* Read TCPUDP Entry (Conn1) */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1+3, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

	ASSERT(_rtl8651_getOutboundNaptFlow( TRUE, intIp, intPort[3], remIp, remPort, &pEntry )==SUCCESS);
	IS_EQUAL_INT( "intIp is not the same", pEntry->insideLocalIpAddr, intIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "intPort is not the same", pEntry->insideLocalPort, intPort[3], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp is not the same", pEntry->insideGlobalIpAddr, extIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort is not the same", pEntry->insideGlobalPort, extPort[3], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remIp is not the 0", pEntry->dstIpAddr, remIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remPort is not the 0", pEntry->dstPort, remPort, __FUNCTION__, __LINE__ );

	ASSERT(_rtl8651_getInboundNaptFlow( TRUE, extIp, extPort[3], remIp, remPort, &pEntry )==SUCCESS);
	IS_EQUAL_INT( "intIp is not the same", pEntry->insideLocalIpAddr, intIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "intPort is not the same", pEntry->insideLocalPort, intPort[3], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp is not the same", pEntry->insideGlobalIpAddr, extIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort is not the same", pEntry->insideGlobalPort, extPort[3], __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remIp is not the 0", pEntry->dstIpAddr, remIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remPort is not the 0", pEntry->dstPort, remPort, __FUNCTION__, __LINE__ );

	rtl8651_enable4WayHash( FALSE );

	return SUCCESS;
}


//
// Add a DMZ connection, and the gport is equal to idx1. (L01)
// Expect: [916] is a Hash1 conneciton.
//
int32 DMZHostUsingHash1(uint32 caseNo) 
{
	rtl865x_tblAsicDrv_naptTcpUdpParam_t Entry0, Entry1;
	int32 ret;
	uint32 insideLocalIpAddr = ntohl( inet_addr("192.168.1.1") );
	uint16 insideLocalPort = 1025;
	uint32 remoteGlobalIpAddr = ntohl( inet_addr("140.113.1.1") );
	uint16 remoteGlobalPort = 80;
	ipaddr_t insideGlobalIpAddr; // out
	uint16 insideGlobalPort;     // out
	uint16 idx1 = rtl8651_naptTcpUdpTableIndex( 1, insideLocalIpAddr, insideLocalPort, 
	                                               remoteGlobalIpAddr, remoteGlobalPort );

	START_TEST_CASE();
	ConfigNaptTestEnvironment();

	insideGlobalIpAddr = ntohl( inet_addr("140.113.214.102") );
	insideGlobalPort = 916;
	ret = rtl8651_addNaptConnection( TRUE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, insideLocalIpAddr, insideLocalPort, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, remoteGlobalIpAddr, remoteGlobalPort );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "insideGlobalIpAddr should be assigned properly.", insideGlobalIpAddr, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "insideGlobalPort should be assigned properly.", insideGlobalPort & (RTL8651_TCPUDPTBL_SIZE-1), idx1, __FUNCTION__, __LINE__ );

	/* Expected TCPUDP Entry */
	Entry1.insideLocalIpAddr	= insideLocalIpAddr;
	Entry1.insideLocalPort		= insideLocalPort;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
	Entry1.tcpFlag				= 3;
	Entry1.offset				= insideGlobalPort >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( insideGlobalIpAddr, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= insideGlobalPort & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0;

	/* Read TCPUDP Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

{
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// test for port re-used
	uint32 remoteIpAddr = ntohl( inet_addr( "140.113.17.5" ) );
	uint16 remotePort = 1177;
	ret = rtl8651_addNaptConnection( TRUE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, insideLocalIpAddr, insideLocalPort, 
	                                 &insideGlobalIpAddr, &insideGlobalPort, remoteIpAddr, remotePort );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	
	/* Read TCPUDP Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
}

	return SUCCESS;
}


//
// This is the case of Server Port.
// The user specified the (isTCP,intIP,intPort,gPort), and the idx2 is equal to gport;
// Therefore, we can use ASIC to accelerate. (L05)
// Expect: [644] is a Hash2 connection, shared by Conn1 and Conn2 (the same internal IP and port).
//
int32 ServerPortUsingOneHash2Entry(uint32 caseNo) 
{
	rtl865x_tblAsicDrv_naptTcpUdpParam_t Entry0, Entry1;
	int32 ret;
	uint32 IntIp1 = ntohl( inet_addr("192.168.1.1") );
	uint16 IntPort1 = 2048+77;
	uint32 ExtIp1 = ntohl( inet_addr("140.113.214.102") );
	uint16 ExtPort1 = 2048+644;
	uint16 idx12 = rtl8651_naptTcpUdpTableIndex( 1, IntIp1, IntPort1,
	                                               0, 0 );

	START_TEST_CASE();
	ConfigNaptTestEnvironment();

	//// Conn1 ///////////////////////////////////////////////////////////////////////////////////////////
	ret = rtl8651_addNaptConnection( TRUE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, IntIp1, IntPort1, 
	                                 &ExtIp1, &ExtPort1, 0, 0 );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/* Expected In-bound Entry */
	Entry1.insideLocalIpAddr	= IntIp1;
	Entry1.insideLocalPort		= IntPort1;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 0 | 0; // quiet=@; dir=0; outbound=0;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 0 | 0; // quiet=1; dir=0; outbound=0;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= ExtPort1 >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( ExtIp1, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= idx12;
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 1;

	/* Read TCPUDP Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx12, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

{
	//// Conn2 ///////////////////////////////////////////////////////////////////////////////////////////
	// test for port re-used
	uint32 IntIp2 = ntohl( inet_addr("192.168.1.1") );
	uint16 IntPort2 = 2048+77;
	uint32 ExtIp2 = ntohl( inet_addr("140.113.214.102") );
	uint16 ExtPort2 = 2048+644;
	uint32 RemIp2 = ntohl( inet_addr( "140.113.17.5" ) );
	uint16 RemPort2 = 1177;
	uint16 idx22 = rtl8651_naptTcpUdpTableIndex( 1, IntIp1, IntPort1,
	                                                0, 0 );
	ret = rtl8651_addNaptConnection( TRUE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, IntIp2, IntPort2, 
	                                 &ExtIp2, &ExtPort2, RemIp2, RemPort2 );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	
	/* Expected In-Entry ( still is Conn1 ) */
	memset((int8 *)&Entry1, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	Entry1.insideLocalIpAddr	= IntIp1;
	Entry1.insideLocalPort		= IntPort1;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 0 | 0; // quiet=@; dir=0; outbound=0;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 0 | 0; // quiet=1; dir=0; outbound=0;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= ExtPort1 >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( ExtIp1, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= idx12;
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 1;
	/* Read TCPUDP Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( ExtPort2 & (RTL8651_TCPUDPTBL_SIZE-1), &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
	
	/* Expected Out-Entry */
	memset((int8 *)&Entry1, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	Entry1.insideLocalIpAddr	= IntIp2;
	Entry1.insideLocalPort		= IntPort2;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 0 | 0; // quiet=@; dir=0; outbound=0;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 0 | 0; // quiet=1; dir=0; outbound=0;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= ExtPort2 >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( ExtIp2, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= ExtPort2 & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 1;
	/* Read TCPUDP Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx22 & (RTL8651_TCPUDPTBL_SIZE-1), &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
}

	return SUCCESS;
}


//
// This is the case of Server Port.
// The user specified the (isTCP,intIP,intPort,gPort), but inIdx!=outIdx.
// Therefore we must place in/out entry in different entries.
// Therefore, we can use ASIC to accelerate. (L07)
// Expect: 
//         [ 73] is a Hash2 connection (Conn1). 
//         [501] is a Hash1 conenction (Conn3).
//         [306] is a Hash1 conenction (Conn4).
//
//                     in  out   intPort  ExtPort  RemPort
//         Conn1: H2(  80,  73)       80       80      ---
//         Conn2: H1( 638, 928)       80       80     1177
//         Conn3: H1( 501, 357)       80     1104     3344  
//         Conn4: H1( 141, 306)     5566       80     7788  
//
int32 ServerPortUsingTwoHash2Entries(uint32 caseNo) 
{
	rtl865x_tblAsicDrv_naptTcpUdpParam_t Entry0, Entry1;
	int32 ret;

	START_TEST_CASE();
	ConfigNaptTestEnvironment();

{
	//// Conn1 ///////////////////////////////////////////////////////////////////////////////////////////
	uint32 IntIp1 = ntohl( inet_addr("192.168.1.1") );
	uint16 IntPort1 = 80;
	ipaddr_t ExtIp1 = ntohl( inet_addr("140.113.214.102") );
	uint16 ExtPort1 = 80;
	uint16 idx12 = rtl8651_naptTcpUdpTableIndex( 1, IntIp1, IntPort1, 
	                                                0, 0 );
	                                               
	ret = rtl8651_addNaptConnection( TRUE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, IntIp1, IntPort1, 
	                                 &ExtIp1, &ExtPort1, 0, 0 );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	/* Expected In-Entry */
	Entry1.insideLocalIpAddr	= IntIp1;
	Entry1.insideLocalPort		= IntPort1;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 2 | 0; // quiet=@; dir=1; outbound=0;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 2 | 0; // quiet=1; dir=1; outbound=0;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= ExtPort1 >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( ExtIp1, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= ExtPort1 & (RTL8651_TCPUDPTBL_SIZE-1);;
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 1;
	/* Read In-Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( ExtPort1 & (RTL8651_TCPUDPTBL_SIZE-1), &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

	/* Expected Out-Entry */
	Entry1.insideLocalIpAddr	= IntIp1;
	Entry1.insideLocalPort		= IntPort1;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 2 | 1; // quiet=@; dir=1; outbound=1;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 2 | 1; // quiet=1; dir=1; outbound=1;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= ExtPort1 >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( ExtIp1, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= ExtPort1 & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 1;
	/* Read Out-Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx12, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

}
{
	//// Conn2 ///////////////////////////////////////////////////////////////////////////////////////////
	// test for port re-used
	uint32 IntIp2 = ntohl( inet_addr("192.168.1.1") );
	uint16 IntPort2 = 80;
	ipaddr_t ExtIp2 = ntohl( inet_addr("140.113.214.102") );
	uint16 ExtPort2 = 80;
	uint32 RemIp2 = ntohl( inet_addr( "140.113.17.5" ) );
	uint16 RemPort2 = 1177;
	uint16 idx2in = rtl8651_naptTcpUdpTableIndex(1, RemIp2, RemPort2, ExtIp2, ExtPort2);
	uint16 idx2uot = rtl8651_naptTcpUdpTableIndex(1, IntIp2, IntPort2, RemIp2, RemPort2);
	ret = rtl8651_addNaptConnection( TRUE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, IntIp2, IntPort2,
	                                 &ExtIp2, &ExtPort2, RemIp2, RemPort2 );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	
	/* Expected In-Entry */
	Entry1.insideLocalIpAddr	= IntIp2;
	Entry1.insideLocalPort		= IntPort2;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 2 | 0; // quiet=@; dir=1; outbound=0;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 2 | 0; // quiet=1; dir=1; outbound=0;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= ExtPort2 & TCPUDPTBL_INBOUND_PORTMSK;
	Entry1.selExtIPIdx			= (ExtPort2 >> TCPUDPTBL_INBOUND_PORTOFFSET) & TCPUDPTBL_INBOUND_PORTOFFSETMSK;
	Entry1.selEIdx				= rtl8651_naptTcpUdpTableIndex(1, RemIp2, RemPort2, 0, 0);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0;
	/* Read TCPUDP Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx2in, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
	
	/* Expected Out-Entry */
	Entry1.insideLocalIpAddr	= IntIp2;
	Entry1.insideLocalPort		= IntPort2;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 2 | 1; // quiet=@; dir=1; outbound=1;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 2 | 1; // quiet=1; dir=1; outbound=1;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= ExtPort2 >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= 0;
	Entry1.selEIdx				= ExtPort2 & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0;
	/* Read Out-Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx2uot, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
}	
{
	//// Conn3 ///////////////////////////////////////////////////////////////////////////////////////////
	uint32 IntIp3 = ntohl( inet_addr("192.168.1.1") );
	uint16 IntPort3 = 80;
	ipaddr_t ExtIp3 = ntohl( inet_addr("140.113.214.102") );
	uint16 ExtPort3 = 1104;
	uint32 RemIp3 = ntohl( inet_addr( "140.113.17.5" ) );
	uint16 RemPort3 = 3344;
	uint16 idx3in = rtl8651_naptTcpUdpTableIndex(1, RemIp3, RemPort3, ExtIp3, ExtPort3);
	uint16 idx3out = rtl8651_naptTcpUdpTableIndex( 1, IntIp3, IntPort3, RemIp3, RemPort3);
	
	ret = rtl8651_addNaptConnection( TRUE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, IntIp3, IntPort3,
	                                 &ExtIp3, &ExtPort3, RemIp3, RemPort3 );

	/* Expected In-Entry */
	Entry1.insideLocalIpAddr	= IntIp3;
	Entry1.insideLocalPort		= IntPort3;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 2 | 0; // quiet=@; dir=1; outbound=0;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 2 | 0; // quiet=1; dir=1; outbound=0;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= ExtPort3 & TCPUDPTBL_INBOUND_PORTMSK;
	Entry1.selExtIPIdx			= (ExtPort3 >> TCPUDPTBL_INBOUND_PORTOFFSET) & TCPUDPTBL_INBOUND_PORTOFFSETMSK;
	Entry1.selEIdx				= rtl8651_naptTcpUdpTableIndex(1, RemIp3, RemPort3, 0, 0);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0;
	/* Read In-Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx3in, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
	
	/* Expected Out-Entry */
	memset((int8 *)&Entry1, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	Entry1.insideLocalIpAddr	= IntIp3;
	Entry1.insideLocalPort		= IntPort3;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 2 | 1; // quiet=@; dir=1; outbound=1;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 2 | 1; // quiet=1; dir=1; outbound=1;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= ExtPort3 >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= 0;
	Entry1.selEIdx				= ExtPort3 & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0;
	/* Read Out-Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx3out, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
}	
{
	//// Conn4 ///////////////////////////////////////////////////////////////////////////////////////////
	uint32 IntIp4 = ntohl( inet_addr("192.168.1.1") );
	uint16 IntPort4 = 5566;
	ipaddr_t ExtIp4 = ntohl( inet_addr("140.113.214.102") );
	uint16 ExtPort4 = 80;
	uint32 RemIp4 = ntohl( inet_addr( "140.113.17.5" ) );
	uint16 RemPort4 = 7788;
	uint16 idx4in = rtl8651_naptTcpUdpTableIndex(1, RemIp4, RemPort4, ExtIp4, ExtPort4);
	uint16 idx4out = rtl8651_naptTcpUdpTableIndex(1, IntIp4, IntPort4, RemIp4, RemPort4);
	
	ret = rtl8651_addNaptConnection( TRUE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, IntIp4, IntPort4,
	                                 &ExtIp4, &ExtPort4, RemIp4, RemPort4 );


	/* Expected In-Entry */
	memset((int8 *)&Entry1, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	Entry1.insideLocalIpAddr	= IntIp4;
	Entry1.insideLocalPort		= IntPort4;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
	Entry1.tcpFlag				= 0 | 2 | 0; // quiet=0; dir=1; outbound=0;
	Entry1.offset				= ExtPort4 & TCPUDPTBL_INBOUND_PORTMSK;
	Entry1.selExtIPIdx			= (ExtPort4 >> TCPUDPTBL_INBOUND_PORTOFFSET) & TCPUDPTBL_INBOUND_PORTOFFSETMSK;
	Entry1.selEIdx				= rtl8651_naptTcpUdpTableIndex(1, RemIp4, RemPort4, 0, 0);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0;
	/* Read In-Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx4in, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
	
	/* Expected Out-Entry */
	memset((int8 *)&Entry1, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	Entry1.insideLocalIpAddr	= IntIp4;
	Entry1.insideLocalPort		= IntPort4;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
	Entry1.tcpFlag				= 0 | 2 | 1; // quiet=0; dir=1; outbound=1;
	Entry1.offset				= ExtPort4 >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= 0;
	Entry1.selEIdx				= ExtPort4 & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0;
	/* Read Out-Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx4out, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
}

	return SUCCESS;
}


//
// This is the case for Netmeeting ALG.
// Since server port connection is added, newly-outbound packet should match the existed server port conneciton.
// Therefore, the extIp/extPort will be the same with the server port, not HASH1 connction.
//
int32 ServerPortAndNewHash1(uint32 caseNo) 
{
	rtl865x_tblAsicDrv_naptTcpUdpParam_t Entry0, Entry1;
	int32 ret;
	uint32 intIp = ntohl( inet_addr("192.168.1.1") );
	uint16 intPort = 1025;
	ipaddr_t extIp;
	uint16 extPort;
	ipaddr_t remIp = ntohl( inet_addr("140.113.1.1") );
	uint16 remPort = 80;
	struct rtl8651_tblDrv_naptTcpUdpFlowEntry_s *pEntry;
	uint16 idx2 = rtl8651_naptTcpUdpTableIndex( 1, intIp, intPort, 
	                                               0, 0 );

	START_TEST_CASE();
	ConfigNaptTestEnvironment();

	extIp = ntohl( inet_addr("140.113.214.102") );
	extPort = 1025;
	ret = rtl8651_addNaptConnection( TRUE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, intIp, intPort, 
	                                 &extIp, &extPort, 0, 0 );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "insideGlobalIpAddr should be assigned properly.", extIp, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );

	/* Expected TCPUDP Entry (Outbound) */
	Entry1.insideLocalIpAddr	= intIp;
	Entry1.insideLocalPort		= intPort;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 2 | 1; // quiet=@; dir=1; outbound=1;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 2 | 1; // quiet=1; dir=1; outbound=1;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= extPort >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( extIp, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= extPort&(RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 1;
	/* Read TCPUDP Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx2, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

	/* Expected TCPUDP Entry (Inbound) */
	Entry1.insideLocalIpAddr	= intIp;
	Entry1.insideLocalPort		= intPort;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 2 | 0; // quiet=@; dir=1; outbound=0;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 2 | 0; // quiet=1; dir=1; outbound=0;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= extPort >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( extIp, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= extPort&(RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 1;
	/* Read TCPUDP Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( extPort&(RTL8651_TCPUDPTBL_SIZE-1), &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

	ASSERT(_rtl8651_getOutboundNaptFlow( TRUE, intIp, intPort, remIp, remPort, &pEntry )==SUCCESS);
	IS_EQUAL_INT( "intIp is not the same", pEntry->insideLocalIpAddr, intIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "intPort is not the same", pEntry->insideLocalPort, intPort, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp is not the same", pEntry->insideGlobalIpAddr, extIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort is not the same", pEntry->insideGlobalPort, extPort, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remIp is not the 0", pEntry->dstIpAddr, 0, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remPort is not the 0", pEntry->dstPort, 0, __FUNCTION__, __LINE__ );

	ASSERT(_rtl8651_getInboundNaptFlow( TRUE, extIp, extPort, remIp, remPort, &pEntry )==SUCCESS);
	IS_EQUAL_INT( "intIp is not the same", pEntry->insideLocalIpAddr, intIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "intPort is not the same", pEntry->insideLocalPort, intPort, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extIp is not the same", pEntry->insideGlobalIpAddr, extIp, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "extPort is not the same", pEntry->insideGlobalPort, extPort, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remIp is not the 0", pEntry->dstIpAddr, 0, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "remPort is not the 0", pEntry->dstPort, 0, __FUNCTION__, __LINE__ );

	return SUCCESS;
}



int32 findIdx(uint32 trgIdx) 
{
	uint32 IntIp3 = ntohl( inet_addr("192.168.1.2") );
	/*uint16 IntPort3 = 512;
	uint32 ExtIp3 = ntohl( inet_addr("140.113.214.102") );
	uint16 ExtPort3 = 372;
	uint32 RemIp3 = ntohl( inet_addr("0.0.0.0") );
	uint16 RemPort3 = 0;*/
	uint16 idx;
	int i;

	for (i = 1; i < (1 << 16) - 1; i++)
	{
		idx= rtl8651_naptTcpUdpTableIndex( 1, IntIp3, i, 0, 0 );
		if (idx == trgIdx)
			break;
	}
	return i;
}


//
//	This case is used to test the hybrid connections.
//	We will add four connections: Hash1, UPnP, Server Port, and DMZ Host.
//  They are designed to be overwriten one-by-one.
//
//	Expect:
//			[ 269] Conn1 Hash1
//                       UPnP (outbound, in candidate)
//			[ 270] Conn2 UPnP (inbound)
//			             Server Port (outbound, in candidate)
//          [ 271] Conn3 Server Port (inbound)
//                       DMZ Host (outbound, in candidate)
//          [ 272] Conn4 DMZ Host (inbound)
//
//
//                 IntPort  ExtPort  RemPort
//          Conn1     1025    o:269      571
//                            i:269
//          Conn2     1024    o:269        0
//                            i:270
//          Conn3      257    o:270        0
//                            i:271
//          Conn4     1536    o:271       80
//                            i:272
//
int32 TestUpdateNaptConnectionHybrid1(uint32 caseNo) 
{
	rtl865x_tblAsicDrv_naptTcpUdpParam_t Entry0, Entry1;
	int32 ret;
	rtl8651_tblDrv_naptTcpUdpFlowEntry_t *pEntry;
	int idxPort = findIdx(402);
	uint32 IntIp1 = ntohl( inet_addr("192.168.1.1") );
	uint16 IntPort1 = 1025;
	uint32 ExtIp1;
	uint16 ExtPort1;
	uint32 RemIp1 = ntohl( inet_addr("140.113.209.90") );
	uint16 RemPort1 = 571;
	uint16 idx1Out = rtl8651_naptTcpUdpTableIndex( 1, IntIp1, IntPort1,
	                                                RemIp1, RemPort1 );
	uint16 idx1In = rtl8651_naptTcpUdpTableIndex(1, RemIp1, RemPort1, ExtIp1, ExtPort1);

	uint32 IntIp3 = ntohl( inet_addr("192.168.1.2") );
	uint16 IntPort3 = idxPort;
	uint32 ExtIp3 = ntohl( inet_addr("140.113.214.102") );
	uint16 ExtPort3 = 372;
	uint32 RemIp3 = ntohl( inet_addr("0.0.0.0") );
	uint16 RemPort3 = 0;
	uint16 idx3Out = rtl8651_naptTcpUdpTableIndex(1, IntIp3, IntPort3, 0, 0);
	uint32 IntIp4 = ntohl( inet_addr("192.168.1.1") );
	uint16 IntPort4 = 1794;
	uint32 ExtIp4 = ntohl( inet_addr("140.113.214.102") );
	uint16 ExtPort4 = 272;
	uint32 RemIp4 = ntohl( inet_addr("140.113.209.90") );
	uint16 RemPort4 = 80;
	uint16 idx4In = rtl8651_naptTcpUdpTableIndex(1, RemIp4, RemPort4, ExtIp4, ExtPort4);
	uint16 idx4Out = rtl8651_naptTcpUdpTableIndex(1, IntIp4, IntPort4, RemIp4, RemPort4);

	START_TEST_CASE();
	ConfigNaptTestEnvironment();
	_rtl8651_naptSmartFlowSwapByPPS( TRUE, 1 );
	rtl8651_enable4WayHash( TRUE );

{
	//// Conn 1, Hash 1 //////////////////////////////////////////////////////////////////////////////////
	ret = rtl8651_addNaptConnection( FALSE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, IntIp1, IntPort1,
	                                 &ExtIp1, &ExtPort1, RemIp1, RemPort1 );
	pEntry = _rtl8651_findNaptConnection( idx1Out, TRUE, IntIp1, IntPort1, RemIp1, RemPort1 );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT( "idx1 should be equal to 322", idx1Out, 322, __FUNCTION__, __LINE__ );
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1Out, &Entry0 ) == SUCCESS);
	IS_EQUAL_INT("conn 1's tcpFlag should be equal to 3", Entry0.tcpFlag, 3, __FUNCTION__, __LINE__ );
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1In, &Entry0 ) == SUCCESS);
	IS_EQUAL_INT("conn 1's IntIp is wrong", Entry0.insideLocalIpAddr, IntIp1, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT("conn 1's isDedicated should be equal to 0", Entry0.isDedicated, 0, __FUNCTION__, __LINE__ );
	IS_EQUAL_INT("conn 1's tcpFlag should be equal to 2", Entry0.tcpFlag, 2, __FUNCTION__, __LINE__ );

	//// Conn 3, Server Port /////////////////////////////////////////////////////////////////////////////
	ret = rtl8651_addNaptConnection( TRUE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, IntIp3, IntPort3,
	                                 &ExtIp3, &ExtPort3, RemIp3, RemPort3 );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

	//// Conn 4, DMZ Host ////////////////////////////////////////////////////////////////////////////////
	ret = rtl8651_addNaptConnection( TRUE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW, IntIp4, IntPort4,
	                                 &ExtIp4, &ExtPort4, RemIp4, RemPort4 );
	IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );

 	//// Conn 4 will not win ASIC[372] ///////////////////////////////////////////////////////////////////////
	pEntry = _rtl8651_findNaptConnection( idx4Out, TRUE, IntIp4, IntPort4, RemIp4, RemPort4 );
	IS_EQUAL_POINTER( "findNaptConnection() failed", pEntry, NULL, __FUNCTION__, __LINE__ );
	pEntry = _rtl8651_findNaptConnectionByExtIpPort( TRUE, ExtIp4, ExtPort4, RemIp4, RemPort4 );
	IS_NOT_EQUAL_POINTER( "findNaptConnection() failed", pEntry, NULL, __FUNCTION__, __LINE__ );
	_rtl8651_updateNaptConnection( pEntry, TCP_ESTABLISHED_FLOW,
	                               _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) ) );

	/* Expect Conn4 will hold the ASIC entry [464] */
	Entry1.insideLocalIpAddr	= IntIp4;
	Entry1.insideLocalPort		= IntPort4;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
	Entry1.tcpFlag				= 0 | 2 | 0; // quiet=0; dir=1; outbound=1;
	Entry1.offset				= ExtPort4 & TCPUDPTBL_INBOUND_PORTMSK;
	Entry1.selExtIPIdx			= (ExtPort4 >> TCPUDPTBL_INBOUND_PORTOFFSET) & TCPUDPTBL_INBOUND_PORTOFFSETMSK;
	Entry1.selEIdx				= rtl8651_naptTcpUdpTableIndex(1, RemIp4, RemPort4, 0, 0);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0;
	/* Read Conn4 In-Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx4In & (RTL8651_TCPUDPTBL_SIZE-1), &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

 	//// Conn 3 will win ASIC[402] and [372] ////////////////////////////////////////////////////////////
	pEntry = _rtl8651_findNaptConnection( idx3Out, TRUE, IntIp3, IntPort3, RemIp3, RemPort3 );
	IS_NOT_EQUAL_POINTER( "findNaptConnection() failed", pEntry, NULL, __FUNCTION__, __LINE__ );
	_rtl8651_updateNaptConnection( pEntry, TCP_ESTABLISHED_FLOW,
	                               _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) ) );
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx3Out & (RTL8651_TCPUDPTBL_SIZE-1), &Entry0 ) == SUCCESS);

	/* Expect Conn3 will hold the outbound ASIC entry [402] */
	Entry1.insideLocalIpAddr	= IntIp3;
	Entry1.insideLocalPort		= IntPort3;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 2 | 1; // quiet=@; dir=1; outbound=1;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 2 | 1; // quiet=1; dir=1; outbound=1;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= ExtPort3 >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( ExtIp3, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= ExtPort3 & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 1;
	/* Read Conn3 Out-Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx3Out & (RTL8651_TCPUDPTBL_SIZE-1), &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );

	/* Expect Conn3 will hold the inbound ASIC entry [372] */
	Entry1.insideLocalIpAddr	= IntIp3;
	Entry1.insideLocalPort		= IntPort3;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
#ifdef ADD_FINITE_HASH2_SP
	Entry1.tcpFlag				= 0 | 2 | 0; // quiet=@; dir=1; outbound=0;
#else/*ADD_FINITE_HASH2_SP*/
	Entry1.tcpFlag				= 4 | 2 | 0; // quiet=1; dir=1; outbound=0;
#endif/*ADD_FINITE_HASH2_SP*/
	Entry1.offset				= ExtPort3 >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( ExtIp3, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= ExtPort3 & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 1;
	/* Read Conn3 In-Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( ExtPort3 & (RTL8651_TCPUDPTBL_SIZE-1), &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );


 	//// Conn 2 will win ASIC[269] and [270] ////////////////////////////////////////////////////////////
	//pEntry = _rtl8651_findNaptConnection( idx32, TRUE, IntIp2, IntPort2, RemIp2, RemPort2 );
	IS_NOT_EQUAL_POINTER( "findNaptConnection() failed", pEntry, NULL, __FUNCTION__, __LINE__ );
	_rtl8651_updateNaptConnection( pEntry, TCP_ESTABLISHED_FLOW,
	                               _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) ) );

	/* Expect Conn2 will hold the outbound ASIC entry [269] 
	Entry1.insideLocalIpAddr	= IntIp2;
	Entry1.insideLocalPort		= IntPort2;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
	Entry1.tcpFlag				= 0 | 2 | 1; // quiet=0; dir=1; outbound=1;
	Entry1.offset				= ExtPort2 >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( ExtIp2, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= ExtPort2 & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 1*/;
	/* Read Conn2 Out-Entry 
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx22 & (RTL8651_TCPUDPTBL_SIZE-1), &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 )*/;

	/* Expect Conn2 will hold the inbound ASIC entry [270] 
	Entry1.insideLocalIpAddr	= IntIp2;
	Entry1.insideLocalPort		= IntPort2;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
	Entry1.tcpFlag				= 0 | 2 | 0; // quiet=0; dir=1; outbound=1;
	Entry1.offset				= ExtPort2 >> RTL8651_TCPUDPTBL_BITS;
	Entry1.selExtIPIdx			= IP_TABLE_INDEX( _rtl8651_getIPTableEntry( ExtIp2, 0, IP_ATTR_NAPT ) );
	Entry1.selEIdx				= ExtPort2 & (RTL8651_TCPUDPTBL_SIZE-1);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 1*/;
	/* Read Conn2 In-Entry 
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( ExtPort2 & (RTL8651_TCPUDPTBL_SIZE-1), &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 )*/;


 	//// Conn 1 will win ASIC[402] ////////////////////////////////////////////////////////////////////
	pEntry = _rtl8651_findNaptConnection( idx1Out, TRUE, IntIp1, IntPort1, RemIp1, RemPort1 );
	IS_NOT_EQUAL_POINTER( "findNaptConnection() failed", pEntry, NULL, __FUNCTION__, __LINE__ );
	_rtl8651_updateNaptConnection( pEntry, TCP_ESTABLISHED_FLOW,
	                               _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) ) );

	/* Expect Conn1 will hold the ASIC entry [402] */
	Entry1.insideLocalIpAddr	= IntIp1;
	Entry1.insideLocalPort		= IntPort1;
	Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
	Entry1.tcpFlag				= 2;
	Entry1.offset				= ExtPort1 & TCPUDPTBL_INBOUND_PORTMSK;
	Entry1.selExtIPIdx			= (ExtPort1 >> TCPUDPTBL_INBOUND_PORTOFFSET) & TCPUDPTBL_INBOUND_PORTOFFSETMSK;
	Entry1.selEIdx				= rtl8651_naptTcpUdpTableIndex(1, RemIp1, RemPort1, 0, 0);
	Entry1.isTcp				= TRUE;
	Entry1.isCollision			= 1;
	Entry1.isCollision2			= 1;
	Entry1.isStatic				= 1;
	Entry1.isValid				= 1;
	Entry1.isDedicated			= 0;
	/* Read Conn1 Entry */
	memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
	ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1In, &Entry0 ) == SUCCESS);
	TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
}

	rtl8651_enable4WayHash( FALSE );
	return SUCCESS;
}


int32 NaptEnhanceHash1TypicalCase(uint32 caseNo) 
{
	rtl865x_tblAsicDrv_naptTcpUdpParam_t Entry0, Entry1;
	uint32 insideLocalIpAddr = ntohl( inet_addr("192.168.1.4") );
	uint16 insideLocalPort = 1058;
	uint32 remoteGlobalIpAddr = ntohl( inet_addr("140.113.1.1") );
	uint16 remoteGlobalPort = 80;
	ipaddr_t insideGlobalIpAddr; // out
	uint16 insideGlobalPort;     // out
	int32 ret, i;
	int Ports[4] = { 1058, 2081, 3104, 4135 }; // Their idx1 is 69.
	
	uint16 idx1 = rtl8651_naptTcpUdpTableIndex( 1, insideLocalIpAddr, insideLocalPort, remoteGlobalIpAddr, remoteGlobalPort );
	
	ConfigNaptTestEnvironment();
	rtl8651_enable4WayHash( TRUE );

	// Add 4 connections
	for( i = 0; i < 4; i++ )
	{
		insideLocalPort = Ports[i];
		ret = rtl8651_addNaptConnection( FALSE|DONT_BIRD_CDROUTER, TCP_ESTABLISHED_FLOW,
		                                 insideLocalIpAddr, insideLocalPort, 
		                                 &insideGlobalIpAddr, &insideGlobalPort,
		                                 remoteGlobalIpAddr, remoteGlobalPort );
		IS_EQUAL_INT( "addNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT( "insideGlobalIpAddr should be assigned properly.", insideGlobalIpAddr, NAPT_INTF_RTK1, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT( "insideGlobalPort should be assigned properly.", insideGlobalPort & (RTL8651_TCPUDPTBL_SIZE-1), idx1, __FUNCTION__, __LINE__ );
		insideLocalPort = Ports[i];
		
		// Expected TCPUDP Entry 
		Entry1.insideLocalIpAddr	= insideLocalIpAddr;
		Entry1.insideLocalPort		= insideLocalPort;
		Entry1.ageSec				= _rtl8651_NaptAgingToSec( _rtl8651_NaptAgingToUnit( rtl8651GlobalControl.tcpLongTimeout ) );
		Entry1.tcpFlag				= 3;
		Entry1.offset				= insideGlobalPort >> RTL8651_TCPUDPTBL_BITS;
		Entry1.selExtIPIdx			= 0;
		Entry1.selEIdx				= insideGlobalPort & (RTL8651_TCPUDPTBL_SIZE-1);
		Entry1.isTcp				= TRUE;
		Entry1.isCollision			= 1;
		Entry1.isCollision2			= 1;
		Entry1.isStatic				= 1;
		Entry1.isValid				= 1;
		Entry1.isDedicated			= 0;

		// Read TCPUDP Entry 
		memset((int8 *)&Entry0, 0, sizeof(rtl865x_tblAsicDrv_naptTcpUdpParam_t));
		ASSERT(rtl8651_getAsicNaptTcpUdpTable( idx1, &Entry0 ) == SUCCESS);
		TCPUDP_ENTRY_CMP( &Entry0, &Entry1 );
		
		idx1 = WRAPIN_4WAY( idx1, 1 );
	}

	// test getOutboundNaptFlow()
	for( i = 0; i < 4; i++ )
	{
		struct rtl8651_tblDrv_naptTcpUdpFlowEntry_s Entry;
		struct rtl8651_tblDrv_naptTcpUdpFlowEntry_s *pEntry = &Entry;
		insideLocalPort = Ports[i];
		ret = _rtl8651_getOutboundNaptFlow( TRUE, insideLocalIpAddr, insideLocalPort,
		                                          remoteGlobalIpAddr, remoteGlobalPort,
		                                          &pEntry );
		IS_EQUAL_INT( "getOutboundNaptFlow() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT( "insideLocalPort should be the same.", insideLocalPort, pEntry->insideLocalPort, __FUNCTION__, __LINE__ );
	}
	
	// test rtl8651_delNaptConnection()
	for( i = 0; i < 4; i++ )
	{
		insideLocalPort = Ports[i];
		ret = rtl8651_delNaptConnection( TRUE, insideLocalIpAddr, insideLocalPort, remoteGlobalIpAddr, remoteGlobalPort );
		IS_EQUAL_INT( "delNaptConnection() failed", ret, SUCCESS, __FUNCTION__, __LINE__ );
	}

	// disable 4-way hash
	rtl8651_enable4WayHash( FALSE );
	
	return SUCCESS;
}


#if 0
/*****************************************************************
		Logging releated functions
*****************************************************************/
#ifdef _RTL_NEW_LOGGING_MODEL
typedef struct _rtl8651_mylog_s{
	unsigned long moduleId;
	unsigned long logNo;
	rtl8651_logInfo_t info;
}_rtl8651_mylog_t;
#else
typedef struct _rtl8651_mylog_s{
	unsigned long  dsid;
	unsigned long  moduleId;
	unsigned char  proto;
	char			 direction;
	unsigned long  sip;
	unsigned long  dip;
	unsigned short sport;
	unsigned short dport;
	unsigned char  type;
	unsigned char  action;
	char			* msg;
}_rtl8651_mylog_t;
#endif
#define MAX_MYLOG_POOL_SIZE		10

static int32 Log_count;
static _rtl8651_mylog_t *Log_Current;
static _rtl8651_mylog_t Log_pool[MAX_MYLOG_POOL_SIZE];

#ifdef _RTL_NEW_LOGGING_MODEL
int32 LogFunc
(
	unsigned long	moduleId,
	unsigned long	logNo,
	rtl8651_logInfo_t	*info
)
{
	if (Log_count == MAX_MYLOG_POOL_SIZE)
	{
		rtlglue_printf("Log system full !!\n");
		return FAILED;
	}

	// log
	Log_pool[Log_count].moduleId = moduleId;
	Log_pool[Log_count].logNo = logNo;
	memcpy(&(Log_pool[Log_count].info), info, sizeof(Log_pool[Log_count].info));

	Log_count ++;

	return SUCCESS;
}
#else
int32 LogFunc
(
unsigned long  dsid,
unsigned long  moduleId,
unsigned char  proto,
char           	direction,
unsigned long  sip,
unsigned long  dip,
unsigned short sport,
unsigned short dport,
unsigned char  type,
unsigned char  action,
char			* msg
)
{
	// check pool is full or not
	if (Log_count == MAX_MYLOG_POOL_SIZE)
	{
		rtlglue_printf("Log system full !!\n");
		return FAILED;
	}

	// log
	Log_pool[Log_count].dsid		= dsid;
	Log_pool[Log_count].moduleId	= moduleId;
	Log_pool[Log_count].proto		= proto;
	Log_pool[Log_count].direction	= direction;
	Log_pool[Log_count].sip		= sip;
	Log_pool[Log_count].dip		= dip;
	Log_pool[Log_count].sport		= sport;
	Log_pool[Log_count].dport		= dport;
	Log_pool[Log_count].type		= type;
	Log_pool[Log_count].action	= action;
	Log_pool[Log_count].msg		= (char*)malloc(strlen(msg) + 1);
	strncpy(Log_pool[Log_count].msg, msg, strlen(msg));
	Log_pool[Log_count].msg[strlen(msg)] = 0;

	Log_count ++;

	return SUCCESS;
}
#endif


/*
	Compare Log
*/
#ifdef _RTL_NEW_LOGGING_MODEL
/*
			- compare logNo only
*/


static int32 log_reinit(void)
{
	// flush data in Log pool
	_rtl8651_mylog_t *ptr = Log_pool;

	while (Log_count > 0)
	{
		memset(Log_pool, 0, sizeof(_rtl8651_mylog_t));
		ptr ++;
		Log_count --;
	}

	Log_Current = Log_pool;
	return SUCCESS;
}
#else
/*
			- compare msg only
*/


static int32 log_reinit(void)
{
	// flush data in Log pool
	_rtl8651_mylog_t *ptr = Log_pool;

	while (Log_count > 0)
	{
		free(ptr->msg);
		memset(Log_pool, 0, sizeof(_rtl8651_mylog_t));

		ptr ++;
		Log_count --;
	}

	Log_Current = Log_pool;
	return SUCCESS;
}
#endif	/* _RTL_NEW_LOGGING_MODEL */






static uint16 DEFAULT_IP_ID	= 0xa;
		
static int32 value_cmp(int32 val1, int32 val2, int8 *_fun_, uint32 _line_)
{
	IS_EQUAL_INT("value not equal!", val1, val2, _fun_, _line_);
	return SUCCESS;
}



static struct rtl_mBuf *confPppoeHSB(rtl8651_PktConf_t *config, rtl8651_PktConf_t *conf_err, uint32 pktType,
	uint16 rx_port, uint8 vlanIdx, uint8 pppoeIdx, ether_addr_t smac, ether_addr_t dmac, uint16 sessionId,
	ipaddr_t sip, ipaddr_t dip, uint8 ttl, uint8 mf, uint16 offset,
	uint16 sport, uint16 dport,
	uint8 TCP_flag, uint16 mss,
	uint8 ICMP_type, uint16 ICMP_id, int8* data_ptr, uint32 data_len)
{
	rtl8651_PktConf_t pktConf, *conf = &pktConf;

	if (config)
		conf = config;

	RESET_CONF(conf);
	/***********************
		Default Value
	***********************/
	// L2
	conf->l2Flag |= L2_PPPoE;
	conf->ErrFlag = 0;
	conf->pppoe.type = 0x8864;
	conf->pppoe.session = sessionId;
	if (pktType >= _PKT_TYPE_IP)
		conf->conf_Flags |= (CSUM_IP|CSUM_L4);

	// L3
	conf->ip.id = DEFAULT_IP_ID;
	// L4
		// TCP
		conf->conf_tcp_seq = 0xb;
		conf->conf_tcp_ack = 0xc;
		conf->conf_tcp_win = 0xfffe;
		// ICMP
		conf->icmp.code = 0;
		conf->icmp.seq = 0x0;

	/***********************
		Configurable Value
	***********************/
	conf->pktType = pktType;
	SET_RX_PORT(conf->conf_portlist, rx_port);
	conf->conf_vlanIdx = vlanIdx;
	conf->conf_pppoeIdx = pppoeIdx;

	CP_MAC (smac, conf->conf_smac);
	CP_MAC (dmac, conf->conf_dmac);
	conf->conf_sip = (uint32)sip;
	conf->conf_dip = (uint32)dip;
	conf->ip.ttl = ttl;
	conf->ip.mf = mf;
	conf->ip.offset = offset;
	conf->conf_sport = sport;
	conf->conf_dport = dport;
	conf->conf_tcp_flag = TCP_flag;
	conf->conf_tcp_mss = mss;
	conf->icmp.type = ICMP_type;
	conf->icmp.id = ICMP_id;
	conf->payload.content = data_ptr;
	conf->payload.length = data_len;
	conf->conf_category = RTL8651_DEFAULT_TRAPPING_DISPATCH_CATEGORY;

	if (conf_err)
		return genIcmpErrMbufPkt(conf_err, conf);
	else
		return genMbufPkt(conf);
}


static struct rtl_mBuf *confPppoeHSA(rtl8651_PktConf_t *config, rtl8651_PktConf_t *conf_err, uint32 pktType,
	uint16* tx_port, uint8 vlanIdx, uint8 pppoeIdx, ether_addr_t smac, ether_addr_t dmac,
	ipaddr_t sip, ipaddr_t dip, uint8 ttl, uint8 mf, uint16 offset,
	uint16 sport, uint16 dport,
	uint8 TCP_flag, uint16 mss,
	uint8 ICMP_type, uint16 ICMP_id, int8* data_ptr, uint32 data_len)
{
	uint16 *tx = tx_port;
	rtl8651_PktConf_t pktConf, *conf = &pktConf;

	if (config)
		conf = config;

	RESET_CONF(conf);
	/***********************
		Default Value
	***********************/
	// L2
	conf->l2Flag = 0;
	conf->ErrFlag = 0;
	conf->conf_Flags |= (PKTHDR_PPPOE_AUTOADD);
	if (pktType >= _PKT_TYPE_IP)
		conf->conf_Flags |= (CSUM_IP_OK|CSUM_L4_OK);
	
	// L3
	conf->ip.id = DEFAULT_IP_ID;
	// L4
		// TCP
		conf->conf_tcp_seq = 0xb;
		conf->conf_tcp_ack = 0xc;
		conf->conf_tcp_win = 0xfffe;
		// ICMP
		conf->icmp.code = 0;
		conf->icmp.seq = 0x0;

	/***********************
		Configurable Value
	***********************/
	conf->pktType = pktType;
	if (tx != NULL)
		while (*tx != 0xffff)	// last one
		{
			ADD_TX_PORT(conf->conf_portlist, *tx);
			tx ++;
		}
	conf->conf_vlanIdx = vlanIdx;
	conf->conf_pppoeIdx = pppoeIdx;

	CP_MAC (smac, conf->conf_smac);
	CP_MAC (dmac, conf->conf_dmac);
	conf->conf_sip = (uint32)sip;
	conf->conf_dip = (uint32)dip;
	conf->ip.ttl = ttl;
	conf->ip.mf = mf;
	conf->ip.offset = offset;
	conf->conf_sport = sport;
	conf->conf_dport = dport;
	conf->conf_tcp_flag = TCP_flag;
	conf->conf_tcp_mss = mss;
	conf->icmp.type = ICMP_type;
	conf->icmp.id = ICMP_id;
	conf->payload.content = data_ptr;
	conf->payload.length = data_len;
	conf->conf_category = RTL8651_DEFAULT_TRAPPING_DISPATCH_CATEGORY;

 	if (conf_err)
		return genIcmpErrMbufPkt(conf_err, conf);
	else
		return genMbufPkt(conf);
}

struct rtl_mBuf *confVlanHSB(rtl8651_PktConf_t *config, rtl8651_PktConf_t*conf_err, uint32 pktType,
	uint16 rx_port, uint8 vlanIdx, uint8 pppoeIdx, ether_addr_t smac, ether_addr_t dmac,
	ipaddr_t sip, ipaddr_t dip, uint8 ttl, uint8 mf, uint16 offset,
	uint16 sport, uint16 dport,
	uint8 TCP_flag, uint16 mss,
	uint8 ICMP_type, uint16 ICMP_id, int8* data_ptr, uint32 data_len)
{
	rtl8651_PktConf_t pktConf, *conf = &pktConf;

	if (config)
		conf = config;

	RESET_CONF(conf);
	/***********************
		Default Value
	***********************/
	// L2
	conf->l2Flag = 0;
	conf->ErrFlag = 0;
	if (pktType >= _PKT_TYPE_IP)
		conf->conf_Flags |= (CSUM_IP|CSUM_L4);

	// L3
	conf->ip.id = DEFAULT_IP_ID;
	// L4
		// TCP
		conf->conf_tcp_seq = 0xb;
		conf->conf_tcp_ack = 0xc;
		conf->conf_tcp_win = 0xfffe;
		// ICMP
		conf->icmp.code = 0;
		conf->icmp.seq = 0x0;

	/***********************
		Configurable Value
	***********************/
	conf->pktType = pktType;
	SET_RX_PORT(conf->conf_portlist, rx_port);
	conf->conf_vlanIdx = vlanIdx;
	conf->conf_pppoeIdx = pppoeIdx;

	CP_MAC (smac, conf->conf_smac);
	CP_MAC (dmac, conf->conf_dmac);
	conf->conf_sip = (uint32)sip;
	conf->conf_dip = (uint32)dip;
	conf->ip.ttl = ttl;
	conf->ip.mf = mf;
	conf->ip.offset = offset;
	conf->conf_sport = sport;
	conf->conf_dport = dport;
	conf->conf_tcp_flag = TCP_flag;
	conf->conf_tcp_mss = mss;
	conf->icmp.type = ICMP_type;
	conf->icmp.id = ICMP_id;
	conf->payload.content = data_ptr;
	conf->payload.length = data_len;
	conf->conf_category = RTL8651_DEFAULT_TRAPPING_DISPATCH_CATEGORY;

	if (conf_err)
		return genIcmpErrMbufPkt(conf_err, conf);
	else
		return genMbufPkt(conf);
}

static struct rtl_mBuf *confVlanHSA(rtl8651_PktConf_t *config, rtl8651_PktConf_t *conf_err, uint32 pktType,
	uint16* tx_port, uint8 vlanIdx, uint8 pppoeIdx, ether_addr_t smac, ether_addr_t dmac,
	ipaddr_t sip, ipaddr_t dip, uint8 ttl, uint8 mf, uint16 offset,
	uint16 sport, uint16 dport,
	uint8 TCP_flag, uint16 mss,
	uint8 ICMP_type, uint16 ICMP_id, int8* data_ptr, uint32 data_len)
{
	uint16 *tx = tx_port;
	rtl8651_PktConf_t pktConf, *conf = &pktConf;

	if (config)
		conf = config;

	RESET_CONF(conf);
	/***********************
		Default Value
	***********************/
	// L2
	conf->l2Flag = 0;
	conf->ErrFlag = 0;
	if (pktType >= _PKT_TYPE_IP)
		conf->conf_Flags |= (CSUM_IP_OK|CSUM_L4_OK);
	// L3
	conf->ip.id = DEFAULT_IP_ID;
	// L4
		// TCP
		conf->conf_tcp_seq = 0xb;
		conf->conf_tcp_ack = 0xc;
		conf->conf_tcp_win = 0xfffe;
		// ICMP
		conf->icmp.code = 0;
		conf->icmp.seq = 0x0;

	/***********************
		Configurable Value
	***********************/
	conf->pktType = pktType;
	if (tx != NULL)
		while (*tx != 0xffff)	// last one
		{
			ADD_TX_PORT(conf->conf_portlist, *tx);
			tx ++;
		}
	conf->conf_vlanIdx = vlanIdx;
	conf->conf_pppoeIdx = pppoeIdx;

	CP_MAC (smac, conf->conf_smac);
	CP_MAC (dmac, conf->conf_dmac);
	conf->conf_sip = (uint32)sip;
	conf->conf_dip = (uint32)dip;
	conf->ip.ttl = ttl;
	conf->ip.mf = mf;
	conf->ip.offset = offset;
	conf->conf_sport = sport;
	conf->conf_dport = dport;
	conf->conf_tcp_flag = TCP_flag;
	conf->conf_tcp_mss = mss;
	conf->icmp.type = ICMP_type;
	conf->icmp.id = ICMP_id;
	conf->payload.content = data_ptr;
	conf->payload.length = data_len;
	conf->conf_category = RTL8651_DEFAULT_TRAPPING_DISPATCH_CATEGORY;

	if (conf_err)
		return genIcmpErrMbufPkt(conf_err, conf);
	else
		return genMbufPkt(conf);
}

static int32 log_init(uint32 dsid, uint32 moduleId){
	int32 ret;

	// reinit rtl8651 log system
	rtl8651_log_init();

	if ((ret = rtl8651_installLoggingFunction(&LogFunc)) != SUCCESS)
		return ret;

	if ((ret = rtl8651a_enableLogging(dsid, moduleId, 1)) != SUCCESS)
		return ret;

	return log_reinit();
}


static int32 fwdeng_function_configuration1(void){

	rtl8651_fwdEngineArp(1);	
	rtl8651_fwdEngineIcmp(1);
	rtl8651_fwdEngineIcmpRoutingMsg(1);
	rtl8651_fwdEngineDMZHostIcmpPassThrough(1);
	rtl8651_fwdEngineProcessL34(1);
	rtl8651_fwdEngineProcessIPFragment(1);
	rtl8651_fwdEngineProcessIPMulticast(1);
	rtl8651_fwdEngineArpProxy(1);
	rtl8651_dosProc_blockSip_enable(0, 0);		// turn-OFF sip block
	rtl8651_fwdEngineProcessPortBouncing(1);	// turn-ON port bouncing
	return SUCCESS;
}


/****************************************************************
		Configuration functions
*****************************************************************/
static int32 fwdeng_env_configuration_1(void){
	/******************************
		vlan 1: pppoe/WAN
				IP/mask		:	10.113.1.1 / 32
				session id	:	11111

		vlan 2: vlan/LAN
				IP/mask		:	192.168.1.254 / 24

		vlan 3: vlan/LAN
				IP/mask		:	192.168.2.254 / 24

		vlan 4: vlan/WAN
				IP/mask		:	10.113.2.1 / 24

	******************************/
	ether_addr_t mac = { { 0x00, 0x03, 0x04, 0x05, 0x06, 0x01 } };
	ipaddr_t ipaddr_ext, ipaddr_int, ipaddr_next, mask;

	/* VLAN 1: subnet 1 */
	/* member port 0, pppoe, pppoeid 1000, session id 11111, mtu 1492 */
	ASSERT(rtl8651_assignVlanMacAddress(1, &mac, 1) == SUCCESS);
	ASSERT(rtl8651_addNetworkIntf("pppoe") == SUCCESS);
	ASSERT(rtl8651_specifyNetworkIntfLinkLayerType("pppoe", RTL8651_LL_PPPOE, 1) == SUCCESS);
	ASSERT(rtl8651_addPppoeSession(1000, 1) == SUCCESS);
	ASSERT(rtl8651_addExtNetworkInterface("pppoe") == SUCCESS);
	ASSERT(rtl8651_addExternalMulticastPort(0) == SUCCESS);
	ipaddr_ext = ntohl(inet_addr("10.113.1.1"));
	mask = ntohl(inet_addr("255.255.255.255"));
	mac.octet[5] = 0x02;
	ASSERT(rtl8651_addIpIntf("pppoe", ipaddr_ext, mask) == SUCCESS);
	ASSERT(rtl8651_bindPppoeSession(ipaddr_ext, 1000) == SUCCESS);
	ASSERT(rtl8651_setPppoeSessionProperty(1000, 11111, &mac, 0x00) == SUCCESS);
	ASSERT(rtl8651_setNetMtu("pppoe", 1492) == SUCCESS);

	/* VLAN 2: subnet 2 */
	/* member port 1,2,3, vlan */
	ASSERT(rtl8651_addVlan(2) == SUCCESS);
	ASSERT(rtl8651_delVlanPortMember(1, 1) == SUCCESS);
	ASSERT(rtl8651_addVlanPortMember(2, 1) == SUCCESS);
	ASSERT(rtl8651_delVlanPortMember(1, 2) == SUCCESS);
	ASSERT(rtl8651_addVlanPortMember(2, 2) == SUCCESS);
	ASSERT(rtl8651_delVlanPortMember(1, 3) == SUCCESS);
	ASSERT(rtl8651_addVlanPortMember(2, 3) == SUCCESS);
	mac.octet[5] = 0x03;
	ASSERT(rtl8651_assignVlanMacAddress(2, &mac, 1) == SUCCESS);
	ASSERT(rtl8651_addNetworkIntf("lan") == SUCCESS);
	ASSERT(rtl8651_specifyNetworkIntfLinkLayerType("lan", RTL8651_LL_VLAN, 2) == SUCCESS);
	ipaddr_int = ntohl(inet_addr("192.168.1.254"));
	mask = ntohl(inet_addr("255.255.255.0"));
	ASSERT(rtl8651_addIpIntf("lan", ipaddr_int, mask) == SUCCESS);

	/* VLAN 3: subnet 3 */
	/* member port 4, vlan */
	ASSERT(rtl8651_addVlan(3) == SUCCESS);
	ASSERT(rtl8651_delVlanPortMember(1, 4) == SUCCESS);
	ASSERT(rtl8651_addVlanPortMember(3, 4) == SUCCESS);
	mac.octet[5] = 0x04;
	ASSERT(rtl8651_assignVlanMacAddress(3, &mac, 1) == SUCCESS);
	ASSERT(rtl8651_addNetworkIntf("lan2") == SUCCESS);
	ASSERT(rtl8651_specifyNetworkIntfLinkLayerType("lan2", RTL8651_LL_VLAN, 3) == SUCCESS);
	ipaddr_int = ntohl(inet_addr("192.168.2.254"));
	mask = ntohl(inet_addr("255.255.255.0"));
	ASSERT(rtl8651_addIpIntf("lan2", ipaddr_int, mask) == SUCCESS);

	/* VLAN 4: subnet 4 */
	/* member port 5, vlan */
	ASSERT(rtl8651_addVlan(4) == SUCCESS);
	ASSERT(rtl8651_delVlanPortMember(1, 5) == SUCCESS);
	ASSERT(rtl8651_addVlanPortMember(4, 5) == SUCCESS);
	mac.octet[5] = 0x05;
	ASSERT(rtl8651_assignVlanMacAddress(4, &mac, 1) == SUCCESS);
	ASSERT(rtl8651_addNetworkIntf("wan2") == SUCCESS);
	ASSERT(rtl8651_specifyNetworkIntfLinkLayerType("wan2", RTL8651_LL_VLAN, 4) == SUCCESS);
	ASSERT(rtl8651_addExtNetworkInterface("wan2") == SUCCESS);
	ASSERT(rtl8651_addExternalMulticastPort(5) == SUCCESS);
	ipaddr_ext = ntohl(inet_addr("10.113.2.1"));
	mask = ntohl(inet_addr("255.255.255.0"));
	ASSERT(rtl8651_addIpIntf("wan2", ipaddr_ext, mask) == SUCCESS);

	/* add default route on pppoe interface */
	ASSERT(rtl8651_addRoute(0, 0, "pppoe", 0) == SUCCESS);
	ipaddr_ext = ntohl(inet_addr("140.115.0.0"));
	mask = ntohl(inet_addr("255.255.0.0"));
	ipaddr_next = ntohl(inet_addr("10.113.2.254"));
	ASSERT(rtl8651_addRoute(ipaddr_ext, mask, "wan2", ipaddr_next) == SUCCESS);

	/* flush all DoS setting and turn-on dos check for session 0 */
	rtl8651_dosOneSecondTimer();
	rtl8651a_setDosStatus(0, 0xffffffff);
	log_init(0, RTL8651_LOGMODULE_DOS);	// enable DOS logging
	mbufList_Reinit();

	// disable multicast Query broadcasting
	{
		rtl8651_tblDrvMCastPara_t para;

		para.MCastUpstream_Timeout	= 0xffffffff;
		para.MCastMember_ExpireTime	= 0xffffffff;
		para.igmp_sqi					= 0xffffffff;
		para.igmp_sqc				= 0;	// disable startup query
		para.igmp_response_tolerance_delay	= 0xffffffff;
		para.igmp_qri					= 0xffffffff;
		para.igmp_qi					= 0xffffffff;
		para.igmp_oqpi				= 0xffffffff;
		para.igmp_lmqi				= 0xffffffff;
		para.igmp_lmqc				= 0;
		para.igmp_group_query_interval	= 0xffffffff;
		ASSERT(rtl8651_multicastSetPara(&para) == SUCCESS);
		rtl8651_multicastPeriodicQuery(FALSE);		// disable periodically query
	}
	return SUCCESS;
}


int32 PacketCase(uint32 caseNo)
{
	int8 pkt_content[2048];
	rtl8651_PktConf_t	conf;
	int32 retval;
	struct rtl_mBuf* mbuf_in, *mbuf_out, *mbuf_comp;

	/**********************************************
		NAPT test
		1 wan(pppoe), 1 lan

	***********************************************/
	/*
		Config Environments
	*/
	ipaddr_t ip_npi = ntohl(inet_addr("192.168.1.1"));
	uint16 port_npi = 2210;
	ether_addr_t mac_npi = {{0x00, 0x00, 0x00, 0x01, 0x02, 0x03}};
	ipaddr_t ip_rp = ntohl(inet_addr("140.113.1.1"));
//	ipaddr_t ip_rp2 = ntohl(inet_addr("140.113.17.5"));
	uint16 port_rp = 1234;
/*	uint16 port_rp1 = 4321;
	uint16 port_rp2 = 5678;*/
//	ether_addr_t mac_rp = {{0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0c}};
	ipaddr_t ip_gw_ext = ntohl(inet_addr("10.113.1.1"));
	uint16 port_gw;
/*	uint16 port_gw2;
	ipaddr_t ip_gw_int = ntohl(inet_addr("192.168.1.254"));*/
	ether_addr_t mac_gw_ext = {{0x00, 0x03, 0x04, 0x05, 0x06, 0x01}};
	ether_addr_t mac_pppoe = {{0x00, 0x03, 0x04, 0x05, 0x06, 0x02}};
	ether_addr_t mac_gw_int = {{0x00, 0x03, 0x04, 0x05, 0x06, 0x03}};
	//ether_addr_t mac_bcast = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
	bzero(pkt_content, sizeof(pkt_content));

	START_TEST_CASE();
	fwdeng_env_configuration_1();
	ASSERT(rtl8651_addNaptMapping(ip_gw_ext) == SUCCESS);
	ASSERT(rtl8651_addArp(ip_npi, &mac_npi, "lan", 1) == SUCCESS);
	fwdeng_function_configuration1();

	rtl8651_setNatType( NAT_TYPE_SYMMETRIC );


{
	/* config */
	/* NPI->RP */
	RESET_CONF(&conf);
	conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
	conf.l2Flag						= 0;									/* no VLAN/PPPoE/SNAP */
	conf.ErrFlag						= 0;
	conf.conf_Flags					|= (CSUM_IP_OK|CSUM_L4_OK);
	CP_MAC (mac_npi, conf.conf_smac);
	CP_MAC (mac_gw_int, conf.conf_dmac);
	conf.conf_sip						= ip_npi;
	conf.conf_dip						= ip_rp;
	conf.ip.id							= 0xab;
	conf.ip.mf						= 0;
	conf.ip.offset						= 0;
	conf.ip.ttl							= 123;
	conf.conf_sport					= port_npi;
	conf.conf_dport					= port_rp;
	conf.payload.content				= pkt_content;
	conf.payload.length				= 10;

	conf.conf_vlanIdx					= 2;
	SET_RX_PORT(conf.conf_portlist, 1);

	/* packet input */
	mbufList_Reinit();	// re-init mbufList
	mbuf_in = genMbufPkt(&conf);
	retval = rtl8651_fwdEngineInput((void*)mbuf_in->m_pkthdr);

	/* check result */
	VALUE_CMP(retval, SUCCESS);
	VALUE_CMP(mbufList_nmbuf(), 1);
	mbuf_out = mbufList_dequeue();

	// get external port
	ASSERT(get_mbuf_IpPort(mbuf_out, NULL, &port_gw, NULL, NULL)==SUCCESS);

	/* check output packet */
	/* NPI->RP (after NAPT) */
	RESET_CONF(&conf);
	conf.pktType						= _PKT_TYPE_UDP;					/* udp packet */
	conf.l2Flag						= 0;									/* no VLAN/PPPoE/SNAP */
	conf.ErrFlag						= 0;
	conf.conf_Flags					|= (PKTHDR_PPPOE_AUTOADD|CSUM_IP|CSUM_L4);
	CP_MAC(mac_gw_ext, conf.conf_smac);
	CP_MAC(mac_pppoe, conf.conf_dmac);
	conf.conf_sip						= ip_gw_ext;
	conf.conf_dip						= ip_rp;
	conf.ip.id							= 0xab;
	conf.ip.mf						= 0;
	conf.ip.offset						= 0;
	conf.ip.ttl							= 122;
	conf.conf_sport					= port_gw;
	conf.conf_dport					= port_rp;
	conf.payload.content				= pkt_content;
	conf.payload.length				= 10;
	conf.conf_vlanIdx					= 1;
	ADD_TX_PORT(conf.conf_portlist, 0);

	mbuf_comp = genMbufPkt(&conf);
	MBUF_CMP(mbuf_out, mbuf_comp);

	freeMbuf(mbuf_out);
	freeMbuf(mbuf_comp);
}



	return SUCCESS;
}
#endif
