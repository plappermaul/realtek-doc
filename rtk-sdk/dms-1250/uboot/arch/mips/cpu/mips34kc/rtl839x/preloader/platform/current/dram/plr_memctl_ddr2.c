#include <preloader.h>
//#include "bspchip.h"
#include "plr_dram_gen2.h"
#include "plr_dram_gen2_memctl.h"

//#include "rtk_soc_common.h"
//#include "memctl.h"

#ifdef MEMCTL_DDR2_SUPPORT

const unsigned int ddr2_dtr_para[] = { 5/*CAS(tCLK)*/,15/*WR(ns)*/,5/*CWL(tCLK)*/,8/*RTP(ns)*/,8/*WTR(ns)*/,
                                       7800/*REFI(us)*/,13/*RP(ns)*/,13/*RCD(ns)*/,8/*RRD(ns)*/,
                                       35/*FAWG(ns)*/,128/*RFC(ns)*/,45/*RAS(ns)*/};
#if 0
const unsigned int ddr2_dtr_para[] = { 4/*CAS(tCLK)*/,15/*WR(ns)*/,3/*CWL(tCLK)*/,8/*RTP(ns)*/,8/*WTR(ns)*/,
                                       7800/*REFI(us)*/,15/*RP(ns)*/,15/*RCD(ns)*/,10/*RRD(ns)*/,
                                       50/*FAWG(ns)*/,195/*RFC(ns)*/,45/*RAS(ns)*/};
#endif

//void _DTR_DDR2_MRS_setting(unsigned int *sug_dtr, unsigned int *mr);
void memctlc_ddr2_mrs_setting(void);
unsigned int memctlc_is_DDR2(void);


