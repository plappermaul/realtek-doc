#include <common.h>
#include <asm/mipsregs.h>
#include <misc_setting.h>

#define MIPS_CONFIG1_ICACHE_OFF (16)
#define MIPS_CONFIG1_DCACHE_OFF (7)



#define ST_RTL9602CVD5CG (5)

static char *apro_board_string[] = {
    [ST_RTL9603C]  = "9603C\0 \0",
    [ST_RTL9603CP] = "9603CP\0 \0",
    [ST_RTL9603CW] = "9603CW\0 \0",
    [ST_RTL9603CE] = "9603CE\0 \0",
    [ST_RTL9606C]  = "9606C\0 \0",
    [ST_RTL9607C]  = "9607C\0 \0",
    [ST_RTL9607CP] = "9607CP\0 \0",
    [ST_RTL9607E]  = "9607E\0 \0",
    [ST_RTL9607EP] = "9607EP\0 \0",
    [ST_RTL8198D]  = "8198D\0 \0",
    [ST_RTL8198DE] = "9607c\0 \0",
};

enum {
	_9603cvd_string = 0,
	_9602cvd_string = 1,
};
static char *rtl9603cvd_board_string[] = {
	[_9603cvd_string] = "9603CVD\0 \0",
	[_9602cvd_string] = "9602CVD\0 \0",
};

char *board_prompt = "Unknown\0 \0";

static char *board_prompt_get(void) {
    char *prompt = board_prompt;
    if ((OTTO_SID == PLR_SID_APRO) || (OTTO_SID == PLR_SID_APRO_GEN2)) {
        switch (OTTO_CID) {
            case ST_RTL9603CT:
            case ST_RTL9603C:
            case ST_RTL9603CP:
            case ST_RTL9603CW:
            case ST_RTL9603CE:
            case ST_RTL9606C:
            case ST_RTL9607C:
            case ST_RTL9607CP:
            case ST_RTL9607E:
            case ST_RTL9607EP:
            case ST_RTL8198D:
            case ST_RTL8198DE:
                prompt = apro_board_string[OTTO_CID];
                break;
        }
    } else if (OTTO_SID == PLR_SID_9603CVD) {
        if ((OTTO_CID & 0xffff) == ST_RTL9602CVD5CG) {
            prompt = rtl9603cvd_board_string[_9602cvd_string];
        } else {
            prompt = rtl9603cvd_board_string[_9603cvd_string];
        }
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

static char* otto_soc_gen(void)
{
    char *soc_gen = "\0";
    if(OTTO_SID == 1){
        soc_gen = "v2";
    }
    return soc_gen;
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
		"Board: RTL%s%s, %s:%dMHz RLX5281:%dMHz LX:%dMHz DDR%d:%dMHz SCLK:%dMHz\n",
		board_prompt_get(),
        otto_soc_gen(),
		otto_cpu_name(),
		GET_CPU0_MHZ(),
		GET_CPU1_MHZ(),
		GET_LX_MHZ(),
		dram_type(),
		GET_MEM_MHZ(),
		GET_SPIF_MHZ());
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
