//#include <string.h>
#include <soc.h>
#include <cg/cg.h>
#include <cli/cli_util.h>
#include <cli/cli_access.h>

#define PLL_INFO	    cg_info_query.dev_freq
//extern void mc_rxi310_init(void);
//extern void mc_cntlr_zq_calibration(void);
extern int cg_config_transform(cg_config_t *c);
extern void cg_cpu_pll_init(cg_config_t *c);
extern void cg_lx_pll_init(cg_config_t *c);
extern void cg_spif_pll_init(cg_config_t *c);
cli_cmd_ret_t
cli_pll_setup(const void *user, u32_t argc, const char *argv[]) {
	//cg_init();
	cg_config_t c;
	cg_config_transform(&c);
	cg_cpu_pll_init(&c);
	cg_lx_pll_init(&c);
	cg_spif_pll_init(&c);
	extern cg_info_t cg_info_query;
	printf("II: CPU %dMHz, MEM %dMHz, LX %dMHz, SPIF %dMHz, SNAF %dMHz\n",
		cg_info_query.dev_freq.cpu_mhz,
		cg_info_query.dev_freq.mem_mhz,
		cg_info_query.dev_freq.lx_mhz,
		cg_info_query.dev_freq.spif_mhz,
		cg_info_query.dev_freq.snaf_mhz);
	return CCR_OK;
}

cli_add_node(pll, get, VZERO);
cli_add_parent(pll, set);

#define DEFINE_PLL_INT_VAR(name, is_dec, get_func_body, set_func_body) \
    SECTION_CLI_VAR int _CLI_VAR_CLI_ ## name ## _get_int_(u32_t *result) {get_func_body; return 0;} \
    SECTION_CLI_VAR int _CLI_VAR_CLI_ ## name ## _set_int_(u32_t value) {set_func_body; return 0;} \
    CLI_DEFINE_VAR(name, pll, 1, 0, is_dec, \
        _CLI_VAR_CLI_ ## name ## _get_int_, \
        _CLI_VAR_CLI_ ## name ## _set_int_)

DEFINE_PLL_INT_VAR(cpu_mhz, 1, {*result=PLL_INFO.cpu_mhz;}, {PLL_INFO.cpu_mhz=value;});
DEFINE_PLL_INT_VAR(mem_mhz, 1, {*result=PLL_INFO.mem_mhz;}, {PLL_INFO.mem_mhz=value;});
DEFINE_PLL_INT_VAR(lx_mhz, 1, {*result=PLL_INFO.lx_mhz;}, {PLL_INFO.lx_mhz=value;});
DEFINE_PLL_INT_VAR(spif_mhz, 1, {*result=PLL_INFO.spif_mhz;}, {PLL_INFO.spif_mhz=value;});
DEFINE_PLL_INT_VAR(snaf_mhz, 1, {*result=PLL_INFO.snaf_mhz;}, {PLL_INFO.snaf_mhz=value;});
