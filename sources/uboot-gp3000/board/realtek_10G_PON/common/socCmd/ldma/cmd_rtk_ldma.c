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
/************************/
extern void rtk_pmu_setup_start(void);
extern u32 rtk_pmu_stop_count_get(void);
extern unsigned int ldma_rand_r(unsigned int *seedp);
extern unsigned int ldma_rand(void);
extern void ldma_srand(unsigned int seed);

/************************/
#if defined(CONFIG_RTK_V8_FPGA_PLATFORM)
#define CPU_CLK		(40*1000*1000)		//40 MHZ
#else
#define CPU_CLK		(1000*1000*1000)		//1000 MHZ
#endif

/****************************/
#define RET_OK 0
#define LDMA_MAX_LEN_IN_DESC                    4096

enum msg_lvl_print{
	meg_lvl_datacmp=3,
	meg_lvl_proc=4,
	meg_lvl_all=9,
};
static int meg_lvl = meg_lvl_all;
static unsigned short show_cpu_cycle = 0;
#define ERR_MSG(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#define STD_MSG(fmt, ...)  { if(meg_lvl>= meg_lvl_proc) printf(fmt, ##__VA_ARGS__); }
#define DATA_MSG(fmt, ...)  { if(meg_lvl>= meg_lvl_datacmp) printf(fmt, ##__VA_ARGS__); }

/* Desc bit definition */
#define LDMA_INT_ON_COMPLETE	(1<<15)
#define LDMA_CHAN_RESET			(1<<28)
#define LDMA_CHAN_ENABLE		(1<<31)
#define LDMA_INT_FOV_EN			(1<<1)
#define LDMA_INT_DD_EN			(1<<0)

/* LDMA CONF */
#define Q_MAX   16
#define Q_OFF 0x40
#define Q_DEPTH_FULL    0x10
#define LDMA_IRQ_MUX_MAX    8

/* GIC */
#define GIC_SPI_PENDING 0x4f8000284
#define LDMA_IRQ_PERI_START	4
#ifdef CONFIG_TARGET_RTK_ELNATH
#define LDMA_GIC_IRQ_MAX	8
static int ldma_gic_mapping[LDMA_GIC_IRQ_MAX] = { 31, 32, 33, 34, 35, 36, 37, 38};
/* per int 0*/
static int ldma_per_int_mapping[LDMA_GIC_IRQ_MAX] = { 18, 19, 20, 21};
#define PER_PERIPHERAL_INTERRUPT_0_IRQ		2

#define PER_PERIPHERAL_INTERRUPT_LDMA_REG	PER_PERIPHERAL_INTERRUPT_00
#define PER_PERIPHERAL_INTERRUPT1_LDMA_REG	PER_PERIPHERAL_INTERRUPT_10
#define PER_PERIPHERAL_INTERRUPT_LDMA_t		PER_PERIPHERAL_INTERRUPT_00_t
#define PER_PERIPHERAL_INTENABLE_LDMA_REG	PER_PERIPHERAL_INTENABLE_00
#else
#define LDMA_GIC_IRQ_MAX	4
static int ldma_gic_mapping[LDMA_GIC_IRQ_MAX] = { 33, 34, 35, 36};
/* per int 0*/
static int ldma_per_int_mapping[LDMA_GIC_IRQ_MAX] = { 18, 19, 20, 21};
#define PER_PERIPHERAL_INTERRUPT_0_IRQ		2
#define PER_PERIPHERAL_INTERRUPT_LDMA_REG	PER_PERIPHERAL_INTERRUPT_0
#define PER_PERIPHERAL_INTERRUPT1_LDMA_REG	PER_PERIPHERAL_INTERRUPT_1
#define PER_PERIPHERAL_INTERRUPT_LDMA_t		PER_PERIPHERAL_INTERRUPT_0_t
#define PER_PERIPHERAL_INTENABLE_LDMA_REG	PER_PERIPHERAL_INTENABLE_0
#endif
/* DATA */

#define LDMA_COPY_DONE_POLL_NUM 2000000000
#define SRAM_DST        0x80000000
#define SRAM_SRC        0x80004000
#define SRAM_TEST_SIZE_MAX        0x4000
#define SRAM_TEST_SIZE        0x2000

#define DRAM_DST        0x00000000
#define DRAM_SRC        0x01000000 //16MB
#define DRAM_TEST_MAX	0x01000000 //16MB
#define DRAM_TEST_SIZE  0x00400000
static u32 dram_size_test   = DRAM_TEST_SIZE; //default , 4MB
static u32 irq_mux_test	= 0x0;
static u32 test_queue_id = 0;
enum test_case{
	dram2dram_irq_by_env = 0,
    sram2sram ,
    sram2dram,
	dram2sram,
    dram2dram,
    dram2dram_irq_all,
    d2d_irq0_ratelimet_cmp,
    d2d_loop,
    d2d_loop_comp,
    case_max,
};

typedef struct ldma_queue_desc{
    u32 qid;
    u64 dst;
    u64 src;
    u32 len;
    int error;
    u8 irq_mux;
    u8 irq_en;
	u8 pat;
	u8 data_not_chk;
	u32 cost_ccnt;
} ldma_queue_desc_t;
static void ldma_queue_write(u32 qid, u32 value, u32 addr){
    u32 reg_off;
    reg_off = qid*Q_OFF;
    writel(value, (unsigned long)(addr +reg_off));
}

static u32 ldma_queue_read(u32 qid, u32 addr){
    u32 reg_off;
    reg_off = qid*Q_OFF;
    return (readl((unsigned long)(addr +reg_off)));
}


static u8 ldma_init_done = 0;



static void ldma_init_data_by_desc(ldma_queue_desc_t *qdsc, int pat){
	int i;
	u8 pat_1;
	u8 *target;
	
	pat_1 = (u8) pat;
	target = (u8 *)qdsc->src;
	//    memset((void *)qdsc->src , pat, qdsc->len);
	for(i=0;i<qdsc->len; i++){
		*target = pat_1;
		target++;
		pat_1++;
	}

    memset((void *)qdsc->dst , 0x0,qdsc->len);

    flush_dcache_range(qdsc->src, qdsc->src + qdsc->len);
    flush_dcache_range(qdsc->dst, qdsc->dst+ qdsc->len);

}


