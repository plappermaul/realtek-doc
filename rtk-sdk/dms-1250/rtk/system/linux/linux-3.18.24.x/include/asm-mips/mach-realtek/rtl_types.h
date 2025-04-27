/*
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
*
* Program : The header file of realtek type definition
* Abstract :
* Author :
* $Id: rtl_types.h,v 1.20 2007/04/04 15:48:54 chenyl Exp $
* $Log: rtl_types.h,v $
* Revision 1.20  2007/04/04 15:48:54  chenyl
* +: cleshell commend for asic register configuration : mmtu ( multicast mtu )
*
* Revision 1.19  2006/08/29 13:00:00  chenyl
* *: New SWNIC driver for RTL865xC
* *: some rearrange in re_core.c for Bootstrap sequence being more readable.
*
* Revision 1.18  2006/02/27 07:47:06  ympan
* +: No change by ympan
*
* Revision 1.17  2005/09/27 05:59:44  chenyl
* *: modify IRAM / DRAM layout:
*   IRAM/DRAM-FWD       : external used, swNic ... blahblah
*   IRAM/DRAM-L2-FWD    : fwdengine internal used, L2 and below (ex. preprocess, postprocess)
*   IRAM/DRAM-L34-FWD   : fwdengine internal used, L3/L4 process only (ex. Routing, decision table...etc).
*
*   => If L34 is used, we strongly suggest L2 must be used, too.
*
* Revision 1.16  2005/08/23 14:38:26  chenyl
* +: apply prioirty IRAM/DRAM usage
*
* Revision 1.15  2005/08/22 07:33:55  chenyl
* *: don't set DRAM/IRAM for other OSs yet.
*
* Revision 1.14  2005/08/18 09:14:08  chenyl
* *: add code to porting to other OSs
*
* Revision 1.13  2005/08/18 06:29:29  chenyl
* +: always define the rtlglue_printf in rtl_types.h
*
* Revision 1.12  2005/07/01 09:34:41  yjlou
* *: porting swNic2.c into model code.
*
* Revision 1.11  2005/06/19 05:29:37  yjlou
* *: use 'unsigned int' to replace 'size_t'
* *: define spinlock_t when RTL865X_MODEL_USER defined.
*
* Revision 1.10  2005/06/10 05:32:22  yjlou
* +: Porting CLE Shell to Linux user space for model test.
*    See RTL865X_MODEL_USER compile flags.
*
* Revision 1.9  2005/01/10 03:21:43  yjlou
* *: always define __IRAM and __DRAM
*
* Revision 1.8  2004/07/23 13:42:45  tony
* *: remove all warning messages
*
* Revision 1.7  2004/07/05 08:25:32  chenyl
* +: define __IRAM, __DRAM for module test
*
* Revision 1.6  2004/07/04 15:04:55  cfliu
* +: add IRAM and DRAM
*
* Revision 1.5  2004/04/20 03:44:03  tony
* if disable define "RTL865X_OVER_KERNEL" and "RTL865X_OVER_LINUX", __KERNEL__ and __linux__ will be undefined.
*
* Revision 1.4  2004/03/19 13:13:35  cfliu
* Reorganize ROME driver local header files. Put all private data structures into different .h file corrsponding to its layering
* Rename printf, printk, malloc, free with rtlglue_XXX prefix
*
* Revision 1.3  2004/03/05 07:44:27  cfliu
* fix header file problem for ctype.h
*
* Revision 1.2  2004/03/03 10:40:38  yjlou
* *: commit for mergence the difference in rtl86xx_tbl/ since 2004/02/26.
*
* Revision 1.1  2004/02/25 14:26:33  chhuang
* *** empty log message ***
*
* Revision 1.3  2004/02/25 14:24:52  chhuang
* *** empty log message ***
*
* Revision 1.8  2003/12/10 06:30:12  tony
* add linux/config.h, disable define CONFIG_RTL865X_NICDRV2 in mbuf.c by default
*
* Revision 1.7  2003/12/03 14:25:43  cfliu
* change SIZE_T to _SIZE_T. Linux kernel seems to recognize _SIZE_T
*
* Revision 1.6  2003/10/01 12:29:02  tony
* #define DEBUG_P(args...) while(0);
*
* Revision 1.5  2003/10/01 10:31:47  tony
* solve all the compiler warnning messages in the board.c
*
* Revision 1.4  2003/09/30 06:07:50  orlando
* check in RTL8651BLDRV_V20_20030930
*
* Revision 1.30  2003/07/21 06:27:49  cfliu
* no message
*
* Revision 1.29  2003/04/30 15:32:30  cfliu
* move macros to types.h
*
* Revision 1.28  2003/03/13 10:29:22  cfliu
* Remove unused symbols
*
* Revision 1.27  2003/03/06 05:00:04  cfliu
* Move '#pragma ghs inlineprologue' to rtl_depend.h since it is compiler dependent
*
* Revision 1.26  2003/03/06 03:41:46  danwu
* Prevent compiler from generating internal sub-routine call code at the
*  function prologue and epilogue automatically
*
* Revision 1.25  2003/03/03 09:16:35  hiwu
* remove ip4a
*
* Revision 1.24  2003/02/18 10:04:06  jzchen
* Add ether_addr_t to compatable with protocol stack's ether_addr
*
* Revision 1.23  2003/01/21 05:59:51  cfliu
* add min, max, SETBITS, CLEARBITS, etc.
*
* Revision 1.22  2002/11/25 07:31:30  cfliu
* Remove _POSIX_SOURCE since it is cygwin specific
*
* Revision 1.21  2002/09/30 11:51:49  jzchen
* Add ASSERT_ISR for not print inside ISR
*
* Revision 1.20  2002/09/18 01:43:24  jzchen
* Add type limit definition
*
* Revision 1.19  2002/09/16 00:14:34  elvis
* remove struct posix_handle_t (change the handle type from
*  structure to uint32)
*
* Revision 1.18  2002/08/20 01:40:40  danwu
* Add definitions of ipaddr_t & macaddr_t.
*
* Revision 1.17  2002/07/30 04:36:30  danwu
* Add ASSERT_CSP.
*
* Revision 1.16  2002/07/19 06:47:30  cfliu
* Add _POSIX_SOURCE symbol
*
* Revision 1.15  2002/07/05 02:10:39  elvis
* Add new types for OSK
*
* Revision 1.14  2002/07/03 12:36:21  orlando
* <rtl_depend.h> will use type definitions. Has to be moved to
* be after the type declaration lines.
*
* Revision 1.13  2002/07/03 09:19:00  cfliu
* Removed all standard header files from source code. They would be included by <core/types.h>-><rtl_depend.h>
*
* Revision 1.12  2002/07/03 09:16:48  cfliu
* Removed all standard header files from source code. They would be included by <core/types.h>-><rtl_depend.h>
*
* Revision 1.11  2002/07/03 07:14:47  orlando
* Add "struct posix_handle_t_", used by POSIX module.
*
* Revision 1.9  2002/06/21 03:15:36  cfliu
* Add time.h for struct timeval
*
* Revision 1.8  2002/06/14 01:58:03  cfliu
* Move sa_family_t to socket
*
* Revision 1.7  2002/06/13 09:37:42  cfliu
* Move byte order conversion routines to socket
*
* Revision 1.6  2002/05/23 04:24:37  hiwu
* change memaddr_t to calladdr_t
*
* Revision 1.5  2002/05/13 10:15:16  hiwu
* add new type definition
*
* Revision 1.4  2002/05/09 05:21:51  cfliu
* Add parenthesis around swaps16, swapl32
*
* Revision 1.3  2002/04/30 03:07:34  orlando
* Remove UIxx_T definitions to conform with new
* naming conventions.
*
* Revision 1.2  2002/04/29 10:10:32  hiwu
* add NTOHS macro
*
* Revision 1.1.1.1  2002/04/26 08:53:53  orlando
* Initial source tree creation.
*
* Revision 1.9  2002/04/25 03:59:05  cfliu
* no message
*
* Revision 1.8  2002/04/08 08:08:04  hiwu
* initial version
*
*/


