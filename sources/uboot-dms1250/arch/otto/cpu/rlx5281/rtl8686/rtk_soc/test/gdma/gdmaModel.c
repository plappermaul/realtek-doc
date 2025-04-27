/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Model code for pattern match
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: gdmaModel.c,v 1.27 2006-12-01 07:28:05 qy_wang Exp $
*/

#include "rtl_types.h"
#include "asicregs.h"
#include "gdma.h"
#include "gdmaModel.h"
#include "kmp.h"
#include "gdma_glue.h"
#include "modelTrace.h"
//#ifndef RTL865XC_MODEL_USE
//#include "model_cmd.h"
//#endif
#include "icExport.h"




static int32 srcBlkIdx;
static int32 srcPtrIdx;
static int32 dstBlkIdx;
static int32 dstPtrIdx;
static uint32 fsmIdx = -1;

#define SOURCE_BLOCK_LENGTH( idx ) ( READ_VIR32(GDMASBL0+idx*8) & GDMA_BLKLENMASK )
#define SOURCE_LAST_DATA_BLOCK( idx ) ( READ_VIR32(GDMASBL0+idx*8) & GDMA_LDB ? TRUE : FALSE )
#define DEST_BLOCK_LENGTH( idx ) ( READ_VIR32(GDMADBL0+idx*8) & GDMA_BLKLENMASK )
#define DEST_LAST_DATA_BLOCK( idx ) ( READ_VIR32(GDMADBL0+idx*8) & GDMA_LDB ? TRUE : FALSE )
#define _EQ '='
#define _CR 0x0d
#define _LF 0x0a

/* for debug readRule() function */
#undef _READ_RULE_DEBUG_

/* for evaluate pattern match performance */
#undef _PATTERM_PERFORMANCE_


static int32 resetSourcePointer( void )
{
	srcBlkIdx = 0;
	srcPtrIdx = 0;
	MT_WATCH("RESET");
	return SUCCESS;
}

static int32 resetDestinationPointer( void )
{
	dstBlkIdx = 0;
	dstPtrIdx = 0;
	MT_WATCH("RESET");
	return SUCCESS;
}
#ifdef CONFIG_RTL0371S
	#define TO_VIR_ADDR(phy_adr) ((phy_adr-0x4000000)|((unsigned int) 0xa0000000))
#else
	#define TO_VIR_ADDR(phy_adr) ((phy_adr)|((unsigned int) 0xa0000000))
#endif
static int32 readFromSource( void )
{
	int32 retval;

	if ( srcPtrIdx >= SOURCE_BLOCK_LENGTH(srcBlkIdx)  )
	{
		if ( SOURCE_LAST_DATA_BLOCK( srcBlkIdx ) )
		{
			/*WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_SBUDRIP);*/
			MT_WATCH("Source last block reached");
			return FAILED;
		}
		else
		{
			srcBlkIdx++;
			srcPtrIdx = 0;
		}
	}
	
	if ( srcBlkIdx >= 8 )
	{
		MT_WATCH("srcBlkIdx >= 8");
		return FAILED;
	}
	if ( SOURCE_BLOCK_LENGTH(srcBlkIdx) == 0 )
	{
		/*WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_SBUDRIP);*/
		MT_WATCH("source block length is 0");
		return FAILED;
	}
	
	retval = READ_VIR8( TO_VIR_ADDR( READ_VIR32(GDMASBP0+srcBlkIdx*8) + srcPtrIdx) );
	srcPtrIdx++;
	return retval;
}


static int32 readFromDestination( int32 pointerInc )
{
	int32 retval;
	
	if ( dstPtrIdx >= DEST_BLOCK_LENGTH(dstBlkIdx)  )
	{
		if ( DEST_LAST_DATA_BLOCK( dstBlkIdx ) )
		{
			/*WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_DBUDRIP);*/
			MT_WATCH("Destination last block reached");
			return FAILED;
		}
		else
		{
			dstBlkIdx++;
			dstPtrIdx = 0;
		}
	}
	
	if ( dstBlkIdx >= 8 )
	{
		MT_WATCH("dstBlkIdx >= 8");
		return FAILED;
	}
	if ( DEST_BLOCK_LENGTH(dstBlkIdx) == 0 )
	{
		MT_WATCH("destination block length is 0");
		return FAILED;
	}

	retval = READ_VIR8( TO_VIR_ADDR(READ_VIR32(GDMADBP0+dstBlkIdx*8) + dstPtrIdx) );
	if ( pointerInc ) dstPtrIdx++;
	return retval;
}


static int32 readRule( uint32 ruleIdx, void* pRule )
{
	uint32 subSM; /* sub state machine index */
#ifdef _READ_RULE_DEBUG_
	pmRule_t0 *p = (pmRule_t0*)pRule;
#endif
	uint32* pSubSM;

	subSM = ruleIdx >> INTERNAL_RULE_ORDER;

	/* If the rule is not in SRAM, reload it. */
	if ( subSM != fsmIdx )
	{
		fsmIdx = subSM;
	}
	pSubSM = (uint32*)*(uint32*)(READ_VIR32(GDMADBP0)+subSM*4);
#ifdef _READ_RULE_DEBUG_
	rtlglue_printf("[ruleIdx:0x%05x] pSubSM=0x%08x  ", ruleIdx, (uint32)pSubSM);
	rtlglue_printf("rule=0x%08x\n", pSubSM[ruleIdx&((1<<INTERNAL_RULE_ORDER)-1)] );
#endif

	*(uint32*)pRule = pSubSM[ruleIdx&((1<<INTERNAL_RULE_ORDER)-1)];
#ifdef _READ_RULE_DEBUG_
	rtlglue_printf("                  char='%c'(0x%02x) ", p->matchChar>=0x20&&p->matchChar<=0x7E?p->matchChar:'?', p->matchChar );
	rtlglue_printf("type=%1x ", p->type );
	rtlglue_printf("not=%1x ==> ", p->not );
	rtlglue_printf("cpu=%1x ", p->cpu );
	rtlglue_printf("nxt=0x%05x\n", p->jmpState );
#endif
	return SUCCESS;
}


