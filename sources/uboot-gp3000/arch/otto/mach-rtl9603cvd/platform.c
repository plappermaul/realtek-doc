#include <common.h>
#include <asm/mipsregs.h>
#include <swcore_reg.h>
#include <misc_setting.h>

#define MIPS_CONFIG1_ICACHE_OFF (16)
#define MIPS_CONFIG1_DCACHE_OFF (7)

#define OTTO_CID ((*(soc_t *)(0x9F000020)).cid)&0xFFFF

#define ST_RTL9602CVD5CG (5)

enum {
	_9607c_string = 0,
	_9603c_string = 1,
	_9603cvd_string = 2,
	_9602cvd_string = 3,
	_default_string = 4,
};
static char *board_string[] = {
	[_9607c_string] = "9607C\0 \0",
	[_9603c_string] = "9603C\0 \0",
	[_9603cvd_string] = "9603CVD\0 \0",
	[_9602cvd_string] = "9602CVD\0 \0",
	[_default_string] = "9607C/03C/03CVD\0 \0",
};

int otto_is_apro(void) {
	return (RFLD(MODEL_NAME_INFO, rtl_id) == 0x9607);
}

static char *board_prompt_get(void) {
	char *prompt;

	if (otto_is_apro()) {
		switch (OTTO_CID) {
		case ST_RTL9603CT:
		case ST_RTL9603C:
		case ST_RTL9603CP:
		case ST_RTL9603CW:
		case ST_RTL9603CE:
			prompt = board_string[_9603c_string];
			break;
		case ST_RTL9607C:
		case ST_RTL9607CP:
		case ST_RTL9607E:
		case ST_RTL9607EP:
			prompt = board_string[_9607c_string];
			break;
		default:
			prompt = board_string[_default_string];
			break;
		}
	} else if ((RFLD(MODEL_NAME_INFO, rtl_id) == 0x9603)) {
		if ((OTTO_CID & 0xffff) == ST_RTL9602CVD5CG) {
			prompt = board_string[_9602cvd_string];
		} else {
			prompt = board_string[_9603cvd_string];
		}
	} else {
		prompt = board_string[_default_string];
	}
	return prompt;
}

static char *otto_cpu_name_string[] = {
	[0] = "MIPS interAptiv UP",
	[1] = "MIPS interAptiv",
	"N/A",
	"N/A",
	[4] = "MIPS 34Kc",
	[5] = "RLX 5281",
	"N/A",
	[7] = "RLX 5181",
	"N/A",
	[9] = "MIPS 4Kec"
};

static int otto_cpu_id(void) {
	return read_c0_prid();
}

static char* otto_cpu_name(void) {
	/* hash(prid) = (((id>>8)&0xf) + ((id>>12)&0xf)) % 10;
		 5281: 0x0???_dc02 => hash() => 5
		 5181: 0x0???_cf01 => hash() => 7
		 4KEc: 0x??01_90?? => hash() => 9
		 34Kc: 0x??01_95?? => hash() => 4
		 IA:   0x??01_a1?? => hash() => 1
		 IAUP: 0x??01_a0?? => hash() => 0
	*/
	uint32_t cpu_hash = otto_cpu_id();
	cpu_hash = (((cpu_hash>>8)&0xf) + ((cpu_hash>>12)&0xf)) % 10;
	return otto_cpu_name_string[cpu_hash];
}

static int otto_cpu_is_rlx5x81(void) {
	return ((otto_cpu_id()>>14) & 0x1);
}

static int otto_cache_line_size(int cache_type) {
	int cache_line_size_b;

	if (otto_cpu_is_rlx5x81()) {
		cache_line_size_b = CONFIG_SYS_CACHELINE_SIZE;
	} else {
		cache_line_size_b = read_c0_config1() >> (cache_type + 3);
		cache_line_size_b &= 0x7;
		cache_line_size_b = 1 << (cache_line_size_b + 1);
	}

	return cache_line_size_b;
}

static int otto_mips_cache_way_num(int cache_type) {
	int cache_way_num;

	cache_way_num = read_c0_config1() >> (cache_type + 0);
	cache_way_num &= 0x7;
	cache_way_num += 1;

	return cache_way_num;
}

static int otto_mips_cache_set_num(int cache_type) {
	int cache_set_num;

	cache_set_num = read_c0_config1() >> (cache_type + 6);
	cache_set_num &= 0x7;
	cache_set_num = 1 << (cache_set_num + 6);

	return cache_set_num;
}

static void otto_cache_dump_info(void) {
	int way, set, ls;
	if (otto_cpu_is_rlx5x81()) {
		printf("ICACHE: %dKB (%d-byte)\n", CONFIG_SYS_ICACHE_SIZE/1024, CONFIG_SYS_CACHELINE_SIZE);
		printf("DCACHE: %dKB (%d-byte)\n", CONFIG_SYS_DCACHE_SIZE/1024, CONFIG_SYS_CACHELINE_SIZE);
	} else {
		way = otto_mips_cache_way_num(MIPS_CONFIG1_ICACHE_OFF);
		set = otto_mips_cache_set_num(MIPS_CONFIG1_ICACHE_OFF);
		ls = otto_cache_line_size(MIPS_CONFIG1_ICACHE_OFF);
		printf("ICACHE: %dKB (%d-byte/%d-way/%d-set)\n",
		       way*set*ls/1024,
		       ls, way, set);
		way = otto_mips_cache_way_num(MIPS_CONFIG1_DCACHE_OFF);
		set = otto_mips_cache_set_num(MIPS_CONFIG1_DCACHE_OFF);
		ls = otto_cache_line_size(MIPS_CONFIG1_DCACHE_OFF);
		printf("DCACHE: %dKB (%d-byte/%d-way/%d-set)\n",
		       way*set*ls/1024,
		       ls, way, set);
	}
	return;
}

static uint32_t
dram_type(void) {
	uint32_t mcr = REG32(0xb8001000);
	return ((mcr >> 28) + 1);
}

int checkboard (void) {
	printf(
		"Board: RTL%s %s:%dMHz LX:%dMHz DDR%d:%dMHz\n",
		board_prompt_get(),
		otto_cpu_name(),
		GET_CPU_MHZ(),
		GET_LX_MHZ(),
		dram_type(),
		GET_MEM_MHZ());
	otto_cache_dump_info();

	return 0;
}

#ifdef CONFIG_SPI_NOR_FLASH
#	include <nor_spi/nor_spif_register.h>
#	define RFLD_SNAFCFR(x) ((REG32(0xb801a400) >> 4) & 0x7)
static void sw_set_spif_div(void) {
	RMOD_SFCR(spi_clk_div, RFLD_SNAFCFR(spi_clk_div));
	return;
}
PATCH_REG(sw_set_spif_div, 2);
#endif
