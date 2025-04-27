#include <common.h>
#include <config.h>
#include <soc.h>
#include <spi_flash.h>
#include <asm/symb_define.h>
#include "nor_spi_gen3/nor_spif_core.h"

#define FLASHI                   (norsf_info)

int otto_spi_flash_read(struct spi_flash *flash,  u32 offset, size_t len, void *buf);
int otto_spi_flash_write(struct spi_flash *flash, u32 offset, size_t len, const void *buf);
int otto_spi_flash_erase(struct spi_flash *flash, u32 offset, size_t len);

static struct spi_slave otto_slave;
static struct spi_flash otto_sf;

static uint32_t enable_message = 0;

#ifdef CONFIG_CMD_JFFS2
#include <flash.h>
flash_info_t flash_info[2];		//FIXME if more than 2 flash supported
#endif

void spi_disable_message(void) {
	enable_message = 0;
	return;
}

void spi_enable_message(void) {
	enable_message = 1;
	return;
}

void spi_flash_init(void) {
	norsf_detect();

	printf("%dx%d MB (plr_flash_info @ %p)\n",
	       FLASHI.num_chips, FLASHI.size_per_chip_b / 1024 / 1024, &norsf_info);

	otto_sf.spi = &otto_slave;
	otto_sf.name = NULL;
	otto_sf.size = FLASHI.size_per_chip_b;
	otto_sf.page_size = FLASHI.cmd_info->cprog_lim_b;
	otto_sf.sector_size = FLASHI.sec_sz_b;
	otto_sf.read  = otto_spi_flash_read;
	otto_sf.write = otto_spi_flash_write;
	otto_sf.erase = otto_spi_flash_erase;

#ifdef CONFIG_CMD_JFFS2
	unsigned int i, j, offset;
	unsigned int unit_size = FLASHI.sec_sz_b;
	unsigned int num_chips = FLASHI.num_chips;
//	printf("[debug] start to init flash_info (%d)\n", num_chips);
	for (i=0; i<num_chips; i++) {
		flash_info[i].size = otto_sf.size;
		flash_info[i].sector_count = (flash_info[i].size / unit_size);
		flash_info[i].flash_id = norsf_rdid(0);
		for (j=0, offset=0;
		     offset<(otto_sf.size);
		     j++, offset+=unit_size) {
			flash_info[i].start[j] = CONFIG_SYS_FLASH_BASE+offset;
		}
//		printf("[debug] <%d> unit size=0x%x, unit count=%d\n",
//		       i, unit_size, flash_info[i].sector_count);
	}
#endif
	return;
}

struct spi_flash *
spi_flash_probe(unsigned int bus, unsigned int cs,
                unsigned int max_hz __attribute__((unused)),
                unsigned int spi_mode __attribute__((unused))) {
	if (cs >= FLASHI.num_chips) {
		return NULL;
	}

	otto_slave.bus = bus;
	otto_slave.cs = cs;

	return &otto_sf;
}

void spi_flash_free(struct spi_flash *flash __attribute__((unused))) {
	return;
};

int otto_spi_flash_read(struct spi_flash *flash, u32 offset,
                        size_t len, void *buf) {
	return NORSF_READ(offset, len, buf, 0);
}

int otto_spi_flash_write(struct spi_flash *flash, u32 offset,
                         size_t len, const void *buf) {
	int res = NORSF_PROG(offset, len, buf, enable_message);
	if (!res) {
		res = memcmp(buf, (void *)(NORSF_CFLASH_BASE + offset), len);
		if (res) {
			puts("EE: Programmed data differs from the source\n");
			printf("    Run `cmp.b %p %08x' for detail\n", buf, NORSF_CFLASH_BASE + offset);
		}
	}
	return res;
}

int otto_spi_flash_erase(struct spi_flash *flash, u32 offset,
                         size_t len) {
	return NORSF_ERASE(offset, len, enable_message, 0);
}

int sdk_spi_flash_chip_num(void) {
	return FLASHI.num_chips;
}

/* Return size by Bytes */
int sdk_spi_flash_chip_size(int chip_index) {
    if(chip_index < FLASHI.num_chips)
    	return FLASHI.size_per_chip_b;
    else
        return 0;
}


