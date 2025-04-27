/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Model code for pattern match
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: gdmaTest.c,v 1.43 2007-06-01 06:48:38 yjlou Exp $
*/

#include "rtl_types.h"
#include "asicregs.h"
//#ifdef CONFIG_RTL865XC
//#if 1
//#include "rtl865xC_tblAsicDrv.h"
//#else
//#include "rtl8651_tblAsicDrv.h"
//#endif
#include "icExport.h"
#include "gdma.h"
#include "kmp.h"
#include "gdmaModel.h"
#include "virtualMac.h"
#include "gdma_glue.h"
#include "gdma_utils.h"
//#include "icTest.h"
#include "gdma_test.h"
#include "drvTest.h"
//#include "modelRandom.h"

//#define MEMCTL_DEBUG_PRINTF(...)
#define MEMCTL_DEBUG_PRINTF printf

/* If defined, all the detailed test will be done.
 * If not defined, only partial test will be done, for example, destination length larger than 4 bytes.
 */
#define FULL_TEST


#if 0
	#define EXPECT_ACCEPT( s ) do {} while (0)
	#define EXPECT_NOT_ACCEPT( s ) do {} while (0)
#else
#define EXPECT_ACCEPT( state ) \
	do { \
		if ( ( state&STATE_ESCAPE ) != STATE_ESCAPE ) \
		{ \
			rtlglue_printf( "[%s():%d] Expect ACCEPT, but not accept (0x%06x)\n", __FUNCTION__, __LINE__, state ); \
			return FAILED; \
		} \
	} while (0)

#define EXPECT_NOT_ACCEPT( state ) \
	do { \
		if ( ( state&STATE_ESCAPE ) == STATE_ESCAPE ) \
		{ \
			rtlglue_printf( "[%s():%d] Expect NOT ACCEPT, but accepted (0x%06x)\n", __FUNCTION__, __LINE__, state ); \
			return FAILED; \
		} \
	} while (0)
#endif


#define HEAPSIZE (8*8*1024)
static uint8 _sb1[HEAPSIZE]; /* source heap (expected) */
static uint8 _db1[HEAPSIZE]; /* destination heap (expected) */
static uint8 _sb2[HEAPSIZE]; /* source heap (to be compared) */
static uint8 _db2[HEAPSIZE]; /* destination heap (to be compared) */
static uint8 *sb1, *db1, *sb2, *db2; /* Uncached address pointer for above heap */
/*	Enumerate data block buffers
 *	for model code and IC to compare results.
 */
uint64 enumbal_total_case_no = 0;
uint32 internal_gdmacnr = 0;
typedef struct enumbuf_s
{
	uint8 *src;
	uint8 *dst;
} enumbuf_t;
static enumbuf_t enumbuf[2];

#define OPT_IGNCLR	 0x00000001 /* ignore clean buffer */
#define OPT_IGNSRC   0x00000002 /* ignore comparing source block */
#define OPT_CACHED1  0x00000004 /* Cached data block 1, including sb1 and db1 */
#define OPT_CACHED2  0x00000008 /* Cached data block 2, including sb2 and db2 */
#define OPT_ALL      0xFFFFFFFF /* all optimize */
static uint32 gdmaOptimize = 0;
void testGdmaOptimize( uint32 flags, uint32 enable )
{
	if ( enable )
		gdmaOptimize = gdmaOptimize | flags;
	else
		gdmaOptimize = gdmaOptimize & ~flags;

        _cache_flush();

	if ( flags & OPT_CACHED1 )
	{
		if ( enable )
		{
			sb1 = (_sb1);
			db1 = (_db1);
			enumbuf[0].src = (_sb1);
			enumbuf[0].dst = (_db1);
		}
		else
		{
			sb1 = UNCACHED_ADDRESS(_sb1);
			db1 = UNCACHED_ADDRESS(_db1);
			enumbuf[0].src = UNCACHED_ADDRESS(_sb1);
			enumbuf[0].dst = UNCACHED_ADDRESS(_db1);
		}
	}
	if ( flags & OPT_CACHED2 )
	{
		if ( enable )
		{
			sb2 = (_sb2);
			db2 = (_db2);
			enumbuf[1].src = (_sb2);
			enumbuf[1].dst = (_db2);
		}
		else
		{
			sb2 = UNCACHED_ADDRESS(_sb2);
			db2 = UNCACHED_ADDRESS(_db2);
			enumbuf[1].src = UNCACHED_ADDRESS(_sb2);
			enumbuf[1].dst = UNCACHED_ADDRESS(_db2);
		}
	}
}


/*
 *  flags:
 */
#define CLEAN_SB1 0x00000001
#define CLEAN_SB2 0x00000002
#define CLEAN_DB1 0x00000004
#define CLEAN_DB2 0x00000008
#define CLEAN_ALL (CLEAN_SB1|CLEAN_SB2|CLEAN_DB1|CLEAN_DB2)
#define CLEAN_OVERALL 0x80000000 /* for gdmaInit() use */
void clearGdmaBuffer( uint32 flags )
{
	uint32 *su1, *su2, *du1, *du2;
	int i;

	if ( (flags&CLEAN_OVERALL)==0 )
	{
		if ( gdmaOptimize & OPT_IGNCLR )
		{
			/* Only one situation we will return to do nothing: optimization is enabled, and is not called by gdmaInit(). */
			return;
		}
	}

	su1 = (uint32*)&sb1[0];
	su2 = (uint32*)&sb2[0];
	du1 = (uint32*)&db1[0];
	du2 = (uint32*)&db2[0];
	if ( flags & CLEAN_SB1 )
	{
		for( i = 0; i < HEAPSIZE/4; i++ )
		{
			*su1 = 0;
			su1++;
		}
	}
	if ( flags & CLEAN_SB2 )
	{
		for( i = 0; i < HEAPSIZE/4; i++ )
		{
			*su2 = 0;
			su2++;
		}
	}
	if ( flags & CLEAN_DB1 )
	{
		for( i = 0; i < HEAPSIZE/4; i++ )
		{
			*du1 = 0;
			du1++;
		}
	}
	if ( flags & CLEAN_DB2 )
	{
		for( i = 0; i < HEAPSIZE/4; i++ )
		{
			*du2 = 0;
			du2++;
		}
	}
}


/* initial routine for test GDMA */
int32 testGdmaInit( void )
{
	testGdmaOptimize( OPT_ALL, FALSE ); /* turn off all optimize */
	testGdmaOptimize( OPT_IGNCLR|OPT_IGNSRC, TRUE ); /* then, enable IGNCLR and IGNSRC */
	enumbal_total_case_no = 0;

	clearGdmaBuffer( CLEAN_OVERALL|CLEAN_ALL );
	
	return SUCCESS;
}


int32 displayStateMachine( pmVsm_t* pVsm )
{
	char* pDot;
	int32 retval = SUCCESS;

	//retval = vsmGenDOT( pVsm, &pDot );
	//ASSERT( retval==SUCCESS );
	//ASSERT( pDot != NULL );

//#ifdef RTL865X_MODEL_USER
#if 0
	if ( getenv( "DISPLAY" ) )
	{
		/* DISPLAY is set, and we asuume that user have X window to show picture. */
		FILE *fp;
		char* cmd = "/usr/bin/dot -Tgif /dev/stdin -o /dev/stdout | /usr/X11R6/bin/xview /dev/stdin";
		
		fp = popen( cmd, "w" );
		if ( fp == NULL )
		{
			rtlglue_printf( "%s():%d cmd[%s] error:%s\n", __FUNCTION__, __LINE__, cmd, strerror(errno) );
			ASSERT ( fp!=NULL );
		}

		fprintf( fp, "%s", pDot );
		fclose( fp );
	}
	else
	{
		rtlglue_printf( "To show graphic of state machine, setenv DISPLAY in your environment variable.\n" );
		rtlglue_printf( "Generated DOT:\n%s\n", pDot );
	}
#else
	//rtlglue_printf( "Generated DOT:\n%*s\n", strlen(pDot), pDot );
#endif
	//if ( pDot )
	//	rtlglue_free( pDot );

	return retval;
}


/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  test bench                                                              **
 **                                                                          **
 ******************************************************************************
 ******************************************************************************/




/******************************************************************************
 **  case 1                                                                  **
 ******************************************************************************/
/* KMP: nano */
subStateMachine_t subSM1 =
{
	rules:
	{
		{ 
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 2,
			matchChar: 'n'
		},
		{ 
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 0,
			matchChar: 'n'
		},
		{ 
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 5,
			matchChar: 'a'
		},
		{
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 0,
			matchChar: 'n'
		},
		{ 
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 0,
			matchChar: 'a'
		},
		{ 
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 7,
			matchChar: 'n'
		},
		{ 
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 0,
			matchChar: 'n'
		},
		{ 
			type:0,
			not: 0,
			cpu: 1,
			jmpState: STATE_ACCEPT,
			matchChar: 'o'
		},
		{ 
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 5,
			matchChar: 'a'
		},
		{ 
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 2,
			matchChar: 'o'
		},
	},
};
stateMachine_t SM1 =
{
	subsm:
	{
		&subSM1,
		NULL
	}
};
uint8 testString1[] = "nanano";
descriptor_t descTestString1[] = 
{
	{
		pData: (void*)testString1,
		ldb: 1,
		length: sizeof(testString1)/sizeof(testString1[0])-1
	},
};

int32 testGdmaPatternMatch1(uint32 caseNo)
{
	int i;
	int32 retval;

	testGdmaInit();
	
	rtlglue_printf( "TEST 1 (KMP:nano,Expect:ACCEPT):\n" );
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
	rtlglue_printf("i = 0x%d\n", i);
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, ((uint32*)descTestString1)[0] );
		WRITE_MEM32( GDMASBL0, (descTestString1[0].ldb<<31)|(descTestString1[0].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM1 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR );
		//while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ) /* wait for complete. */
		{
			rtlglue_printf("GDMAISR = 0x%x\n", READ_MEM32( GDMAISR ));

		}
	
		IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
		EXPECT_ACCEPT( READ_MEM32( GDMAICVL ) );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), 6, __FUNCTION__, __LINE__ );
	}

	return SUCCESS;
}


/******************************************************************************
 **  case 2                                                                  **
 ******************************************************************************/
/* RE: {ab|bc} */
subStateMachine_t subSM2 =
{
	rules:
	{
		{/*0:A*/
			not: 0,
			cpu: 0,
			matchChar: 'a',
			jmpState: 3,
		},
		{/*1*/
			not: 0,
			cpu: 0,
			matchChar: 'b',
			jmpState: 6,
		},
		{/*2*/
			not: 1,
			cpu: 0,
			matchChar: 'b',
			jmpState: 0,
		},
		{/*3:B*/
			not: 0,
			cpu: 0,
			matchChar: 'a',
			jmpState: 3,
		},
		{/*4*/
			not: 0,
			cpu: 1,
			matchChar: 'b',
			jmpState: STATE_ACCEPT,
		},
		{/*5*/
			not: 1,
			cpu: 0,
			matchChar: 'b',
			jmpState: 0,
		},
		{/*6:C*/
			not: 0,
			cpu: 0,
			matchChar: 'a',
			jmpState: 3,
		},
		{/*7*/
			not: 0,
			cpu: 0,
			matchChar: 'b',
			jmpState: 6,
		},
		{/*8*/
			not: 0,
			cpu: 1,
			matchChar: 'c',
			jmpState: STATE_ACCEPT,
		},
		{/*9*/
			not: 1,
			cpu: 0,
			matchChar: 'c',
			jmpState: 0,
		},
	},
};
stateMachine_t SM2 =
{
	subsm:
	{
		&subSM2,
		NULL
	},
};
uint8 testString2[] = "aacacccb";
descriptor_t descTestString2[] = 
{
	{
		pData: (void*)testString2,
		ldb: 1,
		length: sizeof(testString2)/sizeof(testString2[0])-1
	},
};
uint8 testString21[] = "aacabbccc";
descriptor_t descTestString21[] = 
{
	{
		pData: (void*)testString21,
		ldb: 1,
		length: sizeof(testString21)/sizeof(testString21[0])-1
	},
};
uint8 testString22[] = "aacbcaacc";
descriptor_t descTestString22[] = 
{
	{
		pData: (void*)testString22,
		ldb: 1,
		length: sizeof(testString22)/sizeof(testString22[0])-1
	},
};
uint8 testString23[] = "acbaaccaccbaaccaxyyyyyyyyyy";
descriptor_t descTestString23[] = 
{
	{
		pData: (void*)testString23,
		ldb: 1,
		length: sizeof(testString23)/sizeof(testString23[0])-1
	},
};
uint8 testString241[] = "aaaccc";
uint8 testString242[] = "acb";
uint8 testString243[] = "ccc";
descriptor_t descTestString24[] = 
{
	{
		pData: (void*)testString241,
		ldb: 0,
		length: sizeof(testString241)/sizeof(testString241[0])-1
	},
	{
		pData: (void*)testString242,
		ldb: 0,
		length: sizeof(testString242)/sizeof(testString242[0])-1
	},
	{
		pData: (void*)testString243,
		ldb: 1,
		length: sizeof(testString243)/sizeof(testString243[0])-1
	},
};

int32 testGdmaPatternMatch2(uint32 caseNo)
{
	int i;
	int32 retval;
	
	testGdmaInit();
	
	rtlglue_printf( "TEST 2 (RE:{ab|bc},Expect:NOT ACCEPT):\n" );
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, (uint32)(descTestString2[0].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString2[0].ldb<<31)|(descTestString2[0].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM2 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, 0, __FUNCTION__, __LINE__ );
		EXPECT_NOT_ACCEPT( READ_MEM32( GDMAICVL ) );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), descTestString2[0].length, __FUNCTION__, __LINE__ );
	}

	rtlglue_printf( "TEST 2-1 (RE:{ab|bc},Expect:ACCEPT):\n" );
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, (uint32)(descTestString21[0].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString21[0].ldb<<31)|(descTestString21[0].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM2 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
		EXPECT_ACCEPT( READ_MEM32( GDMAICVL ) );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), 5, __FUNCTION__, __LINE__ );
	}

	rtlglue_printf( "TEST 2-2 (RE:{ab|bc},Expect:ACCEPT):\n" );
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, (uint32)(descTestString22[0].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString22[0].ldb<<31)|(descTestString22[0].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM2 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
		EXPECT_ACCEPT( READ_MEM32( GDMAICVL ) );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), 5, __FUNCTION__, __LINE__ );
	}

	rtlglue_printf( "TEST 2-3 (RE:{ab|bc},Expect:NOT ACCEPT):\n" );
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, (uint32)(descTestString23[0].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString23[0].ldb<<31)|(descTestString23[0].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM2 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, 0, __FUNCTION__, __LINE__ );
		EXPECT_NOT_ACCEPT( READ_MEM32( GDMAICVL ) );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), descTestString23[0].length, __FUNCTION__, __LINE__ );
	}
	
	rtlglue_printf( "TEST 2-4 (RE:{ab|bc},Expect:ACCEPT):\n" );
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, (uint32)(descTestString24[0].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString24[0].ldb<<31)|(descTestString24[0].length) );
		WRITE_MEM32( GDMASBP1, (uint32)(descTestString24[1].pData) );
		WRITE_MEM32( GDMASBL1, (descTestString24[1].ldb<<31)|(descTestString24[1].length) );
		WRITE_MEM32( GDMASBP2, (uint32)(descTestString24[2].pData) );
		WRITE_MEM32( GDMASBL2, (descTestString24[2].ldb<<31)|(descTestString24[2].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM2 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
		EXPECT_ACCEPT( READ_MEM32( GDMAICVL ) );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), 10, __FUNCTION__, __LINE__ );
	}
	
	return SUCCESS;
}

	
/******************************************************************************
 **  case 3                                                                  **
 ******************************************************************************/
/* RE: ^abcde */
subStateMachine_t subSM3 =
{
	rules:
	{
		{/*0:A*/
			not: 0,
			cpu: 0,
			matchChar: 'a',
			jmpState: 2,
		},
		{/*1:!A*/
			not: 1,
			cpu: 1,
			matchChar: 'a',
			jmpState: STATE_DEAD,
		},
		{/*2:B*/
			not: 0,
			cpu: 0,
			matchChar: 'b',
			jmpState: 4,
		},
		{/*3:!B*/
			not: 1,
			cpu: 1,
			matchChar: 'b',
			jmpState: STATE_DEAD,
		},
		{/*4:C*/
			not: 0,
			cpu: 0,
			matchChar: 'c',
			jmpState: 6,
		},
		{/*5:!C*/
			not: 1,
			cpu: 1,
			matchChar: 'c',
			jmpState: STATE_DEAD,
		},
		{/*6:D*/
			not: 0,
			cpu: 0,
			matchChar: 'd',
			jmpState: 8,
		},
		{/*7:!D*/
			not: 1,
			cpu: 1,
			matchChar: 'd',
			jmpState: STATE_DEAD,
		},
		{/*8:E*/
			not: 0,
			cpu: 1,
			matchChar: 'e',
			jmpState: STATE_ACCEPT,
		},
		{/*9:!E*/
			not: 1,
			cpu: 1,
			matchChar: 'e',
			jmpState: STATE_DEAD,
		},
	},
};
stateMachine_t SM3 =
{
	subsm:
	{
		&subSM3,
		NULL
	}
};
uint8 testString31[] = "ab";
descriptor_t descTestString31[] = 
{
	{
		pData: (void*)testString31,
		ldb: 1,
		length: sizeof(testString31)/sizeof(testString31[0])-1
	},
};
uint8 testString32[] = "c";
descriptor_t descTestString32[] = 
{
	{
		pData: (void*)testString32,
		ldb: 1,
		length: sizeof(testString32)/sizeof(testString32[0])-1
	},
};
uint8 testString33[] = "de";
descriptor_t descTestString33[] = 
{
	{
		pData: (void*)testString33,
		ldb: 1,
		length: sizeof(testString33)/sizeof(testString33[0])-1
	},
};

int32 testGdmaPatternMatch3(uint32 caseNo)
{
	int i;
	int32 retval;
	
	testGdmaInit();
	
	rtlglue_printf( "TEST 3 (RE:^abcde,fragment packet,Expect 1st:NOT ACCEPT):\n" );
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, (uint32)(descTestString31[0].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString31[0].ldb<<31)|(descTestString31[0].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM3 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		
		IS_EQUAL_INT("NeedCPU is set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, 0, __FUNCTION__, __LINE__ );
		EXPECT_NOT_ACCEPT( READ_MEM32( GDMAICVL ) );
		IS_EQUAL_INT("Final State is not matched: {expected:real}=", 4, READ_MEM32( GDMAICVL ), __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), descTestString31[0].length, __FUNCTION__, __LINE__ );
	}
	
	rtlglue_printf( "                                  Expect 2nd:NOT ACCEPT):\n" );
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 4 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, (uint32)(descTestString32[0].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString32[0].ldb<<31)|(descTestString32[0].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM3 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		
		IS_EQUAL_INT("NeedCPU is set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, 0, __FUNCTION__, __LINE__ );
		EXPECT_NOT_ACCEPT( READ_MEM32( GDMAICVL ) );
		IS_EQUAL_INT("Final State is not matched: {expected:real}=", 6, READ_MEM32( GDMAICVL ), __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), descTestString32[0].length, __FUNCTION__, __LINE__ );
	}
	
	rtlglue_printf( "                                  Expect 3rd:ACCEPT):\n" );
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 6 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, (uint32)(descTestString33[0].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString33[0].ldb<<31)|(descTestString33[0].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM3 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

		IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
		EXPECT_ACCEPT( READ_MEM32( GDMAICVL ) );
		IS_EQUAL_INT("Final State is not matched: {expected:real}=", STATE_ACCEPT, READ_MEM32( GDMAICVL ), __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), 2, __FUNCTION__, __LINE__ );
	}
	
	return SUCCESS;
}
	

/******************************************************************************
 **  case 4                                                                  **
 ******************************************************************************/
/* RE: ^ab */
subStateMachine_t subSM40 =
{
	rules:
	{
		{/*0x0000:A*/
			not: 0,
			cpu: 0,
			matchChar: 'a',
			jmpState: 0x0100,
		},
		{/*0x0001:!A*/
			not: 1,
			cpu: 1,
			matchChar: 'a',
			jmpState: STATE_DEAD,
		},
	},
};
subStateMachine_t subSM41 =
{
	rules:
	{
		{/*0x0100:B*/
			not: 0,
			cpu: 1,
			matchChar: 'b',
			jmpState: STATE_ACCEPT,
		},
		{/*0x0101:!B*/
			not: 1,
			cpu: 1,
			matchChar: 'b',
			jmpState: STATE_DEAD,
		},
	},
};
stateMachine_t SM4 =
{
	subsm:
	{
		&subSM40,
		&subSM41,
		NULL
	},
};
uint8 testString4[] = "ab";
descriptor_t descTestString4[] = 
{
	{
		pData: (void*)testString4,
		ldb: 1,
		length: sizeof(testString4)/sizeof(testString4[0])-1
	},
};

int32 testGdmaPatternMatch4(uint32 caseNo)
{
	int i;
	int32 retval;

	testGdmaInit();
	
	rtlglue_printf( "TEST 4 (RE:^ab,Inter-sub-SM,Expect:ACCEPT):\n" );
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, (uint32)(descTestString4[0].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString4[0].ldb<<31)|(descTestString4[0].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM4 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
		EXPECT_ACCEPT( READ_MEM32( GDMAICVL ) );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), 2, __FUNCTION__, __LINE__ );
	}

	return SUCCESS;
}


/******************************************************************************
 **  case 5                                                                  **
 ******************************************************************************/
/* KMP: nanano */
uint8 testString51[] = "aaaoonoananananoaa";
descriptor_t descTestString51[] = 
{
	{
		pData: (void*)testString51,
		ldb: 1,
		length: sizeof(testString51)/sizeof(testString51[0])-1
	},
};
uint8 testString52[] = "nanananananaonananaoonanaao";
descriptor_t descTestString52[] = 
{
	{
		pData: (void*)testString52,
		ldb: 1,
		length: sizeof(testString52)/sizeof(testString52[0])-1
	},
};

int32 testGdmaPatternMatch5(uint32 caseNo)
{
	int i;
	int32 retval;
	pmVsm_t vsm;

	testGdmaInit();
	vsmInit( &vsm );
	vsmGenFromKMP( &vsm, (uint8*)"nanano", 6 );

	rtlglue_printf( "TEST 5-1 (KMP:nanano,Expect:ACCEPT):\n" );
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, (uint32)(descTestString51[0].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString51[0].ldb<<31)|(descTestString51[0].length) );
		WRITE_MEM32( GDMADBP0, (uint32)vsm.prefix[0].pStateMachine );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

		IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
		EXPECT_ACCEPT( READ_MEM32( GDMAICVL ) );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), 16, __FUNCTION__, __LINE__ );
	}
	
	rtlglue_printf( "TEST 5-2 (KMP:nanano,Expect:NOT ACCEPT):\n" );
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, (uint32)(descTestString52[0].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString52[0].ldb<<31)|(descTestString52[0].length) );
		WRITE_MEM32( GDMADBP0, (uint32)vsm.prefix[0].pStateMachine );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

		IS_EQUAL_INT("NeedCPU is set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, 0, __FUNCTION__, __LINE__ );
		EXPECT_NOT_ACCEPT( READ_MEM32( GDMAICVL ) );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), descTestString52[0].length, __FUNCTION__, __LINE__ );
	}
	
	vsmFree( &vsm );
	return SUCCESS;
}
	

/******************************************************************************
 **  case 6                                                                  **
 ******************************************************************************/
/* KMP: porn-porn-.... */
#define TEST_STRING_LENGTH 2005
descriptor_t descTestString6[1];

int32 testGdmaPatternMatch6(uint32 caseNo)
{
	int i, j, k;
	int accept, notaccept;
	int32 retval;
	pmVsm_t vsm;
	uint8 *pat = "nanonino"; /*"porn-porn-porn-porn-porn-porn-porn-porn-porn-porn-porn-porn.info/free-hardcore-interracial-porn-gallery-hardcore-interracial-porn/free-hardcore-interracial-porn-gallery.php";*/
	uint8 *found;

	testGdmaInit();
	descTestString6[0].pData = sb1;
	descTestString6[0].ldb = 1;
	descTestString6[0].length = TEST_STRING_LENGTH;
	
	rtlglue_printf( "TEST 6 (KMP:pron-pron-...,Expect:as same as strstr()):\n" );

	vsmInit( &vsm );
	retval = vsmGenFromKMP( &vsm, pat, strlen(pat) );
	ASSERT( retval==SUCCESS );

	displayStateMachine( &vsm );
	
	accept = notaccept = 0;
	for( i = 0; i < 100; i++ )
	{
		for( j = 0; j < TEST_STRING_LENGTH-1; j++ )
			sb1[j] = rtlglue_random()%26 + 'a';
		sb1[j] = '\0';
		if ( rtlglue_random()&0x00010000 ) /* We use bit 16, instead of random()%2 */
		{
			k = rtlglue_random()%(TEST_STRING_LENGTH-1-strlen(pat));
			memcpy( &sb1[k], pat, strlen(pat) );
		}
		
		for( k = IC_TYPE_MIN; k < IC_TYPE_MAX; k++ )
		{
			retval = model_setTestTarget( k );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			WRITE_MEM32( GDMACNR, 0 );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			WRITE_MEM32( GDMAIMR, 0 );
			WRITE_MEM32( GDMAISR, 0xffffffff );
			WRITE_MEM32( GDMAICVL, 0 );
			WRITE_MEM32( GDMAICVR, 0 );
			WRITE_MEM32( GDMASBP0, (uint32)(descTestString6[0].pData) );
			WRITE_MEM32( GDMASBL0, (descTestString6[0].ldb<<31)|(descTestString6[0].length) );
			WRITE_MEM32( GDMADBP0, (uint32)vsm.prefix[0].pStateMachine );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
			while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

			if ( ( found = strstr( sb1, pat ) ) )
			{
				accept++;
				IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
				EXPECT_ACCEPT( READ_MEM32( GDMAICVL ) );
				IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), (found-sb1)+strlen(pat), __FUNCTION__, __LINE__ );
			}
			else
			{
				notaccept++;
				IS_EQUAL_INT("NeedCPU is set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, 0, __FUNCTION__, __LINE__ );
				EXPECT_NOT_ACCEPT( READ_MEM32( GDMAICVL ) );
				IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), descTestString6[0].length, __FUNCTION__, __LINE__ );
			}
		}
	}
	/*modelGDMA_dump();*/
	vsmFree( &vsm );

	rtlglue_printf("Accepted:%d  Not Accepted:%d\n", accept, notaccept );

	return SUCCESS;
}


#if 0
/******************************************************************************
 **  read Clam AV database file 'virus.txt'                                  **
 ******************************************************************************/
