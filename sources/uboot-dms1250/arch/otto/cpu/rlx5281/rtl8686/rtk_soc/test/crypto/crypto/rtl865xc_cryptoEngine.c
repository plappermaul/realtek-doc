/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003  
* All rights reserved.
* 
* Program : 8651B crypto engine driver code
* Abstract : 
* $Id: rtl865xc_cryptoEngine.c,v 1.8 2006/03/17 08:55:36 yjlou Exp $
*
* --------------------------------------------------------------------
*/

#include <common.h>
#include "rtl865xc_cryptoEngine.h"
#include "crypto.h"
//#ifdef CONFIG_RTL865X_MODEL_KERNEL
//#include "modelTrace.h"
//#include "icExport.h"
//#include "virtualMac.h"
//#endif
//#include <linux/autoconf.h>
//#include <asicRegs.h>
//#include "8672_asicregs.h"
//#include <rtl_glue.h>
//#include <rtl_utils.h>
#include "assert.h"
#include "rtl865xc_asicregs.h"
/*
#define rtlglue_malloc malloc
#define rtlglue_free free
#define rtlglue_printf printf
*/
/*  Crypto engine debug level
 */
#define _CRYPTO_DEBUG_ 1


/*  Data Segment for IC Verification
 *    In the normal mode (also compatible mode), 'DATA_SEG' is set to 1.
 *    When we want to test the multiple data segment mode, we shall set 'DATA_SEG' more than 1.
 */
#define DATA_SEG 1


uint32 numSrc, numDst; /* the number of source & destination descriptor */
uint32 idxCpuSrc, idxAsicSrc;
uint32 idxCpuDst, idxAsicDst;

/*
Assumption:
1. rtlglue_malloc() can get 4-byte aligned memory block
*/
rtl865xc_crypto_source_t *ipssdar;
rtl865xc_crypto_dest_t *ipsddar;
/*Counters for interrupt*/
static uint32 cryptoDoneIntCounter, cryptoAllDoneIntCounter;



int32 rtl8651b_cryptoEngine_alloc(uint32 descNum) 
{
	void *p; /* for memory allocate */

	/* Allocate descriptor ring */
	if ( ipssdar == NULL )
	{
		numSrc = descNum;
		p = rtlglue_malloc( numSrc * sizeof( rtl865xc_crypto_source_t ) );
		if ( p == NULL ) 
		{ 
			rtlglue_printf("%s():%d memory allocate failed.\n", __FUNCTION__, __LINE__ ); 
			numSrc = -1;
			return FAILED; 
		}
		ipssdar = UNCACHED_ADDRESS(p);
		memset( ipssdar, 0, numSrc * sizeof( rtl865xc_crypto_source_t ) );
		WRITE_MEM32( IPSSDAR, PHYSICAL_ADDRESS((uint32)ipssdar) );
		#if _CRYPTO_DEBUG_>=2
		rtlglue_printf( "Set IPSSDAR=0x%08x, ipssdar = 0x%08x\n", PHYSICAL_ADDRESS((uint32)ipssdar) , ipssdar);
		#endif
		ipssdar[numSrc-1].eor = 1;
	}
	else
	{
		WRITE_MEM32( IPSSDAR, PHYSICAL_ADDRESS((uint32)ipssdar) );
	}

	/* Allocate descriptor ring */
	if ( ipsddar == NULL )
	{
		numDst = descNum;
		p = rtlglue_malloc( numDst * sizeof( rtl865xc_crypto_dest_t ) );
		if ( p == NULL ) 
		{ 
			rtlglue_printf("%s():%d memory allocate failed.\n", __FUNCTION__, __LINE__ );
			numDst = -1;
			return FAILED; 
		}
		ipsddar = UNCACHED_ADDRESS(p);
		memset( ipsddar, 0, numDst * sizeof( rtl865xc_crypto_dest_t ) );
		WRITE_MEM32( IPSDDAR, PHYSICAL_ADDRESS((uint32)ipsddar) );
		#if _CRYPTO_DEBUG_>=2
		rtlglue_printf( "Set IPSDDAR=0x%08x\n", PHYSICAL_ADDRESS((uint32)ipsddar) );
		#endif
		ipsddar[numDst-1].eor = 1;
	}
	else
	{
		WRITE_MEM32( IPSDDAR, PHYSICAL_ADDRESS((uint32)ipsddar) );
	}

	return SUCCESS;
}

