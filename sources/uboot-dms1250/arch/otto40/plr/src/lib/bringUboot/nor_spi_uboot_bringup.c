#include <util.h>
#include <dram/memcntlr.h>
#include <lib/lzma/tlzma.h>

extern u32_t next_env;
extern void start_of_next_env, end_of_next_env;

void uboot_bring_up(void) {
	const uimage_header_t *uhdr_info;
	u32_t load_addr, img_size, entry_addr;
	u8_t *src_addr;
	s32_t res;

	if (ISTAT_GET(cal) != MEM_CAL_OK) {
		puts("EE: DRAM is not ready\n");
		return;
	}

	/* Get image info, if any. */
	uhdr_info  = (uimage_header_t *)(&next_env);
	load_addr  = uhdr_info->ih_load;
	img_size   = uhdr_info->ih_size;
	entry_addr = uhdr_info->ih_ep;
	src_addr   = (u8_t *)(uhdr_info+1);

	if ((MAGIC_UBOOT_2011 != uhdr_info->ih_magic) ||
	    (UIH_COMP_NONE == uhdr_info->ih_comp)) {
		if (MAGIC_UBOOT_2011 != uhdr_info->ih_magic) {
			img_size  = (&end_of_next_env - &start_of_next_env);
			load_addr = entry_addr = UBOOT_TEXT_BASE;
			src_addr  = (u8_t *)&next_env;
		}

		printf("II: Relocating U-Boot (%08x <- %08x)... (%d B)...",
		       load_addr, src_addr, img_size);
		inline_memcpy(load_addr, src_addr, img_size);
	} else {
		/* Image header found. */
		if (UIH_COMP_LZMA == uhdr_info->ih_comp) {
			printf("II: Decompressing U-Boot (%p <- %p)... ",
			       load_addr, (uhdr_info+1));

			if ((CPU_GET_STACK_PTR() & 0xD0000000) != 0x80000000) {
				res = lzma_chsp_jump((u8_t *)(uhdr_info+1), (u8_t *)load_addr, &img_size, NEW_STACK_AT_DRAM);
			} else {
				res = lzma_decompress((u8_t *)(uhdr_info+1), (u8_t *)load_addr, &img_size);
			}

			if (res != DECOMPRESS_OK) {
				printf("FAIL(%d)\n", res);
				return;
			} else {
				printf("(%d KB <- %d KB) ", img_size >> 10, uhdr_info->ih_size >> 10);
			}
		} else {
			puts("EE: Unsupported compression type\n");
			return;
		}
	}
	puts("OK\nII: Starting U-boot... \n");

	dcache_wr_inv_all();
	icache_inv_all();
	((fpv_t *)entry_addr)();
	puts("EE: Should not run to here... \n");
	while(1); // should never return
}

REG_INIT_FUNC(uboot_bring_up, 38);
