#ifndef _SWCORE_REG_H_
#define _SWCORE_REG_H_
#include <reg_skc35.h>

/* in apro, [5:4] is model_char_3rd; while [4] in 9603cvd stands for test_cut. */
/* to avoid confusion, set it as DNC until further requests. */
SKC35_REG_DEC(
	MODEL_NAME_INFO, 0xbb010000,
	RFIELD(31, 16, rtl_id);
	RFIELD(15, 11, model_char_1st);
	RFIELD(10, 6, model_char_2nd);
	RF_DNC(5, 4);
	RFIELD(3, 0, rtl_vid);
	);

/* for apro only */
SKC35_REG_DEC(
	SCT, 0xbb010008,
	RFIELD(31, 28, en);
	RF_RSV(27, 5);
	RFIELD(4, 0, sct);
	);

SKC35_REG_DEC(
	IO_MODE_EN_apro, 0xbb023014,
	RF_RSV(31, 23);
	RF_DNC(22, 5);
	RFIELD(4, 4, uart0_en);
	RF_DNC(3, 0);
	);

/* 9603cvd */
SKC35_REG_DEC(
	IO_MODE_EN_9603cvd, 0xbb023014,
	RF_RSV(31, 21);
	RF_DNC(20, 7);
	RFIELD(6, 6, uart0_en);
	RF_DNC(5, 0);
	);
#endif
