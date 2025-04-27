/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Source File for IC to Export Registers
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: icExport.c,v 1.32 2007-06-01 06:49:18 yjlou Exp $
*/

#include "rtl_types.h"
#include "asicregs.h"
#include "assert.h"
#include "gdma_glue.h"
#include "icTest.h"
#include "icExport.h"
#include "gdma.h"
#include "virtualMac.h"
#include "rtl865xC_tblAsicDrv.h"

/* for export ACL */
//#define RTL8651_TBLDRV_LOCAL_H
//#include "rtl8651_aclLocal.h"
//#include "rtl8651_layer2local.h"
/* end of export ACL */


//#define STDOUT_FILENAME "/dev/stdout"


uint32 expGroup = 0;

/* Output File */
char expFileName[256];
static int fdFile = -1; /* File Descriptor of Output File */
static int expOutputtedline; /* current outputted line */
#define MAX_SLICE_LINE 49500

enum EXPORT_REGISTER_FORMAT expFormat = 0; /* default for none */
enum IC_TYPE expIcType = IC_TYPE_MODEL;


#define MAX_VIRTUAL_MAP 16
typedef struct VIRTUAL_MAP_S
{
	uint32 virtualStart, virtualEnd;  /* if virtualStart==virtualEnd, mean invalid */
	uint32 physicalStart;
} VIRTUAL_MAP_T;
VIRTUAL_MAP_T virtualMap[MAX_VIRTUAL_MAP];

/* for runModelExport() to check */
uint32 defaultModelToPhysicalAdded;

/*===========================================================================*
 *   VIRTUAL ADDRESS TO PHYSICAL ADDRESS                                     *
 *===========================================================================*/
/*
 * map back to physical address, for example:
 *   [pVirtualSWReg,pVirtualSWReg+64K} --> REAL_SWCORE_BASE
 */
uint32 queryModelToPhysical( uint32 virtual )
{
	int i;
	for( i = 0; i < MAX_VIRTUAL_MAP; i++ )
	{
		if ( virtual >= virtualMap[i].virtualStart && virtual < virtualMap[i].virtualEnd )
		{
			return virtual - virtualMap[i].virtualStart + virtualMap[i].physicalStart;
		}
	}

	return virtual;
}

/*===========================================================================*
 *   PHYSICAL ADDRESS TO VIRTUAL ADDRESS                                     *
 *===========================================================================*/
/*
 * map virtual address to physical address, for example:
 *   REAL_SWCORE_BASE --> [pVirtualSWReg,pVirtualSWReg+64K}
 */
uint32 queryPhysicalToModel( uint32 physical )
{
	int i;
	for( i = 0; i < MAX_VIRTUAL_MAP; i++ )
	{
		if ( physical >= virtualMap[i].physicalStart && 
		     physical < (virtualMap[i].physicalStart+(virtualMap[i].virtualEnd-virtualMap[i].virtualStart)) )
		{
			return physical - virtualMap[i].physicalStart + virtualMap[i].virtualStart;
		}
	}

	return physical;
}

/*
 * add a pair of [virtual,physical].
 */
int32 addModelToPhysical( uint32 virtualStart, uint32 length, uint32 physicalStart )
{
	int i;
	for( i = MAX_VIRTUAL_MAP-1; i >= 0 ; i-- )
	{
		if ( virtualMap[i].virtualStart == virtualMap[i].virtualEnd )
		{
			virtualMap[i].virtualStart = virtualStart;
			virtualMap[i].virtualEnd = virtualStart+length;
			virtualMap[i].physicalStart = physicalStart;
			return SUCCESS;
		}
	}
	return FAILED;
}

/*
 * delete a pair of [virtual,physical].
 */
int32 delModelToPhysical( uint32 virtualStart )
{
	int i;
	for( i = 0; i < MAX_VIRTUAL_MAP; i++ )
	{
		if ( virtualMap[i].virtualStart == virtualStart )
		{
			virtualMap[i].virtualStart = virtualMap[i].virtualEnd;
			virtualMap[i].physicalStart = 0xDeadC0de;
			return SUCCESS;
		}
	}
	return FAILED;
}


/*===========================================================================*
 *   EXPORT-RELATED FUNCTIONS                                                *
 *===========================================================================*/
/*
 *	This function is used by GDMA/Crypto/Swcore export functions.
 *	If this function returns FAILED, those export functions can return immediately to speed up.
 *	If returns SUCCESS, it means export function is enabled, and export functions should be invoken.
 */
inline int32 modelExportDumpable( void )
{

	if ( fdFile == -1 ) return FAILED; /* file is not opened */

	return SUCCESS;
}

/*
 *	This function is used to dump raw data from 'str'.
 *	It is useful when we want to generate header and 'nop' command.
 *	This function will NOT generate any newline, please prepare the tailing newline in the 'str'.
 */
char* modelExportRaw( char* str )
{
	int i;

	if ( modelExportDumpable() == SUCCESS )
	{
		rtlglue_printf( "%s", str  );

		/* increase expOutputtedline */
		for( i = 0; str[i]; i++ )
		{
			if ( str[i] == '\n' ) expOutputtedline++;
		}
	}

	return str;
}