static int32 writeToDestination( uint32 ch )
{
	if ( dstPtrIdx >= DEST_BLOCK_LENGTH(dstBlkIdx)  )
	{
		if ( DEST_LAST_DATA_BLOCK( dstBlkIdx ) )
		{
			/*WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_DBUDRIP);*/
			MT_WATCH("Destination last block reached");
			return FAILED;
		}
		else
		{
			dstBlkIdx++;
			dstPtrIdx = 0;
		}
	}
	
	if ( dstBlkIdx >= 8 )
	{
		MT_WATCH("dstBlkIdx >= 8");
		return FAILED;
	}
	if ( DEST_BLOCK_LENGTH(dstBlkIdx) == 0 )
	{
		MT_WATCH("destination block length is 0");
		return FAILED;
	}
	WRITE_VIR8( TO_VIR_ADDR(READ_VIR32(GDMADBP0+dstBlkIdx*8) + dstPtrIdx), ch );
	dstPtrIdx++;
	return SUCCESS;
}


/********************************************************************
 *   Memory Copy
 ********************************************************************/
static int32 modelGdmaMemoryCopy( void )
{
    uint8 src;
    int32 retval;
	enum STATE
	{
		INPUT0,
		OUTPUT0,
		STOPPED,
	};
	enum STATE state;
	src = 0; /* To initialize a value */

	resetSourcePointer();
	resetDestinationPointer();
	for( state = INPUT0;
	     state != STOPPED;
	     )
	{
		switch( state )
		{
			case INPUT0:
				retval = readFromSource();

				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					src = retval;
					state = OUTPUT0;
				}
				break;

			case OUTPUT0:
				retval = writeToDestination( src );
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					src = retval;
					state = INPUT0;
				}
				break;

			case STOPPED:
				break;
		}
	}

#if 0
	/* If source is under-run and destination is also under-run, clear the source under-run bit. */
	if ( ( READ_VIR32(GDMAISR)&GDMA_SBUDRIP ) && 
	     ( readFromDestination( TRUE ) == FAILED ) )
	{
		WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)&~(GDMA_SBUDRIP|GDMA_DBUDRIP));
	}
#endif
	
	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP );
	return SUCCESS;
}


/********************************************************************
 *   Checksum Offload
 ********************************************************************/
static int32 modelGdmaChecksumOffload( void )
{
	int32 retval;
	uint32 remind;
	uint32 sum;
	enum STATE
	{
		INPUT0,
		INPUT1,
		STOPPED,
	};
	enum STATE state;
	
	resetSourcePointer();
	resetDestinationPointer();
	for( state = INPUT0, remind = 0, sum = READ_VIR32( GDMAICVL );
	     state != STOPPED;
	     )
	{

		switch ( state )
		{
			case INPUT0:
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					remind = retval;
					state = INPUT1;
				}
				break;

			case INPUT1:
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					remind = ( remind << 8 ) | retval;
					sum += remind;
					state = INPUT0;
				}
				break;

			case STOPPED:
				break;
		}
	}

	WRITE_VIR32(GDMAICVL, sum);
	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP );
	return SUCCESS;
}


/********************************************************************
 *   Memory Compare
 ********************************************************************/
static int32 modelGdmaSequentialTCAM( void )
{
    int32 retval;
    int32 i;
    uint8 src[0x40];/* Mask & IP */
    uint32 dst[0x08];/* IP */
    int entSize=0; /* IP and Mask Entry Size, unit: bytes */
    uint32 index = 0;
	enum STATE
	{
		INPUT0,
		STOPPED,
	};
	enum STATE state;
	uint32 needCPU = 0;

	memset( dst, 0, sizeof(dst) );
	switch( READ_VIR32( GDMACNR ) & GDMA_ENTSIZMASK )
	{
		case GDMA_ENTSIZ256: /* Fall Through */
			entSize += 16;
			dst[4] = READ_VIR32( GDMADBP4 );
			dst[5] = READ_VIR32( GDMADBP5 );
			dst[6] = READ_VIR32( GDMADBP6 );
			dst[7] = READ_VIR32( GDMADBP7 );
		case GDMA_ENTSIZ128: /* Fall Through */
			entSize += 8;
			dst[2] = READ_VIR32( GDMADBP2 );
			dst[3] = READ_VIR32( GDMADBP3 );
		case GDMA_ENTSIZ64: /* Fall Through */
			entSize += 4;
			dst[1] = READ_VIR32( GDMADBP1 );
		case GDMA_ENTSIZ32:
			entSize += 4;
			dst[0] = READ_VIR32( GDMADBP0 );
			break;
	}

	resetSourcePointer();
	resetDestinationPointer();
	WRITE_VIR32(GDMAICVL, 0x00000000 );
	for( state = INPUT0;
	     state != STOPPED;
	     )
	{
		switch( state )
		{
			case INPUT0:
				/* Load Source */
				memset( src, 0, sizeof(src) );
				for( i = 0; i < entSize*2; i++ )
				{
					retval = readFromSource();
					if ( retval == FAILED )
					{
						needCPU = GDMA_NEEDCPUIP;
						goto out;
					}
					else
						src[i] = retval;
				}

				/* Compare with Destination */
				for( i = 0; i < entSize/4; i++ )
				{
					uint32 ip, mask;
					ip = big_endian(*(uint32*)&src[i*8+0]);
					mask = big_endian(*(uint32*)&src[i*8+4]);
					/*rtlglue_printf("[%d:%d]: dst[%d]=%08x, ip=%08x, mask=%08x\n", index, i, i, dst[i], ip, mask );*/
					if ( ( dst[i] & mask ) != ( ip & mask ) )
						goto not_match;
				}
				goto matched;
not_match:
				index++;
				break;
matched:
				state = STOPPED;
				break;
			
			case STOPPED:
				break;
		}
	}
out:
	WRITE_VIR32(GDMAICVL, index);
	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP|needCPU );
	return SUCCESS;
}			