int32 testGdmaPatternMatch7(uint32 caseNo)
{
	static uint8 buf[4096];
	static pmVsm_t vsm[30000]; 
	uint32 cntVsm = 0;
	char* filename = "virus.txt";
	FILE* fp;
	int32 i;
	int32 maxPatternLen, maxCntInst;
	double maxRatio;

	maxPatternLen = maxCntInst = maxRatio = 0;
	
	/* generate state machine from pattern */
	fp = fopen( filename, "r" );
	if ( fp==NULL )
	{
		char buf[256];
		snprintf( buf, sizeof(buf)-1, "open '%s' file error.", filename );
		perror( buf );
		return FAILED;
	}
	else
	{
		while( fgets( buf, sizeof(buf)-1, fp ) )
		{
			uint8 str[512];
			uint32 cntInst;
			int32 lenPattern;
			
			vsmInit( &vsm[cntVsm] );
			for( i = 0, lenPattern = 0; buf[i]&&buf[i+1]; i+=2, lenPattern++ )
			{
				char ch[3];

				ch[0] = buf[i];
				ch[1] = buf[i+1];
				ch[2] = 0;

				str[lenPattern] = strtol( ch, NULL, 16 );
			}
			if ( vsmGenFromKMP( &vsm[cntVsm], str, lenPattern )==FAILED )
			{
				rtlglue_printf( "vsmGenFromKMP(%d) error.\n", cntVsm );
				return FAILED;
			}

			cntInst = vsm[cntVsm].prefix[0].pStateMachine->cntTotalRules;
			rtlglue_printf("[%6d] patternLen=%d cntInst=%d  ratio=%.2f\n", cntVsm, lenPattern, cntInst, (cntInst*1.0)/lenPattern );

			if ( maxPatternLen < lenPattern ) maxPatternLen = lenPattern;
			if ( maxCntInst < cntInst ) maxCntInst = cntInst;
			if ( maxRatio < (cntInst*1.0)/lenPattern ) maxRatio = (cntInst*1.0)/lenPattern;
			
			cntVsm++;
		}
		fclose( fp );
	}

	rtlglue_printf("MaxPatternLen=%d MaxCntInst=%d MaxRatio=%.2f\n", maxPatternLen, maxCntInst, maxRatio );
	
#if 0
	{
		static uint8 packet[1500] = "123456";
		descriptor_t descPacket[] = 
		{
			{
				pData: (void*)packet,
				end: 1,
				length: sizeof(packet)/sizeof(packet[0])
			},
		};
		/* parse packet .... */
		for( i = 0; i < cntSubSM; i++ )
		{
			WRITE_MEM32( GDMACR, 1<<28 );
			WRITE_MEM32( GDMASDR, (uint32)descPacket );
			WRITE_MEM32( GDMADDR, (uint32)vsm.prefix[0].pStateMachine );
			if ( modelGDMA() != SUCCESS ) rtlglue_printf( "\n*** State Machine ERROR ***\n" );
		}
#endif

	/* free VSM */
	{
		int i;
		for( i = 0; i < cntVsm; i++ )
		{
			vsmFree( &vsm[i] );
		}
	}

	return SUCCESS;
}


/******************************************************************************
 **  read URL black list with 'url.txt '                                    **
 ******************************************************************************/
int32 testGdmaPatternMatch8(uint32 caseNo)
{
	static uint8 buf[4096];
	static pmVsm_t vsm[30000];
	char *filename = "urls.txt";
	FILE* fp;
	int32 maxPatternLen, maxCntInst;
	double maxRatio;
	uint32 cntVsm = 0;

	maxPatternLen = maxCntInst = maxRatio = 0;
	
	/* generate state machine from pattern */
	fp = fopen( filename, "r" );
	if ( fp==NULL )
	{
		char buf[256];
		snprintf( buf, sizeof(buf)-1, "open '%s' file error.", filename );
		perror( buf );
		return FAILED;
	}
	else
	{
		while( fgets( buf, sizeof(buf)-1, fp ) && cntVsm<sizeof(vsm)/sizeof(vsm[0]) )
		{
			uint32 cntInst;
			int32 lenPattern;

			vsmInit( &vsm[cntVsm] );
			lenPattern = strlen( buf );

			if ( vsmGenFromKMP( &vsm[cntVsm], buf, lenPattern )==FAILED )
			{
				rtlglue_printf( "vsmGenFromKMP(%d) error.\n", cntVsm );
				return FAILED;
			}

			cntInst = vsm[cntVsm].prefix[0].pStateMachine->cntTotalRules;
			rtlglue_printf("[%6d] patternLen=%d cntInst=%d  ratio=%.2f\n", cntVsm, lenPattern, cntInst, (cntInst*1.0)/lenPattern );

			if ( maxPatternLen < lenPattern ) maxPatternLen = lenPattern;
			if ( maxCntInst < cntInst ) maxCntInst = cntInst;
			if ( maxRatio < (cntInst*1.0)/lenPattern ) maxRatio = (cntInst*1.0)/lenPattern;
			
			cntVsm++;
		}
		fclose( fp );
	}

	rtlglue_printf("MaxPatternLen=%d MaxCntInst=%d MaxRatio=%.2f\n", maxPatternLen, maxCntInst, maxRatio );

#if 0
	{
		static uint8 packet[1500] = "123456";
		descriptor_t descPacket[] = 
		{
			{
				pData: (void*)packet,
				end: 1,
				length: sizeof(packet)/sizeof(packet[0])
			},
		};
		int32 i;
		/* parse packet .... */
		for( i = 0; i < cntSubSM; i++ )
		{
			WRITE_MEM32( GDMACR, 1<<28 );
			WRITE_MEM32( GDMASDR, (uint32)descPacket );
			WRITE_MEM32( GDMADDR, (uint32)vsm.prefix[0].pStateMachine );
			if ( modelGDMA() != SUCCESS ) rtlglue_printf( "\n*** State Machine ERROR ***\n" );
		}
	}
#endif

	/* free VSM */
	{
		int i;
		for( i = 0; i < cntVsm; i++ )
		{
			vsmFree( &vsm[i] );
		}
	}

	return SUCCESS;
}


/******************************************************************************
 **  Test case to evaluate the performance of KMP algorithm                  **
 **  The test case is diverted from 'drv romeperf bmp'                       **
 ******************************************************************************/
uint8 testString9[1501];
descriptor_t descTestString9[] = 
{
	{
		pData: (void*)testString9,
		end: 1,
		length: sizeof(testString9)/sizeof(testString9[0])-1
	},
};

int32 testGdmaPatternMatch9(uint32 caseNo)
{
	int i, j;
	int32 retval;
	pmVsm_t vsm;
	uint8 pattern[] = "free-quit-kit.how-quit-smoking.com/picture-of-someone-smoking-a-cigarette.how-quit-smoking.com";
	int patLen;
	int loop = 1000000;
	uint32 seed = 0x5566;
	char randChar[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz:/?&-";

	rtlglue_printf( "TEST 9 (To evaluate the performance of KMP algorithm):\n" );
	rtlglue_printf( "  Test Loop=%d\n", loop );
	rtlglue_printf( "  Random Seed=%d\n", seed );
	rtlglue_printf( "  pattern='%s'\n", pattern );

	
	for( patLen = 1; patLen < strlen(pattern); patLen+=8 )
	{
		vsmInit( &vsm );
		retval = vsmGenFromKMP( &vsm, pattern, patLen );
		ASSERT( retval==SUCCESS );
		displayStateMachine( &vsm );
		
		for( loop = 1000000; loop > 0; loop-- )
		{
			char subChar[sizeof(pattern)];

			sched_yield();
			
			for( j = 0; j < sizeof(testString9)-1; j++ )
			{
				testString9[j] = randChar[seed%(sizeof(randChar)-1)];
				seed = seed * 0x13579bdf + 0xfdb97531;
			}
			testString9[j] = '\0';
			//rtlglue_printf( "  packet='%s'\n", testString9 );
			
			WRITE_MEM32( GDMACR, 1<<28 );
			WRITE_MEM32( GDMAIMR, 0 );
			WRITE_MEM32( GDMAISR, 0xffffffff );
			WRITE_MEM32( GDMAICVL, 0 );
			WRITE_MEM32( GDMAICVR, 0 );
			WRITE_MEM32( GDMASDR, (uint32)descTestString9 );
			WRITE_MEM32( GDMADDR, (uint32)vsm.prefix[0].pStateMachine );
			if ( modelGDMA() != SUCCESS ) rtlglue_printf( "\n*** State Machine ERROR ***\n" );

			/* sub-string of pattern */
			strncpy( subChar, pattern, patLen );
			subChar[patLen] = '\0';
			if ( strstr( testString9, subChar ) )
				EXPECT_ACCEPT( READ_MEM32( GDMAICVL ) );
			else
				EXPECT_NOT_ACCEPT( READ_MEM32( GDMAICVL ) );
		}

		rtlglue_printf( "patLen=%d\n", patLen );
		modelGDMA_dump();
		vsmFree( &vsm );
	}

	return SUCCESS;
}

#endif


/******************************************************************************
 **  case 10: test cache mechanism, 5 pages are continuious                  **
 ******************************************************************************/
int32 testGdmaPatternMatch10(uint32 caseNo)
{
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	uint32 strflag;
	int32 retval;
	pmRule_t0 *rule;
	uint32 needCpuMask;
	int32 offset;
	stateMachine_t *SM10;
	typedef struct result_s
	{
		uint32 isr;
		uint32 icvl;
		uint32 icvr;
	} result_t;
	result_t result[2];
	
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	
	rtlglue_printf( "TEST 10 (test cache mechanism):\n" );

	rule = (pmRule_t0*)db2; /* use uncached address */

//	_cache_flush();
	SM10 = UNCACHED_ADDRESS( &rule[2000] ); /* We place SM10 in the middle of subsm */
	SM10->subsm[0] = (subStateMachine_t*)(&rule[0]);
	SM10->subsm[1] = (subStateMachine_t*)(&rule[256]);
	SM10->subsm[2] = (subStateMachine_t*)(&rule[512]);
	SM10->subsm[3] = (subStateMachine_t*)(&rule[768]);
	SM10->subsm[4] = (subStateMachine_t*)(&rule[1024]);
	SM10->subsm[5] = (subStateMachine_t*)(&rule[1280]);
	SM10->subsm[6] = (subStateMachine_t*)(&rule[1536]);
	SM10->subsm[7] = NULL;

	for( offset = -1; offset <= 0; offset++ )
	{
		/* Rule 0 */
		rule[0].not = 0;
		rule[0].matchChar = 'a';
		rule[0].type = GDMA_RULETYPE_0;
		rule[0].jmpState = 256+offset;
		/* Rule 1 */
		rule[1].not = 0;
		rule[1].matchChar = 'b';
		rule[1].type = GDMA_RULETYPE_0;
		rule[1].jmpState = 512+offset;
		/* Rule 2 */
		rule[2].not = 1;
		rule[2].matchChar = 'a';
		rule[2].type = GDMA_RULETYPE_0;
		rule[2].jmpState = 1900;
		rule[2].cpu = 1;
		
		/* Rule 255 */
		rule[256+offset+0].not = 0;
		rule[256+offset+0].matchChar = 'a';
		rule[256+offset+0].type = GDMA_RULETYPE_0;
		rule[256+offset+0].jmpState = 768+offset;
		/* Rule 256 */
		rule[256+offset+1].not = 0;
		rule[256+offset+1].matchChar = 'b';
		rule[256+offset+1].type = GDMA_RULETYPE_0;
		rule[256+offset+1].jmpState = 1024+offset;
		/* Rule 257 */
		rule[256+offset+2].not = 1;
		rule[256+offset+2].matchChar = 'b';
		rule[256+offset+2].type = GDMA_RULETYPE_0;
		rule[256+offset+2].jmpState = 1901;
		rule[256+offset+2].cpu = 1;
		
		/* Rule 511 */
		rule[512+offset+0].not = 0;
		rule[512+offset+0].matchChar = 'a';
		rule[512+offset+0].type = GDMA_RULETYPE_0;
		rule[512+offset+0].jmpState = 1280+offset;
		/* Rule 512 */
		rule[512+offset+1].not = 0;
		rule[512+offset+1].matchChar = 'b';
		rule[512+offset+1].type = GDMA_RULETYPE_0;
		rule[512+offset+1].jmpState = 1536+offset;
		/* Rule 513 */
		rule[512+offset+2].not = 1;
		rule[512+offset+2].matchChar = 'b';
		rule[512+offset+2].type = GDMA_RULETYPE_0;
		rule[512+offset+2].jmpState = 1902;
		rule[512+offset+2].cpu = 1;
		
		/* Rule 767 */
		rule[768+offset+0].not = 0;
		rule[768+offset+0].matchChar = 'a';
		rule[768+offset+0].type = GDMA_RULETYPE_0;
		rule[768+offset+0].jmpState = 1800;
		rule[768+offset+0].cpu = 1;
		/* Rule 768 */
		rule[768+offset+1].not = 0;
		rule[768+offset+1].matchChar = 'b';
		rule[768+offset+1].type = GDMA_RULETYPE_0;
		rule[768+offset+1].jmpState = 1801;
		rule[768+offset+1].cpu = 1;
		/* Rule 769 */
		rule[768+offset+2].not = 1;
		rule[768+offset+2].matchChar = 'b';
		rule[768+offset+2].type = GDMA_RULETYPE_0;
		rule[768+offset+2].jmpState = 1903;
		rule[768+offset+2].cpu = 1;
		
		/* Rule 1023 */
		rule[1024+offset+0].not = 0;
		rule[1024+offset+0].matchChar = 'a';
		rule[1024+offset+0].type = GDMA_RULETYPE_0;
		rule[1024+offset+0].jmpState = 1802;
		rule[1024+offset+0].cpu = 1;
		/* Rule 1024 */
		rule[1024+offset+1].not = 0;
		rule[1024+offset+1].matchChar = 'b';
		rule[1024+offset+1].type = GDMA_RULETYPE_0;
		rule[1024+offset+1].jmpState = 1803;
		rule[1024+offset+1].cpu = 1;
		/* Rule 1025 */
		rule[1024+offset+2].not = 1;
		rule[1024+offset+2].matchChar = 'b';
		rule[1024+offset+2].type = GDMA_RULETYPE_0;
		rule[1024+offset+2].jmpState = 1904;
		rule[1024+offset+2].cpu = 1;
		
		/* Rule 1279 */
		rule[1280+offset+0].not = 0;
		rule[1280+offset+0].matchChar = 'a';
		rule[1280+offset+0].type = GDMA_RULETYPE_0;
		rule[1280+offset+0].jmpState = 1804;
		rule[1280+offset+0].cpu = 1;
		/* Rule 1280 */
		rule[1280+offset+1].not = 0;
		rule[1280+offset+1].matchChar = 'b';
		rule[1280+offset+1].type = GDMA_RULETYPE_0;
		rule[1280+offset+1].jmpState = 1805;
		rule[1280+offset+1].cpu = 1;
		/* Rule 1281 */
		rule[1280+offset+2].not = 1;
		rule[1280+offset+2].matchChar = 'b';
		rule[1280+offset+2].type = GDMA_RULETYPE_0;
		rule[1280+offset+2].jmpState = 1905;
		rule[1280+offset+2].cpu = 1;
		
		/* Rule 1535 */
		rule[1536+offset+0].not = 0;
		rule[1536+offset+0].matchChar = 'a';
		rule[1536+offset+0].type = GDMA_RULETYPE_0;
		rule[1536+offset+0].jmpState = 1806;
		rule[1536+offset+0].cpu = 1;
		/* Rule 1536 */
		rule[1536+offset+1].not = 0;
		rule[1536+offset+1].matchChar = 'b';
		rule[1536+offset+1].type = GDMA_RULETYPE_0;
		rule[1536+offset+1].jmpState = 1807;
		rule[1536+offset+1].cpu = 1;
		/* Rule 1537 */
		rule[1536+offset+2].not = 1;
		rule[1536+offset+2].matchChar = 'b';
		rule[1536+offset+2].type = GDMA_RULETYPE_0;
		rule[1536+offset+2].jmpState = 1906;
		rule[1536+offset+2].cpu = 1;
		
		for( needCpuMask = 0; needCpuMask < (1<<6); needCpuMask++ )
		{
			/* toggle nedd CPU bit */
			rule[0].cpu = (needCpuMask&(1<<0)?TRUE:FALSE);
			rule[1].cpu = (needCpuMask&(1<<1)?TRUE:FALSE);
			rule[256+offset+0].cpu = (needCpuMask&(1<<2)?TRUE:FALSE);
			rule[256+offset+1].cpu = (needCpuMask&(1<<3)?TRUE:FALSE);
			rule[512+offset+0].cpu = (needCpuMask&(1<<4)?TRUE:FALSE);
			rule[512+offset+1].cpu = (needCpuMask&(1<<5)?TRUE:FALSE);

			#define STRING_LEN (3)
			for( strflag = 0; strflag < (1<<(STRING_LEN*2)); strflag++ )
			{
				for( j = 0; j < STRING_LEN; j++ )
				{
					switch ( (strflag>>(j*2)) & 3 )
					{
						case 0: enumbuf[0].src[j] = enumbuf[1].src[j] = 'a'; break;
						case 1: enumbuf[0].src[j] = enumbuf[1].src[j] = 'b'; break;
						case 2: enumbuf[0].src[j] = enumbuf[1].src[j] = 'x'; break;
						case 3: enumbuf[0].src[j] = enumbuf[1].src[j] = '\0'; break;
					}
				}
				enumbuf[0].src[j] = enumbuf[1].src[j] = '\0'; /* NULL terminate for rtlglue_printf() */
			
				/* rtlglue_printf( "o:%2d cpuM:0x%03x strF:0x%01x %s\n", offset, needCpuMask, strflag, enumbuf[0].src ); */
				for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
				{					
					retval = model_setTestTarget( _i[i] );
					if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
					
					WRITE_MEM32( GDMACNR, 0 );
					WRITE_MEM32( GDMACNR, GDMA_ENABLE );
					WRITE_MEM32( GDMAIMR, 0 );
					WRITE_MEM32( GDMAISR, 0xffffffff );
					WRITE_MEM32( GDMAICVL, 0 );
					WRITE_MEM32( GDMAICVR, 0 );
					WRITE_MEM32( GDMASBP0, (uint32)(enumbuf[i].src) );
					WRITE_MEM32( GDMASBL0, (1<<31)|(STRING_LEN) );
					WRITE_MEM32( GDMADBP0, (uint32)SM10 );
					WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
					while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

#if 0
					rtlglue_printf( "READ_MEM32(GDMAISR)=%08x\n", READ_MEM32(GDMAISR) );
					rtlglue_printf( "READ_MEM32(GDMAICVL)=%08x\n", READ_MEM32(GDMAICVL) );
					rtlglue_printf( "READ_MEM32(GDMAICVR)=%08x\n", READ_MEM32(GDMAICVR) );
#endif
					result[i].isr = READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP; /* we only concern NEEDCPU bit */
					result[i].icvl = READ_MEM32( GDMAICVL );
					result[i].icvr = READ_MEM32( GDMAICVR );
				}

				IS_EQUAL_INT("GDMA ISR is not the same: {model:IC}=", result[0].isr, result[1].isr, __FUNCTION__, __LINE__ );
				IS_EQUAL_INT("Final Rule is not the same: {model:IC}=", result[0].icvl, result[1].icvl, __FUNCTION__, __LINE__ );
				IS_EQUAL_INT("Processed Length is not the same: {model:IC}=", result[0].icvr, result[1].icvr, __FUNCTION__, __LINE__ );
			}
		}
	}
	return SUCCESS;
}


/******************************************************************************
 **  case 11: in the final rule of current page, branch to current page      **
 ******************************************************************************/
int32 testGdmaPatternMatch11(uint32 caseNo)
{
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	int32 retval;
	pmRule_t0 *rule;
	uint32 needCpuMask;
	stateMachine_t *SM11;
	static char *teststr[] =
	{
		"b",       /* 0x1001 */
		"a",       /* 0x00ff */
		"aa",      /* 0x00ff */
		"aaa",     /* 0x00ff */
		"ab",      /* 0x01fd */
		"aab",     /* 0x01fd */
		"aaab",    /* 0x01fd */
		"aaabb",   /* 0x1002 */
		"aaaba",   /* 0x01ff */
		"aaabaa",  /* 0x1003 */
		"aaabab",  /* 0x02ff */
		"aaababb", /* 0x0278 */
		"aaababbb",/* 0x1005 */
		"aaababba",/* 0x0501 */
		"aaababa", /* 0x0264 */
		"aaababab",/* 0x1004 */
		"aaababaa",/* 0x0400 */
		"ababaab", /* 0x1007 */
		"ababaaa", /* 0x0343 */
		"ababaaab",/* 0x1006 */
		"ababaaaa",/* 0x0502 */
	};
	typedef struct result_s
	{
		uint32 isr;
		uint32 icvl;
		uint32 icvr;
	} result_t;
	result_t result[2];
	
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	
	rtlglue_printf( "TEST 11 (in the final rule of current page, branch to current page):\n" );

	rule = (pmRule_t0*)db2; /* use uncached address */

//        _cache_flush();

	SM11 = UNCACHED_ADDRESS( &rule[2000] ); /* We place SM10 in the middle of subsm */
	SM11->subsm[0] = (subStateMachine_t*)(&rule[0x0000]);
	SM11->subsm[1] = (subStateMachine_t*)(&rule[0x0100]);
	SM11->subsm[2] = (subStateMachine_t*)(&rule[0x0200]);
	SM11->subsm[3] = (subStateMachine_t*)(&rule[0x0300]);
	SM11->subsm[4] = (subStateMachine_t*)(&rule[0x0400]);
	SM11->subsm[5] = NULL;

	{
		/* Rule 0 */
		rule[0x0000].not = 0;
		rule[0x0000].matchChar = 'a';
		rule[0x0000].type = GDMA_RULETYPE_0;
		rule[0x0000].jmpState = 0x00ff;
		/* Rule 1 */
		rule[0x0001].not = 1;
		rule[0x0001].matchChar = 'a';
		rule[0x0001].type = GDMA_RULETYPE_0;
		rule[0x0001].jmpState = 0x1001;
		rule[0x0001].cpu = 1;
		
		/* Rule 255 */
		rule[0x00ff].not = 0;
		rule[0x00ff].matchChar = 'a';
		rule[0x00ff].type = GDMA_RULETYPE_0;
		rule[0x00ff].jmpState = 0x00ff;
		/* Rule 256 */
		rule[0x0100].not = 1;
		rule[0x0100].matchChar = 'a';
		rule[0x0100].type = GDMA_RULETYPE_0;
		rule[0x0100].jmpState = 0x01fd;
	
		/* Rule 509 */
		rule[0x01fd].not = 1;
		rule[0x01fd].matchChar = 'a';
		rule[0x01fd].type = GDMA_RULETYPE_0;
		rule[0x01fd].jmpState = 0x1002;
		rule[0x01fd].cpu = 1;
		/* Rule 510 */
		rule[0x01fe].not = 0;
		rule[0x01fe].matchChar = 'a';
		rule[0x01fe].type = GDMA_RULETYPE_0;
		rule[0x01fe].jmpState = 0x01ff;
		/* Rule 511 */
		rule[0x01ff].not = 0;
		rule[0x01ff].matchChar = 'b';
		rule[0x01ff].type = GDMA_RULETYPE_0;
		rule[0x01ff].jmpState = 0x02ff;
		/* Rule 512 */
		rule[0x0200].not = 1;
		rule[0x0200].matchChar = 'b';
		rule[0x0200].type = GDMA_RULETYPE_0;
		rule[0x0200].jmpState = 0x1003;
		rule[0x0200].cpu = 1;

		/* Rule 612 */
		rule[0x0264].not = 0;
		rule[0x0264].matchChar = 'a';
		rule[0x0264].type = GDMA_RULETYPE_0;
		rule[0x0264].jmpState = 0x0400;
		/* Rule 613 */
		rule[0x0265].not = 1;
		rule[0x0265].matchChar = 'a';
		rule[0x0265].type = GDMA_RULETYPE_0;
		rule[0x0265].jmpState = 0x1004;
		rule[0x0265].cpu = 1;
		/* Rule 632 */
		rule[0x0278].not = 0;
		rule[0x0278].matchChar = 'a';
		rule[0x0278].type = GDMA_RULETYPE_0;
		rule[0x0278].jmpState = 0x0501;
		rule[0x0278].cpu = 1;
		/* Rule 633 */
		rule[0x0279].not = 1;
		rule[0x0279].matchChar = 'a';
		rule[0x0279].type = GDMA_RULETYPE_0;
		rule[0x0279].jmpState = 0x1005;
		rule[0x0279].cpu = 1;
		/* Rule 767 */
		rule[0x02ff].not = 0;
		rule[0x02ff].matchChar = 'a';
		rule[0x02ff].type = GDMA_RULETYPE_0;
		rule[0x02ff].jmpState = 0x0264;
		/* Rule 768 */
		rule[0x0300].not = 1;
		rule[0x0300].matchChar = 'a';
		rule[0x0300].type = GDMA_RULETYPE_0;
		rule[0x0300].jmpState = 0x0278;

		/* Rule 835 */
		rule[0x0343].not = 0;
		rule[0x0343].matchChar = 'a';
		rule[0x0343].type = GDMA_RULETYPE_0;
		rule[0x0343].jmpState = 0x0502;
		rule[0x0343].cpu = 1;
		/* Rule 836 */
		rule[0x0344].not = 1;
		rule[0x0344].matchChar = 'a';
		rule[0x0344].type = GDMA_RULETYPE_0;
		rule[0x0344].jmpState = 0x1006;
		rule[0x0344].cpu = 1;
		
		/* Rule 1024 */
		rule[0x0400].not = 0;
		rule[0x0400].matchChar = 'a';
		rule[0x0400].type = GDMA_RULETYPE_0;
		rule[0x0400].jmpState = 0x0343;
		/* Rule 1025 */
		rule[0x0401].not = 1;
		rule[0x0401].matchChar = 'a';
		rule[0x0401].type = GDMA_RULETYPE_0;
		rule[0x0401].jmpState = 0x1007;
		rule[0x0401].cpu = 1;

		for( needCpuMask = 0; needCpuMask < (1<<9); needCpuMask++ )
		{
			/* toggle nedd CPU bit */
			rule[0x0000].cpu = (needCpuMask&(1<<0)?TRUE:FALSE);
			rule[0x00ff].cpu = (needCpuMask&(1<<1)?TRUE:FALSE);
			rule[0x0100].cpu = (needCpuMask&(1<<2)?TRUE:FALSE);
			rule[0x01fe].cpu = (needCpuMask&(1<<3)?TRUE:FALSE);
			rule[0x01ff].cpu = (needCpuMask&(1<<4)?TRUE:FALSE);
			rule[0x0264].cpu = (needCpuMask&(1<<5)?TRUE:FALSE);
			rule[0x02ff].cpu = (needCpuMask&(1<<6)?TRUE:FALSE);
			rule[0x0300].cpu = (needCpuMask&(1<<7)?TRUE:FALSE);
			rule[0x0400].cpu = (needCpuMask&(1<<8)?TRUE:FALSE);

			for( j = 0; j < sizeof(teststr)/sizeof(teststr[0]); j++ )
			{
				strcpy( enumbuf[0].src, teststr[j] );
				strcpy( enumbuf[1].src, teststr[j] );
			
				/* rtlglue_printf( "cpuM:0x%03x j:%d %s: ", needCpuMask, j, enumbuf[0].src ); */
				for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
				{					
					retval = model_setTestTarget( _i[i] );
					if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
					
					WRITE_MEM32( GDMACNR, 0 );
					WRITE_MEM32( GDMACNR, GDMA_ENABLE );
					WRITE_MEM32( GDMAIMR, 0 );
					WRITE_MEM32( GDMAISR, 0xffffffff );
					WRITE_MEM32( GDMAICVL, 0 );
					WRITE_MEM32( GDMAICVR, 0 );
					WRITE_MEM32( GDMASBP0, (uint32)(enumbuf[i].src) );
					WRITE_MEM32( GDMASBL0, (1<<31)|(strlen(enumbuf[0].src)) ); /* use cached address to speed up */
					WRITE_MEM32( GDMADBP0, (uint32)SM11 );
					WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
					while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

#if 0
					rtlglue_printf( "READ_MEM32(GDMAISR)=%08x\n", READ_MEM32(GDMAISR) );
					rtlglue_printf( "READ_MEM32(GDMAICVL)=%08x\n", READ_MEM32(GDMAICVL) );
					rtlglue_printf( "READ_MEM32(GDMAICVR)=%08x\n", READ_MEM32(GDMAICVR) );
#endif
					result[i].isr = READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP; /* we only concern NEEDCPU bit */
					result[i].icvl = READ_MEM32( GDMAICVL );
					result[i].icvr = READ_MEM32( GDMAICVR );
				}
				/*rtlglue_printf("0x%04x\n", result[0].icvl );*/

				IS_EQUAL_INT("GDMA ISR is not the same: {model:IC}=", result[0].isr, result[1].isr, __FUNCTION__, __LINE__ );
				IS_EQUAL_INT("Final Rule is not the same: {model:IC}=", result[0].icvl, result[1].icvl, __FUNCTION__, __LINE__ );
				IS_EQUAL_INT("Processed Length is not the same: {model:IC}=", result[0].icvr, result[1].icvr, __FUNCTION__, __LINE__ );
			}
		}
	}
	return SUCCESS;
}


/******************************************************************************
 **  case 12: initial state is not in page 0                                 **
 ******************************************************************************/
int32 testGdmaPatternMatch12(uint32 caseNo)
{
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	int32 retval;
	pmRule_t0 *rule;
	uint32 needCpuMask;
	stateMachine_t *SM12;
	struct
	{
		uint32 iIcvl;
		uint8 *str;
		uint32 rIcvl;
		uint32 rIcvr;
	} 
	static testvec[] = 
	{
		{
			iIcvl: 0x0123,
			str: "a",
			rIcvl: 0x00ff,
			rIcvr: 0x0001,
		},
		{
			iIcvl: 0x0123,
			str: "b",
			rIcvl: 0x0234,
			rIcvr: 0x0001,
		},
		{
			iIcvl: 0x0123,
			str: "aa",
			rIcvl: 0x0301,
			rIcvr: 0x0002,
		},
		{
			iIcvl: 0x0123,
			str: "aaa",
			rIcvl: 0x0405,
			rIcvr: 0x0003,
		},
		{
			iIcvl: 0x0123,
			str: "aab",
			rIcvl: 0x0406,
			rIcvr: 0x0003,
		},
		{
			iIcvl: 0x0123,
			str: "ab",
			rIcvl: 0x0402,
			rIcvr: 0x0002,
		},
		{
			iIcvl: 0x0123,
			str: "ba",
			rIcvl: 0x0403,
			rIcvr: 0x0002,
		},
		{
			iIcvl: 0x0123,
			str: "bb",
			rIcvl: 0x0404,
			rIcvr: 0x0002,
		},
	}
	;
	typedef struct result_s
	{
		uint32 isr;
		uint32 icvl;
		uint32 icvr;
	} result_t;
	result_t result[2];
	
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	
	rtlglue_printf( "TEST 12 (initial state is not in page 0):\n" );

	rule = (pmRule_t0*)db2; /* use uncached address */

//        _cache_flush();

	SM12 = UNCACHED_ADDRESS( &rule[0x0500] ); /* We place SM10 in the middle of subsm */
	SM12->subsm[0] = (subStateMachine_t*)(&rule[0x0000]);
	SM12->subsm[1] = (subStateMachine_t*)(&rule[0x0100]);
	SM12->subsm[2] = (subStateMachine_t*)(&rule[0x0200]);
	SM12->subsm[3] = (subStateMachine_t*)(&rule[0x0300]);
	SM12->subsm[4] = (subStateMachine_t*)(&rule[0x0400]);
	SM12->subsm[5] = NULL;

	{
		/* Rule 255 */
		rule[0x00ff].not = 0;
		rule[0x00ff].matchChar = 'a';
		rule[0x00ff].type = GDMA_RULETYPE_0;
		rule[0x00ff].jmpState = 0x0301;
		/* Rule 255 */
		rule[0x0100].not = 1;
		rule[0x0100].matchChar = 'a';
		rule[0x0100].type = GDMA_RULETYPE_0;
		rule[0x0100].jmpState = 0x0402;
		rule[0x0100].cpu = 1;
	
		/* Rule 291 */
		rule[0x0123].not = 0;
		rule[0x0123].matchChar = 'a';
		rule[0x0123].type = GDMA_RULETYPE_0;
		rule[0x0123].jmpState = 0x00ff;
		/* Rule 292 */
		rule[0x0124].not = 1;
		rule[0x0124].matchChar = 'a';
		rule[0x0124].type = GDMA_RULETYPE_0;
		rule[0x0124].jmpState = 0x0234;

		/* Rule 564 */
		rule[0x0234].not = 0;
		rule[0x0234].matchChar = 'a';
		rule[0x0234].type = GDMA_RULETYPE_0;
		rule[0x0234].jmpState = 0x0403;
		rule[0x0234].cpu = 1;
		/* Rule 565 */
		rule[0x0235].not = 1;
		rule[0x0235].matchChar = 'a';
		rule[0x0235].type = GDMA_RULETYPE_0;
		rule[0x0235].jmpState = 0x0404;
		rule[0x0235].cpu = 1;

		/* Rule 769 */
		rule[0x0301].not = 0;
		rule[0x0301].matchChar = 'a';
		rule[0x0301].type = GDMA_RULETYPE_0;
		rule[0x0301].jmpState = 0x0405;
		rule[0x0301].cpu = 1;
		/* Rule 770 */
		rule[0x0302].not = 1;
		rule[0x0302].matchChar = 'a';
		rule[0x0302].type = GDMA_RULETYPE_0;
		rule[0x0302].jmpState = 0x0406;
		rule[0x0302].cpu = 1;

		/* In kernel mode, we can refer model and FPGA to compare results. */
		for( needCpuMask = 0; needCpuMask < (1<<3); needCpuMask++ )
		{
			/* toggle nedd CPU bit */
			rule[0x00ff].cpu = (needCpuMask&(1<<0)?TRUE:FALSE);
			rule[0x0123].cpu = (needCpuMask&(1<<1)?TRUE:FALSE);
			rule[0x0124].cpu = (needCpuMask&(1<<2)?TRUE:FALSE);

			for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
			{
				strcpy( enumbuf[0].src, testvec[j].str );
				strcpy( enumbuf[1].src, testvec[j].str );
			
				/* rtlglue_printf( "cpuM:0x%03x j:%d src:%s ==>", needCpuMask, j, enumbuf[0].src ); */
				for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
				{					
					retval = model_setTestTarget( _i[i] );
					if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
					
					WRITE_MEM32( GDMACNR, 0 );
					WRITE_MEM32( GDMACNR, GDMA_ENABLE );
					WRITE_MEM32( GDMAIMR, 0 );
					WRITE_MEM32( GDMAISR, 0xffffffff );
					WRITE_MEM32( GDMAICVL, testvec[j].iIcvl );
					WRITE_MEM32( GDMAICVR, 0 );
					WRITE_MEM32( GDMASBP0, (uint32)(enumbuf[i].src) );
					WRITE_MEM32( GDMASBL0, (1<<31)|(strlen(enumbuf[0].src)) );
					WRITE_MEM32( GDMADBP0, (uint32)SM12 );
					WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
					while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

#if 0
					rtlglue_printf( "READ_MEM32(GDMAISR)=%08x\n", READ_MEM32(GDMAISR) );
					rtlglue_printf( "READ_MEM32(GDMAICVL)=%08x\n", READ_MEM32(GDMAICVL) );
					rtlglue_printf( "READ_MEM32(GDMAICVR)=%08x\n", READ_MEM32(GDMAICVR) );
#endif
					rtlglue_printf("[i=%d] ", i );
					if ( needCpuMask == 0 )
					{
						/* Only when no needCpu bit test, we will compare with expected result. */
						IS_EQUAL_INT("GDMA ICVL is not the expected: {expected:real}=", testvec[j].rIcvl, READ_MEM32( GDMAICVL ), __FUNCTION__, __LINE__ );
						IS_EQUAL_INT("GDMA ICVR is not the expected: {expected:real}=", testvec[j].rIcvr, READ_MEM32( GDMAICVR ), __FUNCTION__, __LINE__ );
					}
					
					result[i].isr = READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP; /* we only concern NEEDCPU bit */
					result[i].icvl = READ_MEM32( GDMAICVL );
					result[i].icvr = READ_MEM32( GDMAICVR );
				}
				rtlglue_printf("State=0x%04x\n", result[0].icvl );

				IS_EQUAL_INT("GDMA ISR is not the same: {model:IC}=", result[0].isr, result[1].isr, __FUNCTION__, __LINE__ );
				IS_EQUAL_INT("Final Rule is not the same: {model:IC}=", result[0].icvl, result[1].icvl, __FUNCTION__, __LINE__ );
				IS_EQUAL_INT("Processed Length is not the same: {model:IC}=", result[0].icvr, result[1].icvr, __FUNCTION__, __LINE__ );
			}
		}
	}
	return SUCCESS;
}


/******************************************************************************
 **  case 13: highest performance benchmark (from testGdmaPatternMatch6())   **
 ******************************************************************************/
descriptor_t descTestString13[8];
int32 testGdmaPatternMatch13(uint32 caseNo)
{
	int i, j, k;
	int accept, notaccept;
	int32 retval;
	pmVsm_t vsm;
	uint8 *pat = "nanano";
	uint8 *found;
	static int _i[] = { IC_TYPE_REAL };
	uint32 msStart, msStop;
	uint32 rIcvl[sizeof(_i)/sizeof(_i[0])];
	uint32 round = 0;
	uint32 perf;
	uint32 testlen = (1<<26);

	testGdmaInit();

	for( j = 0; j < 8; j++ )
	{
		descTestString13[j].pData = &sb1[MAX_BLOCK_LENGTH*j];
		descTestString13[j].ldb = (j==7)?TRUE:FALSE;
		descTestString13[j].length = MAX_BLOCK_LENGTH;
	}
	
	rtlglue_printf( "TEST 13 (highest performance benchmark):\n" );

	vsmInit( &vsm );
	retval = vsmGenFromKMP( &vsm, pat, strlen(pat) );
	ASSERT( retval==SUCCESS );

	/*displayStateMachine( &vsm );*/

	/* prepare random data */ 
	for( j = 0; j < ALL_BLOCK_LENGTH; j++ )
		sb1[j] = rtlglue_random()%26 + 'a';
	sb1[ALL_BLOCK_LENGTH-1] = '\0';
	if ( 0 && rtlglue_random()&0x00010000 ) /* We use bit 16, instead of random()%2 */
	{
		j = rtlglue_random()%(ALL_BLOCK_LENGTH-1-strlen(pat));
		memcpy( &sb1[j], pat, strlen(pat) );
	}
	if ( ( found = strstr( sb1, pat ) ) )
		rtlglue_printf("ACCEPT\n");
	else
		rtlglue_printf("NOT ACCEPT\n");
	
	accept = notaccept = 0;
	rtlglue_getmstime( &msStart );
	rIcvl[0] = 0;
	for( k = 0, round = 0; k < ((testlen/ALL_BLOCK_LENGTH)+1); k++, round++ )
	{
		/*rtlglue_printf("[%d] ", round );*/
		
		for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
		{
			retval = model_setTestTarget( _i[i] );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			WRITE_MEM32( GDMACNR, 0 );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			WRITE_MEM32( GDMAIMR, 0 );
			WRITE_MEM32( GDMAISR, 0xffffffff );
			WRITE_MEM32( GDMAICVL, rIcvl[i] );
			WRITE_MEM32( GDMAICVR, 0 );
			for( j = 0; j < 8; j++ )
			{
				WRITE_MEM32( GDMASBP0+j*8, (uint32)(descTestString13[j].pData) );
				WRITE_MEM32( GDMASBL0+j*8, (descTestString13[j].ldb<<31)|(descTestString13[j].length) );
			}
			WRITE_MEM32( GDMADBP0, (uint32)vsm.prefix[0].pStateMachine );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
			while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

			if ( found )
			{
				accept++;
				IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
				EXPECT_ACCEPT( READ_MEM32( GDMAICVL ) );
				IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), (found-sb1)+strlen(pat), __FUNCTION__, __LINE__ );
				rIcvl[i] = 0;
			}
			else
			{
				uint32 totalLen = 0;
				
				notaccept++;
				IS_EQUAL_INT("NeedCPU is set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, 0, __FUNCTION__, __LINE__ );
				EXPECT_NOT_ACCEPT( READ_MEM32( GDMAICVL ) );
				for( j = 0; j < 8; j++ )
					totalLen += descTestString13[j].length;
				IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), totalLen, __FUNCTION__, __LINE__ );
				rIcvl[i] = READ_MEM32( GDMAICVL );
			}
		}
	}
	rtlglue_getmstime( &msStop );

	vsmFree( &vsm );

	rtlglue_printf("Accepted:%d  Not Accepted:%d\n", accept, notaccept );
	if ( (msStop-msStart) > 0 )
	{
		perf = round*ALL_BLOCK_LENGTH/(msStop-msStart)*1000/1024*100/*percent*//1024/*for scale*/;
		rtlglue_printf("Total time: %d ms, throuput: %d.%02dMBps\n", msStop-msStart, perf/100, perf%100 );
	}
	else
	{
		rtlglue_printf("Total time is too small to calculate performance. Please enlarge testlen.\n" );
	}

	return SUCCESS;
}


/******************************************************************************
 **  case 14: lowest performance benchmark (from testGdmaPatternMatch13())   **
 **           0x00000:'\x00' --> 0x00100                                     **
 **           0x00001:'\x01' --> 0x00200                                     **
 **               :                                                          **
 **           0x00100:'\x00' --> 0x00000                                     **
 **           0x00101:'\x01' --> 0x00000                                     **
 **               :                                                          **
 **           0x00200:'\x00' --> 0x00000                                     **
 **           0x00201:'\x01' --> 0x00000                                     **
 **               :                                                          **
 ******************************************************************************/
descriptor_t descTestString14[8];
int32 testGdmaPatternMatch14(uint32 caseNo)
{
	int i, j, k;
	int32 retval;
	static int _i[] = { IC_TYPE_REAL };
	uint32 msStart, msStop;
	uint32 round = 0;
	uint32 perf;
	pmRule_t0 *rule;
	stateMachine_t *SM14;
	uint32 maxState = 256;
	uint32 testlen = (1<<18);

	testGdmaInit();

	for( j = 0; j < 8; j++ )
	{
		descTestString14[j].pData = &sb1[MAX_BLOCK_LENGTH*j];
		descTestString14[j].ldb = (j==7)?TRUE:FALSE;
		descTestString14[j].length = MAX_BLOCK_LENGTH;
	}
	
	rtlglue_printf( "TEST 14 (lowest performance benchmark):\n" );

//        _cache_flush();

	/* prepare subSM */
	SM14 = UNCACHED_ADDRESS( db2 );
	for( i = 0; i < maxState+1; i++ )
	{
		rule = (pmRule_t0*)UNCACHED_ADDRESS(rtlglue_malloc(INTERNAL_RULE_NUMBER*sizeof(pmRule_t0))); /* use uncached address */
		memset( rule, 0, (INTERNAL_RULE_NUMBER*sizeof(pmRule_t0)) );
		SM14->subsm[i] = (subStateMachine_t*)rule;
	}

	/* prepare rules */
	for( i = 0; i < maxState; i++ )
	{
		/* set page 0 */
		rule = &SM14->subsm[0]->rules[0];
		rule[i].matchChar = i;
		rule[i].type = 0;
		rule[i].jmpState = (i+1)*INTERNAL_RULE_NUMBER;

		/* set page i+1 */
		rule = &SM14->subsm[i+1]->rules[0];
		for( j = 0; j < maxState; j++ )
		{
			rule[j].matchChar = j;
			rule[j].type = 0;
			rule[j].jmpState = 0x00000;
		}
	}

#if 0 /* for debug */
	for( i = 0; i < maxState+1; i++ )
	{
		char str[32];
		sprintf( str, "smbSM[0x%03x]", i );
		memDump( SM14->subsm[i], maxState*sizeof(pmRule_t0), str );
	}
	/*displayStateMachine( &vsm );*/
#endif

	/* prepare random data */ 
	for( j = 0; j < ALL_BLOCK_LENGTH; j++ )
		sb1[j] = rtlglue_random()%maxState;

	rtlglue_getmstime( &msStart );
	for( k = 0, round = 0; k < ((testlen/ALL_BLOCK_LENGTH)+1); k++, round++ )
	{
		/*rtlglue_printf("[%d] ", round );*/
		
		for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
		{
			retval = model_setTestTarget( _i[i] );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			WRITE_MEM32( GDMACNR, 0 );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			WRITE_MEM32( GDMAIMR, 0 );
			WRITE_MEM32( GDMAISR, 0xffffffff );
			WRITE_MEM32( GDMAICVL, 0 );
			WRITE_MEM32( GDMAICVR, 0 );
			for( j = 0; j < 8; j++ )
			{
				WRITE_MEM32( GDMASBP0+j*8, (uint32)(descTestString14[j].pData) );
				WRITE_MEM32( GDMASBL0+j*8, (descTestString14[j].ldb<<31)|(descTestString14[j].length) );
			}
			WRITE_MEM32( GDMADBP0, (uint32)&SM14->subsm[0] );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR);
			while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		}
	}
	rtlglue_getmstime( &msStop );

	for( i = 0; i < maxState+1; i++ )
	{
		rule = &SM14->subsm[i]->rules[0];
		rtlglue_free( CACHED_ADDRESS( rule ) );
	}

	if ( (msStop-msStart) > 0 )
	{
		perf = round*ALL_BLOCK_LENGTH/(msStop-msStart)*1000/1024*100/*percent*//1024/*for scale*/;
		rtlglue_printf("Total time: %d ms, throuput: %d.%02dMBps\n", msStop-msStart, perf/100, perf%100 );
	}
	else
	{
		rtlglue_printf("Total time is too small to calculate performance. Please enlarge testlen.\n" );
	}

	return SUCCESS;
}



