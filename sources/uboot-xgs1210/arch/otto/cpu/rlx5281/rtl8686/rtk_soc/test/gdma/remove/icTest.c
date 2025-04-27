/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Model Code for 865xC
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: icTest.c,v 1.132 2007-11-05 12:00:24 stevewei Exp $
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
#include "l2PacketTest.h"
#include "l34Test.h"
#include "l34PacketTest.h"
#include "tblTest.h"
#include "modelRandom.h"
#include "hsModel.h"
#ifdef CONFIG_RTL865XC
#include "rtl865xC_tblAsicDrv.h"
#else
#include "rtl8651_tblAsicDrv.h"
#endif
#include "rtl8651_tblDrvLocal.h"
#define RTL8651_TBLDRV_LOCAL_H
#include "rtl8651_aclLocal.h"
#include "rtl8651_layer2local.h"
#include "drvTest.h"
#include "fwdTest.h"
#include "rtl8651_tblDrvFwd.h"
#include "virtualMac.h"
#include "gdmaTest.h"
#include "nicTest.h"
#include "naptTest.h"
#include "parserTest.h"
#include "translatorTest.h"
#include "ft2Test.h"
#if defined(VSV)||defined(MIILIKE)
#include "vsv_conn.h"
#endif
#include "linux/delay.h"

#define DRVTEST_CASENO_LOWERBOUND	100
#define DRVTEST_CASENO_UPPERBOUND	10000

struct MODEL_TEST_CASE_S
{	
	int32 no;
	char* name;
	int32 (*fp)(uint32);
	uint32 group;
};
typedef struct MODEL_TEST_CASE_S MODEL_TEST_CASE_T;





#define MODEL_TEST_CASE( case_no, func, group_mask ) \
	{ \
		no: case_no, \
		name: #func, \
		fp: func, \
		group: group_mask, \
	}


#define DRV_TEST_CASE( case_no, func, group_mask ) \
	{ \
		no: case_no, \
		name: #func, \
		fp: func, \
		group: group_mask, \
	}


/*
 * To make source code more clear, we define this macro.
 * This is for modelCompHsb() and modelCompHsa() functions.
 */
#define CHECK_EQUAL( p1, p2, field, retvar, retval ) \
	do { \
		if ( p1->field != p2->field ) \
		{ \
			rtlglue_printf( "%s(): '%s' NOT equal( %d , %d ).\n", __FUNCTION__, #field, p1->field, p2->field ); \
			retvar = retval; \
		} \
	} while(0);

int32 compHsb( hsb_param_t* hsb1, hsb_param_t* hsb2 )
{
	int32 retval = SUCCESS;

	CHECK_EQUAL( hsb1, hsb2, spa, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, len, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, vid, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, tagif, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, pppoeif, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, sip, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, sprt, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, dip, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, dprt, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, ipptl, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, ipfg, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, iptos, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, tcpfg, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, type, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, patmatch, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, ethtype, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, da[0], retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, da[1], retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, da[2], retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, da[3], retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, da[4], retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, da[5], retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, sa[0], retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, sa[1], retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, sa[2], retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, sa[3], retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, sa[4], retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, sa[5], retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, hiprior, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, snap, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, udpnocs, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, ttlst, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, dirtx, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, l3csok, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, l4csok, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, ipfo0_n, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, llcothr, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, urlmch, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, extspa, retval, FAILED );
	CHECK_EQUAL( hsb1, hsb2, extl2, retval, FAILED );

	return retval;
}


int32 compHsa( hsa_param_t* hsa1, hsa_param_t* hsa2 )
{
	int32 retval = SUCCESS;
	
	CHECK_EQUAL( hsa1, hsa2, nhmac[0], retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, nhmac[1], retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, nhmac[2], retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, nhmac[3], retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, nhmac[4], retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, nhmac[5], retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, trip, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, port, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, l3csdt, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, l4csdt, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, egif, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, l2tr, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, l34tr, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, dirtxo, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, typeo, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, snapo, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, rxtag, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, dvid, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, pppoeifo, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, pppidx, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, leno, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, l3csoko, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, l4csoko, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, frag, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, lastfrag, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, ipmcastr, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, svid, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, fragpkt, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, ttl_1if, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, dpc, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, spao, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, hwfwrd, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, dpext, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, spaext, retval, FAILED );
	CHECK_EQUAL( hsa1, hsa2, why2cpu, retval, FAILED );

	return retval;
}


/*
 *  Test the READ_VIR32()/READ_VIR16()/READ_VIR8() functions.
 *  READ_VIR*() functions are availiable only in MODEL mode.
 */
static uint8 forTestVir[4];
int32 testModel_readvir(uint32 caseNo)
{
	int32 retval = FAILED;
	uint8 *cp;
	uint32 var32;
	uint16 var16;
	uint8 var8;
	int i;

	/* for( i = IC_TYPE_MODEL; i < IC_TYPE_MODEL; i++ ) */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		/* We first insert a known value */
		cp = (unsigned char*)forTestVir;
		cp[0] = 0x01;
		cp[1] = 0x23;
		cp[2] = 0x45;
		cp[3] = 0x67;
	
		/* Then, read with 32 bits */
		var32 = READ_VIR32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "READ_VIR32() error", var32, 0x01234567, __FUNCTION__, __LINE__ );
		
		/* Second, read 16 bits */
		var16 = READ_VIR16( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "READ_VIR16() error", var16, 0x0123, __FUNCTION__, __LINE__ );
		var16 = READ_VIR16( (uint32)forTestVir+2 );
		IS_EQUAL_INT( "READ_VIR16() error", var16, 0x4567, __FUNCTION__, __LINE__ );

		/* Third, read 8-bits */
		var8 = READ_VIR8( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "READ_VIR8() error", var8, 0x01, __FUNCTION__, __LINE__ );
		var8 = READ_VIR8( (uint32)forTestVir+1 );
		IS_EQUAL_INT( "READ_VIR8() error", var8, 0x23, __FUNCTION__, __LINE__ );
		var8 = READ_VIR8( (uint32)forTestVir+2 );
		IS_EQUAL_INT( "READ_VIR8() error", var8, 0x45, __FUNCTION__, __LINE__ );
		var8 = READ_VIR8( (uint32)forTestVir+3 );
		IS_EQUAL_INT( "READ_VIR8() error", var8, 0x67, __FUNCTION__, __LINE__ );
		
	}

	return SUCCESS;
}


/*
 *  Test the WRITE_VIR32()/WRITE_VIR16()/WRITE_VIR8() functions.
 *  WRITE_VIR*() functions are availiable only in MODEL mode.
 */