#define CONFIG_RTL8316S
/********************************************************************
 *   Memory Set
 ********************************************************************/
static int32 modelGdmaMemorySet( void )
{
    uint32 src;
    int32 retval;
	enum STATE
	{
		OUTPUT0,
		OUTPUT1,
		OUTPUT2,
		OUTPUT3,
		STOPPED,
	};
	enum STATE state;

	resetSourcePointer();
	resetDestinationPointer();
	src = READ_VIR32( GDMAICVL );
	for( state = OUTPUT0;
	     state != STOPPED;
	     )
	{
		switch( state )
		{
			case OUTPUT0:
#ifdef CONFIG_RTL8316S
				retval = writeToDestination( (src>>0x18) & 0xff );
#else
				retval = writeToDestination( (src>>0x00) & 0xff );
#endif
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					state = OUTPUT1;
				}
				break;

			case OUTPUT1:
#ifdef CONFIG_RTL8316S
				retval = writeToDestination( (src>>0x10) & 0xff );
#else
				retval = writeToDestination( (src>>0x08) & 0xff );
#endif
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					state = OUTPUT2;
				}
				break;

			case OUTPUT2:
#ifdef CONFIG_RTL8316S
				retval = writeToDestination( (src>>0x08) & 0xff );
#else
				retval = writeToDestination( (src>>0x10) & 0xff );
#endif
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					state = OUTPUT3;
				}
				break;

			case OUTPUT3:
#ifdef CONFIG_RTL8316S
				retval = writeToDestination( (src>>0x00) & 0xff );
#else
				retval = writeToDestination( (src>>0x18) & 0xff );
#endif
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					state = OUTPUT0;
				}
				break;

			case STOPPED:
				break;
		}
	}

	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP );
	return SUCCESS;
}


/********************************************************************
 *   Michael, porting from 8185ag_tkip.c
 ********************************************************************/
static inline uint32 rotr(int bits, uint32 a)
{
	uint32 c,d,e,f,g;
    c = (0x0001 << bits)-1;
    d = ~c;

    e = (a & d) >> bits;
    f = (a & c) << (32 - bits);

    g = e | f;

    return (g & 0xffffffff );
}

static inline uint32 rotl(int bits, uint32 a)
{
	uint32 c,d,e,f,g;
    c = (0x0001 << (32-bits))-1;
    d = ~c;

    e = (a & c) << bits;
    f = (a & d) >> (32 - bits);

    g = e | f;

    return (g & 0xffffffff );
}

static inline uint32 xswap(uint32 in)
{
	uint32 a,b,c,d,out;

    a = in & 0xff;
    b = (in >> 8) & 0xff;
    c = (in >> 16) & 0xff;
    d = (in >> 24) & 0xff;

    out = 0;
    out |= c << 24;
    out |= d << 16;
    out |= a << 8;
    out |= b;

    return out;
}

static inline void block_function(
                    uint32 l,
                    uint32 r,
                    uint32 *l_out,
                    uint32 *r_out)
{
    r = r ^ rotl(17,l);
    l = (l + r);
    r = r ^ xswap(l);
    l = (l + r);
    r = r ^ rotl(3,l);
    l = (l + r);
    r = r ^ rotr(2,l);
	l = (l + r);
    *l_out = l;
    *r_out = r;
}

static int32 modelGdmaWirelessMIC( void )
{
    uint32 l_out, r_out;
    uint32 l,r;
    uint32 m;
    int32 retval;
	enum STATE
	{
		INPUT0,
		INPUT1,
		INPUT2,
		INPUT3,
		STOPPED,
	};
	enum STATE state;

	l = READ_VIR32( GDMAICVL );
	r = READ_VIR32( GDMAICVR );

	resetSourcePointer();
	resetDestinationPointer();
	for( state = INPUT0, m = 0;
	     state != STOPPED;
	     )
	{
		switch( state )
		{
			case INPUT0:
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					m = retval;
					state = INPUT1;
				}
				break;
				
			case INPUT1:
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					m |= retval<<8;
					state = INPUT2;
				}
				break;
				
			case INPUT2:
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					m |= retval<<16;
					state = INPUT3;
				}
				break;
				
			case INPUT3:
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					m |= retval<<24;
					state = INPUT0;

			        l = l ^ m;
			        block_function(l,r,&l_out,&r_out);
			        l = l_out;
			        r = r_out;
				}
				break;

			case STOPPED:
				break;
			}
	}

	WRITE_VIR32( GDMAICVL, l );
	WRITE_VIR32( GDMAICVR, r );
	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP );
	return SUCCESS;
}


