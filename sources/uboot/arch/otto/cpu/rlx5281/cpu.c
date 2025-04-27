/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>

void next_cpu_config(void);

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	next_cpu_config();
	void (*f)(void) = (void *) 0xbfc00000;
	f();
	fprintf(stderr, "*** reset failed ***\n");
	return 0;
}

void flush_cache(ulong start_addr, ulong size)
{
	rlx5281_cache_flush_dcache();
	rlx5281_cache_invali_icache();
}

void __attribute__((nomips16)) write_one_tlb(int index, u32 pagemask, u32 hi, u32 low0, u32 low1)
{
	write_c0_entrylo0(low0);
	write_c0_entrylo1(low1);
	write_c0_index(index);	
	tlb_write_indexed();
}

/*
 * For the reasion of the internal CPU function is triggered by 0->1 bit transaction in the predefined bit,
 * We show the configuration information bases on the left bit that asssum we don't clear the function bit 1->0.
 */

extern void en_wb_buf_merge(void);
extern void dis_wb_buf_merge(void);
extern void en_wb_buf(void);
extern void dis_wb_buf(void);
extern void en_cache_wa(void);
extern void dis_cache_wa(void);
extern void en_bran_predic(void);
extern void dis_bran_predic(void);
extern unsigned int is_b_predic_en(void);
extern unsigned int is_wb_en(void);
extern unsigned int is_wb_buf_merge_en(void);
extern unsigned int is_wa_en(void);

#define EN_BP	(0x1)
#define EN_WB	(0x2)
#define EN_CWB	(0x4)
#define EN_WA	(0x8)
void set_cpu_config(unsigned int cpu_config)
{
	/* Branch Prediction */
	if( (cpu_config&EN_BP) != 0x0)
	   en_bran_predic();
	else
	   dis_bran_predic();

	/* Write Buffer */
	if( (cpu_config&EN_WB) != 0x0)
		en_wb_buf();	
	else
		dis_wb_buf();

	/* Cached Write Buffer merge */
	if((cpu_config&EN_CWB) != 0x0)
		en_wb_buf_merge();
	else
		dis_wb_buf_merge();

	/* Write allocation/Write Back */
	if((cpu_config&EN_WA) != 0x0)
		en_cache_wa();
	else
		dis_cache_wa();

}

void show_cpu_config(void)
{
	unsigned int cpu_config;

	cpu_config = 0;

	printf("(BP,WB,CWBM,WA) = (");

	/* Branch Prediction */
	if(is_b_predic_en())
		printf("1,");
	else
		printf("0,");
		
	/* Write Buffer */
	if(is_wb_en())
		printf("1,");
	else
		printf("0,");

	/* Cached Write Buffer merge */
	if(is_wb_buf_merge_en())
		printf("1,");
	else
		printf("0,");

	/* Write allocation/Write Back */
	if(is_wa_en())
		printf("1");
	else
		printf("0");


	printf(")");

	return;
}


void next_cpu_config(void)
{
	unsigned int cpu_config;

	cpu_config = 0;
	/* Branch Prediction */
	if(is_b_predic_en())
		cpu_config = cpu_config | EN_BP;
	/* Write Buffer */
	if(is_wb_en())
		cpu_config = cpu_config | EN_WB;
	/* Cached Write Buffer merge */
	if(is_wb_buf_merge_en())
		cpu_config = cpu_config | EN_CWB;
	/* Write allocation/Write Back */
	if(is_wa_en())
		cpu_config = cpu_config | EN_WA;


	/* Next CPU configuration */
	cpu_config++;

	/* Setup CPU configuration */
	set_cpu_config(cpu_config);

	return;
}

