#include <common.h>
#include <cortina/ca77xx/ca77xx_registers.h>
#include <asm/io.h>
#include <linux/compiler.h>

#define ca7774_clk_dev_dbg(...) 	printf("ca7774_clk: " __VA_ARGS__ )
#define base GLOBAL_JTAG_ID

#define MINIMUM_DELAY_PLL_FOR_LOCK 10000
#define MINIMUM_DELAY_PLL_FOR_SWITCH 10

struct ca7774_cpll_global_strap_shadow_entry {

    unsigned int cortex;

    unsigned int prediv;

    unsigned int divn;

    unsigned int divsel;

};


static struct ca7774_cpll_global_strap_shadow_entry
 ca7774_cpll_global_strap_table[] = {
    {.cortex = 400000000,.prediv = 3,.divn = 237,.divsel = 0x26,},
    {.cortex = 625000000,.prediv = 3,.divn = 247,.divsel = 0x4,},
    {.cortex = 700000000,.prediv = 1,.divn = 165,.divsel = 0x4,},

    {.cortex = 800000000,.prediv = 1,.divn = 189,.divsel = 0x4,},
    {.cortex = 900000000,.prediv = 1,.divn = 213,.divsel = 0x4,},
    {.cortex = 1000000000,.prediv = 3,.divn = 197,.divsel = 0x2,},
    {.cortex = 1100000000,.prediv = 3,.divn = 217,.divsel = 0x2,},
    {.cortex = 1200000000,.prediv = 3,.divn = 237,.divsel = 0x2,}
};



void rtk_cortex_freq_tbl_show(void)
{

    int i;

    int tbl_size =
        (sizeof(ca7774_cpll_global_strap_table) /
         sizeof(struct ca7774_cpll_global_strap_shadow_entry));

    printf("IDX: CPU_FREQ\n");

    for (i = 0; i < tbl_size; i++) {

        printf("%d: %u\n", i, ca7774_cpll_global_strap_table[i].cortex);

    }

}



unsigned long rtk_cortex_freq_get(void)
{

    GLOBAL_CPLLDIV_t cplldiv;

    GLOBAL_CPLL0_t cpll0;

    int global_strap_mode_idx = 0;

    unsigned long current_rate;


    cplldiv.wrd = readl(base + 0x134);


    if ((cplldiv.bf.cpll_div_override == 1) && (cplldiv.bf.cf_sel == 0)) {


        cpll0.wrd = readl(base + 0x160);


        current_rate =
            DIV_ROUND_UP(25000000, (cpll0.bf.DIV_PREDIV_SEL + 2));

        current_rate = current_rate * (cpll0.bf.SCPU_DIV_DIVN + 3);

        current_rate =
            current_rate / ((cplldiv.bf.cortex_divsel >> 1) & 0xf);

        /* round down to nearist Khz */
        current_rate = current_rate / 1000;

        current_rate = current_rate * 1000;

    } else {

        global_strap_mode_idx = (readl(base + 0x14) >> 1) & 0x7;

        if (global_strap_mode_idx >=
            (sizeof(ca7774_cpll_global_strap_table) /
             sizeof(struct ca7774_cpll_global_strap_shadow_entry))) {

            printf("Error: global_strap_mode_idx=%u, Not supported!\n",
                   global_strap_mode_idx);

            return 0;

        }

        current_rate =
            ca7774_cpll_global_strap_table[global_strap_mode_idx].cortex;

    }


    //printf("%s current rate is %lu\n", __func__, current_rate);


    return current_rate;

}




static int rtk_cortex_cpll_init_check(void)
{

    GLOBAL_CPLLDIV_t cplldiv;


    cplldiv.wrd = readl(base + 0x134);


    if ((cplldiv.bf.cpll_div_override == 1)
        || (cplldiv.bf.cpll_mode_override == 1)) {

        return 1;

    }

    return 0;

}