/******************************************************************************
 **  case 15 - a TCP connections                                             **
 ******************************************************************************/
subStateMachine_t subSM15 =
{
	rules:
	{
		{ /* 0 - DMAC0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 2,
			matchChar: '\x00'
		},
		{ /* 1 - DMAC0 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x00'
		},
		{ /* 2 - DMAC1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 4,
			matchChar: '\x00'
		},
		{ /* 3 - DMAC1 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x00'
		},
		{ /* 4 - DMAC2 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 6,
			matchChar: '\x10'
		},
		{ /* 5 - DMAC2 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x10'
		},
		{ /* 6 - DMAC3 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 8,
			matchChar: '\x11'
		},
		{ /* 7 - DMAC3 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x11'
		},
		{ /* 8 - DMAC4 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 10,
			matchChar: '\x12'
		},
		{ /* 9 - DMAC4 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x12'
		},
		{ /* 10 - DMAC5 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 12,
			matchChar: '\x20'
		},
		{ /* 11 - DMAC5 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x20'
		},
		
		{ /* 12 - SMAC0 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 14,
			matchChar: '\x00'
		},
		{ /* 13 - SMAC0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 14,
			matchChar: '\x00'
		},
		{ /* 14 - SMAC1 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 16,
			matchChar: '\x00'
		},
		{ /* 15 - SMAC1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 16,
			matchChar: '\x00'
		},
		{ /* 16 - SMAC2 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 18,
			matchChar: '\x00'
		},
		{ /* 17 - SMAC2 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 18,
			matchChar: '\x00'
		},
		{ /* 18 - SMAC3 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 20,
			matchChar: '\x00'
		},
		{ /* 19 - SMAC3 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 20,
			matchChar: '\x00'
		},
		{ /* 20 - SMAC4 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 22,
			matchChar: '\x00'
		},
		{ /* 21 - SMAC4 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 22,
			matchChar: '\x00'
		},
		{ /* 22 - SMAC5 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 24,
			matchChar: '\x00'
		},
		{ /* 23 - SMAC5 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 24,
			matchChar: '\x00'
		},
		
		{ /* 24 - ethtype0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 26,
			matchChar: '\x08'
		},
		{ /* 25 - ethtype0 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x08'
		},
		{ /* 26 - ethtype1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 28,
			matchChar: '\x00'
		},
		{ /* 27 - ethtype1 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x00'
		},

		{ /* 28 - IP Ver/Len */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 30,
			matchChar: '\x45'
		},
		{ /* 29 - IP Ver/Len */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x45'
		},

		{ /* 30 - IP TOS */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 32,
			matchChar: '\x00'
		},
		{ /* 31 - IP TOS */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 32,
			matchChar: '\x00'
		},

		{ /* 32 - IP LEN0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 34,
			matchChar: '\x00'
		},
		{ /* 33 - IP LEN0 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 34,
			matchChar: '\x00'
		},
		{ /* 34 - IP LEN1 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 36,
			matchChar: '\x00'
		},
		{ /* 35 - IP LEN1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 36,
			matchChar: '\x00'
		},

		{ /* 36 - IP ID0 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 38,
			matchChar: '\x00'
		},
		{ /* 37 - IP ID0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 38,
			matchChar: '\x00'
		},
		{ /* 38 - IP ID1 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 40,
			matchChar: '\x00'
		},
		{ /* 39 - IP ID1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 40,
			matchChar: '\x00'
		},

		{ /* 40 - IP frag0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 42,
			matchChar: '\x00'
		},
		{ /* 41 - IP frag0 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 42,
			matchChar: '\x00'
		},
		{ /* 42 - IP frag1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 44,
			matchChar: '\x00'
		},
		{ /* 43 - IP frag1 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 44,
			matchChar: '\x00'
		},
		
		{ /* 44 - IP TTL */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 46,
			matchChar: '\x00'
		},
		{ /* 45 - IP TTL */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 46,
			matchChar: '\x00'
		},
		
		{ /* 46 - IP Protocol */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 48,
			matchChar: '\x06'
		},
		{ /* 47 - IP Protocol */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x06'
		},
		
		{ /* 48 - IP ChkSum0 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 50,
			matchChar: '\x00'
		},
		{ /* 49 - IP ChkSum0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 50,
			matchChar: '\x00'
		},
		{ /* 50 - IP ChkSum1 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 52,
			matchChar: '\x00'
		},
		{ /* 51 - IP ChkSum1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 52,
			matchChar: '\x00'
		},
		
		{ /* 52 - IP SIP0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 54,
			matchChar: '\xc0'
		},
		{ /* 53 - IP SIP0 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\xc0'
		},
		{ /* 54 - IP SIP1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 56,
			matchChar: '\xa8'
		},
		{ /* 55 - IP SIP1 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\xa8'
		},
		{ /* 56 - IP SIP2 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 58,
			matchChar: '\x01'
		},
		{ /* 57 - IP SIP2 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x01'
		},
		{ /* 58 - IP SIP3 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 60,
			matchChar: '\x01'
		},
		{ /* 59 - IP SIP3 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x01'
		},
		
		{ /* 60 - IP DIP0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 62,
			matchChar: '\x8c'
		},
		{ /* 61 - IP DIP0 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x8c'
		},
		{ /* 62 - IP DIP1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 64,
			matchChar: '\x71'
		},
		{ /* 63 - IP DIP1 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x71'
		},
		{ /* 64 - IP DIP2 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 66,
			matchChar: '\xd6'
		},
		{ /* 65 - IP DIP2 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\xd6'
		},
		{ /* 66 - IP DIP3 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 68,
			matchChar: '\x66'
		},
		{ /* 67 - IP DIP3 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x66'
		},
		
		{ /* 68 - TCP SPORT0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 70,
			matchChar: '\x12'
		},
		{ /* 69 - TCP SPORT0 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x12'
		},
		{ /* 70 - TCP SPORT1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 72,
			matchChar: '\x34'
		},
		{ /* 71 - TCP SPORT1 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x34'
		},

		{ /* 72 - TCP SPORT0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 74,
			matchChar: '\x00'
		},
		{ /* 73 - TCP SPORT0 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x00'
		},
		{ /* 74 - TCP SPORT1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 76,
			matchChar: '\x50'
		},
		{ /* 75 - TCP SPORT1 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0xfffff,
			matchChar: '\x50'
		},

		{ /* 76 - TCP SEQ0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 78,
			matchChar: '\x00'
		},
		{ /* 77 - TCP SEQ0 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 78,
			matchChar: '\x00'
		},
		{ /* 78 - TCP SEQ1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 80,
			matchChar: '\x00'
		},
		{ /* 79 - TCP SEQ1 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 80,
			matchChar: '\x00'
		},
		{ /* 80 - TCP SEQ2 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 82,
			matchChar: '\x00'
		},
		{ /* 81 - TCP SEQ2 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 82,
			matchChar: '\x00'
		},
		{ /* 82 - TCP SEQ3 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 84,
			matchChar: '\x00'
		},
		{ /* 83 - TCP SEQ3 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 84,
			matchChar: '\x00'
		},

		{ /* 84 - TCP ACK0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 86,
			matchChar: '\x00'
		},
		{ /* 85 - TCP ACK0 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 86,
			matchChar: '\x00'
		},
		{ /* 86 - TCP ACK1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 88,
			matchChar: '\x00'
		},
		{ /* 87 - TCP ACK1 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 88,
			matchChar: '\x00'
		},
		{ /* 88 - TCP ACK2 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 90,
			matchChar: '\x00'
		},
		{ /* 89 - TCP ACK2 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 90,
			matchChar: '\x00'
		},
		{ /* 90 - TCP ACK3 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 92,
			matchChar: '\x00'
		},
		{ /* 91 - TCP ACK3 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 92,
			matchChar: '\x00'
		},

		{ /* 92 - TCP Flag0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 94,
			matchChar: '\x00'
		},
		{ /* 93 - TCP Flag0 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 94,
			matchChar: '\x00'
		},
		{ /* 94 - TCP Flag1-ACK */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 98,
			matchChar: '\x10'
		},
		{ /* 95 - TCP Flag1-PUSH */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 98,
			matchChar: '\x08'
		},
		{ /* 96 - TCP Flag1-ACK,PUSH */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 98,
			matchChar: '\x18'
		},
		{ /* 97 - TCP Flag1 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 0x98,
			matchChar: '\x10'
		},

		{ /* 98 - TCP window0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 100,
			matchChar: '\x00'
		},
		{ /* 99 - TCP window0 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 100,
			matchChar: '\x00'
		},
		{ /* 100 - TCP window1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 102,
			matchChar: '\x00'
		},
		{ /* 101 - TCP window1 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 102,
			matchChar: '\x00'
		},

		{ /* 102 - TCP Chksum0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 104,
			matchChar: '\x00'
		},
		{ /* 103 - TCP Chksum0 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 104,
			matchChar: '\x00'
		},
		{ /* 104 - TCP Chksum1 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 106,
			matchChar: '\x00'
		},
		{ /* 105 - TCP Chksum1 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 106,
			matchChar: '\x00'
		},

		{ /* 106 - TCP urgent0 */
			type:0,
			not: 0,
			cpu: 0,
			jmpState: 108,
			matchChar: '\x00'
		},
		{ /* 107 - TCP urgent0 */
			type:0,
			not: 1,
			cpu: 0,
			jmpState: 108,
			matchChar: '\x00'
		},
		{ /* 108 - TCP urgent1 */
			type:0,
			not: 0,
			cpu: 1,
			jmpState: 110,
			matchChar: '\x00'
		},
		{ /* 109 - TCP urgent1 */
			type:0,
			not: 1,
			cpu: 1,
			jmpState: 110,
			matchChar: '\x00'
		},

	},
};
stateMachine_t SM15 =
{
	subsm:
	{
		&subSM15,
		NULL
	}
};
uint8 testString15[] = 
	/*DMAC*/"\x00\x00\x10\x11\x12\x20" 
	/*SMAC*/"\x22\x33\x44\x55\x66\x77"
	/*ethtype*/"\x08\x00"
	/*VerLen/TOS*/"\x45\x00"/*Len/ID*/"\x00\x40\x55\x66"/*frag/ttl*/"\x00\x00\x40"/*protocol/chksum*/"\x06\x12\x34"
	/*SIP*/"\xc0\xa8\x01\x01"
	/*DIP*/"\x8c\x71\xd6\x66"
	/*sport*/"\x12\x34"
	/*dport*/"\x00\x50"
	/*seq*/"\x00\x01\x02\x03"/*ack*/"\x04\x05\x06\x07"/*flags*/"\x00\x18"
	/*window*/"\x77\x88"/*chksum*/"\x56\x78"/*urgent*/"\x00\x00"
	/*data*/"GET / HTTP/1.0\n\n"
;
descriptor_t descTestString15[] = 
{
	{ /* PTR to Ethernet Header */
		pData: (void*)&testString15[0],
		ldb: 1,
		length: sizeof(testString15)/sizeof(testString15[0])-1
	},
	{ /* PTR to IP Header */
		pData: (void*)&testString15[14],
		ldb: 1,
		length: sizeof(testString15)/sizeof(testString15[0])-1
	},
	{ /* PTR to SIP */
		pData: (void*)&testString15[14+12],
		ldb: 1,
		length: sizeof(testString15)/sizeof(testString15[0])-1
	},
};

int32 testGdmaPatternMatch15(uint32 caseNo)
{
	int i;
	int32 retval;

	testGdmaInit();
	
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		/* Scan from Ethernet Header */
		rtlglue_printf( "TEST 1 (Scan from Ethernet Header):\n" );
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, *((uint32*)&descTestString15[0].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString15[0].ldb<<31)|(descTestString15[0].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM15 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR );
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
	
		IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Expect Matched TCP Connction", READ_MEM32(GDMAICVL), 110, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), 14+20+20/*eth+IP+TCP*/, __FUNCTION__, __LINE__ );

		/* Scan from IP Header */
		rtlglue_printf( "TEST 2 (Scan from IP Header):\n" );
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 28 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, *((uint32*)&descTestString15[1].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString15[1].ldb<<31)|(descTestString15[1].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM15 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR );
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
	
		IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Expect Matched TCP Connction", READ_MEM32(GDMAICVL), 110, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), 20+20/*IP+TCP*/, __FUNCTION__, __LINE__ );

		/* Scan from SIP */
		rtlglue_printf( "TEST 3 (Scan from SIP):\n" );
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 52 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, *((uint32*)&descTestString15[2].pData) );
		WRITE_MEM32( GDMASBL0, (descTestString15[2].ldb<<31)|(descTestString15[2].length) );
		WRITE_MEM32( GDMADBP0, (uint32)&SM15 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_PATTERN|internalUsedGDMACNR );
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
	
		IS_EQUAL_INT("NeedCPU is not set", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Expect Matched TCP Connction", READ_MEM32(GDMAICVL), 110, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Processed length is not the same", READ_MEM32(GDMAICVR), 4+4+20/*SIP+DIP+TCP*/, __FUNCTION__, __LINE__ );
	}

	return SUCCESS;
}
#ifdef DULL_GDMA
int32 dup_GDMA_data(uint32 *distGdmaBase, uint32 *srcGdmaBase,\
      uint32 *srcGdmaDataBase, uint32 srcGdmaDataLength, uint32 *distGdmaDataBase, uint32 distGdmaDataLength)
{
	uint32 i;
	uint32 tmpValue;
	uint32 *tmpsrc, *tmpdist;
	volatile uint32 *pSrcDiscrptor, *pDistDistcriptor;
	volatile uint32 *pSrcLength, *pDistLength;

	/* 
         *  copy whole data
         */

		tmpsrc = srcGdmaDataBase;
		tmpdist = distGdmaDataBase;
	if(distGdmaDataLength < srcGdmaDataLength) /* check data length */
	{
		rtlglue_printf("dup_GDMA_data error: source data length > distination data length.\n");
		rtlglue_printf("%s, %d\n", __FUNCTION__, __LINE__);
		return -1; /* return fail;*/
	}
	for(i=0; i<srcGdmaDataLength; i=i+4)
	{
		*tmpdist = *tmpsrc;
		tmpdist++;
		tmpsrc++;
	}

	/* 
	 * copy register data 
	 */

	/* source discriptor */
	srcDiscrptor = srcGdmaBase + 5;
	srcLength = srcGdmaBase + 6;
	distDiscrptor = distGdmaBase + 5;
	distLength = distGdmaBase + 6;
	for(i=0; i<8; i++)
	{
		/* src descriptor pointer */
		tmpValue = *pSrcDiscrptor;
		tmpValue = tmpValue - (uint32)srcGdmaDataBase;
		tmpValue = tmpValue + GDMA2_SRC_BASE;
		*pDistDistcriptor = tmpValue;

		/* src descriptor length */
		*distLength = *srcLength;
		if( 0 != ((*srcLength)&(0x80000000)) )
		{
			break;
		}

		pDistDiscrptor+=2;
		pDistLength+=2;
		pSrcDiscrptor+=2;
		pSrcLength+=2;
	}
	/* distination discriptor */
	srcDiscrptor = srcGdmaBase + 21;
	srcLength = srcGdmaBase + 22;
	distDiscrptor = distGdmaBase + 21;
	distLength = distGdmaBase + 22;
	for(i=0; i<8; i++)
	{
		/* src descriptor pointer */
		tmpValue = *pSrcDiscrptor;
		tmpValue = tmpValue - (uint32)srcGdmaDataBase;
		tmpValue = tmpValue + GDMA2_SRC_BASE;
		*pDistDistcriptor = tmpValue;

		/* src descriptor length */
		*distLength = *srcLength;
		if( 0 != ((*srcLength)&(0x80000000)) )
		{
			break;
		}

		pDistDiscrptor+=2;
		pDistLength+=2;
		pSrcDiscrptor+=2;
		pSrcLength+=2;
	}
}

#endif
/*
 *	To compare {sb1,db1} and {sb2,db2}
 */
int32 compare_data_block( void )
{
	int32 retval = SUCCESS;
	int i;
#if 1
	uint32 *du1, *du2;
	uint32 DU1, DU2;

	if ( gdmaOptimize&OPT_IGNSRC )
	{
		/* do nothing, if we need to ignore comparing source block. */
	}
	else
	{
		uint32 *su1, *su2;
		uint32 SU1, SU2;

		su1 = (uint32*)&sb1[0];
		su2 = (uint32*)&sb2[0];
		for( i = 0; i < HEAPSIZE/4; i++ )
		{
			if ( (SU1=*su1) != (SU2=*su2) )
			{
				rtlglue_printf( "sb1(%p) sb2(%p) is diff since %drd bytes sb1(0x%02x), sb2(0x%02x).\n", sb1, sb2, i*4, SU1, SU2 );
				if ( ( SU1 != *su1 ) || ( SU2 != *su2 ) ) rtlglue_printf( "strange! *su1=0x%08x, *su2=0x%08x; however, SU1=0x%08x, SU2=0x%08x\n", *su1, *su2, SU1, SU2 );
				retval = FAILED;
				break;
			}
			su1++;
			su2++;
		}
	}

	du1 = (uint32*)&db1[0];
	du2 = (uint32*)&db2[0];
	for( i = 0; i < HEAPSIZE/4; i++ )
	{
		if ( (DU1=*du1) != (DU2=*du2) )
		{
			rtlglue_printf( "db1(%p) db2(%p) is diff since %drd bytes db1(0x%02x), db2(0x%02x).\n", db1, db2, i*4, DU1, DU2 );
			if ( ( DU1 != *du1 ) || ( DU2 != *du2 ) ) rtlglue_printf( "strange! *du1=0x%08x, *du2=0x%08x; however, DU1=0x%08x, DU2=0x%08x\n", *du1, *du2, DU1, DU2 );
			retval = FAILED;
			break;
		}
		du1++;
		du2++;
	}
	
#elif 0
	int i;

	for( i = 0; i < HEAPSIZE; i++ )
	{
		if ( sb1[i] != sb2[i] )
		{
			rtlglue_printf( "sb1(%p) sb2(%p) is diff since %drd bytes sb1(0x%02x), sb2(0x%02x).\n", sb1, sb2, i, sb1[i], sb2[i] );
			retval = FAILED;
			break;
		}
	}
	for( i = 0; i < HEAPSIZE; i++ )
	{
		if ( db1[i] != db2[i] )
		{
			rtlglue_printf( "db1(%p) db2(%p) is diff since %drd bytes db1(0x%02x), db2(0x%02x).\n", db1, db2, i, db1[i], db2[i] );
			retval = FAILED;
			break;
		}
	}
#else
	if ( memcmp( sb1, sb2, HEAPSIZE ) != 0 )
	{
		rtlglue_printf( "sb1(%p) sb2(%p) is different.\n", sb1, sb2 );
		return FAILED;
	}
	if ( memcmp( db1, db2, HEAPSIZE ) != 0 )
	{
		rtlglue_printf( "db1(%p) db2(%p) is different.\n", db1, db2 );
		return FAILED;
	}
	return SUCCESS;
#endif

	return retval;
}



/***************************************************************************************************
 ***************************************************************************************************
 **	These functions will enumulate block alignment and length.                                    **
 **	Results are stored in either source block pointer/length or destination block pointer/length. **
 **	(4BA: 4 bytes aligned)                                                                        **
 **		pointer: 4BA, 		length: 4BA                                                           **
 **		pointer: 4BA+1,		length: 4BA                                                           **
 **		pointer: 4BA+2,		length: 4BA                                                           **
 **		pointer: 4BA+3,		length: 4BA                                                           **
 **	#block = { 1, 2, 3, 4, 5, 6, 7, 8 };                                                          **
 **	pointer = { 4BA+0, 4BA+1, 4BA+2, 4BA+3, 16BA+3, FIFOBoundary, Bank+1, ChipBoundary };         **
 **	length = { 4BA+0, 4BA+1, 4BA+2, 4BA+3, FIFOSize, BankSize,  };                                **
 ***************************************************************************************************
 ***************************************************************************************************/
/* block length combination:
 *              |   src>dst
 *              |  NA  |Allow
 *         -----+------+------
 *           NA |  ==  | S>D
 * dst<src -----+------+------
 *         Allow|  D<S | All
 */
/*
 *	Given 'seq' and 'totolLen', this function will return corresponding 'align' and 'length'.
 *	The current indexes of 'align' and 'length' are stored in seq[].
 */
int32 enumBAL( int32 maxBlockNum, int32 editBlock, int32 srcLen, int32 dstLen,
               int32* tsl, int32* tsa, int32* tdl, int32* tda, /* templates */
               int32 nsl, int32 nsa, int32 ndl, int32 nda, /* number of templates */
               int32* psl, int32* psa, int32* pdl, int32* pda, /* to be stored enumerated variables */
               CBBAL_T callback, uint32 flags )
{
	int sl, sa, dl, da; /* loop variables for srcLen/srcAlign/dstLen/dstAlign */
	int32 retval;

	if ( editBlock >= maxBlockNum ) /* no more deep */
		return SUCCESS;
	if ( ( !(flags&ENUMBAL_IGNORE_SRC) && srcLen<=0 ) ||
	     ( !(flags&ENUMBAL_IGNORE_DST) && dstLen<=0 ) )
	{
		/*rtlglue_printf( "srcLen=%d dstLen=%d\n", srcLen, dstLen );*/
		return SUCCESS;
	}

	if ( flags & ENUMBAL_IGNORE_SRC )
	{
		sl = 0;
		sa = 0;
		nsl = 0;
		nsa = 0;
		flags |= ENUMBAL_ALLOW_DgS;
		goto escape_src_loop;
	}
	if ( flags & ENUMBAL_FREE_SRC )
	{
		sl = 0;
		sa = 0;
		goto escape_src_loop;
	}
	for( sl = 0; sl < nsl; sl++ )
	{
		psl[editBlock] = tsl[sl];
		for( sa = 0; sa < nsa; sa++ )
		{
			psa[editBlock] = tsa[sa];
			/* to indicate no more block after */
			if ( editBlock+1 < maxBlockNum )
				psl[editBlock+1] = 0;
				
			if ( flags & ENUMBAL_IGNORE_DST )
			{
				dl = 0;
				da = 0;
				ndl = 0;
				nda = 0;
				flags |= ENUMBAL_ALLOW_SgD;
				goto escape_dst_loop;
			}
			
escape_src_loop:
			for( dl = 0; dl < ndl; dl++ )
			{
				pdl[editBlock] = tdl[dl];
				for( da = 0; da < nda; da++ )
				{
					int i, src, dst; /* for compute total block length */
					
escape_dst_loop:
					pda[editBlock] = tda[da];
					
					/* to indicate no more block after */
					if ( editBlock+1 < maxBlockNum )
						pdl[editBlock+1] = 0;

					for( i = 0, src = 0; i <= editBlock; i++ ) src += psl[i];
					for( i = 0, dst = 0; i <= editBlock; i++ ) dst += pdl[i];

					/* build source blocks according to destination blocks. */
					if ( flags & ENUMBAL_FREE_SRC )
					{
						int _src = dst;
						nsl = 0;
						nsa = 0;
						for( i = 0; i < 8; i++ )
						{
							if ( _src > MAX_BLOCK_LENGTH )
								psl[i] = MAX_BLOCK_LENGTH;
							else
								psl[i] = _src;
							_src -= psl[i];
							psa[i] = 0;
							nsl++;
							nsa++;
							if ( _src <= 0 ) break;
						}
						/* to indicate no more block after */
						if ( i != 8 )
							psl[i+1] = 0;

						src = dst;
					}

					/* before calling callback function, we must check if it fits criterions. */
					if ( ( src==dst ) ||
					     ( src>dst && (flags&ENUMBAL_ALLOW_SgD) ) ||
					     ( dst>src && (flags&ENUMBAL_ALLOW_DgS) ) )
					{
						/* source/destination block alignment and length have been ready, call the provided function to test. */
						if ( ( retval = callback( psl, psa, pdl, pda ) ) != SUCCESS )
							return retval;
					}

					/* recursive to enumerate next level */
					if ( ( editBlock+1 < maxBlockNum ) /* to speed up, we will check editBlock before calling self-recursive function. */ &&
					     ( retval = enumBAL( maxBlockNum, editBlock+1, srcLen-psl[editBlock], dstLen-pdl[editBlock], 
					                         tsl, tsa, tdl, tda, nsl, nsa, ndl, nda,
					                         psl, psa, pdl, pda, callback, flags ) ) != SUCCESS )
						return retval;
				}
			}
			
			if ( flags & ENUMBAL_FREE_SRC )
				goto escape_src_loop2;
		}
	}

escape_src_loop2: do {} while (0);

	return SUCCESS;
}


/*
 *	To test enumBAL().
 *
 *	Assume sl=4, sa=4, dl=4, da=4, and ENUMBAL_ALLOW_EQUAL. 
 *	Block number is changed from 1 to 8:
 *
 *    Total Loop   ALLOW_EQUAL     ALLOW_ALL
 *		Block No
 *		       1            64           256
 *		       2         11328         65792
 *		       3       2387008      16843008
 *		       4     532704320    4311810304
 *		       5  122486287424
 *		       6              
 *		       7              
 *		       8              
 *
 *	Total estimated time is about 7 minutes for block no 2 and ALLOW_EQUAL.
 */
int32 cbEnumBAL( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	DEBUG_BAL();
	
	return SUCCESS;
}
int32 testGdmaEnumBAL(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	static int32 tmplSrcLen[] = { 1, 2, 3, 4 };
	static int32 tmplSrcAlign[] = { 0, 1, 2, 3 };
	static int32 tmplDstLen[] = { 1, 2, 3, 4 };
	static int32 tmplDstAlign[] = { 0, 1, 2, 3 };
	int32 retval;

	testGdmaInit();

	memset( sa, 0, sizeof(sa) );
	memset( sl, 0, sizeof(sl) );
	memset( da, 0, sizeof(da) );
	memset( dl, 0, sizeof(dl) );

	retval = enumBAL( 2, 0, 99, 99, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
	                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
	                  sl, sa, dl, da, cbEnumBAL, ENUMBAL_ALLOW_EQUAL );
	//rtlglue_printf("enumbal_total_case_no=%llu\n", enumbal_total_case_no );
	rtlglue_printf("enumbal_total_case_no=%u\n", enumbal_total_case_no );
	return retval;
}


/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  Memory Functions                                                        **
 **                                                                          **
 ******************************************************************************
 ******************************************************************************/
/*
 *  +----------------+
 *  | AAAAAAAAAAAAAAA|  Block A/B/C/D is 64-bytes long and located at offset 1.
 *  |AAAAAAAAAAAAAAAA|  We will test 4 memory functions together, and alignment issue.
 *  |AAAAAAAAAAAAAAAA|  
 *  |AAAAAAAAAAAAAAAA|  0. init A with meaningful string
 *  |ABBBBBBBBBBBBBBB|  1. memcpy from A to B
 *  |BBBBBBBBBBBBBBBB|  2. memset C and D with 0xa5
 *  |BBBBBBBBBBBBBBBB|  3. memXor (A,B) and (C,D)
 *  |BBBBBBBBBBBBBBBB|  4. memcmp with C and D, and expected identical
 *  |BCCCCCCCCCCCCCCC|
 *  |CCCCCCCCCCCCCCCC|
 *  |CCCCCCCCCCCCCCCC|
 *  |CCCCCCCCCCCCCCCC|
 *  |CDDDDDDDDDDDDDDD|
 *  |DDDDDDDDDDDDDDDD|
 *  |DDDDDDDDDDDDDDDD|
 *  |DDDDDDDDDDDDDDDD|
 *  |D               |
 *  +----------------+
 */
#ifdef CONFIG_RTL0371S
#define TO_PHY_ADDR(addr) (((addr)&((unsigned int)0x0FFFFFFF)) + (unsigned int)(0x4000000))
#else
#define TO_PHY_ADDR(addr) (((addr)&((unsigned int)0x0FFFFFFF)))
#endif
int32 testGdmaMemory1(uint32 caseNo)
{
	int i, j;
	int32 retval;
	uint8 str[64] = "Hi, everybody. I am Louis. I come from Taiwan. Glad to meet you.";

	testGdmaInit();

	printf("GDAM_BASE(0x%08x)\n", GDMA_BASE);
	printf("Start %s: Memory Mix test. memcpy, memset, memXor and memcmp \n", __FUNCTION__);
	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		/* clean memory */
		clearGdmaBuffer(CLEAN_ALL);

		/* 0. init A with meaningful string */
		memcpy( sb2+1, str, sizeof(str) );

//		_cache_flush();

		/* 1. memcpy from A to B */
		WRITE_MEM32( GDMACNR, 0 ); /* Reset GDMA */
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMCPY );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)sb2+1+sizeof(str)*0) ); /* A */
		WRITE_MEM32( GDMASBL0, GDMA_LDB|sizeof(str) );
		WRITE_MEM32( GDMADBP0, TO_PHY_ADDR((uint32)sb2+1+sizeof(str)*1) ); /* B */
		WRITE_MEM32( GDMADBL0, GDMA_LDB|sizeof(str) );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMCPY|internal_gdmacnr );

		/* Expected data */
		memcpy( sb1+1+sizeof(str)*0, str, sizeof(str) );
		memcpy( sb1+1+sizeof(str)*1, str, sizeof(str) );
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

#if 0
		IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Source block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
		ASSERT( compare_data_block() == SUCCESS );
		
		/* 2. memset C and D with 0xa5 */
		WRITE_MEM32( GDMACNR, 0 ); /* Reset GDMA */
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMSET );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0xa5a5a5a5 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP1, 0 );
		WRITE_MEM32( GDMASBL1, GDMA_LDB );
		WRITE_MEM32( GDMADBP0, TO_PHY_ADDR((uint32)sb2+1+sizeof(str)*2)); /* C */
		WRITE_MEM32( GDMADBL0, sizeof(str) );
		WRITE_MEM32( GDMADBP1, TO_PHY_ADDR((uint32)sb2+1+sizeof(str)*3)); /* D */
		WRITE_MEM32( GDMADBL1, GDMA_LDB|sizeof(str) );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMSET|internal_gdmacnr );

		/* Expected data */
		memset( sb1+1+sizeof(str)*2, 0xa5, sizeof(str) );
		memset( sb1+1+sizeof(str)*3, 0xa5, sizeof(str) );
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

#if 0
		IS_EQUAL_INT("Dest block should under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, GDMA_DBUDRIP, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Source block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
		ASSERT( compare_data_block() == SUCCESS );
		
		/* 3. memXor (A,B) and (C,D) */
		WRITE_MEM32( GDMACNR, 0 ); /* Reset GDMA */
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMXOR );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)sb2+1+sizeof(str)*0)); /* A,B */
		WRITE_MEM32( GDMASBL0, GDMA_LDB|(sizeof(str)*2) );
		WRITE_MEM32( GDMADBP0, TO_PHY_ADDR((uint32)sb2+1+sizeof(str)*2) ); /* C,D */
		WRITE_MEM32( GDMADBL0, GDMA_LDB|(sizeof(str)*2) );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMXOR|internal_gdmacnr );

		/* Expected data */
		for( j = 0; j < sizeof(str)*2; j++ )
		{
			sb1[1+sizeof(str)*2+j] ^= sb1[1+sizeof(str)*0+j];
		}
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

#if 0
		IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Source block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
		ASSERT( compare_data_block() == SUCCESS );
		
		/* 4. memcmp with C and D */
		WRITE_MEM32( GDMACNR, 0 ); /* Reset GDMA */
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMCMP );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)sb2+1+sizeof(str)*2) ); /* C */
		WRITE_MEM32( GDMASBL0, GDMA_LDB|(sizeof(str)*1) );
		WRITE_MEM32( GDMADBP0, TO_PHY_ADDR((uint32)sb2+1+sizeof(str)*3) ); /* D */
		WRITE_MEM32( GDMADBL0, GDMA_LDB|(sizeof(str)*1) );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMCMP|internal_gdmacnr );
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		
#if 0
		IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Source block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
		IS_EQUAL_INT("C and D should be identical", READ_MEM32(GDMAICVL), 0, __FUNCTION__, __LINE__ );
		ASSERT( compare_data_block() == SUCCESS );
		
	}

	printf("\n");
	return SUCCESS;
}


/*
 *	Test memory-copy function
 *	Only the first block's address and length is unaligned.
 */
int32 testGdmaMemory2(uint32 caseNo)
{
	uint8 str[64] = "Hi, everybody. I am Louis. I come from Taiwan. Glad to meet you.";
	int32 retval;
	int i; /* i for ic type */
	int ns, nd, as, ad; /* n:length of first block, d:#block, m:length of last block */
	static int _as[] = { 0, 1, 2, 3 }; /* align address of first source block */
	static int _ad[] = { 0, 1, 2, 3 }; /* align address of first destination block */
	static int _ns[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }; /* length of first source block */
	static int _nd[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 }; /* length of first destination block */
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	
	testGdmaInit();

	printf("Start %s: Test memory-copy function. Only the first block's address and length is unaligned.\n", __FUNCTION__);
	for( ns = 0; ns < sizeof(_ns)/sizeof(_ns[0]); ns++ )
	{
		for( as = 0; as < sizeof(_as)/sizeof(_as[0]); as++ )
		{
			for( nd = 0; nd < sizeof(_nd)/sizeof(_nd[0]); nd++ )
			{
				for( ad = 0; ad < sizeof(_ad)/sizeof(_ad[0]); ad++ )
				{
					for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
					{
						retval = model_setTestTarget( _i[i] );
						if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

						printf("\r\\\\");
						/* Reset GDMA */
						WRITE_MEM32( GDMACNR, 0 );
						WRITE_MEM32( GDMACNR, GDMA_ENABLE );
							
						/* clean memory */
						clearGdmaBuffer(CLEAN_SB2|CLEAN_DB2);
						
						/* 0. init A with meaningful string */
						memcpy( sb2+_as[as], str, _ns[ns] );
						memcpy( sb2+256-_as[as], str+_ns[ns], sizeof(str)-_ns[ns] );

//						_cache_flush();

						/* 1. memcpy from A to B */
						WRITE_MEM32( GDMACNR, 0 ); /* Reset GDMA */
						WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMCPY );
						WRITE_MEM32( GDMAIMR, 0 );
						WRITE_MEM32( GDMAISR, 0xffffffff );
						WRITE_MEM32( GDMAICVL, 0 );
						WRITE_MEM32( GDMAICVR, 0 );
						WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)sb2+_as[as]));
						WRITE_MEM32( GDMASBL0, _ns[ns] );
						WRITE_MEM32( GDMASBP1, TO_PHY_ADDR((uint32)sb2+256-_as[as]));
						WRITE_MEM32( GDMASBL1, GDMA_LDB|(sizeof(str)-_ns[ns]) );
						WRITE_MEM32( GDMADBP0, TO_PHY_ADDR((uint32)db2+_ad[ad]));
						WRITE_MEM32( GDMADBL0, _nd[nd] );
						WRITE_MEM32( GDMADBP1, TO_PHY_ADDR((uint32)db2+256-_as[as]));
						WRITE_MEM32( GDMADBL1, GDMA_LDB|(sizeof(str)-_nd[nd]) );
						WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMCPY|internal_gdmacnr );


						/* Expected data */
						memcpy( sb1, sb2, HEAPSIZE );
						memcpy( db1, db2, HEAPSIZE );
						while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
						MEMCTL_DEBUG_PRINTF("\r//");

#if 0
						IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
						IS_EQUAL_INT("Source block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
						ASSERT( compare_data_block() == SUCCESS );
						
					}					
				}
			}
		}
	}
	printf("\n");
	return SUCCESS;
}


