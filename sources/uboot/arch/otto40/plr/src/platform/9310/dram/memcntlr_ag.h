#ifndef MEMCNTLR_AG_H
#define MEMCNTLR_AG_H

#ifndef SECTION_MC_VAR
    #define SECTION_MC_VAR          __attribute__ ((section (".mc_var")))
#endif

typedef void (mc_dump_var_t)(mc_info_t *r);
typedef void (mc_xlat_var_t)(mc_info_t *r);

typedef struct {
    const char *name;   // remove?
    mc_dump_var_t *dump;
    mc_xlat_var_t *xlat;
} mc_var_t;

// type = dram_param, cntlr_opt, register_set
#define DEFINE_RXI310_VAR(_var, _type, xlat_func) \
    MEMCNTLR_SECTION void mc_dump_##_var(mc_info_t *r) { \
        printf("  " #_var ": %d\n", r->_type->_var); \
	    return; } \
    mc_var_t _MC_VAR_##_var##_ SECTION_MC_VAR = {\
        .name = #_var,\
        .dump = mc_dump_##_var,\
        .xlat = xlat_func,\
    };


    /* CK:   dpi_post_pi_sel0,
       CMD:  dpi_post_pi_sel1,
       DQS0: dpi_post_pi_sel2,
       DQS1: dpi_post_pi_sel3,
       DQS2: dpi_post_pi_sel4,
       DQS3: dpi_post_pi_sel5,
       CK1:  dpi_post_pi_sel6 */

    /* Reg_dpi_mck_clk_en
       ck_en[7 :0] :clock enable
            ck_en[0] :mck_ck
            ck_en[1] :mck_cmd
            ck_en[2] :mck_dqs_0
            ck_en[3] :mck_dqs_1
            ck_en[4] :mck_dqs_2
            ck_en[5] :mck_dqs_3
            ck_en[6] :dll_clk
            ck_en[7] :mck_ck_1 */

    /* reg_dpi_clk_oe[6:0]
       clk_oe[6:0]: PLL clock output enable
           clk_oe[0]:mck_ck
           clk_oe[1]:mck_cmd
           clk_oe[2]:mck_dqs_0
           clk_oe[3]:mck_dqs_1
           clk_oe[4]:mck_dqs_2
           clk_oe[5]:mck_dqs_3
           clk_oe[6]:mck_ck_1 */

#endif //MEMCNTLR_AG_H
