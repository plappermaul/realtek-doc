#ifndef __REG_SKC35_H__
#define __REG_SKC35_H__

#include <reg_map_util.h>

#define rmod(rtype, ival, ...) do {	\
		rtype##_T dummyr = { .v = ival };	\
		PP_NARG(__VA_ARGS__)(dummyr, __VA_ARGS__); \
		RVAL(rtype) = dummyr.v; \
	} while(0)

#define RFIELD16(_msb, _lsb, name) unsigned short name:(_msb - _lsb + 1)

/* Reserved field. */
#define RF_RSV(_msb, _lsb) unsigned int __rsv_##_msb##_##_lsb##__:(_msb-_lsb+1)
#define RF_RSV16(_msb, _lsb) unsigned short __rsv_##_msb##_##_lsb##__:(_msb-_lsb+1)

/* DC for don't care. This field is meaningful to RTL, but software don't give a shit. */
#define RF_DNC(_msb, _lsb) unsigned int __dnc_##_msb##_##_lsb##__:(_msb-_lsb+1)
#define RF_DNC16(_msb, _lsb) unsigned short __dnc_##_msb##_##_lsb##__:(_msb-_lsb+1)

#define RVAL(__reg) (*((regval)__reg##ar))
#define RMOD(__reg, ...) rmod(__reg, RVAL(__reg), __VA_ARGS__)
#define RFLD(__reg, fld) (*((const volatile __reg##_T *)__reg##ar)).f.fld

#define ASSERT_CONCAT_(a, b, c) a##b##c
#define ASSERT_CONCAT(a, b, c) ASSERT_CONCAT_(a, b, c)
#define skc35_assert(e) enum { ASSERT_CONCAT(assert_ln, __LINE__, __COUNTER__) = 1/(!!(e)) }

/* A register is composed by name, address, and fields;
   SKC35_REG_DEC() takes these parameters to declares
   1. a type named 'reg_name'_T,
   2. an address named 'reg_name'ar,
   3. an enum to makesure type size is 4-byte (static check; no actual code generated.) */
#define SKC35_REG_DEC(__reg, __addr, ...) \
	typedef union { \
		struct { \
			__VA_ARGS__ \
		} f; \
		unsigned int v;	\
	} __reg##_T; \
	static const unsigned int __reg##ar = __addr; \
	skc35_assert(sizeof(__reg##_T) == 4)

#endif
