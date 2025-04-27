#include <asm/arch/bspchip.h>
#include <asm/arch/rtk_soc_common.h>
#include <asm/arch//memctl.h>

/*
 * Data Declaration
 */
extern tap_info_t DQS_RANGE[10];
extern tap_info_t W90_RANGE[10];


extern unsigned int board_DRAM_freq_mhz(void);
extern unsigned int board_LX_freq_mhz(void);
extern unsigned int board_CPU_freq_mhz(void);

void _update_phy_param(void);
void _DTR_DDR1_MRS_setting(unsigned int *sug_dtr, unsigned int *mr);
void _DTR_DDR2_MRS_setting(unsigned int *sug_dtr, unsigned int *mr);
void _DTR_DDR3_MRS_setting(unsigned int *sug_dtr, unsigned int *mr);
unsigned int _get_DRAM_csnum(void);
void memctlc_dram_phy_reset(void);
void _DTR_suggestion(unsigned int *sug_dtr, unsigned int ref_ms, unsigned int rp_ns,\
                             unsigned int rcd_ns, unsigned int ras_ns, unsigned int rfc_ns, \
                             unsigned int wr_ns , unsigned int rrd_ns, unsigned int fawg_ns,\
                             unsigned int wtr_ns, unsigned int rtp_ns, unsigned int cas_10_ns, \
                             unsigned int mem_freq);


#define DCACHE_LINE_SIZE (0x20)

static unsigned int stf_patterns[] = {  \
			      0xffffffff, 
			      0x00000000
		   	   };

static unsigned int adj_patterns[] = {  \
			      0x5A5A5A5A, 
		    	      0xA5A5A5A5, 
			      0xCCCCCCCC, 
			      0x33333333, 
			      0xCC33CC33, 
			      0x33CC33CC
		   	   };


/* Real Memory controller setting function. */
void __memctl_unmapping(unsigned int segNo, unsigned int cpu_addr, \
                        unsigned int size_no)
{
	unsigned int reg_base_addr;
	volatile unsigned int *p_addr_reg;
	volatile unsigned int *p_size_reg;
	volatile unsigned int *p_base_reg;

	if(_is_Master_CPU()){
		reg_base_addr = C0UMSAR_REG_ADDR;
	}else{
		reg_base_addr = C1UMSAR_REG_ADDR;
	}

	p_addr_reg = (unsigned int *)(reg_base_addr + (MEMCTL_UNMAP_REG_SET_SIZE*segNo));
	p_size_reg = (unsigned int *)(reg_base_addr + (MEMCTL_UNMAP_REG_SET_SIZE*segNo) + 0x4 );

	*(p_size_reg) = size_no;
	*(p_addr_reg) = soc_vir_to_phy_addr(cpu_addr) | MEMCTL_UNMAP_SEG_ENABLE;
	
	return;
}


/* Function Name:
 *      memctl_unmapping
 * Descripton:
 *      Configure the memory controller ummapping.
 * Input:
 *      segNo       : 0~3, indicate the configured register set.
 *	cpu_addr    : The mapped CPU virtual address.
 *	unmap_size_no:	MEMCTL_UNMAP_SIZE_256B          
 *			MEMCTL_UNMAP_SIZE_512B          
 *			MEMCTL_UNMAP_SIZE_1KB           
 *			MEMCTL_UNMAP_SIZE_2KB          
 *			MEMCTL_UNMAP_SIZE_4KB           
 *			MEMCTL_UNMAP_SIZE_8KB          
 *			MEMCTL_UNMAP_SIZE_16KB         
 *			MEMCTL_UNMAP_SIZE_32KB        
 *			MEMCTL_UNMAP_SIZE_64KB        
 *			MEMCTL_UNMAP_SIZE_128KB      
 *			MEMCTL_UNMAP_SIZE_256KB     
 *			MEMCTL_UNMAP_SIZE_512KB    
 *			MEMCTL_UNMAP_SIZE_1MB     
 * Output:
 *      None
 * Return:
 *      >= 0: Success
 *	<  0: Fail
 * Note:
 * 	None 
 */
int memctl_unmapping(unsigned int segNo, unsigned int cpu_addr, \
		   unsigned int unmap_size_no)
{
	unsigned int size_in_bytes;
	int          ret_code;

	ret_code = 0;

	/* Parameter checking */
	if(segNo >= MEMCTL_UNMAP_REG_SET_NUM){
		printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		printf("Wrong memctl unmapping segment number(%d), only support (0 ~ %d)\n", segNo, (MEMCTL_UNMAP_REG_SET_NUM-1));
		ret_code = -1;
	}
	/* Parameter checking */
	if(unmap_size_no > MEMCTL_UNMAP_SIZE_1MB){
		printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		printf("Wrong unmap_size_no: %d\n");
		ret_code = -1;
	}


	/* Alignmemt checking */
	size_in_bytes = 128 << unmap_size_no;

	if((cpu_addr % size_in_bytes) != 0){
		printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		printf("cpu_addr(0x%08x) isn't nature aligned with unmap size(0x%08x):\n", \
			cpu_addr, size_in_bytes);
		ret_code = -1;
	}


	/* Overlapping checking */


	if(ret_code >= 0){
		__memctl_unmapping(segNo, cpu_addr, unmap_size_no);
	}
	return ret_code;

}


/* Real Memory controller setting function. */
void __memctl_unmapping_disable(unsigned int segNo)
{
	unsigned int reg_base_addr;
	volatile unsigned int *p_addr_reg;

	if(_is_Master_CPU()){
		reg_base_addr = C0UMSAR_REG_ADDR;
	}else{
		reg_base_addr = C1UMSAR_REG_ADDR;
	}

	p_addr_reg = (unsigned int *)(reg_base_addr + (MEMCTL_UNMAP_REG_SET_SIZE*segNo));

	*(p_addr_reg) = *(p_addr_reg) & (~MEMCTL_UNMAP_SEG_ENABLE);
	
	return;
}


/* Function Name:
 *      memctl_unmapping_disable
 * Descripton:
 *      Configure the memory controller ummapping.
 * Input:
 *      segNo       : 0~3, indicate the configured register set.
 * Output:
 *      None
 * Return:
 *      >= 0: Success
 *	<  0: Fail
 * Note:
 * 	None 
 */
int memctl_unmapping_disable(unsigned int segNo)
{
	unsigned int size_in_bytes;
	int          ret_code;

	ret_code = 0;

	/* Parameter checking */
	if(segNo >= MEMCTL_UNMAP_REG_SET_NUM){
		printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		printf("Wrong memctl unmapping segment number(%d), only support (0 ~ %d)\n", segNo, (MEMCTL_UNMAP_REG_SET_NUM-1));
		ret_code = -1;
	}

	/* Overlapping checking */


	if(ret_code >= 0){
		__memctl_unmapping_disable(segNo);
	}
	return ret_code;

}


int memctlc_ddr_pin_detection(void)
{
	volatile unsigned int *dram_addr;
	volatile unsigned int data_temp;
	unsigned int detection_pattern;
	unsigned int i, j;
	int ret_value;
	
	dram_addr = (unsigned int *)0x80000000;

	ret_value = 0;
	/*
	 * DQ Pins
	 */

	/* 1. Stock At Fault: 0x0, 0xFFFFFFFF */
	for(i=0; i< sizeof(stf_patterns)/sizeof(unsigned int); i++){
		detection_pattern = stf_patterns[i];
		/* 0. Flush and invalidate Data cache */
		write_back_invalidate_dcache();
		/* 1. Read data into Data cache */
		data_temp = *dram_addr;
		/* 2. Write detection pattern into data cache */
		for(j=0; j<(DCACHE_LINE_SIZE/sizeof(unsigned int));j++){
			*(dram_addr+j) = detection_pattern;
		}
		/* 3. Flush and invalidate data cache */
		write_back_invalidate_dcache();
		/* 4. Read back and print the result */
		for(j=0; j<(DCACHE_LINE_SIZE/sizeof(unsigned int));j++){
			data_temp = *(dram_addr+j);
		}

		if( detection_pattern != *(dram_addr+2)){
			printf("%s, %d: DQs STF Fault, pattern(0x%08x)!=0x%08x\n", __FUNCTION__,\
				 __LINE__, detection_pattern, *(dram_addr+2));
			ret_value = -1;
		}
	}
	


	/* 2. Individual DQ pins: 0x00010001, 0x00020002, 0x00040004, 0x00080008, .... */
	for(i=0; i< 16; i++){
		detection_pattern = (1 << i) | ((1 << i) << 16);
		/* 0. Flush and invalidate Data cache */
		write_back_invalidate_dcache();
		/* 1. Read data into Data cache */
		data_temp = *dram_addr;
		/* 2. Write detection pattern into data cache */
		for(j=0; j<(DCACHE_LINE_SIZE/sizeof(unsigned int));j++){
			*(dram_addr+j) = detection_pattern;
		}
		/* 3. Flush and invalidate data cache */
		write_back_invalidate_dcache();
		/* 4. Read back and print the result */
		for(j=0; j<(DCACHE_LINE_SIZE/sizeof(unsigned int));j++){
			data_temp = *(dram_addr+j);
		}

		if( detection_pattern != *(dram_addr+2)){
			printf("%s, %d: DQ(%d) detetion error, pattern(0x%08x)!=0x%08x\n", __FUNCTION__, \
				__LINE__, i, detection_pattern, *(dram_addr+2));
			ret_value = -1;
		}
	}

	/* 3. Adjacent Pins Short: 0x33CC33CC, 0xCC33CC33, 0xCCCCCCCC, 0x33333333, 0xA5A5A5A5, 0x5A5A5A5A */
	for(i=0; i< sizeof(adj_patterns)/sizeof(unsigned int); i++){
		detection_pattern = stf_patterns[i];
		/* 0. Flush and invalidate Data cache */
		write_back_invalidate_dcache();
		/* 1. Read data into Data cache */
		data_temp = *dram_addr;
		/* 2. Write detection pattern into data cache */
		for(j=0; j<(DCACHE_LINE_SIZE/sizeof(unsigned int));j++){
			*(dram_addr+j) = detection_pattern;
		}
		/* 3. Flush and invalidate data cache */
		write_back_invalidate_dcache();
		/* 4. Read back and print the result */
		for(j=0; j<(DCACHE_LINE_SIZE/sizeof(unsigned int));j++){
			data_temp = *(dram_addr+j);
		}

		if( detection_pattern != *(dram_addr+2)){
			printf("%s, %d: DQs Adjacent pins short, pattern(0x%08x)!=0x%08x\n", __FUNCTION__,\
				 __LINE__, detection_pattern, *(dram_addr+2));
			ret_value = -1;
		}
	}


	/*
	 * Address Pins
	 */


	return ret_value;
}


void _set_dmcr(unsigned int dmcr_value)
{
	/* Error cheching here ?*/
	REG(DMCR) = dmcr_value;
	while(REG(DMCR) & 0x80000000);

	return;
}

void memctlc_set_DRAM_buswidth(unsigned int buswidth)
{
	volatile unsigned int *dcr;
	unsigned int t_cas, dmcr_value;
	/* get DCR value */
	dcr = (unsigned int *)DCR;


	switch (buswidth){
		case 8:
			if(memctls_is_DDR()){
				t_cas = ((REG(DTR0) & (~(DTR0_CAS_MASK))) >> DTR0_CAS_FD_S);
				/*0:2.5, 1:2, 2:3*/
				if(t_cas == 0)
					dmcr_value = 0x00100062;
				else if(t_cas == 1)
					dmcr_value = 0x00100022;
				else if(t_cas == 2)
					dmcr_value = 0x00100032;
				else{
					printf("%s, %d: Error t_cas value(%d)\n", __FUNCTION__, __LINE__, t_cas);
					return;
				}
				_set_dmcr(dmcr_value);
			}
			*dcr = (*dcr & (~((unsigned int)DCR_DBUSWID_MASK)));
			break;

		case 16:
			if(memctls_is_DDR()){
				t_cas = ((REG(DTR0) & (~(DTR0_CAS_MASK))) >> DTR0_CAS_FD_S);
				/*0:2.5, 1:2, 2:3*/
				if(t_cas == 0)
					dmcr_value = 0x00100062;
				else if(t_cas == 1)
					dmcr_value = 0x00100022;
				else if(t_cas == 2)
					dmcr_value = 0x00100032;
				else{
					printf("%s, %d: Error t_cas value(%d)\n", __FUNCTION__, __LINE__, t_cas);
					return;
				}
				_set_dmcr(dmcr_value);
			}
			*dcr = (*dcr & (~((unsigned int)DCR_DBUSWID_MASK))) | (unsigned int)(1<<DCR_DBUSWID_FD_S);
			break;

		default:
			printf("%s, %d: Error buswidth value(%d)\n", __FUNCTION__, __LINE__, buswidth);
			break;
	}

	return;
}
void memctlc_set_DRAM_colnum(unsigned int col_num)
{
	volatile unsigned int *dcr;

	/* get DCR value */
	dcr = (unsigned int *)DCR;


	switch (col_num){
		case 256:
			*dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK)));
			break;

		case 512:
			*dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(1<<DCR_COLCNT_FD_S);
			break;

		case 1024:
			*dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(2<<DCR_COLCNT_FD_S);
			break;

		case 2048:
			*dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(3<<DCR_COLCNT_FD_S);
			break;

		case 4096:
			*dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(4<<DCR_COLCNT_FD_S);
			break;

		default:
			printf("%s, %d: Error column number value(%d)\n", __FUNCTION__, __LINE__, col_num);
			break;
	}

	return;


}
void memctlc_set_DRAM_rownum(unsigned int row_num)
{
	volatile unsigned int *dcr;

	/* get DCR value */
	dcr = (unsigned int *)DCR;


	switch (row_num){
		case 2048:
			*dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK)));
			break;

		case 4096:
			*dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(1<<DCR_ROWCNT_FD_S);
			break;

		case 8192:
			*dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(2<<DCR_ROWCNT_FD_S);
			break;

		case 16384:
			*dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(3<<DCR_ROWCNT_FD_S);
			break;

		case (32*1024):
			*dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(4<<DCR_ROWCNT_FD_S);
			break;

		case (64*1024):
			*dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(5<<DCR_ROWCNT_FD_S);
			break;

		default:
			printf("%s, %d: Error row number value(%d)\n", __FUNCTION__, __LINE__, row_num);
			break;
	}

	return;


}
void memctlc_set_DRAM_banknum(unsigned int bank_num)
{
	volatile unsigned int *dcr, *dmcr;

	/* get DCR value */
	dcr = (unsigned int *)DCR;
	dmcr = (unsigned int *)DMCR;


	if(memctls_is_DDR()){
		switch (bank_num){
			case 2:
				*dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK)));
				break;

			case 4:
				*dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK))) | (unsigned int)(1<<DCR_BANKCNT_FD_S);
				break;

			default:
				printf("%s, %d: Error DDR1 bank number value(%d)\n", __FUNCTION__, __LINE__, bank_num);
				break;
		}
	}else{
		switch (bank_num){
			case 4:
				*dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK))) | (unsigned int)(1<<DCR_BANKCNT_FD_S);
				break;

			case 8:
				*dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK))) | (unsigned int)(2<<DCR_BANKCNT_FD_S);
				break;

			default:
				printf("%s, %d: Error DDR2/3 bank number value(%d)\n", __FUNCTION__, __LINE__, bank_num);
				break;
		}
	}

	return;

}
void memctlc_set_DRAM_chipnum(unsigned int chip_num)
{
	volatile unsigned int *dcr;

	/* get DCR value */
	dcr = (unsigned int *)DCR;


	switch (chip_num){
		case 1:
			*dcr = (*dcr & (~((unsigned int)DCR_DCHIPSEL_MASK)));
			break;

		case 2:
			*dcr = (*dcr & (~((unsigned int)DCR_DCHIPSEL_MASK))) | (unsigned int)(1<<DCR_DCHIPSEL_FD_S);
			break;

		default:
			printf("%s, %d: Error chip number value(%d)\n", __FUNCTION__, __LINE__, chip_num);
			break;
	}

	return;

}


typedef struct ad_action{
	int wtap_moving;
	int r_r_tap_moving;
	int r_f_tap_moving;
	int done;
} t_ad_action;




int memctlc_ddr123_calibration(void)
{

	t_ad_action decision_report[32];
	int bit_cali_status[32];
	unsigned int error_bit_mask;
	int i;

	/* 1.Initialization PHY control parameter and algorithm status. */
	for(i=0;i<32;i++){
		bit_cali_status[i] = 0;
		decision_report[i].wtap_moving = 0;
		decision_report[i].r_r_tap_moving = 0;
		decision_report[i].r_f_tap_moving = 0;
		decision_report[i].done = 0;	
	}

	while(1){

		/* 2.Write/read data patterns and report error_bit_mask */

		/* 3.Delay tap adjustment algorithm with the error_bit_mask and bit_cali_status input. */

		/* 4.Checking and execution the decision report. */


	}

	return 0;
}


/* Function Name: 
 * 	memctlc_dqs_calibration_expansion
 * Descripton:
 *	Expand the over all delay tap spectrum for the input DQ.
 * Input:
 *	dram_type: MEMCTL_DRAM_TYPE_DDRI, MEMCTL_DRAM_TYPE_DDRII, MEMCTL_DRAM_TYPE_DDRIII
 * Output:
 * 	None
 * Return:
 *	None.
 */
#define MEMCTL_BASE 		(0xB8001000)
#define DDR_PHY_CONTR_BASE	(MEMCTL_BASE + (0x500))
#define DACCR			(DDR_PHY_CONTR_BASE + (0x0))
#define DACSPCR			(DDR_PHY_CONTR_BASE + (0x4))
#define DACSPAR			(DDR_PHY_CONTR_BASE + (0x8))
#define DACSPSR			(DDR_PHY_CONTR_BASE + (0xC))
#define DACDQ0RR		(DDR_PHY_CONTR_BASE + (0x10))
#define DACDQ1RR		(DDR_PHY_CONTR_BASE + (0x14))
#define DACDQ2RR		(DDR_PHY_CONTR_BASE + (0x18))
#define DACDQ3RR		(DDR_PHY_CONTR_BASE + (0x1C))
#define DACDQ4RR		(DDR_PHY_CONTR_BASE + (0x20))
#define DACDQ5RR		(DDR_PHY_CONTR_BASE + (0x24))
#define DACDQ6RR		(DDR_PHY_CONTR_BASE + (0x28))
#define DACDQ7RR		(DDR_PHY_CONTR_BASE + (0x2C))
#define DACDQ8RR		(DDR_PHY_CONTR_BASE + (0x30))
#define DACDQ9RR		(DDR_PHY_CONTR_BASE + (0x34))
#define DACDQ10RR		(DDR_PHY_CONTR_BASE + (0x38))
#define DACDQ11RR		(DDR_PHY_CONTR_BASE + (0x3C))
#define DACDQ12RR		(DDR_PHY_CONTR_BASE + (0x40))
#define DACDQ13RR		(DDR_PHY_CONTR_BASE + (0x44))
#define DACDQ14RR		(DDR_PHY_CONTR_BASE + (0x48))
#define DACDQ15RR		(DDR_PHY_CONTR_BASE + (0x4C))
#define DACDQ0FR		(DDR_PHY_CONTR_BASE + (0x50))
#define DACDQ1FR		(DDR_PHY_CONTR_BASE + (0x54))
#define DACDQ2FR		(DDR_PHY_CONTR_BASE + (0x58))
#define DACDQ3FR		(DDR_PHY_CONTR_BASE + (0x5C))
#define DACDQ4FR		(DDR_PHY_CONTR_BASE + (0x60))
#define DACDQ5FR		(DDR_PHY_CONTR_BASE + (0x64))
#define DACDQ6FR		(DDR_PHY_CONTR_BASE + (0x68))
#define DACDQ7FR		(DDR_PHY_CONTR_BASE + (0x6C))
#define DACDQ8FR		(DDR_PHY_CONTR_BASE + (0x70))
#define DACDQ9FR		(DDR_PHY_CONTR_BASE + (0x74))
#define DACDQ10FR		(DDR_PHY_CONTR_BASE + (0x78))
#define DACDQ11FR		(DDR_PHY_CONTR_BASE + (0x7C))
#define DACDQ12FR		(DDR_PHY_CONTR_BASE + (0x80))
#define DACDQ13FR		(DDR_PHY_CONTR_BASE + (0x84))
#define DACDQ14FR		(DDR_PHY_CONTR_BASE + (0x88))
#define DACDQ15FR		(DDR_PHY_CONTR_BASE + (0x8C))
#define DCDQMR			(DDR_PHY_CONTR_BASE + (0x90))
//#define DMCR			(MEMCTL_BASE + (0x1C))
#define DMCR_MRS_BUSY		(0x80000000)

#define MAX_NUM_OF_DQ		(16)
#define DRAM_TEST_ADDR		(0x80000000)
#define TEST_BYTE_SIZE		(0x8000)

#if 0
unsigned int cali_pattern[] = { 0x00010000, 0x00000000, 0x0000FFFF, 0xFFFF0000,\
				0x00FF00FF, 0xFF00FF00, 0xF0F0F0F0, 0x0F0F0F0F,\
 				0x5A5AA5A5, 0xA5A55A5A, 0x5A5AA5A5, 0xA5A55A5A,\
				0xA5A55A5A, 0x5A5AA5A5, 0xA5A55A5A, 0x5A5AA5A5,\
				0x5555AAAA, 0xAAAA5555, 0x5555AAAA, 0xAAAA5555,\
				0xAAAA5555, 0x5555AAAA, 0xAAAA5555, 0x5555AAAA\
			      };

#else
unsigned int cali_pattern[] = { 0x00000000, 0x0000FFFF, 0xFFFF0000, 0x00FF00FF, \
				0xFF00FF00, 0xF0F0F0F0, 0x0F0F0F0F, 0x5A5AA5A5, \
				0xA5A55A5A, 0x5A5AA5A5, 0xA5A55A5A, 0xA5A55A5A, \
				0x5A5AA5A5, 0xA5A55A5A, 0x5A5AA5A5, 0x5555AAAA, \
				0xAAAA5555, 0x5555AAAA, 0xAAAA5555, 0xAAAA5555, \
				0x5555AAAA, 0xAAAA5555, 0x5555AAAA, 0xFFFFFFFF, \
				0x00000010, 0x00000020, 0x00000040, 0x00000080, \
				0x00000001, 0x00000002, 0x00000004, 0x00000008, \
				0x00001000, 0x00002000, 0x00004000, 0x00008000, \
				0x00000100, 0x00000200, 0x00000400, 0x00000800, \
				0x00010000, 0x00020000, 0x00040000, 0x00080000, \
				0x01000000, 0x02000000, 0x04000000, 0x08000000, \
				0x00100000, 0x00200000, 0x00400000, 0x00800000, \
				~0x10000000, ~0x20000000, ~0x40000000, ~0x80000000, \
				~0x00000010, ~0x00000020, ~0x00000040, ~0x00000080, \
				~0x00000001, ~0x00000002, ~0x00000004, ~0x00000008, \
				~0x00001000, ~0x00002000, ~0x00004000, ~0x00008000, \
				~0x00000100, ~0x00000200, ~0x00000400, ~0x00000800, \
				~0x00010000, ~0x00020000, ~0x00040000, ~0x00080000, \
				~0x01000000, ~0x02000000, ~0x04000000, ~0x08000000, \
				~0x00100000, ~0x00200000, ~0x00400000, ~0x00800000, \
				~0x10000000, ~0x20000000, ~0x40000000, ~0x80000000\
			     };
#endif
void _update_phy_param(void)
{
	volatile unsigned int *dmcr;
	volatile unsigned int *dcr;
	volatile unsigned int *dacr;
	volatile unsigned int dacr_tmp1, dacr_tmp2;
	volatile unsigned int dmcr_tmp;

	dmcr = (unsigned int *)DMCR;
	dcr = (unsigned int *)DCR;
	dacr = (unsigned int *)DACCR;
	
	/* Write DMCR register to sync the parameters to phy control. */
	dmcr_tmp = *dmcr;
	/* Waiting for the completion of the update procedure. */
	while((*dmcr & ((unsigned int)DMCR_MRS_BUSY)) != 0);

	__asm__ __volatile__("": : :"memory");

	/* reset phy buffer pointer */
	dacr_tmp1 = *dacr;
	dacr_tmp1 = dacr_tmp1 & (0xFFFFFFEF);
	dacr_tmp2 = dacr_tmp1 | (0x10);
	*dacr = dacr_tmp1 ;
	__asm__ __volatile__("": : :"memory");
	*dacr = dacr_tmp2 ;

	return;
}


#define DQCDQ_WTAP	(24)
#define DQCDQ_CURTAP	(8)
#define TAP_STAUTS_USELESS (0)
#define TAP_STAUTS_USEFULL (1)
#define _flush_inv_dcache write_back_invalidate_dcache
//#define _flush_inv_dcache rlx5281_cache_flush_dcache

