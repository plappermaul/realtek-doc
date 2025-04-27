#ifndef _INIT_RESULT_HELPER_H_
#define _INIT_RESULT_HELPER_H_

#define ISTAT_SET(type, stat) do { ((mem_init_result_t *)(&_soc.dram_init_result))->f.type = stat; } while(0)
#define ISTAT_GET(type) (((mem_init_result_t *)(&_soc.dram_init_result))->f.type)
#define ISTAT_STR(type) (init_result_message_##type[ISTAT_GET(type)])
#define INIT_RESULT_GROUP(grp, ...) IR_NARG(__VA_ARGS__)(grp, __VA_ARGS__)

#define IR_NARG(...)  PP_NARG_(__VA_ARGS__,PP_IRSET_N())
#define PP_IRSET_N()                                          _irset63, _irset62, _irset61, _irset60, \
  _irset59, _irset58, _irset57, _irset56, _irset55, _irset54, _irset53, _irset52, _irset51, _irset50, \
  _irset49, _irset48, _irset47, _irset46, _irset45, _irset44, _irset43, _irset42, _irset41, _irset40, \
  _irset39, _irset38, _irset37, _irset36, _irset35, _irset34, _irset33, _irset32, _irset31, _irset30, \
  _irset29, _irset28, _irset27, _irset26, _irset25, _irset24, _irset23, _irset22, _irset21, _irset20, \
  _irset19, _irset18, _irset17, _irset16, _irset15, _irset14, _irset13, _irset12, _irset11, _irset10, \
  _irset09, _irset08, _irset07, _irset06, _irset05, _irset04, _irset03, _irset02, _irset01,       00

#define _irset01(grp, r1) \
	typedef enum { \
		r1 = 0, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
	}

#define _irset02(grp, r1, r2) \
	typedef enum { \
		r1 = 0, \
		r2, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
	}

#define _irset03(grp, r1, r2, r3) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
	}

#define _irset04(grp, r1, r2, r3, r4) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
	}

#define _irset05(grp, r1, r2, r3, r4, r5) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
	}

#define _irset06(grp, r1, r2, r3, r4, r5, r6) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
	}

#define _irset07(grp, r1, r2, r3, r4, r5, r6, r7) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
	}

#define _irset08(grp, r1, r2, r3, r4, r5, r6, r7, r8) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
	}

#define _irset09(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
	}

#define _irset10(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
	}

#define _irset11(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
	}

#define _irset12(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
	}

#define _irset13(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
	}

#define _irset14(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
	}

#define _irset15(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
	}

#define _irset16(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
	}

#define _irset17(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
	}

#define _irset18(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
	}

#define _irset19(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
	}

#define _irset20(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
	}

#define _irset21(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
	}

#define _irset22(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
	}

#define _irset23(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
	}

#define _irset24(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
	}

#define _irset25(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
	}

#define _irset26(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
	}

#define _irset27(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
	}

#define _irset28(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
	}

#define _irset29(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
	}

#define _irset30(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
	}

#define _irset31(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
	}

#define _irset32(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
	}

#define _irset33(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
	}

#define _irset34(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
	}

#define _irset35(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
	}

#define _irset36(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
	}

#define _irset37(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
	}

#define _irset38(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
	}

#define _irset39(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
	}

#define _irset40(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
	}

#define _irset41(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
	}

#define _irset42(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
	}

#define _irset43(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
	}

#define _irset44(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
	}

#define _irset45(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
	}

#define _irset46(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
	}

#define _irset47(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
	}

#define _irset48(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
	}

#define _irset49(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
	}

#define _irset50(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
	}

#define _irset51(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
	}

#define _irset52(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51, r52) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
		r52, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
		#r52, \
	}

#define _irset53(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51, r52, r53) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
		r52, \
		r53, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
		#r52, \
		#r53, \
	}

#define _irset54(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51, r52, r53, r54) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
		r52, \
		r53, \
		r54, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
		#r52, \
		#r53, \
		#r54, \
	}

#define _irset55(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51, r52, r53, r54, r55) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
		r52, \
		r53, \
		r54, \
		r55, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
		#r52, \
		#r53, \
		#r54, \
		#r55, \
	}

#define _irset56(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51, r52, r53, r54, r55, r56) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
		r52, \
		r53, \
		r54, \
		r55, \
		r56, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
		#r52, \
		#r53, \
		#r54, \
		#r55, \
		#r56, \
	}

#define _irset57(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51, r52, r53, r54, r55, r56, r57) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
		r52, \
		r53, \
		r54, \
		r55, \
		r56, \
		r57, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
		#r52, \
		#r53, \
		#r54, \
		#r55, \
		#r56, \
		#r57, \
	}

