#ifndef _REGISTER_MAP_H_
#define _REGISTER_MAP_H_
/*-----------------------------------------------------
 Extraced from ./chip_index.xml: v0.0.19|2014-05-06
 RP Last modified at Tue May  6 03:01:43 2014
-----------------------------------------------------*/
typedef volatile unsigned int * regval;

#define RLOAD(_var, _reg)  _reg##_T _var = { .v = _reg##rv }
#define RTYPE(_var, _reg)  _reg##_T _var = { .v = 0 }
#define RSTORE(_var, _reg) _reg##rv = _var.v

/* for __VA_NARG__ */
#define PP_NARG(...)  PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N(_01,_02,_03,_04,_05,_06,_07,_08,_09,_10, \
                 _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
                 _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
                 _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
                 _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
                 _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
                 _61,_62,_63,  N,...) N
#define PP_RSEQ_N()                      63, _rset62, 61, _rset60, \
  59, _rset58, 57, _rset56, 55, _rset54, 53, _rset52, 51, _rset50, \
  49, _rset48, 47, _rset46, 45, _rset44, 43, _rset42, 41, _rset40, \
  39, _rset38, 37, _rset36, 35, _rset34, 33, _rset32, 31, _rset30, \
  29, _rset28, 27, _rset26, 25, _rset24, 23, _rset22, 21, _rset20, \
  19, _rset18, 17, _rset16, 15, _rset14, 13, _rset12, 11, _rset10, \
  09,  _rset8, 07,  _rset6, 05,  _rset4, 03,  _rset2, 01,      00
#define rset(rtype, ival, ...) do {            \
		rtype##_T dummyr = { .v = ival };          \
		PP_NARG(__VA_ARGS__)(dummyr, __VA_ARGS__); \
		rtype##rv = dummyr.v;                      \
	} while(0)
#define _rset2(var, \
               f00, v00) do { \
		var.f.f00 = v00; \
	} while(0)

#define _rset4(var, \
               f00, v00, f01, v01) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
	} while(0)

#define _rset6(var, \
               f00, v00, f01, v01, f02, v02) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
	} while(0)

#define _rset8(var, \
               f00, v00, f01, v01, f02, v02, f03, v03) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
	} while(0)

#define _rset10(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
	} while(0)

#define _rset12(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
	} while(0)

#define _rset14(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
	} while(0)

#define _rset16(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
	} while(0)

#define _rset18(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
	} while(0)

#define _rset20(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
	} while(0)

#define _rset22(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
	} while(0)

#define _rset24(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
	} while(0)

#define _rset26(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
	} while(0)

#define _rset28(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
	} while(0)

#define _rset30(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
	} while(0)

#define _rset32(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
	} while(0)

#define _rset34(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
	} while(0)

#define _rset36(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
	} while(0)

#define _rset38(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
	} while(0)

#define _rset40(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18, f19, v19) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
		var.f.f19 = v19; \
	} while(0)

#define _rset42(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18, f19, v19, \
                f20, v20) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
		var.f.f19 = v19; \
		var.f.f20 = v20; \
	} while(0)

#define _rset44(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18, f19, v19, \
                f20, v20, f21, v21) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
		var.f.f19 = v19; \
		var.f.f20 = v20; \
		var.f.f21 = v21; \
	} while(0)

#define _rset46(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18, f19, v19, \
                f20, v20, f21, v21, f22, v22) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
		var.f.f19 = v19; \
		var.f.f20 = v20; \
		var.f.f21 = v21; \
		var.f.f22 = v22; \
	} while(0)

#define _rset48(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18, f19, v19, \
                f20, v20, f21, v21, f22, v22, f23, v23) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
		var.f.f19 = v19; \
		var.f.f20 = v20; \
		var.f.f21 = v21; \
		var.f.f22 = v22; \
		var.f.f23 = v23; \
	} while(0)

#define _rset50(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18, f19, v19, \
                f20, v20, f21, v21, f22, v22, f23, v23, \
                f24, v24) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
		var.f.f19 = v19; \
		var.f.f20 = v20; \
		var.f.f21 = v21; \
		var.f.f22 = v22; \
		var.f.f23 = v23; \
		var.f.f24 = v24; \
	} while(0)

#define _rset52(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18, f19, v19, \
                f20, v20, f21, v21, f22, v22, f23, v23, \
                f24, v24, f25, v25) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
		var.f.f19 = v19; \
		var.f.f20 = v20; \
		var.f.f21 = v21; \
		var.f.f22 = v22; \
		var.f.f23 = v23; \
		var.f.f24 = v24; \
		var.f.f25 = v25; \
	} while(0)

#define _rset54(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18, f19, v19, \
                f20, v20, f21, v21, f22, v22, f23, v23, \
                f24, v24, f25, v25, f26, v26) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
		var.f.f19 = v19; \
		var.f.f20 = v20; \
		var.f.f21 = v21; \
		var.f.f22 = v22; \
		var.f.f23 = v23; \
		var.f.f24 = v24; \
		var.f.f25 = v25; \
		var.f.f26 = v26; \
	} while(0)

#define _rset56(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18, f19, v19, \
                f20, v20, f21, v21, f22, v22, f23, v23, \
                f24, v24, f25, v25, f26, v26, f27, v27) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
		var.f.f19 = v19; \
		var.f.f20 = v20; \
		var.f.f21 = v21; \
		var.f.f22 = v22; \
		var.f.f23 = v23; \
		var.f.f24 = v24; \
		var.f.f25 = v25; \
		var.f.f26 = v26; \
		var.f.f27 = v27; \
	} while(0)

#define _rset58(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18, f19, v19, \
                f20, v20, f21, v21, f22, v22, f23, v23, \
                f24, v24, f25, v25, f26, v26, f27, v27, \
                f28, v28) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
		var.f.f19 = v19; \
		var.f.f20 = v20; \
		var.f.f21 = v21; \
		var.f.f22 = v22; \
		var.f.f23 = v23; \
		var.f.f24 = v24; \
		var.f.f25 = v25; \
		var.f.f26 = v26; \
		var.f.f27 = v27; \
		var.f.f28 = v28; \
	} while(0)

#define _rset60(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18, f19, v19, \
                f20, v20, f21, v21, f22, v22, f23, v23, \
                f24, v24, f25, v25, f26, v26, f27, v27, \
                f28, v28, f29, v29) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
		var.f.f19 = v19; \
		var.f.f20 = v20; \
		var.f.f21 = v21; \
		var.f.f22 = v22; \
		var.f.f23 = v23; \
		var.f.f24 = v24; \
		var.f.f25 = v25; \
		var.f.f26 = v26; \
		var.f.f27 = v27; \
		var.f.f28 = v28; \
		var.f.f29 = v29; \
	} while(0)

#define _rset62(var, \
                f00, v00, f01, v01, f02, v02, f03, v03, \
                f04, v04, f05, v05, f06, v06, f07, v07, \
                f08, v08, f09, v09, f10, v10, f11, v11, \
                f12, v12, f13, v13, f14, v14, f15, v15, \
                f16, v16, f17, v17, f18, v18, f19, v19, \
                f20, v20, f21, v21, f22, v22, f23, v23, \
                f24, v24, f25, v25, f26, v26, f27, v27, \
                f28, v28, f29, v29, f30, v30) do { \
		var.f.f00 = v00; \
		var.f.f01 = v01; \
		var.f.f02 = v02; \
		var.f.f03 = v03; \
		var.f.f04 = v04; \
		var.f.f05 = v05; \
		var.f.f06 = v06; \
		var.f.f07 = v07; \
		var.f.f08 = v08; \
		var.f.f09 = v09; \
		var.f.f10 = v10; \
		var.f.f11 = v11; \
		var.f.f12 = v12; \
		var.f.f13 = v13; \
		var.f.f14 = v14; \
		var.f.f15 = v15; \
		var.f.f16 = v16; \
		var.f.f17 = v17; \
		var.f.f18 = v18; \
		var.f.f19 = v19; \
		var.f.f20 = v20; \
		var.f.f21 = v21; \
		var.f.f22 = v22; \
		var.f.f23 = v23; \
		var.f.f24 = v24; \
		var.f.f25 = v25; \
		var.f.f26 = v26; \
		var.f.f27 = v27; \
		var.f.f28 = v28; \
		var.f.f29 = v29; \
		var.f.f30 = v30; \
	} while(0)

