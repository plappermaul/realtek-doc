#include <common.h>
#include <exports.h>
#include "memory_performance.h"
//#define REG32(reg) (*(volatile unsigned long *)(reg)
#define GICR_BASE                           0xB8003000
/* Timer control registers 
*/
#define TC0DATA                             (0x100 + GICR_BASE)       /* Timer/Counter 0 data */
#define TC1DATA                             (0x104 + GICR_BASE)       /* Timer/Counter 1 data */
#define TC0CNT                              (0x108 + GICR_BASE)       /* Timer/Counter 0 count */
#define TC1CNT                              (0x10C + GICR_BASE)       /* Timer/Counter 1 count */
#define TCCNR                               (0x110 + GICR_BASE)       /* Timer/Counter control */
#define TCIR                                (0x114 + GICR_BASE)       /* Timer/Counter intertupt */
#define CDBR                                (0x118 + GICR_BASE)       /* Clock division base */
#define WDTCNR                              (0x11C + GICR_BASE)       /* Watchdog timer control */


/* Timer/Counter data register field definitions 
*/
#define TCD_OFFSET                          8
/* Timer/Counter control register field defintions 
*/
#define TC0EN                               (1 << 31)       /* Timer/Counter 0 enable */
#define TC0MODE_COUNTER                     0               /* Timer/Counter 0 counter mode */
#define TC0MODE_TIMER                       (1 << 30)       /* Timer/Counter 0 timer mode */
#define TC1EN                               (1 << 29)       /* Timer/Counter 1 enable */
#define TC1MODE_COUNTER                     0               /* Timer/Counter 1 counter mode */
#define TC1MODE_TIMER                       (1 << 28)       /* Timer/Counter 1 timer mode */
/* Timer/Counter interrupt register field definitions 
*/
#define TC0IE                               (1 << 31)       /* Timer/Counter 0 interrupt enable */
#define TC1IE                               (1 << 30)       /* Timer/Counter 1 interrupt enable */
#define TC0IP                               (1 << 29)       /* Timer/Counter 0 interrupt pending */
#define TC1IP                               (1 << 28)       /* Timer/Counter 1 interrupt pending */
/* Clock division base register field definitions 
*/
#define DIVF_OFFSET                         16

#define GET_MSTIME(t)	(t = (uint32)(REG32(TC0CNT)/(CFG_HZ/1000)));
#define GET_CLOCK(t)	(t = (uint32)REG32(TC0CNT));



void printf_CP3_counter_divided(uint32 div_number);
void printf_CP3_counter(void);

extern uint32 cp3_read_byte(uint32 addr, uint32 times);
extern uint32 cp3_write_byte(uint32 addr, uint32 times);
extern uint32 cp3_read_half(uint32 addr, uint32 times);
extern uint32 cp3_write_half(uint32 addr, uint32 times);
extern uint32 cp3_read_word(uint32 addr, uint32 times);
extern uint32 cp3_write_word(uint32 addr, uint32 times);
extern uint32 cp3_read_tword(uint32 addr, uint32 times);
extern uint32 cp3_write_tword(uint32 addr, uint32 times);
extern uint32 read_byte(uint32 addr, uint32 times);
extern uint32 write_byte(uint32 addr, uint32 times);
extern uint32 read_half(uint32 addr, uint32 times);
extern uint32 write_half(uint32 addr, uint32 times);
extern uint32 read_word(uint32 addr, uint32 times);
extern uint32 write_word(uint32 addr, uint32 times);
extern uint32 read_tword(uint32 addr, uint32 times);
extern uint32 write_tword(uint32 addr, uint32 times);
extern uint32 read_line(uint32 addr, uint32 times);
extern uint32 flush_cache_test(uint32 addr, uint32 times);
extern void enable_CP3(void);
extern void enable_IMEM0(void);
extern void enable_IMEM1(void);
extern void disable_IMEM0(void);
extern void disable_IMEM1(void);
extern void IMEM0_refill(void);
extern void clear_CP3_counter(void);
extern void load_imem0Range(uint32 top, uint32 base);
#ifdef CONFIG_RTL8316S
extern void mips_cache_flush_all();
#define _cache_flush_dcache mips_cache_flush_all
#else
extern void rlx5181_cache_flush_dcache(void);
#define _cache_flush_dcache rlx5181_cache_flush_dcache
#endif
extern void read_imem_8bytes(uint32 *addr, uint32 *buff);
extern void imemDmemDMA(uint32 sdram_addr, uint32 mem_addr, uint32 mem_base, \
			uint32 byte_size, uint32 direction, uint32 target, uint32 mode);
