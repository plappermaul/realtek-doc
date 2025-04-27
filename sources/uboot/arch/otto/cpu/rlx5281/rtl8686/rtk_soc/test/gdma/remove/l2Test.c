/*
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
*
* Program : Model Code for 865xC
* Abstract :
* Author : (rupert@realtek.com.tw)
* $Id: l2Test.c,v 1.58 2006-11-27 15:37:48 yjlou Exp $
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
#include "crc32.h"
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
#define  WAIT_ON_COMPLETE {}
#define  ETHERTYPE_II	0x0700 /* Must higher than 0x600*/
#define  LLC_OTHER		0xFFFF
#define  RFC_1042		0x1122
#define  RANDOM_RUN_NUM 10000
#define  IPX_ETHERTYPE	0xE0E0
#define  PPPOECONTROL_ETHERTYPE 0x8863
#define  PPPOESESSION_ETHERTYPE 0x8864
#define VERBOSE 1

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *  Shared Packet Memory Space for test cases.
 *
 *	Most of test cases need a memory block to store packet.
 *  The common solution is defining every packet in local variable.
 *  However, it will cause crash because Linux kernel only have 4K-bytes stack for kernel thread.
 *  An alternative way is calling rtlglue_malloc() in each test case.
 *  It is not suit for ASSERT() macro, which returns function without free allocated memory.
 *
 *  Therefore, we reserve a block of memory for packet buffer.
 *  All test case needs to do is 'assign the pointer to this buffer'.
 *
 */
uint8 sharedPkt[3][_PKT_LEN];

void  AllPortForward(int state)
{
	int i,offset;
	for(i=0; i<RTL865XC_PORT_NUMBER; i++) 
	{
		offset=i*4;
		switch(state)
		{
			case RTL8651_PORTSTA_DISABLED:
				WRITE_MEM32(PCRP0+offset, ( READ_MEM32(PCRP0+offset)&(~0x0e0)) );
				break;
			case RTL8651_PORTSTA_BLOCKING:
			case RTL8651_PORTSTA_LISTENING:
				WRITE_MEM32(PCRP0+offset, ( READ_MEM32(PCRP0+offset)&(~0x060)) | (0x1<<5) );
				break;
			case RTL8651_PORTSTA_LEARNING:
				WRITE_MEM32(PCRP0+offset, ( READ_MEM32(PCRP0+offset)&(~0x060)) | (0x2<<5) );
				break;
			case RTL8651_PORTSTA_FORWARDING:
				WRITE_MEM32(PCRP0+offset, ( READ_MEM32(PCRP0+offset)&(~0x060)) | (0x3<<5) );
				break;

		};		
	}
}
int32  testLayer2TableReadWrite(uint32 caseNo)
{
	rtl865x_tblAsicDrv_l2Param_t l2t;
	#define TEST_MAC	"00-00-00-00-01-01"
	rtl8651_clearAsicAllTable();
	rtl8651_clearRegister();
	rtl8651_setAsicOperationLayer(2);
	bzero((void*) &l2t, sizeof(l2t));
	strtomac(&l2t.macAddr, TEST_MAC);
	l2t.memberPortMask = (0x1);
	l2t.isStatic=1;
	if (rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_MAC), 0, &l2t))
		rtlglue_printf("FAILED Set Layer2 Table\n");
	return 0;

}

int32 testLayer2UniLearning(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 0,		len: 64,
		vid: 0,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 2,		iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch: 0,		ethtype: 0x7788,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0xff,0xff,0x00,0x00,0x00},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,		l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,		urlmch: 0,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i,j;
	_rtl8651_tblDrvAclRule_t rule;
	rtl865x_tblAsicDrv_intfParam_t intf;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	ether_addr_t  mac;
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		rtl8651_setAsicOperationLayer(4);
		layer4Config();
		WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_51B_CPU_REASON);
		/* Enable ingress filtering. */
		WRITE_MEM32( VCR0,READ_MEM32(VCR0) | EN_ALL_PORT_VLAN_INGRESS_FILTER);	
		/* Set PVID of all port to VLAN 8. */
		for (j=0;j<5;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		rtl8651_totalExtPortNum=3;
		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rule.pktOpApp    = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule( 0, &rule);

		/* Config VLAN 8 (vidx:0) */
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = TRUE;
		intf.inAclEnd = 0;
		intf.inAclStart = 0;
		intf.outAclEnd = 0;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );

		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_EXT0|PM_PORT_EXT2;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );

		hsb.spa = 0;	
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-00-FF-FF-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"FF-FF-FF-FF-FF-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		hsb.spa=0x2;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"FF-FF-FF-FF-FF-FF"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-00-00-FF-FF-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );          
		IS_EQUAL_INT_DETAIL("Pakcet should be learned ", hsa.dp,PM_PORT_0, __FUNCTION__, __LINE__,i );



		hsb.spa = 0x7;	
		hsb.extspa=1;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-FF-FF-00-00-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-FF-FF-FF-FF-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		hsb.spa=0x2;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-FF-FF-FF-FF-FF"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-FF-FF-00-00-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );          
		IS_EQUAL_INT_DETAIL("Pakcet should be learned ", hsa.dpext,PM_PORT_EXT1>>(RTL8651_MAC_NUMBER+1), __FUNCTION__, __LINE__,i );
		
	}


	retval = SUCCESS;	
	return retval;
}
int32 testLayer2FIDLearning(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 0,		len: 64,
		vid: 0x0234,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,		iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch: 0,		ethtype: 0x7788,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00, 0x80,0xc0,0x00, 0x00,0x00},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,		l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,		urlmch: 1,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	uint32 i,vlanid;
	_rtl8651_tblDrvAclRule_t rule;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
	
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		rtl8651_setAsicOperationLayer(2);
		/* Enable ingress filtering. */
		WRITE_MEM32( VCR0,READ_MEM32(VCR0) | EN_ALL_PORT_VLAN_INGRESS_FILTER);	
		rtl8651_clearAsicPvid();
		rtl8651_setAsicPvid(0, 8);
		rtl8651_setAsicPvid(1, 8);
		rtl8651_setAsicPvid(2, 8);
		rtl8651_setAsicPvid(3, 9);
		rtl8651_setAsicPvid(4, 9);
		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rtl8651_setAsicAclRule( 0, &rule);

		/* Config VLAN 8 (vidx:0) */
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 3;
		intf.inAclStart = 0;
		intf.outAclEnd = 4;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );

		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.untagPortMask = 0;
		vlan.memberPortMask = 0x3;
		vlan.fid=0;
		rtl8651_getAsicPvid(3,&vlanid);
		rtl8651_setAsicVlan( intf.vid, &vlan );   

		/* Config VLAN 9  */
		bzero(&intf, sizeof(intf));
		intf.vid = 9;
		intf.inAclEnd = 3;
		intf.inAclStart = 0;
		intf.outAclEnd = 8;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = 0x1c;
		vlan.fid=1;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );   
  	        
		hsb.spa=0x1;
		hsb.sa[0]=0;			hsb.sa[1]=0x80;
		hsb.sa[2]=0xc0;		hsb.sa[3]=0x10;	
		hsb.sa[4]=0x10;		hsb.sa[5]=0;	
		hsb.da[0]=0xff;		hsb.da[1]=0xff;
		hsb.da[2]=0xff;		hsb.da[3]=0xff;
		hsb.da[4]=0xff;		hsb.da[5]=0xff;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		hsb.spa=0x3;
		hsb.sa[0]=0;			hsb.sa[1]=0x80;
		hsb.sa[2]=0xc0;		hsb.sa[3]=0x10;
		hsb.sa[4]=0x10;		hsb.sa[5]=0;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		hsb.spa=0x4;
		hsb.sa[0]=0;			hsb.sa[1]=0x80;
		hsb.sa[2]=0xc0;		hsb.sa[3]=0x11;
		hsb.sa[4]=0x10;		hsb.sa[5]=0;			
		hsb.da[0]=0;			hsb.da[1]=0x80;
		hsb.da[2]=0xc0;		hsb.da[3]=0x10;
		hsb.da[4]=0x10;		hsb.da[5]=0;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to port 3 ", hsa.dp,(1<<3), __FUNCTION__, __LINE__,i );
		hsb.spa=0x0;
		hsb.sa[0]=0;			hsb.sa[1]=0x80;
		hsb.sa[2]=0xc1;		hsb.sa[3]=0x11;
		hsb.sa[4]=0x10;		hsb.sa[5]=0;			
		hsb.da[0]=0;			hsb.da[1]=0x80;
		hsb.da[2]=0xc0;		hsb.da[3]=0x10;
		hsb.da[4]=0x10;		hsb.da[5]=0;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward to port 1 ", hsa.dp,PM_PORT_1, __FUNCTION__, __LINE__ ,i);


			
	}
	
	retval = SUCCESS;	
	return retval;
}
int32 testLayer2UniLRULearning(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch:0,		ethtype: 0x7788,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00, 0x80,0xc0,0x00, 0x00,0x00},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch: 0,
		extspa: 0,		extl2: 0,
	};
	hsa_param_t hsa;
	int i,j;
	_rtl8651_tblDrvAclRule_t rule;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	ether_addr_t mac;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();		
		TESTING_MODE;
		rtl8651_setAsicOperationLayer(2);
		/* Enable ingress filtering. */
		WRITE_MEM32( VCR0,READ_MEM32(VCR0) | EN_ALL_PORT_VLAN_INGRESS_FILTER);	
		WRITE_MEM32(FFCR,READ_MEM32(FFCR)&~EN_UNUNICAST_TOCPU);
		WRITE_MEM32( TEACR,READ_MEM32(TEACR)&~EN_L2LRUHASH);		
		/* Set PVID of all port to VLAN 8. */
		for (j=0;j<3;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rtl8651_setAsicAclRule( 0, &rule);
		/* Config VLAN 8 */
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 0;
		intf.inAclStart = 0;
		intf.outAclEnd = 0;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;

		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = 0x3f;
		rtl8651_setAsicVlan( intf.vid, &vlan );	
		
		hsb.spa=0x0;	
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-80-C0-00-00-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"FF-FF-FF-FF-FF-FF"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		hsb.spa=0x1;			
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-80-C0-10-10-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );			
		ASSERT( retval == SUCCESS );

		hsb.spa=0x1;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-80-C0-11-11-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );			
		ASSERT( retval == SUCCESS );
		
		hsb.spa=0x1;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-80-C0-14-14-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );			
		ASSERT( retval == SUCCESS );
		
		hsb.spa=0x2;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-80-C0-18-18-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );						
		ASSERT( retval == SUCCESS );

		hsb.spa=0x1;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"02-80-C0-18-18-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-80-C0-18-18-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );						
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be broadcast,but ....", hsa.dp,0x3d, __FUNCTION__, __LINE__ ,i);
		WRITE_MEM32( TEACR,READ_MEM32(TEACR)|EN_L2LRUHASH);		
		hsb.spa=0x2;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-80-C0-18-18-00"),sizeof(ether_addr_t));		
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );						
		ASSERT( retval == SUCCESS );
		hsb.spa=0x1;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"02-80-C0-18-18-00"),sizeof(ether_addr_t));		
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-80-C0-18-18-00"),sizeof(ether_addr_t));
		hsb.sa[0]=0x02;		hsb.sa[1]=0x80;
		hsb.da[0]=0x00;		hsb.da[1]=0x80;
		hsb.da[2]=0xc0;		hsb.da[3]=0x18;
		hsb.da[4]=0x18;	       hsb.da[5]=0x00;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );						
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be learn,but ....", hsa.dp,0x4, __FUNCTION__, __LINE__,i );

		
	}
	retval = SUCCESS;	
	return retval;
}
int32 testLayer2UnwareVlan(uint32 caseNo)
{
	int32 retval;	
	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x09,		tagif: 1,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch:0,		ethtype: 0x7788,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00, 0x80,0xc0,0x00, 0x00,0x00},
		hiprior: 0,		snap: 1,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	_rtl8651_tblDrvAclRule_t rule;
	ether_addr_t mac;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();		
		TESTING_MODE;
		/* Enable ingress filtering.  1q unware*/
		WRITE_MEM32( VCR0,READ_MEM32(VCR0)| EN_ALL_PORT_VLAN_INGRESS_FILTER);	
		/* Set PVID of all port to VLAN 8. */
		for (j=0;j<3;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rtl8651_setAsicAclRule( 0, &rule);
		rtl8651_setAsicOperationLayer(2);
		/* Config VLAN 8 */
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 0;
		intf.inAclStart = 0;
		intf.outAclEnd = 0;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		
		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );	
		
		/**********************************************************************
		TAG Packet :0x9,
		ignore TAG 
		Expect  ignore TAG  and forward
 		**********************************************************************/
		hsb.spa=0x0;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-80-C0-00-00-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"FF-FF-FF-FF-FF-FF"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be  drop ....", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		TAG Packet :0x9,
		ignore TAG 
		Expect VID  8 and forward
 		**********************************************************************/
		WRITE_MEM32( VCR0,READ_MEM32(VCR0)| EN_ALL_PORT_VLAN_INGRESS_FILTER|EN_1QTAGVIDIGNORE);	
		hsb.spa=0x1;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-80-C0-18-18-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-80-C0-00-00-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );			
		ASSERT( retval == SUCCESS );		
		IS_EQUAL_INT_DETAIL("Pakcet should be  forward ....", hsa.dp,PM_PORT_0|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		TAG Packet :0x9,
		ignore TAG 
		Expect VID  8 and forward
 		**********************************************************************/
		WRITE_MEM32(VCR0,0);	
		hsb.spa=0x6;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-80-C0-18-18-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-80-C0-00-00-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be  forward ....", hsa.dp,0, __FUNCTION__, __LINE__ ,i);

		WRITE_MEM32(VCR0,0);	
		hsb.spa=0x7;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-80-C0-18-18-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-80-C0-00-00-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );				
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be  forward ....", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
		

			
	}	
	retval=SUCCESS;
	return retval;
}
int32 testLayer2VlanNoExist(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x09,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch: 0,		ethtype: 0x7788,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00, 0x80,0xc0,0x00, 0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j;
	_rtl8651_tblDrvAclRule_t rule;	

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		rtl8651_setAsicOperationLayer(2);
		/* Enable ingress filtering. */
		WRITE_MEM32(SWTCR0,READ_MEM32(SWTCR0)|EnUkVIDtoCPU);
		/* Set PVID of all port to VLAN 8. */
		for (j=0;j<3;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rtl8651_setAsicAclRule( 0, &rule);
		
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT("Pakcet should be to cpu, but ...", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ );




		/**********************************************************************		
		Expect To CPU because vlan non exist before vlan ingress filter 
 		**********************************************************************/
		WRITE_MEM32( VCR0,READ_MEM32(VCR0) | EN_ALL_PORT_VLAN_INGRESS_FILTER);	
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		ASSERT( retval == SUCCESS );			
		IS_EQUAL_INT("Pakcet should be to cpu, but ...", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ );

		
	}	
	retval=SUCCESS;
	return retval;

}
int32 testLayer2UnkonwUniCast(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x08,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch: 0,		ethtype: 0x7788,
		da:{0x00,0xff,0xff,0xff,0xff,0xff},
		sa:{0x20, 0x80,0xc0,0x00, 0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j;
	_rtl8651_tblDrvAclRule_t rule;	
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		rtl8651_setAsicOperationLayer(2);
		/* Enable ingress filtering. */
		WRITE_MEM32( VCR0,READ_MEM32(VCR0) | EN_ALL_PORT_VLAN_INGRESS_FILTER);	
		WRITE_MEM32(FFCR,READ_MEM32(FFCR)|EN_UNUNICAST_TOCPU);
		/* Set PVID of all port to VLAN 8. */
		for (j=0;j<3;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rtl8651_setAsicAclRule( 0, &rule);
		/* Config VLAN 8 */
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 0;
		intf.inAclStart = 0;
		intf.outAclEnd = 0;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );		

		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_4;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  to to PM_PORT_1|PM_PORT_2|PM_PORT_4, but ...", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_4|PM_PORT_NOTPHY, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("Pakcet should be forward CPU ...", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		ONLY VLAN Member
		Unknown Unicast
 		**********************************************************************/
		WRITE_MEM32(FFCR,READ_MEM32(FFCR)&~EN_UNUNICAST_TOCPU);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  to to PM_PORT_1|PM_PORT_2|PM_PORT_4, but ...", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_4, __FUNCTION__, __LINE__ ,i);
		
	}	
	retval=SUCCESS;
	return retval;	
}
int32 testLayer2Broadcast(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x08,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch: 0,		ethtype: 0x7788,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x20, 0x80,0xc0,0x00, 0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i;
	rtl865x_tblAsicDrv_vlanParam_t vlan;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		rtl8651_setAsicOperationLayer(3);
		WRITE_MEM32(MSCR,READ_MEM32(MSCR)&~(EN_IN_ACL|EN_OUT_ACL));
		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan(0, &vlan );	
		/**********************************************************************
		ONLY VLAN Member
		Unknown Broadcast
 		**********************************************************************/
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be only to to porlist XX,but.", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__ ,i);				
		/**********************************************************************
		include cpu port
		Unknown Broadcast
 		**********************************************************************/
		WRITE_MEM32(FFCR,READ_MEM32(FFCR)|EN_UNMCAST_TOCPU);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet will  be  forward to cpu ...", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_NOTPHY, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("Pakcet will be forward to cpu.", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************		
		Unknown Broadcast
		forward only PM_PORT_0
 		**********************************************************************/
		WRITE_MEM32(MGFCR_E0R0, 0xFFFFFFFF);
		WRITE_MEM32(MGFCR_E0R1, 0x0FFFFFFF);
		WRITE_MEM32(MGFCR_E0R2, 0x04);
		WRITE_MEM32(FFCR,READ_MEM32(FFCR)|EN_UNMCAST_TOCPU);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet will  be  forward to cpu ...", hsa.dp,PM_PORT_2, __FUNCTION__, __LINE__ ,i);

		/**********************************************************************		
		Unknown Broadcast
		forward only PM_PORT_0 | PM_PORT_1
 		**********************************************************************/
		WRITE_MEM32(MGFCR_E0R0, 0xFFFFFFFF);
		WRITE_MEM32(MGFCR_E0R1, 0x0FFFFFFF);
		WRITE_MEM32(MGFCR_E0R2, 0x06);
		WRITE_MEM32(MGFCR_E0R0, 0xFFFFFFFF);
		WRITE_MEM32(MGFCR_E1R1, 0x0FFFFFFF);
		WRITE_MEM32(MGFCR_E2R2, 0x06);

		WRITE_MEM32(FFCR,READ_MEM32(FFCR)|EN_UNMCAST_TOCPU);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet will  be  forward to cpu ...", hsa.dp,PM_PORT_1|PM_PORT_2, __FUNCTION__, __LINE__ ,i);

		
	}	
	retval=SUCCESS;
	return retval;	
}
int32 testLayer2UnKnownMACForward(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 124,
		vid: 0x00,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_IP,
		patmatch:0,		ethtype: 0x0800,
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
	int i,j;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	ether_addr_t  mac;	
		
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_51B_CPU_REASON);
		for (j=0;j<4;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );		
		rtl8651_setAsicOperationLayer(2);
		WRITE_MEM32(MSCR,READ_MEM32(MSCR)&~(EN_IN_ACL|EN_OUT_ACL));
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 1;
		intf.inAclStart = 0;
		intf.outAclEnd = 1;
		intf.outAclStart = 1;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.memberPortMask |=(PM_PORT_EXT0 |PM_PORT_EXT1|PM_PORT_EXT2)>>1;
		vlan.untagPortMask = 0;
		rtl8651_totalExtPortNum=3;
		rtl8651_setAsicVlan( intf.vid, &vlan );	
		/**********************************************************************
		Expect:  forward one port	
		**********************************************************************/
		hsb.spa=0;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-FF-FF-00-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-FF-FF-FF-FF-FF"),sizeof(ether_addr_t));
		WRITE_MEM32(MGFCR_E0R0, 0xFFFFFFFF);
		WRITE_MEM32(MGFCR_E0R1, 0x0FFF00FF);
		WRITE_MEM32(MGFCR_E0R2, 0x8);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  only forward ....", hsa.dp,PM_PORT_3, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		Expect:  forward one port	
		**********************************************************************/
		hsb.spa=0;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-FF-FF-00-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-01-FF-FF-FF-FF"),sizeof(ether_addr_t));
		WRITE_MEM32(MGFCR_E0R0, 0xFFFFFFFF);
		WRITE_MEM32(MGFCR_E0R1, 0x0FFF0001);
		WRITE_MEM32(MGFCR_E0R2, 0x02);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  odrop....", hsa.dp,PM_PORT_1, __FUNCTION__, __LINE__ ,i);	


		/**********************************************************************
		source physical filter
		Expect:  forward one port	
		**********************************************************************/
		hsb.spa=0;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-FF-FF-00-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-01-FF-FF-FF-FF"),sizeof(ether_addr_t));
		WRITE_MEM32(MGFCR_E0R0, 0xFFFFFFFF);
		WRITE_MEM32(MGFCR_E0R1, 0x0FFF0001);
		WRITE_MEM32(MGFCR_E0R2, 0x01);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  odrop....", hsa.dp,0, __FUNCTION__, __LINE__ ,i);	



		hsb.spa=0;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-FF-FF-00-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-01-FF-FF-FF-FF"),sizeof(ether_addr_t));
		WRITE_MEM32(MGFCR_E0R0, 0xFFFFFFFF);
		WRITE_MEM32(MGFCR_E0R1, 0x0FFF0001);
		WRITE_MEM32(MGFCR_E0R2, 0x02);
		WRITE_MEM32(MGFCR_E1R0, 0xFFFFFFFF);
		WRITE_MEM32(MGFCR_E1R1, 0x0FFF0001);
		WRITE_MEM32(MGFCR_E1R2, 0x03);
		WRITE_MEM32(MGFCR_E2R0, 0xFFFFFFFF);
		WRITE_MEM32(MGFCR_E2R1, 0x0FFF0001);
		WRITE_MEM32(MGFCR_E2R2, 0x01);
		
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  drop....", hsa.dp,PM_PORT_1, __FUNCTION__, __LINE__ ,i);	

		
	}
	retval=SUCCESS;
	return retval;

}