/*
 *	memory copy: however, the first source block and the second source block might be overlapped.
 *	The first destination block and the second destination block also might be overlapped.
 */
int32 cbMemory3( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	uint8 str[64] = "Hi, everybody. I am Louis. I come from Taiwan. Glad to meet you.";
	uint8 *pch1, *pch2;

	DEBUG_BAL();
	clearGdmaBuffer(CLEAN_ALL);
	testGdmaOptimize( OPT_CACHED1, TRUE );
	
	for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
	{
		retval = model_setTestTarget( _i[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		printf("\r\\\\");

		/* Reset GDMA */
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			
		/* source block */							
		pch1 = str;
		for( j = 0; j < 8; j++ )
		{
			pch2 = enumbuf[i].src+sa[j];
			memcpy( pch2, pch1, sl[j] );
			pch1 += sl[j];
			WRITE_MEM32( GDMASBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			if ( j==7 || sl[j+1]==0 )
			{
				WRITE_MEM32( GDMASBL0+j*8, GDMA_LDB|(sl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMASBL0+j*8, sl[j] );
			}
		}

		/* destination block */							
		for( j = 0; j < 8; j++ )
		{
			pch2 = enumbuf[i].dst+da[j];
			WRITE_MEM32( GDMADBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			if ( j==7 || dl[j+1]==0)
			{
				WRITE_MEM32( GDMADBL0+j*8, GDMA_LDB|(dl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMADBL0+j*8, dl[j] );
			}
		}

		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMCPY );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMCPY|internal_gdmacnr);

		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		MEMCTL_DEBUG_PRINTF("\r//");
	}

	ASSERT( compare_data_block() == SUCCESS );

	return SUCCESS;
}
int32 testGdmaMemory3(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	static int32 tmplSrcLen[] = { 1, 2, 3, 4 };
	static int32 tmplSrcAlign[] = { 0, 1, 2, 3 };
	static int32 tmplDstLen[] = { 1, 2, 3, 4 };
	static int32 tmplDstAlign[] = { 0, 1, 2, 3 };

	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	printf("Start %s: memory copy: however, the first source block and the second source block might be overlapped.\n", __FUNCTION__);

	memset( sa, 0, sizeof(sa) );
	memset( sl, 0, sizeof(sl) );
	memset( da, 0, sizeof(da) );
	memset( dl, 0, sizeof(dl) );

	return enumBAL( 2, 0, 99, 99, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
	                sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
	                sl, sa, dl, da, cbMemory3, ENUMBAL_ALLOW_EQUAL );
}


/*
 *	memory copy: copy from cbMemory3(): however, source and destination blocks are not overlapped, but continuous.
 *	In some cases, there exists some gaps between blocks (by 'align' field).
 */
int32 cbMemory4( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	uint8 str[64] = "Hi, everybody. I am Louis. I come from Taiwan. Glad to meet you.";
	uint8 *pch1, *pch2;

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif
	DEBUG_BAL();

	clearGdmaBuffer(CLEAN_ALL);
	for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
	{
		retval = model_setTestTarget( _i[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		printf("\r\\\\");

		/* Reset GDMA */
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			
		/* source block */							
		pch1 = str; /* pointer to source text */
		pch2 = enumbuf[i].src; /* pointer to source data block */
		for( j = 0; j < 8; j++ )
		{
			pch2 += sa[j];
			memcpy( pch2, pch1, sl[j] );
			WRITE_MEM32( GDMASBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch1 += sl[j];
			pch2 += sl[j];
			if ( j==7 || sl[j+1]==0 )
			{
				WRITE_MEM32( GDMASBL0+j*8, GDMA_LDB|(sl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMASBL0+j*8, sl[j] );
			}
		}

		pch2 = enumbuf[i].dst; /* pointer to source data block */
		/* destination block */							
		for( j = 0; j < 8; j++ )
		{
			pch2 += da[j];
			WRITE_MEM32( GDMADBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch2 += dl[j];
			if ( j==7 || dl[j+1]==0)
			{
				WRITE_MEM32( GDMADBL0+j*8, GDMA_LDB|(dl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMADBL0+j*8, dl[j] );
			}
		}

		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMCPY );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMCPY|internal_gdmacnr );

		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		MEMCTL_DEBUG_PRINTF("\r//");
	}

	ASSERT( compare_data_block() == SUCCESS );

//	printf("\n");
	return SUCCESS;
}
int32 testGdmaMemory4(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	static int32 tmplSrcLen[] = { 1, 2, 3, 4, 5, 12, 16, 17, 33 };
	static int32 tmplSrcAlign[] = { 0, 1, 2, 3 };
	static int32 tmplDstLen[] = { 1, 2, 3, 4, 5, 12, 16, 17, 33 };
	static int32 tmplDstAlign[] = { 0, 1, 2, 3 };

	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	printf("Start %s:memory copy: copy from cbMemory3(): however, source and destination blocks are not overlapped, but continuous. \n", __FUNCTION__);

	memset( sa, 0, sizeof(sa) );
	memset( sl, 0, sizeof(sl) );
	memset( da, 0, sizeof(da) );
	memset( dl, 0, sizeof(dl) );

	return enumBAL( 2, 0, 99, 99, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
	                sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
	                sl, sa, dl, da, cbMemory4, ENUMBAL_ALLOW_EQUAL );
}


/*
 *	memory set: copy from cbMemory4(): however, source blocks are not overlapped, but continuous.
 *	In some cases, there exists some gaps between blocks (by 'align' field).
 */
int32 cbMemory5( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	uint8 *pch2;
	uint32 pat;

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif
	DEBUG_BAL();

	pat = rtlglue_random();

	for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
	{
		retval = model_setTestTarget( _i[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		printf("\r\\\\");
		/* Reset GDMA */
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			

		/* source block */							
		for( j = 0; j < 8; j++ )
		{
			WRITE_MEM32( GDMASBP0+j*8, TO_PHY_ADDR(0x00000000) );
			WRITE_MEM32( GDMASBL0+j*8, 0x00000000 );
		}

		/* Destination Block */
		pch2 = enumbuf[i].dst; /* pointer to source data block */
		for( j = 0; j < 8; j++ )
		{
			pch2 += da[j];
			WRITE_MEM32( GDMADBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch2 += dl[j];
			if ( j==7 || dl[j+1]==0 )
			{
				WRITE_MEM32( GDMADBL0+j*8, GDMA_LDB|(dl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMADBL0+j*8, dl[j] );
			}
		}
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, pat );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMSET|internal_gdmacnr );

		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		MEMCTL_DEBUG_PRINTF("\r//");
	}

	ASSERT( compare_data_block() == SUCCESS );

	return SUCCESS;
}
int32 testGdmaMemory5(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	int32 retval;

	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	clearGdmaBuffer(CLEAN_ALL);
	printf("Start %s: memory set: copy from cbMemory4(): however, source blocks are not overlapped, but continuous.\n", __FUNCTION__);

	{
		static int32 tmplSrcLen[] = { 0 };
		static int32 tmplSrcAlign[] = { 0 };
		static int32 tmplDstLen[] = { 1, 2, 3, 4, 5, 12, 16, 17, 33 };
		static int32 tmplDstAlign[] = { 0, 1, 2, 3 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 2, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory5, ENUMBAL_IGNORE_SRC );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 0 };
		static int32 tmplSrcAlign[] = { 0 };
		static int32 tmplDstLen[] = { 15, 31, 63, 127 };
		static int32 tmplDstAlign[] = { 0, 1, 2, 3 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 4, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory5, ENUMBAL_IGNORE_SRC );
		if ( retval != SUCCESS ) return retval;
	}

	printf("\n");
	return SUCCESS;
}


/*
 *	memory XOR: copy from cbMemory4(): however, source and destination blocks are not overlapped, but continuous.
 *	In some cases, there exists some gaps between blocks (by 'align' field).
 */
int32 cbMemory6( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	static uint8 str[512];
	uint8 *pch1, *pch2;
	uint32 pat;

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif
	DEBUG_BAL();

	pat = rtlglue_random();
	for( j = 0; j < sizeof(str); j++ )
		str[j] = rtlglue_random() >> 8; /* we use bit [15:8]. */
	
	/* clean memory */
	clearGdmaBuffer(CLEAN_ALL);

	for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
	{
		retval = model_setTestTarget( _i[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		printf("\r\\\\");
		/* Reset GDMA */
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			
		/* source block */							
		pch1 = str; /* pointer to source text */
		pch2 = enumbuf[i].src; /* pointer to source data block */
		for( j = 0; j < 8; j++ )
		{
			pch2 += sa[j];
			memcpy( pch2, pch1, sl[j] );
			WRITE_MEM32( GDMASBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch1 += sl[j];
			pch2 += sl[j];
			if ( j==7 || sl[j+1]==0 )
			{
				WRITE_MEM32( GDMASBL0+j*8, GDMA_LDB|(sl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMASBL0+j*8, sl[j] );
			}
		}

		pch2 = enumbuf[i].dst; /* pointer to source data block */
		/* destination block */							
		for( j = 0; j < 8; j++ )
		{
			pch2 += da[j];
			memset( pch2, pat, sl[j] );
			WRITE_MEM32( GDMADBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch2 += dl[j];
			if ( j==7 || dl[j+1]==0)
			{
				WRITE_MEM32( GDMADBL0+j*8, GDMA_LDB|(dl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMADBL0+j*8, dl[j] );
			}
		}
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMXOR);
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMXOR|internal_gdmacnr );

		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		MEMCTL_DEBUG_PRINTF("\r//");
	}

	ASSERT( compare_data_block() == SUCCESS );

	printf("\n");
	return SUCCESS;
}
int32 testGdmaMemory6(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	static int32 tmplSrcLen[] = { 1, 2, 3, 4, 5, 12, 16, 17, 33, 129 };
	static int32 tmplSrcAlign[] = { 0, 1, 2, 3 };
	static int32 tmplDstLen[] = { 1, 2, 3, 4, 5, 12, 16, 17, 33, 129 };
	static int32 tmplDstAlign[] = { 0, 1, 2, 3 };

	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	printf("Start %s:memory XOR: copy from cbMemory4(): however, source and destination blocks are not overlapped, but continuous. \n", __FUNCTION__);

	memset( sa, 0, sizeof(sa) );
	memset( sl, 0, sizeof(sl) );
	memset( da, 0, sizeof(da) );
	memset( dl, 0, sizeof(dl) );

	return enumBAL( 2, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
	                sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
	                sl, sa, dl, da, cbMemory6, ENUMBAL_ALLOW_EQUAL );
}


/*
 *	memory compare: copy from cbMemory6(): however, source and destination blocks are not overlapped, but continuous.
 *	In some cases, there exists some gaps between blocks (by 'align' field).
 */
static uint32 memory7matchNo;
int32 cbMemory7( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	static uint32 str1[32];
	static uint32 str2[32];
	uint8 *pch1, *pch2;
	uint32 result[2];

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif
	DEBUG_BAL();

	if ( rtlglue_random() & 0x00110000 )
	{
		/* 75% cases will result not the same. */
		for( j = 0; j < sizeof(str1)/sizeof(str1[0]); j++ )
		{
			str1[j] = rtlglue_random();
			str2[j] = rtlglue_random();
		}
	}
	else
	{
		/* 25% cases will result identical. */
		for( j = 0; j < sizeof(str1)/sizeof(str1[0]); j++ )
		{
			str1[j] = rtlglue_random();
			str2[j] = str1[j];
		}
	}

	clearGdmaBuffer(CLEAN_ALL);
	for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
	{
		retval = model_setTestTarget( _i[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		printf("\r\\\\");
		/* Reset GDMA */
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			
		/* source block */							
		pch1 = (uint8*)str1; /* pointer to source text */
		pch2 = enumbuf[i].src; /* pointer to source data block */
		for( j = 0; j < 8; j++ )
		{
			pch2 += sa[j];
			memcpy( pch2, pch1, sl[j] );
			WRITE_MEM32( GDMASBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch1 += sl[j];
			pch2 += sl[j];
			if ( j==7 || sl[j+1]==0 )
			{
				WRITE_MEM32( GDMASBL0+j*8, GDMA_LDB|(sl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMASBL0+j*8, sl[j] );
			}
		}

		pch1 = (uint8*)str2; /* pointer to source text */
		pch2 = enumbuf[i].dst; /* pointer to source data block */
		/* destination block */							
		for( j = 0; j < 8; j++ )
		{
			pch2 += da[j];
			memcpy( pch2, pch1, dl[j] );
			WRITE_MEM32( GDMADBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch1 += dl[j];
			pch2 += dl[j];
			if ( j==7 || dl[j+1]==0)
			{
				WRITE_MEM32( GDMADBL0+j*8, GDMA_LDB|(dl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMADBL0+j*8, dl[j] );
			}
		}

		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMCMP);
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMCMP|internal_gdmacnr );

		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		MEMCTL_DEBUG_PRINTF("\r//");

		result[i] = READ_MEM32( GDMAICVL );
		if ( result[i] == 0x00000000 ) memory7matchNo++;
	}

	IS_EQUAL_INT( "Compare result is different. {Model,Real}=", result[0], result[1], __FUNCTION__, __LINE__ );

	return SUCCESS;
}
int32 testGdmaMemory7(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	static int32 tmplSrcLen[] = { 1, 2, 3, 4, 5, 12, 16, 17, 33 };
	static int32 tmplSrcAlign[] = { 0, 1, 2, 3 };
	static int32 tmplDstLen[] = { 1, 2, 3, 4, 5, 12, 16, 17, 33 };
	static int32 tmplDstAlign[] = { 0, 1, 2, 3 };
	int32 retval;

	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	printf("Start %s:memory compare: copy from cbMemory6(): however, source and destination blocks are not overlapped, but continuous. \n", __FUNCTION__);

	memset( sa, 0, sizeof(sa) );
	memset( sl, 0, sizeof(sl) );
	memset( da, 0, sizeof(da) );
	memset( dl, 0, sizeof(dl) );
	memory7matchNo = 0;
	retval = enumBAL( 2, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
	                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
	                  sl, sa, dl, da, cbMemory7, ENUMBAL_ALLOW_EQUAL );
	rtlglue_printf( "memory7matchNo=%d\n", memory7matchNo );
	return retval;
}


/*
 *	memory copy: copy from cbMemory4(): however, source and destination blocks are not overlapped, but continuous.
 *	In some cases, there exists some gaps between blocks (by 'align' field).
 */
int32 cbMemory8( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	uint8 str[64] = "Hi, everybody. I am Louis. I come from Taiwan. Glad to meet you.";
	uint8 *pch1, *pch2;

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif
	DEBUG_BAL();

	clearGdmaBuffer(CLEAN_ALL);
	for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
	{
		retval = model_setTestTarget( _i[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		printf("\r\\\\");
		/* Reset GDMA */
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			
		/* source block */							
		pch1 = str; /* pointer to source text */
		pch2 = enumbuf[i].src; /* pointer to source data block */
		for( j = 0; j < 8; j++ )
		{
			pch2 += sa[j];
			memcpy( pch2, pch1, sl[j] );
			WRITE_MEM32( GDMASBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch1 += sl[j];
			pch2 += sl[j];
			if ( j==7 || sl[j+1]==0 )
			{
				WRITE_MEM32( GDMASBL0+j*8, GDMA_LDB|(sl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMASBL0+j*8, sl[j] );
			}
		}

		pch2 = enumbuf[i].dst; /* pointer to source data block */
		/* destination block */							
		for( j = 0; j < 8; j++ )
		{
			pch2 += da[j];
			WRITE_MEM32( GDMADBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch2 += dl[j];
			if ( j==7 || dl[j+1]==0)
			{
				WRITE_MEM32( GDMADBL0+j*8, GDMA_LDB|(dl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMADBL0+j*8, dl[j] );
			}
		}

		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMCPY );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMCPY|internal_gdmacnr );

		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		MEMCTL_DEBUG_PRINTF("\r//");
	}

	ASSERT( compare_data_block() == SUCCESS );

	return SUCCESS;
}
int32 testGdmaMemory8(uint32 caseNo)
{
	int32 retval;
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	
	printf("Start %s: memory copy: copy from cbMemory4(): however, source and destination blocks are not overlapped, but continuous.\n", __FUNCTION__);
	{
		static int32 tplSL[] = { 1, 2 };
		static int32 tplSA[] = { 1 };
		static int32 tplDL[] = { 1, 2 };
		static int32 tplDA[] = { 1 };

		testGdmaInit();
		testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 8, 0, 99, 99, tplSL, tplSA, tplDL, tplDA, 
		                  sizeof(tplSL)/sizeof(tplSL[0]), sizeof(tplSA)/sizeof(tplSA[0]), sizeof(tplDL)/sizeof(tplDL[0]), sizeof(tplDA)/sizeof(tplDA[0]),
		                  sl, sa, dl, da, cbMemory8, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}

	{
		static int32 tplSL[] = { 1, 3 };
		static int32 tplSA[] = { 2 };
		static int32 tplDL[] = { 1, 3 };
		static int32 tplDA[] = { 2 };

		testGdmaInit();
		testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 8, 0, 99, 99, tplSL, tplSA, tplDL, tplDA, 
		                  sizeof(tplSL)/sizeof(tplSL[0]), sizeof(tplSA)/sizeof(tplSA[0]), sizeof(tplDL)/sizeof(tplDL[0]), sizeof(tplDA)/sizeof(tplDA[0]),
		                  sl, sa, dl, da, cbMemory8, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}

	{
		static int32 tplSL[] = { 1, 3 };
		static int32 tplSA[] = { 3 };
		static int32 tplDL[] = { 1, 3 };
		static int32 tplDA[] = { 3 };

		testGdmaInit();
		testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 8, 0, 99, 99, tplSL, tplSA, tplDL, tplDA, 
		                  sizeof(tplSL)/sizeof(tplSL[0]), sizeof(tplSA)/sizeof(tplSA[0]), sizeof(tplDL)/sizeof(tplDL[0]), sizeof(tplDA)/sizeof(tplDA[0]),
		                  sl, sa, dl, da, cbMemory8, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}

	{
		static int32 tplSL[] = { 1, 4 };
		static int32 tplSA[] = { 0 };
		static int32 tplDL[] = { 1, 4 };
		static int32 tplDA[] = { 1 };

		testGdmaInit();
		testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 8, 0, 99, 99, tplSL, tplSA, tplDL, tplDA, 
		                  sizeof(tplSL)/sizeof(tplSL[0]), sizeof(tplSA)/sizeof(tplSA[0]), sizeof(tplDL)/sizeof(tplDL[0]), sizeof(tplDA)/sizeof(tplDA[0]),
		                  sl, sa, dl, da, cbMemory8, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}

	printf("\n");
	return SUCCESS;
}


/*
 *	memory XOR: copy from cbMemory6(): for the bug of dst 1:0 4:1
 */
int32 cbMemory9( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	static uint8 str1[8192];
	static uint8 str2[8192];
	uint8 *pch1, *pch2;
	uint32 pat;

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif
	DEBUG_BAL();

	pat = rtlglue_random();
	for( j = 0; j < sizeof(str1); j++ )
		str1[j] = rtlglue_random() >> 8; /* we use bit [15:8]. */
	for( j = 0; j < sizeof(str2); j++ )
		str2[j] = rtlglue_random() >> 8; /* we use bit [15:8]. */
	
	/* clean memory */
	clearGdmaBuffer(CLEAN_ALL);

	for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
	{
		retval = model_setTestTarget( _i[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		printf("\r\\\\");
		/* Reset GDMA */
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			

		/* source block */							
		pch1 = str1; /* pointer to source text */
		pch2 = enumbuf[i].src; /* pointer to source data block */
		for( j = 0; j < 8; j++ )
		{
			pch2 += sa[j];
			memcpy( pch2, pch1, sl[j] );
			WRITE_MEM32( GDMASBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch1 += sl[j];
			pch2 += sl[j];
			if ( j==7 || sl[j+1]==0 )
			{
				WRITE_MEM32( GDMASBL0+j*8, GDMA_LDB|(sl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMASBL0+j*8, sl[j] );
			}
		}

		pch1 = str2;
		pch2 = enumbuf[i].dst; /* pointer to source data block */
		/* destination block */							
		for( j = 0; j < 8; j++ )
		{
			pch2 += da[j];
#if 1
			memcpy( pch2, pch1, dl[j] );
#else
			memset( pch2, pat, dl[j] );
#endif
			WRITE_MEM32( GDMADBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch1 += dl[j];
			pch2 += dl[j];
			if ( j==7 || dl[j+1]==0)
			{
				WRITE_MEM32( GDMADBL0+j*8, GDMA_LDB|(dl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMADBL0+j*8, dl[j] );
			}
		}

		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMXOR);
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMXOR|internal_gdmacnr );

		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		MEMCTL_DEBUG_PRINTF("\r//");
	}

	ASSERT( compare_data_block() == SUCCESS );

	return SUCCESS;
}
int32 testGdmaMemory9(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	int32 retval;

	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	printf("Start %s:memory XOR: copy from cbMemory6(): for the bug of dst 1:0 4:1 \n", __FUNCTION__);

	{
		static int32 tmplSrcLen[] = { 1, 4 };
		static int32 tmplSrcAlign[] = { 0 };
		static int32 tmplDstLen[] = { 1, 4 };
		static int32 tmplDstAlign[] = { 0 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 8, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 4 };
		static int32 tmplSrcAlign[] = { 0 };
		static int32 tmplDstLen[] = { 1, 4 };
		static int32 tmplDstAlign[] = { 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 8, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 4 };
		static int32 tmplSrcAlign[] = { 1 };
		static int32 tmplDstLen[] = { 1, 4 };
		static int32 tmplDstAlign[] = { 0 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 8, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 4 };
		static int32 tmplSrcAlign[] = { 1 };
		static int32 tmplDstLen[] = { 1, 4 };
		static int32 tmplDstAlign[] = { 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 8, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 7 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 7 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 8 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 8 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 15 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 15 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 16 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 16 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 31 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 31 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 32 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 32 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 33 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 33 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 63 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 63 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 64 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 64 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 65 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 65 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 127 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 127 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 128 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 128 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 129 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 129 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 2, 130 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 2, 130 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 255 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 255 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 256 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 256 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 257 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 257 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 511 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 511 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 512 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 512 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 1023 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 1023 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 1024 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 1024 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 2047 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 2047 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 2048 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 2048 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 4095 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 4095 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 99999, 99999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 4096 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 4096 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 99999, 99999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 8190 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 1, 8190 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 99999, 99999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { -1 };
		static int32 tmplSrcAlign[] = { -1 };
		static int32 tmplDstLen[] = { 1, 8191 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 99999, 99999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL);
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 2, 62 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 2, 62 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 2, 62 };
		static int32 tmplSrcAlign[] = { 2, 3 };
		static int32 tmplDstLen[] = { 2, 62 };
		static int32 tmplDstAlign[] = { 2, 3 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 2, 63 };
		static int32 tmplSrcAlign[] = { 0, 1 };
		static int32 tmplDstLen[] = { 2, 63 };
		static int32 tmplDstAlign[] = { 0, 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );
		retval = enumBAL( 3, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory9, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}

	printf("\n");
	return SUCCESS;
}


/*
 *	memory copy: copy from cbMemory4(): random data and depth 3
 */
int32 cbMemory10( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	static uint32 str[16];
	uint8 *pch1, *pch2;

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif
	DEBUG_BAL();

	for( j = 0; j < sizeof(str)/sizeof(str[0]); j++ )
	{
		str[j] = rtlglue_random();
	}

	clearGdmaBuffer(CLEAN_ALL);
	for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
	{
		retval = model_setTestTarget( _i[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		printf("\r\\\\");
		/* Reset GDMA */
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			
		/* source block */							
		pch1 = (uint8*)str; /* pointer to source text */
		pch2 = enumbuf[i].src; /* pointer to source data block */
		for( j = 0; j < 8; j++ )
		{
			pch2 += sa[j];
			memcpy( pch2, pch1, sl[j] );
			WRITE_MEM32( GDMASBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch1 += sl[j];
			pch2 += sl[j];
			if ( j==7 || sl[j+1]==0 )
			{
				WRITE_MEM32( GDMASBL0+j*8, GDMA_LDB|(sl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMASBL0+j*8, sl[j] );
			}
		}

		pch2 = enumbuf[i].dst; /* pointer to source data block */
		/* destination block */							
		for( j = 0; j < 8; j++ )
		{
			pch2 += da[j];
			WRITE_MEM32( GDMADBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch2 += dl[j];
			if ( j==7 || dl[j+1]==0)
			{
				WRITE_MEM32( GDMADBL0+j*8, GDMA_LDB|(dl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMADBL0+j*8, dl[j] );
			}
		}

		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMCPY );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMCPY|internal_gdmacnr );

		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		MEMCTL_DEBUG_PRINTF("\r//");
	}

	ASSERT( compare_data_block() == SUCCESS );

	return SUCCESS;
}
int32 testGdmaMemory10(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	int32 retval;

	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	printf("Start %s:memory copy: copy from cbMemory4(): random data and depth 3 \n", __FUNCTION__);

	{
		static int32 tmplSrcLen[] = { 1, 2 };
		static int32 tmplSrcAlign[] = { 1 };
		static int32 tmplDstLen[] = { 1, 2 };
		static int32 tmplDstAlign[] = { 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 4, 0, 99, 99, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory10, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 2 };
		static int32 tmplSrcAlign[] = { 0 };
		static int32 tmplDstLen[] = { 1, 2 };
		static int32 tmplDstAlign[] = { 0 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 4, 0, 99, 99, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory10, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 2 };
		static int32 tmplSrcAlign[] = { 1 };
		static int32 tmplDstLen[] = { 1, 2 };
		static int32 tmplDstAlign[] = { 0 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 4, 0, 99, 99, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory10, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 2 };
		static int32 tmplSrcAlign[] = { 0 };
		static int32 tmplDstLen[] = { 1, 2 };
		static int32 tmplDstAlign[] = { 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 4, 0, 99, 99, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory10, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 2, 3 };
		static int32 tmplSrcAlign[] = { 0 };
		static int32 tmplDstLen[] = { 2, 3 };
		static int32 tmplDstAlign[] = { 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 4, 0, 99, 99, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory10, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 3, 4 };
		static int32 tmplSrcAlign[] = { 3 };
		static int32 tmplDstLen[] = { 2, 3 };
		static int32 tmplDstAlign[] = { 1 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 4, 0, 99, 99, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory10, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 4 };
		static int32 tmplSrcAlign[] = { 2 };
		static int32 tmplDstLen[] = { 3, 4 };
		static int32 tmplDstAlign[] = { 3 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 4, 0, 99, 99, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory10, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}

	printf("\n");
	return SUCCESS;
}


/*
 *	memory copy: copy from cbMemory10(): but this is large data version.
 */
int32 cbMemory11( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	static uint32 str[8192+32];
	uint8 *pch1, *pch2;

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif
	DEBUG_BAL();

	for( j = 0; j < sizeof(str)/sizeof(str[0]); j++ )
	{
		str[j] = rtlglue_random();
	}

	clearGdmaBuffer(CLEAN_ALL);
	for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
	{
		retval = model_setTestTarget( _i[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		printf("\r\\");
		/* Reset GDMA */
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			
		/* source block */							
		pch1 = (uint8*)str; /* pointer to source text */
		pch2 = enumbuf[i].src; /* pointer to source data block */
		for( j = 0; j < 8; j++ )
		{
			pch2 += sa[j];
			memcpy( pch2, pch1, sl[j] );
			WRITE_MEM32( GDMASBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch1 += sl[j];
			pch2 += sl[j];
			if ( j==7 || sl[j+1]==0 )
			{
				WRITE_MEM32( GDMASBL0+j*8, GDMA_LDB|(sl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMASBL0+j*8, sl[j] );
			}
		}

		pch2 = enumbuf[i].dst; /* pointer to source data block */
		/* destination block */							
		for( j = 0; j < 8; j++ )
		{
			pch2 += da[j];
			WRITE_MEM32( GDMADBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			pch2 += dl[j];
			if ( j==7 || dl[j+1]==0)
			{
				WRITE_MEM32( GDMADBL0+j*8, GDMA_LDB|(dl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMADBL0+j*8, dl[j] );
			}
		}

		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMCPY );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMCPY|internal_gdmacnr );

		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		MEMCTL_DEBUG_PRINTF("\r//");
	}

	ASSERT( compare_data_block() == SUCCESS );

	return SUCCESS;
}
int32 testGdmaMemory11(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	int32 retval;

	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	printf("Start %s:memory copy: copy from cbMemory10(): but this is large data version. \n", __FUNCTION__);

	{
		static int32 tmplSrcLen[] = { 128, 256, 384, 512 };
		static int32 tmplSrcAlign[] = { 1 };
		static int32 tmplDstLen[] = { 128, 256, 384, 512 };
		static int32 tmplDstAlign[] = { 2 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 4, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory11, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 512, 1024, 2048, 4096 };
		static int32 tmplSrcAlign[] = { 2 };
		static int32 tmplDstLen[] = { 512, 1024, 2048, 4096 };
		static int32 tmplDstAlign[] = { 4 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 4, 0, 9999, 9999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory11, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 1, 8190, 8191 };
		static int32 tmplSrcAlign[] = { 3 };
		static int32 tmplDstLen[] = { 1, 8190, 8191 };
		static int32 tmplDstAlign[] = { 0 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 4, 0, 99999, 99999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory11, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}
	{
		static int32 tmplSrcLen[] = { 2, 8189, 8191 };
		static int32 tmplSrcAlign[] = { 3 };
		static int32 tmplDstLen[] = { 2, 8189, 8191 };
		static int32 tmplDstAlign[] = { 0 };
		
		memset( sa, 0, sizeof(sa) );
		memset( sl, 0, sizeof(sl) );
		memset( da, 0, sizeof(da) );
		memset( dl, 0, sizeof(dl) );

		retval = enumBAL( 4, 0, 99999, 99999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
		                  sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
		                  sl, sa, dl, da, cbMemory11, ENUMBAL_ALLOW_EQUAL );
		if ( retval != SUCCESS ) return retval;
	}

	printf("\n");
	return SUCCESS;
}


/*
 *	memory XOR: copy from cbMemory9(): to garentee the first block only is bug free for any length and any alignment.
 */
int32 testGdmaMemory12(uint32 caseNo)
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	int len, sa, da;
	
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif
	printf("Start %s: memory XOR copy from cbMemory9(): to garentee the first block only is bug free for any length and any alignment.\n", __FUNCTION__);
	for( len = 1; len <= MAX_BLOCK_LENGTH; len++ )
	{
		for( sa = 0; sa <= 3; sa++ )
		{
			for( da = 0; da <= 3; da++ )
			{
				/*rtlglue_printf("len[%d] sa[%d] da[%d]\n", len, sa, da );*/
				for( j = 0; j < len; j++ )
				{
					enumbuf[0].src[j+sa] = enumbuf[1].src[j+sa] = rtlglue_random();
					enumbuf[0].dst[j+da] = enumbuf[1].dst[j+da] = rtlglue_random();
				}
				
				for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
				{
					retval = model_setTestTarget( _i[i] );
					if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

					printf("\r\\\\");
					/* Reset GDMA */
					WRITE_MEM32( GDMACNR, 0 );
					WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)(&enumbuf[i].src[sa])) );
					WRITE_MEM32( GDMASBL0, (1<<31)|(len) );
					WRITE_MEM32( GDMADBP0, TO_PHY_ADDR((uint32)(&enumbuf[i].dst[da])) );
					WRITE_MEM32( GDMADBL0, (1<<31)|(len) );
					WRITE_MEM32( GDMAIMR, 0 );
					WRITE_MEM32( GDMAISR, 0xffffffff );
					WRITE_MEM32( GDMAICVL, 0 );
					WRITE_MEM32( GDMAICVR, 0 );
					WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMXOR|internal_gdmacnr );

					while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
					MEMCTL_DEBUG_PRINTF("\r//");
				}

				ASSERT( compare_data_block() == SUCCESS );
			}
		}
	}
	
	printf("\n");
	return SUCCESS;
}


/*
 *	memory Copy: copy from testGdmaMemory12(): to garentee the first block only is bug free for any length and any alignment.
 */
int32 testGdmaMemory13(uint32 caseNo)
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	int len, sa, da;
	
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif
	printf("Start %s: memory Copy: copy from testGdmaMemory12(): to garentee the first block only is bug free for any length and any alignment.\n", __FUNCTION__);

	for( len = 1; len <= MAX_BLOCK_LENGTH; len++ )
	{
		for( sa = 0; sa <= 3; sa++ )
		{
			for( da = 0; da <= 3; da++ )
			{
				/*rtlglue_printf("len[%d] sa[%d] da[%d]\n", len, sa, da );*/
				for( j = 0; j < len; j++ )
				{
					enumbuf[0].src[j+sa] = enumbuf[1].src[j+sa] = rtlglue_random();
					/*enumbuf[0].dst[j+da] = enumbuf[1].dst[j+da] = rtlglue_random();*/
				}
				
				for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
				{
					retval = model_setTestTarget( _i[i] );
					if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

					printf("\r\\\\");
					/* Reset GDMA */
					WRITE_MEM32( GDMACNR, 0 );
					WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)(&enumbuf[i].src[sa])) );
					WRITE_MEM32( GDMASBL0, (1<<31)|(len) );
					WRITE_MEM32( GDMADBP0, TO_PHY_ADDR((uint32)(&enumbuf[i].dst[da])) );
					WRITE_MEM32( GDMADBL0, (1<<31)|(len) );
					WRITE_MEM32( GDMAIMR, 0 );
					WRITE_MEM32( GDMAISR, 0xffffffff );
					WRITE_MEM32( GDMAICVL, 0 );
					WRITE_MEM32( GDMAICVR, 0 );
					WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMCPY|internal_gdmacnr );

					while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
					MEMCTL_DEBUG_PRINTF("\r//");
				}

				ASSERT( compare_data_block() == SUCCESS );
			}
		}
	}
	
	printf("\n");
	return SUCCESS;
}


/*
 *	memory Set: copy from testGdmaMemory12(): to garentee the first block only is bug free for any length and any alignment.
 */
int32 testGdmaMemory14(uint32 caseNo)
{
	int32 retval;
	int i;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	int len, sa, da;
	
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	printf("Start %s: memory Set: copy from testGdmaMemory12(): to garentee the first block only is bug free for any length and any alignment.\n", __FUNCTION__);

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif

	for( len = 1; len <= MAX_BLOCK_LENGTH; len++ )
	{
		for( sa = 0; sa <= 3; sa++ )
		{
			for( da = 0; da <= 3; da++ )
			{
				uint32 pat;
				
				/*rtlglue_printf("len[%d] sa[%d] da[%d]\n", len, sa, da );*/
#if 0
				for( j = 0; j < len; j++ )
				{
					enumbuf[0].src[j+sa] = enumbuf[1].src[j+sa] = rtlglue_random();
					enumbuf[0].dst[j+da] = enumbuf[1].dst[j+da] = rtlglue_random();
				}
#endif
				pat = rtlglue_random();
				
				for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
				{
					retval = model_setTestTarget( _i[i] );
					if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

					printf("\r\\\\");
					/* Reset GDMA */
					WRITE_MEM32( GDMACNR, 0 );
					WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)(&enumbuf[i].src[sa])) );
					WRITE_MEM32( GDMASBL0, (1<<31)|(len) );
					WRITE_MEM32( GDMADBP0, TO_PHY_ADDR((uint32)(&enumbuf[i].dst[da])) );
					WRITE_MEM32( GDMADBL0, (1<<31)|(len) );
					WRITE_MEM32( GDMAIMR, 0 );
					WRITE_MEM32( GDMAISR, 0xffffffff );
					WRITE_MEM32( GDMAICVL, pat );
					WRITE_MEM32( GDMAICVR, 0 );
					WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMCPY|internal_gdmacnr );

					while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
					MEMCTL_DEBUG_PRINTF("\r//");
				}

				ASSERT( compare_data_block() == SUCCESS );
			}
		}
	}
	
	printf("\n");
	return SUCCESS;
}


/*
 *	memory Compare: copy from testGdmaMemory12(): to garentee the first block only is bug free for any length and any alignment.
 */
int32 testGdmaMemory15(uint32 caseNo)
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	int len, sa, da;
	uint32 rIcvl[2];
	
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	printf("Start %s:memory Compare: copy from testGdmaMemory12(): to garentee the first block only is bug free for any length and any alignment. \n", __FUNCTION__);

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif

	//for( len = 1; len <= MAX_BLOCK_LENGTH; len++ )
	//for( len = 0x266; len <= MAX_BLOCK_LENGTH; len++ )
	for( len = 0x1; len <= MAX_BLOCK_LENGTH; len++ )
	{
		for( sa = 0; sa <= 3; sa++ )
		{
			for( da = 0; da <= 3; da++ )
			{
				uint32 sameLen;

				sameLen = rtlglue_random()%(len+1); /* we leave 1 possibility for 'NOT matched'. */
				/*rtlglue_printf("len[%d,%d] sa[%d] da[%d]\n", len, sameLen, sa, da );*/

				/* In the first 'sameLen' bytes, they are the same. */
				for( j = 0; j < sameLen; j++ )
				{
					enumbuf[0].src[j+sa] = enumbuf[1].src[j+sa] = enumbuf[0].dst[j+da] = enumbuf[1].dst[j+da] = rtlglue_random();
				}
				/* The remaining bytes are different. */
				for( j = sameLen; j < len; j++ )
				{
					enumbuf[0].src[j+sa] = enumbuf[1].src[j+sa] = rtlglue_random();
					enumbuf[0].dst[j+da] = enumbuf[1].dst[j+da] = rtlglue_random();
				}

				for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
				{
					retval = model_setTestTarget( _i[i] );
					if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

					printf("\r\\\\");
					/* Reset GDMA */
					WRITE_MEM32( GDMACNR, 0 );
					WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)(&enumbuf[i].src[sa])) );
					WRITE_MEM32( GDMASBL0, (1<<31)|(len) );
					WRITE_MEM32( GDMADBP0, TO_PHY_ADDR((uint32)(&enumbuf[i].dst[da])) );
					WRITE_MEM32( GDMADBL0, (1<<31)|(len) );
					WRITE_MEM32( GDMAIMR, 0 );
					WRITE_MEM32( GDMAISR, 0xffffffff );
					WRITE_MEM32( GDMAICVL, 0 );
					WRITE_MEM32( GDMAICVR, 0 );
					WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MEMCMP|internal_gdmacnr );

					while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
					MEMCTL_DEBUG_PRINTF("\r//");

					rIcvl[i] = READ_MEM32( GDMAICVL );
				}

				IS_EQUAL_INT("ICVL is not the same {model,real}:", rIcvl[0], rIcvl[1], __FUNCTION__, __LINE__ );
			}
		}
	}
	
	printf("\n");
	return SUCCESS;
}


/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  Checksum Offload                                                        **
 **                                                                          **
 ******************************************************************************
 ******************************************************************************/
/*
 * Test fixed pattern
 */
int32 testGdmaChecksum1(uint32 caseNo)
{
	struct TEST_VECTOR
	{
		uint32 icv;
		char *source;
		int len;
		uint32 sum;
	}
	static testvec[] =
	{
		{
			icv: 0xdeadc0de, /* test initial value */
			source: "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			len: 20,
			sum: 0xdeadc0de,
		},
		{
			icv: 0x0011+0x004c, /* 0 + protocol + length */
			source: "\
\x45\x00\x00\x60\x27\x98\x00\x00\x7f\x11\x0b\x68\xc0\xa8\x9a\xcf\
\xac\x0a\x01\x0b\x01\xff\x00\x89\x00\x4c\x93\xdf\x82\x76\x40\x00\
\x00\x01\x00\x00\x00\x00\x00\x01\x20\x41\x42\x41\x43\x46\x50\x46\
\x50\x45\x4e\x46\x44\x45\x43\x46\x43\x45\x50\x46\x48\x46\x44\x45\
\x46\x46\x50\x46\x50\x41\x43\x41\x42\x00\x00\x20\x00\x01\xc0\x0c\
\x00\x20\x00\x01\x00\x04\x93\xe0\x00\x06\xe0\x00\xc0\xa8\x00\xb9\
",
			len: 96,
			sum: 0x000afff5,
		},
		{
			icv: 0x0006+0x0086, /* 0 + protocol + length */
			source: "\
\x45\x00\x00\x9a\xe4\x9e\x40\x00\x7d\x06\x0f\xd8\xac\x15\x01\x9c\
\xc0\xa8\x9a\x8d\x00\x50\x04\x90\x4b\x16\x78\xeb\x8e\xa6\x25\x8f\
\x80\x18\xfe\x61\xcb\x10\x00\x00\x01\x01\x08\x0a\x03\x9c\x48\x43\
\x00\x70\x2b\xbb\x48\x54\x54\x50\x2f\x31\x2e\x31\x20\x33\x30\x34\
\x20\x4f\x4b\x0d\x0a\x53\x65\x72\x76\x65\x72\x3a\x20\x4d\x69\x63\
\x72\x6f\x73\x6f\x66\x74\x2d\x49\x49\x53\x2f\x35\x2e\x30\x0d\x0a\
\x44\x61\x74\x65\x3a\x20\x54\x68\x75\x2c\x20\x30\x34\x20\x41\x75\
\x67\x20\x32\x30\x30\x35\x20\x31\x34\x3a\x33\x33\x3a\x32\x33\x20\
\x47\x4d\x54\x0d\x0a\x43\x6f\x6e\x6e\x65\x63\x74\x69\x6f\x6e\x3a\
\x20\x63\x6c\x6f\x73\x65\x0d\x0a\x0d\x0a",
			len: 154,
			sum: 0x0013ffec,
		},
	};
	int i, j;
	int32 retval;
	
	testGdmaInit();

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
		{
			int _offset[] = { 0, 2, -1 };
			int *offset;
			
			for( offset = _offset; *offset>=0; offset++ )
			{
				/* clean memory */
				clearGdmaBuffer(CLEAN_ALL);

				/* Reset GDMA */
				WRITE_MEM32( GDMACNR, 0 );
				WRITE_MEM32( GDMACNR, GDMA_ENABLE );
				
				memcpy( sb2+*offset, testvec[j].source, testvec[j].len );
				WRITE_MEM32( GDMAIMR, 0 );
				WRITE_MEM32( GDMAISR, 0xffffffff );
				WRITE_MEM32( GDMAICVL, testvec[j].icv );
				WRITE_MEM32( GDMAICVR, 0 );
				WRITE_MEM32( GDMASBP0, (uint32)sb2+0x0c+*offset ); /* SIP + DIP */
				if ( (testvec[j].len-20) > 0 )
				{
					WRITE_MEM32( GDMASBL0, 8 );
					WRITE_MEM32( GDMASBP1, (uint32)sb2+0x14+*offset ); /* UDP + payload */
					WRITE_MEM32( GDMASBL1, GDMA_LDB|(testvec[j].len-20) );
				}
				else
				{
					WRITE_MEM32( GDMASBL0, GDMA_LDB|8 );
				}
				WRITE_MEM32( GDMADBP0, 0 );
				WRITE_MEM32( GDMADBL0, 0 );
				WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_CHKOFF|internalUsedGDMACNR );

				/* Expected data */
				memcpy( sb1+*offset, testvec[j].source, testvec[j].len );
				while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

#if 0
				IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
				IS_EQUAL_INT("Source block should under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, GDMA_SBUDRIP, __FUNCTION__, __LINE__ );
#endif
				IS_EQUAL_INT("Checksum not equal!", READ_MEM32(GDMAICVL), testvec[j].sum, __FUNCTION__, __LINE__ );
				ASSERT( compare_data_block() == SUCCESS );
			}
		}
	}

	return SUCCESS;
}


/*
 *	check sum: copy from cbMemory4(): source blocks are not overlapped, but continuous.
 *	In some cases, there exists some gaps between blocks (by 'align' field).
 */
int32 cbChecksum2( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int32 retval;
	int i, j, k;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	uint8 *pch2;
	uint32 iChksum;
	uint32 rChksum[2];

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif
	DEBUG_BAL();

	iChksum = rtlglue_random();
	
	/* generate source data */							
	pch2 = enumbuf[0].src; /* pointer to source data block */
	for( j = 0; j < 8; j++ )
	{
		pch2 += sa[j];
		for( k = 0; k < sl[j]; k++ ) /* generate random content */
			pch2[k] = rtlglue_random()>>16; /* We use bit [23:16]. */
		pch2 += sl[j];
	}

	for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
	{
		retval = model_setTestTarget( _i[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		/* Reset GDMA */
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			
		/* source block */							
		pch2 = enumbuf[0].src; /* pointer to source data block */
		for( j = 0; j < 8; j++ )
		{
			pch2 += sa[j];
			WRITE_MEM32( GDMASBP0+j*8, (uint32)pch2 );
			pch2 += sl[j];
			if ( j==7 || sl[j+1]==0 )
			{
				WRITE_MEM32( GDMASBL0+j*8, GDMA_LDB|(sl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMASBL0+j*8, sl[j] );
			}
		}

		for( j = 0; j < 8; j++ )
		{
			WRITE_MEM32( GDMADBP0+j*8, 0x00000000 );
			WRITE_MEM32( GDMADBL0+j*8, 0x00000000 );
		}
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, iChksum );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_CHKOFF|internalUsedGDMACNR );

		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

		rChksum[i] = READ_MEM32( GDMAICVL );
	}

	IS_EQUAL_INT("Checksum is not the same. {Model:Real}=", rChksum[0], rChksum[1], __FUNCTION__, __LINE__ );

	return SUCCESS;
}
int32 testGdmaChecksum2(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	static int32 tmplSrcLen[] = { 2, 4, 6, 8, 10, 24, 32, 34, 66, 126, 128, 130, 254, 256, 258, 506, 512, 516, 1498 };
	static int32 tmplSrcAlign[] = { 0, 1, 2, 3 };
	static int32 tmplDstLen[] = { 0 };
	static int32 tmplDstAlign[] = { 0 };

	testGdmaInit();
	clearGdmaBuffer(CLEAN_ALL);

	memset( sa, 0, sizeof(sa) );
	memset( sl, 0, sizeof(sl) );
	memset( da, 0, sizeof(da) );
	memset( dl, 0, sizeof(dl) );

	return enumBAL( 2, 0, 99999, 99999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
	                sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
	                sl, sa, dl, da, cbChecksum2, ENUMBAL_ALLOW_SgD|ENUMBAL_IGNORE_DST );
}


/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  Sequential TCAM Offload                                                 **
 **                                                                          **
 ******************************************************************************
 ******************************************************************************/
/*
 * Test fixed pattern
 */
int32 testGdmaSequentialTCAM(uint32 caseNo)
{
	enum IC_TYPE i;
	int j;
	int32 retval;


volatile uint32 *start_addr;
uint32 processed_len;
uint32 k;


	struct TEST_VECTOR
	{
		uint32 entSize; /* unit: byte */
		uint8 dst[32];
		uint8 src[64*8];
		int count;
		int32 match; /* -1 means no match */
	}
	static testvec[] =
	{
		{
			entSize: 4,
			dst: { 192,168,154,254 },
			src: { 192,168,154,241,255,255,255,255,
			       192,168,154,242,255,255,255,255,
			       192,168,251,  0,255,255,255,  0,
			       192,168,154,  0,255,255,255,  0,
			         0,  0,  0,  0,  0,  0,  0,  0,
			     },
			count: 5,
			match: 3,
		},
		{
			entSize: 8,
			dst: { 192,168,154,141,                192,168, 87,  1 },
			src: { 192,168,154,241,255,255,255,255,192,168,154,242,255,255,255,255,
			       192,168,154,254,255,255,255,254,192,168, 87,111,255,255,255,254,
			       192,168,251,  0,255,255,255,  0,192,168, 87,  0,255,255,255,  0,
			       192,168,154,  0,255,255,255,  0,192,168,251,  0,255,255,255,  0,
			       192,168,154,254,255,255,255,  0,192,168, 87,111,255,255,255,  0,
			       192,168,111,111,255,255,  0,  0,192,168,111,111,255,255,  0,  0,
			     },
			count: 6,
			match: 4,
		},
		{
			entSize: 16,
			dst: { 0x20,0x02,0x8c,0x71,                     0x1b,0x32,0x00,0x00,                     0x00,0x00,0x00,0x00,                     0x8c,0x71,0x1b,0x32 },
			src: { 0xfe,0x80,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*Link-Local*/
			       0xfe,0xC0,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*Site-Local*/
			       0x20,0x01,0x00,0x00,0xff,0xff,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*Commercial*/
			       0x3F,0xFE,0x00,0x00,0xff,0xff,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*For Experiment*/
			       0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x01,0xff,0xff,0xff,0xff, /*Localhost*/
			       0x20,0x02,0x8c,0x71,0xff,0xff,0xff,0xff, 0x1b,0x32,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*6-to-4 Auto Tunnel*/
			       0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*Any Address*/
			     },
			count: 7,
			match: 5,
		},
		{
			entSize: 32,
			dst: { 0x20,0x02,0x8c,0x71,                     0x1b,0x32,0x00,0x00,                     0x00,0x00,0x00,0x00,                     0x8c,0x71,0x1b,0x32,                     0x20,0x02,0x8c,0x71,                     0x1b,0x32,0x00,0x00,                     0x00,0x00,0x00,0x00,                     0x55,0xaa,0x77,0x88 },
			src: { 0xfe,0x80,0x00,0x00,
                               0xff,0xff,0xff,0xff, 
                               0x00,0x00,0x00,0x00,
                               0xff,0xff,0xff,0xff,
                               0x00,0x00,0x00,0x00,
                               0x00,0x00,0x00,0x00, 
                               0x00,0x00,0x00,0x00,
                               0x00,0x00,0x00,0x00, 
                               0xfe,0x80,0x00,0x00,
                               0xff,0xff,0xff,0xff, 
                               0x00,0x00,0x00,0x00,
                               0xff,0xff,0xff,0xff, 
                               0x00,0x00,0x00,0x00,
                               0x00,0x00,0x00,0x00, 
                               0x00,0x00,0x00,0x00,
                               0x00,0x00,0x00,0x00, /*Link-Local*/
			       0xfe,0xC0,0x00,0x00,
                               0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0xfe,0xC0,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*Site-Local*/
			       0x20,0x01,0x00,0x00,0xff,0xff,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x20,0x01,0x00,0x00,0xff,0xff,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*Commercial*/
			       0x3F,0xFE,0x00,0x00,0xff,0xff,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x3F,0xFE,0x00,0x00,0xff,0xff,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*For Experiment*/
			       0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x01,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x01,0xff,0xff,0xff,0xff, /*Localhost*/
			       0x20,0x02,0x8c,0x71,0xff,0xff,0xff,0xff, 0x1b,0x32,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x20,0x02,0x8c,0x71,0xff,0xff,0xff,0xff, 0x1b,0x32,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x55,0xaa,0x77,0x88,0xff,0xff,0xff,0xff, /*6-to-4 Auto Tunnel*/
			       0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*Any Address*/
			     },
			count: 7,
			match: 5,
		},
		{
			entSize: 4,
			dst: { 192,168,154,254 },
			src: { 192,168,154,241,255,255,255,255,
			       192,168,154,242,255,255,255,255,
			       192,168,251,  0,255,255,255,  0,
			       192,168,152,  0,255,255,255,  0,
			       192,168,153,  0,255,255,255,  0,
			     },
			count: 5,
			match: -1,
		},
	};

	testGdmaInit();

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
		{
			/* clean memory */
			clearGdmaBuffer(CLEAN_ALL);

			/* Reset GDMA */
			WRITE_MEM32( GDMACNR, 0 );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			
			memcpy( sb2, testvec[j].src, (testvec[j].count*testvec[j].entSize*2) );

//			_cache_flush();

			WRITE_MEM32( GDMAIMR, 0 );
			WRITE_MEM32( GDMAISR, 0xffffffff );
			WRITE_MEM32( GDMAICVL, 0 );
			WRITE_MEM32( GDMAICVR, 0 );
			WRITE_MEM32( GDMASBP0, (uint32)sb2 );
			WRITE_MEM32( GDMASBL0, GDMA_LDB|(testvec[j].count*testvec[j].entSize*2) );
			WRITE_MEM32( GDMADBP0, big_endian(((uint32*)testvec[j].dst)[0]) );
			WRITE_MEM32( GDMADBL0, 0 );
			WRITE_MEM32( GDMADBP1, big_endian(((uint32*)testvec[j].dst)[1]) );
			WRITE_MEM32( GDMADBL1, 0 );
			WRITE_MEM32( GDMADBP2, big_endian(((uint32*)testvec[j].dst)[2]) );
			WRITE_MEM32( GDMADBL2, 0 );
			WRITE_MEM32( GDMADBP3, big_endian(((uint32*)testvec[j].dst)[3]) );
			WRITE_MEM32( GDMADBL3, 0 );
			WRITE_MEM32( GDMADBP4, big_endian(((uint32*)testvec[j].dst)[4]) );
			WRITE_MEM32( GDMADBL4, 0 );
			WRITE_MEM32( GDMADBP5, big_endian(((uint32*)testvec[j].dst)[5]) );
			WRITE_MEM32( GDMADBL5, 0 );
			WRITE_MEM32( GDMADBP6, big_endian(((uint32*)testvec[j].dst)[6]) );
			WRITE_MEM32( GDMADBL6, 0 );
			WRITE_MEM32( GDMADBP7, big_endian(((uint32*)testvec[j].dst)[7]) );
			WRITE_MEM32( GDMADBL7, 0 );
#if 0
if(j==3)
{
	rtlglue_printf("register_data:\n");
	start_addr = (uint32 *)GDMASBP0;
	for(k=0; k<32; k++){
		rtlglue_printf("\t.word\t0x%08x\n", *start_addr);
		start_addr++;
	}
	rtlglue_printf("register_data_end:\n");

	rtlglue_printf("src_data1:\n");
	start_addr = *((uint32 *)(GDMASBP0));
	processed_len = *((uint32 *)(GDMASBL0));
	processed_len = processed_len & 0x7FFFFFFF;
rtlglue_printf("processed_len = %d, processed_len/4 = %d\n", processed_len , processed_len/4);
	for(k=0; k<(processed_len/4); k++){
		rtlglue_printf("\t.word\t0x%08x\n", *start_addr);
		start_addr++;
	}
	rtlglue_printf("src_data1_end:\n");
rtlglue_printf("0x%08x", (GDMA_ENABLE|GDMA_POLL|GDMA_STCAM| (testvec[j].entSize==32?GDMA_ENTSIZ256:testvec[j].entSize==16?GDMA_ENTSIZ128:testvec[j].entSize== 8?GDMA_ENTSIZ64:GDMA_ENTSIZ32)| internalUsedGDMACNR));
}
#endif
			WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_STCAM| (testvec[j].entSize==32?GDMA_ENTSIZ256:
			                                                         testvec[j].entSize==16?GDMA_ENTSIZ128:
			                                                         testvec[j].entSize== 8?GDMA_ENTSIZ64:GDMA_ENTSIZ32)|
			                                                       internalUsedGDMACNR );

			/* Expected data */
			memcpy( sb1, testvec[j].src, (testvec[j].count*testvec[j].entSize*2) );
			while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

			/*IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );*/
//rtlglue_printf("testvec[%d].match = %d\n", j, testvec[j].match);
			if ( testvec[j].match >= 0 )
			{
				/* should matched !!! */
				IS_EQUAL_INT("Should be matched, however the result is NOT matched.", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, 0, __FUNCTION__, __LINE__ );
				IS_EQUAL_INT("Match Index is not right!", READ_MEM32(GDMAICVL), testvec[j].match, __FUNCTION__, __LINE__ );
			}
			else
			{
				/* should NOT matched */
				IS_EQUAL_INT("Should be NOT matched, however the result is matched.", READ_MEM32(GDMAISR)&GDMA_NEEDCPUIP, GDMA_NEEDCPUIP, __FUNCTION__, __LINE__ );
			}
			ASSERT( compare_data_block() == SUCCESS );
		}
	}

	return SUCCESS;
}


/*
 *	sequential TCAM: Random test for FPGA and model code.
 */
int32 testGdmaSequentialTCAM2( uint32 caseNo )
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	uint32 idxMatched[2];
	uint32 isMatched[2];
	uint32 loop;
	uint32 ipSize;
	uint32 ipaddress[8];
	uint32 matchedNo = 0;

	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */

	for( ipSize = GDMA_ENTSIZ32; ipSize <= GDMA_ENTSIZ256; ipSize += GDMA_ENTSIZ64 ) /* test entry size for 32/64/128/256 bits */
	{
		/* loop 100 -- about 45 minutes */
		for( loop = 0; loop < 100; loop++ )
		{
			/* We have some chances to generate 'matched' case.
			 * It depends on the 'mask bits'.
			 */

			for( j = 0; j < HEAPSIZE; j+=4 )
			{
				*(uint32*)&enumbuf[0].src[j] = *(uint32*)&enumbuf[1].src[j] = rtlglue_random();
			}
			for( j = 0; j < sizeof(ipaddress)/sizeof(ipaddress[0]); j++ )
			{
				ipaddress[j] = rtlglue_random();
			}

			for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
			{
				/* rtlglue_printf( "ipSize:%d loop:%d i:%d\n", 32*(1<<((ipSize-GDMA_ENTSIZ32)>>20)), loop, i ); */

				retval = model_setTestTarget( _i[i] );
				if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

				/* Reset GDMA */
				WRITE_MEM32( GDMACNR, 0 );
				WRITE_MEM32( GDMACNR, GDMA_ENABLE );
					
				/* source block */							
				for( j = 0; j < 8; j++ )
				{
					WRITE_MEM32( GDMASBP0+j*8, (uint32)(&enumbuf[i].src[j*8191]) );
					if ( j==7 )
						WRITE_MEM32( GDMASBL0+j*8, GDMA_LDB|8191 );
					else
						WRITE_MEM32( GDMASBL0+j*8, 8191 );
				}

				for( j = 0; j < 8; j++ )
				{
					WRITE_MEM32( GDMADBP0+j*8, ipaddress[j] );
					WRITE_MEM32( GDMADBL0+j*8, 0x00000000 );
				}
				WRITE_MEM32( GDMAIMR, 0 );
				WRITE_MEM32( GDMAISR, 0xffffffff );
				WRITE_MEM32( GDMAICVL, 0xffffffff );
				WRITE_MEM32( GDMAICVR, 0 );
				WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_STCAM|ipSize|internalUsedGDMACNR );

				while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

				if ( READ_MEM32( GDMAISR ) & GDMA_NEEDCPUIP )
				{
					/* Not matched */
					isMatched[i] = FALSE;
				}
				else
				{
					/* Matched */
					isMatched[i] = TRUE;
					//IS_EQUAL_INT("Matched Idx is not the same {expected, real}=", expectedMatchIdx, READ_MEM32(GDMAICVL), __FUNCTION__, __LINE__ );
					idxMatched[i] = READ_MEM32( GDMAICVL );
				}
				
				if ( isMatched[0] ) matchedNo++;
			}

			if ( isMatched[0] != isMatched[1] )
			{
				rtlglue_printf( "%s:%d  Model code says '%s', but IC says '%s'.", __FUNCTION__, __LINE__,
				                isMatched[0]?"matched":"not matched",
				                isMatched[1]?"matched":"not matched" );
				return FAILED;
			}
			if ( isMatched[0] == TRUE && idxMatched[0] != idxMatched[1] )
			{
				rtlglue_printf( "%s:%d  Model code matched %drd rule, but IC matched %drd rule.", __FUNCTION__, __LINE__,
				                idxMatched[0],
				                idxMatched[1] );
				return FAILED;
			}
		}
	}

	rtlglue_printf( "matchedNo=%u\n", matchedNo );
	return SUCCESS;
}


/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  Wireless MIC                                                            **
 **                                                                          **
 ******************************************************************************
 ******************************************************************************/
/*
 * Test fixed pattern
 */
int32 testGdmaWirelessMIC(uint32 caseNo)
{

	int i, j;
	int32 retval;

struct TEST_VECTOR
{
	uint32 icvl;
	uint32 icvr;
	char *source;
	uint32 len;
	uint32 outl;
	uint32 outr;
} 
static testvec[6] =
{
	{
		icvl: 0x000000000,
		icvr: 0x000000000,
		source: "\x5a\x00\x00\x00\x00\x00\x00\x00",
		len: 8,
		outl: 0x1c5c9282,
		outr: 0xb830d1a1,
	},
	{
		icvl: 0x1c5c9282,
		icvr: 0xb830d1a1,
		source: "M\x5a\x00\x00\x00\x00\x00\x00",
		len: 8,
		outl: 0xca214743,
		outr: 0x3f9b6340,
	},
	{
		icvl: 0xca214743,
		icvr: 0x3f9b6340,
		source: "Mi\x5a\x00\x00\x00\x00\x00",
		len: 8,
		outl: 0xcabef9e8,
		outr: 0x295d7ee9,
	},
	{
		icvl: 0xcabef9e8,
		icvr: 0x295d7ee9,
		source: "Mic\x5a\x00\x00\x00\x00",
		len: 8,
		outl: 0xc68f0390,
		outr: 0xdbc113cf,
	},
	{
		icvl: 0xc68f0390,
		icvr: 0xdbc113cf,
		source: "Mich\x5a\x00\x00\x00\x00\x00\x00\x00",
		len: 12,
		outl: 0x05105eD5,
		outr: 0x86891210,
	},
	{
		icvl: 0x05105eD5,
		icvr: 0x86891210,
		source: "Michael\x5a\x00\x00\x00\x00",
		len: 12,
		outl: 0x122b940a,
		outr: 0x46a5ca4e,
	},
};
	
	testGdmaInit();

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
		{
			int _offset[] = { 0, -1 };
			int *offset;
			
			for( offset = _offset; *offset>=0; offset++ )
			{
				/* clean memory */
				clearGdmaBuffer(CLEAN_ALL);

				printf("\r\\\\");
				/* Reset GDMA */
				WRITE_MEM32( GDMACNR, 0 );
				WRITE_MEM32( GDMACNR, GDMA_ENABLE );
				
				memcpy( sb2+*offset, testvec[j].source, testvec[j].len );
				WRITE_MEM32( GDMAIMR, 0 );
				WRITE_MEM32( GDMAISR, 0xffffffff );
				WRITE_MEM32( GDMAICVL, testvec[j].icvl );
				WRITE_MEM32( GDMAICVR, testvec[j].icvr );
				WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)sb2+*offset) ); /* SIP + DIP */
				WRITE_MEM32( GDMASBL0, GDMA_LDB|testvec[j].len );
				WRITE_MEM32( GDMADBP0, 0 );
				WRITE_MEM32( GDMADBL0, 0 );
				WRITE_MEM32( GDMACNR, (unsigned int)(GDMA_ENABLE|GDMA_POLL|GDMA_MIC)|internal_gdmacnr );
//				printf("GDMACNR = 0x%08x\n", GDMACNR);
//				printf("GDMA_POLL = 0x%08x\n", GDMA_POLL);
//				printf("GDMA_MIC = 0x%08x\n", GDMA_MIC);
//				printf("internal_gdmacnr = 0x%08x\n", internal_gdmacnr);
//				printf("GDMA_ENABLE|GDMA_POLL|GDMA_MIC|internal_gdmacnr = 0x%08x\n", GDMA_ENABLE|GDMA_POLL|GDMA_MIC|internal_gdmacnr);

				/* Expected data */
				memcpy( sb1+*offset, testvec[j].source, testvec[j].len );
				while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
				MEMCTL_DEBUG_PRINTF("\r//");

#if 0
				IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
				IS_EQUAL_INT("Source block should under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, GDMA_SBUDRIP, __FUNCTION__, __LINE__ );
#endif
				IS_EQUAL_INT("Checksum not equal!", READ_MEM32(GDMAICVL), testvec[j].outl, __FUNCTION__, __LINE__ );
				IS_EQUAL_INT("Checksum not equal!", READ_MEM32(GDMAICVR), testvec[j].outr, __FUNCTION__, __LINE__ );
				ASSERT( compare_data_block() == SUCCESS );
			}
		}
	}
	printf("\n");
	return SUCCESS;
}


/*
 *	Wireless MIC: We will generate random data and length.
 *	In some cases, there exists some gaps between blocks (by 'align' field).
 */
int32 cbGdmaWirelessMIC2( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	uint32 icvl[sizeof(_i)/sizeof(_i[0])], icvr[sizeof(_i)/sizeof(_i[0])];
	uint32 iICVL, iICVR;
	uint8 *pch;
	static char debuginfo[256];

#if 0 /* #if 1 ==> to speed up, we output cases of no-gap between blocks (we just test the first block is unaligned). */
	  /* #if 0 ==> we will output cases inserted gaps between blocks. */
	for( j = 1; j < 8; j++ )
		if ( sa[j]>0 || da[j]>0 ) return SUCCESS;
#endif
	DEBUG_BAL();

#if 0
	sprintf( debuginfo, "src[]={%d:%d,%d:%d,%d:%d,%d:%d,%d:%d,%d:%d,%d:%d,%d:%d}",
	         sl[0], sa[0], sl[1], sa[1], sl[2], sa[2], sl[3], sa[3], sl[4], sa[4], sl[5], sa[5], sl[6], sa[6], sl[7], sa[7] );
	modelExportComment( debuginfo );
	sprintf( debuginfo, "dst[]={%d:%d,%d:%d,%d:%d,%d:%d,%d:%d,%d:%d,%d:%d,%d:%d}",
	         dl[0], da[0], dl[1], da[1], dl[2], da[2], dl[3], da[3], dl[4], da[4], dl[5], da[5], dl[6], da[6], dl[7], da[7] );
	modelExportComment( debuginfo );

	printf("src[]={%d:%d,%d:%d,%d:%d,%d:%d,%d:%d,%d:%d,%d:%d,%d:%d}\n",
	         sl[0], sa[0], sl[1], sa[1], sl[2], sa[2], sl[3], sa[3], sl[4], sa[4], sl[5], sa[5], sl[6], sa[6], sl[7], sa[7] );
	printf("dst[]={%d:%d,%d:%d,%d:%d,%d:%d,%d:%d,%d:%d,%d:%d,%d:%d}\n",
	         dl[0], da[0], dl[1], da[1], dl[2], da[2], dl[3], da[3], dl[4], da[4], dl[5], da[5], dl[6], da[6], dl[7], da[7] );
#endif

	/* prepare random data*/
	iICVL = rtlglue_random();
	iICVR = rtlglue_random();

	/* clean memory */
	clearGdmaBuffer(CLEAN_SB1);
	pch = sb1; /* pointer to source data block */
	for( j = 0; j < 8; j++ )
	{
		int k;
		
		pch += sa[j];
		for( k = 0; k < sl[j]; k++ )
			pch[k] = rtlglue_random() & 0xff;
		pch += sl[j];
	}

	for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
	{
		retval = model_setTestTarget( _i[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		printf("\r\\\\");
		/* Reset GDMA */
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			
		/* source block */							
		pch = sb1; /* pointer to source data block */
		for( j = 0; j < 8; j++ )
		{
			pch += sa[j];
			WRITE_MEM32( GDMASBP0+j*8, TO_PHY_ADDR((uint32)pch) );
			pch += sl[j];
			if ( j==7 || sl[j+1]==0 )
			{
				WRITE_MEM32( GDMASBL0+j*8, GDMA_LDB|(sl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMASBL0+j*8, sl[j] );
			}
		}

		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, iICVL );
		WRITE_MEM32( GDMAICVR, iICVR );
#if 0
		if(_i[i] == IC_TYPE_REAL){
			if((sl[0]==12) && (sa[0] == 1) && (sl[1]==8188) && (sa[1]==2)){
				printf("GDMACNR(0x%08x)  = 0x%08x\n", GDMAIMR,  0);
				printf("GDMACNR(0x%08x)  = 0x%08x\n", GDMACNR,  GDMA_ENABLE);

				printf("GDMASBP0(0x%08x)  = 0x%08x\n", GDMASBP0, READ_MEM32(GDMASBP0) );
				printf("GDMASBL0(0x%08x)  = 0x%08x\n", GDMASBL0, READ_MEM32(GDMASBL0) );
				printf("GDMASBP1(0x%08x)  = 0x%08x\n", GDMASBP1, READ_MEM32(GDMASBP1) );
				printf("GDMASBL1(0x%08x)  = 0x%08x\n", GDMASBL1, READ_MEM32(GDMASBL1) );
				

				printf("GDMAIMR(0x%08x)  = 0x%08x\n", GDMAIMR,  0);
				printf("GDMAISR(0x%08x)  = 0x%08x\n", GDMAISR,  0xffffffff);
				printf("GDMAICVL(0x%08x) = 0x%08x\n", GDMAICVL, iICVL);
				printf("GDMAICVR(0x%08x) = 0x%08x\n", GDMAICVR, iICVR);
				printf("GDMACNR(0x%08x)  = 0x%08x\n", GDMACNR,  GDMA_ENABLE|GDMA_POLL|GDMA_MIC|internal_gdmacnr);

			}
			if(icvl[0]==0x41625a72){
			//	printf("pch = (0x%p)\n", pch);
				printf("GDMACNR(0x%08x)  = 0x%08x\n", GDMAIMR,  0);
				printf("GDMACNR(0x%08x)  = 0x%08x\n", GDMACNR,  GDMA_ENABLE);

				printf("GDMASBP0(0x%08x)  = 0x%08x\n", GDMASBP0, READ_MEM32(GDMASBP0) );
				printf("GDMASBL0(0x%08x)  = 0x%08x\n", GDMASBL0, READ_MEM32(GDMASBL0) );
				printf("GDMASBP1(0x%08x)  = 0x%08x\n", GDMASBP1, READ_MEM32(GDMASBP1) );
				printf("GDMASBL1(0x%08x)  = 0x%08x\n", GDMASBL1, READ_MEM32(GDMASBL1) );
				

				printf("GDMAIMR(0x%08x)  = 0x%08x\n", GDMAIMR,  0);
				printf("GDMAISR(0x%08x)  = 0x%08x\n", GDMAISR,  0xffffffff);
				printf("GDMAICVL(0x%08x) = 0x%08x\n", GDMAICVL, iICVL);
				printf("GDMAICVR(0x%08x) = 0x%08x\n", GDMAICVR, iICVR);
				printf("GDMACNR(0x%08x)  = 0x%08x\n", GDMACNR,  GDMA_ENABLE|GDMA_POLL|GDMA_MIC|internal_gdmacnr);
			}
		}
#endif
		//printf("Write GDMACNR(0x%08x)=(0x%08x)", GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MIC|internal_gdmacnr);
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MIC|internal_gdmacnr );
		//printf("Read GDMACNR(0x%08x)=(0x%08x)", GDMACNR,READ_MEM32( GDMACNR ));

		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		MEMCTL_DEBUG_PRINTF("\r//");

		icvl[i] = READ_MEM32(GDMAICVL);
		icvr[i] = READ_MEM32(GDMAICVR);

		if ( _i[i] == IC_TYPE_REAL )
		{
			IS_EQUAL_INT("ICVL not equal!", icvl[0], icvl[1], __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("ICVR not equal!", icvr[0], icvr[1], __FUNCTION__, __LINE__ );
		}
	}


	return SUCCESS;
}
int32 testGdmaWirelessMIC2(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	static int32 tmplSrcLen[] = { 4, 8, 12, 16, 20, 24, 28, 32, 36, 0x1FFC };
	static int32 tmplSrcAlign[] = { 0, 1, 2, 3 };
	static int32 tmplDstLen[] = { 0 };
	static int32 tmplDstAlign[] = { 0 };

	testGdmaInit();

	memset( sa, 0, sizeof(sa) );
	memset( sl, 0, sizeof(sl) );
	memset( da, 0, sizeof(da) );
	memset( dl, 0, sizeof(dl) );

	return enumBAL( 2, 0, ALL_BLOCK_LENGTH, ALL_BLOCK_LENGTH, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
	                sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
	                sl, sa, dl, da, cbGdmaWirelessMIC2, ENUMBAL_ALLOW_ALL );
}


/*
 *	Random test to compare model code and FPGA code. 
 */
int32 testGdmaWirelessMIC3(uint32 caseNo)
{
	int i;
	int k;
	int32 retval;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL };
	int loop;
	uint32 result[2][2];
	uint32 iICVL, iICVR;
	uint32 length;

	testGdmaInit();

	/* loop 10 times --> about 7 seconds */
	for( loop = 0; loop < 5000; loop++ )
	{
		int l;
			
		iICVL = rtlglue_random();
		iICVR = rtlglue_random();
		for( l = 0; l < HEAPSIZE; l++ )
			sb1[l] = rtlglue_random() % 0xff;
		length = ((rtlglue_random()%2047)+1)*4;
		//length = 0x1ffc;

		for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
		{
			retval = model_setTestTarget( _i[i] );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			printf("\r\\\\");
			/* Reset GDMA */
			WRITE_MEM32( GDMACNR, 0 );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			WRITE_MEM32( GDMAIMR, 0 );
			WRITE_MEM32( GDMAISR, 0xffffffff );
			WRITE_MEM32( GDMAICVL, iICVL );
			WRITE_MEM32( GDMAICVR, iICVR );
			WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)sb1) );
			WRITE_MEM32( GDMASBL0, GDMA_LDB|length );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MIC|internal_gdmacnr );

			while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
			MEMCTL_DEBUG_PRINTF("\r//");

			/* Save result */
			result[i][0] = READ_MEM32( GDMAICVL );
			result[i][1] = READ_MEM32( GDMAICVR );
		}
//continue;
		/* compare the result */
		if ( memcmp( result[0], result[1], sizeof(result[0]) ) != 0 )
		{
			rtlglue_printf( "The results of model code and IC are not the same.\nModel code: [0x%08x:0x%08x]  IC: [0x%08x:0x%08x]\n",
			                result[0][0], result[0][1], result[1][0], result[1][1] );
			return FAILED;
		}
	}

	printf("\n");
	return SUCCESS;
}


/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  Base 64                                                                 **
 **                                                                          **
 ******************************************************************************
 ******************************************************************************/
/*
 * Test basic pattern
 */
int32 testGdmaBase64Encode1(uint32 caseNo)
{
	struct TEST_VECTOR
	{
		char *decoded;
		char *encoded;
	}
	static testvec[] =
	{
		{
			decoded: "\x01\x02\x04",
			encoded: "\x41\x51\x49\x45",
		},
		{
			decoded: "\n",
			encoded: "Cg==",
		},
		{
			decoded: "01",
			encoded: "MDE=",
		},
		{
			decoded: "\xFF\xFF\xFF\xFF",
			encoded: "/////w==",
		},
		{
			decoded: "This is a test for base64\n",
			encoded: "VGhpcyBpcyBhIHRlc3QgZm9yIGJhc2U2NAo=",
		},
		{
			decoded: "\
#include<stdio.h>\n\
#include<math.h>\n\
\n\
int main( int argc, char** argv )\n\
{\n\
	double dR;\n\
	double total = 0;\n\
	double i;\n\
\n\
	dR = atof( argv[1] );\n\
	printf(\"R=%lf\\n\", dR );\n\
\n\
	for( i = 1; i <= 50; i++ )\n\
	{\n\
		total += 6600 / ( pow( 1 + (dR/10000)/12.0, i ) );\n\
	}\n\
\n\
	printf(\"total=%lf\\n\", total );\n\
\n\
	return 0;\n\
}\n",
			encoded: "\
I2luY2x1ZGU8c3RkaW8uaD4KI2luY2x1ZGU8bWF0aC5oPgoKaW50IG1haW4oIGludCBhcmdjLCBj\r\n\
aGFyKiogYXJndiApCnsKCWRvdWJsZSBkUjsKCWRvdWJsZSB0b3RhbCA9IDA7Cglkb3VibGUgaTsK\r\n\
CglkUiA9IGF0b2YoIGFyZ3ZbMV0gKTsKCXByaW50ZigiUj0lbGZcbiIsIGRSICk7CgoJZm9yKCBp\r\n\
ID0gMTsgaSA8PSA1MDsgaSsrICkKCXsKCQl0b3RhbCArPSA2NjAwIC8gKCBwb3coIDEgKyAoZFIv\r\n\
MTAwMDApLzEyLjAsIGkgKSApOwoJfQoKCXByaW50ZigidG90YWw9JWxmXG4iLCB0b3RhbCApOwoK\r\n\
CXJldHVybiAwOwp9Cg==",
		},
	};
	int i, j;
	int32 retval;
	int maxLen = 76;
	
	testGdmaInit();

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
		{
			/* clean memory */
			clearGdmaBuffer(CLEAN_ALL);

			/* Reset GDMA */
			WRITE_MEM32( GDMACNR, 0 );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE );
				
			memcpy( sb2, testvec[j].decoded, strlen(testvec[j].decoded) );
			WRITE_MEM32( GDMAIMR, 0 );
			WRITE_MEM32( GDMAISR, 0xffffffff );
			WRITE_MEM32( GDMAICVL, maxLen/4-1 ); /* Line Length */
			WRITE_MEM32( GDMAICVR, 0 );
			WRITE_MEM32( GDMASBP0, (uint32)sb2 );
			WRITE_MEM32( GDMASBL0, GDMA_LDB|strlen(testvec[j].decoded) );
			WRITE_MEM32( GDMADBP0, (uint32)db2 );
#if 0
			WRITE_MEM32( GDMADBL0, GDMA_LDB|(strlen(testvec[j].encoded)) );
#else
			/* old fashion to compute output length */
			WRITE_MEM32( GDMADBL0, GDMA_LDB|(strlen(testvec[j].decoded)*4/3+(strlen(testvec[j].decoded)*4%3?3:0)+strlen(testvec[j].decoded)*4/3/maxLen*2) );
#endif
			WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_B64ENC|internalUsedGDMACNR );

			/* Expected data */
			memcpy( sb1, testvec[j].decoded, strlen(testvec[j].decoded) );
			memcpy( db1, testvec[j].encoded, strlen(testvec[j].encoded) );

			while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

#if 0
			IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Source block should under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, GDMA_SBUDRIP, __FUNCTION__, __LINE__ );
#endif
			IS_EQUAL_INT("Processed length not match!  real,expected=", READ_MEM32(GDMAICVL)>>16, strlen(testvec[j].decoded), __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Output length not match!  real,expected=", READ_MEM32(GDMAICVL)&0xffff, strlen(testvec[j].encoded), __FUNCTION__, __LINE__ );
			ASSERT( compare_data_block() == SUCCESS );
		}
	}

	return SUCCESS;
}


/*
 *	To compare model code and FPGA code.
 *	first block: aligned address, unaligned length
 *	other blocks: aligned address, aligned length
 */
int32 testGdmaBase64Encode2(uint32 caseNo)
{
	struct TEST_VECTOR
	{
		char *decoded;
		char *encoded;
	}
	static testvec[] =
	{
		{
			decoded: "\
#include<stdio.h>\n\
#include<math.h>\n\
\n\
int main( int argc, char** argv )\n\
{\n\
	double dR;\n\
	double total = 0;\n\
	double i;\n\
\n\
	dR = atof( argv[1] );\n\
	printf(\"R=%lf\\n\", dR );\n\
\n\
	for( i = 1; i <= 50; i++ )\n\
	{\n\
		total += 6600 / ( pow( 1 + (dR/10000)/12.0, i ) );\n\
	}\n\
\n\
	printf(\"total=%lf\\n\", total );\n\
\n\
	return 0;\n\
}\n",
			encoded: "\
I2luY2x1ZGU8c3RkaW8uaD4KI2luY2x1ZGU8bWF0aC5oPgoKaW50IG1haW4oIGludCBhcmdjLCBj\r\n\
aGFyKiogYXJndiApCnsKCWRvdWJsZSBkUjsKCWRvdWJsZSB0b3RhbCA9IDA7Cglkb3VibGUgaTsK\r\n\
CglkUiA9IGF0b2YoIGFyZ3ZbMV0gKTsKCXByaW50ZigiUj0lbGZcbiIsIGRSICk7CgoJZm9yKCBp\r\n\
ID0gMTsgaSA8PSA1MDsgaSsrICkKCXsKCQl0b3RhbCArPSA2NjAwIC8gKCBwb3coIDEgKyAoZFIv\r\n\
MTAwMDApLzEyLjAsIGkgKSApOwoJfQoKCXByaW50ZigidG90YWw9JWxmXG4iLCB0b3RhbCApOwoK\r\n\
CXJldHVybiAwOwp9Cg==",
		},
	};
	int i, j;
	int n, d, m; /* n:length of first block, d:#block, m:length of last block */
	int32 retval;
	int maxLen = 76;
	static int _n[] = { 1, 6, 11, 16 }; /* length of first block */
	static int _m[] = { 0, 4, 8, 12 }; /* length of last block */
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL };
	
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */

	for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
	{
		for( n = 0; n < sizeof(_n)/sizeof(_n[0]); n++ )
		{
			for( d = 2; d <= 8; d++ )
			{
				for( m = 0; m < sizeof(_m)/sizeof(_m[0]); m++ )
				{
					int x; /* for d */
					static char buf[256];
					static int l[8];

					clearGdmaBuffer(CLEAN_ALL);
					for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
					{
						retval = model_setTestTarget( _i[i] );
						if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
						
						/* clean memory */
						l[0] = l[1] = l[2] = l[3] = l[4] = l[5] = l[6] = l[7] = 0;

						/* Reset GDMA */
						WRITE_MEM32( GDMACNR, 0 );
						WRITE_MEM32( GDMACNR, GDMA_ENABLE );
							
						memcpy( enumbuf[i].src, testvec[j].decoded, strlen(testvec[j].decoded) );
						WRITE_MEM32( GDMAIMR, 0 );
						WRITE_MEM32( GDMAISR, 0xffffffff );
						WRITE_MEM32( GDMAICVL, maxLen/4-1 ); /* Line Length */
						WRITE_MEM32( GDMAICVR, 0 );
						WRITE_MEM32( GDMASBP0, (uint32)enumbuf[i].src );
						WRITE_MEM32( GDMASBL0, GDMA_LDB|strlen(testvec[j].decoded) );

						/* 1st block */
						WRITE_MEM32( GDMADBP0, (uint32)enumbuf[i].dst );
						if ( d<=2 && _m[m]==0 )
							WRITE_MEM32( GDMADBL0, GDMA_LDB|_n[n] ); /* We have ONLY one block. */
						else
							WRITE_MEM32( GDMADBL0, _n[n] ); /* We have more than one block. */
						l[0] = _n[n];
						/* 2nd...(d-1)rd block */
						for( x = 1; x < d-1; x++ )
						{
							WRITE_MEM32( GDMADBP0+x*8, (uint32)enumbuf[i].dst+256*x );
							if ( _m[m]==0 && x==d-2 )
								WRITE_MEM32( GDMADBL0+x*8, GDMA_LDB|16 ); /* We don't have any block after this. */
							else
								WRITE_MEM32( GDMADBL0+x*8, 16 ); /* We have one more block after this. */
							l[x] = 16;
						}
						/* d-rd block */
						if ( d>1 && _m[m]>0 )
						{
							WRITE_MEM32( GDMADBP0+x*8, (uint32)enumbuf[i].dst+256*x );
							WRITE_MEM32( GDMADBL0+x*8, GDMA_LDB|((_m[m]+3)&~3) ); /* Yes, this is the last block. */
							l[x] = _m[m];
						}

						modelExportComment( "***********************************************************************" );
#if 0
						sprintf( buf, "_n[%d]=%d, d=%d, _m[%d]=%d", n, _n[n], d, m, _m[m] );
						modelExportComment( buf );
#endif
						sprintf( buf, "Dest. Block = %d+%d+%d+%d+%d+%d+%d+%d", l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7] );
						modelExportComment( buf );
						WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_B64ENC|internalUsedGDMACNR );

						while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

						ASSERT( (READ_MEM32( GDMAICVL )&0xffff) >= ( l[0] + l[1] + l[2] + l[3] + l[4] + l[5] + l[6] + l[7] ) );

						/* We have two results now, compare them. */
						if ( i == 1 )
							ASSERT( compare_data_block() == SUCCESS );
					}
				}
			}
		}
	}

	return SUCCESS;
}


/*
 *	To compare model code and FPGA code.
 *	first block: unaligned address (0~3), unaligned length (1~4)
 *	second block: aligned address, aligned length (4)
 */
int32 testGdmaBase64Encode3(uint32 caseNo)
{
	struct TEST_VECTOR
	{
		char *decoded;
		char *encoded;
	}
	static testvec[] =
	{
		{
			decoded: "#include<stdio.",
			encoded: "I2luY2x1ZGU8c3RkaW8u",
		},
	};
	int32 retval;
	int maxLen = 76;
	int i, j; /* i for ic type, j for test vector */
	int ns, nd, as, ad; /* n:length of first block, d:#block, m:length of last block */
	static int _as[] = { 0, 1, 2, 3 }; /* align address of first source block */
	static int _ad[] = { 0, 1, 2, 3 }; /* align address of first destination block */
	static int _ns[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }; /* length of first source block */
	static int _nd[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 }; /* length of first destination block */
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */

	for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
	{
		for( ns = 0; ns < sizeof(_ns)/sizeof(_ns[0]); ns++ )
		{
			for( as = 0; as < sizeof(_as)/sizeof(_as[0]); as++ )
			{
				for( nd = 0; nd < sizeof(_nd)/sizeof(_nd[0]); nd++ )
				{
					for( ad = 0; ad < sizeof(_ad)/sizeof(_ad[0]); ad++ )
					{
						clearGdmaBuffer(CLEAN_ALL);
						for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
						{
							static char buf[256];
							
							retval = model_setTestTarget( _i[i] );
							if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

							/* Reset GDMA */
							WRITE_MEM32( GDMACNR, 0 );
							WRITE_MEM32( GDMACNR, GDMA_ENABLE );
								
							memcpy( enumbuf[i].src+_as[as], testvec[j].decoded, _ns[ns] );
							memcpy( enumbuf[i].src+256, testvec[j].decoded+_ns[ns], strlen(testvec[j].decoded)-_ns[ns] );

							WRITE_MEM32( GDMAIMR, 0 );
							WRITE_MEM32( GDMAISR, 0xffffffff );
							WRITE_MEM32( GDMAICVL, maxLen/4-1 ); /* Line Length */
							WRITE_MEM32( GDMAICVR, 0 );

							/* source blocks */
							WRITE_MEM32( GDMASBP0, (uint32)enumbuf[i].src+_as[as] );
							WRITE_MEM32( GDMASBL0, _ns[ns] );
							WRITE_MEM32( GDMASBP1, (uint32)enumbuf[i].src+256);
							WRITE_MEM32( GDMASBL1, GDMA_LDB|(strlen(testvec[j].decoded)-_ns[ns]) );

							/* destination blocks */
							WRITE_MEM32( GDMADBP0, (uint32)enumbuf[i].dst+_ad[ad] );
							WRITE_MEM32( GDMADBL0, _nd[nd] ); /* We have more than one block. */
							WRITE_MEM32( GDMADBP1, (uint32)enumbuf[i].dst+256 );
							WRITE_MEM32( GDMADBL1, GDMA_LDB|((strlen(testvec[j].encoded)-_nd[nd]+7)&~3) ); /* Yes, this is the last block. */

							modelExportComment( "***********************************************************************" );
#if 0
							sprintf( buf, "_nd[%d]=%d, d=%d, _m[%d]=%d", n, _nd[n], d, m, _m[m] );
							modelExportComment( buf );
#endif
							sprintf( buf, "tv[%d]  first source=Align(%d),Length(%d)  first dest=Align(%d),Length(%d)", j, _as[as], _ns[ns], _ad[ad], _nd[nd] );
							modelExportComment( buf );
							WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_B64ENC|internalUsedGDMACNR );

							while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

							IS_EQUAL_INT("Output length not match!  real,expected=", READ_MEM32(GDMAICVL)&0xffff, strlen(testvec[j].encoded), __FUNCTION__, __LINE__ );

							/* We have two results now, compare them. */
							if ( i == 1 )
								ASSERT( compare_data_block() == SUCCESS );
						}					
					}
				}
			}
		}
	}

	return SUCCESS;
}