void memctlc_dqs_calibration_expansion(unsigned int dq, unsigned int buswidth)
{
	volatile unsigned int *dacqdqr;
	volatile unsigned int *dacqdqf;
	volatile unsigned int read_in_buf;
	unsigned int r_r_tap_status, r_f_tap_status;
	unsigned int r_r_tap_dmask, r_f_tap_dmask;
	unsigned int i, j;
	unsigned int *test_addr;
	unsigned int w_tap, r_r_tap, r_f_tap, r_tap;

	if((dq>=MAX_NUM_OF_DQ) || (dq >= buswidth)){
		printf("%s, %d: DQ number input error (%d) > %d\n", __FUNCTION__, __LINE__, dq, MAX_NUM_OF_DQ);
	}

	/* Locate the DACDQ register for the input dq. */
	dacqdqr = DACDQ0RR;
	dacqdqr = dacqdqr + dq;
	dacqdqf = DACDQ0FR;
	dacqdqf = dacqdqf + dq;

	/* Initialize the DACDQ register */
	w_tap = 0;
	r_r_tap = 0;
	r_f_tap = 0;

	*dacqdqr = 0;
	*dacqdqf = 0;

	_update_phy_param();


	/* Flush and invalidate DCache */
	_flush_inv_dcache();

	for(w_tap=0; w_tap<32; w_tap++){
		printf("W(%02d): ", w_tap);
		for(r_tap=0; r_tap<32; r_tap++){
			/* Configure write delay tap and read delay tap. */
			r_r_tap = r_tap;
			r_f_tap = r_tap;

			*dacqdqr = (w_tap << DQCDQ_WTAP) | (r_r_tap << DQCDQ_CURTAP);
			*dacqdqf = (r_f_tap << DQCDQ_CURTAP);
			_update_phy_param();

			r_r_tap_status = TAP_STAUTS_USEFULL; 
			r_f_tap_status = TAP_STAUTS_USEFULL;
		
			/* Build up data mask for the dq. */
			r_r_tap_dmask = 0;
			r_f_tap_dmask = 0;
			j = 32;
			do{
				unsigned int tmp_r;
				unsigned int tmp_f;

				tmp_r = ((1 << dq) << buswidth);
				tmp_f = (1 << dq);

				r_r_tap_dmask = r_r_tap_dmask | tmp_r;
				r_f_tap_dmask = r_f_tap_dmask | tmp_f;

				j = j - buswidth;
				if(j > buswidth){
					r_r_tap_dmask = r_r_tap_dmask << (2*buswidth);
					r_f_tap_dmask = r_f_tap_dmask << (2*buswidth);
				}
			}while(j > buswidth);
//			printf("r_r_tap_dmask = 0x%08x, r_f_tap_dmask = 0x%08x, dacqdqr(0x%08p) = (0x%08x), dacqdqf(0x%08p) = (0x%08x)\n",\
				 r_r_tap_dmask, r_f_tap_dmask, dacqdqr, *dacqdqr, dacqdqf, *dacqdqf);
	
//			for(i=0; i< sizeof(cali_pattern)/sizeof(unsigned int); i++){
//				if( (r_r_tap_status == TAP_STAUTS_USELESS) && (r_f_tap_status == TAP_STAUTS_USELESS) )
//					break;
				
				/* Read data into Dcache and fill it with the pattern. */
				test_addr = (unsigned int *)DRAM_TEST_ADDR;
				i = 0;
				for(j=0; j < TEST_BYTE_SIZE; j=j+4){
					read_in_buf = *test_addr;
					*test_addr = cali_pattern[i];
					i = (i+1) % (sizeof(cali_pattern)/sizeof(unsigned int));
					test_addr++;
				}
	
				/* Trigger burst write through dcache flush. */
				 _flush_inv_dcache();

				/* Burst read back data into dcache. */
				test_addr = (unsigned int *)DRAM_TEST_ADDR;
				for(j=0; j < TEST_BYTE_SIZE; j= j + DCACHE_LINE_SIZE ){
					read_in_buf = *test_addr;
					test_addr = (unsigned int*)((unsigned int)test_addr + ((unsigned int)DCACHE_LINE_SIZE));
				}
			
			
				/* Verify it for DQ rising edge. */
				test_addr = (unsigned int *)DRAM_TEST_ADDR;
				i = 0;
				/* Verify it only when it's still useful. */
				if(r_r_tap_status == TAP_STAUTS_USEFULL){
					for(j=0; j < TEST_BYTE_SIZE; j=j+4){
						if( (*test_addr & r_r_tap_dmask) != (cali_pattern[i] & r_r_tap_dmask)){
								r_r_tap_status = TAP_STAUTS_USELESS;
								break;		
						}
						i = (i+1) % (sizeof(cali_pattern)/sizeof(unsigned int));
						test_addr++;
					}
				}


				/* Verify it for DQ falling edge. */
				test_addr = (unsigned int *)DRAM_TEST_ADDR;
				i = 0;
				/* Verify it only when it's still useful. */
				if(r_f_tap_status == TAP_STAUTS_USEFULL){
					for(j=0; j < TEST_BYTE_SIZE; j=j+4){
						if( (*test_addr & r_f_tap_dmask) != (cali_pattern[i] & r_f_tap_dmask)){
								r_f_tap_status = TAP_STAUTS_USELESS;
								break;		
						}
						i = (i+1) % (sizeof(cali_pattern)/sizeof(unsigned int));
						test_addr++;
					}
				}
	
//			}

			if(r_r_tap_status == TAP_STAUTS_USEFULL)
				printf("(%02d,", r_r_tap);
			else
				printf("(xx,");

			if(r_f_tap_status == TAP_STAUTS_USEFULL)
				printf("%02d)", r_f_tap);
			else
				printf("xx)");
		}

		printf("\n");
	}


	return;

}


/* Function Name: 
 * 	memctlc_dqm_calibration
 * Descripton:
 *	Calibration DQS delay tap for DQM signals.
 * Input:
 *	dram_type: MEMCTL_DRAM_TYPE_DDRI, MEMCTL_DRAM_TYPE_DDRII, MEMCTL_DRAM_TYPE_DDRIII
 * Output:
 * 	None
 * Return:
 *	MEMCTL_PASS: calibration pass.
 *	MEMCTL_FAIL: calibration fail.
 */
void memctlc_dqm_calibration(void)
{
	volatile unsigned int *dcdqmr;
	volatile unsigned int *w_test_addr;
	volatile unsigned short *h_test_addr;
	volatile unsigned int read_tmp;
	unsigned int dqm0, dqm1;
	unsigned int dqm_tap_min, dqm_tap_max;


	dcdqmr    = (unsigned int *)DCDQMR;
	w_test_addr = (unsigned int *)0xa0000000;
	h_test_addr = (unsigned short *)0xa0000000;

	/* initialize the word value with 0*/
	*w_test_addr = 0;

	dqm_tap_min = 32;//invalid value
	dqm_tap_max = 32;//invalid value

	//printf("dqm0: ");
	for(dqm0=0; dqm0<32; dqm0++){
		/* setting dqm0 tap delay */
		*dcdqmr = (*dcdqmr & 0x00FFFFFF) | (dqm0 << 24) ;

		_update_phy_param();

		*h_test_addr = (unsigned short)0xFFFF;

		__asm__ __volatile__("": : :"memory");

		if(*w_test_addr !=  ((unsigned int)0xFFFF0000)){
			__asm__ __volatile__("": : :"memory");
			if((dqm_tap_min != 32) && (dqm_tap_max == 32)){
				dqm_tap_max = dqm0 - 1;
				if((dqm_tap_max - dqm_tap_min) < MIN_WRITE_MASK_DELAY_WINDOW){
					dqm_tap_min = 32;
					dqm_tap_max = 32;
				}
			}
		//	printf("xx, ");
		}else{
			if(dqm_tap_min == 32)
				dqm_tap_min = dqm0;

			if((dqm0==31) && (dqm_tap_max == 32))
				dqm_tap_max = dqm0;

		//	printf("%02d, ", dqm0);
		}
	
		__asm__ __volatile__("": : :"memory");
		*w_test_addr = 0x0;
	}
//	printf("\n");

	if((dqm_tap_max - dqm_tap_min) < MIN_WRITE_MASK_DELAY_WINDOW)
		*dcdqmr = (*dcdqmr & 0x00FFFFFF);
	else
		*dcdqmr = (*dcdqmr & 0x00FFFFFF) | (((dqm_tap_max + dqm_tap_min)/2) << 24) ;

	_update_phy_param();

//	printf("dqm_tap_max(%d), dqm_tap_min(%d)\n", dqm_tap_max, dqm_tap_min);

	w_test_addr = (unsigned int *)0xa0000000;
	h_test_addr = (unsigned short *)0xa0000002;
	*w_test_addr = 0;

	dqm_tap_min = 32;//invalid value
	dqm_tap_max = 32;//invalid value

	//printf("dqm1: ");
	for(dqm1=0; dqm1<32; dqm1++){

		/* setting dqm0 tap delay */
		*dcdqmr = (*dcdqmr & 0xFF00FFFF) | (dqm1 << 16) ;

		_update_phy_param();

		*h_test_addr = (unsigned short)0xFFFF;
		__asm__ __volatile__("": : :"memory");

		if(*w_test_addr !=  ((unsigned int)0xFFFF)){
			__asm__ __volatile__("": : :"memory");
			if((dqm_tap_min != 32) && (dqm_tap_max == 32)){
				dqm_tap_max = dqm1 - 1;
				if((dqm_tap_max - dqm_tap_min) < MIN_WRITE_MASK_DELAY_WINDOW){
					dqm_tap_min =32;
					dqm_tap_max = 32;
				}
			}
//			printf("xx, ");
		}else{
			if(dqm_tap_min == 32)
				dqm_tap_min = dqm1;

			if((dqm0==31) && (dqm_tap_max == 32))
				dqm_tap_max = dqm1;
//			printf("%02d, ", dqm1);
		}

		__asm__ __volatile__("": : :"memory");
		*w_test_addr = 0x0;
	}
//	printf("\n");
//	printf("dqm_tap_max(%d), dqm_tap_min(%d)\n", dqm_tap_max, dqm_tap_min);
	if((dqm_tap_max - dqm_tap_min) < MIN_WRITE_MASK_DELAY_WINDOW)
		*dcdqmr = (*dcdqmr & 0xFF00FFFF);
	else
		*dcdqmr = (*dcdqmr & 0xFF00FFFF) | (((dqm_tap_max + dqm_tap_min)/2) << 16) ;
	

//	*dcdqmr = 0x0;
	_update_phy_param();

	return 0;
}

#if 1

/* Function Name: 
 * 	memctlc_dqs_calibration
 * Descripton:
 *	Calibration DQS delay tap for read/write operations.
 * Input:
 *	dram_type: MEMCTL_DRAM_TYPE_DDRI, MEMCTL_DRAM_TYPE_DDRII, MEMCTL_DRAM_TYPE_DDRIII
 * Output:
 * 	None
 * Return:
 *	MEMCTL_PASS: calibration pass.
 *	MEMCTL_FAIL: calibration fail.
 */
/*
 * Write tap  
 *31 |
 *30 |
 *29 |
 *28 |
 *27 |
 *26 |
 *25 |
 *24 |
 *23 |
 *22 |
 *21 |
 *20 |
 *19 |
 *18 |
 *17 |
 *16 |
 *15 |
 *14 |
 *13 |
 *12 |                                  B
 *11 |                                  ^
 *10 |                                  |
 * 9 |                                  |
 * 8 |                                  |
 * 7 |                                  |
 * 6 |                                  |--------------------X--------------------->C
 * 5 |                                  |
 * 4 |                                  |
 * 3 |                                  |
 * 2 |--------------------------------->A
 * 1 |-------------------------------------------------------------------------------------------------------------------
 * 0 |-------------------------------------------------------------------------------------------------------------------
 *   |____________________________________________________________________________________________________________________
 *    0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31 Read tap
 */

/* Function Name: 
 * 	_memctlc_set_c_w_tap
 * Descripton:
 *	Write r_r_tap value into the current write  delay tap of the target dq.
 * Input:
 *	dq 	: Target DQ.
 *	wtap	: The value to set the current write delay tap.
 * Output:
 * 	None.
 * Return:
 *	None.
 * Note:
 *	We should call _update_phy_param() to update the DDR phy parameters after
 *	configuring the read/write delay tap.
 */
void _memctlc_set_c_w_tap(unsigned int dq, unsigned int wtap)
{
	
	volatile unsigned int *dacqdqr;

	dacqdqr = (unsigned int *)DACDQ0RR;
	dacqdqr = dacqdqr + dq;

	if((dq > 15) || (wtap > 31))
		printf("%s, %d: input dq(%d), wtap(%d) error\n", __FUNCTION__, __LINE__, dq, wtap);
	else{
		*dacqdqr = ((*dacqdqr) & 0xE0FFFFFF) | (wtap << 24);
	}
	 _update_phy_param();
#ifdef MEMCTL_DDR_CALI_DEBUG_L2
	printf("%s, %d: ", __FUNCTION__, __LINE__);
	printf("dq(%d), wtap(0x%d), data(0x%08p)=0x%08x\n", dq, wtap, dacqdqr, *dacqdqr);
#endif
}



/* Function Name: 
 * 	_memctlc_set_c_r_r_tap
 * Descripton:
 *	Write r_r_tap value into the current read delay tap of the dq rising edge.
 * Input:
 *	dq 	: Target DQ.
 *	r_r_tap	: The setting value of the current read delay tap of the dq rising edge.
 * Output:
 * 	None.
 * Return:
 *	None.
 * Note:
 *	We should call _update_phy_param() to update the DDR phy parameters after
 *	configuring the read/write delay tap.
 */
void _memctlc_set_c_r_r_tap(unsigned int dq, unsigned int r_r_tap)
{
	volatile unsigned int *dacqdqr;

	dacqdqr = (unsigned int *)DACDQ0RR;
	dacqdqr = dacqdqr + dq;

	if((dq > 15) || (r_r_tap > 31))
		printf("%s, %d: input dq(%d), wtap(%d) error\n", __FUNCTION__, __LINE__, dq, r_r_tap);
	else{
		*dacqdqr = ((*dacqdqr) & 0xFFFFE0FF) | (r_r_tap << 8);
	}
	 _update_phy_param();
#ifdef MEMCTL_DDR_CALI_DEBUG_L2
	printf("%s, %d: ", __FUNCTION__, __LINE__);
	printf("dq(%d), r_r_tap(0x%d), data(0x%08p)=0x%08x\n", dq, r_r_tap, dacqdqr, *dacqdqr);
#endif
}


/* Function Name: 
 * 	_memctlc_set_c_r_f_tap
 * Descripton:
 *	Write r_f_tap value into the current read delay tap of the dq falling edge.
 * Input:
 *	dq 	: Target DQ.
 *	r_f_tap	: The setting value of the current read delay tap of the dq falling edge.
 * Output:
 * 	None.
 * Return:
 *	None.
 * Note:
 *	We should call _update_phy_param() to update the DDR phy parameters after
 *	configuring the read/write delay tap.
 */

void _memctlc_set_c_r_f_tap(unsigned int dq, unsigned int r_f_tap)
{
	
	volatile unsigned int *dacqdqf;

	dacqdqf = (unsigned int *)DACDQ0FR;
	dacqdqf = dacqdqf + dq;

	if((dq > 15) || (r_f_tap > 31))
		printf("%s, %d: input dq(%d), wtap(%d) error\n", __FUNCTION__, __LINE__, dq, r_f_tap);
	else{
		*dacqdqf = ((*dacqdqf) & 0xFFFFE0FF) | (r_f_tap << 8);
	}
	 _update_phy_param();
#ifdef MEMCTL_DDR_CALI_DEBUG_L2
	printf("%s, %d: ", __FUNCTION__, __LINE__);
	printf("dq(%d), r_f_tap(0x%d), data(0x%08p)=0x%08x\n", dq, r_f_tap, dacqdqf, *dacqdqf);
#endif

}


/* Function Name: 
 * 	_memctlc_extract_c_w_tap
 * Descripton:
 *	Extract the current write delay tap of the dq.
 * Input:
 *	dq  : Target DQ.
 * Output:
 * 	None.
 * Return:
 *	The current write delay tap of the dq.
 */
unsigned int _memctlc_extract_c_w_tap(unsigned int dq)
{
	volatile unsigned int *dacqdqr;

	dacqdqr = (unsigned int *)DACDQ0RR;
	dacqdqr = dacqdqr + dq;

#ifdef MEMCTL_DDR_CALI_DEBUG_L2
	printf("%s, %d: ", __FUNCTION__, __LINE__);
	printf("dq(%d), data(0x%08p)=0x%08x\n", dq, dacqdqr, *dacqdqr);
#endif
	if(dq > 15){
		printf("%s, %d: input dq(%d) error\n", __FUNCTION__, __LINE__, dq);
		return 0;
	}
	else{
		return (((unsigned int)(0x1F000000) & (*(dacqdqr))) >> 24);
	}

}


/* Function Name: 
 * 	_memctlc_extract_c_r_r_tap
 * Descripton:
 *	Extract the current read delay tap of the dq rising edge.
 * Input:
 *	dq  : Target DQ.
 * Output:
 * 	None.
 * Return:
 *	The current read delay tap of the dq rising edge.
 */
unsigned int _memctlc_extract_c_r_r_tap(unsigned int dq)
{
	volatile unsigned int *dacqdqr;

	dacqdqr = (unsigned int *)DACDQ0RR;
	dacqdqr = dacqdqr + dq;

#ifdef MEMCTL_DDR_CALI_DEBUG_L2
	printf("%s, %d: ", __FUNCTION__, __LINE__);
	printf("dq(%d), data(0x%08p)=0x%08x\n", dq, dacqdqr, *dacqdqr);
#endif
	if(dq > 15){
		printf("%s, %d: input dq(%d) error\n", __FUNCTION__, __LINE__, dq);
		return 0;
	}
	else{
		return (((unsigned int)(0x00001F00) & (*(dacqdqr))) >> 8);
	}


}


/* Function Name: 
 * 	_memctlc_extract_c_r_f_tap
 * Descripton:
 *	Extract the current read delay tap of the dq falling edge.
 * Input:
 *	dq  : Target DQ.
 * Output:
 * 	None.
 * Return:
 *	The current read delay tap of the dq falling edge.
 */
unsigned int _memctlc_extract_c_r_f_tap(unsigned int dq)
{
	volatile unsigned int *dacqdqf;

	dacqdqf = (unsigned int *)DACDQ0FR;
	dacqdqf = dacqdqf + dq;

#ifdef MEMCTL_DDR_CALI_DEBUG_L2
	printf("%s, %d: ", __FUNCTION__, __LINE__);
	printf("dq(%d), data(0x%08p)=0x%08x\n", dq, dacqdqf, *dacqdqf);
#endif
	if(dq > 15){
		printf("%s, %d: input dq(%d) error\n", __FUNCTION__, __LINE__, dq);
		return 0;
	}
	else{
		return (((unsigned int)(0x00001F00) & (*(dacqdqf))) >> 8);
	}

}


/* Function Name: 
 * 	_do_wfr_proc
 * Descripton:
 *	Do  write data into the target_addr location with word_size 
 *	data words, flush and burst read them back into Dcache.
 * Input:
 *	target_addr: The testing address.
 *	word_size  : The number of words to test.
 * Output:
 * 	None.
 * Return:
 *	None.
 */
void _do_wfr_proc(unsigned int target_addr, unsigned int word_size)
{
	volatile unsigned int *pSrc;
	volatile unsigned int *pattern_addr;
	volatile unsigned int data_tmp;
	unsigned int i, j;

#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d: ", __FUNCTION__, __LINE__);
	printf("target_addr(0x%x), word_size(0x%x)\n", target_addr, word_size);
#endif
	pSrc = (unsigned int *)target_addr;
	pattern_addr = (unsigned int *)((unsigned int)cali_pattern | (unsigned int)0x20000000);


	/* 
	 * Clear the test area. 
	 */
	j = 0;
	data_tmp = 0;
	for(i=0;i<word_size;i++){
		*(pSrc+i) = 0;
		j = (j + 1)%(sizeof(cali_pattern)/sizeof(unsigned int));
	}
	/* Flush and invalidate DCache */
	_flush_inv_dcache();


	j = 0;
	data_tmp = 0;
	/* Read and data into DCache */
	for(i=0;i<word_size;i++){
		//data_tmp = *(pSrc+i);
		data_tmp = data_tmp | *(pSrc+i);
		//*(pSrc+i) = cali_pattern[j];
		*(pSrc+i) = pattern_addr[j];
		j = (j + 1)%(sizeof(cali_pattern)/sizeof(unsigned int));
	}

	/* Flush and invalidate DCache */
	_flush_inv_dcache();

	/* Burst read back data into DCache */
	for(i=0;i<word_size;i= i + (DCACHE_LINE_SIZE/sizeof(unsigned int))){
		data_tmp = data_tmp | *(pSrc+i);
	}

	return;
}


unsigned int memctlc_count_delay_group_tap(void)
{
	return 0;
}




/* Function Name: 
 * 	_Inc_dqs_delay_tap_A
 * Descripton:
 *	None
 * Input:
 *	test_addr: The DRAM starting address for the calibration.
 *	word_size: The number of word size for the calibration.
 *	buswidth : DRAM bus width.
 *	dq 	 : The target dq bit.
 *	w_max	 : Maximum write delay tap value.
 *	w_min	 : Minimum write delay tap value.
 *	r_r_max	 : Maximum read delay tap value for DQS rising edge.
 *	r_r_min	 : Minimum read delay tap value for DQS rising edge.
 *	r_f_max	 : Maximum read delay tap value for DQS falling edge.
 *	r_f_min	 : Minimum read delay tap value for DQS falling edge.
 * Output:
 * 	None
 * Return:
 *	_MEMCTL_CALI_SATAUS_OK:
 *	_MEMCTL_CALI_PHASE_A_OVERFLOW:
 */
unsigned int _Inc_dqs_delay_tap_A(unsigned int target_bit, unsigned int buswidth, \
				  unsigned int w_max,     unsigned int w_min,  \
				  unsigned int r_r_max,   unsigned int r_r_min, \
                         	  unsigned int r_f_max,   unsigned int r_f_min )
{
	unsigned int dq, r_r_tap_dmask, r_f_tap_dmask, j;
	unsigned int w_delay_tap, r_r_delay_tap, r_f_delay_tap;
	volatile unsigned int *dacqdqr, *dacqdqf;	

#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d:\n", __FUNCTION__, __LINE__);
	printf("target_bit(0x%x), buswidth(%d) ", target_bit,  buswidth);
	printf("w_max(%02d), w_min(%02d), r_r_max(%02d), r_r_min(%02d), r_f_max(%02d), r_f_min(%02d)\n", w_max, w_min, r_r_max, r_r_min, r_f_max, r_f_min);
#endif
	dacqdqr = (unsigned int *)DACDQ0RR;
	dacqdqf = (unsigned int *)DACDQ0FR;

	for(dq = 0; dq < buswidth; dq++){
		/* Build up data mask for the dq. */
		r_r_tap_dmask = 0;
		r_f_tap_dmask = 0;
		j = 32;
		do{
			unsigned int tmp_r;
			unsigned int tmp_f;

			tmp_r = ((1 << dq) << buswidth);
			tmp_f = (1 << dq);

			r_r_tap_dmask = r_r_tap_dmask | tmp_r;
			r_f_tap_dmask = r_f_tap_dmask | tmp_f;

			j = j - buswidth;
			if(j > buswidth){
				r_r_tap_dmask = r_r_tap_dmask << (2*buswidth);
				r_f_tap_dmask = r_f_tap_dmask << (2*buswidth);
			}
		}while(j > buswidth);

		if((target_bit&r_r_tap_dmask) || (target_bit&r_f_tap_dmask)){
			/* extract read delay tap */
			w_delay_tap =   _memctlc_extract_c_w_tap(dq);
			r_r_delay_tap = _memctlc_extract_c_r_r_tap(dq);
			r_f_delay_tap = _memctlc_extract_c_r_f_tap(dq);
	
			/* The case required to carry into w_delay tap */
			if( ((target_bit & r_r_tap_dmask) && (r_r_delay_tap >= r_r_max)) || \
			    ((target_bit & r_f_tap_dmask) && (r_f_delay_tap >= r_f_max)) ){
				if(w_delay_tap >= w_max){
					return _MEMCTL_CALI_PHASE_A_OVERFLOW;
				}
				w_delay_tap++;
				r_r_delay_tap = r_r_min;
				r_f_delay_tap = r_f_min;
				_memctlc_set_c_w_tap(dq, w_delay_tap);
				_memctlc_set_c_r_r_tap(dq, r_r_delay_tap);
				_memctlc_set_c_r_f_tap(dq, r_f_delay_tap);

			}else{/* Only add read delay tap */
				if(target_bit & r_r_tap_dmask){
					r_r_delay_tap++;
					_memctlc_set_c_r_r_tap(dq, r_r_delay_tap);
				}

				if(target_bit & r_f_tap_dmask){
					r_f_delay_tap++;
					_memctlc_set_c_r_f_tap(dq, r_f_delay_tap);
				}
			}

#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d: dq(%d)\n", __FUNCTION__, __LINE__, dq);
#endif
		}
	}

	return _MEMCTL_CALI_SATAUS_OK;
}