#ifndef _RTL_TYPES_H
#define _RTL_TYPES_H

/*
 * Internal names for basic integral types.  Omit the typedef if
 * not possible for a machine/compiler combination.
 */
#ifdef __linux__
#ifdef __KERNEL__
#include <linux/version.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
#include <linux/config.h>
#endif
#include <linux/ctype.h>
#include <linux/module.h>
#include <linux/string.h>
#endif /*__KERNEL__*/
#endif /*__linux__*/

/* ===============================================================================
        IRAM / DRAM definition
    =============================================================================== */
#undef __DRAM_GEN
#undef __DRAM_FWD
#undef __DRAM_L2_FWD
#undef __DRAM_L34_FWD
#undef __DRAM_EXTDEV
#undef __DRAM_AIRGO
#undef __DRAM_RTKWLAN
#undef __DRAM_CRYPTO
#undef __DRAM_VOIP
#undef __DRAM_TX
#undef __DRAM

#undef __IRAM_GEN
#undef __IRAM_FWD
#undef __IRAM_L2_FWD
#undef __IRAM_L34_FWD
#undef __IRAM_EXTDEV
#undef __IRAM_AIRGO
#undef __IRAM_RTKWLAN
#undef __IRAM_CRYPTO
#undef __IRAM_VOIP
#undef __IRAM_TX
#undef __IRAM