static uint32 _descNum;
static int8 _mode32Bytes;
/*
 *  descNum -- number of descriptor to be allocated.
 *  mode32Bytes -- 0: 16word
 *                 1: 32word
 *                 2: 64word
 */
int32 rtl8651b_cryptoEngine_init(uint32 descNum, int8 mode32Bytes) 
{
	uint32 burstSize;

	_descNum = descNum;
	_mode32Bytes = mode32Bytes;
	
	/* mode32Bytes==0 or 1 is backward-compatible */
	if ( mode32Bytes==0 ) burstSize = IPS_DMBS_16|IPS_SMBS_16;
	else if ( mode32Bytes==1 ) burstSize = IPS_DMBS_32|IPS_SMBS_32;
	else if ( mode32Bytes==2 ) burstSize = IPS_DMBS_64|IPS_SMBS_64;
	else if ( mode32Bytes==3 ) burstSize = IPS_DMBS_128|IPS_SMBS_128;
	else return FAILED;

	if ( descNum == 0 ) return FAILED;
	if ( descNum < (2+DATA_SEG) ) return FAILED; /* key+iv+dataSegs */
	
	/* Software Reset */
	WRITE_MEM32( IPSCSR, READ_MEM32(IPSCSR)|IPS_SRST );

	rtl8651b_cryptoEngine_alloc( descNum );
	
	idxCpuSrc = idxAsicSrc = 0;
	idxCpuDst = idxAsicDst = 0;

	/* We must delay a while after software reset. */
	WRITE_MEM32( IPSCTR, 0/*IPS_LBKM*/|IPS_SAWB|IPS_CKE|burstSize );
	WRITE_MEM32( IPSCSR, READ_MEM32(IPSCSR)|IPS_SDUEIP|IPS_SDLEIP|IPS_DDUEIP|IPS_DDOKIP ); /* write 1 to clear */

	return SUCCESS;
}


int32 rtl8651b_cryptoEngine_free( void ) 
{
	if ( ipssdar )
	{
		rtlglue_free( KSEG0_ADDRESS( ipssdar ) );
		ipssdar = NULL;
	}
	if ( ipsddar )
	{
		rtlglue_free( KSEG0_ADDRESS( ipsddar ) );
		ipsddar = NULL;
	}
	return SUCCESS;
}


int32 rtl8651b_cryptoEngine_exit( void ) 
{
	rtl8651b_cryptoEngine_free();
	
	return SUCCESS;
}


int32 rtl8651b_cryptoMemcpy(void *dest)
{
	return FAILED;
}

/* Generic DMA */
#define GDMA_BASE       (0xB800A000)            /* 0xB800A000 */
#define GDMACNR         (GDMA_BASE+0x00)        /* Generic DMA Control Register */
#define GDMAIMR         (GDMA_BASE+0x04)        /* Generic DMA Interrupt Mask Register */
#define GDMAISR         (GDMA_BASE+0x08)        /* Generic DMA Interrupt Status Register */
#define GDMAICVL        (GDMA_BASE+0x0C)        /* Generic DMA Initial Checksum Value (Left Part) Register */
#define GDMAICVR        (GDMA_BASE+0x10)        /* Generic DMA Initial Checksum Value (Right Part) Register */
#define GDMASBP0        (GDMA_BASE+0x20)        /* Generic DMA Source Block Pointer 0 Register */
#define GDMASBL0        (GDMA_BASE+0x24)        /* Generic DMA Source Block Length 0 Register */
#define GDMASBP1        (GDMA_BASE+0x28)        /* Generic DMA Source Block Pointer 1 Register */
#define GDMASBL1        (GDMA_BASE+0x2C)        /* Generic DMA Source Block Length 1 Register */

