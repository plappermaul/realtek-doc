#include <common.h>
#include <asm/io.h>

#define GLOBAL_UVLO_CONTROL	0xf4320280
#define GLOBAL_UVLO_TRIG_CONTROL	0xf4320284
#define GLOBAL_UVLO_RECV_CTRL0	0xf4320288
#define GLOBAL_UVLO_RECV_CTRL1	0xf432028c
#define GLOBAL_UVLO_RECV_CTRL2	0xf4320290
#define GLOBAL_UVLO_RECV_CTRL3	0xf4320294
#define GLOBAL_UVLO_RECV_CTRL4	0xf4320298
#define GLOBAL_UVLO_PROF_INTEN	0xf43202a0
#define	GLOBAL_UVLO_SW_RECV	0xf43202b8
#define GLOBAL_UVLO_PROF_CTRL0	0xf43202bc

void VSFC_verification(unsigned int loop)		//(unsigned int det_recv_sel)
{
	volatile unsigned int *uvlo_ctrl,*uvlo_trig_ctrol,*uvlo_recv_ctrl0,*uvlo_recv_ctrl1,*uvlo_recv_ctrl2,*uvlo_recv_ctrl3;
	volatile unsigned int *uvlo_sw_recv,*uvlo_prof_ctrl0,*uvlo_prof_inten;
	unsigned int count=0;
	uvlo_ctrl = (unsigned int *)GLOBAL_UVLO_CONTROL;
	uvlo_trig_ctrol = (unsigned int *)GLOBAL_UVLO_TRIG_CONTROL;
	uvlo_recv_ctrl0 = (unsigned int *)GLOBAL_UVLO_RECV_CTRL0;
	uvlo_recv_ctrl1 = (unsigned int *)GLOBAL_UVLO_RECV_CTRL1;
	uvlo_recv_ctrl2 = (unsigned int *)GLOBAL_UVLO_RECV_CTRL2;
	uvlo_recv_ctrl3 = (unsigned int *)GLOBAL_UVLO_RECV_CTRL3;
	uvlo_prof_inten	= (unsigned int *)GLOBAL_UVLO_PROF_INTEN;
	uvlo_sw_recv	= (unsigned int *)GLOBAL_UVLO_SW_RECV;
	uvlo_prof_ctrl0 = (unsigned int *)GLOBAL_UVLO_PROF_CTRL0;

	for(count=0;count<loop;count++)
	{
		//step 1 : config sequence
		*uvlo_ctrl = ((*uvlo_ctrl) & (0xfffff8ff)) | (0x4<<8);	//1. CTRL.trig_sel = 0xf432_0280[10:8] = 3’h4
		*uvlo_ctrl &= 0xffffffef;								//2. CTRL.div_sel = 0xf432_0280[4] = 1’b0
		*uvlo_trig_ctrol &= (~(7<<4));							//3. TRIG_CTRL.fss_min = 0xf432_0284[6:4] = 3’h0
		*uvlo_recv_ctrl0 = ((*uvlo_recv_ctrl0) & 0xfffffffc) |0x1;		//4. RECV_CTRL0.det_recv_sel = 0xf432_0288[1:0] = 2’b1
		*uvlo_recv_ctrl1 = 0xaaaa0000;							//5. RECV_CTRL1.lowf = 0xf432_028C[31:16] = 0xaaaa
		*uvlo_recv_ctrl1 = 0xaaaa0080;							//6. RECV_CTRL1.cycle_lowf = 0xf432_028C[15:0] = 0x80
		*uvlo_recv_ctrl2 = 0x88880010;							//7. RECV_CTRL2.recv1 = 0xf432_0290[31:16] = 0x8888 (寫2次)
		*uvlo_recv_ctrl2 = 0x88880010;							//7. RECV_CTRL2.recv1 = 0xf432_0290[31:16] = 0x8888 (寫2次)
		*uvlo_recv_ctrl2 = 0x88880100;							//8. RECV_CTRL2.cycle_recv1  = 0xf432_0290[15:0] = 0x100
		*uvlo_recv_ctrl3 = 0x08080010;							//9. RECV_CTRL3.recv2 = 0xf432_0294[31:16] = 0x0808
		*uvlo_recv_ctrl3 = 0x08080140;							//10. RECV_CTRL3.cycle_recv2 = 0xf432_0294[15:0] = 0x140
		*uvlo_ctrl |= 0x1;										//11. CTRL.glb_en =0xf432_0280[0] = 0x1
		*uvlo_ctrl |= 0x1;										//12. 0xf432_0280[0] = glob_en = 1’b1
		*uvlo_sw_recv &= (0xfffffffb);							//13. 0xf432_02b8[2] = sw_recv_en = 1’b0
		*uvlo_recv_ctrl0 |= 0x3;								//14. 0xf432_0288 [1:0] = det_recv_sel = global_uvlo_bus[75:74] = 2’b11
		*uvlo_prof_inten |= (0x3<<1);							//15. 0xf432_02a0 [2:1] = [sw_recv_inten / prof_inten] = 2’b11
		*uvlo_prof_inten = *uvlo_prof_inten;
		*uvlo_prof_ctrl0 &= 0xfff00000 | 0xfff;					//16. 0xf432_02bc[19:0] = cycle_prof = 20’h00fff
		*uvlo_prof_ctrl0 = *uvlo_prof_ctrl0;
		*uvlo_prof_ctrl0 |= (1<<31);							//17. 0xf432_02bc[31] = prof_en = 1’b1
		*uvlo_prof_ctrl0 = *uvlo_prof_ctrl0;

		printf("[%d]0xf8000210 = 0x%x , 0xf8000290 = 0x%x , 0xf8000110 = 0x%x \n\r",count, readl(0xf8000210),readl(0xf8000290),readl(0xf8000110));


		//step 2 : Trigger UVLO / FSS IR drop, 0xf432_0280[27:23] = 5’h1F
		*uvlo_ctrl |= (0x1f<<23);

		//step 3 :wait 5us, check lowf -> recv1 -> recv2 -> normal的順序變動
		printf(".\n\r");	

		//step 4 :wait more than 5us => measure CPU Power Consumption has down grade => dessert UVLO / FSS IR drop => 0xf432_0280[27:23] = 5’h00
		printf("measure CPU Power Consumption.\n\r");
		udelay(3000000); //wait 3s
		*uvlo_ctrl &= 0xf07fffff;
		
		//step 5 :檢查GIC600 IRQS bit [111] 是否有trigger 為1, dv_test_top.dut.cortex_blk.u_cortex_wrapper.u_cortexmisc.prof_int 
		printf("0xf8000210 = 0x%x , 0xf8000290 = 0x%x , 0xf800029c = 0x%x \n\r",readl(0xf8000210),readl(0xf8000290),readl(0xf800029c));
		writel((readl(0xf800029c)|0x2), 0xf800029c);
		udelay(10000);		//wait 10ms
		printf("0xf8000210 = 0x%x , 0xf8000290 = 0x%x , 0xf800029c = 0x%x \n\r",readl(0xf8000210),readl(0xf8000290),readl(0xf800029c));
		udelay(3000000);		//wait 3s
	}
}	

static int do_vsfc_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned char loop = 1;
    loop = simple_strtoul(argv[1], NULL, 10);

    VSFC_verification(loop);

    return 0;
}

U_BOOT_CMD(
    vsfc, 10, 1,    do_vsfc_test,
    "Perform vsfc test",
    "vsfc\n"
);