/*
 *	This function is used to dump 'addr' and 'value'.
 *	'addr' will be converted if it is a model-space variable, for example, 0x08091CA4 --> 0x00F91CA4.
 *	'value' will be raw-output.
 *	'mask' is a bit mask to indicate which bits are what we care. 1 for care, 0 for not-care.
 */
char* modelExportValueMask( uint32 addr, uint32 value, uint32 mask )
{
	static char buf[256];


	switch( expFormat )
	{
		case EXPORT_RTK_SIM:
			sprintf( buf, "memw %08X %08X;\n", queryModelToPhysical(addr), value );
			break;
			
		case EXPORT_RTK_EXPECT:
			sprintf( buf, "memr %08X %08X;\n", queryModelToPhysical(addr), value );
			if ( (mask&0xf0000000)==0 ) buf[14] = 'X';
			if ( (mask&0x0f000000)==0 ) buf[15] = 'X';
			if ( (mask&0x00f00000)==0 ) buf[16] = 'X';
			if ( (mask&0x000f0000)==0 ) buf[17] = 'X';
			if ( (mask&0x0000f000)==0 ) buf[18] = 'X';
			if ( (mask&0x00000f00)==0 ) buf[19] = 'X';
			if ( (mask&0x000000f0)==0 ) buf[20] = 'X';
			if ( (mask&0x0000000f)==0 ) buf[21] = 'X';
			break;
			
		case EXPORT_CLESHELL:
			sprintf( buf, "memory write %08x %08x;\n", addr, value );
			break;

		case EXPORT_ICEMON:
			sprintf( buf, "ew 0x%08x=0x%08x;\n", addr, value );
			break;
			
	}

	modelExportRaw( buf );
	
	return buf;
}


/*
 *	This function is used if the 'addr' is a register that pointed to SDRAM.
 *	The value of this register will be converted to physical address of SDRAM.
 */
char* modelExportPointerRegister( uint32 addr )
{
	char *buf;
	uint32 value;

	value = READ_VIR32( addr );
	buf = modelExportValue( addr, queryModelToPhysical(value) );

	return buf;
}


/*
 *	This function is used for 'addr' is a normal register.
 *	'mask' is a bit mask to indicate which bits are what we care. 1 for care, 0 for not-care.
 */
char* modelExportRegisterMask( uint32 addr, uint32 mask )
{
	char *buf;
	uint32 value;

	value = READ_VIR32( addr );
	buf = modelExportValueMask( addr, value, mask );

	return buf;
}


/*
 *	This function is for burst memory dump.
 *	The 'addr' will be converted to physical address, either SDRAM or ASIC register.
 *	The unit of len is bytes.
 */
char* modelExportMemory( uint32 addr, uint32 len )
{
	static char buf[1024*16];
	static char tmp[32];
	uint32 value;
	int i, j;


	if ( len & 3 ) 
	{
		rtlglue_printf( "Currently, we do not support len(%d) that is not times of 4. ", len );
		len = (len+3)&~3;
		rtlglue_printf( "Round up to %d.\n", len );
	}

	buf[0] = '\0';
	if ( len == 0 ) return buf;
	switch( expFormat )
	{
		case EXPORT_RTK_SIM:
			for( i = 0; i < len; i+=j )
			{
				buf[0] = '\0';
				sprintf( tmp, "memw %08X ", queryModelToPhysical(addr+i) );
				strcat( buf, tmp );
				for( j = 0; (j<31*4)&&(i+j+4)<len; j+=4 )
				{
					value = READ_VIR32( addr+i+j );
					sprintf( tmp, "%08X,\n              ", value );
					strcat( buf, tmp );					
				}
				value = READ_VIR32( addr+i+j );
				sprintf( tmp, "%08X;\n", value );
				strcat( buf, tmp );
				j+=4;

				modelExportRaw( buf );
			}
			break;
			
		case EXPORT_RTK_EXPECT:
			for( i = 0; i < len; i+=j )
			{
				buf[0] = '\0';
				
				sprintf( tmp, "memr %08X ", queryModelToPhysical(addr+i) );
				strcat( buf, tmp );
				for( j = 0; (j<31*4)&&(i+j+4)<len; j+=4 )
				{
					value = READ_VIR32( addr+i+j );
					sprintf( tmp, "%08X,\n              ", value );
					strcat( buf, tmp );					
				}
				value = READ_VIR32( addr+i+j );
				sprintf( tmp, "%08X;\n", value );
				strcat( buf, tmp );
				j+=4;

				modelExportRaw( buf );
			}
			break;
			
		case EXPORT_CLESHELL:
			for( i = 0; i < len; i+=4 )
			{
				value = READ_VIR32( addr + i );
				sprintf( buf, "memory write %08x %08x;\n", addr + i, value );
				modelExportRaw( buf );
			}
			break;

		case EXPORT_ICEMON:
			for( i = 0; i < len; i+=4 )
			{
				value = READ_VIR32( addr + i );
				sprintf( buf, "ew 0x%08x=0x%08x;\n", addr + i, value );
				modelExportRaw( buf );
			}
			break;
	}

	buf[0] = '\0';
	return buf;
}


/*
 *	This function is provided to model code.
 *	It will output human-readable comment according to 'format' field.
 *	Leaving pszComment with NULL pointer will output a empty new line.
 */