/*-----------------------------------------------------
 Extraced from file_System_Control.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:5; //0
		unsigned int clkm90_phs_45:1; //0
		unsigned int old_gnt_nand:1; //0
		unsigned int old_gnt_voip:1; //0
		unsigned int lx_arb_del_en:1; //0
		unsigned int mbz_1:10; //0
		unsigned int lx1_cmu_md:2; //0
		unsigned int mbz_2:9; //0
		unsigned int mbz_3:1; //0
		unsigned int mbz_4:1; //0
	} f;
	unsigned int v;
} LX_GRANT_CTRL_T;
#define LX_GRANT_CTRLrv (*((regval)0xb8000108))
#define LX_GRANT_CTRLdv (0x00000000)
#define RMOD_LX_GRANT_CTRL(...) rset(LX_GRANT_CTRL, LX_GRANT_CTRLrv, __VA_ARGS__)
#define RIZS_LX_GRANT_CTRL(...) rset(LX_GRANT_CTRL, 0, __VA_ARGS__)
#define RFLD_LX_GRANT_CTRL(fld) (*((const volatile LX_GRANT_CTRL_T *)0xb8000108)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int ddrckm90_tap:5; //0
		unsigned int mbz_1:3; //0
		unsigned int ddrckm_tap:5; //0
		unsigned int mbz_2:3; //0
		unsigned int ddrck_phs_sel:5; //0
	} f;
	unsigned int v;
} DRCKO_T;
#define DRCKOrv (*((regval)0xb800021c))
#define DRCKOdv (0x00000000)
#define RMOD_DRCKO(...) rset(DRCKO, DRCKOrv, __VA_ARGS__)
#define RIZS_DRCKO(...) rset(DRCKO, 0, __VA_ARGS__)
#define RFLD_DRCKO(fld) (*((const volatile DRCKO_T *)0xb800021c)).f.fld

/*-----------------------------------------------------
 Extraced from file_CPU0_Memory_Global.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int dram_type:4; //1
		unsigned int boot_sel:4; //0
		unsigned int ip_ref:1; //0
		unsigned int dp_ref:1; //0
		unsigned int eeprom_type:1; //0
		unsigned int d_signal:1; //0
		unsigned int flash_map0_dis:1; //0
		unsigned int flash_map1_dis:1; //1
		unsigned int mbz_0:3; //0
		unsigned int d_init_trig:1; //0
		unsigned int ocp1_frq_slower:1; //0
		unsigned int lx1_frq_slower:1; //0
		unsigned int lx2_frq_slower:1; //0
		unsigned int lx3_frq_slower:1; //0
		unsigned int ocp0_frq_slower:1; //0
		unsigned int ocp1_rbf_mask_en:1; //0
		unsigned int ocp0_rbf_mask_en:1; //0
		unsigned int ocp1_rbf_f_dis:1; //1
		unsigned int ocp0_rbf_f_dis:1; //1
		unsigned int sync_lx0_dram:1; //0
		unsigned int sync_lx1_dram:1; //0
		unsigned int sync_lx2_dram:1; //0
		unsigned int sync_lx3_dram:1; //0
		unsigned int sync_ocp0_dram:1; //0
	} f;
	unsigned int v;
} MCR_T;
#define MCRrv (*((regval)0xb8001000))
#define MCRdv (0x10040060)
#define RMOD_MCR(...) rset(MCR, MCRrv, __VA_ARGS__)
#define RIZS_MCR(...) rset(MCR, 0, __VA_ARGS__)
#define RFLD_MCR(fld) (*((const volatile MCR_T *)0xb8001000)).f.fld

typedef union {
	struct {
		unsigned int lx_jit_to_en:1; //0
		unsigned int mbz_0:1; //0
		unsigned int bankcnt:2; //1
		unsigned int mbz_1:2; //0
		unsigned int dbuswid:2; //1
		unsigned int rowcnt:4; //0
		unsigned int colcnt:4; //0
		unsigned int dchipsel:1; //1
		unsigned int fast_rx:1; //0
		unsigned int bstref:1; //0
		unsigned int mbz_2:13; //0
	} f;
	unsigned int v;
} DCR_T;
#define DCRrv (*((regval)0xb8001004))
#define DCRdv (0x11008000)
#define RMOD_DCR(...) rset(DCR, DCRrv, __VA_ARGS__)
#define RIZS_DCR(...) rset(DCR, 0, __VA_ARGS__)
#define RFLD_DCR(fld) (*((const volatile DCR_T *)0xb8001004)).f.fld

typedef union {
	struct {
		unsigned int t_cas:4; //2
		unsigned int t_wr:4; //3
		unsigned int t_cwl:4; //0
		unsigned int t_rtp:4; //15
		unsigned int t_wtr:4; //15
		unsigned int t_refi:4; //0
		unsigned int t_refi_unit:4; //4
		unsigned int mbz_0:4; //0
	} f;
	unsigned int v;
} DTR0_T;
#define DTR0rv (*((regval)0xb8001008))
#define DTR0dv (0x230ff040)
#define RMOD_DTR0(...) rset(DTR0, DTR0rv, __VA_ARGS__)
#define RIZS_DTR0(...) rset(DTR0, 0, __VA_ARGS__)
#define RFLD_DTR0(fld) (*((const volatile DTR0_T *)0xb8001008)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int t_rp:5; //31
		unsigned int mbz_1:3; //0
		unsigned int t_rcd:5; //31
		unsigned int mbz_2:3; //0
		unsigned int t_rrd:5; //31
		unsigned int mbz_3:3; //0
		unsigned int t_fawg:5; //31
	} f;
	unsigned int v;
} DTR1_T;
#define DTR1rv (*((regval)0xb800100c))
#define DTR1dv (0x1f1f1f1f)
#define RMOD_DTR1(...) rset(DTR1, DTR1rv, __VA_ARGS__)
#define RIZS_DTR1(...) rset(DTR1, 0, __VA_ARGS__)
#define RFLD_DTR1(fld) (*((const volatile DTR1_T *)0xb800100c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int t_rfc:8; //255
		unsigned int mbz_1:2; //0
		unsigned int t_ras:6; //63
		unsigned int mbz_2:12; //0
	} f;
	unsigned int v;
} DTR2_T;
#define DTR2rv (*((regval)0xb8001010))
#define DTR2dv (0x0ff3f000)
#define RMOD_DTR2(...) rset(DTR2, DTR2rv, __VA_ARGS__)
#define RIZS_DTR2(...) rset(DTR2, 0, __VA_ARGS__)
#define RFLD_DTR2(fld) (*((const volatile DTR2_T *)0xb8001010)).f.fld

typedef union {
	struct {
		unsigned int dtr_up_busy_mrs_busy:1; //0
		unsigned int mbz_0:6; //0
		unsigned int dis_dram_ref:1; //0
		unsigned int mbz_1:3; //0
		unsigned int mr_mode_en:1; //0
		unsigned int mbz_2:2; //0
		unsigned int mr_mode:2; //0
		unsigned int mbz_3:2; //0
		unsigned int mr_data:14; //0
	} f;
	unsigned int v;
} DMCR_T;
#define DMCRrv (*((regval)0xb800101c))
#define DMCRdv (0x00000000)
#define RMOD_DMCR(...) rset(DMCR, DMCRrv, __VA_ARGS__)
#define RIZS_DMCR(...) rset(DMCR, 0, __VA_ARGS__)
#define RFLD_DMCR(fld) (*((const volatile DMCR_T *)0xb800101c)).f.fld

typedef union {
	struct {
		unsigned int ocp_req:32; //2863311530
	} f;
	unsigned int v;
} GIAR0_T;
#define GIAR0rv (*((regval)0xb8001020))
#define GIAR0dv (0xaaaaaaaa)
#define RMOD_GIAR0(...) rset(GIAR0, GIAR0rv, __VA_ARGS__)
#define RIZS_GIAR0(...) rset(GIAR0, 0, __VA_ARGS__)
#define RFLD_GIAR0(fld) (*((const volatile GIAR0_T *)0xb8001020)).f.fld

typedef union {
	struct {
		unsigned int lx_req:32; //1431655765
	} f;
	unsigned int v;
} GIAR1_T;
#define GIAR1rv (*((regval)0xb8001024))
#define GIAR1dv (0x55555555)
#define RMOD_GIAR1(...) rset(GIAR1, GIAR1rv, __VA_ARGS__)
#define RIZS_GIAR1(...) rset(GIAR1, 0, __VA_ARGS__)
#define RFLD_GIAR1(fld) (*((const volatile GIAR1_T *)0xb8001024)).f.fld

typedef union {
	struct {
		unsigned int lx0_req:32; //3991793133
	} f;
	unsigned int v;
} LXIAR0_T;
#define LXIAR0rv (*((regval)0xb8001028))
#define LXIAR0dv (0xedededed)
#define RMOD_LXIAR0(...) rset(LXIAR0, LXIAR0rv, __VA_ARGS__)
#define RIZS_LXIAR0(...) rset(LXIAR0, 0, __VA_ARGS__)
#define RFLD_LXIAR0(fld) (*((const volatile LXIAR0_T *)0xb8001028)).f.fld

typedef union {
	struct {
		unsigned int lx1_req:32; //3082270647
	} f;
	unsigned int v;
} LXIAR1_T;
#define LXIAR1rv (*((regval)0xb800102c))
#define LXIAR1dv (0xb7b7b7b7)
#define RMOD_LXIAR1(...) rset(LXIAR1, LXIAR1rv, __VA_ARGS__)
#define RIZS_LXIAR1(...) rset(LXIAR1, 0, __VA_ARGS__)
#define RFLD_LXIAR1(fld) (*((const volatile LXIAR1_T *)0xb800102c)).f.fld

typedef union {
	struct {
		unsigned int lx2_req:32; //3739147998
	} f;
	unsigned int v;
} LXIAR2_T;
#define LXIAR2rv (*((regval)0xb8001030))
#define LXIAR2dv (0xdededede)
#define RMOD_LXIAR2(...) rset(LXIAR2, LXIAR2rv, __VA_ARGS__)
#define RIZS_LXIAR2(...) rset(LXIAR2, 0, __VA_ARGS__)
#define RFLD_LXIAR2(fld) (*((const volatile LXIAR2_T *)0xb8001030)).f.fld

typedef union {
	struct {
		unsigned int lx3_req:32; //2071690107
	} f;
	unsigned int v;
} LXIAR3_T;
#define LXIAR3rv (*((regval)0xb8001034))
#define LXIAR3dv (0x7b7b7b7b)
#define RMOD_LXIAR3(...) rset(LXIAR3, LXIAR3rv, __VA_ARGS__)
#define RIZS_LXIAR3(...) rset(LXIAR3, 0, __VA_ARGS__)
#define RFLD_LXIAR3(fld) (*((const volatile LXIAR3_T *)0xb8001034)).f.fld

typedef union {
	struct {
		unsigned int flush_ocp_cmd:1; //0
		unsigned int dram_cmd_going:1; //1
		unsigned int mbz_0:30; //0
	} f;
	unsigned int v;
} MSRR_T;
#define MSRRrv (*((regval)0xb8001038))
#define MSRRdv (0x40000000)
#define RMOD_MSRR(...) rset(MSRR, MSRRrv, __VA_ARGS__)
#define RIZS_MSRR(...) rset(MSRR, 0, __VA_ARGS__)
#define RFLD_MSRR(fld) (*((const volatile MSRR_T *)0xb8001038)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int dbg_mem_ctrl_sel:4; //0
	} f;
	unsigned int v;
} MCDSR_T;
#define MCDSRrv (*((regval)0xb800103c))
#define MCDSRdv (0x00000000)
#define RMOD_MCDSR(...) rset(MCDSR, MCDSRrv, __VA_ARGS__)
#define RIZS_MCDSR(...) rset(MCDSR, 0, __VA_ARGS__)
#define RFLD_MCDSR(fld) (*((const volatile MCDSR_T *)0xb800103c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:2; //0
		unsigned int pm_mode:2; //0
		unsigned int t_cke:4; //15
		unsigned int mbz_1:2; //0
		unsigned int trsd:10; //1023
		unsigned int mbz_2:2; //0
		unsigned int t_xsref:10; //1023
	} f;
	unsigned int v;
} MPMR0_T;
#define MPMR0rv (*((regval)0xb8001040))
#define MPMR0dv (0x0f3ff3ff)
#define RMOD_MPMR0(...) rset(MPMR0, MPMR0rv, __VA_ARGS__)
#define RIZS_MPMR0(...) rset(MPMR0, 0, __VA_ARGS__)
#define RFLD_MPMR0(fld) (*((const volatile MPMR0_T *)0xb8001040)).f.fld

typedef union {
	struct {
		unsigned int t_xard:4; //15
		unsigned int t_axpd:4; //15
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} MPMR1_T;
#define MPMR1rv (*((regval)0xb8001044))
#define MPMR1dv (0xff000000)
#define RMOD_MPMR1(...) rset(MPMR1, MPMR1rv, __VA_ARGS__)
#define RIZS_MPMR1(...) rset(MPMR1, 0, __VA_ARGS__)
#define RFLD_MPMR1(fld) (*((const volatile MPMR1_T *)0xb8001044)).f.fld

typedef union {
	struct {
		unsigned int dqs0_en_hclk:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqs0_en_tap:5; //0
		unsigned int dqs1_en_hclk:1; //0
		unsigned int mbz_1:2; //0
		unsigned int dqs1_en_tap:5; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} DIDER_T;
#define DIDERrv (*((regval)0xb8001050))
#define DIDERdv (0x00000000)
#define RMOD_DIDER(...) rset(DIDER, DIDERrv, __VA_ARGS__)
#define RIZS_DIDER(...) rset(DIDER, 0, __VA_ARGS__)
#define RFLD_DIDER(fld) (*((const volatile DIDER_T *)0xb8001050)).f.fld

typedef union {
	struct {
		unsigned int ocd_ready:1; //1
		unsigned int mbz_0:31; //0
	} f;
	unsigned int v;
} D2OCR_T;
#define D2OCRrv (*((regval)0xb8001070))
#define D2OCRdv (0x80000000)
#define RMOD_D2OCR(...) rset(D2OCR, D2OCRrv, __VA_ARGS__)
#define RIZS_D2OCR(...) rset(D2OCR, 0, __VA_ARGS__)
#define RFLD_D2OCR(fld) (*((const volatile D2OCR_T *)0xb8001070)).f.fld

typedef union {
	struct {
		unsigned int arb_sel:1; //0
		unsigned int lx_cmd_delay:1; //0
		unsigned int mbz_0:18; //0
		unsigned int addrb29_lock_dis:1; //0
		unsigned int dqs1_delay_h:2; //0
		unsigned int mbz_1:1; //0
		unsigned int dqs1_delay_l:3; //0
		unsigned int dqs0_delay:5; //0
	} f;
	unsigned int v;
} D2ODR0_T;
#define D2ODR0rv (*((regval)0xb8001074))
#define D2ODR0dv (0x00000000)
#define RMOD_D2ODR0(...) rset(D2ODR0, D2ODR0rv, __VA_ARGS__)
#define RIZS_D2ODR0(...) rset(D2ODR0, 0, __VA_ARGS__)
#define RFLD_D2ODR0(fld) (*((const volatile D2ODR0_T *)0xb8001074)).f.fld

typedef union {
	struct {
		unsigned int ocd_dt3:16; //0
		unsigned int ocd_dt2:16; //0
	} f;
	unsigned int v;
} D2ODR1_T;
#define D2ODR1rv (*((regval)0xb8001078))
#define D2ODR1dv (0x00000000)
#define RMOD_D2ODR1(...) rset(D2ODR1, D2ODR1rv, __VA_ARGS__)
#define RIZS_D2ODR1(...) rset(D2ODR1, 0, __VA_ARGS__)
#define RFLD_D2ODR1(fld) (*((const volatile D2ODR1_T *)0xb8001078)).f.fld

typedef union {
	struct {
		unsigned int odt_always_on:1; //0
		unsigned int te_always_on:1; //0
		unsigned int mbz_0:30; //0
	} f;
	unsigned int v;
} D23OSCR_T;
#define D23OSCRrv (*((regval)0xb800107c))
#define D23OSCRdv (0x00000000)
#define RMOD_D23OSCR(...) rset(D23OSCR, D23OSCRrv, __VA_ARGS__)
#define RIZS_D23OSCR(...) rset(D23OSCR, 0, __VA_ARGS__)
#define RFLD_D23OSCR(fld) (*((const volatile D23OSCR_T *)0xb800107c)).f.fld

typedef union {
	struct {
		unsigned int zq_long_tri:1; //0
		unsigned int zq_short_en:1; //0
		unsigned int mbz_0:7; //0
		unsigned int t_zqcs:7; //127
		unsigned int mbz_1:16; //0
	} f;
	unsigned int v;
} D3ZQCCR_T;
#define D3ZQCCRrv (*((regval)0xb8001080))
#define D3ZQCCRdv (0x007f0000)
#define RMOD_D3ZQCCR(...) rset(D3ZQCCR, D3ZQCCRrv, __VA_ARGS__)
#define RIZS_D3ZQCCR(...) rset(D3ZQCCR, 0, __VA_ARGS__)
#define RFLD_D3ZQCCR(fld) (*((const volatile D3ZQCCR_T *)0xb8001080)).f.fld

typedef union {
	struct {
		unsigned int zq_pad_force_on:1; //0
		unsigned int mbz_0:2; //0
		unsigned int zctrl_clk_sel:1; //1
		unsigned int trim_mode:4; //15
		unsigned int mbz_1:24; //0
	} f;
	unsigned int v;
} DDZQPR_T;
#define DDZQPRrv (*((regval)0xb8001090))
#define DDZQPRdv (0x1f000000)
#define RMOD_DDZQPR(...) rset(DDZQPR, DDZQPRrv, __VA_ARGS__)
#define RIZS_DDZQPR(...) rset(DDZQPR, 0, __VA_ARGS__)
#define RFLD_DDZQPR(fld) (*((const volatile DDZQPR_T *)0xb8001090)).f.fld

typedef union {
	struct {
		unsigned int zctrl_start:1; //0
		unsigned int mbz_0:17; //0
		unsigned int zprog:14; //0
	} f;
	unsigned int v;
} DDZQPCR_T;
#define DDZQPCRrv (*((regval)0xb8001094))
#define DDZQPCRdv (0x00000000)
#define RMOD_DDZQPCR(...) rset(DDZQPCR, DDZQPCRrv, __VA_ARGS__)
#define RIZS_DDZQPCR(...) rset(DDZQPCR, 0, __VA_ARGS__)
#define RFLD_DDZQPCR(fld) (*((const volatile DDZQPCR_T *)0xb8001094)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int zctrl_status:31; //0
	} f;
	unsigned int v;
} DDZQPSR_T;
#define DDZQPSRrv (*((regval)0xb8001098))
#define DDZQPSRdv (0x00000000)
#define RMOD_DDZQPSR(...) rset(DDZQPSR, DDZQPSRrv, __VA_ARGS__)
#define RIZS_DDZQPSR(...) rset(DDZQPSR, 0, __VA_ARGS__)
#define RFLD_DDZQPSR(fld) (*((const volatile DDZQPSR_T *)0xb8001098)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int err_flag:4; //0
	} f;
	unsigned int v;
} MCESR0_T;
#define MCESR0rv (*((regval)0xb80010a0))
#define MCESR0dv (0x00000000)
#define RMOD_MCESR0(...) rset(MCESR0, MCESR0rv, __VA_ARGS__)
#define RIZS_MCESR0(...) rset(MCESR0, 0, __VA_ARGS__)
#define RFLD_MCESR0(fld) (*((const volatile MCESR0_T *)0xb80010a0)).f.fld

typedef union {
	struct {
		unsigned int err1_dqr_flag:16; //0
		unsigned int err1_dqf_flag:16; //0
	} f;
	unsigned int v;
} MCESR1_T;
#define MCESR1rv (*((regval)0xb80010a4))
#define MCESR1dv (0x00000000)
#define RMOD_MCESR1(...) rset(MCESR1, MCESR1rv, __VA_ARGS__)
#define RIZS_MCESR1(...) rset(MCESR1, 0, __VA_ARGS__)
#define RFLD_MCESR1(fld) (*((const volatile MCESR1_T *)0xb80010a4)).f.fld

typedef union {
	struct {
		unsigned int err2_dqr_flag:16; //0
		unsigned int err2_dqf_flag:16; //0
	} f;
	unsigned int v;
} MCESR2_T;
#define MCESR2rv (*((regval)0xb80010a8))
#define MCESR2dv (0x00000000)
#define RMOD_MCESR2(...) rset(MCESR2, MCESR2rv, __VA_ARGS__)
#define RIZS_MCESR2(...) rset(MCESR2, 0, __VA_ARGS__)
#define RFLD_MCESR2(fld) (*((const volatile MCESR2_T *)0xb80010a8)).f.fld

typedef union {
	struct {
		unsigned int mcv_year:8; //18
		unsigned int mcv_date:16; //1297
		unsigned int mcv_hour:8; //24
	} f;
	unsigned int v;
} MCVR_T;
#define MCVRrv (*((regval)0xb80010f8))
#define MCVRdv (0x12051118)
#define RMOD_MCVR(...) rset(MCVR, MCVRrv, __VA_ARGS__)
#define RIZS_MCVR(...) rset(MCVR, 0, __VA_ARGS__)
#define RFLD_MCVR(fld) (*((const volatile MCVR_T *)0xb80010f8)).f.fld

typedef union {
	struct {
		unsigned int soc_plat_num:16; //25190
		unsigned int soc_plat_cut_num:4; //0
		unsigned int mbz_0:12; //0
	} f;
	unsigned int v;
} SOCPNR_T;
#define SOCPNRrv (*((regval)0xb80010fc))
#define SOCPNRdv (0x62660000)
#define RMOD_SOCPNR(...) rset(SOCPNR, SOCPNRrv, __VA_ARGS__)
#define RIZS_SOCPNR(...) rset(SOCPNR, 0, __VA_ARGS__)
#define RFLD_SOCPNR(fld) (*((const volatile SOCPNR_T *)0xb80010fc)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int addr:23; //0
		unsigned int mbz_1:7; //0
		unsigned int enunmap:1; //0
	} f;
	unsigned int v;
} UMSAR0_T;
#define UMSAR0rv (*((regval)0xb8001300))
#define UMSAR0dv (0x00000000)
#define RMOD_UMSAR0(...) rset(UMSAR0, UMSAR0rv, __VA_ARGS__)
#define RIZS_UMSAR0(...) rset(UMSAR0, 0, __VA_ARGS__)
#define RFLD_UMSAR0(fld) (*((const volatile UMSAR0_T *)0xb8001300)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} UMSSR0_T;
#define UMSSR0rv (*((regval)0xb8001304))
#define UMSSR0dv (0x00000000)
#define RMOD_UMSSR0(...) rset(UMSSR0, UMSSR0rv, __VA_ARGS__)
#define RIZS_UMSSR0(...) rset(UMSSR0, 0, __VA_ARGS__)
#define RFLD_UMSSR0(fld) (*((const volatile UMSSR0_T *)0xb8001304)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int addr:23; //0
		unsigned int mbz_1:7; //0
		unsigned int enunmap:1; //0
	} f;
	unsigned int v;
} UMSAR1_T;
#define UMSAR1rv (*((regval)0xb8001310))
#define UMSAR1dv (0x00000000)
#define RMOD_UMSAR1(...) rset(UMSAR1, UMSAR1rv, __VA_ARGS__)
#define RIZS_UMSAR1(...) rset(UMSAR1, 0, __VA_ARGS__)
#define RFLD_UMSAR1(fld) (*((const volatile UMSAR1_T *)0xb8001310)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} UMSSR1_T;
#define UMSSR1rv (*((regval)0xb8001314))
#define UMSSR1dv (0x00000000)
#define RMOD_UMSSR1(...) rset(UMSSR1, UMSSR1rv, __VA_ARGS__)
#define RIZS_UMSSR1(...) rset(UMSSR1, 0, __VA_ARGS__)
#define RFLD_UMSSR1(fld) (*((const volatile UMSSR1_T *)0xb8001314)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int addr:23; //0
		unsigned int mbz_1:7; //0
		unsigned int enunmap:1; //0
	} f;
	unsigned int v;
} UMSAR2_T;
#define UMSAR2rv (*((regval)0xb8001320))
#define UMSAR2dv (0x00000000)
#define RMOD_UMSAR2(...) rset(UMSAR2, UMSAR2rv, __VA_ARGS__)
#define RIZS_UMSAR2(...) rset(UMSAR2, 0, __VA_ARGS__)
#define RFLD_UMSAR2(fld) (*((const volatile UMSAR2_T *)0xb8001320)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} UMSSR2_T;
#define UMSSR2rv (*((regval)0xb8001324))
#define UMSSR2dv (0x00000000)
#define RMOD_UMSSR2(...) rset(UMSSR2, UMSSR2rv, __VA_ARGS__)
#define RIZS_UMSSR2(...) rset(UMSSR2, 0, __VA_ARGS__)
#define RFLD_UMSSR2(fld) (*((const volatile UMSSR2_T *)0xb8001324)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int addr:23; //0
		unsigned int mbz_1:7; //0
		unsigned int enunmap:1; //0
	} f;
	unsigned int v;
} UMSAR3_T;
#define UMSAR3rv (*((regval)0xb8001330))
#define UMSAR3dv (0x00000000)
#define RMOD_UMSAR3(...) rset(UMSAR3, UMSAR3rv, __VA_ARGS__)
#define RIZS_UMSAR3(...) rset(UMSAR3, 0, __VA_ARGS__)
#define RFLD_UMSAR3(fld) (*((const volatile UMSAR3_T *)0xb8001330)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} UMSSR3_T;
#define UMSSR3rv (*((regval)0xb8001334))
#define UMSSR3dv (0x00000000)
#define RMOD_UMSSR3(...) rset(UMSSR3, UMSSR3rv, __VA_ARGS__)
#define RIZS_UMSSR3(...) rset(UMSSR3, 0, __VA_ARGS__)
#define RFLD_UMSSR3(fld) (*((const volatile UMSSR3_T *)0xb8001334)).f.fld

/*-----------------------------------------------------
 Extraced from file_CPU0_DDR_Auto_Calbrt.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int ac_mode:1; //1
		unsigned int mbz_0:1; //0
		unsigned int mbz_1:9; //0
		unsigned int dqs0_group_tap:5; //0
		unsigned int mbz_2:3; //0
		unsigned int dqs1_group_tap:5; //0
		unsigned int mbz_3:2; //0
		unsigned int ac_dyn_bptr_clr_en:1; //0
		unsigned int ac_bptr_clear:1; //0
		unsigned int ac_debug_sel:4; //0
	} f;
	unsigned int v;
} DACCR_T;
#define DACCRrv (*((regval)0xb8001500))
#define DACCRdv (0x80000000)
#define RMOD_DACCR(...) rset(DACCR, DACCRrv, __VA_ARGS__)
#define RIZS_DACCR(...) rset(DACCR, 0, __VA_ARGS__)
#define RFLD_DACCR(fld) (*((const volatile DACCR_T *)0xb8001500)).f.fld

typedef union {
	struct {
		unsigned int ac_silen_period_en:1; //0
		unsigned int must_be_zero:10; //0
		unsigned int ac_silen_trig:1; //0
		unsigned int ac_silen_period_unit:4; //0
		unsigned int ac_silen_period:8; //0
		unsigned int ac_silen_len:8; //0
	} f;
	unsigned int v;
} DACSPCR_T;
#define DACSPCRrv (*((regval)0xb8001504))
#define DACSPCRdv (0x00000000)
#define RMOD_DACSPCR(...) rset(DACSPCR, DACSPCRrv, __VA_ARGS__)
#define RIZS_DACSPCR(...) rset(DACSPCR, 0, __VA_ARGS__)
#define RFLD_DACSPCR(fld) (*((const volatile DACSPCR_T *)0xb8001504)).f.fld

typedef union {
	struct {
		unsigned int ac_silen_addr:32; //0
	} f;
	unsigned int v;
} DACSPAR_T;
#define DACSPARrv (*((regval)0xb8001508))
#define DACSPARdv (0x00000000)
#define RMOD_DACSPAR(...) rset(DACSPAR, DACSPARrv, __VA_ARGS__)
#define RIZS_DACSPAR(...) rset(DACSPAR, 0, __VA_ARGS__)
#define RFLD_DACSPAR(fld) (*((const volatile DACSPAR_T *)0xb8001508)).f.fld

typedef union {
	struct {
		unsigned int ac_sps_dq15r:1; //0
		unsigned int ac_sps_dq14r:1; //0
		unsigned int ac_sps_dq13r:1; //0
		unsigned int ac_sps_dq12r:1; //0
		unsigned int ac_sps_dq11r:1; //0
		unsigned int ac_sps_dq10r:1; //0
		unsigned int ac_sps_dq9r:1; //0
		unsigned int ac_sps_dq8r:1; //0
		unsigned int ac_sps_dq7r:1; //0
		unsigned int ac_sps_dq6r:1; //0
		unsigned int ac_sps_dq5r:1; //0
		unsigned int ac_sps_dq4r:1; //0
		unsigned int ac_sps_dq3r:1; //0
		unsigned int ac_sps_dq2r:1; //0
		unsigned int ac_sps_dq1r:1; //0
		unsigned int ac_sps_dq0r:1; //0
		unsigned int ac_sps_dq15f:1; //0
		unsigned int ac_sps_dq14f:1; //0
		unsigned int ac_sps_dq13f:1; //0
		unsigned int ac_sps_dq12f:1; //0
		unsigned int ac_sps_dq11f:1; //0
		unsigned int ac_sps_dq10f:1; //0
		unsigned int ac_sps_dq9f:1; //0
		unsigned int ac_sps_dq8f:1; //0
		unsigned int ac_sps_dq7f:1; //0
		unsigned int ac_sps_dq6f:1; //0
		unsigned int ac_sps_dq5f:1; //0
		unsigned int ac_sps_dq4f:1; //0
		unsigned int ac_sps_dq3f:1; //0
		unsigned int ac_sps_dq2f:1; //0
		unsigned int ac_sps_dq1f:1; //0
		unsigned int ac_sps_dq0f:1; //0
	} f;
	unsigned int v;
} DACSPSR_T;
#define DACSPSRrv (*((regval)0xb800150c))
#define DACSPSRdv (0x00000000)
#define RMOD_DACSPSR(...) rset(DACSPSR, DACSPSRrv, __VA_ARGS__)
#define RIZS_DACSPSR(...) rset(DACSPSR, 0, __VA_ARGS__)
#define RFLD_DACSPSR(fld) (*((const volatile DACSPSR_T *)0xb800150c)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ0_IDX_RR_T;
#define DACDQ0_IDX_RRrv (*((regval)0xb8001510))
#define DACDQ0_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ0_IDX_RR(...) rset(DACDQ0_IDX_RR, DACDQ0_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ0_IDX_RR(...) rset(DACDQ0_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ0_IDX_RR(fld) (*((const volatile DACDQ0_IDX_RR_T *)0xb8001510)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ1_IDX_RR_T;
#define DACDQ1_IDX_RRrv (*((regval)0xb8001514))
#define DACDQ1_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ1_IDX_RR(...) rset(DACDQ1_IDX_RR, DACDQ1_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ1_IDX_RR(...) rset(DACDQ1_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ1_IDX_RR(fld) (*((const volatile DACDQ1_IDX_RR_T *)0xb8001514)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ2_IDX_RR_T;
#define DACDQ2_IDX_RRrv (*((regval)0xb8001518))
#define DACDQ2_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ2_IDX_RR(...) rset(DACDQ2_IDX_RR, DACDQ2_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ2_IDX_RR(...) rset(DACDQ2_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ2_IDX_RR(fld) (*((const volatile DACDQ2_IDX_RR_T *)0xb8001518)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ3_IDX_RR_T;
#define DACDQ3_IDX_RRrv (*((regval)0xb800151c))
#define DACDQ3_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ3_IDX_RR(...) rset(DACDQ3_IDX_RR, DACDQ3_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ3_IDX_RR(...) rset(DACDQ3_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ3_IDX_RR(fld) (*((const volatile DACDQ3_IDX_RR_T *)0xb800151c)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ4_IDX_RR_T;
#define DACDQ4_IDX_RRrv (*((regval)0xb8001520))
#define DACDQ4_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ4_IDX_RR(...) rset(DACDQ4_IDX_RR, DACDQ4_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ4_IDX_RR(...) rset(DACDQ4_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ4_IDX_RR(fld) (*((const volatile DACDQ4_IDX_RR_T *)0xb8001520)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ5_IDX_RR_T;
#define DACDQ5_IDX_RRrv (*((regval)0xb8001524))
#define DACDQ5_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ5_IDX_RR(...) rset(DACDQ5_IDX_RR, DACDQ5_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ5_IDX_RR(...) rset(DACDQ5_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ5_IDX_RR(fld) (*((const volatile DACDQ5_IDX_RR_T *)0xb8001524)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ6_IDX_RR_T;
#define DACDQ6_IDX_RRrv (*((regval)0xb8001528))
#define DACDQ6_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ6_IDX_RR(...) rset(DACDQ6_IDX_RR, DACDQ6_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ6_IDX_RR(...) rset(DACDQ6_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ6_IDX_RR(fld) (*((const volatile DACDQ6_IDX_RR_T *)0xb8001528)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ7_IDX_RR_T;
#define DACDQ7_IDX_RRrv (*((regval)0xb800152c))
#define DACDQ7_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ7_IDX_RR(...) rset(DACDQ7_IDX_RR, DACDQ7_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ7_IDX_RR(...) rset(DACDQ7_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ7_IDX_RR(fld) (*((const volatile DACDQ7_IDX_RR_T *)0xb800152c)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ8_IDX_RR_T;
#define DACDQ8_IDX_RRrv (*((regval)0xb8001530))
#define DACDQ8_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ8_IDX_RR(...) rset(DACDQ8_IDX_RR, DACDQ8_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ8_IDX_RR(...) rset(DACDQ8_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ8_IDX_RR(fld) (*((const volatile DACDQ8_IDX_RR_T *)0xb8001530)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ9_IDX_RR_T;
#define DACDQ9_IDX_RRrv (*((regval)0xb8001534))
#define DACDQ9_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ9_IDX_RR(...) rset(DACDQ9_IDX_RR, DACDQ9_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ9_IDX_RR(...) rset(DACDQ9_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ9_IDX_RR(fld) (*((const volatile DACDQ9_IDX_RR_T *)0xb8001534)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ10_IDX_RR_T;
#define DACDQ10_IDX_RRrv (*((regval)0xb8001538))
#define DACDQ10_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ10_IDX_RR(...) rset(DACDQ10_IDX_RR, DACDQ10_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ10_IDX_RR(...) rset(DACDQ10_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ10_IDX_RR(fld) (*((const volatile DACDQ10_IDX_RR_T *)0xb8001538)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ11_IDX_RR_T;
#define DACDQ11_IDX_RRrv (*((regval)0xb800153c))
#define DACDQ11_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ11_IDX_RR(...) rset(DACDQ11_IDX_RR, DACDQ11_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ11_IDX_RR(...) rset(DACDQ11_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ11_IDX_RR(fld) (*((const volatile DACDQ11_IDX_RR_T *)0xb800153c)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ12_IDX_RR_T;
#define DACDQ12_IDX_RRrv (*((regval)0xb8001540))
#define DACDQ12_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ12_IDX_RR(...) rset(DACDQ12_IDX_RR, DACDQ12_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ12_IDX_RR(...) rset(DACDQ12_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ12_IDX_RR(fld) (*((const volatile DACDQ12_IDX_RR_T *)0xb8001540)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ13_IDX_RR_T;
#define DACDQ13_IDX_RRrv (*((regval)0xb8001544))
#define DACDQ13_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ13_IDX_RR(...) rset(DACDQ13_IDX_RR, DACDQ13_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ13_IDX_RR(...) rset(DACDQ13_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ13_IDX_RR(fld) (*((const volatile DACDQ13_IDX_RR_T *)0xb8001544)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ14_IDX_RR_T;
#define DACDQ14_IDX_RRrv (*((regval)0xb8001548))
#define DACDQ14_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ14_IDX_RR(...) rset(DACDQ14_IDX_RR, DACDQ14_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ14_IDX_RR(...) rset(DACDQ14_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ14_IDX_RR(fld) (*((const volatile DACDQ14_IDX_RR_T *)0xb8001548)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ15_IDX_RR_T;
#define DACDQ15_IDX_RRrv (*((regval)0xb800154c))
#define DACDQ15_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ15_IDX_RR(...) rset(DACDQ15_IDX_RR, DACDQ15_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ15_IDX_RR(...) rset(DACDQ15_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ15_IDX_RR(fld) (*((const volatile DACDQ15_IDX_RR_T *)0xb800154c)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ0_IDX_FR_T;
#define DACDQ0_IDX_FRrv (*((regval)0xb8001550))
#define DACDQ0_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ0_IDX_FR(...) rset(DACDQ0_IDX_FR, DACDQ0_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ0_IDX_FR(...) rset(DACDQ0_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ0_IDX_FR(fld) (*((const volatile DACDQ0_IDX_FR_T *)0xb8001550)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ1_IDX_FR_T;
#define DACDQ1_IDX_FRrv (*((regval)0xb8001554))
#define DACDQ1_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ1_IDX_FR(...) rset(DACDQ1_IDX_FR, DACDQ1_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ1_IDX_FR(...) rset(DACDQ1_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ1_IDX_FR(fld) (*((const volatile DACDQ1_IDX_FR_T *)0xb8001554)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ2_IDX_FR_T;
#define DACDQ2_IDX_FRrv (*((regval)0xb8001558))
#define DACDQ2_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ2_IDX_FR(...) rset(DACDQ2_IDX_FR, DACDQ2_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ2_IDX_FR(...) rset(DACDQ2_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ2_IDX_FR(fld) (*((const volatile DACDQ2_IDX_FR_T *)0xb8001558)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ3_IDX_FR_T;
#define DACDQ3_IDX_FRrv (*((regval)0xb800155c))
#define DACDQ3_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ3_IDX_FR(...) rset(DACDQ3_IDX_FR, DACDQ3_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ3_IDX_FR(...) rset(DACDQ3_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ3_IDX_FR(fld) (*((const volatile DACDQ3_IDX_FR_T *)0xb800155c)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ4_IDX_FR_T;
#define DACDQ4_IDX_FRrv (*((regval)0xb8001560))
#define DACDQ4_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ4_IDX_FR(...) rset(DACDQ4_IDX_FR, DACDQ4_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ4_IDX_FR(...) rset(DACDQ4_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ4_IDX_FR(fld) (*((const volatile DACDQ4_IDX_FR_T *)0xb8001560)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ5_IDX_FR_T;
#define DACDQ5_IDX_FRrv (*((regval)0xb8001564))
#define DACDQ5_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ5_IDX_FR(...) rset(DACDQ5_IDX_FR, DACDQ5_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ5_IDX_FR(...) rset(DACDQ5_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ5_IDX_FR(fld) (*((const volatile DACDQ5_IDX_FR_T *)0xb8001564)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ6_IDX_FR_T;
#define DACDQ6_IDX_FRrv (*((regval)0xb8001568))
#define DACDQ6_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ6_IDX_FR(...) rset(DACDQ6_IDX_FR, DACDQ6_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ6_IDX_FR(...) rset(DACDQ6_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ6_IDX_FR(fld) (*((const volatile DACDQ6_IDX_FR_T *)0xb8001568)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ7_IDX_FR_T;
#define DACDQ7_IDX_FRrv (*((regval)0xb800156c))
#define DACDQ7_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ7_IDX_FR(...) rset(DACDQ7_IDX_FR, DACDQ7_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ7_IDX_FR(...) rset(DACDQ7_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ7_IDX_FR(fld) (*((const volatile DACDQ7_IDX_FR_T *)0xb800156c)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ8_IDX_FR_T;
#define DACDQ8_IDX_FRrv (*((regval)0xb8001570))
#define DACDQ8_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ8_IDX_FR(...) rset(DACDQ8_IDX_FR, DACDQ8_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ8_IDX_FR(...) rset(DACDQ8_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ8_IDX_FR(fld) (*((const volatile DACDQ8_IDX_FR_T *)0xb8001570)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ9_IDX_FR_T;
#define DACDQ9_IDX_FRrv (*((regval)0xb8001574))
#define DACDQ9_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ9_IDX_FR(...) rset(DACDQ9_IDX_FR, DACDQ9_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ9_IDX_FR(...) rset(DACDQ9_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ9_IDX_FR(fld) (*((const volatile DACDQ9_IDX_FR_T *)0xb8001574)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ10_IDX_FR_T;
#define DACDQ10_IDX_FRrv (*((regval)0xb8001578))
#define DACDQ10_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ10_IDX_FR(...) rset(DACDQ10_IDX_FR, DACDQ10_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ10_IDX_FR(...) rset(DACDQ10_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ10_IDX_FR(fld) (*((const volatile DACDQ10_IDX_FR_T *)0xb8001578)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ11_IDX_FR_T;
#define DACDQ11_IDX_FRrv (*((regval)0xb800157c))
#define DACDQ11_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ11_IDX_FR(...) rset(DACDQ11_IDX_FR, DACDQ11_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ11_IDX_FR(...) rset(DACDQ11_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ11_IDX_FR(fld) (*((const volatile DACDQ11_IDX_FR_T *)0xb800157c)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ12_IDX_FR_T;
#define DACDQ12_IDX_FRrv (*((regval)0xb8001580))
#define DACDQ12_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ12_IDX_FR(...) rset(DACDQ12_IDX_FR, DACDQ12_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ12_IDX_FR(...) rset(DACDQ12_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ12_IDX_FR(fld) (*((const volatile DACDQ12_IDX_FR_T *)0xb8001580)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ13_IDX_FR_T;
#define DACDQ13_IDX_FRrv (*((regval)0xb8001584))
#define DACDQ13_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ13_IDX_FR(...) rset(DACDQ13_IDX_FR, DACDQ13_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ13_IDX_FR(...) rset(DACDQ13_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ13_IDX_FR(fld) (*((const volatile DACDQ13_IDX_FR_T *)0xb8001584)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ14_IDX_FR_T;
#define DACDQ14_IDX_FRrv (*((regval)0xb8001588))
#define DACDQ14_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ14_IDX_FR(...) rset(DACDQ14_IDX_FR, DACDQ14_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ14_IDX_FR(...) rset(DACDQ14_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ14_IDX_FR(fld) (*((const volatile DACDQ14_IDX_FR_T *)0xb8001588)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ15_IDX_FR_T;
#define DACDQ15_IDX_FRrv (*((regval)0xb800158c))
#define DACDQ15_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ15_IDX_FR(...) rset(DACDQ15_IDX_FR, DACDQ15_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ15_IDX_FR(...) rset(DACDQ15_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ15_IDX_FR(fld) (*((const volatile DACDQ15_IDX_FR_T *)0xb800158c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int dqm0_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqm1_phase_shift_90:5; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} DCDQMR_T;
#define DCDQMRrv (*((regval)0xb8001590))
#define DCDQMRdv (0x00000000)
#define RMOD_DCDQMR(...) rset(DCDQMR, DCDQMRrv, __VA_ARGS__)
#define RIZS_DCDQMR(...) rset(DCDQMR, 0, __VA_ARGS__)
#define RFLD_DCDQMR(fld) (*((const volatile DCDQMR_T *)0xb8001590)).f.fld

typedef union {
	struct {
		unsigned int dq7r_fifo_d:4; //0
		unsigned int dq6r_fifo_d:4; //0
		unsigned int dq5r_fifo_d:4; //0
		unsigned int dq4r_fifo_d:4; //0
		unsigned int dq3r_fifo_d:4; //0
		unsigned int dq2r_fifo_d:3; //0
		unsigned int dq1r_fifo_d:5; //0
		unsigned int dq0r_fifo_d:4; //0
	} f;
	unsigned int v;
} DCFDRR0_T;
#define DCFDRR0rv (*((regval)0xb8001594))
#define DCFDRR0dv (0x00000000)
#define RMOD_DCFDRR0(...) rset(DCFDRR0, DCFDRR0rv, __VA_ARGS__)
#define RIZS_DCFDRR0(...) rset(DCFDRR0, 0, __VA_ARGS__)
#define RFLD_DCFDRR0(fld) (*((const volatile DCFDRR0_T *)0xb8001594)).f.fld

typedef union {
	struct {
		unsigned int dq15r_fifo_d:4; //0
		unsigned int dq14r_fifo_d:4; //0
		unsigned int dq13r_fifo_d:4; //0
		unsigned int dq12r_fifo_d:4; //0
		unsigned int dq11r_fifo_d:4; //0
		unsigned int dq10r_fifo_d:3; //0
		unsigned int dq9r_fifo_d:5; //0
		unsigned int dq8r_fifo_d:4; //0
	} f;
	unsigned int v;
} DCFDRR1_T;
#define DCFDRR1rv (*((regval)0xb8001598))
#define DCFDRR1dv (0x00000000)
#define RMOD_DCFDRR1(...) rset(DCFDRR1, DCFDRR1rv, __VA_ARGS__)
#define RIZS_DCFDRR1(...) rset(DCFDRR1, 0, __VA_ARGS__)
#define RFLD_DCFDRR1(fld) (*((const volatile DCFDRR1_T *)0xb8001598)).f.fld

typedef union {
	struct {
		unsigned int dq7f_fifo_d:4; //0
		unsigned int dq6f_fifo_d:4; //0
		unsigned int dq5f_fifo_d:4; //0
		unsigned int dq4f_fifo_d:4; //0
		unsigned int dq3f_fifo_d:4; //0
		unsigned int dq2f_fifo_d:3; //0
		unsigned int dq1f_fifo_d:5; //0
		unsigned int dq0f_fifo_d:4; //0
	} f;
	unsigned int v;
} DCFDFR0_T;
#define DCFDFR0rv (*((regval)0xb800159c))
#define DCFDFR0dv (0x00000000)
#define RMOD_DCFDFR0(...) rset(DCFDFR0, DCFDFR0rv, __VA_ARGS__)
#define RIZS_DCFDFR0(...) rset(DCFDFR0, 0, __VA_ARGS__)
#define RFLD_DCFDFR0(fld) (*((const volatile DCFDFR0_T *)0xb800159c)).f.fld

typedef union {
	struct {
		unsigned int dq15f_fifo_d:4; //0
		unsigned int dq14f_fifo_d:4; //0
		unsigned int dq13f_fifo_d:4; //0
		unsigned int dq12f_fifo_d:4; //0
		unsigned int dq11f_fifo_d:4; //0
		unsigned int dq10f_fifo_d:3; //0
		unsigned int dq9f_fifo_d:5; //0
		unsigned int dq8f_fifo_d:4; //0
	} f;
	unsigned int v;
} DCFDFR1_T;
#define DCFDFR1rv (*((regval)0xb80015a0))
#define DCFDFR1dv (0x00000000)
#define RMOD_DCFDFR1(...) rset(DCFDFR1, DCFDFR1rv, __VA_ARGS__)
#define RIZS_DCFDFR1(...) rset(DCFDFR1, 0, __VA_ARGS__)
#define RFLD_DCFDFR1(fld) (*((const volatile DCFDFR1_T *)0xb80015a0)).f.fld

typedef union {
	struct {
		unsigned int fifo_d_debug_en:1; //0
		unsigned int mbz_0:30; //0
		unsigned int fifo_d_sel:1; //0
	} f;
	unsigned int v;
} DCFDDCR_T;
#define DCFDDCRrv (*((regval)0xb80015a4))
#define DCFDDCRdv (0x00000000)
#define RMOD_DCFDDCR(...) rset(DCFDDCR, DCFDDCRrv, __VA_ARGS__)
#define RIZS_DCFDDCR(...) rset(DCFDDCR, 0, __VA_ARGS__)
#define RFLD_DCFDDCR(fld) (*((const volatile DCFDDCR_T *)0xb80015a4)).f.fld

/*-----------------------------------------------------
 Extraced from file_CPU0_Memory_Sharing.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int offset:11; //0
		unsigned int mbz_1:20; //0
	} f;
	unsigned int v;
} VPE0_DRAM_ZONE0_OFFSET_T;
#define VPE0_DRAM_ZONE0_OFFSETrv (*((regval)0xb8001700))
#define VPE0_DRAM_ZONE0_OFFSETdv (0x00000000)
#define RMOD_VPE0_DRAM_ZONE0_OFFSET(...) rset(VPE0_DRAM_ZONE0_OFFSET, VPE0_DRAM_ZONE0_OFFSETrv, __VA_ARGS__)
#define RIZS_VPE0_DRAM_ZONE0_OFFSET(...) rset(VPE0_DRAM_ZONE0_OFFSET, 0, __VA_ARGS__)
#define RFLD_VPE0_DRAM_ZONE0_OFFSET(fld) (*((const volatile VPE0_DRAM_ZONE0_OFFSET_T *)0xb8001700)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int max:8; //0
		unsigned int addr_offset:20; //1048575
	} f;
	unsigned int v;
} VPE0_DRAM_ZONE0_MAX_ADDR_T;
#define VPE0_DRAM_ZONE0_MAX_ADDRrv (*((regval)0xb8001704))
#define VPE0_DRAM_ZONE0_MAX_ADDRdv (0x000fffff)
#define RMOD_VPE0_DRAM_ZONE0_MAX_ADDR(...) rset(VPE0_DRAM_ZONE0_MAX_ADDR, VPE0_DRAM_ZONE0_MAX_ADDRrv, __VA_ARGS__)
#define RIZS_VPE0_DRAM_ZONE0_MAX_ADDR(...) rset(VPE0_DRAM_ZONE0_MAX_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE0_DRAM_ZONE0_MAX_ADDR(fld) (*((const volatile VPE0_DRAM_ZONE0_MAX_ADDR_T *)0xb8001704)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int offset:11; //0
		unsigned int mbz_1:20; //0
	} f;
	unsigned int v;
} VPE0_DRAM_ZONE1_OFFSET_T;
#define VPE0_DRAM_ZONE1_OFFSETrv (*((regval)0xb8001710))
#define VPE0_DRAM_ZONE1_OFFSETdv (0x00000000)
#define RMOD_VPE0_DRAM_ZONE1_OFFSET(...) rset(VPE0_DRAM_ZONE1_OFFSET, VPE0_DRAM_ZONE1_OFFSETrv, __VA_ARGS__)
#define RIZS_VPE0_DRAM_ZONE1_OFFSET(...) rset(VPE0_DRAM_ZONE1_OFFSET, 0, __VA_ARGS__)
#define RFLD_VPE0_DRAM_ZONE1_OFFSET(fld) (*((const volatile VPE0_DRAM_ZONE1_OFFSET_T *)0xb8001710)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int max:6; //0
		unsigned int addr_offset:20; //1048575
	} f;
	unsigned int v;
} VPE0_DRAM_ZONE1_MAX_ADDR_T;
#define VPE0_DRAM_ZONE1_MAX_ADDRrv (*((regval)0xb8001714))
#define VPE0_DRAM_ZONE1_MAX_ADDRdv (0x000fffff)
#define RMOD_VPE0_DRAM_ZONE1_MAX_ADDR(...) rset(VPE0_DRAM_ZONE1_MAX_ADDR, VPE0_DRAM_ZONE1_MAX_ADDRrv, __VA_ARGS__)
#define RIZS_VPE0_DRAM_ZONE1_MAX_ADDR(...) rset(VPE0_DRAM_ZONE1_MAX_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE0_DRAM_ZONE1_MAX_ADDR(fld) (*((const volatile VPE0_DRAM_ZONE1_MAX_ADDR_T *)0xb8001714)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int offset:11; //0
		unsigned int mbz_1:20; //0
	} f;
	unsigned int v;
} VPE0_DRAM_ZONE2_OFFSET_T;
#define VPE0_DRAM_ZONE2_OFFSETrv (*((regval)0xb8001720))
#define VPE0_DRAM_ZONE2_OFFSETdv (0x00000000)
#define RMOD_VPE0_DRAM_ZONE2_OFFSET(...) rset(VPE0_DRAM_ZONE2_OFFSET, VPE0_DRAM_ZONE2_OFFSETrv, __VA_ARGS__)
#define RIZS_VPE0_DRAM_ZONE2_OFFSET(...) rset(VPE0_DRAM_ZONE2_OFFSET, 0, __VA_ARGS__)
#define RFLD_VPE0_DRAM_ZONE2_OFFSET(fld) (*((const volatile VPE0_DRAM_ZONE2_OFFSET_T *)0xb8001720)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int max:11; //0
		unsigned int addr_offset:20; //1048575
	} f;
	unsigned int v;
} VPE0_DRAM_ZONE2_MAX_ADDR_T;
#define VPE0_DRAM_ZONE2_MAX_ADDRrv (*((regval)0xb8001724))
#define VPE0_DRAM_ZONE2_MAX_ADDRdv (0x000fffff)
#define RMOD_VPE0_DRAM_ZONE2_MAX_ADDR(...) rset(VPE0_DRAM_ZONE2_MAX_ADDR, VPE0_DRAM_ZONE2_MAX_ADDRrv, __VA_ARGS__)
#define RIZS_VPE0_DRAM_ZONE2_MAX_ADDR(...) rset(VPE0_DRAM_ZONE2_MAX_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE0_DRAM_ZONE2_MAX_ADDR(fld) (*((const volatile VPE0_DRAM_ZONE2_MAX_ADDR_T *)0xb8001724)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:30; //0
		unsigned int p0_ila_valid:1; //0
		unsigned int p0_arce:1; //0
	} f;
	unsigned int v;
} VPE0_RANGE_CTRL_T;
#define VPE0_RANGE_CTRLrv (*((regval)0xb8001730))
#define VPE0_RANGE_CTRLdv (0x00000000)
#define RMOD_VPE0_RANGE_CTRL(...) rset(VPE0_RANGE_CTRL, VPE0_RANGE_CTRLrv, __VA_ARGS__)
#define RIZS_VPE0_RANGE_CTRL(...) rset(VPE0_RANGE_CTRL, 0, __VA_ARGS__)
#define RFLD_VPE0_RANGE_CTRL(fld) (*((const volatile VPE0_RANGE_CTRL_T *)0xb8001730)).f.fld

typedef union {
	struct {
		unsigned int p0_ila:32; //0
	} f;
	unsigned int v;
} VPE0_ILGL_LOGIC_ADDR_T;
#define VPE0_ILGL_LOGIC_ADDRrv (*((regval)0xb8001734))
#define VPE0_ILGL_LOGIC_ADDRdv (0x00000000)
#define RMOD_VPE0_ILGL_LOGIC_ADDR(...) rset(VPE0_ILGL_LOGIC_ADDR, VPE0_ILGL_LOGIC_ADDRrv, __VA_ARGS__)
#define RIZS_VPE0_ILGL_LOGIC_ADDR(...) rset(VPE0_ILGL_LOGIC_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE0_ILGL_LOGIC_ADDR(fld) (*((const volatile VPE0_ILGL_LOGIC_ADDR_T *)0xb8001734)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int offset:11; //0
		unsigned int mbz_1:20; //0
	} f;
	unsigned int v;
} VPE1_DRAM_ZONE0_OFFSET_T;
#define VPE1_DRAM_ZONE0_OFFSETrv (*((regval)0xb8001740))
#define VPE1_DRAM_ZONE0_OFFSETdv (0x00000000)
#define RMOD_VPE1_DRAM_ZONE0_OFFSET(...) rset(VPE1_DRAM_ZONE0_OFFSET, VPE1_DRAM_ZONE0_OFFSETrv, __VA_ARGS__)
#define RIZS_VPE1_DRAM_ZONE0_OFFSET(...) rset(VPE1_DRAM_ZONE0_OFFSET, 0, __VA_ARGS__)
#define RFLD_VPE1_DRAM_ZONE0_OFFSET(fld) (*((const volatile VPE1_DRAM_ZONE0_OFFSET_T *)0xb8001740)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int max:8; //0
		unsigned int addr_offset:20; //1048575
	} f;
	unsigned int v;
} VPE1_DRAM_ZONE0_MAX_ADDR_T;
#define VPE1_DRAM_ZONE0_MAX_ADDRrv (*((regval)0xb8001744))
#define VPE1_DRAM_ZONE0_MAX_ADDRdv (0x000fffff)
#define RMOD_VPE1_DRAM_ZONE0_MAX_ADDR(...) rset(VPE1_DRAM_ZONE0_MAX_ADDR, VPE1_DRAM_ZONE0_MAX_ADDRrv, __VA_ARGS__)
#define RIZS_VPE1_DRAM_ZONE0_MAX_ADDR(...) rset(VPE1_DRAM_ZONE0_MAX_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE1_DRAM_ZONE0_MAX_ADDR(fld) (*((const volatile VPE1_DRAM_ZONE0_MAX_ADDR_T *)0xb8001744)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int offset:11; //0
		unsigned int mbz_1:20; //0
	} f;
	unsigned int v;
} VPE1_DRAM_ZONE1_OFFSET_T;
#define VPE1_DRAM_ZONE1_OFFSETrv (*((regval)0xb8001750))
#define VPE1_DRAM_ZONE1_OFFSETdv (0x00000000)
#define RMOD_VPE1_DRAM_ZONE1_OFFSET(...) rset(VPE1_DRAM_ZONE1_OFFSET, VPE1_DRAM_ZONE1_OFFSETrv, __VA_ARGS__)
#define RIZS_VPE1_DRAM_ZONE1_OFFSET(...) rset(VPE1_DRAM_ZONE1_OFFSET, 0, __VA_ARGS__)
#define RFLD_VPE1_DRAM_ZONE1_OFFSET(fld) (*((const volatile VPE1_DRAM_ZONE1_OFFSET_T *)0xb8001750)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int max:6; //0
		unsigned int addr_offset:20; //1048575
	} f;
	unsigned int v;
} VPE1_DRAM_ZONE1_MAX_ADDR_T;
#define VPE1_DRAM_ZONE1_MAX_ADDRrv (*((regval)0xb8001754))
#define VPE1_DRAM_ZONE1_MAX_ADDRdv (0x000fffff)
#define RMOD_VPE1_DRAM_ZONE1_MAX_ADDR(...) rset(VPE1_DRAM_ZONE1_MAX_ADDR, VPE1_DRAM_ZONE1_MAX_ADDRrv, __VA_ARGS__)
#define RIZS_VPE1_DRAM_ZONE1_MAX_ADDR(...) rset(VPE1_DRAM_ZONE1_MAX_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE1_DRAM_ZONE1_MAX_ADDR(fld) (*((const volatile VPE1_DRAM_ZONE1_MAX_ADDR_T *)0xb8001754)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int offset:11; //0
		unsigned int mbz_1:20; //0
	} f;
	unsigned int v;
} VPE1_DRAM_ZONE2_OFFSET_T;
#define VPE1_DRAM_ZONE2_OFFSETrv (*((regval)0xb8001760))
#define VPE1_DRAM_ZONE2_OFFSETdv (0x00000000)
#define RMOD_VPE1_DRAM_ZONE2_OFFSET(...) rset(VPE1_DRAM_ZONE2_OFFSET, VPE1_DRAM_ZONE2_OFFSETrv, __VA_ARGS__)
#define RIZS_VPE1_DRAM_ZONE2_OFFSET(...) rset(VPE1_DRAM_ZONE2_OFFSET, 0, __VA_ARGS__)
#define RFLD_VPE1_DRAM_ZONE2_OFFSET(fld) (*((const volatile VPE1_DRAM_ZONE2_OFFSET_T *)0xb8001760)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int max:11; //0
		unsigned int addr_offset:20; //1048575
	} f;
	unsigned int v;
} VPE1_DRAM_ZONE2_MAX_ADDR_T;
#define VPE1_DRAM_ZONE2_MAX_ADDRrv (*((regval)0xb8001764))
#define VPE1_DRAM_ZONE2_MAX_ADDRdv (0x000fffff)
#define RMOD_VPE1_DRAM_ZONE2_MAX_ADDR(...) rset(VPE1_DRAM_ZONE2_MAX_ADDR, VPE1_DRAM_ZONE2_MAX_ADDRrv, __VA_ARGS__)
#define RIZS_VPE1_DRAM_ZONE2_MAX_ADDR(...) rset(VPE1_DRAM_ZONE2_MAX_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE1_DRAM_ZONE2_MAX_ADDR(fld) (*((const volatile VPE1_DRAM_ZONE2_MAX_ADDR_T *)0xb8001764)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:30; //0
		unsigned int p1_ila_valid:1; //0
		unsigned int p1_arce:1; //0
	} f;
	unsigned int v;
} VPE1_RANGE_CTRL_T;
#define VPE1_RANGE_CTRLrv (*((regval)0xb8001770))
#define VPE1_RANGE_CTRLdv (0x00000000)
#define RMOD_VPE1_RANGE_CTRL(...) rset(VPE1_RANGE_CTRL, VPE1_RANGE_CTRLrv, __VA_ARGS__)
#define RIZS_VPE1_RANGE_CTRL(...) rset(VPE1_RANGE_CTRL, 0, __VA_ARGS__)
#define RFLD_VPE1_RANGE_CTRL(fld) (*((const volatile VPE1_RANGE_CTRL_T *)0xb8001770)).f.fld

typedef union {
	struct {
		unsigned int p1_ila:32; //0
	} f;
	unsigned int v;
} VPE1_ILGL_LOGIC_ADDR_T;
#define VPE1_ILGL_LOGIC_ADDRrv (*((regval)0xb8001774))
#define VPE1_ILGL_LOGIC_ADDRdv (0x00000000)
#define RMOD_VPE1_ILGL_LOGIC_ADDR(...) rset(VPE1_ILGL_LOGIC_ADDR, VPE1_ILGL_LOGIC_ADDRrv, __VA_ARGS__)
#define RIZS_VPE1_ILGL_LOGIC_ADDR(...) rset(VPE1_ILGL_LOGIC_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE1_ILGL_LOGIC_ADDR(fld) (*((const volatile VPE1_ILGL_LOGIC_ADDR_T *)0xb8001774)).f.fld

/*-----------------------------------------------------
 Extraced from file_CPU0_SPI_Flash.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int spi_clk_div:3; //7
		unsigned int rbo:1; //1
		unsigned int wbo:1; //1
		unsigned int spi_tcs:5; //31
		unsigned int must_be_zero:22; //0
	} f;
	unsigned int v;
} SFCR_T;
#define SFCRrv (*((regval)0xb8001200))
#define SFCRdv (0xffc00000)
#define RMOD_SFCR(...) rset(SFCR, SFCRrv, __VA_ARGS__)
#define RIZS_SFCR(...) rset(SFCR, 0, __VA_ARGS__)
#define RFLD_SFCR(fld) (*((const volatile SFCR_T *)0xb8001200)).f.fld

typedef union {
	struct {
		unsigned int sfcmd:8; //3
		unsigned int sfsize:3; //7
		unsigned int rdopt:1; //0
		unsigned int cmd_io:2; //0
		unsigned int addr_io:2; //0
		unsigned int mbz_0:3; //0
		unsigned int data_io:2; //0
		unsigned int hold_till_sfdr2:1; //0
		unsigned int mbz_1:10; //0
	} f;
	unsigned int v;
} SFCR2_T;
#define SFCR2rv (*((regval)0xb8001204))
#define SFCR2dv (0x03e00000)
#define RMOD_SFCR2(...) rset(SFCR2, SFCR2rv, __VA_ARGS__)
#define RIZS_SFCR2(...) rset(SFCR2, 0, __VA_ARGS__)
#define RFLD_SFCR2(fld) (*((const volatile SFCR2_T *)0xb8001204)).f.fld

typedef union {
	struct {
		unsigned int spi_csb0:1; //1
		unsigned int spi_csb1:1; //1
		unsigned int len:2; //3
		unsigned int spi_rdy:1; //1
		unsigned int io_width:2; //0
		unsigned int chip_sel:1; //0
		unsigned int cmd_byte:8; //0
		unsigned int mbz_0:16; //0
	} f;
	unsigned int v;
} SFCSR_T;
#define SFCSRrv (*((regval)0xb8001208))
#define SFCSRdv (0xf8000000)
#define RMOD_SFCSR(...) rset(SFCSR, SFCSRrv, __VA_ARGS__)
#define RIZS_SFCSR(...) rset(SFCSR, 0, __VA_ARGS__)
#define RFLD_SFCSR(fld) (*((const volatile SFCSR_T *)0xb8001208)).f.fld

typedef union {
	struct {
		unsigned int data3:8; //0
		unsigned int data2:8; //0
		unsigned int data1:8; //0
		unsigned int data0:8; //0
	} f;
	unsigned int v;
} SFDR_T;
#define SFDRrv (*((regval)0xb800120c))
#define SFDRdv (0x00000000)
#define RMOD_SFDR(...) rset(SFDR, SFDRrv, __VA_ARGS__)
#define RIZS_SFDR(...) rset(SFDR, 0, __VA_ARGS__)
#define RFLD_SFDR(fld) (*((const volatile SFDR_T *)0xb800120c)).f.fld

typedef union {
	struct {
		unsigned int data3:8; //0
		unsigned int data2:8; //0
		unsigned int data1:8; //0
		unsigned int data0:8; //0
	} f;
	unsigned int v;
} SFDR2_T;
#define SFDR2rv (*((regval)0xb8001210))
#define SFDR2dv (0x00000000)
#define RMOD_SFDR2(...) rset(SFDR2, SFDR2rv, __VA_ARGS__)
#define RIZS_SFDR2(...) rset(SFDR2, 0, __VA_ARGS__)
#define RFLD_SFDR2(fld) (*((const volatile SFDR2_T *)0xb8001210)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int io3_delay:5; //0
		unsigned int mbz_1:3; //0
		unsigned int io2_delay:5; //0
		unsigned int mbz_2:3; //0
		unsigned int io1_delay:5; //0
		unsigned int mbz_3:3; //0
		unsigned int io0_delay:5; //0
	} f;
	unsigned int v;
} SFRDR_T;
#define SFRDRrv (*((regval)0xb8001218))
#define SFRDRdv (0x00000000)
#define RMOD_SFRDR(...) rset(SFRDR, SFRDRrv, __VA_ARGS__)
#define RIZS_SFRDR(...) rset(SFRDR, 0, __VA_ARGS__)
#define RFLD_SFRDR(fld) (*((const volatile SFRDR_T *)0xb8001218)).f.fld

/*-----------------------------------------------------
 Extraced from file_CPU0_SRAM.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int sa:24; //0
		unsigned int mbz_0:7; //0
		unsigned int ensram:1; //0
	} f;
	unsigned int v;
} VPE0_SRAM_SEG0_ADDR_T;
#define VPE0_SRAM_SEG0_ADDRrv (*((regval)0xb8004000))
#define VPE0_SRAM_SEG0_ADDRdv (0x00000000)
#define RMOD_VPE0_SRAM_SEG0_ADDR(...) rset(VPE0_SRAM_SEG0_ADDR, VPE0_SRAM_SEG0_ADDRrv, __VA_ARGS__)
#define RIZS_VPE0_SRAM_SEG0_ADDR(...) rset(VPE0_SRAM_SEG0_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE0_SRAM_SEG0_ADDR(fld) (*((const volatile VPE0_SRAM_SEG0_ADDR_T *)0xb8004000)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //9
	} f;
	unsigned int v;
} VPE0_SRAM_SEG0_SIZE_T;
#define VPE0_SRAM_SEG0_SIZErv (*((regval)0xb8004004))
#define VPE0_SRAM_SEG0_SIZEdv (0x00000009)
#define RMOD_VPE0_SRAM_SEG0_SIZE(...) rset(VPE0_SRAM_SEG0_SIZE, VPE0_SRAM_SEG0_SIZErv, __VA_ARGS__)
#define RIZS_VPE0_SRAM_SEG0_SIZE(...) rset(VPE0_SRAM_SEG0_SIZE, 0, __VA_ARGS__)
#define RFLD_VPE0_SRAM_SEG0_SIZE(fld) (*((const volatile VPE0_SRAM_SEG0_SIZE_T *)0xb8004004)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:12; //0
		unsigned int base:12; //768
		unsigned int mbz_1:8; //0
	} f;
	unsigned int v;
} VPE0_SRAM_SEG0_BASE_T;
#define VPE0_SRAM_SEG0_BASErv (*((regval)0xb8004008))
#define VPE0_SRAM_SEG0_BASEdv (0x00030000)
#define RMOD_VPE0_SRAM_SEG0_BASE(...) rset(VPE0_SRAM_SEG0_BASE, VPE0_SRAM_SEG0_BASErv, __VA_ARGS__)
#define RIZS_VPE0_SRAM_SEG0_BASE(...) rset(VPE0_SRAM_SEG0_BASE, 0, __VA_ARGS__)
#define RFLD_VPE0_SRAM_SEG0_BASE(fld) (*((const volatile VPE0_SRAM_SEG0_BASE_T *)0xb8004008)).f.fld

typedef union {
	struct {
		unsigned int sa:24; //0
		unsigned int mbz_0:7; //0
		unsigned int ensram:1; //0
	} f;
	unsigned int v;
} VPE0_SRAM_SEG1_ADDR_T;
#define VPE0_SRAM_SEG1_ADDRrv (*((regval)0xb8004010))
#define VPE0_SRAM_SEG1_ADDRdv (0x00000000)
#define RMOD_VPE0_SRAM_SEG1_ADDR(...) rset(VPE0_SRAM_SEG1_ADDR, VPE0_SRAM_SEG1_ADDRrv, __VA_ARGS__)
#define RIZS_VPE0_SRAM_SEG1_ADDR(...) rset(VPE0_SRAM_SEG1_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE0_SRAM_SEG1_ADDR(fld) (*((const volatile VPE0_SRAM_SEG1_ADDR_T *)0xb8004010)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} VPE0_SRAM_SEG1_SIZE_T;
#define VPE0_SRAM_SEG1_SIZErv (*((regval)0xb8004014))
#define VPE0_SRAM_SEG1_SIZEdv (0x00000000)
#define RMOD_VPE0_SRAM_SEG1_SIZE(...) rset(VPE0_SRAM_SEG1_SIZE, VPE0_SRAM_SEG1_SIZErv, __VA_ARGS__)
#define RIZS_VPE0_SRAM_SEG1_SIZE(...) rset(VPE0_SRAM_SEG1_SIZE, 0, __VA_ARGS__)
#define RFLD_VPE0_SRAM_SEG1_SIZE(fld) (*((const volatile VPE0_SRAM_SEG1_SIZE_T *)0xb8004014)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:12; //0
		unsigned int base:12; //0
		unsigned int mbz_1:8; //0
	} f;
	unsigned int v;
} VPE0_SRAM_SEG1_BASE_T;
#define VPE0_SRAM_SEG1_BASErv (*((regval)0xb8004018))
#define VPE0_SRAM_SEG1_BASEdv (0x00000000)
#define RMOD_VPE0_SRAM_SEG1_BASE(...) rset(VPE0_SRAM_SEG1_BASE, VPE0_SRAM_SEG1_BASErv, __VA_ARGS__)
#define RIZS_VPE0_SRAM_SEG1_BASE(...) rset(VPE0_SRAM_SEG1_BASE, 0, __VA_ARGS__)
#define RFLD_VPE0_SRAM_SEG1_BASE(fld) (*((const volatile VPE0_SRAM_SEG1_BASE_T *)0xb8004018)).f.fld

typedef union {
	struct {
		unsigned int sa:24; //0
		unsigned int mbz_0:7; //0
		unsigned int ensram:1; //0
	} f;
	unsigned int v;
} VPE0_SRAM_SEG2_ADDR_T;
#define VPE0_SRAM_SEG2_ADDRrv (*((regval)0xb8004020))
#define VPE0_SRAM_SEG2_ADDRdv (0x00000000)
#define RMOD_VPE0_SRAM_SEG2_ADDR(...) rset(VPE0_SRAM_SEG2_ADDR, VPE0_SRAM_SEG2_ADDRrv, __VA_ARGS__)
#define RIZS_VPE0_SRAM_SEG2_ADDR(...) rset(VPE0_SRAM_SEG2_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE0_SRAM_SEG2_ADDR(fld) (*((const volatile VPE0_SRAM_SEG2_ADDR_T *)0xb8004020)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} VPE0_SRAM_SEG2_SIZE_T;
#define VPE0_SRAM_SEG2_SIZErv (*((regval)0xb8004024))
#define VPE0_SRAM_SEG2_SIZEdv (0x00000000)
#define RMOD_VPE0_SRAM_SEG2_SIZE(...) rset(VPE0_SRAM_SEG2_SIZE, VPE0_SRAM_SEG2_SIZErv, __VA_ARGS__)
#define RIZS_VPE0_SRAM_SEG2_SIZE(...) rset(VPE0_SRAM_SEG2_SIZE, 0, __VA_ARGS__)
#define RFLD_VPE0_SRAM_SEG2_SIZE(fld) (*((const volatile VPE0_SRAM_SEG2_SIZE_T *)0xb8004024)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:12; //0
		unsigned int base:12; //0
		unsigned int mbz_1:8; //0
	} f;
	unsigned int v;
} VPE0_SRAM_SEG2_BASE_T;
#define VPE0_SRAM_SEG2_BASErv (*((regval)0xb8004028))
#define VPE0_SRAM_SEG2_BASEdv (0x00000000)
#define RMOD_VPE0_SRAM_SEG2_BASE(...) rset(VPE0_SRAM_SEG2_BASE, VPE0_SRAM_SEG2_BASErv, __VA_ARGS__)
#define RIZS_VPE0_SRAM_SEG2_BASE(...) rset(VPE0_SRAM_SEG2_BASE, 0, __VA_ARGS__)
#define RFLD_VPE0_SRAM_SEG2_BASE(fld) (*((const volatile VPE0_SRAM_SEG2_BASE_T *)0xb8004028)).f.fld

typedef union {
	struct {
		unsigned int sa:24; //0
		unsigned int mbz_0:7; //0
		unsigned int ensram:1; //0
	} f;
	unsigned int v;
} VPE0_SRAM_SEG3_ADDR_T;
#define VPE0_SRAM_SEG3_ADDRrv (*((regval)0xb8004030))
#define VPE0_SRAM_SEG3_ADDRdv (0x00000000)
#define RMOD_VPE0_SRAM_SEG3_ADDR(...) rset(VPE0_SRAM_SEG3_ADDR, VPE0_SRAM_SEG3_ADDRrv, __VA_ARGS__)
#define RIZS_VPE0_SRAM_SEG3_ADDR(...) rset(VPE0_SRAM_SEG3_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE0_SRAM_SEG3_ADDR(fld) (*((const volatile VPE0_SRAM_SEG3_ADDR_T *)0xb8004030)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} VPE0_SRAM_SEG3_SIZE_T;
#define VPE0_SRAM_SEG3_SIZErv (*((regval)0xb8004034))
#define VPE0_SRAM_SEG3_SIZEdv (0x00000000)
#define RMOD_VPE0_SRAM_SEG3_SIZE(...) rset(VPE0_SRAM_SEG3_SIZE, VPE0_SRAM_SEG3_SIZErv, __VA_ARGS__)
#define RIZS_VPE0_SRAM_SEG3_SIZE(...) rset(VPE0_SRAM_SEG3_SIZE, 0, __VA_ARGS__)
#define RFLD_VPE0_SRAM_SEG3_SIZE(fld) (*((const volatile VPE0_SRAM_SEG3_SIZE_T *)0xb8004034)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:12; //0
		unsigned int base:12; //0
		unsigned int mbz_1:8; //0
	} f;
	unsigned int v;
} VPE0_SRAM_SEG3_BASE_T;
#define VPE0_SRAM_SEG3_BASErv (*((regval)0xb8004038))
#define VPE0_SRAM_SEG3_BASEdv (0x00000000)
#define RMOD_VPE0_SRAM_SEG3_BASE(...) rset(VPE0_SRAM_SEG3_BASE, VPE0_SRAM_SEG3_BASErv, __VA_ARGS__)
#define RIZS_VPE0_SRAM_SEG3_BASE(...) rset(VPE0_SRAM_SEG3_BASE, 0, __VA_ARGS__)
#define RFLD_VPE0_SRAM_SEG3_BASE(fld) (*((const volatile VPE0_SRAM_SEG3_BASE_T *)0xb8004038)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int offset:11; //0
		unsigned int mbz_1:20; //0
	} f;
	unsigned int v;
} VPE0_DRAM_OFFSET_INFO_ZONE0_T;
#define VPE0_DRAM_OFFSET_INFO_ZONE0rv (*((regval)0xb8004090))
#define VPE0_DRAM_OFFSET_INFO_ZONE0dv (0x00000000)
#define RMOD_VPE0_DRAM_OFFSET_INFO_ZONE0(...) rset(VPE0_DRAM_OFFSET_INFO_ZONE0, VPE0_DRAM_OFFSET_INFO_ZONE0rv, __VA_ARGS__)
#define RIZS_VPE0_DRAM_OFFSET_INFO_ZONE0(...) rset(VPE0_DRAM_OFFSET_INFO_ZONE0, 0, __VA_ARGS__)
#define RFLD_VPE0_DRAM_OFFSET_INFO_ZONE0(fld) (*((const volatile VPE0_DRAM_OFFSET_INFO_ZONE0_T *)0xb8004090)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int offset:11; //0
		unsigned int mbz_1:20; //0
	} f;
	unsigned int v;
} VPE0_DRAM_OFFSET_INFO_ZONE2_T;
#define VPE0_DRAM_OFFSET_INFO_ZONE2rv (*((regval)0xb8004094))
#define VPE0_DRAM_OFFSET_INFO_ZONE2dv (0x00000000)
#define RMOD_VPE0_DRAM_OFFSET_INFO_ZONE2(...) rset(VPE0_DRAM_OFFSET_INFO_ZONE2, VPE0_DRAM_OFFSET_INFO_ZONE2rv, __VA_ARGS__)
#define RIZS_VPE0_DRAM_OFFSET_INFO_ZONE2(...) rset(VPE0_DRAM_OFFSET_INFO_ZONE2, 0, __VA_ARGS__)
#define RFLD_VPE0_DRAM_OFFSET_INFO_ZONE2(fld) (*((const volatile VPE0_DRAM_OFFSET_INFO_ZONE2_T *)0xb8004094)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int offset:11; //0
		unsigned int mbz_1:20; //0
	} f;
	unsigned int v;
} VPE1_DRAM_OFFSET_INFO_ZONE0_T;
#define VPE1_DRAM_OFFSET_INFO_ZONE0rv (*((regval)0xb80040a0))
#define VPE1_DRAM_OFFSET_INFO_ZONE0dv (0x00000000)
#define RMOD_VPE1_DRAM_OFFSET_INFO_ZONE0(...) rset(VPE1_DRAM_OFFSET_INFO_ZONE0, VPE1_DRAM_OFFSET_INFO_ZONE0rv, __VA_ARGS__)
#define RIZS_VPE1_DRAM_OFFSET_INFO_ZONE0(...) rset(VPE1_DRAM_OFFSET_INFO_ZONE0, 0, __VA_ARGS__)
#define RFLD_VPE1_DRAM_OFFSET_INFO_ZONE0(fld) (*((const volatile VPE1_DRAM_OFFSET_INFO_ZONE0_T *)0xb80040a0)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int offset:11; //0
		unsigned int mbz_1:20; //0
	} f;
	unsigned int v;
} VPE1_DRAM_OFFSET_INFO_ZONE2_T;
#define VPE1_DRAM_OFFSET_INFO_ZONE2rv (*((regval)0xb80040a4))
#define VPE1_DRAM_OFFSET_INFO_ZONE2dv (0x00000000)
#define RMOD_VPE1_DRAM_OFFSET_INFO_ZONE2(...) rset(VPE1_DRAM_OFFSET_INFO_ZONE2, VPE1_DRAM_OFFSET_INFO_ZONE2rv, __VA_ARGS__)
#define RIZS_VPE1_DRAM_OFFSET_INFO_ZONE2(...) rset(VPE1_DRAM_OFFSET_INFO_ZONE2, 0, __VA_ARGS__)
#define RFLD_VPE1_DRAM_OFFSET_INFO_ZONE2(fld) (*((const volatile VPE1_DRAM_OFFSET_INFO_ZONE2_T *)0xb80040a4)).f.fld

/*-----------------------------------------------------
 Extraced from file_OCP0_Timeout_Monitor.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int ocp_tcen:1; //1
		unsigned int ocp_berr:1; //0
		unsigned int ocp_tct:4; //7
		unsigned int mbz_0:26; //0
	} f;
	unsigned int v;
} OCP0_BUS_TO_CTRL_T;
#define OCP0_BUS_TO_CTRLrv (*((regval)0xb8005100))
#define OCP0_BUS_TO_CTRLdv (0x9c000000)
#define RMOD_OCP0_BUS_TO_CTRL(...) rset(OCP0_BUS_TO_CTRL, OCP0_BUS_TO_CTRLrv, __VA_ARGS__)
#define RIZS_OCP0_BUS_TO_CTRL(...) rset(OCP0_BUS_TO_CTRL, 0, __VA_ARGS__)
#define RFLD_OCP0_BUS_TO_CTRL(fld) (*((const volatile OCP0_BUS_TO_CTRL_T *)0xb8005100)).f.fld

typedef union {
	struct {
		unsigned int ocp_ip:1; //0
		unsigned int mbz_0:31; //0
	} f;
	unsigned int v;
} OCP0_BUS_TO_INTR_T;
#define OCP0_BUS_TO_INTRrv (*((regval)0xb8005104))
#define OCP0_BUS_TO_INTRdv (0x00000000)
#define RMOD_OCP0_BUS_TO_INTR(...) rset(OCP0_BUS_TO_INTR, OCP0_BUS_TO_INTRrv, __VA_ARGS__)
#define RIZS_OCP0_BUS_TO_INTR(...) rset(OCP0_BUS_TO_INTR, 0, __VA_ARGS__)
#define RFLD_OCP0_BUS_TO_INTR(fld) (*((const volatile OCP0_BUS_TO_INTR_T *)0xb8005104)).f.fld

typedef union {
	struct {
		unsigned int ocp_addr:32; //0
	} f;
	unsigned int v;
} OCP0_BUS_TO_MONT_ADDR_T;
#define OCP0_BUS_TO_MONT_ADDRrv (*((regval)0xb8005108))
#define OCP0_BUS_TO_MONT_ADDRdv (0x00000000)
#define RMOD_OCP0_BUS_TO_MONT_ADDR(...) rset(OCP0_BUS_TO_MONT_ADDR, OCP0_BUS_TO_MONT_ADDRrv, __VA_ARGS__)
#define RIZS_OCP0_BUS_TO_MONT_ADDR(...) rset(OCP0_BUS_TO_MONT_ADDR, 0, __VA_ARGS__)
#define RFLD_OCP0_BUS_TO_MONT_ADDR(fld) (*((const volatile OCP0_BUS_TO_MONT_ADDR_T *)0xb8005108)).f.fld

/*-----------------------------------------------------
 Extraced from file_CPU1_MCR.xml
-----------------------------------------------------*/
/*-----------------------------------------------------
 Extraced from file_CPU1_Memory_Global.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int addr:23; //0
		unsigned int mbz_1:7; //0
		unsigned int enunmap:1; //0
	} f;
	unsigned int v;
} VPE1_UNMAP_MEM_SEG0_ADDR_T;
#define VPE1_UNMAP_MEM_SEG0_ADDRrv (*((regval)0xb8001340))
#define VPE1_UNMAP_MEM_SEG0_ADDRdv (0x00000000)
#define RMOD_VPE1_UNMAP_MEM_SEG0_ADDR(...) rset(VPE1_UNMAP_MEM_SEG0_ADDR, VPE1_UNMAP_MEM_SEG0_ADDRrv, __VA_ARGS__)
#define RIZS_VPE1_UNMAP_MEM_SEG0_ADDR(...) rset(VPE1_UNMAP_MEM_SEG0_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE1_UNMAP_MEM_SEG0_ADDR(fld) (*((const volatile VPE1_UNMAP_MEM_SEG0_ADDR_T *)0xb8001340)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} VPE1_UNMAP_MEM_SEG0_SIZE_T;
#define VPE1_UNMAP_MEM_SEG0_SIZErv (*((regval)0xb8001344))
#define VPE1_UNMAP_MEM_SEG0_SIZEdv (0x00000000)
#define RMOD_VPE1_UNMAP_MEM_SEG0_SIZE(...) rset(VPE1_UNMAP_MEM_SEG0_SIZE, VPE1_UNMAP_MEM_SEG0_SIZErv, __VA_ARGS__)
#define RIZS_VPE1_UNMAP_MEM_SEG0_SIZE(...) rset(VPE1_UNMAP_MEM_SEG0_SIZE, 0, __VA_ARGS__)
#define RFLD_VPE1_UNMAP_MEM_SEG0_SIZE(fld) (*((const volatile VPE1_UNMAP_MEM_SEG0_SIZE_T *)0xb8001344)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int addr:23; //0
		unsigned int mbz_1:7; //0
		unsigned int enunmap:1; //0
	} f;
	unsigned int v;
} VPE1_UNMAP_MEM_SEG1_ADDR_T;
#define VPE1_UNMAP_MEM_SEG1_ADDRrv (*((regval)0xb8001350))
#define VPE1_UNMAP_MEM_SEG1_ADDRdv (0x00000000)
#define RMOD_VPE1_UNMAP_MEM_SEG1_ADDR(...) rset(VPE1_UNMAP_MEM_SEG1_ADDR, VPE1_UNMAP_MEM_SEG1_ADDRrv, __VA_ARGS__)
#define RIZS_VPE1_UNMAP_MEM_SEG1_ADDR(...) rset(VPE1_UNMAP_MEM_SEG1_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE1_UNMAP_MEM_SEG1_ADDR(fld) (*((const volatile VPE1_UNMAP_MEM_SEG1_ADDR_T *)0xb8001350)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} VPE1_UNMAP_MEM_SEG1_SIZE_T;
#define VPE1_UNMAP_MEM_SEG1_SIZErv (*((regval)0xb8001354))
#define VPE1_UNMAP_MEM_SEG1_SIZEdv (0x00000000)
#define RMOD_VPE1_UNMAP_MEM_SEG1_SIZE(...) rset(VPE1_UNMAP_MEM_SEG1_SIZE, VPE1_UNMAP_MEM_SEG1_SIZErv, __VA_ARGS__)
#define RIZS_VPE1_UNMAP_MEM_SEG1_SIZE(...) rset(VPE1_UNMAP_MEM_SEG1_SIZE, 0, __VA_ARGS__)
#define RFLD_VPE1_UNMAP_MEM_SEG1_SIZE(fld) (*((const volatile VPE1_UNMAP_MEM_SEG1_SIZE_T *)0xb8001354)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int addr:23; //0
		unsigned int mbz_1:7; //0
		unsigned int enunmap:1; //0
	} f;
	unsigned int v;
} VPE1_UNMAP_MEM_SEG2_ADDR_T;
#define VPE1_UNMAP_MEM_SEG2_ADDRrv (*((regval)0xb8001360))
#define VPE1_UNMAP_MEM_SEG2_ADDRdv (0x00000000)
#define RMOD_VPE1_UNMAP_MEM_SEG2_ADDR(...) rset(VPE1_UNMAP_MEM_SEG2_ADDR, VPE1_UNMAP_MEM_SEG2_ADDRrv, __VA_ARGS__)
#define RIZS_VPE1_UNMAP_MEM_SEG2_ADDR(...) rset(VPE1_UNMAP_MEM_SEG2_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE1_UNMAP_MEM_SEG2_ADDR(fld) (*((const volatile VPE1_UNMAP_MEM_SEG2_ADDR_T *)0xb8001360)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} VPE1_UNMAP_MEM_SEG2_SIZE_T;
#define VPE1_UNMAP_MEM_SEG2_SIZErv (*((regval)0xb8001364))
#define VPE1_UNMAP_MEM_SEG2_SIZEdv (0x00000000)
#define RMOD_VPE1_UNMAP_MEM_SEG2_SIZE(...) rset(VPE1_UNMAP_MEM_SEG2_SIZE, VPE1_UNMAP_MEM_SEG2_SIZErv, __VA_ARGS__)
#define RIZS_VPE1_UNMAP_MEM_SEG2_SIZE(...) rset(VPE1_UNMAP_MEM_SEG2_SIZE, 0, __VA_ARGS__)
#define RFLD_VPE1_UNMAP_MEM_SEG2_SIZE(fld) (*((const volatile VPE1_UNMAP_MEM_SEG2_SIZE_T *)0xb8001364)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int addr:23; //0
		unsigned int mbz_1:7; //0
		unsigned int enunmap:1; //0
	} f;
	unsigned int v;
} VPE1_UNMAP_MEM_SEG3_ADDR_T;
#define VPE1_UNMAP_MEM_SEG3_ADDRrv (*((regval)0xb8001370))
#define VPE1_UNMAP_MEM_SEG3_ADDRdv (0x00000000)
#define RMOD_VPE1_UNMAP_MEM_SEG3_ADDR(...) rset(VPE1_UNMAP_MEM_SEG3_ADDR, VPE1_UNMAP_MEM_SEG3_ADDRrv, __VA_ARGS__)
#define RIZS_VPE1_UNMAP_MEM_SEG3_ADDR(...) rset(VPE1_UNMAP_MEM_SEG3_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE1_UNMAP_MEM_SEG3_ADDR(fld) (*((const volatile VPE1_UNMAP_MEM_SEG3_ADDR_T *)0xb8001370)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} VPE1_UNMAP_MEM_SEG3_SIZE_T;
#define VPE1_UNMAP_MEM_SEG3_SIZErv (*((regval)0xb8001374))
#define VPE1_UNMAP_MEM_SEG3_SIZEdv (0x00000000)
#define RMOD_VPE1_UNMAP_MEM_SEG3_SIZE(...) rset(VPE1_UNMAP_MEM_SEG3_SIZE, VPE1_UNMAP_MEM_SEG3_SIZErv, __VA_ARGS__)
#define RIZS_VPE1_UNMAP_MEM_SEG3_SIZE(...) rset(VPE1_UNMAP_MEM_SEG3_SIZE, 0, __VA_ARGS__)
#define RFLD_VPE1_UNMAP_MEM_SEG3_SIZE(fld) (*((const volatile VPE1_UNMAP_MEM_SEG3_SIZE_T *)0xb8001374)).f.fld

/*-----------------------------------------------------
 Extraced from file_CPU1_SRAM.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int sa:24; //10469376
		unsigned int mbz_0:7; //0
		unsigned int ensram:1; //1
	} f;
	unsigned int v;
} VPE1_SRAM_SEG0_ADDR_T;
#define VPE1_SRAM_SEG0_ADDRrv (*((regval)0xb8004040))
#define VPE1_SRAM_SEG0_ADDRdv (0x9fc00001)
#define RMOD_VPE1_SRAM_SEG0_ADDR(...) rset(VPE1_SRAM_SEG0_ADDR, VPE1_SRAM_SEG0_ADDRrv, __VA_ARGS__)
#define RIZS_VPE1_SRAM_SEG0_ADDR(...) rset(VPE1_SRAM_SEG0_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE1_SRAM_SEG0_ADDR(fld) (*((const volatile VPE1_SRAM_SEG0_ADDR_T *)0xb8004040)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //9
	} f;
	unsigned int v;
} VPE1_SRAM_SEG0_SIZE_T;
#define VPE1_SRAM_SEG0_SIZErv (*((regval)0xb8004044))
#define VPE1_SRAM_SEG0_SIZEdv (0x00000009)
#define RMOD_VPE1_SRAM_SEG0_SIZE(...) rset(VPE1_SRAM_SEG0_SIZE, VPE1_SRAM_SEG0_SIZErv, __VA_ARGS__)
#define RIZS_VPE1_SRAM_SEG0_SIZE(...) rset(VPE1_SRAM_SEG0_SIZE, 0, __VA_ARGS__)
#define RFLD_VPE1_SRAM_SEG0_SIZE(fld) (*((const volatile VPE1_SRAM_SEG0_SIZE_T *)0xb8004044)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:12; //0
		unsigned int base:12; //768
		unsigned int mbz_1:8; //0
	} f;
	unsigned int v;
} VPE1_SRAM_SEG0_BASE_T;
#define VPE1_SRAM_SEG0_BASErv (*((regval)0xb8004048))
#define VPE1_SRAM_SEG0_BASEdv (0x00030000)
#define RMOD_VPE1_SRAM_SEG0_BASE(...) rset(VPE1_SRAM_SEG0_BASE, VPE1_SRAM_SEG0_BASErv, __VA_ARGS__)
#define RIZS_VPE1_SRAM_SEG0_BASE(...) rset(VPE1_SRAM_SEG0_BASE, 0, __VA_ARGS__)
#define RFLD_VPE1_SRAM_SEG0_BASE(fld) (*((const volatile VPE1_SRAM_SEG0_BASE_T *)0xb8004048)).f.fld

typedef union {
	struct {
		unsigned int sa:24; //0
		unsigned int mbz_0:7; //0
		unsigned int ensram:1; //0
	} f;
	unsigned int v;
} VPE1_SRAM_SEG1_ADDR_T;
#define VPE1_SRAM_SEG1_ADDRrv (*((regval)0xb8004050))
#define VPE1_SRAM_SEG1_ADDRdv (0x00000000)
#define RMOD_VPE1_SRAM_SEG1_ADDR(...) rset(VPE1_SRAM_SEG1_ADDR, VPE1_SRAM_SEG1_ADDRrv, __VA_ARGS__)
#define RIZS_VPE1_SRAM_SEG1_ADDR(...) rset(VPE1_SRAM_SEG1_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE1_SRAM_SEG1_ADDR(fld) (*((const volatile VPE1_SRAM_SEG1_ADDR_T *)0xb8004050)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} VPE1_SRAM_SEG1_SIZE_T;
#define VPE1_SRAM_SEG1_SIZErv (*((regval)0xb8004054))
#define VPE1_SRAM_SEG1_SIZEdv (0x00000000)
#define RMOD_VPE1_SRAM_SEG1_SIZE(...) rset(VPE1_SRAM_SEG1_SIZE, VPE1_SRAM_SEG1_SIZErv, __VA_ARGS__)
#define RIZS_VPE1_SRAM_SEG1_SIZE(...) rset(VPE1_SRAM_SEG1_SIZE, 0, __VA_ARGS__)
#define RFLD_VPE1_SRAM_SEG1_SIZE(fld) (*((const volatile VPE1_SRAM_SEG1_SIZE_T *)0xb8004054)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:12; //0
		unsigned int base:12; //0
		unsigned int mbz_1:8; //0
	} f;
	unsigned int v;
} VPE1_SRAM_SEG1_BASE_T;
#define VPE1_SRAM_SEG1_BASErv (*((regval)0xb8004058))
#define VPE1_SRAM_SEG1_BASEdv (0x00000000)
#define RMOD_VPE1_SRAM_SEG1_BASE(...) rset(VPE1_SRAM_SEG1_BASE, VPE1_SRAM_SEG1_BASErv, __VA_ARGS__)
#define RIZS_VPE1_SRAM_SEG1_BASE(...) rset(VPE1_SRAM_SEG1_BASE, 0, __VA_ARGS__)
#define RFLD_VPE1_SRAM_SEG1_BASE(fld) (*((const volatile VPE1_SRAM_SEG1_BASE_T *)0xb8004058)).f.fld

typedef union {
	struct {
		unsigned int sa:24; //0
		unsigned int mbz_0:7; //0
		unsigned int ensram:1; //0
	} f;
	unsigned int v;
} VPE1_SRAM_SEG2_ADDR_T;
#define VPE1_SRAM_SEG2_ADDRrv (*((regval)0xb8004060))
#define VPE1_SRAM_SEG2_ADDRdv (0x00000000)
#define RMOD_VPE1_SRAM_SEG2_ADDR(...) rset(VPE1_SRAM_SEG2_ADDR, VPE1_SRAM_SEG2_ADDRrv, __VA_ARGS__)
#define RIZS_VPE1_SRAM_SEG2_ADDR(...) rset(VPE1_SRAM_SEG2_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE1_SRAM_SEG2_ADDR(fld) (*((const volatile VPE1_SRAM_SEG2_ADDR_T *)0xb8004060)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} VPE1_SRAM_SEG2_SIZE_T;
#define VPE1_SRAM_SEG2_SIZErv (*((regval)0xb8004064))
#define VPE1_SRAM_SEG2_SIZEdv (0x00000000)
#define RMOD_VPE1_SRAM_SEG2_SIZE(...) rset(VPE1_SRAM_SEG2_SIZE, VPE1_SRAM_SEG2_SIZErv, __VA_ARGS__)
#define RIZS_VPE1_SRAM_SEG2_SIZE(...) rset(VPE1_SRAM_SEG2_SIZE, 0, __VA_ARGS__)
#define RFLD_VPE1_SRAM_SEG2_SIZE(fld) (*((const volatile VPE1_SRAM_SEG2_SIZE_T *)0xb8004064)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:12; //0
		unsigned int base:12; //0
		unsigned int mbz_1:8; //0
	} f;
	unsigned int v;
} VPE1_SRAM_SEG2_BASE_T;
#define VPE1_SRAM_SEG2_BASErv (*((regval)0xb8004068))
#define VPE1_SRAM_SEG2_BASEdv (0x00000000)
#define RMOD_VPE1_SRAM_SEG2_BASE(...) rset(VPE1_SRAM_SEG2_BASE, VPE1_SRAM_SEG2_BASErv, __VA_ARGS__)
#define RIZS_VPE1_SRAM_SEG2_BASE(...) rset(VPE1_SRAM_SEG2_BASE, 0, __VA_ARGS__)
#define RFLD_VPE1_SRAM_SEG2_BASE(fld) (*((const volatile VPE1_SRAM_SEG2_BASE_T *)0xb8004068)).f.fld

typedef union {
	struct {
		unsigned int sa:24; //0
		unsigned int mbz_0:7; //0
		unsigned int ensram:1; //0
	} f;
	unsigned int v;
} VPE1_SRAM_SEG3_ADDR_T;
#define VPE1_SRAM_SEG3_ADDRrv (*((regval)0xb8004070))
#define VPE1_SRAM_SEG3_ADDRdv (0x00000000)
#define RMOD_VPE1_SRAM_SEG3_ADDR(...) rset(VPE1_SRAM_SEG3_ADDR, VPE1_SRAM_SEG3_ADDRrv, __VA_ARGS__)
#define RIZS_VPE1_SRAM_SEG3_ADDR(...) rset(VPE1_SRAM_SEG3_ADDR, 0, __VA_ARGS__)
#define RFLD_VPE1_SRAM_SEG3_ADDR(fld) (*((const volatile VPE1_SRAM_SEG3_ADDR_T *)0xb8004070)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} VPE1_SRAM_SEG3_SIZE_T;
#define VPE1_SRAM_SEG3_SIZErv (*((regval)0xb8004074))
#define VPE1_SRAM_SEG3_SIZEdv (0x00000000)
#define RMOD_VPE1_SRAM_SEG3_SIZE(...) rset(VPE1_SRAM_SEG3_SIZE, VPE1_SRAM_SEG3_SIZErv, __VA_ARGS__)
#define RIZS_VPE1_SRAM_SEG3_SIZE(...) rset(VPE1_SRAM_SEG3_SIZE, 0, __VA_ARGS__)
#define RFLD_VPE1_SRAM_SEG3_SIZE(fld) (*((const volatile VPE1_SRAM_SEG3_SIZE_T *)0xb8004074)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:12; //0
		unsigned int base:12; //0
		unsigned int mbz_1:8; //0
	} f;
	unsigned int v;
} VPE1_SRAM_SEG3_BASE_T;
#define VPE1_SRAM_SEG3_BASErv (*((regval)0xb8004078))
#define VPE1_SRAM_SEG3_BASEdv (0x00000000)
#define RMOD_VPE1_SRAM_SEG3_BASE(...) rset(VPE1_SRAM_SEG3_BASE, VPE1_SRAM_SEG3_BASErv, __VA_ARGS__)
#define RIZS_VPE1_SRAM_SEG3_BASE(...) rset(VPE1_SRAM_SEG3_BASE, 0, __VA_ARGS__)
#define RFLD_VPE1_SRAM_SEG3_BASE(fld) (*((const volatile VPE1_SRAM_SEG3_BASE_T *)0xb8004078)).f.fld

/*-----------------------------------------------------
 Extraced from file_OCP1_Timeout_Monitor.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int ocp_tcen:1; //1
		unsigned int ocp_berr:1; //0
		unsigned int ocp_tct:4; //7
		unsigned int mbz_0:26; //0
	} f;
	unsigned int v;
} OCP1_BUS_TO_CTRL_T;
#define OCP1_BUS_TO_CTRLrv (*((regval)0xb8005110))
#define OCP1_BUS_TO_CTRLdv (0x9c000000)
#define RMOD_OCP1_BUS_TO_CTRL(...) rset(OCP1_BUS_TO_CTRL, OCP1_BUS_TO_CTRLrv, __VA_ARGS__)
#define RIZS_OCP1_BUS_TO_CTRL(...) rset(OCP1_BUS_TO_CTRL, 0, __VA_ARGS__)
#define RFLD_OCP1_BUS_TO_CTRL(fld) (*((const volatile OCP1_BUS_TO_CTRL_T *)0xb8005110)).f.fld

typedef union {
	struct {
		unsigned int ocp_ip:1; //0
		unsigned int mbz_0:31; //0
	} f;
	unsigned int v;
} OCP1_BUS_TO_INTR_T;
#define OCP1_BUS_TO_INTRrv (*((regval)0xb8005114))
#define OCP1_BUS_TO_INTRdv (0x00000000)
#define RMOD_OCP1_BUS_TO_INTR(...) rset(OCP1_BUS_TO_INTR, OCP1_BUS_TO_INTRrv, __VA_ARGS__)
#define RIZS_OCP1_BUS_TO_INTR(...) rset(OCP1_BUS_TO_INTR, 0, __VA_ARGS__)
#define RFLD_OCP1_BUS_TO_INTR(fld) (*((const volatile OCP1_BUS_TO_INTR_T *)0xb8005114)).f.fld

typedef union {
	struct {
		unsigned int ocp_addr:32; //0
	} f;
	unsigned int v;
} OCP1_BUS_TO_MONT_ADDR_T;
#define OCP1_BUS_TO_MONT_ADDRrv (*((regval)0xb8005118))
#define OCP1_BUS_TO_MONT_ADDRdv (0x00000000)
#define RMOD_OCP1_BUS_TO_MONT_ADDR(...) rset(OCP1_BUS_TO_MONT_ADDR, OCP1_BUS_TO_MONT_ADDRrv, __VA_ARGS__)
#define RIZS_OCP1_BUS_TO_MONT_ADDR(...) rset(OCP1_BUS_TO_MONT_ADDR, 0, __VA_ARGS__)
#define RFLD_OCP1_BUS_TO_MONT_ADDR(fld) (*((const volatile OCP1_BUS_TO_MONT_ADDR_T *)0xb8005118)).f.fld

/*-----------------------------------------------------
 Extraced from file_ROM.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int romaddr:11; //0
		unsigned int mbz_1:19; //0
		unsigned int enrom:1; //0
	} f;
	unsigned int v;
} ROM_SEG_ADDR_T;
#define ROM_SEG_ADDRrv (*((regval)0xb8004080))
#define ROM_SEG_ADDRdv (0x00000000)
#define RMOD_ROM_SEG_ADDR(...) rset(ROM_SEG_ADDR, ROM_SEG_ADDRrv, __VA_ARGS__)
#define RIZS_ROM_SEG_ADDR(...) rset(ROM_SEG_ADDR, 0, __VA_ARGS__)
#define RFLD_ROM_SEG_ADDR(fld) (*((const volatile ROM_SEG_ADDR_T *)0xb8004080)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} ROM_SEG_SIZE_T;
#define ROM_SEG_SIZErv (*((regval)0xb8004084))
#define ROM_SEG_SIZEdv (0x00000000)
#define RMOD_ROM_SEG_SIZE(...) rset(ROM_SEG_SIZE, ROM_SEG_SIZErv, __VA_ARGS__)
#define RIZS_ROM_SEG_SIZE(...) rset(ROM_SEG_SIZE, 0, __VA_ARGS__)
#define RFLD_ROM_SEG_SIZE(fld) (*((const volatile ROM_SEG_SIZE_T *)0xb8004084)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:27; //0
		unsigned int rom_clk_freq:1; //0
		unsigned int mbz_1:1; //0
		unsigned int rom_d_stable:3; //3
	} f;
	unsigned int v;
} ROM_CTRL_T;
#define ROM_CTRLrv (*((regval)0xb8004088))
#define ROM_CTRLdv (0x00000003)
#define RMOD_ROM_CTRL(...) rset(ROM_CTRL, ROM_CTRLrv, __VA_ARGS__)
#define RIZS_ROM_CTRL(...) rset(ROM_CTRL, 0, __VA_ARGS__)
#define RFLD_ROM_CTRL(fld) (*((const volatile ROM_CTRL_T *)0xb8004088)).f.fld

/*-----------------------------------------------------
 Extraced from file_IPC_ASIC.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:31; //0
		unsigned int cpu0_mutex:1; //0
	} f;
	unsigned int v;
} CPU0_IPC_MUTEX_T;
#define CPU0_IPC_MUTEXrv (*((regval)0xb8141040))
#define CPU0_IPC_MUTEXdv (0x00000000)
#define RMOD_CPU0_IPC_MUTEX(...) rset(CPU0_IPC_MUTEX, CPU0_IPC_MUTEXrv, __VA_ARGS__)
#define RIZS_CPU0_IPC_MUTEX(...) rset(CPU0_IPC_MUTEX, 0, __VA_ARGS__)
#define RFLD_CPU0_IPC_MUTEX(fld) (*((const volatile CPU0_IPC_MUTEX_T *)0xb8141040)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:31; //0
		unsigned int cpu1_mutex:1; //0
	} f;
	unsigned int v;
} CPU1_IPC_MUTEX_T;
#define CPU1_IPC_MUTEXrv (*((regval)0xb8141044))
#define CPU1_IPC_MUTEXdv (0x00000000)
#define RMOD_CPU1_IPC_MUTEX(...) rset(CPU1_IPC_MUTEX, CPU1_IPC_MUTEXrv, __VA_ARGS__)
#define RIZS_CPU1_IPC_MUTEX(...) rset(CPU1_IPC_MUTEX, 0, __VA_ARGS__)
#define RFLD_CPU1_IPC_MUTEX(fld) (*((const volatile CPU1_IPC_MUTEX_T *)0xb8141044)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:30; //0
		unsigned int mutex_owner_id:2; //0
	} f;
	unsigned int v;
} MUTEX_OWNER_T;
#define MUTEX_OWNERrv (*((regval)0xb8141048))
#define MUTEX_OWNERdv (0x00000000)
#define RMOD_MUTEX_OWNER(...) rset(MUTEX_OWNER, MUTEX_OWNERrv, __VA_ARGS__)
#define RIZS_MUTEX_OWNER(...) rset(MUTEX_OWNER, 0, __VA_ARGS__)
#define RFLD_MUTEX_OWNER(fld) (*((const volatile MUTEX_OWNER_T *)0xb8141048)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:31; //0
		unsigned int sync_event:1; //0
	} f;
	unsigned int v;
} IPC_EVENT_TRIG0_T;
#define IPC_EVENT_TRIG0rv (*((regval)0xb814104c))
#define IPC_EVENT_TRIG0dv (0x00000000)
#define RMOD_IPC_EVENT_TRIG0(...) rset(IPC_EVENT_TRIG0, IPC_EVENT_TRIG0rv, __VA_ARGS__)
#define RIZS_IPC_EVENT_TRIG0(...) rset(IPC_EVENT_TRIG0, 0, __VA_ARGS__)
#define RFLD_IPC_EVENT_TRIG0(fld) (*((const volatile IPC_EVENT_TRIG0_T *)0xb814104c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:31; //0
		unsigned int sync_event:1; //0
	} f;
	unsigned int v;
} IPC_EVENT_TRIG1_T;
#define IPC_EVENT_TRIG1rv (*((regval)0xb8141050))
#define IPC_EVENT_TRIG1dv (0x00000000)
#define RMOD_IPC_EVENT_TRIG1(...) rset(IPC_EVENT_TRIG1, IPC_EVENT_TRIG1rv, __VA_ARGS__)
#define RIZS_IPC_EVENT_TRIG1(...) rset(IPC_EVENT_TRIG1, 0, __VA_ARGS__)
#define RFLD_IPC_EVENT_TRIG1(fld) (*((const volatile IPC_EVENT_TRIG1_T *)0xb8141050)).f.fld

/*-----------------------------------------------------
 Extraced from file_UART.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int rbr_thr_dll:8; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART0_RX_TX_DIV_L_T;
#define UART0_RX_TX_DIV_Lrv (*((regval)0xb8002000))
#define UART0_RX_TX_DIV_Ldv (0x00000000)
#define RMOD_UART0_RX_TX_DIV_L(...) rset(UART0_RX_TX_DIV_L, UART0_RX_TX_DIV_Lrv, __VA_ARGS__)
#define RIZS_UART0_RX_TX_DIV_L(...) rset(UART0_RX_TX_DIV_L, 0, __VA_ARGS__)
#define RFLD_UART0_RX_TX_DIV_L(fld) (*((const volatile UART0_RX_TX_DIV_L_T *)0xb8002000)).f.fld

typedef union {
	struct {
		unsigned int ier_dlm:8; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART0_INTR_EN_DIV_M_T;
#define UART0_INTR_EN_DIV_Mrv (*((regval)0xb8002004))
#define UART0_INTR_EN_DIV_Mdv (0x00000000)
#define RMOD_UART0_INTR_EN_DIV_M(...) rset(UART0_INTR_EN_DIV_M, UART0_INTR_EN_DIV_Mrv, __VA_ARGS__)
#define RIZS_UART0_INTR_EN_DIV_M(...) rset(UART0_INTR_EN_DIV_M, 0, __VA_ARGS__)
#define RFLD_UART0_INTR_EN_DIV_M(fld) (*((const volatile UART0_INTR_EN_DIV_M_T *)0xb8002004)).f.fld

typedef union {
	struct {
		unsigned int iir_fcr:8; //193
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART0_INTR_IDEN_FIFO_CTRL_T;
#define UART0_INTR_IDEN_FIFO_CTRLrv (*((regval)0xb8002008))
#define UART0_INTR_IDEN_FIFO_CTRLdv (0xc1000000)
#define RMOD_UART0_INTR_IDEN_FIFO_CTRL(...) rset(UART0_INTR_IDEN_FIFO_CTRL, UART0_INTR_IDEN_FIFO_CTRLrv, __VA_ARGS__)
#define RIZS_UART0_INTR_IDEN_FIFO_CTRL(...) rset(UART0_INTR_IDEN_FIFO_CTRL, 0, __VA_ARGS__)
#define RFLD_UART0_INTR_IDEN_FIFO_CTRL(fld) (*((const volatile UART0_INTR_IDEN_FIFO_CTRL_T *)0xb8002008)).f.fld

typedef union {
	struct {
		unsigned int dlab:1; //0
		unsigned int brk:1; //0
		unsigned int eps:2; //0
		unsigned int pen:1; //0
		unsigned int stb:1; //0
		unsigned int wls_1:1; //1
		unsigned int wls_0:1; //1
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART0_LINE_CTRL_T;
#define UART0_LINE_CTRLrv (*((regval)0xb800200c))
#define UART0_LINE_CTRLdv (0x03000000)
#define RMOD_UART0_LINE_CTRL(...) rset(UART0_LINE_CTRL, UART0_LINE_CTRLrv, __VA_ARGS__)
#define RIZS_UART0_LINE_CTRL(...) rset(UART0_LINE_CTRL, 0, __VA_ARGS__)
#define RFLD_UART0_LINE_CTRL(fld) (*((const volatile UART0_LINE_CTRL_T *)0xb800200c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int lxclk_sel:1; //0
		unsigned int afe:1; //0
		unsigned int loop:1; //0
		unsigned int out2:1; //0
		unsigned int out1:1; //0
		unsigned int rts:1; //0
		unsigned int dtr:1; //0
		unsigned int mbz_1:24; //0
	} f;
	unsigned int v;
} UART0_MODEM_CTRL_T;
#define UART0_MODEM_CTRLrv (*((regval)0xb8002010))
#define UART0_MODEM_CTRLdv (0x00000000)
#define RMOD_UART0_MODEM_CTRL(...) rset(UART0_MODEM_CTRL, UART0_MODEM_CTRLrv, __VA_ARGS__)
#define RIZS_UART0_MODEM_CTRL(...) rset(UART0_MODEM_CTRL, 0, __VA_ARGS__)
#define RFLD_UART0_MODEM_CTRL(fld) (*((const volatile UART0_MODEM_CTRL_T *)0xb8002010)).f.fld

typedef union {
	struct {
		unsigned int rfe:1; //0
		unsigned int temt:1; //1
		unsigned int thre:1; //1
		unsigned int bi:1; //0
		unsigned int fe:1; //0
		unsigned int pe:1; //0
		unsigned int oe:1; //0
		unsigned int dr:1; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART0_LINE_STS_T;
#define UART0_LINE_STSrv (*((regval)0xb8002014))
#define UART0_LINE_STSdv (0x60000000)
#define RMOD_UART0_LINE_STS(...) rset(UART0_LINE_STS, UART0_LINE_STSrv, __VA_ARGS__)
#define RIZS_UART0_LINE_STS(...) rset(UART0_LINE_STS, 0, __VA_ARGS__)
#define RFLD_UART0_LINE_STS(fld) (*((const volatile UART0_LINE_STS_T *)0xb8002014)).f.fld

typedef union {
	struct {
		unsigned int dcts:1; //0
		unsigned int ddsr:1; //0
		unsigned int teri:1; //0
		unsigned int ddcd:1; //1
		unsigned int cts:1; //0
		unsigned int dsr:1; //0
		unsigned int ri:1; //0
		unsigned int dcd:1; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART0_MODEM_STS_T;
#define UART0_MODEM_STSrv (*((regval)0xb8002018))
#define UART0_MODEM_STSdv (0x10000000)
#define RMOD_UART0_MODEM_STS(...) rset(UART0_MODEM_STS, UART0_MODEM_STSrv, __VA_ARGS__)
#define RIZS_UART0_MODEM_STS(...) rset(UART0_MODEM_STS, 0, __VA_ARGS__)
#define RFLD_UART0_MODEM_STS(fld) (*((const volatile UART0_MODEM_STS_T *)0xb8002018)).f.fld

typedef union {
	struct {
		unsigned int rbr_thr_dll:8; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART1_RX_TX_DIV_L_T;
#define UART1_RX_TX_DIV_Lrv (*((regval)0xb8002100))
#define UART1_RX_TX_DIV_Ldv (0x00000000)
#define RMOD_UART1_RX_TX_DIV_L(...) rset(UART1_RX_TX_DIV_L, UART1_RX_TX_DIV_Lrv, __VA_ARGS__)
#define RIZS_UART1_RX_TX_DIV_L(...) rset(UART1_RX_TX_DIV_L, 0, __VA_ARGS__)
#define RFLD_UART1_RX_TX_DIV_L(fld) (*((const volatile UART1_RX_TX_DIV_L_T *)0xb8002100)).f.fld

typedef union {
	struct {
		unsigned int ier_dlm:8; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART1_INTR_EN_DIV_M_T;
#define UART1_INTR_EN_DIV_Mrv (*((regval)0xb8002104))
#define UART1_INTR_EN_DIV_Mdv (0x00000000)
#define RMOD_UART1_INTR_EN_DIV_M(...) rset(UART1_INTR_EN_DIV_M, UART1_INTR_EN_DIV_Mrv, __VA_ARGS__)
#define RIZS_UART1_INTR_EN_DIV_M(...) rset(UART1_INTR_EN_DIV_M, 0, __VA_ARGS__)
#define RFLD_UART1_INTR_EN_DIV_M(fld) (*((const volatile UART1_INTR_EN_DIV_M_T *)0xb8002104)).f.fld

typedef union {
	struct {
		unsigned int iir_fcr:8; //193
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART1_INTR_IDEN_FIFO_CTRL_T;
#define UART1_INTR_IDEN_FIFO_CTRLrv (*((regval)0xb8002108))
#define UART1_INTR_IDEN_FIFO_CTRLdv (0xc1000000)
#define RMOD_UART1_INTR_IDEN_FIFO_CTRL(...) rset(UART1_INTR_IDEN_FIFO_CTRL, UART1_INTR_IDEN_FIFO_CTRLrv, __VA_ARGS__)
#define RIZS_UART1_INTR_IDEN_FIFO_CTRL(...) rset(UART1_INTR_IDEN_FIFO_CTRL, 0, __VA_ARGS__)
#define RFLD_UART1_INTR_IDEN_FIFO_CTRL(fld) (*((const volatile UART1_INTR_IDEN_FIFO_CTRL_T *)0xb8002108)).f.fld

typedef union {
	struct {
		unsigned int dlab:1; //0
		unsigned int brk:1; //0
		unsigned int eps:2; //0
		unsigned int pen:1; //0
		unsigned int stb:1; //0
		unsigned int wls_1:1; //1
		unsigned int wls_0:1; //1
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART1_LINE_CTRL_T;
#define UART1_LINE_CTRLrv (*((regval)0xb800210c))
#define UART1_LINE_CTRLdv (0x03000000)
#define RMOD_UART1_LINE_CTRL(...) rset(UART1_LINE_CTRL, UART1_LINE_CTRLrv, __VA_ARGS__)
#define RIZS_UART1_LINE_CTRL(...) rset(UART1_LINE_CTRL, 0, __VA_ARGS__)
#define RFLD_UART1_LINE_CTRL(fld) (*((const volatile UART1_LINE_CTRL_T *)0xb800210c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:2; //0
		unsigned int afe:1; //0
		unsigned int loop:1; //0
		unsigned int out2:1; //0
		unsigned int out1:1; //0
		unsigned int rts:1; //0
		unsigned int dtr:1; //0
		unsigned int mbz_1:24; //0
	} f;
	unsigned int v;
} UART1_MODEM_CTRL_T;
#define UART1_MODEM_CTRLrv (*((regval)0xb8002110))
#define UART1_MODEM_CTRLdv (0x00000000)
#define RMOD_UART1_MODEM_CTRL(...) rset(UART1_MODEM_CTRL, UART1_MODEM_CTRLrv, __VA_ARGS__)
#define RIZS_UART1_MODEM_CTRL(...) rset(UART1_MODEM_CTRL, 0, __VA_ARGS__)
#define RFLD_UART1_MODEM_CTRL(fld) (*((const volatile UART1_MODEM_CTRL_T *)0xb8002110)).f.fld

typedef union {
	struct {
		unsigned int rfe:1; //0
		unsigned int temt:1; //1
		unsigned int thre:1; //1
		unsigned int bi:1; //0
		unsigned int fe:1; //0
		unsigned int pe:1; //0
		unsigned int oe:1; //0
		unsigned int dr:1; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART1_LINE_STS_T;
#define UART1_LINE_STSrv (*((regval)0xb8002114))
#define UART1_LINE_STSdv (0x60000000)
#define RMOD_UART1_LINE_STS(...) rset(UART1_LINE_STS, UART1_LINE_STSrv, __VA_ARGS__)
#define RIZS_UART1_LINE_STS(...) rset(UART1_LINE_STS, 0, __VA_ARGS__)
#define RFLD_UART1_LINE_STS(fld) (*((const volatile UART1_LINE_STS_T *)0xb8002114)).f.fld

typedef union {
	struct {
		unsigned int dcts:1; //0
		unsigned int ddsr:1; //0
		unsigned int teri:1; //0
		unsigned int ddcd:1; //1
		unsigned int cts:1; //0
		unsigned int dsr:1; //0
		unsigned int ri:1; //0
		unsigned int dcd:1; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART1_MODEM_STS_T;
#define UART1_MODEM_STSrv (*((regval)0xb8002118))
#define UART1_MODEM_STSdv (0x10000000)
#define RMOD_UART1_MODEM_STS(...) rset(UART1_MODEM_STS, UART1_MODEM_STSrv, __VA_ARGS__)
#define RIZS_UART1_MODEM_STS(...) rset(UART1_MODEM_STS, 0, __VA_ARGS__)
#define RFLD_UART1_MODEM_STS(fld) (*((const volatile UART1_MODEM_STS_T *)0xb8002118)).f.fld

typedef union {
	struct {
		unsigned int rbr_thr_dll:8; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART2_RX_TX_DIV_L_T;
#define UART2_RX_TX_DIV_Lrv (*((regval)0xb8002200))
#define UART2_RX_TX_DIV_Ldv (0x00000000)
#define RMOD_UART2_RX_TX_DIV_L(...) rset(UART2_RX_TX_DIV_L, UART2_RX_TX_DIV_Lrv, __VA_ARGS__)
#define RIZS_UART2_RX_TX_DIV_L(...) rset(UART2_RX_TX_DIV_L, 0, __VA_ARGS__)
#define RFLD_UART2_RX_TX_DIV_L(fld) (*((const volatile UART2_RX_TX_DIV_L_T *)0xb8002200)).f.fld

typedef union {
	struct {
		unsigned int ier_dlm:8; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART2_INTR_EN_DIV_M_T;
#define UART2_INTR_EN_DIV_Mrv (*((regval)0xb8002204))
#define UART2_INTR_EN_DIV_Mdv (0x00000000)
#define RMOD_UART2_INTR_EN_DIV_M(...) rset(UART2_INTR_EN_DIV_M, UART2_INTR_EN_DIV_Mrv, __VA_ARGS__)
#define RIZS_UART2_INTR_EN_DIV_M(...) rset(UART2_INTR_EN_DIV_M, 0, __VA_ARGS__)
#define RFLD_UART2_INTR_EN_DIV_M(fld) (*((const volatile UART2_INTR_EN_DIV_M_T *)0xb8002204)).f.fld

typedef union {
	struct {
		unsigned int iir_fcr:8; //193
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART2_INTR_IDEN_FIFO_CTRL_T;
#define UART2_INTR_IDEN_FIFO_CTRLrv (*((regval)0xb8002208))
#define UART2_INTR_IDEN_FIFO_CTRLdv (0xc1000000)
#define RMOD_UART2_INTR_IDEN_FIFO_CTRL(...) rset(UART2_INTR_IDEN_FIFO_CTRL, UART2_INTR_IDEN_FIFO_CTRLrv, __VA_ARGS__)
#define RIZS_UART2_INTR_IDEN_FIFO_CTRL(...) rset(UART2_INTR_IDEN_FIFO_CTRL, 0, __VA_ARGS__)
#define RFLD_UART2_INTR_IDEN_FIFO_CTRL(fld) (*((const volatile UART2_INTR_IDEN_FIFO_CTRL_T *)0xb8002208)).f.fld

typedef union {
	struct {
		unsigned int dlab:1; //0
		unsigned int brk:1; //0
		unsigned int eps:2; //0
		unsigned int pen:1; //0
		unsigned int stb:1; //0
		unsigned int wls_1:1; //1
		unsigned int wls_0:1; //1
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART2_LINE_CTRL_T;
#define UART2_LINE_CTRLrv (*((regval)0xb800220c))
#define UART2_LINE_CTRLdv (0x03000000)
#define RMOD_UART2_LINE_CTRL(...) rset(UART2_LINE_CTRL, UART2_LINE_CTRLrv, __VA_ARGS__)
#define RIZS_UART2_LINE_CTRL(...) rset(UART2_LINE_CTRL, 0, __VA_ARGS__)
#define RFLD_UART2_LINE_CTRL(fld) (*((const volatile UART2_LINE_CTRL_T *)0xb800220c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:2; //0
		unsigned int afe:1; //0
		unsigned int loop:1; //0
		unsigned int out2:1; //0
		unsigned int out1:1; //0
		unsigned int rts:1; //0
		unsigned int dtr:1; //0
		unsigned int mbz_1:24; //0
	} f;
	unsigned int v;
} UART2_MODEM_CTRL_T;
#define UART2_MODEM_CTRLrv (*((regval)0xb8002210))
#define UART2_MODEM_CTRLdv (0x00000000)
#define RMOD_UART2_MODEM_CTRL(...) rset(UART2_MODEM_CTRL, UART2_MODEM_CTRLrv, __VA_ARGS__)
#define RIZS_UART2_MODEM_CTRL(...) rset(UART2_MODEM_CTRL, 0, __VA_ARGS__)
#define RFLD_UART2_MODEM_CTRL(fld) (*((const volatile UART2_MODEM_CTRL_T *)0xb8002210)).f.fld

typedef union {
	struct {
		unsigned int rfe:1; //0
		unsigned int temt:1; //1
		unsigned int thre:1; //1
		unsigned int bi:1; //0
		unsigned int fe:1; //0
		unsigned int pe:1; //0
		unsigned int oe:1; //0
		unsigned int dr:1; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART2_LINE_STS_T;
#define UART2_LINE_STSrv (*((regval)0xb8002214))
#define UART2_LINE_STSdv (0x60000000)
#define RMOD_UART2_LINE_STS(...) rset(UART2_LINE_STS, UART2_LINE_STSrv, __VA_ARGS__)
#define RIZS_UART2_LINE_STS(...) rset(UART2_LINE_STS, 0, __VA_ARGS__)
#define RFLD_UART2_LINE_STS(fld) (*((const volatile UART2_LINE_STS_T *)0xb8002214)).f.fld

typedef union {
	struct {
		unsigned int dcts:1; //0
		unsigned int ddsr:1; //0
		unsigned int teri:1; //0
		unsigned int ddcd:1; //1
		unsigned int cts:1; //0
		unsigned int dsr:1; //0
		unsigned int ri:1; //0
		unsigned int dcd:1; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART2_MODEM_STS_T;
#define UART2_MODEM_STSrv (*((regval)0xb8002218))
#define UART2_MODEM_STSdv (0x10000000)
#define RMOD_UART2_MODEM_STS(...) rset(UART2_MODEM_STS, UART2_MODEM_STSrv, __VA_ARGS__)
#define RIZS_UART2_MODEM_STS(...) rset(UART2_MODEM_STS, 0, __VA_ARGS__)
#define RFLD_UART2_MODEM_STS(fld) (*((const volatile UART2_MODEM_STS_T *)0xb8002218)).f.fld

typedef union {
	struct {
		unsigned int rbr_thr_dll:8; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART3_RX_TX_DIV_L_T;
#define UART3_RX_TX_DIV_Lrv (*((regval)0xb8002300))
#define UART3_RX_TX_DIV_Ldv (0x00000000)
#define RMOD_UART3_RX_TX_DIV_L(...) rset(UART3_RX_TX_DIV_L, UART3_RX_TX_DIV_Lrv, __VA_ARGS__)
#define RIZS_UART3_RX_TX_DIV_L(...) rset(UART3_RX_TX_DIV_L, 0, __VA_ARGS__)
#define RFLD_UART3_RX_TX_DIV_L(fld) (*((const volatile UART3_RX_TX_DIV_L_T *)0xb8002300)).f.fld

typedef union {
	struct {
		unsigned int ier_dlm:8; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART3_INTR_EN_DIV_M_T;
#define UART3_INTR_EN_DIV_Mrv (*((regval)0xb8002304))
#define UART3_INTR_EN_DIV_Mdv (0x00000000)
#define RMOD_UART3_INTR_EN_DIV_M(...) rset(UART3_INTR_EN_DIV_M, UART3_INTR_EN_DIV_Mrv, __VA_ARGS__)
#define RIZS_UART3_INTR_EN_DIV_M(...) rset(UART3_INTR_EN_DIV_M, 0, __VA_ARGS__)
#define RFLD_UART3_INTR_EN_DIV_M(fld) (*((const volatile UART3_INTR_EN_DIV_M_T *)0xb8002304)).f.fld

typedef union {
	struct {
		unsigned int iir_fcr:8; //193
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART3_INTR_IDEN_FIFO_CTRL_T;
#define UART3_INTR_IDEN_FIFO_CTRLrv (*((regval)0xb8002308))
#define UART3_INTR_IDEN_FIFO_CTRLdv (0xc1000000)
#define RMOD_UART3_INTR_IDEN_FIFO_CTRL(...) rset(UART3_INTR_IDEN_FIFO_CTRL, UART3_INTR_IDEN_FIFO_CTRLrv, __VA_ARGS__)
#define RIZS_UART3_INTR_IDEN_FIFO_CTRL(...) rset(UART3_INTR_IDEN_FIFO_CTRL, 0, __VA_ARGS__)
#define RFLD_UART3_INTR_IDEN_FIFO_CTRL(fld) (*((const volatile UART3_INTR_IDEN_FIFO_CTRL_T *)0xb8002308)).f.fld

typedef union {
	struct {
		unsigned int dlab:1; //0
		unsigned int brk:1; //0
		unsigned int eps:2; //0
		unsigned int pen:1; //0
		unsigned int stb:1; //0
		unsigned int wls_1:1; //1
		unsigned int wls_0:1; //1
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART3_LINE_CTRL_T;
#define UART3_LINE_CTRLrv (*((regval)0xb800230c))
#define UART3_LINE_CTRLdv (0x03000000)
#define RMOD_UART3_LINE_CTRL(...) rset(UART3_LINE_CTRL, UART3_LINE_CTRLrv, __VA_ARGS__)
#define RIZS_UART3_LINE_CTRL(...) rset(UART3_LINE_CTRL, 0, __VA_ARGS__)
#define RFLD_UART3_LINE_CTRL(fld) (*((const volatile UART3_LINE_CTRL_T *)0xb800230c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:2; //0
		unsigned int afe:1; //0
		unsigned int loop:1; //0
		unsigned int out2:1; //0
		unsigned int out1:1; //0
		unsigned int rts:1; //0
		unsigned int dtr:1; //0
		unsigned int mbz_1:24; //0
	} f;
	unsigned int v;
} UART3_MODEM_CTRL_T;
#define UART3_MODEM_CTRLrv (*((regval)0xb8002310))
#define UART3_MODEM_CTRLdv (0x00000000)
#define RMOD_UART3_MODEM_CTRL(...) rset(UART3_MODEM_CTRL, UART3_MODEM_CTRLrv, __VA_ARGS__)
#define RIZS_UART3_MODEM_CTRL(...) rset(UART3_MODEM_CTRL, 0, __VA_ARGS__)
#define RFLD_UART3_MODEM_CTRL(fld) (*((const volatile UART3_MODEM_CTRL_T *)0xb8002310)).f.fld

typedef union {
	struct {
		unsigned int rfe:1; //0
		unsigned int temt:1; //1
		unsigned int thre:1; //1
		unsigned int bi:1; //0
		unsigned int fe:1; //0
		unsigned int pe:1; //0
		unsigned int oe:1; //0
		unsigned int dr:1; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART3_LINE_STS_T;
#define UART3_LINE_STSrv (*((regval)0xb8002314))
#define UART3_LINE_STSdv (0x60000000)
#define RMOD_UART3_LINE_STS(...) rset(UART3_LINE_STS, UART3_LINE_STSrv, __VA_ARGS__)
#define RIZS_UART3_LINE_STS(...) rset(UART3_LINE_STS, 0, __VA_ARGS__)
#define RFLD_UART3_LINE_STS(fld) (*((const volatile UART3_LINE_STS_T *)0xb8002314)).f.fld

typedef union {
	struct {
		unsigned int dcts:1; //0
		unsigned int ddsr:1; //0
		unsigned int teri:1; //0
		unsigned int ddcd:1; //1
		unsigned int cts:1; //0
		unsigned int dsr:1; //0
		unsigned int ri:1; //0
		unsigned int dcd:1; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART3_MODEM_STS_T;
#define UART3_MODEM_STSrv (*((regval)0xb8002318))
#define UART3_MODEM_STSdv (0x10000000)
#define RMOD_UART3_MODEM_STS(...) rset(UART3_MODEM_STS, UART3_MODEM_STSrv, __VA_ARGS__)
#define RIZS_UART3_MODEM_STS(...) rset(UART3_MODEM_STS, 0, __VA_ARGS__)
#define RFLD_UART3_MODEM_STS(fld) (*((const volatile UART3_MODEM_STS_T *)0xb8002318)).f.fld

/*-----------------------------------------------------
 Extraced from file_INTC.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int tmo_ie:1; //0
		unsigned int voipacc_ie:1; //0
		unsigned int spi_ie:1; //0
		unsigned int xsi_ie:1; //0
		unsigned int mbz_0:1; //0
		unsigned int gmac_ie:1; //0
		unsigned int mbz_1:4; //0
		unsigned int gdma1_ie:1; //0
		unsigned int gdma0_ie:1; //0
		unsigned int security_ie:1; //0
		unsigned int pcm1_ie:1; //0
		unsigned int pcm0_ie:1; //0
		unsigned int pcie1_ie:1; //0
		unsigned int pcie0_ie:1; //0
		unsigned int usb_h2_ie:1; //0
		unsigned int usb_h3_ie:1; //0
		unsigned int peripheral_ie:1; //0
		unsigned int mbz_2:1; //0
		unsigned int gpio_intr1:1; //0
		unsigned int gpio_intr0:1; //0
		unsigned int switch_ie:1; //0
		unsigned int sata_ie:1; //0
		unsigned int nfbi_ie:1; //0
		unsigned int mbz_3:2; //0
		unsigned int fftacc_ie:1; //0
		unsigned int nandctl_ie:1; //0
		unsigned int ipc_cpu_to_dsp:1; //0
		unsigned int ipc_dsp_to_cpu:1; //0
	} f;
	unsigned int v;
} VPE0_GIMR0_T;
#define VPE0_GIMR0rv (*((regval)0xb8003000))
#define VPE0_GIMR0dv (0x00000000)
#define RMOD_VPE0_GIMR0(...) rset(VPE0_GIMR0, VPE0_GIMR0rv, __VA_ARGS__)
#define RIZS_VPE0_GIMR0(...) rset(VPE0_GIMR0, 0, __VA_ARGS__)
#define RFLD_VPE0_GIMR0(fld) (*((const volatile VPE0_GIMR0_T *)0xb8003000)).f.fld

typedef union {
	struct {
		unsigned int lbctmom2_ie:1; //0
		unsigned int lbctmom1_ie:1; //0
		unsigned int lbctmom0_ie:1; //0
		unsigned int lbctmos3_ie:1; //0
		unsigned int lbctmos2_ie:1; //0
		unsigned int lbctmos1_ie:1; //0
		unsigned int lbctmos0_ie:1; //0
		unsigned int ocpto1_ie:1; //0
		unsigned int ocpto0_ie:1; //0
		unsigned int ocp1_cpu0_ila_ip:1; //0
		unsigned int ocp0_cpu1_ila_ip:1; //0
		unsigned int uart3_ie:1; //0
		unsigned int uart2_ie:1; //0
		unsigned int uart1_ie:1; //0
		unsigned int uart0_ie:1; //0
		unsigned int tc5_ie:1; //0
		unsigned int tc4_ie:1; //0
		unsigned int tc3_ie:1; //0
		unsigned int tc2_ie:1; //0
		unsigned int tc1_ie:1; //0
		unsigned int tc0_ie:1; //0
		unsigned int gpio_efgh_ie:1; //0
		unsigned int gpio_abcd_ie:1; //0
		unsigned int wdt_ph2to_ie:1; //0
		unsigned int wdt_ph1to_ie:1; //0
		unsigned int tc5_delay_int_ie:1; //0
		unsigned int tc4_delay_int_ie:1; //0
		unsigned int tc3_delay_int_ie:1; //0
		unsigned int tc2_delay_int_ie:1; //0
		unsigned int tc1_delay_int_ie:1; //0
		unsigned int tc0_delay_int_ie:1; //0
		unsigned int mbz_0:1; //0
	} f;
	unsigned int v;
} VPE0_GIMR1_T;
#define VPE0_GIMR1rv (*((regval)0xb8003004))
#define VPE0_GIMR1dv (0x00000000)
#define RMOD_VPE0_GIMR1(...) rset(VPE0_GIMR1, VPE0_GIMR1rv, __VA_ARGS__)
#define RIZS_VPE0_GIMR1(...) rset(VPE0_GIMR1, 0, __VA_ARGS__)
#define RFLD_VPE0_GIMR1(fld) (*((const volatile VPE0_GIMR1_T *)0xb8003004)).f.fld

typedef union {
	struct {
		unsigned int tmo_ip:1; //0
		unsigned int voipacc_ip:1; //0
		unsigned int spi_ip:1; //0
		unsigned int xsi_ip:1; //0
		unsigned int mbz_0:1; //0
		unsigned int gmac_ip:1; //0
		unsigned int mbz_1:4; //0
		unsigned int gdma1_ip:1; //0
		unsigned int gdma0_ip:1; //0
		unsigned int security_ip:1; //0
		unsigned int pcm0_ip:1; //0
		unsigned int pcm1_ip:1; //0
		unsigned int pcie1_ip:1; //0
		unsigned int pcie0_ip:1; //0
		unsigned int usb_h2_ip:1; //0
		unsigned int usb_h3_ip:1; //0
		unsigned int peripheral_ip:1; //0
		unsigned int mbz_2:1; //0
		unsigned int gpio_intr1:1; //0
		unsigned int gpio_intr0:1; //0
		unsigned int switch_ip:1; //0
		unsigned int sata_ip:1; //0
		unsigned int nfbi_ip:1; //0
		unsigned int mbz_3:2; //0
		unsigned int fftacc_ip:1; //0
		unsigned int nandctl_ip:1; //0
		unsigned int ipc_cpu_to_dsp_ip:1; //0
		unsigned int ipc_dsp_to_cpu_ip:1; //0
	} f;
	unsigned int v;
} VPE0_GISR0_T;
#define VPE0_GISR0rv (*((regval)0xb8003008))
#define VPE0_GISR0dv (0x00000000)
#define RMOD_VPE0_GISR0(...) rset(VPE0_GISR0, VPE0_GISR0rv, __VA_ARGS__)
#define RIZS_VPE0_GISR0(...) rset(VPE0_GISR0, 0, __VA_ARGS__)
#define RFLD_VPE0_GISR0(fld) (*((const volatile VPE0_GISR0_T *)0xb8003008)).f.fld

typedef union {
	struct {
		unsigned int lbctmom2_ip:1; //0
		unsigned int lbctmom1_ip:1; //0
		unsigned int lbctmom0_ip:1; //0
		unsigned int lbctmos3_ip:1; //0
		unsigned int lbctmos2_ip:1; //0
		unsigned int lbctmos1_ip:1; //0
		unsigned int lbctmos0_ip:1; //0
		unsigned int ocpto1_ip:1; //0
		unsigned int ocpto0_ip:1; //0
		unsigned int ocp1_cpu0_ila_ip:1; //0
		unsigned int ocp0_cpu1_ila_ip:1; //0
		unsigned int uart3_ip:1; //0
		unsigned int uart2_ip:1; //0
		unsigned int uart1_ip:1; //0
		unsigned int uart0_ip:1; //0
		unsigned int tc5_ip:1; //0
		unsigned int tc4_ip:1; //0
		unsigned int tc3_ip:1; //0
		unsigned int tc2_ip:1; //0
		unsigned int tc1_ip:1; //0
		unsigned int tc0_ip:1; //0
		unsigned int gpio_efgh_ip:1; //0
		unsigned int gpio_abcd_ip:1; //0
		unsigned int wdt_ph2to_ip:1; //0
		unsigned int wdt_ph1to_ip:1; //0
		unsigned int tc5_delay_int_ip:1; //0
		unsigned int tc4_delay_int_ip:1; //0
		unsigned int tc3_delay_int_ip:1; //0
		unsigned int tc2_delay_int_ip:1; //0
		unsigned int tc1_delay_int_ip:1; //0
		unsigned int tc0_delay_int_ip:1; //0
		unsigned int mbz_0:1; //0
	} f;
	unsigned int v;
} VPE0_GISR1_T;
#define VPE0_GISR1rv (*((regval)0xb800300c))
#define VPE0_GISR1dv (0x00000000)
#define RMOD_VPE0_GISR1(...) rset(VPE0_GISR1, VPE0_GISR1rv, __VA_ARGS__)
#define RIZS_VPE0_GISR1(...) rset(VPE0_GISR1, 0, __VA_ARGS__)
#define RFLD_VPE0_GISR1(fld) (*((const volatile VPE0_GISR1_T *)0xb800300c)).f.fld

typedef union {
	struct {
		unsigned int lbctmom2_rs:4; //0
		unsigned int lbctmom1_rs:4; //0
		unsigned int lbctmom0_rs:4; //0
		unsigned int lbctmos3_rs:4; //0
		unsigned int lbctmos2_rs:4; //0
		unsigned int lbctmos1_rs:4; //0
		unsigned int lbctmos0_rs:4; //0
		unsigned int ocpto1_rs:4; //0
	} f;
	unsigned int v;
} VPE0_IRR0_T;
#define VPE0_IRR0rv (*((regval)0xb8003010))
#define VPE0_IRR0dv (0x00000000)
#define RMOD_VPE0_IRR0(...) rset(VPE0_IRR0, VPE0_IRR0rv, __VA_ARGS__)
#define RIZS_VPE0_IRR0(...) rset(VPE0_IRR0, 0, __VA_ARGS__)
#define RFLD_VPE0_IRR0(fld) (*((const volatile VPE0_IRR0_T *)0xb8003010)).f.fld

typedef union {
	struct {
		unsigned int ocpto0_rs:4; //0
		unsigned int ocp1_cpu0_ila_rs:4; //0
		unsigned int ocp0_cpu1_ila_rs:4; //0
		unsigned int voipacc_rs:4; //0
		unsigned int spi_rs:4; //0
		unsigned int xsi_rs:4; //0
		unsigned int mbz_0:4; //0
		unsigned int gmac_rs:4; //0
	} f;
	unsigned int v;
} VPE0_IRR1_T;
#define VPE0_IRR1rv (*((regval)0xb8003014))
#define VPE0_IRR1dv (0x00000000)
#define RMOD_VPE0_IRR1(...) rset(VPE0_IRR1, VPE0_IRR1rv, __VA_ARGS__)
#define RIZS_VPE0_IRR1(...) rset(VPE0_IRR1, 0, __VA_ARGS__)
#define RFLD_VPE0_IRR1(fld) (*((const volatile VPE0_IRR1_T *)0xb8003014)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:8; //0
		unsigned int gpio_intr1:4; //0
		unsigned int gpio_intr0:4; //0
		unsigned int gdma1_rs:4; //0
		unsigned int gdma0_rs:4; //0
		unsigned int security_rs:4; //0
		unsigned int pcm1_rs:4; //0
	} f;
	unsigned int v;
} VPE0_IRR2_T;
#define VPE0_IRR2rv (*((regval)0xb8003018))
#define VPE0_IRR2dv (0x00000000)
#define RMOD_VPE0_IRR2(...) rset(VPE0_IRR2, VPE0_IRR2rv, __VA_ARGS__)
#define RIZS_VPE0_IRR2(...) rset(VPE0_IRR2, 0, __VA_ARGS__)
#define RFLD_VPE0_IRR2(fld) (*((const volatile VPE0_IRR2_T *)0xb8003018)).f.fld

typedef union {
	struct {
		unsigned int pcm0_rs:4; //0
		unsigned int pcie1_rs:4; //0
		unsigned int pcie0_rs:4; //0
		unsigned int usb_h2_rs:4; //0
		unsigned int usb_h3_rs:4; //0
		unsigned int uart3_rs:4; //0
		unsigned int uart2_rs:4; //0
		unsigned int uart1_rs:4; //0
	} f;
	unsigned int v;
} VPE0_IRR3_T;
#define VPE0_IRR3rv (*((regval)0xb800301c))
#define VPE0_IRR3dv (0x00000000)
#define RMOD_VPE0_IRR3(...) rset(VPE0_IRR3, VPE0_IRR3rv, __VA_ARGS__)
#define RIZS_VPE0_IRR3(...) rset(VPE0_IRR3, 0, __VA_ARGS__)
#define RFLD_VPE0_IRR3(fld) (*((const volatile VPE0_IRR3_T *)0xb800301c)).f.fld

typedef union {
	struct {
		unsigned int uart0_rs:4; //0
		unsigned int tc5_rs:4; //0
		unsigned int tc4_rs:4; //0
		unsigned int tc3_rs:4; //0
		unsigned int tc2_rs:4; //0
		unsigned int tc1_rs:4; //0
		unsigned int tc0_rs:4; //0
		unsigned int gpio_efgh_rs:4; //0
	} f;
	unsigned int v;
} VPE0_IRR4_T;
#define VPE0_IRR4rv (*((regval)0xb8003020))
#define VPE0_IRR4dv (0x00000000)
#define RMOD_VPE0_IRR4(...) rset(VPE0_IRR4, VPE0_IRR4rv, __VA_ARGS__)
#define RIZS_VPE0_IRR4(...) rset(VPE0_IRR4, 0, __VA_ARGS__)
#define RFLD_VPE0_IRR4(fld) (*((const volatile VPE0_IRR4_T *)0xb8003020)).f.fld

typedef union {
	struct {
		unsigned int gpio_abcd_rs:4; //0
		unsigned int switch_rs:4; //0
		unsigned int sata_rs:4; //0
		unsigned int wdog_ph2to_rs:4; //0
		unsigned int wdog_ph1to_rs:4; //0
		unsigned int fftacc_rs:4; //0
		unsigned int flsh_rs:4; //0
		unsigned int mbz_0:4; //0
	} f;
	unsigned int v;
} VPE0_IRR5_T;
#define VPE0_IRR5rv (*((regval)0xb8003024))
#define VPE0_IRR5dv (0x00000000)
#define RMOD_VPE0_IRR5(...) rset(VPE0_IRR5, VPE0_IRR5rv, __VA_ARGS__)
#define RIZS_VPE0_IRR5(...) rset(VPE0_IRR5, 0, __VA_ARGS__)
#define RFLD_VPE0_IRR5(fld) (*((const volatile VPE0_IRR5_T *)0xb8003024)).f.fld

typedef union {
	struct {
		unsigned int ipc_cpu_to_dsp_rs:4; //0
		unsigned int ipc_dsp_to_cpu_rs:4; //0
		unsigned int tc5_delay_int5_rs:4; //0
		unsigned int tc4_delay_int4_rs:4; //0
		unsigned int tc3_delay_int3_rs:4; //0
		unsigned int tc2_delay_int2_rs:4; //0
		unsigned int tc1_delay_int1_rs:4; //0
		unsigned int tc0_delay_int0_rs:4; //0
	} f;
	unsigned int v;
} VPE0_IRR6_T;
#define VPE0_IRR6rv (*((regval)0xb8003028))
#define VPE0_IRR6dv (0x00000000)
#define RMOD_VPE0_IRR6(...) rset(VPE0_IRR6, VPE0_IRR6rv, __VA_ARGS__)
#define RIZS_VPE0_IRR6(...) rset(VPE0_IRR6, 0, __VA_ARGS__)
#define RFLD_VPE0_IRR6(fld) (*((const volatile VPE0_IRR6_T *)0xb8003028)).f.fld

typedef union {
	struct {
		unsigned int tmo_ie:1; //0
		unsigned int voipacc_ie:1; //0
		unsigned int spi_ie:1; //0
		unsigned int ptm_ie:1; //0
		unsigned int mbz_0:1; //0
		unsigned int gmac_ie:1; //0
		unsigned int mbz_1:4; //0
		unsigned int gdma1_ie:1; //0
		unsigned int gdma0_ie:1; //0
		unsigned int security_ie:1; //0
		unsigned int pcm1_ie:1; //0
		unsigned int pcm0_ie:1; //0
		unsigned int pcie1_ie:1; //0
		unsigned int pcie0_ie:1; //0
		unsigned int usb_h2_ie:1; //0
		unsigned int usb_h3_ie:1; //0
		unsigned int peripheral_ie:1; //0
		unsigned int mbz_2:3; //0
		unsigned int switch_ie:1; //0
		unsigned int sata_ie:1; //0
		unsigned int nfbi_ie:1; //0
		unsigned int mbz_3:2; //0
		unsigned int flsh_ie:1; //0
		unsigned int utmd_ie:1; //0
		unsigned int ipc_cpu_to_dsp:1; //0
		unsigned int ipc_dsp_to_cpu:1; //0
	} f;
	unsigned int v;
} VPE1_GIMR0_T;
#define VPE1_GIMR0rv (*((regval)0xb8003100))
#define VPE1_GIMR0dv (0x00000000)
#define RMOD_VPE1_GIMR0(...) rset(VPE1_GIMR0, VPE1_GIMR0rv, __VA_ARGS__)
#define RIZS_VPE1_GIMR0(...) rset(VPE1_GIMR0, 0, __VA_ARGS__)
#define RFLD_VPE1_GIMR0(fld) (*((const volatile VPE1_GIMR0_T *)0xb8003100)).f.fld

typedef union {
	struct {
		unsigned int lbctmom2_ie:1; //0
		unsigned int lbctmom1_ie:1; //0
		unsigned int lbctmom0_ie:1; //0
		unsigned int lbctmos3_ie:1; //0
		unsigned int lbctmos2_ie:1; //0
		unsigned int lbctmos1_ie:1; //0
		unsigned int lbctmos0_ie:1; //0
		unsigned int ocpto1_ie:1; //0
		unsigned int ocpto0_ie:1; //0
		unsigned int ocp1_cpu0_ila_ip:1; //0
		unsigned int ocp0_cpu1_ila_ip:1; //0
		unsigned int uart3_ie:1; //0
		unsigned int uart2_ie:1; //0
		unsigned int uart1_ie:1; //0
		unsigned int uart0_ie:1; //0
		unsigned int tc5_ie:1; //0
		unsigned int tc4_ie:1; //0
		unsigned int tc3_ie:1; //0
		unsigned int tc2_ie:1; //0
		unsigned int tc1_ie:1; //0
		unsigned int tc0_ie:1; //0
		unsigned int gpio_efgh_ie:1; //0
		unsigned int gpio_abcd_ie:1; //0
		unsigned int wdt_ph2to_ie:1; //0
		unsigned int wdt_ph1to_ie:1; //0
		unsigned int tc5_delay_int_ie:1; //0
		unsigned int tc4_delay_int_ie:1; //0
		unsigned int tc3_delay_int_ie:1; //0
		unsigned int tc2_delay_int_ie:1; //0
		unsigned int tc1_delay_int_ie:1; //0
		unsigned int tc0_delay_int_ie:1; //0
		unsigned int mbz_0:1; //0
	} f;
	unsigned int v;
} VPE1_GIMR1_T;
#define VPE1_GIMR1rv (*((regval)0xb8003104))
#define VPE1_GIMR1dv (0x00000000)
#define RMOD_VPE1_GIMR1(...) rset(VPE1_GIMR1, VPE1_GIMR1rv, __VA_ARGS__)
#define RIZS_VPE1_GIMR1(...) rset(VPE1_GIMR1, 0, __VA_ARGS__)
#define RFLD_VPE1_GIMR1(fld) (*((const volatile VPE1_GIMR1_T *)0xb8003104)).f.fld

typedef union {
	struct {
		unsigned int tmo_ip:1; //0
		unsigned int voipacc_ip:1; //0
		unsigned int spi_ip:1; //0
		unsigned int ptm_ip:1; //0
		unsigned int mbz_0:1; //0
		unsigned int gmac_ip:1; //0
		unsigned int mbz_1:4; //0
		unsigned int gdma1_ip:1; //0
		unsigned int gdma0_ip:1; //0
		unsigned int security_ip:1; //0
		unsigned int pcm0_ip:1; //0
		unsigned int pcm1_ip:1; //0
		unsigned int pcie1_ip:1; //0
		unsigned int pcie0_ip:1; //0
		unsigned int usb_h2_ip:1; //0
		unsigned int usb_h3_ip:1; //0
		unsigned int peripheral_ip:1; //0
		unsigned int mbz_2:1; //0
		unsigned int gpio_intr1:1; //0
		unsigned int gpio_intr0:1; //0
		unsigned int switch_ip:1; //0
		unsigned int sata_ip:1; //0
		unsigned int nfbi_ip:1; //0
		unsigned int mbz_3:2; //0
		unsigned int fftacc_ip:1; //0
		unsigned int flsh_ip:1; //0
		unsigned int ipc_cpu_to_dsp_ip:1; //0
		unsigned int ipc_dsp_to_cpu_ip:1; //0
	} f;
	unsigned int v;
} VPE1_GISR0_T;
#define VPE1_GISR0rv (*((regval)0xb8003108))
#define VPE1_GISR0dv (0x00000000)
#define RMOD_VPE1_GISR0(...) rset(VPE1_GISR0, VPE1_GISR0rv, __VA_ARGS__)
#define RIZS_VPE1_GISR0(...) rset(VPE1_GISR0, 0, __VA_ARGS__)
#define RFLD_VPE1_GISR0(fld) (*((const volatile VPE1_GISR0_T *)0xb8003108)).f.fld

typedef union {
	struct {
		unsigned int lbctmom2_ip:1; //0
		unsigned int lbctmom1_ip:1; //0
		unsigned int lbctmom0_ip:1; //0
		unsigned int lbctmos3_ip:1; //0
		unsigned int lbctmos2_ip:1; //0
		unsigned int lbctmos1_ip:1; //0
		unsigned int lbctmos0_ip:1; //0
		unsigned int ocpto1_ip:1; //0
		unsigned int ocpto0_ip:1; //0
		unsigned int ocp1_cpu0_ila_ip:1; //0
		unsigned int ocp0_cpu1_ila_ip:1; //0
		unsigned int uart3_ip:1; //0
		unsigned int uart2_ip:1; //0
		unsigned int uart1_ip:1; //0
		unsigned int uart0_ip:1; //0
		unsigned int tc5_ip:1; //0
		unsigned int tc4_ip:1; //0
		unsigned int tc3_ip:1; //0
		unsigned int tc2_ip:1; //0
		unsigned int tc1_ip:1; //0
		unsigned int tc0_ip:1; //0
		unsigned int gpio_efgh_ip:1; //0
		unsigned int gpio_abcd_ip:1; //0
		unsigned int wdt_ph2to_ip:1; //0
		unsigned int wdt_ph1to_ip:1; //0
		unsigned int tc5_delay_int_ip:1; //0
		unsigned int tc4_delay_int_ip:1; //0
		unsigned int tc3_delay_int_ip:1; //0
		unsigned int tc2_delay_int_ip:1; //0
		unsigned int tc1_delay_int_ip:1; //0
		unsigned int tc0_delay_int_ip:1; //0
		unsigned int mbz_0:1; //0
	} f;
	unsigned int v;
} VPE1_GISR1_T;
#define VPE1_GISR1rv (*((regval)0xb800310c))
#define VPE1_GISR1dv (0x00000000)
#define RMOD_VPE1_GISR1(...) rset(VPE1_GISR1, VPE1_GISR1rv, __VA_ARGS__)
#define RIZS_VPE1_GISR1(...) rset(VPE1_GISR1, 0, __VA_ARGS__)
#define RFLD_VPE1_GISR1(fld) (*((const volatile VPE1_GISR1_T *)0xb800310c)).f.fld

typedef union {
	struct {
		unsigned int lbctmom2_rs:4; //0
		unsigned int lbctmom1_rs:4; //0
		unsigned int lbctmom0_rs:4; //0
		unsigned int lbctmos3_rs:4; //0
		unsigned int lbctmos2_rs:4; //0
		unsigned int lbctmos1_rs:4; //0
		unsigned int lbctmos0_rs:4; //0
		unsigned int ocpto1_rs:4; //0
	} f;
	unsigned int v;
} VPE1_IRR0_T;
#define VPE1_IRR0rv (*((regval)0xb8003110))
#define VPE1_IRR0dv (0x00000000)
#define RMOD_VPE1_IRR0(...) rset(VPE1_IRR0, VPE1_IRR0rv, __VA_ARGS__)
#define RIZS_VPE1_IRR0(...) rset(VPE1_IRR0, 0, __VA_ARGS__)
#define RFLD_VPE1_IRR0(fld) (*((const volatile VPE1_IRR0_T *)0xb8003110)).f.fld

typedef union {
	struct {
		unsigned int ocpto0_rs:4; //0
		unsigned int ocp1_cpu0_ila_rs:4; //0
		unsigned int ocp0_cpu1_ila_rs:4; //0
		unsigned int voipacc_rs:4; //0
		unsigned int spi_rs:4; //0
		unsigned int ptm_rs:4; //0
		unsigned int mbz_0:4; //0
		unsigned int gmac_rs:4; //0
	} f;
	unsigned int v;
} VPE1_IRR1_T;
#define VPE1_IRR1rv (*((regval)0xb8003114))
#define VPE1_IRR1dv (0x00000000)
#define RMOD_VPE1_IRR1(...) rset(VPE1_IRR1, VPE1_IRR1rv, __VA_ARGS__)
#define RIZS_VPE1_IRR1(...) rset(VPE1_IRR1, 0, __VA_ARGS__)
#define RFLD_VPE1_IRR1(fld) (*((const volatile VPE1_IRR1_T *)0xb8003114)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:8; //0
		unsigned int gpio_intr1:4; //0
		unsigned int cpio_intr0:4; //0
		unsigned int gdma1_rs:4; //0
		unsigned int gdma0_rs:4; //0
		unsigned int security_rs:4; //0
		unsigned int pcm1_rs:4; //0
	} f;
	unsigned int v;
} VPE1_IRR2_T;
#define VPE1_IRR2rv (*((regval)0xb8003118))
#define VPE1_IRR2dv (0x00000000)
#define RMOD_VPE1_IRR2(...) rset(VPE1_IRR2, VPE1_IRR2rv, __VA_ARGS__)
#define RIZS_VPE1_IRR2(...) rset(VPE1_IRR2, 0, __VA_ARGS__)
#define RFLD_VPE1_IRR2(fld) (*((const volatile VPE1_IRR2_T *)0xb8003118)).f.fld

typedef union {
	struct {
		unsigned int pcm0_rs:4; //0
		unsigned int pcie1_rs:4; //0
		unsigned int pcie0_rs:4; //0
		unsigned int usb_h2_rs:4; //0
		unsigned int usb_h3_rs:4; //0
		unsigned int uart3_rs:4; //0
		unsigned int uart2_rs:4; //0
		unsigned int uart1_rs:4; //0
	} f;
	unsigned int v;
} VPE1_IRR3_T;
#define VPE1_IRR3rv (*((regval)0xb800311c))
#define VPE1_IRR3dv (0x00000000)
#define RMOD_VPE1_IRR3(...) rset(VPE1_IRR3, VPE1_IRR3rv, __VA_ARGS__)
#define RIZS_VPE1_IRR3(...) rset(VPE1_IRR3, 0, __VA_ARGS__)
#define RFLD_VPE1_IRR3(fld) (*((const volatile VPE1_IRR3_T *)0xb800311c)).f.fld

typedef union {
	struct {
		unsigned int uart0_rs:4; //0
		unsigned int tc5_rs:4; //0
		unsigned int tc4_rs:4; //0
		unsigned int tc3_rs:4; //0
		unsigned int tc2_rs:4; //0
		unsigned int tc1_rs:4; //0
		unsigned int tc0_rs:4; //0
		unsigned int gpio_efgh_rs:4; //0
	} f;
	unsigned int v;
} VPE1_IRR4_T;
#define VPE1_IRR4rv (*((regval)0xb8003120))
#define VPE1_IRR4dv (0x00000000)
#define RMOD_VPE1_IRR4(...) rset(VPE1_IRR4, VPE1_IRR4rv, __VA_ARGS__)
#define RIZS_VPE1_IRR4(...) rset(VPE1_IRR4, 0, __VA_ARGS__)
#define RFLD_VPE1_IRR4(fld) (*((const volatile VPE1_IRR4_T *)0xb8003120)).f.fld

typedef union {
	struct {
		unsigned int gpio_abcd_rs:4; //0
		unsigned int switch_rs:4; //0
		unsigned int sata_rs:4; //0
		unsigned int wdog_ph2to_rs:4; //0
		unsigned int wdog_ph1to_rs:4; //0
		unsigned int fftacc_rs:4; //0
		unsigned int flsh_rs:4; //0
		unsigned int mbz_0:4; //0
	} f;
	unsigned int v;
} VPE1_IRR5_T;
#define VPE1_IRR5rv (*((regval)0xb8003124))
#define VPE1_IRR5dv (0x00000000)
#define RMOD_VPE1_IRR5(...) rset(VPE1_IRR5, VPE1_IRR5rv, __VA_ARGS__)
#define RIZS_VPE1_IRR5(...) rset(VPE1_IRR5, 0, __VA_ARGS__)
#define RFLD_VPE1_IRR5(fld) (*((const volatile VPE1_IRR5_T *)0xb8003124)).f.fld

/*-----------------------------------------------------
 Extraced from file_Timer.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc0data:28; //0
	} f;
	unsigned int v;
} TIMER_CNT0_DATA_T;
#define TIMER_CNT0_DATArv (*((regval)0xb8003200))
#define TIMER_CNT0_DATAdv (0x00000000)
#define RMOD_TIMER_CNT0_DATA(...) rset(TIMER_CNT0_DATA, TIMER_CNT0_DATArv, __VA_ARGS__)
#define RIZS_TIMER_CNT0_DATA(...) rset(TIMER_CNT0_DATA, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT0_DATA(fld) (*((const volatile TIMER_CNT0_DATA_T *)0xb8003200)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc0value:28; //0
	} f;
	unsigned int v;
} TIMER_CNT0_CNT_T;
#define TIMER_CNT0_CNTrv (*((regval)0xb8003204))
#define TIMER_CNT0_CNTdv (0x00000000)
#define RMOD_TIMER_CNT0_CNT(...) rset(TIMER_CNT0_CNT, TIMER_CNT0_CNTrv, __VA_ARGS__)
#define RIZS_TIMER_CNT0_CNT(...) rset(TIMER_CNT0_CNT, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT0_CNT(fld) (*((const volatile TIMER_CNT0_CNT_T *)0xb8003204)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int tc0en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc0mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int tc0divfac:16; //0
	} f;
	unsigned int v;
} TIMER_CNT0_CNTR_T;
#define TIMER_CNT0_CNTRrv (*((regval)0xb8003208))
#define TIMER_CNT0_CNTRdv (0x00000000)
#define RMOD_TIMER_CNT0_CNTR(...) rset(TIMER_CNT0_CNTR, TIMER_CNT0_CNTRrv, __VA_ARGS__)
#define RIZS_TIMER_CNT0_CNTR(...) rset(TIMER_CNT0_CNTR, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT0_CNTR(fld) (*((const volatile TIMER_CNT0_CNTR_T *)0xb8003208)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int tc0ie:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc0ip:1; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} TIMER_CNT0_INT_T;
#define TIMER_CNT0_INTrv (*((regval)0xb800320c))
#define TIMER_CNT0_INTdv (0x00000000)
#define RMOD_TIMER_CNT0_INT(...) rset(TIMER_CNT0_INT, TIMER_CNT0_INTrv, __VA_ARGS__)
#define RIZS_TIMER_CNT0_INT(...) rset(TIMER_CNT0_INT, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT0_INT(fld) (*((const volatile TIMER_CNT0_INT_T *)0xb800320c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc1data:28; //0
	} f;
	unsigned int v;
} TIMER_CNT1_DATA_T;
#define TIMER_CNT1_DATArv (*((regval)0xb8003210))
#define TIMER_CNT1_DATAdv (0x00000000)
#define RMOD_TIMER_CNT1_DATA(...) rset(TIMER_CNT1_DATA, TIMER_CNT1_DATArv, __VA_ARGS__)
#define RIZS_TIMER_CNT1_DATA(...) rset(TIMER_CNT1_DATA, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT1_DATA(fld) (*((const volatile TIMER_CNT1_DATA_T *)0xb8003210)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc1value:28; //0
	} f;
	unsigned int v;
} TIMER_CNT1_CNT_T;
#define TIMER_CNT1_CNTrv (*((regval)0xb8003214))
#define TIMER_CNT1_CNTdv (0x00000000)
#define RMOD_TIMER_CNT1_CNT(...) rset(TIMER_CNT1_CNT, TIMER_CNT1_CNTrv, __VA_ARGS__)
#define RIZS_TIMER_CNT1_CNT(...) rset(TIMER_CNT1_CNT, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT1_CNT(fld) (*((const volatile TIMER_CNT1_CNT_T *)0xb8003214)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int tc1en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc1mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int tc1divfac:16; //0
	} f;
	unsigned int v;
} TIMER_CNT1_CNTR_T;
#define TIMER_CNT1_CNTRrv (*((regval)0xb8003218))
#define TIMER_CNT1_CNTRdv (0x00000000)
#define RMOD_TIMER_CNT1_CNTR(...) rset(TIMER_CNT1_CNTR, TIMER_CNT1_CNTRrv, __VA_ARGS__)
#define RIZS_TIMER_CNT1_CNTR(...) rset(TIMER_CNT1_CNTR, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT1_CNTR(fld) (*((const volatile TIMER_CNT1_CNTR_T *)0xb8003218)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int tc1ie:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc1ip:1; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} TIMER_CNT1_INT_T;
#define TIMER_CNT1_INTrv (*((regval)0xb800321c))
#define TIMER_CNT1_INTdv (0x00000000)
#define RMOD_TIMER_CNT1_INT(...) rset(TIMER_CNT1_INT, TIMER_CNT1_INTrv, __VA_ARGS__)
#define RIZS_TIMER_CNT1_INT(...) rset(TIMER_CNT1_INT, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT1_INT(fld) (*((const volatile TIMER_CNT1_INT_T *)0xb800321c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc2data:28; //0
	} f;
	unsigned int v;
} TIMER_CNT2_DATA_T;
#define TIMER_CNT2_DATArv (*((regval)0xb8003220))
#define TIMER_CNT2_DATAdv (0x00000000)
#define RMOD_TIMER_CNT2_DATA(...) rset(TIMER_CNT2_DATA, TIMER_CNT2_DATArv, __VA_ARGS__)
#define RIZS_TIMER_CNT2_DATA(...) rset(TIMER_CNT2_DATA, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT2_DATA(fld) (*((const volatile TIMER_CNT2_DATA_T *)0xb8003220)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc2value:28; //0
	} f;
	unsigned int v;
} TIMER_CNT2_CNT_T;
#define TIMER_CNT2_CNTrv (*((regval)0xb8003224))
#define TIMER_CNT2_CNTdv (0x00000000)
#define RMOD_TIMER_CNT2_CNT(...) rset(TIMER_CNT2_CNT, TIMER_CNT2_CNTrv, __VA_ARGS__)
#define RIZS_TIMER_CNT2_CNT(...) rset(TIMER_CNT2_CNT, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT2_CNT(fld) (*((const volatile TIMER_CNT2_CNT_T *)0xb8003224)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int tc2en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc2mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int tc2divfac:16; //0
	} f;
	unsigned int v;
} TIMER_CNT2_CNTR_T;
#define TIMER_CNT2_CNTRrv (*((regval)0xb8003228))
#define TIMER_CNT2_CNTRdv (0x00000000)
#define RMOD_TIMER_CNT2_CNTR(...) rset(TIMER_CNT2_CNTR, TIMER_CNT2_CNTRrv, __VA_ARGS__)
#define RIZS_TIMER_CNT2_CNTR(...) rset(TIMER_CNT2_CNTR, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT2_CNTR(fld) (*((const volatile TIMER_CNT2_CNTR_T *)0xb8003228)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int tc2ie:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc2ip:1; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} TIMER_CNT2_INT_T;
#define TIMER_CNT2_INTrv (*((regval)0xb800322c))
#define TIMER_CNT2_INTdv (0x00000000)
#define RMOD_TIMER_CNT2_INT(...) rset(TIMER_CNT2_INT, TIMER_CNT2_INTrv, __VA_ARGS__)
#define RIZS_TIMER_CNT2_INT(...) rset(TIMER_CNT2_INT, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT2_INT(fld) (*((const volatile TIMER_CNT2_INT_T *)0xb800322c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc3data:28; //0
	} f;
	unsigned int v;
} TIMER_CNT3_DATA_T;
#define TIMER_CNT3_DATArv (*((regval)0xb8003230))
#define TIMER_CNT3_DATAdv (0x00000000)
#define RMOD_TIMER_CNT3_DATA(...) rset(TIMER_CNT3_DATA, TIMER_CNT3_DATArv, __VA_ARGS__)
#define RIZS_TIMER_CNT3_DATA(...) rset(TIMER_CNT3_DATA, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT3_DATA(fld) (*((const volatile TIMER_CNT3_DATA_T *)0xb8003230)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc3value:28; //0
	} f;
	unsigned int v;
} TIMER_CNT3_CNT_T;
#define TIMER_CNT3_CNTrv (*((regval)0xb8003234))
#define TIMER_CNT3_CNTdv (0x00000000)
#define RMOD_TIMER_CNT3_CNT(...) rset(TIMER_CNT3_CNT, TIMER_CNT3_CNTrv, __VA_ARGS__)
#define RIZS_TIMER_CNT3_CNT(...) rset(TIMER_CNT3_CNT, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT3_CNT(fld) (*((const volatile TIMER_CNT3_CNT_T *)0xb8003234)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int tc3en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc3mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int tc3divfac:16; //0
	} f;
	unsigned int v;
} TIMER_CNT3_CNTR_T;
#define TIMER_CNT3_CNTRrv (*((regval)0xb8003238))
#define TIMER_CNT3_CNTRdv (0x00000000)
#define RMOD_TIMER_CNT3_CNTR(...) rset(TIMER_CNT3_CNTR, TIMER_CNT3_CNTRrv, __VA_ARGS__)
#define RIZS_TIMER_CNT3_CNTR(...) rset(TIMER_CNT3_CNTR, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT3_CNTR(fld) (*((const volatile TIMER_CNT3_CNTR_T *)0xb8003238)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int tc3ie:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc3ip:1; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} TIMER_CNT3_INT_T;
#define TIMER_CNT3_INTrv (*((regval)0xb800323c))
#define TIMER_CNT3_INTdv (0x00000000)
#define RMOD_TIMER_CNT3_INT(...) rset(TIMER_CNT3_INT, TIMER_CNT3_INTrv, __VA_ARGS__)
#define RIZS_TIMER_CNT3_INT(...) rset(TIMER_CNT3_INT, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT3_INT(fld) (*((const volatile TIMER_CNT3_INT_T *)0xb800323c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc4data:28; //0
	} f;
	unsigned int v;
} TIMER_CNT4_DATA_T;
#define TIMER_CNT4_DATArv (*((regval)0xb8003240))
#define TIMER_CNT4_DATAdv (0x00000000)
#define RMOD_TIMER_CNT4_DATA(...) rset(TIMER_CNT4_DATA, TIMER_CNT4_DATArv, __VA_ARGS__)
#define RIZS_TIMER_CNT4_DATA(...) rset(TIMER_CNT4_DATA, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT4_DATA(fld) (*((const volatile TIMER_CNT4_DATA_T *)0xb8003240)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc4value:28; //0
	} f;
	unsigned int v;
} TIMER_CNT4_CNT_T;
#define TIMER_CNT4_CNTrv (*((regval)0xb8003244))
#define TIMER_CNT4_CNTdv (0x00000000)
#define RMOD_TIMER_CNT4_CNT(...) rset(TIMER_CNT4_CNT, TIMER_CNT4_CNTrv, __VA_ARGS__)
#define RIZS_TIMER_CNT4_CNT(...) rset(TIMER_CNT4_CNT, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT4_CNT(fld) (*((const volatile TIMER_CNT4_CNT_T *)0xb8003244)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int tc4en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc4mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int tc4divfac:16; //0
	} f;
	unsigned int v;
} TIMER_CNT4_CNTR_T;
#define TIMER_CNT4_CNTRrv (*((regval)0xb8003248))
#define TIMER_CNT4_CNTRdv (0x00000000)
#define RMOD_TIMER_CNT4_CNTR(...) rset(TIMER_CNT4_CNTR, TIMER_CNT4_CNTRrv, __VA_ARGS__)
#define RIZS_TIMER_CNT4_CNTR(...) rset(TIMER_CNT4_CNTR, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT4_CNTR(fld) (*((const volatile TIMER_CNT4_CNTR_T *)0xb8003248)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int tc4ie:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc4ip:1; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} TIMER_CNT4_INT_T;
#define TIMER_CNT4_INTrv (*((regval)0xb800324c))
#define TIMER_CNT4_INTdv (0x00000000)
#define RMOD_TIMER_CNT4_INT(...) rset(TIMER_CNT4_INT, TIMER_CNT4_INTrv, __VA_ARGS__)
#define RIZS_TIMER_CNT4_INT(...) rset(TIMER_CNT4_INT, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT4_INT(fld) (*((const volatile TIMER_CNT4_INT_T *)0xb800324c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc5data:28; //0
	} f;
	unsigned int v;
} TIMER_CNT5_DATA_T;
#define TIMER_CNT5_DATArv (*((regval)0xb8003250))
#define TIMER_CNT5_DATAdv (0x00000000)
#define RMOD_TIMER_CNT5_DATA(...) rset(TIMER_CNT5_DATA, TIMER_CNT5_DATArv, __VA_ARGS__)
#define RIZS_TIMER_CNT5_DATA(...) rset(TIMER_CNT5_DATA, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT5_DATA(fld) (*((const volatile TIMER_CNT5_DATA_T *)0xb8003250)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc5value:28; //0
	} f;
	unsigned int v;
} TIMER_CNT5_CNT_T;
#define TIMER_CNT5_CNTrv (*((regval)0xb8003254))
#define TIMER_CNT5_CNTdv (0x00000000)
#define RMOD_TIMER_CNT5_CNT(...) rset(TIMER_CNT5_CNT, TIMER_CNT5_CNTrv, __VA_ARGS__)
#define RIZS_TIMER_CNT5_CNT(...) rset(TIMER_CNT5_CNT, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT5_CNT(fld) (*((const volatile TIMER_CNT5_CNT_T *)0xb8003254)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int tc5en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc5mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int tc5divfac:16; //0
	} f;
	unsigned int v;
} TIMER_CNT5_CNTR_T;
#define TIMER_CNT5_CNTRrv (*((regval)0xb8003258))
#define TIMER_CNT5_CNTRdv (0x00000000)
#define RMOD_TIMER_CNT5_CNTR(...) rset(TIMER_CNT5_CNTR, TIMER_CNT5_CNTRrv, __VA_ARGS__)
#define RIZS_TIMER_CNT5_CNTR(...) rset(TIMER_CNT5_CNTR, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT5_CNTR(fld) (*((const volatile TIMER_CNT5_CNTR_T *)0xb8003258)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int tc5ie:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc5ip:1; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} TIMER_CNT5_INT_T;
#define TIMER_CNT5_INTrv (*((regval)0xb800325c))
#define TIMER_CNT5_INTdv (0x00000000)
#define RMOD_TIMER_CNT5_INT(...) rset(TIMER_CNT5_INT, TIMER_CNT5_INTrv, __VA_ARGS__)
#define RIZS_TIMER_CNT5_INT(...) rset(TIMER_CNT5_INT, 0, __VA_ARGS__)
#define RFLD_TIMER_CNT5_INT(fld) (*((const volatile TIMER_CNT5_INT_T *)0xb800325c)).f.fld

typedef union {
	struct {
		unsigned int wdt_kick:1; //0
		unsigned int mbz_0:31; //0
	} f;
	unsigned int v;
} WDTCNTRR_T;
#define WDTCNTRRrv (*((regval)0xb8003260))
#define WDTCNTRRdv (0x00000000)
#define RMOD_WDTCNTRR(...) rset(WDTCNTRR, WDTCNTRRrv, __VA_ARGS__)
#define RIZS_WDTCNTRR(...) rset(WDTCNTRR, 0, __VA_ARGS__)
#define RFLD_WDTCNTRR(fld) (*((const volatile WDTCNTRR_T *)0xb8003260)).f.fld

typedef union {
	struct {
		unsigned int ph1_ip:1; //0
		unsigned int ph2_ip:1; //0
		unsigned int mbz_0:30; //0
	} f;
	unsigned int v;
} WDTINTRR_T;
#define WDTINTRRrv (*((regval)0xb8003264))
#define WDTINTRRdv (0x00000000)
#define RMOD_WDTINTRR(...) rset(WDTINTRR, WDTINTRRrv, __VA_ARGS__)
#define RIZS_WDTINTRR(...) rset(WDTINTRR, 0, __VA_ARGS__)
#define RFLD_WDTINTRR(fld) (*((const volatile WDTINTRR_T *)0xb8003264)).f.fld

typedef union {
	struct {
		unsigned int wdt_e:1; //0
		unsigned int wdt_clk_sc:2; //0
		unsigned int mbz_0:2; //0
		unsigned int ph1_to:5; //0
		unsigned int mbz_1:2; //0
		unsigned int ph2_to:5; //0
		unsigned int mbz_2:13; //0
		unsigned int wdt_reset_mode:2; //0
	} f;
	unsigned int v;
} WDT_CTRL_T;
#define WDT_CTRLrv (*((regval)0xb8003268))
#define WDT_CTRLdv (0x00000000)
#define RMOD_WDT_CTRL(...) rset(WDT_CTRL, WDT_CTRLrv, __VA_ARGS__)
#define RIZS_WDT_CTRL(...) rset(WDT_CTRL, 0, __VA_ARGS__)
#define RFLD_WDT_CTRL(fld) (*((const volatile WDT_CTRL_T *)0xb8003268)).f.fld

/*-----------------------------------------------------
 Extraced from file_GPIO.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int drc_d:8; //0
		unsigned int drc_c:8; //0
		unsigned int drc_b:8; //0
		unsigned int drc_a:8; //0
	} f;
	unsigned int v;
} PABCD_DIR_T;
#define PABCD_DIRrv (*((regval)0xb8003308))
#define PABCD_DIRdv (0x00000000)
#define RMOD_PABCD_DIR(...) rset(PABCD_DIR, PABCD_DIRrv, __VA_ARGS__)
#define RIZS_PABCD_DIR(...) rset(PABCD_DIR, 0, __VA_ARGS__)
#define RFLD_PABCD_DIR(fld) (*((const volatile PABCD_DIR_T *)0xb8003308)).f.fld

typedef union {
	struct {
		unsigned int pd_d:8; //0
		unsigned int pd_c:8; //0
		unsigned int pd_b:8; //0
		unsigned int pd_a:8; //0
	} f;
	unsigned int v;
} PABCD_DAT_T;
#define PABCD_DATrv (*((regval)0xb800330c))
#define PABCD_DATdv (0x00000000)
#define RMOD_PABCD_DAT(...) rset(PABCD_DAT, PABCD_DATrv, __VA_ARGS__)
#define RIZS_PABCD_DAT(...) rset(PABCD_DAT, 0, __VA_ARGS__)
#define RFLD_PABCD_DAT(fld) (*((const volatile PABCD_DAT_T *)0xb800330c)).f.fld

typedef union {
	struct {
		unsigned int ips_d:8; //0
		unsigned int ips_c:8; //0
		unsigned int ips_b:8; //0
		unsigned int ips_a:8; //0
	} f;
	unsigned int v;
} PABCD_ISR_T;
#define PABCD_ISRrv (*((regval)0xb8003310))
#define PABCD_ISRdv (0x00000000)
#define RMOD_PABCD_ISR(...) rset(PABCD_ISR, PABCD_ISRrv, __VA_ARGS__)
#define RIZS_PABCD_ISR(...) rset(PABCD_ISR, 0, __VA_ARGS__)
#define RFLD_PABCD_ISR(fld) (*((const volatile PABCD_ISR_T *)0xb8003310)).f.fld

typedef union {
	struct {
		unsigned int pb7_im:2; //0
		unsigned int pb6_im:2; //0
		unsigned int pb5_im:2; //0
		unsigned int pb4_im:2; //0
		unsigned int pb3_im:2; //0
		unsigned int pb2_im:2; //0
		unsigned int pb1_im:2; //0
		unsigned int pb0_im:2; //0
		unsigned int pa7_im:2; //0
		unsigned int pa6_im:2; //0
		unsigned int pa5_im:2; //0
		unsigned int pa4_im:2; //0
		unsigned int pa3_im:2; //0
		unsigned int pa2_im:2; //0
		unsigned int pa1_im:2; //0
		unsigned int pa0_im:2; //0
	} f;
	unsigned int v;
} PAB_IMR_T;
#define PAB_IMRrv (*((regval)0xb8003314))
#define PAB_IMRdv (0x00000000)
#define RMOD_PAB_IMR(...) rset(PAB_IMR, PAB_IMRrv, __VA_ARGS__)
#define RIZS_PAB_IMR(...) rset(PAB_IMR, 0, __VA_ARGS__)
#define RFLD_PAB_IMR(fld) (*((const volatile PAB_IMR_T *)0xb8003314)).f.fld

typedef union {
	struct {
		unsigned int pd7_im:2; //0
		unsigned int pd6_im:2; //0
		unsigned int pd5_im:2; //0
		unsigned int pd4_im:2; //0
		unsigned int pd3_im:2; //0
		unsigned int pd2_im:2; //0
		unsigned int pd1_im:2; //0
		unsigned int pd0_im:2; //0
		unsigned int pc7_im:2; //0
		unsigned int pc6_im:2; //0
		unsigned int pc5_im:2; //0
		unsigned int pc4_im:2; //0
		unsigned int pc3_im:2; //0
		unsigned int pc2_im:2; //0
		unsigned int pc1_im:2; //0
		unsigned int pc0_im:2; //0
	} f;
	unsigned int v;
} PCD_IMR_T;
#define PCD_IMRrv (*((regval)0xb8003318))
#define PCD_IMRdv (0x00000000)
#define RMOD_PCD_IMR(...) rset(PCD_IMR, PCD_IMRrv, __VA_ARGS__)
#define RIZS_PCD_IMR(...) rset(PCD_IMR, 0, __VA_ARGS__)
#define RFLD_PCD_IMR(fld) (*((const volatile PCD_IMR_T *)0xb8003318)).f.fld

typedef union {
	struct {
		unsigned int drc_h:8; //0
		unsigned int drc_g:8; //0
		unsigned int drc_f:8; //0
		unsigned int drc_e:8; //0
	} f;
	unsigned int v;
} PEFGH_DIR_T;
#define PEFGH_DIRrv (*((regval)0xb8003324))
#define PEFGH_DIRdv (0x00000000)
#define RMOD_PEFGH_DIR(...) rset(PEFGH_DIR, PEFGH_DIRrv, __VA_ARGS__)
#define RIZS_PEFGH_DIR(...) rset(PEFGH_DIR, 0, __VA_ARGS__)
#define RFLD_PEFGH_DIR(fld) (*((const volatile PEFGH_DIR_T *)0xb8003324)).f.fld

typedef union {
	struct {
		unsigned int ph_h:8; //0
		unsigned int ph_g:8; //0
		unsigned int ph_f:8; //0
		unsigned int ph_e:8; //0
	} f;
	unsigned int v;
} PEFGH_DAT_T;
#define PEFGH_DATrv (*((regval)0xb8003328))
#define PEFGH_DATdv (0x00000000)
#define RMOD_PEFGH_DAT(...) rset(PEFGH_DAT, PEFGH_DATrv, __VA_ARGS__)
#define RIZS_PEFGH_DAT(...) rset(PEFGH_DAT, 0, __VA_ARGS__)
#define RFLD_PEFGH_DAT(fld) (*((const volatile PEFGH_DAT_T *)0xb8003328)).f.fld

typedef union {
	struct {
		unsigned int ips_h:8; //0
		unsigned int ips_g:8; //0
		unsigned int ips_f:8; //0
		unsigned int ips_e:8; //0
	} f;
	unsigned int v;
} PEFGH_ISR_T;
#define PEFGH_ISRrv (*((regval)0xb800332c))
#define PEFGH_ISRdv (0x00000000)
#define RMOD_PEFGH_ISR(...) rset(PEFGH_ISR, PEFGH_ISRrv, __VA_ARGS__)
#define RIZS_PEFGH_ISR(...) rset(PEFGH_ISR, 0, __VA_ARGS__)
#define RFLD_PEFGH_ISR(fld) (*((const volatile PEFGH_ISR_T *)0xb800332c)).f.fld

typedef union {
	struct {
		unsigned int pf7_im:2; //0
		unsigned int pf6_im:2; //0
		unsigned int pf5_im:2; //0
		unsigned int pf4_im:2; //0
		unsigned int pf3_im:2; //0
		unsigned int pf2_im:2; //0
		unsigned int pf1_im:2; //0
		unsigned int pf0_im:2; //0
		unsigned int pe7_im:2; //0
		unsigned int pe6_im:2; //0
		unsigned int pe5_im:2; //0
		unsigned int pe4_im:2; //0
		unsigned int pe3_im:2; //0
		unsigned int pe2_im:2; //0
		unsigned int pe1_im:2; //0
		unsigned int pe0_im:2; //0
	} f;
	unsigned int v;
} PEF_IMR_T;
#define PEF_IMRrv (*((regval)0xb8003330))
#define PEF_IMRdv (0x00000000)
#define RMOD_PEF_IMR(...) rset(PEF_IMR, PEF_IMRrv, __VA_ARGS__)
#define RIZS_PEF_IMR(...) rset(PEF_IMR, 0, __VA_ARGS__)
#define RFLD_PEF_IMR(fld) (*((const volatile PEF_IMR_T *)0xb8003330)).f.fld

typedef union {
	struct {
		unsigned int ph7_im:2; //0
		unsigned int ph6_im:2; //0
		unsigned int ph5_im:2; //0
		unsigned int ph4_im:2; //0
		unsigned int ph3_im:2; //0
		unsigned int ph2_im:2; //0
		unsigned int ph1_im:2; //0
		unsigned int ph0_im:2; //0
		unsigned int pg7_im:2; //0
		unsigned int pg6_im:2; //0
		unsigned int pg5_im:2; //0
		unsigned int pg4_im:2; //0
		unsigned int pg3_im:2; //0
		unsigned int pg2_im:2; //0
		unsigned int pg1_im:2; //0
		unsigned int pg0_im:2; //0
	} f;
	unsigned int v;
} PGH_IMR_T;
#define PGH_IMRrv (*((regval)0xb8003334))
#define PGH_IMRdv (0x00000000)
#define RMOD_PGH_IMR(...) rset(PGH_IMR, PGH_IMRrv, __VA_ARGS__)
#define RIZS_PGH_IMR(...) rset(PGH_IMR, 0, __VA_ARGS__)
#define RFLD_PGH_IMR(fld) (*((const volatile PGH_IMR_T *)0xb8003334)).f.fld

/*-----------------------------------------------------
 Extraced from file_IMEM_DMEM_DMA.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:14; //0
		unsigned int idmem_start_addr:18; //0
	} f;
	unsigned int v;
} IMEM_DMEM_SA_T;
#define IMEM_DMEM_SArv (*((regval)0xb8006000))
#define IMEM_DMEM_SAdv (0x00000000)
#define RMOD_IMEM_DMEM_SA(...) rset(IMEM_DMEM_SA, IMEM_DMEM_SArv, __VA_ARGS__)
#define RIZS_IMEM_DMEM_SA(...) rset(IMEM_DMEM_SA, 0, __VA_ARGS__)
#define RFLD_IMEM_DMEM_SA(fld) (*((const volatile IMEM_DMEM_SA_T *)0xb8006000)).f.fld

typedef union {
	struct {
		unsigned int ext_mem_start_addr:30; //0
		unsigned int mbz_0:2; //0
	} f;
	unsigned int v;
} EXTM_SA_T;
#define EXTM_SArv (*((regval)0xb8006004))
#define EXTM_SAdv (0x00000000)
#define RMOD_EXTM_SA(...) rset(EXTM_SA, EXTM_SArv, __VA_ARGS__)
#define RIZS_EXTM_SA(...) rset(EXTM_SA, 0, __VA_ARGS__)
#define RFLD_EXTM_SA(fld) (*((const volatile EXTM_SA_T *)0xb8006004)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int idmem_sel:3; //0
		unsigned int idmem_ac_mode:1; //0
		unsigned int idmem_dma_rst:1; //0
		unsigned int idmem_dma_start:1; //0
		unsigned int idmem_dma_dir:1; //0
		unsigned int idmem_dma_size:19; //0
	} f;
	unsigned int v;
} DMDMA_CTL_T;
#define DMDMA_CTLrv (*((regval)0xb8006008))
#define DMDMA_CTLdv (0x00000000)
#define RMOD_DMDMA_CTL(...) rset(DMDMA_CTL, DMDMA_CTLrv, __VA_ARGS__)
#define RIZS_DMDMA_CTL(...) rset(DMDMA_CTL, 0, __VA_ARGS__)
#define RFLD_DMDMA_CTL(fld) (*((const volatile DMDMA_CTL_T *)0xb8006008)).f.fld

/*-----------------------------------------------------
 Extraced from file_IPSEC.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int sdsa_sdca:30; //0
		unsigned int mbz_0:2; //0
	} f;
	unsigned int v;
} IPSSDAR_T;
#define IPSSDARrv (*((regval)0xb800c000))
#define IPSSDARdv (0x00000000)
#define RMOD_IPSSDAR(...) rset(IPSSDAR, IPSSDARrv, __VA_ARGS__)
#define RIZS_IPSSDAR(...) rset(IPSSDAR, 0, __VA_ARGS__)
#define RFLD_IPSSDAR(fld) (*((const volatile IPSSDAR_T *)0xb800c000)).f.fld

typedef union {
	struct {
		unsigned int ddsa_ddca:30; //0
		unsigned int mbz_0:2; //0
	} f;
	unsigned int v;
} IPSDDAR_T;
#define IPSDDARrv (*((regval)0xb800c004))
#define IPSDDARdv (0x00000000)
#define RMOD_IPSDDAR(...) rset(IPSDDAR, IPSDDARrv, __VA_ARGS__)
#define RIZS_IPSDDAR(...) rset(IPSDDAR, 0, __VA_ARGS__)
#define RFLD_IPSDDAR(fld) (*((const volatile IPSDDAR_T *)0xb800c004)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:16; //0
		unsigned int sdue:1; //0
		unsigned int sdle:1; //0
		unsigned int ddue:1; //0
		unsigned int ddok:1; //0
		unsigned int dabf:1; //0
		unsigned int mbz_1:2; //0
		unsigned int dbgs:1; //0
		unsigned int mbz_2:6; //0
		unsigned int poll:1; //0
		unsigned int srst:1; //0
	} f;
	unsigned int v;
} IPSCSR_T;
#define IPSCSRrv (*((regval)0xb800c008))
#define IPSCSRdv (0x00000000)
#define RMOD_IPSCSR(...) rset(IPSCSR, IPSCSRrv, __VA_ARGS__)
#define RIZS_IPSCSR(...) rset(IPSCSR, 0, __VA_ARGS__)
#define RFLD_IPSCSR(fld) (*((const volatile IPSCSR_T *)0xb800c008)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:16; //0
		unsigned int sduem:1; //0
		unsigned int sdlem:1; //0
		unsigned int dduem:1; //0
		unsigned int ddokm:1; //0
		unsigned int dabfm:1; //0
		unsigned int mbz_1:2; //0
		unsigned int lbkm:1; //0
		unsigned int sawb:1; //0
		unsigned int cke:1; //0
		unsigned int dmbs:3; //2
		unsigned int smbs:3; //2
	} f;
	unsigned int v;
} IPSCTR_T;
#define IPSCTRrv (*((regval)0xb800c00c))
#define IPSCTRdv (0x00000012)
#define RMOD_IPSCTR(...) rset(IPSCTR, IPSCTRrv, __VA_ARGS__)
#define RIZS_IPSCTR(...) rset(IPSCTR, 0, __VA_ARGS__)
#define RFLD_IPSCTR(fld) (*((const volatile IPSCTR_T *)0xb800c00c)).f.fld

/*-----------------------------------------------------
 Extraced from file_NAND_FLASH.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int nafc_rc:2; //0
		unsigned int nafc_ac:2; //0
		unsigned int nafc_nf:1; //0
		unsigned int debug_select:3; //0
		unsigned int wp_b:1; //1
		unsigned int bch_12t_en:1; //0
		unsigned int bch_24t_en:1; //0
		unsigned int mbz_0:16; //0
		unsigned int ce_trhz:1; //1
		unsigned int ce_tdh:4; //2
	} f;
	unsigned int v;
} NACFR_T;
#define NACFRrv (*((regval)0xb801a000))
#define NACFRdv (0x00800012)
#define RMOD_NACFR(...) rset(NACFR, NACFRrv, __VA_ARGS__)
#define RIZS_NACFR(...) rset(NACFR, 0, __VA_ARGS__)
#define RFLD_NACFR(fld) (*((const volatile NACFR_T *)0xb801a000)).f.fld

typedef union {
	struct {
		unsigned int ready:1; //1
		unsigned int ecc:1; //1
		unsigned int rbo:1; //0
		unsigned int wbo:1; //0
		unsigned int ie:1; //0
		unsigned int mbz_0:7; //0
		unsigned int ce_twp:4; //15
		unsigned int ce_twh:4; //15
		unsigned int ce_trr:4; //15
		unsigned int ce_th:4; //15
		unsigned int ce_ts:4; //15
	} f;
	unsigned int v;
} NACR_T;
#define NACRrv (*((regval)0xb801a004))
#define NACRdv (0xc00fffff)
#define RMOD_NACR(...) rset(NACR, NACRrv, __VA_ARGS__)
#define RIZS_NACR(...) rset(NACR, 0, __VA_ARGS__)
#define RFLD_NACR(fld) (*((const volatile NACR_T *)0xb801a004)).f.fld

typedef union {
	struct {
		unsigned int cecs1:1; //0
		unsigned int cecs0:1; //0
		unsigned int mbz_0:22; //0
		unsigned int ce_cmd:8; //0
	} f;
	unsigned int v;
} NACMR_T;
#define NACMRrv (*((regval)0xb801a008))
#define NACMRdv (0x00000000)
#define RMOD_NACMR(...) rset(NACMR, NACMRrv, __VA_ARGS__)
#define RIZS_NACMR(...) rset(NACMR, 0, __VA_ARGS__)
#define RFLD_NACMR(fld) (*((const volatile NACMR_T *)0xb801a008)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int ennextadcyc:1; //0
		unsigned int ad2en:1; //0
		unsigned int ad1en:1; //0
		unsigned int ad0en:1; //0
		unsigned int ce_addr2:8; //0
		unsigned int ce_addr1:8; //0
		unsigned int ce_addr0:8; //0
	} f;
	unsigned int v;
} NAADR_T;
#define NAADRrv (*((regval)0xb801a00c))
#define NAADRdv (0x00000000)
#define RMOD_NAADR(...) rset(NAADR, NAADRrv, __VA_ARGS__)
#define RIZS_NAADR(...) rset(NAADR, 0, __VA_ARGS__)
#define RFLD_NAADR(fld) (*((const volatile NAADR_T *)0xb801a00c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:23; //0
		unsigned int tag_sel:2; //0
		unsigned int tag_dis:1; //1
		unsigned int decs1:1; //0
		unsigned int decs0:1; //0
		unsigned int dmare:1; //0
		unsigned int dmawe:1; //0
		unsigned int lbc_bsz:2; //3
	} f;
	unsigned int v;
} NADCRR_T;
#define NADCRRrv (*((regval)0xb801a010))
#define NADCRRdv (0x00000043)
#define RMOD_NADCRR(...) rset(NADCRR, NADCRRrv, __VA_ARGS__)
#define RIZS_NADCRR(...) rset(NADCRR, 0, __VA_ARGS__)
#define RFLD_NADCRR(fld) (*((const volatile NADCRR_T *)0xb801a010)).f.fld

typedef union {
	struct {
		unsigned int data3:8; //0
		unsigned int data2:8; //0
		unsigned int data1:8; //0
		unsigned int data0:8; //0
	} f;
	unsigned int v;
} NADR_T;
#define NADRrv (*((regval)0xb801a014))
#define NADRdv (0x00000000)
#define RMOD_NADR(...) rset(NADR, NADRrv, __VA_ARGS__)
#define RIZS_NADR(...) rset(NADR, 0, __VA_ARGS__)
#define RFLD_NADR(fld) (*((const volatile NADR_T *)0xb801a014)).f.fld

typedef union {
	struct {
		unsigned int addr:32; //0
	} f;
	unsigned int v;
} NADFSAR_T;
#define NADFSARrv (*((regval)0xb801a018))
#define NADFSARdv (0x00000000)
#define RMOD_NADFSAR(...) rset(NADFSAR, NADFSARrv, __VA_ARGS__)
#define RIZS_NADFSAR(...) rset(NADFSAR, 0, __VA_ARGS__)
#define RFLD_NADFSAR(fld) (*((const volatile NADFSAR_T *)0xb801a018)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:29; //0
		unsigned int addr:3; //0
	} f;
	unsigned int v;
} NADFSAR2_T;
#define NADFSAR2rv (*((regval)0xb801a01c))
#define NADFSAR2dv (0x00000000)
#define RMOD_NADFSAR2(...) rset(NADFSAR2, NADFSAR2rv, __VA_ARGS__)
#define RIZS_NADFSAR2(...) rset(NADFSAR2, 0, __VA_ARGS__)
#define RFLD_NADFSAR2(fld) (*((const volatile NADFSAR2_T *)0xb801a01c)).f.fld

typedef union {
	struct {
		unsigned int addr:32; //2680160256
	} f;
	unsigned int v;
} NADRSAR_T;
#define NADRSARrv (*((regval)0xb801a020))
#define NADRSARdv (0x9fc00000)
#define RMOD_NADRSAR(...) rset(NADRSAR, NADRSARrv, __VA_ARGS__)
#define RIZS_NADRSAR(...) rset(NADRSAR, 0, __VA_ARGS__)
#define RFLD_NADRSAR(fld) (*((const volatile NADRSAR_T *)0xb801a020)).f.fld

typedef union {
	struct {
		unsigned int addr:32; //0
	} f;
	unsigned int v;
} NADTSAR_T;
#define NADTSARrv (*((regval)0xb801a024))
#define NADTSARdv (0x00000000)
#define RMOD_NADTSAR(...) rset(NADTSAR, NADTSARrv, __VA_ARGS__)
#define RIZS_NADTSAR(...) rset(NADTSAR, 0, __VA_ARGS__)
#define RFLD_NADTSAR(fld) (*((const volatile NADTSAR_T *)0xb801a024)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:23; //0
		unsigned int all_one:1; //0
		unsigned int necn:4; //0
		unsigned int nrer:1; //0
		unsigned int nwer:1; //0
		unsigned int ndrs:1; //0
		unsigned int ndws:1; //0
	} f;
	unsigned int v;
} NASR_T;
#define NASRrv (*((regval)0xb801a028))
#define NASRdv (0x00000000)
#define RMOD_NASR(...) rset(NASR, NASRrv, __VA_ARGS__)
#define RIZS_NASR(...) rset(NASR, 0, __VA_ARGS__)
#define RFLD_NASR(fld) (*((const volatile NASR_T *)0xb801a028)).f.fld

typedef union {
	struct {
		unsigned int napr1_1:32; //0
	} f;
	unsigned int v;
} NAPR1_1_T;
#define NAPR1_1rv (*((regval)0xb801a03c))
#define NAPR1_1dv (0x00000000)
#define RMOD_NAPR1_1(...) rset(NAPR1_1, NAPR1_1rv, __VA_ARGS__)
#define RIZS_NAPR1_1(...) rset(NAPR1_1, 0, __VA_ARGS__)
#define RFLD_NAPR1_1(fld) (*((const volatile NAPR1_1_T *)0xb801a03c)).f.fld

typedef union {
	struct {
		unsigned int napr1_2:32; //0
	} f;
	unsigned int v;
} NAPR1_2_T;
#define NAPR1_2rv (*((regval)0xb801a040))
#define NAPR1_2dv (0x00000000)
#define RMOD_NAPR1_2(...) rset(NAPR1_2, NAPR1_2rv, __VA_ARGS__)
#define RIZS_NAPR1_2(...) rset(NAPR1_2, 0, __VA_ARGS__)
#define RFLD_NAPR1_2(fld) (*((const volatile NAPR1_2_T *)0xb801a040)).f.fld

typedef union {
	struct {
		unsigned int napr1_3:32; //0
	} f;
	unsigned int v;
} NAPR1_3_T;
#define NAPR1_3rv (*((regval)0xb801a044))
#define NAPR1_3dv (0x00000000)
#define RMOD_NAPR1_3(...) rset(NAPR1_3, NAPR1_3rv, __VA_ARGS__)
#define RIZS_NAPR1_3(...) rset(NAPR1_3, 0, __VA_ARGS__)
#define RFLD_NAPR1_3(fld) (*((const volatile NAPR1_3_T *)0xb801a044)).f.fld

typedef union {
	struct {
		unsigned int napr2_1:32; //0
	} f;
	unsigned int v;
} NAPR2_1_T;
#define NAPR2_1rv (*((regval)0xb801a048))
#define NAPR2_1dv (0x00000000)
#define RMOD_NAPR2_1(...) rset(NAPR2_1, NAPR2_1rv, __VA_ARGS__)
#define RIZS_NAPR2_1(...) rset(NAPR2_1, 0, __VA_ARGS__)
#define RFLD_NAPR2_1(fld) (*((const volatile NAPR2_1_T *)0xb801a048)).f.fld

typedef union {
	struct {
		unsigned int napr2_2:32; //0
	} f;
	unsigned int v;
} NAPR2_2_T;
#define NAPR2_2rv (*((regval)0xb801a04c))
#define NAPR2_2dv (0x00000000)
#define RMOD_NAPR2_2(...) rset(NAPR2_2, NAPR2_2rv, __VA_ARGS__)
#define RIZS_NAPR2_2(...) rset(NAPR2_2, 0, __VA_ARGS__)
#define RFLD_NAPR2_2(fld) (*((const volatile NAPR2_2_T *)0xb801a04c)).f.fld

typedef union {
	struct {
		unsigned int napr2_3:32; //0
	} f;
	unsigned int v;
} NAPR2_3_T;
#define NAPR2_3rv (*((regval)0xb801a050))
#define NAPR2_3dv (0x00000000)
#define RMOD_NAPR2_3(...) rset(NAPR2_3, NAPR2_3rv, __VA_ARGS__)
#define RIZS_NAPR2_3(...) rset(NAPR2_3, 0, __VA_ARGS__)
#define RFLD_NAPR2_3(fld) (*((const volatile NAPR2_3_T *)0xb801a050)).f.fld

typedef union {
	struct {
		unsigned int napr3_1:32; //0
	} f;
	unsigned int v;
} NAPR3_1_T;
#define NAPR3_1rv (*((regval)0xb801a054))
#define NAPR3_1dv (0x00000000)
#define RMOD_NAPR3_1(...) rset(NAPR3_1, NAPR3_1rv, __VA_ARGS__)
#define RIZS_NAPR3_1(...) rset(NAPR3_1, 0, __VA_ARGS__)
#define RFLD_NAPR3_1(fld) (*((const volatile NAPR3_1_T *)0xb801a054)).f.fld

typedef union {
	struct {
		unsigned int napr3_2:32; //0
	} f;
	unsigned int v;
} NAPR3_2_T;
#define NAPR3_2rv (*((regval)0xb801a058))
#define NAPR3_2dv (0x00000000)
#define RMOD_NAPR3_2(...) rset(NAPR3_2, NAPR3_2rv, __VA_ARGS__)
#define RIZS_NAPR3_2(...) rset(NAPR3_2, 0, __VA_ARGS__)
#define RFLD_NAPR3_2(fld) (*((const volatile NAPR3_2_T *)0xb801a058)).f.fld

typedef union {
	struct {
		unsigned int napr3_3:32; //0
	} f;
	unsigned int v;
} NAPR3_3_T;
#define NAPR3_3rv (*((regval)0xb801a05c))
#define NAPR3_3dv (0x00000000)
#define RMOD_NAPR3_3(...) rset(NAPR3_3, NAPR3_3rv, __VA_ARGS__)
#define RIZS_NAPR3_3(...) rset(NAPR3_3, 0, __VA_ARGS__)
#define RFLD_NAPR3_3(fld) (*((const volatile NAPR3_3_T *)0xb801a05c)).f.fld

typedef union {
	struct {
		unsigned int napr4_1:32; //0
	} f;
	unsigned int v;
} NAPR4_1_T;
#define NAPR4_1rv (*((regval)0xb801a060))
#define NAPR4_1dv (0x00000000)
#define RMOD_NAPR4_1(...) rset(NAPR4_1, NAPR4_1rv, __VA_ARGS__)
#define RIZS_NAPR4_1(...) rset(NAPR4_1, 0, __VA_ARGS__)
#define RFLD_NAPR4_1(fld) (*((const volatile NAPR4_1_T *)0xb801a060)).f.fld

typedef union {
	struct {
		unsigned int napr4_2:32; //0
	} f;
	unsigned int v;
} NAPR4_2_T;
#define NAPR4_2rv (*((regval)0xb801a064))
#define NAPR4_2dv (0x00000000)
#define RMOD_NAPR4_2(...) rset(NAPR4_2, NAPR4_2rv, __VA_ARGS__)
#define RIZS_NAPR4_2(...) rset(NAPR4_2, 0, __VA_ARGS__)
#define RFLD_NAPR4_2(fld) (*((const volatile NAPR4_2_T *)0xb801a064)).f.fld

typedef union {
	struct {
		unsigned int napr4_3:32; //0
	} f;
	unsigned int v;
} NAPR4_3_T;
#define NAPR4_3rv (*((regval)0xb801a068))
#define NAPR4_3dv (0x00000000)
#define RMOD_NAPR4_3(...) rset(NAPR4_3, NAPR4_3rv, __VA_ARGS__)
#define RIZS_NAPR4_3(...) rset(NAPR4_3, 0, __VA_ARGS__)
#define RFLD_NAPR4_3(fld) (*((const volatile NAPR4_3_T *)0xb801a068)).f.fld

/*-----------------------------------------------------
 Extraced from file_OCP_LX_BRIDGE.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int dbg_ctrl_sel:8; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} OCP_LX_BRIDGE_DBG_T;
#define OCP_LX_BRIDGE_DBGrv (*((regval)0xb8141000))
#define OCP_LX_BRIDGE_DBGdv (0x00000000)
#define RMOD_OCP_LX_BRIDGE_DBG(...) rset(OCP_LX_BRIDGE_DBG, OCP_LX_BRIDGE_DBGrv, __VA_ARGS__)
#define RIZS_OCP_LX_BRIDGE_DBG(...) rset(OCP_LX_BRIDGE_DBG, 0, __VA_ARGS__)
#define RFLD_OCP_LX_BRIDGE_DBG(fld) (*((const volatile OCP_LX_BRIDGE_DBG_T *)0xb8141000)).f.fld

#endif
