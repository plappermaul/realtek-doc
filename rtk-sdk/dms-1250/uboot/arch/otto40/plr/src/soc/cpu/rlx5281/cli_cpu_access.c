#include <string.h>
#include <soc.h>
#include <cpu/cpu.h>
#include <cli/cli_util.h>
#include <cli/cli_access.h>

#ifndef SECTION_CLI_ACCESS
#define SECTION_CLI_ACCESS
#endif

// CCTL0
#define CCTL0_dinval		(1<< 0)
#define CCTL0_iinval		(1<< 1)
#define CCTL0_imem0fill		(1<< 4)
#define CCTL0_imem0_OFF		(1<< 5)
#define CCTL0_imem0_ON		(1<< 6)
#define CCTL0_dwalloc_ON	(1<< 7)
#define CCTL0_dwb       	(1<< 8)
#define CCTL0_dmem0_ON		(1<<10)
#define CCTL0_dmem0_OFF		(1<<11)
#define CCTL0_dwalloc_OFF	(1<<15)
#define CCTL0_wbmerge_ON	(1<<20)
#define CCTL0_wbmerge_OFF	(1<<21)
#define CCTL0_wbuf_ON		(1<<22)
#define CCTL0_wbuf_OFF		(1<<23)
#define CCTL0_fastwb_ON		(1<<24)
#define CCTL0_fastwb_OFF	(1<<25)
#define CCTL0_lub_ON		(1<<26)
#define CCTL0_lub_OFF		(1<<27)
#define CCTL0_nbl_ON		(1<<28)
#define CCTL0_nbl_OFF		(1<<29)

// CCTL1
#define CCTL1_iwaypd_ON		(1<< 0)
#define CCTL1_iwaypd_OFF	(1<< 1)
#define CCTL1_imem1fill		(1<< 4)
#define CCTL1_imem1_OFF		(1<< 5)
#define CCTL1_imem1_ON		(1<< 6)
#define CCTL1_dmem1_ON		(1<<10)
#define CCTL1_dmem1_OFF		(1<<11)
#define CCTL1_mergeuc_ON	(1<<20)
#define CCTL1_mergeuc_OFF	(1<<21)
#define CCTL1_l2c_ON		(1<<24)
#define CCTL1_l2c_OFF		(1<<25)
#define CCTL1_l2mem_ON		(1<<26)
#define CCTL1_l2mem_OFF		(1<<27)

// CCTL2 Offset
#define CCTL2_icmask_OFFSET  	(0)
#define CCTL2_dcmask_OFFSET  	(8)
#define CCTL2_l2cmask_OFFSET 	(24)

// CCTL3
#define CCTL3_F_imem0	 	(1<< 0)
#define CCTL3_F_imem1	 	(1<< 1)
#define CCTL3_F_dmem0	 	(1<< 8)
#define CCTL3_F_dmem1	 	(1<< 9)
#define CCTL3_F_dwalloc 	(1<<12)
#define CCTL3_F_l2mem 		(1<<16)
#define CCTL3_F_l2c 		(1<<17)
#define CCTL3_F_wbuf	 	(1<<20)
#define CCTL3_F_fastwb	 	(1<<21)
#define CCTL3_F_wbmerge 	(1<<22)
#define CCTL3_F_mergeuc 	(1<<23)
#define CCTL3_F_nbl        	(1<<24)
#define CCTL3_F_lub        	(1<<25)