/* Generic DMA Control Register */
#define GDMA_ENABLE                     (1<<31)         /* Enable GDMA */
#define GDMA_POLL                       (1<<30)         /* Kick off GDMA */
#define GDMA_FUNCMASK           (0xf<<24)       /* GDMA Function Mask */
#define GDMA_MEMCPY                     (0x0<<24)       /* Memory Copy */
#define GDMA_CHKOFF                     (0x1<<24)       /* Checksum Offload */
#define GDMA_STCAM                      (0x2<<24)       /* Sequential T-CAM */
#define GDMA_MEMSET                     (0x3<<24)       /* Memory Set */
#define GDMA_B64ENC                     (0x4<<24)       /* Base 64 Encode */
#define GDMA_B64DEC                     (0x5<<24)       /* Base 64 Decode */
#define GDMA_QPENC                      (0x6<<24)       /* Quoted Printable Encode */
#define GDMA_QPDEC                      (0x7<<24)       /* Quoted Printable Decode */
#define GDMA_MIC                        (0x8<<24)       /* Wireless MIC */
#define GDMA_MEMXOR                     (0x9<<24)       /* Memory XOR */
#define GDMA_MEMCMP                     (0xa<<24)       /* Memory Compare */
#define GDMA_BYTESWAP           (0xb<<24)       /* Byte Swap */
#define GDMA_PATTERN            (0xc<<24)       /* Pattern Match */
#define GDMA_SWAPTYPE0          (0<<22)         /* Original:{0,1,2,3} => {1,0,3,2} */
#define GDMA_SWAPTYPE1          (1<<22)         /* Original:{0,1,2,3} => {3,2,1,0} */
#define GDMA_ENTSIZMASK         (3<<20)         /* T-CAM Entry Size Mask */
#define GDMA_ENTSIZ32           (0<<20)         /* T-CAM Entry Size 32 bits */
#define GDMA_ENTSIZ64           (1<<20)         /* T-CAM Entry Size 64 bits */
#define GDMA_ENTSIZ128          (2<<20)         /* T-CAM Entry Size 128 bits */
#define GDMA_ENTSIZ256          (3<<20)         /* T-CAM Entry Size 256 bits */

/* Generic DMA Interrupt Mask Register */
#define GDMA_COMPIE                     (1<<31)         /* Completed Interrupt Enable */
#define GDMA_NEEDCPUIE          (1<<28)         /* Need-CPU Interrupt Enable */

/* Generic DMA Interrupt Status Register */
#define GDMA_COMPIP                     (1<<31)         /* Completed Interrupt Status (write 1 to clear) */
#define GDMA_NEEDCPUIP          (1<<28)         /* Need-CPU Interrupt Status (write 1 to clear) */

/* Generic DMA Source Block Length n. Register */
#define GDMA_LDB                        (1<<31)         /* Last Data Block */
#define GDMA_BLKLENMASK         (0x1fff)        /* Block Length (valid value: from 1 to 8K-1 bytes) */
//#define internalUsedGDMACNR (0x000000C0)
#define internalUsedGDMACNR (0x00000000)