/* Function Name: 
 * 	_memctlc_cali_phase_A
 * Descripton:
 *	Phase A of the DQS read delay tap for the input DQ.
 * Input:
 *	test_addr: The DRAM starting address for the calibration.
 *	word_size: The number of word size for the calibration.
 *	buswidth : DRAM bus width.
 *	dq 	 : The target dq bit.
 *	w_max	 : Maximum write delay tap value.
 *	w_min	 : Minimum write delay tap value.
 *	r_r_max	 : Maximum read delay tap value for DQS rising edge.
 *	r_r_min	 : Minimum read delay tap value for DQS rising edge.
 *	r_f_max	 : Maximum read delay tap value for DQS falling edge.
 *	r_f_min	 : Minimum read delay tap value for DQS falling edge.
 * Output:
 * 	None
 * Return:
 *	_MEMCTL_CALI_PHASE_A_SATAUS_OK:	Find the left down point of the window successfully.
 *	_MEMCTL_CALI_PHASE_A_OVERFLOW : No usable point in the input window.
 */
unsigned int _memctlc_cali_phase_A(unsigned int test_addr, unsigned int word_size, unsigned int buswidth, \
			 unsigned int dq, 	 unsigned int w_max, 	 unsigned int w_min, \
			 unsigned int r_r_max, 	 unsigned int r_r_min, \
			 unsigned int r_f_max,   unsigned int r_f_min )
{
	volatile unsigned int *pSrc;
	unsigned int r_r_tap_dmask, r_f_tap_dmask;
	unsigned int w_tap, r_tap_offset;
	unsigned int error_bit_mask;
	unsigned int error_bit_tmp;
	unsigned int i, j;

#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d:\n", __FUNCTION__, __LINE__);
	printf("test_addr(0x%x), word_size(0x%x), buswidth(%d), dq(%d) ", test_addr, word_size, buswidth, dq);
	printf("w_max(%02d), w_min(%02d), r_r_max(%02d), r_r_min(%02d), r_f_max(%02d), r_f_min(%02d)\n", w_max, w_min, r_r_max, r_r_min, r_f_max, r_f_min);
#endif
	/* Build up data mask for the dq. */
	r_r_tap_dmask = 0;
	r_f_tap_dmask = 0;
	j = 32;
	do{
		unsigned int tmp_r;
		unsigned int tmp_f;

		tmp_r = ((1 << dq) << buswidth);
		tmp_f = (1 << dq);

		r_r_tap_dmask = r_r_tap_dmask | tmp_r;
		r_f_tap_dmask = r_f_tap_dmask | tmp_f;

		j = j - buswidth;
		if(j > buswidth){
			r_r_tap_dmask = r_r_tap_dmask << (2*buswidth);
			r_f_tap_dmask = r_f_tap_dmask << (2*buswidth);
		}
	}while(j > buswidth);

	_memctlc_set_c_w_tap(dq, w_min);
	_memctlc_set_c_r_r_tap(dq, r_r_min);
	_memctlc_set_c_r_f_tap(dq, r_f_min);

	do{
		/* Configure the delay tap setting. */
		error_bit_mask = 0;

		_do_wfr_proc(test_addr, word_size);

		/* Now the data is already in DCache. */
		j = 0;
		pSrc = (unsigned int *)test_addr;
		for(i=0;i< word_size;i++){
			error_bit_tmp = (cali_pattern[j] & ~(*(pSrc+i)))|(*(pSrc+i) & (~cali_pattern[j]));
			error_bit_tmp = (error_bit_tmp & (r_r_tap_dmask)) | (error_bit_tmp & (r_f_tap_dmask));
			error_bit_mask = error_bit_mask | error_bit_tmp;
			if(error_bit_mask == (r_r_tap_dmask | r_f_tap_dmask))
				break;

			j = (j + 1)%(sizeof(cali_pattern)/sizeof(unsigned int));
		}


#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d: ", __FUNCTION__, __LINE__);
	printf("error_bit_mask = 0x%08x\n", error_bit_mask);
#endif
		if(error_bit_mask == 0)
			return _MEMCTL_CALI_PHASE_A_SATAUS_OK;
		else{
			if(_MEMCTL_CALI_PHASE_A_OVERFLOW == _Inc_dqs_delay_tap_A(error_bit_mask,\
			    buswidth,w_max, w_min, r_r_max, r_r_min, r_f_max, r_f_min)){
			return _MEMCTL_CALI_PHASE_A_OVERFLOW;
			}
		}
	}while(error_bit_mask != 0);

	return _MEMCTL_CALI_PHASE_A_OVERFLOW;
}


/* Function Name: 
 * 	_memctlc_cali_phase_B
 * Descripton:
 *	Phase B of the DQS write delay tap calibration of the input DQ.
 *	It adjusts the write delay tap and looking for the first
 *	write delay tap value.
 * Input:
 *	test_addr: The DRAM starting address for the calibration.
 *	word_size: The number of word size for the calibration.
 *	buswidth : DRAM bus width.
 *	dq 	 : The target dq bit.
 *	w_max	 : Maximum write delay tap value.
 *	w_min	 : Minimum write delay tap value.
 *	r_r_max	 : Maximum read delay tap value for DQS rising edge.
 *	r_r_min	 : Minimum read delay tap value for DQS rising edge.
 *	r_f_max	 : Maximum read delay tap value for DQS falling edge.
 *	r_f_min	 : Minimum read delay tap value for DQS falling edge.
 * Output:
 * 	None
 * Return:
 * 	None
 * Note:
 *	Stop when detecting error. The current write delay tap value
 * 	is the maximum usable value.
 */
void _memctlc_cali_phase_B(unsigned int test_addr, unsigned int word_size, unsigned int buswidth, \
			 unsigned int dq, 	 unsigned int w_max, 	 unsigned int w_min, \
			 unsigned int r_r_max, 	 unsigned int r_r_min, \
			 unsigned int r_f_max,   unsigned int r_f_min )
{
	volatile unsigned int *pSrc;
	unsigned int w_tap, w_tap_offset;
	unsigned int error_bit_mask;
	unsigned int error_bit_tmp;
	unsigned int r_r_tap_dmask, r_f_tap_dmask;
	unsigned int i, j;
	unsigned int wtap;

#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d:\n", __FUNCTION__, __LINE__);
	printf("test_addr(0x%x), word_size(0x%x), buswidth(%d), dq(%d), ", test_addr, word_size, buswidth, dq);
	printf("w_max(%02d), w_min(%02d), r_r_max(%02d), r_r_min(%02d), r_f_max(%02d), r_f_min(%02d)\n", w_max, w_min, r_r_max, r_r_min, r_f_max, r_f_min);
#endif
	/* Build up data mask for the dq. */
	r_r_tap_dmask = 0;
	r_f_tap_dmask = 0;
	j = 32;
	do{
		unsigned int tmp_r;
		unsigned int tmp_f;

		tmp_r = ((1 << dq) << buswidth);
		tmp_f = (1 << dq);

		r_r_tap_dmask = r_r_tap_dmask | tmp_r;
		r_f_tap_dmask = r_f_tap_dmask | tmp_f;

		j = j - buswidth;
		if(j > buswidth){
			r_r_tap_dmask = r_r_tap_dmask << (2*buswidth);
			r_f_tap_dmask = r_f_tap_dmask << (2*buswidth);
		}
	}while(j > buswidth);

	_memctlc_set_c_w_tap(dq, w_min+1);
	_memctlc_set_c_r_r_tap(dq, r_r_min);
	_memctlc_set_c_r_f_tap(dq, r_f_min);

	do{
		error_bit_mask = 0;
		_do_wfr_proc(test_addr, word_size);

		/* Now the data is already in DCache. */
		j = 0;
		pSrc = (unsigned int *)test_addr;
		for(i=0;i< word_size;i++){
			error_bit_tmp = (cali_pattern[j] & ~(*(pSrc+i)))|(*(pSrc+i) & (~cali_pattern[j]));
			error_bit_mask = (error_bit_tmp & (r_r_tap_dmask)) | (error_bit_tmp & (r_f_tap_dmask));
			if(error_bit_mask != 0)
				break;
//	if(_memctlc_extract_c_w_tap(dq) > 8)
//		printf("cali_pattern[%d] = 0x%08x, *(pSrc+%d) = 0x%08x\n", j, cali_pattern[j], i, *(pSrc+i));
			j = (j + 1)%(sizeof(cali_pattern)/sizeof(unsigned int));
		}

#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d: ", __FUNCTION__, __LINE__);
	printf("error_bit_mask = 0x%08x\n", error_bit_mask);
#endif

		/* Adjust write delay tap. */
		wtap = _memctlc_extract_c_w_tap(dq);
		if(error_bit_mask != 0){
			_memctlc_set_c_w_tap(dq, wtap-1);
			return;
			
		}
		else{
			if(wtap >= w_max)
				return;
			_memctlc_set_c_w_tap(dq, wtap+1);
		}
	}while(error_bit_mask == 0);

}


/* Function Name: 
 * 	_memctlc_cali_phase_C
 * Descripton:
 *	Phase C of the DQS read delay tap calibration of the input DQ.
 *	It adjusts the read delay tap and looking for the last
 *	usable read delay tap value.
 * Input:
 *	test_addr: The DRAM starting address for the calibration.
 *	word_size: The number of word size for the calibration.
 *	buswidth : DRAM bus width.
 *	dq 	 : The target dq bit.
 *	c_w_tap	 : The current write delay tap value.
 *	r_r_max	 : Maximum read delay tap value for DQS rising edge.
 *	r_r_min	 : Minimum read delay tap value for DQS rising edge.
 *	r_f_max	 : Maximum read delay tap value for DQS falling edge.
 *	r_f_min	 : Minimum read delay tap value for DQS falling edge.
 * Output:
 * 	None
 * Return:
 * 	None
 * Note:
 *	Stop when detecting error. The current read delay tap value
 * 	is the maximum usable read delay tap value.
 */
void _memctlc_cali_phase_C(unsigned int test_addr, unsigned int word_size, unsigned int buswidth, \
			 unsigned int dq,  	 unsigned int c_w_tap, \
			 unsigned int r_r_max, 	 unsigned int r_r_min, \
			 unsigned int r_f_max,   unsigned int r_f_min )
{
	volatile unsigned int *pSrc;
	unsigned int w_tap, w_tap_offset;
	unsigned int error_bit_mask;
	unsigned int error_bit_tmp;
	unsigned int r_r_tap_dmask, r_f_tap_dmask;
	unsigned int i, j;
	unsigned int r_r_tap, r_f_tap;
	unsigned int r_r_tap_done, r_f_tap_done;
	volatile unsigned int *pattern_addr;

#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d:\n", __FUNCTION__, __LINE__);
	printf("test_addr(0x%x), word_size(0x%x), buswidth(%d), dq(%d), ", test_addr, word_size, buswidth, dq);
	printf("c_w_tap(%02d), r_r_max(%02d), r_r_min(%02d), r_f_max(%02d), r_f_min(%02d)\n", c_w_tap, r_r_max, r_r_min, r_f_max, r_f_min);
#endif
	/* Build up data mask for the dq. */
	r_r_tap_dmask = 0;
	r_f_tap_dmask = 0;
	r_r_tap_done = 0;
	r_f_tap_done = 0;
	j = 32;
	do{
		unsigned int tmp_r;
		unsigned int tmp_f;

		tmp_r = ((1 << dq) << buswidth);
		tmp_f = (1 << dq);

		r_r_tap_dmask = r_r_tap_dmask | tmp_r;
		r_f_tap_dmask = r_f_tap_dmask | tmp_f;

		j = j - buswidth;
		if(j > buswidth){
			r_r_tap_dmask = r_r_tap_dmask << (2*buswidth);
			r_f_tap_dmask = r_f_tap_dmask << (2*buswidth);
		}
	}while(j > buswidth);

	_memctlc_set_c_w_tap(dq, c_w_tap);
	_memctlc_set_c_r_r_tap(dq, r_r_min);
	_memctlc_set_c_r_f_tap(dq, r_f_min);

	do{
		error_bit_mask = 0;
		_do_wfr_proc(test_addr, word_size);

		/* Now the data is already in DCache. */
		j = 0;
		pSrc = (unsigned int *)test_addr;
		pattern_addr = (unsigned int *)((unsigned int)cali_pattern | (unsigned int)0x20000000);
		for(i=0;i< word_size;i++){
			//error_bit_tmp = (cali_pattern[j] & ~(*(pSrc+i)))|(*(pSrc+i) & (~cali_pattern[j]));
			error_bit_tmp = (pattern_addr[j] & ~(*(pSrc+i)))|(*(pSrc+i) & (~pattern_addr[j]));
			error_bit_mask = (error_bit_tmp & (r_r_tap_dmask)) | (error_bit_tmp & (r_f_tap_dmask));
			if((error_bit_mask & (r_r_tap_dmask)) && (error_bit_mask & (r_f_tap_dmask)))
				break;

			j = (j + 1)%(sizeof(cali_pattern)/sizeof(unsigned int));
		}

		r_r_tap = _memctlc_extract_c_r_r_tap(dq);
		r_f_tap = _memctlc_extract_c_r_f_tap(dq);
//printf("%s, %d: r_r_tap(%d), r_r_max(%d), _rf_tap(%d), r_f_max(%d), error_bit_mask(0x%08x), r_r_tap_dmask(0x%08x), r_f_tap_dmask(0x%08x)\n",\
		 __FUNCTION__, __LINE__, r_r_tap, r_r_max, r_f_tap, r_f_max, error_bit_mask, r_r_tap_dmask, r_f_tap_dmask);

		if(((error_bit_mask & r_r_tap_dmask) != 0) && ((error_bit_mask & r_f_tap_dmask) != 0)){
			_memctlc_set_c_r_r_tap(dq, r_r_tap-1);
			_memctlc_set_c_r_f_tap(dq, r_f_tap-1);
			return;

		}
		else if((error_bit_mask & r_r_tap_dmask) != 0){
			r_r_tap_done = 1;
			if(r_f_tap<r_f_max){
				if(r_f_tap_done == 0)
					_memctlc_set_c_r_f_tap(dq, r_f_tap+1);
			}else{
				_memctlc_set_c_r_r_tap(dq, r_r_tap-1);
				return;
			}
			
		}
		else if((error_bit_mask & r_f_tap_dmask) != 0){
			r_f_tap_done = 1;
			if(r_r_tap<r_r_max){
				if(r_r_tap_done == 0)
					_memctlc_set_c_r_r_tap(dq, r_r_tap+1);
			}else{
				_memctlc_set_c_r_f_tap(dq, r_f_tap-1);
				return;
			}
		}
		else{
			if((r_r_tap >= r_r_max) && (r_f_tap >= r_f_max)){
				return;
			}else if(r_r_tap >= r_r_max){
				if(r_f_tap_done == 0)
					_memctlc_set_c_r_f_tap(dq, r_f_tap+1);
				else
					return;
			}else if(r_f_tap >= r_f_max){
				if(r_r_tap_done == 0)
					_memctlc_set_c_r_r_tap(dq, r_r_tap+1);
				else
					return;
			}else{
				if(r_f_tap_done == 0)
					_memctlc_set_c_r_f_tap(dq, r_f_tap+1);
				if(r_r_tap_done == 0)
					_memctlc_set_c_r_r_tap(dq, r_r_tap+1);
			}
		}
		if((r_f_tap_done != 0) && (r_r_tap_done != 0))
			return;
	}while(1);

}

/*
 * Write tap  
 *31 |
 *30 |                                                             
 *29 |                                                             B4 
 *28 |                                                             ^
 *27 |                                                             |
 *26 |                                                             |
 *25 |                                                             |
 *24 |                                                             |----------------------X----------------->C4
 *23 |                                                             |
 *22 |                                                             |
 *21 |                                                             |
 *20 |                                                             |
 *19 |------------------------------------------------------------>A4
 *18 |-------------------------------------------------------------------------------------------------------------------
 *17 |
 *16 |                                               B2                          B3
 *15 |                                               ^Read delay                 ^
 *14 |                                               |too small,                 |
 *13 |                                               |search ahead.              |
 *12 |                                               |---------->C2              |-------------------------->C3
 *11 |                                               |                           |
 *10 |                                               |                           |
 * 9 |                                               |              =============>A3               
 * 8 |---------------------------------------------->A2             ====================================================                        
 * 7 |-------------------------------------------------------------------------------------------------------------------                                  
 * 6 |-------------------------------------------------------------------------------------------------------------------
 * 5 |                                  B1xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 * 4 |                                  ^ xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 * 3 |                                  | xWrite delay tap window is too small, we don't search ahead for read delay tap.
 * 2 |--------------------------------->A1xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 * 1 |-------------------------------------------------------------------------------------------------------------------
 * 0 |-------------------------------------------------------------------------------------------------------------------
 *   |____________________________________________________________________________________________________________________
 *    0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31 Read tap
 */

/* Function Name: 
 * 	_memctlc_ddr_cali_dq
 * Descripton:
 *	calibrate DQS delay tap window for the input DQ.
 * Input:
 *	test_addr: The DRAM starting address for the calibration.
 *	word_size: The number of word size for the calibration.
 *	buswidth : DRAM bus width.
 *	dq 	 : The target dq bit.
 *	w_max	 : Maximum write delay tap value.
 *	w_min	 : Minimum write delay tap value.
 *	r_r_max	 : Maximum read delay tap value for DQS rising edge.
 *	r_r_min	 : Minimum read delay tap value for DQS rising edge.
 *	r_f_max	 : Maximum read delay tap value for DQS falling edge.
 *	r_f_min	 : Minimum read delay tap value for DQS falling edge.
 * Output:
 * 	None
 * Return:
 *	_MEMCTL_CALI_PASS	     : 
 *	_MEMCTL_CALI_PHASE_A_OVERFLOW:
 *	_MEMCTL_CALI_PHASE_B_OVERFLOW:
 *	_MEMCTL_CALI_PHASE_C_OVERFLOW:
 *	_MEMCTL_CALI_STATUS_UNKNOWN  :
 */
int _memctlc_ddr_cali_dq(unsigned int test_addr, unsigned int word_size, unsigned int buswidth, \
			 unsigned int dq, 	 unsigned int w_max, 	 unsigned int w_min, \
			 unsigned int r_r_max, 	 unsigned int r_r_min, \
			 unsigned int r_f_max,   unsigned int r_f_min )
{

	unsigned int c_w_min, c_r_r_min, c_r_f_min, c_w_max, c_r_r_max, c_r_f_max;
	unsigned int retv;
	unsigned int cali_status;

#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d:\n", __FUNCTION__, __LINE__);
	printf("test_addr(0x%x), word_size(0x%x), buswidth(%d), dq(%d), ", test_addr, word_size, buswidth, dq);
	printf("w_max(%02d), w_min(%02d), r_r_max(%02d), r_r_min(%02d), r_f_max(%02d), r_f_min(%02d)\n", w_max, w_min, r_r_max, r_r_min, r_f_max, r_f_min);
#endif

	/* Configure the write tap and read tap of the dq to w_min and r_min. */
	cali_status = _MEMCTL_CALI_STATUS_LOOKING_A;
	while(cali_status!= _MEMCTL_CALI_SATAUS_DONE){
		switch (cali_status) {
			/* Phase A */
			case _MEMCTL_CALI_STATUS_LOOKING_A:
#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d: _MEMCTL_CALI_STATUS_LOOKING_A\n", __FUNCTION__, __LINE__);
#endif		
				retv = _memctlc_cali_phase_A(test_addr, word_size, buswidth, dq, w_max, w_min, r_r_max, r_r_min, r_f_max, r_f_min);
				if(retv == _MEMCTL_CALI_PHASE_A_SATAUS_OK)
					cali_status = _MEMCTL_CALI_STATUS_LOOKING_B;
				else
					return retv;

				break;

			/* Phase B */
			case _MEMCTL_CALI_STATUS_LOOKING_B:
#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d: _MEMCTL_CALI_STATUS_LOOKING_B\n", __FUNCTION__, __LINE__);
#endif		
				c_w_min = _memctlc_extract_c_w_tap(dq);
				c_r_r_min = _memctlc_extract_c_r_r_tap(dq);
				c_r_f_min = _memctlc_extract_c_r_f_tap(dq);
				if( ((r_r_max - c_r_r_min) < MIN_READ_DELAY_WINDOW) || ((r_f_max - c_r_f_min) < MIN_READ_DELAY_WINDOW) )
					return _memctlc_ddr_cali_dq(test_addr, word_size, buswidth, dq, w_max, (c_w_min+1), r_r_max, r_r_min, r_f_max, r_f_min);

				_memctlc_cali_phase_B(test_addr, word_size, buswidth, dq, w_max, w_min, r_r_max, r_r_min, r_f_max, r_f_min);

				c_w_max = _memctlc_extract_c_w_tap(dq);

				/* B1 case */
				if((c_w_max - c_w_min) < MIN_WRITE_DELAY_WINDOW){
					if( (c_w_max >= w_max) | ((w_max - (c_w_max+1)) < MIN_WRITE_DELAY_WINDOW) ){
						return _MEMCTL_CALI_PHASE_B_OVERFLOW;
					}
					else{ /* if the left write delay tap is big enough, searching for A2 */
						return _memctlc_ddr_cali_dq(test_addr, word_size, buswidth, dq, w_max, (c_w_max+1), r_r_max, r_r_min, r_f_max, r_f_min);
					}
				}

				cali_status = _MEMCTL_CALI_STATUS_LOOKING_C;
				break;

			/*
			 * Phase C 
			 */
			case _MEMCTL_CALI_STATUS_LOOKING_C:
#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d: _MEMCTL_CALI_STATUS_LOOKING_C\n", __FUNCTION__, __LINE__);
#endif		
				/* Setting write tap to the center of the usable range. */

				_memctlc_cali_phase_C(test_addr, word_size, buswidth, dq, (c_w_max+c_w_min)/2, r_r_max, r_r_min, r_f_max, r_f_min);

				c_r_r_max = _memctlc_extract_c_r_r_tap(dq);
				c_r_f_max = _memctlc_extract_c_r_f_tap(dq);

//printf("%s, %d: c_r_r_max = %d, c_r_f_max = %d\n", __FUNCTION__, __LINE__, c_r_r_max, c_r_f_max);
				/* 
				 * C2 case: Read delay tap window too samll 
				 */
				/* If the left read delay tap window is bigger enough, searching for A3 */
				if(((c_r_r_max - c_r_r_min) < MIN_READ_DELAY_WINDOW) && \
				   ((c_r_f_max - c_r_f_min) < MIN_READ_DELAY_WINDOW)) {
					if(((r_f_max - (c_r_f_max+1)) >= MIN_READ_DELAY_WINDOW)&& \
					    ((r_r_max - (c_r_r_max+1)) >= MIN_READ_DELAY_WINDOW)){
					   	/* Adjust both rising and falling dq delay tap. */
						if(_MEMCTL_CALI_PASS == \
							_memctlc_ddr_cali_dq(test_addr, word_size, buswidth, dq, c_w_max, \
									c_w_min, r_r_max, (c_r_r_max+1), r_f_max, (c_r_f_max+1))){
							return _MEMCTL_CALI_PASS;
						}else{
							return _memctlc_ddr_cali_dq(test_addr, word_size, buswidth, dq, \
									w_max, (c_w_max+1), r_r_max, r_r_min, r_f_max, r_f_min);
						}
					}else{
						return _memctlc_ddr_cali_dq(test_addr, word_size, buswidth, dq, \
									w_max, (c_w_max+1), r_r_max, r_r_min, r_f_max, r_f_min);
					}
				}else if((c_r_r_max - c_r_r_min) < MIN_READ_DELAY_WINDOW){
				   	/* Adjust rising dq delay tap to (c_r_r_max+1) and reset falling dq delay tap to c_r_f_min. */
					if(_MEMCTL_CALI_PASS == \
								_memctlc_ddr_cali_dq(test_addr, word_size, buswidth, dq, c_w_max, \
								c_w_min, r_r_max, (c_r_r_max+1), r_f_max, c_r_f_min)){
						return _MEMCTL_CALI_PASS;
					}else{
						if((w_max -(c_w_max+1)) > MIN_WRITE_DELAY_WINDOW){
							return _memctlc_ddr_cali_dq(test_addr, word_size, buswidth, dq, \
								w_max, (c_w_max+1), r_r_max, r_r_min, r_f_max, r_f_min);
						}
						else{
							return _MEMCTL_CALI_PHASE_C_OVERFLOW;
						}
					}

				}else if((c_r_f_max - c_r_f_min) < MIN_READ_DELAY_WINDOW){
				   	/* Adjust falling dq delay tap to (c_r_f_min+1) and reset falling dq delay tap. c_r_r_min. */
					if(_MEMCTL_CALI_PASS == \
								_memctlc_ddr_cali_dq(test_addr, word_size, buswidth, dq, c_w_max, \
								c_w_min, r_r_max, c_r_r_min, r_f_max, (c_r_f_max+1))){
						return _MEMCTL_CALI_PASS;
					}else{
						if((w_max -(c_w_max+1)) > MIN_WRITE_DELAY_WINDOW){
							return _memctlc_ddr_cali_dq(test_addr, word_size, buswidth, dq, \
								w_max, (c_w_max+1), r_r_max, r_r_min, r_f_max, r_f_min);
						}
						else{
							return _MEMCTL_CALI_PHASE_C_OVERFLOW;
						}
					}
				
				}else{
					/* Find a valid window */
					_memctlc_set_c_r_r_tap(dq, (c_r_r_max+c_r_r_min)/2);
					_memctlc_set_c_r_f_tap(dq, (c_r_f_max+c_r_f_min)/2);
					return _MEMCTL_CALI_PASS;
				}
				
				break;

			defaut:	
				printf("function %s, line %d: Unknown calibration status\n", __FUNCTION__, __LINE__);
				return _MEMCTL_CALI_STATUS_UNKNOWN;
		}

	}

	/* impossible situation */
	return _MEMCTL_CALI_STATUS_UNKNOWN;
}

