#ifndef DSS_H
#define DSS_H

#include <reg_map_util.h>

// first dss
typedef union {
	struct {
		unsigned int mbz:6; //0
		unsigned int data_in:20; //0
		unsigned int wire_sel:1; //0
		unsigned int ro_sel:3; //0
		unsigned int dss_en:1; //0
		unsigned int dss_rst_n:1; //0
	} f;
	unsigned int v;
} DSS0_CTRL0_T;

#define DSS0_CTRL0rv (*((regval)0xb8000250))
#define DSS0_CTRL0dv (0x00000000)
#define RMOD_DSS0_CTRL0(...) rset(DSS0_CTRL0, DSS0_CTRL0rv, __VA_ARGS__)
#define RIZS_DSS0_CTRL0(...) rset(DSS0_CTRL0, 0, __VA_ARGS__)
#define RFLD_DSS0_CTRL0(fld) (*((const volatile DSS0_CTRL0_T *)0xb8000250)).f.fld

// second dss
typedef union {
	struct {
		unsigned int mbz:6; //0
		unsigned int data_in:20; //0
		unsigned int wire_sel:1; //0
		unsigned int ro_sel:3; //0
		unsigned int dss_en:1; //0
		unsigned int dss_rst_n:1; //0
	} f;
	unsigned int v;
} DSS1_CTRL0_T;

#define DSS1_CTRL0rv (*((regval)0xb800025c))
#define DSS1_CTRL0dv (0x00000000)
#define RMOD_DSS1_CTRL0(...) rset(DSS1_CTRL0, DSS1_CTRL0rv, __VA_ARGS__)
#define RIZS_DSS1_CTRL0(...) rset(DSS1_CTRL0, 0, __VA_ARGS__)
#define RFLD_DSS1_CTRL0(fld) (*((const volatile DSS1_CTRL0_T *)0xb800025c)).f.fld

// first dss
typedef union {
	struct {
		unsigned int mbz:1; //0
		unsigned int ready:1; //0
		unsigned int wsort_go:1; //0
		unsigned int mbz2:9; //0
		unsigned int count_out:20; //0
	} f;
	unsigned int v;
} DSS0_CTRL1_T;

#define DSS0_CTRL1rv (*((regval)0xb8000260))
#define DSS0_CTRL1dv (0x00000000)
#define RMOD_DSS0_CTRL1(...) rset(DSS0_CTRL1, DSS0_CTRL1rv, __VA_ARGS__)
#define RIZS_DSS0_CTRL1(...) rset(DSS0_CTRL1, 0, __VA_ARGS__)
#define RFLD_DSS0_CTRL1(fld) (*((const volatile DSS0_CTRL1_T *)0xb8000260)).f.fld


typedef union {
	struct {
		unsigned int mbz:16; //0
		unsigned int dss_dbgo:16; //0
	} f;
	unsigned int v;
} DSS0_CTRL2_T;

#define DSS0_CTRL2rv (*((regval)0xb8000264))
#define DSS0_CTRL2dv (0x00000000)
#define RMOD_DSS0_CTRL2(...) rset(DSS0_CTRL2, DSS1_CTRL4rv, __VA_ARGS__)
#define RIZS_DSS0_CTRL2(...) rset(DSS0_CTRL2, 0, __VA_ARGS__)
#define RFLD_DSS0_CTRL2(fld) (*((const volatile DSS0_CTRL2_T *)0xb8000264)).f.fld

typedef union {
	struct {
		unsigned int mbz:1; //0
		unsigned int ready:1; //0
		unsigned int wsort_go:1; //0
		unsigned int mbz2:9; //0
		unsigned int count_out:20; //0
	} f;
	unsigned int v;
} DSS1_CTRL1_T;

#define DSS1_CTRL1rv (*((regval)0xb8000268))
#define DSS1_CTRL1dv (0x00000000)
#define RMOD_DSS1_CTRL1(...) rset(DSS1_CTRL1, DSS1_CTRL1rv, __VA_ARGS__)
#define RIZS_DSS1_CTRL1(...) rset(DSS1_CTRL1, 0, __VA_ARGS__)
#define RFLD_DSS1_CTRL1(fld) (*((const volatile DSS1_CTRL1_T *)0xb8000268)).f.fld


typedef union {
	struct {
		unsigned int mbz:16; //0
		unsigned int dss_dbgo:16; //0
	} f;
	unsigned int v;
} DSS1_CTRL2_T;

#define DSS1_CTRL2rv (*((regval)0xb800026c))
#define DSS1_CTRL2dv (0x00000000)
#define RMOD_DSS1_CTRL2(...) rset(DSS1_CTRL2, DSS1_CTRL2rv, __VA_ARGS__)
#define RIZS_DSS1_CTRL2(...) rset(DSS1_CTRL2, 0, __VA_ARGS__)
#define RFLD_DSS1_CTRL2(fld) (*((const volatile DSS1_CTRL2_T *)0xb800026c)).f.fld
#endif