/*
 * Test fixed pattern (processed length is equal to strlen(encoded), and outputted length is equal to strlen(decoded).)
 */
int32 testGdmaBase64Decode1(uint32 caseNo)
{
	struct TEST_VECTOR
	{
		char *decoded;
		char *encoded;
	}
	static testvec[] =
	{
		{
			decoded: "\x01\x02\x04",
			encoded: "\x41\x51\x49\x45",
		},
		{
			decoded: "\n",
			encoded: "Cg=",
		},
		{
			decoded: "01",
			encoded: "MDE=",
		},
		{
			decoded: "\x01\x02\x04",
			encoded: "=\x41===\x51-----\x49*****\x45",
		},
		{
			decoded: "\x01\x02\x04",
			encoded: "-----\x41()\x51\\\x49\x45*****\r\n",
		},
		{
			decoded: "\n",
			encoded: "=Cg=",
		},
		{
			decoded: "\n",
			encoded: "=C=g=",
		},
		{
			decoded: "\n",
			encoded: "=#C---=g=",
		},
		{
			decoded: "01",
			encoded: "MDE=",
		},
		{
			decoded: "01",
			encoded: "M=---D---E=",
		},
		{
			decoded: "\xFF\xFF\xFF\xFF",
			encoded: "/////w=",
		},
		{
			decoded: "\xFF\xFF\xFF\xFF",
			encoded: "\\/\\/\\/\\/\\/w=",
		},
		{
			decoded: "\x04\x10\x41\x08\x20\x82",
			encoded: "BBBB=CCCC",
		},
		{
			decoded: "\x04\x10\x41\x08\x20\x82",
			encoded: "BBBB==CCCC",
		},
		{
			decoded: "\x04\x10\x41\x08\x20\x82",
			encoded: "BBBBC=CCC",
		},
		{
			decoded: "\x04\x10\x41\x08\x20\x82",
			encoded: "BBBBC==CCC",
		},
		{
			decoded: "\x04\x10\x41\x08\x20\x82",
			encoded: "BBBB=====CCCC",
		},
		{
			decoded: "This is a test for base64\n",
			encoded: "VGhpcyBpcyBhIHRlc3QgZm9yIGJhc2U2NAo=",
		},
		{
			decoded: "\
#include<stdio.h>\n\
#include<math.h>\n\
\n\
int main( int argc, char** argv )\n\
{\n\
	double dR;\n\
	double total = 0;\n\
	double i;\n\
\n\
	dR = atof( argv[1] );\n\
	printf(\"R=%lf\\n\", dR );\n\
\n\
	for( i = 1; i <= 50; i++ )\n\
	{\n\
		total += 6600 / ( pow( 1 + (dR/10000)/12.0, i ) );\n\
	}\n\
\n\
	printf(\"total=%lf\\n\", total );\n\
\n\
	return 0;\n\
}\n",
			encoded: "\
I2luY2x1ZGU8c3RkaW8uaD4KI2luY2x1ZGU8bWF0aC5oPgoKaW50IG1haW4oIGludCBhcmdjLCBj\r\n\
aGFyKiogYXJndiApCnsKCWRvdWJsZSBkUjsKCWRvdWJsZSB0b3RhbCA9IDA7Cglkb3VibGUgaTsK\r\n\
CglkUiA9IGF0b2YoIGFyZ3ZbMV0gKTsKCXByaW50ZigiUj0lbGZcbiIsIGRSICk7CgoJZm9yKCBp\r\n\
ID0gMTsgaSA8PSA1MDsgaSsrICkKCXsKCQl0b3RhbCArPSA2NjAwIC8gKCBwb3coIDEgKyAoZFIv\r\n\
MTAwMDApLzEyLjAsIGkgKSApOwoJfQoKCXByaW50ZigidG90YWw9JWxmXG4iLCB0b3RhbCApOwoK\r\n\
CXJldHVybiAwOwp9Cg=",
		},
	};
	int i, j;
	int32 retval;
	int maxLen = 76;
	
	testGdmaInit();

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
		{
			/* clean memory */
			clearGdmaBuffer(CLEAN_ALL);
			
			/* Reset GDMA */
			WRITE_MEM32( GDMACNR, 0 );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE );
				
			memcpy( sb2, testvec[j].encoded, strlen(testvec[j].encoded) );
			WRITE_MEM32( GDMAIMR, 0 );
			WRITE_MEM32( GDMAISR, 0xffffffff );
			WRITE_MEM32( GDMAICVL, maxLen/4-1 ); /* Line Length */
			WRITE_MEM32( GDMAICVR, 0 );
			WRITE_MEM32( GDMASBP0, (uint32)sb2 );
			WRITE_MEM32( GDMASBL0, GDMA_LDB|strlen(testvec[j].encoded) );
			WRITE_MEM32( GDMADBP0, (uint32)db2 );
			WRITE_MEM32( GDMADBL0, GDMA_LDB|strlen(testvec[j].decoded) );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_B64DEC|internalUsedGDMACNR );

			/* Expected data */
			memcpy( sb1, testvec[j].encoded, strlen(testvec[j].encoded) );
			memcpy( db1, testvec[j].decoded, strlen(testvec[j].decoded) );

			while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

