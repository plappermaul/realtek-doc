#include <preloader.h>
//#include "bspchip.h"
#include "plr_dram_gen2.h"
#include "plr_dram_gen2_memctl.h"

//#include "rtk_soc_common.h"
//#include "memctl.h"

#ifdef MEMCTL_DDR3_SUPPORT
/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

unsigned int memctlc_is_DDR3(void);
void memctlc_ddr3_dll_reset(void);
//void _DTR_DDR3_MRS_setting(unsigned int *sug_dtr, unsigned int *mr);


void memctlc_ddr3_dll_reset(void)
{
	volatile unsigned int *dmcr, *dtr0;
	volatile unsigned int delay_time;
	unsigned int dtr[3] __attribute__((unused));
	unsigned int mr[4];
	//unsigned int odt_value, ocd_value;

	dmcr = (volatile unsigned int *)DMCR;
	dtr0 = (volatile unsigned int *)DTR0;
	
	dtr[0]= *dtr0;
	dtr[1]= *(dtr0 + 1);
	dtr[2]= *(dtr0 + 2);

#if 0
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
	printf("mr[0]=0x%08x\n", mr[0]);
	printf("mr[1]=0x%08x\n", mr[1]);
	printf("mr[2]=0x%08x\n", mr[2]);
	printf("mr[3]=0x%08x\n", mr[3]);
#endif 
    
    mr[0] = DRAMI.DDR3_mr0;
    mr[1] = DRAMI.DDR3_mr1;
    mr[2] = DRAMI.DDR3_mr2;
    mr[3] = DRAMI.DDR3_mr3;

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

	/* 4.1 Set mr1@DDR3, to cover DLL disable case */
	*dmcr = mr[1];
	while(*dmcr & DMCR_MRS_BUSY);

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


/* Function Name: 
 * 	memctlc_is_DDR3
 * Descripton:
 *	Determine whether the DRAM type is DDR3 SDRAM.
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
	if(MCR_DRAMTYPE_DDR3 == (REG32(MCR) & MCR_DRAMTYPE_MASK))
		return 1;
	else
		return 0;
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
#if 0 
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
#endif /* #if 0 */

#endif