/* Function Name: 
 * 	_memctlc_cali_phase_A
 * Descripton:
 *	
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	.
 */
void _memctlc_show_error_status(unsigned int error_status)
{
	switch (error_status){

		case _MEMCTL_CALI_STATUS_LOOKING_A:
			printf("_MEMCTL_CALI_STATUS_LOOKING_A ");
			break;

		case _MEMCTL_CALI_STATUS_LOOKING_B:
			printf("_MEMCTL_CALI_STATUS_LOOKING_B");
			break;

		case _MEMCTL_CALI_STATUS_LOOKING_C:
			printf("_MEMCTL_CALI_STATUS_LOOKING_C");
			break;

		case _MEMCTL_CALI_SATAUS_DONE:
			printf("_MEMCTL_CALI_SATAUS_DONE");
			break;

		case _MEMCTL_CALI_SATAUS_FAIL:
			printf("_MEMCTL_CALI_SATAUS_FAIL");
			break;

		case _MEMCTL_CALI_SATAUS_OVERFLOW:
			printf("_MEMCTL_CALI_SATAUS_OVERFLOW");
			break;

		case _MEMCTL_CALI_SATAUS_OK:
			printf("_MEMCTL_CALI_SATAUS_OK");
			break;

		case _MEMCTL_CALI_PHASE_A_OVERFLOW:
			printf("_MEMCTL_CALI_PHASE_A_OVERFLOW");
			break;

		case _MEMCTL_CALI_PHASE_B_OVERFLOW:
			printf("_MEMCTL_CALI_PHASE_B_OVERFLOW");
			break;

		case _MEMCTL_CALI_PHASE_C_OVERFLOW:
			printf("_MEMCTL_CALI_PHASE_C_OVERFLOW");
			break;

		case _MEMCTL_CALI_STATUS_UNKNOWN:
			printf("_MEMCTL_CALI_STATUS_UNKNOWN");
			break;

		default:
			printf("unknown status number \n");
			break;
	}
}


/* Function Name: 
 * 	memctlc_dqs_calibration
 * Descripton:
 *	
 * Input:
 *	dram_type:
 *	ac_mode	 :
 *	dqs_mode :
 *	buswidth :
 *	test_addr:
 *	word_size:
 * Output:
 * 	None
 * Return:
 *	MEMCTL_CALI_PASS:
 *	MEMCTL_CALI_FAIL:
 */


int memctlc_dqs_calibration(unsigned int dram_type, unsigned int ac_mode,\
			    unsigned int dqs_mode, unsigned int buswidth,\
			    unsigned int test_addr, unsigned int word_size)
{
	volatile unsigned int *dacqdqr;
	volatile unsigned int *dacqdqf;
	volatile unsigned int *dacspcr;
	volatile unsigned int *daccr;
	volatile unsigned int reg_tmp;
	unsigned int cali_status;
	unsigned int dq, i;
	unsigned int reg_array[32];
#ifdef MEMCTL_DDR_CALI_DEBUG
	printf("%s, %d: start DDR calibration\n", __FUNCTION__, __LINE__);
#endif 
	/* 
	 * Initialization calibration registers 
	 */
	/* 0 Assign address variable */
	daccr   = (unsigned int *)DACCR;    /* DDR Auto-Calibration Configuration Register */
	dacqdqr = (unsigned int *)DACDQ0RR; /* DDR Auot-Calibration for DQS0 Rising edge on DQ0 Register */
	dacqdqf = (unsigned int *)DACDQ0FR; /* DDR Auot-Calibration for DQS0 Falling edge on DQ0 Register  */
	dacspcr = (unsigned int *)DACSPCR;  /* Silence Pattern Control Register */

	reg_tmp = 0;
	/* 1 DQS signal mode. Only configurable when DRAM_TYPE == DDR2 */
	if(MEMCTL_DDR_DQS_SE == dqs_mode){/* Single end */
		reg_tmp = reg_tmp | (unsigned int)MEMCTL_DACCR_DQS_MODE_SE;
	}else{ 			   /* Differential */
		reg_tmp = reg_tmp | (unsigned int)MEMCTL_DACCR_DQS_MODE_DIF;
	}

	/* 2 Configure calibration mode */
	if(MEMCTL_DDR_DLL_MODE == ac_mode){/* Analog DLL mode */
		reg_tmp = reg_tmp | (unsigned int)MEMCTL_DACCR_AC_MODE_DLL;
	}else{ 			    /* Digital Delay Line */
		reg_tmp = reg_tmp | (unsigned int)MEMCTL_DACCR_AC_MODE_DIG;
		/* Configure a proper delay tap group. Valid only in digital delay line mode. */
		/* We need to refer the memory clock for choosing the delay tap group. */
		reg_tmp = reg_tmp | (memctlc_count_delay_group_tap() << MEMCTL_DACCR_DQS0_GRUP_TAP_SHIFT);
		reg_tmp = reg_tmp | (memctlc_count_delay_group_tap() << MEMCTL_DACCR_DQS1_GRUP_TAP_SHIFT);
	}

	*daccr = reg_tmp;

	/* 3 Disable Silence Pattern Generation. */	
	*dacspcr = *dacspcr & ((unsigned int)MEMCTL_DACSPCR_PERIOD_EN_MASK);

	/* 4 Clear all DQx delay tap setting */
	for(i=0; i<16; i++){
		*(dacqdqr+i) = (unsigned int)0;
		*(dacqdqf+i) = (unsigned int)0;
	}
	_update_phy_param();

	for(dq = 0; dq < buswidth; dq++){
		cali_status = _memctlc_ddr_cali_dq(test_addr, word_size, buswidth, dq, 31, 0, 31, 0, 31, 0);
		if(cali_status != _MEMCTL_CALI_PASS){
			_memctlc_show_error_status(cali_status);
			printf("%s, %d: Error DDR DQS calibration fail on DQ(%d)\n", __FUNCTION__, __LINE__, dq);
			return MEMCTL_CALI_FAIL;
		}
		//else
		//	printf("dq(%d) passed\n", dq);
	}

	return MEMCTL_CALI_PASS;
}

/* Function Name: 
 * 	memctlc_dqs_calibration
 * Descripton:
 *	
 * Input:
 *	dram_type:
 *	ac_mode	 :
 *	dqs_mode :
 *	buswidth :
 *	test_addr:
 *	word_size:
 * Output:
 * 	None
 * Return:
 *	MEMCTL_CALI_PASS:
 *	MEMCTL_CALI_FAIL:
 */

#define MEMCTL_AUTO_CALI_NOT_FOUND (32)
#define MEMCTL_DACSPCR_AC_SILEN_PERIOD_EN (1<<31)
#define MEMCTL_DACSPCR_AC_SILEN_TRIG (1<<20)
#define MEMCTL_DACDQ_DQ_AC_EN	     (1<<31)

int memctlc_hw_auto_calibration( unsigned int buswidth, unsigned int test_addr)
{
	volatile unsigned int *dacqdq;
	volatile unsigned int *dacspcr;
	volatile unsigned int *dacspar;
	volatile unsigned int *daccr;
	volatile unsigned int wait_time;
	unsigned int *sil_pat_addr;
	unsigned int i, ret_value, tap_offset;

	unsigned int delay_tap_reg_bk[32];
	unsigned int delay_tap_max[32];
	unsigned int delay_tap_min[32];
	unsigned int target_delay_tap[32];
	unsigned int delay_tap_cur;

	/* 
	 * 0. Backup delay tap setting 
	 */
	ret_value = MEMCTL_CALI_PASS;
	daccr   = (unsigned int *)DACCR;    /* DDR Auto-Calibration Configuration Register */
	dacqdq  = (unsigned int *)DACDQ0RR; /* DDR Auot-Calibration for DQS0 Rising edge on DQ0 Register */
	dacspcr = (unsigned int *)DACSPCR;  /* Silence Pattern Control Register */
	dacspar = (unsigned int *)DACSPAR;  /* Silence Pattern Address Register */


	for(i = 0; i < 32 ; i++){
		delay_tap_reg_bk[i] = *(dacqdq+i);
		target_delay_tap[i] = delay_tap_reg_bk[i];
		delay_tap_max[i] = MEMCTL_AUTO_CALI_NOT_FOUND;
		delay_tap_min[i] = MEMCTL_AUTO_CALI_NOT_FOUND;
	}
	delay_tap_cur = 0;

#if 0
	printf("calibration for current delay tap:\n");
	for(i = 0; i < 32 ; i++){
		printf("delay_tap_max[%d] = %d, delay_tap_min[%d] = %d\n", i, (delay_tap_reg_bk[i] & 0x00FF0000) >> 16, i, (delay_tap_reg_bk[i] & 0x000000FF));
	}
#endif
	/* set to the minimun usable value. */
	for(i = 0; i < 32 ; i++){
		target_delay_tap[i] = (target_delay_tap[i] & 0xFFFF00FF) | ((target_delay_tap[i] & 0x000000FF) << 8);
	}

	/* 
	 * 1. Configure the silence pattern and control register 
	 */
	sil_pat_addr = (unsigned int *)test_addr;
	for(i=0; i < sizeof(cali_pattern)/sizeof(unsigned int);i++){
		*(sil_pat_addr+i) = cali_pattern[i];
	}
	*dacspcr = ((sizeof(cali_pattern)/sizeof(unsigned int)) - 1) & (0xFF);
	*dacspar = test_addr;

	/* 
	 * 2. Searching max delay tap window. 
	 */

	/* 2.1 Minimum tap value for the Maximum delay tap. */
	for(delay_tap_cur=0; delay_tap_cur < 32; delay_tap_cur++){
		/* Initialize the maximum delay tap*/
		for(i = 0; i < 32 ; i++){
			if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
				*(dacqdq+i) = *(dacqdq+i) & 0xFF00FFFF | (delay_tap_cur << 16);
		}
		_update_phy_param();

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);
	
		/* Enable the HW auto-calibration update mechanism. */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
				*(dacqdq+i) = *(dacqdq+i) | MEMCTL_DACDQ_DQ_AC_EN;
		}

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);

		/* Check the maximun delay tap value */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
				/* find minmun tap of the maximun delay tap. */
				if( ((*(dacqdq+i) & 0x00FF0000) >> 16) > delay_tap_cur ){
					delay_tap_min[i] = delay_tap_cur;
				}
				/* Disable the HW auto-calibration update mechanism.*/
				*(dacqdq+i) = *(dacqdq+i) & (~MEMCTL_DACDQ_DQ_AC_EN);
			}
		}

	}

	/* Check wheather there is failured DQ. */
	for(i = 0; i < 32 ; i++){
		if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
			printf("%s, %d: Fail to calibraton for HW auto-calibration. Register address(0x%08x)\n",\
				 __FUNCTION__, __LINE__, dacqdq+i);
			ret_value = MEMCTL_CALI_FAIL;
		}
		if(ret_value == MEMCTL_CALI_FAIL)
			goto go_out;
	}

	/* Initialize the maximum delay tap*/
	for(i = 0; i < 32 ; i++){
		if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
			*(dacqdq+i) = (delay_tap_reg_bk[i] & 0xFF00FFFF) | delay_tap_min[i];
	}
	_update_phy_param();

	/* 2.2 Maximum tap value for the Maximum delay tap. */
	for(tap_offset=0; tap_offset < 32; tap_offset++){
		/* Initialize the maximum delay tap*/
		for(i = 0; i < 32 ; i++){
			if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
				if ((tap_offset+delay_tap_min[i]) < 32)
					*(dacqdq+i) = (delay_tap_reg_bk[i] & 0xFF00FFFF) | ((delay_tap_min[i] + tap_offset) << 16);
				else
					delay_tap_max[i] = 31;
			}
		}
		_update_phy_param();

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);
	
		/* Enable the HW auto-calibration update mechanism. */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
				*(dacqdq+i) = *(dacqdq+i) | MEMCTL_DACDQ_DQ_AC_EN;
		}

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);

		/* Check the maximun delay tap value */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
				/* find maximum tap of the maximun delay tap. */
				if( ((*(dacqdq+i) & 0x00FF0000) >> 16) == (tap_offset+delay_tap_min[i]) ){
					delay_tap_max[i] = tap_offset+delay_tap_min[i]-1;
				}
				/* Disable the HW auto-calibration update mechanism.*/
				*(dacqdq+i) = *(dacqdq+i) & (~MEMCTL_DACDQ_DQ_AC_EN);
			}
		}

	}

	/* Check wheather there is failured DQ. */
	for(i = 0; i < 32 ; i++){
		if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
			printf("%s, %d: Fail to calibraton for HW auto-calibration. Register address(0x%08x)\n",\
				 __FUNCTION__, __LINE__, dacqdq+i);
			ret_value = MEMCTL_CALI_FAIL;
		}
		if(ret_value == MEMCTL_CALI_FAIL)
			goto go_out;
	}

#if 0
	printf("calibration for maximum delay tap:\n");
	for(i=0;i<32;i++){
		printf("delay_tap_max[%d] = %d, delay_tap_min[%d] = %d\n", i, delay_tap_max[i], i, delay_tap_min[i]);
	}
#endif

	/* Commit (max+1) to the register back up array */
	for(i=0;i<32;i++){
		delay_tap_reg_bk[i] = (delay_tap_reg_bk[i] & 0xFF00FFFF) | ((delay_tap_max[i] + 1) << 16);
	}

	for(i=0;i<32;i++){
		target_delay_tap[i] = (target_delay_tap[i] & 0xFF00FFFF) | ((delay_tap_max[i]-2) << 16) ;
	}



	/*
	 * 3. Searching min delay tap window. 
	 */
	/* 3.1 Minimum tap value for the Maximum delay tap. */
	for(delay_tap_cur=1; delay_tap_cur < 32; delay_tap_cur++){ /* start from 1 */
		/* Initialize the maximum delay tap*/
		for(i = 0; i < 32 ; i++){
			if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
				*(dacqdq+i) = *(dacqdq+i) & 0xFFFFFF00 | (delay_tap_cur);
		}
		_update_phy_param();

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);
	
		/* Enable the HW auto-calibration update mechanism. */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
				*(dacqdq+i) = *(dacqdq+i) | MEMCTL_DACDQ_DQ_AC_EN;
		}

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);

		/* Check the maximun delay tap value */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
				/* find minmun tap of the maximun delay tap. */
				if( (*(dacqdq+i) & 0x000000FF) < delay_tap_cur ){
					delay_tap_min[i] = delay_tap_cur;
				}
				/* Disable the HW auto-calibration update mechanism.*/
				*(dacqdq+i) = *(dacqdq+i) & (~MEMCTL_DACDQ_DQ_AC_EN);
			}
		}
	}


	/* Check wheather there is failured DQ. */
	for(i = 0; i < 32 ; i++){
		if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
			printf("%s, %d: Fail to calibraton for HW auto-calibration. Register address(0x%08x)\n",\
				 __FUNCTION__, __LINE__, dacqdq+i);
			ret_value = MEMCTL_CALI_FAIL;
		}
		if(ret_value == MEMCTL_CALI_FAIL)
			goto go_out;
	}


	/* Initialize the minimum delay tap*/
	for(i = 0; i < 32 ; i++){
		if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
			*(dacqdq+i) = (delay_tap_reg_bk[i] & 0xFFFFFF00) | delay_tap_min[i];
	}
	_update_phy_param();

	/* 3.2 Maximum tap value for the Maximum delay tap. */
	for(tap_offset=0; tap_offset < 32; tap_offset++){
		/* Initialize the maximum delay tap*/
		for(i = 0; i < 32 ; i++){
			if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
				if ((tap_offset+delay_tap_min[i]) < 32)
					*(dacqdq+i) = (delay_tap_reg_bk[i] & 0xFFFFFF00) | ((delay_tap_min[i] + tap_offset));
				else
					delay_tap_max[i] = 31;
			}
		}
		_update_phy_param();

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);
	
		/* Enable the HW auto-calibration update mechanism. */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
				*(dacqdq+i) = *(dacqdq+i) | MEMCTL_DACDQ_DQ_AC_EN;
		}

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);

		/* Check the maximun delay tap value */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
				/* find maximum tap of the minimun delay tap. */
				if( (*(dacqdq+i) & 0x000000FF) == (tap_offset+delay_tap_min[i]) ){
					delay_tap_max[i] = tap_offset+delay_tap_min[i]-1;
				}
				/* Disable the HW auto-calibration update mechanism.*/
				*(dacqdq+i) = *(dacqdq+i) & (~MEMCTL_DACDQ_DQ_AC_EN);
			}
		}

	}

	/* Check wheather there is failured DQ. */
	for(i = 0; i < 32 ; i++){
		if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
			printf("%s, %d: Fail to calibraton for HW auto-calibration. Register address(0x%08x)\n",\
				 __FUNCTION__, __LINE__, dacqdq+i);
			ret_value = MEMCTL_CALI_FAIL;
		}
		if(ret_value == MEMCTL_CALI_FAIL)
			goto go_out;
	}

	for(i=0;i<32;i++){
		target_delay_tap[i] = (target_delay_tap[i] & 0xFFFFFF00) | ((delay_tap_min[i])) ;
	}

#if 0
	printf("calibration for minimum delay tap:\n");
	for(i=0;i<32;i++){
		printf("delay_tap_max[%d] = %d, delay_tap_min[%d] = %d\n", i, delay_tap_max[i], i, delay_tap_min[i]);
	}
#endif


	for(i = 0; i < 32 ; i++){
		*(dacqdq+i) = target_delay_tap[i];
	}
	_update_phy_param();

	/* Enable the HW auto-calibration update mechanism. */
	for(i = 0; i < 32 ; i++){
		*(dacqdq+i) = *(dacqdq+i) | MEMCTL_DACDQ_DQ_AC_EN;
	}

	/* Enable periodic silence pattern generation. */
	//*((volatile unsigned int *)0xB8001008) = 0xfffff7c0;
	//_update_phy_param();

	*dacspcr = (*dacspcr & 0xFFF000FF) | 0x00000F00;
	_update_phy_param();

	*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_PERIOD_EN;

	_update_phy_param();
	//*((volatile unsigned int *)(0xb8001500)) = 0x80000000;
	//__asm__ __volatile__("": : :"memory");
	//*((volatile unsigned int *)(0xb8001500)) = 0x80000010;
go_out:
	return ret_value;
}
#if 0
/* Function Name: 
 * 	_memctlc_cali_phase_A
 * Descripton:
 *	
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	.
 */
unsigned int _memctlc_cali_phase_A(unsigned int src_addr, unsigned int word_size, unsigned int buswidth, \
                         unsigned int dq,        unsigned int w_max,     unsigned int w_min, \
                         unsigned int r_r_max,   unsigned int r_r_min, \
                         unsigned int r_f_max,   unsigned int r_f_min )
{
	volatile unsigned int *pSrc;
	unsigned int i, j;
	unsigned int error_bit_mask;
	unsigned int error_bit_tmp;

	do{
		error_bit_mask = 0;
		/* do write, flush and read data test procedure. */
		_do_wfr_proc(src_addr, word_size);

		/* Now the data is already in DCache. */
		pSrc = (unsigned int *)src_addr;
		j = 0;
		for(i=0;i< word_size;i++){
			j = (j + 1)%(sizeof(cali_pattern)/sizeof(unsigned int));
			error_bit_tmp = (cali_pattern[j] & ~(*(pSrc+i)))|(*(pSrc+i) & (~cali_pattern[j]));
			error_bit_mask = error_bit_mask | error_bit_tmp;
			if(error_bit_mask == 0xffffffff)
				break;
		}

		/* Adjust delay tap */
		if(_MEMCTL_CALI_SATAUS_OVERFLOW == _Inc_dqs_delay_tap_A(error_bit_mask, buswidth)){
			return _MEMCTL_CALI_STATUS_FAIL;
		}

	}while(error_bit_mask != 0x0);

	return _MEMCTL_CALI_STATUS_LOOKING_B;
}


/* Function Name: 
 * 	_memctlc_cali_phase_B
 * Descripton:
 *	
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	.
 */

unsigned int _memctlc_cali_phase_B(unsigned int src_addr, unsigned int word_size, unsigned int buswidth)
{
	
	unsigned int reg_array[32];

	_memctlc_store_phy_para(reg_array);

	/* check whether there is DQ bit with write delay tap == 0x1f */


	if(_MEMCTL_WRITE_DELAY_TAP_WIN_TOO_SMALL ==  _memctlc_check_write_delay_tap_win(reg_array)){
		_memctlc_restore_phy_para(reg_array, target_mask);
		return _MEMCTL_CALI_STATUS_LOOKING_A;
	}

	return _MEMCTL_CALI_STATUS_LOOKING_C;
}


/* Function Name: 
 * 	_memctlc_cali_phase_C
 * Descripton:
 *	
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	.
 */
unsigned int _memctlc_cali_phase_C(void)
{


}

#endif


/* Function Name: 
 * 	memctlc_check_ZQ
 * Descripton:
 * 	Check ZQ calibration status
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	None
 */
void memctlc_check_ZQ(void)
{
	volatile unsigned int *zq_cali_reg;
	volatile unsigned int *zq_cali_status_reg;


	zq_cali_reg = (volatile unsigned int *)0xB8001094;
	zq_cali_status_reg = (volatile unsigned int *)0xB8001098;

	printf("Checking ZQ calibration Status:\n");
	printf("\tZQ Program(0x%08p): 0x%08x\n", zq_cali_reg, *zq_cali_reg);
	printf("\tZQ Status (0x%08p): 0x%08x\n", zq_cali_status_reg, *zq_cali_status_reg);

	if(*zq_cali_status_reg & 0x20000000){
		printf("\tZQ calibration Fail!\n");
	}
	else{
		printf("\tZQ calibration Pass\n");
	}
}

/* Function Name: 
 * 	memctlc_ZQ_calibration
 * Descripton:
 *	Do chip side ZQ configuration
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	.
 */
