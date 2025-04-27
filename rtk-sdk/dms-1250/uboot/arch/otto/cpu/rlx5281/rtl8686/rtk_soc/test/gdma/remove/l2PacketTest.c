/*
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
*
* Program : Model Code for 865xC
* Abstract :
* Author : (rupert@realtek.com.tw)
* $Id: l2PacketTest.c,v 1.7 2006-08-29 12:59:59 chenyl Exp $
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
#include "l2PacketTest.h"
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

/***********************************************************************
 *  This is example code is golden sample.
 *
 *  In this case, we will demo how to input a packet with virtualMacInput(),
 *  and retrieve the output packet with virtualMacOutput().
 ***********************************************************************/
int32 testLayer2Forward2(uint32 caseNo)
{
	int32 retval;
	enum IC_TYPE i;
	rtl865x_tblAsicDrv_l2Param_t l2t;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	uint8 *pkt0, *pkt1;
	int32 len0, len1;
	enum PORT_MASK portmask;
	rtl8651_PktConf_t pktconf;

	pkt0 = sharedPkt[0];
	pkt1 = sharedPkt[1];
	memset( pkt0, 0xff, _PKT_LEN );
	memset( pkt1, 0xff, _PKT_LEN );
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		rtlglue_printf( "model_setTestTarget('%s')\n", i==IC_TYPE_REAL?"REAL IC":"MODEL CODE" );
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		NORMAL_MODE;
		rtl8651_setAsicOperationLayer(2);
		
		/**********************************************************************
		 * set ASIC registers
		 **********************************************************************/
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));
		WRITE_MEM32(QNUMCR, P0QNum_1|P1QNum_1|P2QNum_1|P3QNum_1|P4QNum_1|P5QNum_1|P6QNum_1 );
		WRITE_MEM32(SIRR, READ_MEM32(SIRR)|TRXRDY );
		AllPortForward(RTL8651_PORTSTA_FORWARDING);	
	
		/**********************************************************************
		 * set L2 entry
		 **********************************************************************/
		bzero((void*) &l2t, sizeof(l2t));
		strtomac(&l2t.macAddr,"00-01-02-03-04-05");
		l2t.memberPortMask = PM_PORT_2;
		l2t.isStatic = 1;
		l2t.auth=0;
		l2t.nhFlag=TRUE;
		retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-01-02-03-04-05"), 0, &l2t);
		ASSERT( retval == SUCCESS );
		
		/**********************************************************************
		 * Set Net Interface 0
		 *---------------------------------------------------------------------*/
		bzero((void*) &intf, sizeof(intf));
		strtomac(&intf.macAddr, "00-00-10-11-12-20" );
		intf.macAddrNumber = 1;
		intf.vid = 8;
		intf.mtu = 1500;
		intf.inAclStart = 0;
		intf.inAclEnd = 0;
		intf.outAclStart = 0;
		intf.outAclEnd = 0;
		intf.enableRoute = 0;
		intf.valid = 1;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval == SUCCESS );

		/**********************************************************************
		 * set VLAN 8 member port 0x3f		
		 **********************************************************************/
		memset( &vlan, 0x0, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.untagPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		retval = rtl8651_setAsicVlan(8,&vlan);
		ASSERT( retval == SUCCESS );

		/**********************************************************************
		 * Port Based VLAN Control Register
		 **********************************************************************/
		WRITE_MEM32( PVCR0, (READ_MEM32(PVCR0)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port 0 */
		WRITE_MEM32( PVCR0, (READ_MEM32(PVCR0)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port 1 */
		WRITE_MEM32( PVCR1, (READ_MEM32(PVCR1)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port 2 */
		WRITE_MEM32( PVCR1, (READ_MEM32(PVCR1)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port 3 */
		WRITE_MEM32( PVCR2, (READ_MEM32(PVCR2)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port 4 */
		WRITE_MEM32( PVCR2, (READ_MEM32(PVCR2)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port 5 */
		WRITE_MEM32( PVCR3, (READ_MEM32(PVCR3)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port ext 1 */
		WRITE_MEM32( PVCR3, (READ_MEM32(PVCR3)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port ext 2 */
		WRITE_MEM32( PVCR4, (READ_MEM32(PVCR4)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port ext 3 */

		{
			int round;
			for( round = 0; round<1; round++ )
			{
				uint32 lenPayload;

#if 1 /* fixed length */
				lenPayload = 46; /* Just make for 64-bytes packet. */
				/* lenPayload = (16*1024-2-6-6-2-4); */
#elif 1 /* random for Giga Jumbo */
				lenPayload = rtlglue_random()%(16*1024-2-6-6-2-4+1);
				lenPayload = rtlglue_random()%(9216-6-6-2-4+1);
				/* lenPayload = 16383-6-6-2-4; */
				/* lenPayload = 9216-6-6-2-4; */
#else /* random for 100M packet */
				lenPayload = rtlglue_random()%(1522-6-6-2-4+1);
#endif
				rtlglue_printf( "[round=%d] lenPayload=%d\n", round, lenPayload );

				/**********************************************************************
				 * Generate sending packet
				 *---------------------------------------------------------------------*/
				memset( &pktconf, 0, sizeof(pktconf) );
				pktconf.pktType = _PKT_TYPE_ETHER;
				pktconf.l2Flag = 0;
				strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-05" );
				strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
				pktconf.conf_ethtype = 0x1234;
				pktconf.vlan.vid = 0x0;
				pktconf.vlan.cfi = 0;
				pktconf.vlan.prio = 0;
				pktconf.llc.dsap = 0;
				pktconf.llc.ssap = 0;
				pktconf.pppoe.type = 0;
				pktconf.pppoe.session = 0;
				pktconf.payload.length = lenPayload;
				pktconf.payload.content = "";
				len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
				ASSERT( len0 > 0 );
		
				/*---------------------------------------------------------------------
				 * Send to SWCORE
				 *---------------------------------------------------------------------*/
				retval = virtualMacInput( PN_PORT1, pkt0, len0 );
				ASSERT( retval == SUCCESS );
		
				/**********************************************************************
				 * Generate expected packet
				 *---------------------------------------------------------------------*/
				memset( &pktconf, 0, sizeof(pktconf) );
				pktconf.pktType = _PKT_TYPE_ETHER;
				pktconf.l2Flag = 0;
				strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-05" );
				strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
				pktconf.conf_ethtype = 0x1234;
				pktconf.vlan.vid = 0x0;
				pktconf.vlan.cfi = 0;
				pktconf.vlan.prio = 0;
				pktconf.llc.dsap = 0;
				pktconf.llc.ssap = 0;
				pktconf.pppoe.type = 0;
				pktconf.pppoe.session = 0;
				pktconf.payload.length = lenPayload;
				pktconf.payload.content = "";
				len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
				ASSERT( len0 > 0 );
		
				/*---------------------------------------------------------------------
				 * Output packet from SWCORE and Compare
				 *---------------------------------------------------------------------*/
				portmask = PM_PORT_ALL;
				len1 = _PKT_LEN; /* indicate buffer length */
				retval = virtualMacOutput( &portmask, pkt1, &len1 );
				ASSERT( retval == SUCCESS );
				IS_EQUAL_INT_DETAIL( "Pakcet should be forwarded to port list ....", portmask, PM_PORT_2, __FUNCTION__, __LINE__, i );
				ASSERT( 0==memComp( pkt0, pkt1, max(len0,len1), "Packet content is not equal!" ) );
				IS_EQUAL_INT_DETAIL( "Output packet length is different ...", len0, len1, __FUNCTION__, __LINE__, i );
			}		
		}
	}
	
	retval=SUCCESS;
	return retval;

}


/***********************************************************************
 *
 *  This is example code for RANDOM pattern test bench.
 *
 *  In this case, we will input a packet with virtualMacInput(),
 *  and retrieve the output packet with virtualMacOutput().
 ***********************************************************************/
int32 testLayer2Forward3(uint32 caseNo)
{
	int32 retval;
	int i;
	enum IC_TYPE ic_type[2] = { IC_TYPE_REAL, IC_TYPE_MODEL };
	rtl865x_tblAsicDrv_l2Param_t l2t;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	uint8 *pkt[2];
	int32 len[2];
	int32 pktout[2]; /* record if any packet out */
	enum PORT_MASK portmask[2];
	rtl8651_PktConf_t pktconf;

	pkt[0] = sharedPkt[0];
	pkt[1] = sharedPkt[1];
	memset( pkt[0], 0xff, _PKT_LEN );
	memset( pkt[1], 0xff, _PKT_LEN );

	/**********************************************************************
	 * Generate Random Packet
	 **********************************************************************/
	memset( &pktconf, 0, sizeof(pktconf) );
	pktconf.pktType = _PKT_TYPE_ETHER;
	pktconf.l2Flag = 0;
	strtomac((void*)&pktconf.conf_dmac,"00-01-02-03-04-05");
	strtomac((void*)&pktconf.conf_smac,"00-00-00-01-01-01");
	pktconf.conf_ethtype = rtlglue_random();
	pktconf.vlan.vid = 0x0;
	pktconf.vlan.cfi = 0;
	pktconf.vlan.prio = 0;
	pktconf.llc.dsap = 0;
	pktconf.llc.ssap = 0;
	pktconf.pppoe.type = 0;
	pktconf.pppoe.session = 0;
	pktconf.payload.length = 0;
	pktconf.payload.content = "";
	len[0] = pktGen( &pktconf, pkt[0] ) + 4/*CRC*/;
	ASSERT( len[0] > 0 );
	/* Copy packet 0 to packet 1 */
	memcpy( pkt[1], pkt[0], len[0] );
	len[1] = len[0];

	/**********************************************************************
	 * Send pattern to IC and model code 
	 **********************************************************************/
	for( i = 0; i < sizeof(ic_type)/sizeof(ic_type[0]); i++ )
	{
		retval = model_setTestTarget( ic_type[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */		
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		NORMAL_MODE;
		AllPortForward(RTL8651_PORTSTA_FORWARDING);	
		rtl8651_setAsicOperationLayer(2);
		
		/**********************************************************************
		 * set ASIC registers
		 **********************************************************************/
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));
		
		/**********************************************************************
		 * set L2 entry
		 **********************************************************************/
		bzero((void*) &l2t, sizeof(l2t));
		strtomac(&l2t.macAddr,"00-01-02-03-04-05");
		l2t.memberPortMask = PM_PORT_1;
		l2t.isStatic = 1;
		l2t.auth=0;
		l2t.nhFlag=TRUE;
		retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-01-02-03-04-05"), 0, &l2t);
		ASSERT( retval == SUCCESS );
		
		/**********************************************************************
		 * set VLAN 0 member port 0x3f		
		 **********************************************************************/
		memset( &vlan, 0x0, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.untagPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		retval = rtl8651_setAsicVlan(0,&vlan);
		ASSERT( retval == SUCCESS );

		/**********************************************************************
		 * send to SWCORE
		 **********************************************************************/
		retval = virtualMacInput( PN_PORT0, pkt[i], len[i] );
		ASSERT( retval==SUCCESS );
	}		

	/**********************************************************************
	 * Compare the result of IC and model code (when all of them are supported)
	 **********************************************************************/
	if ( model_tryTestTarget( ic_type[0] )==SUCCESS &&
	     model_tryTestTarget( ic_type[1] )==SUCCESS )
	{
		enum PORT_NUM pn;

		for( pn = PN_PORT0; pn <= PN_PORT5; pn++ )
		{
			for( i = 0; i < sizeof(ic_type)/sizeof(ic_type[0]); i++ )
			{
				retval = model_setTestTarget( ic_type[i] );
				ASSERT( retval == SUCCESS );
				
				/**********************************************************************
				 * Retrieve output packet from SWCORE
				 **********************************************************************/
				len[i] = _PKT_LEN; /* indicate buffer length */
				portmask[i] = 1<<pn; /* Only one port in each check */
				pktout[i] = virtualMacOutput( &portmask[i], pkt[i], &len[i] );
			}

			/**********************************************************************
			 * Compare results
			 **********************************************************************/
			IS_EQUAL_INT_DETAIL( "Packet output result is different", pktout[0], pktout[1], __FUNCTION__, __LINE__, ic_type[i] );
			IS_EQUAL_INT_DETAIL( "Pakcet should be forwarded to port list ....", portmask[0], portmask[1], __FUNCTION__, __LINE__, ic_type[i] );
			if ( 0==memComp( pkt[0], pkt[1], max(len[0],len[1]), "Output packet content is not equal!" ) )
			{
				retval = SUCCESS;
			}
			else
			{
				retval = FAILED;
				break;
			}
			IS_EQUAL_INT_DETAIL( "Output packet length is different ...", len[0], len[1], __FUNCTION__, __LINE__, i );
		}
	}
	else
	{/* no need to compare */
		retval = SUCCESS; 
	}
	return retval;

}


/***********************************************************************
 *  1. A L2 packet comes into SWCORE, expect a unknown unicast occupies.
 *  2. At the same time, the source MAC is learned.
 *  3. The replied packet comes into SWCORE, expect it is unicast, instead of broadcast.
 ***********************************************************************/
int32 testLayer2Forward4(uint32 caseNo)
{
	int32 retval;
	enum IC_TYPE i;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	uint8 *pkt0, *pkt1;
	int32 len0, len1;
	enum PORT_MASK portmask;
	rtl8651_PktConf_t pktconf;

	pkt0 = sharedPkt[0];
	pkt1 = sharedPkt[1];
	memset( pkt0, 0xff, _PKT_LEN );
	memset( pkt1, 0xff, _PKT_LEN );
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		rtlglue_printf( "model_setTestTarget('%s')\n", i==IC_TYPE_REAL?"REAL IC":"MODEL CODE" );
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		NORMAL_MODE;
		rtl8651_setAsicOperationLayer(2);
		
		/**********************************************************************
		 * set ASIC registers
		 **********************************************************************/
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));
		WRITE_MEM32(QNUMCR, P0QNum_1|P1QNum_1|P2QNum_1|P3QNum_1|P4QNum_1|P5QNum_1|P6QNum_1 );
		WRITE_MEM32(SIRR, READ_MEM32(SIRR)|TRXRDY );
		AllPortForward(RTL8651_PORTSTA_FORWARDING);	

		/**********************************************************************
		 * Set Net Interface 0
		 *---------------------------------------------------------------------*/
		bzero((void*) &intf, sizeof(intf));
		strtomac(&intf.macAddr, "00-00-10-11-12-20" );
		intf.macAddrNumber = 1;
		intf.vid = 8;
		intf.mtu = 1500;
		intf.inAclStart = 0;
		intf.inAclEnd = 0;
		intf.outAclStart = 0;
		intf.outAclEnd = 0;
		intf.enableRoute = 0;
		intf.valid = 1;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval == SUCCESS );

		/**********************************************************************
		 * set VLAN 8 member port 0x3f		
		 **********************************************************************/
		memset( &vlan, 0x0, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.untagPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		retval = rtl8651_setAsicVlan(8,&vlan);
		ASSERT( retval == SUCCESS );

		/**********************************************************************
		 * Port Based VLAN Control Register
		 **********************************************************************/
		WRITE_MEM32( PVCR0, (READ_MEM32(PVCR0)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port 0 */
		WRITE_MEM32( PVCR0, (READ_MEM32(PVCR0)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port 1 */
		WRITE_MEM32( PVCR1, (READ_MEM32(PVCR1)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port 2 */
		WRITE_MEM32( PVCR1, (READ_MEM32(PVCR1)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port 3 */
		WRITE_MEM32( PVCR2, (READ_MEM32(PVCR2)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port 4 */
		WRITE_MEM32( PVCR2, (READ_MEM32(PVCR2)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port 5 */
		WRITE_MEM32( PVCR3, (READ_MEM32(PVCR3)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port ext 1 */
		WRITE_MEM32( PVCR3, (READ_MEM32(PVCR3)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port ext 2 */
		WRITE_MEM32( PVCR4, (READ_MEM32(PVCR4)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port ext 3 */

		/**********************************************************************
		 * Generate sending packet
		 *---------------------------------------------------------------------*/
		memset( &pktconf, 0, sizeof(pktconf) );
		pktconf.pktType = _PKT_TYPE_ETHER;
		pktconf.l2Flag = 0;
		strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-05" );
		strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
		pktconf.conf_ethtype = 0x1234;
		pktconf.vlan.vid = 0x0;
		pktconf.vlan.cfi = 0;
		pktconf.vlan.prio = 0;
		pktconf.llc.dsap = 0;
		pktconf.llc.ssap = 0;
		pktconf.pppoe.type = 0;
		pktconf.pppoe.session = 0;
		pktconf.payload.length = 0;
		pktconf.payload.content = "";
		len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
		ASSERT( len0 > 0 );

		/*---------------------------------------------------------------------
		 * Send to SWCORE
		 *---------------------------------------------------------------------*/
		retval = virtualMacInput( PN_PORT0, pkt0, len0 );
		ASSERT( retval == SUCCESS );

		/**********************************************************************
		 * Generate expected packet
		 *---------------------------------------------------------------------*/
		memset( &pktconf, 0, sizeof(pktconf) );
		pktconf.pktType = _PKT_TYPE_ETHER;
		pktconf.l2Flag = 0;
		strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-05" );
		strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
		pktconf.conf_ethtype = 0x1234;
		pktconf.vlan.vid = 0x0;
		pktconf.vlan.cfi = 0;
		pktconf.vlan.prio = 0;
		pktconf.llc.dsap = 0;
		pktconf.llc.ssap = 0;
		pktconf.pppoe.type = 0;
		pktconf.pppoe.session = 0;
		pktconf.payload.length = 0;
		pktconf.payload.content = "";
		len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
		ASSERT( len0 > 0 );

		/*---------------------------------------------------------------------
		 * Output packet from SWCORE and Compare
		 *---------------------------------------------------------------------*/
		{ /* Unknown unicast */
			enum PORT_NUM port;
			for( port = PN_PORT0; port<=PN_PORT5; port++ )
			{
				if ( port==PN_PORT0 ) continue; /* This is the source port, the unknown unicast packet will NOT come here. */

				portmask = PM_PORT_ALL;
				len1 = _PKT_LEN; /* indicate buffer length */
				retval = virtualMacOutput( &portmask, pkt1, &len1 );
				ASSERT( retval == SUCCESS );
				IS_EQUAL_INT_DETAIL( "Pakcet should be forward only to port list ...", portmask, 1<<port, __FUNCTION__, __LINE__, i );
				ASSERT( 0==memComp( pkt0, pkt1, max(len0,len1), "Packet content is not equal!" ) );
				IS_EQUAL_INT_DETAIL( "Output packet length is different ...", len0, len1, __FUNCTION__, __LINE__, i );
			}
		}

		/**********************************************************************
		 * Generate sending packet ( L2 Entry should be learned )
		 *---------------------------------------------------------------------*/
		memset( &pktconf, 0, sizeof(pktconf) );
		pktconf.pktType = _PKT_TYPE_ETHER;
		pktconf.l2Flag = 0;
		strtomac( (void*)&pktconf.conf_dmac, "00-00-00-01-01-01" );
		strtomac( (void*)&pktconf.conf_smac, "00-01-02-03-04-05" );
		pktconf.conf_ethtype = 0x5566;
		pktconf.vlan.vid = 0x0;
		pktconf.vlan.cfi = 0;
		pktconf.vlan.prio = 0;
		pktconf.llc.dsap = 0;
		pktconf.llc.ssap = 0;
		pktconf.pppoe.type = 0;
		pktconf.pppoe.session = 0;
		pktconf.payload.length = 0;
		pktconf.payload.content = "";
		len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
		ASSERT( len0 > 0 );

		/*---------------------------------------------------------------------
		 * Send to SWCORE
		 *---------------------------------------------------------------------*/
		retval = virtualMacInput( PN_PORT2, pkt0, len0 );
		ASSERT( retval == SUCCESS );

		/**********************************************************************
		 * Generate expected packet
		 *---------------------------------------------------------------------*/
		memset( &pktconf, 0, sizeof(pktconf) );
		pktconf.pktType = _PKT_TYPE_ETHER;
		pktconf.l2Flag = 0;
		strtomac( (void*)&pktconf.conf_dmac, "00-00-00-01-01-01" );
		strtomac( (void*)&pktconf.conf_smac, "00-01-02-03-04-05" );
		pktconf.conf_ethtype = 0x5566;
		pktconf.vlan.vid = 0x0;
		pktconf.vlan.cfi = 0;
		pktconf.vlan.prio = 0;
		pktconf.llc.dsap = 0;
		pktconf.llc.ssap = 0;
		pktconf.pppoe.type = 0;
		pktconf.pppoe.session = 0;
		pktconf.payload.length = 0;
		pktconf.payload.content = "";
		len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
		ASSERT( len0 > 0 );

		/*---------------------------------------------------------------------
		 * Output packet from SWCORE and Compare
		 *---------------------------------------------------------------------*/
		portmask = PM_PORT_ALL;
		len1 = _PKT_LEN; /* indicate buffer length */
		retval = virtualMacOutput( &portmask, pkt1, &len1 );
		ASSERT( retval == SUCCESS );
		IS_EQUAL_INT_DETAIL( "Pakcet should be forward only to port list ...", portmask, PM_PORT_0, __FUNCTION__, __LINE__, i );
		ASSERT( 0==memComp( pkt0, pkt1, max(len0,len1), "Packet content is not equal!" ) );
		IS_EQUAL_INT_DETAIL( "Output packet length is different ...", len0, len1, __FUNCTION__, __LINE__, i );
	}		
	
	retval=SUCCESS;
	return retval;

}


/***********************************************************************
 *  In this case, we test Giga mode GMII/SerDes Interface.
 *  We will try to send a packet from port0->5, port1->2, port4->3, port5->0.
 ***********************************************************************/
int32 testLayer2Forward5(uint32 caseNo)
{
	int32 retval;
	enum IC_TYPE i;
	rtl865x_tblAsicDrv_l2Param_t l2t;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	uint8 *pkt0, *pkt1;
	int32 len0, len1;
	enum PORT_MASK portmask;
	rtl8651_PktConf_t pktconf;

	pkt0 = sharedPkt[0];
	pkt1 = sharedPkt[1];
	memset( pkt0, 0xff, _PKT_LEN );
	memset( pkt1, 0xff, _PKT_LEN );
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		rtlglue_printf( "model_setTestTarget('%s')\n", i==IC_TYPE_REAL?"REAL IC":"MODEL CODE" );
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		NORMAL_MODE;
		rtl8651_setAsicOperationLayer(2);
		
		/**********************************************************************
		 * set ASIC registers
		 **********************************************************************/
		WRITE_MEM32(MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL));
		WRITE_MEM32(QNUMCR, P0QNum_1|P1QNum_1|P2QNum_1|P3QNum_1|P4QNum_1|P5QNum_1|P6QNum_1 );
		WRITE_MEM32(SIRR, READ_MEM32(SIRR)|TRXRDY );
		AllPortForward(RTL8651_PORTSTA_FORWARDING);	
	
		/**********************************************************************
		 * set L2 entry (Port0)
		 **********************************************************************/
		bzero((void*) &l2t, sizeof(l2t));
		strtomac(&l2t.macAddr,"00-01-02-03-04-00");
		l2t.memberPortMask = PM_PORT_0;
		l2t.isStatic = 1;
		l2t.auth=0;
		l2t.nhFlag=TRUE;
		retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-01-02-03-04-00"), 0, &l2t);
		ASSERT( retval == SUCCESS );
		/**********************************************************************
		 * set L2 entry (Port2)
		 **********************************************************************/
		bzero((void*) &l2t, sizeof(l2t));
		strtomac(&l2t.macAddr,"00-01-02-03-04-02");
		l2t.memberPortMask = PM_PORT_2;
		l2t.isStatic = 1;
		l2t.auth=0;
		l2t.nhFlag=TRUE;
		retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-01-02-03-04-02"), 0, &l2t);
		ASSERT( retval == SUCCESS );
		/**********************************************************************
		 * set L2 entry (Port3)
		 **********************************************************************/
		bzero((void*) &l2t, sizeof(l2t));
		strtomac(&l2t.macAddr,"00-01-02-03-04-03");
		l2t.memberPortMask = PM_PORT_3;
		l2t.isStatic = 1;
		l2t.auth=0;
		l2t.nhFlag=TRUE;
		retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-01-02-03-04-03"), 0, &l2t);
		ASSERT( retval == SUCCESS );
		/**********************************************************************
		 * set L2 entry (Port5)
		 **********************************************************************/
		bzero((void*) &l2t, sizeof(l2t));
		strtomac(&l2t.macAddr,"00-01-02-03-04-05");
		l2t.memberPortMask = PM_PORT_5;
		l2t.isStatic = 1;
		l2t.auth=0;
		l2t.nhFlag=TRUE;
		retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-01-02-03-04-05"), 0, &l2t);
		ASSERT( retval == SUCCESS );
		
		/**********************************************************************
		 * Set Net Interface 0
		 *---------------------------------------------------------------------*/
		bzero((void*) &intf, sizeof(intf));
		strtomac(&intf.macAddr, "00-00-10-11-12-20" );
		intf.macAddrNumber = 1;
		intf.vid = 8;
		intf.mtu = 1500;
		intf.inAclStart = 0;
		intf.inAclEnd = 0;
		intf.outAclStart = 0;
		intf.outAclEnd = 0;
		intf.enableRoute = 0;
		intf.valid = 1;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval == SUCCESS );

		/**********************************************************************
		 * set VLAN 8 member port 0x3f		
		 **********************************************************************/
		memset( &vlan, 0x0, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.untagPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		retval = rtl8651_setAsicVlan(8,&vlan);
		ASSERT( retval == SUCCESS );

		/**********************************************************************
		 * Port Based VLAN Control Register
		 **********************************************************************/
		WRITE_MEM32( PVCR0, (READ_MEM32(PVCR0)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port 0 */
		WRITE_MEM32( PVCR0, (READ_MEM32(PVCR0)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port 1 */
		WRITE_MEM32( PVCR1, (READ_MEM32(PVCR1)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port 2 */
		WRITE_MEM32( PVCR1, (READ_MEM32(PVCR1)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port 3 */
		WRITE_MEM32( PVCR2, (READ_MEM32(PVCR2)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port 4 */
		WRITE_MEM32( PVCR2, (READ_MEM32(PVCR2)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port 5 */
		WRITE_MEM32( PVCR3, (READ_MEM32(PVCR3)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port ext 1 */
		WRITE_MEM32( PVCR3, (READ_MEM32(PVCR3)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port ext 2 */
		WRITE_MEM32( PVCR4, (READ_MEM32(PVCR4)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port ext 3 */

		{
			int round;
			for( round = 0; round<1; round++ )
			{
				uint32 lenPayload;

#undef TEST_JUMBO /* define this if you want to test jumbo frame. */
#ifdef TEST_JUMBO
				WRITE_MEM32( PCRP0, (READ_MEM32(PCRP0)&~AcptMaxLen_MASK)|AcptMaxLen_16K );
				WRITE_MEM32( PCRP1, (READ_MEM32(PCRP1)&~AcptMaxLen_MASK)|AcptMaxLen_16K );
				WRITE_MEM32( PCRP2, (READ_MEM32(PCRP2)&~AcptMaxLen_MASK)|AcptMaxLen_16K );
				WRITE_MEM32( PCRP3, (READ_MEM32(PCRP3)&~AcptMaxLen_MASK)|AcptMaxLen_16K );
				WRITE_MEM32( PCRP4, (READ_MEM32(PCRP4)&~AcptMaxLen_MASK)|AcptMaxLen_16K );
				WRITE_MEM32( PCRP5, (READ_MEM32(PCRP5)&~AcptMaxLen_MASK)|AcptMaxLen_16K );
				WRITE_MEM32( SBFCR2, S_Max_SBuf_FCOFF_MASK|S_Max_SBuf_FCON_MASK );
				WRITE_MEM32( PBFCR2, P_MaxDSC_FCOFF_MASK|P_MaxDSC_FCON_MASK );
#endif
#if 1 /* fixed length */
				lenPayload = 46; /* Just make for 64-bytes packet. */
				/* lenPayload = (16*1024-2-6-6-2-4); */
#elif 1 /* random for Giga Jumbo */
				lenPayload = rtlglue_random()%(16*1024-2-6-6-2-4+1);
				/*lenPayload = rtlglue_random()%(9216-6-6-2-4+1); */
				/* lenPayload = 16383-6-6-2-4; */
				/* lenPayload = 9216-6-6-2-4; */
#else /* random for 100M packet */
				lenPayload = rtlglue_random()%(1522-6-6-2-4+1);
#endif
				rtlglue_printf( "[round=%d] lenPayload=%d\n", round, lenPayload );

				/**********************************************************************
				 * Generate sending packet (0->5)
				 *---------------------------------------------------------------------*/
				memset( &pktconf, 0, sizeof(pktconf) );
				pktconf.pktType = _PKT_TYPE_ETHER;
				pktconf.l2Flag = 0;
				strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-05" );
				strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
				pktconf.conf_ethtype = 0x1234;
				pktconf.vlan.vid = 0x0;
				pktconf.vlan.cfi = 0;
				pktconf.vlan.prio = 0;
				pktconf.llc.dsap = 0;
				pktconf.llc.ssap = 0;
				pktconf.pppoe.type = 0;
				pktconf.pppoe.session = 0;
				pktconf.payload.length = lenPayload;
				pktconf.payload.content = "";
				len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
				ASSERT( len0 > 0 );
		
				/*---------------------------------------------------------------------
				 * Send to SWCORE
				 *---------------------------------------------------------------------*/
				retval = virtualMacInput( PN_PORT0, pkt0, len0 );
				ASSERT( retval == SUCCESS );
		
				/**********************************************************************
				 * Generate expected packet
				 *---------------------------------------------------------------------*/
				memset( &pktconf, 0, sizeof(pktconf) );
				pktconf.pktType = _PKT_TYPE_ETHER;
				pktconf.l2Flag = 0;
				strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-05" );
				strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
				pktconf.conf_ethtype = 0x1234;
				pktconf.vlan.vid = 0x0;
				pktconf.vlan.cfi = 0;
				pktconf.vlan.prio = 0;
				pktconf.llc.dsap = 0;
				pktconf.llc.ssap = 0;
				pktconf.pppoe.type = 0;
				pktconf.pppoe.session = 0;
				pktconf.payload.length = lenPayload;
				pktconf.payload.content = "";
				len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
				ASSERT( len0 > 0 );
		
				/*---------------------------------------------------------------------
				 * Output packet from SWCORE and Compare
				 *---------------------------------------------------------------------*/
				portmask = PM_PORT_ALL;
				len1 = _PKT_LEN; /* indicate buffer length */
				retval = virtualMacOutput( &portmask, pkt1, &len1 );
				ASSERT( retval == SUCCESS );
				IS_EQUAL_INT_DETAIL( "Pakcet should be forwarded to port list ....", portmask, PM_PORT_5, __FUNCTION__, __LINE__, i );
				ASSERT( 0==memComp( pkt0, pkt1, max(len0,len1), "Packet content is not equal!" ) );
				IS_EQUAL_INT_DETAIL( "Output packet length is different ...", len0, len1, __FUNCTION__, __LINE__, i );
	
				/**********************************************************************
				 * Generate sending packet (1->2)
				 *---------------------------------------------------------------------*/
				memset( &pktconf, 0, sizeof(pktconf) );
				pktconf.pktType = _PKT_TYPE_ETHER;
				pktconf.l2Flag = 0;
				strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-02" );
				strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
				pktconf.conf_ethtype = 0x1234;
				pktconf.vlan.vid = 0x0;
				pktconf.vlan.cfi = 0;
				pktconf.vlan.prio = 0;
				pktconf.llc.dsap = 0;
				pktconf.llc.ssap = 0;
				pktconf.pppoe.type = 0;
				pktconf.pppoe.session = 0;
				pktconf.payload.length = lenPayload;
				pktconf.payload.content = "";
				len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
				ASSERT( len0 > 0 );
		
				/*---------------------------------------------------------------------
				 * Send to SWCORE
				 *---------------------------------------------------------------------*/
				retval = virtualMacInput( PN_PORT1, pkt0, len0 );
				ASSERT( retval == SUCCESS );
		
				/**********************************************************************
				 * Generate expected packet
				 *---------------------------------------------------------------------*/
				memset( &pktconf, 0, sizeof(pktconf) );
				pktconf.pktType = _PKT_TYPE_ETHER;
				pktconf.l2Flag = 0;
				strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-02" );
				strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
				pktconf.conf_ethtype = 0x1234;
				pktconf.vlan.vid = 0x0;
				pktconf.vlan.cfi = 0;
				pktconf.vlan.prio = 0;
				pktconf.llc.dsap = 0;
				pktconf.llc.ssap = 0;
				pktconf.pppoe.type = 0;
				pktconf.pppoe.session = 0;
				pktconf.payload.length = lenPayload;
				pktconf.payload.content = "";
				len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
				ASSERT( len0 > 0 );
		
				/*---------------------------------------------------------------------
				 * Output packet from SWCORE and Compare
				 *---------------------------------------------------------------------*/
				portmask = PM_PORT_ALL;
				len1 = _PKT_LEN; /* indicate buffer length */
				retval = virtualMacOutput( &portmask, pkt1, &len1 );
				ASSERT( retval == SUCCESS );
				IS_EQUAL_INT_DETAIL( "Pakcet should be forwarded to port list ....", portmask, PM_PORT_2, __FUNCTION__, __LINE__, i );
				ASSERT( 0==memComp( pkt0, pkt1, max(len0,len1), "Packet content is not equal!" ) );
				IS_EQUAL_INT_DETAIL( "Output packet length is different ...", len0, len1, __FUNCTION__, __LINE__, i );

				/**********************************************************************
				 * Generate sending packet (4->3)
				 *---------------------------------------------------------------------*/
				memset( &pktconf, 0, sizeof(pktconf) );
				pktconf.pktType = _PKT_TYPE_ETHER;
				pktconf.l2Flag = 0;
				strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-03" );
				strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
				pktconf.conf_ethtype = 0x1234;
				pktconf.vlan.vid = 0x0;
				pktconf.vlan.cfi = 0;
				pktconf.vlan.prio = 0;
				pktconf.llc.dsap = 0;
				pktconf.llc.ssap = 0;
				pktconf.pppoe.type = 0;
				pktconf.pppoe.session = 0;
				pktconf.payload.length = lenPayload;
				pktconf.payload.content = "";
				len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
				ASSERT( len0 > 0 );
		
				/*---------------------------------------------------------------------
				 * Send to SWCORE
				 *---------------------------------------------------------------------*/
				retval = virtualMacInput( PN_PORT4, pkt0, len0 );
				ASSERT( retval == SUCCESS );
		
				/**********************************************************************
				 * Generate expected packet
				 *---------------------------------------------------------------------*/
				memset( &pktconf, 0, sizeof(pktconf) );
				pktconf.pktType = _PKT_TYPE_ETHER;
				pktconf.l2Flag = 0;
				strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-03" );
				strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
				pktconf.conf_ethtype = 0x1234;
				pktconf.vlan.vid = 0x0;
				pktconf.vlan.cfi = 0;
				pktconf.vlan.prio = 0;
				pktconf.llc.dsap = 0;
				pktconf.llc.ssap = 0;
				pktconf.pppoe.type = 0;
				pktconf.pppoe.session = 0;
				pktconf.payload.length = lenPayload;
				pktconf.payload.content = "";
				len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
				ASSERT( len0 > 0 );
		
				/*---------------------------------------------------------------------
				 * Output packet from SWCORE and Compare
				 *---------------------------------------------------------------------*/
				portmask = PM_PORT_ALL;
				len1 = _PKT_LEN; /* indicate buffer length */
				retval = virtualMacOutput( &portmask, pkt1, &len1 );
				ASSERT( retval == SUCCESS );
				IS_EQUAL_INT_DETAIL( "Pakcet should be forwarded to port list ....", portmask, PM_PORT_3, __FUNCTION__, __LINE__, i );
				ASSERT( 0==memComp( pkt0, pkt1, max(len0,len1), "Packet content is not equal!" ) );
				IS_EQUAL_INT_DETAIL( "Output packet length is different ...", len0, len1, __FUNCTION__, __LINE__, i );

				/**********************************************************************
				 * Generate sending packet (5->0)
				 *---------------------------------------------------------------------*/
				memset( &pktconf, 0, sizeof(pktconf) );
				pktconf.pktType = _PKT_TYPE_ETHER;
				pktconf.l2Flag = 0;
				strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-00" );
				strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
				pktconf.conf_ethtype = 0x1234;
				pktconf.vlan.vid = 0x0;
				pktconf.vlan.cfi = 0;
				pktconf.vlan.prio = 0;
				pktconf.llc.dsap = 0;
				pktconf.llc.ssap = 0;
				pktconf.pppoe.type = 0;
				pktconf.pppoe.session = 0;
				pktconf.payload.length = lenPayload;
				pktconf.payload.content = "";
				len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
				ASSERT( len0 > 0 );
		
				/*---------------------------------------------------------------------
				 * Send to SWCORE
				 *---------------------------------------------------------------------*/
				retval = virtualMacInput( PN_PORT5, pkt0, len0 );
				ASSERT( retval == SUCCESS );
		
				/**********************************************************************
				 * Generate expected packet
				 *---------------------------------------------------------------------*/
				memset( &pktconf, 0, sizeof(pktconf) );
				pktconf.pktType = _PKT_TYPE_ETHER;
				pktconf.l2Flag = 0;
				strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-00" );
				strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
				pktconf.conf_ethtype = 0x1234;
				pktconf.vlan.vid = 0x0;
				pktconf.vlan.cfi = 0;
				pktconf.vlan.prio = 0;
				pktconf.llc.dsap = 0;
				pktconf.llc.ssap = 0;
				pktconf.pppoe.type = 0;
				pktconf.pppoe.session = 0;
				pktconf.payload.length = lenPayload;
				pktconf.payload.content = "";
				len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
				ASSERT( len0 > 0 );
		
				/*---------------------------------------------------------------------
				 * Output packet from SWCORE and Compare
				 *---------------------------------------------------------------------*/
				portmask = PM_PORT_ALL;
				len1 = _PKT_LEN; /* indicate buffer length */
				retval = virtualMacOutput( &portmask, pkt1, &len1 );
				ASSERT( retval == SUCCESS );
				IS_EQUAL_INT_DETAIL( "Pakcet should be forwarded to port list ....", portmask, PM_PORT_0, __FUNCTION__, __LINE__, i );
				ASSERT( 0==memComp( pkt0, pkt1, max(len0,len1), "Packet content is not equal!" ) );
				IS_EQUAL_INT_DETAIL( "Output packet length is different ...", len0, len1, __FUNCTION__, __LINE__, i );
			}		
		}
	}
	
	retval=SUCCESS;
	return retval;

}


/***********************************************************************
 *  Test Jumbo frame boundary
 ***********************************************************************/
int32 testLayer2Jumbo(uint32 caseNo)
{
	int32 retval;
	enum IC_TYPE i;
	rtl865x_tblAsicDrv_l2Param_t l2t;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;
	uint8 *pkt0, *pkt1;
	int32 len0, len1;
	enum PORT_MASK portmask;
	rtl8651_PktConf_t pktconf;
	struct
	{
		uint32 acptMax;
		uint32 lenPayload;
		uint32 fwd;
	} bench[] =
	{
		{ acptMax: AcptMaxLen_1536, lenPayload:  1536-6-6-2-4, fwd: TRUE, },
		{ acptMax: AcptMaxLen_1536, lenPayload:  1537-6-6-2-4, fwd:FALSE, },
		{ acptMax: AcptMaxLen_1552, lenPayload:  1552-6-6-2-4, fwd: TRUE, },
		{ acptMax: AcptMaxLen_1552, lenPayload:  1553-6-6-2-4, fwd:FALSE, },
		{ acptMax: AcptMaxLen_9K,   lenPayload:  9216-6-6-2-4, fwd: TRUE, },
		{ acptMax: AcptMaxLen_9K,   lenPayload:  9217-6-6-2-4, fwd:FALSE, },
		{ acptMax: AcptMaxLen_16K,  lenPayload: 16382-6-6-2-4-8-4, fwd: TRUE, },
		{ acptMax: AcptMaxLen_16K,  lenPayload: 16383-6-6-2-4-8-4, fwd:FALSE, },
	};
	uint32 benchno;

	pkt0 = sharedPkt[0];
	pkt1 = sharedPkt[1];
	memset( pkt0, 0xff, _PKT_LEN );
	memset( pkt1, 0xff, _PKT_LEN );
	
	/* Compare the HSA of expected, model, FPGA, and real IC. */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		rtlglue_printf( "model_setTestTarget('%s')\n", i==IC_TYPE_REAL?"REAL IC":"MODEL CODE" );
		rtl8651_clearAsicAllTable();
		rtl8651_clearRegister();
		NORMAL_MODE;
		rtl8651_setAsicOperationLayer(2);
		
		/**********************************************************************
		 * set ASIC registers
		 **********************************************************************/
		WRITE_MEM32( MSCR, READ_MEM32(MSCR)& ~(EN_IN_ACL|EN_OUT_ACL) );
		WRITE_MEM32( QNUMCR, P0QNum_1|P1QNum_1|P2QNum_1|P3QNum_1|P4QNum_1|P5QNum_1|P6QNum_1 );
		WRITE_MEM32( SIRR, READ_MEM32(SIRR)|TRXRDY );
		WRITE_MEM32( SBFCR2, S_Max_SBuf_FCOFF_MASK|S_Max_SBuf_FCON_MASK );
		WRITE_MEM32( PBFCR2, P_MaxDSC_FCOFF_MASK|P_MaxDSC_FCON_MASK ); /* Set to the maximum flow control */
		AllPortForward(RTL8651_PORTSTA_FORWARDING);	
	
		/**********************************************************************
		 * set L2 entry
		 **********************************************************************/
		bzero((void*) &l2t, sizeof(l2t));
		strtomac(&l2t.macAddr,"00-01-02-03-04-05");
		l2t.memberPortMask = PM_PORT_2;
		l2t.isStatic = 1;
		l2t.auth=0;
		l2t.nhFlag=TRUE;
		retval = rtl8651_setAsicL2Table(getL2TableRowFromMacStr("00-01-02-03-04-05"), 0, &l2t);
		ASSERT( retval == SUCCESS );
		
		/**********************************************************************
		 * Set Net Interface 0
		 *---------------------------------------------------------------------*/
		bzero((void*) &intf, sizeof(intf));
		strtomac(&intf.macAddr, "00-00-10-11-12-20" );
		intf.macAddrNumber = 1;
		intf.vid = 8;
		intf.mtu = 1500;
		intf.inAclStart = 0;
		intf.inAclEnd = 0;
		intf.outAclStart = 0;
		intf.outAclEnd = 0;
		intf.enableRoute = 0;
		intf.valid = 1;
		retval = rtl8651_setAsicNetInterface( RTL865XC_NETIFTBL_SIZE, &intf );
		ASSERT( retval == SUCCESS );

		/**********************************************************************
		 * set VLAN 8 member port 0x3f		
		 **********************************************************************/
		memset( &vlan, 0x0, sizeof(vlan) );
		vlan.memberPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		vlan.untagPortMask = PM_PORT_0|PM_PORT_1|PM_PORT_2|PM_PORT_3|PM_PORT_4|PM_PORT_5;
		retval = rtl8651_setAsicVlan(8,&vlan);
		ASSERT( retval == SUCCESS );

		/**********************************************************************
		 * Port Based VLAN Control Register
		 **********************************************************************/
		WRITE_MEM32( PVCR0, (READ_MEM32(PVCR0)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port 0 */
		WRITE_MEM32( PVCR0, (READ_MEM32(PVCR0)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port 1 */
		WRITE_MEM32( PVCR1, (READ_MEM32(PVCR1)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port 2 */
		WRITE_MEM32( PVCR1, (READ_MEM32(PVCR1)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port 3 */
		WRITE_MEM32( PVCR2, (READ_MEM32(PVCR2)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port 4 */
		WRITE_MEM32( PVCR2, (READ_MEM32(PVCR2)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port 5 */
		WRITE_MEM32( PVCR3, (READ_MEM32(PVCR3)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port ext 1 */
		WRITE_MEM32( PVCR3, (READ_MEM32(PVCR3)&~(0xffff<<16))|(0/*priority*/<<28)|(0x008/*PVID*/<<16) ); /* Port ext 2 */
		WRITE_MEM32( PVCR4, (READ_MEM32(PVCR4)&~(0xffff<< 0))|(0/*priority*/<<12)|(0x008/*PVID*/<< 0) ); /* Port ext 3 */

		for( benchno = 0; benchno<(sizeof(bench)/sizeof(bench[0])); benchno++ )
		{
			int round;
			for( round = 0; round<1; round++ )
			{
				uint32 lenPayload;

				lenPayload = bench[benchno].lenPayload;
				rtlglue_printf( "[round=%d, benchno=%d] acptMax=0x%08x lenPayload=%d\n", round, benchno, bench[benchno].acptMax, lenPayload );

				/**********************************************************************
				 * Set ASIC Registers
				 *---------------------------------------------------------------------*/
				WRITE_MEM32( PCRP1, (READ_MEM32(PCRP1)&~AcptMaxLen_MASK)|bench[benchno].acptMax );

				/**********************************************************************
				 * Generate sending packet
				 *---------------------------------------------------------------------*/
				memset( &pktconf, 0, sizeof(pktconf) );
				pktconf.pktType = _PKT_TYPE_ETHER;
				pktconf.l2Flag = 0;
				strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-05" );
				strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
				pktconf.conf_ethtype = 0x1234;
				pktconf.vlan.vid = 0x0;
				pktconf.vlan.cfi = 0;
				pktconf.vlan.prio = 0;
				pktconf.llc.dsap = 0;
				pktconf.llc.ssap = 0;
				pktconf.pppoe.type = 0;
				pktconf.pppoe.session = 0;
				pktconf.payload.length = lenPayload;
				pktconf.payload.content = "";
				len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
				ASSERT( len0 > 0 );
		
				/*---------------------------------------------------------------------
				 * Send to SWCORE
				 *---------------------------------------------------------------------*/
				retval = virtualMacInput( PN_PORT1, pkt0, len0 );
				ASSERT( retval == SUCCESS );
		
				/**********************************************************************
				 * Generate expected packet
				 *---------------------------------------------------------------------*/
				memset( &pktconf, 0, sizeof(pktconf) );
				pktconf.pktType = _PKT_TYPE_ETHER;
				pktconf.l2Flag = 0;
				strtomac( (void*)&pktconf.conf_dmac, "00-01-02-03-04-05" );
				strtomac( (void*)&pktconf.conf_smac, "00-00-00-01-01-01" );
				pktconf.conf_ethtype = 0x1234;
				pktconf.vlan.vid = 0x0;
				pktconf.vlan.cfi = 0;
				pktconf.vlan.prio = 0;
				pktconf.llc.dsap = 0;
				pktconf.llc.ssap = 0;
				pktconf.pppoe.type = 0;
				pktconf.pppoe.session = 0;
				pktconf.payload.length = lenPayload;
				pktconf.payload.content = "";
				len0 = pktGen( &pktconf, pkt0 ) + 4/*CRC*/;
				ASSERT( len0 > 0 );
		
				/*---------------------------------------------------------------------
				 * Output packet from SWCORE and Compare
				 *---------------------------------------------------------------------*/
				portmask = PM_PORT_ALL;
				len1 = _PKT_LEN; /* indicate buffer length */
				retval = virtualMacOutput( &portmask, pkt1, &len1 );
				if ( bench[benchno].fwd==TRUE )
				{
					ASSERT( retval == SUCCESS );
					IS_EQUAL_INT_DETAIL( "Pakcet should be forwarded to port list ....", portmask, PM_PORT_2, __FUNCTION__, __LINE__, i );
					ASSERT( 0==memComp( pkt0, pkt1, max(len0,len1), "Packet content is not equal!" ) );
					IS_EQUAL_INT_DETAIL( "Output packet length is different ...", len0, len1, __FUNCTION__, __LINE__, i );
				}
				else
				{
					ASSERT( retval != SUCCESS );
					IS_EQUAL_INT_DETAIL( "Pakcet should be forwarded to port list ....", portmask, 0, __FUNCTION__, __LINE__, i );
				}
			}		
		}
	}
	
	retval=SUCCESS;
	return retval;

}