#if 0
			if ( testvec[j].encoded[strlen(testvec[j].encoded)-1] == '=' )
				IS_EQUAL_INT("Source block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, 0, __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
			IS_EQUAL_INT("Processed length not match!  real,expected=", READ_MEM32(GDMAICVL)>>16, strlen(testvec[j].encoded), __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Output length not match!  real,expected=", READ_MEM32(GDMAICVL)&0xffff, strlen(testvec[j].decoded), __FUNCTION__, __LINE__ );
			ASSERT( compare_data_block() == SUCCESS );
		}
	}

	return SUCCESS;
}


/*
 * Test fixed pattern (test processed and outputted length)
 */
int32 testGdmaBase64Decode2(uint32 caseNo)
{
	struct TEST_VECTOR
	{
		char *encoded;
		int processed;
		char *decoded;
		int outputted;
	}
	static testvec[] =
	{
#if 0 /* IC no need to support */
		{ /* 0-S */
			encoded: "",
			processed: 0,
			decoded: "",
			outputted: 0,
		},
#endif
#ifdef FULL_TEST
		{ /* 0-0-0-0-0-S */
			encoded: "====",
			processed: 4,
			decoded: "",
			outputted: 0,
		},
		{ /* 0-0-1-S */
			encoded: "=\x41",
			processed: 1,
			decoded: "",
			outputted: 0,
		},
		{ /* 0-0-1-1-S */
			encoded: "=\x41=",
			processed: 1,
			decoded: "",
			outputted: 0,
		},
		{ /* 0-0-1-2-S */
			encoded: "=\x41\x51",
			processed: 1,
			decoded: "",
			outputted: 0,
		},
		{ /* 0-0-1-2-2-S */
			encoded: "-\x41\x51-",
			processed: 1,
			decoded: "",
			outputted: 0,
		},
		{ /* 0-0-1-2-EQ */
			encoded: "=\x41\x51=",
			processed: 4,
			decoded: "\x01",
			outputted: 1,
		},
		{ /* 0-0-1-2-2-EQ */
			encoded: "-\x41\x51-=",
			processed: 5,
			decoded: "\x01",
			outputted: 1,
		},
		{ /* 0-0-1-2-3-S */
			encoded: "=\x41\x51\x49",
			processed: 1,
			decoded: "",
			outputted: 0,
		},
		{ /* 0-0-1-2-3-3-3-S */
			encoded: "=\x41\x51\x49\n\r",
			processed: 1,
			decoded: "",
			outputted: 0,
		},
		{ /* 0-0-1-2-3-E */
			encoded: "=\x41\x51\x49=",
			processed: 5,
			decoded: "\x01\x02",
			outputted: 2,
		},
		{ /* 0-0-1-2-3-3-3-E */
			encoded: "=\x41\x51\x49\r\n=",
			processed: 7,
			decoded: "\x01\x02",
			outputted: 2,
		},
		{ /* 0-0-1-1-2-2-3-3-3-E */
			encoded: "=\x41-\x51-\x49\r\n=",
			processed: 9,
			decoded: "\x01\x02",
			outputted: 2,
		},
		{ /* 0-0-1-1-1-1-1-1-2-2-2-2-2-2-3-3-3-3-3-3-3-E */
			encoded: "=\x41-----\x51-----\x49\r\n\r\n\r\n=",
			processed: 21,
			decoded: "\x01\x02",
			outputted: 2,
		},
		{ /* 0-0-1-2-3-0-S */
			encoded: "=\x41\x51\x49\x45",
			processed: 5,
			decoded: "\x01\x02\x04",
			outputted: 3,
		},
		{ /* 0-0-1-1-2-2-3-3-0-S */
			encoded: "-\x41-\x51-\x49-\x45",
			processed: 8,
			decoded: "\x01\x02\x04",
			outputted: 3,
		},
		{ /* 0-0-1-2-3-0-0-S */
			encoded: "=\x41\x51\x49\x45=",
			processed: 6,
			decoded: "\x01\x02\x04",
			outputted: 3,
		},
		{ /* 0-0-1-2-3-0-0-1-1-S */
			encoded: "=\x41\x51\x49\x45=/=",
			processed: 6,
			decoded: "\x01\x02\x04",
			outputted: 3,
		},
		{ /* 0-0-1-2-3-0-0-1-2-2-S */
			encoded: "=\x41\x51\x49\x45=//-",
			processed: 6,
			decoded: "\x01\x02\x04",
			outputted: 3,
		},
#endif
		{ /* 0-0-1-2-3-0-0-1-2-E */
			encoded: "=\x41\x51\x49\x45=//=",
			processed: 9,
			decoded: "\x01\x02\x04\xFF",
			outputted: 4,
		},
#ifdef FULL_TEST
		{ /* 0-0-1-2-3-0-0-1-2-3-3-S */
			encoded: "=\x41\x51\x49\x45=///-",
			processed: 6,
			decoded: "\x01\x02\x04",
			outputted: 3,
		},
#endif
		{ /* 0-0-1-2-3-0-0-1-2-3-E */
			encoded: "=\x41\x51\x49\x45=///=",
			processed: 10,
			decoded: "\x01\x02\x04\xFF\xFF",
			outputted: 5,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-S */
			encoded: "=\x41\x51\x49\x45=////=",
			processed: 11,
			decoded: "\x01\x02\x04\xFF\xFF\xFF",
			outputted: 6,
		},
		
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-S */
			encoded: "=\x41\x51\x49\x45=////=/",
			processed: 11,
			decoded: "\x01\x02\x04\xFF\xFF\xFF",
			outputted: 6,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-1-S */
			encoded: "=\x41\x51\x49\x45=////-/-",
			processed: 11,
			decoded: "\x01\x02\x04\xFF\xFF\xFF",
			outputted: 6,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-1-2-S */
			encoded: "=\x41\x51\x49\x45=////-/-/",
			processed: 11,
			decoded: "\x01\x02\x04\xFF\xFF\xFF",
			outputted: 6,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-1-2-E */
			encoded: "=AQIE=////-/-/=",
			processed: 15,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF",
			outputted: 7,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-S */
			encoded: "=AQIE=////-/-/-",
			processed: 11,
			decoded: "\x01\x02\x04\xFF\xFF\xFF",
			outputted: 6,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-E */
			encoded: "=AQIE=////-/-/=",
			processed: 15,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF",
			outputted: 7,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-E */
			encoded: "=AQIE=////-/-/-=",
			processed: 16,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF",
			outputted: 7,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-3-S */
			encoded: "=AQIE=////-/-/-/",
			processed: 11,
			decoded: "\x01\x02\x04\xFF\xFF\xFF",
			outputted: 6,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-3-E */
			encoded: "=AQIE=////-/-/-/=",
			processed: 17,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF",
			outputted: 8,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-3-3-S */
			encoded: "=AQIE=////-/-/-/-",
			processed: 11,
			decoded: "\x01\x02\x04\xFF\xFF\xFF",
			outputted: 6,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-3-3-E */
			encoded: "=AQIE=////-/-/-/-=",
			processed: 18,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF",
			outputted: 8,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-3-3-0-S */
			encoded: "=AQIE=////-/-/-/-/",
			processed: 18,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-3-3-0-0-S */
			encoded: "=AQIE=////-/-/-/-/-",
			processed: 19,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-3-3-0-S */
			encoded: "=AQIE=/////-//-/",
			processed: 16,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-2-3-0-S */
			encoded: "=AQIE=////////",
			processed: 14,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-2-2-3-0-0-S */
			encoded: "=AQIE=////-//-//-",
			processed: 17,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-0-1-2-2-3-0-0-0-S */
			encoded: "=AQIE=////-//-//--",
			processed: 18,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-0-0-0-S */
			encoded: "=AQIE=/////-/-//--",
			processed: 18,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-0-0-0-1-S */
			encoded: "=AQIE=/////-/-//--/",
			processed: 18,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-0-0-0-1-1-S */
			encoded: "=AQIE=/////-/-//--/-",
			processed: 18,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-0-0-0-1-1-2-S */
			encoded: "=AQIE=/////-/-//--/-/",
			processed: 18,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-0-0-0-1-1-2-E */
			encoded: "=AQIE=/////-/-//--/-/=",
			processed: 22,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 10,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-0-0-0-1-1-2-2-S */
			encoded: "=AQIE=/////-/-//--/-/-",
			processed: 18,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-0-0-0-1-1-2-2-E */
			encoded: "=AQIE=/////-/-//--/-/-=",
			processed: 23,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 10,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-0-0-0-1-1-2-2-3-S */
			encoded: "=AQIE=/////-/-//--/-/-/",
			processed: 18,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-0-0-0-1-1-2-2-3-E */
			encoded: "=AQIE=/////-/-//--/-/-/=",
			processed: 24,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 11,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-0-0-0-1-1-2-2-3-3-S */
			encoded: "=AQIE=/////-/-//--/-/-/-",
			processed: 18,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 9,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-0-0-0-1-1-2-2-3-3-E */
			encoded: "=AQIE=/////-/-//--/-/-/-=",
			processed: 25,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 11,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-0-0-0-1-1-2-2-3-3-0-S */
			encoded: "=AQIE=/////-/-//--/-/-/-/",
			processed: 25,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-0-1-1-2-2-3-3-0-0-S */
			encoded: "=AQIE=/////-/-/-/-/-/-/-/-",
			processed: 26,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-S */
			encoded: "=AQIE=/////-/-/-/-////-/",
			processed: 23,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-1-S */
			encoded: "=AQIE=/////-/-/-/-////-/-",
			processed: 23,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-S */
			encoded: "=AQIE=/////-/-/-/-////-//",
			processed: 23,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-E */
			encoded: "=AQIE=/////-/-/-/-////-//=",
			processed: 26,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 13,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-2-S */
			encoded: "=AQIE=/////-/-/-/-////-//-",
			processed: 23,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-2-E */
			encoded: "=AQIE=/////-/-/-/-////-//-=",
			processed: 27,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 13,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-S */
			encoded: "=AQIE=/////-/-/-/-////-///",
			processed: 23,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-E */
			encoded: "=AQIE=/////-/-/-/-////-///=",
			processed: 27,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 14,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-1-2-3-S */
			encoded: "=AQIE=/////-/-/-/-////-/-//",
			processed: 23,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-1-2-3-E */
			encoded: "=AQIE=/////-/-/-/-////-/-//=",
			processed: 28,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 14,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-2-2-3-3-S */
			encoded: "=AQIE=/////-/-/-/-////-//--/-",
			processed: 23,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-2-2-3-3-E */
			encoded: "=AQIE=/////-/-/-/-////-//--/-=",
			processed: 30,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 14,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-2-2-2-3-3-S */
			encoded: "=AQIE=/////-/-/-/-////-//---/-",
			processed: 23,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-2-2-2-3-3-E */
			encoded: "=AQIE=/////-/-/-/-////-//---/-=",
			processed: 31,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 14,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-2-2-2-2-3-3-S */
			encoded: "=AQIE=/////-/-/-/-////-//----/-",
			processed: 23,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-2-2-2-2-3-3-E */
			encoded: "=AQIE=/////-/-/-/-////-//----/-=",
			processed: 32,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 14,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-2-2-2-2-2-3-3-S */
			encoded: "=AQIE=/////-/-/-/-////-//-----/-",
			processed: 23,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-2-2-2-2-2-3-3-E */
			encoded: "=AQIE=/////-/-/-/-////-//-----/-=",
			processed: 33,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 14,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-3-S */
			encoded: "=AQIE=/////-/-/-/-////-///-",
			processed: 23,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 12,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-3-E */
			encoded: "=AQIE=/////-/-/-/-////-///-=",
			processed: 28,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
			outputted: 14,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-S */
			encoded: "=AQIE=/////-/-/-/-////-AQIE",
			processed: 27,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04",
			outputted: 15,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-S */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=",
			processed: 28,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04",
			outputted: 15,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-S */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=/",
			processed: 28,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04",
			outputted: 15,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-1-S */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=/-",
			processed: 28,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04",
			outputted: 15,
		},
		
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-2-S */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=//",
			processed: 28,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04",
			outputted: 15,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-2-E */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=//=",
			processed: 31,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04\xFF",
			outputted: 16,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-1-2-S */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=/-/",
			processed: 28,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04",
			outputted: 15,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-1-2-E */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=/-/=",
			processed: 32,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04\xFF",
			outputted: 16,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-2-2-S */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=//-",
			processed: 28,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04",
			outputted: 15,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-2-2-E */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=//-=",
			processed: 32,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04\xFF",
			outputted: 16,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-S */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=/-/-",
			processed: 28,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04",
			outputted: 15,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-E */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=/-/-=",
			processed: 33,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04\xFF",
			outputted: 16,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-2-3-S */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=///",
			processed: 28,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04",
			outputted: 15,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-2-3-E */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=///=",
			processed: 32,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04\xFF\xFF",
			outputted: 17,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-3-3-3-S */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=/-/-/--",
			processed: 28,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04",
			outputted: 15,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-1-2-2-3-3-3-E */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=/-/-/--=",
			processed: 36,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04\xFF\xFF",
			outputted: 17,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-2-3-0-S */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=////",
			processed: 32,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04\xFF\xFF\xFF",
			outputted: 18,
		},
		{ /* 0-0-1-2-3-0-0-1-2-3-0-1-1-2-2-3-3-0-1-2-3-0-0-1-2-3-0-0-1-2-3-3-3-3-3-3-0-0-0-0-0-0-S */
			encoded: "=AQIE=/////-/-/-/-////-AQIE=///-----/-----",
			processed: 42,
			decoded: "\x01\x02\x04\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x02\x04\xFF\xFF\xFF",
			outputted: 18,
		},
	};
	int i, j;
	int32 retval;
	int maxLen = 76;
	
	testGdmaInit();

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
		{
			/* clean memory */
			clearGdmaBuffer(CLEAN_ALL);
			
			/* Reset GDMA */
			WRITE_MEM32( GDMACNR, 0 );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE );
				
			memcpy( sb2, testvec[j].encoded, strlen(testvec[j].encoded) );
			WRITE_MEM32( GDMAIMR, 0 );
			WRITE_MEM32( GDMAISR, 0xffffffff );
			WRITE_MEM32( GDMAICVL, maxLen/4-1 ); /* Line Length */
			WRITE_MEM32( GDMAICVR, 0 );
			WRITE_MEM32( GDMASBP0, (uint32)sb2 );
			WRITE_MEM32( GDMASBL0, GDMA_LDB|strlen(testvec[j].encoded) );
			WRITE_MEM32( GDMADBP0, (uint32)db2 );
			if ( strlen(testvec[j].decoded) == 0 ) /* IC does NOT accept destination length is zero. see spec. */
				WRITE_MEM32( GDMADBL0, GDMA_LDB|1 );
			else
				WRITE_MEM32( GDMADBL0, GDMA_LDB|strlen(testvec[j].decoded) );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_B64DEC|internalUsedGDMACNR );

			/* Expected data */
			memcpy( sb1, testvec[j].encoded, strlen(testvec[j].encoded) );
			memcpy( db1, testvec[j].decoded, strlen(testvec[j].decoded) );

			while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

			/*memDump( sb2, strlen(testvec[j].encoded), "sd2" );*/
			/*memDump( db2, strlen(testvec[j].decoded), "dd2" );*/