int32 testLayer2Multicast(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x08,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch: 0,		ethtype: 0x7788,
		da:{0x01,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00, 0x80,0xc0,0x00, 0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j;
	_rtl8651_tblDrvAclRule_t rule;	
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();		
		TESTING_MODE;
		rtl8651_setAsicOperationLayer(2);
		/* Enable ingress filtering. */
		WRITE_MEM32( VCR0, EN_ALL_PORT_VLAN_INGRESS_FILTER);	
				/* Set PVID of all port to VLAN 8. */
		for (j=0;j<3;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rtl8651_setAsicAclRule( 0, &rule);
		/* Config VLAN 8 */
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 0;
		intf.inAclStart = 0;
		intf.outAclEnd = 0;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );

		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );	
		/**********************************************************************
		ONLY VLAN Member
		Unknown Multicast
 		**********************************************************************/
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		IS_EQUAL_INT_DETAIL("Pakcet should be only to to porlist XX,but.", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__ ,i);		
		/**********************************************************************
		include CPU Port
		Unknown Multicast
 		**********************************************************************/
		WRITE_MEM32(FFCR,READ_MEM32(FFCR)|EN_UNMCAST_TOCPU);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet will be forward to cpu ...", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_NOTPHY, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("Pakcet will be forward to cpu.", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__ ,i);
		WRITE_MEM32(MGFCR_E0R0, 0xFFFFFFFF);
		WRITE_MEM32(MGFCR_E0R1, 0x0FFF00FF);
		WRITE_MEM32(MGFCR_E0R2, 0x01);

		
	}	
	retval=SUCCESS;
	
	return retval;
}
int32 testLayer28021XPortBase(uint32 caseNo)
{
	int32 retval;

	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x08,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch: 0,		ethtype: 0x7788,
		da:{0x01,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00, 0x80,0xc0,0x00, 0x00,0x00},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j;
	_rtl8651_tblDrvAclRule_t rule;	
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	ether_addr_t mac;
	rtl865x_tblAsicDrv_l2Param_t l2tsrc,l2tdst;
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();		
		TESTING_MODE;
		rtl8651_setAsicOperationLayer(2);
		/* Enable ingress filtering. */
		WRITE_MEM32( VCR0, READ_MEM32(VCR0) &  EN_ALL_PORT_VLAN_INGRESS_FILTER);	
		WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0|EN_DOT1XPB_P1);				
		for (j=0;j<3;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rtl8651_setAsicAclRule( 0, &rule);
		/* Config VLAN 8 */
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 0;
		intf.inAclStart = 0;
		intf.outAclEnd = 0;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		
		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );			
		/**********************************************************************
		Enable PORT 0 801x Port base Control 
		Enable PORT 1 801x Port base Control 
		Port 0 status is unauthenticated.
		Port 1 status is unauthenticated.
		Expect Drop
 		**********************************************************************/
 		hsb.spa=0;
		bzero(&l2tdst,sizeof(l2tdst));
		bzero(&l2tsrc,sizeof(l2tdst));
		memcpy((void*)&l2tdst.macAddr,(void*)strtomac(&mac,"01-ff-ff-ff-ff-ff"),sizeof(l2tdst.macAddr));
		memcpy((void*)&l2tsrc.macAddr,(void*)strtomac(&mac,"00-80-c0-00-00-00"),sizeof(l2tsrc.macAddr));
		memcpy(&hsb.da,&l2tdst,sizeof(hsb.da));
		memcpy(&hsb.sa,&l2tsrc,sizeof(hsb.sa));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be to drop, but ...", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		Enable PORT 0 801x Port base Control 
		Enable PORT 1 801x Port base Control 
		Port 0 status is authenticated.
		Port 1 status is unauthenticated.
		Expect Forward
 		**********************************************************************/
		WRITE_MEM32(DOT1XPORTCR,EN_DOT1XPB_P0_AUTH|READ_MEM32(DOT1XPORTCR));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be to forward ...", hsa.dp,PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5,__FUNCTION__, __LINE__  ,i);
		/**********************************************************************
		Enable PORT 0 801x Port base Control  
		Enable PORT 1 801x Port base Control and Control is only for In Direction.
		Port 0 status is authenticated.
		Port 1 status is unauthenticated.
		Expect Forward
 		**********************************************************************/
		WRITE_MEM32(DOT1XPORTCR,EN_DOT1XPB_P1_OPINDIR|READ_MEM32(DOT1XPORTCR));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be to forward ...", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5,__FUNCTION__, __LINE__  ,i);
		/**********************************************************************
		Enable PORT 0 801x Port base Control  
		Enable PORT 1 801x Port base Control and Control is only for In Direction.
		Port 0 status is authenticated.
		Port 1 status is authenticated.
		Expect Forward
 		**********************************************************************/
		WRITE_MEM32(DOT1XPORTCR,EN_DOT1XPB_P1_OPINDIR|EN_DOT1XPB_P1_AUTH|READ_MEM32(DOT1XPORTCR));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be to forward ...", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5,__FUNCTION__, __LINE__  ,i);
		/**********************************************************************
		Enable PORT 0 801x Port base Control  
		Enable PORT 1 801x Port base Control.
		Port 0 status is authenticated.
		Port 1 status is authenticated.
		Expect Forward
 		**********************************************************************/
		WRITE_MEM32(DOT1XPORTCR,EN_DOT1XPB_P0|EN_DOT1XPB_P0_OPINDIR|EN_DOT1XPB_P0_AUTH|
									EN_DOT1XPB_P1|EN_DOT1XPB_P1_OPINDIR|EN_DOT1XPB_P1_AUTH);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be to forward ...", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5,__FUNCTION__, __LINE__  ,i);	

		/**********************************************************************
		Enable PORT 0 801x Port base Control  
		Enable PORT 1 801x Port base Control.
		Port 0 status is authenticated.
		Port 1 status is authenticated.
		....
		...
		Expect Forward
 		**********************************************************************/
		WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0|EN_DOT1XPB_P0_OPINDIR|EN_DOT1XPB_P0_AUTH|
									EN_DOT1XPB_P1|
									EN_DOT1XPB_P2|	EN_DOT1XPB_P2_OPINDIR|
									EN_DOT1XPB_P4|	EN_DOT1XPB_P4_AUTH|									
									EN_DOT1XPB_P5);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be to forward ...", hsa.dp,PM_PORT_2|PM_PORT_3|PM_PORT_4,__FUNCTION__, __LINE__  ,i);





	}		
	retval=SUCCESS;
	return retval;
}
#define TEST_801X_MAC "00-00-00-00-88-89"
#define TEST_SA_MAC "00-00-00-00-88-8A"
#define TEST_DA_MAC     "ff-ff-ff-ff-ff-ff"
int32 testLayer28021XMacBase(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x08,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch: 0,		ethtype: 0x7788,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x00,0x00,0x88,0x89},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j;
	uint32 time,timeout;
	_rtl8651_tblDrvAclRule_t rule;	
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	rtl865x_tblAsicDrv_l2Param_t l2t;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();		
		TESTING_MODE;
		rtl8651_setAsicOperationLayer(2);
		/* Enable ingress filtering. */
		WRITE_MEM32( VCR0,READ_MEM32(VCR0) | EN_ALL_PORT_VLAN_INGRESS_FILTER);	
		WRITE_MEM32(DOT1XMACCR, EN_8021X_PORT0_MAC_CONTROL);
		for (j=0;j<3;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rtl8651_setAsicAclRule( 0, &rule);
		/* Config VLAN 8 */
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 0;
		intf.inAclStart = 0;
		intf.outAclEnd = 0;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		
		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );			
		/* config l2 table */
		bzero((void*) &l2t, sizeof(l2t));
		/* wan 0 isp modem */
		/**********************************************************************
		SA is unauthenticated.
		DA is unauthenticated.
		Expect Drop
 		**********************************************************************/
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = (1);
		l2t.isStatic = 1;
		l2t.auth=0;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);		
		memcpy(&hsb.sa[0],&l2t.macAddr,sizeof(ether_addr_t));
 		strtomac(&l2t.macAddr,TEST_DA_MAC);
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(ether_addr_t));
		hsb.spa=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be to drop, but ...", hsa.dp,0,__FUNCTION__, __LINE__  ,i);
		/**********************************************************************
		SA is authenticated.
		DA is authenticated.
		Expect foward
		**********************************************************************/		
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_0;
		l2t.isStatic = 1;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
		retval = virtualMacSetHsb( &hsb );
		strtomac(&l2t.macAddr,TEST_DA_MAC);
		l2t.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		l2t.isStatic = 1;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_DA_MAC), 0, &l2t);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__,i );


		/**********************************************************************
		SA is unauthenticated.
		DA is authenticated , but da is broadcast
		Expect foward
		**********************************************************************/		


		WRITE_MEM32(DOT1XMACCR, (EN_8021X_PORT0_MAC_CONTROL|
									EN_8021X_PORT1_MAC_CONTROL|
									EN_8021X_PORT2_MAC_CONTROL));

		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_0;
		l2t.isStatic = 0;
		l2t.auth=0;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
		hsb.spa=3;		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__,i );

		/**********************************************************************
		SA is authenticated and is static.		
		DA is authenticated.
		Expect Drop
 		**********************************************************************/
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_0;
		l2t.isStatic = 1;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);

		hsb.spa=1;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  dropped,...", hsa.dp,0x0, __FUNCTION__, __LINE__ ,i);		

		/**********************************************************************
		SA is authenticated and is dynamic
		DA is authenticated.
		Expect foward
		**********************************************************************/		
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_0;
		l2t.isStatic = 0;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
		strtomac(&l2t.macAddr,TEST_DA_MAC);
		l2t.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		l2t.isStatic = 1;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_DA_MAC), 0, &l2t);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_0|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__,i );

		
		/**********************************************************************
		conti last case 
		SA is authenticated and is dynamic
		DA is authenticated.
		Expect foward to PM_PORT_1
		**********************************************************************/		
		strtomac(&l2t.macAddr,TEST_SA_MAC);
		l2t.memberPortMask = PM_PORT_2;
		l2t.isStatic = 0;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_SA_MAC), 0, &l2t);
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		
		strtomac(&l2t.macAddr,TEST_SA_MAC);
		memcpy(&hsb.sa[0],&l2t.macAddr,sizeof(l2t.macAddr));
		hsb.spa=2;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_1, __FUNCTION__, __LINE__,i );


		if (i==0x1000)
		{
			strtomac(&l2t.macAddr,TEST_801X_MAC);
			l2t.memberPortMask = PM_PORT_0;
			l2t.isStatic = 0;
			l2t.auth=1;
			l2t.ageSec=100;
			rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
			rtlglue_printf("wait DA  Timeout\n");
			rtlglue_getmstime(&time);				
			do
			{
				rtlglue_getmstime(&timeout);				
				if ((timeout-time) >(1600*1000))
				{
					rtlglue_printf("\nWait L2 Entry Aging Change Too Long. (%d second Wait) \n",(timeout-time)/1000);			
					return FAILED;
				}else if ((timeout-time)%1000==0)
				{
					rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);				
				}
				rtl8651_getAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC),0,&l2t);
			}
			while (l2t.ageSec!=0);
			hsb.spa=2;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );	
			IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_0|PM_PORT_1|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__,i );
		}


		/**********************************************************************
		conti last case 
		SA is authenticated and is dynamic
		DA is unauthenticated.
		EN_DOT1XMAC_OPDIR_IN = 0 (Both Direction)
		Expect foward to 0
		**********************************************************************/		
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_1;
		l2t.isStatic = 0;
		l2t.auth=0;
		l2t.ageSec=300;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be Drop  ....  ", hsa.dp,0, __FUNCTION__, __LINE__,i );
		/**********************************************************************
		conti last case 
		SA is authenticated and is dynamic and L2 entry exists
		DA is unauthenticated.
		Expect foward 
		**********************************************************************/		
		strtomac(&l2t.macAddr,"00-22-33-44-55-66");		
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__,i );
		/**********************************************************************
		conti last case 
		SA is authenticated and is dynamic and L2 entry exists
		DA is unknown unicast.
		Expect foward 
		**********************************************************************/		
		strtomac(&l2t.macAddr,"01-22-33-44-55-66");
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		
		hsb.spa=2;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__,i );

		/**********************************************************************
		conti last case 
		SA is authenticated and is dynamic and L2 entry exists
		DA is unknown unicast.
		EN_DOT1XMAC_OPDIR_IN = 1
		Expect foward 
		**********************************************************************/		
		WRITE_MEM32(DOT1XMACCR,EN_DOT1XMAC_OPDIR_IN|READ_MEM32(DOT1XMACCR));
		strtomac(&l2t.macAddr,"00-22-33-44-55-66");
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__,i );


		WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0|
									EN_DOT1XPB_P1|
									EN_DOT1XPB_P2|	EN_DOT1XPB_P2_OPINDIR|
									EN_DOT1XPB_P4|	EN_DOT1XPB_P4_AUTH|									
									EN_DOT1XPB_P5);

		/**********************************************************************
		SA is authenticated and is dynamic and L2 entry exists
		DA is unknown unicast.
		Expect foward 
		**********************************************************************/		
		WRITE_MEM32(DOT1XMACCR,EN_8021X_PORT4_MAC_CONTROL|READ_MEM32(DOT1XMACCR));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_2|PM_PORT_4, __FUNCTION__, __LINE__,i );




		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_1;
		l2t.isStatic = 0;
		l2t.auth=1;
		l2t.ageSec=0;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		

		strtomac(&l2t.macAddr,TEST_SA_MAC);
		l2t.memberPortMask = PM_PORT_2;
		l2t.isStatic = 0;
		l2t.auth=0;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_SA_MAC), 0, &l2t);

		WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0|
									EN_DOT1XPB_P1);
		hsb.spa=5;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be to forward ...", hsa.dp,PM_PORT_2|PM_PORT_3|PM_PORT_4,__FUNCTION__, __LINE__  ,i);
		/**********************************************************************
		SA is authenticated and static entry 
		DA is unknown unicast.
		Expect Drop 
		**********************************************************************/		
		WRITE_MEM32(DOT1XMACCR,EN_8021X_PORT2_MAC_CONTROL|EN_8021X_PORT5_MAC_CONTROL);
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_1;
		l2t.isStatic = 0;
		l2t.auth=1;
		l2t.ageSec=0;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		
		strtomac(&l2t.macAddr,TEST_SA_MAC);
		l2t.memberPortMask = PM_PORT_2;
		l2t.isStatic = 1;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_SA_MAC), 0, &l2t);
		hsb.spa=5;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be to drop ...", hsa.dp,0,__FUNCTION__, __LINE__  ,i);




	}		
	retval=SUCCESS;
	return retval;
}