int32 testModel_writevir(uint32 caseNo)
{
	int32 retval = FAILED;
	uint8 *cp;
	uint32 var32;
	int i;

	/* for( i = IC_TYPE_MODEL; i < IC_TYPE_MODEL; i++ ) */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		/* We first insert a known value */
		cp = (unsigned char*)forTestVir;
	
		/* Then, write with 32 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_VIR32( (uint32)forTestVir+0, 0x01234567 );
		var32 = READ_VIR32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "READ_VIR32() error", var32, 0x01234567, __FUNCTION__, __LINE__ );
		
		/* Second, write high 16 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_VIR16( (uint32)forTestVir+0, 0x0123 );
		var32 = READ_VIR32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "WRITE_VIR16() error", var32, 0x0123FFFF, __FUNCTION__, __LINE__ );

		/* write low 16 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_VIR16( (uint32)forTestVir+2, 0x4567 );
		var32 = READ_VIR32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "WRITE_VIR16() error", var32, 0xFFFF4567, __FUNCTION__, __LINE__ );

		/* write highest 8 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_VIR8( (uint32)forTestVir+0, 0x01 );
		var32 = READ_VIR32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "WRITE_VIR16() error", var32, 0x01FFFFFF, __FUNCTION__, __LINE__ );

		/* write 2nd highest 8 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_VIR8( (uint32)forTestVir+1, 0x23 );
		var32 = READ_VIR32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "WRITE_VIR16() error", var32, 0xFF23FFFF, __FUNCTION__, __LINE__ );

		/* write 2nd lowest 8 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_VIR8( (uint32)forTestVir+2, 0x45 );
		var32 = READ_VIR32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "WRITE_VIR16() error", var32, 0xFFFF45FF, __FUNCTION__, __LINE__ );

		/* write lowest 8 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_VIR8( (uint32)forTestVir+3, 0x67 );
		var32 = READ_VIR32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "WRITE_VIR16() error", var32, 0xFFFFFF67, __FUNCTION__, __LINE__ );

		
	}

	return SUCCESS;
}


/*
 *  Test the WRITE_MEM32()/WRITE_MEM16()/WRITE_MEM8() functions.
 *  WRITE_MEM*() functions are availiable only in MODEL mode.
 */
int32 testModel_writemem(uint32 caseNo)
{
	int32 retval = FAILED;
	uint8 *cp;
	uint32 var32;
	int i;

	/* for( i = IC_TYPE_MODEL; i < IC_TYPE_MODEL; i++ ) */
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		/* We first insert a known value */
		cp = (unsigned char*)forTestVir;
	
		/* Then, write with 32 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_MEM32( (uint32)forTestVir+0, 0x01234567 );
		var32 = READ_MEM32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "READ_MEM32() error", var32, 0x01234567, __FUNCTION__, __LINE__ );
		
		/* Second, write high 16 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_MEM16( (uint32)forTestVir+0, 0x0123 );
		var32 = READ_MEM32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "WRITE_MEM16() error", var32, 0x0123FFFF, __FUNCTION__, __LINE__ );

		/* write low 16 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_MEM16( (uint32)forTestVir+2, 0x4567 );
		var32 = READ_MEM32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "WRITE_MEM16() error", var32, 0xFFFF4567, __FUNCTION__, __LINE__ );

		/* write highest 8 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_MEM8( (uint32)forTestVir+0, 0x01 );
		var32 = READ_MEM32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "WRITE_MEM16() error", var32, 0x01FFFFFF, __FUNCTION__, __LINE__ );

		/* write 2nd highest 8 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_MEM8( (uint32)forTestVir+1, 0x23 );
		var32 = READ_MEM32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "WRITE_MEM16() error", var32, 0xFF23FFFF, __FUNCTION__, __LINE__ );

		/* write 2nd lowest 8 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_MEM8( (uint32)forTestVir+2, 0x45 );
		var32 = READ_MEM32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "WRITE_MEM16() error", var32, 0xFFFF45FF, __FUNCTION__, __LINE__ );

		/* write lowest 8 bits */
		cp[0] = 0xFF; cp[1] = 0xFF; cp[2] = 0xFF; cp[3] = 0xFF;
		WRITE_MEM8( (uint32)forTestVir+3, 0x67 );
		var32 = READ_MEM32( (uint32)forTestVir+0 );
		IS_EQUAL_INT( "WRITE_MEM16() error", var32, 0xFFFFFF67, __FUNCTION__, __LINE__ );

		
	}

	return SUCCESS;
}


/*
 *  Test the random function.
 */
int32 testModel_random(uint32 caseNo)
{
	int32 retval = FAILED;
	int i, j;
	
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		rtlglue_srandom( modelRandomSeed );
		rtlglue_printf( "\ni=%d\n", i );
		for( j = 0; j < 0x40; j++ )
		{
			rtlglue_printf( "%08x  ", rtlglue_random() );
		}
	}

	return SUCCESS;
}


/*
 *  This function tests the concurrences between model_tryTestTarget() and model_setTestTarget().
 *  We expect the return value is identical for every IC type.
 */
int32 testModel_setTestTarget(uint32 caseNo)
{
	enum IC_TYPE i;
	
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		ASSERT( model_tryTestTarget(i)==model_setTestTarget(i) );
	}
	
	return SUCCESS;
}


/*
*  Test the 802.1d Reserved Address
*
*  We expect these packet with DMAC=01:80:C2:00:00:0x will be trapped to CPU.
*/
int32 testModel_802_1dAdress(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 0,
		len: 64,
		vid: 0x124,
		tagif: 1,
		pppoeif: 1,
		sip: 0x11223344,
		sprt: 0x5566,
		dip: 0x778899aa,
		dprt: 0xbbcc,
		ipptl: 0xdd,
		ipfg: 0x0,
		iptos: 0x65,
		tcpfg: 0x00,
		type: 0,
		patmatch: 1,
		ethtype: 0x7788,
		da: { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x00 },
		sa: { 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc },
		hiprior: 0,
		snap: 1,
		udpnocs: 0,
		ttlst: 2,
		dirtx: 0,
		l3csok: 1,
		l4csok: 1,
		ipfo0_n: 1,
		llcothr: 0,
		urlmch: 1,
		extspa: 0,
		extl2: 0,
	};
	hsa_param_t hsa;
	int i, da5;

	for( da5 = 0; da5 <= 0xf; da5++ )
	{
		hsb.da[5] = da5;

		/* Compare the HSA of expected, model, FPGA, and real IC. */
		for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
		{
			retval = model_setTestTarget( i );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			TESTING_MODE;
			rtl8651_setAsicOperationLayer( 2 );
			WRITE_MEM32( RMACR, MADDR00_10|MADDR00 );
			WRITE_MEM32(QIDDPCR,(1<<PBP_PRI_OFFSET));
			WRITE_MEM32(PBPCR,7);
			
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			IS_EQUAL_INT("Pakcet should be trapped to NOTPHY.", hsa.dp, PM_PORT_NOTPHY, __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Pakcet should be trapped to CPU.", hsa.dpext, PM_PORT_CPU, __FUNCTION__, __LINE__ );
		}
	}
	
	retval = SUCCESS;
	return retval;
}


