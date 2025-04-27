/*
 * ----------------------------------------------------------------
 * Copyright c                  Realtek Semiconductor Corporation, 2002  
 * All rights reserved.
 * 
 *
 * Abstract: Switch core driver source code.
 *
 * $Author: yachang $
 *
 * ---------------------------------------------------------------
 */
#include "swCore.h"

#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))

#define MACReg(offset, val)		(WRITE_MEM32(SWITCH_BASE + offset, val))

int swCore_init(void)
{
	//printf("%s %d\n", __func__, __LINE__);

	MACReg(0x025000, 0x13042);//METER_TB_CTRL

	//Threshold
	MACReg(0x0230FC, 0x0000000B);//FC_TYPE, FC_JUMBO_SIZE=0x11

	MACReg(0x023100, 0x00001B58);//FC_DROP_ALL_TH
	MACReg(0x023104, 0x00001A86);//FC_PAUSE_ALL_TH
	MACReg(0x023108, 0x0CDA0C94);//FC_GLB_FCOFF_HI_TH: ON_TH 
	MACReg(0x02310C, 0x0C080BC2);//FC_GLB_FCOFF_LO_TH: ON_TH 
	MACReg(0x023110, 0x0CDA0C94);//FC_GLB_HI_TH: ON_TH 
	MACReg(0x023114, 0x0C080BC2);//FC_GLB_LO_TH: ON_TH 
	MACReg(0x023118, 0x054004EC);//FC_P_HI_TH: ON_TH 
	MACReg(0x02311C, 0x00E00054);//FC_P_LO_TH: ON_TH 
	MACReg(0x023120, 0x054004EC);//FC_P_FCOFF_HI_TH: ON_TH 
	MACReg(0x023124, 0x00E00054);//FC_P_FCOFF_LO_TH: ON_TH 
	MACReg(0x02D000, 0x005782BC);//FC_Q_EGR_DROP_TH: P1, P0
	MACReg(0x02D004, 0x005782BC);//FC_Q_EGR_DROP_TH: P3, P2
	MACReg(0x02D008, 0x005782BC);//FC_Q_EGR_DROP_TH: P5, P4
	MACReg(0x02D00C, 0x000002BC);//FC_Q_EGR_DROP_TH: P6

	MACReg(0x02D020, 0x000000A8);//FC_Q_EGR_GAP_TH()

	MACReg(0x02D010, 0x036B1B58);//FC_P_EGR_DROP_TH P1, P0
	MACReg(0x02D014, 0x036B1B58);//FC_P_EGR_DROP_TH P3, P2
	MACReg(0x02D018, 0x036B1B58);//FC_P_EGR_DROP_TH P5, P4
	MACReg(0x02D01C, 0x00001B58);//FC_P_EGR_DROP_TH P7, P6
	MACReg(0x02D024, 0x0000015E);//FC_P_EGR_GAP_TH

	/* Otto preloader uses GPON SRAM and it is not used in U-Boot */
	//MACReg(0x0001E0, 0x1);//PB_3M=1
	MACReg(0x000068, 0x400);//SW_RST=1

	//bypass CRC
	MACReg(0x013104, 0xffffffff);
	MACReg(0x0001c4, 0x00000002);

	MACReg(0x02dcbc, 0x1);
	MACReg(0x02ddcc, 0x1);
	MACReg(0x02dabc, 0x1ffff);

	MACReg(0x48, 0x1110);//HOTCMD_EN[2:0]=1

	//port set isolation port all vidx 0
	MACReg(0x00027000, 0x00000000);
	//vlan set entry 0  member 0-6
	MACReg(0x00013008, 0x00000000);
	MACReg(0x0001300c, 0x0000007f);
	//vlan set vlan vid 0 policing disable
	MACReg(0x00012000, 0x00000011);
	MACReg(0x00012008, 0x00000000);
	MACReg(0x0001200c, 0x00000000);
	MACReg(0x00012010, 0x00000000);
	MACReg(0x00012014, 0x00000000);
	MACReg(0x00012018, 0x00000000);
	MACReg(0x00012000, 0x00000019);

	WRITE_MEM32(0xbb00007c, 0x1);

	*((volatile unsigned int *)0xb8000044) = *((volatile unsigned int *)0xb8000044) | 1;
	//WRITE_MEM32(0xb8000044, 0x5f);

	//force cpu port link up
	MACReg(0xac, 0x76);

	return 0;
}



