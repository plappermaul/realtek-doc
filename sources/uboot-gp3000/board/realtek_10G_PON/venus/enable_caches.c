#include <common.h>
#include <cpu_func.h>
#ifdef CONFIG_ARM64
#include <asm/armv8/mmu.h>


static struct mm_region venus_mem_map[] = {
  {
	/* DDR Support range
	 * 0x0_0000_0000 ! 0x0_7FFF_FFFF
	 */
		.virt = 0x00000000UL,
		.phys = 0x00000000UL,
		.size = 0x80000000,
		.attrs = (PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_INNER_SHARE)
  },
  {
	  /* Device mem mapping 
	   * 0x0_8000_0000 ~ 0x5_FFFF_FFFF 
	   */
		.virt = 0x080000000UL,
		.phys = 0x080000000UL,
		.size = 0x580000000,
                .attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
                         PTE_BLOCK_NON_SHARE |
                         PTE_BLOCK_PXN | PTE_BLOCK_UXN
  },
  {
    /* List terminator */
    0,
  }};

struct mm_region *mem_map = venus_mem_map;
#endif
/* Since D$ can't be enabled due to various improper DMAs from driver,
   cortina env. has to override the official weak function.
*/
void enable_caches(void) {
	icache_enable();
#ifdef CONFIG_CORTINA_SERIAL_NAND
	dcache_enable();
#endif
	return;
}