char* modelExportComment( char* pszComment )
{
	static char buf[256];

	switch( expFormat )
	{
		case EXPORT_RTK_SIM:
			if ( pszComment )
				sprintf( buf, "// %s\n", pszComment );
			else
				buf[0]= '\0';
			break;
			
		case EXPORT_RTK_EXPECT:
			if ( pszComment )
				sprintf( buf, "// %s\n", pszComment );
			else
				buf[0]= '\0';
			break;
			
		case EXPORT_CLESHELL:
			if ( pszComment )
				sprintf( buf, "// %s\n", pszComment );
			else
				buf[0]= '\0';
			break;

		case EXPORT_ICEMON:
			if ( pszComment )
				sprintf( buf, "/* %s */", pszComment );
			else
				buf[0]= '\0';
			break;
	}

	modelExportRaw( buf );

	return buf;
}


/*
 *	This function is provided to model code.
 *	It will output 'nop XXXXX;' for delay several cycles in simulation.
 */
char* modelExportNop( int32 delayCycles )
{
	static char buf[256];

#ifdef CONFIG_RTL865XC
	/* IC type is not matched. */
	{
		enum IC_TYPE icType;

		model_getTestTarget( &icType );
		if ( icType != expIcType ) return NULL;
	}
#endif

	switch( expFormat )
	{
		case EXPORT_RTK_SIM:
			sprintf( buf, "nop %d;\n", delayCycles );
			break;
			
		case EXPORT_RTK_EXPECT:
			sprintf( buf, "nop %d;\n", delayCycles );
			break;
			
		case EXPORT_CLESHELL:
			buf[0]= '\0';
			break;

		case EXPORT_ICEMON:
			buf[0]= '\0';
			break;
	}

	modelExportRaw( buf );

	return buf;
}
/*===========================================================================*
 *   END OF EXPORT FUNCTIONS                                                 *
 *===========================================================================*/



/*===========================================================================*
 *   IC-SPECIFIC EXPORT FUNCTION                                             *
 *===========================================================================*/
/*
 *	This function only dumps GDMA related registers and memory content.
 */
