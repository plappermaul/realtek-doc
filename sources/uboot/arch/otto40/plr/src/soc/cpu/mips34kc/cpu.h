#ifndef CPU_H
#define CPU_H

#define CKUSEG 0x00000000
#define CKSEG0 0x80000000
#define CKSEG1 0xa0000000
#define CKSEG2 0xc0000000
#define CKSEG3 0xe0000000

#define CP0_INDEX       $0
#define CP0_RANDOM      $1
#define CP0_ENTRYLO     $2
#define CP0_ENTRYLO0    $2
#define CP0_ENTRYLO1    $3
#define CP0_CONF        $3
#define CP0_CONTEXT     $4
#define CP0_PAGEMASK    $5
#define CP0_WIRED       $6
#define CP0_INFO        $7
#define CP0_BADVADDR    $8
#define CP0_COUNT       $9
#define CP0_ENTRYHI     $10
#define CP0_COMPARE     $11
#define CP0_STATUS      $12
#define CP0_CAUSE       $13
#define CP0_EPC         $14
#define CP0_PRID        $15
#define CP0_CONFIG      $16
#define CP0_CONFIG7     $16,7
#define CP0_LLADDR      $17
#define CP0_WATCHLO     $18
#define CP0_WATCHHI     $19
#define CP0_XCONTEXT    $20
#define CP0_FRAMEMASK   $21
#define CP0_DIAGNOSTIC  $22
#define CP0_PERFORMANCE $25
#define CP0_ECC         $26
#define CP0_CACHEERR    $27
#define CP0_ITAGLO      $28
#define CP0_DTAGLO      $28,2
#define CP0_TAGHI       $29
#define CP0_ERROREPC    $30

#define CP0_STATUS_IE   (1<<0)
#define CP0_STATUS_EXL  (1<<1)
#define CP0_STATUS_ERL  (1<<2)

#define CP0_CONF_CACHABLE_NC_WT_nWA   0
#define CP0_CONF_UNCACHED             2
#define CP0_CONF_CACHABLE_NC_WB_WA    3
#define CP0_CONF_UNCACHED_ACCELERATED 7
#define CP0_CONF_CACHE_MASK           0x7

#define Index_Invalidate_I    0x00
#define Index_Writeback_Inv_D 0x01
#define Index_Load_Tag_I      0x04
#define Index_Load_Tag_D      0x05
#define Index_Store_Tag_I     0x08
#define Index_Store_Tag_D     0x09
#define Hit_Invalidate_I      0x10
#define Hit_Invalidate_D      0x11
#define Fill_I                0x14
#define Hit_Writeback_Inv_D   0x15

#define zero $0
#define AT   $1
#define v0   $2  /* return value */
#define v1   $3
#define a0   $4  /* argument registers */
#define a1   $5
#define a2   $6
#define a3   $7
#define t0   $8  /* caller saved */
#define t1   $9
#define t2   $10
#define t3   $11
#define t4   $12
#define t5   $13
#define t6   $14
#define t7   $15
#define s0   $16 /* callee saved */
#define s1   $17
#define s2   $18
#define s3   $19
#define s4   $20
#define s5   $21
#define s6   $22
#define s7   $23
#define t8   $24 /* caller saved */
#define t9   $25
#define k0   $26
#define k1   $27
#define gp   $28
#define sp   $29
#define s8   $30
#define ra   $31

#ifndef ASM_NL
    #define ASM_NL              ";\n\t"
#endif

#define CCTL_REG $20
#define CACHE_OP_I_INV		(0x10)
#define CACHE_OP_D_INV		(0x11)
#define CACHE_OP_D_WB_INV	(0x15)
#define CACHE_OP_D_WB		(0x19)
#define CACHE_OP_I_FETCH_LOCK	(0x1C)
#define CACHE_OP_D_FETCH_LOCK	(0x1D)
#define DCACHE_LINE_SIZE	(CACHELINE_SIZE)
#define ICACHE_LINE_SIZE	(CACHELINE_SIZE)

#define cacheop(cmd, addr_reg) cache cmd, 0(addr_reg)

#define set_zero_64(dst_reg) \
	sw	zero, (0*4 - 64)(dst_reg); \
	sw	zero, (1*4 - 64)(dst_reg); \
	sw	zero, (2*4 - 64)(dst_reg); \
	sw	zero, (3*4 - 64)(dst_reg); \
	sw	zero, (4*4 - 64)(dst_reg); \
	sw	zero, (5*4 - 64)(dst_reg); \
	sw	zero, (6*4 - 64)(dst_reg); \
	sw	zero, (7*4 - 64)(dst_reg); \
	sw	zero, (8*4 - 64)(dst_reg); \
	sw	zero, (9*4 - 64)(dst_reg); \
	sw	zero, (10*4 - 64)(dst_reg); \
	sw	zero, (11*4 - 64)(dst_reg); \
	sw	zero, (12*4 - 64)(dst_reg); \
	sw	zero, (13*4 - 64)(dst_reg); \
	sw	zero, (14*4 - 64)(dst_reg); \
	sw	zero, (15*4 - 64)(dst_reg);
    
/* So far RS is safe on MIPS so no actual disable code */
#define RETURN_STACK_SAVE(_bpctl_backup)
#define RETURN_STACK_RESTORE(_bpctl_backup)
	
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
//#define asm_mtc0_1(value, mtc_reg) __asm_mtc0(value, mtc_reg, 1)
//#define asm_mfc0_2(mfc_reg) __asm_mfc0(mfc_reg, 2)
//#define asm_mtc0_2(value, mtc_reg) __asm_mtc0(value, mtc_reg, 2)
//#define asm_mfc0_3(mfc_reg) __asm_mfc0(mfc_reg, 3)

#define CPU_GET_CP0_CYCLE_COUNT() \
    ({ int __res;                           \
        __asm__ __volatile__("mfc0 %0, $9;"   \
        : "=r" (__res)); \
        __res;                                \
    })


#define CPU_GET_STACK_PTR() \
    ({ int __res;           \
       __asm__ __volatile__("move %0, $29" \
       : "=r"(__res)); \
       __res; \
    })

#ifndef __ASSEMBLER__
#define CACHE_TYPE_I (16)
#define CACHE_TYPE_D (7)
#endif

#endif //CPU_H