/*
*  Test the vlan ingress check model code
*
*  We expect these packet with DMAC=01:80:C2:00:00:0x will be trapped to CPU.
*/
int32 testModel_vlanIngressCheck(uint32 caseNo)
{
	int32 retval = SUCCESS;
	hsb_param_t hsb =
	{
		spa: 0,
		len: 64,
		vid: 0x234,
		tagif: 0,
		pppoeif: 1,
		sip: 0x11223344,
		sprt: 0x5566,
		dip: 0x778899aa,
		dprt: 0xbbcc,
		ipptl: 0xdd,
		ipfg: 0x0,
		iptos: 0x65,
		tcpfg: 0x00,
		type: 0,
		patmatch: 1,
		ethtype: 0x7788,
		da: { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 },
		sa: { 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc },
		hiprior: 0,
		snap: 1,
		udpnocs: 0,
		ttlst: 2,
		dirtx: 0,
		l3csok: 1,
		l4csok: 1,
		ipfo0_n: 1,
		llcothr: 0,
		urlmch: 1,
		extspa: 0,
		extl2: 0,
	};

	hsa_param_t hsa;
	int i, port;
	_rtl8651_tblDrvAclRule_t rule;
	rtl865x_tblAsicDrv_vlanParam_t vlan;
	rtl865x_tblAsicDrv_intfParam_t intf;

	retval = model_setTestTarget( IC_TYPE_MODEL );
	if ( retval == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */

	/* Enable ingress filtering. */
	rtl8651_setAsicOperationLayer( 2 );
	WRITE_MEM32( VCR0, READ_MEM32(VCR0)| 0x1ff/*EN_VLAN_INGRESS_FILTER*/ ); /* Enable VLAN Ingress Filtering for every port (port0 ~ port8) */

	/* Set PVID of all port to VLAN 8. */
	WRITE_MEM32(( PVCR0+0*4), ((0x8<<0)|(0x8<<16)|(0x4<<12)) );
	WRITE_MEM32(( PVCR0+1*4), ((0x8<<0)|(0x8<<16)) );
	WRITE_MEM32(( PVCR0+2*4), ((0x8<<0)|(0x8<<16)) );

	/* Set permit rule. */
	bzero(&rule, sizeof(_rtl8651_tblDrvAclRule_t));
	rule.actionType_ = RTL8651_ACL_PERMIT;
	rule.pktOpApp = RTL8651_ACLTBL_ONLY_L2;
	rule.ruleType_ = RTL8651_ACL_MAC;
	rtl8651_setAsicAclRule( 0, &rule);

	/* Config VLAN 8 (vidx:0) */
	bzero( &vlan, sizeof(vlan) );
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

	for( port = PN_PORT0; port <= PN_PORT5; port++ )
	{
		hsb.spa = port;
		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = 0x1f & (~( 1 << port ));
		rtl8651_setAsicVlan( intf.vid, &vlan );
		
		/* Compare the HSA of expected, model, FPGA, and real IC. */
		for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
		{
			retval = model_setTestTarget( i );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
			TESTING_MODE;
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			IS_EQUAL_INT("Pakcet shouldn't  be forwarded, however forward", hsa.dp, 0, __FUNCTION__, __LINE__ );
		}

		/* let VLAN has no member port, all packets should be dropped. */
		hsb.spa = port;
		bzero( &vlan, sizeof(vlan) );
		vlan.memberPortMask = 0;
		rtl8651_setAsicVlan( intf.vid, &vlan );
		
		/* Compare the HSA of expected, model, FPGA, and real IC. */
		for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
		{
			retval = model_setTestTarget( i );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
			
			retval = virtualMacSetHsb( &hsb );
			assert( retval == SUCCESS );
			retval = virtualMacGetHsa( &hsa );
			IS_EQUAL_INT("Pakcet shoulde be dropped !", hsa.dp, 0, __FUNCTION__, __LINE__ );
		}

	}

	retval = SUCCESS;	
	return retval;
}


/*
 *  Check if CPU can read/write every bits in swcore tables.
 */
int32 testSwcoreTables( uint32 caseNo )
{
	struct _VECTOR_
	{
		uint32 tableType; /* Tables type */
		uint32 numEntry; /* number of entry of this table */
		uint32 expect[8]; /* expect bits */
	} vectors[16]; /* for all swcore tables */
	uint32 numTV; /* number of test vector */
	uint32 i;
	int32 retval;
#ifdef CONFIG_RTL865X_MODEL_TEST_FT2
	uint16 ft2_result0 = 0;	// for test all 0 pattern
	uint16 ft2_result1 = 0;	// for test all 1 pattern
#endif

	/* initilize testing vectors */
	memset( vectors, 0, sizeof(vectors) );
	numTV = 0;
	{
		{
			vectors[numTV].tableType = TYPE_L2_SWITCH_TABLE;
			vectors[numTV].numEntry = 1024;
			vectors[numTV].expect[0] = 0xffffffff;
			vectors[numTV].expect[1] = 0x03ffffff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_ARP_TABLE;
			vectors[numTV].numEntry = 512;
			vectors[numTV].expect[0] = 0x0000ffff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_L3_ROUTING_TABLE;
			vectors[numTV].numEntry = 8;
			vectors[numTV].expect[0] = 0xffffffff;
			vectors[numTV].expect[1] = 0x1fffffff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_MULTICAST_TABLE;
			vectors[numTV].numEntry = 64;
			vectors[numTV].expect[0] = 0xffffffff;
			vectors[numTV].expect[1] = 0xffffffff;
			vectors[numTV].expect[2] = 0x3fffffff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_NETINTERFACE_TABLE;
			vectors[numTV].numEntry = 8;
			vectors[numTV].expect[0] = 0xffffffff;
			vectors[numTV].expect[1] = 0xffffffff;
			vectors[numTV].expect[2] = 0xffffffff;
			vectors[numTV].expect[3] = 0x00000fff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_EXT_INT_IP_TABLE;
			vectors[numTV].numEntry = 16;
			vectors[numTV].expect[0] = 0xffffffff;
			vectors[numTV].expect[1] = 0xffffffff;
			vectors[numTV].expect[2] = 0x000000ff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_VLAN_TABLE;
			vectors[numTV].numEntry = 2048;
			vectors[numTV].expect[0] = 0x000fffff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_VLAN1_TABLE;
			vectors[numTV].numEntry = 2048;
			vectors[numTV].expect[0] = 0x000fffff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_SERVER_PORT_TABLE;
			vectors[numTV].numEntry = 16;
			vectors[numTV].expect[0] = 0xffffffff;
			vectors[numTV].expect[1] = 0xffffffff;
			vectors[numTV].expect[2] = 0xffffffff;
			vectors[numTV].expect[3] = 0x000000ff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_L4_TCP_UDP_TABLE;
			vectors[numTV].numEntry = 1024;
			vectors[numTV].expect[0] = 0xffffffff;
			vectors[numTV].expect[1] = 0xffffffff;
			vectors[numTV].expect[2] = 0x00ffffff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_L4_ICMP_TABLE;
			vectors[numTV].numEntry = 32;
			vectors[numTV].expect[0] = 0xffffffff;
			vectors[numTV].expect[1] = 0xffffffff;
			vectors[numTV].expect[2] = 0x1fffffff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_PPPOE_TABLE;
			vectors[numTV].numEntry = 8;
			vectors[numTV].expect[0] = 0x0007ffff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_ACL_RULE_TABLE;
			vectors[numTV].numEntry = 128;
			vectors[numTV].expect[0] = 0xffffffff;
			vectors[numTV].expect[1] = 0xffffffff;
			vectors[numTV].expect[2] = 0xffffffff;
			vectors[numTV].expect[3] = 0xffffffff;
			vectors[numTV].expect[4] = 0xffffffff;
			vectors[numTV].expect[5] = 0xffffffff;
			vectors[numTV].expect[6] = 0xffffffff;
			vectors[numTV].expect[7] = 0x07ffffff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_NEXT_HOP_TABLE;
			vectors[numTV].numEntry = 32;
			vectors[numTV].expect[0] = 0x001fffff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_RATE_LIMIT_TABLE;
			vectors[numTV].numEntry = 32;
			vectors[numTV].expect[0] = 0x3fffffff;
			vectors[numTV].expect[1] = 0x3fffffff;
			vectors[numTV].expect[2] = 0x00ffffff;
			numTV++;
		}
	}
	{
		{
			vectors[numTV].tableType = TYPE_ALG_TABLE;
			vectors[numTV].numEntry = 48;
			vectors[numTV].expect[0] = 0x000fffff;
			numTV++;
		}
	}

#ifdef CONFIG_RTL865X_MODEL_TEST_FT2
	mdelay(200); /* to make sure the FullReset process of the switch core is stable before R/W tables */
#endif

	/* start test ... */
	retval = model_setTestTarget( IC_TYPE_REAL );
	if ( retval == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */

	rtlglue_printf( "\n" );
	for( i = 0; i < numTV; i++ )
	{
		uint32 eidx; /* entry index */
		uint32 word[8];
		uint32 allzero[8] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
		uint32 allone[8] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};

		for( eidx = 0; eidx < vectors[i].numEntry; eidx++ )
		{
#ifdef CONFIG_RTL865X_MODEL_TEST_FT2
	/* To speed up, do not show any message */
#else
			rtlglue_printf( "\r[tableType=%2d,eidx=%d]...", vectors[i].tableType, eidx );
#endif

			/********************************************************
			 * Test all 0 pattern
			 ********************************************************/
			retval = _rtl8651_forceAddAsicEntry( vectors[i].tableType, eidx, allzero );
			ASSERT( retval==SUCCESS );

			memset( word, 0, sizeof(word) );
			retval = _rtl8651_readAsicEntry( vectors[i].tableType, eidx, word );
			ASSERT( retval==SUCCESS );

			{
				int w; /* word index */
				for( w = 0; w <= 7; w++ )
				{
					if ( word[w] != 0 )
					{
						rtlglue_printf( "\n[tableType=%2d,eidx=%d] all zero test fail, word[%d]=0x%08x, expect=0x00000000\n", vectors[i].tableType, eidx, w, word[w] );
						/*return FAILED;*/
#ifdef	CONFIG_RTL865X_MODEL_TEST_FT2
						ft2_result0 |= (1 << vectors[i].tableType);	// Fail
#endif
					}
				}
			}

			/********************************************************
			 * Test all 1 pattern
			 ********************************************************/
			retval = _rtl8651_forceAddAsicEntry( vectors[i].tableType, eidx, allone );
			ASSERT( retval==SUCCESS );

			memset( word, 0, sizeof(word) );
			retval = _rtl8651_readAsicEntry( vectors[i].tableType, eidx, word );
			ASSERT( retval==SUCCESS );

			{
				int w; /* word index */
				for( w = 0; w <= 7; w++ )
				{
					if ( word[w] != vectors[i].expect[w] )
					{
						rtlglue_printf( "\n[tableType=%2d,eidx=%d] all one test fail, word[%d]=0x%08x, expect=0x%08x\n", vectors[i].tableType, eidx, w, word[w], vectors[i].expect[w] );
						/*return FAILED;*/
#ifdef	CONFIG_RTL865X_MODEL_TEST_FT2
						ft2_result1 |= (1 << vectors[i].tableType);	// Fail
#endif
					}
				}
			}
		}
	}
	
#ifdef CONFIG_RTL865X_MODEL_TEST_FT2
	for( i = 0; i < numTV; i++ )
	{
		if ((ft2_result0 & (1 << i)) || (ft2_result1 & (1 << i))) {
			rtlglue_printf("FAIL!{F0%02d}\n", i + 50);
		} else {
			rtlglue_printf("PASS!{P0%02d}\n", i + 50);
		}
	}
		
	if ((ft2_result0 != 0) || (ft2_result1 != 0)) {
		return FAILED;
	}
	/* To speed up, do not show any message */
#else
	rtlglue_printf( "\nTest Done\n" );
#endif /* end of  CONFIG_RTL865X_MODEL_TEST_FT2 */
	
	return SUCCESS;
}




/****************************************************************************
 ****__*****__*************                                                ** 
 ***|  |***|  |****__******                                                ** 
 ***|  |***|  |***|  |*****                                                ** 
 ***|  |***|  |***|__|*****                                                ** 
 ***|  |***|  |************      Add Your Model Test Cases HERE !!!        ** 
 ***|   ___   |****__******                                                ** 
 ***|  |***|  |***|  |*****                                                ** 
 ***|  |***|  |***|  |*****                                                ** 
 ***|  |***|  |***|  |*****                                                ** 
 ***|  |***|  |***|  |*****                                                ** 
 ***|__|***|__|***|__|*****                                                **
 ****************************************************************************/
MODEL_TEST_CASE_T modelTestCase[] =
{
	MODEL_TEST_CASE(            1, testModel_readvir, GRP_ALL ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testModel_writevir, GRP_ALL ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testModel_writemem, GRP_ALL ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testModel_random, GRP_ALL ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testModel_setTestTarget, GRP_ALL ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testModel_802_1dAdress, GRP_ALL | GRP_PRE_CORE ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testModel_vlanIngressCheck, GRP_ALL | GRP_PRE_CORE ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testSwcoreTables, GRP_ALL ),

	/* GDMA */
	MODEL_TEST_CASE(        10101, testGdmaPatternMatch1, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaPatternMatch2, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaPatternMatch3, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaPatternMatch4, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaPatternMatch5, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaPatternMatch6, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaPatternMatch10, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaPatternMatch11, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaPatternMatch12, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaPatternMatch13, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaPatternMatch14, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaPatternMatch15, GRP_ALL | GRP_GDMA ),

	MODEL_TEST_CASE(        10201, testGdmaMemory1, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory2, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory3, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory4, GRP_ALL | GRP_GDMA ),
#ifdef RTL865X_MODEL_KERNEL
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory5, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory6, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory7, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory8, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory9, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory10, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory11, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory12, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory13, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory14, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaMemory15, GRP_ALL | GRP_GDMA ),
#endif
	MODEL_TEST_CASE(        10301, testGdmaChecksum1, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaChecksum2, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE(        10401, testGdmaSequentialTCAM, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaSequentialTCAM2, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE(        10501, testGdmaWirelessMIC, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaWirelessMIC2, GRP_ALL | GRP_GDMA ),
#ifdef RTL865X_MODEL_KERNEL
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaWirelessMIC3, GRP_ALL | GRP_GDMA ),
#endif
	MODEL_TEST_CASE(        10601, testGdmaBase64Encode1, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaBase64Encode2, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaBase64Encode3, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaBase64Decode1, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaBase64Decode2, GRP_ALL | GRP_GDMA ),
#ifdef RTL865X_MODEL_USER
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaBase64EncDec1, GRP_ALL | GRP_GDMA ),
#endif
#ifdef RTL865X_MODEL_KERNEL
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaBase64EncDec2, GRP_ALL | GRP_GDMA ),
#endif
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaQuotedPrintableEncode1, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaQuotedPrintableDecode1, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaQuotedPrintableDecode2, GRP_ALL | GRP_GDMA ),
#ifdef RTL865X_MODEL_KERNEL
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaQuotedPrintableEncDec2, GRP_ALL | GRP_GDMA ),
#endif
	MODEL_TEST_CASE(        10701, testGdmaByteSwap1, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaByteSwap2, GRP_ALL | GRP_GDMA ),