#if defined(__linux__)&&defined(__KERNEL__)&&defined(CONFIG_RTL865X)
    #define __DRAM_GEN          __attribute__  ((section(".dram-gen")))
    #define __DRAM_FWD          __attribute__  ((section(".dram-fwd")))
    #define __DRAM_L2_FWD       __attribute__  ((section(".dram-l2-fwd")))
    #define __DRAM_L34_FWD      __attribute__  ((section(".dram-l34-fwd")))
    #define __DRAM_EXTDEV       __attribute__  ((section(".dram-extdev")))
    #define __DRAM_AIRGO        __attribute__  ((section(".dram-airgo")))
    #define __DRAM_RTKWLAN      __attribute__  ((section(".dram-rtkwlan")))
    #define __DRAM_CRYPTO       __attribute__  ((section(".dram-crypto")))
    #define __DRAM_VOIP         __attribute__  ((section(".dram-voip")))
    #define __DRAM_TX           __attribute__  ((section(".dram-tx")))
    #define __DRAM              __attribute__  ((section(".dram")))

    #define __IRAM_GEN          __attribute__  ((section(".iram-gen")))
    #define __IRAM_FWD          __attribute__  ((section(".iram-fwd")))
    #define __IRAM_L2_FWD       __attribute__  ((section(".iram-l2-fwd")))
    #define __IRAM_L34_FWD      __attribute__  ((section(".iram-l34-fwd")))
    #define __IRAM_EXTDEV       __attribute__  ((section(".iram-extdev")))
    #define __IRAM_AIRGO        __attribute__  ((section(".iram-airgo")))
    #define __IRAM_RTKWLAN      __attribute__  ((section(".iram-rtkwlan")))
    #define __IRAM_CRYPTO       __attribute__  ((section(".iram-crypto")))
    #define __IRAM_VOIP         __attribute__  ((section(".iram-voip")))
    #define __IRAM_TX           __attribute__  ((section(".iram-tx")))
    #define __IRAM              __attribute__  ((section(".iram")))
#else
    #define __DRAM_GEN
    #define __DRAM_FWD
    #define __DRAM_L2_FWD
    #define __DRAM_L34_FWD
    #define __DRAM_EXTDEV
    #define __DRAM_AIRGO
    #define __DRAM_RTKWLAN
    #define __DRAM_CRYPTO
    #define __DRAM_VOIP
    #define __DRAM_TX
    #define __DRAM

    #define __IRAM_GEN
    #define __IRAM_FWD
    #define __IRAM_L2_FWD
    #define __IRAM_L34_FWD
    #define __IRAM_EXTDEV
    #define __IRAM_AIRGO
    #define __IRAM_RTKWLAN
    #define __IRAM_CRYPTO
    #define __IRAM_VOIP
    #define __IRAM_TX
    #define __IRAM
#endif

/* ===============================================================================
        print macro
    =============================================================================== */
#if defined(__linux__)&&defined(__KERNEL__)

    #define rtlglue_printf  printk

#else   /* defined(__linux__)&&defined(__KERNEL__) */

#ifdef  RTL865X_TEST
    #include <ctype.h>
#endif  /* RTL865X_TEST */

#define rtlglue_printf  printf

#endif  /* defined(__linux__)&&defined(__KERNEL__) */

/* ===============================================================================
        Type definition
    =============================================================================== */

#ifndef uint64
#define uint64                  unsigned long long
#endif
#ifndef int64
#define int64                   long long
#endif
#ifndef uint32
#define uint32                  unsigned int
#endif
#ifndef int32
#define int32                   int
#endif
#ifndef uint16
#define uint16                  unsigned short
#endif
#ifndef int16
#define int16                   short
#endif
#ifndef uint8
#define uint8                   unsigned char
#endif
#ifndef int8
#define int8                    char
#endif
#ifndef ipaddr_t
#define ipaddr_t                uint32
#endif
#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN          6
#endif