#if 0
			if ( testvec[j].encoded[strlen(testvec[j].encoded)-1] == '=' )
				IS_EQUAL_INT("Source block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, 0, __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
			IS_EQUAL_INT("Processed length not match!  real,expected=", READ_MEM32(GDMAICVL)>>16, testvec[j].processed, __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Output length not match!  real,expected=", READ_MEM32(GDMAICVL)&0xffff, testvec[j].outputted, __FUNCTION__, __LINE__ );
			ASSERT( compare_data_block() == SUCCESS );
		}
	}

	return SUCCESS;
}


//#ifdef RTL865X_MODEL_USER
#if 0
/*
 *	Thank to Limit's pattern
 *	We will use realworld case to test.
 */
#undef _DEBUG_BASE64_ENCDEC1_
#define HUGE_BUFSIZ (1024*1024)
static uint8 HugeBuffer1[HUGE_BUFSIZ];
static uint8 HugeBuffer2[HUGE_BUFSIZ];
static uint8 expectedEncoded[HUGE_BUFSIZ];
static int32 expectedEncLen;
static uint8 expectedDecoded[HUGE_BUFSIZ];
static int32 expectedDecLen;
int32 testGdmaBase64EncDec1(uint32 caseNo)
{
	struct TEST_VECTOR
	{
		char *decoded;
		char *encoded;
	}
	static testvec[] =
	{
		{
			decoded: "Limit/b64-00.dec",
			encoded: "Limit/b64-00.enc",
		},
		{
			decoded: "Limit/b64-01.dec",
			encoded: "Limit/b64-01.enc",
		},
		{
			decoded: "Limit/b64-02.dec",
			encoded: "Limit/b64-02.enc",
		},
		{
			decoded: "Limit/b64-03.dec",
			encoded: "Limit/b64-03.enc",
		},
		{
			decoded: "Limit/b64-10.dec",
			encoded: "Limit/b64-10.enc",
		},
		{
			decoded: "Limit/b64-11.dec",
			encoded: "Limit/b64-11.enc",
		},
		{
			decoded: "Limit/b64-12.dec",
			encoded: "Limit/b64-12.enc",
		},
		{
			decoded: "Limit/b64-13.dec",
			encoded: "Limit/b64-13.enc",
		},
		{
			decoded: "Limit/b64-14.dec",
			encoded: "Limit/b64-14.enc",
		},
		{
			decoded: "Limit/b64-15.dec",
			encoded: "Limit/b64-15.enc",
		},
		{
			decoded: "Limit/b64-16.dec",
			encoded: "Limit/b64-16.enc",
		},
		{
			decoded: "Limit/b64-17.dec",
			encoded: "Limit/b64-17.enc",
		},
		{
			decoded: "Limit/b64-18.dec",
			encoded: "Limit/b64-18.enc",
		},
		{
			decoded: "Limit/b64-19.dec",
			encoded: "Limit/b64-19.enc",
		},
		{
			decoded: "Limit/b64-1a.dec",
			encoded: "Limit/b64-1a.enc",
		},
		{
			decoded: "Limit/b64-1b.dec",
			encoded: "Limit/b64-1b.enc",
		},
		{
			decoded: "Limit/b64-1c.dec",
			encoded: "Limit/b64-1c.enc",
		},
		{
			decoded: "Limit/b64-1d.dec",
			encoded: "Limit/b64-1d.enc",
		},
		{
			decoded: "Limit/b64-1e.dec",
			encoded: "Limit/b64-1e.enc",
		},
		{
			decoded: "Limit/b64-1f.dec",
			encoded: "Limit/b64-1f.enc",
		},
		{
			decoded: "Limit/b64-20.dec",
			encoded: "Limit/b64-20.enc",
		},
		{
			decoded: "Limit/b64-21.dec",
			encoded: "Limit/b64-21.enc",
		},
		{
			decoded: "Limit/b64-22.dec",
			encoded: "Limit/b64-22.enc",
		},
		{
			decoded: "Limit/b64-23.dec",
			encoded: "Limit/b64-23.enc",
		},
		{
			decoded: "Limit/b64-24.dec",
			encoded: "Limit/b64-24.enc",
		},
		{
			decoded: "Limit/b64-25.dec",
			encoded: "Limit/b64-25.enc",
		},
		{
			decoded: "Limit/b64-26.dec",
			encoded: "Limit/b64-26.enc",
		},
		{
			decoded: "Limit/b64-30.dec",
			encoded: "Limit/b64-30.enc",
		},
		{
			decoded: "Limit/b64-31.dec",
			encoded: "Limit/b64-31.enc",
		},
		{
			decoded: "Limit/b64-32.dec",
			encoded: "Limit/b64-32.enc",
		},
		{
			decoded: "Limit/b64-33.dec",
			encoded: "Limit/b64-33.enc",
		},
		{
			decoded: "Limit/b64-34.dec",
			encoded: "Limit/b64-34.enc",
		},
		{
			decoded: "Limit/b64-35.dec",
			encoded: "Limit/b64-35.enc",
		},
		{
			decoded: "Limit/b64-36.dec",
			encoded: "Limit/b64-36.enc",
		},
		{
			decoded: "Limit/b64-37.dec",
			encoded: "Limit/b64-37.enc",
		},
		{
			decoded: "Limit/b64-38.dec",
			encoded: "Limit/b64-38.enc",
		},
		{
			decoded: "Limit/b64-39.dec",
			encoded: "Limit/b64-39.enc",
		},
		{
			decoded: "Limit/b64-3a.dec",
			encoded: "Limit/b64-3a.enc",
		},
		{
			decoded: "Limit/b64-3b.dec",
			encoded: "Limit/b64-3b.enc",
		},
	};
	int i, j;
	int fdDecoded;
#ifdef _DEBUG_BASE64_ENCDEC1_
	int fdDebug;
#endif
	int fdEncoded;
	int32 retval;

	testGdmaInit();

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */
		
		for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
		{
			fdEncoded = open( testvec[j].encoded, O_RDONLY );
			fdDecoded = open( testvec[j].decoded, O_RDONLY );
#ifdef _DEBUG_BASE64_ENCDEC1_
			fdDebug = open( "debug.txt", O_CREAT| O_WRONLY|O_TRUNC, 0666 );
#endif

			if ( fdEncoded == -1 || fdDecoded == -1 )
			{
				if ( fdEncoded != -1 ) close( fdEncoded );
				else rtlglue_printf( "open('%s') error\n", testvec[j].encoded );
				if ( fdDecoded != -1 ) close( fdDecoded );
				else rtlglue_printf( "open('%s') error\n", testvec[j].decoded );
#if 1 /* Louis: it is ok for open error. Because only I have these test pattern. */
				retval = SUCCESS;
#else
				retval = FAILED;
#endif
				goto out;
			}

			{
				int32 inLen;
				int32 outLen;
				int32 outputted;
				int32 processed;
				int n;
				uint8 *buf1 = &HugeBuffer1[0];
				uint8 *buf2 = &HugeBuffer2[0];

				/* load raw data */
				expectedDecLen = read( fdDecoded, expectedDecoded, HUGE_BUFSIZ );
				ASSERT( expectedDecLen > 0 );
				ASSERT( expectedDecLen < HUGE_BUFSIZ ); /* We expect the whole file is smaller than buffer. */

				expectedEncLen = read( fdEncoded, expectedEncoded, HUGE_BUFSIZ );
				ASSERT( expectedEncLen > 0 );
				ASSERT( expectedEncLen < HUGE_BUFSIZ ); /* We expect the whole file is smaller than buffer. */

#if 0
				/*=========================================================
				 * Test Encode
				 *
				 *=========================================================*/

				inLen = expectedDecLen;
				processed = 0;
				outputted = 0;
				while ( processed < inLen )
				{
					int32 p, o;
					int32 remindLen;
					
					outLen = HUGE_BUFSIZ;
					if ( inLen-processed >= 47823/*(MAX_DEST_LENGTH*19/26/3*3)*/ )
						remindLen = 47823/*(MAX_DEST_LENGTH*19/26/3*3)*/;
					else
						remindLen = inLen-processed;
					/*rtlglue_printf( "raw:%p(%d) enc:%p(%d) ==> ", &buf1[processed], remindLen, &buf2[outputted], outLen-outputted );*/
					rtl865x_b64enc( &expectedDecoded[processed], remindLen, &buf2[outputted], outLen-outputted, &p, &o );
					/*rtlglue_printf( "proc=%d, outp=%d\n", p, o );*/
					ASSERT( p > 0 );

					processed += p;
					outputted += o;

					if ( remindLen == 47823 )
					{
						/* add CR and LF at tail of buffer */
						buf2[outputted++] = '\r';
						buf2[outputted++] = '\n';
					}
				}
				/* buf1: not used
				 * buf2: encoded data
				 */
#ifdef _DEBUG_BASE64_ENCDEC1_
				if ( -1 == write( fdDebug, buf2, outputted ) ) rtlglue_printf( "write(%d) debug info error (outputted=%d)!\n", fdDebug, outputted );
				close( fdDebug );
#endif

				IS_EQUAL_INT("Processed length is not match! {expect,real}=", expectedDecLen, processed, __FUNCTION__, __LINE__ );
				IS_EQUAL_INT("Outputted length is not match! {expect,real}=", expectedEncLen, outputted, __FUNCTION__, __LINE__ );
				for( n = 0; n < expectedEncLen; n++ )
				{
					if ( expectedEncoded[n] != buf2[n] )
					{
						rtlglue_printf( "Offset %d is different: Expect is 0x%02x, but fact is 0x%02x.\n", n, expectedEncoded[n], buf2[n] );
						memDump( &expectedEncoded[n-15], 0x31, "Expected (partial data)" );
						memDump( &buf2[n-15], 0x31, "Expected (partial data)" );
						break;
					}
				}
#else
				outputted = expectedEncLen;
				memcpy( buf2, expectedEncoded, expectedEncLen );
#endif

				/*=========================================================
				 * Test Decode
				 *
				 *=========================================================*/
				inLen = outputted;
				processed = 0;
				outputted = 0;
				while ( processed < inLen )
				{
					int32 p, o;
					int32 remindLen;
					
					outLen = HUGE_BUFSIZ;
					if ( inLen-processed >= MAX_DEST_LENGTH )
						remindLen = MAX_DEST_LENGTH;
					else
						remindLen = inLen-processed;
					/*rtlglue_printf( "raw:%p(%d) enc:%p(%d) ==> ", &buf1[processed], remindLen, &buf2[outputted], outLen-outputted );*/
					rtl865x_b64dec( &buf2[processed], remindLen, &buf1[outputted], outLen-outputted, &p, &o );
					/*rtlglue_printf( "proc=%d, outp=%d\n", p, o );*/
					ASSERT( p > 0 );

					processed += p;
					outputted += o;
				}
				/* buf1: decoded data
				 * buf2: encoded data
				 */

#ifdef _DEBUG_BASE64_ENCDEC1_
				if ( -1 == write( fdDebug, buf2, outputted ) ) rtlglue_printf( "write(%d) debug info error (outputted=%d)!\n", fdDebug, outputted );
				close( fdDebug );
#endif

				IS_EQUAL_INT("Processed length is not match! {expect,real}=", expectedEncLen, processed, __FUNCTION__, __LINE__ );
				IS_EQUAL_INT("Outputted length is not match! {expect,real}=", expectedDecLen, outputted, __FUNCTION__, __LINE__ );
				for( n = 0; n < expectedDecLen; n++ )
				{
					if ( buf1[n] != expectedDecoded[n] )
					{
						rtlglue_printf( "Offset %d is different: Expect is 0x%02x, but fact is 0x%02x.\n", n, buf1[n], expectedDecoded[n] );
						memDump( &buf1[n-15], 0x31, "Expected (partial data)" );
						memDump( &expectedDecoded[n-15], 0x31, "Expected (partial data)" );
						break;
					}
				}
				
			}

			close( fdEncoded );
			close( fdDecoded );
		}
	}

	retval = SUCCESS;
out:
	return retval;
}
#endif


/*
 *	Use random data to compare FPGA and model code.
 */
int32 testGdmaBase64EncDec2(uint32 caseNo)
{
	int i, j;
	uint32 *p0, *p1;
	int32 retval;
	int loop;
	int32 processed[2], outputted[2];
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */

	/* loop 1 times ==> about 10 seconds */
	for( loop = 0; loop < 300; loop++ )
	{
		rtlglue_printf( "[%d-", loop );
		/* We will generate first 2 data block with random data */
		p0 = (uint32*)&enumbuf[0].src[0];
		p1 = (uint32*)&enumbuf[1].src[0];
		for( j = 0; j < MAX_BLOCK_LENGTH*2/sizeof(uint32); j++ )
		{
			*(p0++) = *(p1++) = rtlglue_random();
		}
		
		for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
		{
			rtlglue_printf( "%d-", i );
			retval = model_setTestTarget( _i[i] );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			retval = rtl865x_b64enc( &enumbuf[i].src[0], MAX_BLOCK_LENGTH*2,
			                         &enumbuf[i].dst[0], MAX_BLOCK_LENGTH*8,
			                         &processed[i], &outputted[i] );
			if ( retval != SUCCESS )
			{
				rtlglue_printf( "i=%d L%d: rtl865x_b64enc() error: retval=%d\n", i, __LINE__, retval );
				return retval;
			}

			if ( processed[i]<=0 || outputted[i]<=0 )
			{
				rtlglue_printf( "i=%d L%d: rtl865x_b64enc() error: processed=%d, outputted=%d\n", i, __LINE__, processed[i], outputted[i] );
				return FAILED;
			}
		}

		rtlglue_printf( "]" );
		if ( processed[0] != processed[1] )
		{
			rtlglue_printf( "L%d: processed length is not the same: processed[0]=%d, processed[1]=%d\n", __LINE__, processed[0], processed[1] );
			return FAILED;
		}
		if ( outputted[0] != outputted[1] )
		{
			rtlglue_printf( "L%d: outputted length is not the same: outputted[0]=%d, outputted[1]=%d\n", __LINE__, outputted[0], outputted[1] );
			return FAILED;
		}
		ASSERT( compare_data_block() == SUCCESS );

		for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
		{
			retval = model_setTestTarget( _i[i] );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			retval = rtl865x_b64dec( &enumbuf[i].dst[0], outputted[i],
			                         &enumbuf[i].src[0], processed[i]+32/* we need to reserve FIFO length for IC */,
			                         &processed[i], &outputted[i] );
			if ( retval != SUCCESS )
			{
				rtlglue_printf( "i=%d L%d: rtl865x_b64dec() error: retval=%d\n", i, __LINE__, retval );
				return retval;
			}

			if ( processed[i]<=0 || outputted[i]<=0 )
			{
				rtlglue_printf( "i=%d L%d: rtl865x_b64dec() error: processed=%d, outputted=%d\n", i, __LINE__, processed[i], outputted[i] );
				return FAILED;
			}
		}

		ASSERT( compare_data_block() == SUCCESS );
		if ( processed[0] != processed[1] )
		{
			rtlglue_printf( "L%d: processed length is not the same: processed[0]=%d, processed[1]=%d\n", __LINE__, processed[0], processed[1] );
			return FAILED;
		}
		if ( outputted[0] != outputted[1] )
		{
			rtlglue_printf( "L%d: outputted length is not the same: outputted[0]=%d, outputted[1]=%d\n", __LINE__, outputted[0], outputted[1] );
			return FAILED;
		}
	}

	return SUCCESS;
}


/*
 * Test fixed pattern
 */
int32 testGdmaQuotedPrintableEncode1(uint32 caseNo)
{
	struct TEST_VECTOR
	{
		char *decoded;
		char *encoded;
	}
	static testvec[] =
	{
		{
			decoded: "=",
			encoded: "=3D",
		},
		{
			decoded: "0123456789012345678901234567890123456789012345678901234567890123456789012\xAA",
			encoded: "0123456789012345678901234567890123456789012345678901234567890123456789012=\r\n=AA",
		},
		{
			decoded: "01234567890123456789012345678901234567890123456789012345678901234567890123\xAA",
			encoded: "01234567890123456789012345678901234567890123456789012345678901234567890123=\r\n=AA",
		},
		{
			decoded: "012345678901234567890123456789012345678901234567890123456789012345678901234\xAA",
			encoded: "012345678901234567890123456789012345678901234567890123456789012345678901234=\r\n=AA",
		},
		{
			decoded: "\n0123456789012345678901234567890123456789012345678901234567890123456789012345\r0123456789012345678901234567890123456789012345678901234567890123456789012345",
			encoded: "\n012345678901234567890123456789012345678901234567890123456789012345678901234=\r\n5\r012345678901234567890123456789012345678901234567890123456789012345678901234=\r\n5",
		},
		{
			decoded: "三國演義",
			encoded: "=A4T=B0=EA=BAt=B8q",
		},
		{
			decoded: "\x0a\x0d\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f""0123456789\x3a\x3b\x3c\x3e\x3f\x40""ABCDEFGHIJKLMNOPQRSTUVWXYZ\x5b\x5c\x5d\x5e\x5f\x60""abcdefghijklmnopqrstuvwxyz\x7b\x7c\x7d\x7e",
			encoded: "\x0a\x0d\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f""0123456789\x3a\x3b\x3c\x3e\x3f\x40""ABCDEFGHIJKLMNOPQRSTUVWXYZ\x5b\x5c\x5d\x5e\x5f\x60""abcdefghijkl=\r\nmnopqrstuvwxyz\x7b\x7c\x7d\x7e",
		},
		{
			decoded: "Fw:  [密技] 投籃機1分鐘78顆球!?",
			encoded: "Fw:=20=20[=B1K=A7=DE]=20=A7=EB=C4x=BE=F71=A4=C0=C4=C178=C1=FB=B2y!?",
		},
		{
			decoded: "\
毒瘤這時大叫一聲衝跳進投籃機裡，開始用右手瘋狂抽插籃框！一開始大家被他這個舉動嚇了一跳，不知道他想幹麻？只是覺得他的行為有點猥褻。\r\n\
這時我們聽到投籃機瘋狂發出進籃的音效「??、??、??」，還看到上面的球數顯示，10顆、20顆、25顆、34顆這樣瘋狂提升！我們才知道他在幹嘛。那種感覺真有夠像基紐隊長用計量器看孫悟空的戰鬥力數值快速提升。\r\n\
毒瘤右手插累了換左手，左手插累了又換右手，我們在外頭不停狂笑，還不忘提醒他：「剩下5秒了！5、4、3、2.............」正當大家數到2時，毒瘤突然大叫一聲： 「一哭喲~~~~~~~~~~！(日文：去了~~~~~~~~~~！)」\r\n\
然後時間一到右手軟在籃框裡。毒瘤這時喘呼呼的問大家：「怎麼樣！厲害吧！」\r\n",
			encoded: "\
=ACr=BDF=B3o=AE=C9=A4j=A5s=A4@=C1n=BD=C4=B8=F5=B6i=A7=EB=C4x=BE=F7=B8=CC=A1=\r\n\
A=B6}=A9l=A5=CE=A5k=A4=E2=BA=C6=A8g=A9=E2=B4=A1=C4x=AE=D8=A1I=A4@=B6}=A9l=\r\n\
=A4j=AEa=B3Q=A5L=B3o=AD=D3=C1|=B0=CA=C0~=A4F=A4@=B8=F5=A1A=A4=A3=AA=BE=B9D=\r\n\
=A5L=B7Q=B7F=B3=C2=A1H=A5u=ACO=C4=B1=B1o=A5L=AA=BA=A6=E6=AC=B0=A6=B3=C2I=B5=\r\n\
T=C1=B6=A1C\r\n\
=B3o=AE=C9=A7=DA=AD=CC=C5=A5=A8=EC=A7=EB=C4x=BE=F7=BA=C6=A8g=B5o=A5X=B6i=C4=\r\n\
x=AA=BA=AD=B5=AE=C4=A1u?\?=A1B?\?=A1B?\?=A1v=A1A=C1=D9=AC=DD=A8=EC=A4W=AD=B1=\r\n\
=AA=BA=B2y=BC=C6=C5=E3=A5=DC=A1A10=C1=FB=A1B20=C1=FB=A1B25=C1=FB=A1B34=C1=\r\n\
=FB=B3o=BC=CB=BA=C6=A8g=B4=A3=A4=C9=A1I=A7=DA=AD=CC=A4~=AA=BE=B9D=A5L=A6b=\r\n\
=B7F=B9=C0=A1C=A8=BA=BA=D8=B7P=C4=B1=AFu=A6=B3=B0=F7=B9=B3=B0=F2=AF=C3=B6=\r\n\
=A4=AA=F8=A5=CE=ADp=B6q=BE=B9=AC=DD=AE]=AE=A9=AA=C5=AA=BA=BE=D4=B0=AB=A4O=\r\n\
=BC=C6=AD=C8=A7=D6=B3t=B4=A3=A4=C9=A1C\r\n\
=ACr=BDF=A5k=A4=E2=B4=A1=B2=D6=A4F=B4=AB=A5=AA=A4=E2=A1A=A5=AA=A4=E2=B4=A1=\r\n\
=B2=D6=A4F=A4S=B4=AB=A5k=A4=E2=A1A=A7=DA=AD=CC=A6b=A5~=C0Y=A4=A3=B0=B1=A8g=\r\n\
=AF=BA=A1A=C1=D9=A4=A3=A7=D1=B4=A3=BF=F4=A5L=A1G=A1u=B3=D1=A4U5=AC=ED=A4F=\r\n\
=A1I5=A1B4=A1B3=A1B2.............=A1v=A5=BF=B7=ED=A4j=AEa=BC=C6=A8=EC2=AE=\r\n\
=C9=A1A=ACr=BDF=AC=F0=B5M=A4j=A5s=A4@=C1n=A1G=20=A1u=A4@=AD=FA=B3=E9~~~~~~~=\r\n\
~~~=A1I(=A4=E9=A4=E5=A1G=A5h=A4F~~~~~~~~~~=A1I)=A1v\r\n\
=B5M=AB=E1=AE=C9=B6=A1=A4@=A8=EC=A5k=A4=E2=B3n=A6b=C4x=AE=D8=B8=CC=A1C=ACr=\r\n\
=BDF=B3o=AE=C9=B3=DD=A9I=A9I=AA=BA=B0=DD=A4j=AEa=A1G=A1u=AB=E7=BB=F2=BC=CB=\r\n\
=A1I=BCF=AE`=A7a=A1I=A1v\r\n\
",
		},
		{	/* 75 spaces + 76 spaces + 77 spaces */
			decoded: "\
                                                                           \r\n\
                                                                            \r\n\
                                                                             \r\n\
",
			encoded: "\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20\r\n\
",
		},
		{ /* output 125 bytes */
			decoded: "                                        ",
			encoded: "=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20",
		},
		{ /* output 128 bytes */
			decoded: "                                         ",
			encoded: "=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20",
		},
		{ /* output 131 bytes */
			decoded: "                                          ",
			encoded: "=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20",
		},
		{ /* output ? bytes */
			decoded: "\x1D\x9B\xD6G)\xD6\x20H\x01                                        ",
			encoded: "=1D=9B=D6G)=D6=20H=01=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20",
		},
		{ /* output ? bytes */
			decoded: "\x1D\x9B\xD6G)\xD6\x20H\x01                                       XY",
			encoded: "=1D=9B=D6G)=D6=20H=01=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20XY",
		},
	};
	int i, j;
	int32 retval;
	int maxLen = 76;
	
	testGdmaInit();

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
		{
			/* clean memory */
			clearGdmaBuffer(CLEAN_ALL);
			
			/* Reset GDMA */
			WRITE_MEM32( GDMACNR, 0 );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE );
				
			memcpy( sb2, testvec[j].decoded, strlen(testvec[j].decoded) );
			WRITE_MEM32( GDMAIMR, 0 );
			WRITE_MEM32( GDMAISR, 0xffffffff );
			WRITE_MEM32( GDMAICVL, maxLen/4-1 ); /* Line Length */
			WRITE_MEM32( GDMAICVR, 0 );
			WRITE_MEM32( GDMASBP0, (uint32)sb2 );
			WRITE_MEM32( GDMASBL0, GDMA_LDB|strlen(testvec[j].decoded) );
			WRITE_MEM32( GDMADBP0, (uint32)db2 );
			WRITE_MEM32( GDMADBL0, GDMA_LDB|(strlen(testvec[j].encoded)) );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_QPENC|internalUsedGDMACNR );

			/* Expected data */
			memcpy( sb1, testvec[j].decoded, strlen(testvec[j].decoded) );
			memcpy( db1, testvec[j].encoded, strlen(testvec[j].encoded) );

			while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

			/*memDump( sb1, strlen(testvec[j].decoded), "sd1" );*/
			/*memDump( sb2, strlen(testvec[j].decoded), "sd2" );*/
			/*memDump( db1, strlen(testvec[j].encoded), "dd1" );*/
			/*memDump( db2, strlen(testvec[j].encoded), "dd2" );*/
#if 0
			IS_EQUAL_INT("Source block should under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, GDMA_SBUDRIP, __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
			IS_EQUAL_INT("Processed length not match!  real,expected=", READ_MEM32(GDMAICVL)>>16, strlen(testvec[j].decoded), __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Output length not match!  real,expected=", READ_MEM32(GDMAICVL)&0xffff, strlen(testvec[j].encoded), __FUNCTION__, __LINE__ );
			ASSERT( compare_data_block() == SUCCESS );
		}
	}

	return SUCCESS;
}


/*
 * Test fixed pattern (without any exception)
 */
int32 testGdmaQuotedPrintableDecode1(uint32 caseNo)
{
	struct TEST_VECTOR
	{
		char *decoded;
		char *encoded;
	}
	static testvec[] =
	{
		{
			decoded: "=",
			encoded: "=3D",
		},
		{
			decoded: "0\r\n12",
			encoded: "0\r\n12",
		},
		{
			decoded: "0123456789ABCDEF",
			encoded: "0=\r\n12=\r\n345=\r\n6789=\r\nABCDEF",
		},
		{
			decoded: "AB",
			encoded: "A=\r\nB",
		},
		{
			decoded: "三國演義",
			encoded: "=A4T=B0=EA=BAt=B8q",
		},
		{
			decoded: "\x0a\x0d\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f""0123456789\x3a\x3b\x3c\x3e\x3f\x40""ABCDEFGHIJKLMNOPQRSTUVWXYZ\x5b\x5c\x5d\x5e\x5f\x60""abcdefghijklmnopqrstuvwxyz\x7b\x7c\x7d\x7e",
			encoded: "\x0a\x0d\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f""0123456789\x3a\x3b\x3c\x3e\x3f\x40""ABCDEFGHIJKLMNOPQRSTUVWXYZ\x5b\x5c\x5d\x5e\x5f\x60""abcdefghijkl=\r\nmnopqrstuvwxyz\x7b\x7c\x7d\x7e",
		},
		{
			decoded: "Fw:  [密技] 投籃機1分鐘78顆球!?",
			encoded: "Fw:=20=20[=B1K=A7=DE]=20=A7=EB=C4x=BE=F71=A4=C0=C4=C178=C1=FB=B2y!?",
		},
		{
			decoded: "\
毒瘤這時大叫一聲衝跳進投籃機裡，開始用右手瘋狂抽插籃框！一開始大家被他這個舉動嚇了一跳，不知道他想幹麻？只是覺得他的行為有點猥褻。\r\n\
這時我們聽到投籃機瘋狂發出進籃的音效「??、??、??」，還看到上面的球數顯示，10顆、20顆、25顆、34顆這樣瘋狂提升！我們才知道他在幹嘛。那種感覺真有夠像基紐隊長用計量器看孫悟空的戰鬥力數值快速提升。\r\n\
毒瘤右手插累了換左手，左手插累了又換右手，我們在外頭不停狂笑，還不忘提醒他：「剩下5秒了！5、4、3、2.............」正當大家數到2時，毒瘤突然大叫一聲： 「一哭喲~~~~~~~~~~！(日文：去了~~~~~~~~~~！)」\r\n\
然後時間一到右手軟在籃框裡。毒瘤這時喘呼呼的問大家：「怎麼樣！厲害吧！」\r\n",
			encoded: "\
=ACr=BDF=B3o=AE=C9=A4j=A5s=A4@=C1n=BD=C4=B8=F5=B6i=A7=EB=C4x=BE=F7=B8=CC=A1=\r\n\
A=B6}=A9l=A5=CE=A5k=A4=E2=BA=C6=A8g=A9=E2=B4=A1=C4x=AE=D8=A1I=A4@=B6}=A9l=\r\n\
=A4j=AEa=B3Q=A5L=B3o=AD=D3=C1|=B0=CA=C0~=A4F=A4@=B8=F5=A1A=A4=A3=AA=BE=B9D=\r\n\
=A5L=B7Q=B7F=B3=C2=A1H=A5u=ACO=C4=B1=B1o=A5L=AA=BA=A6=E6=AC=B0=A6=B3=C2I=B5=\r\n\
T=C1=B6=A1C\r\n\
=B3o=AE=C9=A7=DA=AD=CC=C5=A5=A8=EC=A7=EB=C4x=BE=F7=BA=C6=A8g=B5o=A5X=B6i=C4=\r\n\
x=AA=BA=AD=B5=AE=C4=A1u?\?=A1B?\?=A1B?\?=A1v=A1A=C1=D9=AC=DD=A8=EC=A4W=AD=B1=\r\n\
=AA=BA=B2y=BC=C6=C5=E3=A5=DC=A1A10=C1=FB=A1B20=C1=FB=A1B25=C1=FB=A1B34=C1=\r\n\
=FB=B3o=BC=CB=BA=C6=A8g=B4=A3=A4=C9=A1I=A7=DA=AD=CC=A4~=AA=BE=B9D=A5L=A6b=\r\n\
=B7F=B9=C0=A1C=A8=BA=BA=D8=B7P=C4=B1=AFu=A6=B3=B0=F7=B9=B3=B0=F2=AF=C3=B6=\r\n\
=A4=AA=F8=A5=CE=ADp=B6q=BE=B9=AC=DD=AE]=AE=A9=AA=C5=AA=BA=BE=D4=B0=AB=A4O=\r\n\
=BC=C6=AD=C8=A7=D6=B3t=B4=A3=A4=C9=A1C\r\n\
=ACr=BDF=A5k=A4=E2=B4=A1=B2=D6=A4F=B4=AB=A5=AA=A4=E2=A1A=A5=AA=A4=E2=B4=A1=\r\n\
=B2=D6=A4F=A4S=B4=AB=A5k=A4=E2=A1A=A7=DA=AD=CC=A6b=A5~=C0Y=A4=A3=B0=B1=A8g=\r\n\
=AF=BA=A1A=C1=D9=A4=A3=A7=D1=B4=A3=BF=F4=A5L=A1G=A1u=B3=D1=A4U5=AC=ED=A4F=\r\n\
=A1I5=A1B4=A1B3=A1B2.............=A1v=A5=BF=B7=ED=A4j=AEa=BC=C6=A8=EC2=AE=\r\n\
=C9=A1A=ACr=BDF=AC=F0=B5M=A4j=A5s=A4@=C1n=A1G=20=A1u=A4@=AD=FA=B3=E9~~~~~~~=\r\n\
~~~=A1I(=A4=E9=A4=E5=A1G=A5h=A4F~~~~~~~~~~=A1I)=A1v\r\n\
=B5M=AB=E1=AE=C9=B6=A1=A4@=A8=EC=A5k=A4=E2=B3n=A6b=C4x=AE=D8=B8=CC=A1C=ACr=\r\n\
=BDF=B3o=AE=C9=B3=DD=A9I=A9I=AA=BA=B0=DD=A4j=AEa=A1G=A1u=AB=E7=BB=F2=BC=CB=\r\n\
=A1I=BCF=AE`=A7a=A1I=A1v\r\n\
",
		},
		{	/* 75 spaces + 76 spaces + 77 spaces */
			decoded: "\
                                                                           \r\n\
                                                                            \r\n\
                                                                             \r\n\
",
			encoded: "\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=\r\n\
=20=20\r\n\
",
		},
		{	/* Outlook: 75 spaces + 76 spaces + 77 spaces */
			decoded: "\
                                                                           \r\n\
                                                                            \r\n\
                                                                             \r\n\
",
			encoded: "\
                                                                         =\r\n\
 =20\r\n\
                                                                         =\r\n\
  =20\r\n\
                                                                         =\r\n\
   =20\r\n\
",
		},
		{	/* IRA News */
			decoded: "\
IRA宣佈結束對英的武裝抗爭 國際咸表歡迎 寄給朋友 　 友善列印    \r\n\
                          \r\n\
                  【中廣新聞網 】 廣　告\r\n\
                                  \r\n\
                             \r\n\
                             \r\n\
                        \
長期以暴力手段爭取南北愛爾蘭統一的「愛爾蘭共和軍」IRA，台北時間昨天深夜正式宣佈，將結束和英國長達三十年的武裝抗爭，改以政治與民主談判方式處理北愛爾蘭問題；英國首相布萊爾(新聞、網站)讚揚IRA的這項決定「具有空前的歷史意義」，國際間也深表歡迎，不過國際輿論也強調，IRA必須「言行一致」，才能證明他們確實有民主談判的誠意（葉柏毅報導）\r\n\
",
			encoded: "\
IRA=AB=C5=A7G=B5=B2=A7=F4=B9=EF=AD^=AA=BA=AAZ=B8=CB=A7=DC=AA=A7 =\r\n\
=B0=EA=BB=DA=ABw=AA=ED=C5w=AA=EF =B1H=B5=B9=AAB=A4=CD =A1@ =\r\n\
=A4=CD=B5=BD=A6C=A6L   =20\r\n\
                         =20\r\n\
                  =A1i=A4=A4=BCs=B7s=BBD=BA=F4 =A1j =BCs=A1@=A7i\r\n\
                                 =20\r\n\
                            =20\r\n\
                            =20\r\n\
                        =\r\n\
=AA=F8=B4=C1=A5H=BC=C9=A4O=A4=E2=ACq=AA=A7=A8=FA=ABn=A5_=B7R=BA=B8=C4=F5=B2=\r\n\
=CE=A4@=AA=BA=A1u=B7R=BA=B8=C4=F5=A6@=A9M=ADx=A1vIRA=A1A=A5x=A5_=AE=C9=B6=\r\n\
=A1=ACQ=A4=D1=B2`=A9]=A5=BF=A6=A1=AB=C5=A7G=A1A=B1N=B5=B2=A7=F4=A9M=AD^=B0=\r\n\
=EA=AA=F8=B9F=A4T=A4Q=A6~=AA=BA=AAZ=B8=CB=A7=DC=AA=A7=A1A=A7=EF=A5H=ACF=AA=\r\n\
v=BBP=A5=C1=A5D=BD=CD=A7P=A4=E8=A6=A1=B3B=B2z=A5_=B7R=BA=B8=C4=F5=B0=DD=C3=\r\n\
D=A1F=AD^=B0=EA=AD=BA=AC=DB=A5=AC=B5=DC=BA=B8(=B7s=BBD=A1B=BA=F4=AF=B8)=C6=\r\n\
g=B4=ADIRA=AA=BA=B3o=B6=B5=A8M=A9w=A1u=A8=E3=A6=B3=AA=C5=ABe=AA=BA=BE=FA=A5=\r\n\
v=B7N=B8q=A1v=A1A=B0=EA=BB=DA=B6=A1=A4]=B2`=AA=ED=C5w=AA=EF=A1A=A4=A3=B9L=\r\n\
=B0=EA=BB=DA=C1=D6=BD=D7=A4]=B1j=BD=D5=A1AIRA=A5=B2=B6=B7=A1u=A8=A5=A6=E6=\r\n\
=A4@=ADP=A1v=A1A=A4~=AF=E0=C3=D2=A9=FA=A5L=AD=CC=BDT=B9=EA=A6=B3=A5=C1=A5=\r\n\
D=BD=CD=A7P=AA=BA=B8=DB=B7N=A1]=B8=AD=ACf=BC=DD=B3=F8=BE=C9=A1^\r\n\
",
		},
	};
	int i, j;
	int32 retval;
	int maxLen = 76;
	
	testGdmaInit();

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
		{
			/* clean memory */
			clearGdmaBuffer(CLEAN_ALL);
			
			/* Reset GDMA */
			WRITE_MEM32( GDMACNR, 0 );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE );
				
			memcpy( sb2, testvec[j].encoded, strlen(testvec[j].encoded) );
			WRITE_MEM32( GDMAIMR, 0 );
			WRITE_MEM32( GDMAISR, 0xffffffff );
			WRITE_MEM32( GDMAICVL, maxLen/4-1 ); /* Line Length */
			WRITE_MEM32( GDMAICVR, 0 );
			WRITE_MEM32( GDMASBP0, (uint32)sb2 );
			WRITE_MEM32( GDMASBL0, GDMA_LDB|strlen(testvec[j].encoded) );
			WRITE_MEM32( GDMADBP0, (uint32)db2 );
			WRITE_MEM32( GDMADBL0, GDMA_LDB|(strlen(testvec[j].decoded)) );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_QPDEC|internalUsedGDMACNR );

			/* Expected data */
			memcpy( sb1, testvec[j].encoded, strlen(testvec[j].encoded) );
			memcpy( db1, testvec[j].decoded, strlen(testvec[j].decoded) );

			while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

			/*memDump( sb1, strlen(testvec[j].encoded), "sd1" );*/
			/*memDump( sb2, strlen(testvec[j].encoded), "sd2" );*/
			/*memDump( db1, strlen(testvec[j].decoded), "dd1" );*/
			/*memDump( db2, strlen(testvec[j].decoded), "dd2" );*/