/*data, key and iv does not have 4-byte alignment limitatiuon
 *mode bits:
 *        0x00   0x20
 *        DES  / AES
 * 0x01:  3DES   none
 * 0x02:  ECB    ECB
 * 0x04:  Enc    Enc
 * 0x08:  nBlk   nBlk
 * 0x10:  DMA    DMA
 *
 * CBC_AES:0x20
 * ECB_AES:0x22
 * CTR_AES:0x23
*/
int32 rtl8651b_cryptoEngine_des(uint32 mode, int8 *data, uint32 len, int8 *key, int8 *iv ) 
{
	static rtl865xc_crypto_source_t prepSrc[2+DATA_SEG]; /* MS=00: KeyArray+IvArray+DataSegs */
	static rtl865xc_crypto_dest_t prepDst[1];
	uint32 prepNum = 0; /* the number of descriptor prepare to write ASIC */
	int i;
uint32* p_w;
int j;

//#define TEST_ARBITOR_LOCK
#undef TEST_ARBITOR_LOCK
#ifdef TEST_ARBITOR_LOCK
        struct TEST_VECTOR
        {
                uint32 icvl;
                uint32 icvr;
                char *source;
                uint32 len;
                uint32 outl;
                uint32 outr;
        }
        static testvec[] =
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
	static unsigned int lock_before_kick;
	static unsigned int gmda_mic_i = 0;
	volatile uint32 *pmr;
	volatile uint32 *ipscsr;
	volatile uint32 *gdmacnr;
	volatile uint32 p_i;
	uint32 value_pmr;
	uint32 value_ipscsr;
	uint32 value_gdmacnr;
	uint32 check;
	uint32 value_lxarb;
	uint32 *lxarb;


	pmr = (uint32 *)0xB8001040;
	ipscsr = (uint32 *)IPSCSR;
	gdmacnr = (uint32 *)GDMACNR;
	gmda_mic_i = ((gmda_mic_i + 1)%6);
	//memset(testvec[gmda_mic_i].source, 0xff, testvec[].len);
#ifdef CONFIG_RTL8316S
mips_cache_flush_all();
#else
_cache_flush_all();
#endif
	/*
	 * Set up GDMA register 
 	*/
	/* Reset GDMA */
	WRITE_MEM32( GDMACNR, 0 );
	WRITE_MEM32( GDMACNR, GDMA_ENABLE );

	WRITE_MEM32( GDMAIMR, 0 );
	WRITE_MEM32( GDMAISR, 0xffffffff );
	WRITE_MEM32( GDMAICVL, testvec[gmda_mic_i].icvl );
	WRITE_MEM32( GDMAICVR, testvec[gmda_mic_i].icvr );
	WRITE_MEM32( GDMASBP0, testvec[gmda_mic_i].source); /* SIP + DIP */
	WRITE_MEM32( GDMASBL0, GDMA_LDB|testvec[gmda_mic_i].len );
	WRITE_MEM32( GDMADBP0, 0 );
	WRITE_MEM32( GDMADBL0, 0 );

	check = 0;
	lock_before_kick=1;
	//LOCK LX BUS
	if(lock_before_kick==1)
	{
		WRITE_MEM32( 0xB8003400 , READ_MEM32(0xB8003400)|(1<<27));
		while( (READ_MEM32(0xB8003400)&(1<<26)) == 0);
		//printf("\n#1: arbitor locked \n");
#if 1	
		for(p_i=0;p_i<1024;p_i++)
		{
	                value_pmr = *pmr;
			value_ipscsr = *ipscsr;
			value_ipscsr = value_ipscsr | IPS_POLL;
			value_gdmacnr = GDMA_ENABLE|GDMA_POLL|GDMA_MIC|internalUsedGDMACNR;
			
			/* if it enter self-refresh mode, kick off GDMA and IPSEC */
			if((value_pmr & 0xC0000000) == 0x80000000)
			{
				*ipscsr = value_ipscsr;
				*gdmacnr = value_gdmacnr;
                        	check = 1;
                	}
			if(check==1)
				value_pmr = *pmr;
                	if(p_i == 1)
                        	*pmr = 0x8BFF1000;
		}
		if((value_pmr & 0xC0000000) != 0x80000000)
		{
			printf(" kick off GDMA/IPSEC weak up MEMCTL\n");
		}
#else
		/* kick off GDMA */
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MIC|internalUsedGDMACNR );
		/* kick off crypto engine */
		WRITE_MEM32( IPSCSR, READ_MEM32(IPSCSR)|IPS_POLL );
#endif
	}
	//memcpy(testvec[1].source, testvec[0].source, testvec[0].len);