extern void DMEM_read_cp0_Flush_no_data(uint32 dmem_addr, \
			uint32 dram_addr, uint32 cnt_mode);
extern void DMEM_read_Cache_Ins_Flush_total_with_size(uint32 dmem_addr, \
			uint32 dram_addr, uint32 cnt_mode, uint32 data_size);
extern void DMEM_read_Cache_Ins_Flush_total_with_size_WA(uint32 dmem_addr, \
			uint32 dram_addr, uint32 cnt_mode, uint32 data_size);
extern void DMEM_read_cp0_Flush_total_with_size(uint32 dmem_addr, \
			uint32 dram_addr,uint32 cnt_mode, uint32 data_size);
extern void DMEM_read_cp0_Flush_480_flush(uint32 dmem_addr, \
			uint32 dram_addr,uint32 cnt_mode);
extern void DMEM_read_cp0_Flush_480_total(uint32 dmem_addr, \
			uint32 dram_addr,uint32 cnt_mode);
extern void DMEM_read_Cache_Ins_Flush_480_read_d_in(uint32 dmem_addr, \
			uint32 dram_addr,uint32 cnt_mode);
extern void DMEM_read_Cache_Ins_Flush_480_write_d_in(uint32 dmem_addr, \
			uint32 dram_addr,uint32 cnt_mode);
extern void DMEM_read_Cache_Ins_Flush_480_cache_ins(uint32 dmem_addr, \
			uint32 dram_addr,uint32 cnt_mode);
extern void DMEM_read_Cache_Ins_Flush_480_total(uint32 dmem_addr, \
			uint32 dram_addr,uint32 cnt_mode);
extern void DMEM_read_Cache_Ins_Flush_160(uint32 dmem_addr, \
			uint32 dram_addr,uint32 cnt_mode);
extern void DMEM_read_TWord_480_UR_B( uint32 dmem_addr, \
			uint32 dram_addr,uint32 cnt_mode);
extern void DMEM_read_TWord_480_UR_I( uint32 dmem_addr, \
			uint32 dram_addr,uint32 cnt_mode);
extern void DMEM_read_TWord_160_UR_B( uint32 dmem_addr, \
			uint32 dram_addr,uint32 cnt_mode);
extern void DMEM_read_TWord_160_UR_I( uint32 dmem_addr, \
			uint32 dram_addr, uint32 cnt_mode);
extern void DMEM_read_TWord_LP( uint32 dmem_addr, \
			uint32 dram_addr, uint32 cnt_mode, uint32 numOfLoops);
extern void Full_Cache_Read_DMEM_TWord_write_LP( uint32 dmem_addr, \
			uint32 dram_addr, uint32 cnt_mode, uint32 test_size);
extern void Full_Cache_Read_DMEM_TWord_write_LP_INV( uint32 dmem_addr, \
			uint32 dram_addr, uint32 cnt_mode, uint32 test_size);
extern void Full_Cache_Read_DMEM_TWord_write_LP_RW( uint32 dmem_addr, \
			uint32 dram_addr, uint32 cnt_mode, uint32 test_size);
extern void Empty_Cache_Read_DMEM_TWord_write_LP( uint32 dmem_addr, \
			uint32 dram_addr, uint32 cnt_mode, uint32 test_size);
extern void Empty_Cache_Read_DMEM_TWord_write_LP_INV( uint32 dmem_addr, \
			uint32 dram_addr, uint32 cnt_mode, uint32 test_size);
extern void Empty_Cache_Read_DMEM_TWord_write_LP_RW( uint32 dmem_addr, \
			uint32 dram_addr, uint32 cnt_mode, uint32 test_size);