void memctlc_ddr2_mrs_setting(void)
{
	volatile unsigned int *dmcr;
	volatile unsigned int delay_time;
	unsigned int mr[4];
    //unsigned int odt_value;

	dmcr = (volatile unsigned int *)DMCR;
#if 0
	volatile unsigned int *dtr0;
    unsigned int dtr[3];	
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
#endif 

    mr[0] = DRAMI.DDR2_mr;
    mr[1] = DRAMI.DDR2_emr1;
    mr[2] = DRAMI.DDR2_emr2;
    mr[3] = DRAMI.DDR2_emr3;

#if 0
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

#else
	/* Power-up Initialization
	* ...
    * e) Issue an EMRS command to EMR(2).
    * f) Issue an EMRS command to EMR(3).
    * g) Issue EMRS to enable DLL. 
    * h) Issue a Mode Register Set command for DLL reset.
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
    
	*dmcr = mr[1] | DDR2_EMR1_DLL_DIS;
	while(*dmcr & DMCR_MRS_BUSY);

	/* Enable DLL */
	*dmcr = mr[1] & (~DDR2_EMR1_DLL_DIS);
	while(*dmcr & DMCR_MRS_BUSY);
    
	/* Reset DLL */
    *dmcr = (*dmcr & 0xFFFCFFFF);
	while(*dmcr & DMCR_MRS_BUSY);
    
	*dmcr = mr[0] | DDR2_MR_DLL_RESET_YES ;
	while(*dmcr & DMCR_MRS_BUSY);

	/* Waiting 200 clock cycles */
	delay_time = 0x2000;
	while(delay_time--);


	/* Set emr1@DDR1, to cover DLL disable case */
	*dmcr = mr[1];
	while(*dmcr & DMCR_MRS_BUSY);    
#endif

	/* reset phy fifo */
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

typedef union {
    struct {
        unsigned int dummy:19;
        unsigned int pd:1;
        unsigned int wr:3;
        unsigned int dll:1;
        unsigned int tm:1;
        unsigned int cas:3;
        unsigned int bt:1;
        unsigned int bl:3;
    } f;
    unsigned int v;
} mrs_ddr2_mr_t;

typedef union {
    struct {
        unsigned int dummy:19;
        unsigned int qoff:1;
        unsigned int rdqs:1;
        unsigned int _dqs:1;
        unsigned int ocd:3;
        unsigned int rtt1:1;
        unsigned int al:3;
        unsigned int rtt_0:1;
        unsigned int dic:1;
        unsigned int dll:1;
    } f;
    unsigned int v;
} mrs_ddr2_emr1_t;

typedef union {
    struct {
        unsigned int dummy:24;
        unsigned int srf:1;
        unsigned int dummy1:3;
        unsigned int dcc:1;
        unsigned int pasr:3;
    } f;
    unsigned int v;
} mrs_ddr2_emr2_t;

typedef union {
    struct {
        unsigned int dummy:32;
    } f;
    unsigned int v;
} mrs_ddr2_emr3_t;

void memctlc_ddr2_gen_DTR_and_MRS(unsigned int freq) 
{
	u32_t cas, wr;
    memctrl_dtr0_t dtr0={.v=0};
    memctrl_dtr1_t dtr1={.v=0};    
    memctrl_dtr2_t dtr2={.v=0};

    if (freq>266 && freq<=400)
        cas=ddr2_dtr_para[0]+1;
    else {
        if (freq<125 && freq>400) {
            printf("Warning: frequency(%dMHz) is out of range\n", freq);
        }
        cas=ddr2_dtr_para[0];
    }
    dtr0.f.cas=cas-1;
    wr=NS2CLK_ROUNDUP(freq,ddr2_dtr_para[1]);
    dtr0.f.wr=wr-1;
    dtr0.f.cwl=dtr0.f.cas-1;
    dtr0.f.rtp=NS2CLK_ROUNDUP(freq,ddr2_dtr_para[3])-1;
    dtr0.f.wtr=NS2CLK_ROUNDUP(freq,ddr2_dtr_para[4])-1;
    memctlc_set_refi(&(dtr0.v), NS2CLK_ROUNDUP(freq,ddr2_dtr_para[5]));
    dtr1.f.rp=NS2CLK_ROUNDUP(freq,ddr2_dtr_para[6])-1;
    dtr1.f.rcd=NS2CLK_ROUNDUP(freq,ddr2_dtr_para[7])-1;
    dtr1.f.rrd=NS2CLK_ROUNDUP(freq,ddr2_dtr_para[8])-1;
    dtr1.f.fawg=NS2CLK_ROUNDUP(freq,ddr2_dtr_para[9])-1;
    dtr2.f.rfc=NS2CLK_ROUNDUP(freq,ddr2_dtr_para[10])-1;
    dtr2.f.ras=NS2CLK_ROUNDUP(freq,ddr2_dtr_para[11])-1;

	parameter_soc_rwp->dram_info.dtr0=dtr0.v;
    parameter_soc_rwp->dram_info.dtr1=dtr1.v;
    parameter_soc_rwp->dram_info.dtr2=dtr2.v;

    printf("DTR0, CL(%d) WR(%d) CWL(%d) RTP(%d) WTR(%d) REFI(%d*%d)\n", 
        dtr0.f.cas+1, dtr0.f.wr+1, dtr0.f.cwl+1, dtr0.f.rtp+1,
        dtr0.f.wtr+1, dtr0.f.refi, dtr0.f.refiunit);
    printf("DTR1, RP(%d) RCD(%d) RRD(%d) FAWG(%d)\nDTR2, RFC(%d) RAS(%d)\n", 
        dtr1.f.rp+1, dtr1.f.rcd+1, dtr1.f.rrd+1, dtr1.f.fawg+1,
        dtr2.f.rfc+1, dtr2.f.ras+1);

	// SYNC MRS
    mrs_ddr2_mr_t mr0={.v=0};
    mrs_ddr2_emr1_t mr1={.v=0};
    mrs_ddr2_emr2_t mr2={.v=0};
    mrs_ddr2_emr3_t mr3={.v=0};

    mr0.f.wr=wr-1;
    mr0.f.cas=cas;
	mr0.v|= DMCR_MR_MODE_EN|DMCR_MRS_MODE_MR|DDR2_MR_BURST_4;
    printf("mr0.f.cas(%d) mr0.f.wr(%d) mr0.f.pd(%d)\n", mr0.f.cas, mr0.f.wr, mr0.f.pd);
    
    mr1.v|= DDR2_EMR1_RTT_150|DMCR_MR_MODE_EN|DMCR_MRS_MODE_EMR1;

    mr2.v|= DMCR_MR_MODE_EN|DMCR_MRS_MODE_EMR2;

    mr3.v|= DMCR_MR_MODE_EN|DMCR_MRS_MODE_EMR3;

    parameter_soc_rwp->dram_info.DDR2_mr=mr0.v;
    parameter_soc_rwp->dram_info.DDR2_emr1=mr1.v;
    parameter_soc_rwp->dram_info.DDR2_emr2=mr2.v;
    parameter_soc_rwp->dram_info.DDR2_emr3=mr3.v;
    printf("mr0(0x%08x), mr1(0x%08x), mr2(0x%08x), mr3(0x%08x)\n",mr0.v,mr1.v,mr2.v,mr3.v );

}

#endif /* end of MEMCTL_DDR2_SUPPORT */
