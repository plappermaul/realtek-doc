#ifndef __MIPS_CACHE_OP_H__
#define __MIPS_CACHE_OP_H__

#define cacheop(cmd, addr_reg) cache cmd, 0(addr_reg)

#define set_zero_64(dst_reg) \
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
    
#define GCR_ADDR	(0xBFBF8000)
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

#define Index_Invalidate_I    	0x00
#define Index_Writeback_Inv_D 	0x01
#define Index_Writeback_Inv_SD	0x03
#define Index_Load_Tag_I      	0x04
#define Index_Load_Tag_D      	0x05
#define Index_Store_Tag_I     	0x08
#define Index_Store_Tag_D     	0x09
#define Hit_Invalidate_I      	0x10
#define Hit_Invalidate_D      	0x11
#define Fill_I                	0x14
#define Hit_Writeback_Inv_D   	0x15


#define CACHELINE_SIZE		32

#define ICACHE_SIZE	65536
#define DCACHE_SIZE	32768
#define MIPS_MAX_CACHE_SIZE	65536


#define __read_32bit_c0_register(source, sel)                           \
({ int __res;                                                            \
        if (sel == 0)                                                   \
                __asm__ __volatile__(                                   \
                        "mfc0\t%0, " #source "\n\t"                     \
                        : "=r" (__res));                                \
        else                                                            \
                __asm__ __volatile__(                                   \
                        ".set\tmips32\n\t"                              \
                        "mfc0\t%0, " #source ", " #sel "\n\t"           \
                        ".set\tmips0\n\t"                               \
                        : "=r" (__res));                                \
        __res;                                                          \
})

#define __write_32bit_c0_register(register, sel, value)                 \
({                                                                      \
        if (sel == 0)                                                   \
                __asm__ __volatile__(                                   \
                        "mtc0\t%z0, " #register "\n\t"                  \
                        : : "Jr" ((unsigned int)(value)));              \
        else                                                            \
                __asm__ __volatile__(                                   \
                        ".set\tmips32\n\t"                              \
                        "mtc0\t%z0, " #register ", " #sel "\n\t"        \
                        ".set\tmips0"                                   \
                        : : "Jr" ((unsigned int)(value)));              \
})

#define read_c0_config1()       __read_32bit_c0_register($16, 1)
#define read_c0_config2()       __read_32bit_c0_register($16, 2)
#define write_c0_config2(val)   __write_32bit_c0_register($16, 2, val)

#define cache_op(op,addr)                                               \
        __asm__ __volatile__(                                           \
        "       .set    push                                    \n"     \
        "       .set    noreorder                               \n"     \
        "       .set    mips3\n\t                               \n"     \
        "       cache   %0, %1                                  \n"     \
        "       .set    pop                                     \n"     \
        :                                                               \
        : "i" (op), "R" (*(unsigned char *)(addr)))

#define Index_Writeback_Inv_D           0x01

#define _mips_sync() __asm__ __volatile__ ("sync  0x3" : : : "memory")

#endif //__MIPS_CACHE_OP_H__