/*
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Misc functions
 */
#include <common.h>
#include <command.h>
#include <cpu_func.h>
#include <asm/io.h>
#include <linux/delay.h>
/************************/
extern void rtk_pmu_setup_start(void);
extern u64 rtk_pmu_stop_count_get(void);
extern u64 rtk_pmu_count_get(void);
/****************************/
#define RET_OK 0

enum msg_lvl_print{
	meg_lvl_datacmp=3,
	meg_lvl_proc=4,
	meg_lvl_dbg=7,
	meg_lvl_all=9,
};
static int meg_lvl = meg_lvl_proc;

#define ERR_MSG(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#define STD_MSG(fmt, ...)  { if(meg_lvl>= meg_lvl_proc) printf(fmt, ##__VA_ARGS__); }
#define DATA_MSG(fmt, ...)  { if(meg_lvl>= meg_lvl_datacmp) printf(fmt, ##__VA_ARGS__); }
#define DBG_MSG(fmt, ...)  { if(meg_lvl>= meg_lvl_dbg) printf(fmt, ##__VA_ARGS__); }

/* GIC */
#define GIC_SPI_PENDING 0x4f8000284
#define GIC_SPI_PER_PERIPHERAL_INTERRUPT_0	0x2
#define GIC_SPI_MAX	128
/* REG*/
#if 0
#define PER_TMR_LD1		0xf4329050
#define PER_TMR_CTRL1	(PER_TMR_LD1 + 0x4)
#define PER_TMR_CNT1	(PER_TMR_LD1 + 0x8)
#define PER_TMR_IE1_0	(PER_TMR_LD1 + 0xc)
#define PER_TMR_IE1_1	(PER_TMR_LD1 + 0x10)
#define PER_TMR_IE1_1	(PER_TMR_LD1 + 0x10)
#define PER_TMR_INT1_0	(PER_TMR_LD1 + 0x14)
#define PER_TMR_INT1_1	(PER_TMR_LD1 + 0x18)
#define PER_TMR_STAT1	(PER_TMR_LD1 + 0x1c)
#endif
#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
#define CPU_CLK		  50000000 /*ASIC  50 MHZ */
#define PER_CLK 	  25000000 /*FPGA 25MHZ*/
#else
#define CPU_CLK		1000000000 /* 1000 MHZ */
#define PER_CLK 	 125000000 /*  125 MHZ*/
#endif

#define TCLK_TO_NS  	(1000000000/PER_CLK)
#define CCNT_TO_NS		(1000000000/CPU_CLK)

#define PER_TMR_REG_OFF	(0x20)
#define PTIMER_MAX 4
#define ROLL_MODE   0x0
#define LOAD_MODE   0x1

#define CLKSEL_DIRECT	 0x0
#define CLKSEL_DV64 	0x1
#define CLKSEL_DV1024 	0x10
#define CLKSEL_DV4096 	0x11

/* DATA */
#define TEST_LOAD_VAL	(PER_CLK)  /* 1s */

static void per_write(int idx, u32 val, u64 addr){
    writel(val, addr + idx*PER_TMR_REG_OFF);
}

static u32 per_read(int idx, u64 addr){
    return readl( addr + idx*PER_TMR_REG_OFF);
}


static u64 per_timer_diff(u64 ccnt, u64 tcnt, int cdiv){
	u64 diff_ns, diff;
	u64 ccnt_ns , tcnt_ns;
	u32 div_val;
	switch (cdiv){
	case 0:
		div_val=1;
		break;
	case 1:
		div_val=64;
		break;
	case 2:
		div_val=1024;
		break;
	case 3:
		div_val=4096;
		break;
	default:
		div_val=1;
		break;
	}


	ccnt_ns = CCNT_TO_NS*ccnt;
	tcnt_ns = TCLK_TO_NS*tcnt*div_val;
	diff  = ccnt_ns - tcnt_ns;
	printf("CPU \t Time=%llu(ns), %llu(ms)\n", ccnt_ns, ccnt_ns/1000000);
	printf("PTimer \t Time=%llu(ns), %llu(ms)\n", tcnt_ns, tcnt_ns/1000000);
	if(tcnt_ns>ccnt_ns){
		diff_ns  =  (tcnt_ns - ccnt_ns);
		diff = diff_ns*1000000/ccnt_ns;
	}else{
		diff_ns =  ccnt_ns - tcnt_ns;
		diff = diff_ns*1000000/ccnt_ns;

	}
	printf("CCNT and PTIMER deviation: %llu(ns) about %llu/100000\n",diff_ns,  diff);
	return 0;
}

