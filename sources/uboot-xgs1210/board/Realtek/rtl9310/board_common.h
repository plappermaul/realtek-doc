#ifndef __BOARD_COMMON_H__
#define __BOARD_COMMON_H__

#define TO_UNCACHED_ADDR(addr) ((addr) | (0x20000000))
#define TO_CACHED_ADDR(addr) ((addr) & (0xDFFFFFFF))
#define REG(r) 			(*((unsigned int *)r))

#ifndef __ASSEMBLY__
typedef unsigned int uint32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef int	int32;
typedef char	int8;
typedef short	int16;
#endif //end of #ifndef __ASSEMBLY__

#endif //end of #ifndef __BOARD_COMMON_H__