/********************************************************************
 *   Memory XOR
 ********************************************************************/
static int32 modelGdmaMemoryXor( void )
{
    uint8 src, dst;
    int32 retval;
	enum STATE
	{
		INPUT0,
		INPUT1,
		OUTPUT0,
		STOPPED,
	};
	enum STATE state;
	src = dst = 0; /* To initialize a value */

	resetSourcePointer();
	resetDestinationPointer();
	for( state = INPUT0;
	     state != STOPPED;
	     )
	{
		switch( state )
		{
			case INPUT0:
				retval = readFromSource();

				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					src = retval;
					state = INPUT1;
				}
				break;

			case INPUT1:
				retval = readFromDestination( FALSE );

				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					dst = retval;
					state = OUTPUT0;
				}
				break;

			case OUTPUT0:
				dst = src ^ dst;
				retval = writeToDestination( dst );
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					state = INPUT0;
				}
				break;

			case STOPPED:
				break;
		}
	}

#if 0
	/* If source is under-run and destination is also under-run, clear the source under-run bit. */
	if ( ( READ_VIR32(GDMAISR)&GDMA_SBUDRIP ) && 
	     ( readFromDestination( TRUE ) == FAILED ) )
	{
		WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)&~(GDMA_SBUDRIP|GDMA_DBUDRIP));
	}
#endif

	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP );
	return SUCCESS;
}


/********************************************************************
 *   BASE64 CHARSET
 ********************************************************************/
static int8 b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static int32 modelGdmaBase64Encode( void )
{
	int32 retval;
	uint32 remind;
	uint32 processed;
	uint32 outputted;
	int len;
	int maxLen;
	enum STATE
	{
		INPUT0,
		INPUT1,
		INPUT2,
		INPUT3,
		STOPPED,
	};
	enum STATE state;
	
	resetSourcePointer();
	resetDestinationPointer();

	maxLen = ( ( READ_VIR32(GDMAICVL) & 0xff ) + 1 ) * 4;

	for( state = INPUT0, remind = 0, processed = 0, outputted = 0, len = 0;
	     state != STOPPED;
	     )
	{

		switch ( state )
		{
			case INPUT0: /* Wait for first byte */
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					remind = (remind<<8)|retval;

					if ( len >= maxLen )
					{
						/* exceed maxLen, insert CRLF */
						len = 0;
						if ( writeToDestination( _CR ) == FAILED ) goto out;
						outputted++;
						if ( writeToDestination( _LF ) == FAILED ) goto out;
						outputted++;
					}
					if ( writeToDestination( b64[(remind&0xfc)>>2] ) == FAILED ) goto out;
					outputted++;
					processed++;
					len++;
					state = INPUT1;
				}
				break;

			case INPUT1: /* Wait for second byte */
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
					if ( writeToDestination( b64[(remind&0x03)<<4] ) == FAILED ) goto out;					
					outputted++;
					if ( writeToDestination( _EQ ) == FAILED ) goto out;
					outputted++;
					if ( writeToDestination( _EQ ) == FAILED ) goto out;
					outputted++;
				}
				else
				{
					remind = (remind<<8)|retval;

					if ( len >= maxLen )
					{
						/* exceed maxLen, insert CRLF */
						len = 0;
						if ( writeToDestination( _CR ) == FAILED ) goto out;
						outputted++;
						if ( writeToDestination( _LF ) == FAILED ) goto out;
						outputted++;
					}
					if ( writeToDestination( b64[(remind&0x3f0)>>4] ) == FAILED ) goto out;
					outputted++;
					processed++;
					len++;
					state = INPUT2;
				}
				break;

			case INPUT2:
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
					if ( writeToDestination( b64[(remind&0xf)<<2] ) == FAILED ) goto out;					
					outputted++;
					if ( writeToDestination( _EQ ) == FAILED ) goto out;
					outputted++;
				}
				else
				{
					remind = (remind<<8)|retval;

					if ( len >= maxLen )
					{
						/* exceed maxLen, insert CRLF */
						len = 0;
						if ( writeToDestination( _CR ) == FAILED ) goto out;
						outputted++;
						if ( writeToDestination( _LF ) == FAILED ) goto out;
						outputted++;
					}
					if ( writeToDestination( b64[(remind&0xfc0)>>6] ) == FAILED ) goto out;
					outputted++;
					processed++;
					len++;
					state = INPUT3;
				}
				break;

			case INPUT3:
				if ( len >= maxLen )
				{
					/* exceed maxLen, insert CRLF */
					len = 0;
					if ( writeToDestination( _CR ) == FAILED ) goto out;
					outputted++;
					if ( writeToDestination( _LF ) == FAILED ) goto out;
					outputted++;
				}
				if ( writeToDestination( b64[(remind&0x3f)] ) == FAILED ) goto out;
				outputted++;
				len++;
				remind = 0;
				state = INPUT0;
				break;

			case STOPPED:
				break;
		}
	}

