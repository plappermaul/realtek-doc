#include "dram_share.h"

unsigned int _DS_vir_to_bus_addr(unsigned int vir_addr)
{
	/* 0x80000000 ~ 0xBFFFFFFF: Ummapped address */
	if((vir_addr < 0xC0000000)&&(vir_addr >= 0x80000000)){
		return (vir_addr&((unsigned int)0x1FFFFFFF));
	}
	else{
		/*1. 0x00000000 ~ 0x7FFFFFFF: TLB decided User space */
		/*2. 0xC0000000 ~ 0xFFFFFFFF  TLB decided Kernel space */
		return _tlb_trans(vir_addr);
	}
}

/*
 * Input bus address and return physical address.
 * return 0xFFFFFFFF if the input address is illegle.
 */	
unsigned int _DS_bus_to_phy_addr(unsigned int bus_addr)
{
	unsigned int *p_offset_reg, *p_max_reg;
	unsigned int phy_addr;
	unsigned int cnt_base;
	/* locate the zone 
	 * zone 0: bus address (0x00000000 ~ 0x0FFFFFFF)
	 * zone 1: bus address (0x10000000 ~ 0x1FFFFFFF)
	 * zone 2: bus address (0x20000000 ~ 0x7FFFFFFF)
	 */
	if(is_Master_cpu()){
		cnt_base = DRAM_SHARE_REG_BASE;
	}else{
		cnt_base = DRAM_SHARE_REG_BASE+0x40;
	}

	/* Zone 0 */
	if((((unsigned int)ZONE0_BASE)<=bus_addr)&&(((unsigned int)ZONE1_BASE)>bus_addr)){
		p_offset_reg = (unsigned int *)(cnt_base);
		p_max_reg = (unsigned int *)(cnt_base+0x4);
		if(*p_max_reg < bus_addr)
			phy_addr = 0xFFFFFFFF;
		else
			phy_addr = bus_addr + (*p_offset_reg);
	}else if((((unsigned int)ZONE1_BASE)<=bus_addr)&&(((unsigned int)ZONE2_BASE)>bus_addr)){/* Zone 1 */
		p_offset_reg = (unsigned int *)(cnt_base+0x10);
		p_max_reg = (unsigned int *)((cnt_base)+0x14);
		if(*p_max_reg < bus_addr)
			phy_addr = 0xFFFFFFFF;
		else
			phy_addr = bus_addr + (*p_offset_reg);
	
	}else if((((unsigned int)ZONE2_BASE)<=bus_addr)&&(((unsigned int)ZONE_MAX)>=bus_addr)){/* Zone 2 */
		p_offset_reg = (unsigned int *)(cnt_base+0x20);
		p_max_reg = (unsigned int *)(cnt_base+0x24);
		if(*p_max_reg < bus_addr)
			phy_addr = 0xFFFFFFFF;
		else
			phy_addr = bus_addr + (*p_offset_reg);
	}else{ 	   /* illegle: 0x80000000 ~ 0xFFFFFFFF  */
		phy_addr = 0xFFFFFFFF;
	}


	return phy_addr;
}

unsigned int _tlb_trans(unsigned int vir_addr)
{
	/*1. 0x00000000 ~ 0x7FFFFFFF ==> 0x20000000 ~ 0x9FFFFFFF */
	if(vir_addr < 0x80000000){
		return (vir_addr+((unsigned int)0x20000000));
	}
	else{ /*2. 0xC0000000 ~ 0xFFFFFFFF ==> 0x20000000 ~ 0x5FFFFFFF */
		return (vir_addr-((unsigned int)0xA0000000));
	}
}

int is_Master_cpu(void)
{
	unsigned int *c0dmar0;

	c0dmar0 = (unsigned int *)C0DMAR0;

	if(*c0dmar0 != (unsigned int)0x0)
		return 1;
	else
		return 0;
}