#ifdef RLX5281
/* event string coresponding ot even_num[] */
char* event_str[][4] = {
				{"Cycles",                  "INST_FETCHES", \
		           	 "LOAD_STORE_INS",           "DCACHE_MISS"},\
				{"INST_FEATCH",             "ICACHE_MISS",  \
            		 	 "ICACHE_MISS_CYCLE",        "STORE_INST"}, \
				{"LOAD_INST",               "LORDSTROE_INST",\
				 "COMPLETED_INST",           "CYCLES"},\
				{"IDCACHE_MISSE_SOFT",      "DCACHE_MISSE", \           
				 "DCACHE_MISS_CYCLE",        "L2_CACHE_HIT"},\
				{"L2_CACHE_HIT_CYCLE",      "L2_CACHE_MISS", \
				 "L2_CACHE_MISS_CYCLE",      "BRANCEH_PREDICTION"},
				{"BRANCEH_PREDICTION_MISS", "BTB_MISS",\
				 "BTB_TARGET_MISS",          "CBP_MISS"},
				{"RS_POP",                  "RS_MISS",\
				 "ICACHE_WAY_PREDICT_HIT",   "IBUF_FULL_CYCLE"},
				{"IBUF_EMPTY_CYCLE",        "I_SIDE_MMU_EXCEPTION",\
				 "D_SIDE_MMU_EXCEPTION",     "CYCLES"         },
				{"WBUF_MERGE",              "WBUF_UC_MERGE",\
				 "WBUF_XFORM",               "ZERO_INST_COMMIT_CYCLE"},\
				{"PIPE_A_INST_COMMIT_CYCLE", "PIPE_B_INST_COMMIT_CYCLE",\
				 "DUAL_INST_COMMIT_CYCLE",  "OCP_BUS_READS"},\
				{"OCP_BUS_WRITES",           "OCP_BUS_UC_READ",\
				 "OCP_BUS_UC_WRITE",        "OCP_BUS_DATA_READ"}\
		     };
uint32 event_num[] = {
				0x0a060108, 0x04030201, 0x08070605, 0x0C0B0A09,
				0x100F0E0D, 0x14131211, 0x18171615, 0x081B1A19,
				0x201F1E1D, 0x24232221, 0x28272625
		     };
#else
char* event_str[][4] = {
				{"Cycles",                  "INST_FETCHES", \
		           	 "LOAD_STORE_INS",           "DCACHE_MISS"}\
			};
uint32 event_num[] = { 
				0x1a161110 
		     };

#endif

inline static void timer_init(void)
{
        REG32(TCCNR) = 0;
        //REG32(CDBR) = 256 << DIVF_OFFSET;
        REG32(CDBR) = 8 << DIVF_OFFSET;
        REG32(TC0DATA) = 0xFFFFFF << 8;
        //REG32(TCIR) = TC0IE;
        //REG32(TCCNR) = TC0EN | TC0MODE_TIMER;
}