out:
	WRITE_VIR32(GDMAICVL,processed<<16|outputted);
	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP );
	return SUCCESS;
}


static int32 modelGdmaBase64Decode( void )
{
	int32 retval;
	uint32 remind;
	uint32 processed;
	uint32 subp;
	uint32 outputted;
	int8 *p;
	enum STATE
	{
		INPUT0,
		INPUT1,
		INPUT2,
		INPUT3,
		STOPPED,
	};
	enum STATE state;
	
	resetSourcePointer();
	resetDestinationPointer();
	for( state = INPUT0, remind = 0, processed = 0, outputted = 0, subp = 0;
	     state != STOPPED;
	     )
	{

		switch ( state )
		{
			case INPUT0: /* Wait for first byte */
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					if ( ( p = strchr( b64, retval ) ) )
					{
						subp++;
						remind = (remind<<6)|(p-b64);
						state = INPUT1;
					}
					else
					{
						processed++;
					}
				}
				break;

			case INPUT1: /* Wait for second byte */
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					subp++;
					if ( ( p = strchr( b64, retval ) ) )
					{
						remind = (remind<<6)|(p-b64);
						state = INPUT2;
					}
				}
				break;

			case INPUT2:
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					if ( ( p = strchr( b64, retval ) ) )
					{
						subp++;
						remind = (remind<<6)|(p-b64);
						state = INPUT3;
					}
					else
					{
						if ( retval == _EQ )
						{
							if ( writeToDestination( remind>>4 ) == FAILED ) goto out;
							outputted++;
							processed += (subp+1);
							state = STOPPED;
						}
						else
						{
							/* non-base64 char, and not _EQ */
							subp++;
						}
					}
				}
				break;

			case INPUT3:
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					if ( ( p = strchr( b64, retval ) ) )
					{
						remind = (remind<<6)|(p-b64);
						if ( writeToDestination( remind>>16 ) == FAILED ) goto out;
						outputted++;
						if ( writeToDestination( (remind>>8)&0xff ) == FAILED ) goto out;
						outputted++;
						if ( writeToDestination( remind&0xff ) == FAILED ) goto out;
						outputted++;
						processed += (subp+1);
						state = INPUT0;
						remind = 0;
						subp = 0;
					}
					else
					{
						if ( retval == _EQ )
						{
							if ( writeToDestination( remind>>10 ) == FAILED ) goto out;
							outputted++;
							if ( writeToDestination( (remind>>2)&0xff ) == FAILED ) goto out;
							outputted++;
							processed += (subp+1);
							state = STOPPED;
						}
						else
						{
							/* non-base64 char, and not _EQ */
							subp++;
						}
					}
				}
				break;

			case STOPPED:
				break;
		}
	}

out:
	WRITE_VIR32(GDMAICVL,processed<<16|outputted);
	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP );
	return SUCCESS;
}


/********************************************************************
 *   QUOTED-PRINTABLE CHARSET
 ********************************************************************/
static uint8 qp[] = "0123456789ABCDEF";

/*
 *	This function will check if 'ch' needs QP to encode.
 *	If yes, it will return TRUE and encoded 3 characters in ch1, ch2.
 *	If no, it will return FALSE.
 */
static int32 needEncodeQP( uint8 ch, uint8* ch1, uint8* ch2 )
{
	if ( ( ch == 0x0a ) ||
	     ( ch == 0x0d ) ||
	     ( ch >= 0x21 && ch <= 0x3C ) ||
	     ( ch >= 0x3E && ch <= 0x7E ) )
	{
		/* No need to encode */
		return FALSE;
	}
	else
	{
		*ch1 = qp[ch>>4];
		*ch2 = qp[ch&0xf];
		return TRUE;
	}
}


static int32 modelGdmaQuotedPrintableEncode( void )
{
	int32 retval;
	uint32 processed;
	uint32 outputted;
	int len;
	int maxLen;
	enum STATE
	{
		INPUT0,
		STOPPED,
	};
	enum STATE state;
	
	resetSourcePointer();
	resetDestinationPointer();

	maxLen = ( ( READ_VIR32(GDMAICVL) & 0xff ) + 1 ) * 4;

	for( state = INPUT0, processed = 0, outputted = 0, len = 0;
	     state != STOPPED;
	     )
	{

		switch ( state )
		{
			case INPUT0:
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					uint8 ch1, ch2;
					
					if ( needEncodeQP( retval, &ch1, &ch2 ) )
					{
						uint32 outputted2 = 0;
						
						if ( len+3 >= maxLen )
						{
							/* exceed maxLen, insert =CRLF */
							len = 0;
							if ( writeToDestination( _EQ ) == FAILED ) goto out;
							if ( writeToDestination( _CR ) == FAILED ) goto out;
							if ( writeToDestination( _LF ) == FAILED ) goto out;
							outputted2 = 3;
						}

						/* output encoded characters */
						if ( writeToDestination( _EQ ) == FAILED ) goto out;
						if ( writeToDestination( ch1 ) == FAILED ) goto out;
						if ( writeToDestination( ch2 ) == FAILED ) goto out;
						outputted += ( 3 + outputted2 );
						len += 3;
					}
					else
					{
						if ( retval == _LF || retval == _CR )
						{
							/* output line feed and carrier return character */
							if ( writeToDestination( retval ) == FAILED ) goto out;
							outputted++;
							len = 0;
						}
						else
						{
							uint32 outputted2 = 0;
							
							if ( len+1 >= maxLen )
							{
								/* exceed maxLen, insert =CRLF */
								len = 0;
								if ( writeToDestination( _EQ ) == FAILED ) goto out;
								if ( writeToDestination( _CR ) == FAILED ) goto out;
								if ( writeToDestination( _LF ) == FAILED ) goto out;
								outputted2 = 3;
							}

							/* output original character */
							if ( writeToDestination( retval ) == FAILED ) goto out;
							outputted += ( 1 + outputted2 );
							len++;
						}
					}
					
					processed++;
					state = INPUT0;
				}
				break;

			case STOPPED:
				break;
		}
	}

