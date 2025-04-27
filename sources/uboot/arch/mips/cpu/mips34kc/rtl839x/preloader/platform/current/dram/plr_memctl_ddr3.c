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
const unsigned int ddr3_dtr_para[] = { 5/*CAS(tCLK)*/,15/*WR(ns)*/,5/*CWL(tCLK)*/,4/*RTP(tCLK)*/,4/*WTR(tCLK)*/,
                                       7800/*REFI(us)*/,15/*RP(ns)*/,15/*RCD(ns)*/,4/*RRD(tCLK)*/,
                                       50/*FAWG(ns)*/,160/*RFC(ns)*/,38/*RAS(ns)*/};

/*
 * Function Declaration
 */
unsigned int memctlc_is_DDR3(void);
void memctlc_ddr3_mrs_setting(void);

void memctlc_ddr3_mrs_setting(void)
{
	volatile unsigned int *dmcr;
	volatile unsigned int delay_time;
	unsigned int mr[4];
	//unsigned int odt_value, ocd_value;

	dmcr = (volatile unsigned int *)DMCR;

#if 0
    volatile unsigned int *dtr0;
    unsigned int dtr[3];
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
	printf("mr[0]=0x%08x\n", mr[0]);
	printf("mr[1]=0x%08x\n", mr[1]);
	printf("mr[2]=0x%08x\n", mr[2]);
	printf("mr[3]=0x%08x\n", mr[3]);
#endif 
    
    mr[0] = DRAMI.DDR3_mr0;
    mr[1] = DRAMI.DDR3_mr1;
    mr[2] = DRAMI.DDR3_mr2;
    mr[3] = DRAMI.DDR3_mr3;


#if 0
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

#else
	/* Power-up Initialization
	* ...
    * 6. Issue MRS Command to load MR2 with all application settings. 
    * 7. Issue MRS Command to load MR3 with all application settings.
    * 8. Issue MRS Command to load MR1 with all application settings and DLL enabled.
    * 9. Issue MRS Command to load MR0 with all application settings and ¡§DLL reset¡¨.
    * 10. Issue ZQCL command to starting ZQ calibration.    
    */

	/* Set EMR2 */
	*dmcr = (*dmcr & 0xFFFCFFFF) | 0x20000;
	while(*dmcr & DMCR_MRS_BUSY);
    
	*dmcr = mr[2];
	while(*dmcr & DMCR_MRS_BUSY);

	/* Set EMR3 */
    *dmcr = (*dmcr & 0xFFFCFFFF) | 0x30000;
	while(*dmcr & DMCR_MRS_BUSY);
    
	*dmcr = mr[3];
	while(*dmcr & DMCR_MRS_BUSY);

	/* Disable DLL */
    *dmcr = (*dmcr & 0xFFFCFFFF) | 0x10000;
	while(*dmcr & DMCR_MRS_BUSY);
    
	*dmcr = mr[1] | DDR3_EMR1_DLL_DIS;
	while(*dmcr & DMCR_MRS_BUSY);

	/* Enable DLL */
	*dmcr = mr[1] & (~DDR3_EMR1_DLL_DIS);
	while(*dmcr & DMCR_MRS_BUSY);

    /* Reset DLL */
    *dmcr = (*dmcr & 0xFFFCFFFF);
	while(*dmcr & DMCR_MRS_BUSY);
    
	*dmcr = mr[0] | DDR3_MR_DLL_RESET_YES ;
	while(*dmcr & DMCR_MRS_BUSY);

	/* Waiting 200 clock cycles */
	delay_time = 0x1000;
	while(delay_time--);    

	/* Set mr1@DDR3, to cover DLL disable case */
	*dmcr = mr[1];
	while(*dmcr & DMCR_MRS_BUSY);
    
#endif

	/* reset phy fifo */
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

typedef union {
    struct {
        unsigned int dummy:19;
        unsigned int ppd:1;
        unsigned int wr:3;
        unsigned int dll:1;
        unsigned int tm:1;
        unsigned int cas3_1:3;
        unsigned int rbt:1;
        unsigned int cas0:1;
        unsigned int bl:2;
    } f;
    unsigned int v;
} mrs_ddr3_mr0_t;

typedef union {
    struct {
        unsigned int dummy:19;
        unsigned int qoff:1;
        unsigned int tdqs:1;
        unsigned int dummy1:1;
        unsigned int rtt_nom_2:1;
        unsigned int dummy2:1;
        unsigned int level:1;
        unsigned int rtt_nom_1:1;
        unsigned int dic_1:1;
        unsigned int al:2;
        unsigned int rtt_nom_0:1;
        unsigned int dic_0:1;
        unsigned int dll:1;
    } f;
    unsigned int v;
} mrs_ddr3_mr1_t;

typedef union {
    struct {
        unsigned int dummy:21;
        unsigned int rtt_wr:2;
        unsigned int dummy1:1;
        unsigned int srt:1;
        unsigned int asr:1;
        unsigned int cwl:3;
        unsigned int pasr:3;
    } f;
    unsigned int v;
} mrs_ddr3_mr2_t;

typedef union {
    struct {
        unsigned int dummy:29;
        unsigned int mpr:1;
        unsigned int mpr_loc:2;
    } f;
    unsigned int v;
} mrs_ddr3_mr3_t;

void memctlc_ddr3_gen_DTR_and_MRS(unsigned int freq) 
{
	u32_t cas, wr, cwl;
    memctrl_dtr0_t dtr0={.v=0};
    memctrl_dtr1_t dtr1={.v=0};    
    memctrl_dtr2_t dtr2={.v=0};

    if (freq>333 && freq<=400) {
        cas=ddr3_dtr_para[0]+1;
    } else {
        if (freq<125 && freq>400) 
            printf("Warning: frequency(%dMHz) is out of range\n", freq);
        cas=ddr3_dtr_para[0];
    }
    dtr0.f.cas=cas-1;
    wr=NS2CLK_ROUNDUP(freq,ddr3_dtr_para[1]);
    dtr0.f.wr=wr-1;
    cwl=ddr3_dtr_para[2];
    dtr0.f.cwl=cwl-1;
    dtr0.f.rtp=ddr3_dtr_para[3]-1;
    dtr0.f.wtr=ddr3_dtr_para[4]-1;
    memctlc_set_refi(&(dtr0.v), NS2CLK_ROUNDUP(freq,ddr3_dtr_para[5]));
    dtr0.f.dummy=0;
    dtr1.f.rp=NS2CLK_ROUNDUP(freq,ddr3_dtr_para[6])-1;
    dtr1.f.rcd=NS2CLK_ROUNDUP(freq,ddr3_dtr_para[7])-1;
    dtr1.f.rrd=ddr3_dtr_para[8]-1;
    dtr1.f.fawg=NS2CLK_ROUNDUP(freq,ddr3_dtr_para[9])-1;
    dtr2.f.rfc=NS2CLK_ROUNDUP(freq,ddr3_dtr_para[10])-1;
    dtr2.f.ras=NS2CLK_ROUNDUP(freq,ddr3_dtr_para[11])-1;

	parameter_soc_rwp->dram_info.dtr0=dtr0.v;
    parameter_soc_rwp->dram_info.dtr1=dtr1.v;
    parameter_soc_rwp->dram_info.dtr2=dtr2.v;

    /*printf("DTR0, CL(%d) WR(%d) CWL(%d) RTP(%d) WTR(%d) REFI(%d*%d)\n", 
        dtr0.f.cas+1, dtr0.f.wr+1, dtr0.f.cwl+1, dtr0.f.rtp+1,
        dtr0.f.wtr+1, dtr0.f.refi, dtr0.f.refiunit);
    printf("DTR1, RP(%d) RCD(%d) RRD(%d) FAWG(%d)\nDTR2, RFC(%d) RAS(%d)\n", 
        dtr1.f.rp+1, dtr1.f.rcd+1, dtr1.f.rrd+1, dtr1.f.fawg+1,
        dtr2.f.rfc+1, dtr2.f.ras+1);*/

	// SYNC MRS
    mrs_ddr3_mr0_t mr0={.v=0};
    mrs_ddr3_mr1_t mr1={.v=0};
    mrs_ddr3_mr2_t mr2={.v=0};
    mrs_ddr3_mr3_t mr3={.v=0};

    mr0.f.ppd=1;
    mr0.f.wr=(wr-4);
    mr0.f.cas3_1=(cas-4);
	mr0.v|= DMCR_MR_MODE_EN|DMCR_MRS_MODE_MR;
    //printf("mr0.f.cas3_1(%d) mr0.f.wr(%d) mr0.f.ppd(%d)\n", mr0.f.cas3_1, mr0.f.wr, mr0.f.ppd);
    
    mr1.v|= DDR3_EMR1_RTT_NOM_RZQ_DIV2|DMCR_MR_MODE_EN|DMCR_MRS_MODE_EMR1;

    mr2.f.cwl=cwl-5;
    mr2.v|= DMCR_MR_MODE_EN|DMCR_MRS_MODE_EMR2;

    mr3.v|= DMCR_MR_MODE_EN|DMCR_MRS_MODE_EMR3;

    parameter_soc_rwp->dram_info.DDR3_mr0=mr0.v;
    parameter_soc_rwp->dram_info.DDR3_mr1=mr1.v;
    parameter_soc_rwp->dram_info.DDR3_mr2=mr2.v;
    parameter_soc_rwp->dram_info.DDR3_mr3=mr3.v;
    //printf("mr0(0x%08x), mr1(0x%08x), mr2(0x%08x), mr3(0x%08x)\n",mr0.v,mr1.v,mr2.v,mr3.v );
}

#endif