int32 testLayer28021XGuestVlan(uint32 caseNo)
{
	int32 retval;

	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x08,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch: 0,		ethtype: 0x7788,
		da:{0x01,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x00,0x00,0x88,0x89},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j;
	uint32 guestVid;
	_rtl8651_tblDrvAclRule_t rule;	
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	rtl865x_tblAsicDrv_l2Param_t l2t;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();
		TESTING_MODE;
		rtl8651_setAsicOperationLayer(2);
		/* Enable ingress filtering. */
		guestVid=13;
		WRITE_MEM32( VCR0,READ_MEM32(VCR0) | EN_ALL_PORT_VLAN_INGRESS_FILTER);	
		WRITE_MEM32(DOT1XMACCR, EN_8021X_PORT0_MAC_CONTROL);
		for (j=0;j<3;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		WRITE_MEM32(GVGCR,guestVid|EN_8021X_TOGVLAN);
		bzero(&vlan, sizeof(vlan));
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		rtl8651_setAsicVlan( guestVid, &vlan);

		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		rtl8651_setAsicVlan(intf.vid, &vlan);

		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rtl8651_setAsicAclRule( 0, &rule);
		/* Config VLAN 8 */
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 0;
		intf.inAclStart = 0;
		intf.outAclEnd = 0;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		
		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );			
		/**********************************************************************
		Expect Forward
 		***********************************************************************/
		bzero((void*) &l2t, sizeof(l2t));
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_1;
		l2t.isStatic = 1;
		l2t.auth=0;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);		
		hsb.spa=0;
		memcpy((void*)&hsb.sa,(void*)&l2t.macAddr,sizeof(l2t.macAddr));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward,... ", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		Test Different Vlan member, broadcast is different
		Expect Forward To Guest Vlan Member
		 **********************************************************************/
  		WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0);
 		WRITE_MEM32(DOT1XMACCR, (EN_8021X_PORT0_MAC_CONTROL|
									EN_8021X_PORT1_MAC_CONTROL|
									EN_8021X_PORT2_MAC_CONTROL));		
		WRITE_MEM32(GVGCR,guestVid|EN_8021X_TOGVLAN);
		bzero(&vlan, sizeof(vlan));
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
		rtl8651_setAsicVlan( guestVid, &vlan);
 		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		SA is unauthenticated.
		DA is unauthenticated.
		Expect Forward to guest vlan member
 		**********************************************************************/
		WRITE_MEM32(GVGCR,guestVid|EN_8021X_TOGVLAN);
		bzero(&vlan, sizeof(vlan));
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
		rtl8651_setAsicVlan( guestVid, &vlan);
 		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);				
		/**********************************************************************
		Expect Forward To CPU
 		**********************************************************************/
		WRITE_MEM32(GVGCR,guestVid |EN_8021X_TOCPU);
 		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i);		
		/**********************************************************************
		Expect Drop
 		**********************************************************************/
		WRITE_MEM32(GVGCR,guestVid |EN_8021X_DROP);
 		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, ....", hsa.dp,0, __FUNCTION__, __LINE__,i);		
		/**********************************************************************
		SA is authenticated.
		DA is authenticated.
		Source Port is unauthenticated
		Expect Drop because SA belongs to Guest VLAN is disallowed to talk to authorized DA
		**********************************************************************/		
		WRITE_MEM32(GVGCR,guestVid |EN_8021X_TOGVLAN);
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_0;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
		strtomac(&l2t.macAddr,"00-ff-ff-ff-ff-00");
		l2t.memberPortMask = PM_PORT_1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-ff-ff-ff-ff-00"), 0, &l2t);
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		
		hsb.spa=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be drop ....  ", hsa.dp,0, __FUNCTION__, __LINE__,i );
		/**********************************************************************
		SA is unauthenticated.
		DA is authenticated.
		Port is authenticated
		Expect Drop because SA belongs to Guest VLAN is disallowed to talk to authorized DA
		**********************************************************************/		
		WRITE_MEM32(GVGCR,guestVid |EN_8021X_TOGVLAN);
  		WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0|EN_DOT1XPB_P0_AUTH);
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_0;
		l2t.auth=0;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
		strtomac(&l2t.macAddr,"00-ff-ff-ff-ff-00");
		l2t.memberPortMask = PM_PORT_1;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-ff-ff-ff-ff-00"), 0, &l2t);
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		
		hsb.spa=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be drop ....  ", hsa.dp,0, __FUNCTION__, __LINE__,i );
		/**********************************************************************
		SA is unauthenticated.
		DA is authenticated.
		Source Port is authenticated.
		Expect Forward because SA belongs to Guest VLAN is allowed to talk to authorized DA
		**********************************************************************/		
		WRITE_MEM32(GVGCR,EN_GVOPDIR| READ_MEM32(GVGCR));
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_0;
		l2t.auth=0;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
		strtomac(&l2t.macAddr,"00-ff-ff-ff-ff-00");
		l2t.memberPortMask = PM_PORT_1;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-ff-ff-ff-ff-00"), 0, &l2t);
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		
		hsb.spa=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be Forward ....  ", hsa.dp,PM_PORT_1, __FUNCTION__, __LINE__,i );

		/**********************************************************************
		SA is authenticated.
		DA is authenticated.
		Source Port is authenticated
		Expect Forward 
		**********************************************************************/		
		WRITE_MEM32(GVGCR,(~EN_GVOPDIR)&READ_MEM32(GVGCR));
 		WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0|EN_DOT1XPB_P0_AUTH);
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_0;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
		strtomac(&l2t.macAddr,"00-ff-ff-ff-ff-00");
		l2t.memberPortMask = PM_PORT_1;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-ff-ff-ff-ff-00"), 0, &l2t);
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		
		hsb.spa=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_1, __FUNCTION__, __LINE__,i );

		/**********************************************************************
		SA is authenticated.
		DA is unauthenticated.
		Source Port is authenticated
		Expect Drop 
		**********************************************************************/		
		WRITE_MEM32(GVGCR,(~EN_GVOPDIR)&READ_MEM32(GVGCR));
 		WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0|EN_DOT1XPB_P0_AUTH);
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.memberPortMask = PM_PORT_0;
		l2t.auth=1;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
		strtomac(&l2t.macAddr,"00-ff-ff-ff-ff-00");
		l2t.memberPortMask = PM_PORT_1;
		l2t.auth=0;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-ff-ff-ff-ff-00"), 0, &l2t);
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		
		hsb.spa=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be drop ....  ", hsa.dp,0, __FUNCTION__, __LINE__,i );

		/**********************************************************************
		SA is unauthenticated.
		DA is authenticated but aging is 0.
		Expect forward. 
		**********************************************************************/		
		rtl8651_setAsicVlan( guestVid, &vlan );
		WRITE_MEM32(GVGCR,(~EN_GVOPDIR)&READ_MEM32(GVGCR));


 		WRITE_MEM32(DOT1XPORTCR, 0);		
		WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0|EN_DOT1XPB_P1);
		WRITE_MEM32(DOT1XMACCR,EN_8021X_PORT2_MAC_CONTROL|EN_8021X_PORT0_MAC_CONTROL);
		strtomac(&l2t.macAddr,TEST_801X_MAC);
		l2t.isStatic=1;
		l2t.memberPortMask = PM_PORT_EXT0;
		l2t.auth=0;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TEST_801X_MAC), 0, &l2t);
		strtomac(&l2t.macAddr,"00-ff-ff-ff-ff-00");
		l2t.memberPortMask = PM_PORT_2;
		l2t.isStatic=0;
		l2t.auth=1;
		l2t.ageSec=0;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-ff-ff-ff-ff-00"), 0, &l2t);
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		
		hsb.spa=2;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_0|PM_PORT_1|PM_PORT_3 , __FUNCTION__, __LINE__,i );

		
		WRITE_MEM32(DOT1XPORTCR, EN_DOT1XPB_P0);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_0|PM_PORT_1 |PM_PORT_3, __FUNCTION__, __LINE__,i );
		



		WRITE_MEM32(DOT1XMACCR,EN_8021X_PORT2_MAC_CONTROL|EN_8021X_PORT3_MAC_CONTROL);		
		strtomac(&l2t.macAddr,"00-ff-ff-ff-ff-00");
		l2t.memberPortMask = PM_PORT_3;
		l2t.isStatic=0;
		l2t.auth=1;
		l2t.ageSec=300;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-ff-ff-ff-ff-00"), 0, &l2t);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be drop ....  ", hsa.dp,0, __FUNCTION__, __LINE__,i );


		strtomac(&l2t.macAddr,"ff-ff-ff-ff-00-00");
		l2t.memberPortMask = PM_PORT_3|PM_PORT_2|PM_PORT_4;
		l2t.isStatic=0;
		l2t.auth=1;
		l2t.ageSec=300;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr("ff-ff-ff-ff-00-00"), 0, &l2t);
		WRITE_MEM32(DOT1XMACCR,EN_8021X_PORT2_MAC_CONTROL);		
		
		memcpy(&hsb.da[0],&l2t.macAddr,sizeof(l2t.macAddr));		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_3, __FUNCTION__, __LINE__,i );




		WRITE_MEM32(DOT1XMACCR,EN_8021X_PORT2_MAC_CONTROL);		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be drop ....  ", hsa.dp,8, __FUNCTION__, __LINE__,i );

		WRITE_MEM32(GVGCR,(EN_GVOPDIR)|READ_MEM32(GVGCR));
		WRITE_MEM32(DOT1XMACCR,EN_8021X_PORT2_MAC_CONTROL|EN_8021X_PORT3_MAC_CONTROL);		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward ....  ", hsa.dp,PM_PORT_3, __FUNCTION__, __LINE__,i );

		
		
	}		
	retval=SUCCESS;
	return retval;
}

