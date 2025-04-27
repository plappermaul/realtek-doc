#ifndef _MEMCNTLR_UTIL_
#define _MEMCNTLR_UTIL_

#ifndef MEMCNTLR_SECTION
#define MEMCNTLR_SECTION
#endif

#ifndef MEMCNTLR_DATA_SECTION
#define MEMCNTLR_DATA_SECTION
#endif

#define SECTION_DRAM_PROBE_FUNC __attribute__ ((section (".dram_probe_func")))
#define REG_DRAM_PROBE_FUNC(fn) dram_probe_t* __dp_ ## ## fn ## _ \
	SECTION_DRAM_PROBE_FUNC = (dram_probe_t*) fn

#define MEMERR() do { __asm__ __volatile__ ("sdbbp;"); } while (1)

u32_t cg_query_freq(u32_t);
#define GET_MEM_MHZ() cg_query_freq(1)

/* Macros for benzo.py */
#define ARY(var, bnum, q) s##bnum##_t var[q]
#define FLD(var, b)    s32_t var:b
#define BOOL(var)      FLD(var, 2)

#define ARYR(var, tpe, q) tpe var[q]
#define FLDR(var, tpe)    tpe var

MEMCNTLR_SECTION void mc_info_probe(void);
MEMCNTLR_SECTION void mc_info_translation(void);
MEMCNTLR_SECTION void mc_info_to_reg(void);
MEMCNTLR_SECTION void mc_cntlr_zq_calibration(void);
MEMCNTLR_SECTION void mc_calibration(void);
MEMCNTLR_SECTION void mc_result_decode(void);
MEMCNTLR_SECTION u32_t mc_zqc_retry(u32_t odtp, u32_t odtn, u32_t ocdp, u32_t ocdn, u32_t zqc_cnt);

/* Auto-K Helper (akh) functions */
MEMCNTLR_SECTION u32_t mc_akh_get_zq_setting(u32_t orig);
MEMCNTLR_SECTION u32_t mc_akh_is_ddr2_drv_reduced(u32_t orig);
MEMCNTLR_SECTION u32_t mc_akh_get_ddr2_odt_setting(u32_t orig);
MEMCNTLR_SECTION u32_t mc_akh_is_override_autok(void);
MEMCNTLR_SECTION u32_t mc_akh_rx_win_sel_mul(u32_t orig);
MEMCNTLR_SECTION u32_t mc_akh_rx_win_sel_div(u32_t orig);

#include <init_result_helper.h>

INIT_RESULT_GROUP(probe,
                  MEM_PROBE_UNINIT,
                  MEM_PROBE_OK,
                  MEM_PROBE_FAIL,
                  MEM_PROBE_FUNC_MISSING);

INIT_RESULT_GROUP(xlat,
                  MEM_XLAT_UNINIT,
                  MEM_XLAT_OK,
                  MEM_DRAM_TYPE_ERROR,
                  MEM_BANK_NUMBER_ERROR,
                  MEM_BUS_WIDTH_ERROR,
                  MEM_ROW_NUM_ERROR,
                  MEM_COL_NUM_ERROR,
                  MEM_CL_ERROR,
                  MEM_CWL_ERROR,
                  MEM_WR_ERROR,
                  MEM_TREFI_ERROR,
                  MEM_TCK_ERROR,
                  MEM_MR0_ERROR,
                  MEM_MR1_ERROR,
                  MEM_MR2_ERROR,
                  MEM_MR3_ERROR,
                  MEM_CNTLR_ODT_OCD_ERROR);

INIT_RESULT_GROUP(cntlr_zq,
                  MEM_CNTLR_ZQ_UNINIT,
                  MEM_CNTLR_ZQ_STRICT_OK,
                  MEM_CNTLR_ZQ_TIMEOUT,
                  MEM_CNTLR_ZQ_ERROR,
                  MEM_CNTLR_ZQ_RETRIED_STRICT_OK,
                  MEM_CNTLR_ZQ_RELAXED_OK);

INIT_RESULT_GROUP(to_reg,
                  MEM_TO_REG_UNINIT,
                  MEM_TO_REG_OK);

INIT_RESULT_GROUP(cal,
                  MEM_CAL_UNINIT,
                  MEM_CAL_OK,
                  MEM_CAL_FAIL);
#endif