int memory_per(int argc, void* arg[])
{
	uint32 tclock;
	uint32 run_times;
	uint32 top, base;
	uint32 imem_read_buff[2];
	uint32 *pMem;
	uint32 dot_num;
	uint32 test_size;

	uint32 cnt_mode;
	uint32 dmem_addr;
	uint32 dram_addr;
	uint32 numOfLoops;
	uint32 i;
	uint32 data_size;

	run_times = TEST_TIMES;
	
	//*((volatile uint32 *)(0xb8000010)) = *((volatile uint32 *)(0xb8000010)) | 0x1180400;
	enable_CP3();
	//base =  PHYSADDR(read_byte);
	//base = base & (0xFFFF0000);
	//top = base + 0x1fff;

	_cache_flush_dcache();
//	disable_IMEM0();
	base = 0x1feb000;
	top  = 0x1febfff;
	
	pMem = (uint32 *)write_byte;
	printf("write_byte = 0x%08x, top = 0x%08x, base = 0x%08x\n",write_byte ,top+0x1000, base);
//	load_imem0Range(top, base);
//	load_imem1Range(top+0x1000, base+0x1000);
//	IMEM0_refill();
//	IMEM1_refill();

//	printf("ICVL/ICVR shall be 0xA769EB37/0xB1DA5A0E\n");
	//imemDmemDMA(0x81f00000, 0x81f00000 ,0x81f00000, 0x2000, 1, 0, 0 );
	//enable_IMEM0();
	//disable_IMEM0();
//	read_imem_8bytes(pMem , &imem_read_buff[0]);
//	printf("0x%08x 0x%08x\n", imem_read_buff[0], imem_read_buff[1]);


	/* CP3 Read byte */
	clear_CP3_counter();
	tclock = cp3_read_byte(TEST_ADDR, run_times); 
	dot_num = ((tclock*100)/run_times)%100;
	printf("%d \tcp3 read byte:\t %d.%02d clock per read\n", \ 
			tclock, (tclock)/run_times, dot_num);

	/* CP3 write byte */
	clear_CP3_counter();
	tclock = cp3_write_byte(TEST_ADDR, run_times); 
	dot_num = ((tclock*100)/run_times)%100;
	printf("%d \tcp3 write byte:\t %d.%02d clock per write\n", \ 
			tclock, (tclock)/run_times, dot_num);

	/* CP3 Read half */
	clear_CP3_counter();
	tclock = cp3_read_half(TEST_ADDR, run_times); 
	dot_num = ((tclock*100)/run_times)%100;
	printf("%d \tcp3 read half:\t %d.%02d clock per half\n", \ 
			tclock, (tclock)/run_times, dot_num);

	/* CP3 write half */
	clear_CP3_counter();
	tclock = cp3_write_half(TEST_ADDR, run_times);
	dot_num = ((tclock*100)/run_times)%100;
	printf("%d \tcp3 write half:\t %d.%02d clock per half\n", \ 
			tclock, (tclock)/run_times, dot_num);

	/* CP3 Read word */
	clear_CP3_counter();
	tclock = cp3_read_word(TEST_ADDR, run_times); 
	dot_num = ((tclock*100)/run_times)%100;
	printf("%d \tcp3 read word:\t %d.%02d clock per word\n", \ 
			tclock, (tclock)/run_times, dot_num);

	/* CP3 write word */
	clear_CP3_counter();
	tclock = cp3_write_word(TEST_ADDR, run_times); 
	dot_num = ((tclock*100)/run_times)%100;
	printf("%d \tcp3 write word:\t %d.%02d clock per word\n", \ 
			tclock, (tclock)/run_times, dot_num);

	/* CP3 Read tword */
	clear_CP3_counter();
	tclock = cp3_read_tword(TEST_ADDR, run_times); 
	dot_num = ((tclock*100)/run_times)%100;
	printf("%d \tcp3 read tword:\t %d.%02d clock per tword\n", \ 
			tclock, (tclock)/run_times, dot_num);

	/* CP3 write word */
	clear_CP3_counter();
	tclock = cp3_write_tword(TEST_ADDR, run_times);
	dot_num = ((tclock*100)/run_times)%100;
	printf("%d \tcp3 write tword:\t %d.%02d clock per tword\n", \ 
			tclock, (tclock)/run_times, dot_num);

	/* Read cache line */
	clear_CP3_counter();
	tclock = cp3_read_line(TEST_ADDR, 0x2000);
	dot_num = ((tclock*100)/32)%100;
	printf("%d \tcp3 line read:\t %d.%02d clock per line\n", \ 
			tclock, (tclock)/32, dot_num);

	test_size = 0x2000;
	/* Write cache line*/
	clear_CP3_counter();
	tclock = cp3_flush_cache_test(TEST_ADDR, test_size);
	dot_num = ((tclock*100)/(test_size/32))%100;
	printf("%d \tcp3 line write:\t %d.%02d clock per line\n", \ 
			tclock, (tclock)/(test_size/32), dot_num);

#if 0
	/* Write byte */
	timer_init();
	tclock = write_byte(TEST_ADDR, run_times);
	tclock = tclock >> 5;
	dot_num = ((tclock*(CPU_MHZ/LX_MHZ)*100)/run_times)%100;
	printf("%d \twrite byte:\t %d.%02d clock per write\n", \
			tclock*(CPU_MHZ/LX_MHZ), (tclock*(CPU_MHZ/LX_MHZ))/run_times, dot_num);
	/* Read byte */
	timer_init();
	tclock = read_byte(TEST_ADDR, run_times); /* 100 times lb instructions */
	tclock = tclock >> 5;
	dot_num = ((tclock*(CPU_MHZ/LX_MHZ)*100)/run_times)%100;
	printf("%d \tread byte:\t %d.%02d clock per read\n", \ 
			tclock*(CPU_MHZ/LX_MHZ), (tclock*(CPU_MHZ/LX_MHZ))/run_times, dot_num);

	/* Write half-word */
	timer_init();
	tclock = write_half(TEST_ADDR, run_times);
	tclock = tclock >> 5;
	dot_num = ((tclock*(CPU_MHZ/LX_MHZ)*100)/run_times)%100;
	printf("%d \twrite half:\t %d.%02d clock per write half\n", \
			tclock*(CPU_MHZ/LX_MHZ), (tclock*(CPU_MHZ/LX_MHZ))/run_times, dot_num);

	/* Read half-word */
	timer_init();
	tclock = read_half(TEST_ADDR, run_times);
	tclock = tclock >> 5;
	dot_num = ((tclock*(CPU_MHZ/LX_MHZ)*100)/run_times)%100;
	printf("%d \tread half:\t %d.%02d clock per read half\n", \
			tclock*(CPU_MHZ/LX_MHZ), (tclock*(CPU_MHZ/LX_MHZ))/run_times, dot_num);

	/* Write word */
	timer_init();
	tclock = write_word(TEST_ADDR, run_times);
	tclock = tclock >> 5;
	dot_num = ((tclock*(CPU_MHZ/LX_MHZ)*100)/run_times)%100;
	printf("%d \twrite word:\t %d.%02d clock per write word\n",\
			tclock*(CPU_MHZ/LX_MHZ), (tclock*(CPU_MHZ/LX_MHZ))/run_times, dot_num);

	/* Read word */
	timer_init();
	tclock = read_word(TEST_ADDR, run_times/10);
	tclock = tclock >> 5;
	dot_num = ((tclock*(CPU_MHZ/LX_MHZ)*100)/run_times)%100;
	printf("%d \tread word:\t %d.%02d clock per read word\n", \
			tclock*(CPU_MHZ/LX_MHZ), (tclock*(CPU_MHZ/LX_MHZ))/run_times, dot_num);

	/* Write twin-word */
	timer_init();
	tclock = write_tword(TEST_ADDR, run_times/10);
	tclock = tclock >> 5;
	dot_num = ((tclock*(CPU_MHZ/LX_MHZ)*100)/run_times)%100;
	printf("%d \twrite tword:\t %d.%02d clock per write twin-word\n", \
			tclock*(CPU_MHZ/LX_MHZ), (tclock*(CPU_MHZ/LX_MHZ))/run_times, dot_num);

	/* Read twin-word */
	timer_init();
	tclock = read_tword(TEST_ADDR, run_times/10);
	tclock = tclock >> 5;
	dot_num = ((tclock*(CPU_MHZ/LX_MHZ)*100)/run_times)%100;
	printf("%d \t8-aligned read tword:\t %d.%02d clock per read twin-word\n", \
			tclock*(CPU_MHZ/LX_MHZ), (tclock*(CPU_MHZ/LX_MHZ))/run_times, dot_num);
#endif
#if 0
	/* Read 4byte excluded 8byte alinged twin-word: illegal alignement(only 8bytes alignement supported) */
	timer_init();
	tclock = read_tword(TEST_ADDR+4, run_times/10);
	tclock = tclock >> 5;
	dot_num = ((tclock*(CPU_MHZ/LX_MHZ)*100)/run_times)%100;
	printf("%d \t4-aligned read tword:\t %d.%02d clock per read twin-word\n", \
			tclock*(CPU_MHZ/LX_MHZ), (tclock*(CPU_MHZ/LX_MHZ))/run_times, dot_num);
	/* Read cache line */
	timer_init();
	tclock = read_line(TEST_ADDR, 0x2000);
	tclock = tclock >> 5;
	dot_num = ((tclock*(CPU_MHZ/LX_MHZ)*100)/(32))%100;
	printf("%d \tread line:\t %d.%02d clock per read line\n", \
			tclock*(CPU_MHZ/LX_MHZ), (tclock*(CPU_MHZ/LX_MHZ))/(32), dot_num);

	//test_size = CACHE_SIZE;
	test_size = 0x2000;
	/* Write cache line*/
	timer_init();
	tclock = flush_cache_test(TEST_ADDR, test_size);
	tclock = tclock >> 5;
	dot_num = ((tclock*(CPU_MHZ/LX_MHZ)*100)/(test_size/CACHE_LINE_SIZE))%100;
	printf("%d \twrite line:\t %d.%02d clock per write line\n", \
			tclock*(CPU_MHZ/LX_MHZ), (tclock*(CPU_MHZ/LX_MHZ))/(test_size/CACHE_LINE_SIZE), dot_num);
#endif


//#ifndef CONFIG_RTL8316S
#if 0
        base =  PHYSADDR(DMEM_read_TWord_160_UR_I);
        base = base & (0xFFFF0000);
        top = base + IMEM0_SIZE - 1;
	load_imem0Range(top, base);
	IMEM0_refill();

	printf("IMEM0: base 0x%08x, top 0x%08x\n", base, top);
	
	dmem_addr = DMEM0_ADDRESS;
	dram_addr  = DRAM_ADDRESS;

	base = PHYSADDR(DMEM0_ADDRESS);
	top  = base + DMEM0_SIZE-1;
	load_dmem0Range(top, base);
	load_dmem1Range(top+DMEM0_SIZE, base+DMEM0_SIZE);
	printf("DMEM0: base 0x%08x, top 0x%08x\n", base, top);
	enable_DMEM0();
	enable_DMEM1();
	//for(i=0; i < sizeof(event_num)/4; i++){
	for(i=0; i < 1; i++){
		cnt_mode = event_num[i];
		printf("                               %s           %s    %s  %s\n", \
				event_str[i][0], event_str[i][1], event_str[i][2], event_str[i][3]);

		clear_CP3_counter();
		printf("Cache_Ins_Flush_480_read_D_in: ");
		DMEM_read_Cache_Ins_Flush_480_read_d_in(dmem_addr, TO_CACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("Cache_Ins_Flush_480_write_d_in:");
		DMEM_read_Cache_Ins_Flush_480_write_d_in(dmem_addr, TO_CACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("Cache_Ins_Flush_480_cache_ins: ");
		DMEM_read_Cache_Ins_Flush_480_cache_ins(dmem_addr, TO_CACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("Cache_Ins_Flush_480_total:     ");
		DMEM_read_Cache_Ins_Flush_480_total(dmem_addr, TO_CACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("DMEM_read_cp0_Flush_480_flush: ");
		DMEM_read_cp0_Flush_480_flush(dmem_addr, TO_CACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("DMEM_read_cp0_Flush_480_total: ");
		DMEM_read_cp0_Flush_480_total(dmem_addr, TO_CACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("Cache_Ins_Flush_160_read_D_in: ");
		DMEM_read_Cache_Ins_Flush_160_read_d_in(dmem_addr, TO_CACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("Cache_Ins_Flush_160_write_d_in:");
		DMEM_read_Cache_Ins_Flush_160_write_d_in(dmem_addr, TO_CACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("Cache_Ins_Flush_160_cache_ins: ");
		DMEM_read_Cache_Ins_Flush_160_cache_ins(dmem_addr, TO_CACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("Cache_Ins_Flush_160_total:     ");
		DMEM_read_Cache_Ins_Flush_160_total(dmem_addr, TO_CACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		numOfLoops = 60;
		clear_CP3_counter();
		printf("DMEM_read_TWord_LP:60          ");
		DMEM_read_TWord_LP(dmem_addr, TO_UNCACHED_ADDR(dram_addr), cnt_mode,  numOfLoops);
		printf_CP3_counter();

		numOfLoops = 20;
		clear_CP3_counter();
		printf("DMEM_read_TWord_LP:20          ");
		DMEM_read_TWord_LP(dmem_addr, TO_UNCACHED_ADDR(dram_addr), cnt_mode,  numOfLoops);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("DMEM_read_TWord_480_UR_B:      ");
		DMEM_read_TWord_480_UR_B(dmem_addr, TO_UNCACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("DMEM_read_TWord_480_UR_I:      ");
		DMEM_read_TWord_480_UR_I(dmem_addr, TO_UNCACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("DMEM_read_TWord_160_UR_B:      ");
		DMEM_read_TWord_160_UR_B(dmem_addr, TO_UNCACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();

		clear_CP3_counter();
		printf("DMEM_read_TWord_160_UR_I:      ");
		DMEM_read_TWord_160_UR_I(dmem_addr, TO_UNCACHED_ADDR(dram_addr), cnt_mode);
		printf_CP3_counter();
	}

        cnt_mode = event_num[0];
	printf("                         CYCLES  CYCLES_PER_LINE\n");
	clear_CP3_counter();
	printf("DMEM_read_cp0_Flush_0x0:    ");
	DMEM_read_cp0_Flush_no_data(dmem_addr, TO_CACHED_ADDR(dram_addr), cnt_mode);
	printf_CP3_counter_divided(1);
	printf("\n");
	printf("\t");
	printf("DMEM_read_TWord_LP\t\t");
	printf("DMEM_read_cp0_Flush\t\t");
	printf("DMEM_read_Cache_Ins_Flush\t\t");
//	printf("DMEM_read_Cache_Ins_Flush_WA");
	printf("\n");
	for(data_size = CACHE_LINE_SIZE ; data_size <= 0x2000 ; data_size = data_size+CACHE_LINE_SIZE)
	{
		printf("%06d\t", data_size);
		clear_CP3_counter();
		DMEM_read_TWord_LP(dmem_addr, TO_UNCACHED_ADDR(dram_addr), \
			cnt_mode,  (data_size/TWIN_WORD_SIZE));
		printf_CP3_counter_divided(data_size/CACHE_LINE_SIZE);

		clear_CP3_counter();
		DMEM_read_cp0_Flush_total_with_size(dmem_addr, \
			TO_CACHED_ADDR(dram_addr), cnt_mode, data_size);
		printf_CP3_counter_divided(data_size/CACHE_LINE_SIZE);

		clear_CP3_counter();
		DMEM_read_Cache_Ins_Flush_total_with_size(dmem_addr, \
			TO_CACHED_ADDR(dram_addr), cnt_mode, data_size);
		printf_CP3_counter_divided(data_size/CACHE_LINE_SIZE);

//		clear_CP3_counter();
//		DMEM_read_Cache_Ins_Flush_total_with_size_WA(dmem_addr, \
//			TO_CACHED_ADDR(dram_addr), cnt_mode, data_size);
//		printf_CP3_counter_divided(data_size/CACHE_LINE_SIZE);

		printf("\n");
	}
#endif
#if 0
		printf("\t");
		printf("UnCached Tword Read to DMEM\t\t");
		printf("Cache Tword Read to DMEM With Cache Conflict\t\t");
		printf("Cache Tword Read to DMEM Without Cache Conflict\t\t");
	//	printf("TWord_Read_DMEM_TWord_write_LP\t");
	//	printf("Full_Cache_Read_DMEM_TWord_write_LP_INV: ");
	//	printf("Full_Cache_Read_DMEM_TWord_write_LP_RW :  ");
	//	printf("Full_Cache_Read_DMEM_TWord_write_LP\t");
	//	printf("Empty_Cache_Read_DMEM_TWord_write_LP_INV:");
	//	printf("Empty_Cache_Read_DMEM_TWord_write_LP_RW :");
	//	printf("Empty_Cache_Read_DMEM_TWord_write_LP\t");
	printf("\n");
	for(data_size = CACHE_LINE_SIZE ; data_size <= 0x2000 ; data_size = data_size+CACHE_LINE_SIZE)
	//for(data_size = CACHE_LINE_SIZE ; data_size <= CACHE_LINE_SIZE ; data_size = data_size+CACHE_LINE_SIZE)
	{
		printf("%06d\t", data_size);
		//for(i=1; i<sizeof(event_num)/4; i++){
		//cnt_mode = event_num[i];
		//printf("                               %s           %s    %s  %s\n", \
				event_str[i][0], event_str[i][1], event_str[i][2], event_str[i][3]);
		clear_CP3_counter();
		//printf("%d ", data_size);
		TWord_Read_DMEM_TWord_write_LP(dmem_addr, \
			TO_UNCACHED_ADDR(dram_addr), cnt_mode, data_size);
		//printf_CP3_counter();
		printf_CP3_counter_divided(data_size/CACHE_LINE_SIZE);
		//}
#if 0
		clear_CP3_counter();
		//printf("%d ", data_size);
		Full_Cache_Read_DMEM_TWord_write_LP_INV(dmem_addr, \
			TO_CACHED_ADDR(dram_addr), cnt_mode, data_size);
		printf_CP3_counter_divided(data_size/CACHE_LINE_SIZE);

		clear_CP3_counter();
		//printf("%d ", data_size);
		Full_Cache_Read_DMEM_TWord_write_LP_RW(dmem_addr, \
			TO_CACHED_ADDR(dram_addr), cnt_mode, data_size);
		printf_CP3_counter_divided(data_size/CACHE_LINE_SIZE);

#endif
		clear_CP3_counter();
		//printf("%d ", data_size);
		Full_Cache_Read_DMEM_TWord_write_LP(dmem_addr, \
			TO_CACHED_ADDR(dram_addr), cnt_mode, data_size);
		printf_CP3_counter_divided(data_size/CACHE_LINE_SIZE);

#if 0
		clear_CP3_counter();
		//printf("%d ", data_size);
		Empty_Cache_Read_DMEM_TWord_write_LP_INV(dmem_addr, \
			TO_CACHED_ADDR(dram_addr), cnt_mode, data_size);
		printf_CP3_counter_divided(data_size/CACHE_LINE_SIZE);
		clear_CP3_counter();
		//printf("%d ", data_size);
		Empty_Cache_Read_DMEM_TWord_write_LP_RW(dmem_addr, \
			TO_CACHED_ADDR(dram_addr), cnt_mode, data_size);
		printf_CP3_counter_divided(data_size/CACHE_LINE_SIZE);
#endif
		clear_CP3_counter();
		//printf("%d ", data_size);
		Empty_Cache_Read_DMEM_TWord_write_LP(dmem_addr, \
			TO_CACHED_ADDR(dram_addr), cnt_mode, data_size);
		printf_CP3_counter_divided(data_size/CACHE_LINE_SIZE);
		printf("\n");
	}
#endif


//	disable_IMEM0();
//	disable_DMEM0();
//	disable_DMEM1();

	return 0;
}

void printf_CP3_counter_divided(uint32 div_number)
{
	volatile uint32 delay;
	uint32 couter[8];

        save_counter(&couter[0], &couter[2], &couter[4], &couter[6]);
        printf("%06d\t", couter[1]);
        printf("%06d\t", couter[1]/div_number);
	delay = 250000;
	while(delay--);
	
}

void printf_CP3_counter(void)
{
	uint32 couter[8];
        save_counter(&couter[0], &couter[2], &couter[4], &couter[6]);
#if 0
        printf("Cycles:            0x%08x%08x\n", couter[0], couter[1]);
        printf("LORDSTROE_INST:    0x%08x%08x\n", couter[2], couter[3]);
        printf("DCACHE_MISS:       0x%08x%08x\n", couter[4], couter[5]);
        printf("DCACHE_MISS_CYCLE: 0x%08x%08x\n", couter[6], couter[7]);
#else
        printf("%08d\t\t", couter[1]);
        printf("%08d\t", couter[3]);
        printf("%08d\t", couter[5]);
        printf("%08d\t\n", couter[7]);
#endif

}