static void rtk_cortex_set(struct ca7774_cpll_global_strap_shadow_entry
                           globalFreqEntry)
{

    GLOBAL_CPLLDIV_t cplldiv;

    GLOBAL_CPLL0_t cpll0;

    unsigned int cortex;

    unsigned int prediv;

    unsigned int divn;

    unsigned int divsel;



    cortex = globalFreqEntry.cortex;

    prediv = globalFreqEntry.prediv;

    divn = globalFreqEntry.divn;

    divsel = globalFreqEntry.divsel;



    ca7774_clk_dev_dbg("%s line %d\n", __func__, __LINE__);

    ca7774_clk_dev_dbg("%s cortex %d\n", __func__, cortex);

    ca7774_clk_dev_dbg("%s prediv %d\n", __func__, prediv);

    ca7774_clk_dev_dbg("%s divn %d\n", __func__, divn);

    ca7774_clk_dev_dbg("%s divsel %d\n", __func__, divsel);


    /* switch to FPLL clock */
    ca7774_clk_dev_dbg("%s Switch to FPLL clock\n", __func__);

    cplldiv.wrd = readl(base + 0x134);

    cplldiv.bf.cf_sel = 1;

    writel(cplldiv.wrd, base + 0x134);


    mdelay(10);



    /* if we have not found a match, do a noop */
    if (cortex == 0)
    {

        ca7774_clk_dev_dbg
            ("%s requested speed invalid, ignoring request\n", __func__);

        return;

    }



    cpll0.wrd = readl(base + 0x160);


    /* reprogram CPLL PREDIV */
    ca7774_clk_dev_dbg("%s reprograming CPLL PREDIV\n", __func__);

    ca7774_clk_dev_dbg("%s setting prediv to %d\n", __func__, prediv);

    cpll0.bf.DIV_PREDIV_SEL = prediv;



    /* reprogram the CPLL DIVN */
    ca7774_clk_dev_dbg("%s reprogramming CPLL DIVN\n", __func__);

    ca7774_clk_dev_dbg("%s setting divn to %d\n", __func__, divn);

    cpll0.bf.SCPU_DIV_DIVN = divn;


    ca7774_clk_dev_dbg("%s setting CPLL0 to 0x%08x\n", __func__,
                       cpll0.wrd);

    writel(cpll0.wrd, base + 0x160);


    mdelay(10);



    /* reprogram CPLL Cortex DIVSEL */
    cplldiv.wrd = readl(base + 0x134);

    ca7774_clk_dev_dbg("%s reprogramm CORTEX DIVSEL\n", __func__);

    ca7774_clk_dev_dbg("%s setting cortex divsel to 0x%x\n", __func__,
                       divsel);

    cplldiv.bf.cortex_divsel = divsel;

    writel(cplldiv.wrd, base + 0x134);



    /* switch back to CPLL clock */
    ca7774_clk_dev_dbg("%s Switch back to using CPLL clock\n", __func__);


    cplldiv.wrd = readl(base + 0x134);

    cplldiv.bf.cf_sel = 0;

    cplldiv.bf.cpll_mode_override = 1;

    cplldiv.bf.cpll_div_override = 1;

    ca7774_clk_dev_dbg("%s setting CPLLDIV to 0x%08x\n", __func__,
                       cplldiv.wrd);

    writel(cplldiv.wrd, base + 0x134);

    udelay(MINIMUM_DELAY_PLL_FOR_LOCK);

}



