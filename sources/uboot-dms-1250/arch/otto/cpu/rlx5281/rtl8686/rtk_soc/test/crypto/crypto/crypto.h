/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003
* All rights reserved.
*
* Program : Header file for all crypto sources
* Abstract : This file includes all common definitions of crypto engine.
* $Id: crypto.h,v 1.3 2006/02/13 12:36:09 yjlou Exp $
* --------------------------------------------------------------------
*/

#ifndef __CRYPTO_H__
#define __CRYPTO_H__

//#include <rtl_glue.h>
//#include <rtl_types.h>
//#include <common.h>

#include "rtl_types.h"
#include <exports.h>

#define rtlglue_malloc malloc
#define rtlglue_free free
#define rtlglue_printf printf

/*************************
 * Put rtl_glue code here.
 *************************/
void rtlglue_srandom( uint32 seed );
uint32 rtlglue_random( void );
uint32 rtlglue_getmstime( uint32* pTime );
inline int32 rtlglue_flushDCache(uint32 start, uint32 size);

void rtl8651b_cryptoEngine_initdata(void);
int32 rtl8651b_cryptoEngine_init(uint32 descNum, int8 mode32Bytes);
int32 rtl8651b_cryptoEngine_exit(void);
int32 rtl8651b_cryptoEngine_alloc(uint32 descNum);
int32 rtl8651b_cryptoEngine_free(void);


void rtl8651b_cryptoEngineGetIntCounter(uint32 * doneCounter, uint32 * allDoneCounter);


/**************************************************************************
 * Data Structure for Descriptor
 **************************************************************************/
typedef struct rtl865xc_crypto_source_s
{
	uint32 own:1;
	uint32 eor:1;
	uint32 fs:1;
	uint32 resv1:1;
	uint32 ms:2;
	uint32 kam:3;
	uint32 aeskl:2;
	uint32 trides:1;
	uint32 cbc:1;
	uint32 ctr:1;
	uint32 hmac:1;
	uint32 md5:1;
	uint32 resv2:2;
	uint32 sbl:14;

	uint32 resv3:8;
	uint32 a2eo:8;
	uint32 resv4:2;
	uint32 enl:14;

	uint32 resv5:8;
	uint32 apl:8;
	uint32 resv6:16;

	uint32 sdbp;
} rtl865xc_crypto_source_t;

typedef struct rtl865xc_crypto_dest_s
{
	uint32 own:1;
	uint32 eor:1;
	uint32 resv1:16;
	uint32 dbl:14;
	uint32 ddbp;
	uint32 resv2;
	uint32 icv[5];
} rtl865xc_crypto_dest_t;


extern uint32 numSrc, numDst; /* the number of source & destination descriptor */
extern uint32 idxCpuSrc, idxAsicSrc;
extern uint32 idxCpuDst, idxAsicDst;
extern rtl865xc_crypto_source_t *ipssdar;
extern rtl865xc_crypto_dest_t *ipsddar;


#define MAX_PKTLEN (1<<14)


#endif// __CRYPTO_H__
