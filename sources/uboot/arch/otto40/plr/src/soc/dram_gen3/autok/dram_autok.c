#include <dram/autok/dram_autok.h>

extern void memctlc_config_DTR(unsigned int default_instruction, unsigned int dram_size);
extern u32_t DDR_Calibration(unsigned char full_scan);
extern unsigned int memctlc_config_DRAM_size(void);
extern int memctlc_ZQ_calibration(unsigned int auto_cali_value);
extern void memctlc_dll_setup(void);
extern void memctrlc_config_DRAM_MCR_SETTING(unsigned int);

__attribute__((weak)) u8_t efuse_6_rd(void) {
	return 0;
}

unsigned int auto_cali_value[]= {
	CONFIG_DRAM_PREFERED_ZQ_PROGRAM
};


SECTION_AUTOK
void memctlc_dram_phy_reset(void)
{
    REG32(DACCR) = REG32(DACCR) & ((u32_t) 0xFFFFFFEF);
    REG32(DACCR) = REG32(DACCR) | ((u32_t) 0x10);

    return;
}


SECTION_AUTOK
u32_t dram_autok(void) {
	unsigned int dram_size=0;
	u32_t ret = 0;
#ifdef AUTOK_DEBUG
	int j;
    unsigned int *ptr;
#endif
#ifdef CONFIG_FULL_AUTOK
	int i;
	int is_zq_fail;
#endif

	printf("\nDRAM AUTO CALIBRATION\n");

	do {
#ifdef CONFIG_FULL_AUTOK
		/* gerneal DTR config */
		memctlc_config_DTR(1, dram_size);

		/*ZQ Configuration*/
		is_zq_fail = 1;

		auto_cali_value[0] = mc_akh_get_zq_setting(auto_cali_value[0]);

		if(is_zq_fail){//user-defined value fail, try other predefine value
			for(i=0; i< (sizeof(auto_cali_value)/sizeof(unsigned int));i++){
				if(0 == memctlc_ZQ_calibration(auto_cali_value[i])){
					/* We found one .*/
					break;
				}
			}
			if(i >= (sizeof(auto_cali_value)/sizeof(unsigned int)) ){
				printf("ZQ calibration failed\n");
				break;
			}
		}

		/*DLL setup*/
		memctlc_dll_setup();
#endif
		/* DQ delay tap selection */
		ret = DDR_Calibration(1);
		if (ret != 0) break;

#ifdef CONFIG_FULL_AUTOK
		/* reset dram phy */
		memctlc_dram_phy_reset();

		/* dram size detection */
		if (efuse_6_rd()) {
			dram_size = 0x00800000 << efuse_6_rd();
			memctrlc_config_DRAM_MCR_SETTING(dram_size);
		} else {
			dram_size = memctlc_config_DRAM_size();
		}

		/* set DTR accroding to dram size */
		memctlc_config_DTR(0, dram_size);
#endif

#ifdef AUTOK_DEBUG
		/* print regs settings after autok*/
		printf("II: Regs Settings:\n");
		printf("0xb8001000: 0x%08x\n", REG32(0xb8001000));
		printf("0xb8001004: 0x%08x\n", REG32(0xb8001004));
		printf("0xb8001008: 0x%08x\n", REG32(0xb8001008));
		printf("0xb800100c: 0x%08x\n", REG32(0xb800100c));
		printf("0xb8001010: 0x%08x\n", REG32(0xb8001010));
		printf("0xb800021c: 0x%08x\n", REG32(0xb800021c));
		printf("0xb8001090: 0x%08x\n", REG32(0xb8001090));
		printf("0xb8001094: 0x%08x\n", REG32(0xb8001094));
		printf("0xb8001590: 0x%08x\n", REG32(0xb8001590));
		printf("0xb8001050: 0x%08x\n", REG32(0xb8001050));
		printf("0xb8001500: 0x%08x\n", REG32(0xb8001500));
		printf("0xb8000208: 0x%08x\n", REG32(0xb8000208));
		printf("0xb800020c: 0x%08x\n", REG32(0xb800020c));
		printf("0xb8000210: 0x%08x\n", REG32(0xb8000210));
		printf("0xb80015b0: 0x%08x\n", REG32(0xb80015b0));
		printf("0xb80015b4: 0x%08x\n", REG32(0xb80015b4));
		printf("0xb80015b8: 0x%08x\n", REG32(0xb80015b8));
		printf("0xb80015bc: 0x%08x\n", REG32(0xb80015bc));

		ptr = (volatile unsigned int *) 0xb8001510;
		for(j=0;j < 32; j++) {
			printf("0x%08x: 0x%08x\n",(ptr+j), *(ptr+j));
		}
#endif
		printf("AutoK: dram auto calibrtaion is done\n\n");
		return ret;
	} while(0);

	printf("AutoK: dram auto calibrtaion failed!!!\n\n");
	return ret;
}