static u64 gic_pending_chk(u32 hw_irq){
	u32 irq_pending;
  	u64 pending_addr = 0;

	if(hw_irq>=GIC_SPI_MAX){
		printf("Error: hw_irq(%u) >=%u\n", hw_irq, GIC_SPI_MAX);
		return 0;
	}

	pending_addr =  (GIC_SPI_PENDING + (hw_irq/32)*4);
	irq_pending = readl(pending_addr);
         //printf("[%s %d] pending_addr=0x%llx, value=0x%x, bit(0x%x)\n", __func__, __LINE__, 	pending_addr , irq_pending , ( 1<<(hw_irq%32)));
	if(irq_pending & 1<<(hw_irq%32)){
		return 1;
	}else{
		return 0;
	}
}
static u64 per_timer_int_polling(int id){
#ifdef CONFIG_TARGET_RTK_ELNATH
    PER_PERIPHERAL_INTERRUPT_00_t    per_int0;
    PER_PERIPHERAL_INTERRUPT_10_t    per_int1;
#else
    PER_PERIPHERAL_INTERRUPT_0_t    per_int0;
    PER_PERIPHERAL_INTERRUPT_1_t    per_int1;
#endif
    PER_TMR_INT1_0_t    reg_tmr0_pending;
    PER_TMR_INT1_1_t    reg_tmr1_pending;
    u64 timeout = 100000000000;
	u64 ccnt = 0;
    while(timeout){
        reg_tmr0_pending.wrd = per_read(id, PER_TMR_INT1_0);

	    if(reg_tmr0_pending.bf.tmr1i){
			ccnt = rtk_pmu_stop_count_get();
		    per_write(id,0x0,PER_TMR_CTRL1 );//disable timer
			if(gic_pending_chk(GIC_SPI_PER_PERIPHERAL_INTERRUPT_0)){
					printf("Peri_timer(%d) GIC INT:  PASS\n", id);
			}else{
					printf("Peri_timer(%d) GIC INT:  FAILD\n", id);
			}

	        reg_tmr1_pending.wrd = per_read(id, PER_TMR_INT1_1);
		    if(reg_tmr1_pending.bf.tmr1i == 0){
				printf("Peri_timer int 1:  FAIL. (It should be pending)\n");
			}
#ifdef CONFIG_TARGET_RTK_ELNATH
			per_int0.wrd = readl(PER_PERIPHERAL_INTERRUPT_00);
			per_int1.wrd = readl(PER_PERIPHERAL_INTERRUPT_10);
#else
			per_int0.wrd = readl(PER_PERIPHERAL_INTERRUPT_0);
			per_int1.wrd = readl(PER_PERIPHERAL_INTERRUPT_1);
#endif
			if( (per_int0.wrd & (0x4<<id)) && (per_int1.wrd & (0x4<<id)) ) {
				//printf("CCNT : 0x%llx\n", ccnt);
			}else{
				printf("Peri int 0/1:  FAIL\n");
			}

			per_write(id, 0x1, PER_TMR_INT1_0);
			per_write(id, 0x1, PER_TMR_INT1_1);

		//	printf("CCNT : 0x%llx\n", ccnt);

			break;
		}
        timeout--;
    }
	if(timeout==0){
		printf("Error: Timer Interrupt is not pending! PER_TMR_INT%d_0=0x%08x, PER_TMR_INT%d_1=0x%08x\n ", id, reg_tmr0_pending.wrd,id,  reg_tmr1_pending.wrd);
	}
	return ccnt;
}