#define BURST_ACCESS
int32 modelExportGdmaRegisters( void )
{
	if ( modelExportDumpable() != SUCCESS ) return SUCCESS; /* fast return */
	
	{
		uint32 i, len;
#ifdef BURST_ACCESS /* use burst access for designer */
#else
		uint32 j;
#endif
		
		/* export source data block content */
		for( i = 0; i < 8; i++ )
		{
			uint32 start, end;

			start = READ_VIR32(GDMASBP0+i*8);
			len = READ_VIR32( GDMASBL0+i*8 ) & GDMA_BLKLENMASK;
			end = start + len;

			/* align to 4-bytes */
			start &= ~3;
			end = ( end + 3 ) & ~3;
			
#ifdef BURST_ACCESS /* use burst access for designer */
			if ( end > start ) modelExportMemory( start, end-start );
#else
			for( j = start; j < end; j += 4 )
			{
				modelExportRegister( j );
			}
#endif

			if ( READ_VIR32( GDMASBL0+i*8 ) & GDMA_LDB ) break;
		}

		/* If this is pattern match, we need special process for destination data block. */
		if ( ( READ_VIR32(GDMACNR) & GDMA_FUNCMASK ) == GDMA_PATTERN )
		{
			uint32 **pVsm;
			uint32 *pSub;
			uint32 inst;
			uint32 i, j;
			
			pVsm = (uint32 **)READ_VIR32( GDMADBP0 );

			for( i = 0; i < SUB_STATE_MACHINE_NUMBER; i++ )
			{
				if (( pSub = (uint32*)big_endian(READ_VIR32((uint32)(pVsm+i))) ))
				{
					modelExportValue( (uint32)(pVsm+i), queryModelToPhysical((uint32)pSub) );
					for( j = 0; j < INTERNAL_RULE_NUMBER; j++ )
					{
						if (( inst = big_endian(READ_VIR32((uint32)(pSub+j))) ))
						{
							modelExportValue( (uint32)(pSub+j), inst );
						}
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			/* export destination data block content */
			for( i = 0; i < 8; i++ )
			{
				uint32 start, end;

				start = READ_VIR32(GDMADBP0+i*8);
				len = READ_VIR32( GDMADBL0+i*8 ) & GDMA_BLKLENMASK;
				if ( len > 0 )
				{
					end = start + len;

					/* align to 4-bytes */
					start &= ~3;
					end = ( end + 3 ) & ~3;
					
#ifdef BURST_ACCESS /* use burst access for designer */
					if ( end > start ) modelExportMemory( start, end-start );
#else
					for( j = start; j < end; j += 4 )
					{
						modelExportRegister( j );
					}
#endif
				}

				if ( READ_VIR32( GDMADBL0+i*8 ) & GDMA_LDB ) break;
			}
		}

		/* Export register */
		modelExportRegister( GDMAIMR );
		modelExportRegister( GDMAISR );
#if 0 /* If we want to export masked ICVL, mark as #if 1. */
		if ( ( ( READ_VIR32(GDMACNR) & GDMA_FUNCMASK ) == GDMA_B64DEC ) ||
		     ( ( READ_VIR32(GDMACNR) & GDMA_FUNCMASK ) == GDMA_B64ENC ) ||
		     ( ( READ_VIR32(GDMACNR) & GDMA_FUNCMASK ) == GDMA_QPDEC ) ||
		     ( ( READ_VIR32(GDMACNR) & GDMA_FUNCMASK ) == GDMA_QPENC ) )
			modelExportRegisterMask( GDMAICVL, 0x00000000 ); /* only outputed length is what we care. */
		else
#endif
			modelExportRegister( GDMAICVL );
		modelExportRegister( GDMAICVR );

		/* export source data block register */
		for( i = 0; i < 8; i++ )
		{
			modelExportPointerRegister( GDMASBP0+i*8 );
			modelExportRegister( GDMASBL0+i*8 );
			if ( READ_VIR32( GDMASBL0+i*8 ) & GDMA_LDB ) break;
		}

		/* export destination data block register */
		for( i = 0; i < 8; i++ )
		{
			modelExportPointerRegister( GDMADBP0+i*8 );
			modelExportRegister( GDMADBL0+i*8 );
			if ( READ_VIR32( GDMADBL0+i*8 ) & GDMA_LDB ) break;
		}

		modelExportRegister( GDMACNR );
	}

	/* we slice file ONLY after dumping expecting results. */
	if ( expFormat==EXPORT_RTK_EXPECT && expOutputtedline>MAX_SLICE_LINE )
		modelExportOpenFile( NULL );
	
	return SUCCESS;
}

#if 0
#if defined(CONFIG_RTL865XB_EXP_CRYPTOENGINE) && defined(CONFIG_RTL865XC)
/*
 *	This function only dumps Crypto related registers and memory content.
 */
int32 modelExportCryptoRegisters( void )
{
	if ( modelExportDumpable() != SUCCESS ) return SUCCESS; /* fast return */
	
	{
		rtl865xc_crypto_source_t *pIpssdar;
		rtl865xc_crypto_dest_t *pIpsddar;

		/* export source and destination descriptor */
		pIpssdar = ipssdar;
		do
		{
			modelExportMemory( (uint32)pIpssdar, sizeof(*pIpssdar) );
			if ( pIpssdar->sbl )
			{
				uint32 start, end;

				start = (uint32)KSEG1_ADDRESS(pIpssdar->sdbp);
				end = start + pIpssdar->sbl;
				/* align to 4-bytes */
				start &= ~3;
				end = ( end + 3 ) & ~3;
				modelExportMemory( start, end-start );
			}
		} while ( (pIpssdar++)->eor == 0 );

		pIpsddar = ipsddar;
		do
		{
			modelExportMemory( (uint32)pIpsddar, sizeof(*pIpsddar) );
			if ( pIpsddar->dbl )
			{
				uint32 start, end;

				start = (uint32)KSEG1_ADDRESS(pIpsddar->ddbp);
				end = start + pIpsddar->dbl;
				/* align to 4-bytes */
				start &= ~3;
				end = ( end + 3 ) & ~3;
				modelExportMemory( start, end-start );
			}
		} while ( (pIpsddar++)->eor == 0 );

		/* Dump registers */
		modelExportRegister( IPSCTR );
		modelExportPointerRegister( IPSSDAR );
		modelExportPointerRegister( IPSDDAR );
		modelExportRegister( IPSCSR );
	}
	return SUCCESS;
}
#endif


/*
 *	This function dumps SWCORE tables with indirect access pattern.
 *
 *  Input: contentAddress -- the address of entry (for SWTAA)
 */
int32 modelExportSwcoreTable( uint32 type, uint32 idx, uint8* comment )
{
	uint32 p = (((uint32)rtl8651_asicTableAccessAddrBase(type)) + idx*RTL8651_ASICTABLE_ENTRY_LENGTH); /* pointer to read */
	uint32 swtacr;
	
	switch( expFormat )
	{
		case EXPORT_RTK_SIM:
			if ( comment ) modelExportComment( comment );

			/* Export swcore table with indirect access */
			WRITE_VIR32( TCR0, READ_VIR32(p+0*4) );
			WRITE_VIR32( TCR1, READ_VIR32(p+1*4) );
			WRITE_VIR32( TCR2, READ_VIR32(p+2*4) );
			WRITE_VIR32( TCR3, READ_VIR32(p+3*4) );
			WRITE_VIR32( TCR4, READ_VIR32(p+4*4) );
			WRITE_VIR32( TCR5, READ_VIR32(p+5*4) );
			WRITE_VIR32( TCR6, READ_VIR32(p+6*4) );
			WRITE_VIR32( TCR7, READ_VIR32(p+7*4) );

			WRITE_VIR32( SWTAA, ((uint32)REAL_SWTBL_BASE+((type)<<16)) + idx*RTL8651_ASICTABLE_ENTRY_LENGTH );
			swtacr = READ_VIR32( SWTACR ); /* keep original value since we will modify it to export later */
			WRITE_VIR32( SWTACR, CMD_FORCE|ACTION_START );

			modelExportRegister( SWTAA );
			modelExportMemory( TCR0, 4*8 );
			modelExportRegister( SWTACR );

			/* After export, we recover to original value */
			WRITE_MEM32( SWTACR, swtacr );
			break;

		default:
			modelExportMemory( p, 4*8 );
			break;
	}
	
	return SUCCESS;
}


/*
 *  This function only dumps SWCORE related registers.
 */
int32 modelExportSwcoreRegisters( void )
{
#if 0
	modelExportComment( "SWMACCR_BASE" );
	modelExportMemory( SWMACCR_BASE, 0x4c );
	modelExportComment( "PCRAM_BASE" );
	modelExportMemory( PCRAM_BASE, 0x54 );
	modelExportComment( "SWMISC_BASE" );
	modelExportMemory( SWMISC_BASE, 0x0c );
	modelExportComment( "ALE_BASE" );
	modelExportMemory( ALE_BASE, 0x5c );
	modelExportComment( "SBFCTR" );
	modelExportMemory( SBFCTR, 0xc0 );
	modelExportComment( "OQNCR_BASE" );
	modelExportMemory( OQNCR_BASE, 0x7C );
	modelExportComment( "PSCR" );
	modelExportMemory( PSCR, 0x10c );
	modelExportComment( "VCR" );
	modelExportMemory( VCR0, 0x9c );
	modelExportComment( "DOT1X_BASE" );
	modelExportMemory( DOT1X_BASE, 0x0c );
#endif

	modelExportComment( "MIB Counters P0 In" );
	modelExportMemory( MIB_COUNTER_BASE+0x100, 0x05c );
	modelExportComment( "MIB Counters P1 In" );
	modelExportMemory( MIB_COUNTER_BASE+0x180, 0x05c );
	modelExportComment( "MIB Counters P2 In" );
	modelExportMemory( MIB_COUNTER_BASE+0x200, 0x05c );
	modelExportComment( "MIB Counters P3 In" );
	modelExportMemory( MIB_COUNTER_BASE+0x280, 0x05c );
	modelExportComment( "MIB Counters P4 In" );
	modelExportMemory( MIB_COUNTER_BASE+0x300, 0x05c );
	modelExportComment( "MIB Counters P5 In" );
	modelExportMemory( MIB_COUNTER_BASE+0x380, 0x05c );
	modelExportComment( "MIB Counters Ext0 In" );
	modelExportMemory( MIB_COUNTER_BASE+0x400, 0x05c );

	modelExportComment( "MIB Counters P0 Out" );
	modelExportMemory( MIB_COUNTER_BASE+0x800, 0x038 );
	modelExportComment( "MIB Counters P1 Out" );
	modelExportMemory( MIB_COUNTER_BASE+0x880, 0x038 );
	modelExportComment( "MIB Counters P2 Out" );
	modelExportMemory( MIB_COUNTER_BASE+0x900, 0x038 );
	modelExportComment( "MIB Counters P3 Out" );
	modelExportMemory( MIB_COUNTER_BASE+0x980, 0x038 );
	modelExportComment( "MIB Counters P4 Out" );
	modelExportMemory( MIB_COUNTER_BASE+0xa00, 0x038 );
	modelExportComment( "MIB Counters P5 Out" );
	modelExportMemory( MIB_COUNTER_BASE+0xa80, 0x038 );
	modelExportComment( "MIB Counters Ext0 Out" );
	modelExportMemory( MIB_COUNTER_BASE+0xb00, 0x038 );

	modelExportComment( "CPU Interface Registers" );
	modelExportMemory( CPU_IFACE_BASE, 0x3c );
	modelExportComment( "SWCORE Misc Registers" );
	modelExportMemory( MISC_BASE+0x000, 0x028 );
	modelExportMemory( MISC_BASE+0x038, 0x004 );
	modelExportMemory( MISC_BASE+0x07C, 0x004 );
	modelExportMemory( MISC_BASE+0x0A0, 0x030 );
	modelExportMemory( MISC_BASE+0x100, 0x008 );

#if 0
	modelExportComment( "PHY Registers" );
	modelExportMemory( PHY_BASE, 0xE0 );
	modelExportComment( "System Registers" );
	modelExportMemory( SYSTEM_BASE, 0x40 );
#endif

	return SUCCESS;
}


/*
 *	This function only dumps SWCORE related registers and table content.
 */
int32 modelExportSwcoreRegistersAndTables( void )
{
	static char buf[256]; /* for comment */

	if ( modelExportDumpable() != SUCCESS ) return SUCCESS; /* fast return */
	
	sprintf( buf, "Export SWCORE Registers and Tables" );
	modelExportComment( buf );
	
	/*-----------------------------------------------------------------------
	 * Export SWCORE-related Registers
	 *---------------------------------------------------------------------*/
	modelExportSwcoreRegisters();

	/*-----------------------------------------------------------------------
	 * Export SWCORE-related Tables
	 *---------------------------------------------------------------------*/
	{
		int i;
		rtl865x_tblAsicDrv_l2Param_t L2;

		modelExportComment( "L2 Table" );
		for( i = 0; i < RTL8651_MACTBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicL2Table( i>>2, i&3, &L2 ) == SUCCESS )
			{
				sprintf( buf, "Row=%d Col=%d", i>>2, i&3 );
				modelExportSwcoreTable( TYPE_L2_SWITCH_TABLE, i, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_arpParam_t arp;

		modelExportComment( "ARP Table" );
		for( i = 0; i < RTL8651_ARPTBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicArp( i, &arp ) == SUCCESS &&
			     arp.aging>0 )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_ARP_TABLE, i, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_routingParam_t routing;

		modelExportComment( "Routing Table" );
		for( i = 0; i < RTL8651_ROUTINGTBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicRouting( i, &routing ) == SUCCESS )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_L3_ROUTING_TABLE, i, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_multiCastParam_t multicast;

		modelExportComment( "Multicast Table" );
		for( i = 0; i < RTL8651_MULTICASTTBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicIpMulticastTable( i, &multicast ) == SUCCESS )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_MULTICAST_TABLE, i, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_intfParam_t intf;

		modelExportComment( "Net Interface Table" );
		for( i = 0; i < RTL865XC_NETIFTBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicNetInterface( i, &intf ) == SUCCESS &&
			     intf.valid )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_NETINTERFACE_TABLE, i&7, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_vlanParam_t vlan;

		modelExportComment( "VLAN Table" );
		for( i = 0; i < 4096/*RTL8651_VLANTBL_SIZE*/; i++ )
		{
			if ( rtl8651_getAsicVlan( i, &vlan ) == SUCCESS &&
			     vlan.memberPortMask/*no member port means invalid*/ )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_VLAN_TABLE, i&4095, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_extIntIpParam_t ip;

		modelExportComment( "IP Table" );
		for( i = 0; i < RTL8651_IPTABLE_SIZE; i++ )
		{
			if ( rtl8651_getAsicExtIntIpTable( i, &ip ) == SUCCESS )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_EXT_INT_IP_TABLE, i, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_algParam_t alg;

		modelExportComment( "ALG Table" );
		for( i = 0; i < RTL865XC_ALGTBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicAlg( i, &alg ) == SUCCESS )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_ALG_TABLE, i, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_serverPortParam_t serverPort;

		modelExportComment( "Server Port Table" );
		for( i = 0; i < RTL8651_SERVERPORTTBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicServerPortTable( i, &serverPort ) == SUCCESS )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_SERVER_PORT_TABLE, i, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_naptTcpUdpParam_t tcpudp;

		modelExportComment( "TCPUDP Table" );
		for( i = 0; i < RTL8651_TCPUDPTBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicNaptTcpUdpTable( i, &tcpudp ) == SUCCESS &&
			     ( tcpudp.isValid==1 || tcpudp.isDedicated==1 ) )
			{
				sprintf( buf, "Index=%d", i );
				modelExportComment( buf );
				modelExportSwcoreTable( TYPE_L4_TCP_UDP_TABLE, i, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_naptIcmpParam_t icmp;

		modelExportComment( "ICMP Table" );
		for( i = 0; i < RTL8651_ICMPTBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicNaptIcmpTable( i, &icmp ) == SUCCESS &&
			     icmp.isValid )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_L4_ICMP_TABLE, i, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_pppoeParam_t pppoe;

		modelExportComment( "PPPoE Table" );
		for( i = 0; i < RTL8651_PPPOETBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicPppoe( i, &pppoe ) == SUCCESS &&
			     pppoe.age>0 )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_PPPOE_TABLE, i, buf );
			}
		}
	}
	{
		int i;
		_rtl8651_tblDrvAclRule_t acl;

		modelExportComment( "ACL Table" );
		for( i = 0; i < RTL8651_ACLTBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicAclRule( i, &acl ) == SUCCESS )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_ACL_RULE_TABLE, i, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_nextHopParam_t nexthop;

		modelExportComment( "Nexthop Table" );
		for( i = 0; i < RTL8651_NEXTHOPTBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicNextHopTable( i, &nexthop ) == SUCCESS )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_NEXT_HOP_TABLE, i, buf );
			}
		}
	}
	{
		int i;
		rtl865x_tblAsicDrv_rateLimitParam_t rateLimit;

		modelExportComment( "RateLimit Table" );
		for( i = 0; i < RTL8651_RATELIMITTBL_SIZE; i++ )
		{
			if ( rtl8651_getAsicRateLimitTable( i, &rateLimit ) == SUCCESS )
			{
				sprintf( buf, "Index=%d", i );
				modelExportSwcoreTable( TYPE_RATE_LIMIT_TABLE, i, buf );
			}
		}
	}

	return SUCCESS;
}

/*
 *	This function is called by program.
 *	It will output all IC registers.
 *	So, if your module needs to support export function, please add your code here.
 */
int32 modelExportAllRegisters( void )
{
	if ( modelExportDumpable() != SUCCESS ) return SUCCESS; /* fast return */
	
	modelExportGdmaRegisters();
#if defined(CONFIG_RTL865XB_EXP_CRYPTOENGINE) && defined(CONFIG_RTL865XC)
	modelExportCryptoRegisters();
#endif
	modelExportSwcoreRegistersAndTables();

	return SUCCESS;
}
#endif


int32 modelExportSetOutputForm( enum EXPORT_REGISTER_FORMAT newFormat )
{
	expFormat = newFormat;
	return SUCCESS;
}


int32 modelExportGetOutputForm( enum EXPORT_REGISTER_FORMAT *oldFormat )
{
	if ( oldFormat )
		*oldFormat = expFormat;
	return SUCCESS;
}


int32 modelExportSetIcType( enum IC_TYPE newType )
{
	expIcType = newType;
	return SUCCESS;
}


int32 modelExportGetIcType( enum IC_TYPE *oldType )
{
	if ( oldType )
		*oldType = expIcType;
	return SUCCESS;
}


static char SdramHeader[] = "\
pwr_rst;\n\
\n\
nop    2FF;\n\
\n\
//=================================================\n\
//   B800_1000: DeFAult 68A0_0000   \n\
//\n\
//   BROMSIZE       =Breg000[31:29];\n\
//   BSDRSIZE       =Breg000[28:26];\n\
//   BT_CAS         =Breg000[25];\n\
//   BFLSH0BW       =Breg000[24:23]; //reAD only\n\
//   BFLSH1BW       =Breg000[22:21];\n\
//   BSDBUSWID      =Breg000[20];\n\
//   BMCK1X         =Breg000[18];\n\
//   BBCKRATE       =Breg000[17:15];\n\
//   BLEDN          =Breg000[14];\n\
//   BSDCKDLY       =Breg000[1:0];\n\
//\n\
//================================================\n\
\n\
// 18001000 \n\
memw   18001000 68B00000;\n\
\n\
//===============================================\n\
//   B800_1004 : DeFAult FFFF_FFFF\n\
//\n\
//   BFLSH0_TCS=Breg004[31:28];\n\
//   BFLSH0_TOE=Breg004[27:24];\n\
//   BFLSH1_TCS=Breg004[23:20];\n\
//   BFLSH1_TOE=Breg004[19:16];\n\
//\n\
//===============================================\n\
\n\
memr 18001004 FFFFFFFF;\n\
memw 18001004 FFFFF884;\n\
memr 18001004 ;\n\
\n\
//===============================================\n\
//  B800_1008 : DeFAult FFFF_FFFF\n\
//\n\
//  menD_mr_sel=Breg008[20];\n\
//  BT_RCD     =Breg008[12:10];\n\
//  BT_RAS     =Breg008[9:5];\n\
//  BT_RFC     =Breg008[4:0];\n\
//\n\
//===============================================\n\
\n\
// 18001008 \n\
memr   18001008 00001FFF;\n\
memw   18001008 000004C9; //CheCking Burst reAD As CHGROW, memCtrl=synC\n\
memr   18001008 ;\n\
\n\
//===============================================\n\
//  B800_1100 : DeFAult 30FF_FFFF\n\
//\n\
//  BCE4RDY  = Breg100[31];\n\
//  reserveD = Breg100[30:24];\n\
//  BRBS     = Breg100[29];\n\
//  BWBS     = Breg100[28];\n\
//  BCE4_TWP = Breg100[23:20];\n\
//  BCE4_TWB = Breg100[19:16];\n\
//  BCE4_TRR = Breg100[15:12];\n\
//  BCE4_TREA= Breg100[11:8];\n\
//  BCE4_TH  = Breg100[7:4];\n\
//  BCE4_TS  = Breg100[3:0];\n\
//===============================================\n\
\n\
memr  18001100 30FFFFFF;\n\
memw  18001100 30FFFFFF;\n\
\n\
//===============================================\n\
// B800_1104: NAND type stAtus register \n\
//\n\
// NF_RDY    = Breg104[31]\n\
// NCF	     = Breg104[30:28]\n\
// NF_BW     = Breg104[27]\n\
//\n\
//===============================================\n\
\n\
memr  18001104 ;\n\
\n\
//===============================================\n\
// B800_1200: SPI FlAsh Control register\n\
//\n\
// BSPILEN =  Breg200[2:0];\n\
// BSPICS0B=  Breg200[3];\n\
// BSPICS1B=  Breg200[4];\n\
// BSPIWBS =  Breg200[5];\n\
// BSPIRBS =  Breg200[6];\n\
//\n\
//===============================================\n\
\n\
memw 18001200 0000047C;\n\
memr 18001200 ;\n\
\n\
//========= Header end ==========================\n\
\n";

int32 modelExportOpenFile( char* pszFilename )
{
	
//#ifdef RTL865X_MODEL_USER
#if 0
	if ( fdFile != -1 )
		modelExportCloseFile();

	if ( pszFilename == NULL )
	{
		if ( !strcmp( expFileName, STDOUT_FILENAME ) )
		{
			/* origin is stdout, we do not need to create slice file. */
			pszFilename = expFileName;
		}
		else
		{
			/* create slice files */
			static char buf[256];
			int slice;

			pszFilename = expFileName;
			/* find out a non-exist file */
			for( slice = 0; slice < 10000; slice++ )
			{
				if ( slice == 0 )
					sprintf( buf, "%s", pszFilename );
				else
					sprintf( buf, "%s.%04d", pszFilename, slice );
				
				fdFile = open( buf, O_RDONLY, 0666 );
				if ( fdFile == -1 )
					break;
				else
					close(fdFile);
			}
			if ( slice == 10000 && fdFile == -1 )
			{
				rtlglue_printf( "No more slice '%s.xxxx' allowed.\n", pszFilename );
				return FAILED;
			}
			else
			{
				pszFilename = buf;
			}
		}
	}
	else
	{
		/* save log filename */
		if ( !strcmp( pszFilename, "std" ) )
			pszFilename = STDOUT_FILENAME;
		strcpy( expFileName, pszFilename );
	}

	/* open log file */
	fdFile = open( pszFilename, O_CREAT | O_RDWR | O_APPEND, 0666 );
	if ( fdFile == -1 )
	{
		rtlglue_printf( "Open export logfile '%s' error: %s\n", pszFilename, strerror(errno) );
		return FAILED;
	}

	rtlglue_printf( "Export logfile '%s' is opened.\n", pszFilename );
#else
	fdFile = 0; /* set 0 to print out */
#endif
	expOutputtedline = 0;
	
	{
		enum IC_TYPE ic_type;
		enum IC_TYPE exp_ic_type;

		/* to output SDRAM setting header, we must set IC type to meet export IC type. */
		model_getTestTarget( &ic_type );
		modelExportGetIcType( &exp_ic_type );
		model_setTestTarget( exp_ic_type );
		modelExportRaw( SdramHeader );
		model_setTestTarget( ic_type );
	}
	return SUCCESS;
}

#if 0
int32 modelExportCloseFile( void )
{
//#ifdef RTL865X_MODEL_USER
#if 0
	if ( fdFile != -1 )
	{
		close( fdFile );
		fdFile = -1;
	}
#else
	fdFile = -1; /* set -1 not to print out */
#endif
	expOutputtedline = 0;

	rtlglue_printf( "Export log file '%s' is closed.\n", expFileName );
	return SUCCESS;
}


int32 runModelExport(uint32 userId,  int32 argc,int8 **saved)
{
	int8 *nextToken;
	int32 size;

	if ( defaultModelToPhysicalAdded == FALSE )
	{
		addModelToPhysical( (uint32)REAL_SWTBL_BASE, VIRTUAL_SWCORE_TBL_SIZE, PHYSICAL_ADDRESS(REAL_SWTBL_BASE) );
		addModelToPhysical( (uint32)REAL_SWCORE_BASE, VIRTUAL_SWCORE_REG_SIZE, PHYSICAL_ADDRESS(REAL_SWCORE_BASE) );
		addModelToPhysical( (uint32)REAL_SYSTEM_BASE, VIRTUAL_SYSTEM_REG_SIZE, PHYSICAL_ADDRESS(REAL_SYSTEM_BASE) );
		addModelToPhysical( (uint32)REAL_HSB_BASE, HSB_SIZE, PHYSICAL_ADDRESS(REAL_HSB_BASE) );
		addModelToPhysical( (uint32)REAL_HSA_BASE, HSA_SIZE, PHYSICAL_ADDRESS(REAL_HSA_BASE) );
//#ifdef RTL865X_MODEL_USER
#if 0
		addModelToPhysical( 0x08000000, 0x02000000, 0x00000000 ); /* map user-space memory map to physical address. */
#else /*RTL865X_MODEL_KERNEL*/
		addModelToPhysical( 0x80000000, 0x02000000, 0x00000000 ); /* KSEG0 */
		addModelToPhysical( 0xA0000000, 0x02000000, 0x00000000 ); /* KSEG1 */
#endif
		/* before map virtual address, we must switch to model code mode. Therefore, the address will be correct. */
		{
			enum IC_TYPE ic_type;

			model_getTestTarget( &ic_type );
			model_setTestTarget( IC_TYPE_MODEL );
			addModelToPhysical( (uint32)pVirtualSWTable, VIRTUAL_SWCORE_TBL_SIZE, PHYSICAL_ADDRESS(REAL_SWTBL_BASE) );
			addModelToPhysical( (uint32)pVirtualSWReg, VIRTUAL_SWCORE_REG_SIZE, PHYSICAL_ADDRESS(REAL_SWCORE_BASE) );
			addModelToPhysical( (uint32)pVirtualSysReg, VIRTUAL_SYSTEM_REG_SIZE, PHYSICAL_ADDRESS(REAL_SYSTEM_BASE) );
			addModelToPhysical( (uint32)pVirtualHsb, HSB_SIZE, PHYSICAL_ADDRESS(REAL_HSB_BASE) );
			addModelToPhysical( (uint32)pVirtualHsa, HSA_SIZE, PHYSICAL_ADDRESS(REAL_HSA_BASE) );
			model_setTestTarget( ic_type );
		}

		defaultModelToPhysicalAdded = TRUE;
	}
	cle_getNextCmdToken(&nextToken,&size,saved);
	if ( !strcmp( nextToken, "snapshot" ) )
	{
		modelExportSwcoreRegisters();
	}
	else if ( !strcmp( nextToken, "open" ) )
	{
		char *filename;
		
		/* get filename */
		cle_getNextCmdToken(&nextToken,&size,saved);
		filename = nextToken;
		
		return modelExportOpenFile( filename );
	}
	else if ( !strcmp( nextToken, "close" ) )
	{
		return modelExportCloseFile();
	}
	else if ( !strcmp( nextToken, "map" ) )
	{
		int i;

		rtlglue_printf("Model Address Space | Physical\n" );
		rtlglue_printf("--------------------+-------------\n" );
		for( i = 0; i < MAX_VIRTUAL_MAP; i++ )
		{
			if ( virtualMap[i].virtualStart != virtualMap[i].virtualEnd )
				rtlglue_printf("[%08x-%08x] | %08x\n", virtualMap[i].virtualStart, virtualMap[i].virtualEnd, virtualMap[i].physicalStart );
		}
	}
	else if ( !strcmp( nextToken, "swcoretable" ) )
	{
		modelExportSwcoreRegistersAndTables();
	}
	else if ( !strcmp( nextToken, "real" ) )
	{
		modelExportSetIcType( IC_TYPE_REAL );
	}
	else if ( !strcmp( nextToken, "model" ) )
	{
		modelExportSetIcType( IC_TYPE_MODEL );
	}
	else
		return FAILED;

	return SUCCESS;
}
#endif