#define _irset58(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51, r52, r53, r54, r55, r56, r57, r58) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
		r52, \
		r53, \
		r54, \
		r55, \
		r56, \
		r57, \
		r58, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
		#r52, \
		#r53, \
		#r54, \
		#r55, \
		#r56, \
		#r57, \
		#r58, \
	}

#define _irset59(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51, r52, r53, r54, r55, r56, r57, r58, r59) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
		r52, \
		r53, \
		r54, \
		r55, \
		r56, \
		r57, \
		r58, \
		r59, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
		#r52, \
		#r53, \
		#r54, \
		#r55, \
		#r56, \
		#r57, \
		#r58, \
		#r59, \
	}

#define _irset60(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51, r52, r53, r54, r55, r56, r57, r58, r59, r60) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
		r52, \
		r53, \
		r54, \
		r55, \
		r56, \
		r57, \
		r58, \
		r59, \
		r60, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
		#r52, \
		#r53, \
		#r54, \
		#r55, \
		#r56, \
		#r57, \
		#r58, \
		#r59, \
		#r60, \
	}

#define _irset61(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51, r52, r53, r54, r55, r56, r57, r58, r59, r60, r61) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
		r52, \
		r53, \
		r54, \
		r55, \
		r56, \
		r57, \
		r58, \
		r59, \
		r60, \
		r61, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
		#r52, \
		#r53, \
		#r54, \
		#r55, \
		#r56, \
		#r57, \
		#r58, \
		#r59, \
		#r60, \
		#r61, \
	}

#define _irset62(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51, r52, r53, r54, r55, r56, r57, r58, r59, r60, r61, r62) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
		r52, \
		r53, \
		r54, \
		r55, \
		r56, \
		r57, \
		r58, \
		r59, \
		r60, \
		r61, \
		r62, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
		#r52, \
		#r53, \
		#r54, \
		#r55, \
		#r56, \
		#r57, \
		#r58, \
		#r59, \
		#r60, \
		#r61, \
		#r62, \
	}

#define _irset63(grp, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31, r32, r33, r34, r35, r36, r37, r38, r39, r40, r41, r42, r43, r44, r45, r46, r47, r48, r49, r50, r51, r52, r53, r54, r55, r56, r57, r58, r59, r60, r61, r62, r63) \
	typedef enum { \
		r1 = 0, \
		r2, \
		r3, \
		r4, \
		r5, \
		r6, \
		r7, \
		r8, \
		r9, \
		r10, \
		r11, \
		r12, \
		r13, \
		r14, \
		r15, \
		r16, \
		r17, \
		r18, \
		r19, \
		r20, \
		r21, \
		r22, \
		r23, \
		r24, \
		r25, \
		r26, \
		r27, \
		r28, \
		r29, \
		r30, \
		r31, \
		r32, \
		r33, \
		r34, \
		r35, \
		r36, \
		r37, \
		r38, \
		r39, \
		r40, \
		r41, \
		r42, \
		r43, \
		r44, \
		r45, \
		r46, \
		r47, \
		r48, \
		r49, \
		r50, \
		r51, \
		r52, \
		r53, \
		r54, \
		r55, \
		r56, \
		r57, \
		r58, \
		r59, \
		r60, \
		r61, \
		r62, \
		r63, \
	} init_result_##grp; \
	const static s8_t *init_result_message_##grp[] __attribute__ ((unused)) = { \
		#r1, \
		#r2, \
		#r3, \
		#r4, \
		#r5, \
		#r6, \
		#r7, \
		#r8, \
		#r9, \
		#r10, \
		#r11, \
		#r12, \
		#r13, \
		#r14, \
		#r15, \
		#r16, \
		#r17, \
		#r18, \
		#r19, \
		#r20, \
		#r21, \
		#r22, \
		#r23, \
		#r24, \
		#r25, \
		#r26, \
		#r27, \
		#r28, \
		#r29, \
		#r30, \
		#r31, \
		#r32, \
		#r33, \
		#r34, \
		#r35, \
		#r36, \
		#r37, \
		#r38, \
		#r39, \
		#r40, \
		#r41, \
		#r42, \
		#r43, \
		#r44, \
		#r45, \
		#r46, \
		#r47, \
		#r48, \
		#r49, \
		#r50, \
		#r51, \
		#r52, \
		#r53, \
		#r54, \
		#r55, \
		#r56, \
		#r57, \
		#r58, \
		#r59, \
		#r60, \
		#r61, \
		#r62, \
		#r63, \
	}

#endif
