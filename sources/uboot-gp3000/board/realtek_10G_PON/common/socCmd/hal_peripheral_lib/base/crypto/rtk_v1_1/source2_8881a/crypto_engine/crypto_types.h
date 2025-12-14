#ifndef __CRYPTO_TYPES_H
#define __CRYPTO_TYPES_H

#ifdef __mips__
#include "rtl_types.h"
#else
#ifndef SUCCESS
#define SUCCESS     0
#endif
#ifndef FAILED
#define FAILED -1
#endif
typedef unsigned long long  uint64;
typedef long long       int64;
typedef unsigned int    uint32;
typedef int         int32;
typedef unsigned short  uint16;
typedef short           int16;
typedef unsigned char   uint8;
typedef char            int8;
#endif

#include "crypto_wrapper.h"

#endif