#ifdef RTL865X_MODEL_KERNEL
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaByteSwap3, GRP_ALL | GRP_GDMA ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testGdmaByteSwap4, GRP_ALL | GRP_GDMA ),
#endif
	MODEL_TEST_CASE(        10001, testGdmaEnumBAL, GRP_NONE ),

	/* NIC */
	MODEL_TEST_CASE(        30000, testNicTx, GRP_ALL | GRP_NIC ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicTxQueuePriority, GRP_ALL | GRP_NIC ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicRx, GRP_ALL | GRP_NIC ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicRxRunout, GRP_ALL | GRP_NIC ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicRxPriority, GRP_ALL | GRP_NIC ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicRegs, GRP_ALL | GRP_NIC ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicRxQueueidDescringMapping, GRP_ALL | GRP_NIC ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicTxPppoeTagOffload, GRP_ALL | GRP_NIC ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicTxCrcCheck, GRP_ALL | GRP_NIC ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicTxJumboFrame, GRP_ALL | GRP_NIC ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicRxJumboFrame, GRP_ALL | GRP_NIC ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicRxPkthdr, GRP_ALL | GRP_NIC ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicRxMbufNewSpec, GRP_ALL | GRP_NIC ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicLoopback, GRP_ALL | GRP_NIC ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicTxTagAddRemoveModify, GRP_ALL | GRP_NIC ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicTxGreChecksumOffload, GRP_ALL | GRP_NIC ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicTxMibCounter, GRP_ALL | GRP_NIC ),		
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicCpuReason, GRP_ALL | GRP_NIC ),		
	MODEL_TEST_CASE( AUTO_CASE_NO, testNicRxDRR, GRP_ALL | GRP_NIC ),	

	/* Parser */
	MODEL_TEST_CASE(        40000, parserTagForDirectTXfromCPU, GRP_ALL | GRP_PKT_PARSER ),
	MODEL_TEST_CASE( AUTO_CASE_NO, parserPatternMatch, GRP_ALL | GRP_PKT_PARSER ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, parserChecksumCheck, GRP_ALL | GRP_PKT_PARSER ),
	MODEL_TEST_CASE( AUTO_CASE_NO, parserUrlTrap, GRP_ALL | GRP_PKT_PARSER ),
	MODEL_TEST_CASE( AUTO_CASE_NO, parserRxFromPhysicalPort, GRP_ALL | GRP_PKT_PARSER ),
	MODEL_TEST_CASE( AUTO_CASE_NO, parserRxFromExtPort, GRP_ALL | GRP_PKT_PARSER ),

	/* Translator */
	MODEL_TEST_CASE(        45000, testTranslatorOnly1, GRP_ALL | GRP_PKT_TRANSLATOR ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testModelChainPMC, GRP_ALL | GRP_PKT_TRANSLATOR ),
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testTranslatorOnlyNIC, GRP_ALL | GRP_PKT_TRANSLATOR ), */

	/* Layer 2*/
	MODEL_TEST_CASE(        50000, testLayer2Forward, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2Multicast, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testSpanningTree, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testProtocolBaseVlan, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testRandomPrioritySelection, GRP_ALL | GRP_L2 ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testPrioritySelection, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testRandom8021Q, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testRandom8021X, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testReservedMulticast, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2Aging, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testPatternMatch, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2PriorityTag, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2UniLearning, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2UniLRULearning, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2FIDLearning, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2UnwareVlan, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2VlanNoExist, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2UnkonwUniCast, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer28021XPortBase, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer28021XMacBase, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer28021XGuestVlan,GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2Forward2,GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2Forward3,GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2Forward4,GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2Forward5,GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2Jumbo,GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2TrapToCpu, GRP_ALL | GRP_L2 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testRandomSpanningTree, GRP_ALL | GRP_L2 ),
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2Static, GRP_ALL | GRP_L2 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2SourceBlock, GRP_ALL | GRP_L2 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2NHFlag, GRP_ALL | GRP_L2 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testLayer2Auth, GRP_ALL | GRP_L2 ), */

	/* ARP */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testArpAging,GRP_ALL|GRP_L34), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testRouteWithoutArpEntry, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testRouteArpWithoutL2Entry, GRP_ALL | GRP_L34 ), */

	/* VLAN */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testVlanUntag, GRP_ALL | GRP_L2 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testProtcolBase, GRP_ALL | GRP_L34 ),	*/

	/* Rate Limit */

	/* Net Interface */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testDmacIsGmac, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testDmacIsGmacWithoutEnableRouting, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testNetInterfaceMacMask, GRP_ALL | GRP_L34 ), */

	/* L3 Routing */
	MODEL_TEST_CASE(        60000, testLayer3MTU, GRP_ALL | GRP_L34 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer3Routing, GRP_ALL | GRP_L34 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer3GuestVLAN, GRP_ALL | GRP_L34 ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testEnhancedGRE, GRP_ALL | GRP_L34 ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testIpOther, GRP_ALL | GRP_L34 ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer3RoutingToCpu, GRP_ALL | GRP_L34 ),
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testL3RoutingInternal, GRP_ALL | GRP_L34 ), */
	
	/* Multicast */
	MODEL_TEST_CASE( AUTO_CASE_NO, testIPMulticast, GRP_ALL|GRP_L34),
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testMulticastWithSVID, GRP_ALL | GRP_L34 ), */

	/*Layer 4*/
	/* Server Port */
	MODEL_TEST_CASE(        70000, testServerPort, GRP_ALL | GRP_L34 ),
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testServerPortTcpOnly, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testServerPortUdpOnly, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testServerPortPriority, GRP_ALL | GRP_L34 ), */
	MODEL_TEST_CASE( AUTO_CASE_NO, testPktIPMulticast, GRP_ALL | GRP_L34 ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testPktRemarking, GRP_ALL | GRP_L34 ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testPktPPPoE, GRP_ALL | GRP_L34 ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testPktGRE, GRP_ALL | GRP_L34 ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testPktServerPort, GRP_ALL | GRP_L34 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testPktRemarkingRandom, GRP_ALL | GRP_L34 ),	

	/* ICMP */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testIcmpOutboundOnly, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testIcmpInboundOnly, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testIcmpBidir, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testPPTP,GRP_ALL | GRP_L34 ), */
	
	/* ACL */
	MODEL_TEST_CASE( AUTO_CASE_NO, testAclIgmpRule, GRP_ALL | GRP_L34 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testACLIcmpRule, GRP_ALL | GRP_L34 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testACLDstRule, GRP_ALL | GRP_L34 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testACLSrcRule, GRP_ALL | GRP_L34 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testALG, GRP_ALL | GRP_L34 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testLocalPublicIP, GRP_ALL | GRP_L34 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testACLIPRule, GRP_ALL | GRP_L34 ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testACLTCPRule, GRP_ALL | GRP_L34 ),		
	MODEL_TEST_CASE( AUTO_CASE_NO, testACLUDPRule, GRP_ALL | GRP_L34 ),

	/* NAPT */
	MODEL_TEST_CASE(        80000, testNaptLanToWan, GRP_ALL | GRP_NAPT ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testNaptLanToWan2, GRP_ALL | GRP_NAPT ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testNaptLanToWanPriority, GRP_ALL | GRP_NAPT ),
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testNaptHash1, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testNaptHash2, GRP_ALL | GRP_L34 ), */
	MODEL_TEST_CASE( AUTO_CASE_NO, testLayer4Priority, GRP_ALL | GRP_L34 ),	
	MODEL_TEST_CASE( AUTO_CASE_NO, testNaptEnhancedHash1, GRP_ALL | GRP_NAPT ),
	
	/*Driver test*/
	MODEL_TEST_CASE( AUTO_CASE_NO, AddNaptConnectionErrors, GRP_NONE ),/*05*/
	MODEL_TEST_CASE( AUTO_CASE_NO, NaptTypicalCase, GRP_NONE )	,
	MODEL_TEST_CASE( AUTO_CASE_NO, NaptTypicalCaseWith4Way, GRP_NONE ),
	MODEL_TEST_CASE( AUTO_CASE_NO, NaptTestL04, GRP_NONE ),
	MODEL_TEST_CASE( AUTO_CASE_NO, NaptTestL04UdpForCDRouter, GRP_NONE ),
	MODEL_TEST_CASE( AUTO_CASE_NO, testNapt4WayHash, GRP_NONE ),/*10*/
	MODEL_TEST_CASE( AUTO_CASE_NO, DMZHostUsingHash1, GRP_NONE ),
	MODEL_TEST_CASE( AUTO_CASE_NO, ServerPortUsingOneHash2Entry, GRP_NONE ),
	MODEL_TEST_CASE( AUTO_CASE_NO, ServerPortUsingTwoHash2Entries, GRP_NONE ),
	MODEL_TEST_CASE( AUTO_CASE_NO, ServerPortAndNewHash1, GRP_NONE ),
	MODEL_TEST_CASE( AUTO_CASE_NO, TestUpdateNaptConnectionHybrid1, GRP_NONE ),/*15*/
	
	MODEL_TEST_CASE( AUTO_CASE_NO, NaptEnhanceHash1TypicalCase, GRP_NONE ),
	/*MODEL_TEST_CASE( AUTO_CASE_NO, PacketCase, GRP_ALL | GRP_NAPT ),*/
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testSPI,GRP_ALL|GRP_L34), */

	/* ALG */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testALGTcpOnly, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testALGUdpOnly, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testALGOutboundOnly, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testALGInboundOnly, GRP_ALL | GRP_L34 ), */
	/* MODEL_TEST_CASE( AUTO_CASE_NO, testALGBidir, GRP_ALL | GRP_L34 ), */

