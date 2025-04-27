#include <string.h>
#include <soc.h>
#include <dram/memcntlr.h>
#include <cli/cli_util.h>
#include <cli/cli_access.h>

#ifndef SECTION_CLI_ACCESS
#define SECTION_CLI_ACCESS
#endif

#ifndef SECTION_CLI_UTIL
#define SECTION_CLI_UTIL
#endif

void dram_test(void);
void dram_autok(void);

SECTION_CLI_ACCESS void
cli_ddr_test(void) {
	dram_test();
}

SECTION_CLI_ACCESS void
cli_ddr_scan(void) {
	dram_autok();
}

SECTION_CLI_ACCESS void
cli_ddr_reginfo(void) {
	int i;
	volatile unsigned int *ptr;

	/* print regs settings after autok*/
	printf("II: Regs Info:\n");
	printf("MCR\t(0xb8001000): 0x%08x\n", REG32(0xb8001000));
	printf("DCR\t(0xb8001004): 0x%08x\n", REG32(0xb8001004));
	printf("DTR0\t(0xb8001008): 0x%08x\n", REG32(0xb8001008));
	printf("DTR1\t(0xb800100c): 0x%08x\n", REG32(0xb800100c));
	printf("DTR2\t(0xb8001010): 0x%08x\n", REG32(0xb8001010));
	printf("DDZQPR\t(0xb8001090): 0x%08x\n", REG32(0xb8001090));
	printf("DDZQPCR\t(0xb8001094): 0x%08x\n", REG32(0xb8001094));
	printf("DCDQMR\t(0xb8001590): 0x%08x\n", REG32(0xb8001590));
	printf("DIDER\t(0xb8001050): 0x%08x\n", REG32(0xb8001050));
	printf("DACCR\t(0xb8001500): 0x%08x\n", REG32(0xb8001500));
	printf("PLL0\t(0xb8000208): 0x%08x\n", REG32(0xb8000208));
	printf("PLL1\t(0xb800020c): 0x%08x\n", REG32(0xb800020c));
	printf("PLL2\t(0xb8000210): 0x%08x\n", REG32(0xb8000210));
	printf("DCSDCR0\t(0xb80015b0): 0x%08x\n", REG32(0xb80015b0));
	printf("DCSDCR1\t(0xb80015b4): 0x%08x\n", REG32(0xb80015b4));
	printf("DCSDCR2\t(0xb80015b8): 0x%08x\n", REG32(0xb80015b8));
	printf("DCSDCR3\t(0xb80015bc): 0x%08x\n", REG32(0xb80015bc));
	printf("DDRCKODL(0xb800021c): 0x%08x\n", REG32(0xb800021c));
	printf("ANA_DLL0(0xb8000608): 0x%08x\n", REG32(0xb8000608));
	printf("ANA_DLL1(0xb800060c): 0x%08x\n", REG32(0xb800060c));

	printf("\nDACDQ_IDX_RR/FR:\n");
	ptr = (volatile unsigned int *) 0xb8001510;
	for(i=0;i < 32; i++) {
		printf("0x%08x: 0x%08x\n",(ptr+i), *(ptr+i));
	}
}

SECTION_CLI_UTIL cli_cmd_ret_t
cli_std_load_byte(const void *user, u32_t argc, const char *argv[]) {
	if (argc<2) return CCR_INCOMPLETE_CMD;

	u32_t addr=atoi(argv[1]);
	if(argc==2) {      
		printf("$=0x%02x\n", (*(volatile unsigned char *)(addr)));
	} else{
		printf("$=0x%02x, only shows single byte now.\n", (*(volatile unsigned char *)(addr)));
	}
	return CCR_OK;
}


SECTION_CLI_UTIL cli_cmd_ret_t
cli_std_store_byte(const void *user, u32_t argc, const char *argv[]) {
	if (argc<3) return CCR_INCOMPLETE_CMD;
	u32_t addr=atoi(argv[1]);
	u8_t arg=atoi(argv[2]);
	(*(volatile unsigned char *)(addr))=arg;
	return CCR_OK;
}

cli_top_node(db, cli_std_load_byte);
cli_add_help(load_byte, "db <addr>");
cli_top_node(eb, cli_std_store_byte);
cli_add_help(store_byte, "eb <addr> <value>");

cli_add_node(ddr_test, call, (cli_cmd_func_t *)cli_ddr_test);
cli_add_help(ddr_test, "call ddr_test");
cli_add_node(ddr_scan, call, (cli_cmd_func_t *)cli_ddr_scan);
cli_add_help(ddr_scan, "call ddr_scan");
cli_add_node(ddr_reginfo, call, (cli_cmd_func_t *)cli_ddr_reginfo);
cli_add_help(ddr_reginfo, "call ddr_reginfo");
