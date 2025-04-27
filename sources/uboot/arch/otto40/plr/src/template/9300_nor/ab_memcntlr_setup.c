//#include <soc.h>
//#include <info.h>
#include <dram/memcntlr.h>
#include <init_define.h>

extern void writeback_invalidate_dcache_all(void);
#define _memctl_DCache_flush_invalidate writeback_invalidate_dcache_all

#ifndef PROJECT_ON_FPGA

#define _REDUCE_CALIBRATION_
#define CONFIG_SOFTWARE_OVERWRITE_FREQ 1
#define CONFIG_DRAM_AUTO_TIMING_SETTING
#define CONFIG_DRAM_AUTO_SIZE_DETECTION
#define USE_OTTO_CG_FUNCTION


#define CONFIG_DRAM_AUTO_TYPES 1
//#define CONFIG_DDR2_USAGE 1
#define CONFIG_DDR3_USAGE 1

/*
 * DRAM Parameters Setting
 */
#define CONFIG_DRAM_AUTO_PARAMETERS 1

#define CONFIG_DDR2_DRAM_ODT_VALUE (75)
#define CONFIG_DDR3_DRAM_RTT_WR_VALUE (0)
#define CONFIG_DDR3_DRAM_RTT_NOM_VALUE (60)				//kevinchung
#define CONFIG_PREFERED_DRAM_DRIV_STRENGTH (0)			//0 : reduce

#define CONFIG_SYS_PLL_CTRL (0)


void puthex(int h) {
        printf("%x", h);
        return;
}
#include <dram/autok/board_mem_diag.c>
#include <dram/autok/memctl_cali_dram.c>
#include <dram/autok/memctl_utils.c>
#include <dram/autok/memctl.c>

void dram_setup(void) {
	puts("II: DRAM init start\n");
        memctlc_init_dram();
	puts("II: DRAM init done\n");
        ISTAT_SET(cal, MEM_CAL_OK);
        return;

}

REG_INIT_FUNC(dram_setup, 30);

#else  // PROJECT_ON_FPGA
#include <dram/fpga/memcntlr_setup.c>
#endif // PROJECT_ON_FPGA