#ifndef RTL865X_MODEL_KERNEL	/* When compiling model test for FPGA, we don't compile driver test cases. */
	DRV_TEST_CASE( 101, arp_test1, GROUP_TBLDRV ),
	DRV_TEST_CASE( 102, arp_test2, GROUP_TBLDRV ),
	DRV_TEST_CASE( 103, arp_test3, GROUP_TBLDRV ),
	DRV_TEST_CASE( 104, arp_test4, GROUP_TBLDRV ),
	DRV_TEST_CASE( 105, arp_test5, GROUP_TBLDRV ),
	DRV_TEST_CASE( 106, arp_test6, GROUP_TBLDRV ),
	DRV_TEST_CASE( 107, arp_test7, GROUP_TBLDRV ),
	DRV_TEST_CASE( 108, arp_test8, GROUP_TBLDRV ),

	/* Test L3 Table Case ID:		201 -  300 */
	DRV_TEST_CASE( 201, l3_test1, GROUP_TBLDRV ),

	/* Test PPPoE Table Case ID:        	1101 - 1200 */
	DRV_TEST_CASE( 1101, pppoe_test1, GROUP_TBLDRV ),
	DRV_TEST_CASE( 1102, pppoe_test2, GROUP_TBLDRV ),
	DRV_TEST_CASE( 1103, pppoe_test3, GROUP_TBLDRV ),
	DRV_TEST_CASE( 1104, pppoe_test10, GROUP_TBLDRV ),
	DRV_TEST_CASE( 1105, pppoe_test11, GROUP_TBLDRV ),
	DRV_TEST_CASE( 1106, pppoe_test12, GROUP_TBLDRV ),
	DRV_TEST_CASE( 1107, pppoe_test13, GROUP_TBLDRV ),
	DRV_TEST_CASE( 1108, pppoe_test14, GROUP_TBLDRV ),
	
	/* Test: IP interface: 1801 - 1900 */
	DRV_TEST_CASE( 1801, ip_interface_test_1, GROUP_TBLDRV ),

	/* software VLAN: 5158 - 5160 */
#ifdef RTL865XB_DCUT_SWVLAN
	DRV_TEST_CASE( 5158, swVlan_test1, GROUP_TBLDRV ),
	DRV_TEST_CASE( 5159, swVlan_test2, GROUP_TBLDRV ),
#endif

#endif

	/* Final case, DO NOT remove it */
	MODEL_TEST_CASE(       200000, NULL, GRP_ALL ),
};