typedef uint32      memaddr;
typedef struct {
    uint16      mac47_32;
    uint16      mac31_16;
    uint16      mac15_0;
} macaddr_t;

typedef struct ether_addr_s {
    uint8 octet[ETHER_ADDR_LEN];
} ether_addr_t;


#define IPv6_ADDR_LEN               16
typedef struct ipv6addr_s{
       uint8 octet[IPv6_ADDR_LEN];
}ipv6addr_t;



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
#define SUCCESS     0
#endif
#ifndef FAILED
#define FAILED -1
#endif



#define DEBUG_P(args...) while(0);
#ifndef OK
#define OK      0
#endif
#ifndef NOT_OK
#define NOT_OK  1
#endif

#ifndef CLEARBITS
#define CLEARBITS(a,b)  ((a) &= ~(b))
#endif

#ifndef SETBITS
#define SETBITS(a,b)        ((a) |= (b))
#endif

#ifndef ISSET
#define ISSET(a,b)      (((a) & (b))!=0)
#endif

#ifndef ISCLEARED
#define ISCLEARED(a,b)  (((a) & (b))==0)
#endif

#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif             /* max */

#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif             /* min */

//round down x to multiple of y.  Ex: ROUNDDOWN(20, 7)=14
#ifndef ROUNDDOWN
#define ROUNDDOWN(x, y) (((x)/(y))*(y))
#endif

//round up x to multiple of y. Ex: ROUNDUP(11, 7) = 14
#ifndef ROUNDUP
#define ROUNDUP(x, y)   ((((x)+((y)-1))/(y))*(y))  /* to any y */
#endif

#ifndef ROUNDUP2
#define ROUNDUP2(x, y)  (((x)+((y)-1))&(~((y)-1))) /* if y is powers of two */
#endif

#ifndef ROUNDUP4
#define ROUNDUP4(x)     ((1+(((x)-1)>>2))<<2)
#endif

#ifndef IS4BYTEALIGNED
#define IS4BYTEALIGNED(x)    ((((x) & 0x3)==0)? 1 : 0)
#endif

#ifndef __offsetof
#define __offsetof(type, field) ((unsigned long)(&((type *)0)->field))
#endif

#ifndef offsetof
#define offsetof(type, field) __offsetof(type, field)
#endif

#ifndef RTL_PROC_CHECK
#define RTL_PROC_CHECK(expr, success) \
    do {\
            int __retval; \
            if ((__retval = (expr)) != (success))\
            {\
                rtlglue_printf("ERROR >>> [%s]:[%d] failed -- return value: %d\n", __FUNCTION__,__LINE__, __retval);\
                return __retval; \
            }\
        }while(0)
#endif

#ifndef RTL_STREAM_SAME
#define RTL_STREAM_SAME(s1, s2) \
    ((strlen(s1) == strlen(s2)) && (strcmp(s1, s2) == 0))
#endif

