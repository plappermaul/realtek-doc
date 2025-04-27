/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.                                                    
* 
* Abstract : realtek type definition
 *
 * $Author: yachang $
 *
 * $Log: rtl_types.h,v $
 * Revision 1.1.1.1  2012/02/01 07:50:52  yachang
 * First working u-boot for RTL8676
 *
 *
 * Revision 1.1.1.1  2011/06/10 08:06:31  yachang
 *
 *
 * Revision 1.5  2005/09/22 05:22:31  bo_zhao
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2005/09/05 12:38:24  alva
 * initial import for add TFTP server
 *
 * Revision 1.4  2004/08/26 13:53:27  yjlou
 * -: remove all warning messages!
 * +: add compile flags "-Wno-implicit -Werror" in Makefile to treat warning as error!
 *
 * Revision 1.3  2004/05/12 06:35:11  yjlou
 * *: fixed the ASSERT_CSP() and ASSERT_ISR() macro: print #x will cause unpredictable result.
 *
 * Revision 1.2  2004/03/31 01:49:20  yjlou
 * *: all text files are converted to UNIX format.
 *
 * Revision 1.1  2004/03/16 06:36:13  yjlou
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2003/09/25 08:16:56  tony
 *  initial loader tree 
 *
 * Revision 1.1.1.1  2003/05/07 08:16:07  danwu
 * no message
 *
*/



#ifndef _RTL_TYPES_H
#define _RTL_TYPES_H

/*
 * Internal names for basic integral types.  Omit the typedef if
 * not possible for a machine/compiler combination.
 */

typedef unsigned long long	uint64;
typedef long long		int64;
typedef unsigned int	uint32;
typedef int			int32;
typedef unsigned short	uint16;
typedef short			int16;
typedef unsigned char	uint8;
typedef char			int8;

#define UINT32_MAX	UINT_MAX
#define INT32_MIN	INT_MIN
#define INT32_MAX	INT_MAX
#define UINT16_MAX	USHRT_MAX
#define INT16_MIN	SHRT_MIN
#define INT16_MAX	SHRT_MAX
#define UINT8_MAX	UCHAR_MAX
#define INT8_MIN		SCHAR_MIN
#define INT8_MAX	SCHAR_MAX

typedef uint32		memaddr;	
typedef uint32          ipaddr_t;

typedef struct {
    uint16      mac47_32;
    uint16      mac31_16;
    uint16      mac15_0;
    uint16		align;
} macaddr_t;


typedef int8*			calladdr_t;

typedef struct ether_addr_s {
	uint8 octet[6];
} ether_addr_t;

#include "rtl_depend.h"

#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef SUCCESS
#define SUCCESS 	0
#endif
#ifndef FAILED
#define FAILED -1
#endif

#define CLEARBITS(a,b)	((a) &= ~(b))
#define SETBITS(a,b)		((a) |= (b))
#define ISSET(a,b)		(((a) & (b))!=0)
#define ISCLEARED(a,b)	(((a) & (b))==0)


#define bzero(p, size)	memset(p, 0, size)

#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif			   /* max */

#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif			   /* min */


//extern int dprintf(char *fmt, ...);
//#define ASSERT_CSP(x) if (!(x)) {dprintf("\nAssertion fail at file %s, function %s, line number %d: (%s).\n", __FILE__, __FUNCTION__, __LINE__, #x); while(1);}
//#define ASSERT_ISR(x) if (!(x)) {printfByPolling("\nAssertion fail at file %s, function %s, line number %d: (%s).\n", __FILE__, __FUNCTION__, __LINE__, #x); while(1);}

//wei add, because we only use polling mode uart-print
#define ASSERT_CSP(x) if (!(x)) {printf("\nAssertion fail at file %s, function %s, line number %d: (%s).\n", __FILE__, __FUNCTION__, __LINE__, #x); while(1);}
#define ASSERT_ISR(x) if (!(x)) {printf("\nAssertion fail at file %s, function %s, line number %d: (%s).\n", __FILE__, __FUNCTION__, __LINE__, #x);   while(1);}


#endif