int32 testRandom8021X(uint32 caseNo)
{
	int32 retval;

	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x08,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch: 0,		ethtype: 0x7788,
		da:{0x01,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x00,0x00,0x88,0x89},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j,loop,result[6],fidsrc,fiddst;
	int dirtx,tagif,vid,tcpflag;
	uint32 gvgcrValue,vcrValue,maccr_value,vlanVid,srcMacMemberPort,dstMacMemberPort,spa;
	uint32 portcr_value,lanMember,gvlanMember,gvlanVid;
	_rtl8651_tblDrvAclRule_t rule;	
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	rtl865x_tblAsicDrv_l2Param_t l2tsrc,l2tdst;
	int extSpaArray[4]={1,2,4,4};


	retval = model_setTestTarget( IC_TYPE_REAL );
	if ( retval == VMAC_ERROR_NON_AVAILABLE ) 
	{
		rtlglue_printf("Need Real IC\n");
		return SUCCESS;
	}
	retval = model_setTestTarget( IC_TYPE_MODEL );
	if ( retval == VMAC_ERROR_NON_AVAILABLE ) 
	{
		rtlglue_printf("Need MODEL Code\n");
		return SUCCESS;
	}

	for (loop=0;loop<RANDOM_RUN_NUM;loop++)
	{
#ifdef VERBOSE
		rtlglue_printf("Run %d\n", loop);
#endif
		memset((void*)&l2tdst,0,sizeof(l2tdst));
		memset((void*)&l2tsrc,0,sizeof(l2tsrc));
		gvgcrValue=(rtlglue_random()&0xfff) |((rtlglue_random()&0xfff)<<12);
		if ( ((gvgcrValue >>GVLAN_PROCESS_BITS) &0x3) == 0x3)
		{
			gvgcrValue = (~(0x3 <<GVLAN_PROCESS_BITS) & gvgcrValue);
		}

		maccr_value=rtlglue_random();
		dirtx=rtlglue_random()&1;
		vid=rtlglue_random()&0xfff;
		fidsrc=rtlglue_random();
		fiddst=rtlglue_random();
		if (rtlglue_random()&1)
			tcpflag=1;
		else
			tcpflag=1<<6;
		tagif=rtlglue_random()&0x1;
		portcr_value=rtlglue_random();
		lanMember=rtlglue_random()&0x1FF;
		gvlanMember=rtlglue_random()&0x1FF;
		vlanVid=rtlglue_random()&0xFFF;
		gvlanVid=rtlglue_random()&0xFFF;
		do{
			srcMacMemberPort=(1<<(rtlglue_random()))&0x1FF;
		}while(srcMacMemberPort==0);
		do {	
			dstMacMemberPort=(1<<(rtlglue_random()))&0x1FF;
		}while(dstMacMemberPort==0);
		l2tsrc.macAddr.octet[0] = rtlglue_random()&0xEF;
		l2tsrc.macAddr.octet[1] = rtlglue_random()&0xff;
		l2tsrc.macAddr.octet[2] = rtlglue_random()&0xff;
		l2tsrc.macAddr.octet[3] = rtlglue_random()&0xff;
		l2tsrc.macAddr.octet[4] = rtlglue_random()&0xff;
		l2tsrc.macAddr.octet[5] = rtlglue_random()&0xff;
		l2tsrc.memberPortMask = srcMacMemberPort;
		l2tsrc.auth=rtlglue_random()&0x1;		
		l2tsrc.isStatic = rtlglue_random()&0x1;
		l2tsrc.ageSec = (rtlglue_random()&0x3)*100;
		l2tsrc.cpu=rtlglue_random()&0x1;
		
		l2tdst.macAddr.octet[0] = rtlglue_random()&0xff;
		l2tdst.macAddr.octet[1] = rtlglue_random()&0xff;
		l2tdst.macAddr.octet[2] = rtlglue_random()&0xff;
		l2tdst.macAddr.octet[3] = rtlglue_random()&0xff;
		l2tdst.macAddr.octet[4] = rtlglue_random()&0xff;
		l2tdst.macAddr.octet[5] = rtlglue_random()&0xff;

		if (l2tdst.macAddr.octet[0]&0x1)
			l2tdst.memberPortMask	=rtlglue_random()&0x1ff;
		else
			l2tdst.memberPortMask = dstMacMemberPort;
		l2tdst.auth=rtlglue_random()&0x1;
		l2tdst.isStatic = rtlglue_random()&0x1;
		l2tdst.ageSec = (rtlglue_random()&0x3)*100;
		l2tdst.cpu =rtlglue_random()&0x1; 
		spa=rtlglue_random()&0x7;
		vcrValue=rtlglue_random();
		while(spa==6)
		{
			spa =spa-(rtlglue_random()&0x3);
			
		}
		if (spa==7)
		{
			hsb.extspa=extSpaArray[rtlglue_random()&0x3];
			if (((vcrValue >> ((6+hsb.extspa)*2+P0_AcptFType_OFFSET))&0x3)==0x3)
				vcrValue &=  ~(0x3<<((6+hsb.extspa)*2+P0_AcptFType_OFFSET));
		}
		else
		{
			if (((vcrValue >> (spa*2+P0_AcptFType_OFFSET))&0x3)==0x3)
				vcrValue &=  ~(0x3<<(spa*2+P0_AcptFType_OFFSET));
		}
#ifdef VERBOSE		
		rtlglue_printf("gvgcrValue=0x%x;\n",gvgcrValue);
		rtlglue_printf("vcrValue=0x%x;\n",vcrValue);
		rtlglue_printf("maccr_value=0x%x;\n",maccr_value);
		rtlglue_printf("portcr_value=0x%x;\n",portcr_value);
		rtlglue_printf("lanMember=0x%x;\n",lanMember);
		rtlglue_printf("gvlanMember=0x%x;\n",gvlanMember);
		rtlglue_printf("vlanVid=%d;\n",vlanVid);
		rtlglue_printf("gvlanVid=%d;\n",gvlanVid);
		rtlglue_printf("srcMacMemberPort=%d;\n",srcMacMemberPort);
		rtlglue_printf("dstMacMemberPort=%d;\n",dstMacMemberPort);
		rtlglue_printf("spa=%d;\n",spa);
		rtlglue_printf("l2tsrc.macAddr.octet[0]=0x%x;\n",l2tsrc.macAddr.octet[0]);
		rtlglue_printf("l2tsrc.macAddr.octet[1]=0x%x;\n",l2tsrc.macAddr.octet[1]);		
		rtlglue_printf("	l2tsrc.macAddr.octet[2]=0x%x;\n",l2tsrc.macAddr.octet[2]);
		rtlglue_printf("l2tsrc.macAddr.octet[3]=0x%x;\n",l2tsrc.macAddr.octet[3]);
		rtlglue_printf("l2tsrc.macAddr.octet[4]=0x%x;\n",l2tsrc.macAddr.octet[4]);		
		rtlglue_printf("	l2tsrc.macAddr.octet[5]=0x%x;\n",l2tsrc.macAddr.octet[5]);
		rtlglue_printf("l2tsrc.memberPortMask=srcMacMemberPort;\n");		
		rtlglue_printf("l2tsrc.auth=%d;\n",l2tsrc.auth);		
		rtlglue_printf("l2tsrc.isStatic=%d;\n",l2tsrc.isStatic);		
		rtlglue_printf("l2tsrc.ageSec=%d;\n",l2tsrc.ageSec);			
		rtlglue_printf("l2tdst.macAddr.octet[0]=0x%x;\n",l2tdst.macAddr.octet[0]);
		rtlglue_printf("l2tdst.macAddr.octet[1]=0x%x;\n",l2tdst.macAddr.octet[1]);		
		rtlglue_printf("l2tdst.macAddr.octet[2]=0x%x;\n",l2tdst.macAddr.octet[2]);		
		rtlglue_printf("l2tdst.macAddr.octet[3]=0x%x;\n",l2tdst.macAddr.octet[3]);
		rtlglue_printf("l2tdst.macAddr.octet[4]=0x%x;\n",l2tdst.macAddr.octet[4]);		
		rtlglue_printf("l2tdst.macAddr.octet[5]=0x%x;\n",l2tdst.macAddr.octet[5]);		
		rtlglue_printf("l2tdst.memberPortMask=dstMacMemberPort;\n");				
		rtlglue_printf("l2tdst.auth=%d;\n",l2tdst.auth);		
		rtlglue_printf("l2tdst.ageSec=%d;\n",l2tdst.ageSec);		
		rtlglue_printf("l2tdst.isStatic=%d;\n",l2tdst.isStatic);		
		rtlglue_printf("hsb.extspa=%d;\n",hsb.extspa);		
		rtlglue_printf("l2tdst.cpu=%d;\n",l2tdst.cpu);		
		rtlglue_printf("l2tsrc.cpu=%d;\n",l2tsrc.cpu);		
		rtlglue_printf("dirtx=%d;\n",dirtx);		
		rtlglue_printf("vid=%d;\n",vid);		
		rtlglue_printf("tagif=%d;\n",tagif);				
		rtlglue_printf("fidsrc=%d;\n",fidsrc);				
		rtlglue_printf("fiddst=%d;\n",fiddst);		
#endif

		l2tdst.fid=fiddst ^ gvlanMember;
		l2tsrc.fid=fidsrc ^ gvlanMember;
		for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
			
		{
			retval = model_setTestTarget( i );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
			rtl8651_clearRegister();
			rtl8651_clearAsicAllTable();			
			TESTING_MODE;
			rtl8651_totalExtPortNum=3;
			rtl8651_setAsicOperationLayer(2);
			WRITE_MEM32(MSCR,READ_MEM32(MSCR)&~(EN_IN_ACL|EN_OUT_ACL));
			rtl8651_setAsicAgingFunction(FALSE,FALSE);
			WRITE_MEM32(VCR0,vcrValue);	
			WRITE_MEM32(DOT1XMACCR, maccr_value);
			WRITE_MEM32(DOT1XPORTCR, portcr_value);
			WRITE_MEM32(GVGCR,gvgcrValue);
			for (j=0;j<5;j++)	WRITE_MEM32(( PVCR0+j*4), ((vlanVid<<0)|(vlanVid<<16)) );			
			bzero(&vlan, sizeof(vlan));
			vlan.memberPortMask =gvlanMember;
			vlan.untagPortMask = gvlanMember;
			vlan.fid = fiddst;
			rtl8651_setAsicVlan(gvlanVid,&vlan);
			/* Set permit rule. */
			bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
			rule.actionType_ = RTL8651_ACL_PERMIT;
			rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
			rule.ruleType_ = RTL8651_ACL_MAC;
			rtl8651_setAsicAclRule( 0, &rule);
			/* Config VLAN 8 */
			bzero(&intf, sizeof(intf));
			intf.vid = vlanVid;
			intf.valid = 1;
			intf.enableRoute = FALSE;
			intf.inAclEnd = 0;
			intf.inAclStart = 0;
			intf.outAclEnd = 0;
			intf.outAclStart = 0;
			memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
			intf.macAddrNumber = 1;
			intf.mtu = 1500;
			rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
			
			{
				int portIdx;

				for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
				{
					rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
				}
			}

			bzero( &vlan, sizeof(vlan) );
			vlan.memberPortMask =lanMember;
			vlan.untagPortMask = lanMember;
			vlan.fid=fidsrc^fiddst;
			rtl8651_setAsicVlan( intf.vid, &vlan );						
			rtl8651_setAsicL2Table(rtl8651_filterDbIndex(&l2tsrc.macAddr,0), 0, &l2tsrc);
			rtl8651_setAsicL2Table(rtl8651_filterDbIndex(&l2tdst.macAddr,0), 0, &l2tdst);			
			
			hsb.spa =spa;
			hsb.dirtx=dirtx;
			hsb.tagif=tagif;
			hsb.vid=vid;
			hsb.tcpfg=tcpflag;
			memcpy(&hsb.sa[0],&l2tsrc.macAddr,sizeof(ether_addr_t));
			memcpy(&hsb.da[0],&l2tdst.macAddr,sizeof(ether_addr_t));
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );	
			result[i]=hsa.dp;
			result[i+2]=hsa.dpext;
			result[i+4]=hsa.dvid;
			
		}		
		IS_EQUAL_INT("DP  should be the same ....  ", result[0],result[1], __FUNCTION__, __LINE__);
		IS_EQUAL_INT("DPEXT should be the same ....  ", result[2],result[3], __FUNCTION__, __LINE__);
		if ((result[2]!=PM_PORT_CPU) && (result[0]))
			IS_EQUAL_INT("dvid should be the same ....  ", result[4],result[5], __FUNCTION__, __LINE__);
	
		
	}
	retval=SUCCESS;
	return retval;
}
typedef  struct
{
	uint8 protcol;
	uint8 port;
	uint8  valid;	
	uint8  priority;	
	uint16 vlanId;
	uint16 vlanMember;
	

}portProtocolBaseInfo;

