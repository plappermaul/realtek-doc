#include <common.h>

#define GLOBAL_CA55_FSS_CTRL0 0xf4320308
#define GLOBAL_CA55_FSS_CTRL1 0xf432030c
#define GLOBAL_CA55_FSS_CTRL2 0xf4320310
#define GLOBAL_CA55_FSS_STS0  0xf4320314
#define GLOBAL_CA55_FSS_STS1  0xf4320318
#define GLOBAL_CA55_TOP_FSS_CTRL  0xf432031c
#define GLOBAL_CA55_TOP_FSS_STS  0xf4320320

void FSS_calibration_mode(unsigned char cali_mode)
{
	volatile unsigned int *fss_ctrl0,*fss_ctrl1,*fss_ctrl2,*fss_sts0,*fss_sts1,*fss_ctrl,*fss_sts;
	fss_ctrl0 = (unsigned int *)GLOBAL_CA55_FSS_CTRL0;
	fss_ctrl1 = (unsigned int *)GLOBAL_CA55_FSS_CTRL1;
	fss_ctrl2 = (unsigned int *)GLOBAL_CA55_FSS_CTRL2;
	fss_sts0 = (unsigned int *)GLOBAL_CA55_FSS_STS0;
	fss_sts1 = (unsigned int *)GLOBAL_CA55_FSS_STS1;
	fss_ctrl = (unsigned int *)GLOBAL_CA55_TOP_FSS_CTRL;
	fss_sts =  (unsigned int *)GLOBAL_CA55_TOP_FSS_STS;

	printf("ARM CA55 Core FSS Control...\n\r");
	//Toggle rst_n
	*fss_ctrl0 = 0;
	*fss_ctrl0 = 0x0F;

    switch(cali_mode){			//Detect Mode (0), Calibration Mode 1(1)/2(2)/2(3) burst
		case 0:
			*fss_ctrl0 &= 0x0000000F;		//sensor_en=0[n7..n4], cal_2nd_en=0[n27..n24], cal_en=0[n23..n20], path_sel=0[n15..n8]
			*fss_ctrl1 = 0x99991111;		//config_CDL0=1[n15..n12][n11..n8][n7..n4][n3..n0],config_CDL1=9[n31..n28][n27..n24][n23..n20][n19..n16]
			*fss_ctrl2 = 0x00006666;		//config_CDL2=6[n15..n12][n11..n8][n7..n4][n3..n0]
			*fss_ctrl0 |= 0x000000F0;		//sensor_en=1[n7..n4], Start Detect Mode
			printf("Detect Mode : \n\r");
			printf("detect_out =0x%x, detect_1 =0x%x, detect_2 =0x%x \n\r ",((*fss_sts1)>>20) &0x0F,((*fss_sts1)>>24) &0x0F,((*fss_sts1)>>28) &0x0F);
			break;
        case 1:
			*fss_ctrl1 = 0x0;				//config_CDL0=1[n15..n12][n11..n8][n7..n4][n3..n0],config_CDL1=9[n31..n28][n27..n24][n23..n20][n19..n16]
			*fss_ctrl2 = 0x0;				//config_CDL2=6[n15..n12][n11..n8][n7..n4][n3..n0]		
			*fss_ctrl0 &= 0x0000000F;		//sensor_en=0[n7..n4], cal_2nd_en=0[n27..n24], cal_en=0[n23..n20], path_sel=0[n15..n8]
            *fss_ctrl0 |= 0x00F00000;		//cal_en=1[n23..n20], Start Calibration Mode 1
			while(((*fss_sts1)&0x0F)==0);	// output check calibration done[3..0]
			printf("Calibration Mode 1 : \n\r");
			printf("CDL0=0x%x, CDL1=0x%x \n\r", (*fss_sts0) &0x0FFFF, (*fss_sts0) >>16); //cal_cdl0[n15..n12][n11..n8][n7..n4][n3..n0], cal_cdl1[n31..n28][n27..n24][n23..n20][n19..n16]			
            break;
        case 2:
			*fss_ctrl1 = 0x1111;			//config_CDL0=1[n15..n12][n11..n8][n7..n4][n3..n0],config_CDL1=9[n31..n28][n27..n24][n23..n20][n19..n16]
			*fss_ctrl2 = 0x0;				//config_CDL2=6[n15..n12][n11..n8][n7..n4][n3..n0]				
			*fss_ctrl0 &= 0x0000000F;		//sensor_en=0[n7..n4], cal_2nd_en=0[n27..n24], cal_en=0[n23..n20], path_sel=0[n15..n8]
            *fss_ctrl0 |= 0x0F000000;		//cal_2nd_en=1[n27..n24], Start Calibration Mode 2
			while(((*fss_sts1)&0x0F)==0);	// output check calibration done[3..0]
			printf("Calibration Mode 2 : \n\r");
			printf("CDL1=0x%x", (*fss_sts0) >>16);			//cal_cdl1[n31..n28][n27..n24][n23..n20][n19..n16]
			break;
		case 3:
			*fss_ctrl1 = 0x1111;			//config_CDL0=1[n15..n12][n11..n8][n7..n4][n3..n0],config_CDL1=9[n31..n28][n27..n24][n23..n20][n19..n16]
			*fss_ctrl2 = 0x0;				//config_CDL2=6[n15..n12][n11..n8][n7..n4][n3..n0]				
			*fss_ctrl0 &= 0x0000000F;		//sensor_en=0[n7..n4], cal_2nd_en=0[n27..n24], cal_en=0[n23..n20], path_sel=0[n15..n8]
			*fss_ctrl0 |= 0x0F000000;		//cal_2nd_en=1[n27..n24], Start Calibration Mode 2
			*fss_ctrl0 |= 0x00F00000;		//cal_2nd_en=1[n27..n24],cal_en=1[n23..n20], Start Calibration Mode 2 Burst
			udelay(10000);		//delay 10ms
			*fss_ctrl0 &= 0x00F00000;		//cal_2nd_en=1[n27..n24],cal_en=0[n23..n20], Escape Calibration Mode 2 Burst
			while(((*fss_sts1)&0x0F)==0);	// output check calibration done[3..0]
			printf("Calibration Mode 2 Burst: \n\r");
			printf("CDL1=0x%x\n\r", (*fss_sts0) >>16);			//cal_cdl1[n31..n28][n27..n24][n23..n20][n19..n16]
			printf("Cal_Min_CDL1=0x%x\n\r", ((*fss_sts1)>>4) & 0x0ffff);		//cali_min_cdl1[n19..n16][n15..n12][n11..n8][n7..n4]
			break;
        default:
            printf("input error...\n\r");
    }

	//ARM CA55 Top FSS Control
	printf("ARM CA55 Top FSS Control...\n\r");
	//Toggle rst_n
	*fss_ctrl = 0;
	*fss_ctrl = 1;
    switch(cali_mode){
		case 0:
			*fss_ctrl &= 0x00000001;		//sensor_en=0[n1], cal_2nd_en=0[n7], cal_en=0[6], path_sel=0[n3..n2]
			*fss_ctrl |= 0x00069100;		//config_CDL0=1[n11..n8], config_CDL1=9[n15..n12], config_CDL2=6[n19..n16]
			*fss_ctrl |= 0x00000002;		//sensor_en=1[n1], Start Detect Mode
			printf("Detect Mode : \n\r");
			printf("detect_out =0x%x, detect_1 =0x%x, detect_2 =0x%x \n\r ",((*fss_sts)>>2) &0x01,(*fss_sts) &0x01,((*fss_sts)>>1) &0x01);
			break;
		case 1:
			*fss_ctrl &= 0x00000001;		//sensor_en=0[n1], cal_2nd_en=0[n7], cal_en=0[6], path_sel=0[n3..n2]
			*fss_ctrl |= 0x00000040;		//cal_en=1[n6]
			while((((*fss_sts)>>15)&0x01)==0);	// output check calibration done[n15]
			printf("Calibration Mode 1 : \n\r");
			printf("CDL0=0x%x, CDL1=0x%x \n\r", ((*fss_sts)>>3) &0x0F, ((*fss_sts)>>7) &0x0F); //cal_cdl0[n6..n3], cal_cdl1[n10..n7]
			break;
		case 2:
			*fss_ctrl &= 0x00000001;		//sensor_en=0[n1], cal_2nd_en=0[n7], cal_en=0[6], path_sel=0[n3..n2]
			*fss_ctrl |= (0x1<<8);      	//config_CDL0=0x1[n11..n8]
			*fss_ctrl |= (0x1<<7);			//cal_2nd_en=1[n7]
			while((((*fss_sts)>>15)&0x01)==0);	// output check calibration done[n15]
			printf("Calibration Mode 2 : \n\r");
			printf("CDL1=0x%x", ((*fss_sts) >>7) &0x0F);//cal_cdl1[n10..n7]
            break;
        case 3:
			*fss_ctrl &= 0x00000001;		//sensor_en=0[n1], cal_2nd_en=0[n7], cal_en=0[6], path_sel=0[n3..n2]
			*fss_ctrl |= (0x1<<8);      	//config_CDL0=0x1[n11..n8]
			*fss_ctrl |= (0x1<<7);			//cal_2nd_en=1[n7], Start Calibration Mode 2
			*fss_ctrl |= (0x1<<6);			//cal_en=1[n6], Start Calibration Mode 2 Burst
			udelay(10);
			*fss_ctrl &= 0xffffffbf;			//cal_en=1[n6], Start Calibration Mode 2 Burst
			printf("Calibration Mode 2 Burst: \n\r");
			printf("CDL1=0x%x\n\r", ((*fss_sts) >>7)&0x0f);			//cal_cdl1[n10..n7]
			printf("Cal_Min_CDL1=0x%x\n\r", ((*fss_sts)>>11) & 0x0f);		//cali_min_cdl1[n14..n11]
            break;
        default:
            printf("input error...\n\r");
    }
}

static int do_fss_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned char cali_mode = 0;
    cali_mode = simple_strtoul(argv[1], NULL, 10);
    FSS_calibration_mode(cali_mode);
    return 0;
}

U_BOOT_CMD(
    fss, 10, 1,    do_fss_test,
    "Perform RESET of the CPU with DDR warm reset",
    "fss <mode> (mode 0:Detect Mode, 1:Calibration Mode 1, 2:Calibration Mode 2, 3:Calibration Mode 2 Burst)"
);


