/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Base64 Driver
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: base64.c,v 1.3 2006-07-13 16:03:07 chenyl Exp $
*/

#include "rtl_types.h"
#include "gdma_glue.h"
#include "asicregs.h"
#include "assert.h"
#include "gdma.h"


#define DEST_NUM 8

/*
func int32		| arrangeDataRegs	| Arrange Descriptor Registers
parm descriptor_t* | src | pointer to return arranged source descriptor registers
parm descriptor_t* | dst | pointer to return arranged destination descriptor registers
parm uint8* | in | pointer to input data, said un-encoded and binary data.
parm int32 | pProcessed | pointer to store length of input data, and processed length after encoding.
parm uint8* | out | pointer to output buffer (to be stored encoded string)
parm int32 | pOutputted | pointer to store length of output buffer, and outputted length after encoding.
comm
IC have some criteria on destination data block registers.
1. first destination data block is byte-aligned, and length is in byte unit.
2. second destination data block, and third , fourth, fifth, sixth, seventh, and eighth 
*/
void arrangeDataRegs( descriptor_t *src, descriptor_t *dst, uint8 *in, int32 *pProcessed, uint8 *out, int32 *pOutputted )
{
	int i;
	
#if 0 /* for debug */
	rtlglue_printf( "%s():%d processed=%d outputted=%d\n", __FUNCTION__, __LINE__, *pProcessed, *pOutputted );
#endif

	memset( src, 0, sizeof(descriptor_t)*DEST_NUM );
	memset( dst, 0, sizeof(descriptor_t)*DEST_NUM );

	/* make source descriptor */
	for( i = 0; i < DEST_NUM; i++ )
	{
		src[i].pData = &in[i*MAX_BLOCK_LENGTH];
		if ( *pProcessed > MAX_BLOCK_LENGTH )
		{
			src[i].length = MAX_BLOCK_LENGTH;
		}
		else
		{
			src[i].ldb = 1;
			src[i].length = *pProcessed;
			break;
		}

		*pProcessed -= MAX_BLOCK_LENGTH;
	}
	if ( i == DEST_NUM ) src[DEST_NUM-1].ldb = 1; /* Set ldb bit of 8th block if we used 8 blocks. */
#if 0 /* for debug */
	for( i = 0; i < DEST_NUM; i++ )
		rtlglue_printf( "src[%d].pData=%p .ldb=%d .length=%04x\n", i, src[i].pData, src[i].ldb, src[i].length );
#endif

	/* make destination descriptor
	 *
	 * According to IC's criticia, the legnth and address of second destination block, and followed, are limited to 4-byte aligned and 4-byte time long.
	 * Therefore, we need to carefully arrange the first destination block length to meet the criticia.
	 *
	 *      |*out            The length of first block is decided by alignment of *out.
	 *      v
	 *   +-+-+-+-+-+-+-+-+     align   length
	 *   |0|1|2|3|0|1|2|3|       0     0x1FFC
	 *   +-+-+-+-+-+-+-+-+       1     0x1FFF
	 *     |<--->|<--- ...       2     0x1FFE
	 *      first  second        3     0x1FFD
	 *
	 */
	dst[0].pData = &out[0];
	if ( *pOutputted <= MAX_BLOCK_LENGTH )
	{
		dst[0].ldb = 1;
		dst[0].length = *pOutputted;
		goto dst_made;
	}
	else
		dst[0].length = (MAX_BLOCK_LENGTH+1)-((((uint32)out-1)&0x3)+1);
	*pOutputted -= dst[0].length;

	for( i = 1; i < DEST_NUM; i++ )
	{
		dst[i].pData = &dst[i-1].pData[dst[i-1].length];
		assert( (((uint32)dst[i].pData)&0x3)==0x0 ); /* assume 4-byte align */
		
		if ( *pOutputted > (MAX_BLOCK_LENGTH&~0x3) )
		{
			dst[i].length = (MAX_BLOCK_LENGTH&~0x3);
		}
		else
		{
			dst[i].ldb = 1;
			dst[i].length = (*pOutputted)&~0x3;

#if 1 /* length must be larger than or equal to 4 bytes */
			if ( dst[i].length < 4 )
			{
				uint32 moveback = 4 - dst[i].length;
				dst[i-1].length -= moveback;
				dst[i].length += moveback;
			}
#endif
			/* Assert for IC criticia */
			assert( dst[i].length <= (MAX_BLOCK_LENGTH&~0x3) );
			assert( dst[i].length >= 4 );
			assert( ( dst[i].length & 0x3 ) == 0 );
			
			break;
		}

		*pOutputted -= (MAX_BLOCK_LENGTH&~0x3);
	}
	if ( i == DEST_NUM ) dst[DEST_NUM-1].ldb = 1; /* Set ldb bit of 8th block if we used 8 blocks. */
dst_made: do {} while(0);
#if 0 /* for debug */
	for( i = 0; i < DEST_NUM; i++ )
		rtlglue_printf( "dst[%d].pData=%p .ldb=%d .length=%04x\n", i, dst[i].pData, dst[i].ldb, dst[i].length );
#endif
}


