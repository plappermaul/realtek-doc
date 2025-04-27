//#include "mips_cache_ops.h"

//#define L1$_MINV
//#define L2$_EN

#ifndef CONFIG_NFBI_SLAVE
#define CONFIG2_L2B (1 << 12)
void check_L2B(void) {
	unsigned int config2;

        config2 = read_c0_config2();
        
	/* check L2 bypass */
        if (config2 & (1 << 12)) {
                puts("\n\r\n\r[MIPS] Bypass L2 cache! config2: "); puthex(config2); puts("\n\r");
        }
        else {
                puts("\n\r\n\r[MIPS] Enable L2 cache! config2: "); puthex(config2); puts("\n\r");
        }
#ifdef CONFIG_RTL8685SB
        puts("\n\r\n\r[MIPS] Start to run kernel!\n\r\n\r");
#endif
}
#endif

void _1004K_L1_DCache_flush(void){
        unsigned long config1;
        unsigned int lsize;
        unsigned long dcache_size ;
        unsigned long linesz,sets,ways;
        int i;

        config1 = read_c0_config1();

        /* D-Cache */
        lsize = (config1 >> 10) & 7;
        linesz = lsize ? 2 << lsize : 0;
        sets = 32 << (((config1 >> 13) + 1) & 7);
        ways = 1 + ((config1 >> 7) & 7);
        dcache_size = sets *  ways * linesz;

        for(i=CKSEG0;  i < (CKSEG0 + dcache_size); i +=  linesz){
                cache_op(Index_Writeback_Inv_D,i);
				cache_op(Hit_Invalidate_I,i);
        }

        _mips_sync();
}

void _1004K_L1_ICache_flush(void){
	unsigned long config1;
	unsigned int lsize;
	unsigned long icache_size ;
	unsigned long linesz,sets,ways;
	int i;

	config1 = read_c0_config1();

	/* I-Cache */
	lsize = (config1 >> 19) & 7;//4->32B Line Size
	linesz = lsize ? 2 << lsize : 0;//lineSize = 32B
	sets = 32 << (((config1 >> 22) + 1) & 7);
	ways = 1 + ((config1 >> 16) & 7);
	icache_size = sets * ways * linesz;

	for(i=CKSEG0; i < (CKSEG0 + icache_size);  i +=  linesz) {
		cache_op(Index_Invalidate_I,i);
	}

        _mips_sync();

}

#ifdef L1$_MINV
void _1004K_L1_DCache_invalidate(void){
        unsigned long config1;
        unsigned int lsize;
        unsigned long dcache_size ;
        unsigned long linesz,sets,ways;
        int i;
	
        config1 = read_c0_config1();

        /* D-Cache */
        lsize = (config1 >> 10) & 7;
        linesz = lsize ? 2 << lsize : 0;
        sets = 32 << (((config1 >> 13) + 1) & 7);
        ways = 1 + ((config1 >> 7) & 7);
        dcache_size = sets *  ways * linesz;

        for(i=CKSEG0;  i < (CKSEG0 + dcache_size); i +=  linesz){
                cache_op(Hit_Invalidate_D,i);
        }

        _mips_sync();
}

void _1004K_L1_ICache_invalidate(void){
	unsigned long config1;
	unsigned int lsize;
	unsigned long icache_size;
	unsigned long linesz,sets,ways;
	int i;

	config1 = read_c0_config1();

	/* I-Cache */
	lsize = (config1 >> 19) & 7;//4->32B Line Size
	linesz = lsize ? 2 << lsize : 0;//lineSize = 32B
	sets = 32 << (((config1 >> 22) + 1) & 7);
	ways = 1 + ((config1 >> 16) & 7);
	icache_size = sets * ways * linesz;

	for(i=CKSEG0; i < (CKSEG0 + icache_size);  i +=  linesz) {
		cache_op(Hit_Invalidate_I,i);
	}

        _mips_sync();

}
#endif

void flush_l1cache(void) {
    _1004K_L1_DCache_flush();
    _1004K_L1_ICache_flush();
}

#ifdef L2$_EN
void flush_l2cache(void){
    unsigned long cache_size ;
    unsigned long linesz,sets,ways;
    int i;
    unsigned int config2;
    unsigned int tmp;

    config2 = read_c0_config2();

        /* check L2 bypass */
    if (config2 & (1 << 12))
        return;

    /* detect L2-Cache */
    tmp = (config2 >> 4) & 0x0f;
    if (0 < tmp && tmp <= 7)
        linesz = 2 << tmp;
    else
        return;

    tmp = (config2 >> 8) & 0x0f;
    if (0 <= tmp && tmp <= 7)
        sets = 64 << tmp;
    else
        return;

    tmp = (config2 >> 0) & 0x0f;
    if (0 <= tmp && tmp <= 7)
        ways = tmp + 1;
    else
        return;

    cache_size = sets *  ways * linesz;

        //printk("L2cache LineSize=%lu, Sets=%lu, Ways=%lu, CacheSize=%lu\n", linesz, sets, ways, cache_size);

        /* flush L2 cache*/
    for (i = CKSEG0;  i < (CKSEG0 + cache_size); i += linesz) {
      cache_op(Index_Writeback_Inv_SD,i);
    }

    _mips_sync();
}
#endif

void mips_cache_flush(void) {
	flush_l1cache();
#ifdef L2$_EN
	flush_l2cache();
#endif
}