int memctlc_ZQ_calibration(unsigned int auto_cali_value)
{
#ifdef CONFIG_ZQ_AUTO_CALI
	volatile unsigned int *dmcr, *daccr, *zq_cali_reg;
	volatile unsigned int *zq_cali_status_reg;
	volatile unsigned int *sys_dram_clk_drv_reg, *socpnr;
	unsigned int polling_limit;


	dmcr = (volatile unsigned int *)DMCR;
	socpnr = (volatile unsigned int *)SOCPNR;
	daccr = (volatile unsigned int *)DACCR;
	zq_cali_reg = (volatile unsigned int *)0xB8001094;
	zq_cali_status_reg = (volatile unsigned int *)0xB8001098;
	sys_dram_clk_drv_reg = (volatile unsigned int *)0xB800012c;

	/* Disable DRAM refresh operation */
	*dmcr = ((*dmcr | DMCR_DIS_DRAM_REF_MASK) & (~DMCR_MR_MODE_EN_MASK));

	/* Enable ZQ calibration model */
	if( *socpnr == 0x83890000 )
		*daccr = *daccr | 0x40000000;

	/* Trigger the calibration */
	*zq_cali_reg = auto_cali_value | 0x40000000;

	/* Polling to ready */
	polling_limit = 0x10000;
	while(*zq_cali_reg & 0x80000000){
		polling_limit--;
		if(polling_limit == 0){
			printf("%s, %d: Error, ZQ auto-calibration ready polling timeout!\n", __FUNCTION__, __LINE__);
			break;
		}
	}
	
	/* Enable DRAM refresh operation */
	*dmcr = *dmcr &  (~DMCR_DIS_DRAM_REF_MASK) ;
	
	/* Setting the fine tuned DRAM Clock driving */
	*sys_dram_clk_drv_reg = 0xe300db;

	if(*zq_cali_status_reg & 0x20000000)
		return 1; /* Error, calibration fail. */
	else
		return 0; /* Pass, calibration pass.*/

#else
	unsigned int zq_force_value0, zq_force_value1, zq_force_value2;
	volatile unsigned int *zq_pad_ctl_reg;
	//zq_force_value0 = 0x200267; /* 176 demo board*/
	//zq_force_value1 = 0x10001;
	//zq_force_value2 = 0x50000009;
	//zq_force_value0 = 0x002be738; /* 176 demo board*/
	//zq_force_value1 = 0x017d017d;
	//zq_force_value2 = 0x58785809;
	//zq_force_value0 = 0x0022b267; /*OCD 60, ODT 75*/
	zq_force_value0 = 0x0022b49f; /*OCD 60, ODT 50*/
	zq_force_value1 = 0x00570057;
	zq_force_value2 = 0x58282809;

	zq_pad_ctl_reg = (volatile unsigned int *)0xB8000118;

	*zq_pad_ctl_reg     = zq_force_value0;
	*(zq_pad_ctl_reg+1) = zq_force_value0;
	*(zq_pad_ctl_reg+2) = zq_force_value0;
	*(zq_pad_ctl_reg+3) = zq_force_value0;
	*(zq_pad_ctl_reg+6) = zq_force_value0;
	*(zq_pad_ctl_reg+7) = zq_force_value0;
	*(zq_pad_ctl_reg+8) = zq_force_value0;
	*(zq_pad_ctl_reg+9) = zq_force_value0;


	*(zq_pad_ctl_reg+4) = zq_force_value1;
	*(zq_pad_ctl_reg+5) = zq_force_value1;
	*(zq_pad_ctl_reg+10)= zq_force_value1;

	*(zq_pad_ctl_reg+11)= zq_force_value2;


	return 0;
#endif

}

unsigned int _DCR_get_buswidth(void);

/* Function Name: 
 * 	memctlc_extract_min_dqs_window
 * Descripton:
 *	extract the min. dqs window size among DQs' delay window.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	.
 */
unsigned int memctlc_extract_min_dqs_window(void)
{
	volatile unsigned int *dacdqr, *dacdqf;
	unsigned int min_tap, max_tap, min_window; 
	unsigned int cur_window, dq_num, i;

	dacdqr = (volatile unsigned int *)DACDQR;
	dacdqf = (volatile unsigned int *)DACDQF;

	min_window = 0x20;

	dq_num = _DCR_get_buswidth();

	for(i = 0; i <dq_num; i++){
		min_tap = (*dacdqr & DACDQR_DQR_MIN_TAP_MASK) >> DACDQR_DQR_MIN_TAP_FD_S;
		max_tap = (*dacdqr & DACDQR_DQR_MAX_TAP_MASK) >> DACDQR_DQR_MAX_TAP_FD_S;
		if(min_tap >=max_tap){
			min_window = 0;
			break;
		}
		cur_window = max_tap - min_tap;
		if(min_window > cur_window)
			min_window = cur_window;

		dacdqr++;

		min_tap = (*dacdqf & DACDQR_DQR_MIN_TAP_MASK) >> DACDQR_DQR_MIN_TAP_FD_S;
		max_tap = (*dacdqf & DACDQR_DQR_MAX_TAP_MASK) >> DACDQR_DQR_MAX_TAP_FD_S;
		if(min_tap >=max_tap){
			min_window = 0;
			break;
		}
		cur_window = max_tap - min_tap;
		if(min_window > cur_window)
			min_window = cur_window;

		dacdqf++;
	}

	return min_window;
}


void memctlc_set_dqm_delay(void)
{
	volatile unsigned int *dacdqr;
	volatile unsigned int *dcdqmr;
	volatile unsigned int *dcr;
	unsigned int dqm_delay;
	unsigned int dqm_delay_max;
	unsigned int dqm_delay_min;
	unsigned int bk_dcr;
	volatile unsigned char *src_addr;
	unsigned int src_data_len;
	unsigned char c_data;

	
	dcr    = (volatile unsigned int *)DCR;
	dacdqr = (volatile unsigned int *)DACDQR;
	dcdqmr = (volatile unsigned int *)DCDQMR;
	bk_dcr = *dcr;
	
	/* Configure to 8bit */
	*dcr = *dcr & (~DCR_DBUSWID_MASK); 
	dqm_delay_min = 33;
	dqm_delay_max = 32;
	for(dqm_delay=0;dqm_delay<32;dqm_delay++){
		*dcdqmr = (dqm_delay << DCDQMR_DQM0_PHASE_SHIFT_90_FD_S);
		src_addr = (volatile unsigned char *)0xa0000000;
		src_data_len = 0x100;
		c_data = 0;
		while(src_data_len){
			*src_addr++ = c_data++;
			src_data_len--;
		}
		
		src_addr = (volatile unsigned char *)0xa0000000;
		src_data_len = 0x100;
		c_data = 0;
		while(src_data_len){
			if(*src_addr != c_data){
				break;
			}
			src_addr++;
			c_data++;
			src_data_len--;
		}
		if(src_data_len == 0){
			if(dqm_delay_min==33){
				dqm_delay_min = dqm_delay;
			}
		}else{
			if(dqm_delay_min!=33){
				dqm_delay_max = dqm_delay-1;
				break;
			}
		}
	}
	dqm_delay = (dqm_delay_max + dqm_delay_min)/2;
	*dcdqmr = (dqm_delay << DCDQMR_DQM0_PHASE_SHIFT_90_FD_S) | (dqm_delay << DCDQMR_DQM1_PHASE_SHIFT_90_FD_S);
	/* Restore DCR */
	*dcr = bk_dcr;
	return;

}

void memctlc_ddr3_dll_reset(void)
{
	volatile unsigned int *dmcr, *dtr0;
	volatile unsigned int delay_time;
	unsigned int dtr[3], mr[4];
	unsigned int odt_value, ocd_value;

	dmcr = (volatile unsigned int *)DMCR;
	dtr0 = (volatile unsigned int *)DTR0;
	
	dtr[0]= *dtr0;
	dtr[1]= *(dtr0 + 1);
	dtr[2]= *(dtr0 + 2);

	if(get_memory_dram_odt_parameters(&odt_value)){
		if(odt_value != 0){
	   	      switch ((240/odt_value)/2){
			case 1: /* div 2 */
				odt_value = DDR3_EMR1_RTT_NOM_RZQ_DIV2;
				break;
			case 2: /* div 4*/
				odt_value = DDR3_EMR1_RTT_NOM_RZQ_DIV4;
				break;
			case 3: /* div 6 */
				odt_value = DDR3_EMR1_RTT_NOM_RZQ_DIV6;
				break;
			case 4: /* div 8 */
				odt_value = DDR3_EMR1_RTT_NOM_RZQ_DIV8;
				break;
			case 6: /* div 12 */
				odt_value = DDR3_EMR1_RTT_NOM_RZQ_DIV12;
				break;
			default: /* 40 */
				odt_value = DDR3_EMR1_RTT_NOM_RZQ_DIV2;
				break;
		}
	}else{
			odt_value = DDR3_EMR1_RTT_NOM_DIS;
		}
	}else{
		odt_value = DDR3_EMR1_RTT_NOM_RZQ_DIV2;
	}


	if(get_memory_dram_ocd_parameters(&ocd_value)){
		switch (240/ocd_value){
			case 6: /* RZQ/6 */
				ocd_value = DDR3_EMR1_DIC_RZQ_DIV_6;
				break;
			default: /* RZQ/7 */
				ocd_value = DDR3_EMR1_DIC_RZQ_DIV_7;
				break;
		}
	}else{
		ocd_value = DDR3_EMR1_DIC_RZQ_DIV_6;
	}

	_DTR_DDR3_MRS_setting(dtr, mr);

	mr[1] = mr[1] | odt_value | ocd_value;

	/* 1. Disable DLL */
	*dmcr = mr[1] | DDR3_EMR1_DLL_DIS;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 2. Enable DLL */
	*dmcr = mr[1] & (~DDR3_EMR1_DLL_DIS);
	while(*dmcr & DMCR_MRS_BUSY);
	
	/* 3. Reset DLL */
	*dmcr = mr[0] | DDR3_MR_DLL_RESET_YES ;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 4. Waiting 200 clock cycles */
	delay_time = 0x800;
	while(delay_time--);

	/* 5. Set EMR2 */
	*dmcr = mr[2];
	while(*dmcr & DMCR_MRS_BUSY);

	/* 6. Set EMR3 */
	*dmcr = mr[3];
	while(*dmcr & DMCR_MRS_BUSY);

	/* 7. reset phy fifo */
	memctlc_dram_phy_reset();

	return;
}

void memctlc_ddr2_dll_reset(void)
{
	volatile unsigned int *dmcr, *dtr0;
	volatile unsigned int delay_time;
	unsigned int dtr[3], mr[4];
	unsigned int odt_value;

	dmcr = (volatile unsigned int *)DMCR;
	dtr0 = (volatile unsigned int *)DTR0;
	
	dtr[0]= *dtr0;
	dtr[1]= *(dtr0 + 1);
	dtr[2]= *(dtr0 + 2);

	if(get_memory_dram_odt_parameters(&odt_value)){
		switch (odt_value){
			case 0:
				odt_value = DDR2_EMR1_RTT_DIS;
				break;
			case 75:
				odt_value = DDR2_EMR1_RTT_75;
				break;
			case 150:
				odt_value = DDR2_EMR1_RTT_150;
				break;
			default: /* 50 */
				odt_value = DDR2_EMR1_RTT_50;
				break;
		}
	}else{
		odt_value = DDR2_EMR1_RTT_75;
	}
	_DTR_DDR2_MRS_setting(dtr, mr);

	mr[1] = mr[1] | odt_value;

	/* 1. Disable DLL */
	*dmcr = mr[1] | DDR2_EMR1_DLL_DIS;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 2. Enable DLL */
	*dmcr = mr[1] & (~DDR2_EMR1_DLL_DIS);
	while(*dmcr & DMCR_MRS_BUSY);
	
	/* 3. Reset DLL */
	*dmcr = mr[0] | DDR2_MR_DLL_RESET_YES ;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 4. Waiting 200 clock cycles */
	delay_time = 0x800;
	while(delay_time--);

	/* 5. Set EMR2 */
	*dmcr = mr[2];
	while(*dmcr & DMCR_MRS_BUSY);


	/* 6. reset phy fifo */
	memctlc_dram_phy_reset();

	return;
}

void memctlc_ddr1_dll_reset(void)
{
	volatile unsigned int *dmcr, *dtr0;
	volatile unsigned int delay_time;
	unsigned int dtr[3], mr[4];

	dmcr = (volatile unsigned int *)DMCR;
	dtr0 = (volatile unsigned int *)DTR0;
	
	dtr[0]= *dtr0;
	dtr[1]= *(dtr0 + 1);
	dtr[2]= *(dtr0 + 2);


	_DTR_DDR1_MRS_setting(dtr, mr);


	/* 1. Disable DLL */
	*dmcr = mr[1] | DDR1_EMR1_DLL_DIS;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 2. Enable DLL */
	*dmcr = mr[1] & (~DDR1_EMR1_DLL_DIS);
	while(*dmcr & DMCR_MRS_BUSY);
	
	/* 3. Reset DLL */
	*dmcr = mr[0] | DDR1_MR_OP_RST_DLL ;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 4. Waiting 200 clock cycles */
	delay_time = 0x800;
	while(delay_time--);

	/* 5. Normal mode, avoid to reset DLL when updating phy params */
	*dmcr = mr[0];
	while(*dmcr & DMCR_MRS_BUSY);

	/* 6. reset phy fifo */
	memctlc_dram_phy_reset();

	return;
}



#ifdef CONFIG_DRAM_FROCE_DCR_VALUE
#else
//unsigned int ddr1_8bit_size[] =  { };
unsigned int ddr2_8bit_size[] =  { 0x10220000/* 32MB */, 0x10320000/* 64MB */, 0x20320000/* 128MB */, 0x20420000/* 256MB */, 0x20520000/* 512MB */};
//unsigned int ddr3_8bit_size[] =  { };
//unsigned int ddr1_16bit_size[] = { };
unsigned int ddr2_16bit_size[] = { 0x11210000/* 32MB */, 0x11220000/* 64MB */, 0x21220000/* 128MB */, 0x21320000/* 256MB */, 0x21420000/* 512MB */};
//unsigned int ddr3_16bit_size[] = { };
#endif

void memctlc_config_DRAM_size(void)
{
#ifdef CONFIG_DRAM_AUTO_SIZE_DETECTION

	return memctlc_dram_size_detect();
#else
	volatile unsigned int *dcr;
	unsigned int *size_arry;
	unsigned int dcr_value, dram_size;

	dcr 	  = (volatile unsigned int *)DCR;

#ifdef CONFIG_DRAM_FROCE_DCR_VALUE
	*dcr = (unsigned int)CONFIG_DRAM_DCR;
#else
	dram_size = CONFIG_ONE_DRAM_CHIP_SIZE;
	#ifdef CONFIG_DRAM_BUS_WIDTH_8BIT
		size_arry = &ddr2_8bit_size[0];
	#else
		size_arry = &ddr2_16bit_size[0];
	#endif
	
	switch (dram_size){
		case 0x4000000: /* 64MB */
			dcr_value = size_arry[1];
			break;
		case 0x8000000: /* 128MB */
			dcr_value = size_arry[2];
			break;
		case 0x10000000: /* 256MB */
			dcr_value = size_arry[3];
			break;
		case 0x20000000: /* 512MB */
			dcr_value = size_arry[4];
			break;
		default: /* 32MB */
			dcr_value = size_arry[0];
			break;
	}

	#ifdef CONFIG_DRAM_CHIP_NUM_TWO
	dcr_value = dcr_value | 0x8000;
	#endif
	
	*dcr = dcr_value;
#endif
	return;


#endif

}

void memctlc_DDR2_config_DTR(void)
{
	volatile unsigned int *dtr0, *dtr1, *dtr2;
	unsigned int sug_dtr[3];
	unsigned int dram_freq_mhz;
	unsigned int std_rfc_ns, cas_10_ns, tcwl, wr, cas, wtr;


	dtr0 = (volatile unsigned int *)DTR0;
	dtr1 = (volatile unsigned int *)DTR1;
	dtr2 = (volatile unsigned int *)DTR2;

	std_rfc_ns = DDR2_STD_RFC_NS;

	dram_freq_mhz = board_DRAM_freq_mhz();

	if(dram_freq_mhz >= DDR2_CAS5_MAX_MHZ){
		cas_10_ns = (6 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS4_MAX_MHZ){
		cas_10_ns = (5 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS3_MAX_MHZ){
		cas_10_ns = (4 * 1000* 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS2_MAX_MHZ){
		cas_10_ns = (3 * 1000 * 10)/dram_freq_mhz;
	}else{
		cas_10_ns = (2 * 1000 * 10)/dram_freq_mhz;
	}

        _DTR_suggestion(sug_dtr, DDR2_STD_REFI_NS, DDR2_STD_RP_NS, \
                        DDR2_STD_RCD_NS, DDR2_STD_RAS_NS, std_rfc_ns, DDR2_STD_WR_NS,\
                        DDR2_STD_RRD_NS, DDR2_STD_FAWG_NS, DDR2_STD_WTR_NS, DDR2_STD_RTP_NS,\
                        cas_10_ns, dram_freq_mhz);

	cas = ((sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S);
	if(cas < 2){
		cas = 2;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CAS_MASK)) | (cas << DTR0_CAS_FD_S);
	}

	/* DDR2 write cas == read cas - 1*/
	tcwl = cas - 1;
	if(tcwl < 7){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CWL_MASK)) | (tcwl << DTR0_CWL_FD_S);
	}else{
		printf("\tWorning: wrong tCWL computation\n");
	}

	/* DDR2 Write recovery maximum == 6 */
	wr = ((sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S);
	if(wr > 7){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (5 << DTR0_WR_FD_S);
	}

	/* DDR2 Write to read delay cycle at least 2 clock cycles */
	wtr = ((sug_dtr[0] & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);
	if(wtr < 1){
		wtr = 1;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WTR_MASK)) | (wtr << DTR0_WTR_FD_S);
	}


	*dtr0 = sug_dtr[0];
	*dtr1 = sug_dtr[1];
	*dtr2 = sug_dtr[2];

	return;


}


