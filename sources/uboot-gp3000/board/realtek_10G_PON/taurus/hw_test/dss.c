/* repeat step 1 ~ 6 for different settings.
	   ro_sel 1 ~ 5, wire_sel : 0 ~ 1
	   Need to run 10 patterns totally
*/
#include <common.h>

#define GLOBAL_CA55_TOP_DSS30_CTRL 0xf43202f0
#define GLOBAL_CA55_TOP_DSS30_STATUS 0xf43202f4

void Digital_Speed_Sensor(unsigned int data_in)
{
	unsigned short int ro_sel,wire_sel;
	volatile unsigned int *arm_ca55_top_DSS_c30_ctrl,*arm_ca55_top_DSS_c30_status;
	arm_ca55_top_DSS_c30_ctrl = (unsigned int *)GLOBAL_CA55_TOP_DSS30_CTRL;
	arm_ca55_top_DSS_c30_status = (unsigned int *)GLOBAL_CA55_TOP_DSS30_STATUS;

	for(wire_sel=0;wire_sel<2;wire_sel++)
	{
		for(ro_sel=1;ro_sel<6;ro_sel++)
		{
			// Activation control, step 1 : Trun-off control and "speed_en"[25]
			*arm_ca55_top_DSS_c30_ctrl = 0;

			// step 2 : toggle "dss_rst_n"[0]
			*arm_ca55_top_DSS_c30_ctrl = 0x1;

			// step 3 : Setting configuration of DSS, ie: "ro_sel"[3..1], "wire_sel"[4] and "data_in"[24..5]
			*arm_ca55_top_DSS_c30_ctrl = (wire_sel&1)<<4 | (ro_sel&0x7)<<1 | (data_in&0x0FFFFF)<<5;

			// must delay 1T in here.

			// step 4 : Turn on DSS by driving "speed_en"[25] 1
			*arm_ca55_top_DSS_c30_ctrl |= (1<<25);

			// step 5 : Wait "ready"[0] to be high
			while(((*arm_ca55_top_DSS_c30_status)&0x1)==0);

			// step 6 : Read result of output "count_out"[20..1] , "wsort_go"[21]
			printf("wire_sel=%d, ro_sel=%d, wsort_go =%d, data_in=0x%20x.\n\r",wire_sel,ro_sel,(*arm_ca55_top_DSS_c30_status>>21)&0x1, (*arm_ca55_top_DSS_c30_status>>1)&0x0FFFFF);
		}
	}
}


static int do_dss_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned int data_in;
    data_in = simple_strtoul(argv[1], NULL, 16);
    Digital_Speed_Sensor(data_in);

    return 0;
}

U_BOOT_CMD(
    dss, 1, 0,    do_dss_test,
    "Perform RESET of the CPU with DDR warm reset",
    "dss <data_in>"
);