out:
	WRITE_VIR32(GDMAICVL,processed<<16|outputted);
	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP );
	return SUCCESS;
}


/*
 *	This function will decode ch1 and ch2.
 *	If success, it will return TRUE and return decoded character in ch.
 *	If fail, it will return FALSE.
 */
static int32 decodeQP( uint8 *ch, uint8 ch1 )
{
	uint8 r = 0;
	
	if ( ( ch1 >= '0' ) && ( ch1 <= '9' ) ) r = ch1 - '0';
	else if ( ( ch1 >= 'A' ) && ( ch1 <= 'F' ) ) r = ch1 - 'A' + 10;
	else return FALSE;

	*ch = r;
	return TRUE;
}


static int32 modelGdmaQuotedPrintableDecode( void )
{
	int32 retval;
	uint32 processed;
	uint32 outputted;
	uint32 remind;
	enum STATE
	{
		INPUT0,
		INPUT1,
		INPUT2,
		INPUT3,
		STOPPED,
	};
	enum STATE state;
	remind = 0;
	
	resetSourcePointer();
	resetDestinationPointer();

	for( state = INPUT0, processed = 0, outputted = 0;
	     state != STOPPED;
	     )
	{

		switch ( state )
		{
			case INPUT0: /* Normal State */
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					if ( retval == _EQ )
					{
						state = INPUT1;
					}
					else
					{	/* non-_EQ */
						if ( writeToDestination( retval ) == FAILED ) goto out;
						outputted++;
						processed++;
						state = INPUT0;
					}
				}
				break;

			case INPUT1: /* Accepted = symbol, wait first hex */
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					uint8 ch1;
					
					if ( retval == _CR )
					{
						state = INPUT3;
					}
					else if ( decodeQP( &ch1, retval ) )
					{
						remind = ch1;
						state = INPUT2;
					}
					else
					{
						state = STOPPED;
					}
				}
				break;

			case INPUT2: /* Accepted first hex, wait second hex */
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					uint8 ch2;
					
					if ( decodeQP( &ch2, retval ) )
					{
						remind = ( remind << 4 ) | ch2;

						if ( writeToDestination( remind ) == FAILED ) goto out;
						outputted++;
						processed+=3;
						remind = 0;
						state = INPUT0;
					}
					else
					{
						state = STOPPED;
					}
				}
				break;

			case INPUT3: /* Accepted _CR symbol, wait _LF symbol */
				retval = readFromSource();
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					if ( retval == _LF )
					{
						processed+=3;
						state = INPUT0;
					}
					else
					{
						state = STOPPED;
					}
				}
				break;

			case STOPPED:
				break;
		}
	}

out:
	WRITE_VIR32(GDMAICVL,processed<<16|outputted);
	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP );
	return SUCCESS;
}


/********************************************************************
 *   Memory Compare
 ********************************************************************/
static int32 modelGdmaMemoryCompare( void )
{
    uint8 src, dst;
    int32 retval;
	enum STATE
	{
		INPUT0,
		INPUT1,
		STOPPED,
	};
	enum STATE state;
	src = 0; /* To initialize a value */

	resetSourcePointer();
	resetDestinationPointer();
	WRITE_VIR32(GDMAICVL, 0x00000000 );
	for( state = INPUT0;
	     state != STOPPED;
	     )
	{
		switch( state )
		{
			case INPUT0:
				retval = readFromSource();

				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					src = retval;
					state = INPUT1;
				}
				break;

			case INPUT1:
				retval = readFromDestination( TRUE );

				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					dst = retval;

					if ( src == dst )
					{
						/* identical, continue */
						state = INPUT0;
					}
					else
					{
						WRITE_VIR32(GDMAICVL, ((uint32)src-(uint32)dst)&0x01ff );
						state = STOPPED;
					}
				}
				break;

			case STOPPED:
				break;
		}
	}

#if 0
	/* If source is under-run and destination is also under-run, clear the source under-run bit. */
	if ( ( READ_VIR32(GDMAISR)&GDMA_SBUDRIP ) && 
	     ( readFromDestination( TRUE ) == FAILED ) )
	{
		WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)&~(GDMA_SBUDRIP|GDMA_DBUDRIP));
	}
#endif

	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP );
	return SUCCESS;
}


/********************************************************************
 *   Byte Swap
 ********************************************************************/