void memctlc_DDR3_config_DTR(void)
{
	volatile unsigned int *dtr0, *dtr1, *dtr2;
	unsigned int sug_dtr[3];
	unsigned int dram_freq_mhz;
	unsigned int std_rfc_ns, cas_10_ns, tcwl;
	unsigned int rrd, wr, cas, wtr, rtp;


	dtr0 = (volatile unsigned int *)DTR0;
	dtr1 = (volatile unsigned int *)DTR1;
	dtr2 = (volatile unsigned int *)DTR2;

	std_rfc_ns = DDR3_STD_RFC_NS;


	dram_freq_mhz = board_DRAM_freq_mhz();

	if(dram_freq_mhz >= DDR3_CAS10_MAX_MHZ){
		cas_10_ns = (11 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS9_MAX_MHZ){
		cas_10_ns = (10 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS8_MAX_MHZ){
		cas_10_ns = (9 * 1000* 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS7_MAX_MHZ){
		cas_10_ns = (8 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS6_MAX_MHZ){
		cas_10_ns = (7 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS5_MAX_MHZ){
		cas_10_ns = (6 * 1000 * 10)/dram_freq_mhz;
	}else{
		cas_10_ns = (5 * 1000 * 10)/dram_freq_mhz;
	}

        _DTR_suggestion(sug_dtr, DDR3_STD_REFI_NS, DDR3_STD_RP_NS, \
                        DDR3_STD_RCD_NS, DDR3_STD_RAS_NS, std_rfc_ns, DDR2_STD_WR_NS,\
                        DDR3_STD_RRD_NS, DDR3_STD_FAWG_NS, DDR3_STD_WTR_NS, DDR3_STD_RTP_NS,\
                        cas_10_ns, dram_freq_mhz);

	cas = ((sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S);
	if(cas < 4){
		cas = 4;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CAS_MASK)) | (cas << DTR0_CAS_FD_S);
	}

	/* DDR3 write cas */
	if(dram_freq_mhz >= DDR3_CWL7_MAX_MHZ){
		tcwl = 7;
	}else if(dram_freq_mhz >= DDR3_CWL6_MAX_MHZ){
		tcwl = 6;
	}else if(dram_freq_mhz >= DDR3_CWL5_MAX_MHZ){
		tcwl = 5;
	}else{
		tcwl = 4;
	}

	sug_dtr[0] = (sug_dtr[0] & (~DTR0_CWL_MASK)) | (tcwl << DTR0_CWL_FD_S);

	/* DDR3 Write recovery maximum == 12 , min == 5 */
	wr = ((sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S);
	if(wr > 11){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (11 << DTR0_WR_FD_S);
	}else if(wr < 4){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (4 << DTR0_WR_FD_S);
	}else{}

	/* DDR3 Write to read delay cycle at least 4 clock cycles */
	wtr = ((sug_dtr[0] & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);
	if(wtr < 3){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WTR_MASK)) | (3 << DTR0_WTR_FD_S);
	}

	/* DDR3 RTP delay cycle at least 4 clock cycles */
	rtp = ((sug_dtr[0] & DTR0_RTP_MASK) >> DTR0_RTP_FD_S);
	if(rtp < 3){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_RTP_MASK)) | (3 << DTR0_RTP_FD_S);
	}

	/* DDR3 RRD delay cycle at least 4 clock cycles */
	rrd = ((sug_dtr[1] & DTR1_RRD_MASK) >> DTR1_RRD_FD_S);
	if(rrd < 3){
		sug_dtr[1] = (sug_dtr[1] & (~DTR1_RRD_MASK)) | (3 << DTR1_RRD_FD_S);
	}

	*dtr0 = sug_dtr[0];
	*dtr1 = sug_dtr[1];
	*dtr2 = sug_dtr[2];

	return;

}


void memctlc_config_DTR(void)
{
#ifndef	CONFIG_AUTO_DRAM_TIMING_SETTING
	volatile unsigned int *dtr0, *dtr1, *dtr2;

	dtr0 = (volatile unsigned int *)DTR0;
	dtr1 = (volatile unsigned int *)DTR1;
	dtr2 = (volatile unsigned int *)DTR2;

	*dtr0 = CONFIG_DRAM_DTR0;
	*dtr1 = CONFIG_DRAM_DTR1;
	*dtr2 = CONFIG_DRAM_DTR2;
#else
	if(memctlc_is_DDR()){
	}else if(memctlc_is_DDR2()){
		memctlc_DDR2_config_DTR();
	}else{
		memctlc_DDR3_config_DTR();
	}

#endif
	return;
}


/* Function Name: 
 * 	memctlc_init_dram
 * Descripton:
 *	
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	.
 */
unsigned int auto_cali_value[] = { 
					0x00003a25, /* OCD 75ohm, ODT 75ohm */
					0x000012f2, /* OCD 60ohm, ODT 75ohm */
					0x000012a4, /* OCD 60ohm, ODT 150ohm */
					0x00003D71, /* OCD 50ohm, ODT 50ohm */
					0x00003972, /* OCD 60ohm, ODT 60ohm */
					0x000012d1, /* OCD 150ohm, ODT 150ohm */
					0x0000397e, /* OCD 30ohm, ODT 50ohm */
					0x00003f24  /* OCD 60ohm, ODT 50ohm */
				 };
int memctlc_init_dram(void)
{
	unsigned int dram_type, i;
	unsigned int target_clkm_delay, clkm_delay;
	unsigned int max_min_window, cur_min_window;
	unsigned int ddrkodl_init_value, target_ddrkodl_value, zq_cali_value, is_zq_fail;
	unsigned int built_addr;
	unsigned int mem_clk_mhz;
	volatile unsigned int delay_loop;
	volatile unsigned int *sysreg_dram_clk_en_reg;
	volatile unsigned int *ddrkodl_reg;


	/* Runtime deterministic DRAM intialization.
	 * Enter DRAM initialization if it is enabled.
	 * Determine whether we run in DRAM.
	 */
	built_addr = (unsigned int) memctlc_init_dram;
	built_addr = built_addr & 0xF0000000;
	if((built_addr == 0xA0000000) || (built_addr == 0x80000000)){
		goto skip_dram_init;
	}

	mem_clk_mhz = board_DRAM_freq_mhz();

	/* Delay a little bit for waiting for system to enter stable state.*/
	delay_loop = 0x1000;
	while(delay_loop--);
	
	/* Enable DRAM clock */
	sysreg_dram_clk_en_reg = (volatile unsigned int *)SYSREG_DRAM_CLK_EN_REG;
	while(*sysreg_dram_clk_en_reg != SYSREG_DRAM_CLK_EN_MASK ){
		*sysreg_dram_clk_en_reg = SYSREG_DRAM_CLK_EN_MASK;
	}


	/* Delay a little bit for waiting for more stable of the DRAM clock.*/
	delay_loop = 0x1000;
	while(delay_loop--);

#ifndef CONFIG_DRAM_AUTO_SIZE_DETECTION
	memctlc_config_DRAM_size();
#endif
	/* Configure DRAM timing parameters */
	memctlc_config_DTR();

	/* Configure ZQ */
	is_zq_fail = 1;
	if(get_memory_ZQ_parameters(&zq_cali_value)){ //get the value
		is_zq_fail = memctlc_ZQ_calibration(zq_cali_value);
	}
	if(is_zq_fail){//user-defined value fail, try other predefine value
		for(i=0; i< (sizeof(auto_cali_value)/sizeof(unsigned int));i++){
			if(0 == memctlc_ZQ_calibration(auto_cali_value[i])){
				/* We found one .*/
				break;
			}
		}
		if(i >= (sizeof(auto_cali_value)/sizeof(unsigned int)) ){
			printf("ZQ calibration failed\n");
		}
	}

	ddrkodl_reg = (volatile unsigned int *)SYSREG_DDRCKODL_REG;


	if(mem_clk_mhz <= 250){
		ddrkodl_init_value = 0;
		target_clkm_delay = 0;
		target_ddrkodl_value =  ddrkodl_init_value | (target_clkm_delay << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S);
		*ddrkodl_reg = target_ddrkodl_value;
	}else if(mem_clk_mhz < 350){
		ddrkodl_init_value = 0xf0000;
		target_clkm_delay = 0;
		target_ddrkodl_value =  ddrkodl_init_value | (target_clkm_delay << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S);
		*ddrkodl_reg = target_ddrkodl_value;
	}else{
		ddrkodl_init_value = 0xf0000;
		if(get_memory_delay_parameters(&target_ddrkodl_value)){ //get the value
			//we got a predefined value
			//*ddrkodl_reg = (*ddrkodl_reg & 0xFF ) | (target_ddrkodl_value & 0xFFFF00);
			*ddrkodl_reg = target_ddrkodl_value;
			/* Try the value. Search again when fail to try. */
		}else{ //no predefined value
#if 1
			max_min_window = 0;
			target_clkm_delay = 0xffffffff;
			for(clkm_delay = 0;clkm_delay < 32; clkm_delay+=2){
	
				//*ddrkodl_reg = (*ddrkodl_reg & 0xFF ) | \
						(target_ddrkodl_value & 0xFFFF00);
				*ddrkodl_reg = (*ddrkodl_reg & 0xFF ) | \
				((ddrkodl_init_value | (clkm_delay << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S)) & 0xFFFF00) ;
		
				/* Reset DRAM DLL */
				if(memctlc_is_DDR()){
					memctlc_ddr1_dll_reset();
				}else if(memctlc_is_DDR2()){
					memctlc_ddr2_dll_reset();
				}else if(memctlc_is_DDR3()){
					memctlc_ddr3_dll_reset();
				}else{
					printf("%s, %d: Error, Unknown DRAM type! \n", __FUNCTION__, __LINE__);	
					while(1);
				}

				/* DRAM calibration, memctls_init return 1 when failed. */
				if(memctls_init()){
					cur_min_window = 0;
					if( target_clkm_delay != 0xffffffff ){
					/* Failed areadly, no more search. */
					break;
				}
				}else{
					cur_min_window = memctlc_extract_min_dqs_window();
#if 0
				/* cur_min_window == 0 dose fail too.*/
				if(target_clkm_delay != 0xffffffff ){
					if(cur_min_window==0)
						break;
				}
#endif
				//memctlc_set_dqm_delay();
				printf("clkm_delay(%d):cur_min_window=%d\n", clkm_delay, cur_min_window);
			}
			if(cur_min_window >= max_min_window){
				if(cur_min_window < 22){
					max_min_window = cur_min_window;
					target_clkm_delay = clkm_delay;
				}
			}
		}

		if(target_clkm_delay == 0xffffffff )
			printf(" Error: Calibraton failed !\n");

		target_clkm_delay = target_clkm_delay / 2;

		target_ddrkodl_value = ddrkodl_init_value | (target_clkm_delay << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S);
		*ddrkodl_reg = target_ddrkodl_value;
#endif
		}
	}

	/* Reset DRAM DLL */
	if(memctlc_is_DDR()){
		memctlc_ddr1_dll_reset();
	}else if(memctlc_is_DDR2()){
		memctlc_ddr2_dll_reset();
	}else if(memctlc_is_DDR3()){
		memctlc_ddr3_dll_reset();
	}else{
		printf("%s, %d: Error, Unknown DRAM type! \n", __FUNCTION__, __LINE__);	
		while(1);
	}

	memctls_init();
	memctlc_set_dqm_delay();

	/* Configure DRAM size */
	memctlc_config_DRAM_size();

skip_dram_init:
	/* Clock reverse configuration */
	memctlc_clk_rev_check();

	/* Check DRAM Status */
	//board_DRAM_check();

	return 0;
}


/* Function Name: 
 * 	memctlc_dram_pins_connection
 * Descripton:
 *	Check basic connection of DRAM DQs, A0~A9 and CSs pins.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	
 */

#define _cache_flush rlx5281_dcache_flush_inv
#define DRAM_BASE (0x80000000)

int memctlc_dram_pins_connection(void)
{

	/* 
	 * 1. Detect DQ pins 
	 */
	/* 1.1 SAF Checking */
	/* 1.2 Pin short Checking (IC Side)   */
	/* 1.3 Pin short Checking (DRAM Side) */
	

	/* 
	 * 2. Detect address pins 
	 */
	/* 2.1 SAF Checking */
	/* 2.2 Pin short Checking (IC Side)   */
	/* 2.3 Pin short Checking (DRAM Side) */

	/* 
	 * 3. Detect CS pins 
	 */

	
}
#endif

/* Function Name: 
 * 	_get_DRAM_csnum
 * Descripton:
 *	return DRAN total number of bytes.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	DRAM total byte number.
 */
unsigned int _get_DRAM_csnum(void)
{
    unsigned int dcr;

    dcr = *((unsigned int *)(DCR));

    return (((dcr>>15)&1) + 1);

}

/* Function Name: 
 * 	memctlc_dram_size
 * Descripton:
 *	return DRAN total number of bytes.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	DRAM total byte number.
 */
unsigned int memctlc_dram_size(void)
{
    unsigned int dcr;
    int total_bit = 0;


    dcr = *((unsigned int *)(DCR));
    total_bit = 0;
    total_bit += ((dcr>>24)&0x3); //bus width
    total_bit += ((dcr>>20)&0x3)+11; //row count
    total_bit += ((dcr>>16)&0x7)+8 ; //col count
    total_bit += ((dcr>>28)&0x3)+1;  //bank count
    total_bit += (dcr>>15)&1;        //Dram Chip Select

    return ((1<<total_bit));


    //return(1<<total_bit);
}

/* Function Name: 
 * 	memctlc_is_DDR
 * Descripton:
 *	Determine whether the DRAM type is DDR SDRAM.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	1  -DRAM type is DDR SDRAM
 *	0  -DRAM type isn't DDR SDRAM
 */
unsigned int memctlc_is_DDR(void)
{
	if(MCR_DRAMTYPE_DDR == (REG(MCR) & MCR_DRAMTYPE_MASK))
		return 1;
	else
		return 0;
}

/* Function Name: 
 * 	memctlc_is_SDR
 * Descripton:
 *	Determine whether the DRAM type is SDR SDRAM.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	1  -DRAM type is SDR SDRAM
 *	0  -DRAM type isn't SDR SDRAM
 */
unsigned int memctlc_is_DDR3(void)
{
	if(MCR_DRAMTYPE_DDR3 == (REG(MCR) & MCR_DRAMTYPE_MASK))
		return 1;
	else
		return 0;
}

/* Function Name: 
 * 	memctlc_is_DDR2
 * Descripton:
 *	Determine whether the DRAM type is DDR2 SDRAM.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	1  -DRAM type is DDR2 SDRAM
 *	0  -DRAM type isn't DDR2 SDRAM
 */
unsigned int memctlc_is_DDR2(void)
{
	if(MCR_DRAMTYPE_DDR2 == (REG(MCR) & MCR_DRAMTYPE_MASK))
		return 1;
	else
		return 0;
}

/* Function Name: 
 * 	_DTR_refresh_freq_mhz
 * Descripton:
 *	Find out the min. legal DRAM frequency with the current refresh cycles setting in the refi_ns ns.
 * Input:
 *	ms_period -
 * Output:
 * 	None
 * Return:
 *  	0	- refi_ns is too small or refresh cycles is too large.
 *	Others	- Min. legal DRAM freq. with the current refresh cycles setting.
 */
unsigned int _DTR_refresh_freq_mhz(unsigned int refi_ns)
{
	unsigned int refi, refi_unit;
	unsigned int rowcnt;
	unsigned int clk_ns;

	refi      = 1 + ((REG(DTR0) & DTR0_REFI_MASK) >> DTR0_REFI_FD_S);
	refi_unit = 32 << (((REG(DTR0) & DTR0_REFI_UNIT_MASK) >> DTR0_REFI_UNIT_FD_S));

//	rowcnt    = 2048 << ((REG(DCR) & DCR_ROWCNT_MASK) >> DCR_ROWCNT_FD_S);
//	clk_ns = (ms_period * 1000000) / (refi*refi_unit*rowcnt);
	clk_ns = refi_ns / (refi*refi_unit);
	
	if(clk_ns > 0){
		//return (unsigned int)((refi*refi_unit*rowcnt)/(ms_period * 1000));
		return (unsigned int)((refi*refi_unit*1000)/refi_ns);
	}
	else {
		printf("#Warnnig: unstable refresh period setting (%d x %d). ", refi, refi_unit);
		return 0;
	}
}

/* Function Name: 
 * 	_DTR_wtr_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rp) setting.
 * Input:
 *	rp_ns	- tWTR requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(WTR) setting.
 */
unsigned int _DTR_wtr_frq_mhz(unsigned int wtr_ns)
{
	unsigned int wtr;

	/* get tRP value */
	wtr = 1 + ((REG(DTR0) & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);

	return ((1000*wtr)/wtr_ns); /* return number of MHz */
}

/* Function Name: 
 * 	_DTR_rtp_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rp) setting.
 * Input:
 *	rp_ns	- tRP requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rp) setting.
 */
unsigned int _DTR_rtp_frq_mhz(unsigned int rtp_ns)
{
	unsigned int rtp;

	/* get tRP value */
	rtp = 1 + ((REG(DTR0) & DTR0_RTP_MASK) >> DTR0_RTP_FD_S);

	return ((1000*rtp)/rtp_ns); /* return number of MHz */
}


/* Function Name: 
 * 	_DTR_rp_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rp) setting.
 * Input:
 *	rp_ns	- tRP requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rp) setting.
 */
unsigned int _DTR_rp_frq_mhz(unsigned int rp_ns)
{
	unsigned int rp;

	/* get tRP value */
	rp = 1 + ((REG(DTR1) & DTR1_RP_MASK) >> DTR1_RP_FD_S);

	return ((1000*rp)/rp_ns); /* return number of MHz */
}

/* Function Name: 
 * 	_DTR_rrd_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rcd) setting.
 * Input:
 *	rrd_ns	- tRRD requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rrd) setting.
 */
unsigned int _DTR_rrd_frq_mhz(unsigned int rrd_ns)
{
	unsigned int rrd;

	/* get tRCD value */
	rrd = 1 + ((REG(DTR1) & DTR1_RRD_MASK) >> DTR1_RRD_FD_S);

	return ((1000*rrd)/rrd_ns); /* return number of MHz */
}

/* Function Name: 
 * 	_DTR_fawg_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rcd) setting.
 * Input:
 *	fawg_ns	- tFAWG requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(fawg) setting.
 */
unsigned int _DTR_fawg_frq_mhz(unsigned int fawg_ns)
{
	unsigned int fawg;

	/* get tRCD value */
	fawg = 1 + ((REG(DTR1) & DTR1_FAWG_MASK) >> DTR1_FAWG_FD_S);

	return ((1000*fawg)/fawg_ns); /* return number of MHz */
}


/* Function Name: 
 * 	_DTR_rcd_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rcd) setting.
 * Input:
 *	rcd_ns	- tRCD requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rcd) setting.
 */
unsigned int _DTR_rcd_frq_mhz(unsigned int rcd_ns)
{
	unsigned int rcd;

	/* get tRCD value */
	rcd = 1 + ((REG(DTR1) & DTR1_RCD_MASK) >> DTR1_RCD_FD_S);

	return ((1000*rcd)/rcd_ns); /* return number of MHz */
}

/* Function Name: 
 * 	_DTR_ras_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(ras) setting.
 * Input:
 *	ras_ns	- tRAS requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(ras) setting.
 */
unsigned int _DTR_ras_frq_mhz(unsigned int ras_ns)
{
	unsigned int ras;

	/* get tRAS value */
	ras = 1 + ((REG(DTR2) & DTR2_RAS_MASK) >> DTR2_RAS_FD_S);

	return ((1000*ras)/ras_ns); /* return number of MHz */

}

/* Function Name: 
 * 	_DTR_rfc_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rfc) setting.
 * Input:
 *	rfc_ns	- tRFC requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rfc) setting.
 */
unsigned int _DTR_rfc_frq_mhz(unsigned int rfc_ns)
{
	unsigned int rfc;

	/* get tRFC value */
	rfc = 1 + ((REG(DTR2) & DTR2_RFC_MASK) >> DTR2_RFC_FD_S);

	return ((1000*rfc)/rfc_ns); /* return number of MHz */
}


/* Function Name: 
 * 	_DTR_wr_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(wr) setting.
 * Input:
 *	wr_ns	- tWR requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(wr) setting.
 * Note:
 *	Require add 2 more DRAM clock cycles in the current design.
 */
unsigned int _DTR_wr_frq_mhz(unsigned int wr_ns)
{
	unsigned int wr;

	/* get tWR value */
	wr = 1 + ((REG(DTR0) & DTR0_WR_MASK) >> DTR0_WR_FD_S);
#if 0
	if(wr <= 2){
		printf("\t#Warnning: wr < 2 too small.\n");
		return 0;
	}
	else
#endif
	return ((1000*wr)/wr_ns); /* return number of MHz */
}


/* Function Name: 
 * 	_DTR_DDR3_MRS_setting
 * Descripton:
 *	Find out the values of the mode registers according to the DTR0/1/2 setting
 *	for DDR2 SDRAM.
 * Input:
 *	sug_dtr	- The DTR0/1/2 setting.
 * Output:
 *	mr	- The values of the mode registers.
 * Return:
 *	None
 * Note:
 *	None
 */
void _DTR_DDR3_MRS_setting(unsigned int *sug_dtr, unsigned int *mr)
{
	unsigned int cas, wr, cwl;
	/* Default value of Mode registers */
	mr[0] = DMCR_MRS_MODE_MR | DDR3_MR_BURST_8 | DDR3_MR_READ_BURST_NIBBLE | \
		DDR3_MR_TM_NOR | DDR3_MR_DLL_RESET_NO | DDR3_MR_PD_FAST |\
		DMCR_MR_MODE_EN ;

	mr[1] = DDR3_EMR1_DLL_EN | DDR3_EMR1_DIC_RZQ_DIV_6 |\
		DDR3_EMR1_RTT_NOM_DIS | DDR3_EMR1_ADD_0 | DDR3_EMR1_WRITE_LEVEL_DIS | \
		DDR3_EMR1_TDQS_DIS | DDR3_EMR1_QOFF_EN |\
		DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR1;
	mr[2] = DDR3_EMR2_PASR_FULL | DDR3_EMR2_ASR_DIS | DDR3_EMR2_SRT_NOR |\
		DDR3_EMR2_RTT_WR_DIS | DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR2;

	mr[3] = DDR3_EMR3_MPR_OP_NOR | DDR3_EMR3_MPR_LOC_PRE_PAT |\
		DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR3;

	/* Extract CAS and WR in DTR0 */
	cas = (sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S;
	wr = (sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S;
	cwl = (sug_dtr[0] & DTR0_CWL_MASK) >> DTR0_CWL_FD_S;
	switch (cas){
		case 4:
			mr[0] = mr[0] | DDR3_MR_CAS_5;
			break;
		case 5:
			mr[0] = mr[0] | DDR3_MR_CAS_6;
			break;
		case 6:
			mr[0] = mr[0] | DDR3_MR_CAS_7;
			break;
		case 7:
			mr[0] = mr[0] | DDR3_MR_CAS_8;
			break;
		case 8:
			mr[0] = mr[0] | DDR3_MR_CAS_9;
			break;
		case 9:
			mr[0] = mr[0] | DDR3_MR_CAS_10;
			break;
		case 10:
			mr[0] = mr[0] | DDR3_MR_CAS_11;
			break;
		default:
			/* shall be error */
			mr[0] = mr[0] | DDR3_MR_CAS_6;
			break;
	}

	switch (wr){
		case 4:
			mr[0] = mr[0] | DDR3_MR_WR_5;
			break;
		case 5:
			mr[0] = mr[0] | DDR3_MR_WR_6;
			break;
		case 6:
			mr[0] = mr[0] | DDR3_MR_WR_7;
			break;
		case 7:
			mr[0] = mr[0] | DDR3_MR_WR_8;
			break;
		case 8:
			mr[0] = mr[0] | DDR3_MR_WR_9;
			break;
		case 9:
			mr[0] = mr[0] | DDR3_MR_WR_10;
			break;
		case 11:
			mr[0] = mr[0] | DDR3_MR_WR_12;
			break;
		default:
			/* shall be error */
			mr[0] = mr[0] | DDR3_MR_WR_6;
			break;
	}

	switch (cwl){
		case 4:
			mr[2] = mr[2] | DDR3_EMR2_CWL_5;
			break;
		case 5:
			mr[2] = mr[2] | DDR3_EMR2_CWL_6;
			break;
		case 6:
			mr[2] = mr[2] | DDR3_EMR2_CWL_7;
			break;
		case 7:
			mr[2] = mr[2] | DDR3_EMR2_CWL_8;
			break;
		default:
			/* shall be error */
			mr[2] = mr[2] | DDR3_EMR2_CWL_6;
			break;
	}

	return;
}




/* Function Name: 
 * 	_DTR_DDR2_MRS_setting
 * Descripton:
 *	Find out the values of the mode registers according to the DTR0/1/2 setting
 *	for DDR2 SDRAM.
 * Input:
 *	sug_dtr	- The DTR0/1/2 setting.
 * Output:
 *	mr	- The values of the mode registers.
 * Return:
 *	None
 * Note:
 *	None
 */
void _DTR_DDR2_MRS_setting(unsigned int *sug_dtr, unsigned int *mr)
{
	unsigned int cas, wr;
	/* Default value of Mode registers */
	mr[0] = DMCR_MRS_MODE_MR | DDR2_MR_BURST_4 | DDR2_MR_BURST_SEQ | \
		DDR2_MR_TM_NOR | DDR2_MR_DLL_RESET_NO | DDR2_MR_PD_FAST |\
		DMCR_MR_MODE_EN ;

	mr[1] = DDR2_EMR1_DLL_EN | DDR2_EMR1_DIC_FULL |\
		DDR2_EMR1_RTT_DIS | DDR2_EMR1_ADD_0 | DDR2_EMR1_OCD_EX | \
		DDR2_EMR1_QOFF_EN | DDR2_EMR1_NDQS_EN | DDR2_EMR1_RDQS_DIS |\
		DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR1;
	mr[2] = DDR2_EMR2_HTREF_DIS | DDR2_EMR2_DCC_DIS | DDR2_EMR2_PASELF_FULL |\
		DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR2;

	mr[3] = DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR3;

	/* Extract CAS and WR in DTR0 */
	cas = (sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S;
	wr = (sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S;
	switch (cas){
		case 1:
			mr[0] = mr[0] | DDR2_MR_CAS_2;
			break;
		case 2:
			mr[0] = mr[0] | DDR2_MR_CAS_3;
			break;
		case 3:
			mr[0] = mr[0] | DDR2_MR_CAS_4;
			break;
		case 4:
			mr[0] = mr[0] | DDR2_MR_CAS_5;
			break;
		case 5:
			mr[0] = mr[0] | DDR2_MR_CAS_6;
			break;
		default:
			mr[0] = mr[0] | DDR2_MR_CAS_6;
			break;
			
	}

	switch (wr){
		case 1:
			mr[0] = mr[0] | DDR2_MR_WR_2;
			break;
		case 2:
			mr[0] = mr[0] | DDR2_MR_WR_3;
			break;
		case 3:
			mr[0] = mr[0] | DDR2_MR_WR_4;
			break;
		case 4:
			mr[0] = mr[0] | DDR2_MR_WR_5;
			break;
		case 5:
			mr[0] = mr[0] | DDR2_MR_WR_6;
			break;
		case 6:
			mr[0] = mr[0] | DDR2_MR_WR_7;
			break;
		default:
			mr[0] = mr[0] | DDR2_MR_WR_7;
			break;
	}

	return;
}



/* Function Name: 
 * 	_DTR_DDR1_MRS_setting
 * Descripton:
 *	Find out the values of the mode registers according to the DTR0/1/2 setting
 *	for DDR1 SDRAM.
 * Input:
 *	sug_dtr	- The DTR0/1/2 setting.
 * Output:
 *	mr	- The values of the mode registers.
 * Return:
 *	None
 * Note:
 *	None
 */
void _DTR_DDR1_MRS_setting(unsigned int *sug_dtr, unsigned int *mr)
{
	unsigned int cas, buswidth;
	/* Default value of Mode registers */
	mr[0] = DMCR_MRS_MODE_MR | DDR1_MR_BURST_SEQ | DDR1_MR_OP_NOR |\
		DMCR_MR_MODE_EN ;

	mr[1] = DMCR_MRS_MODE_EMR1 | DDR1_EMR1_DLL_EN | DDR1_EMR1_DRV_NOR |\
		DMCR_MR_MODE_EN;

	/* Extract CAS and WR in DTR0 */
	cas = (REG(DTR0) & DTR0_CAS_MASK) >> DTR0_CAS_FD_S;
	buswidth = (REG(DCR) & DCR_DBUSWID_MASK) >> DCR_DBUSWID_FD_S;
	switch (cas){
		case 0:
			mr[0] = mr[0] | DDR1_MR_CAS_25;
			break;
		case 1:
			mr[0] = mr[0] | DDR1_MR_CAS_2;
			break;
		case 2:
			mr[0] = mr[0] | DDR1_MR_CAS_3;
			break;
		default:
			mr[0] = mr[0] | DDR1_MR_CAS_3;
			break;
			
	}

	switch (buswidth){
		case 0:
			mr[0] = mr[0] | DDR1_MR_BURST_4;
			break;
		case 1:
			mr[0] = mr[0] | DDR1_MR_BURST_2;
			break;
		default:
			mr[0] = mr[0] | DDR1_MR_BURST_2;
			break;
	}

	return;
}

/* Function Name: 
 * 	_DTR_suggestion
 * Descripton:
 *	Calculate proper DTR setting.
 * Input:
 *	ref_ms
 *	rp_ns
 *	rcd_ns
 *	ras_ns
 *	rfc_ns
 *	wr_ns
 * Output:
 * 	None
 * Return:
 *	Caluculated DTR setting value for the current environment.
 */
void _DTR_suggestion(unsigned int *sug_dtr, unsigned int refi_ns, unsigned int rp_ns,\
			     unsigned int rcd_ns, unsigned int ras_ns, unsigned int rfc_ns, \
			     unsigned int wr_ns , unsigned int rrd_ns, unsigned int fawg_ns,\
			     unsigned int wtr_ns, unsigned int rtp_ns, unsigned int cas_10_ns, \
			     unsigned int mem_freq)
{
	unsigned int rp, rcd, ras, rfc, wr, wtr, rtp, rrd, fawg;
	unsigned int refi_unit, refi, refi_clks, cas;
	unsigned int dtr_value;
	unsigned int clk_10_ns, rowcnt;
	
	dtr_value = 0;


	clk_10_ns = (1000 * 10) / mem_freq;

	/*count for CAS*/
	cas = (cas_10_ns*10)/clk_10_ns;
	if(cas <= 20){/* Cas = 2*/
		cas = 1;
	}
	else if(cas <= 25){ /* Cas = 2.5*/
		cas = 0;
	}else{
		cas = (cas_10_ns/clk_10_ns);
		if((cas_10_ns%clk_10_ns) != 0)
			cas++;

		cas--;
	}


	/*count for FAWG*/
	fawg = (fawg_ns * 10)/clk_10_ns;
	if(0 != ((fawg_ns*10)%clk_10_ns))
		fawg++;
	fawg--; /* FAWG == 0, apply tFAWG with 1 DRAM clock. */

	/*count for RRD*/
	rrd = (rrd_ns * 10)/clk_10_ns;
	if(0 != ((rrd_ns * 10)%clk_10_ns))
		rrd++;
	rrd--; /* rrd == 0, apply tRRD with 1 DRAM clock. */

	/*count for wtr*/
	wtr = (wtr_ns * 10)/clk_10_ns;
	if(0 != ((wtr_ns*10)%clk_10_ns))
		wtr++;
	wtr--; /* wtr == 0, apply tWTR with 1 DRAM clock. */

	/*count for rtp*/
	rtp = (rtp_ns*10)/clk_10_ns;
	if(0 != ((rtp_ns*10)%clk_10_ns))
		rtp++;
	rtp--; /* wtr == 0, apply tRP with 1 DRAM clock. */


	/*count for rp*/
	rp = (rp_ns*10)/clk_10_ns;
	if(0 != ((rp_ns*10)%clk_10_ns))
		rp++;
	rp--; /* rp == 0, apply tRP with 1 DRAM clock. */

	/*count for rcd*/
	rcd = (rcd_ns*10)/clk_10_ns;
	if(0 != ((rcd_ns*10)%clk_10_ns))
		rcd++;
	
	rcd--; /* rcd == 0, apply tRCD with 1 DRAM clock. */

	/*count for ras*/
	ras = (ras_ns*10)/clk_10_ns;
	if(0 != ((ras_ns*10)%clk_10_ns))
		ras++;
	ras--;

	/*count for rfc*/
	rfc = (rfc_ns*10)/clk_10_ns;
	if(0 != ((rfc_ns*10)%clk_10_ns))
		rfc++;
	rfc--;

	/*count for wr*/
	wr = (wr_ns*10)/clk_10_ns;
	if(0 != ((wr_ns*10)%clk_10_ns))
		wr++;

	wr--; 
	if(wr > 7)
		printf("\twr == %d ,> 7 over range\n", wr);
	

	/* count for DRAM refresh period.*/
	/* get row count */
	//rowcnt = 2048 << ((REG(DCR) & DCR_ROWCNT_MASK) >> DCR_ROWCNT_FD_S);
	//printf("rowcnt(%d)\n", rowcnt);
	//refi_clks = (ref_ms*mem_freq*1000)/rowcnt;
	refi_clks = (refi_ns*mem_freq)/1000;
	for(refi_unit=0; refi_unit <=7; refi_unit++){
		for(refi=0; refi<=15; refi++){
			if(refi_clks < ((32<<refi_unit)*(refi+1))){
				if(refi==0){
					refi = 15;
					if(0 < refi_unit)
						refi_unit--;
				}
				else{
					refi--;
				}
				goto count_dtr;
			}
		}
	}

	if(refi_unit > 7)/* error, not found.*/
		return 0;

count_dtr:
	sug_dtr[0] = ((cas << DTR0_CAS_FD_S)&DTR0_CAS_MASK) | ((wr << DTR0_WR_FD_S)&DTR0_WR_MASK) |\
		     ((rtp << DTR0_RTP_FD_S)&DTR0_RTP_MASK) | ((wtr << DTR0_WTR_FD_S)&DTR0_WTR_MASK) |\
		     ((refi << DTR0_REFI_FD_S)&DTR0_REFI_MASK) | ((refi_unit << DTR0_REFI_UNIT_FD_S)&DTR0_REFI_UNIT_MASK);
	sug_dtr[1] = ((rp << DTR1_RP_FD_S)&DTR1_RP_MASK) | ((rcd << DTR1_RCD_FD_S)&DTR1_RCD_MASK) |\
		     ((rrd << DTR1_RRD_FD_S)&DTR1_RRD_MASK) | ((fawg << DTR1_FAWG_FD_S)&DTR1_FAWG_MASK) ;
	sug_dtr[2] = ((rfc << DTR2_RFC_FD_S)&DTR2_RFC_MASK) | ((ras << DTR2_RAS_FD_S)&DTR2_RAS_MASK);
	//dtr_value = ((rp << DTR_RP_FD_S)&DTR_RP_MASK) | ((rcd << DTR_RCD_FD_S)&DTR_RCD_MASK) | ((ras << DTR_RAS_FD_S)&DTR_RAS_MASK) \
		| ((rfc << DTR_RFC_FD_S)&DTR_RFC_MASK) | ((wr << DTR_WR_FD_S)&DTR_WR_MASK) | ((refi << DTR_REFI_FD_S)&DTR_REFI_MASK) \
		| ((refi_unit << DTR_REFI_UNIT_FD_S)&DTR_REFI_UNIT_MASK);

	return;
}


/* Function Name: 
 * 	_DTR_wr
 * Descripton:
 *	Get WTR setting of MEMCTL
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	Clock cycles of WR.
 */
unsigned int _DTR_wr(void)
{
	unsigned int wr;

	wr = ((REG(DTR0) & DTR0_WR_MASK) >> DTR0_WR_FD_S);

	return wr+1;
}

/* Function Name: 
 * 	_DTR_wtr
 * Descripton:
 *	Get WTR setting of MEMCTL
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	Clock cycles of RRD.
 */
unsigned int _DTR_rrd(void)
{
	unsigned int rrd;

	rrd = ((REG(DTR1) & DTR1_RRD_MASK) >> DTR1_RRD_FD_S);

	return rrd+1;
}


/* Function Name: 
 * 	_DTR_rtp
 * Descripton:
 *	Get RTP setting of MEMCTL
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	Clock cycles of RTP latency
 */
unsigned int _DTR_rtp(void)
{
	unsigned int rtp;

	rtp = ((REG(DTR0) & DTR0_RTP_MASK) >> DTR0_RTP_FD_S);

	return rtp+1;
}

/* Function Name: 
 * 	_DTR_wtr
 * Descripton:
 *	Get WTR setting of MEMCTL
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	Clock cycles of WTR latency
 */
unsigned int _DTR_wtr(void)
{
	unsigned int wtr;

	wtr = ((REG(DTR0) & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);

	return wtr+1;
}

/* Function Name: 
 * 	_DTR_cwl
 * Descripton:
 *	Get Write CAS Latency of MEMCTL
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	Write Cas latency.
 */
unsigned int _DTR_cwl(void)
{
	unsigned int cwl;

	cwl = ((REG(DTR0) & DTR0_CWL_MASK) >> DTR0_CWL_FD_S);

	return cwl+1;
}

/* Function Name: 
 * 	_DTR_two_cas
 * Descripton:
 *	Get 2 times of CAS setting of DCR(cas).
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	Cas latency x 2. (Why x2? We have 2.5 cas latency setting.)
 */
unsigned int _DTR_two_cas(void)
{
	unsigned int cas;

	cas = ((REG(DTR0) & DTR0_CAS_MASK) >> DTR0_CAS_FD_S);

	switch (cas) {
		case 0:
			return 5; /* 2.5 x 2 */
		default:
			return (cas+1)*2;
	}
}


/* Function Name: 
 * 	_DCR_get_buswidth
 * Descripton:
 *	Get bus width setting of DCR(dbuswid).
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	the number of bit of the bus width setting in DCR(dbuswid)
 */
unsigned int _DCR_get_buswidth(void)
{
	unsigned int buswidth;

	buswidth = ((REG(DCR) & DCR_DBUSWID_MASK) >> DCR_DBUSWID_FD_S);

	switch (buswidth) {
		case 0:
			return (8);
		case 1:
			return (16);
		case 2:
			return (32); 
		default:
			printf("#Error: error bus width setting (11)\n");
			return 0;
	}

}


/* Function Name: 
 * 	_DCR_get_chipsel
 * Descripton:
 *	Get chip number setting of DCR(dchipsel).
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	the number of chips of the chip number setting in DCR(dchipsel)
 */
unsigned int _DCR_get_chipsel(void)
{
	unsigned int chipsel;

	chipsel = ((REG(DCR) & DCR_DCHIPSEL_MASK) >> DCR_DCHIPSEL_FD_S);

	if(chipsel)
		return 2;
	else
		return 1;
}


/* Function Name: 
 * 	_DCR_get_rowcnt
 * Descripton:
 *	Get row count setting of DCR(rowcnt).
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	the number of row count of the row count setting in DCR(rowcnt)
 */
unsigned int _DCR_get_rowcnt(void)
{
	unsigned int rowcnt;

	rowcnt = ((REG(DCR) & DCR_ROWCNT_MASK) >> DCR_ROWCNT_FD_S);
	
	return (2048 << rowcnt);
}


/* Function Name: 
 * 	_DCR_get_colcnt
 * Descripton:
 *	Get column count setting of DCR(colcnt).
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	the number of columns of the column count setting in DCR(colcnt)
 */
unsigned int _DCR_get_colcnt(void)
{
	unsigned int colcnt;

	colcnt = ((REG(DCR) & DCR_COLCNT_MASK) >> DCR_COLCNT_FD_S);

	if(4 < colcnt){
		printf("#Eror: error colcnt setting. ( > 4)\n");		
		return 0;
	}
	else
		return (256 << colcnt);

}


/* Function Name: 
 * 	_DCR_get_bankcnt
 * Descripton:
 *	Get bank count setting of DCR(bankcnt).
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	the number of banks of the bank count setting in DCR(bankcnt)
 */
unsigned int _DCR_get_bankcnt(void)
{
	unsigned int bankcnt;

	bankcnt = ((REG(DCR) & DCR_BANKCNT_MASK) >> DCR_BANKCNT_FD_S);
	
	switch (bankcnt)
	{
		case 0:
			return 2;
		case 1:
			return 4;
		case 2:
			return 8;
	}
}


/* Function Name: 
 * 	memctlc_check_DQS_range
 * Descripton:
 *	Check the applied value of DQS delay taps and compared with the experimented data DQS_RANGE.
 *	It shows message related to DQS taps.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	None
 */
void memctlc_check_DQS_range(void)
{
	unsigned int dqs0_tap, dqs1_tap, cal_mode;
	unsigned int dram_freq_mhz, index;
	
	dqs0_tap = (REG(DDCR) & DDCR_DQS0_TAP_MASK) >> DDCR_DQS0_TAP_FD_S;
	dqs1_tap = (REG(DDCR) & DDCR_DQS1_TAP_MASK) >> DDCR_DQS1_TAP_FD_S;
	cal_mode = (REG(DDCR) & DDCR_CAL_MODE_MASK) >> DDCR_CAL_MODE_FD_S;

	printf("DQ/DQS Related Delay:\n");
	if(cal_mode)
		printf("\tCAL_MODE: Digtial\n");
	else
		printf("\tCAL_MODE: DLL\n");


	printf("\tDQS0_tap = %d, DQS1_tap = %d : ", dqs0_tap, dqs1_tap);
	dram_freq_mhz = board_DRAM_freq_mhz();
	for(index = 0; index < (sizeof(DQS_RANGE)/sizeof(tap_info_t)); index++){
		if(DQS_RANGE[index].mode == cal_mode){
			if(DQS_RANGE[index].mhz >= dram_freq_mhz)
				break;
		}
	}

	if(index >= (sizeof(DQS_RANGE)/sizeof(tap_info_t)))
		printf("#Error DRAM frequency %dMHz\n", dram_freq_mhz);

	if(dqs0_tap < DQS_RANGE[index].tap_min || dqs0_tap > DQS_RANGE[index].tap_max \
			|| dqs1_tap < DQS_RANGE[index].tap_min || dqs1_tap > DQS_RANGE[index].tap_max)
		printf("#Warning: current DQS tap setting may not be stable\n");
	else
		printf("ok ");

	printf("(Reference DQS taps %d ~ %d on %dMHz)\n", DQS_RANGE[index].tap_min, DQS_RANGE[index].tap_max, DQS_RANGE[index].mhz);

	return;
}


/* Function Name: 
 * 	memctlc_check_90phase_range
 * Descripton:
 *	Check the applied value of phase shift 90 delay taps and compared with the experimented data W90_RANGE.
 *	It shows message related to phase shift 90 taps.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	None
 */
void memctlc_check_90phase_range(void)
{
	unsigned int phase_tap, cal_mode;
	unsigned int dram_freq_mhz, index;
	
	phase_tap = (REG(DCDR) & DCDR_PHASE_SHIFT_MASK) >> DCDR_PHASE_SHIFT_FD_S;
	cal_mode = (REG(DDCR) & DDCR_CAL_MODE_MASK) >> DDCR_CAL_MODE_FD_S;

	printf("\tphase_shift_90_tap = %d: ", phase_tap);

	dram_freq_mhz = board_DRAM_freq_mhz();

	for(index = 0; index < (sizeof(W90_RANGE)/sizeof(tap_info_t)); index++){
		if(W90_RANGE[index].mode == cal_mode){
			if(W90_RANGE[index].mhz >= dram_freq_mhz)
				break;
		}
	}

	if(index >= (sizeof(W90_RANGE)/sizeof(tap_info_t)))
		printf("#Error DRAM frequency %dMHz\n", dram_freq_mhz);

	if(phase_tap < W90_RANGE[index].tap_min || phase_tap > W90_RANGE[index].tap_max)
		printf("#Warnning: Phase shift 90 taps setting may not be stable\n");
	else
		printf("ok ");

	printf("(Reference phase shift 90 taps %d ~ %d on %dMHz)\n", W90_RANGE[index].tap_min, W90_RANGE[index].tap_max, W90_RANGE[index].mhz);

	return;
}



/* Function Name: 
 * 	memctlc_check_DCR
 * Descripton:
 *	Check the DCR setting of the current environment.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	None
 * Note:
 *	- It shows some messages for the result of the checking.
 */
void memctlc_check_DCR(void)
{

	unsigned int	two_cas, buswidth, chipsel, rowcnt;
	unsigned int	colcnt, bankcnt, dram_total_size;
	unsigned int	dram_freq_mhz;


	/* Get board DRAM freq. */
	dram_freq_mhz = board_DRAM_freq_mhz(); 

	/* 
	 * Check DCR
	 */
	/* 1. Bus width     */
	buswidth = _DCR_get_buswidth();
	printf("\tbus width = %dbit\n", buswidth);
	/* 2. Chip select   */
	chipsel = _DCR_get_chipsel();
	printf("\tnumber of chips = %d\n", chipsel);
	/* 3. Row count     */
	rowcnt = _DCR_get_rowcnt();
	printf("\trow count = %d\n", rowcnt);
	/* 4. Column count  */
	colcnt = _DCR_get_colcnt();
	printf("\tcolumn count = %d\n", colcnt);
	/* 5. Bank count    */
	bankcnt = _DCR_get_bankcnt();
	printf("\tbank count = %d\n", bankcnt);

	/* 6. Show total DRAM size */
	dram_total_size = rowcnt*colcnt*(buswidth/8)*chipsel*bankcnt;
	printf("\tDRAM Size = %dMB\n", (dram_total_size/1024/1024));

	return;
}

/* Function Name: 
 * 	memctlc_check_DTR_DDR3
 * Descripton:
 *	Check the DTR setting of the current environment for DDR3 SDRAM.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	None
 * Note:
 *	- It shows some messages for the result of the checking.
 */
void memctlc_check_DTR_DDR3(unsigned int dram_freq_mhz)
{
	unsigned int	two_cas;
	unsigned int	min_freq_mhz, max_freq_mhz;
	unsigned int	tcwl, wr, std_rtp_ns, std_rrd_ns;
	unsigned int	std_ras_ns, std_wr_ns;
	unsigned int	std_refi_ns, std_wtr_ns, std_fawg_ns;
	unsigned int	std_rp_ns, std_rcd_ns, std_rfc_ns;
	unsigned int	sug_dtr[3], sug_mr[4];
	unsigned int	dram_size;
	unsigned int	cas_10_ns, cas, wtr, rtp, rrd, _rfc_ns;


	/* 
	 * Check DTR
	 */
	std_wr_ns  = DDR3_STD_WR_NS;
	std_rtp_ns  = DDR3_STD_RTP_NS;
	std_wtr_ns  = DDR3_STD_WTR_NS;
	std_refi_ns = DDR3_STD_REFI_NS; /* 7.8us */
	std_rp_ns  = DDR3_STD_RP_NS;
	std_rcd_ns = DDR3_STD_RCD_NS;
	std_rrd_ns = DDR3_STD_RRD_NS;
	std_fawg_ns = DDR3_STD_FAWG_NS;
	std_ras_ns = DDR3_STD_RAS_NS;
	
	dram_size = memctlc_dram_size()/_get_DRAM_csnum();
	switch (dram_size){
		case 0x4000000:
			_rfc_ns = DDR3_STD_RFC_64MB_NS;
			break;
		case 0x8000000:
			_rfc_ns = DDR3_STD_RFC_128MB_NS;
			break;
		case 0x10000000:
			_rfc_ns = DDR3_STD_RFC_256MB_NS;
			break;
		case 0x20000000:
			_rfc_ns = DDR3_STD_RFC_512MB_NS;
			break;
		default:
			_rfc_ns = DDR3_STD_RFC_1GB_NS;
			break;
	}

	std_rfc_ns = DDR3_STD_RFC_NS;

	printf("DTR Checking Rules: \n");
	printf("\tt_refi = %dns, \n", std_refi_ns);
	printf("\tt_wr = %dns\n", std_wr_ns);
	printf("\tt_rtp = %dns\n", std_rtp_ns);
	printf("\tt_wtr = %dns\n", std_wtr_ns);
	printf("\tt_rp = %dns\n", std_rp_ns);
	printf("\tt_rcd = %dns\n", std_rcd_ns);
	printf("\tt_fawg = %dns\n", std_fawg_ns);
	printf("\tt_ras = %dns\n", std_ras_ns);
	printf("\tt_rfc = %dns, _rfc_ns(%d)\n", std_rfc_ns, _rfc_ns);

	printf("Checking Current setting: \n");
	/* 1. CAS latency   */
	two_cas = _DTR_two_cas();
	printf("\tcas(%d.%d): ", (two_cas/2), ((two_cas*10)/2)%10);
	if(two_cas > 22)
		printf("#Warnning: No CAS > 11 for DDR3 SDRAM\n");
	else if((two_cas == 10) && (DDR3_CAS5_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 12) && (DDR3_CAS6_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 14) && (DDR3_CAS7_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 16) && (DDR3_CAS8_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 18) && (DDR3_CAS9_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 20) && (DDR3_CAS10_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 22) && (DDR3_CAS11_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else
		printf("ok\n");


	/* 2. T_WR  (15ns) */
	wr = _DTR_wr();
	printf("\twr: ");
	max_freq_mhz = _DTR_wr_frq_mhz(std_wr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR0(wr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_wr_ns, max_freq_mhz);
	if((wr < 5) | (wr > 12)){
		printf("#Warnning: DTR0(wr) setting (%d clks) may vilolates the min./max. requirement(5~12 clks) for current setting\n", wr+1);
	}else{
		printf("ok\n");
	}

	/* 3. CAS Write latency */
	tcwl = _DTR_cwl();
	printf("\tcwl(%d): ", tcwl);
	if(tcwl > 8)
		printf("#Warnning: No CWL > 8 for DDR3 SDRAM\n");
	else if((tcwl == 5) && (DDR3_CWL5_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CWL might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((tcwl == 6) && (DDR3_CWL6_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CWL might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((tcwl == 7) && (DDR3_CWL7_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CWL might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((tcwl == 8) && (DDR3_CAS8_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CWL might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else
		printf("ok\n");


	/* 4. T_RTP  (20ns) */
	rtp = _DTR_rtp();
	printf("\trtp: ");
	max_freq_mhz = _DTR_rtp_frq_mhz(std_rtp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rtp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rtp_ns, max_freq_mhz);

	if(rtp < 4){
		printf("#Warnning: DTR1(rtp) setting (%d clks) may vilolates the min. requirement(4 clks) for current setting\n", rtp);
	}else{
		printf("ok\n");
	}


	/* 5. T_WTR  (20ns) */
	wtr = _DTR_wtr();
	printf("\twtr: ");
	max_freq_mhz = _DTR_wtr_frq_mhz(std_wtr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(wtr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_wtr_ns, max_freq_mhz);

	if(wtr < 4){
		printf("#Warnning: DTR1(wtr) setting (%d clks) may vilolates the min. requirement(4 clks) for current setting\n", wtr);
	}else{
		printf("ok\n");
	}

	/* 6. Refresh period (7.8us) */
	printf("\trefresh cycles: ");
	min_freq_mhz  = _DTR_refresh_freq_mhz(std_refi_ns);
	if(0 != min_freq_mhz){
		printf("(DRAM freq. have to >= %dMHz) ", min_freq_mhz);
		if(dram_freq_mhz < min_freq_mhz)
			printf("\n\t#Warnning: DTR setting may vilolate the requirement of DRAM refresh in %dms.\n", std_refi_ns);
		else
			printf("ok\n");
	}
	else
		printf("DRAM freq. have to > 1000MHz\n");


	/* 7. T_RP  (20ns) */
	printf("\trp: ");
	max_freq_mhz = _DTR_rp_frq_mhz(std_rp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rp_ns, max_freq_mhz);
	else
		printf("ok\n");
	/* 8. T_RCD (20ns) */
	printf("\trcd: ");
	max_freq_mhz = _DTR_rcd_frq_mhz(std_rcd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rcd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rcd_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 9. T_RRD (20ns) */
	rrd = _DTR_rrd();
	printf("\trrd: ");
	max_freq_mhz = _DTR_rrd_frq_mhz(std_rrd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rrd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rrd_ns, max_freq_mhz);
	
	if(rrd < 4){
		printf("#Warnning: DTR1(rrd) setting (%d clks) may vilolates the min. requirement(4 clks) for current setting\n", rrd);
	}else{
		printf("ok\n");
	}
	/* 10. T_FAWG (20ns) */
	printf("\tfawg: ");
	max_freq_mhz = _DTR_fawg_frq_mhz(std_fawg_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(fawg) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_fawg_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 11. T_RFC */
	printf("\trfc: ");
	max_freq_mhz = _DTR_rfc_frq_mhz(std_rfc_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rfc) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rfc_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 12. T_RAS (45ns) */
	printf("\tras: ");
	max_freq_mhz = _DTR_ras_frq_mhz(std_ras_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(ras) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_ras_ns, max_freq_mhz);
	else
		printf("ok\n");

	if(dram_freq_mhz >= DDR3_CAS10_MAX_MHZ){
		cas_10_ns = (11 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS9_MAX_MHZ){
		cas_10_ns = (10 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS8_MAX_MHZ){
		cas_10_ns = (9 * 1000* 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS7_MAX_MHZ){
		cas_10_ns = (8 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS6_MAX_MHZ){
		cas_10_ns = (7 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS5_MAX_MHZ){
		cas_10_ns = (6 * 1000 * 10)/dram_freq_mhz;
	}else{
		cas_10_ns = (5 * 1000 * 10)/dram_freq_mhz;
	}

        _DTR_suggestion(sug_dtr, DDR3_STD_REFI_NS, DDR3_STD_RP_NS, \
                        DDR3_STD_RCD_NS, DDR3_STD_RAS_NS, std_rfc_ns, DDR2_STD_WR_NS,\
                        DDR3_STD_RRD_NS, DDR3_STD_FAWG_NS, DDR3_STD_WTR_NS, DDR3_STD_RTP_NS,\
                        cas_10_ns, dram_freq_mhz);

	cas = ((sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S);
	if(cas < 4){
		cas = 4;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CAS_MASK)) | (cas << DTR0_CAS_FD_S);
	}

	/* DDR3 write cas */
	if(dram_freq_mhz >= DDR3_CWL7_MAX_MHZ){
		tcwl = 7;
	}else if(dram_freq_mhz >= DDR3_CWL6_MAX_MHZ){
		tcwl = 6;
	}else if(dram_freq_mhz >= DDR3_CWL5_MAX_MHZ){
		tcwl = 5;
	}else{
		tcwl = 4;
	}

	sug_dtr[0] = (sug_dtr[0] & (~DTR0_CWL_MASK)) | (tcwl << DTR0_CWL_FD_S);

	/* DDR3 Write recovery maximum == 12 , min == 5 */
	wr = ((sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S);
	if(wr > 11){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (11 << DTR0_WR_FD_S);
	}else if(wr < 4){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (4 << DTR0_WR_FD_S);
	}else{}

	/* DDR3 Write to read delay cycle at least 4 clock cycles */
	wtr = ((sug_dtr[0] & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);
	if(wtr < 3){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WTR_MASK)) | (3 << DTR0_WTR_FD_S);
	}

	/* DDR3 RTP delay cycle at least 4 clock cycles */
	rtp = ((sug_dtr[0] & DTR0_RTP_MASK) >> DTR0_RTP_FD_S);
	if(rtp < 3){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_RTP_MASK)) | (3 << DTR0_RTP_FD_S);
	}

	/* DDR3 RRD delay cycle at least 4 clock cycles */
	rrd = ((sug_dtr[1] & DTR1_RRD_MASK) >> DTR1_RRD_FD_S);
	if(rrd < 3){
		sug_dtr[1] = (sug_dtr[1] & (~DTR1_RRD_MASK)) | (3 << DTR1_RRD_FD_S);
	}

	_DTR_DDR2_MRS_setting(sug_dtr, sug_mr);
	printf("\tWe suggeset DTR setting for current environment: (0x%08x), (0x%08x), (0x%08x)\n",\
			 sug_dtr[0],  sug_dtr[1],  sug_dtr[2]);
	printf("\tApply suggested DTR by\n\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n",\
			 DTR0, sug_dtr[0], DTR1, sug_dtr[1]);
	printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n", DTR2, sug_dtr[2], DMCR, sug_mr[0]);
	printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n", DMCR, sug_mr[1], DMCR, sug_mr[2]);
	

}



/* Function Name: 
 * 	memctlc_check_DTR_DDR2
 * Descripton:
 *	Check the DTR setting of the current environment for DDR2 SDRAM.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	None
 * Note:
 *	- It shows some messages for the result of the checking.
 */
void memctlc_check_DTR_DDR2(unsigned int dram_freq_mhz)
{
	unsigned int	two_cas;
	unsigned int	min_freq_mhz, max_freq_mhz;
	unsigned int	tcwl, wr, std_rtp_ns, std_rrd_ns;
	unsigned int	std_ras_ns, std_wr_ns;
	unsigned int	std_refi_ns, std_wtr_ns, std_fawg_ns;
	unsigned int	std_rp_ns, std_rcd_ns, std_rfc_ns;
	unsigned int	sug_dtr[3], sug_mr[4];
	unsigned int	dram_size;
	unsigned int	cas_10_ns, cas, wtr, _rfc_ns;


	/* 
	 * Check DTR
	 */
	std_wr_ns  = DDR2_STD_WR_NS;
	std_rtp_ns  = DDR2_STD_RTP_NS;
	std_wtr_ns  = DDR2_STD_WTR_NS;
	std_refi_ns = DDR2_STD_REFI_NS; /* 7.8us */
	std_rp_ns  = DDR2_STD_RP_NS;
	std_rcd_ns = DDR2_STD_RCD_NS;
	std_rrd_ns = DDR2_STD_RRD_NS;
	std_fawg_ns = DDR2_STD_FAWG_NS;
	std_ras_ns = DDR2_STD_RAS_NS;
	
	dram_size = memctlc_dram_size()/_get_DRAM_csnum();
	switch (dram_size){
		case 0x2000000:
			_rfc_ns = DDR2_STD_RFC_32MB_NS;
			break;
		case 0x4000000:
			_rfc_ns = DDR2_STD_RFC_64MB_NS;
			break;
		case 0x8000000:
			_rfc_ns = DDR2_STD_RFC_128MB_NS;
			break;
		case 0x10000000:
			_rfc_ns = DDR2_STD_RFC_256MB_NS;
			break;
		case 0x20000000:
			_rfc_ns = DDR2_STD_RFC_512MB_NS;
			break;
		default:
			_rfc_ns = DDR2_STD_RFC_512MB_NS;
			break;
	}

	std_rfc_ns = DDR2_STD_RFC_NS;


	printf("DTR Checking Rules: \n");
	printf("\tt_refi = %dns, \n", std_refi_ns);
	printf("\tt_wr = %dns\n", std_wr_ns);
	printf("\tt_rtp = %dns\n", std_rtp_ns);
	printf("\tt_wtr = %dns\n", std_wtr_ns);
	printf("\tt_rp = %dns\n", std_rp_ns);
	printf("\tt_rcd = %dns\n", std_rcd_ns);
	printf("\tt_fawg = %dns\n", std_fawg_ns);
	printf("\tt_ras = %dns\n", std_ras_ns);
	printf("\tt_rfc = %dns, _rfc_ns(%d)\n", std_rfc_ns, _rfc_ns);

	printf("Checking Current setting: \n");
	/* 1. CAS latency   */
	two_cas = _DTR_two_cas();
	printf("\tcas(%d.%d): ", (two_cas/2), ((two_cas*10)/2)%10);
	if(two_cas > 12)
		printf("#Warnning: No CAS > 6 for DDR2 SDRAM\n");
	else if((two_cas == 4) && (DDR2_CAS2_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 6) && (DDR2_CAS3_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 8) && (DDR2_CAS4_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 10) && (DDR2_CAS5_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 12) && (DDR2_CAS6_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else
		printf("ok\n");
	
	/* 2. T_WR  (15ns) */
	printf("\twr: ");
	max_freq_mhz = _DTR_wr_frq_mhz(std_wr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR0(wr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_wr_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 3. CAS Write latency */
	tcwl = _DTR_cwl();
	printf("\tcwl(%d): ", tcwl);
	if(tcwl != ((two_cas/2) - 1) )
		printf("#Warnning: DTR0(cwl) != cas-1 DRAM clock\n");
	else
		printf("ok\n");

	/* 4. T_RTP  (20ns) */
	printf("\trtp: ");
	max_freq_mhz = _DTR_rtp_frq_mhz(std_rtp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rtp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rtp_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 5. T_WTR  (20ns) */
	printf("\twtr: ");
	max_freq_mhz = _DTR_wtr_frq_mhz(std_wtr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(wtr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_wtr_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 6. Refresh period (64ms) */
	printf("\trefresh cycles: ");
	min_freq_mhz  = _DTR_refresh_freq_mhz(std_refi_ns);
	if(0 != min_freq_mhz){
		printf("(DRAM freq. have to >= %dMHz) ", min_freq_mhz);
		if(dram_freq_mhz < min_freq_mhz)
			printf("\n\t#Warnning: DTR setting may vilolate the requirement of DRAM refresh in %dns.\n", std_refi_ns);
		else
			printf("ok\n");
	}
	else
		printf("DRAM freq. have to > 1000MHz\n");


	/* 7. T_RP  (20ns) */
	printf("\trp: ");
	max_freq_mhz = _DTR_rp_frq_mhz(std_rp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rp_ns, max_freq_mhz);
	else
		printf("ok\n");
	/* 8. T_RCD (20ns) */
	printf("\trcd: ");
	max_freq_mhz = _DTR_rcd_frq_mhz(std_rcd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rcd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rcd_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 9. T_RRD (20ns) */
	printf("\trrd: ");
	max_freq_mhz = _DTR_rrd_frq_mhz(std_rrd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rrd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rrd_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 10. T_FAWG (20ns) */
	printf("\tfawg: ");
	max_freq_mhz = _DTR_fawg_frq_mhz(std_fawg_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(fawg) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_fawg_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 11. T_RFC */
	printf("\trfc: ");
	max_freq_mhz = _DTR_rfc_frq_mhz(std_rfc_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rfc) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rfc_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 12. T_RAS (45ns) */
	printf("\tras: ");
	max_freq_mhz = _DTR_ras_frq_mhz(std_ras_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(ras) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_ras_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 13. Suggest DTR Setting for Current Memory freq. */
	if(dram_freq_mhz >= DDR2_CAS5_MAX_MHZ){
		cas_10_ns = (6 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS4_MAX_MHZ){
		cas_10_ns = (5 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS3_MAX_MHZ){
		cas_10_ns = (4 * 1000* 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS2_MAX_MHZ){
		cas_10_ns = (3 * 1000 * 10)/dram_freq_mhz;
	}else{
		cas_10_ns = (2 * 1000 * 10)/dram_freq_mhz;
	}
	_DTR_suggestion(sug_dtr, DDR2_STD_REFI_NS, DDR2_STD_RP_NS, \
			DDR2_STD_RCD_NS, DDR2_STD_RAS_NS, std_rfc_ns, DDR2_STD_WR_NS,\
			DDR2_STD_RRD_NS, DDR2_STD_FAWG_NS, DDR2_STD_WTR_NS, DDR2_STD_RTP_NS,\
			cas_10_ns, dram_freq_mhz);

	/* Check for Minimum CAS support */
	cas = ((sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S);
	if(cas < 2){
		cas = 2;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CAS_MASK)) | (cas << DTR0_CAS_FD_S);
	}
	/* DDR2 write cas == read cas - 1*/
	tcwl = cas - 1;
	if(tcwl < 7){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CWL_MASK)) | (tcwl << DTR0_CWL_FD_S);
	}else{
		printf("\tWorning: wrong tCWL computation\n");
	}

	/* DDR2 Write recovery maximum == 6 */
	wr = ((sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S);
	if(wr > 7){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (5 << DTR0_WR_FD_S);
	}

	/* DDR2 Write to read delay cycle at least 2 clock cycles */
	wtr = ((sug_dtr[0] & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);
	if(wtr < 1){
		wtr = 1;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WTR_MASK)) | (wtr << DTR0_WTR_FD_S);
	}

	_DTR_DDR2_MRS_setting(sug_dtr, sug_mr);
	printf("\tWe suggeset DTR setting for current environment: (0x%08x), (0x%08x), (0x%08x)\n",\
			 sug_dtr[0],  sug_dtr[1],  sug_dtr[2]);
	printf("\tApply suggested DTR by\n\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n",\
			 DTR0, sug_dtr[0], DTR1, sug_dtr[1]);
	printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n", DTR2, sug_dtr[2], DMCR, sug_mr[0]);
	printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n", DMCR, sug_mr[1], DMCR, sug_mr[2]);
	

}


/* Function Name: 
 * 	memctlc_check_DTR_DDR
 * Descripton:
 *	Check the DTR setting of the current environment for DDR1 SDRAM.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	None
 * Note:
 *	- It shows some messages for the result of the checking.
 */
void memctlc_check_DTR_DDR(unsigned int dram_freq_mhz)
{
	unsigned int	two_cas;
	unsigned int	min_freq_mhz, max_freq_mhz;
	unsigned int	tcwl, std_rtp_ns, std_rrd_ns;
	unsigned int	std_ras_ns, std_wr_ns;
	unsigned int	std_refi_ns, std_wtr_ns, std_fawg_ns;
	unsigned int	std_rp_ns, std_rcd_ns, std_rfc_ns;
	unsigned int	sug_dtr[3], sug_mr[4];
	unsigned int	cas_10_ns;


	/* 
	 * Check DTR
	 */
	std_wr_ns   = DDR_STD_WR_NS;
	std_rtp_ns  = DDR_STD_RTP_NS;
	std_wtr_ns  = DDR_STD_WTR_NS;
	std_refi_ns  = DDR_STD_REFI_NS; /* 7.8us */
	std_rp_ns   = DDR_STD_RP_NS;
	std_rcd_ns  = DDR_STD_RCD_NS;
	std_rrd_ns  = DDR_STD_RRD_NS;
	std_fawg_ns = DDR_STD_FAWG_NS;
	std_ras_ns  = DDR_STD_RAS_NS;
	std_rfc_ns  = DDR_STD_RFC_NS;

	printf("DTR Checking Rules: \n");
	printf("\tt_refi = %dns, \n", DDR_STD_REFI_NS);
	printf("\tt_wr = %dns\n",  DDR_STD_WR_NS);
	printf("\tt_wtr = %dns\n", DDR_STD_WTR_NS);
	printf("\tt_rp = %dns\n",  DDR_STD_RP_NS);
	printf("\tt_rcd = %dns\n", DDR_STD_RCD_NS);
	printf("\tt_ras = %dns\n", DDR_STD_RAS_NS);
	printf("\tt_rfc = %dns\n", DDR_STD_RFC_NS);

	printf("Checking Current setting: \n");
	/* 1. CAS latency   */
	two_cas = _DTR_two_cas();
	printf("\tcas(%d.%d): ", (two_cas/2), ((two_cas*10)/2)%10);
	if(two_cas > 6)
		printf("#Warnning: No CAS > 3 for DDR SDRAM\n");
	else if((two_cas == 4) && (DDR_CAS2_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 5) && (DDR_CAS25_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 6) && (DDR_CAS3_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else
		printf("ok\n");
	
	/* 2. T_WR  (15ns) */
	printf("\twr: ");
	max_freq_mhz = _DTR_wr_frq_mhz(std_wr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR0(wr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_wr_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 3. CAS Write latency */
	tcwl = _DTR_cwl();
	printf("\tcwl(%d): ", tcwl);
	if(tcwl != 1 )
		printf("#Warnning: DTR0(cwl) != 1 DRAM clock\n");
	else
		printf("ok\n");

	/* 4. DDR1 no T_RTP */

	/* 5. T_WTR  (20ns) */
	printf("\twtr: ");
	max_freq_mhz = _DTR_wtr_frq_mhz(std_wtr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(wtr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_wtr_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 6. Refresh period (7.8us) */
	printf("\trefresh cycles: ");
	min_freq_mhz  = _DTR_refresh_freq_mhz(std_refi_ns);
	if(0 != min_freq_mhz){
		printf("(DRAM freq. have to >= %dMHz) ", min_freq_mhz);
		if(dram_freq_mhz < min_freq_mhz)
			printf("\n\t#Warnning: DTR setting may vilolate the requirement of DRAM refresh in %dns.\n", std_refi_ns);
		else
			printf("ok\n");
	}
	else
		printf("DRAM freq. have to > 1000MHz\n");


	/* 7. T_RP  (20ns) */
	printf("\trp: ");
	max_freq_mhz = _DTR_rp_frq_mhz(std_rp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rp_ns, max_freq_mhz);
	else
		printf("ok\n");
	/* 8. T_RCD (20ns) */
	printf("\trcd: ");
	max_freq_mhz = _DTR_rcd_frq_mhz(std_rcd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rcd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rcd_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 9. T_RRD (20ns) */
	printf("\trrd: ");
	max_freq_mhz = _DTR_rrd_frq_mhz(std_rrd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rrd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rrd_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 10. DDR1 no T_FAWG */

	/* 11. T_RFC (75ns) */
	printf("\trfc: ");
	max_freq_mhz = _DTR_rfc_frq_mhz(std_rfc_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rfc) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rfc_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 12. T_RAS (45ns) */
	printf("\tras: ");
	max_freq_mhz = _DTR_ras_frq_mhz(std_ras_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(ras) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_ras_ns, max_freq_mhz);
	else
		printf("ok\n");

	/* 13. Suggest DTR Setting for Current Memory freq. */
	if(dram_freq_mhz >= DDR_CAS3_MAX_MHZ){
		cas_10_ns = (3 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR_CAS25_MAX_MHZ){
		cas_10_ns = (2 * 1500 * 10)/dram_freq_mhz;
	}else{
		cas_10_ns = (2 * 1000 * 10)/dram_freq_mhz;
	}
	_DTR_suggestion(sug_dtr, DDR_STD_REFI_NS, DDR_STD_RP_NS, \
			DDR_STD_RCD_NS, DDR_STD_RAS_NS, DDR_STD_RFC_NS, DDR_STD_WR_NS,\
			DDR_STD_RRD_NS, DDR_STD_FAWG_NS, DDR_STD_WTR_NS, DDR_STD_RTP_NS,\
			cas_10_ns, dram_freq_mhz);
	_DTR_DDR1_MRS_setting(sug_dtr, sug_mr);
	printf("\tWe suggeset DTR setting for current environment: (0x%08x), (0x%08x), (0x%08x)\n",\
			 sug_dtr[0],  sug_dtr[1],  sug_dtr[2]);
	printf("\tApply suggested DTR by\n\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n",\
			 DTR0, sug_dtr[0], DTR1, sug_dtr[1]);
	printf("\t\"mw 0x%08x 0x%08x;\"\n", DTR2, sug_dtr[2]);
	printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n", DMCR, sug_mr[0], DMCR, sug_mr[1]);

	return;
}

/* Function Name: 
 * 	memctlc_check_DTR
 * Descripton:
 *	Check the DTR setting of the current environment.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	None
 * Note:
 *	- It shows some messages for the result of the checking.
 */
void memctlc_check_DTR(unsigned int dram_freq_mhz)
{
	if(memctls_is_DDR()){
		memctlc_check_DTR_DDR(dram_freq_mhz);
	}else if(memctls_is_DDR2()){
		memctlc_check_DTR_DDR2(dram_freq_mhz);
	}else if(memctls_is_DDR3()){
		memctlc_check_DTR_DDR3(dram_freq_mhz);
	}else{
		_memctl_debug_printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		_memctl_debug_printf("Error Unkown DRAM Type\n");
	}

	return;
}

void memctlc_dram_phy_reset(void)
{
	volatile unsigned int *phy_ctl;

	phy_ctl = (volatile unsigned int *)DACCR;
	*phy_ctl = *phy_ctl & ((unsigned int) 0xFFFFFFEF);
	*phy_ctl = *phy_ctl | ((unsigned int) 0x10);
	_memctl_debug_printf("memctlc_dram_phy_reset: 0x%08p(0x%08x)\n", phy_ctl, *phy_ctl);

	return;
}

int memctl_dram_para_set(unsigned int width, unsigned int row, unsigned int column, \
			 unsigned int bank,  unsigned int cs)
{
	unsigned int dcr_value;
	volatile unsigned int *p_dcr;


	_memctl_debug_printf("%s, %d: width(%d), row(%d), column(%d), bank(%d), cs(%d)\n",\
				 __FUNCTION__, __LINE__, width, row, column, bank, cs);
	memctlc_set_DRAM_buswidth(width);
	memctlc_set_DRAM_colnum(column);
	memctlc_set_DRAM_rownum(row);
	memctlc_set_DRAM_banknum(bank);
	memctlc_set_DRAM_chipnum(cs);

	/* Reset PHY FIFO pointer */
	memctlc_dram_phy_reset();

	return MEMCTL_DRAM_PARAM_OK;
}

/* Function Name: 
 * 	memctlc_dram_size_detect
 * Descripton:
 *	Detect the size of current DRAM Chip
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	The number of bytes of current DRAM chips
 * Note:
 *	None
 */
unsigned int memctlc_dram_size_detect(void)
{
	unsigned int i;
	unsigned int width, row, col, bk, cs, max_bk;
	volatile unsigned int *p_dcr;
	volatile unsigned int *test_addr0, *test_addr1;
	volatile unsigned int test_v0, test_v1, tmp;
	p_dcr	    = (volatile unsigned int *)DCR;

	/* Intialize DRAM parameters */
	width = MEMCTL_DRAM_MIN_WIDTH;
	row   = MEMCTL_DRAM_MIN_ROWS ;
	col   = MEMCTL_DRAM_MIN_COLS ;
	cs    = MEMCTL_DRAM_MIN_CS;

	/* Configure to the maximun bank number */
	if(memctls_is_DDR()){
		bk = MEMCTL_DRAM_DDR_MIN_BANK_NUM;
	}else if(memctls_is_DDR2()){
		bk = MEMCTL_DRAM_DDR2_MIN_BANK_NUM;
	}else{
		bk = MEMCTL_DRAM_DDR3_MIN_BANK_NUM;
	}	


	/* 0. Buswidth detection */
	test_addr0 = (volatile unsigned int *)(0xA0000000);
	test_v0 = 0x12345678;
	test_v1 = 0x00000000;
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(MEMCTL_DRAM_MAX_WIDTH, row, col, bk , cs)){
			memctlc_dram_phy_reset();
			*test_addr0 = test_v1;
			*test_addr0 = test_v0;
			//_memctl_debug_printf("test_addr(0x%08x)!= test_v0(0x%08x)\n", *test_addr0, test_v0);
			if( test_v0 !=  *test_addr0 ){
				width = MEMCTL_DRAM_MIN_WIDTH;
			}else{
				width = MEMCTL_DRAM_MAX_WIDTH;
			}
	}


	/* 1. Chip detection */
	test_addr0 = (volatile unsigned int *)(0xA0000000);
	test_v0 = 0xCACA0000;
	test_v1 = 0xACAC0000;
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, bk , MEMCTL_DRAM_MAX_CS)){
		while(cs < MEMCTL_DRAM_MAX_CS){
			memctlc_dram_phy_reset();
			test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col*row*bk*cs);
			*test_addr0 = 0x0;
			*test_addr1 = 0x0;
			*test_addr0 = test_v0;
			*test_addr1 = test_v1;
			if( test_v0 ==  *test_addr0 ){
				if( test_v1 ==  *test_addr1 ){
					cs = cs << 1;
					test_v0++;
					test_v1++;
					continue;
				}
			}
			break;
		}
	}

	/* 2. Bank detction */
	test_addr0 = (volatile unsigned int *)(0xA0000000);
	test_v0 = 0x33330000;
	test_v1 = 0xCCCC0000;
	if(memctls_is_DDR()){
		max_bk = MEMCTL_DRAM_DDR_MAX_BANK_NUM;
	}else if(memctls_is_DDR2()){
		max_bk = MEMCTL_DRAM_DDR2_MAX_BANK_NUM;
	}else{
		max_bk = MEMCTL_DRAM_DDR3_MAX_BANK_NUM;
	}

	//_memctl_debug_printf("%s,%d: width(%d), row(%d), col(%d), max_bk(%d), cs(%d)\n", __FUNCTION__, __LINE__, width, row, col, max_bk, cs);
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, max_bk, cs)){
		while(bk < max_bk){
			memctlc_dram_phy_reset();
			test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col*row*bk);
			_memctl_debug_printf("DCR(0x%08x):", *((volatile unsigned int *)DCR) );
			_memctl_debug_printf("BK:(0x%08p)\n", test_addr1);
			*test_addr0 = 0x0;
			*test_addr1 = 0x0;
			*test_addr0 = test_v0;
			*test_addr1 = test_v1;
			tmp = *test_addr0;
			tmp = *test_addr1;
			//_memctl_debug_printf("test_addr0(0x%p):0x%x\n", test_addr0, *test_addr0);
			//_memctl_debug_printf("test_addr1(0x%p):0x%x\n", test_addr1, *test_addr1);
			if( test_v0 ==  *test_addr0 ){
				if( test_v1 ==  *test_addr1 ){
					bk = bk << 1;
					test_v0++;
					test_v1++;
					continue;
				}
			}
			break;
		}
	}

	/* 3. Row detction */
	test_addr0 = (volatile unsigned int *)(0xA0000000);
	test_v0 = 0xCAFE0000;
	test_v1 = 0xDEAD0000;
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, MEMCTL_DRAM_MAX_ROWS, col, bk, cs)){
		while(row < MEMCTL_DRAM_MAX_ROWS){
			memctlc_dram_phy_reset();
			test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col*row);
			_memctl_debug_printf("DCR(0x%08x):", *((volatile unsigned int *)DCR) );
			_memctl_debug_printf("row:(0x%08p)\n", test_addr1);
			*test_addr0 = test_v0;
			*test_addr1 = test_v1;
			tmp = *test_addr0;
			tmp = *test_addr1;
			_memctl_debug_printf("test_addr0(0x%p):0x%x\n", test_addr0, *test_addr0);
			_memctl_debug_printf("test_addr1(0x%p):0x%x\n", test_addr1, *test_addr1);
			_memctl_debug_printf("row = %d\n", row);
			if( test_v0 ==  *test_addr0 ){
				if( test_v1 ==  *test_addr1 ){
					row = row << 1;
					test_v0++;
					test_v1++;
					continue;
				}
			}
			break;
		}
	}

	/* 4. Column detection */
	test_addr0 = (volatile unsigned int *)(0xA0000000);
	test_v0 = 0x5A5A0000;
	test_v1 = 0xA5A50000;
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, MEMCTL_DRAM_MAX_COLS, bk, cs)){
		while(col < MEMCTL_DRAM_MAX_COLS){
			memctlc_dram_phy_reset();
			test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col);
			*test_addr0 = 0x0;
			*test_addr1 = 0x0;
			_memctl_debug_printf("DCR(0x%08x):", *((volatile unsigned int *)DCR) );
			_memctl_debug_printf("col:(0x%08p)\n", test_addr1);
			*test_addr0 = test_v0;
			*test_addr1 = test_v1;
			tmp = *test_addr0;
			tmp = *test_addr1;
			_memctl_debug_printf("test_addr0(0x%p):0x%x\n", test_addr0, *test_addr0);
			_memctl_debug_printf("test_addr1(0x%p):0x%x\n", test_addr1, *test_addr1);
			if( test_v0 ==  *test_addr0 ){
				if( test_v1 ==  *test_addr1 ){
					col = col << 1;
					test_v0++;
					test_v1++;
					continue;
				}
			}
			break;
		}
	}

	/* 5. Width detction */
	test_addr0 = (volatile unsigned int *)(0xA0000000);
	test_addr1 = (volatile unsigned int *)(0xA0000000);
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, bk, cs)){
		while(width < MEMCTL_DRAM_MAX_WIDTH){
		memctlc_dram_phy_reset();
		*test_addr0 = 0x3333CCCC;
		__asm__ __volatile__("": : :"memory");
		if( 0x3333CCCC !=  *test_addr0 ){
			width = width >> 1;
			continue;
		}
		__asm__ __volatile__("": : :"memory");
		*test_addr1 = 0x12345678;
		__asm__ __volatile__("": : :"memory");
		if( 0x12345678 !=  *test_addr1 ){
			width = width >> 1;
			continue;
		}
		break;
		}
	}



	memctlc_dram_phy_reset();
	_memctl_debug_printf("DCR(0x%08p): 0x%08x\n", p_dcr, *p_dcr);
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, bk, cs)){
		_memctl_debug_printf("Width	  : %d\n", width);
		_memctl_debug_printf("Row  	  : %d\n", row);
		_memctl_debug_printf("Column    : %d\n", col);
		_memctl_debug_printf("Bank 	  : %d\n", bk);
		_memctl_debug_printf("Chip 	  : %d\n", cs);
		_memctl_debug_printf("total size: %x\n", (unsigned int)((width/8)*row*col*bk*cs));
		goto test_pass;
	}else{
		_memctl_debug_printf("Error! memctl_dram_para_set failed, function: %s, line:%d", __FUNCTION__, __LINE__);
		goto test_fail;
	}

test_pass:
	return (unsigned int)((width/8)*row*col*bk*cs);
test_fail:
	return 0;
}


/*
 * setting clock reverse indication.
 * Can't run in DRAM.
 */
void memctlc_clk_rev_check(void)
{

	unsigned int *clk_rev_ctl_reg;
	unsigned int clk_rev;
	unsigned int cpu_clk;
	unsigned int mem_clk;
	unsigned int lx_clk;

	clk_rev = 0;
	clk_rev_ctl_reg = (unsigned int *)SYSREG_CMUCTLR_REG;	

	cpu_clk = board_CPU_freq_mhz();
	mem_clk = board_DRAM_freq_mhz();
	lx_clk = board_LX_freq_mhz();

	if(cpu_clk < mem_clk)
		clk_rev = (clk_rev | SYSREG_OCP0_SMALLER_MASK | SYSREG_OCP1_SMALLER_MASK);

	if(lx_clk < mem_clk){
		clk_rev = (clk_rev | SYSREG_LX0_SMALLER_MASK | SYSREG_LX1_SMALLER_MASK | SYSREG_LX2_SMALLER_MASK);
		//clk_rev = (clk_rev | LX1_SMALLER_MEM ); /* 20110830: We only can change LX1 freq. */
	}
	*clk_rev_ctl_reg = *clk_rev_ctl_reg | clk_rev;	

	return;
}
