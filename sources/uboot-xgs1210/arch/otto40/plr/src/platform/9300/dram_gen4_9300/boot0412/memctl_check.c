#include "./bspchip.h"
#include "./memctl.h"
#include "./memctl_func.h"

/*Board DQS Range (mode, MHz, min tap, max tap)*/
/* Mode: DLL/Digital */
tap_info_t DQS_RANGE[10] =	{
					/*100MHz*/{DLL_MODE, 100, 3, 16} , {DIGITAL_MODE, 100, 1, 23}, 
					/*125MHz*/{DLL_MODE, 125, 3, 13} , {DIGITAL_MODE, 125, 1, 18},
					/*150MHz*/{DLL_MODE, 150, 3, 12} , {DIGITAL_MODE, 150, 1, 13},
					/*175MHz*/{DLL_MODE, 175, 3, 11} , {DIGITAL_MODE, 175, 1, 10},
					/*200MHz*/{DLL_MODE, 200, 4, 10} , {DIGITAL_MODE, 200, 1, 8}
				};
/*28M Demoboard: Write 90 phase range (mode, MHz, min tap, max tap) */
tap_info_t W90_RANGE[10] =	{
					/*100MHz*/{DLL_MODE, 100, 0, 19} , {DIGITAL_MODE, 100, 0, 19},
					/*125MHz*/{DLL_MODE, 125, 0, 16} , {DIGITAL_MODE, 125, 0, 16},
					/*150MHz*/{DLL_MODE, 150, 0, 14} , {DIGITAL_MODE, 150, 0, 14},
					/*175MHz*/{DLL_MODE, 175, 1, 13} , {DIGITAL_MODE, 175, 1, 13},
					/*200MHz*/{DLL_MODE, 200, 2, 12} , {DIGITAL_MODE, 200, 2, 12},
				};

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
	unsigned int reg_v, odtn, odtp, ocdn, ocdp;

	zq_cali_reg = (volatile unsigned int *)0xB8001094;
	zq_cali_status_reg = (volatile unsigned int *)0xB8001098;

	printf("Checking ZQ calibration Status:\n\r");
	printf("\tZQ Program(0x%08X): 0x%08x\n\r", (unsigned int)zq_cali_reg, *zq_cali_reg);
	printf("\tZQ Status (0x%08X): 0x%08x\n\r", (unsigned int)zq_cali_status_reg, *zq_cali_status_reg);

	reg_v = *zq_cali_status_reg;   
	if(reg_v & 0x20000000){
		odtp = ((reg_v >> 27) & 0x3);        
		odtn = ((reg_v >> 25) & 0x3);        
		ocdp = ((reg_v >> 23) & 0x3);        
		ocdn = ((reg_v >> 21) & 0x3);   

		if((odtp != 0) || /* ODTP must be completed with no error */          
			(odtn == 2) || /* ODTN must NOT be underflow (may tolerate code overflow error) */           
			(ocdp == 1) || /* OCDP must NOT be overflow (may tolerate code underflow error) */           
			(ocdn == 1))   /* OCDN must NOT be overflow (may tolerate code underflow error) */  { 
			printf("\tZQ calibration Fail!\n\r");	    
		}else{
			printf("\tZQ Calibration Pass\n\r");
		} 
		
		
	}
	else{
		printf("\tZQ calibration Pass\n\r");
	}
}				
				
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
//	unsigned int rowcnt;
	unsigned int clk_ns;

	refi      = 1 + ((REG32(DTR0) & DTR0_REFI_MASK) >> DTR0_REFI_FD_S);
	refi_unit = 32 << (((REG32(DTR0) & DTR0_REFI_UNIT_MASK) >> DTR0_REFI_UNIT_FD_S));

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
unsigned int _DTR_refresh_time_ns(unsigned int dram_freq)
{
	unsigned int refi, refi_unit;
	unsigned int refi_ns;

	refi      = 1 + ((REG32(DTR0) & DTR0_REFI_MASK) >> DTR0_REFI_FD_S);
	refi_unit = 32 << (((REG32(DTR0) & DTR0_REFI_UNIT_MASK) >> DTR0_REFI_UNIT_FD_S));

	refi_ns = (1000/dram_freq) * refi_unit * refi;
	return refi_ns;	
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
	wtr = 1 + ((REG32(DTR0) & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);

	return ((1000*wtr)/wtr_ns); /* return number of MHz */
}


/* Function Name: 
 * 	_DTR_wtr_time_ns
 * Descripton:
 *	Calculate the DRAM t_wtr in ns unit
 * Input:
 *	rp_ns	- tWTR requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(WTR) setting.
 */
unsigned int _DTR_wtr_time_ns(unsigned int dram_freq)
{
	unsigned int wtr, wtr_ns;

	/* get tRP value */
	wtr = 1 + ((REG32(DTR0) & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);

	wtr_ns = (1000/dram_freq)*wtr;

	return (wtr_ns); /* return number of nano second */
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
	rtp = 1 + ((REG32(DTR0) & DTR0_RTP_MASK) >> DTR0_RTP_FD_S);

	return ((1000*rtp)/rtp_ns); /* return number of MHz */
}


/* Function Name: 
 * 	_DTR_rtp_time_ns
 * Descripton:
 *	Calculate the DRAM t_rtp in ns unit
 * Input:
 *	rp_ns	- tRP requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rp) setting.
 */
unsigned int _DTR_rtp_time_ns(unsigned int dram_freq)
{
	unsigned int rtp, rtp_ns;

	/* get tRP value */
	rtp = 1 + ((REG32(DTR0) & DTR0_RTP_MASK) >> DTR0_RTP_FD_S);

	rtp_ns = (1000/dram_freq) * rtp;

	return (rtp_ns); /* return number of nano second */
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
	rp = 1 + ((REG32(DTR1) & DTR1_RP_MASK) >> DTR1_RP_FD_S);

	return ((1000*rp)/rp_ns); /* return number of MHz */
}

/* Function Name: 
 * 	_DTR_rp_time_ns
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rp) setting.
 * Input:
 *	dram_freq	- tRP requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rp) setting.
 */
unsigned int _DTR_rp_time_ns(unsigned int dram_freq)
{
	unsigned int rp, rp_ns;

	/* get tRP value */
	rp = 1 + ((REG32(DTR1) & DTR1_RP_MASK) >> DTR1_RP_FD_S);

	rp_ns = (1000/dram_freq)*rp;

	return (rp_ns); /* return number of nano second */
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
	rrd = 1 + ((REG32(DTR1) & DTR1_RRD_MASK) >> DTR1_RRD_FD_S);

	return ((1000*rrd)/rrd_ns); /* return number of MHz */
}

/* Function Name: 
 * 	_DTR_rrd_time_ns
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rcd) setting.
 * Input:
 *	rrd_ns	- tRRD requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rrd) setting.
 */
unsigned int _DTR_rrd_time_ns(unsigned int dram_freq)
{
	unsigned int rrd, rrd_ns;

	/* get tRCD value */
	rrd = 1 + ((REG32(DTR1) & DTR1_RRD_MASK) >> DTR1_RRD_FD_S);

	rrd_ns = (1000/dram_freq)* rrd; 

	return (rrd_ns); /* return number of nano second */
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
	fawg = 1 + ((REG32(DTR1) & DTR1_FAWG_MASK) >> DTR1_FAWG_FD_S);

	return ((1000*fawg)/fawg_ns); /* return number of MHz */
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
unsigned int _DTR_fawg_time_ns(unsigned int dram_freq)
{
	unsigned int fawg, fawg_ns;

	/* get tRCD value */
	fawg = 1 + ((REG32(DTR1) & DTR1_FAWG_MASK) >> DTR1_FAWG_FD_S);

	fawg_ns = (1000/dram_freq)*fawg;

	return (fawg_ns); /* return number of nano second */
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
	rcd = 1 + ((REG32(DTR1) & DTR1_RCD_MASK) >> DTR1_RCD_FD_S);

	return ((1000*rcd)/rcd_ns); /* return number of MHz */
}

/* Function Name: 
 * 	_DTR_rcd_tims_ns
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rcd) setting.
 * Input:
 *	dram_freq	- tRCD requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rcd) setting.
 */
unsigned int _DTR_rcd_tims_ns(unsigned int dram_freq)
{
	unsigned int rcd, rcd_ns;

	/* get tRCD value */
	rcd = 1 + ((REG32(DTR1) & DTR1_RCD_MASK) >> DTR1_RCD_FD_S);

	rcd_ns = (1000/dram_freq)*rcd;

	return (rcd_ns); /* return number of nano second */
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
	ras = 1 + ((REG32(DTR2) & DTR2_RAS_MASK) >> DTR2_RAS_FD_S);

	return ((1000*ras)/ras_ns); /* return number of MHz */

}

/* Function Name: 
 * 	_DTR_ras_time_ns
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(ras) setting.
 * Input:
 *	ras_ns	- tRAS requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(ras) setting.
 */
unsigned int _DTR_ras_time_ns(unsigned int dram_freq)
{
	unsigned int ras, ras_ns;

	/* get tRAS value */
	ras = 1 + ((REG32(DTR2) & DTR2_RAS_MASK) >> DTR2_RAS_FD_S);

	ras_ns = (1000/dram_freq)*ras;

	return (ras_ns); /* return number of nano second */

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
	rfc = 1 + ((REG32(DTR2) & DTR2_RFC_MASK) >> DTR2_RFC_FD_S);

	return ((1000*rfc)/rfc_ns); /* return number of MHz */
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
unsigned int _DTR_rfc_time_ns(unsigned int dram_freq)
{
	unsigned int rfc, rfc_ns;

	/* get tRFC value */
	rfc = 1 + ((REG32(DTR2) & DTR2_RFC_MASK) >> DTR2_RFC_FD_S);

	rfc_ns = (1000/dram_freq)*rfc;

	return (rfc_ns); /* return number of nano second */
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
	wr = 1 + ((REG32(DTR0) & DTR0_WR_MASK) >> DTR0_WR_FD_S);
#if 0
	if(wr <= 2){
		printf("\t#Warnning: wr < 2 too small.\n\r");
		return 0;
	}
	else
#endif
	return ((1000*wr)/wr_ns); /* return number of MHz */
}

/* Function Name: 
 * 	_DTR_wr_time_ns
 * Descripton:
 *	Calculate the DRAM t_wr in ns unit
 * Input:
 *	dram_freq - Dram frequency in MHz.
 * Output:
 * 	None
 * Return:
 *	Real DRAM t_wr on demo board.
 * Note:
 *	Require add 2 more DRAM clock cycles in the current design.
 */
unsigned int _DTR_wr_time_ns(unsigned int dram_freq)
{
	unsigned int wr, wr_ns;

	/* get tWR value */
	wr = 1 + ((REG32(DTR0) & DTR0_WR_MASK) >> DTR0_WR_FD_S);

	wr_ns = (1000/dram_freq) * wr;

	return wr_ns; /* return number of nano second */
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

	wr = ((REG32(DTR0) & DTR0_WR_MASK) >> DTR0_WR_FD_S);

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

	rrd = ((REG32(DTR1) & DTR1_RRD_MASK) >> DTR1_RRD_FD_S);

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

	rtp = ((REG32(DTR0) & DTR0_RTP_MASK) >> DTR0_RTP_FD_S);

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

	wtr = ((REG32(DTR0) & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);

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

	cwl = ((REG32(DTR0) & DTR0_CWL_MASK) >> DTR0_CWL_FD_S);

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

	cas = ((REG32(DTR0) & DTR0_CAS_MASK) >> DTR0_CAS_FD_S);

	switch (cas) {
		case 0:
			return 5; /* 2.5 x 2 */
		default:
			return (cas+1)*2;
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

	chipsel = ((REG32(DCR) & DCR_DCHIPSEL_MASK) >> DCR_DCHIPSEL_FD_S);

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

	rowcnt = ((REG32(DCR) & DCR_ROWCNT_MASK) >> DCR_ROWCNT_FD_S);
	
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

	colcnt = ((REG32(DCR) & DCR_COLCNT_MASK) >> DCR_COLCNT_FD_S);

	if(4 < colcnt){
		printf("#Eror: error colcnt setting. ( > 4)\n\r");		
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

	bankcnt = ((REG32(DCR) & DCR_BANKCNT_MASK) >> DCR_BANKCNT_FD_S);
	
	switch (bankcnt)
	{
		case 0:
			return 2;
		case 1:
			return 4;
		case 2:
			return 8;
	}
	
	return 2;
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

	buswidth = ((REG32(DCR) & DCR_DBUSWID_MASK) >> DCR_DBUSWID_FD_S);

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
	unsigned int clk_10_ns;
	
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
		return ;

count_dtr:
	sug_dtr[0] = ((cas << DTR0_CAS_FD_S)&DTR0_CAS_MASK) | ((wr << DTR0_WR_FD_S)&DTR0_WR_MASK) |\
		     ((rtp << DTR0_RTP_FD_S)&DTR0_RTP_MASK) | ((wtr << DTR0_WTR_FD_S)&DTR0_WTR_MASK) |\
		     ((refi << DTR0_REFI_FD_S)&DTR0_REFI_MASK) | ((refi_unit << DTR0_REFI_UNIT_FD_S)&DTR0_REFI_UNIT_MASK);
	sug_dtr[1] = ((rp << DTR1_RP_FD_S)&DTR1_RP_MASK) | ((rcd << DTR1_RCD_FD_S)&DTR1_RCD_MASK) |\
		     ((rrd << DTR1_RRD_FD_S)&DTR1_RRD_MASK) | ((fawg << DTR1_FAWG_FD_S)&DTR1_FAWG_MASK) ;
	sug_dtr[2] = ((rfc << DTR2_RFC_FD_S)&DTR2_RFC_MASK) | ((ras << DTR2_RAS_FD_S)&DTR2_RAS_MASK);
	/*dtr_value = ((rp << DTR_RP_FD_S)&DTR_RP_MASK) | ((rcd << DTR_RCD_FD_S)&DTR_RCD_MASK) | ((ras << DTR_RAS_FD_S)&DTR_RAS_MASK) \
		| ((rfc << DTR_RFC_FD_S)&DTR_RFC_MASK) | ((wr << DTR_WR_FD_S)&DTR_WR_MASK) | ((refi << DTR_REFI_FD_S)&DTR_REFI_MASK) \
		| ((refi_unit << DTR_REFI_UNIT_FD_S)&DTR_REFI_UNIT_MASK);*/

	return;
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
	
	dqs0_tap = (REG32(DDCR) & DDCR_DQS0_TAP_MASK) >> DDCR_DQS0_TAP_FD_S;
	dqs1_tap = (REG32(DDCR) & DDCR_DQS1_TAP_MASK) >> DDCR_DQS1_TAP_FD_S;
	cal_mode = (REG32(DDCR) & DDCR_CAL_MODE_MASK) >> DDCR_CAL_MODE_FD_S;

	printf("DQ/DQS Related Delay:\n\r");
	if(cal_mode)
		printf("\tCAL_MODE: Digtial\n\r");
	else
		printf("\tCAL_MODE: DLL\n\r");


	printf("\tDQS0_tap = %d, DQS1_tap = %d : ", dqs0_tap, dqs1_tap);
	dram_freq_mhz = board_DRAM_freq_mhz();
	for(index = 0; index < (sizeof(DQS_RANGE)/sizeof(tap_info_t)); index++){
		if(DQS_RANGE[index].mode == cal_mode){
			if(DQS_RANGE[index].mhz >= dram_freq_mhz)
				break;
		}
	}

	if(index >= (sizeof(DQS_RANGE)/sizeof(tap_info_t)))
		printf("#Error DRAM frequency %dMHz\n\r", dram_freq_mhz);

	if(dqs0_tap < DQS_RANGE[index].tap_min || dqs0_tap > DQS_RANGE[index].tap_max \
			|| dqs1_tap < DQS_RANGE[index].tap_min || dqs1_tap > DQS_RANGE[index].tap_max)
		printf("#Warning: current DQS tap setting may not be stable\n\r");
	else
		printf("ok ");

	printf("(Reference DQS taps %d ~ %d on %dMHz)\n\r", DQS_RANGE[index].tap_min, DQS_RANGE[index].tap_max, DQS_RANGE[index].mhz);

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
	
	phase_tap = (REG32(DCDR) & DCDR_PHASE_SHIFT_MASK) >> DCDR_PHASE_SHIFT_FD_S;
	cal_mode = (REG32(DDCR) & DDCR_CAL_MODE_MASK) >> DDCR_CAL_MODE_FD_S;

	printf("\tphase_shift_90_tap = %d: ", phase_tap);

	dram_freq_mhz = board_DRAM_freq_mhz();

	for(index = 0; index < (sizeof(W90_RANGE)/sizeof(tap_info_t)); index++){
		if(W90_RANGE[index].mode == cal_mode){
			if(W90_RANGE[index].mhz >= dram_freq_mhz)
				break;
		}
	}

	if(index >= (sizeof(W90_RANGE)/sizeof(tap_info_t)))
		printf("#Error DRAM frequency %dMHz\n\r", dram_freq_mhz);

	if(phase_tap < W90_RANGE[index].tap_min || phase_tap > W90_RANGE[index].tap_max)
		printf("#Warnning: Phase shift 90 taps setting may not be stable\n\r");
	else
		printf("ok ");

	printf("(Reference phase shift 90 taps %d ~ %d on %dMHz)\n\r", W90_RANGE[index].tap_min, W90_RANGE[index].tap_max, W90_RANGE[index].mhz);

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

	unsigned int	buswidth, chipsel, rowcnt;
	unsigned int	colcnt, bankcnt, dram_total_size;
	unsigned int	dram_freq_mhz;


	/* Get board DRAM freq. */
	dram_freq_mhz = board_DRAM_freq_mhz(); 

	/* 
	 * Check DCR
	 */
	/* 1. Bus width     */
	buswidth = _DCR_get_buswidth();
	printf("\tbus width = %dbit\n\r", buswidth);
	/* 2. Chip select   */
	chipsel = _DCR_get_chipsel();
	printf("\tnumber of chips = %d\n\r", chipsel);
	/* 3. Row count     */
	rowcnt = _DCR_get_rowcnt();
	printf("\trow count = %d\n\r", rowcnt);
	/* 4. Column count  */
	colcnt = _DCR_get_colcnt();
	printf("\tcolumn count = %d\n\r", colcnt);
	/* 5. Bank count    */
	bankcnt = _DCR_get_bankcnt();
	printf("\tbank count = %d\n\r", bankcnt);

	/* 6. Show total DRAM size */
	dram_total_size = rowcnt*colcnt*(buswidth/8)*chipsel*bankcnt;
	printf("\tDRAM Size = %dMB\n\r", (dram_total_size/1024/1024));

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
#ifdef CONFIG_DDR3_USAGE
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

	printf("DTR Checking Rules: \n\r");
	printf("\tt_refi = %dns, \n\r", std_refi_ns);
	printf("\tt_wr = %dns\n\r", std_wr_ns);
	printf("\tt_rtp = %dns\n\r", std_rtp_ns);
	printf("\tt_wtr = %dns\n\r", std_wtr_ns);
	printf("\tt_rp = %dns\n\r", std_rp_ns);
	printf("\tt_rcd = %dns\n\r", std_rcd_ns);
	printf("\tt_fawg = %dns\n\r", std_fawg_ns);
	printf("\tt_ras = %dns\n\r", std_ras_ns);
	printf("\tt_rfc = %dns, _rfc_ns(%d)\n\r", std_rfc_ns, _rfc_ns);

	printf("Checking Current setting: \n\r");
	/* 1. CAS latency   */
	two_cas = _DTR_two_cas();
	printf("\tcas(%d.%d): ", (two_cas/2), ((two_cas*10)/2)%10);
	if(two_cas > 22)
		printf("#Warnning: No CAS > 11 for DDR3 SDRAM\n\r");
	else if((two_cas == 10) && (DDR3_CAS5_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((two_cas == 12) && (DDR3_CAS6_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((two_cas == 14) && (DDR3_CAS7_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((two_cas == 16) && (DDR3_CAS8_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((two_cas == 18) && (DDR3_CAS9_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((two_cas == 20) && (DDR3_CAS10_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((two_cas == 22) && (DDR3_CAS11_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else
		printf("ok\n\r");


	/* 2. T_WR  (15ns) */
	wr = _DTR_wr();
	printf("\twr: ");
	max_freq_mhz = _DTR_wr_frq_mhz(std_wr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR0(wr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_wr_ns, max_freq_mhz);
	if((wr < 5) | (wr > 12)){
		printf("#Warnning: DTR0(wr) setting (%d clks) may vilolates the min./max. requirement(5~12 clks) for current setting\n\r", wr+1);
	}else{
		printf("ok\n\r");
	}

	/* 3. CAS Write latency */
	tcwl = _DTR_cwl();
	printf("\tcwl(%d): ", tcwl);
	if(tcwl > 8)
		printf("#Warnning: No CWL > 8 for DDR3 SDRAM\n\r");
	else if((tcwl == 5) && (DDR3_CWL5_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CWL might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((tcwl == 6) && (DDR3_CWL6_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CWL might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((tcwl == 7) && (DDR3_CWL7_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CWL might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((tcwl == 8) && (DDR3_CAS8_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CWL might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else
		printf("ok\n\r");


	/* 4. T_RTP  (20ns) */
	rtp = _DTR_rtp();
	printf("\trtp: ");
	max_freq_mhz = _DTR_rtp_frq_mhz(std_rtp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rtp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rtp_ns, max_freq_mhz);

	if(rtp < 4){
		printf("#Warnning: DTR1(rtp) setting (%d clks) may vilolates the min. requirement(4 clks) for current setting\n\r", rtp);
	}else{
		printf("ok\n\r");
	}


	/* 5. T_WTR  (20ns) */
	wtr = _DTR_wtr();
	printf("\twtr: ");
	max_freq_mhz = _DTR_wtr_frq_mhz(std_wtr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(wtr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_wtr_ns, max_freq_mhz);

	if(wtr < 4){
		printf("#Warnning: DTR1(wtr) setting (%d clks) may vilolates the min. requirement(4 clks) for current setting\n\r", wtr);
	}else{
		printf("ok\n\r");
	}

	/* 6. Refresh period (7.8us) */
	printf("\trefresh cycles: ");
	min_freq_mhz  = _DTR_refresh_freq_mhz(std_refi_ns);
	if(0 != min_freq_mhz){
		printf("(DRAM freq. have to >= %dMHz) ", min_freq_mhz);
		if(dram_freq_mhz < min_freq_mhz)
			printf("\n\r\t#Warnning: DTR setting may vilolate the requirement of DRAM refresh in %dms.\n\r", std_refi_ns);
		else
			printf("ok\n\r");
	}
	else
		printf("DRAM freq. have to > 1000MHz\n\r");


	/* 7. T_RP  (20ns) */
	printf("\trp: ");
	max_freq_mhz = _DTR_rp_frq_mhz(std_rp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rp_ns, max_freq_mhz);
	else
		printf("ok\n\r");
	/* 8. T_RCD (20ns) */
	printf("\trcd: ");
	max_freq_mhz = _DTR_rcd_frq_mhz(std_rcd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rcd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rcd_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 9. T_RRD (20ns) */
	rrd = _DTR_rrd();
	printf("\trrd: ");
	max_freq_mhz = _DTR_rrd_frq_mhz(std_rrd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rrd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rrd_ns, max_freq_mhz);
	
	if(rrd < 4){
		printf("#Warnning: DTR1(rrd) setting (%d clks) may vilolates the min. requirement(4 clks) for current setting\n\r", rrd);
	}else{
		printf("ok\n\r");
	}
	/* 10. T_FAWG (20ns) */
	printf("\tfawg: ");
	max_freq_mhz = _DTR_fawg_frq_mhz(std_fawg_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(fawg) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_fawg_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 11. T_RFC */
	printf("\trfc: ");
	max_freq_mhz = _DTR_rfc_frq_mhz(std_rfc_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rfc) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rfc_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 12. T_RAS (45ns) */
	printf("\tras: ");
	max_freq_mhz = _DTR_ras_frq_mhz(std_ras_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(ras) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_ras_ns, max_freq_mhz);
	else
		printf("ok\n\r");

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

	_DTR_DDR3_MRS_setting(sug_dtr, sug_mr);
	printf("\tWe suggeset DTR setting for current environment: (0x%08x), (0x%08x), (0x%08x)\n\r",\
			 sug_dtr[0],  sug_dtr[1],  sug_dtr[2]);
	printf("\tApply suggested DTR by\n\r\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n\r",\
			 DTR0, sug_dtr[0], DTR1, sug_dtr[1]);
	printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n\r", DTR2, sug_dtr[2], DMCR, sug_mr[0]);
	printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n\r", DMCR, sug_mr[1], DMCR, sug_mr[2]);
	

}
#endif


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
#ifdef CONFIG_DDR2_USAGE
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
	unsigned int	real_wr_ns, real_rtp_ns, real_wtr_ns, real_refi_ns;
	unsigned int	real_rp_ns, real_rcd_ns, real_rrd_ns, real_fawg_ns;
	unsigned int	real_rfc_ns, real_ras_ns;


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


	printf("DTR Checking Rules: \n\r");
	printf("\tt_refi = %dns, \n\r", std_refi_ns);
	printf("\tt_wr = %dns\n\r", std_wr_ns);
	printf("\tt_rtp = %dns\n\r", std_rtp_ns);
	printf("\tt_wtr = %dns\n\r", std_wtr_ns);
	printf("\tt_rp = %dns\n\r", std_rp_ns);
	printf("\tt_rcd = %dns\n\r", std_rcd_ns);
	printf("\tt_fawg = %dns\n\r", std_fawg_ns);
	printf("\tt_ras = %dns\n\r", std_ras_ns);
	printf("\tt_rfc = %dns, _rfc_ns(%d)\n\r", std_rfc_ns, _rfc_ns);

	printf("Checking Current setting: \n\r");
	/* 1. CAS latency   */
	two_cas = _DTR_two_cas();
	printf("\tcas\t(%d.%d): ", (two_cas/2), ((two_cas*10)/2)%10);
	if(two_cas > 12)
		printf("#Warnning: No CAS > 6 for DDR2 SDRAM\n\r");
	else if((two_cas == 4) && (DDR2_CAS2_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((two_cas == 6) && (DDR2_CAS3_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((two_cas == 8) && (DDR2_CAS4_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((two_cas == 10) && (DDR2_CAS5_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((two_cas == 12) && (DDR2_CAS6_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else
		printf("ok\n\r");
	
	/* 2. T_WR  (15ns) */
	real_wr_ns = _DTR_wr_time_ns(dram_freq_mhz);
	printf("\twr\t(%dns): ",real_wr_ns);
	max_freq_mhz = _DTR_wr_frq_mhz(std_wr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR0(wr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_wr_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 3. CAS Write latency */
	tcwl = _DTR_cwl();
	printf("\tcwl\t(%d): ", tcwl);
	if(tcwl != ((two_cas/2) - 1) )
		printf("#Warnning: DTR0(cwl) != cas-1 DRAM clock\n\r");
	else
		printf("ok\n\r");

	/* 4. T_RTP  (20ns) */
	real_rtp_ns = _DTR_rtp_time_ns(dram_freq_mhz);
	printf("\trtp\t(%dns): ",real_rtp_ns);
	max_freq_mhz = _DTR_rtp_frq_mhz(std_rtp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rtp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rtp_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 5. T_WTR  (20ns) */
	real_wtr_ns = _DTR_wtr_time_ns(dram_freq_mhz);
	printf("\twtr\t(%dns): ", real_wtr_ns);
	max_freq_mhz = _DTR_wtr_frq_mhz(std_wtr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(wtr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_wtr_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 6. Refresh period (64ms) */
	real_refi_ns = _DTR_refresh_time_ns(dram_freq_mhz);
	printf("\trefresh cycles(%dns): ",real_refi_ns);
	min_freq_mhz  = _DTR_refresh_freq_mhz(std_refi_ns);
	if(0 != min_freq_mhz){
		printf("(DRAM freq. have to >= %dMHz) ", min_freq_mhz);
		if(dram_freq_mhz < min_freq_mhz)
			printf("\n\r\t#Warnning: DTR setting may vilolate the requirement of DRAM refresh in %dns.\n\r", std_refi_ns);
		else
			printf("ok\n\r");
	}
	else
		printf("DRAM freq. have to > 1000MHz\n\r");


	/* 7. T_RP  (20ns) */
	real_rp_ns = _DTR_rp_time_ns(dram_freq_mhz);
	printf("\trp\t(%dns): ",real_rp_ns);
	max_freq_mhz = _DTR_rp_frq_mhz(std_rp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rp_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 8. T_RCD (20ns) */
	real_rcd_ns = _DTR_rcd_tims_ns(dram_freq_mhz);
	printf("\trcd\t(%dns): ",real_rcd_ns);
	max_freq_mhz = _DTR_rcd_frq_mhz(std_rcd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rcd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rcd_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 9. T_RRD (20ns) */
	real_rrd_ns = _DTR_rrd_time_ns(dram_freq_mhz);
	printf("\trrd\t(%dns): ",real_rrd_ns);
	max_freq_mhz = _DTR_rrd_frq_mhz(std_rrd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rrd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rrd_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 10. T_FAWG (20ns) */
	real_fawg_ns = _DTR_fawg_time_ns(dram_freq_mhz);
	printf("\tfawg\t(%dns): ",real_fawg_ns);
	max_freq_mhz = _DTR_fawg_frq_mhz(std_fawg_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(fawg) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_fawg_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 11. T_RFC */
	real_rfc_ns = _DTR_rfc_time_ns(dram_freq_mhz);
	printf("\trfc\t(%dns): ",real_rfc_ns);
	max_freq_mhz = _DTR_rfc_frq_mhz(std_rfc_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rfc) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rfc_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 12. T_RAS (45ns) */
	real_ras_ns = _DTR_ras_time_ns(dram_freq_mhz);
	printf("\tras\t(%dns): ",real_ras_ns);
	max_freq_mhz = _DTR_ras_frq_mhz(std_ras_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(ras) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_ras_ns, max_freq_mhz);
	else
		printf("ok\n\r");

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
		printf("\tWorning: wrong tCWL computation\n\r");
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

#if 1
	_DTR_DDR2_MRS_setting(sug_mr);
#else
	_DTR_DDR2_MRS_setting(sug_dtr, sug_mr);
#endif
	printf("\tWe suggeset DTR setting for current environment: (0x%08x), (0x%08x), (0x%08x)\n\r",\
			 sug_dtr[0],  sug_dtr[1],  sug_dtr[2]);
	printf("\tApply suggested DTR by\n\r\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n\r",\
			 DTR0, sug_dtr[0], DTR1, sug_dtr[1]);
	printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n\r", DTR2, sug_dtr[2], DMCR, sug_mr[0]);
	printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n\r", DMCR, sug_mr[1], DMCR, sug_mr[2]);
	

}
#endif

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
#ifdef CONFIG_DDR1_USAGE
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

	printf("DTR Checking Rules: \n\r");
	printf("\tt_refi = %dns, \n\r", DDR_STD_REFI_NS);
	printf("\tt_wr = %dns\n\r",  DDR_STD_WR_NS);
	printf("\tt_wtr = %dns\n\r", DDR_STD_WTR_NS);
	printf("\tt_rp = %dns\n\r",  DDR_STD_RP_NS);
	printf("\tt_rcd = %dns\n\r", DDR_STD_RCD_NS);
	printf("\tt_ras = %dns\n\r", DDR_STD_RAS_NS);
	printf("\tt_rfc = %dns\n\r", DDR_STD_RFC_NS);

	printf("Checking Current setting: \n\r");
	/* 1. CAS latency   */
	two_cas = _DTR_two_cas();
	printf("\tcas(%d.%d): ", (two_cas/2), ((two_cas*10)/2)%10);
	if(two_cas > 6)
		printf("#Warnning: No CAS > 3 for DDR SDRAM\n\r");
	else if((two_cas == 4) && (DDR_CAS2_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((two_cas == 5) && (DDR_CAS25_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else if((two_cas == 6) && (DDR_CAS3_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n\r", dram_freq_mhz);
	else
		printf("ok\n\r");
	
	/* 2. T_WR  (15ns) */
	printf("\twr: ");
	max_freq_mhz = _DTR_wr_frq_mhz(std_wr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR0(wr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_wr_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 3. CAS Write latency */
	tcwl = _DTR_cwl();
	printf("\tcwl(%d): ", tcwl);
	if(tcwl != 1 )
		printf("#Warnning: DTR0(cwl) != 1 DRAM clock\n\r");
	else
		printf("ok\n\r");

	/* 4. DDR1 no T_RTP */

	/* 5. T_WTR  (20ns) */
	printf("\twtr: ");
	max_freq_mhz = _DTR_wtr_frq_mhz(std_wtr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(wtr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_wtr_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 6. Refresh period (7.8us) */
	printf("\trefresh cycles: ");
	min_freq_mhz  = _DTR_refresh_freq_mhz(std_refi_ns);
	if(0 != min_freq_mhz){
		printf("(DRAM freq. have to >= %dMHz) ", min_freq_mhz);
		if(dram_freq_mhz < min_freq_mhz)
			printf("\n\r\t#Warnning: DTR setting may vilolate the requirement of DRAM refresh in %dns.\n\r", std_refi_ns);
		else
			printf("ok\n\r");
	}
	else
		printf("DRAM freq. have to > 1000MHz\n\r");


	/* 7. T_RP  (20ns) */
	printf("\trp: ");
	max_freq_mhz = _DTR_rp_frq_mhz(std_rp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rp_ns, max_freq_mhz);
	else
		printf("ok\n\r");
	/* 8. T_RCD (20ns) */
	printf("\trcd: ");
	max_freq_mhz = _DTR_rcd_frq_mhz(std_rcd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rcd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rcd_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 9. T_RRD (20ns) */
	printf("\trrd: ");
	max_freq_mhz = _DTR_rrd_frq_mhz(std_rrd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rrd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rrd_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 10. DDR1 no T_FAWG */

	/* 11. T_RFC (75ns) */
	printf("\trfc: ");
	max_freq_mhz = _DTR_rfc_frq_mhz(std_rfc_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rfc) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_rfc_ns, max_freq_mhz);
	else
		printf("ok\n\r");

	/* 12. T_RAS (45ns) */
	printf("\tras: ");
	max_freq_mhz = _DTR_ras_frq_mhz(std_ras_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(ras) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n\r", std_ras_ns, max_freq_mhz);
	else
		printf("ok\n\r");

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

#if 1
	_DTR_DDR1_MRS_setting(sug_mr);
#else
	_DTR_DDR1_MRS_setting(sug_dtr, sug_mr);
#endif

	printf("\tWe suggeset DTR setting for current environment: (0x%08x), (0x%08x), (0x%08x)\n\r",\
			 sug_dtr[0],  sug_dtr[1],  sug_dtr[2]);
	printf("\tApply suggested DTR by\n\r\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n\r",\
			 DTR0, sug_dtr[0], DTR1, sug_dtr[1]);
	printf("\t\"mw 0x%08x 0x%08x;\"\n\r", DTR2, sug_dtr[2]);
	printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n\r", DMCR, sug_mr[0], DMCR, sug_mr[1]);

	return;
}
#endif

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
	if(memctlc_DDR_Type()==1){
#ifdef CONFIG_DDR1_USAGE
		memctlc_check_DTR_DDR(dram_freq_mhz);
#endif
	}else if(memctlc_DDR_Type()==2){
#ifdef CONFIG_DDR2_USAGE
		memctlc_check_DTR_DDR2(dram_freq_mhz);
#endif
	}else if(memctlc_DDR_Type()==3){
#ifdef CONFIG_DDR3_USAGE
		memctlc_check_DTR_DDR3(dram_freq_mhz);
#endif
	}else{
		_memctl_debug_printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		_memctl_debug_printf("Error Unkown DRAM Type\n\r");
	}

	return;
}