static int32 modelGdmaByteswap( void )
{
    uint32 src, dst;
    int32 retval;
	enum STATE
	{
		INPUT0,
		INPUT1,
		INPUT2,
		INPUT3,
		OUTPUT0,
		OUTPUT1,
		OUTPUT2,
		OUTPUT3,
		STOPPED,
	};
	enum STATE state;
	src = dst = 0; /* To initialize a value */

	resetSourcePointer();
	resetDestinationPointer();
	WRITE_VIR32(GDMAICVL, 0x00000000 );
	for( state = INPUT0;
	     state != STOPPED;
	     )
	{
		switch( state )
		{
			case INPUT0:
				retval = readFromSource();

				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					src = retval;
					state = INPUT1;
				}
				break;

 			case INPUT1:
				retval = readFromSource();

				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					src = (src<<8) | retval;
					state = INPUT2;
				}
				break;

 			case INPUT2:
				retval = readFromSource();

				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					src = (src<<8) | retval;
					state = INPUT3;
				}
				break;

			case INPUT3:
				retval = readFromSource();

				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					src = (src<<8) | retval;

					if ( READ_VIR32( GDMACNR ) & GDMA_SWAPTYPE1 )
					{
						dst = (src>>24) | ((src&0x00ff0000)>>8) | ((src&0x0000ff00)<<8) | (src<<24);
					}
					else
					{
						dst = ((src&0xff000000)>>8) | ((src&0x00ff0000)<<8) | ((src&0x0000ff00)>>8) | ((src&0x000000ff)<<8);
					}
					
					state = OUTPUT0;
				}
				break;

			case OUTPUT0:
				retval = writeToDestination( ( dst>>24 ) & 0xff );
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					state = OUTPUT1;
				}
				break;

			case OUTPUT1:
				retval = writeToDestination( ( dst>>16 ) & 0xff );
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					state = OUTPUT2;
				}
				break;

			case OUTPUT2:
				retval = writeToDestination( ( dst>>8 ) & 0xff );
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					state = OUTPUT3;
				}
				break;

			case OUTPUT3:
				retval = writeToDestination( dst & 0xff );
				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					state = INPUT0;
				}
				break;

			case STOPPED:
				break;
		}
	}

#if 0
	/* If source is under-run and destination is also under-run, clear the source under-run bit. */
	if ( ( READ_VIR32(GDMAISR)&GDMA_SBUDRIP ) && 
	     ( readFromDestination( TRUE ) == FAILED ) )
	{
		WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)&~(GDMA_SBUDRIP|GDMA_DBUDRIP));
	}
#endif

	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP );
	return SUCCESS;
}


/********************************************************************
 *   Pattern Match
 ********************************************************************/
typedef struct internal_sram_s
{
	uint32 valid:1;
	uint32 tag:12;
} internal_sram_t;
internal_sram_t intSram[2];
static int32 modelGdmaPatternMatch( void )
{
    uint8 src;
    int32 retval;
	enum STATE
	{
		INPUT0,
		INPUT1,
		STOPPED,
	};
	enum STATE state;
	uint32 ruleIdx;
	pmRule_t0 rule;
	uint32 processed = 0; /* To indicate which character causes FSM stops. */
	uint32 needCPU = 0;
#if defined(_PATTERM_PERFORMANCE_)
	int32 cntMatched = 0;/* statistic for matched count */
	int32 cntNotMatched = 0; /* statistic for matched count */
#endif

	src = 0; /* To initialize a value */

	ruleIdx = READ_VIR32( GDMAICVL );
	memset( intSram, 0, sizeof(intSram) );
	resetSourcePointer();
	resetDestinationPointer();
	for( state = INPUT0;
	     state != STOPPED;
	     )
	{
		switch( state )
		{
			case INPUT0:
				retval = readFromSource();

				if ( retval == FAILED )
				{
					state = STOPPED;
				}
				else
				{
					processed++;
					src = retval;
					state = INPUT1;
				}
				break;

			case INPUT1:
				readRule( ruleIdx, &rule );
#if defined(_READ_RULE_DEBUG_)
				rtlglue_printf("rule[%05x] curr'%c'(%02x)%s'%c'(%02x) ==> %05x%s\n", 
				                ruleIdx, src>=0x20&&src<=0x7E?src:'?', src, rule.not?"!=":"==",
				                rule.matchChar>=0x20&&rule.matchChar<=0x7E?rule.matchChar:'?', rule.matchChar, rule.jmpState, rule.cpu?"(CPU)":"" );
#endif
				switch( rule.type )
				{
					case 0:
						if ( ( rule.not == 0 && src == rule.matchChar ) ||
						     ( rule.not == 1 && src != rule.matchChar ) )
						{
							/* I matched the rule! */
							
							if ( rule.cpu == 1 )
							{
#if 0 /* Limit: To complete loopback branch in one tick, we must ignore the CPU bit. */
								if ( ruleIdx == rule.jmpState )
								{
									/* loopback branch, ignore CPU bit. */
									state = INPUT0;
								}
								else
								{
									state = STOPPED;
									needCPU = GDMA_NEEDCPUIP;
								}
#else
								state = STOPPED;
								needCPU = GDMA_NEEDCPUIP;
#endif
							}
							else
							{
								state = INPUT0;
							}
							
							ruleIdx = rule.jmpState;
#if defined(_PATTERM_PERFORMANCE_)
							cntMatched++;
#endif
						}
						else
						{
							/* The rule is not match. */
							ruleIdx++;
#if defined(_PATTERM_PERFORMANCE_)
							cntNotMatched++;
#endif
						}
						break;
					case 1:
						rtlglue_printf("Unsupported rule type 1.\n");
						break;
					case 2:
						rtlglue_printf("Unsupported rule type 2.\n");
						break;
					default:
						rtlglue_printf("Unsupported rule type %d.\n", rule.type );
						break;
				}
				
				break;

			case STOPPED:
				break;
		}
	}

#if 0
	/* If source is under-run and destination is also under-run, clear the source under-run bit. */
	if ( ( READ_VIR32(GDMAISR)&GDMA_SBUDRIP ) && 
	     ( readFromDestination( TRUE ) == FAILED ) )
	{
		WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)&~(GDMA_SBUDRIP|GDMA_DBUDRIP));
	}