#endif
	
	memset( prepSrc, 0, sizeof(prepSrc) );
	memset( prepDst, 0, sizeof(prepDst) );

	if ( mode & 0x20 )
	{ /* AES */
		if( len == 0 ||
		    len > 16368 ||
		    len & 0xf)
		{
			#if _CRYPTO_DEBUG_>=1
			rtlglue_printf("%s():%d\n", __FUNCTION__, __LINE__ );	
			#endif
			return FAILED;/*Unable to process non-16-byte aligned data*/
		}
		
		/* AES */
		prepSrc[0].own = 1; /* Own by crypto */
		prepSrc[0].ms = 0; /* Mode Select = AES/DES/3DES */
		prepSrc[0].aeskl = 1; /* AES Key Length = 128 bits */
		switch ( mode & 0x03 )
		{
			case 0x00: /* CBC mode */
				prepSrc[0].cbc = 1;
				break;
			case 0x02: /* ECB mode */
				prepSrc[0].cbc = 0;
				break;
			case 0x03: /* Counter mode */
				prepSrc[0].ctr = 1;
				break;
			default:
				#if _CRYPTO_DEBUG_>=1
				rtlglue_printf("%s():%d\n", __FUNCTION__, __LINE__ );	
				#endif
				return FAILED;
				break;
		}
		if ( mode & 0x04 )
		{
			prepSrc[0].kam = 7; /* Key Application Mechanism: Encryption */
		}
		else
		{
			prepSrc[0].kam = 0; /* Key Application Mechanism: Decryption */
		}

		/* first descriptor: Key Array */
		prepSrc[0].fs = 1;
		prepSrc[0].sbl = 128/8; /* Key Array Length */
		prepSrc[0].a2eo = 0;
		prepSrc[0].enl = len;
		prepSrc[0].apl = 0;
		prepSrc[0].sdbp = PHYSICAL_ADDRESS( key );
		
		/* second descriptor: IV Array */
		prepSrc[1].own = 1; /* Own by crypto */
		prepSrc[1].fs = 0;
		prepSrc[1].sbl = 128/8; /* IV Array Length */
		prepSrc[1].a2eo = 0;
		prepSrc[1].enl = len;
		prepSrc[1].apl = 0;
		prepSrc[1].sdbp = PHYSICAL_ADDRESS( iv );
		
		/* third descriptor and so on: Data */
		for( i = 0; i < DATA_SEG; i++ )
		{
			prepSrc[2+i].own = 1; /* Own by crypto */
			prepSrc[2+i].fs = 0;
			if ( i==(DATA_SEG-1) )
				prepSrc[2+i].sbl = len-(len/DATA_SEG)*(DATA_SEG-1); /* Data Length */
			else
				prepSrc[2+i].sbl = len/DATA_SEG; /* Data Length */
			prepSrc[2+i].a2eo = 0;
			prepSrc[2+i].enl = len;
			prepSrc[2+i].apl = 0;
			prepSrc[2+i].sdbp = PHYSICAL_ADDRESS( data+(len/DATA_SEG)*i );
		}

		prepNum = 2+DATA_SEG;
	}
	else
	{ /* DES/3DES */
		if( len == 0 ||
		    len > 16376 ||
		    len & 0x7)
		{
			#if _CRYPTO_DEBUG_>=1
			rtlglue_printf("%s():%d\n", __FUNCTION__, __LINE__ );	
			#endif
			return FAILED;/*Unable to process non-8-byte aligned data*/
		}

		/* DES/3DES */
		prepSrc[0].own = 1; /* Own by crypto */
		prepSrc[0].ms = 0; /* Mode Select = AES/DES/3DES */
		prepSrc[0].aeskl = 0; /* DES/3DES */
		if ( mode & 1 )
		{
			/* 3DES */
			prepSrc[0].trides = 1;
			prepSrc[0].sbl = 24; /* Key Array Length */
			if ( mode & 4 )
				prepSrc[0].kam = 5; /* Encryption */
			else
			{
				static uint8 _swapped[24];
				uint8* swapped = UNCACHED_ADDRESS( _swapped );
				uint8* pk = key; /* for compiler */

				prepSrc[0].kam = 2; /* Decryption */
				/* for descryption, we need to swap K1 and K3 (K1,K2,K3)==>(K3,K2,K1). */
				memcpy( &swapped[16], &pk[0], 8 );
				memcpy( &swapped[8], &pk[8], 8 );
				memcpy( &swapped[0], &pk[16], 8 );
				key = swapped; /* re-pointer to new key */
			}
		}
		else
		{
			/* DES */
			prepSrc[0].trides = 0;
			prepSrc[0].sbl = 8; /* Key Array Length */
			if ( mode & 4 )
				prepSrc[0].kam = 7; /* Encryption */
			else
				prepSrc[0].kam = 0; /* Decryption */
		}
		if ( mode & 2 )
			prepSrc[0].cbc = 0; /* ECB */
		else
			prepSrc[0].cbc = 1; /* CBC */
			
		/* first descriptor: Key Array */
		prepSrc[0].fs = 1;
		prepSrc[0].a2eo = 0;
		prepSrc[0].enl = len;
		prepSrc[0].apl = 0;
		prepSrc[0].sdbp = PHYSICAL_ADDRESS( key );
		
		/* second descriptor: IV Array */
		prepSrc[1].own = 1;
		prepSrc[1].fs = 0;
		prepSrc[1].sbl = 8; /* IV Array Length */
		prepSrc[1].a2eo = 0;
		prepSrc[1].enl = len;
		prepSrc[1].apl = 0;
		prepSrc[1].sdbp = PHYSICAL_ADDRESS( iv );
		
		/* third descriptor and so on: Data */
		for( i = 0; i < DATA_SEG; i++ )
		{
			prepSrc[2+i].own = 1;
			prepSrc[2+i].fs = 0;
			if ( i==(DATA_SEG-1) )
				prepSrc[2+i].sbl = len-(len/DATA_SEG)*(DATA_SEG-1); /* Data Length */
			else
				prepSrc[2+i].sbl = len/DATA_SEG; /* Data Length */
			prepSrc[2+i].a2eo = 0;
			prepSrc[2+i].enl = len;
			prepSrc[2+i].apl = 0;
			prepSrc[2+i].sdbp = PHYSICAL_ADDRESS( data+(len/DATA_SEG)*i );
		}

		prepNum = 2+DATA_SEG;
	}

	/* We assume the CPU and ASIC are pointed to the same descriptor.
	 * However, in async mode, this assumption is invalid.
	 */
	if ( mode & 0x08 )
	{
		/* non-blocking mode, we cannot expect where ASIC is pointing to. */
	}
	else
	{
		/* blocking mode */
		idxAsicSrc = ((rtl865xc_crypto_source_t*)KSEG1_ADDRESS(READ_MEM32( IPSSDAR )))-ipssdar;
		assert( idxAsicSrc == idxCpuSrc );
		idxAsicDst = ((rtl865xc_crypto_dest_t*)KSEG1_ADDRESS(READ_MEM32( IPSDDAR )))-ipsddar;
		assert( idxAsicDst == idxCpuDst );
	}

	/* prepare destination descriptor. */
	prepDst[0].own = 1; /* set owned by ASIC */
	prepDst[0].eor = (idxCpuDst==(numDst-1)); /* If this is the final descriptor, set EOR bit. */
	prepDst[0].dbl = len; /* destination data length */
	prepDst[0].ddbp = PHYSICAL_ADDRESS( data ); /* destination data block pointer */

	/* write prepared descriptors into ASIC */
	/* 1.destination first */
	memcpy( ((char*)&ipsddar[idxCpuDst])+4, ((char*)&prepDst[0])+4, sizeof(ipsddar[0])-4 ); /* skip first word. */
	*(uint32*)&ipsddar[idxCpuDst] = *(uint32*)&prepDst[0]; /* copy the first word. */
	idxCpuDst = (idxCpuDst+1)%numDst;
	
	/* 2.then source */
	for( i = 0; i < prepNum; i++ )
	{
		if ( ipssdar[(idxCpuSrc+i)%numSrc].own == 0 )
		{
			/* Owned by CPU, overwrite it ! */
			prepSrc[i].eor = (((idxCpuSrc+i)%numSrc)==(numSrc-1)); /* If this is the final descriptor, set EOR bit. */
			memcpy( ((char*)&ipssdar[(idxCpuSrc+i)%numSrc])+4, ((char*)&prepSrc[i])+4, sizeof(ipssdar[0])-4 ); /* skip first word. */
			*(uint32*)&ipssdar[(idxCpuSrc+i)%numSrc] = *(uint32*)&prepSrc[i]; /* copy the first word. */
		}
		else
		{
#if 0
			/* Owned by ASIC.
			 * Currently, we do not support this situation. 
			 * This means one of following:
			 *  1. numSrc < prepNum
			 *  2. async operation is fired
			 */
			#if _CRYPTO_DEBUG_>=1
			rtlglue_printf("%s():%d\n", __FUNCTION__, __LINE__ );	
			#endif
			return FAILED;
#endif
		}
	}
	idxCpuSrc = (idxCpuSrc+prepNum)%numSrc;
