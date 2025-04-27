#ifndef _MEMCTL_8685_CFUNC__
#define _MEMCTL_8685_CFUNC__

/* Data structure used for storing tap statics of the DDR calibration experiences. */
typedef struct{
                unsigned int mode;
                unsigned int mhz;
                unsigned int tap_min;
                unsigned int tap_max;
}tap_info_t;

#define _memctl_debug_printf(...)
typedef unsigned int uint32;
typedef unsigned char uint8;

/* C function */
void _memctl_update_phy_param(void);
unsigned int memctlc_DDR_Type(void);

#ifdef DDR1_USAGE
void _DTR_DDR1_MRS_setting(unsigned int *mr);
#endif

#ifdef DDR2_USAGE
void _DTR_DDR2_MRS_setting(unsigned int *mr);
#endif

#ifdef DDR3_USAGE
void _DTR_DDR3_MRS_setting(unsigned int *sug_dtr, unsigned int *mr);
#endif

void memctlc_init_dram_8685(void);
unsigned int board_DRAM_freq_mhz(void);
uint32 DDR_Calibration(unsigned char full_scan);
void _memctl_DCache_flush_invalidate(void);


#endif