int32 testRandom8021Q(uint32 caseNo)
{
	int32 retval;

	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x08,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch: 0,		ethtype: 0x7788,
		da:{0x01,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x00,0x00,0x88,0x89},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j,loop,result[4];
	uint16 dvid[2];
	uint32 vcrValue,vlan1Vid,vlan2Vid,spa;
	uint32 lan1Member,lan2Member;
	_rtl8651_tblDrvAclRule_t rule;	
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	portProtocolBaseInfo	portsInfo[9];
	rtl865x_tblAsicDrv_l2Param_t l2tdst;
	int extSpaArray[4]={0,1,2,2};

	retval = model_setTestTarget( IC_TYPE_REAL );
	if ( retval == VMAC_ERROR_NON_AVAILABLE ) 
	{
		rtlglue_printf("Need Real IC\n");
		return SUCCESS;
	}
	retval = model_setTestTarget( IC_TYPE_MODEL );
	if ( retval == VMAC_ERROR_NON_AVAILABLE ) 
	{
		rtlglue_printf("Need MODEL Code\n");
		return SUCCESS;
	}

	for (loop=0;loop<RANDOM_RUN_NUM;loop++)
	{
		rtlglue_printf("Run %d\n", loop);		
		lan1Member=rtlglue_random()&0x1FF;
		lan2Member=rtlglue_random()&0x1FF;
		vlan1Vid=rtlglue_random()&0xFFF;
		vlan2Vid=rtlglue_random()&0xFFF;

		
		spa=rtlglue_random()&0x7;
		vcrValue=rtlglue_random();		
		while(spa==6)
		{
			spa =spa-(rtlglue_random()&0x3);
			
		}
		if (spa==7)
		{
			hsb.extspa=extSpaArray[rtlglue_random()&0x3];
			if (((vcrValue >> ((6+hsb.extspa)*2+P0_AcptFType_OFFSET))&0x3)==0x3)
				vcrValue &=  ~(0x3<<((6+hsb.extspa)*2+P0_AcptFType_OFFSET));
		}
		else
		{
			if (((vcrValue >> (spa*2+P0_AcptFType_OFFSET))&0x3)==0x3)
				vcrValue &=  ~(0x3<<(spa*2+P0_AcptFType_OFFSET));
		}
		hsb.tagif = rtlglue_random()&0x1;
		hsb.vid = rtlglue_random()&0xFFF;
		hsb.ethtype= rtlglue_random()&0xFFFF;
		hsb.ethtype= rtlglue_random()&0xFFFF;
		hsb.llcothr= rtlglue_random()&0x1;
		if (hsb.llcothr)
			hsb.snap=0;
		else 
			hsb.snap= rtlglue_random()&0x1;
		hsb.spa =spa;
		l2tdst.macAddr.octet[0] = rtlglue_random()&0xff;
		l2tdst.macAddr.octet[1] = rtlglue_random()&0xff;
		l2tdst.macAddr.octet[2] = rtlglue_random()&0xff;
		l2tdst.macAddr.octet[3] = rtlglue_random()&0xff;
		l2tdst.macAddr.octet[4] = rtlglue_random()&0xff;
		l2tdst.macAddr.octet[5] = rtlglue_random()&0xff;

		for (j=0;j<9;j++)
		{
			portsInfo[j].protcol=(rtlglue_random()%6)+1; /* from 1 to 6 */
			portsInfo[j].valid=rtlglue_random()&0x1;
			portsInfo[j].vlanId=rtlglue_random()&0xFFF;
			portsInfo[j].vlanMember=rtlglue_random()&0x1FFF;
		}
#ifdef VERBOSE
		rtlglue_printf("vcrValue=0x%x;\n",vcrValue);
		rtlglue_printf("vlan1Vid=%d;",vlan1Vid);
		rtlglue_printf("vlan2Vid=%d;\n",vlan2Vid);
		rtlglue_printf("lan2Member=0x%x;",lan2Member);
		rtlglue_printf("lan1Member=0x%x;\n",lan1Member);

		rtlglue_printf("hsb.tagif=0x%x;",hsb.tagif);
		
		rtlglue_printf("hsb.vid=0x%x; ",hsb.vid);
		rtlglue_printf("hsb.spa=%d;",hsb.spa);
		rtlglue_printf("hsb.snap=%d;\n",hsb.snap);
		rtlglue_printf("hsb.ethtype=0x%x; ",hsb.ethtype);
		rtlglue_printf("hsb.llcothr=%d; ",hsb.llcothr);
		rtlglue_printf("hsb.extspa=0x%x;\n",hsb.extspa);
		rtlglue_printf("l2tdst.macAddr.octet[0]=0x%x;",l2tdst.macAddr.octet[0]);
		rtlglue_printf("l2tdst.macAddr.octet[1]=0x%x;\n",l2tdst.macAddr.octet[1]);		
		rtlglue_printf("l2tdst.macAddr.octet[2]=0x%x;",l2tdst.macAddr.octet[2]);		
		rtlglue_printf("l2tdst.macAddr.octet[3]=0x%x;\n",l2tdst.macAddr.octet[3]);
		rtlglue_printf("l2tdst.macAddr.octet[4]=0x%x;",l2tdst.macAddr.octet[4]);		
		rtlglue_printf("l2tdst.macAddr.octet[5]=0x%x;\n",l2tdst.macAddr.octet[5]);		
		for (j=0;j<9;j++)
		{
				rtlglue_printf("portsInfo[%d].protcol=%d;",j,portsInfo[j].protcol);
				rtlglue_printf("portsInfo[%d].valid=%d;\n",j,portsInfo[j].valid);
				rtlglue_printf("portsInfo[%d].vlanMember=%d;",j,portsInfo[j].vlanMember);
				rtlglue_printf("portsInfo[%d].vlanId=0x%x;\n",j,portsInfo[j].vlanId);				
		}		
#endif
		for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
			
		{
			retval = model_setTestTarget( i );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
			rtl8651_clearRegister();
			rtl8651_clearAsicAllTable();
			TESTING_MODE;
			rtl8651_totalExtPortNum=3;		
			rtl8651_setAsicOperationLayer(2);
			rtl8651_setAsicAgingFunction(FALSE,FALSE);
			WRITE_MEM32(VCR0,vcrValue);	
			for (j=0;j<5;j++)	WRITE_MEM32(( PVCR0+j*4), ((vlan1Vid<<0)|(vlan2Vid<<16)) );			
			for (j=0;j<9;j++)
			{
				rtl8651_setProtocolBasedVLAN(portsInfo[j].protcol, j,portsInfo[j].valid, portsInfo[j].vlanId|(portsInfo[j].priority<<12) );					
				bzero(&vlan, sizeof(vlan));
				vlan.memberPortMask =portsInfo[j].vlanMember;
				rtl8651_setAsicVlan(portsInfo[j].vlanId,&vlan);

			}

			bzero(&vlan, sizeof(vlan));
			vlan.memberPortMask =lan1Member;
			rtl8651_setAsicVlan(vlan1Vid,&vlan);
			/* Set permit rule. */
			bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
			rule.actionType_ = RTL8651_ACL_PERMIT;
			rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
			rule.ruleType_ = RTL8651_ACL_MAC;
			rtl8651_setAsicAclRule( 0, &rule);
			/* Config VLAN 8 */
			bzero(&intf, sizeof(intf));
			intf.vid = vlan2Vid;
			intf.valid = 1;
			intf.enableRoute = FALSE;
			intf.inAclEnd = 0;
			intf.inAclStart = 0;
			intf.outAclEnd = 0;
			intf.outAclStart = 0;
			memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
			intf.macAddrNumber = 1;
			intf.mtu = 1500;
			rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
			
			{
				int portIdx;

				for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
				{
					rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
				}
			}

			bzero( &vlan, sizeof(vlan) );
			vlan.memberPortMask =lan2Member;
			rtl8651_setAsicVlan( intf.vid, &vlan );									
			memcpy(&hsb.da[0],&l2tdst.macAddr,sizeof(ether_addr_t));
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			assert( retval == SUCCESS );	
			result[i]=hsa.dp;
			result[i+2]=hsa.dpext;
			dvid[i]=hsa.dvid;

			
		}		
		IS_EQUAL_INT("DP  should be the same ....  ", result[0],result[1], __FUNCTION__, __LINE__);
		IS_EQUAL_INT("DPEXT should be the same ....  ", result[2],result[3], __FUNCTION__, __LINE__);
		IS_EQUAL_INT("DVID should be the same ....  ", dvid[0],dvid[1], __FUNCTION__, __LINE__);
		
	}
	retval=SUCCESS;
	return retval;

}
int32 testProtocolBaseVlan(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x00,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch: 0,		ethtype: 0,
		da:{0x01,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x00,0x00, 0x88,0x89},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 0,
		llcothr: 0,			urlmch: 1,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j;
	_rtl8651_tblDrvAclRule_t rule;	
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();		
		TESTING_MODE;
		rtl8651_setAsicOperationLayer(2);
		rtl8651_setProtocolBasedVLAN(RTL8651_PBV_RULE_PPPOE_CONTROL, 0, TRUE, 1024 );	
		rtl8651_setProtocolBasedVLAN(RTL8651_PBV_RULE_PPPOE_SESSION, 0, TRUE, 2024 );	
		rtl8651_setProtocolBasedVLAN(RTL8651_PBV_RULE_IPX, 0x0, TRUE, 10);
		rtl8651_defineProtocolBasedVLAN(RTL8651_PBV_RULE_USR1,0x0,ETHERTYPE_II);
		rtl8651_setProtocolBasedVLAN(RTL8651_PBV_RULE_USR1, 0, TRUE, 0x9 );	
		rtl8651_defineProtocolBasedVLAN(RTL8651_PBV_RULE_USR2,0x2,LLC_OTHER);
		rtl8651_setProtocolBasedVLAN(RTL8651_PBV_RULE_USR2, 0, TRUE, 20 );	
		WRITE_MEM32( VCR0,READ_MEM32(VCR0) | EN_ALL_PORT_VLAN_INGRESS_FILTER);	
		for (j=0;j<3;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rtl8651_setAsicAclRule( 0, &rule);
		/* Config VLAN 8 */
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 0;
		intf.inAclStart = 0;
		intf.outAclEnd = 0;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		
		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.untagPortMask = 0;		
		rtl8651_setAsicVlan( intf.vid, &vlan );			
		/**********************************************************************
	  	Expect DVID = 0x9 and packet should be drop 
		**********************************************************************/

		hsb.ethtype=ETHERTYPE_II;
		hsb.tagif=0;
		hsb.snap=0;
		hsb.llcothr=0;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  dropped ...", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("DVID  should be  dropped ...", hsa.dvid,9, __FUNCTION__, __LINE__ ,i);
		

		/**********************************************************************
		Test Different Vlan member, broadcast is different		
		Expect Forward
		**********************************************************************/
		bzero(&vlan, sizeof(vlan));
		vlan.memberPortMask = PM_PORT_0| PM_PORT_1|PM_PORT_2|PM_PORT_3;		
		rtl8651_setAsicVlan(0x9,&vlan);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward,...", hsa.dp, PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__ ,i);

		/**********************************************************************		
		Expect forward
		DVID 8
		**********************************************************************/		
		hsb.llcothr=1;
		hsb.snap=0;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, ...", hsa.dp, PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("DVID  should be  8 ...", hsa.dvid,8, __FUNCTION__, __LINE__ ,i);


		hsb.llcothr=0;
		hsb.snap=1;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, ...", hsa.dp, PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("DVID  should be  8 ...", hsa.dvid,8, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************		
		Expect drop
		etherType=RFC_1042
		DVID 20
		**********************************************************************/		
		rtl8651_defineProtocolBasedVLAN(RTL8651_PBV_RULE_USR2,0x1,RFC_1042);
		hsb.llcothr=0;		
		hsb.snap=1;
		hsb.ethtype=RFC_1042;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  drop, ...", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("DVID  should be  20  ...", hsa.dvid,20, __FUNCTION__, __LINE__ ,i);



#if 0 /* alpha: (snap==1)&&(llcothr==1) is illegal !!!! */
		/**********************************************************************		
		etherType=RFC_1042
		hsb.llcothr=1;		
		hsb.snap=1;	
		DVID 8
		**********************************************************************/	
		hsb.llcothr=1;		
		hsb.snap=1;
		hsb.ethtype=RFC_1042;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, ...", hsa.dp, PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("DVID  should be  20  ...", hsa.dvid,8, __FUNCTION__, __LINE__ ,i);
#endif
		

		hsb.llcothr=0;		
		hsb.snap=0;
		hsb.ethtype=RFC_1042;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, ...", hsa.dp, PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("DVID  should be  20  ...", hsa.dvid,8, __FUNCTION__, __LINE__ ,i);



		hsb.llcothr=1;		
		hsb.snap=0;
		hsb.ethtype=RFC_1042;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, ...", hsa.dp, PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("DVID  should be  20  ...", hsa.dvid,8, __FUNCTION__, __LINE__ ,i);

		
		/**********************************************************************
		PPPoE Expect drop
		**********************************************************************/
		hsb.snap=0;
		hsb.llcothr=0;
		hsb.ethtype=PPPOECONTROL_ETHERTYPE;		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but dropped", hsa.dp,0x0, __FUNCTION__, __LINE__ ,i);



		hsb.snap=1;
		hsb.llcothr=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, ...", hsa.dp, PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("DVID  should be  8 ...", hsa.dvid,8, __FUNCTION__, __LINE__ ,i);


#if 0 /* alpha: (snap==1)&&(llcothr==1) is illegal !!!! */
		hsb.snap=1;
		hsb.llcothr=1;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, ...", hsa.dp, PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("DVID  should be  8 ...", hsa.dvid,8, __FUNCTION__, __LINE__ ,i);
#endif



		hsb.snap=0;
		hsb.llcothr=1;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, ...", hsa.dp, PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
		IS_EQUAL_INT_DETAIL("DVID  should be  8 ...", hsa.dvid,8, __FUNCTION__, __LINE__ ,i);

		/**********************************************************************
		PPPoE Seesion Expect drop
		 **********************************************************************/
 		hsb.snap=0;
		hsb.llcothr=0;
		hsb.ethtype=PPPOESESSION_ETHERTYPE;		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but dropped", hsa.dp,0x0, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************		 	s
		IPX  Expect drop
		***********************************************************************/
		hsb.llcothr=1;
 		hsb.snap=0;
		hsb.ethtype=IPX_ETHERTYPE;		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but dropped", hsa.dp,0x0, __FUNCTION__, __LINE__ ,i);
		
		/**********************************************************************		 	
		Test LLCOther Bit
		***********************************************************************/
		hsb.llcothr=0;
 		hsb.snap=0;
		hsb.ethtype=IPX_ETHERTYPE;		
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but dropped", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);




		hsb.llcothr=0;
		hsb.snap=1;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but dropped", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);



#if 0 /* alpha: (snap==1)&&(llcothr==1) is illegal !!!! */
		hsb.llcothr=1;
		hsb.snap=1;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but dropped", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
#endif

		/**********************************************************************
			TAG Packet , expect : Forward
		**********************************************************************/
		hsb.tagif=1;
		hsb.vid=0x8;
		hsb.llcothr=0;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		assert( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but dropped", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__ ,i);
	
	}		
	retval=SUCCESS;
	return retval;

}
int32 testLayer2Aging(uint32 caseNo)
{
	#define TESTING_MAC	 "00-00-ff-ff-00-00"
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 124,
		vid: 0x08,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch:0,		ethtype: 0x8137,
		da:{0x00,0x00,0xff,0xff,0x00,0x00},
		sa:{0x00,0x00,0x00,0x00,0x88,0x89},
		hiprior:3,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch:0,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j;
	uint32 time,timeout;
	rtl865x_tblAsicDrv_l2Param_t l2t;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	ether_addr_t mac;
	_rtl8651_tblDrvAclRule_t rule;	
	
#if defined(VERA)||defined(VSV)
	rtlglue_printf( "In VERA mode, it is non-sense to test aging.\n" );
	return SUCCESS; /* In VERA mode, it is non-sense to test aging. */
#endif

	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */	

		rtl8651_clearAsicAllTable();	
		rtl8651_clearRegister();
		TESTING_MODE;
		AllPortForward(RTL8651_PORTSTA_FORWARDING);	
		/* Set PVID of all port to VLAN 8. */
		for (j=0;j<3;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		rtl8651_setAsicOperationLayer(2);
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));		
		bzero((void*) &l2t, sizeof(l2t));
		/* wan 0 isp modem */
		strtomac(&l2t.macAddr,TESTING_MAC);
		l2t.memberPortMask = (0x2);
		l2t.isStatic = 1;
		l2t.auth=0;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr(TESTING_MAC), 0, &l2t);		
		/* Config VLAN 8 */
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 0;
		intf.inAclStart = 0;
		intf.outAclEnd = 0;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		
		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.untagPortMask = 0;
		vlan.memberPortMask = 0x1f;
		rtl8651_setAsicVlan( intf.vid, &vlan );			

		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.pktOpApp = RTL865XC_ACLTBL_ALL_LAYER;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rtl8651_setAsicAclRule( 0, &rule);

		hsb.spa=0;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-FF-FF-01-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-00-FF-FF-00-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		IS_EQUAL_INT_DETAIL("Pakcet should be only forward to PM_PORT_1  to ....", hsa.dp,PM_PORT_1, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Static entry age- time  should be refresh		
		Asic scan every entry once every 100 secs.
		The Biggest Error is 99 secs
		
		**********************************************************************/
		hsb.spa=3;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-FF-FF-00-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-00-FF-FF-01-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );			
		IS_EQUAL_INT_DETAIL("Pakcet should be only forward to PM_PORT_0 ....", hsa.dp,PM_PORT_0, __FUNCTION__, __LINE__,i);
		rtl8651_getAsicL2Table(getL2TableRowFromMacStr(TESTING_MAC), 0, &l2t);
		if (i==IC_TYPE_REAL)
		{
			rtlglue_getmstime(&time);			
			rtlglue_printf("Wait L2 Entry Aging Change(SysTime %d)\n",time);
			rtlglue_getmstime(&time);
			while (rtlglue_getmstime(&timeout)==time);/* Time Sync*/
			do 
			{
				rtlglue_getmstime(&timeout);				
				rtl8651_getAsicL2Table(getL2TableRowFromMacStr(TESTING_MAC), 0, &l2t);
				if ((timeout-time) >(1600*1000))
				{
					rtlglue_printf("\nWait L2 Entry Aging Change Too Long. (%d second Wait) \n",(timeout-time)/1000);			
					return FAILED;
				}else if ((timeout-time)%1000==0)
				{
					rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);
				}
			}while (l2t.ageSec==300);
			rtlglue_printf("\nL2 Entry Age is %d(Time Difference(300 downto 200 sec):  %d (ms) \n",l2t.ageSec,timeout-time);			
			rtlglue_getmstime(&time);			
			rtlglue_printf("Wait L2 Entry Aging Change(SysTime %d)\n",time);
			rtlglue_getmstime(&time);
			while (rtlglue_getmstime(&timeout)==time);/* Time Sync*/
			do 
			{
				rtlglue_getmstime(&timeout);				
				rtl8651_getAsicL2Table(getL2TableRowFromMacStr(TESTING_MAC), 0, &l2t);
				if ((timeout-time) >(1600*1000))
				{
					rtlglue_printf("\nWait L2 Entry Aging Change Too Long. (%d second Wait) \n",(timeout-time)/1000);			
					return FAILED;
				}else if ((timeout-time)%1000==0)
				{
					rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);
				}
			}while (l2t.ageSec==200);
			rtlglue_printf("\nL2 Entry Age is %d(Time Difference(200 downto 100 sec):  %d (ms) \n",l2t.ageSec,timeout-time);			

			rtlglue_printf("Wait L2 Entry Aging Change(SysTime %d)\n",time);

			rtlglue_getmstime(&time);
			while (rtlglue_getmstime(&timeout)==time);/* Time Sync*/
			do 
			{
				rtlglue_getmstime(&timeout);				
				rtl8651_getAsicL2Table(getL2TableRowFromMacStr(TESTING_MAC), 0, &l2t);
				if ((timeout-time) >(1600*1000))
				{
					rtlglue_printf("\nWait L2 Entry Aging Change Too Long. (%d second Wait) \n",(timeout-time)/1000);			
					return FAILED;
				}else if ((timeout-time)%1000==0)
				{
					rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);
				}
			}while (l2t.ageSec==100);
			rtlglue_printf("\nL2 Entry Age is %d(Time Difference(100 downto 0 sec):  %d (ms) \n",l2t.ageSec,timeout-time);			



		}					
		/**********************************************************************
		EXPECT Forward TO PM_PORT_1
		**********************************************************************/
		hsb.spa=0;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-FF-FF-01-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-00-FF-FF-00-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		IS_EQUAL_INT_DETAIL("Pakcet should be only forward 0x2 to , but ....", hsa.dp,PM_PORT_1, __FUNCTION__, __LINE__,i);

		/**********************************************************************
		EXPECT Forward TO PM_PORT_1
		**********************************************************************/
		hsb.spa=0;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-FF-FF-01-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-00-FF-FF-00-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );		
		if (i==IC_TYPE_REAL)
		{
			rtlglue_getmstime(&time);			
			rtlglue_printf("Wait L2 Entry Fast  Aging (SysTime %d)\n",time);
			WRITE_MEM32(TEACR,EN_L2FASTAGING|READ_MEM32(TEACR));
			rtlglue_getmstime(&time);
			while (rtlglue_getmstime(&timeout)==time);/* Time Sync*/
			do 
			{
				rtlglue_getmstime(&timeout);				
				if (rtl8651_getAsicL2Table(getL2TableRowFromMacStr("00-00-FF-FF-01-00"), 0, &l2t)==FAILED)
				{
					rtlglue_printf("\nCan't Get L2 Entry(Aged Out)\n");			
					break;
				}
				if ((timeout-time) >(500*1000))
				{
					rtlglue_printf("\nWait L2 Entry Fast Aging  Too Long. (%d second Wait) \n",(timeout-time)/1000);			
					return FAILED;
				}else if ((timeout-time)%1000==0)
				{
					rtlglue_printf("\r %04d Seconds Pass",(timeout-time)/1000);
				}
			}while (l2t.ageSec>0);
			rtlglue_printf("\nL2 Entry  Age is %d(Time Difference(300 downto 0 sec):  %d (ms) \n",l2t.ageSec,timeout-time);				

		}

	}		
	
	retval=SUCCESS;
	return retval;

}



