/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003  
* All rights reserved.
* 
* Program : 8651B authentication engine driver code
* Abstract : 
* $Id: rtl865xc_authEngine.c,v 1.6 2007/04/04 06:11:04 bo_zhao Exp $
* --------------------------------------------------------------------
*/

#include "rtl865xc_authEngine.h"
//#include <linux/autoconf.h>
#include "asicRegs.h"
//#include <rtl_utils.h>
//#ifdef CONFIG_RTL865X_MODEL_KERNEL
//#include <icExport.h>
//#endif
#include "md5.h"
#include "sha1.h"
#include "hmac.h"
#include "assert.h"
#include "util.h"

#define _AUTH_DEBUG_ 1


/*  Data Segment for IC Verification
 *    In the normal mode (also compatible mode), 'DATA_SEG' is set to 1.
 *    When we want to test the multiple data segment mode, we shall set 'DATA_SEG' more than 1.
 */
#define DATA_SEG 1


static uint8 PAD[RTL8651B_MAX_MD_CBLOCK*2]; /* buffer for rtl8651b_authEngineInputPad and rtl8651b_authEngineOutputPad */
static uint8 *rtl8651b_authEngineInputPad, *rtl8651b_authEngineOutputPad;
static uint8 _rtl8651b_authEngineMd5TempKey[RTL8651B_MD5_DIGEST_LENGTH], _rtl8651b_authEngineSha1TempKey[RTL8651B_SHA1_DIGEST_LENGTH]; /* buffer for *rtl8651b_authEngineMd5TempKey, *rtl8651b_authEngineSha1TempKey */
static uint8 *rtl8651b_authEngineMd5TempKey, *rtl8651b_authEngineSha1TempKey;
static uint32 authDoneIntCounter, authAllDoneIntCounter;

/* Louis note: if you want to use __DRAM, remember to initial this variable in init(). */
//__DRAM_CRYPTO static uint8 rtl8651b_authEngine_paddat[RTL8651B_MAX_MD_CBLOCK];
static uint8 rtl8651b_authEngine_paddat[RTL8651B_MAX_MD_CBLOCK];

int32 rtl8651b_authEngine_alloc(uint32 descNum)
{
	/* Auth Engine is sharing descriptor ring with Crypto Engine. */
	rtl8651b_cryptoEngine_alloc( descNum );
	
	return SUCCESS;
}


static uint32 _descNum;
static int8 _mode32Bytes;
int32 rtl8651b_authEngine_init(uint32 descNum, int8 mode32Bytes)
{
	int i;

	if ( rtl8651b_cryptoEngine_init( descNum, mode32Bytes ) != SUCCESS )
		return FAILED;
	_descNum = descNum;
	_mode32Bytes = mode32Bytes;
	
	/*
	 * Louis note:
	 * Once you enable __DRAM for rtl8651b_authEngine_paddat[],
	 *   you MUST re-initialize rtl8651b_authEngine_paddat[] again.
	 */
	rtl8651b_authEngine_paddat[0] = 0x80;
	for( i = 1; i < RTL8651B_MAX_MD_CBLOCK; i++ )
	{
		rtl8651b_authEngine_paddat[i] = 0x0;
	}

	rtl8651b_authEngine_alloc( descNum );

	/* Since IPAD and OPAD must be concatenated togother, therefore, we must define a buffer of 128 bytes. */
	rtl8651b_authEngineInputPad = UNCACHED_ADDRESS(&PAD[0]);
	rtl8651b_authEngineOutputPad = UNCACHED_ADDRESS(&PAD[64]);
	/* set uncached address of temp key */
	rtl8651b_authEngineMd5TempKey = UNCACHED_ADDRESS(&_rtl8651b_authEngineMd5TempKey[0]);
	rtl8651b_authEngineSha1TempKey = UNCACHED_ADDRESS(&_rtl8651b_authEngineSha1TempKey[0]);

	return SUCCESS;
}


int32 rtl8651b_authEngine_free( void )
{
	/* Auth Engine is sharing descriptor ring with Crypto Engine. */
	rtl8651b_cryptoEngine_free();
	
	return SUCCESS;
}