#ifdef CONFIG_RTL865X_MODEL_TEST_FT2
MODEL_TEST_CASE_T ft2TestCase[] =
{
	MODEL_TEST_CASE(  1, ft2_testImem, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE(  2, ft2_testDmem, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE(  3, testNicTxJumboFrame, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE(  4, testNicRxJumboFrame, GRP_ALL | GRP_FT2 ),	
	MODEL_TEST_CASE(  5, testSwcoreTables, GRP_ALL | GRP_FT2 ),
	/* FT2: GDMA */
	MODEL_TEST_CASE( 11, testGdmaPatternMatch1, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 12, testGdmaMemory1, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 13, testGdmaChecksum1, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 14, testGdmaSequentialTCAM, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 15, testGdmaWirelessMIC, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 16, testGdmaBase64Encode1, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 17, testGdmaBase64Decode1, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 18, testGdmaQuotedPrintableEncode1, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 19, testGdmaQuotedPrintableDecode1, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 20, testGdmaByteSwap1, GRP_ALL | GRP_FT2 ),
	/* FT2: Layer2 */
	MODEL_TEST_CASE( 21, testLayer2Multicast, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 22, testSpanningTree, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 23, testProtocolBaseVlan, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 24, testPrioritySelection, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 25, testLayer2Jumbo,GRP_ALL | GRP_FT2 ),
	/* FT2: Layer3/4 */
	MODEL_TEST_CASE( 26, testLayer3GuestVLAN, GRP_ALL | GRP_FT2 ),	
	MODEL_TEST_CASE( 27, testEnhancedGRE, GRP_ALL | GRP_FT2 ),	
	MODEL_TEST_CASE( 28, testIpOther, GRP_ALL | GRP_FT2 ),	
	MODEL_TEST_CASE( 29, testLayer3RoutingToCpu, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 30, testIPMulticast, GRP_ALL|GRP_FT2),
	MODEL_TEST_CASE( 31, testPktRemarking, GRP_ALL | GRP_FT2 ),	
	MODEL_TEST_CASE( 32, testPktPPPoE, GRP_ALL | GRP_FT2 ),	
	MODEL_TEST_CASE( 33, testPktServerPort, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 34, testAclIgmpRule, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 35, testACLIcmpRule, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 36, testACLDstRule, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 37, testACLSrcRule, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 38, testLocalPublicIP, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 39, testACLIPRule, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 40, testACLTCPRule, GRP_ALL | GRP_FT2 ),		
	MODEL_TEST_CASE( 41, testACLUDPRule, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 42, testLayer4Priority, GRP_ALL | GRP_FT2 ),	
	/* FT2: NAPT */
	MODEL_TEST_CASE( 43, testNaptLanToWan, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 44, testNaptLanToWan2, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 45, testNaptLanToWanPriority, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 46, testNaptEnhancedHash1, GRP_ALL | GRP_FT2 ),	
	/* FT2: PCI */
	MODEL_TEST_CASE( 60, ft2_testPCI_S0, GRP_ALL | GRP_FT2 ),	
	/* FT2: Other */
	MODEL_TEST_CASE( 70, ft2_testSram_BIST_Status, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 71, ft2_testSram_BIST_RLMRAM, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 72, ft2_testSram_BIST_MIBRAM, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 73, ft2_testSram_BIST_AMI, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 74, ft2_testSram_BIST_HTRAM, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 75, ft2_testSram_BIST_L2RAM, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 76, ft2_testSram_BIST_L4RAM, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 77, ft2_testSram_BIST_ACLRAM126, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 78, ft2_testSram_BIST_ACLRAM125, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 79, ft2_testSram_BIST_NICTXRAM, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 80, ft2_testSram_BIST_NICRXRAM, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 81, ft2_testSram_BIST_V4KRAM, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 82, ft2_testSram_BIST_OQRAM, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 83, ft2_testSram_BIST_IQRAM, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 84, ft2_testSram_BIST_HSARAM125, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 85, ft2_testSram_BIST_HSARAM121, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 86, ft2_testSram_BIST_PBRAM, GRP_ALL | GRP_FT2 ),
	/* FT2: CPU MIPS */
	MODEL_TEST_CASE( 87, ft2_testCPU_MIPS, GRP_ALL | GRP_FT2 ),
	MODEL_TEST_CASE( 88, ft2_testCPU_Arithmetic, GRP_ALL | GRP_FT2 ),
	/* FT2: USB 2.0 */	
	MODEL_TEST_CASE( 95, ft2_testUSB_P0_Detect, GRP_ALL ),
	MODEL_TEST_CASE( 96, ft2_testUSB_P0_Loopback, GRP_ALL ),
	MODEL_TEST_CASE( 97, ft2_testUSB_P1_Detect, GRP_ALL ),
	MODEL_TEST_CASE( 98, ft2_testUSB_P1_Loopback, GRP_ALL ),
	/* FT2: USB Control */
	MODEL_TEST_CASE( 101, ft2_testUSB_PowerOn, GRP_ALL ),
	MODEL_TEST_CASE( 102, ft2_testUSB_PowerOff, GRP_ALL ),
	MODEL_TEST_CASE( 103, ft2_testUSB_IncreaseVoltage, GRP_ALL ),
	MODEL_TEST_CASE( 104, ft2_testUSB_DecreaseVoltage, GRP_ALL ),

	/* Final case, DO NOT remove it */
	MODEL_TEST_CASE( 110, NULL, GRP_ALL ),
};

int32 runMpFt2(uint32 userId,  int32 argc,int8 **saved)
{
	int8 *nextToken;
	int32 size, lower, upper, i, retval;
	int32 totalCase = 0, okCase = 0, failCase = 0;	
	uint32 groupmask = 0xffffffff, total_time = 0;
	uint32 caseTotalNo = sizeof(ft2TestCase)/sizeof(ft2TestCase[0]);
	uint32 failCaseIdx[caseTotalNo];

	lower = 0;
	upper = ft2TestCase[caseTotalNo-1].no;
	
	cle_getNextCmdToken(&nextToken,&size,saved);
	if ( !strcmp( nextToken, "ft2" )) groupmask = GRP_FT2;	
	else lower = upper = U32_value(nextToken);

	if ( cle_getNextCmdToken(&nextToken,&size,saved) == SUCCESS )
		upper = U32_value(nextToken);
			
	rtlglue_printf( "Running from %d to %d with group mask 0x%08x:\n\n", lower, upper, groupmask );
	
	for( i = 0; i < caseTotalNo; i++ )
	{		
		MODEL_TEST_CASE_T *pCase = &ft2TestCase[i];
		uint32 begin_time, end_time;
		
		if ( pCase->no < lower || pCase->no > upper ) continue;
		if ( ( pCase->group & groupmask ) == 0 ) continue;
		if ( pCase->fp == NULL ) continue;

		totalCase++;		
		rtlglue_printf( "Running FT2 Test Case %d: %s() ...\n", pCase->no, pCase->name );
		rtlglue_getmstime(&begin_time);

		if ( pCase->no < 50 )
		{
			/* Prepare virtualMac environment */			
			model_setTestTarget( IC_TYPE_REAL );
	
			/* Switch Core FullReset */
			REG32(SSIR) |= FULL_RST;				
			//mdelay(100);  // OK
			//mdelay(30);  // Reset Fail, reset circuit needs 30+ ms
			mdelay(50);  
			
			// Check BIST Done?
			//while((REG32(0xBB804208)&0x7FF) != 0);
			
			REG32(SSIR) |= TRXRDY;			

			// No need ... only for double check
			//if((REG32(SSIR)&TRXRDY)!=TRXRDY) rtlglue_printf("Reset Failed\n");

			if(REG32(GISR)>>1) REG32(GISR) |= 0x1F;
			/* end of Switch Core FullReset */
			
			virtualMacInit();						
		}
					
		retval = pCase->fp( pCase->no );
	
		/* pump all packet in queue */		
		if ( pCase->no < 50 )
			virtualMacPumpPackets( PM_PORT_ALL );

		/* FT2 Testing Error Code */
		switch (pCase->no) {
		case  1: if ( retval == SUCCESS ) { rtlglue_printf("{P010}\n"); } else { rtlglue_printf("{F010}\n"); } break;
		case  2: if ( retval == SUCCESS ) { rtlglue_printf("{P020}\n"); } else { rtlglue_printf("{F020}\n"); } break;
		case  3: if ( retval == SUCCESS ) { rtlglue_printf("{P030}\n"); } else { rtlglue_printf("{F030}\n"); } break;
		case  4: if ( retval == SUCCESS ) { rtlglue_printf("{P040}\n"); } else { rtlglue_printf("{F040}\n"); } break;		
		case 11: if ( retval == SUCCESS ) { rtlglue_printf("{P101}\n"); } else { rtlglue_printf("{F101}\n"); } break;
		case 12: if ( retval == SUCCESS ) { rtlglue_printf("{P102}\n"); } else { rtlglue_printf("{F102}\n"); } break;
		case 13: if ( retval == SUCCESS ) { rtlglue_printf("{P103}\n"); } else { rtlglue_printf("{F103}\n"); } break;
		case 14: if ( retval == SUCCESS ) { rtlglue_printf("{P104}\n"); } else { rtlglue_printf("{F104}\n"); } break;
		case 15: if ( retval == SUCCESS ) { rtlglue_printf("{P105}\n"); } else { rtlglue_printf("{F105}\n"); } break;
		case 16: if ( retval == SUCCESS ) { rtlglue_printf("{P106}\n"); } else { rtlglue_printf("{F106}\n"); } break;
		case 17: if ( retval == SUCCESS ) { rtlglue_printf("{P107}\n"); } else { rtlglue_printf("{F107}\n"); } break;
		case 18: if ( retval == SUCCESS ) { rtlglue_printf("{P108}\n"); } else { rtlglue_printf("{F108}\n"); } break;
		case 19: if ( retval == SUCCESS ) { rtlglue_printf("{P109}\n"); } else { rtlglue_printf("{F109}\n"); } break;
		case 20: if ( retval == SUCCESS ) { rtlglue_printf("{P110}\n"); } else { rtlglue_printf("{F110}\n"); } break;
		case 21: if ( retval == SUCCESS ) { rtlglue_printf("{P201}\n"); } else { rtlglue_printf("{F201}\n"); } break;
		case 22: if ( retval == SUCCESS ) { rtlglue_printf("{P202}\n"); } else { rtlglue_printf("{F202}\n"); } break;
		case 23: if ( retval == SUCCESS ) { rtlglue_printf("{P203}\n"); } else { rtlglue_printf("{F203}\n"); } break;
		case 24: if ( retval == SUCCESS ) { rtlglue_printf("{P204}\n"); } else { rtlglue_printf("{F204}\n"); } break;
		case 25: if ( retval == SUCCESS ) { rtlglue_printf("{P205}\n"); } else { rtlglue_printf("{F205}\n"); } break;
		case 26: if ( retval == SUCCESS ) { rtlglue_printf("{P301}\n"); } else { rtlglue_printf("{F301}\n"); } break;
		case 27: if ( retval == SUCCESS ) { rtlglue_printf("{P302}\n"); } else { rtlglue_printf("{F302}\n"); } break;
		case 28: if ( retval == SUCCESS ) { rtlglue_printf("{P303}\n"); } else { rtlglue_printf("{F303}\n"); } break;
		case 29: if ( retval == SUCCESS ) { rtlglue_printf("{P304}\n"); } else { rtlglue_printf("{F304}\n"); } break;
		case 30: if ( retval == SUCCESS ) { rtlglue_printf("{P305}\n"); } else { rtlglue_printf("{F305}\n"); } break;
		case 31: if ( retval == SUCCESS ) { rtlglue_printf("{P306}\n"); } else { rtlglue_printf("{F306}\n"); } break;
		case 32: if ( retval == SUCCESS ) { rtlglue_printf("{P307}\n"); } else { rtlglue_printf("{F307}\n"); } break;
		case 33: if ( retval == SUCCESS ) { rtlglue_printf("{P308}\n"); } else { rtlglue_printf("{F308}\n"); } break;
		case 34: if ( retval == SUCCESS ) { rtlglue_printf("{P309}\n"); } else { rtlglue_printf("{F309}\n"); } break;
		case 35: if ( retval == SUCCESS ) { rtlglue_printf("{P310}\n"); } else { rtlglue_printf("{F310}\n"); } break;
		case 36: if ( retval == SUCCESS ) { rtlglue_printf("{P311}\n"); } else { rtlglue_printf("{F311}\n"); } break;
		case 37: if ( retval == SUCCESS ) { rtlglue_printf("{P312}\n"); } else { rtlglue_printf("{F312}\n"); } break;
		case 38: if ( retval == SUCCESS ) { rtlglue_printf("{P313}\n"); } else { rtlglue_printf("{F313}\n"); } break;
		case 39: if ( retval == SUCCESS ) { rtlglue_printf("{P314}\n"); } else { rtlglue_printf("{F314}\n"); } break;
		case 40: if ( retval == SUCCESS ) { rtlglue_printf("{P315}\n"); } else { rtlglue_printf("{F315}\n"); } break;
		case 41: if ( retval == SUCCESS ) { rtlglue_printf("{P316}\n"); } else { rtlglue_printf("{F316}\n"); } break;
		case 42: if ( retval == SUCCESS ) { rtlglue_printf("{P317}\n"); } else { rtlglue_printf("{F317}\n"); } break;
		case 43: if ( retval == SUCCESS ) { rtlglue_printf("{P401}\n"); } else { rtlglue_printf("{F401}\n"); } break;
		case 44: if ( retval == SUCCESS ) { rtlglue_printf("{P402}\n"); } else { rtlglue_printf("{F402}\n"); } break;
		case 45: if ( retval == SUCCESS ) { rtlglue_printf("{P403}\n"); } else { rtlglue_printf("{F403}\n"); } break;
		case 46: if ( retval == SUCCESS ) { rtlglue_printf("{P404}\n"); } else { rtlglue_printf("{F404}\n"); } break;	
		case 60: if ( retval == SUCCESS ) { rtlglue_printf("{P601}\n"); } else { rtlglue_printf("{F601}\n"); } break;		
		case 70: if ( retval == SUCCESS ) { rtlglue_printf("{P700}\n"); } else { rtlglue_printf("{F700}\n"); } break;
		case 71: if ( retval == SUCCESS ) { rtlglue_printf("{P701}\n"); } else { rtlglue_printf("{F701}\n"); } break;
		case 72: if ( retval == SUCCESS ) { rtlglue_printf("{P702}\n"); } else { rtlglue_printf("{F702}\n"); } break;
		case 73: if ( retval == SUCCESS ) { rtlglue_printf("{P703}\n"); } else { rtlglue_printf("{F703}\n"); } break;
		case 74: if ( retval == SUCCESS ) { rtlglue_printf("{P704}\n"); } else { rtlglue_printf("{F704}\n"); } break;
		case 75: if ( retval == SUCCESS ) { rtlglue_printf("{P705}\n"); } else { rtlglue_printf("{F705}\n"); } break;
		case 76: if ( retval == SUCCESS ) { rtlglue_printf("{P706}\n"); } else { rtlglue_printf("{F706}\n"); } break;
		case 77: if ( retval == SUCCESS ) { rtlglue_printf("{P707}\n"); } else { rtlglue_printf("{F707}\n"); } break;
		case 78: if ( retval == SUCCESS ) { rtlglue_printf("{P708}\n"); } else { rtlglue_printf("{F708}\n"); } break;
		case 79: if ( retval == SUCCESS ) { rtlglue_printf("{P709}\n"); } else { rtlglue_printf("{F709}\n"); } break;
		case 80: if ( retval == SUCCESS ) { rtlglue_printf("{P710}\n"); } else { rtlglue_printf("{F710}\n"); } break;
		case 81: if ( retval == SUCCESS ) { rtlglue_printf("{P711}\n"); } else { rtlglue_printf("{F711}\n"); } break;
		case 82: if ( retval == SUCCESS ) { rtlglue_printf("{P712}\n"); } else { rtlglue_printf("{F712}\n"); } break;
		case 83: if ( retval == SUCCESS ) { rtlglue_printf("{P713}\n"); } else { rtlglue_printf("{F713}\n"); } break;
		case 84: if ( retval == SUCCESS ) { rtlglue_printf("{P714}\n"); } else { rtlglue_printf("{F714}\n"); } break;
		case 85: if ( retval == SUCCESS ) { rtlglue_printf("{P715}\n"); } else { rtlglue_printf("{F715}\n"); } break;
		case 86: if ( retval == SUCCESS ) { rtlglue_printf("{P716}\n"); } else { rtlglue_printf("{F716}\n"); } break;
		case 87: if ( retval == SUCCESS ) { rtlglue_printf("{P080}\n"); } else { rtlglue_printf("{F080}\n"); } break;
		case 88: if ( retval == SUCCESS ) { rtlglue_printf("{P081}\n"); } else { rtlglue_printf("{F081}\n"); } break;		
		case 95: if ( retval == SUCCESS ) { rtlglue_printf("{P515}\n"); } else { rtlglue_printf("{F515}\n"); } break;
		case 96: if ( retval == SUCCESS ) { rtlglue_printf("{P516}\n"); } else { rtlglue_printf("{F516}\n"); } break;
		case 97: if ( retval == SUCCESS ) { rtlglue_printf("{P517}\n"); } else { rtlglue_printf("{F517}\n"); } break;
		case 98: if ( retval == SUCCESS ) { rtlglue_printf("{P518}\n"); } else { rtlglue_printf("{F518}\n"); } break;		
		default: break;
		}
	
		if ( retval == SUCCESS ){
			rtlglue_printf( " ok!\n\n" );
			okCase++;
		}
		else	{
			rtlglue_printf( "\033[31;43m fail!! \033[m\n\n" );
			failCaseIdx[failCase] = i;
			failCase++;
		}

		rtlglue_getmstime(&end_time);
		total_time += (end_time - begin_time);
	}

	rtlglue_printf( ">>Total Case: %d,  Pass: %d,  Fail: %d\n\n", totalCase, okCase, failCase );
	if ( failCase > 0 ) {		
		rtlglue_printf("  +-- Failed Cases -------------------------------------------\n" );
		for( i = 0; i < failCase; i++ ) {
			rtlglue_printf("  | case %-5d  %-60s |\n", ft2TestCase[failCaseIdx[i]].no, ft2TestCase[failCaseIdx[i]].name );
		}
		rtlglue_printf("  +--------------------------------------------------------------------------+\007\n\n");
	}	

	return SUCCESS;
}
#endif	/* end of CONFIG_RTL865X_MODEL_TEST_FT2 */

int32 runModelTest(uint32 userId,  int32 argc,int8 **saved)
{
	int8 *nextToken;
	int32 size;
	int32 lower, upper;
	int i;
	int32 retval;
	int32 totalCase = 0;
	int32 okCase = 0;
	int32 failCase = 0;
	int32 failCaseIdx[sizeof(modelTestCase)/sizeof(modelTestCase[0])];
	int32 caseNoSeq = 1; /* to generate auto-increased case number. */
	uint32 groupmask = 0xffffffff;
#ifndef RTL865X_MODEL_KERNEL
	int32 drvTest_init = 0;
#endif
	lower = 0;
#if 1
	upper = modelTestCase[sizeof(modelTestCase)/sizeof(modelTestCase[0])-1].no;
#else
	upper = sizeof(modelTestCase)/sizeof(modelTestCase[0]); /* MAX */
#endif
	
	cle_getNextCmdToken(&nextToken,&size,saved);
	if ( !strcmp( nextToken, "all" ) )
		groupmask = GRP_ALL;
	else if ( !strcmp( nextToken, "gdma" ) )
		groupmask = GRP_GDMA;
	else if ( !strcmp( nextToken, "swcore" ) )
		groupmask = GRP_PKT_PARSER|GRP_PKT_TRANSLATOR|GRP_PRE_CORE|GRP_L2|GRP_L34|GRP_NAPT|GRP_POST_CORE|GRP_NIC;
	else if ( !strcmp( nextToken, "parser" ) )
		groupmask = GRP_PKT_PARSER;
	else if ( !strcmp( nextToken, "translator" ) )
		groupmask = GRP_PKT_TRANSLATOR;
	else if ( !strcmp( nextToken, "pre_core" ) )
		groupmask = GRP_PRE_CORE;
	else if ( !strcmp( nextToken, "l2" ) )
		groupmask = GRP_L2;
	else if ( !strcmp( nextToken, "l34" ) )
		groupmask = GRP_L34;
	else if ( !strcmp( nextToken, "napt" ) )
		groupmask = GRP_NAPT;	
	else if ( !strcmp( nextToken, "post_core" ) )
		groupmask = GRP_POST_CORE;
	else if ( !strcmp( nextToken, "nic" ) )
		groupmask = GRP_NIC;

#ifndef RTL865X_MODEL_KERNEL	/* When compiling model test for FPGA, we don't compile driver test cases. */
	else if ( !strcmp( nextToken, "tbldrv") )
		groupmask = GROUP_TBLDRV;
#endif
	else
		lower = upper = U32_value(nextToken);

	if ( cle_getNextCmdToken(&nextToken,&size,saved) == SUCCESS )
		upper = U32_value(nextToken);

	/* generate auto-increased pCase->no */
	for( i = 0; i < sizeof(modelTestCase)/sizeof(modelTestCase[0]); i++ )
	{
		MODEL_TEST_CASE_T *pCase = &modelTestCase[i];
		
		if  ( pCase->no == AUTO_CASE_NO )
			pCase->no = (caseNoSeq++);
		else
			caseNoSeq = pCase->no+1; /* auto learn the next no */
	}

	if ( modelRandomSeedStratagem == ENUM_SEED_SYSTIME )
	{
		uint32 t; /* time_t */
		modelRandomSeed = rtlglue_time(&t);
	}
		
	rtlglue_printf( "Running from %d to %d with group mask 0x%08x, seed=0x%08x:\n\n", lower, upper, groupmask, modelRandomSeed );
	rtlglue_srandom( modelRandomSeed );

	for( i = 0; i < sizeof(modelTestCase)/sizeof(modelTestCase[0]); i++ )
	{
		int t;
		MODEL_TEST_CASE_T *pCase = &modelTestCase[i];

		if ( pCase->no < lower || pCase->no > upper ) continue;
		if ( ( pCase->group & groupmask ) == 0 ) continue;
		if ( pCase->fp == NULL ) continue;

		totalCase++;		
		rtlglue_printf( "Running Model Test Case %d: %s() ...\n", pCase->no, pCase->name );		

		/* Prepare virtualMac environment */
		for( t = IC_TYPE_MIN; t < IC_TYPE_MAX; t++ )
		{
			int ret;
			
			ret = model_setTestTarget( t );
			if ( ret == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			/* Switch Core FullReset */
			REG32(SSIR) |= FULL_RST;			
			mdelay(50);					
			ret = virtualMacInit();
			ASSERT( ret==SUCCESS );
		}
		
		modelIcInit();
		modelIcSetDefaultValue();

#ifndef RTL865X_MODEL_KERNEL	/* When compiling model test for FPGA, we don't compile driver test cases. */
		/* Driver test cases */
		if ( pCase->no > DRVTEST_CASENO_LOWERBOUND /* && pCase->no < DRVTEST_CASENO_UPPERBOUND */)
		{
			if ( drvTest_init == 0 )
			{
				rtl8651_tblDrvInitPara_t configPara;

				/* Set TBLDRV initial parameter */
				memset( &configPara, 0, sizeof(rtl8651_tblDrvInitPara_t) );
				configPara.dialSessionNumber = 8;	/* Set to 8 sessions */

				/* Init TBLDRV & ASIC */
				rtlglue_printf("Table Driver Initialization......\t");
				if ( rtl8651_tblDrvInit( &configPara ) == FAILED )
				{
					rtlglue_printf("failure!!!!\n");
					return 1;
				}
				else 
					rtlglue_printf("ok!\n");

				drvTest_init = 1;	
			}


			/* Re-init Table Driver */
			if ( rtl8651_tblDrvReinit() != SUCCESS )
			{
				rtlglue_printf("drvTest: rtl8651_tblDrvReinit() error!!\n");
				return 1;
			}

			/* Re-init Dirver Mutex */
			if (rtl8651_reinitDrvMutex() != SUCCESS) 
			{
				rtlglue_printf("drvTest: rtl8651_reinitDrvMutex() error!!\n");
				return 1;
			}
#if 0
			// mbuf
			nmbufs = 128;
			nmbclusters = 256;
			nmpkthdrs = 128;
			clusterSize = 2048;
			/* Initialize mbuf system */
			printf("Mbuf Initialization......\t\t");
			if (mBuf_init(nmbufs, nmbclusters, nmpkthdrs, clusterSize, 0) != SUCCESS){
				printf("failure!!!!\n");
				return 1;
			} else printf("ok!\n");

			/* Initialize forwarding engine */
			printf("Forward Engine Initialization......\t");
			if (rtl8651_fwdEngineInit(NULL) != SUCCESS){
				printf("failure!!!!\n");
				return 1;
			} else{
				mbufList_init();
				printf("ok!\n");
			}
#endif

		}
#endif	/* RTL865X_MODEL_KERNEL */
		
		retval = pCase->fp( pCase->no );

		{ /* pump all packet in queue */
			int ret;
			for( t = IC_TYPE_MIN; t < IC_TYPE_MAX; t++ )
			{
				ret = model_setTestTarget( t );
				if ( ret == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

				ret = virtualMacPumpPackets( PM_PORT_ALL );
				ASSERT( ret==SUCCESS );
			}
		}

		if ( retval == SUCCESS )
		{
			rtlglue_printf( " ok!\n\n" );
			okCase++;
		}
		else
		{
			rtlglue_printf( "\033[31;43m fail!! \033[m\n\n" );
			failCaseIdx[failCase] = i;
			failCase++;
		}
		modelIcExit();
	}
	
	rtlglue_printf( ">>Total Case: %d,  Pass: %d,  Fail: %d\n\n", totalCase, okCase, failCase );
	if ( failCase > 0 )
	{
		/* list fail cases */
		int i;
		
		rtlglue_printf("  +-- Failed Cases --------------------------------------- seed=0x%08x --+\n", modelRandomSeed );
		for( i = 0; i < failCase; i++ )
		{
			rtlglue_printf("  | case %-5d  %-60s |\n", modelTestCase[failCaseIdx[i]].no,
			                                           modelTestCase[failCaseIdx[i]].name );
		}
		rtlglue_printf("  +--------------------------------------------------------------------------+\007\n\n");
	}

	/* Finally, we had finished all test cases. We must recover the registers to real address. */
	model_setTestTarget( IC_TYPE_REAL );

	return SUCCESS;
}


int32 _rtl8651_setTarget(uint32 userId,  int32 argc,int8 **saved)
{
	int8 *nextToken;
	int32 size;
	int32 retval;
	
	cle_getNextCmdToken(&nextToken,&size,saved); 
	if (strcmp(nextToken, "ic") == 0) 
	{
		retval = model_setTestTarget(IC_TYPE_REAL);
	}
	else if (strcmp(nextToken, "model") == 0)
	{
		retval = model_setTestTarget(IC_TYPE_MODEL);		
	}
	else if (strcmp(nextToken, "mask") == 0)
	{
		uint32 mask;
		
		if ( cle_getNextCmdToken(&nextToken,&size,saved)==SUCCESS )
		{
			mask = U32_value(nextToken);
			retval = model_setTestTargetMask( mask );
		}
		else
		{
			retval = model_getTestTargetMask( &mask );
		}
		rtlglue_printf( "\nThe target Mask is 0x%08x\n\n", mask );
	}
	else
		retval = FAILED;
	
	return retval;
}