int32 testPatternMatch(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 124,
		vid: 0,			tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,	ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_UDP,
		patmatch:1,		ethtype: 0x8086,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x11,0x11,0x00,0x00},
		hiprior:3,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch:0,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	_rtl8651_tblDrvAclRule_t rule;	
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		rtl8651_clearRegister();
		rtl8651_clearAsicAllTable();		
		TESTING_MODE;
		
		/* Set PVID of all port to VLAN 8. */
		for (j=0;j<3;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		rtl8651_setAsicOperationLayer(2);
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));		
		/* Config VLAN 8 */
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 0;
		intf.inAclStart = 0;
		intf.outAclEnd = 0;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		
		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );			
		/**********************************************************************
		Expect  forward to CPU
		**********************************************************************/
		/* Set permit rule. */
		bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
		rule.actionType_ = RTL8651_ACL_PERMIT;
		rule.pktOpApp = RTL865XC_ACLTBL_ALL_LAYER;
		rule.ruleType_ = RTL8651_ACL_MAC;
		rtl8651_setAsicAclRule( 0, &rule);
		rtl8651_setAsicPortPatternMatch(0,0xff,0xff,2);
		memset( &hsb.da[0], 0xa0, sizeof(intf.macAddr) );
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be CPU, but forward  ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Expect  Drop
		**********************************************************************/
		rtl8651_setAsicPortPatternMatch(0,0xff,0xff,0);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be CPU, but forward  ....", hsa.dp,0, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Expect Mirror to CPU
		**********************************************************************/
		rtl8651_setAsicPortPatternMatch(0,0xff,0xff,1);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should mirror to CPU, but forward  ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_NOTPHY, __FUNCTION__, __LINE__,i);
		IS_EQUAL_INT_DETAIL("Pakcet should mirror to CPU, but forward  ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i);


	}		
	
	retval=SUCCESS;
	return retval;

}

int32 testLayer2PriorityTag(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 124,
		vid: 0x00,		tagif: 1,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch:0,		ethtype: 0x8137,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x21,0x11,0x00,0x00},
		hiprior:3,			snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch:0,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		AllPortForward(RTL8651_PORTSTA_FORWARDING);	
		WRITE_MEM32(MSCR,READ_MEM32(MSCR)|(EN_L2));
		for (j=0;j<3;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
#if 0 /* Louis: this is dummy code since vlan is not set */
		rtl865xc_setAsicVlan(&vlan);
#endif
		memset( &vlan, 0x0, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		rtl8651_setAsicVlan(4095,&vlan);
		/**********************************************************************
		Vid 0
		Expect match port base vid and forward
		**********************************************************************/
		hsb.tagif=1;
		hsb.vid=0;
		hsb.spa=1;
		hsb.ethtype=0x0806;
		hsb.sa[0]=0x0;hsb.sa[1]=0xff;
		hsb.sa[2]=0xff;hsb.sa[3]=0x00;
		hsb.sa[4]=0x0;hsb.sa[5]=0x00;
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but forward  ....", hsa.dp,0x0, __FUNCTION__, __LINE__,i);
		
		WRITE_MEM32( VCR0,READ_MEM32(VCR0) | EN_ALL_PORT_VLAN_INGRESS_FILTER);	
		memset( &vlan, 0x0, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;		
		rtl8651_setAsicVlan(8,&vlan);
		retval = virtualMacSetHsb( &hsb );
		assert( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		assert( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward, but   ....", hsa.dp,PM_PORT_0|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);


		/**********************************************************************
		 hsb.ethtype=PPPOECONTROL_ETHERTYPE;		 
		 expect match protcol base vid and forward
		**********************************************************************/		
		rtl8651_setProtocolBasedVLAN(RTL8651_PBV_RULE_PPPOE_CONTROL, 0, TRUE, 1024 );	
		hsb.ethtype=PPPOECONTROL_ETHERTYPE;
		hsb.spa=0;
		bzero(&vlan, sizeof(vlan));
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4;
		rtl8651_setAsicVlan(1024,&vlan);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ,but....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4, __FUNCTION__, __LINE__,i);
		/**********************************************************************
			 expect match vid 4095  and forward
		 **********************************************************************/		
		hsb.tagif=1;
		hsb.vid=4095;
		hsb.spa=1;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward only to  0x3d, but forward  ....", hsa.dp,0x3d, __FUNCTION__, __LINE__,i);
		

	}		
	
	retval=SUCCESS;
	return retval;

}

		
int32 testLayer2Forward(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x08,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch:0,		ethtype: 0x8137,
		da:{0x01,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x00,0x00,0x88,0x89},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch:0,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i;
	rtl865x_tblAsicDrv_l2Param_t l2t;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	ether_addr_t mac;
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		AllPortForward(RTL8651_PORTSTA_FORWARDING);			
		rtl8651_setAsicOperationLayer(2);		
		/**********************************************************************
		Expect Drop		
		**********************************************************************/
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));		
		bzero((void*) &l2t, sizeof(l2t));
		strtomac(&l2t.macAddr,"01-ff-ff-ff-ff-ff");
		l2t.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_4;
		l2t.isStatic = 1;
		l2t.auth=0;
		l2t.nhFlag=TRUE;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr("01-ff-ff-ff-ff-ff"), 0, &l2t);		
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-00-00-88-89"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"01-FF-FF-FF-FF-FF"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be drop, but forward  ....", hsa.dp,0x0, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Conti last case.
		set VLAN 0 member port 0x3f		
		Expect forward 
		**********************************************************************/
		memset( &vlan, 0x0, sizeof(vlan) );
		vlan.memberPortMask =PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		rtl8651_setAsicVlan(0,&vlan);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );	
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward only to port list ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_4, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Expect: unknown broadcat
		**********************************************************************/
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"F0-F0-F0-F0-F0-F0"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5, __FUNCTION__, __LINE__,i);

	}		
	
	retval=SUCCESS;
	return retval;

}
int32 testReservedMulticast(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 124,
		vid: 0x00,		tagif: 1,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch:0,		ethtype: 0x8137,
		da:{0xff,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x21,0x11,0x00,0x00},
		hiprior:3,			snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch:0,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i,j;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	ether_addr_t mac;

	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		for (j=0;j<4;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );
		rtl8651_setAsicOperationLayer(3);
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));		
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 0;
		intf.inAclStart = 0;
		intf.outAclEnd = 0;
		intf.outAclStart = 0;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		
		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );			
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-00
		Expect:  CPU
		**********************************************************************/		
		WRITE_MEM32(RMACR ,MADDR00);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"01-80-C2-00-00-00"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-00
		Expect:  Forward
		**********************************************************************/		
		WRITE_MEM32(RMACR ,0);
		WRITE_MEM32(MSCR,READ_MEM32(MSCR)|EN_STP);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-02
		Expect:  To CPU
		**********************************************************************/
		WRITE_MEM32(RMACR ,MADDR02);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"01-80-C2-00-00-02"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-02
		Expect:  Forward
		**********************************************************************/
		WRITE_MEM32(RMACR ,0);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-03
		Expect:  To CPU
		**********************************************************************/
		WRITE_MEM32(RMACR ,MADDR03);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"01-80-C2-00-00-03"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-03
		Expect:  Forward
		**********************************************************************/
		WRITE_MEM32(RMACR ,0);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-0E
		Expect:  To CPU
		**********************************************************************/
		WRITE_MEM32(RMACR ,MADDR0E);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"01-80-C2-00-00-0E"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-0E
		Expect:  Forward
		**********************************************************************/
		WRITE_MEM32(RMACR ,0);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-10
		Expect:  To CPU
		**********************************************************************/
		WRITE_MEM32(RMACR ,MADDR10);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"01-80-C2-00-00-10"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-10
		Expect:  Forward
		**********************************************************************/
		WRITE_MEM32(RMACR ,0);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-20
		Expect:  To CPU
		**********************************************************************/
		WRITE_MEM32(RMACR ,MADDR20);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"01-80-C2-00-00-20"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be only to CPU ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-20
		Expect:  Forward
		**********************************************************************/
		WRITE_MEM32(RMACR ,0);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-21
		Expect:  To CPU
		**********************************************************************/
		WRITE_MEM32(RMACR ,MADDR21);
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"01-80-C2-00-00-21"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  to CPU ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i);
		/**********************************************************************
		Multicast Address 01-80-C2-00-00-21
		Expect:  Forward
		**********************************************************************/
		WRITE_MEM32(RMACR ,0);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);
		
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"01-80-C2-00-00-00"),sizeof(ether_addr_t));
		for (j=1;j<=0x10;j++)
		{
			/**********************************************************************
			Multicast Address 01-80-C2-00-00-01 ~ 01-80-C2-00-00-16
			Expect: To CPU
			**********************************************************************/
			WRITE_MEM32(RMACR ,MADDR00_10);
			hsb.da[5] ++;
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );	
			IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i);
			/**********************************************************************
			Multicast Address 01-80-C2-00-00-01 ~ 01-80-C2-00-00-16
			Expect:  Forward
			**********************************************************************/
			WRITE_MEM32(RMACR ,0);
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );	
			IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);
		}

		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"01-80-C2-00-00-20"),sizeof(ether_addr_t));
		for (j=0x20;j<=0x2F;j++)
		{
			/**********************************************************************
			Multicast Address 01-80-C2-00-00-20 ~ 01-80-C2-00-00-2F
			Expect:  To CPU
			**********************************************************************/
			WRITE_MEM32(RMACR ,MADDR20_2F);
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );	
			IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dpext,PM_PORT_CPU, __FUNCTION__, __LINE__,i);
			/**********************************************************************
			Multicast Address 01-80-C2-00-00-20 ~ 01-80-C2-00-00-2F
			Expect:  Forward
			**********************************************************************/
			WRITE_MEM32(RMACR ,0);
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );	
			IS_EQUAL_INT_DETAIL("Pakcet should be forward  ....", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);
			hsb.da[5] ++;
		}



	}		
	
	retval=SUCCESS;
	return retval;


}
int32 testPrioritySelection(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 124,
		vid: 0x00,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_IP,
		patmatch:0,		ethtype: 0x0800,
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
	int i,j;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	_rtl8651_tblDrvAclRule_t aclt;
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_51B_CPU_REASON);
		for (j=0;j<4;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );

		rtl8651_setAsicOperationLayer(2);
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 1;
		intf.inAclStart = 0;
		intf.outAclEnd = 1;
		intf.outAclStart = 1;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		
		{
			int portIdx;

			for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
			{
				rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
			}
		}

		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );	

		
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 	= RTL8651_ACL_MAC;
		aclt.actionType_ 	= RTL8651_ACL_PRIORITY;
		aclt.priority		= 4;
		aclt.pktOpApp	 	= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1,&aclt);		


		hsb.tagif=0;
		hsb.vid=0;
		WRITE_MEM32(QIDDPCR,(1<<PBP_PRI_OFFSET));
		WRITE_MEM32(PBPCR,7);
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("HP should be 7 ....", hsa.priority,7, __FUNCTION__, __LINE__,i);
		IS_EQUAL_INT_DETAIL("DP should be 0xe...", hsa.dp,PM_PORT_1|PM_PORT_2|PM_PORT_3, __FUNCTION__, __LINE__,i);


		WRITE_MEM32(QIDDPCR,(1<<BP8021Q_PRI_OFFSET));
		WRITE_MEM32(LPTM8021Q ,READ_MEM32(LPTM8021Q) |( 6<<EN_8021Q2LTMPRI6));
		hsb.tagif=1;
		hsb.vid=8;
		hsb.hiprior=6;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("HP should be 6 ....", hsa.priority, 6 , __FUNCTION__, __LINE__,i);
		

		WRITE_MEM32(QIDDPCR,(1<<DSCP_PRI_OFFSET));
		WRITE_MEM32(DSCPCR0,READ_MEM32(DSCPCR0)|(5<<DSCP1_PRI));		
		hsb.iptos=0x5;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("HP should be 5 ....", hsa.priority, 5 , __FUNCTION__, __LINE__,i);


		WRITE_MEM32(QIDDPCR,(1<<ACL_PRI_OFFSET));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("HP should be 4 ....", hsa.priority, 4 , __FUNCTION__, __LINE__,i);


		
	}
	retval=SUCCESS;
	return retval;
}

