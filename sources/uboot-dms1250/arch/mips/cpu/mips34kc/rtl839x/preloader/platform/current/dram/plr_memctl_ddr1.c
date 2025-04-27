#include <preloader.h>
//#include "bspchip.h"
#include "plr_dram_gen2.h"
#include "plr_dram_gen2_memctl.h"

//#include "rtk_soc_common.h"
//#include "memctl.h"

#ifdef MEMCTL_DDR1_SUPPORT

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */
unsigned int memctlc_is_DDR(void);
void memctlc_ddr1_dll_reset(void);
//void _DTR_DDR1_MRS_setting(unsigned int *sug_dtr, unsigned int *mr);
//static void _DTR_DDR1_MRS_setting(unsigned int *mr);



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

#if 0
	//_DTR_DDR1_MRS_setting(mr); //_DTR_DDR1_MRS_setting(dtr, mr);
#endif

	mr[0] = DRAMI.DDR1_mr;
	mr[1] = DRAMI.DDR1_emr;


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

	/* 4.1 Set mr0(emr@DDR1), to cover DLL disable case */
	*dmcr = mr[1];
	while(*dmcr & DMCR_MRS_BUSY);

	/* 5. Normal mode, avoid to reset DLL when updating phy params */
	*dmcr = mr[0];
	while(*dmcr & DMCR_MRS_BUSY);

	/* 6. reset phy fifo */
	memctlc_dram_phy_reset();

	return;
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
	if(MCR_DRAMTYPE_DDR == (REG32(MCR) & MCR_DRAMTYPE_MASK))
		return 1;
	else
		return 0;
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
//void _DTR_DDR1_MRS_setting(unsigned int *sug_dtr, unsigned int *mr)
void _DTR_DDR1_MRS_setting(unsigned int *mr)
{
	unsigned int cas, buswidth;
	/* Default value of Mode registers */
	mr[0] = DMCR_MRS_MODE_MR | DDR1_MR_BURST_SEQ | DDR1_MR_OP_NOR |\
		DMCR_MR_MODE_EN ;

	mr[1] = DMCR_MRS_MODE_EMR1 | DDR1_EMR1_DLL_EN | DDR1_EMR1_DRV_NOR |\
		DMCR_MR_MODE_EN;

	/* Extract CAS and WR in DTR0 */
	cas = (REG32(DTR0) & DTR0_CAS_MASK) >> DTR0_CAS_FD_S;
	buswidth = (REG32(DCR) & DCR_DBUSWID_MASK) >> DCR_DBUSWID_FD_S;
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
#endif