static u64 per_timer_int_polling_not_stop(int id){
    PER_TMR_INT1_0_t    reg_tmr0_pending;
    u64 timeout = 100000000000;
	u64 ccnt = 0;
    while(timeout){
        reg_tmr0_pending.wrd = per_read(id, PER_TMR_INT1_0);

	    if(reg_tmr0_pending.bf.tmr1i){
			ccnt = rtk_pmu_count_get();

			per_write(id, 0x1, PER_TMR_INT1_0);
			per_write(id, 0x1, PER_TMR_INT1_1);

			break;
		}
        timeout--;
    }
	if(timeout==0){
		printf("Error: Timer Interrupt is not pending! PER_TMR_INT%d_0=0x%08x\n ", id, reg_tmr0_pending.wrd);
	}
	return ccnt;
}



#define ROLLOVER_VALUE	0xffffffff

/*********************
 LOAD_MODE
*****************************/

static int per_timer_test_ldmode_with_clksel(int id, u32 clksel_v){
	u64 c0, c1,c2;
    PER_TMR_CTRL1_t reg_ctrl;
	PER_TMR_LD1_t reg_ld1;
	u32 per_tmr_cnt_val;
	u32 div_val = 0;
	switch (clksel_v){
	case 0:
		div_val=1;
		break;
	case 1:
		div_val=64;
		break;
	case 2:
		div_val=1024;
		break;
	case 3:
		div_val=4096;
		break;
	default:
		div_val=1;
		break;
	}

    per_write(id, 0,PER_TMR_CTRL1 );

	per_tmr_cnt_val =per_read(id, PER_TMR_CNT1);

    reg_ctrl.bf.rlmode1 = LOAD_MODE;
    reg_ctrl.bf.clksel1 =clksel_v;
    per_write(id, reg_ctrl.wrd,PER_TMR_CTRL1 );

	reg_ld1.bf.load1 =TEST_LOAD_VAL/div_val;
	per_write(id, reg_ld1.wrd, PER_TMR_LD1);

    reg_ctrl.bf.timen1 =1;
    per_write(id, reg_ctrl.wrd,PER_TMR_CTRL1 );

	rtk_pmu_setup_start();
	c0 = per_timer_int_polling_not_stop(id);
	c1 = per_timer_int_polling_not_stop(id);
	c2 = per_timer_int_polling(id);
	printf("\n DIV=%u, LOAD_V=0x%08x\n", div_val, reg_ld1.bf.load1);
	printf("C0=%llu,           C1=%llu,          C2=%llu\n", c0, c1, c2);
	printf("                C0~C1=%llu    ,  C1~C2=%llu\n", c1-c0,c2- c1);
	printf("0~C0:\n");
	per_timer_diff(c0, per_tmr_cnt_val,clksel_v);
	printf("C0~C1:\n");
	per_timer_diff(c1-c0, reg_ld1.bf.load1, clksel_v);
	printf("C1~C2:\n");
	per_timer_diff(c2-c1, reg_ld1.bf.load1, clksel_v);
	return 0;

}

static void per_timer_test_ldmode(int id){
	int i;
	for(i=0;i<4;i++){
		printf("\n\nCLKSEL: %d\n", i);
		per_timer_test_ldmode_with_clksel(id, i);
	}
}

