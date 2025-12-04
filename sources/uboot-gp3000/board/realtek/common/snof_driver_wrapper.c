#include <common.h>
#include <spi.h>
#include <spi_flash.h>

#include <nor_spi/nor_spif_core.h>

#define OTTO_SNOF_UNUSED_P ((void *)0)
#define OTTO_SNOF_UNUSED (0)

static struct spi_slave otto_slave;
static struct spi_flash otto_sf;
static uint32_t enable_message = 1;

static int otto_snof_lock(struct spi_nor *flash, loff_t ofs, uint64_t len) {
	return 0;
};

static int otto_snof_unlock(struct spi_nor *flash, loff_t ofs, uint64_t len) {
	return 0;
}

static int otto_snof_is_locked(struct spi_nor *flash, loff_t ofs, uint64_t len) {
	return 0;
}

static int otto_snof_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf) {
	*retlen = len;
	return NORSF_READ(from, len, buf, 0);
}

static int otto_snof_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u_char *buf) {
	int res = NORSF_PROG(to, len, buf, enable_message);
	*retlen = len;
	if (!res) {
		res = memcmp(buf, (void *)((ulong)(CONFIG_SYS_FLASH_BASE + to)), len);
		if (res) {
			puts("EE: Programmed data differs from the source\n");
			printf("    Run `cmp.b %p %08lx 0x%x' for detail\n", buf, (ulong)(CONFIG_SYS_FLASH_BASE + to), len);
		}
	}
	return res;
}

static int otto_snof_erase(struct mtd_info *mtd, struct erase_info *instr) {
	return NORSF_ERASE(instr->addr, instr->len, enable_message, 0);
}

struct spi_flash *
spi_flash_probe(
	unsigned int busnum, unsigned int cs,
	unsigned int max_hz, unsigned int spi_mode) {

	norsf_detect();

	/* maybe some fields are not required. To be clarified. */
	otto_slave.bus = busnum;
	otto_slave.cs = cs;
	otto_slave.mode = spi_mode;
	otto_slave.wordlen = 8;
	otto_slave.max_read_size = OTTO_SNOF_UNUSED;
	otto_slave.max_write_size = OTTO_SNOF_UNUSED;
	otto_slave.memory_map = OTTO_SNOF_UNUSED_P;
	otto_slave.flags = OTTO_SNOF_UNUSED;

	otto_sf.spi = &otto_slave;
	otto_sf.name = "Otto SPI NOR flash cntlr. Gen. 3";
	otto_sf.size = norsf_info.size_per_chip_b * norsf_info.num_chips;
	otto_sf.page_size = OTTO_SNOF_UNUSED;
	otto_sf.sector_size = norsf_info.sec_sz_b;
	otto_sf.erase_size = norsf_info.sec_sz_b;
	otto_sf.flash_lock = otto_snof_lock;
	otto_sf.flash_unlock = otto_snof_unlock;
	otto_sf.flash_is_locked = otto_snof_is_locked;

        struct mtd_info *mtd = &otto_sf.mtd;
	mtd->name = "SPI-NOR";
	mtd->priv = &otto_sf;
	mtd->type = MTD_NORFLASH;
	mtd->writesize = 1;
	mtd->flags = MTD_CAP_NORFLASH;
	mtd->size = otto_sf.size;
	mtd->_erase = otto_snof_erase;
	mtd->_read = otto_snof_read;
	mtd->_write = otto_snof_write;
	mtd->erasesize = 1;

	return &otto_sf;
}

void spi_flash_free(struct spi_flash *flash) {
#ifdef CONFIG_SPI_FLASH_MTD
	spi_flash_mtd_unregister();
#endif
	return;
}