#endif

	/* to invalidate SRAM content. */
	fsmIdx = -1;
	WRITE_VIR32(GDMAISR, READ_VIR32(GDMAISR)|GDMA_COMPIP|needCPU );
	WRITE_VIR32(GDMAICVL, ruleIdx );
	WRITE_VIR32(GDMAICVR, processed );

#if defined(_PATTERM_PERFORMANCE_)
	{
		int32 totalCycle;

		totalCycle = cntMatched*2+cntNotMatched;
		rtlglue_printf("Rule Matched Count: %d\n", cntMatched );
		rtlglue_printf(" Not Matched Count: %d\n", cntNotMatched );
		rtlglue_printf("Total cycle: %d cycles    %d times\n", totalCycle, 200*1000*1000/totalCycle );
	}
#endif

	return SUCCESS;
}


int32 modelGdma( void )
{
	if ( READ_VIR32(GDMACNR) & GDMA_ENABLE )
	{
		/* Set to output 'memw ...' */
		modelExportComment( "====================================================================" );
		modelExportComment( "  Export GDMA registers and related memory content." );
		switch ( READ_VIR32(GDMACNR) & GDMA_FUNCMASK )
		{
			case GDMA_MEMCPY:
				modelExportComment( "* Memory Copy" );
				break;
			case GDMA_CHKOFF:
				modelExportComment( "* Checksum Offload" );
				break;
			case GDMA_STCAM:
				modelExportComment( "* Sequential TCAM" );
				break;
			case GDMA_MEMSET:
				modelExportComment( "* Memory Set" );
				break;
			case GDMA_B64ENC:
				modelExportComment( "* Base64 Encode" );
				break;
			case GDMA_B64DEC:
				modelExportComment( "* Base64 Decode" );
				break;
			case GDMA_QPENC:
				modelExportComment( "* Quoted Printable Encode" );
				break;
			case GDMA_QPDEC:
				modelExportComment( "* Quoted Printable Decode" );
				break;
			case GDMA_MIC:
				modelExportComment( "* Wireless MIC" );
				break;
			case GDMA_MEMXOR:
				modelExportComment( "* Memory XOR" );
				break;
			case GDMA_MEMCMP:
				modelExportComment( "* Memory Compare" );
				break;
			case GDMA_BYTESWAP:
				modelExportComment( "* Byte Swap" );
				break;
			case GDMA_PATTERN:
				modelExportComment( "* Pattern Match" );
				break;
			default:
				modelExportComment( "* ERROR! Unsupported GDMA Function" );
				break;
		}

		modelExportSetOutputForm(EXPORT_RTK_SIM);
		modelExportGdmaRegisters();

		{/* count nop delay */
			int i, len = 0;
			static char buf[256];

			for( i = 0; i < 8; i++ )
				len += READ_VIR32( GDMASBL0+i*8 ) & GDMA_BLKLENMASK;
			for( i = 0; i < 8; i++ )
				len += ( READ_VIR32( GDMADBL0+i*8 ) & GDMA_BLKLENMASK ) * 2;
			len = ((len+99)/100)*100*2; /* according to limit's suggest. */
			if ( len <= 0 ) len = 100;
			sprintf( buf, "nop %d;\n", len );
			modelExportRaw( buf );
		}
		
		WRITE_VIR32(GDMACNR, READ_VIR32(GDMACNR) & ~GDMA_POLL );
		switch ( READ_VIR32(GDMACNR) & GDMA_FUNCMASK )
		{
			case GDMA_MEMCPY:
				modelGdmaMemoryCopy();
				break;

			case GDMA_CHKOFF:
				modelGdmaChecksumOffload();
				break;
				
			case GDMA_STCAM:
				modelGdmaSequentialTCAM();
				break;

			case GDMA_MEMSET:
				modelGdmaMemorySet();
				break;
				
			case GDMA_B64ENC:
				modelGdmaBase64Encode();
				break;
				
			case GDMA_B64DEC:
				modelGdmaBase64Decode();
				break;
				
			case GDMA_QPENC:
				modelGdmaQuotedPrintableEncode();
				break;
				
			case GDMA_QPDEC:
				modelGdmaQuotedPrintableDecode();
				break;

			case GDMA_MIC:
				modelGdmaWirelessMIC();
				break;
				
			case GDMA_MEMXOR:
				modelGdmaMemoryXor();
				break;
				
			case GDMA_MEMCMP:
				modelGdmaMemoryCompare();
				break;
				
			case GDMA_BYTESWAP:
				modelGdmaByteswap();
				break;

			case GDMA_PATTERN:
				modelGdmaPatternMatch();
				break;

			default:
				break;
		}

		/* Set to output 'memr ...' */
		modelExportComment( "  After execute GDMA, dump expected results." );
		modelExportSetOutputForm(EXPORT_RTK_EXPECT);
		modelExportGdmaRegisters();
		modelExportComment( "--------------------------------------------------------------------" );
		modelExportComment( NULL );
	}
	
	return SUCCESS;
}