static int per_timer_test_rollmode(int id){
	u64 ccnt;
    PER_TMR_CTRL1_t reg_ctrl;
    PER_TMR_IE1_0_t reg_tmr_inte0;
    PER_TMR_IE1_1_t reg_tmr_inte1;
	PER_TMR_LD1_t reg_ld1;
#ifdef CONFIG_TARGET_RTK_ELNATH
	PER_PERIPHERAL_INTENABLE_00_t	reg_pint0;
	PER_PERIPHERAL_INTENABLE_10_t	reg_pint1;

#else
	PER_PERIPHERAL_INTENABLE_0_t	reg_pint0;
	PER_PERIPHERAL_INTENABLE_1_t	reg_pint1;
#endif


/* peri intc */
	reg_pint0.wrd = 0x4<<id;
	reg_pint1.wrd = 0x4<<id;
#ifdef CONFIG_TARGET_RTK_ELNATH
	writel(reg_pint0.wrd, PER_PERIPHERAL_INTENABLE_00);
	writel(reg_pint1.wrd, PER_PERIPHERAL_INTENABLE_10);
#else
	writel(reg_pint0.wrd, PER_PERIPHERAL_INTENABLE_0);
	writel(reg_pint1.wrd, PER_PERIPHERAL_INTENABLE_1);
#endif
/* timer int en */
    reg_tmr_inte0.bf.tmr1e = 1;
    reg_tmr_inte1.bf.tmr1e = 1;

    per_write(id , reg_tmr_inte0.wrd,PER_TMR_IE1_0 );
    per_write(id, reg_tmr_inte1.wrd,PER_TMR_IE1_1 );

/* *******************************
0, ROLL_MODE
- Normal use
*****************************/
    reg_ctrl.bf.rlmode1 = ROLL_MODE;
    reg_ctrl.bf.clksel1 =CLKSEL_DIRECT;
    reg_ctrl.bf.timen1 =1;

    per_write(id, reg_ctrl.wrd,PER_TMR_CTRL1 );
	rtk_pmu_setup_start();
	ccnt = per_timer_int_polling(id);
	per_timer_diff(ccnt, ROLLOVER_VALUE, CLKSEL_DIRECT);

/*********************
1, ROLL_MODE
- load value should not work
*****************************/
    per_write(id, 0,PER_TMR_CTRL1 );
	reg_ld1.bf.load1 =PER_CLK;
	per_write(id, reg_ld1.wrd, PER_TMR_LD1);
    reg_ctrl.bf.rlmode1 = ROLL_MODE;
    reg_ctrl.bf.clksel1 =CLKSEL_DIRECT;
    reg_ctrl.bf.timen1 =1;
    per_write(id, reg_ctrl.wrd,PER_TMR_CTRL1 );

	rtk_pmu_setup_start();
	ccnt = per_timer_int_polling(id);
	per_timer_diff(ccnt, ROLLOVER_VALUE, CLKSEL_DIRECT);


    return 0;
}

static void per_timer_by_id(int id){
	printf("PER_TMR_%d: ROLL_MODE\n", id);
	per_timer_test_rollmode(id);
	printf("\nPER_TMR_%d: LOAD_MODE\n", id);
	per_timer_test_ldmode(id);
}





static int do_per_timer(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 caseid;
//	u64 para=0, para1=0, para2=0,para3=0;
 	caseid = simple_strtoul(argv[1], NULL, 10);
	if(argc == 2){
		if(caseid < PTIMER_MAX){
			per_timer_by_id(caseid);
		}else{
			printf("id should < %d\n", caseid);
		}

	}
	return 0;
}

U_BOOT_CMD(
	per_timer_test ,    5,    0,     do_per_timer,
	"per_timer: ",
	"\n\t\t <case>\n"
	"\t\t \n"
	"    - peri test code."
);

/*****************************************/
#define SYS_TMR_REG_OFF	0x18
#define GIC_SPI_PER_SYS_TIMER_CPU_0_INTERRUPT		107
#define SYS_TIMER_MAX		4
#define SYS_WRITE_LAT	128

#define DATA_MAX	10
static u64 ccnt_data[DATA_MAX] = {0};

static void sys_write(int idx, u32 val, u64 addr){
	int timeout = 10;
    writel(val, addr + idx*SYS_TMR_REG_OFF);
	dsb();
}

static u32 sys_read(int idx, u64 addr){
	dmb();
    return readl( addr + idx*SYS_TMR_REG_OFF);
}

static u64 sys_timer_int_polling(int idx){
	u64 timeout;
	u32 timeout_w1c;
	u32 gic_spi_irq;
	u64 ccnt=0;
	gic_spi_irq = GIC_SPI_PER_SYS_TIMER_CPU_0_INTERRUPT + idx;
	timeout = 1000000000;
	timeout_w1c = 100;
	while(timeout){
		if(sys_read(idx, PER_SYS_TIMER_CPU_0_INTERRUPT)){
			ccnt = rtk_pmu_count_get();
			if(gic_pending_chk(gic_spi_irq)){
				DBG_MSG("\tGIC: PASS - SYS_TIME%d trigger GIC_SPI(%d) Pending\n", idx, gic_spi_irq);
				sys_write(idx, 1, PER_SYS_TIMER_CPU_0_INTERRUPT);

				while(timeout_w1c && sys_read(idx, PER_SYS_TIMER_CPU_0_INTERRUPT)){
					udelay(10);
					timeout_w1c--;
				}


				goto polling_exit;
			}else{
				printf("Error: SYS_TIME%d not trigger GIC_SPI(%d) Pending\n", idx,  gic_spi_irq);
			}
			break;
		}
		timeout--;
	}
	if(timeout==0){
		printf("Error: SYS_TIME%d GIC_SPI(%d) timeout\n", idx, gic_spi_irq);

	}
polling_exit:
	if(gic_pending_chk(gic_spi_irq)){
		printf("[%s %d]	0x%x, 0x%x\n", __func__, __LINE__, readl(PER_SYS_TIMER_CPU_0_INTERRUPT), readl(0x4f8000290));
		printf("[%s %d]Error: SYS_TIME%d GIC_SPI(0x%x) Pending\n", __func__, __LINE__, idx, idx+GIC_SPI_PER_SYS_TIMER_CPU_0_INTERRUPT);
	}
	return ccnt;
}

