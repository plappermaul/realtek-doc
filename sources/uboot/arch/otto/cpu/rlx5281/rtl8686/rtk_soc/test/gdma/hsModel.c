/*
*-------------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
* 
* Abstract: This file provides header stamp access routines.
*
* $Author: yjlou $
* $Id: hsModel.c,v 1.10 2006-04-17 12:06:19 yjlou Exp $
*/
#include "rtl_types.h"
#include "types.h"
#include "asicRegs.h"
#include "assert.h"
#include "hsModel.h"
#include "virtualMac.h"
#include "modelTrace.h"
#ifdef RTL865X_MODEL_USER
#include "rtl_utils.h"
#include "rtl_glue.h"
#endif
#include "rtl8651_debug.h"


int32 modelGetHsb( hsb_param_t* hsb )
{
	hsb_t rawHsb;
	int32 ret = SUCCESS;

	{ /* Word-Access */
		uint32 *pSrc, *pDst;
		uint32 i;

		/* We must assert structure size is the times of 4-bytes. */
		if ( (sizeof(rawHsb)%4) != 0 ) RTL_BUG( "sizeof(rawHsb) is not the times of 4-bytes." );

		pSrc = (uint32*)HSB_BASE;
		pDst = (uint32*)&rawHsb;
		for( i = 0; i < sizeof(rawHsb); i+=4 )
		{
			*pDst = big_endian(*pSrc);
			pSrc++;
			pDst++;
		}
	}

	convertHsbToSoftware( &rawHsb, hsb );
	return ret;
}

int32 modelSetHsb( hsb_param_t* hsb )
{
	hsb_t rawHsb;
	int32 ret = SUCCESS;
	
	convertHsbToAsic( hsb, &rawHsb );
	
	{ /* Word-Access */
		uint32 *pSrc, *pDst;
		uint32 i;

		/* We must assert structure size is the times of 4-bytes. */
		if ( (sizeof(rawHsb)%4) != 0 ) RTL_BUG( "sizeof(rawHsb) is not the times of 4-bytes." );

		pSrc = (uint32*)&rawHsb;
		pDst = (uint32*)HSB_BASE;
		for( i = 0; i < sizeof(rawHsb); i+=4 )
		{
			*pDst = big_endian(*pSrc);
			pSrc++;
			pDst++;
		}
	}

	return ret;
}


int32 modelGetHsa( hsa_param_t* hsa )
{
	hsa_t rawHsa;
	int32 ret = SUCCESS;

	{ /* Word-Access */
		uint32 *pSrc, *pDst;
		uint32 i;

		/* We must assert structure size is the times of 4-bytes. */
		if ( (sizeof(rawHsa)%4) != 0 ) RTL_BUG( "sizeof(rawHsa) is not the times of 4-bytes." );

		pSrc = (uint32*)HSA_BASE;
		pDst = (uint32*)&rawHsa;
		for( i = 0; i < sizeof(rawHsa); i+=4 )
		{
			*pDst = big_endian(*pSrc);
			pSrc++;
			pDst++;
		}
	}

	convertHsaToSoftware( &rawHsa, hsa );
	return ret;
}

int32 modelSetHsa( hsa_param_t* hsa )
{
	hsa_t rawHsa;
	int32 ret = SUCCESS;
	
	convertHsaToAsic( hsa, &rawHsa );
	
	{ /* Word-Access */
		uint32 *pSrc, *pDst;
		uint32 i;

		/* We must assert structure size is the times of 4-bytes. */
		if ( (sizeof(rawHsa)%4) != 0 ) RTL_BUG( "sizeof(rawHsa) is not the times of 4-bytes." );

		pSrc = (uint32*)&rawHsa;
		pDst = (uint32*)HSA_BASE;
		for( i = 0; i < sizeof(rawHsa); i+=4 )
		{
			*pDst = big_endian(*pSrc);
			pSrc++;
			pDst++;
		}
	}

	return ret;
}