#ifndef ASSERT
#define ASSERT(expr) \
            if(!(expr)) {                   \
                    rtlglue_printf( "\033[33;41m%s:%d: assert(%s)\033[m\n", \
                                __FILE__,__LINE__,#expr);       \
                    }
#endif

#define ASSERT_ISR(x) if(!(x)) {while(1);}
#define RTL_STATIC_INLINE   static __inline__

#define ASSERT_CSP(x) if (!(x)) {rtlglue_printf("\nAssert Fail: %s %d", __FILE__, __LINE__); while(1);}

#if defined(RTL865X_TEST) || defined(RTL865X_MODEL_USER)
/* Only model code needs to define the following code. */
typedef struct { } spinlock_t;
typedef uint8 u8;
typedef uint16 u16;
typedef uint32 u32;
#endif

#if defined(DRTL_TBLDRV)||defined(RTL865X_TEST)
//only ROME driver and module test code need to include this header file
#include "rtl_glue.h"
#endif

#if defined(RTL865X_TEST)||defined(RTL865X_MODEL_USER)
#define UNCACHE_MASK            0
#define UNCACHE(addr)           (addr)
#define CACHED(addr)            ((uint32)(addr))
#else
#define UNCACHE_MASK            0x20000000
#define UNCACHE(addr)           ((UNCACHE_MASK)|(uint32)(addr))
#define CACHED(addr)            ((uint32)(addr) & ~(UNCACHE_MASK))
#endif


#define swapl32(x)\
        ((((x) & 0xff000000U) >> 24) | \
         (((x) & 0x00ff0000U) >>  8) | \
         (((x) & 0x0000ff00U) <<  8) | \
         (((x) & 0x000000ffU) << 24))
#define swaps16(x)        \
        ((((x) & 0xff00) >> 8) | \
         (((x) & 0x00ff) << 8))

#ifdef _LITTLE_ENDIAN

/*#ifndef __linux__*/
    #define ntohs(x)   (swaps16(x))
    #define ntohl(x)   (swapl32(x))
    #define htons(x)   (swaps16(x))
    #define htonl(x)   (swapl32(x))
/*#endif*/
    #define NTOHL(d) ((d) = ntohl((d)))
    #define NTOHS(d) ((d) = ntohs((uint16)(d)))
    #define HTONL(d) ((d) = htonl((d)))
    #define HTONS(d) ((d) = htons((uint16)(d)))

    #define GET_UINT16_UNALIGNED( ptr )                 GET_UINT16_LITTLE_ENDIAN_UNALIGNED(ptr)
    #define SET_UINT16_UNALIGNED(  u16value, ptr )      SET_UINT16_LITTLE_ENDIAN_UNALIGNED(  u16value, ptr )
    #define GET_UINT32_UNALIGNED( ptr )                 GET_UINT32_LITTLE_ENDIAN_UNALIGNED(ptr)
    #define SET_UINT32_UNALIGNED(  u32value, ptr )      SET_UINT32_LITTLE_ENDIAN_UNALIGNED(  u32value,ptr)
    #define PKTGET_UINT16_UNALIGNED( ptr )                  GET_UINT16_BIG_ENDIAN_UNALIGNED(ptr)
    #define PKTSET_UINT16_UNALIGNED(  u16value, ptr     )   SET_UINT16_BIG_ENDIAN_UNALIGNED(  u16value, ptr )
    #define PKTGET_UINT32_UNALIGNED( ptr )                  GET_UINT32_BIG_ENDIAN_UNALIGNED(ptr)
    #define PKTSET_UINT32_UNALIGNED(  u32value, ptr )   SET_UINT32_BIG_ENDIAN_UNALIGNED(  u32value,ptr)

#else /*Big endian system */

/* chhuang */
#ifdef __linux__
    #undef ntohs
    #undef ntohl
    #undef htons
    #undef htonl
    #define ntohs(x)   (x)
    #define ntohl(x)   (x)
    #define htons(x)   (x)
    #define htonl(x)   (x)
    #define _LINUX_BYTEORDER_GENERIC_H
#endif /* __linux__ */
    #define NTOHL(d) ((d) = ntohl(d))
    #define NTOHS(d) ((d) = ntohs((uint16)(d)))
    #define HTONL(d) ((d) = htonl(d))
    #define HTONS(d) ((d) = htons((uint16)(d)))
    #define GET_UINT16_UNALIGNED( ptr )             GET_UINT16_BIG_ENDIAN_UNALIGNED(ptr)
    #define SET_UINT16_UNALIGNED(  u16value, ptr )  SET_UINT16_BIG_ENDIAN_UNALIGNED(  u16value, ptr )
    #define GET_UINT32_UNALIGNED( ptr )             GET_UINT32_BIG_ENDIAN_UNALIGNED(ptr)
    #define SET_UINT32_UNALIGNED(  u32value, ptr )  SET_UINT32_BIG_ENDIAN_UNALIGNED(  u32value,ptr)
    #define PKTGET_UINT16_UNALIGNED( ptr )              GET_UINT16_BIG_ENDIAN_UNALIGNED(ptr)
    #define PKTSET_UINT16_UNALIGNED(  u16value, ptr)    SET_UINT16_BIG_ENDIAN_UNALIGNED(  u16value, ptr )
    #define PKTGET_UINT32_UNALIGNED( ptr )              GET_UINT32_BIG_ENDIAN_UNALIGNED(ptr)
    #define PKTSET_UINT32_UNALIGNED(  u32value, ptr)    SET_UINT32_BIG_ENDIAN_UNALIGNED(  u32value,ptr)
#endif

#endif/*_LITTLE_ENDIAN*/

