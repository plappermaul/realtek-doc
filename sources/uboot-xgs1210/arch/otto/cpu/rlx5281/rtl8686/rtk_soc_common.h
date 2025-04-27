
#ifndef __RTL_SOC_COMMON_H__
#define __RTL_SOC_COMMON_H__

#ifndef __ASSEMBLY__
#include <common.h>
#include <exports.h>
#endif

#define FAILED  -1
#define SUCCESS 0
#define TRUE 1
#define FALSE 0

#define TO_UNCACHED_ADDR(addr) ((addr) | (0x20000000))
#define TO_CACHED_ADDR(addr) ((addr) & (0xDFFFFFFF))
#define REG(r)                  (*((unsigned int *)r))
#define TO_PHY_ADDR(a)		((a)&((unsigned int)0x0FFFFFFF))

#ifndef __ASSEMBLY__
typedef unsigned int uint32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef int     int32;
typedef char    int8;
typedef short   int16;
#endif //end of #ifndef __ASSEMBLY__


#endif //endof #ifndef __RTL_SOC_COMMON_H__

