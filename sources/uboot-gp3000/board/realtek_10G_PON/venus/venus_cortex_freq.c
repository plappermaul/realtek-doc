#include <common.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <linux/compat.h>
//#define DEBUG
#ifdef DEBUG
#define venus_clk_dbg(...) 	printf("venus_clk: " __VA_ARGS__ )
#else
#define venus_clk_dbg(...)
#endif
#define base GLOBAL_JTAG_ID

#define clk_mask(width) ((1 << width) - 1)
#define DIVN_BASE		3
#define DIVN_SHIFT		0
#define DIVN_WIDTH		8
#define PREDIV_BASE		2
#define PREDIV_SHIFT		10
#define PREDIV_WIDTH		2

#define MIN_DELAY_PLL_FOR_LOCK		10000
#define MIN_DELAY_PLL_FOR_SWITCH	100


#define FPLL0_DEF			0x000007bd
#define CPLLDIV_DEF			0x00100400
#define CPLL0_DEF			0x00000fed

#define MHZ 				1000000
unsigned long ref_rate = 25000000;

struct ca8289_clk_reg {
    void __iomem *regbase;
    spinlock_t regs_lock;       /* register access lock */
};

struct ca8289_clk_param {
    int type;
    u32 reg;
};
#if 0
struct ca8289_pll_clk {
    void __iomem *reg;
    spinlock_t *lock;           /* register access lock */
    struct clk_hw hw;
};
#endif
struct ca8289_pll_param {
    unsigned int pll;
    unsigned int prediv;
    unsigned int divn;
};

struct ca8289_cpu_strap_speed {
    unsigned int pll;
    unsigned int div;
};

struct ca8289_pe_speed {
    unsigned int pll;
    unsigned int div;
};
#if 0
struct ca8289_cpu_daemon {
    void __iomem *reg;
    struct mutex mlock;         /* daemon lock */
    spinlock_t *lock;           /* register access lock */
    struct clk_hw hw;
    struct clk *clks[CPUD_CLKS_NUM];
};
#endif
static const struct ca8289_pll_param ca8289_pll_param_table[] = {
    {.pll = 40,.prediv = 5,.divn = 200,},       /* CPU */
    {.pll = 44,.prediv = 5,.divn = 220,},       /* CPU */
    {.pll = 48,.prediv = 5,.divn = 240,},       /* CPU */
    {.pll = 50,.prediv = 5,.divn = 250,},       /* CPU */
    {.pll = 56,.prediv = 3,.divn = 168,},       /* CPU */
    {.pll = 64,.prediv = 3,.divn = 192,},       /* FPLL/CPU */
    {.pll = 72,.prediv = 3,.divn = 216,},       /* CPU */
    {.pll = 80,.prediv = 2,.divn = 160,},       /* EPLL */
};

static const struct ca8289_cpu_strap_speed ca8289_cpu_strap_speed_table[] = {
    {.pll = 48,.div = 3,},      /* 400000000 */
    {.pll = 50,.div = 2,},      /* 625000000 */
    {.pll = 56,.div = 2,},      /* 700000000 */
    {.pll = 64,.div = 2,},      /* 800000000 */
    {.pll = 72,.div = 2,},      /* 900000000 */
    {.pll = 40,.div = 1,},      /* 1000000000 */
    {.pll = 44,.div = 1,},      /* 1100000000 */
    {.pll = 48,.div = 1,},      /* 1200000000 */
};

static const struct ca8289_pe_speed ca8289_pe_speed_table[] = {
    {.pll = 64,.div = 2,},      /* 800000000 */
    {}
};