/*
func int32 | rtl865x_b64enc	| Encode with base64 algorithm
parm uint8* | in | pointer to input data, said un-encoded and binary data.
parm int32  | inLen | length of input data
parm uint8* | out | pointer to output buffer (to be stored encoded string)
parm int32  | outLen | length of output data
parm int32* | pProcessed | pointer to store length of input data, and processed length after encoding.
parm int32* | pOutputted | pointer to store length of output buffer, and outputted length after encoding.
comm 
*/
int32 rtl865x_b64enc( uint8 *in, int32 inLen, uint8 *out, int32 outLen, int32 *pProcessed, int32 *pOutputted )
{
	static descriptor_t src[DEST_NUM], dst[DEST_NUM];
	int i;

	if ( pProcessed==NULL || pOutputted==NULL ) return FAILED;

	*pProcessed = inLen;
	*pOutputted = outLen;
	arrangeDataRegs( src, dst, in, pProcessed, out, pOutputted );

	/* Reset GDMA */
	WRITE_MEM32( GDMACNR, 0 );
	WRITE_MEM32( GDMACNR, GDMA_ENABLE );
	WRITE_MEM32( GDMAIMR, 0 );
	WRITE_MEM32( GDMAISR, 0xffffffff );
	WRITE_MEM32( GDMAICVL, 76/4-1 ); /* Line Length */
	WRITE_MEM32( GDMAICVR, 0 );
	for( i = 0; i < DEST_NUM; i++ )
	{
		WRITE_MEM32( GDMASBP0+i*8+0, ((uint32*)&src[i])[0] );
		WRITE_MEM32( GDMASBP0+i*8+4, ((uint32*)&src[i])[1] );
	}
	for( i = 0; i < DEST_NUM; i++ )
	{
		WRITE_MEM32( GDMADBP0+i*8+0, ((uint32*)&dst[i])[0] );
		WRITE_MEM32( GDMADBP0+i*8+4, ((uint32*)&dst[i])[1] );
	}
	WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_B64ENC|internalUsedGDMACNR );

	while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

	*pProcessed = READ_MEM32( GDMAICVL ) >> 16;
	*pOutputted = READ_MEM32( GDMAICVL ) & 0xFFFF;
	
	return SUCCESS;
}


/*
func int32		| rtl865x_b64dec	| Decode with base64 algorithm
parm uint8* | in | pointer to input data, said encoded.
parm int32  | inLen | length of input data
parm uint8* | out | pointer to output buffer (to be stored decoded string, said binary data)
parm int32  | outLen | length of output data
parm int32* | pProcessed | pointer to store length of input data, and processed length after encoding.
parm int32* | pOutputted | pointer to store length of output buffer, and outputted length after encoding.
comm 
*/
int32 rtl865x_b64dec( uint8 *in, int32 inLen, uint8 *out, int32 outLen, int32 *pProcessed, int32 *pOutputted )
{
	static descriptor_t src[DEST_NUM], dst[DEST_NUM];
	int i;

	if ( pProcessed==NULL || pOutputted==NULL ) return FAILED;

	*pProcessed = inLen;
	*pOutputted = outLen;
	arrangeDataRegs( src, dst, in, pProcessed, out, pOutputted );

	/* Reset GDMA */
	WRITE_MEM32( GDMACNR, 0 );
	WRITE_MEM32( GDMACNR, GDMA_ENABLE );
	WRITE_MEM32( GDMAIMR, 0 );
	WRITE_MEM32( GDMAISR, 0xffffffff );
	WRITE_MEM32( GDMAICVL, 76/4-1 ); /* Line Length */
	WRITE_MEM32( GDMAICVR, 0 );
	for( i = 0; i < DEST_NUM; i++ )
	{
		WRITE_MEM32( GDMASBP0+i*8+0, ((uint32*)&src[i])[0] );
		WRITE_MEM32( GDMASBP0+i*8+4, ((uint32*)&src[i])[1] );
	}
	for( i = 0; i < DEST_NUM; i++ )
	{
		WRITE_MEM32( GDMADBP0+i*8+0, ((uint32*)&dst[i])[0] );
		WRITE_MEM32( GDMADBP0+i*8+4, ((uint32*)&dst[i])[1] );
	}
	WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_B64DEC|internalUsedGDMACNR );

	while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

	*pProcessed = READ_MEM32( GDMAICVL ) >> 16;
	*pOutputted = READ_MEM32( GDMAICVL ) & 0xFFFF;
	
	return SUCCESS;
}

