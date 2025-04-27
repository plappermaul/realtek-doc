#include <preloader.h>
//#include "bspchip.h"
#include "plr_dram_gen2.h"
#include "plr_dram_gen2_memctl.h"

//#include "rtk_soc_common.h"
//#include "memctl.h"

#ifdef MEMCTL_DDR2_SUPPORT

//void _DTR_DDR2_MRS_setting(unsigned int *sug_dtr, unsigned int *mr);
void memctlc_ddr2_dll_reset(void);
unsigned int memctlc_is_DDR2(void);


void memctlc_ddr2_dll_reset(void)
{
	volatile unsigned int *dmcr, *dtr0;
	volatile unsigned int delay_time;
	unsigned int dtr[3] __attribute__((unused));
	unsigned int mr[4];
    //unsigned int odt_value;

	dmcr = (volatile unsigned int *)DMCR;
	dtr0 = (volatile unsigned int *)DTR0;
	
	dtr[0]= *dtr0;
	dtr[1]= *(dtr0 + 1);
	dtr[2]= *(dtr0 + 2);

#if 0
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
#endif 

    mr[0] = DRAMI.DDR2_mr;
    mr[1] = DRAMI.DDR2_emr1;
    mr[2] = DRAMI.DDR2_emr2;
    mr[3] = DRAMI.DDR2_emr3;

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
	delay_time = 0x2000;
	while(delay_time--);

	/* 4.1 Set emr1@DDR1, to cover DLL disable case */
	*dmcr = mr[1];
	while(*dmcr & DMCR_MRS_BUSY);

	/* 5. Set EMR2 */
	*dmcr = mr[2];
	while(*dmcr & DMCR_MRS_BUSY);

	/* 5.1 Set EMR3 */
	*dmcr = mr[3];
	while(*dmcr & DMCR_MRS_BUSY);



	/* 6. reset phy fifo */
	memctlc_dram_phy_reset();

	return;
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
	if(MCR_DRAMTYPE_DDR2 == (REG32(MCR) & MCR_DRAMTYPE_MASK))
		return 1;
	else
		return 0;
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
#if 0
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
#endif /* #if 0 */

#endif /* end of MEMCTL_DDR2_SUPPORT */