static void ca8289_pll_set_rate(u32 mult, u64 reg)
{

    unsigned long flags = 0;
    u32 val, divn, prediv;
    int i;

    for (i = 0; i < ARRAY_SIZE(ca8289_pll_param_table); i++) {
        if (ca8289_pll_param_table[i].pll == mult) {
            prediv = ca8289_pll_param_table[i].prediv;
            divn = ca8289_pll_param_table[i].divn;
            flags = 1;
            break;
        }
    }
//      venus_clk_dbg("[%s][%d]0x%lx, 0x%lx, %lu, %lu\n", __func__, __LINE__, mult, flags ,prediv, divn);       
    if (flags) {
        val = readl(reg);
        val &= ~(clk_mask(DIVN_WIDTH) << DIVN_SHIFT);
        val |= (divn - DIVN_BASE) << DIVN_SHIFT;
        val &= ~(clk_mask(PREDIV_WIDTH) << PREDIV_SHIFT);
        val |= (prediv - PREDIV_BASE) << PREDIV_SHIFT;

        //bit 12        RW      0       DIV_PREDIV_BPS
        val &= ~(1 << 12);
        venus_clk_dbg("[%s][%d]writel(0x%8x, 0x%8x)\n", __func__, __LINE__,
                      val, reg);
        writel(val, reg);
        dsb();
        udelay(MIN_DELAY_PLL_FOR_LOCK);
    }
}

/*********************
 Reset CPLL 
 ************************/
static inline void ca8289_cpll_reset(void)
{
    GLOBAL_CPLL0_t cpll0;


    cpll0.wrd = readl(GLOBAL_CPLL0);
    cpll0.bf.SCPU_RSTB = 0;
    cpll0.bf.SCPU_POW = 1;
    venus_clk_dbg("[%s][%d]writel(0x%8x, 0x%8x)\n", __func__, __LINE__,
                  cpll0.wrd, GLOBAL_CPLL0);
    writel(cpll0.wrd, GLOBAL_CPLL0);
    dsb();
    udelay(MIN_DELAY_PLL_FOR_LOCK);

    cpll0.wrd = readl(GLOBAL_CPLL0);
    cpll0.bf.SCPU_RSTB = 0;
    venus_clk_dbg("[%s][%d]writel(0x%8x, 0x%8x)\n", __func__, __LINE__,
                  cpll0.wrd, GLOBAL_CPLL0);
    writel(cpll0.wrd, GLOBAL_CPLL0);
    dsb();
    udelay(MIN_DELAY_PLL_FOR_LOCK);

    cpll0.wrd = readl(GLOBAL_CPLL0);
    cpll0.bf.SCPU_RSTB = 1;
    venus_clk_dbg("[%s][%d]writel(0x%8x, 0x%8x)\n", __func__, __LINE__,
                  cpll0.wrd, GLOBAL_CPLL0);
    writel(cpll0.wrd, GLOBAL_CPLL0);
    dsb();
    udelay(MIN_DELAY_PLL_FOR_LOCK);

}

void rtk_cortex_freq_tbl_show(void)
{
    int i;
    unsigned long mult, div, rate;

    printf("IDX: CPU_FREQ\n");

    for (i = 0; i < ARRAY_SIZE(ca8289_cpu_strap_speed_table); i++) {
        mult = ca8289_cpu_strap_speed_table[i].pll;
        div = ca8289_cpu_strap_speed_table[i].div;
        rate = ref_rate * mult / div;
        printf("%d: %lu\n", i, rate);
    }

}



unsigned long rtk_cortex_freq_get(void)
{
    unsigned long mult, div;
    GLOBAL_CPLLDIV_t cplldiv;

    GLOBAL_CPLL0_t cpll0;

    int global_strap_mode_idx = 0;

    unsigned long current_rate;


    cplldiv.wrd = readl(GLOBAL_CPLLDIV);


    if ((cplldiv.bf.cpll_div_override == 1) && (cplldiv.bf.cf_sel == 0)) {


        cpll0.wrd = readl(GLOBAL_CPLL0);


        current_rate =
            DIV_ROUND_UP(ref_rate, (cpll0.bf.DIV_PREDIV_SEL + 2));

        current_rate = current_rate * (cpll0.bf.SCPU_DIV_DIVN + 3);

        current_rate =
            current_rate / ((cplldiv.bf.cortex_divsel >> 1) & 0xf);

        /* round down to nearist Khz */
        current_rate = current_rate / 1000;

        current_rate = current_rate * 1000;

    } else {

        global_strap_mode_idx = (readl(GLOBAL_STRAP) >> 1) & 0x7;

        if (global_strap_mode_idx >=
            ARRAY_SIZE(ca8289_cpu_strap_speed_table)) {

            printf("Error: global_strap_mode_idx=%u, Not supported!\n",
                   global_strap_mode_idx);

            return 0;

        }
        mult = ca8289_cpu_strap_speed_table[global_strap_mode_idx].pll;
        div = ca8289_cpu_strap_speed_table[global_strap_mode_idx].div;
        current_rate = ref_rate * mult / div;

    }

    return current_rate;
}

