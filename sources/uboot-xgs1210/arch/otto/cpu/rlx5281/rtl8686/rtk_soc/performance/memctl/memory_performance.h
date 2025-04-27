#define TEST_TIMES (100)
#define TEST_ADDR (0xa0000000)
#define CPU_MHZ (400)
#define LX_MHZ (200)
#define DRAM_MHZ (200)
#define PHYSADDR(a)             ((unsigned int)(a) & 0x1fffffff)
#define uint32 unsigned int

#define DMEM0_ADDRESS (0x80a00000)
#define DRAM_ADDRESS  (0x80800000)
#define TO_UNCACHED_ADDR(addr) ((unsigned int)(addr) | (0x20000000))
#define TO_CACHED_ADDR(addr) ((unsigned int)(addr) & (0xDFFFFFFF))
#define TWIN_WORD_SIZE (8)
//#define RLX5281

#ifdef RLX5281
#define IMEM0_SIZE 	(0x8000)
#define IMEM1_SIZE 	(0x2000)
#define DMEM0_SIZE 	(0x1000)
#define DMEM1_SIZE 	(0x1000)
#define DCACHE_SIZE     (0x2000)
#define CACHE_LINE_SIZE (0x20)
#else
#define IMEM0_SIZE 	(0x2000)
#define DMEM0_SIZE 	(0x2000)
#define DCACHE_SIZE     (0x2000)
#define CACHE_LINE_SIZE (0x10)
#endif
