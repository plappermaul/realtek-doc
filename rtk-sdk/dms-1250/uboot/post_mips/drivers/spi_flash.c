/*
 * Serial NOR Flash tests
 */

#include <common.h>
#include <spi_flash.h>
#include <soc.h>
#include <pblr.h>
#include <post.h>
#if CONFIG_POST & CONFIG_SYS_POST_FLASH
extern void spi_flash_init(void);
extern int memctl_flash_test(u32_t offset, u32_t size, int testLoops);

int flash_post_test(int flags)
{
	int ret;
	unsigned int flash_size;
	if(parameters.flash_init_result == INI_RES_UNINIT) {
		spi_flash_init();
    }
	flash_size = 1 << para_flash_info.size_per_chip;
	
  printf("flash size = %dMB\n", (unsigned int)(flash_size / (0x100000)));
  if (flags & POST_SLOWTEST) {
		ret = memctl_flash_test(0,flash_size,1);
	}else {			/* POST_MANUAL */
			ret = memctl_flash_test(flash_size-0x10000, 0x10000,1);
	}
	
	return ret;
}
#endif