/* Only 1 CLK Count */
static inline u64 sys_timer_read(void){
	u64 sys_t1_0, sys_t0_0, st64;
	sys_t0_0 = readl_relaxed(PER_SYS_TIMER0);
	sys_t1_0 = readl_relaxed(PER_SYS_TIMER1);
	st64 = (sys_t1_0<<32) +  sys_t0_0;
	return st64;
}

static inline void sys_timer_compare_write(u32 id, u64 val){
	sys_write(id, (u32)val, PER_SYS_TIMER_COMPARATOR_CPU_00);
	sys_write(id, (u32)(val>>32), PER_SYS_TIMER_COMPARATOR_CPU_01);
}

static inline void sys_timer_autoinc_w(u32 id, u64 val){
	sys_write(id, (u32)val, PER_SYS_TIMER_AUTOINC_CPU_00);
	sys_write(id, (u32)(val>>32), PER_SYS_TIMER_AUTOINC_CPU_01);
}


static int sys_time_test_accuracy(void){
	u64 st1, st0;
	u64 ccnt, ccnt1;
	printf("u-boot delay 1s:\n");
	rtk_pmu_setup_start();
	st0 = sys_timer_read();

	ccnt = rtk_pmu_count_get();
	mdelay(1000);
	st1 = sys_timer_read();;
	ccnt1 = rtk_pmu_count_get();
	printf("CPU \t  Cyclke=%llu, \t Time=%llu(ns)\n", (ccnt1-ccnt), (ccnt1-ccnt)*CCNT_TO_NS);
	printf("SYS_Timer \t TICK=%llu,\t Time=%llu(ns)\n", (st1-st0), (st1-st0)*TCLK_TO_NS);
	return 0;

}
static int sys_time_test_id(int id){
	int i;
	u32 gic_spi_irq = GIC_SPI_PER_SYS_TIMER_CPU_0_INTERRUPT + id;
	printf("Sys_Timer%d - SoftwareCompare(1s):\n", id);

	/* disable int*/
	sys_write(id, 0, PER_SYS_TIMER_CPU_0_INTENABLE);

	if(gic_pending_chk(gic_spi_irq)){
		printf("[%s %d]Error: SYS_TIME%d GIC_SPI(%d) Pending\n", __func__, __LINE__,  id, id+GIC_SPI_PER_SYS_TIMER_CPU_0_INTERRUPT);
	}



	if( sys_read(id, PER_SYS_TIMER_CPU_0_INTERRUPT)){
		printf("Warning: INT pending. Clear it\n");
		sys_write(id, 1,PER_SYS_TIMER_CPU_0_INTERRUPT );
		if(gic_pending_chk(gic_spi_irq)){
			printf("[%s %d]Error: SYS_TIME%d GIC_SPI(%d) Pending\n", __func__, __LINE__, id, id+PER_SYS_TIMER_CPU_0_INTERRUPT);
		}
	}

	for(i=0;i<DATA_MAX;i++){
		ccnt_data[i] = 0xffff;
	}


	/* enable int*/
	sys_write(id, 1, PER_SYS_TIMER_CPU_0_INTENABLE);



	rtk_pmu_setup_start();

	sys_timer_compare_write(id, sys_timer_read() + (PER_CLK/1000));

	for(i=0;i<DATA_MAX;i++){
		ccnt_data[i] = sys_timer_int_polling(id);
		/* read cnt*/
		sys_timer_compare_write(id,  sys_timer_read() + (PER_CLK- SYS_WRITE_LAT));
	}

	/*Dis COMPARATOR*/
	sys_timer_compare_write(id, 0);
	/* reset int bit */
	sys_write(id, 0, PER_SYS_TIMER_CPU_0_INTENABLE);
	sys_write(id, 1,PER_SYS_TIMER_CPU_0_INTERRUPT );

	for(i=0;i<DATA_MAX;i=i+2){
		printf("C%d(%llu) ~ C%d(%llu) = %llu\n", i,   ccnt_data[i], i+1, ccnt_data[i+1], ccnt_data[i+1] - ccnt_data[i] );
	}

	return 0;

}

