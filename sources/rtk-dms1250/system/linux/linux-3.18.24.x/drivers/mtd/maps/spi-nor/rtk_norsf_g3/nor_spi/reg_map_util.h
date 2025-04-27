#ifndef _REG_MAP_UTIL_H_
#define _REG_MAP_UTIL_H_

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
#endif