static void ldma_init(void){
    u32 queue_id;

    LDMA_LDMA_QUEUE_CONTROL_REGISTER_t      reg_crtl;
    LDMA_LDMA_QUEUE_INTERRUPT_ENABLE_t      reg_int;
    LDMA_AXIM2_CONFIG_t reg_axi_m;
#ifndef CONFIG_TARGET_RTK_ELNATH
    LDMA_LDMA_GLB_LDMA_CONTROL_t reg_global_ctrl;
#endif
    if(ldma_init_done){
        return;
    }
/* defuat , 0xff00 :  Q8~Q15 ->PE,  Q0~Q7 -> Cortex */
/* All queue to cortex */
    writel(0x0000, LDMA_LDMA_GLB_QUEUE_ASSIGN_REGISTER);
 /* Rate limte , default : 0x00800400 -> 0x00800000 , to unlimited */
    writel(0x00800000, LDMA_LDMA_GLB_LDMA_RATE_LIMITER);
 /* AXI Master control ,  LDMA_AXIM2_CONFIG */
    reg_axi_m.wrd = readl(LDMA_AXIM2_CONFIG);
    reg_axi_m.bf.axi_write_outtrans_num = 0xf;
    reg_axi_m.bf.axi_read_outtrans_num = 0xf;
    writel(reg_axi_m.wrd, LDMA_AXIM2_CONFIG);
#ifndef CONFIG_TARGET_RTK_ELNATH
/* LDMA_LDMA_GLB_LDMA_CONTROL,  Enable LPAE*/
    reg_global_ctrl.wrd = readl(LDMA_LDMA_GLB_LDMA_CONTROL);
    reg_global_ctrl.bf.lpae_en = 1;
    writel(reg_global_ctrl.wrd, LDMA_LDMA_GLB_LDMA_CONTROL);
#endif
    for(queue_id=0; queue_id<Q_MAX; queue_id++){

        reg_crtl.wrd = 0;
        reg_crtl.bf.reset =1;
        //writel(reg_crtl.wrd, (unsigned long)LDMA_LDMA_QUEUE_CONTROL_REGISTER + reg_off);
        ldma_queue_write(queue_id, reg_crtl.wrd, LDMA_LDMA_QUEUE_CONTROL_REGISTER);

        ldma_queue_write(queue_id, 0, LDMA_LDMA_QUEUE_CONTROL_REGISTER);

        reg_int.wrd =0;
        reg_int.bf.dd_en =1;
        reg_int.bf.fov_en=1;
        ldma_queue_write(queue_id, reg_int.wrd, LDMA_LDMA_QUEUE_INTERRUPT_ENABLE);
        reg_crtl.bf.reset =0;
        reg_crtl.bf.en=1;
        ldma_queue_write(queue_id, reg_crtl.wrd, LDMA_LDMA_QUEUE_CONTROL_REGISTER);
    }
    ldma_init_done = 1;
    return;
}

static void ldma_queue_reset(int qid){
        ldma_queue_write(qid, LDMA_CHAN_RESET, LDMA_LDMA_QUEUE_CONTROL_REGISTER);
        udelay(10);
        ldma_queue_write(qid, 0, LDMA_LDMA_QUEUE_CONTROL_REGISTER);
        ldma_queue_write(qid, LDMA_CHAN_ENABLE, LDMA_LDMA_QUEUE_CONTROL_REGISTER);
}

static int ldma_interrupt_mux_irq_set(ldma_queue_desc_t  *qd){
    u32 reg_v;
	u32 hw_irq;
	int qid = qd->qid;
	u32 irq_mux = qd->irq_mux;

	if(qd->irq_en == 0){
		return 0;
	}

    if(irq_mux>=LDMA_IRQ_MUX_MAX){
      ERR_MSG("ldma_interrupt_mux_irq_set Error. %d >=%d\n", irq_mux, LDMA_IRQ_MUX_MAX);
		return -1;
    }
    reg_v = readl((u64)LDMA_LDMA_INTERRUPT_MUX_IRQ0_ENABLE + 8*irq_mux);
    reg_v |= 1<<qid;
    writel(reg_v, (u64)LDMA_LDMA_INTERRUPT_MUX_IRQ0_ENABLE + 8*irq_mux);
    ldma_queue_reset(qid);
	if(irq_mux>=LDMA_IRQ_PERI_START){
    	hw_irq = ldma_per_int_mapping[irq_mux - LDMA_IRQ_PERI_START];
		/* enable peri_int_0 bit for ldma4~7 */
		writel(1<<hw_irq, PER_PERIPHERAL_INTENABLE_LDMA_REG);
	}
    return 0;
}

static int ldma_polling_for_complete(int qid){
    int loop=0;
    LDMA_LDMA_QUEUE_POINTER_REGISTER_t reg_qp;
    LDMA_LDMA_QUEUE_CONTROL_REGISTER_t      reg_crtl;
    for(loop=0; loop < LDMA_COPY_DONE_POLL_NUM;loop++){
        reg_qp.wrd = ldma_queue_read(qid, LDMA_LDMA_QUEUE_POINTER_REGISTER);
        if((reg_qp.bf.wr_ptr == reg_qp.bf.rd_trans_rdptr) && (reg_qp.bf.desc_depth ==0)){
                break;
        }

    }
    if(loop==LDMA_COPY_DONE_POLL_NUM){
        reg_crtl.wrd = 0;
        reg_crtl.bf.reset =1;
        ldma_queue_write(qid, reg_crtl.wrd, LDMA_LDMA_QUEUE_CONTROL_REGISTER);
        reg_crtl.bf.reset =0;
        ldma_queue_write(qid, reg_crtl.wrd, LDMA_LDMA_QUEUE_CONTROL_REGISTER);
        reg_crtl.bf.en=1;
        ldma_queue_write(qid, reg_crtl.wrd, LDMA_LDMA_QUEUE_CONTROL_REGISTER);
        ERR_MSG("LDMA Q(%d) complete  timeourt!\n", qid);
        return -1;
    }
    return 0;
}