#if 0
	#if _CRYPTO_DEBUG_>=2
	rtlglue_printf( "numSrc=%d prepNum=%d idxCpuSrc=%d(%d) idxCpuDst=%d(%d)\n", numSrc, prepNum, idxCpuSrc, idxAsicSrc, idxCpuDst, idxAsicDst );
	#endif
	memDump( (void*)IPSSDAR, 0x10, "Crypto Engine Registers" );
	for( i = 0; i < prepNum; i++ ) memDump( &ipssdar[(idxCpuSrc+numSrc-prepNum+i)%numSrc], sizeof(prepSrc[i]), "ipssdar" );
	memDump( &ipsddar[(idxCpuDst+numDst-1)%numDst], sizeof(prepDst), "ipsddar" );
#endif

#ifdef CONFIG_RTL865X_MODEL_KERNEL
	model_setTestTarget( IC_TYPE_REAL );
	modelExportSetIcType( IC_TYPE_REAL );
	modelExportSetOutputForm( EXPORT_ICEMON );
	modelExportCryptoRegisters();
#endif

#ifdef CONFIG_RTL8316S
mips_cache_flush_all();
#else
_cache_flush_all();
#endif
	/* Clear OK flag */
	WRITE_MEM32( IPSCSR, READ_MEM32( IPSCSR ) | (IPS_SDUEIP|IPS_SDLEIP|IPS_DDUEIP|IPS_DDOKIP|IPS_DABFIP) );