static int sys_time_test_id_auto(int id, u64 autoi_v){
	u64 st0;
	int i;

	u32 gic_spi_irq = GIC_SPI_PER_SYS_TIMER_CPU_0_INTERRUPT + id;
	printf("Sys_Timer%d - Autoinc mode: Interval %llu(ns), 10 Loops\n", id, autoi_v*TCLK_TO_NS);

	/* disable int*/
	sys_write(id, 0, PER_SYS_TIMER_CPU_0_INTENABLE);

	if(gic_pending_chk(gic_spi_irq)){
		printf("[%s %d]Error: SYS_TIME%d GIC_SPI(%d) Pending\n", __func__, __LINE__, id, id+GIC_SPI_PER_SYS_TIMER_CPU_0_INTERRUPT);
	}



	if( sys_read(id, PER_SYS_TIMER_CPU_0_INTERRUPT)){
		printf("Warning: INT pending. Clear it\n");
		sys_write(id, 1,PER_SYS_TIMER_CPU_0_INTERRUPT );
		if(gic_pending_chk(gic_spi_irq)){
			printf("[%s %d]Error: SYS_TIME%d GIC_SPI(0x%x) Pending\n", __func__, __LINE__, id, id+PER_SYS_TIMER_CPU_0_INTERRUPT);
		}
	}

	/* enable int*/
	sys_write(id, 1, PER_SYS_TIMER_CPU_0_INTENABLE);



	rtk_pmu_setup_start();



	/* autoinc value*/

	sys_timer_autoinc_w(id, autoi_v);

	/* only first to triggier autoi*/
		/* read cnt*/
	st0 = sys_timer_read();

	sys_timer_compare_write(id, st0 + (PER_CLK+SYS_WRITE_LAT));
//	ccnt = sys_timer_int_polling(id);

	for(i=0;i<DATA_MAX;i++){
		ccnt_data[i] = sys_timer_int_polling(id);
	}

	/*Dis AutoINC */
	sys_timer_autoinc_w(id, 0);
	sys_timer_compare_write(id, 0);
	/* reset int bit */
	sys_write(id, 0, PER_SYS_TIMER_CPU_0_INTENABLE);
	sys_write(id, 1,PER_SYS_TIMER_CPU_0_INTERRUPT );

	for(i=0;i<DATA_MAX;i=i+2){
		printf("C%d(%llu) ~ C%d(%llu) = %llu\n", i,   ccnt_data[i], i+1, ccnt_data[i+1], ccnt_data[i+1] - ccnt_data[i] );
	}

	return 0;

}
static void sys_time_test_id_with_autoinc(int id){
	sys_time_test_id_auto(id, PER_CLK);
}
static int do_sys_timer(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 timerx, caseid;
//	u64 para=0, para1=0, para2=0,para3=0;

	if(argc == 3){
	 	timerx = simple_strtoul(argv[1], NULL, 10);
	 	caseid = simple_strtoul(argv[2], NULL, 10);

		if(timerx<SYS_TIMER_MAX){
			switch (caseid){
				case 0:
					sys_time_test_id(timerx);
					break;
				case 1:
					sys_time_test_id_with_autoinc(timerx);
					break;
				default:
					break;
			}


		}else{
			sys_time_test_accuracy();
		}
	}else	if(argc == 2){
			sys_time_test_accuracy();
	}else {
		return CMD_RET_USAGE;
	}

	return 0;
}

U_BOOT_CMD(
	sys_timer_test ,    5,    0,     do_sys_timer,
	"sys_timer: ",
	"\n\t\t <case>\n"
	"\t\t \n"
	"    - sys test code."
);