static void ldma_queue_full_wait(u32 qid){
    LDMA_LDMA_QUEUE_POINTER_REGISTER_t reg_qp;
    reg_qp.wrd = ldma_queue_read(qid, LDMA_LDMA_QUEUE_POINTER_REGISTER);
    while(reg_qp.bf.desc_depth== Q_DEPTH_FULL){//full
        ndelay(100) ;
        reg_qp.wrd = ldma_queue_read(qid, LDMA_LDMA_QUEUE_POINTER_REGISTER);
    }
    return;
}
static u64 ldma_gic_pending_addr(int hw_irq){
    return (GIC_SPI_PENDING + (hw_irq/32)*4);
}

static int ldma_queue_polling_for_gic(ldma_queue_desc_t *qdsc){
    u32 hw_irq;
    u32 irq_pending;
    u64 pending_reg_addr;
    u32 timeout = 1000000000;
    u32 reg_v;
	int ret=0;
	int timeout_per_write;
    hw_irq = ldma_gic_mapping[qdsc->irq_mux];
    pending_reg_addr = ldma_gic_pending_addr(hw_irq);


    while(timeout){
        irq_pending = readl(pending_reg_addr);
    //    printf("%s: Q(%d), irq_pending=0x%x\n", __func__, qdsc->qid, irq_pending);
        if(irq_pending & 1<<(hw_irq%32)){
                STD_MSG("\t\tQ(%d) trigger GIC interrupt. LDMA IRQ_MUX(%d), GIC_SPI(%d)\n", qdsc->qid, qdsc->irq_mux, hw_irq);
                reg_v = readl((u64)LDMA_LDMA_QUEUE_INTERRUPT_STATUS + qdsc->qid*0x40);
                if(reg_v & 0x2){
                        STD_MSG("\t\tQ(%d): Descriptor FIFO overflow\n", qdsc->qid);
                }
                if(reg_v & 0x1){
                        STD_MSG("\t\tQ(%d): Descriptor done interrupt\n", qdsc->qid);
                }
                //clear Queue's int pending
                writel(reg_v,(u64) LDMA_LDMA_QUEUE_INTERRUPT_STATUS + qdsc->qid*0x40);
				timeout_per_write = 1000;
				while(timeout_per_write && (readl((u64) LDMA_LDMA_QUEUE_INTERRUPT_STATUS + qdsc->qid*0x40) != 0)){
					udelay(1);
					timeout_per_write--;
				}
		        irq_pending = readl(pending_reg_addr);
				if(irq_pending & 1<<(hw_irq%32)){
					ERR_MSG("IRQ Error. It should be clean!");
					ret = -1;
				}
                break;
        }
        timeout--;
    }
	if(timeout ==0){
		ERR_MSG("Error: Q(%d),IRQ_MUX(%d). Not  trigger GIC interrupt\n", qdsc->qid, qdsc->irq_mux);
		ret = -1;
	}
    return ret;
}
static int ldma_queue_polling_for_PER_INT_0(ldma_queue_desc_t *qdsc){
    u32 per_hw_irq, hw_irq;
    u32 gic_irq_pending;
    u64 gic_pending_reg_addr;
    u32 timeout = 1000000000;
	u32 timeout_per_write = 1000;
    u32 reg_v;
	PER_PERIPHERAL_INTERRUPT_LDMA_t reg_int0;
	int ret=0;
	hw_irq = PER_PERIPHERAL_INTERRUPT_0_IRQ;
    gic_pending_reg_addr = ldma_gic_pending_addr(hw_irq);
	/* irq_mux: 4~7 */
    per_hw_irq = ldma_per_int_mapping[qdsc->irq_mux - LDMA_IRQ_PERI_START];

    while(timeout){
        gic_irq_pending = readl(gic_pending_reg_addr);
        //printf("%s: Q(%d), gic_irq_pending=0x%x\n", __func__, qdsc->qid, gic_irq_pending);
        if(gic_irq_pending & 1<<(hw_irq%32)){
                STD_MSG("\tOK: Q(%d) trigger GIC interrupt. LDMA IRQ_MUX(%d), GIC_SPI(%d)\n", qdsc->qid, qdsc->irq_mux, hw_irq);
                reg_v = readl((u64)LDMA_LDMA_QUEUE_INTERRUPT_STATUS + qdsc->qid*0x40);
                if(reg_v & 0x2){
                        STD_MSG("\tQ(%d): Descriptor FIFO overflow\n", qdsc->qid);
                }
                if(reg_v & 0x1){
                        STD_MSG("\tQ(%d): Descriptor done interrupt\n", qdsc->qid);
                }
				reg_int0.wrd = readl(PER_PERIPHERAL_INTERRUPT_LDMA_REG);
				if( !(reg_int0.wrd & (1<<per_hw_irq)) ){
					ERR_MSG("Error: Q(%d): PER_PERIPHERAL_INTERRUPT_LDMA_REG not pending.(0x%x) \n", qdsc->qid, readl(PER_PERIPHERAL_INTERRUPT_LDMA_REG));
					return -1;
				}else{
					STD_MSG("\tOK: Q(%d) trigger PER_PERIPHERAL_INTERRUPT_LDMA_REG. LDMA IRQ_MUX(%d), peri_int_0(%d)\n", qdsc->qid, qdsc->irq_mux, per_hw_irq);
					//Peri_int 1
					reg_int0.wrd = readl(PER_PERIPHERAL_INTERRUPT1_LDMA_REG);
							if( !(reg_int0.wrd & (1<<per_hw_irq)) ){
								ERR_MSG("Error: Q(%d): PER_PERIPHERAL_INTERRUPT1_LDMA_REG not pending.(0x%x) \n", qdsc->qid, readl(PER_PERIPHERAL_INTERRUPT1_LDMA_REG));
								return -1;
							}
				}

				/* elnath: irq 4~7 also connect to gic */
				if((qdsc->irq_mux >=LDMA_IRQ_PERI_START) &&  (qdsc->irq_mux < LDMA_GIC_IRQ_MAX)){
					/*chk gic pending */
					    gic_pending_reg_addr = ldma_gic_pending_addr(ldma_gic_mapping[qdsc->irq_mux]);
						gic_irq_pending = readl(gic_pending_reg_addr);
						  if(gic_irq_pending & 1<<((ldma_gic_mapping[qdsc->irq_mux])%32)){
							  STD_MSG("\tOK: Q(%d) trigger GIC interrupt. LDMA IRQ_MUX(%d), GIC_SPI(%d)\n", qdsc->qid, qdsc->irq_mux, ldma_gic_mapping[qdsc->irq_mux]);
						  }else{
							  ERR_MSG("Error: Q(%d),IRQ_MUX(%d). Not  trigger GIC interrupt.  GIC_SPI(%d)\n", qdsc->qid, qdsc->irq_mux, ldma_gic_mapping[qdsc->irq_mux]);
								ret = -1;
						  }
				}


                //clear Queue's int pending
                writel(reg_v,(u64) LDMA_LDMA_QUEUE_INTERRUPT_STATUS + qdsc->qid*0x40);
				timeout_per_write = 1000;
				while(timeout_per_write && (readl((u64) LDMA_LDMA_QUEUE_INTERRUPT_STATUS + qdsc->qid*0x40) != 0)){
					udelay(1);
					timeout_per_write--;
				}
		        gic_irq_pending = readl(gic_pending_reg_addr);
				if(gic_irq_pending & 1<<(hw_irq%32)){
					ERR_MSG("IRQ Error. It should be clean!");
					ret = -1;
				}
                break;
        }
        timeout--;
    }
	if(timeout ==0){
		ERR_MSG("Error: Q(%d),IRQ_MUX(%d). Not  trigger GIC interrupt\n", qdsc->qid, qdsc->irq_mux);
		ret = -1;
	}
    return ret;
}

