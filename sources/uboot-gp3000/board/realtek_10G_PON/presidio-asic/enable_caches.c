#include <common.h>
#include <asm/armv8/mmu.h>

/* Since Presidio uses not D$, mem_map is just declared to pass compilation.
	 Content of mem_map is unverified.
*/
static struct mm_region presidio_mem_map[] = {
  {
		.virt = 0x00000000UL,
		.phys = 0x00000000UL,
		.size = 0x0F000000,
		.attrs = (PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_INNER_SHARE)
  },
  {
		.virt = 0x0F000000UL,
		.phys = 0x0F000000UL,
		.size = 0x01000000,
                .attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
                         PTE_BLOCK_NON_SHARE |
                         PTE_BLOCK_PXN | PTE_BLOCK_UXN
  },
  {
		.virt = 0x10000000UL,
		.phys = 0x10000000UL,
		.size = 0x10000000,
		.attrs = (PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_INNER_SHARE)
  },
  {
		.virt = 0x020000000UL,
		.phys = 0x020000000UL,
		.size = 0x800000000,
                .attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
                         PTE_BLOCK_NON_SHARE |
                         PTE_BLOCK_PXN | PTE_BLOCK_UXN
  },
  {
    /* List terminator */
    0,
  }};

struct mm_region *mem_map = presidio_mem_map;

/* Since D$ can't be enabled due to various improper DMAs from driver,
   cortina env. has to override the official weak function.
*/
void enable_caches(void) {
	icache_enable();
	return;
}