int32 rtl8651b_authEngine_exit( void )
{
	rtl8651b_authEngine_free();
	
	return SUCCESS;
}
#if 0
typedef uint32          memaddr;
void memDump (void *start, uint32 size, int8 * strHeader)
{
        int32 row, column, index, index2, max;
        uint32 buffer[5];
        uint8 *buf, *line, ascii[17];
        int8 empty = ' ';

        if(!start ||(size==0))
                return;
        line = (uint8*)start;

        /*
 *         16 bytes per line
 *                 */
        if (strHeader)
                rtlglue_printf ("%s", strHeader);
        column = size % 16;
        row = (size / 16) + 1;
        for (index = 0; index < row; index++, line += 16)
        {

                /* for un-alignment access */
                buffer[0] = ntohl( READ_MEM32( (((uint32)line)&~3)+ 0 ) );
                buffer[1] = ntohl( READ_MEM32( (((uint32)line)&~3)+ 4 ) );
                buffer[2] = ntohl( READ_MEM32( (((uint32)line)&~3)+ 8 ) );
                buffer[3] = ntohl( READ_MEM32( (((uint32)line)&~3)+12 ) );
                buffer[4] = ntohl( READ_MEM32( (((uint32)line)&~3)+16 ) );
                buf = ((uint8*)buffer) + (((uint32)line)&3);

                memset (ascii, 0, 17);

                max = (index == row - 1) ? column : 16;
                if ( max==0 ) break; /* If we need not dump this line, break it. */

                rtlglue_printf ("\n%08x ", (memaddr) line);

                for (index2 = 0; index2 < max; index2++)
                {
                        if (index2 == 8)
                        rtlglue_printf ("  ");
                        rtlglue_printf ("%02x ", (uint8) buf[index2]);
                        ascii[index2] = ((uint8) buf[index2] < 32) ? empty : buf[index2];
                }

                if (max != 16)
                {
                        if (max < 8)
                                rtlglue_printf ("  ");
                        for (index2 = 16 - max; index2 > 0; index2--)
                                rtlglue_printf ("   ");
                }

                rtlglue_printf ("  %s", ascii);
        }
        rtlglue_printf ("\n");
        return;
}
#endif

/*
@func int32	| rtl8651b_authEngine	| Provide authentication function
@parm uint32	| HASH_MD5/HASH_SHA1/HMAC_MD5/HMAC_SHA1, and bit-wised IOPAD_READY when ipad/opad are pre-computed.
@parm uint8* 	| data	| data to be authenticated.
@parm uint32	| dataLen	| data length
@parm uint8* 	| key		| key (when IOPAD_READY is set, key is pointed to ipad and opad)
@parm uint32	| keyLen	| key length
@parm uint8 * 	| digest	| output digest data
@rvalue SUCCESS	| A pair of <p extIp> and <p extPort> selected. 
@rvalue FAILED 	| Failed to select either <p extIp> or <p extPort>.
@comm 
@devnote
 *mode bits:
 *          0x00     0x01
 *   0x00 HASH_MD5 HASH_SHA1
 *   0x02 HMAC_MD5 HMAC_SHA1
 */
