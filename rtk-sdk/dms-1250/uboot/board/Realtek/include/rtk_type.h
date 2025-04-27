#ifndef __RTK_TYPE_H__
#define __RTK_TYPE_H__

#define Tint8   	char
#define Tbool   	Tint8
#define Tbyte   	Tint8
#define Tchar   	Tint8
#define Tuint8  	unsigned char
#define Tint16  	short
#define Tuint16 	unsigned short
#define Tint32  	int
#define Tuint32 	unsigned int
#define Tint64  	long long
#define Tuint64 	unsigned long long

#ifndef uint64
#define uint64		unsigned long long
#endif
#ifndef int64
#define int64		long long
#endif
#ifndef uint32
#define uint32		unsigned int
#endif
#ifndef int32
#define int32		int
#endif
#ifndef uint16
#define uint16		unsigned short	
#endif
#ifndef int16
#define int16		short
#endif
#ifndef uint8
#define uint8		unsigned char
#endif
#ifndef int8
#define int8		char
#endif
#ifndef ipaddr_t
#define ipaddr_t   	uint32
#endif

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN		6
#endif

#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

#ifndef DISABLE
#define DISABLE 0
#endif
#ifndef ENABLE
#define ENABLE  1
#endif

#ifndef NULL
#define NULL    0
#endif

#ifndef FAILED
#define FAILED  1
#endif
#ifndef SUCCESS
#define SUCCESS 0
#endif

#endif/*__RTK_TYPE_H__*/

