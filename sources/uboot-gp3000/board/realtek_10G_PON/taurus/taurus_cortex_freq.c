#include <common.h>
#include <asm/io.h>

uint64_t ref_rate  = 25000000;

struct taurus_pll_param {
	uint32_t pll;
	uint32_t prediv;
	uint32_t divn;
};

struct taurus_cpu_strap_speed {
	uint32_t pll;
	uint32_t div;
};

static const struct taurus_pll_param taurus_pll_param_table[] __maybe_unused = {
	{ .pll = 40, .prediv = 5, .divn = 200, },	/* CPU */
	{ .pll = 44, .prediv = 5, .divn = 220, },	/* CPU */
	{ .pll = 48, .prediv = 5, .divn = 240, },	/* CPU */
	{ .pll = 50, .prediv = 5, .divn = 250, },	/* CPU */
	{ .pll = 56, .prediv = 3, .divn = 168, },	/* CPU */
	{ .pll = 64, .prediv = 3, .divn = 192, },	/* FPLL/CPU */
	{ .pll = 72, .prediv = 3, .divn = 216, },	/* CPU */
	{ .pll = 80, .prediv = 2, .divn = 160, },	/* EPLL */
};

static const struct taurus_cpu_strap_speed taurus_cpu_strap_speed_table[] = {
	{ .pll = 48, .div = 3, }, /* 400000000 */
	{ .pll = 50, .div = 2, }, /* 625000000 */
	{ .pll = 56, .div = 2, }, /* 700000000 */
	{ .pll = 64, .div = 2, }, /* 800000000 */
	{ .pll = 72, .div = 2, }, /* 900000000 */
	{ .pll = 40, .div = 1, }, /* 1000000000 */
	{ .pll = 44, .div = 1, }, /* 1100000000 */
	{ .pll = 48, .div = 1, }, /* 1200000000 */
};

uint64_t rtk_cortex_freq_get(void)
{
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
    uint64_t mult, div;
    GLOBAL_CPLLDIV_t cplldiv;
    GLOBAL_CPLL0_t cpll0;

    int global_strap_mode_idx = 0;
    uint64_t current_rate;
    cplldiv.wrd = readl(GLOBAL_CPLLDIV);

    if ((cplldiv.bf.cpll_div_override == 1) && (cplldiv.bf.cf_sel == 0)) {
        cpll0.wrd = readl(GLOBAL_CPLL0);
        current_rate = DIV_ROUND_UP(ref_rate, (cpll0.bf.DIV_PREDIV_SEL + 2));
        current_rate = current_rate * (cpll0.bf.SCPU_DIV_DIVN + 3);
        current_rate = current_rate / ((cplldiv.bf.cortex_divsel >> 1) & 0xf);

        /* round down to nearist Khz */
        current_rate = current_rate / 1000;
        current_rate = current_rate * 1000;
    } else {
        global_strap_mode_idx = (readl(GLOBAL_STRAP) >> 1) & 0x7;
        if (global_strap_mode_idx >=  ARRAY_SIZE(taurus_cpu_strap_speed_table)) {
            printf("Error: global_strap_mode_idx=%u, Not supported!\n", global_strap_mode_idx);
            return 0;
        }
        mult = taurus_cpu_strap_speed_table[global_strap_mode_idx].pll;
        div = taurus_cpu_strap_speed_table[global_strap_mode_idx].div;
        current_rate = ref_rate * mult / div;
    }

    return current_rate;
}