static void rtk_cortex_init_and_set(struct
                                    ca7774_cpll_global_strap_shadow_entry
                                    globalFreqEntry)
{

    GLOBAL_CPLL0_t cpll0;

    GLOBAL_FPLL0_t fpll0;

    GLOBAL_CPLLDIV_t cplldiv;


    ca7774_clk_dev_dbg("%s line %d\n", __func__, __LINE__);

    ca7774_clk_dev_dbg("base 0x%lx\n", (unsigned long int) base);


    ca7774_clk_dev_dbg("Writing setup and guard timings to CPLLMUX\n");

    /* magic values received from HW team */
    ca7774_clk_dev_dbg("%s DUMP write to CPLLMUX 0x%08x\n", __func__,
                       0xff000000);

    writel(0xff000000, base + 0x138);


#if 1                           /* dump the resulting registers */
    ca7774_clk_dev_dbg("%s DUMP CPLLDIV 0x%08x\n", __func__,
                       readl(base + 0x134));

    ca7774_clk_dev_dbg("%s DUMP CPLLMUX 0x%08x\n", __func__,
                       readl(base + 0x138));

    ca7774_clk_dev_dbg("%s DUMP CPLL0 0x%08x\n", __func__,
                       readl(base + 0x160));

    ca7774_clk_dev_dbg("%s DUMP FPLL0 0x%08x\n", __func__,
                       readl(base + 0x170));


#endif                          /* 
                                 */

    ca7774_clk_dev_dbg("Setup FPLL to 1600 Mhz\n");

    fpll0.wrd = readl(base + 0x170);

    fpll0.bf.DIV_PREDIV_BPS = 0;

    /* divide by 1 + 2 = 3, 25Mhz/3 = 8.33333 Mhz */
    fpll0.bf.DIV_PREDIV_SEL = 1;

    fpll0.bf.SCPU_RSTB = 1;

    fpll0.bf.SCPU_POW = 1;

    /* multiple by 189 + 3 = 192, 8.3333 Mhz & 192 = 1600 Mhz */
    fpll0.bf.SCPU_DIV_DIVN = 189;

    ca7774_clk_dev_dbg("%s DUMP write to FPLL0 0x%08x\n", __func__,
                       fpll0.wrd);

    writel(fpll0.wrd, base + 0x170);

    mdelay(100);


    ca7774_clk_dev_dbg("Setting Cortex FPLL DIVSEL to /2\n");


    cplldiv.wrd = readl(base + 0x134);

    cplldiv.bf.f2c_divsel = 2 << 1;

    ca7774_clk_dev_dbg("%s DUMP write to CPLLDIV 0x%08x\n", __func__,
                       cplldiv.wrd);

    writel(cplldiv.wrd, base + 0x134);

    mdelay(100);


    ca7774_clk_dev_dbg("Temporarily Switching Cortex Clk to FPLL\n");

    cplldiv.wrd = readl(base + 0x134);

    cplldiv.bf.cpll_div_override = 0;

    cplldiv.bf.cci_onetoone = 0;

    cplldiv.bf.cpll_mode_override = 0;

    cplldiv.bf.cf_sel = 1;

    ca7774_clk_dev_dbg("%s DUMP write to CPLLDIV 0x%08x\n", __func__,
                       cplldiv.wrd);

    writel(cplldiv.wrd, base + 0x134);

    mdelay(100);


    ca7774_clk_dev_dbg
        ("Setting up dynamic CPLL match requested Global Strap Mode Values\n");

    cpll0.wrd = readl(base + 0x160);

    cpll0.bf.DIV_PREDIV_BPS = 0;

    ca7774_clk_dev_dbg("Setting CPLL.PREDIV to %d\n",
                       globalFreqEntry.prediv);

    cpll0.bf.DIV_PREDIV_SEL = globalFreqEntry.prediv;

    cpll0.bf.SCPU_RSTB = 1;

    cpll0.bf.SCPU_POW = 1;

    ca7774_clk_dev_dbg("Settting CPLL.DIVN to %d\n", globalFreqEntry.divn);

    cpll0.bf.SCPU_DIV_DIVN = globalFreqEntry.divn;

    ca7774_clk_dev_dbg("%s DUMP write to CPLL0 0x%08x\n", __func__,
                       cpll0.wrd);

    writel(cpll0.wrd, base + 0x160);

    ca7774_clk_dev_dbg
        ("%s DUMP waiting for new CPLL0 values to stabilize\n", __func__);

    udelay(MINIMUM_DELAY_PLL_FOR_LOCK);


    cplldiv.wrd = readl(base + 0x134);

    ca7774_clk_dev_dbg("Setting Cortex CPLL DIVSEL to 0x%x\n",
                       globalFreqEntry.divsel);

    cplldiv.bf.cortex_divsel = globalFreqEntry.divsel;

    cplldiv.bf.cpll_div_override = 1;

    cplldiv.bf.cpll_mode_override = 1;

    cplldiv.bf.cf_sel = 0;

    ca7774_clk_dev_dbg("%s DUMP write to CPLLDIV 0x%08x\n", __func__,
                       cplldiv.wrd);

    ca7774_clk_dev_dbg("Switching Cortex Clk back to CPLL/divsel\n");

    writel(cplldiv.wrd, base + 0x134);

    udelay(MINIMUM_DELAY_PLL_FOR_SWITCH);



}

int rtk_cortex_freq_set(uint speed_tlb_idx)
{


    struct ca7774_cpll_global_strap_shadow_entry globalFreqEntry;

    unsigned long current_rate;


    printf("speed_tlb_idx=%u, %lu ,%lu\n", speed_tlb_idx,
           sizeof(ca7774_cpll_global_strap_table),
           sizeof(struct ca7774_cpll_global_strap_shadow_entry));

    if (speed_tlb_idx >=
        (sizeof(ca7774_cpll_global_strap_table) /
         sizeof(struct ca7774_cpll_global_strap_shadow_entry))) {

        printf("Error: speed_tlb_idx=%u, %lu ,%lu\n", speed_tlb_idx,
               sizeof(ca7774_cpll_global_strap_table),
               sizeof(struct ca7774_cpll_global_strap_shadow_entry));

        return -1;

    }



    globalFreqEntry = ca7774_cpll_global_strap_table[speed_tlb_idx];


    if (rtk_cortex_cpll_init_check()) {

        rtk_cortex_set(globalFreqEntry);

    } else {

        rtk_cortex_init_and_set(globalFreqEntry);

    }



    current_rate = rtk_cortex_freq_get();

    printf("Current CPU Frequency: %lu\n", current_rate);


    return 0;

}
