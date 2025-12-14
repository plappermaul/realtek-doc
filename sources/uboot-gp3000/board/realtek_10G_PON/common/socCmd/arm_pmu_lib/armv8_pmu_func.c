#include <common.h>
#include <command.h>

/*
 * Per-CPU PMCR: config reg
 */
#define ARMV8_PMCR_E		(1 << 0) /* Enable all counters */
#define ARMV8_PMCR_P		(1 << 1) /* Reset all counters */
#define ARMV8_PMCR_C		(1 << 2) /* Cycle counter reset */
#define ARMV8_PMCR_D		(1 << 3) /* CCNT counts every 64th cpu cycle */
#define ARMV8_PMCR_X		(1 << 4) /* Export to ETM */
#define ARMV8_PMCR_DP		(1 << 5) /* Disable CCNT if non-invasive debug*/
#define	ARMV8_PMCR_N_SHIFT	11	 /* Number of counters supported */
#define	ARMV8_PMCR_N_MASK	0x1f
#define	ARMV8_PMCR_MASK		0x3f	 /* Mask for writable bits */


#define PMEVTYPER_NSH		(1 << 27) /* Count events in EL2. */


#define BIT(nr)			(1UL << (nr))
#define	ARMV8_IDX_CYCLE_COUNTER	0
#define REG32(addr)             (*((volatile uint32_t *)(addr)))


#define nop() __asm__ __volatile__("nop");
/*********************************/
#define ST_RETIRED 0x07  //ST_RETIRED Instruction architecturally executed, condition check pass - store:

#define LD_RETIRED 0x06  //LD_RETIRED Instruction architecturally executed, condition check pass - load:
#define CPU_CYCLES 0x11
#define STALL_BACKEND_LD  0xE7
#define STALL_BACKEND_ST  0xE8
#define BUS_ACCESS_RD 0x60
#define BUS_ACCESS_WR 0x61


/**** EL3 ****************/
/* Event counting allowed in Secure state */
static inline void armv8pmu_secure_state_en(void)
{

	u32 value = 0;

	asm volatile("mrs %0, CurrentEL" : "=r" (value));

	if((value & 0xc) == 0xc){

		asm volatile("mrs %0, mdcr_el3" : "=r" (value));
		value|= BIT(17);
		dmb();
		asm volatile("msr mdcr_el3, %0" :: "r" (value));
	}
	return;
}



/***********Count 0********************/

static inline u32 armv8pmu_pmevcntr0_read(void){
	u32 val = 0;
	asm volatile("mrs %0, pmevcntr0_el0" : "=r" (val));
	return val;

}


/* PMEVTYPER<n>_EL0*/
static inline u32 armv8pmu_pmevtyper0_read(void){
	u32 val = 0;
	asm volatile("mrs %0, pmevtyper0_el0" : "=r" (val));
	return val;

}
static inline void armv8pmu_pmevtyper0_write(u32 val){

	asm volatile("msr pmevtyper0_el0, %0" :: "r" (val));
}
/***********Count 1********************/

static inline u32 armv8pmu_pmevcntr1_read(void){
	u32 val = 0;
	asm volatile("mrs %0, pmevcntr1_el0" : "=r" (val));
	return val;

}


/* PMEVTYPER<n>_EL0*/
static inline u32 armv8pmu_pmevtyper1_read(void){
	u32 val = 0;
	asm volatile("mrs %0, pmevtyper1_el0" : "=r" (val));
	return val;

}
static inline void armv8pmu_pmevtyper1_write(u32 val){

	asm volatile("msr pmevtyper1_el0, %0" :: "r" (val));
}
/***********Count 2********************/

static inline u32 armv8pmu_pmevcntr2_read(void){
	u32 val = 0;
	asm volatile("mrs %0, pmevcntr2_el0" : "=r" (val));
	return val;

}


/* PMEVTYPER<n>_EL0*/
static inline u32 armv8pmu_pmevtyper2_read(void){
	u32 val = 0;
	asm volatile("mrs %0, pmevtyper2_el0" : "=r" (val));
	return val;

}
static inline void armv8pmu_pmevtyper2_write(u32 val){

	asm volatile("msr pmevtyper2_el0, %0" :: "r" (val));
}
/***********Count 3********************/
static inline u32 armv8pmu_pmevcntr3_read(void){
	u32 val = 0;
	asm volatile("mrs %0, pmevcntr3_el0" : "=r" (val));
	return val;

}

/* PMEVTYPER<n>_EL0*/
static inline u32 armv8pmu_pmevtyper3_read(void){
	u32 val = 0;
	asm volatile("mrs %0, pmevtyper3_el0" : "=r" (val));
	return val;

}
static inline void armv8pmu_pmevtyper3_write(u32 val){

	asm volatile("msr pmevtyper3_el0, %0" :: "r" (val));
}
/***********Count 4********************/
static inline u32 armv8pmu_pmevcntr4_read(void){
	u32 val = 0;
	asm volatile("mrs %0, pmevcntr4_el0" : "=r" (val));
	return val;

}

/* PMEVTYPER<n>_EL0*/
static inline u32 armv8pmu_pmevtyper4_read(void){
	u32 val = 0;
	asm volatile("mrs %0, pmevtyper4_el0" : "=r" (val));
	return val;

}
static inline void armv8pmu_pmevtyper4_write(u32 val){

	asm volatile("msr pmevtyper4_el0, %0" :: "r" (val));
}


/***********Count 5********************/
static inline u32 armv8pmu_pmevcntr5_read(void){
	u32 val = 0;
	asm volatile("mrs %0, pmevcntr5_el0" : "=r" (val));
	return val;

}