static int ldma_queue_polling_for_irq_handler(ldma_queue_desc_t *qdsc){
	int ret=0;
	if(qdsc->irq_mux<LDMA_IRQ_PERI_START){
		ret = ldma_queue_polling_for_gic(qdsc);
	}else if(qdsc->irq_mux<LDMA_IRQ_MUX_MAX){
		ret = ldma_queue_polling_for_PER_INT_0(qdsc);
	}else{
		ret = -1;
	}
	return ret;
}

static int ldma_async_copy(u32 qid, u32 dst, u32 src, u32 len){
    int loop =0, length = len;
    LDMA_LDMA_QUEUE_DESCRIPTOR_DATA1_t reg_dst;
    LDMA_LDMA_QUEUE_DESCRIPTOR_DATA2_t reg_size;
    LDMA_LDMA_QUEUE_DESCRIPTOR_DATA0_t reg_src;

    if(qid>=Q_MAX){
        return -1;
    }
    if(dst==src || len==0){
        return 0;
    }
    reg_size.wrd=0;
    while(length > 0){
        ldma_queue_full_wait(qid);
        reg_src.wrd = src + loop*LDMA_MAX_LEN_IN_DESC;
        reg_dst.wrd = dst + loop*LDMA_MAX_LEN_IN_DESC;
        reg_size.bf.length = (length >= LDMA_MAX_LEN_IN_DESC) ? 0x0FFF : (length - 1);
        ldma_queue_write(qid, reg_dst.wrd, LDMA_LDMA_QUEUE_DESCRIPTOR_DATA1);
        ldma_queue_write(qid, reg_size.wrd, LDMA_LDMA_QUEUE_DESCRIPTOR_DATA2);
        ldma_queue_write(qid, reg_src.wrd, LDMA_LDMA_QUEUE_DESCRIPTOR_DATA0);
        length -= LDMA_MAX_LEN_IN_DESC;
        loop++;
    }
    return 0;
}

static int ldma_sync_copy(u32 qid, u32 dst, u32 src, u32 len){
    int ret;
    ret = ldma_async_copy(qid, dst, src, len);
    if(ret !=0){
        return -1;
    }{
        ret = ldma_polling_for_complete(qid);
    }
    return ret;
}


static int ldma_async_copy_by_desc(ldma_queue_desc_t *qdsc){
    int loop =0, length = 0;
    LDMA_LDMA_QUEUE_DESCRIPTOR_DATA1_t reg_dst;
    LDMA_LDMA_QUEUE_DESCRIPTOR_DATA2_t reg_size;
    LDMA_LDMA_QUEUE_DESCRIPTOR_DATA0_t reg_src;

    length = qdsc->len;

    if(qdsc->qid>=Q_MAX){
		ERR_MSG("Error: qid(%u)>Q_MAX(%u)\n", qdsc->qid, Q_MAX);
        return -1;
    }
    if(qdsc->dst==qdsc->src || qdsc->len==0){
		ERR_MSG("Error: dst==src or len(%u)==0\n",qdsc->len);
        return -1;
    }
    reg_size.wrd=0;
    while(length > 0){
        ldma_queue_full_wait(qdsc->qid);
        reg_src.wrd = qdsc->src + loop*LDMA_MAX_LEN_IN_DESC;
        reg_dst.wrd =qdsc->dst + loop*LDMA_MAX_LEN_IN_DESC;

        reg_size.wrd = 0;
        reg_size.bf.length = (length >= LDMA_MAX_LEN_IN_DESC) ? 0x0FFF : (length - 1);
        //
        if(length<=LDMA_MAX_LEN_IN_DESC){
            reg_size.bf.ioc = qdsc->irq_en;
        }
        ldma_queue_write(qdsc->qid, reg_dst.wrd, LDMA_LDMA_QUEUE_DESCRIPTOR_DATA1);
        ldma_queue_write(qdsc->qid, reg_size.wrd, LDMA_LDMA_QUEUE_DESCRIPTOR_DATA2);
        ldma_queue_write(qdsc->qid, reg_src.wrd, LDMA_LDMA_QUEUE_DESCRIPTOR_DATA0);
        length -= LDMA_MAX_LEN_IN_DESC;
        loop++;
    }
    return 0;
}
static int ldma_copy_by_desc(ldma_queue_desc_t *qdsc){
    int ret;

	rtk_pmu_setup_start();
    ret = ldma_async_copy_by_desc(qdsc);
    if(ret !=0){
        ret = -1;
    }else{
   		if(qdsc->irq_en ){
    	    ret = ldma_queue_polling_for_irq_handler(qdsc);
  		}else{
		    ret = ldma_polling_for_complete(qdsc->qid);
	   	}
	}
	qdsc->cost_ccnt =rtk_pmu_stop_count_get();
	if(ret==0){
		if(show_cpu_cycle){
			printf("\t\t Q(%d), SRC: 0x%llx, DST:0x%llx, SIZE:0x%x - TimeCost: %u(cpu cycle), %u(ns)\n",  qdsc->qid, qdsc->src,qdsc->dst , qdsc->len,  qdsc->cost_ccnt, qdsc->cost_ccnt*(1000000000/CPU_CLK));
		}else{
		    STD_MSG("\t\t Q(%d), SRC: 0x%llx, DST:0x%llx, SIZE:0x%x - TimeCost: %u(cpu cycle)\n",  qdsc->qid, qdsc->src,qdsc->dst , qdsc->len,  qdsc->cost_ccnt);
		}
	}

    return ret;
}

