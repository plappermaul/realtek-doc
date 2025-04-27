#define __ASM__
#include <soc.h>

#define SYS_STATUSar            (0xB8000044)
#define PLL_SW_DIV_CTRLar       (0xBB00E220)

#define MODLE_NAME_INFOar	    (0xBB000004)
#define UNCACHE_SRAM_ADDR       (0xBF000000)
#define PLL_GLB_CTRL0ar         (0xBB00E200)
#define PLL_SRAM_CTRLar         (0xB8000700)
#define CPU_SRAM_ASY_TIMINGar   (0xB80040F8)

GSFUNC(fetch_and_lock_icache_range)
        move    t0, a0
        li      t1, CACHELINE_SIZE
        move    t2, a1
i_fl_loop:
        cacheop(CACHE_OP_I_FETCH_LOCK, t0)
        addu    t0, t1
        bge     t2, t0, i_fl_loop
        j ra
END(fetch_and_lock_icache_range)

GSFUNC(fetch_and_lock_dcache_range)
        move    t0, a0
        li      t1, CACHELINE_SIZE
        move    t2, a1
d_fl_loop:
        cacheop(CACHE_OP_D_FETCH_LOCK, t0)
        addu    t0, t1
        bge     t2, t0, d_fl_loop
        j ra
END(fetch_and_lock_dcache_range)

GSFUNC(_change_cpu_sram_pll)
        .set    noreorder
        // disable xxx, keep t7 as original CP0_CONFIG value
        mfc0    t7, CP0_CONFIG, 7
        ori     t1, t7, 0xC
        mtc0    t1, CP0_CONFIG, 7
        li      t8, 1       // keep t8 as 1

        // Write and Read SRAM, clear ongoing sram traffic
        li      t0, UNCACHE_SRAM_ADDR
        lw      t1, 0(t0)
        lw      t2, 4(t0)
        sw      t1, 0(t0)
        sw      t2, 4(t0)
        lw      t1, 0(t0)
        lw      t2, 4(t0)        
        
        li      t6, PLL_GLB_CTRL0ar
        li      t5, SYS_STATUSar
        li      t4, PLL_SW_DIV_CTRLar
        
        // change pll to lx
        lw      t1, 0(t5)
        ins     t1, zero, 0x2, 0x1
        sw      t1, 0(t5)

        // configure cpu pll,
        sw      a0, 8(t6)       // a0, cpu pll0
        sw      a1, 16(t6)      // a1, cpu pllm
        
        // SW_CMU_EN/CPU_CMU_EN trigger
	    li	    t2, MODLE_NAME_INFOar
	    lw	    t3, 0(t2)
        ext	t3, t3, 0x4, 0x1	// ignore bit 5
        beqz	t3, 2f
        li	t1, 0x3
        lw	t3, 0(t2)
        ext	t3, t3, 0x0, 0x4	// version
        bge	t3, t1, 2f
	    nop

        lw      t1, 0(t4)
        ins     t1, zero, 17, 0x1
        sw      t1, 0(t4)
        ins     t1, t8, 17, 0x1
	    b	    1f
        sw      t1, 0(t4)
              
2:	    lw      t1, 0(t6)
	    ins     t1, zero, 30, 0x1
	    sw      t1, 0(t6)
	    ins     t1, t8, 30, 0x1
	    sw      t1, 0(t6)

        // polling ready
1:      lw      t2, 0(t6)
        ext     t3, t2, 0xF, 0x1
        beqz    t3, 1b
        
        // change pll to configued CPU freq.
        lw      t1, 0(t5)
        ins     t1, t8, 0x2, 0x1
        sw      t1, 0(t5)        

        li      t5, PLL_SRAM_CTRLar
        li      t4, CPU_SRAM_ASY_TIMINGar
        
        // config sram pll and slow bit
        sw      a2, 0(t5)       // a2, sram pll
        sw      a3, 0(t4)       // a3, sram_async_timing
       
        li      t0, 10000
2:      bnez    t0, 2b
        addiu   t0, t0, -1
       
        // restore 
        mtc0    t7, CP0_CONFIG, 7
        
        jr  ra
        nop
END(_change_cpu_sram_pll)