#if 0
/* Do not use NOP inst
 * HINT 0 : NOP , No operation - not guaranteed to take time to execute
 */

#define nop() __asm__ __volatile__("mov wzr, wzr\n" : : : "memory");

#define nop10x() __asm__ __volatile__("mov wzr, wzr\n\t" \
                                      "mov wzr, wzr\n\t" \
                                      "mov wzr, wzr\n\t" \
                                      "mov wzr, wzr\n\t" \
                                      "mov wzr, wzr\n\t" \
                                      "mov wzr, wzr\n\t" \
                                      "mov wzr, wzr\n\t" \
                                      "mov wzr, wzr\n\t" \
                                      "mov wzr, wzr\n\t"  : : : "memory");
static inline void NOP_10X(int x)
{
    int i;
    for (i = 0; i < x; i++) {
        nop10x();
    }
}

static inline void CPU_NOP(void)
{
    dsb();
    NOP_10X(100);
}
#endif


int rtk_cortex_freq_set(uint i)
{
    GLOBAL_CPLLDIV_t cplldiv;
    GLOBAL_CPLL0_t cpll0;
    GLOBAL_FPLL0_t fpll0;
    u8 cortex_divsel = 0;
    unsigned long cpu_clk;
    unsigned long mult, div;

    if (i >= (ARRAY_SIZE(ca8289_cpu_strap_speed_table))) {
        printf("Input IDX > %lu\n",
               ARRAY_SIZE(ca8289_cpu_strap_speed_table) - 1);
        rtk_cortex_freq_tbl_show();
        return -1;
    }


    mult = ca8289_cpu_strap_speed_table[i].pll;
    div = ca8289_cpu_strap_speed_table[i].div;
    cpu_clk = ref_rate * mult / div;
#if 1
    venus_clk_dbg("IDX[%d] : pll=%u, div=%u, CPU_CLOCK:%lu(MHZ)\n", i,
                  ca8289_cpu_strap_speed_table[i].pll,
                  ca8289_cpu_strap_speed_table[i].div, cpu_clk / MHZ);
#endif
    if (cpu_clk == rtk_cortex_freq_get()) {
        printf("Current rate has been %lu(MHZ)\n", cpu_clk / MHZ);
        return 0;
    }

    /* 1. init FPLL */
    fpll0.wrd = readl(GLOBAL_FPLL0);
    if (fpll0.wrd != FPLL0_DEF) {
        venus_clk_dbg("[%s][%d]GLOBAL_FPLL0: 0x%x\n", __func__, __LINE__,
                      FPLL0_DEF);
        writel(FPLL0_DEF, GLOBAL_FPLL0);
        /* For 25MHz refclk, /3 predivider, divn=189 (/192) gives 1.6GHz output clock */
        /* FPLL CLK of CPU is 1.6GHZ/4 = 400MHZ */
        /* printf or udelay */
        udelay(MIN_DELAY_PLL_FOR_LOCK);
    }


    /* 2. Switch clk source of cpu to FPLL */
    cplldiv.wrd = readl(GLOBAL_CPLLDIV);

    cplldiv.bf.cf_sel = 1;
    cplldiv.bf.f2c_divsel = 0x4;        //default vaule, 1600MHZ(EPLL)/4 = 400MHZ
    venus_clk_dbg("[%s][%d]writel(0x%8x, 0x%8x)\n", __func__, __LINE__,
                  cplldiv.wrd, GLOBAL_CPLLDIV);
    writel(cplldiv.wrd, GLOBAL_CPLLDIV);
    dsb();
    udelay(MIN_DELAY_PLL_FOR_SWITCH);
    isb();
    venus_clk_dbg("[%s][%d]Now, Cortex use FPLL CLK as CPU clk\n",
                  __func__, __LINE__);
    /*Now, Cortex use FPLL CLK for CPU clk */

    /*********************************************************************/

    /* 3. set CPLLDIV and CPLL) */
    /* CPLL0 , 1b8 */
    ca8289_pll_set_rate(ca8289_cpu_strap_speed_table[i].pll, GLOBAL_CPLL0);

    /* PE , 198 */
    venus_clk_dbg("[%s][%d]writel(0x%8x, 0x%8x)\n", __func__, __LINE__,
                  GLOBAL_PEDIV_dft, GLOBAL_PEDIV);
    writel(GLOBAL_PEDIV_dft, GLOBAL_PEDIV);
    udelay(MIN_DELAY_PLL_FOR_LOCK);

    /* cortex_div */
    cplldiv.wrd = readl(GLOBAL_CPLLDIV);
    cortex_divsel = ca8289_cpu_strap_speed_table[i].div << 1;
    cplldiv.bf.cortex_divsel = cortex_divsel;
    venus_clk_dbg("[%s][%d]writel(0x%8x, 0x%8x)\n", __func__, __LINE__,
                  cplldiv.wrd, GLOBAL_CPLLDIV);
    writel(cplldiv.wrd, GLOBAL_CPLLDIV);
    dsb();
    udelay(MIN_DELAY_PLL_FOR_LOCK);



    /* Reset CPLL */
    {

        cpll0.wrd = readl(GLOBAL_CPLL0);
        cpll0.bf.SCPU_RSTB = 0;
        cpll0.bf.SCPU_POW = 1;
        venus_clk_dbg("[%s][%d]writel(0x%8x, 0x%8x)\n", __func__, __LINE__,
                      cpll0.wrd, GLOBAL_CPLL0);
        writel(cpll0.wrd, GLOBAL_CPLL0);
        dsb();
        udelay(MIN_DELAY_PLL_FOR_LOCK);

        cpll0.wrd = readl(GLOBAL_CPLL0);
        cpll0.bf.SCPU_RSTB = 0;
        venus_clk_dbg("[%s][%d]writel(0x%8x, 0x%8x)\n", __func__, __LINE__,
                      cpll0.wrd, GLOBAL_CPLL0);
        writel(cpll0.wrd, GLOBAL_CPLL0);
        dsb();
        udelay(MIN_DELAY_PLL_FOR_LOCK);


        cpll0.wrd = readl(GLOBAL_CPLL0);
        cpll0.bf.SCPU_RSTB = 1;
        venus_clk_dbg("[%s][%d]writel(0x%8x, 0x%8x)\n", __func__, __LINE__,
                      cpll0.wrd, GLOBAL_CPLL0);
        writel(cpll0.wrd, GLOBAL_CPLL0);
        dsb();
        udelay(MIN_DELAY_PLL_FOR_LOCK);
    }

    /* Override defalut CPLL setting(strap pin) */
    /*  USE CPLL setting to replace HW setting */

    cplldiv.wrd = readl(GLOBAL_CPLLDIV);
    cplldiv.bf.cpll_mode_override = 1;
    cplldiv.bf.cpll_div_override = 1;
    venus_clk_dbg("[%s][%d]writel(0x%8x, 0x%8x)\n", __func__, __LINE__,
                  cplldiv.wrd, GLOBAL_CPLLDIV);
    writel(cplldiv.wrd, GLOBAL_CPLLDIV);
    dsb();
    udelay(MIN_DELAY_PLL_FOR_SWITCH);

    /* 6. Switch clk source of cpu to CPLL */
    cplldiv.wrd = readl(GLOBAL_CPLLDIV);
    cplldiv.bf.cf_sel = 0;
    venus_clk_dbg("[%s][%d]writel(0x%8x, 0x%8x)\n", __func__, __LINE__,
                  cplldiv.wrd, GLOBAL_CPLLDIV);
    writel(cplldiv.wrd, GLOBAL_CPLLDIV);
    dsb();
    udelay(MIN_DELAY_PLL_FOR_SWITCH);
    isb();
    printk("Current CPU CLOCK: %lu(MHZ)\n", cpu_clk / MHZ);
    return 0;

}