for(i=0;i<1000;i++);

#ifdef TEST_ARBITOR_LOCK
	//LOCK LX BUS
	if(lock_before_kick==0)
        {
		/* kick off crypto engine */
		WRITE_MEM32( IPSCSR, READ_MEM32(IPSCSR)|IPS_POLL );
		/* kick off GDMA */
		WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_MIC|internalUsedGDMACNR );
		/* delay random number of loops */

		WRITE_MEM32( 0xB8003400 , READ_MEM32(0xB8003400)|(1<<27));
		while( (READ_MEM32(0xB8003400)&(1<<26)) == 0);
		//printf("\n#0 :arbitor locked \n");
	}



	if(lock_before_kick==1)
		lock_before_kick = 0;
	else
		lock_before_kick=1;
#else

//        flash_sect_erase(0xbd000000, 0xbd001fff);
	/* check dram data and flash data */
//	for(i=0;i<0x1000; i = i + 4)
//	{
//		if (*((uint32*)((0xbd000000)+i))!=0xFFFFFFFF)
//		{
  //     		         printf("addr(0x%x): 0x%x != 0xFFFFFFFF:  %s, %d\n",\   
//				((0xbd000000)+i), *((uint32*)((0xbd000000)+i)), __FUNCTION__, __LINE__);
//		}
//	}
	/* kick off crypto engine */
	WRITE_MEM32( IPSCSR, READ_MEM32(IPSCSR)|IPS_POLL );
