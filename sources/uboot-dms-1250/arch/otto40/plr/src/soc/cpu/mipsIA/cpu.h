#ifndef CPU_H
#define CPU_H

#define GCR_ADDR	(0xBFBF8000)

#define GCR_CPC_BASE  (GCR_ADDR + 0x88)
#define CPC_BASE_ADDR (0xbfbf0000)
#define CPC_CO_BASE_ADDR (CPC_BASE_ADDR + 0x4000)

#define CKUSEG 		(0x00000000)
#define CKSEG0 		(0x80000000)
#define CKSEG1 		(0xA0000000)
#define CKSEG2 		(0xC0000000)
#define CKSEG3 		(0xE0000000)

#define CP0_INDEX	$0
#define CP0_INX		$0
#define CP0_RANDOM	$1
#define CP0_RAND	$1
#define CP0_ENTRYLO0	$2
#define CP0_TLBLO0	$2
#define CP0_ENTRYLO1	$3
#define CP0_TLBLO1	$3
#define CP0_CONTEXT	$4
#define CP0_CTXT	$4
#define CP0_PAGEMASK	$5
#define CP0_PAGEGRAIN	$5,1
#define CP0_WIRED	$6
#define CP0_HWRENA	$7
#define CP0_BADVADDR 	$8
#define CP0_VADDR 	$8
#define CP0_COUNT 	$9
#define CP0_ENTRYHI	$10
#define CP0_TLBHI	$10
#define CP0_COMPARE	$11
#define CP0_STATUS	$12
#define CP0_SR		$12
#define CP0_INTCTL	$12,1
#define CP0_SRSCTL	$12,2
#define CP0_SRSMAP	$12,3
#define CP0_CAUSE	$13
#define CP0_CR		$13
#define CP0_EPC 	$14
#define CP0_PRID	$15
#define CP0_EBASE	$15,1
#define CP0_CONFIG	$16
#define CP0_CONFIG0	$16,0
#define CP0_CONFIG1	$16,1
#define CP0_CONFIG2	$16,2
#define CP0_CONFIG3	$16,3
#define CP0_LLADDR	$17
#define CP0_WATCHLO	$18
#define CP0_WATCHHI	$19
#define CP0_DEBUG	$23
#define CP0_DEPC	$24
#define CP0_PERFCNT	$25
#define CP0_ERRCTL	$26
#define CP0_CACHEERR	$27
#define CP0_TAGLO	$28
#define CP0_ITAGLO	$28
#define CP0_DTAGLO	$28,2
#define CP0_TAGLO2	$28,4
#define CP0_DATALO	$28,1
#define CP0_IDATALO	$28,1
#define CP0_DDATALO	$28,3
#define CP0_DATALO2	$28,5
#define CP0_TAGHI	$29
#define CP0_DATAHI	$29,1
#define CP0_DATAHI2	$29,5
#define CP0_ERRPC	$30
#define CP0_DESAVE	$31

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
#define Index_Store_Tag_SD    0x0b
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

/*
 * MIPS32 Config1 Register (CP0 Register 16, Select 1)
 */
#define CFG1_M          0x80000000      /* Config2 implemented */
#define CFG1_MMUSMASK   0x7e000000      /* mmu size - 1 */
#define CFG1_MMUSSHIFT  25
#define CFG1_ISMASK     0x01c00000      /* icache lines 64<<n */
#define CFG1_ISSHIFT    22
#define CFG1_ILMASK     0x00380000      /* icache line size 2<<n */
#define CFG1_ILSHIFT    19
#define CFG1_IAMASK     0x00070000      /* icache ways - 1 */
#define CFG1_IASHIFT    16
#define CFG1_DSMASK     0x0000e000      /* dcache lines 64<<n */
#define CFG1_DSSHIFT    13
#define CFG1_DLMASK     0x00001c00      /* dcache line size 2<<n */
#define CFG1_DLSHIFT    10
#define CFG1_DAMASK     0x00000380      /* dcache ways - 1 */
#define CFG1_DASHIFT    7
#define CFG1_C2         0x00000040      /* Coprocessor 2 present */
#define CFG1_MD         0x00000020      /* MDMX implemented */
#define CFG1_PC         0x00000010      /* performance counters implemented */
#define CFG1_WR         0x00000008      /* watch registers implemented */
#define CFG1_CA         0x00000004      /* compression (mips16) implemented */
#define CFG1_EP         0x00000002      /* ejtag implemented */
#define CFG1_FP         0x00000001      /* fpu implemented */

#define cacheop(cmd, addr_reg) cache cmd, 0(addr_reg)

#define DCACHE_LINE_SIZE (CACHELINE_SIZE)

#ifndef ASM_NL
#define ASM_NL ";\n\t"
#endif

#define set_zero_64(dst_reg)	  \
	sw    zero, (0*4 - 64)(dst_reg); \
	sw    zero, (1*4 - 64)(dst_reg); \
	sw    zero, (2*4 - 64)(dst_reg); \
	sw    zero, (3*4 - 64)(dst_reg); \
	sw    zero, (4*4 - 64)(dst_reg); \
	sw    zero, (5*4 - 64)(dst_reg); \
	sw    zero, (6*4 - 64)(dst_reg); \
	sw    zero, (7*4 - 64)(dst_reg); \
	sw    zero, (8*4 - 64)(dst_reg); \
	sw    zero, (9*4 - 64)(dst_reg); \
	sw    zero, (10*4 - 64)(dst_reg); \
	sw    zero, (11*4 - 64)(dst_reg); \
	sw    zero, (12*4 - 64)(dst_reg); \
	sw    zero, (13*4 - 64)(dst_reg); \
	sw    zero, (14*4 - 64)(dst_reg); \
	sw    zero, (15*4 - 64)(dst_reg);

#define CPU_GET_CP0_CYCLE_COUNT()	  \
	({ int __res; \
		__asm__ __volatile__("mfc0 %0, $9;" \
		                     : "=r" (__res)); \
		__res; \
	})

#define CPU_GET_STACK_PTR()	  \
	({ int __res; \
		__asm__ __volatile__("move %0, $29" \
		                     : "=r"(__res)); \
		__res; \
	})

/* So far RS is safe on MIPS so no actual disable code */
#define RETURN_STACK_SAVE(_bpctl_backup)
#define RETURN_STACK_RESTORE(_bpctl_backup)

#endif //CPU_H
