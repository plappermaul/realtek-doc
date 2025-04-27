
#include <asm/arch/sramctl.h>

unsigned int _is_Master_CPU(void)
{
	return 1;	
}

unsigned int soc_vir_to_phy_addr(unsigned int vir_addr)
{
	return (vir_addr&0x1FFFFFFF);
}

/* Real SRAM controller setting function. */
void __sram_mapping(unsigned int segNo, unsigned int cpu_addr, \
                 unsigned int sram_addr, unsigned int sram_size_no)
{
	unsigned int reg_base_addr;
	volatile unsigned int *p_addr_reg;
	volatile unsigned int *p_size_reg;
	volatile unsigned int *p_base_reg;

	if(_is_Master_CPU()){
		reg_base_addr = C0SRAMSAR_REG_ADDR;
	}else{
		reg_base_addr = C1SRAMSAR_REG_ADDR;
	}

	p_addr_reg = (unsigned int *)(reg_base_addr + (SRAM_REG_SET_SIZE*segNo));
	p_size_reg = (unsigned int *)(reg_base_addr + (SRAM_REG_SET_SIZE*segNo) + 0x4 );
	p_base_reg = (unsigned int *)(reg_base_addr + (SRAM_REG_SET_SIZE*segNo) + 0x8 );

	*(p_size_reg) = sram_size_no;
	*(p_base_reg) = sram_addr;
	*(p_addr_reg) = soc_vir_to_phy_addr(cpu_addr) | SRAM_SEG_ENABLE;
	
	return;
}


/* Real SRAM controller setting function. */
void __sram_unmapping(unsigned int segNo)
{
	unsigned int reg_base_addr;
	volatile unsigned int *p_addr_reg;
	volatile unsigned int *p_size_reg;
	volatile unsigned int *p_base_reg;

	if(_is_Master_CPU()){
		reg_base_addr = C0SRAMSAR_REG_ADDR;
	}else{
		reg_base_addr = C1SRAMSAR_REG_ADDR;
	}

	p_addr_reg = (unsigned int *)(reg_base_addr + (SRAM_REG_SET_SIZE*segNo));

	*(p_addr_reg) = *(p_addr_reg) & (~SRAM_SEG_ENABLE);
	
	return;
}

/* Function Name:
 *      sram_mapping
 * Descripton:
 *      Configure the SRAM controller.
 * Input:
 *      segNo       : 0~3, indicate the configured register set.
 *	cpu_addr    : The mapped CPU virtual address.
 *	sram_addr   : The sram internal address to be mapped.
 *	sram_size_no:	SRAM_SIZE_256B          
 *			SRAM_SIZE_512B          
 *			SRAM_SIZE_1KB           
 *			SRAM_SIZE_2KB          
 *			SRAM_SIZE_4KB           
 *			SRAM_SIZE_8KB          
 *			SRAM_SIZE_16KB         
 *			SRAM_SIZE_32KB        
 *			SRAM_SIZE_64KB        
 *			SRAM_SIZE_128KB      
 *			SRAM_SIZE_256KB     
 *			SRAM_SIZE_512KB    
 *			SRAM_SIZE_1MB     
 * Output:
 *      None
 * Return:
 *      >= 0: Success
 *	<  0: Fail
 * Note:
 * 	None 
 */
int sram_mapping(unsigned int segNo, unsigned int cpu_addr, \
		 unsigned int sram_addr, unsigned int sram_size_no)
{
	unsigned int size_in_bytes;
	int          ret_code;

	ret_code = 0;

	/* Parameter checking */
	if(segNo >= SRAM_REG_SET_NUM){
		printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		printf("Wrong SRAM segment number(%d), only support (0 ~ %d)\n", segNo, (SRAM_REG_SET_NUM-1));
		ret_code = -1;
	}
	/* Parameter checking */
	if(sram_size_no > SRAM_SIZE_1MB){
		printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		printf("Wrong sram_size_no: %d\n");
		ret_code = -1;
	}


	/* Alignmemt checking */
	size_in_bytes = 128 << sram_size_no;

	if((sram_addr % size_in_bytes) != 0){
		printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		printf("sram_addr(0x%08x) isn't nature aligned with sram size(0x%08x):\n",\
			sram_addr, size_in_bytes);
		ret_code = -1;
	}
	if((cpu_addr % size_in_bytes) != 0){
		printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		printf("cpu_addr(0x%08x) isn't nature aligned with sram size(0x%08x):\n", \
			cpu_addr, size_in_bytes);
		ret_code = -1;
	}


	/* Overlapping checking */


	if(ret_code >= 0){
		__sram_mapping(segNo, cpu_addr, sram_addr, sram_size_no);

	}
	return ret_code;

}


/* Function Name:
 *      sram_unmapping
 * Descripton:
 *      Disable memory mapping of the SRAM controller.
 * Inputt:
 *      segNo       : 0~3 SRAM controller register set, indicate the target register set.
 * Output:
 *      None
 * Return:
 *      >= 0: Success
 *	<  0: Fail
 * Note:
 * 	None 
 */

int sram_unmapping(unsigned int segNo)
{
	int ret_code;

	ret_code = 0;

	if(segNo >= SRAM_REG_SET_NUM){
		printf("Wrong SRAM segment number(%d), only support (0 ~ %d)\n", segNo, (SRAM_REG_SET_NUM-1));
		return -1;
	}
	
	if(ret_code >= 0)
		__sram_unmapping(segNo);


	return ret_code;

}

/* Function Name:
 *      sram_show_mapinfo
 * Descripton:
 *      Show the sram mapping formation.
 * Inputt:
 *      segNo       : 0~3 SRAM controller register set, indicate the target register set.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 * 	None 
 */
void sram_show_mapinfo(void)
{
	unsigned int seg_num;
	unsigned int *sram_reg_base;

	seg_num = SRAM_REG_SET_NUM; /*Master and Slave CPUs*/
	
	/* Show Master CPU SRAM configuration */
	sram_reg_base = (unsigned int *)C0SRAMSAR_REG_ADDR;
	printf("CPU0 SRAM Controller Configuration:\n");
	printf("Reg base: 0x(%08p)\n", sram_reg_base);
	printf("SegNo, Segment Address, Base Address, Size\n");
	for(seg_num=0; seg_num<SRAM_REG_SET_NUM; seg_num++){
		printf("%d:", seg_num);
		printf("0x%08x", *sram_reg_base);
		printf("0x%08x", *(sram_reg_base+2));
		printf("0x%08x", *(sram_reg_base+1) << 8); /* 1: 256 bytes, therefore shit 8 bit. */
		sram_reg_base = sram_reg_base + 4;
	}
	
	/* Show Slave CPU SRAM configuration */
	sram_reg_base = (unsigned int *)C0SRAMSAR_REG_ADDR;
	printf("CPU1 SRAM Controller Configuration:\n");
	printf("Reg base: 0x(%08p)\n", sram_reg_base);
	printf("SegNo, Segment Address, Base Address, Size\n");
	for(seg_num=0; seg_num<SRAM_REG_SET_NUM; seg_num++){
		printf("%d:", seg_num);
		printf("0x%08x", *sram_reg_base);
		printf("0x%08x", *(sram_reg_base+2));
		printf("0x%08x", *(sram_reg_base+1) << 8); /* 1: 256 bytes, therefore shit 8 bit. */
		sram_reg_base = sram_reg_base + 4;
	}

	return;
}
