#ifndef CPU_H
#define CPU_H

#include <cpu/regdef.h>

#ifndef ASM_NL
    #define ASM_NL              ";\n\t"
#endif

#define BPCTL_REG $4
#define CCTL_REG $20
#define CACHE_OP_D_INV		(0x11)
#define CACHE_OP_D_WB_INV	(0x15)
#define CACHE_OP_D_WB		(0x19)
#define CACHE_OP_I_INV		(0x10)
#define DCACHE_LINE_SIZE	(CACHELINE_SIZE)
#define ICACHE_LINE_SIZE	(CACHELINE_SIZE)


    
#define CPU_BASIC_INIT    \
    /* Clear watch registers */\
	mtc0	zero, CP0_WATCHLO;\
	mtc0	zero, CP0_WATCHHI;\
    /* STATUS register */\
	mfc0	k0, CP0_STATUS;\
	li		k1, ~CP0_STATUS_IE;\
	and		k0, k1;\
	mtc0	k0, CP0_STATUS;\
	/* (Watch Pending), SW0/1 should be cleared */\
	mtc0	zero, CP0_CAUSE;\
	/* Timer */\
	mtc0	zero, CP0_COUNT;\
	mtc0	zero, CP0_COMPARE;\

	
#define CACHE_INIT  \
    jal     rlx5281_cache_reset;\
    nop;\
    jal     rlx5281_dis_wb_buf_merge;\
    nop;\


#define __asm_mfc0(mfc_reg, mfc_sel) ({\
    unsigned int __ret;\
    __asm__ __volatile__ (\
        "mfc0 %0," TO_STR(mfc_reg) "," TO_STR(mfc_sel) ";\n\t"\
        : "=r" (__ret));\
        __ret;})

#define __asm_mtc0(value, mtc_reg, mtc_sel) ({\
    unsigned int __value=(value);\
    __asm__ __volatile__ (\
        "mtc0 %0, " TO_STR(mtc_reg) "," TO_STR(mtc_sel) ";\n\t"\
        : : "r" (__value)); })

#define asm_mfc0(mfc_reg) __asm_mfc0(mfc_reg, 0)
#define asm_mtc0(value, mtc_reg) __asm_mtc0(value, mtc_reg, 0)
#define asm_mtc0_1(value, mtc_reg) __asm_mtc0(value, mtc_reg, 1)
#define asm_mfc0_2(mfc_reg) __asm_mfc0(mfc_reg, 2)
#define asm_mtc0_2(value, mtc_reg) __asm_mtc0(value, mtc_reg, 2)
#define asm_mfc0_3(mfc_reg) __asm_mfc0(mfc_reg, 3)

#define asm_mflxc0(mflxc0_reg) ({\
    unsigned int __ret;\
        __asm__ __volatile__ (\
            "mflxc0 %0," TO_STR(mflxc0_reg) ";\n\t"\
            : "=r" (__ret));\
            __ret;})

#define asm_mtlxc0(value, mtlxc_reg) ({\
    unsigned int __value=(value);\
    __asm__ __volatile__ (\
        "mtlxc0 %0, " TO_STR(mtlxc_reg) ";\n\t"\
        : : "r" (__value)); })

#define CPU_GET_CP0_CYCLE_COUNT() \
    ({ int __res;                           \
        __asm__ __volatile__("mfc0 %0, $9;"   \
        : "=r" (__res)); \
        __res;                                \
    })

#define CPU_GET_STACK_PTR() ({	  \
			int __res; \
			asm volatile ("move %0, $29": "=r"(__res)); \
			__res; \
		})

#endif //CPU_H