#if 0
			IS_EQUAL_INT("Source block should under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, GDMA_SBUDRIP, __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
			IS_EQUAL_INT("Processed length not match!  real,expected=", READ_MEM32(GDMAICVL)>>16, strlen(testvec[j].encoded), __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Output length not match!  real,expected=", READ_MEM32(GDMAICVL)&0xffff, strlen(testvec[j].decoded), __FUNCTION__, __LINE__ );
			ASSERT( compare_data_block() == SUCCESS );
		}
	}

	return SUCCESS;
}


/*
 * Test fixed pattern (processed/outputted might not be equal to the length of encoded/decoded).
 */
int32 testGdmaQuotedPrintableDecode2(uint32 caseNo)
{
	struct TEST_VECTOR
	{
		char *encoded;
		int processed;
		char *decoded;
		int outputted;
	}
	static testvec[] =
	{
#if 0 /* IC no need to support */
		{ /* 0-S */
			encoded: "",
			processed: 0,
			decoded: "",
			outputted: 0,
		},
#endif
#ifdef FULL_TEST
		{ /* 0-0-S */
			encoded: "-",
			processed: 1,
			decoded: "-",
			outputted: 1,
		},
		{ /* 0-0-0-S */
			encoded: "AB",
			processed: 2,
			decoded: "AB",
			outputted: 2,
		},
		{ /* 0-0-1-S */
			encoded: "C=",
			processed: 1,
			decoded: "C",
			outputted: 1,
		},
		{ /* 0-0-1-n */
			encoded: "0==",
			processed: 1,
			decoded: "0",
			outputted: 1,
		},
		{ /* 0-0-1-3-S */
			encoded: "1=\r",
			processed: 1,
			decoded: "1",
			outputted: 1,
		},
		{ /* 0-0-1-3-n */
			encoded: "1=\r\b",
			processed: 1,
			decoded: "1",
			outputted: 1,
		},
		{ /* 0-0-1-3-0-S */
			encoded: "-=\r\n",
			processed: 4,
			decoded: "-",
			outputted: 1,
		},
		{ /* 0-0-1-3-0-0-S */
			encoded: "-=\r\n*",
			processed: 5,
			decoded: "-*",
			outputted: 2,
		},
		{ /* 0-0-1-2-S */
			encoded: "-=4",
			processed: 1,
			decoded: "-",
			outputted: 1,
		},
		{ /* 0-0-1-2-n */
			encoded: "-=4a",
			processed: 1,
			decoded: "-",
			outputted: 1,
		},
		{ /* 0-0-1-2-0-S */
			encoded: "-=41",
			processed: 4,
			decoded: "-A",
			outputted: 2,
		},
		
		{ /* 0-0-0-1-2-0-1-S */
			encoded: "-+=41=",
			processed: 5,
			decoded: "-+A",
			outputted: 3,
		},
		{ /* 0-0-0-1-2-0-1-n */
			encoded: "-+=41=n",
			processed: 5,
			decoded: "-+A",
			outputted: 3,
		},
		{ /* 0-0-0-1-2-0-1-3-n */
			encoded: "-+=41=\rX",
			processed: 5,
			decoded: "-+A",
			outputted: 3,
		},
		{ /* 0-0-0-1-2-0-1-3-S */
			encoded: "-+=41=\rX",
			processed: 5,
			decoded: "-+A",
			outputted: 3,
		},
		{ /* 0-0-0-1-2-0-1-3-0-S */
			encoded: "-+=41=\r\n",
			processed: 8,
			decoded: "-+A",
			outputted: 3,
		},
		{ /* 0-0-0-1-2-0-1-2-S */
			encoded: "-+=41=4",
			processed: 5,
			decoded: "-+A",
			outputted: 3,
		},
		{ /* 0-0-0-1-2-0-1-2-n */
			encoded: "-+=41=4X",
			processed: 5,
			decoded: "-+A",
			outputted: 3,
		},
#endif
		{ /* 0-0-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42",
			processed: 8,
			decoded: "-+AB",
			outputted: 4,
		},

		{ /* 0-0-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=",
			processed: 8,
			decoded: "-+AB",
			outputted: 4,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=X",
			processed: 8,
			decoded: "-+AB",
			outputted: 4,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=\r",
			processed: 8,
			decoded: "-+AB",
			outputted: 4,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-3-n */
			encoded: "-+=41=42=\rX",
			processed: 8,
			decoded: "-+AB",
			outputted: 4,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=\r\n",
			processed: 11,
			decoded: "-+AB",
			outputted: 4,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=4",
			processed: 8,
			decoded: "-+AB",
			outputted: 4,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=4X",
			processed: 8,
			decoded: "-+AB",
			outputted: 4,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43",
			processed: 11,
			decoded: "-+ABC",
			outputted: 5,
		},

		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=",
			processed: 11,
			decoded: "-+ABC",
			outputted: 5,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=X",
			processed: 11,
			decoded: "-+ABC",
			outputted: 5,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=\r",
			processed: 11,
			decoded: "-+ABC",
			outputted: 5,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-3-n-S */
			encoded: "-+=41=42=43=\rX",
			processed: 11,
			decoded: "-+ABC",
			outputted: 5,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=\r\n",
			processed: 14,
			decoded: "-+ABC",
			outputted: 5,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=4",
			processed: 11,
			decoded: "-+ABC",
			outputted: 5,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=43=4X",
			processed: 11,
			decoded: "-+ABC",
			outputted: 5,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44",
			processed: 14,
			decoded: "-+ABCD",
			outputted: 6,
		},

		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=44=",
			processed: 14,
			decoded: "-+ABCD",
			outputted: 6,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=44=X",
			processed: 14,
			decoded: "-+ABCD",
			outputted: 6,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=44=\r",
			processed: 14,
			decoded: "-+ABCD",
			outputted: 6,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-n-S */
			encoded: "-+=41=42=43=44=\rX",
			processed: 14,
			decoded: "-+ABCD",
			outputted: 6,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=44=\r\n",
			processed: 17,
			decoded: "-+ABCD",
			outputted: 6,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=4",
			processed: 14,
			decoded: "-+ABCD",
			outputted: 6,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=43=44=4X",
			processed: 14,
			decoded: "-+ABCD",
			outputted: 6,
		},
		
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45",
			processed: 17,
			decoded: "-+ABCDE",
			outputted: 7,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=44=45=",
			processed: 17,
			decoded: "-+ABCDE",
			outputted: 7,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=44=45=X",
			processed: 17,
			decoded: "-+ABCDE",
			outputted: 7,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=44=45=\r",
			processed: 17,
			decoded: "-+ABCDE",
			outputted: 7,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-n */
			encoded: "-+=41=42=43=44=45=\rX",
			processed: 17,
			decoded: "-+ABCDE",
			outputted: 7,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=44=45=\r\n",
			processed: 20,
			decoded: "-+ABCDE",
			outputted: 7,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=45=4",
			processed: 17,
			decoded: "-+ABCDE",
			outputted: 7,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=43=44=45=4X",
			processed: 17,
			decoded: "-+ABCDE",
			outputted: 7,
		},
		
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46",
			processed: 20,
			decoded: "-+ABCDEF",
			outputted: 8,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=44=45=46=",
			processed: 20,
			decoded: "-+ABCDEF",
			outputted: 8,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=44=45=46=X",
			processed: 20,
			decoded: "-+ABCDEF",
			outputted: 8,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=44=45=46=4X",
			processed: 20,
			decoded: "-+ABCDEF",
			outputted: 8,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-n */
			encoded: "-+=41=42=43=44=45=46=\rX",
			processed: 20,
			decoded: "-+ABCDEF",
			outputted: 8,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=44=45=46=\r\n",
			processed: 23,
			decoded: "-+ABCDEF",
			outputted: 8,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=45=46=4",
			processed: 20,
			decoded: "-+ABCDEF",
			outputted: 8,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=43=44=45=46=4X",
			processed: 20,
			decoded: "-+ABCDEF",
			outputted: 8,
		},
		
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47",
			processed: 23,
			decoded: "-+ABCDEFG",
			outputted: 9,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=44=45=46=47=",
			processed: 23,
			decoded: "-+ABCDEFG",
			outputted: 9,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=44=45=46=47=X",
			processed: 23,
			decoded: "-+ABCDEFG",
			outputted: 9,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=44=45=46=47=\r",
			processed: 23,
			decoded: "-+ABCDEFG",
			outputted: 9,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-n */
			encoded: "-+=41=42=43=44=45=46=47=\rX",
			processed: 23,
			decoded: "-+ABCDEFG",
			outputted: 9,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=44=45=46=47=\r\n",
			processed: 26,
			decoded: "-+ABCDEFG",
			outputted: 9,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=45=46=47=4",
			processed: 23,
			decoded: "-+ABCDEFG",
			outputted: 9,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=43=44=45=46=47=4X",
			processed: 23,
			decoded: "-+ABCDEFG",
			outputted: 9,
		},

		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48",
			processed: 26,
			decoded: "-+ABCDEFGH",
			outputted: 10,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=44=45=46=47=48=",
			processed: 26,
			decoded: "-+ABCDEFGH",
			outputted: 10,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=44=45=46=47=48=X",
			processed: 26,
			decoded: "-+ABCDEFGH",
			outputted: 10,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=44=45=46=47=48=\r",
			processed: 26,
			decoded: "-+ABCDEFGH",
			outputted: 10,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-n */
			encoded: "-+=41=42=43=44=45=46=47=48=\rX",
			processed: 26,
			decoded: "-+ABCDEFGH",
			outputted: 10,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=\r\n",
			processed: 29,
			decoded: "-+ABCDEFGH",
			outputted: 10,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=45=46=47=48=4",
			processed: 26,
			decoded: "-+ABCDEFGH",
			outputted: 10,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=43=44=45=46=47=48=4X",
			processed: 26,
			decoded: "-+ABCDEFGH",
			outputted: 10,
		},

		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49",
			processed: 29,
			decoded: "-+ABCDEFGHI",
			outputted: 11,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=",
			processed: 29,
			decoded: "-+ABCDEFGHI",
			outputted: 11,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=X",
			processed: 29,
			decoded: "-+ABCDEFGHI",
			outputted: 11,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=\r",
			processed: 29,
			decoded: "-+ABCDEFGHI",
			outputted: 11,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=\rX",
			processed: 29,
			decoded: "-+ABCDEFGHI",
			outputted: 11,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=\r\n",
			processed: 32,
			decoded: "-+ABCDEFGHI",
			outputted: 11,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4",
			processed: 29,
			decoded: "-+ABCDEFGHI",
			outputted: 11,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4X",
			processed: 29,
			decoded: "-+ABCDEFGHI",
			outputted: 11,
		},

		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A",
			processed: 32,
			decoded: "-+ABCDEFGHIJ",
			outputted: 12,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=",
			processed: 32,
			decoded: "-+ABCDEFGHIJ",
			outputted: 12,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=X",
			processed: 32,
			decoded: "-+ABCDEFGHIJ",
			outputted: 12,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=\r",
			processed: 32,
			decoded: "-+ABCDEFGHIJ",
			outputted: 12,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=\rX",
			processed: 32,
			decoded: "-+ABCDEFGHIJ",
			outputted: 12,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=\r\n",
			processed: 35,
			decoded: "-+ABCDEFGHIJ",
			outputted: 12,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4",
			processed: 32,
			decoded: "-+ABCDEFGHIJ",
			outputted: 12,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4X",
			processed: 32,
			decoded: "-+ABCDEFGHIJ",
			outputted: 12,
		},

		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B",
			processed: 35,
			decoded: "-+ABCDEFGHIJK",
			outputted: 13,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=",
			processed: 35,
			decoded: "-+ABCDEFGHIJK",
			outputted: 13,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=X",
			processed: 35,
			decoded: "-+ABCDEFGHIJK",
			outputted: 13,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=\r",
			processed: 35,
			decoded: "-+ABCDEFGHIJK",
			outputted: 13,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=\rX",
			processed: 35,
			decoded: "-+ABCDEFGHIJK",
			outputted: 13,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=\r\n",
			processed: 38,
			decoded: "-+ABCDEFGHIJK",
			outputted: 13,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4",
			processed: 35,
			decoded: "-+ABCDEFGHIJK",
			outputted: 13,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4X",
			processed: 35,
			decoded: "-+ABCDEFGHIJK",
			outputted: 13,
		},

		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C",
			processed: 38,
			decoded: "-+ABCDEFGHIJKL",
			outputted: 14,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=",
			processed: 38,
			decoded: "-+ABCDEFGHIJKL",
			outputted: 14,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=X",
			processed: 38,
			decoded: "-+ABCDEFGHIJKL",
			outputted: 14,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=\r",
			processed: 38,
			decoded: "-+ABCDEFGHIJKL",
			outputted: 14,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=\rX",
			processed: 38,
			decoded: "-+ABCDEFGHIJKL",
			outputted: 14,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=\r\n",
			processed: 41,
			decoded: "-+ABCDEFGHIJKL",
			outputted: 14,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4",
			processed: 38,
			decoded: "-+ABCDEFGHIJKL",
			outputted: 14,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4X",
			processed: 38,
			decoded: "-+ABCDEFGHIJKL",
			outputted: 14,
		},

		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D",
			processed: 41,
			decoded: "-+ABCDEFGHIJKLM",
			outputted: 15,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=",
			processed: 41,
			decoded: "-+ABCDEFGHIJKLM",
			outputted: 15,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=X",
			processed: 41,
			decoded: "-+ABCDEFGHIJKLM",
			outputted: 15,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=\r",
			processed: 41,
			decoded: "-+ABCDEFGHIJKLM",
			outputted: 15,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=\rX",
			processed: 41,
			decoded: "-+ABCDEFGHIJKLM",
			outputted: 15,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=\r\n",
			processed: 44,
			decoded: "-+ABCDEFGHIJKLM",
			outputted: 15,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4",
			processed: 41,
			decoded: "-+ABCDEFGHIJKLM",
			outputted: 15,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4X",
			processed: 41,
			decoded: "-+ABCDEFGHIJKLM",
			outputted: 15,
		},

		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E",
			processed: 44,
			decoded: "-+ABCDEFGHIJKLMN",
			outputted: 16,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=",
			processed: 44,
			decoded: "-+ABCDEFGHIJKLMN",
			outputted: 16,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=X",
			processed: 44,
			decoded: "-+ABCDEFGHIJKLMN",
			outputted: 16,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=\r",
			processed: 44,
			decoded: "-+ABCDEFGHIJKLMN",
			outputted: 16,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=\rX",
			processed: 44,
			decoded: "-+ABCDEFGHIJKLMN",
			outputted: 16,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=\r\n",
			processed: 47,
			decoded: "-+ABCDEFGHIJKLMN",
			outputted: 16,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4",
			processed: 44,
			decoded: "-+ABCDEFGHIJKLMN",
			outputted: 16,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4X",
			processed: 44,
			decoded: "-+ABCDEFGHIJKLMN",
			outputted: 16,
		},

		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F",
			processed: 47,
			decoded: "-+ABCDEFGHIJKLMNO",
			outputted: 17,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=",
			processed: 47,
			decoded: "-+ABCDEFGHIJKLMNO",
			outputted: 17,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=X",
			processed: 47,
			decoded: "-+ABCDEFGHIJKLMNO",
			outputted: 17,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=\r",
			processed: 47,
			decoded: "-+ABCDEFGHIJKLMNO",
			outputted: 17,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=\rX",
			processed: 47,
			decoded: "-+ABCDEFGHIJKLMNO",
			outputted: 17,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-3-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=\r\n",
			processed: 50,
			decoded: "-+ABCDEFGHIJKLMNO",
			outputted: 17,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=5",
			processed: 47,
			decoded: "-+ABCDEFGHIJKLMNO",
			outputted: 17,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-n */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=5X",
			processed: 47,
			decoded: "-+ABCDEFGHIJKLMNO",
			outputted: 17,
		},

		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=50",
			processed: 50,
			decoded: "-+ABCDEFGHIJKLMNOP",
			outputted: 18,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=50-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=50-+-+-+-+-+-+-+-+-+-+-+-+-+-+",
			processed: 128,
			decoded: "-+ABCDEFGHIJKLMNOP-+ABCDEFGHIJKLMNOP-+-+-+-+-+-+-+-+-+-+-+-+-+-+",
			outputted: 64,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=50-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=50-+-+-+-+-+-+-+-+-+-+-+-+-+-+-",
			processed: 129,
			decoded: "-+ABCDEFGHIJKLMNOP-+ABCDEFGHIJKLMNOP-+-+-+-+-+-+-+-+-+-+-+-+-+-+-",
			outputted: 65,
		},
		{ /* 0-0-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-1-2-0-S */
			encoded: "-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=50-+=41=42=43=44=45=46=47=48=49=4A=4B=4C=4D=4E=4F=50-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+",
			processed: 130,
			decoded: "-+ABCDEFGHIJKLMNOP-+ABCDEFGHIJKLMNOP-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+",
			outputted: 66,
		},
	};
	int i, j;
	int32 retval;
	int maxLen = 76;
	
	testGdmaInit();

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		for( j = 0; j < sizeof(testvec)/sizeof(testvec[0]); j++ )
		{
			/* clean memory */
			clearGdmaBuffer(CLEAN_ALL);
			
			/* Reset GDMA */
			WRITE_MEM32( GDMACNR, 0 );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE );
				
			memcpy( sb2, testvec[j].encoded, strlen(testvec[j].encoded) );
			WRITE_MEM32( GDMAIMR, 0 );
			WRITE_MEM32( GDMAISR, 0xffffffff );
			WRITE_MEM32( GDMAICVL, maxLen/4-1 ); /* Line Length */
			WRITE_MEM32( GDMAICVR, 0 );
			WRITE_MEM32( GDMASBP0, (uint32)sb2 );
			WRITE_MEM32( GDMASBL0, GDMA_LDB|strlen(testvec[j].encoded) );
			WRITE_MEM32( GDMADBP0, (uint32)db2 );
			WRITE_MEM32( GDMADBL0, GDMA_LDB|(strlen(testvec[j].decoded)) );
			WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_QPDEC|internalUsedGDMACNR );

			/* Expected data */
			memcpy( sb1, testvec[j].encoded, strlen(testvec[j].encoded) );
			memcpy( db1, testvec[j].decoded, strlen(testvec[j].decoded) );

			while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

			/*memDump( sb1, strlen(testvec[j].encoded), "sd1" );*/
			/*memDump( sb2, strlen(testvec[j].encoded), "sd2" );*/
			/*memDump( db1, strlen(testvec[j].decoded), "dd1" );*/
			/*memDump( db2, strlen(testvec[j].decoded), "dd2" );*/
#if 0
			IS_EQUAL_INT("Source block should under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, GDMA_SBUDRIP, __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
			IS_EQUAL_INT("Processed length not match!  real,expected=", READ_MEM32(GDMAICVL)>>16, testvec[j].processed, __FUNCTION__, __LINE__ );
			IS_EQUAL_INT("Output length not match!  real,expected=", READ_MEM32(GDMAICVL)&0xffff, testvec[j].outputted, __FUNCTION__, __LINE__ );
			ASSERT( compare_data_block() == SUCCESS );
		}
	}

	return SUCCESS;
}


/*
 *	Use random data to compare FPGA and model code.
 */
int32 testGdmaQuotedPrintableEncDec2(uint32 caseNo)
{
	int i, j;
	uint32 *p0, *p1;
	int32 retval;
	int loop;
	int32 processed[2], outputted[2];
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */

	processed[0] = processed[1] = outputted[0] = outputted[1] = 0;
	/* loop 1 time --> about 14 seconds */
	for( loop = 0; loop < 1000; loop++ )
	{
		rtlglue_printf( "[%d-", loop );

		clearGdmaBuffer(CLEAN_DB1);clearGdmaBuffer(CLEAN_DB2);
		/* We will generate first 2 data block with random data */
		p0 = (uint32*)&enumbuf[0].src[0];
		p1 = (uint32*)&enumbuf[1].src[0];
		for( j = 0; j < MAX_BLOCK_LENGTH*2/sizeof(uint32); j++ )
		{
			*(p0++) = *(p1++) = big_endian( rtlglue_random() );
		}

		for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
		{
			rtlglue_printf( "%d-", i );
			retval = model_setTestTarget( _i[i] );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			retval = rtl865x_qpenc( &enumbuf[i].src[0], MAX_BLOCK_LENGTH*2,
			                         &enumbuf[i].dst[0], MAX_BLOCK_LENGTH*8,
			                         &processed[i], &outputted[i] );
			if ( retval != SUCCESS )
			{
				rtlglue_printf( "i=%d L%d: rtl865x_qpenc() error: retval=%d\n", i, __LINE__, retval );
				return retval;
			}
			rtlglue_printf( "processed=%d, outputted=%d ", processed[i], outputted[i] );

			if ( processed[i]<=0 || outputted[i]<=0 )
			{
				rtlglue_printf( "i=%d L%d: rtl865x_qpenc() error: processed=%d, outputted=%d\n", i, __LINE__, processed[i], outputted[i] );
				return FAILED;
			}
		}

/*if ( loop==41) { rtlglue_printf("db1[38260]=0x%02x%02x%02x%02x\n", db1[38260], db1[38261], db1[38262], db1[38263] ); return FAILED; }*/

		if ( processed[0] != processed[1] )
		{
			rtlglue_printf( "L%d: processed length is not the same: processed[0]=%d, processed[1]=%d\n", __LINE__, processed[0], processed[1] );
			return FAILED;
		}
		if ( outputted[0] != outputted[1] )
		{
			rtlglue_printf( "L%d: outputted length is not the same: outputted[0]=%d, outputted[1]=%d\n", __LINE__, outputted[0], outputted[1] );
			return FAILED;
		}
		ASSERT( compare_data_block() == SUCCESS );

		for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
		{
			rtlglue_printf( "%d-", i );
			retval = model_setTestTarget( _i[i] );
			if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

			retval = rtl865x_qpdec( &enumbuf[i].dst[0], outputted[i],
			                         &enumbuf[i].src[0], processed[i]+32/* we need to reserve FIFO length for IC */,
			                         &processed[i], &outputted[i] );
			if ( retval != SUCCESS )
			{
				rtlglue_printf( "i=%d L%d: rtl865x_qpdec() error: retval=%d\n", i, __LINE__, retval );
				return retval;
			}
			rtlglue_printf( "processed=%d, outputted=%d ", processed[i], outputted[i] );

			if ( processed[i]<=0 || outputted[i]<=0 )
			{
				rtlglue_printf( "i=%d L%d: rtl865x_qpdec() error: processed=%d, outputted=%d\n", i, __LINE__, processed[i], outputted[i] );
				return FAILED;
			}
		}

		rtlglue_printf( "]" );
		if ( processed[0] != processed[1] )
		{
			rtlglue_printf( "L%d: processed length is not the same: processed[0]=%d, processed[1]=%d\n", __LINE__, processed[0], processed[1] );
			return FAILED;
		}
		if ( outputted[0] != outputted[1] )
		{
			rtlglue_printf( "L%d: outputted length is not the same: outputted[0]=%d, outputted[1]=%d\n", __LINE__, outputted[0], outputted[1] );
			return FAILED;
		}
		ASSERT( compare_data_block() == SUCCESS );
	}

	return SUCCESS;
}


/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  Byte Swap                                                               **
 **                                                                          **
 ******************************************************************************
 ******************************************************************************/
int32 testGdmaByteSwap1(uint32 caseNo)
{
	int i, j;
	int32 retval;
	uint8 str[64] = "Hi, everybody. I am Louis. I come from Taiwan. Glad to meet you.";
	
	testGdmaInit();
	printf("Start %s: Byte Swap test.\n", __FUNCTION__);

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		/* clean memory */
		clearGdmaBuffer(CLEAN_ALL);

		/* 0. init A with meaningful string */
		memcpy( sb2, str, sizeof(str) );

		/* Byte Swap */
		WRITE_MEM32( GDMACNR, 0 ); /* Reset GDMA */
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_MEMCPY );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)sb2) );
		WRITE_MEM32( GDMASBL0, GDMA_LDB|sizeof(str) );
		WRITE_MEM32( GDMADBP0, TO_PHY_ADDR((uint32)db2) );
		WRITE_MEM32( GDMADBL0, GDMA_LDB|sizeof(str) );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_BYTESWAP|GDMA_SWAPTYPE0|internal_gdmacnr);

		/* Expected data */
		memcpy( sb1, str, sizeof(str) );
		memcpy( db1, str, sizeof(str) );
		for( j = 0; j < sizeof(str); j+=2 )
		{
			uint8 tmp;
			tmp = db1[j];
			db1[j] = db1[j+1];
			db1[j+1] = tmp;
		}
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

#if 0
		IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Source block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
		ASSERT( compare_data_block() == SUCCESS );
	}

	return SUCCESS;
}


int32 testGdmaByteSwap2(uint32 caseNo)
{
	int i, j;
	int32 retval;
	uint8 str[64] = "Hi, everybody. I am Louis. I come from Taiwan. Glad to meet you.";
	
	testGdmaInit();
	printf("Start %s\n", __FUNCTION__);

	for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
	{
		retval = model_setTestTarget( i );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		/* clean memory */
		clearGdmaBuffer(CLEAN_ALL);

		/* 0. init A with meaningful string */
		memcpy( sb2, str, sizeof(str) );

		/* Byte Swap */
		WRITE_MEM32( GDMACNR, 0 ); /* Reset GDMA */
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)sb2) );
		WRITE_MEM32( GDMASBL0, GDMA_LDB|sizeof(str) );
		WRITE_MEM32( GDMADBP0, TO_PHY_ADDR((uint32)db2) );
		WRITE_MEM32( GDMADBL0, GDMA_LDB|sizeof(str) );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_BYTESWAP|GDMA_SWAPTYPE1|internal_gdmacnr);

		/* Expected data */
		memcpy( sb1, str, sizeof(str) );
		memcpy( db1, str, sizeof(str) );
		for( j = 0; j < sizeof(str); j+=4 )
		{
			uint8 tmp;
			tmp = db1[j];
			db1[j] = db1[j+3];
			db1[j+3] = tmp;
			tmp = db1[j+1];
			db1[j+1] = db1[j+2];
			db1[j+2] = tmp;
		}
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

#if 0
		IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
		IS_EQUAL_INT("Source block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
		ASSERT( compare_data_block() == SUCCESS );
	}

	return SUCCESS;
}


/*
 *	byte swap: from cbMemory3()
 */
int32 cbByteSwap3( int32 *sl, int32 *sa, int32 *dl, int32 *da )
{
	int32 retval;
	int i, j;
	static int _i[] = { IC_TYPE_MODEL, IC_TYPE_REAL }; /* IC type to be compared */
	static uint8 str[2048];
	uint8 *pch1, *pch2;
	uint32 swapType;

	DEBUG_BAL();
	for( j = 0; j < sizeof(str); j++ )
	{
		str[j] = rtlglue_random();
	}
	swapType = (rtlglue_random()&0x00008000)?GDMA_SWAPTYPE1:GDMA_SWAPTYPE0; /* we use bit 15 */

	clearGdmaBuffer(CLEAN_ALL);
	for( i = 0; i < sizeof(_i)/sizeof(_i[0]); i++ )
	{
		retval = model_setTestTarget( _i[i] );
		if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

		printf("\r\\\\");
		/* Reset GDMA */
		WRITE_MEM32( GDMACNR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE );
			
		/* source block */							
		pch1 = str;
		for( j = 0; j < 8; j++ )
		{
			pch2 = enumbuf[i].src+sa[j];
			memcpy( pch2, pch1, sl[j] );
			pch1 += sl[j];
			WRITE_MEM32( GDMASBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			if ( j==7 || sl[j+1]==0 )
			{
				WRITE_MEM32( GDMASBL0+j*8, GDMA_LDB|(sl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMASBL0+j*8, sl[j] );
			}
		}

		/* destination block */							
		for( j = 0; j < 8; j++ )
		{
			pch2 = enumbuf[i].dst+da[j];
			WRITE_MEM32( GDMADBP0+j*8, TO_PHY_ADDR((uint32)pch2) );
			if ( j==7 || dl[j+1]==0)
			{
				WRITE_MEM32( GDMADBL0+j*8, GDMA_LDB|(dl[j]) );
				break;
			}
			else
			{
				WRITE_MEM32( GDMADBL0+j*8, dl[j] );
			}
		}

		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_BYTESWAP );
		WRITE_MEM32( GDMAIMR, 0 );
		WRITE_MEM32( GDMAISR, 0xffffffff );
		WRITE_MEM32( GDMAICVL, 0 );
		WRITE_MEM32( GDMAICVR, 0 );
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_BYTESWAP|swapType|internal_gdmacnr );

		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		MEMCTL_DEBUG_PRINTF("\r//");
	}

	ASSERT( compare_data_block() == SUCCESS );

	return SUCCESS;
}
int32 testGdmaByteSwap3(uint32 caseNo)
{
	static int32 sa[8];
	static int32 sl[8];
	static int32 da[8];
	static int32 dl[8];
	static int32 tmplSrcLen[] = { 4, 8, 12, 16, 1500 };
	static int32 tmplSrcAlign[] = { 0, 1, 2, 3 };
	static int32 tmplDstLen[] = { 4, 8, 12, 16, 1500 };
	static int32 tmplDstAlign[] = { 0, 1, 2, 3 };

	printf("Start %s\n", __FUNCTION__);
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */

	memset( sa, 0, sizeof(sa) );
	memset( sl, 0, sizeof(sl) );
	memset( da, 0, sizeof(da) );
	memset( dl, 0, sizeof(dl) );

	return enumBAL( 2, 0, 99999, 99999, tmplSrcLen, tmplSrcAlign, tmplDstLen, tmplDstAlign, 
	                sizeof(tmplSrcLen)/sizeof(tmplSrcLen[0]), sizeof(tmplSrcAlign)/sizeof(tmplSrcAlign[0]), sizeof(tmplDstLen)/sizeof(tmplDstLen[0]), sizeof(tmplDstAlign)/sizeof(tmplDstAlign[0]),
	                sl, sa, dl, da, cbByteSwap3, ENUMBAL_ALLOW_EQUAL );
}


/*
 *	byte swap: from cbByteSwap2()
 */
int32 testGdmaByteSwap4(uint32 caseNo)
{
	int i, j;
	int32 retval;
	int len, sa, da;
	
	testGdmaInit();
	testGdmaOptimize( OPT_CACHED1, TRUE ); /* enumbuf[0] is for model code */
	printf("Start %s\n", __FUNCTION__);

	for( len = 4; len <= MAX_BLOCK_LENGTH; len+=4 )
	{
		for( sa = 0; sa <= 3; sa++ )
		{
			for( da = 0; da <= 3; da++ )
			{
				uint32 *p0, *p1;
				
				/* 0. init random data. For alignment issue, we will generate more than we need. */
				p0 = (uint32*)&enumbuf[0].src[0];
				p1 = (uint32*)&enumbuf[1].src[0];
				for( j = 0; j < len+4; j+=4, p0++, p1++ )
				{
					*p0 = *p1 = rtlglue_random();
				}

//				_cache_flush();

				for( i = IC_TYPE_MIN; i < IC_TYPE_MAX; i++ )
				{
					retval = model_setTestTarget( i );
					if ( retval == VMAC_ERROR_NON_AVAILABLE ) continue; /* Not support, we don't compare. */

					/* clean memory */
					clearGdmaBuffer(CLEAN_ALL);

					printf("\r\\\\");
					/* Byte Swap */
					WRITE_MEM32( GDMACNR, 0 ); /* Reset GDMA */
					WRITE_MEM32( GDMACNR, GDMA_ENABLE );
					WRITE_MEM32( GDMAIMR, 0 );
					WRITE_MEM32( GDMAISR, 0xffffffff );
					WRITE_MEM32( GDMAICVL, 0 );
					WRITE_MEM32( GDMAICVR, 0 );
					WRITE_MEM32( GDMASBP0, TO_PHY_ADDR((uint32)&enumbuf[i].src[sa]) );
					WRITE_MEM32( GDMASBL0, GDMA_LDB|len );
					WRITE_MEM32( GDMADBP0, TO_PHY_ADDR((uint32)&enumbuf[i].dst[da]) );
					WRITE_MEM32( GDMADBL0, GDMA_LDB|len );
					WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_BYTESWAP|GDMA_SWAPTYPE1|internal_gdmacnr);

					while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
					MEMCTL_DEBUG_PRINTF("\r//");

#if 0
					IS_EQUAL_INT("Dest block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_DBUDRIP, 0, __FUNCTION__, __LINE__ );
					IS_EQUAL_INT("Source block should NOT under-run", READ_MEM32(GDMAISR)&GDMA_SBUDRIP, 0, __FUNCTION__, __LINE__ );
#endif
				}

				ASSERT( compare_data_block() == SUCCESS );
			}
		}
	}
	return SUCCESS;
}
/*
 * The last data block has NO LDB bit.
 */