/* PMEVTYPER<n>_EL0*/
static inline u32 armv8pmu_pmevtyper5_read(void){
	u32 val = 0;
	asm volatile("mrs %0, pmevtyper5_el0" : "=r" (val));
	return val;

}
static inline void armv8pmu_pmevtyper5_write(u32 val){

	asm volatile("msr pmevtyper5_el0, %0" :: "r" (val));
}


/***************************************************/
static inline u32 armv8pmu_pmccfiltr_read(void)
{
	u32 val;
	asm volatile("mrs %0, pmccfiltr_el0" : "=r" (val));
	return val;
}

static inline void armv8pmu_pmccfiltr_write(u32 val)
{

	asm volatile("msr pmccfiltr_el0, %0" :: "r" (val));
}


static inline u32 armv8pmu_pmcr_read(void)
{
	u32 val;
	asm volatile("mrs %0, pmcr_el0" : "=r" (val));
	return val;
}

static inline void armv8pmu_pmcr_write(u32 val)
{
	//val &= ARMV8_PMCR_MASK;
	asm volatile("msr pmcr_el0, %0" :: "r" (val));
}


static inline int armv8pmu_enable_counter(int counter)
{
	asm volatile("msr pmcntenset_el0, %0" :: "r" (BIT(counter)));
	return counter;
}

static inline u64 armv8pmu_read_counter(int idx)
{

	u64 value = 0;

	asm volatile("mrs %0, pmccntr_el0" : "=r" (value));


	return value;
}


void rtk_pmu_setup_start(void){
	int rval = 0;
	u32 pmcr = 0;
	rval =  armv8pmu_pmccfiltr_read();

	armv8pmu_pmccfiltr_write(rval | (1<<27) );//EN EL2 cyccle count
//	printf("pmccfiltr=0x%08x\n", armv8pmu_pmccfiltr_read());
#if 0
	armv8pmu_pmevtyper0_write(PMEVTYPER_NSH | ST_RETIRED );
	armv8pmu_pmevtyper1_write(PMEVTYPER_NSH | STALL_BACKEND_ST );

	armv8pmu_pmevtyper2_write(PMEVTYPER_NSH | LD_RETIRED );
	armv8pmu_pmevtyper3_write(PMEVTYPER_NSH | STALL_BACKEND_LD );

	armv8pmu_pmevtyper4_write(PMEVTYPER_NSH | BUS_ACCESS_RD );
	armv8pmu_pmevtyper5_write(PMEVTYPER_NSH | BUS_ACCESS_WR );
#endif
	armv8pmu_pmcr_write(ARMV8_PMCR_P | ARMV8_PMCR_C); //Disable pmu , and reset cycle, all event counts


//	pmcr = armv8pmu_pmcr_read( );
//	printf("pmcr=0x%08x\n", pmcr);
	/* selct count 0~5, and cycle count */
#if 0
	armv8pmu_enable_counter(0);
	armv8pmu_enable_counter(1);
	armv8pmu_enable_counter(2);
	armv8pmu_enable_counter(3);
	armv8pmu_enable_counter(4);
	armv8pmu_enable_counter(5);
#endif
	armv8pmu_enable_counter(31);
//	printf("C0=%d\n", armv8pmu_read_counter(ARMV8_IDX_CYCLE_COUNTER));
//	printf("EVNCT0=%d\n", armv8pmu_pmevcntr0_read());
	armv8pmu_pmcr_write(pmcr| ARMV8_PMCR_E);
}

static inline void pmu_stop_show(void){
	u32 ccnt, st_retired, stall_backend_st , ld_retired, stall_backend_ld, bus_access_rd, bus_access_wr;
	armv8pmu_pmcr_write(0);// disable PMU.
	dmb();
	nop();nop();nop();nop();nop();

	ccnt = armv8pmu_read_counter(ARMV8_IDX_CYCLE_COUNTER);
	st_retired			= armv8pmu_pmevcntr0_read()  ;
	stall_backend_st	= armv8pmu_pmevcntr1_read()  ;
	ld_retired			= armv8pmu_pmevcntr2_read()  ;
	stall_backend_ld	= armv8pmu_pmevcntr3_read()  ;
	bus_access_rd		= armv8pmu_pmevcntr4_read()  ;
	bus_access_wr		=armv8pmu_pmevcntr5_read()   ;

	printf("CCNT             =%u\n", ccnt				);
	printf("ST_RETIRED       =%u\n", st_retired			);
	printf("STALL_BACKEND_ST =%u\n", stall_backend_st	);
	printf("LD_RETIRED       =%u\n", ld_retired			);
	printf("STALL_BACKEND_LD =%u\n", stall_backend_ld	);
	printf("BUS_ACCESS_RD    =%u\n", bus_access_rd		);
	printf("BUS_ACCESS_WR    =%u\n", bus_access_wr		);

}

u64 rtk_pmu_stop_count_get(void){
	u64 ccnt;
	armv8pmu_pmcr_write(0);// disable PMU.

	ccnt = armv8pmu_read_counter(ARMV8_IDX_CYCLE_COUNTER);
	return ccnt;
}
u64 rtk_pmu_count_get(void){
	u64 ccnt;
	ccnt = armv8pmu_read_counter(ARMV8_IDX_CYCLE_COUNTER);
	return ccnt;
}


static unsigned int y = 1U;

unsigned int ldma_rand_r(unsigned int *seedp)
{
	*seedp ^= (*seedp << 13);
	*seedp ^= (*seedp >> 17);
	*seedp ^= (*seedp << 5);

	return *seedp;
}

unsigned int ldma_rand(void)
{
	return ldma_rand_r(&y);
}

void ldma_srand(unsigned int seed)
{
	y = seed;
}