/*void * memmove(void * dest,const void *src,size_t count)
{
        char *tmp, *s;

        if (dest <= src) {
                tmp = (char *) dest;
                s = (char *) src;
                while (count--)
                        *tmp++ = *s++;
                }
        else {
                tmp = (char *) dest + count;
                s = (char *) src + count;
                while (count--)
                        *--tmp = *--s;
                }

        return dest;
}
*/
int32 rtl8651b_authEngine(uint32 mode, uint8 * data, uint32 dataLen, uint8 * key, uint32 keyLen, uint8 * digest) 
{
	MD5_CTX md5Context;
	SHA1_CTX sha1Context;
	uint32 i;
	uint32 dmaLen; /* Should be 8-byte aligned */
	uint64 len64;
	uint8 *uint8Ptr = (uint8 *)&len64;
	uint8 *ipad, *opad;
	static rtl865xc_crypto_source_t prepSrc[1+DATA_SEG]; /* MS=01: PAD+DataSegs */
	static rtl865xc_crypto_dest_t prepDst[1];
	uint32 prepNum = 0; /* the number of descriptor prepare to write ASIC */

	assert(data && digest);/*Either Hashing or key must exist*/
	memset( prepSrc, 0, sizeof(prepSrc) );
	memset( prepDst, 0, sizeof(prepDst) );

	/*Calculating ipad and opad*/
	if(mode&0x2) 
	{	/*HMAC*/
		assert(key);
		if(keyLen > HMAC_MAX_MD_CBLOCK) 
		{
			if(mode&0x1) 
			{
				SHA1Init(&sha1Context);
				SHA1Update(&sha1Context, key, keyLen);
				SHA1Final(rtl8651b_authEngineSha1TempKey, &sha1Context);
				key = rtl8651b_authEngineSha1TempKey;
				keyLen = SHA_DIGEST_LENGTH;
			}
			else 
			{
				MD5Init(&md5Context);
				MD5Update(&md5Context, key, keyLen);
				MD5Final(rtl8651b_authEngineMd5TempKey, &md5Context);
				key = rtl8651b_authEngineMd5TempKey;
				keyLen = MD5_DIGEST_LENGTH;
			}
		}
		
		if ( mode & RTL8651_AUTH_IOPAD_READY )
		{
			/* ipad and opad is pre-computed. */
			ipad = key;
			opad = key + RTL8651B_MAX_MD_CBLOCK;
		}
		else
		{
			memset(rtl8651b_authEngineInputPad, 0x36, RTL8651B_MAX_MD_CBLOCK);
			memset(rtl8651b_authEngineOutputPad, 0x5c, RTL8651B_MAX_MD_CBLOCK);
			for(i=0; i<keyLen; i++) 
			{
				rtl8651b_authEngineInputPad[i] ^= key[i];
				rtl8651b_authEngineOutputPad[i] ^= key[i];
			}
			ipad = rtl8651b_authEngineInputPad;
			opad = rtl8651b_authEngineOutputPad;
		}

		prepNum = 1+DATA_SEG;
	}
	else
	{	/* HashOnly */
		ipad = NULL;
		opad = NULL;
		prepNum = 1+DATA_SEG;
	}

	/*Do padding for MD5 and SHA-1*/
	i = RTL8651B_MAX_MD_CBLOCK - (dataLen&0x3F);
	if(i>8) 
	{
		memcpy((void *)(data + dataLen), (void *)rtl8651b_authEngine_paddat, i-8);
		dmaLen = (dataLen+64)&0xFFFFFFC0;
	}
	else 
	{
		memcpy((void *)(data + dataLen), (void *)rtl8651b_authEngine_paddat, i+56);
		dmaLen = (dataLen+72)&0xFFFFFFC0;
	}

	if(mode&0x2) 
	{	/*HMAC*/
		if(dmaLen >= 0x3FC0) /* 2**14, MAX_PKTLEN */
		{
			#if _AUTH_DEBUG_ >= 1
			rtlglue_printf("%s():%d dmaLen=%d too long.\n", __FUNCTION__, __LINE__, dmaLen );
			#endif
			return FAILED;/*DMA length larger than 8651b can support*/
		}
	}
	else
	{	/* HashOnly */
		if(dmaLen > 0x3FFF) /* 2**14, MAX_PKTLEN */
		{
			#if _AUTH_DEBUG_ >= 1
			rtlglue_printf("%s():%d dmaLen=%d too long.\n", __FUNCTION__, __LINE__, dmaLen );
			#endif
			return FAILED;/*DMA length larger than 8651b can support*/
		}
	}
	
	if(mode&0x2) /*HMAC*/
		len64 = (dataLen+64)<<3;/*First padding length is*/
	else
		len64 = dataLen<<3;/*First padding length is*/
	for(i=0; i<8; i++) 
	{
		if(mode&0x1) /*SHA-1*/
			data[dmaLen-8+i] = uint8Ptr[i];
		else
			data[dmaLen-i-1] = uint8Ptr[i];
	}

	prepSrc[0].own = 1; /* Own by crypto */
	prepSrc[0].ms = 1; /* Mode Select = SHA1 or MD5 */
	if ( ( mode & 0x02 ) == 0 )
		prepSrc[0].hmac = 0; /* Hash only */
	else
		prepSrc[0].hmac = 1; /* HMAC */

	if ( ( mode & 0x01 ) == 0 )
		prepSrc[0].md5 = 1; /* MD5 */
	else
		prepSrc[0].md5 = 0; /* SHA1 */
	
	/* first descriptor: PAD Array */
	prepSrc[0].fs = 1;
	prepSrc[0].sbl = 128; /* PAD size */
	prepSrc[0].a2eo = 0;
	prepSrc[0].enl = dataLen;
	prepSrc[0].apl = dmaLen-dataLen;
	prepSrc[0].sdbp = PHYSICAL_ADDRESS( ipad );
	/* second descriptor and so on: Data */
	for( i = 0; i < DATA_SEG; i++ )
	{
		prepSrc[1+i].own = 1; /* Own by crypto */
		prepSrc[1+i].fs = 0;
		if ( i==(DATA_SEG-1) )
			prepSrc[1+i].sbl = dmaLen-(dmaLen/DATA_SEG)*(DATA_SEG-1); /* Data Length */
		else
			prepSrc[1+i].sbl = dmaLen/DATA_SEG; /* Data Length */
		prepSrc[1+i].a2eo = 0;
		prepSrc[1+i].enl = dataLen;
		prepSrc[1+i].apl = dmaLen-dataLen;
		prepSrc[1+i].sdbp = PHYSICAL_ADDRESS( data+(dmaLen/DATA_SEG)*i );
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
	prepDst[0].dbl = 0; /* destination data length */
	prepDst[0].ddbp = 0; /* destination data block pointer */

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
			/* Owned by ASIC.
			 * Currently, we do not support this situation. 
			 * This means one of following:
			 *  1. numSrc < prepNum
			 *  2. async operation is fired
			 */
			#if _AUTH_DEBUG_ >= 1
			rtlglue_printf("%s():%d Owned by ASIC, not supported yet.\n", __FUNCTION__, __LINE__ );
			#endif
			return FAILED;
		}
	}
	idxCpuSrc = (idxCpuSrc+prepNum)%numSrc;

	#if _AUTH_DEBUG_ >= 3
	rtlglue_printf( "numSrc=%d prepNum=%d idxCpuSrc=%d(%d) idxCpuDst=%d(%d)\n", numSrc, prepNum, idxCpuSrc, idxAsicSrc, idxCpuDst, idxAsicDst );
	memDump( (void*)IPSSDAR, 0x10, "Auth Engine Registers" );
	for( i = 0; i < prepNum; i++ ) memDump( &ipssdar[(idxCpuSrc+numSrc-prepNum+i)%numSrc], sizeof(prepSrc[i]), "ipssdar" );
	memDump( &ipsddar[(idxCpuDst+numDst-1)%numDst], sizeof(prepDst), "ipsddar" );
	#endif