int testRandomPrioritySelection(uint32 caseNo)
{

	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 124,
		vid: 0x00,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_IP,
		patmatch:0,		ethtype: 0x0800,
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
	int i,j,loop,result[4];
	_rtl8651_tblDrvAclRule_t aclt;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	int aclPriority,dscpPrority,portPriority,dpcr,lptm;
	int spa,vlan1Vid,vlan2Vid,lan1Member;
	int extSpaArray[4]={0,1,2,2};
	portProtocolBaseInfo	portsInfo[9];

	retval = model_setTestTarget( IC_TYPE_REAL );
	if ( retval == VMAC_ERROR_NON_AVAILABLE ) 
	{
		rtlglue_printf("Need Real IC\n");
		return SUCCESS;
	}

	retval = model_setTestTarget( IC_TYPE_MODEL );
	if ( retval == VMAC_ERROR_NON_AVAILABLE ) 
	{
		rtlglue_printf("Need MODEL Code\n");
		return SUCCESS;
	}

for (loop=0;loop<=RANDOM_RUN_NUM;loop++)
	{
		rtlglue_printf("--- %d --------------------------\n", loop );
		aclPriority=rtlglue_random()&0x7;
		dpcr=(rtlglue_random()&(NAPT_PRI_MASK|ACL_PRI_MASK|DSCP_PRI_MASK|BP8021Q_PRI_MASK|PBP_PRI_MASK))|(1<<PBP_PRI_OFFSET); /* At least PBP is set */
		portPriority=rtlglue_random();
		dscpPrority=rtlglue_random();
		lptm=rtlglue_random();
		spa=rtlglue_random()&0x7;
		while(spa==6)
		{
			spa =spa-(rtlglue_random()&0x3);			
		}
		if (spa==7)
		{
			hsb.extspa=extSpaArray[rtlglue_random()&0x3];
		}		
		for (j=0;j<9;j++)
		{
			portsInfo[j].protcol=(rtlglue_random()%6)+1; /* from 1 to 6 */
			portsInfo[j].valid=rtlglue_random()&0x1;
			portsInfo[j].vlanId=rtlglue_random()&0xFFF;
			portsInfo[j].priority=rtlglue_random()&0x7;		
			portsInfo[j].vlanMember=rtlglue_random()&0x1FFF;
		}
		hsb.tagif = rtlglue_random()&0x1;
		hsb.vid = rtlglue_random()&0xFFF;
		hsb.ethtype= rtlglue_random()&0xFFFF;
		hsb.hiprior= rtlglue_random()&0x7;
		hsb.llcothr= rtlglue_random()&0x1;
		if (hsb.llcothr)
			hsb.snap=0;
		else 
			hsb.snap= rtlglue_random()&0x1;
		hsb.spa =spa;
		hsb.iptos=rtlglue_random()&0xff;
		vlan2Vid=rtlglue_random()&0xfff;
		vlan1Vid=rtlglue_random()&0xfff;
		lan1Member = rtlglue_random()&0x1FF;
#ifdef VERBOSE

		rtlglue_printf("vlan1Vid=%d;",vlan1Vid);
		rtlglue_printf("vlan2Vid=%d;\n",vlan2Vid);
		rtlglue_printf("dpcr=0x%x;\n",dpcr);
		rtlglue_printf("lptm=0x%x;\n",lptm);
		rtlglue_printf("portPriority=0x%x;\n",portPriority);
		rtlglue_printf("dscpPrority=0x%x;\n",dscpPrority);
		rtlglue_printf("aclPriority=0x%x;\n",aclPriority);
		rtlglue_printf("lan1Member=0x%x;\n",lan1Member);
		rtlglue_printf("hsb.tagif=0x%x;",hsb.tagif);		
		rtlglue_printf("hsb.vid=0x%x; ",hsb.vid);
		rtlglue_printf("hsb.spa=%d;",hsb.spa);
		rtlglue_printf("hsb.snap=%d;\n",hsb.snap);
		rtlglue_printf("hsb.hiprior=%d;\n",hsb.hiprior);
		rtlglue_printf("hsb.ethtype=0x%x; ",hsb.ethtype);
		rtlglue_printf("hsb.llcothr=%d; ",hsb.llcothr);
		rtlglue_printf("hsb.extspa=0x%x;\n",hsb.extspa);
		rtlglue_printf("hsb.iptos=0x%x;\n",hsb.iptos);
		for (j=0;j<9;j++)
		{
				rtlglue_printf("portsInfo[%d].protcol=%d;",j,portsInfo[j].protcol);
				rtlglue_printf("portsInfo[%d].valid=%d;\n",j,portsInfo[j].valid);
				rtlglue_printf("portsInfo[%d].vlanMember=%d;",j,portsInfo[j].vlanMember);
				rtlglue_printf("portsInfo[%d].vlanId=0x%x;\n",j,portsInfo[j].vlanId);				
				rtlglue_printf("portsInfo[%d].priority=0x%x;\n",j,portsInfo[j].priority);				
		}		
#endif
		for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
		{
			retval = model_setTestTarget( i );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
			
			rtl8651_clearAsicAllTable();
			rtl8651_clearRegister();
			TESTING_MODE;
			rtl8651_totalExtPortNum=3;			
			WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_51B_CPU_REASON|EN_FRAG_TO_ACLPT);
			for (j=0;j<4;j++)	WRITE_MEM32(( PVCR0+j*4), ((vlan1Vid<<0)|(vlan2Vid<<16)) );
	
			for (j=0;j<9;j++)
			{
				rtl8651_setProtocolBasedVLAN(portsInfo[j].protcol, j, portsInfo[j].valid, portsInfo[j].vlanId|(portsInfo[j].priority<<12) );					
				bzero(&vlan, sizeof(vlan));
				vlan.memberPortMask =portsInfo[j].vlanMember;
				rtl8651_setAsicVlan(portsInfo[j].vlanId,&vlan);
			}
			vlan.memberPortMask= (lan1Member ^ hsb.ethtype) &0x1FF;
#if 1 /* Louis: new code */
			rtl8651_setAsicVlan(hsb.vid,&vlan);
#else /* original code */
			vlan.vid=hsb.vid;			
			rtl865xc_setAsicVlan(&vlan);
#endif
			rtl8651_defineProtocolBasedVLAN(RTL8651_PBV_RULE_USR1,0 ,hsb.ethtype);
			rtl8651_setProtocolBasedVLAN(RTL8651_PBV_RULE_USR1, hsb.spa, portsInfo[spa].valid, portsInfo[spa].vlanId |(portsInfo[spa].priority<<12));	
			rtl8651_setAsicOperationLayer(2);
			if (portsInfo[0].protcol ^ portsInfo[1].protcol)
				WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));		
			bzero(&intf, sizeof(intf));
			intf.vid = vlan1Vid;
			intf.valid = 1;
			intf.enableRoute = FALSE;
			intf.inAclEnd = 1;
			intf.inAclStart = 0;
			intf.outAclEnd = 1;
			intf.outAclStart = 1;
			memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
			intf.macAddrNumber = 1;
			intf.mtu = 1500;
			rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
			
			{
				int portIdx;

				for (portIdx = 0 ; portIdx < RTL865XC_PORT_NUMBER ; portIdx ++ )
				{
					rtl865xC_setAsicSpanningTreePortState(portIdx, RTL8651_PORTSTA_FORWARDING);
				}
			}

			bzero( &vlan, sizeof(vlan) );
			vlan.memberPortMask = lan1Member;
			vlan.untagPortMask = 0;
			rtl8651_setAsicVlan( intf.vid, &vlan );				
#if 1 /* Louis: new code */
			rtl8651_setAsicVlan( hsb.vid, &vlan );				
#else /* original code */
			vlan.vid=hsb.vid;
			rtl865xc_setAsicVlan( &vlan );				
#endif
			memset((void*)&aclt, 0, sizeof(aclt));
			aclt.ruleType_ 	= RTL8651_ACL_MAC;
			aclt.actionType_ 	= RTL8651_ACL_PRIORITY;
			aclt.priority		= aclPriority;
			aclt.pktOpApp	 	= RTL865XC_ACLTBL_ALL_LAYER;
			rtl8651_setAsicAclRule(0, &aclt);			
			 memset((void*)&aclt, 0, sizeof(aclt));
			aclt.ruleType_ = RTL8651_ACL_MAC;
			aclt.actionType_ = RTL8651_ACL_PERMIT;
			aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
			rtl8651_setAsicAclRule(1,&aclt);		
			WRITE_MEM32(LPTM8021Q, lptm);
			WRITE_MEM32(PBPCR, portPriority);
			WRITE_MEM32(DSCPCR0,dscpPrority^portPriority);
			WRITE_MEM32(DSCPCR1,dscpPrority^aclPriority);
			WRITE_MEM32(DSCPCR2,dscpPrority^dpcr);
			WRITE_MEM32(DSCPCR3,dscpPrority^lptm);
			WRITE_MEM32(DSCPCR4,dscpPrority^dscpPrority);
			WRITE_MEM32(DSCPCR5,dscpPrority^vlan1Vid);
			WRITE_MEM32(DSCPCR6,dscpPrority^vlan2Vid);
			WRITE_MEM32(QIDDPCR,dpcr);
			WRITE_MEM32(UPTCMCR0,dscpPrority^portPriority);
			WRITE_MEM32(UPTCMCR1,dscpPrority^aclPriority);
			WRITE_MEM32(UPTCMCR2,dscpPrority^dpcr);
			WRITE_MEM32(UPTCMCR3,dscpPrority^lptm);
			WRITE_MEM32(UPTCMCR4,dscpPrority^dscpPrority);
			WRITE_MEM32(UPTCMCR5,dscpPrority^vlan1Vid);
			WRITE_MEM32(QNUMCR, P0QNum_1|P1QNum_1|P2QNum_1|P3QNum_1|P4QNum_1|P5QNum_1);
			retval = virtualMacSetHsb( &hsb );
			ASSERT( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			ASSERT( retval == SUCCESS );	
			result[i] 	= hsa.priority;
			result[i+2] = hsa.dp;
		}
		if (result[2])
		{
			IS_EQUAL_INT("Priority  should be the same ....  ", result[0],result[1], __FUNCTION__, __LINE__);
			IS_EQUAL_INT("DP should be the same ....  ", result[2],result[3], __FUNCTION__, __LINE__);
		}


	}
	return retval;
}

