/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Quoted-Printable Driver
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: qp.c,v 1.3 2006-07-13 16:03:07 chenyl Exp $
*/

#include "rtl_types.h"
#include "gdma_glue.h"
#include "asicregs.h"
#include "assert.h"
#include "gdma.h"


#define DEST_NUM 8

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
int32 rtl865x_qpenc( uint8 *in, int32 inLen, uint8 *out, int32 outLen, int32 *pProcessed, int32 *pOutputted )
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
	WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_QPENC|internalUsedGDMACNR );

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
int32 rtl865x_qpdec( uint8 *in, int32 inLen, uint8 *out, int32 outLen, int32 *pProcessed, int32 *pOutputted )
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
	WRITE_MEM32( GDMACNR, GDMA_ENABLE|GDMA_POLL|GDMA_QPDEC|internalUsedGDMACNR );

	while( ( READ_MEM32( GDMAISR ) & GDMA_COMPIP ) == 0 ); /* wait for complete. */

	*pProcessed = READ_MEM32( GDMAICVL ) >> 16;
	*pOutputted = READ_MEM32( GDMAICVL ) & 0xFFFF;
	
	return SUCCESS;
}

