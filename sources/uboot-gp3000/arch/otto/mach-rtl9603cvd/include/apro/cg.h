#ifndef __APRO_CG_HEADER__
#define __APRO_CG_HEADER__

#include <reg_map_util.h>
#include <apro/cg_dev_freq.h>
#include <apro/ocp_pll.h>
#include <apro/mem_pll.h>
#include <apro/cmu.h>

/****** Default PLL Frequency Information******/
#define CPU_CLK_DEFAULT     (500)
#define LX_PLL_DEFAULT      (200)
#define LX_PLL_DEFAULT_DIV  (0)
#define NOR_PLL_DEFAULT     (200)
#define NOR_PLL_DEFAULT_DIV (1)
#define ROM_CLK_DEFAULT     (250)
#define OCP0_PLL_MIN        (500)
#define OCP0_PLL_MAX        (1200)
#define OCP1_PLL_MIN        (400)
#define OCP1_PLL_MAX        (600)

typedef struct {
    SYS_OCP_PLL_CTRL0_T sys_ocp_pll_ctrl0;
    SYS_OCP_PLL_CTRL1_T sys_ocp_pll_ctrl1;
    SYS_OCP_PLL_CTRL3_T sys_ocp_pll_ctrl3;

    SYS_MEM_PLL_CTRL0_T sys_mem_pll_ctrl0;
    SYS_MEM_PLL_CTRL1_T sys_mem_pll_ctrl1;
    SYS_MEM_PLL_CTRL2_T sys_mem_pll_ctrl2;
    SYS_MEM_PLL_CTRL3_T sys_mem_pll_ctrl3;
    SYS_MEM_PLL_CTRL4_T sys_mem_pll_ctrl4;
    SYS_MEM_PLL_CTRL5_T sys_mem_pll_ctrl5;
    SYS_MEM_PLL_CTRL6_T sys_mem_pll_ctrl6;

    PHY_RG5X_PLL_T      phy_rg5x_pll;

    u16_t sclk_div2ctrl; /* for SPI NAND & SPI NOR controller divisor */
    u32_t spif_rx_delay; /* for SPI NAND & SPI NOR RX delay latency */

    OC0_CMUGCR_T    oc0_cmugcr;  /* for CMU static division */
    OC0_CMUCR_T     oc0_cmucr;   /* for CPU0 slow bit setting */
    OC1_CMUGCR_T    oc1_cmugcr;  /* for CMU static division */
    OC1_CMUCR_T     oc1_cmucr;   /* for CPU1 slow bit setting */
    LBSBCR_apro_T   lx_sbsr;     /* for LXs slow bit setting */
    SCATS_T         scats;       /* for SRAM slow bit setting */
} cg_register_set_t;


#define END_OF_INFO (0xFFFF)

typedef struct {
    u16_t pll_freq_mhz;
    u16_t pll_cfg_div;
    u16_t cal_sclk_mhz;
    u16_t sclk_div2ctrl;
} cg_sclk_info_t;

typedef struct pll_freq_sel_info_s{
    u16_t freq_mhz;
    u16_t cfg_div;
}pll_freq_sel_info_t;

u32_t cg_query_freq(u32_t dev_type);

extern cg_dev_freq_t cg_target_freq;
extern cg_dev_freq_t cg_cur_query;

#ifndef GET_MEM_MHZ
#define GET_MEM_MHZ() (cg_query_freq(CG_DEV_MEM))
#endif

#ifndef GET_CPU_MHZ
#define GET_CPU_MHZ() (cg_query_freq(CG_DEV_CPU0))
#endif

extern u32_t nsu_sclk_limit(u32_t cur_sclk);
extern void set_spi_ctrl_latency(u16_t latency);
extern void set_spi_ctrl_divisor(u16_t clk_div, u16_t spif_mhz);
extern u32_t get_spi_ctrl_divisor(void);
extern u32_t get_default_spi_ctrl_divisor(void);
extern void cg_init(void);

typedef union {
	struct {
		unsigned int no_use31:25; //0
		unsigned int one_ejtag_sel:1; //0
		unsigned int sys_cpu2_en:1; //0
		unsigned int cf_cksel_lx:1; //1
		unsigned int cf_ckse_ocp1:1; //1
		unsigned int cf_ckse_ocp0:1; //1
		unsigned int rdy_for_pathch:1; //0
		unsigned int soc_init_rdy:1; //0
	} f;
	unsigned int v;
} SYS_STATUS_T;
#define SYS_STATUSrv (*((regval)0xb8000044))
#define SYS_STATUSdv (0x0000004C)
#define RMOD_SYS_STATUS(...) rset(SYS_STATUS, SYS_STATUSrv, __VA_ARGS__)
#define RIZS_SYS_STATUS(...) rset(SYS_STATUS, 0, __VA_ARGS__)
#define RFLD_SYS_STATUS(fld) (*((const volatile SYS_STATUS_T *)0xb8000044)).f.fld

#endif