int32 testSpanningTree(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 124,
		vid: 0x00,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_IP,
		patmatch:0,		ethtype: 0x0800,
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
	int i,j;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	_rtl8651_tblDrvAclRule_t aclt;
	ether_addr_t  mac;	
		
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_51B_CPU_REASON);
		for (j=0;j<4;j++)	WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );

		WRITE_MEM32(MSCR,READ_MEM32(MSCR)|EN_STP);
		rtl8651_setAsicOperationLayer(2);
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 1;
		intf.inAclStart = 0;
		intf.outAclEnd = 1;
		intf.outAclStart = 1;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );

		rtl865xC_setAsicSpanningTreePortState(0, RTL8651_PORTSTA_DISABLED);
		rtl865xC_setAsicSpanningTreePortState(1, RTL8651_PORTSTA_BLOCKING);
		rtl865xC_setAsicSpanningTreePortState(2, RTL8651_PORTSTA_LISTENING);
		rtl865xC_setAsicSpanningTreePortState(3, RTL8651_PORTSTA_LEARNING);
		rtl865xC_setAsicSpanningTreePortState(4, RTL8651_PORTSTA_FORWARDING);
		rtl865xC_setAsicSpanningTreePortState(5, RTL8651_PORTSTA_FORWARDING);
		rtl865xC_setAsicSpanningTreePortState(6, RTL8651_PORTSTA_FORWARDING);
		rtl865xC_setAsicSpanningTreePortState(7, RTL8651_PORTSTA_FORWARDING);
		rtl865xC_setAsicSpanningTreePortState(8, RTL8651_PORTSTA_FORWARDING);

		bzero( &vlan, sizeof(vlan) );
		/* Ambiguous  CPU PORT  Expression and Extenstion PORT  */
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.memberPortMask |=(PM_PORT_EXT0 |PM_PORT_EXT1|PM_PORT_EXT2)>>1;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );	
		rtl8651_totalExtPortNum=3;
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 	= RTL8651_ACL_MAC;
		aclt.actionType_ 	= RTL8651_ACL_PRIORITY;
		aclt.priority		= 4;
		aclt.pktOpApp	 	= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1,&aclt);		

		/**********************************************************************
		Expect:  Drop  bcaause port status is disable		
		**********************************************************************/
		hsb.spa=0;
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-FF-FF-00-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"FF-FF-FF-FF-FF-FF"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  drop ....", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		Expect:  Drop  bcaause port status is blocking
		**********************************************************************/		
		hsb.spa=1;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  drop ....", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		Expect:  Drop  because port status is listening	
		**********************************************************************/
		hsb.spa=2;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  drop ....", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		Expect:  Drop  because port status is learning state
		**********************************************************************/	
		hsb.spa=3;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  drop ....", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		Expect:  Drop  bcaause 	 Destination Port  3 is learning state
		**********************************************************************/
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-FF-FF-FF-FF-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-00-FF-FF-00-00"),sizeof(ether_addr_t));		
		hsb.spa=4;
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  drop ....", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
		/**********************************************************************
		Expect:  Forward and Broadcast to   member ports
		**********************************************************************/
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-FF-FF-FF-FF-00"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"FF-FF-FF-FF-FF-FF"),sizeof(ether_addr_t));		
		hsb.spa=4;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("DP should be  forward  ....", hsa.dp,PM_PORT_5, __FUNCTION__, __LINE__ ,i);

		/**********************************************************************
		Expect:  Forward  (the mac address learn )
		**********************************************************************/
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-FF-FF-FF-FF-FF"),sizeof(ether_addr_t));		
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-FF-FF-FF-FF-00"),sizeof(ether_addr_t));
		hsb.spa=5;
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should be  forward ....", hsa.dp,PM_PORT_4, __FUNCTION__, __LINE__ ,i);


		
	}
	retval=SUCCESS;
	return retval;

}


/*
 *  In this case, we will test random cobination of Spanning Tree configuration.
 */
int32 testRandomSpanningTree(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 124,
		vid: 0x00,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: HSB_TYPE_IP,
		patmatch:0,		ethtype: 0x0800,
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
	int i,j;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	_rtl8651_tblDrvAclRule_t aclt;
	ether_addr_t  mac;	
	uint32 portST[RTL8651_MAC_NUMBER+3/*extension port*/];
	uint32 pmFwd; /* Port Masks those allow forward */
	uint32 cntLoop;

	/* Configure environment */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		WRITE_MEM32(SWTCR1,READ_MEM32(SWTCR1)|EN_51B_CPU_REASON);
		for (j=0;j<=4;j++) WRITE_MEM32(( PVCR0+j*4), ((0x8<<0)|(0x8<<16)) );

		WRITE_MEM32(MSCR,READ_MEM32(MSCR)|EN_STP);
		rtl8651_setAsicOperationLayer(2);
		bzero(&intf, sizeof(intf));
		intf.vid = 8;
		intf.valid = 1;
		intf.enableRoute = FALSE;
		intf.inAclEnd = 1;
		intf.inAclStart = 0;
		intf.outAclEnd = 1;
		intf.outAclStart = 1;
		memset( &intf.macAddr, 0xa5, sizeof(intf.macAddr) );
		intf.macAddrNumber = 1;
		intf.mtu = 1500;
		rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );

		bzero( &vlan, sizeof(vlan) );
		/* Ambiguous  CPU PORT  Expression and Extenstion PORT  */
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.memberPortMask |=(PM_PORT_EXT0 |PM_PORT_EXT1|PM_PORT_EXT2)<<RTL8651_PORT_NUMBER;
		vlan.untagPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );	
		rtl8651_totalExtPortNum=3;
		memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ 	= RTL8651_ACL_MAC;
		aclt.actionType_ 	= RTL8651_ACL_PRIORITY;
		aclt.priority		= 4;
		aclt.pktOpApp	 	= RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(0, &aclt);			
		 memset((void*)&aclt, 0, sizeof(aclt));
		aclt.ruleType_ = RTL8651_ACL_MAC;
		aclt.actionType_ = RTL8651_ACL_PERMIT;
		aclt.pktOpApp	 = RTL865XC_ACLTBL_ALL_LAYER;
		rtl8651_setAsicAclRule(1,&aclt);		
	}
	
	for( cntLoop = 0; cntLoop < RANDOM_RUN_NUM; cntLoop++ )
	{
		/* Randomize the configuration */
		pmFwd = 0;
		for( i = 0; i < RTL8651_MAC_NUMBER+3/*extension port*/; i++ )
		{
			switch( rtlglue_random()&3 )
			{
				case 0: portST[i] = STP_PortST_DISABLE; break;
				case 1: portST[i] = STP_PortST_LISTENING; break;
				case 2: portST[i] = STP_PortST_LEARNING; break;
				default: portST[i] = STP_PortST_FORWARDING; pmFwd|=(1<<i); break;
			}
		}
			
		/* Compare the HSA of expected, model, FPGA, and real IC. */
		for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
		{
			retval = model_setTestTarget( i );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
			
			WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~STP_PortST_MASK)|portST[0] );
			WRITE_MEM32( PCRP1, (READ_MEM32(PCRP1)&~STP_PortST_MASK)|portST[1] );
			WRITE_MEM32( PCRP2, (READ_MEM32(PCRP2)&~STP_PortST_MASK)|portST[2] );
			WRITE_MEM32( PCRP3, (READ_MEM32(PCRP3)&~STP_PortST_MASK)|portST[3] );
			WRITE_MEM32( PCRP4, (READ_MEM32(PCRP4)&~STP_PortST_MASK)|portST[4] );
			WRITE_MEM32( PCRP5, (READ_MEM32(PCRP5)&~STP_PortST_MASK)|portST[5] );
			WRITE_MEM32( PCRP6, (READ_MEM32(PCRP6)&~STP_PortST_MASK)|portST[6] );
			WRITE_MEM32( PCRP7, (READ_MEM32(PCRP7)&~STP_PortST_MASK)|portST[7] );
			WRITE_MEM32( PCRP8, (READ_MEM32(PCRP8)&~STP_PortST_MASK)|portST[8] );

			for( j = 0; j < RTL8651_MAC_NUMBER+3/*extension port*/; j++ )
			{
				if ( j < RTL8651_MAC_NUMBER )
					hsb.spa = j;
				else
				{
					hsb.spa = 7/*CPU*/;
					hsb.extspa = j-RTL8651_MAC_NUMBER;
				}
				/* rtlglue_printf( "hsb.spa=%d hsb.expspa=%d\n", hsb.spa, hsb.extspa ); */

				switch ( portST[j] )
				{
					case STP_PortST_DISABLE:
						memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-01-02-00-00"),sizeof(ether_addr_t));
						memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-00-03-04-00-00"),sizeof(ether_addr_t));
						retval = virtualMacSetHsb( &hsb );
						ASSERT( retval == SUCCESS );
						retval = virtualMacGetHsa( &hsa );
						ASSERT( retval == SUCCESS );	
						IS_EQUAL_INT_DETAIL("Pakcet should be  drop ....", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
						break;

					case STP_PortST_LISTENING:
						retval = virtualMacSetHsb( &hsb );
						ASSERT( retval == SUCCESS );
						retval = virtualMacGetHsa( &hsa );
						ASSERT( retval == SUCCESS );	
						IS_EQUAL_INT_DETAIL("Pakcet should be  drop ....", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
						break;

					case STP_PortST_LEARNING:
						retval = virtualMacSetHsb( &hsb );
						ASSERT( retval == SUCCESS );
						retval = virtualMacGetHsa( &hsa );
						ASSERT( retval == SUCCESS );	
						IS_EQUAL_INT_DETAIL("Pakcet should be  drop ....", hsa.dp,0, __FUNCTION__, __LINE__ ,i);
						break;

					case STP_PortST_FORWARDING: /* fall thru */
					default:
						memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-01-02-00-00"),sizeof(ether_addr_t));
						memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"00-00-03-04-00-00"),sizeof(ether_addr_t));
						retval = virtualMacSetHsb( &hsb );
						ASSERT( retval == SUCCESS );
						retval = virtualMacGetHsa( &hsa );
						ASSERT( retval == SUCCESS );

						/* If after egress filtering there is no port allowing forward, we */
						IS_EQUAL_INT_DETAIL("DP should be forward (Physical Port Part) ....", hsa.dp&0x3f,(pmFwd&~(1<<j))&0x3f/*physical port part*/, __FUNCTION__, __LINE__ ,i);
						IS_EQUAL_INT_DETAIL("DP should be forward (CPU Port Only) ....", hsa.dp&0x40,((pmFwd&~(1<<j))&~0x3f)?0x40/*CPU port*/:0x00, __FUNCTION__, __LINE__ ,i);
						IS_EQUAL_INT_DETAIL("DPEXT should be  forward  ....", hsa.dpext,(pmFwd&~(1<<j))>>6/*extension port part*/, __FUNCTION__, __LINE__ ,i);
						break;
				}
			}
		}
	}
	retval=SUCCESS;
	return retval;

}


/*
 *  Set L2.cpu bit to trap packet to CPU (for dial on demand).
 *  1. insert a L2 entry with cpu=1 (also p0, p1, p2, p3)
 *  2. send a packet, and expect to CPU and hsa.hwfwrd=1.
 *  3. insert a L2 entry with cpu=1 (without any port)
 *  4. send a packet, and expect to CPU and hsa.hwfwrd=0.
 */
int32 testLayer2TrapToCpu(uint32 caseNo)
{
	int32 retval;
	hsb_param_t hsb =
	{
		spa: 0,			len: 64,
		vid: 0x08,		tagif: 0,
		pppoeif: 0,		sip: 0x11223344,
		sprt: 0x5566,		dip: 0x778899aa,
		dprt: 0xbbcc,		ipptl: 0xdd,
		ipfg: 0x0,			iptos: 0x65,
		tcpfg: 0x00,		type: 0,
		patmatch:0,		ethtype: 0x8137,
		da:{0x01,0xff,0xff,0xff,0xff,0xff},
		sa:{0x00,0x00,0x00,0x00,0x88,0x89},
		hiprior: 0,		snap: 0,
		udpnocs: 0,		ttlst: 2,
		dirtx: 0,			l3csok: 1,
		l4csok: 1,		ipfo0_n: 1,
		llcothr: 0,			urlmch:0,
		extspa: 0,		extl2: 0
	};
	hsa_param_t hsa;
	int i;
	rtl865x_tblAsicDrv_l2Param_t l2t;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	ether_addr_t mac;
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		TESTING_MODE;
		AllPortForward(RTL8651_PORTSTA_FORWARDING);			
		rtl8651_setAsicOperationLayer(2);	
		
		/**********************************************************************
		Config VLAN Table
		**********************************************************************/
		memset( &vlan, 0x0, sizeof(vlan) );
		vlan.memberPortMask =PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		rtl8651_setAsicVlan(0,&vlan);

		/**********************************************************************
		 *  1. insert a L2 entry with cpu=1 (also p0, p1, p2, p3)
		 **********************************************************************/
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));		
		bzero((void*) &l2t, sizeof(l2t));
		strtomac(&l2t.macAddr,"02-34-56-78-9a-bc");
		l2t.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_4;
		l2t.isStatic = 1;
		l2t.cpu = 1;
		l2t.auth=0;
		l2t.nhFlag=TRUE;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr("02-34-56-78-9a-bc"), 0, &l2t);		
		
		/**********************************************************************
		 *  2. send a packet, and expect to CPU and hsa.hwfwrd=1.
		 **********************************************************************/
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-00-00-88-89"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"02-34-56-78-9a-bc"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("Pakcet should include CPU port", (hsa.dp&PM_PORT_NOTPHY), PM_PORT_NOTPHY, __FUNCTION__, __LINE__, i);
		IS_EQUAL_INT_DETAIL("hsa.hwfwrd should also be set", (hsa.hwfwrd), 1, __FUNCTION__, __LINE__, i);

		/**********************************************************************
		 *  3. insert a L2 entry with cpu=1 (without any port)
		 **********************************************************************/
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));		
		bzero((void*) &l2t, sizeof(l2t));
		strtomac(&l2t.macAddr,"02-34-56-78-9a-bc");
		l2t.memberPortMask = 0;
		l2t.isStatic = 1;
		l2t.cpu = 1;
		l2t.auth=0;
		l2t.nhFlag=TRUE;
		rtl8651_setAsicL2Table(getL2TableRowFromMacStr("02-34-56-78-9a-bc"), 0, &l2t);		
		
		/**********************************************************************
		 *  4. send a packet, and expect to CPU and hsa.hwfwrd=0.
		 **********************************************************************/
		memcpy((void*)&hsb.sa[0],(void*)strtomac(&mac,"00-00-00-00-88-89"),sizeof(ether_addr_t));
		memcpy((void*)&hsb.da[0],(void*)strtomac(&mac,"02-34-56-78-9a-bc"),sizeof(ether_addr_t));
		retval = virtualMacSetHsb( &hsb );
		ASSERT( retval == SUCCESS );
		retval = virtualMacGetHsa( &hsa );
		ASSERT( retval == SUCCESS );	
		IS_EQUAL_INT_DETAIL("hsa.dpc should be 1", hsa.dpc, 1, __FUNCTION__, __LINE__, i);
		IS_EQUAL_INT_DETAIL("Pakcet should be forwarded to CPU port only", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__, i);
		IS_EQUAL_INT_DETAIL("hsa.hwfwrd should be set (means ASIC has dealed with this packet)", (hsa.hwfwrd), 1, __FUNCTION__, __LINE__, i);
	}		
	
	retval=SUCCESS;
	return retval;

}