//        flash_write((0x80000000), 0xbd000000, 0x1000);
#endif
	if ( mode & 0x08 )
	{
		/* non-blocking mode */
		return SUCCESS;
	}
	else
	{

#ifdef TEST_ARBITOR_LOCK
		i = 0;
		/* Test lock */
		while(i<100000)
		{
			if(((READ_MEM32(IPSCSR) & (IPS_SDUEIP|IPS_SDLEIP|IPS_DDUEIP|IPS_DDOKIP|IPS_DABFIP)) == 0)\
   				&& (( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0))
			{
				i++;
			}
			else
			{
				/* error: lock failed */
				break;
			}
		}
		if(i<100000) /* lx arbitor lock fail*/
		{
			printf("error: Lx arbitor lock error, lbk = %d\n", lock_before_kick);
		}
		/* unlock it. */
		lxarb = 0xB8003400;
		value_lxarb = (*lxarb)&(0xF7FFFFFF);
#if 1
		for(p_i=0;p_i<1024;p_i++)
		{
	                value_pmr = *pmr;
			/* if it enter self-refresh mode, unlock GDMA and IPSEC */
			if((value_pmr & 0xC0000000) == 0x80000000)
			{
				*lxarb = value_lxarb;
                	}
                	if(p_i == 1)
                        	*pmr = 0x8BFF1000;
		}
#else
		//printf("#test lock ok ");
		WRITE_MEM32( 0xB8003400 , READ_MEM32(0xB8003400)&(0xF7FFFFFF));
		while( (READ_MEM32(0xB8003400)&(1<<26)) == 1);
		//printf(" arbitor unlocked\n");
#endif
		/* Polling GDMA */
		while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */
		if(READ_MEM32(GDMAICVL) != testvec[gmda_mic_i].outl)
		{
			printf("Checksum not equal!\n");
		}
		if(READ_MEM32(GDMAICVR) != testvec[gmda_mic_i].outr)
		{
			printf("Checksum not equal!\n");
		}
#endif
#if 1
		/* blocking mode */
		while( ( READ_MEM32( IPSCSR ) & (IPS_SDUEIP|IPS_SDLEIP|IPS_DDUEIP|IPS_DDOKIP|IPS_DABFIP) ) == 0 ); /* wait until crypto engine stop */
		assert( ipsddar[(idxCpuDst+numDst-1)%numDst].own==0 ); 

		if ( READ_MEM32( IPSCSR ) & (IPS_SDLEIP|IPS_DABFIP) )
		{
			#if _CRYPTO_DEBUG_>=1
			rtlglue_printf("%s():%d READ_MEM32(IPSCSR)=0x%08x.\n", __FUNCTION__, __LINE__, READ_MEM32( IPSCSR ) );
			#endif
			return FAILED; /* error occurs */
		}

		/* check dram data and flash data */
//		for(i=0;i<0x1000; i = i + 4)
//		{
//			if(*(uint32*)((0x80000000)+i) != *((uint32*)((0xbd000000)+i)))
//			{
  //      		         printf("addr(0x%x): 0x%x != flash(0x%x):  %s, %d\n",\
    //            		      (0x80000000)+i , *(uint32*)((0x80000000)+i), \
//					((0xbd000000)+i), *((uint32*)((0xbd000000)+i)), __FUNCTION__, __LINE__);
//			}
//		}
#endif
	}

	return SUCCESS;
}


int32 rtl8651b_cryptoEngine_des_poll(int32 freq)
{
	return SUCCESS;
}


void rtl8651b_cryptoEngineGetIntCounter(uint32 * doneCounter, uint32 * allDoneCounter) {
	*doneCounter = cryptoDoneIntCounter;
	*allDoneCounter = cryptoAllDoneIntCounter;
}

void rtl8651b_cryptoEngine_initdata(void)
{
	ipssdar = NULL;
	ipsddar = NULL;
	numSrc = 0;
	numDst = 0;
	idxCpuSrc = 0;
	idxAsicSrc = 0;
	idxCpuDst = 0;
	idxAsicDst = 0;
	_descNum = 0;
	 _mode32Bytes = 0;
}