static int ldma_copy_process(ldma_queue_desc_t *qdsc){
	int pat;
	int ret;
	pat =qdsc->pat;


    ret = ldma_interrupt_mux_irq_set(qdsc);
	if(ret != RET_OK){
		goto exit_process;
	}
    ldma_init_data_by_desc(qdsc, pat);

	ret= ldma_copy_by_desc(qdsc);
    if(ret!= RET_OK){
		ERR_MSG("Error: ldma_copy_by_desc() FAIL\n");
		goto exit_process;
	}

	if(qdsc->data_not_chk ==0){
	    if(memcmp((void *)qdsc->src,(void *) qdsc->dst, qdsc->len) ==0){
	        DATA_MSG("(O) DataCMP: PASS\n");
			ret =0;
	    }else{
	        DATA_MSG("(X) DataCMP: FAIL\n");
			ret =-1;
	    }
	}
exit_process:
	qdsc->error = ret;
	return ret;
}

static void ldma_case_s2s(void){
    ldma_queue_desc_t   qdsc={0};

    qdsc.src = SRAM_SRC;
    qdsc.dst=SRAM_DST;
    qdsc.len=SRAM_TEST_SIZE;
    qdsc.qid=test_queue_id;
    qdsc.irq_en =0;
    qdsc.irq_mux=irq_mux_test;
	qdsc.pat =0xC0 + (qdsc.qid<<2) + qdsc.irq_mux;

    printf("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, qdsc.pat);

	ldma_copy_process(&qdsc);

    return;
}
static void ldma_case_s2d(void){
    unsigned long  src, dst;
    unsigned long  test_size;
    test_size = SRAM_TEST_SIZE;
    src= SRAM_SRC;
    dst= DRAM_DST;
    printf("%s: SRC: 0x%lx , DST:0x%lx , SIZE:0x%lx.\n", __func__,src, dst , test_size);
    memset((void *)src , 0x02, test_size);
    memset((void *)dst , 0x0, test_size);
    flush_dcache_range(dst, dst+test_size);

    ldma_sync_copy(test_queue_id, dst, src, test_size);
    if(memcmp((void *)src,(void *) dst, test_size) ==0){
        printf("%s: SRC: 0x%lx , DST:0x%lx  (PASS).\n", __func__,src, dst );
    }else{
        printf("%s: SRC: 0x%lx , DST:0x%lx  (FAIL).\n", __func__,src, dst );
    }
    return;
}
static void ldma_case_d2s(void){
    ldma_queue_desc_t   qdsc={0};
    int pat ;

    qdsc.src = DRAM_SRC;
    qdsc.dst=SRAM_DST;
    qdsc.len=SRAM_TEST_SIZE;
    qdsc.qid=test_queue_id;
    qdsc.irq_en =0;
    qdsc.irq_mux=irq_mux_test;

	pat =0xC0 + (qdsc.qid<<2) + qdsc.irq_mux;

    ldma_interrupt_mux_irq_set(&qdsc);

    printf("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, (u8)pat);

    ldma_init_data_by_desc(&qdsc, pat);

    if(ldma_copy_by_desc(&qdsc)){
		ERR_MSG("Error: %s FAIL\n", __func__);
		return;
	}
    if(memcmp((void *)qdsc.src,(void *) qdsc.dst, qdsc.len) ==0){
        printf("%s: PASS\n", __func__);
    }else{
        printf("%s: FAIL\n", __func__);
    }
    return;
}
static void ldma_case_d2d(void){
    unsigned long  src, dst;
    unsigned long  test_size;
    test_size = DRAM_TEST_SIZE;
    src= DRAM_SRC;
    dst= DRAM_DST;

    printf("%s: SRC: 0x%lx , DST:0x%lx , SIZE:0x%lx.\n", __func__,src, dst , test_size);
    memset((void *)src , 0x03, test_size);
    memset((void *)dst , 0x0, test_size);

    flush_dcache_range(src, src+test_size);
    flush_dcache_range(dst, dst+test_size);

    ldma_sync_copy(test_queue_id, dst, src, test_size);
    if(memcmp((void *)src,(void *) dst, test_size) ==0){
        printf("%s: SRC: 0x%lx , DST:0x%lx  (PASS).\n", __func__,src, dst );
    }else{
        printf("%s: SRC: 0x%lx , DST:0x%lx  (FAIL).\n", __func__,src, dst );
    }
    return;
}

/* setup irq: SPI33	ldma_irq[0] */
static void ldma_case_dram_irq(void){
	int i;
	int result =0;
    ldma_queue_desc_t   qdsc={0};
    int pat = ldma_rand();

    qdsc.src = DRAM_SRC;
    qdsc.dst=DRAM_DST;
    qdsc.len=DRAM_TEST_SIZE;
    qdsc.qid=test_queue_id;
	printf("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x.\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len);

	for(i=0; i< LDMA_IRQ_MUX_MAX; i++){
		qdsc.irq_en =1;
		qdsc.irq_mux=i;

		ldma_interrupt_mux_irq_set(&qdsc);

		printf("LDMA IRQ MUX=%d\n", i);

		ldma_init_data_by_desc(&qdsc, pat);

		if(ldma_copy_by_desc(&qdsc) !=0){
			ERR_MSG("%s: FAIL (in irq)\n", __func__);
		}
		if(memcmp((void *)qdsc.src,(void *) qdsc.dst, qdsc.len) ==0){
			printf("%s: PASS\n", __func__);
			result++;
		}else{
			printf("%s: FAIL (DATA cmp)\n", __func__);
			break;
		}
		printf("\n");
	}
	if(result == LDMA_IRQ_MUX_MAX){
		printf("\n%s Result: PASS\n", __func__);
	}
    return;
}

/* setup irq: SPI33	ldma_irq[1] */
static void ldma_case_dram_irq_size_by_env(void){
    ldma_queue_desc_t   qdsc={0};

    qdsc.src = DRAM_SRC;
    qdsc.dst=DRAM_DST;
    qdsc.len=dram_size_test;
    qdsc.qid=test_queue_id;
    qdsc.irq_en =1;
    qdsc.irq_mux=irq_mux_test;
	qdsc.pat =0xC0 + (qdsc.qid<<2) + qdsc.irq_mux;

    printf("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, qdsc.pat);
	ldma_copy_process(&qdsc);
    return;
}

static void ldma_case_d2d_rate_cmp(void){
    ldma_queue_desc_t   qdsc={0};
	LDMA_LDMA_GLB_LDMA_RATE_LIMITER_t reg_rate={0};
	u32 ccnt_0, ccnt_1;
    qdsc.src = DRAM_SRC;
    qdsc.dst=DRAM_DST;
    qdsc.len=DRAM_TEST_SIZE;
    qdsc.qid=test_queue_id;
    qdsc.irq_en =0;
    qdsc.irq_mux=irq_mux_test;
	qdsc.pat =0xC0 + (qdsc.qid<<2) + qdsc.irq_mux;
	qdsc.data_not_chk = 1;
    printf("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, qdsc.pat);
	reg_rate.wrd = readl(LDMA_LDMA_GLB_LDMA_RATE_LIMITER);
	reg_rate.bf.period = 0x400;
    writel(reg_rate.wrd, LDMA_LDMA_GLB_LDMA_RATE_LIMITER);
	ldma_copy_process(&qdsc);

	ccnt_0 = qdsc.cost_ccnt;
 /* Rate limte , 0x00800000 , to unlimited  - >  : 0x0080xxxx */
	reg_rate.bf.period = 0x800;
    writel(reg_rate.wrd, LDMA_LDMA_GLB_LDMA_RATE_LIMITER);
	ldma_copy_process(&qdsc);

	ccnt_1 = qdsc.cost_ccnt;

	if(ccnt_1 > ccnt_0){
		printf("Final Result: (O) PASS. ccnt_0 < ccnt_1. ccnt_1(%u) cost to ccnt_0(%u)  2x.\n", ccnt_1, ccnt_0);
	}else{
		printf("Final Result: (X) FAIL\n");
	}

 /* Rate limte , default : 0x0080xxxx -> 0x00800000 , to unlimited */
	reg_rate.bf.period = 0;
    writel(reg_rate.wrd, LDMA_LDMA_GLB_LDMA_RATE_LIMITER);

    return;
}

#define D2D_LOOP_MAX	64
static void ldma_case_d2d_loop(void){
    ldma_queue_desc_t   qdsc={0};
	unsigned int i;

    qdsc.src = DRAM_SRC;
    qdsc.dst=DRAM_DST;

	for(i=0;i<D2D_LOOP_MAX;i++){
	    qdsc.qid=test_queue_id;
	    qdsc.irq_en =1;
	    qdsc.irq_mux=irq_mux_test;
		qdsc.pat =0xC0 + (qdsc.qid<<2) + qdsc.irq_mux;

		ldma_srand(qdsc.pat + i);
	    qdsc.len=ldma_rand();
		if(qdsc.len>DRAM_TEST_MAX){
			qdsc.len &= 0xFFFFFF;
		}

	    printf("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, qdsc.pat);

		ldma_copy_process(&qdsc);

		if(qdsc.error != RET_OK){
			printf("Final Result: (X) FAIL\n");
			break;
		}
	}
	if(i==D2D_LOOP_MAX){
		printf("Final Result: (O) PASS\n");
	}
    return;
}

#define D2D_LOOP_COMP_MAX	32
static u32 ldma_rand_test_leng(int seed){
	u32 len = 0x400000,i=0;
	u32 len_tmp;

rand_try:
	ldma_srand(seed+i);
	len_tmp=ldma_rand();
	i=i+0xff;
	if(len_tmp>DRAM_TEST_MAX){
		len= len_tmp & 0xFFFFFF ;
		len = len | len_tmp >> 24;
	}else{
		len=len_tmp;
	}
	if(len==0){
		goto rand_try;
	}
	return len;
}
static void ldma_case_d2d_comp_loop(void){
    ldma_queue_desc_t   qdsc={0};
	unsigned int i;
	unsigned q, irqn;
    qdsc.src = DRAM_SRC;
    qdsc.dst=DRAM_DST;

    STD_MSG("%s: \n", __func__);

	meg_lvl = 1;
	for(q=0; q<Q_MAX; q++){
		for(irqn=0;irqn<LDMA_IRQ_MUX_MAX;irqn++){
			for(i=0;i<D2D_LOOP_COMP_MAX;i++){
			    qdsc.qid=q;
			    qdsc.irq_en =1;
			    qdsc.irq_mux=irqn;
				qdsc.pat =0xC0 + (qdsc.qid<<2) + qdsc.irq_mux;
			    qdsc.len=ldma_rand_test_leng(qdsc.pat + i);

			    STD_MSG("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, qdsc.pat);

				ldma_copy_process(&qdsc);

				if(qdsc.error != RET_OK){
					goto exit_comp;
				}
			}
		}
	}
exit_comp:
	if( q==Q_MAX){
		printf("Final Result: (O) PASS\n");
	}else{
		printf("Final Result: (X) FAIL\n");
	}
	meg_lvl = meg_lvl_all;
    return;
}

static void ldma_process_printf(int x){
	if(x/2){
		printf("\b|");
	}else{
		printf("\b-");
	}
}


#define D2D_LOOP_SRC_DST_MAX	100
static void ldma_case_src_dst_comb(u64 src, u64 dst, u32 size){
    ldma_queue_desc_t   qdsc={0};
	unsigned int i;
	unsigned q, irqn;
    qdsc.src = src;
    qdsc.dst=dst;
	qdsc.len = size;
    STD_MSG("%s: \n", __func__);

	meg_lvl = 1;

	for(q=0; q<Q_MAX; q++){
		printf("\tQ%d with IRQ_MUX : ", q);
		for(irqn=0;irqn<LDMA_IRQ_MUX_MAX;irqn++){
			printf("%d ", irqn);
			for(i=0;i<D2D_LOOP_SRC_DST_MAX;i++){
			    qdsc.qid=q;
			    qdsc.irq_en =1;
			    qdsc.irq_mux=irqn;
				qdsc.pat =(get_timer(0) & 0xff);
			    qdsc.len=qdsc.len/2;
				if(qdsc.len < 4){
					break;
				}
			    STD_MSG("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, qdsc.pat);

				ldma_copy_process(&qdsc);

				if(qdsc.error != RET_OK){
					goto exit_comp;
				}else{
					ldma_process_printf(i);
				}
			}
		}
		printf("\n");
	}
exit_comp:
	if( q==Q_MAX){
		printf("Final Result: (O) PASS\n");
	}else{
		printf("Final Result: (X) FAIL\n");
	}
	meg_lvl = meg_lvl_all;
    return;
}


static void ldma_case_src_dst_comb_size_inc(u64 src, u64 dst, u32 size){
    ldma_queue_desc_t   qdsc={0};
	unsigned int i;
	unsigned ret=0;
    qdsc.src = src;
    qdsc.dst=dst;
	qdsc.len = size;
    //STD_MSG("%s: \n", __func__);
	printf("%s: \n", __func__);
	meg_lvl = 1;

	for(i=1;i<=size;i++){
			    qdsc.qid=0;
			    qdsc.irq_en =0;
			    qdsc.irq_mux=0;
				qdsc.pat =(get_timer(0) & 0xff);
			    qdsc.len=i;
			    //STD_MSG("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, qdsc.pat);

				ldma_copy_process(&qdsc);

				printf("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x --->   ", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len);
				if(qdsc.error != RET_OK){
					ret += 1;
					printf("   (X)\n");
				}else{
					printf("   (O)\n");
				}
		
		printf("\n");
	}

	if( ret==0){
		printf("Final Result: (O) PASS\n");
	}else{
		printf("Final Result: (X) FAIL (%d)\n", ret);
	}
	meg_lvl = meg_lvl_all;
    return;
}


static int ldma_rand_limited_size(u32 size){
	int ret;
	int timeout = 10000;
	do{
		ret = (ldma_rand() % size ) + 1;
		if(timeout<=0){
			ret = 4096;
			break;
		}
	}while ((ret>=size) || (ret ==0));
	
	return ret;
}

static void ldma_case_src_dst_comb_loop(u64 src, u64 dst, u32 size, u32 loop){
    ldma_queue_desc_t   qdsc={0};
	unsigned int l;
	unsigned int i;
	unsigned q, irqn;
    qdsc.src = src;
    qdsc.dst=dst;
	qdsc.len = size;
    STD_MSG("%s: \n", __func__);

	meg_lvl = 1;

for(l=0; l<loop;l++){

	for(q=0; q<Q_MAX; q++){
		printf("\tQ%d with IRQ_MUX : ", q);
		for(irqn=0;irqn<LDMA_IRQ_MUX_MAX;irqn++){
			printf("%d ", irqn);
			for(i=0;i<D2D_LOOP_SRC_DST_MAX;i++){
			    qdsc.qid=q;
			    qdsc.irq_en =1;
			    qdsc.irq_mux=irqn;
				qdsc.pat =(get_timer(0) & 0xff);
			    qdsc.len=ldma_rand_limited_size(size);
				if(qdsc.len < 4){
					break;
				}
			    //STD_MSG("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, qdsc.pat);
					//printf("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, qdsc.pat);
				ldma_copy_process(&qdsc);

				if(qdsc.error != RET_OK){
					printf("%s - FAIL: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, qdsc.pat);
					goto exit_comp;
				}else{
					ldma_process_printf(i);
				}
			}
		}
		printf("\n");
	}
}
exit_comp:
	if(( q==Q_MAX) && (l==loop)){
		printf("Final Result: (O) PASS\n");
	}else{
		printf("Final Result: (X) FAIL\n");
				printf("		  FAIL:Q=%d, Loop=%d\n", q, l);
	}
	meg_lvl = meg_lvl_all;
    return;
}


/*
	int ret;
	int timeout = 10000;
	do{
		ret = (ldma_rand() % size ) + 1;
		if(timeout<=0){
			ret = 4096;
			break;
		}
	}while ((ret>=size) || (ret ==0));
	
	*/

#define COMB_SIZE_RAND_LOOP 10000
#define COMB_SIZE_RAND_RANGE	256
static void ldma_case_src_dst_comb_size_rand(u64 src, u64 dst, u32 size){
    ldma_queue_desc_t   qdsc={0};
	unsigned int i;
	unsigned ret=0;

    //STD_MSG("%s: \n", __func__);
	printf("%s: \n", __func__);
	meg_lvl = 1;
	if(src>dst){
		if((src - dst) < COMB_SIZE_RAND_RANGE){
			printf("Input not accepted. src:0x%llx, dst:0x%llx, size:0x%x\n", src, dst, size);
					return;
		}

	}else{
		if((dst - src) < COMB_SIZE_RAND_RANGE){
			printf("Input not accepted. src:0x%llx, dst:0x%llx, size:0x%x\n", src, dst, size);
					return;
		}
	}

	for(i=0;i<COMB_SIZE_RAND_LOOP;i++){
				qdsc.src = src + (ldma_rand()%COMB_SIZE_RAND_RANGE);
				qdsc.dst =  dst + (ldma_rand()%COMB_SIZE_RAND_RANGE);
				qdsc.len = ldma_rand_limited_size(size);
			    qdsc.qid=0;
			    qdsc.irq_en =0;
			    qdsc.irq_mux=0;
				qdsc.pat =(get_timer(0) & 0xff);

			    //STD_MSG("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, qdsc.pat);

				ldma_copy_process(&qdsc);

				
				if(qdsc.error != RET_OK){
					ret += 1;
					printf("%s:Q(%d), SRC: 0x%llx, DST:0x%llx, SIZE:0x%x --> ", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len);
					printf("  (X)\n");
				}else{
					//printf("  (O)\n");
				}
	}

	if( ret==0){
		printf("Final Result: (O) PASS\n");
	}else{
		printf("Final Result: (X) FAIL (%d)\n", ret);
	}
	meg_lvl = meg_lvl_all;
    return;
}


static void ldma_case_src_dst(u64 src, u64 dst, u32 size){
    ldma_queue_desc_t   qdsc={0};

    qdsc.src = src;
    qdsc.dst=dst;

    STD_MSG("%s: \n", __func__);

	meg_lvl = 1;
	    qdsc.qid=test_queue_id;
	    qdsc.irq_en =0;
	    qdsc.irq_mux=irq_mux_test;
		qdsc.pat =(get_timer(0) & 0xff);
	    qdsc.len=size;

	    STD_MSG("%s: Q(%d), SRC: 0x%llx , DST:0x%llx , SIZE:0x%x, IRQ_MUX:%d, pat:0x%x\n", __func__, qdsc.qid, qdsc.src,qdsc.dst , qdsc.len, qdsc.irq_mux, qdsc.pat);
		ldma_copy_process(&qdsc);

		if(qdsc.error != RET_OK){
					printf("Final Result: (X) FAIL\n");;
		}else{
					printf("Final Result: (O) PASS\n");;
		}


	meg_lvl = meg_lvl_all;
    return;
}
static void ldma_queue_id_set(u32 x){
    if(x<Q_MAX){
        test_queue_id = x;
    }else{
		printf("Wrong input. Max(0x%x)B\n",Q_MAX-1);
	}

}
static int do_ldma(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 caseid;
	u64 para=0, para1=0, para2=0,para3=0;

	if (argc <2)
		return CMD_RET_USAGE;

	if (argc == 2){
    	ldma_init();
		caseid = simple_strtoul(argv[1], NULL, 10);

		switch(caseid){
           case dram2dram_irq_by_env:
                    ldma_case_dram_irq_size_by_env();
                    break;
            case sram2sram:
                    ldma_case_s2s();
                    break;
            case sram2dram:
                    ldma_case_s2d();
                    break;
            case dram2sram:
                    ldma_case_d2s();
                    break;
           case dram2dram:
                    ldma_case_d2d();
                    break;
           case dram2dram_irq_all:
                    ldma_case_dram_irq();
                    break;
			case d2d_irq0_ratelimet_cmp:
					ldma_case_d2d_rate_cmp();
					break;
			case d2d_loop: //7
					ldma_case_d2d_loop();
					break;
			case d2d_loop_comp:
					ldma_case_d2d_comp_loop();
					break;
           default:
                    printf("Not support %d\n", caseid);
                    break;
		}
	}

	if (argc ==3){
        ldma_init();
		//caseid = simple_strtoul(argv[1], NULL, 10);
        para = simple_strtoul(argv[2], NULL, 16);
        if(strcmp(argv[1], "qid") == 0){
           ldma_queue_id_set(para);
        }else
        if(strcmp(argv[1], "dsize") == 0){
			if(para<=DRAM_TEST_MAX){
	           dram_size_test = para;
			}else{
				printf("Wrong input. Max(0x%x)B\n",DRAM_TEST_MAX);
			}
        }else
        if(strcmp(argv[1], "irq_mux") == 0){
			if(para<LDMA_IRQ_MUX_MAX){
	           irq_mux_test=para;
			}else{
				printf("Wrong input. MAX(0x%x)\n", LDMA_IRQ_MUX_MAX);
			}
        }else
		if(strcmp(argv[1], "exetime") == 0){
	           show_cpu_cycle=para;
        }else{
			printf("Not support\n");
        }
        printf("qid=0x%x, dsize=0x%x, irq_mux=0x%x, show_cpu_cycle=%u\n", test_queue_id, dram_size_test, irq_mux_test, show_cpu_cycle);
	}
	if (argc >= 4){
      // ldma_test  <src>  <dat>  <size> <extra>
        ldma_init();
        para = simple_strtoul(argv[1], NULL, 16);
        para1 = simple_strtoul(argv[2], NULL, 16);
        para2 = simple_strtoul(argv[3], NULL, 16);
		if (argc == 5){
            para3 = simple_strtoul(argv[4], NULL, 16);
			printf("ldma: src(0x%llx), dts(0x%llx), size(0x%x), extra(%u)\n", para, para1,(u32) para2,(u32) para3);
			switch(para3){
				case 0:
					ldma_case_src_dst_comb(para, para1, para2);
					break;
				case 1:
					ldma_case_src_dst_comb_size_inc(para, para1, para2);
					break;
				case 2:
					ldma_case_src_dst_comb_size_rand(para, para1, para2);
					break;
				default:
					ldma_case_src_dst_comb_loop(para, para1, para2, para3);
					break;
			}
		}else{
			printf("ldma: src(0x%llx), dts(0x%llx), size(0x%x)\n", para, para1,(u32) para2);
			ldma_case_src_dst(para, para1, para2);
		}
	}

	return 0;
}

U_BOOT_CMD(
	ldma_test ,    5,    0,     do_ldma,
	"ldma: ldma engine",
	"\n\t\t <case>\n"
	"\t\t <qid|irq_mux|dsize> <para>\n"
	"    - ldma test code."
);

