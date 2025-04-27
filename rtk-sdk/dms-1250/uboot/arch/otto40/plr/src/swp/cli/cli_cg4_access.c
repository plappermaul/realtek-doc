//#include <string.h>
#include <soc.h>
#include <cg/cg.h>
#include <cli/cli_util.h>
#include <cli/cli_access.h>

cli_cmd_ret_t
cli_pll_setup(const void *user, u32_t argc, const char *argv[]) {
	cg_xlat_n_assign();
	cg_init();
	cg_result_decode();
	return CCR_OK;
}

cli_add_node(pll, get, VZERO);
cli_add_parent(pll, set);

#define PLL_INFO	    cg_info_query.dev_freq


#define DEFINE_PLL_INT_VAR(name, is_dec, get_func_body, set_func_body) \
    SECTION_CLI_VAR int _CLI_VAR_CLI_ ## name ## _get_int_(u32_t *result) {get_func_body; return 0;} \
    SECTION_CLI_VAR int _CLI_VAR_CLI_ ## name ## _set_int_(u32_t value) {set_func_body; return 0;} \
    CLI_DEFINE_VAR(name, pll, 1, 0, is_dec, \
        _CLI_VAR_CLI_ ## name ## _get_int_, \
        _CLI_VAR_CLI_ ## name ## _set_int_)

DEFINE_PLL_INT_VAR(ocp_mhz, 1, {*result=PLL_INFO.ocp_mhz;}, {PLL_INFO.ocp_mhz=value;});
DEFINE_PLL_INT_VAR(mem_mhz, 1, {*result=PLL_INFO.mem_mhz;}, {PLL_INFO.mem_mhz=value;});
DEFINE_PLL_INT_VAR(lx_mhz, 1, {*result=PLL_INFO.lx_mhz;}, {PLL_INFO.lx_mhz=value;});
DEFINE_PLL_INT_VAR(spif_mhz, 1, {*result=PLL_INFO.spif_mhz;}, {PLL_INFO.spif_mhz=value;});