#ifdef CONFIG_RTL865X_MODEL_KERNEL
	model_setTestTarget( IC_TYPE_REAL );
	modelExportSetIcType( IC_TYPE_REAL );
	modelExportSetOutputForm( EXPORT_ICEMON );
	modelExportCryptoRegisters();
#endif

	/* Clear OK flag */
	WRITE_MEM32( IPSCSR, READ_MEM32( IPSCSR ) | (IPS_SDUEIP|IPS_SDLEIP|IPS_DDUEIP|IPS_DDOKIP|IPS_DABFIP) );

	/* kick off crypto engine */
	WRITE_MEM32( IPSCSR, READ_MEM32(IPSCSR)|IPS_POLL );

	if ( mode & 0x08 )
	{
		/* non-blocking mode */
		return SUCCESS;
	}
	else
	{
		/* blocking mode */
		while( ( READ_MEM32( IPSCSR ) & (IPS_SDUEIP|IPS_SDLEIP|IPS_DDUEIP|IPS_DDOKIP|IPS_DABFIP) ) == 0 ); /* wait until crypto engine stop */
		assert( ipsddar[(idxCpuDst+numDst-1)%numDst].own==0 ); 

		if ( READ_MEM32( IPSCSR ) & (IPS_SDLEIP|IPS_DABFIP) )
		{
			#if _AUTH_DEBUG_ >= 1
			rtlglue_printf("%s():%d READ_MEM32(IPSCSR)=0x%08x.\n", __FUNCTION__, __LINE__, READ_MEM32( IPSCSR ) );
			#endif
			return FAILED; /* error occurs */
		}
	}


	#if _AUTH_DEBUG_ >= 3
	memDump( ipsddar[(idxCpuDst+numDst-1)%numDst].icv, 16, "Digest: ipsddar[(idxCpuDst+numDst-1)%numDst].icv" );
	#endif
	if(mode&0x1)
		memcpy(digest, ipsddar[(idxCpuDst+numDst-1)%numDst].icv, RTL8651B_SHA1_DIGEST_LENGTH);//Avoid 4-byte alignment limitation
	else
		memcpy(digest, ipsddar[(idxCpuDst+numDst-1)%numDst].icv, RTL8651B_MD5_DIGEST_LENGTH);//Avoid 4-byte alignment limitation
	
	return SUCCESS;
}


void rtl8651b_authEngineGetIntCounter(uint32 * doneCounter, uint32 * allDoneCounter) {
	*doneCounter = authDoneIntCounter;
	*allDoneCounter = authAllDoneIntCounter;
}