#define DEFINE_CPU_INT_VAR(name, is_dec, get_func_body, set_func_body) \
        SECTION_CLI_VAR int _CLI_VAR_DDR_ ## name ## _get_int_(u32_t *result) {get_func_body; return 0;} \
        SECTION_CLI_VAR int _CLI_VAR_DDR_ ## name ## _set_int_(u32_t value) {set_func_body; return 0;} \
        CLI_DEFINE_VAR(name, cpu, 1, 0, is_dec, \
                       _CLI_VAR_DDR_ ## name ## _get_int_, \
                       _CLI_VAR_DDR_ ## name ## _set_int_)

#define CCTL0_SET_FUNC_BODY_ON_OFF(fld)	{ if (1==value) {asm_mtc0(0, CCTL_REG); asm_mtc0(CCTL0_##fld##_ON, CCTL_REG);} \
                                          else {asm_mtc0(0, CCTL_REG); asm_mtc0(CCTL0_##fld##_OFF, CCTL_REG);} }
#define CCTL0_SET_FUNC_BODY_NORMAL(fld)	{ if (1==value) {asm_mtc0(0, CCTL_REG); asm_mtc0(CCTL0_##fld, CCTL_REG);}}
#define CCTL1_SET_FUNC_BODY_ON_OFF(fld)	{ if (1==value) {asm_mtc0_1(0, CCTL_REG); asm_mtc0_1(CCTL1_##fld##_ON, CCTL_REG);} \
                                          else {asm_mtc0_1(0, CCTL_REG); asm_mtc0_1(CCTL1_##fld## _OFF, CCTL_REG);} }
#define CCTL1_SET_FUNC_BODY_NORMAL(fld)	{ if (1==value) {asm_mtc0_1(0, CCTL_REG); asm_mtc0_1(CCTL1_##fld, CCTL_REG);}}
#define CCTL2_SET_FUNC_BODY(fld)	{ asm_mtc0_2(((asm_mfc0_2(CCTL_REG)&~(0x3<<CCTL2_##fld##_OFFSET))|((value-1)<<CCTL2_##fld## _OFFSET)), CCTL_REG);}
#define CCTL2_GET_FUNC_BODY(fld)	{ *result=((asm_mfc0_2(CCTL_REG)>>CCTL2_##fld##_OFFSET)&0x3)+1;}
#define CCTL3_GET_FUNC_BODY(fld)	{ *result=(CCTL3_F_##fld ==(asm_mfc0_3(CCTL_REG) & CCTL3_F_##fld ));}

#define DEFINE_CCTL0_INT_VAR_ON_OFF(name) \
        DEFINE_CPU_INT_VAR(name, 1, CCTL3_GET_FUNC_BODY(name), CCTL0_SET_FUNC_BODY_ON_OFF(name))
#define DEFINE_CCTL0_INT_VAR_NORMAL(name) \
        DEFINE_CPU_INT_VAR(name, 1, {*result=0;}, CCTL0_SET_FUNC_BODY_NORMAL(name))
#define DEFINE_CCTL1_INT_VAR_ON_OFF(name) \
        DEFINE_CPU_INT_VAR(name, 1, CCTL3_GET_FUNC_BODY(name), CCTL1_SET_FUNC_BODY_ON_OFF(name))
#define DEFINE_CCTL1_INT_VAR_NORMAL(name) \
        DEFINE_CPU_INT_VAR(name, 1, {*result=0;}, CCTL1_SET_FUNC_BODY_NORMAL(name))
#define DEFINE_CCTL2_INT_VAR(name) \
        DEFINE_CPU_INT_VAR(name, 1, CCTL2_GET_FUNC_BODY(name), CCTL2_SET_FUNC_BODY(name))

/* BPCTL (LX CP0 Reg4)
    | 31-20 |  19-18   |  17-16  |  15-6 |   5   |   4  |   3   |    2     |   1    |    0     |
    | Rsrvd | WayMask? | BTBSize | Rsrvd | CBPOn | RSOn | BTBOn | CBPInval | Rsrvd  | BTBInval |
*/
#define BP_SET_FUNC_BODY()		{if (1==value) asm_mtlxc0(0x38, BPCTL_REG); else asm_mtlxc0(0x0, BPCTL_REG);}
#define BP_GET_FUNC_BODY()		{*result=(0x38==asm_mflxc0(BPCTL_REG));}

		
cli_add_node(cpu, get, VZERO);
cli_add_parent(cpu, set);

DEFINE_CCTL0_INT_VAR_ON_OFF(dwalloc);
DEFINE_CCTL0_INT_VAR_ON_OFF(imem0);
DEFINE_CCTL1_INT_VAR_ON_OFF(imem1);
DEFINE_CCTL0_INT_VAR_ON_OFF(dmem0);
DEFINE_CCTL1_INT_VAR_ON_OFF(dmem1);
DEFINE_CCTL0_INT_VAR_ON_OFF(wbmerge);
DEFINE_CCTL0_INT_VAR_ON_OFF(wbuf);
DEFINE_CCTL0_INT_VAR_ON_OFF(fastwb);
DEFINE_CCTL0_INT_VAR_ON_OFF(lub);
DEFINE_CCTL0_INT_VAR_ON_OFF(nbl);
//DEFINE_CCTL1_INT_VAR_ON_OFF(iwaypd);
DEFINE_CCTL1_INT_VAR_ON_OFF(mergeuc);
//DEFINE_CCTL1_INT_VAR_ON_OFF(l2c);
//DEFINE_CCTL1_INT_VAR_ON_OFF(l2mem);
DEFINE_CCTL2_INT_VAR(icmask);
DEFINE_CCTL2_INT_VAR(dcmask);
//DEFINE_CCTL2_INT_VAR(l2cmask);
DEFINE_CCTL0_INT_VAR_NORMAL(dinval);
DEFINE_CCTL0_INT_VAR_NORMAL(iinval);
DEFINE_CCTL0_INT_VAR_NORMAL(dwb);
DEFINE_CCTL0_INT_VAR_NORMAL(imem0fill);
DEFINE_CCTL1_INT_VAR_NORMAL(imem1fill);
DEFINE_CPU_INT_VAR(bp, 1, BP_GET_FUNC_BODY(), BP_SET_FUNC_BODY());

