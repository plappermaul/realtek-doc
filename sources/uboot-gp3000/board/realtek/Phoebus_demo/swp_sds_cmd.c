#include <common.h>
#include <net.h>


void set_disable(void)
{
	//Disable SDS mode
	*(volatile unsigned int*)(0xBB000274)=0x3F;

}

void set_sgmii(void)
{
	printf("Set sgmii\n");
	*(volatile unsigned int*)(0xb8000600)=0x20000002;
	*(volatile unsigned int*)(0xbb0001e4)=0x0;
	*(volatile unsigned int*)(0xbb000250)=0xbfff;
	*(volatile unsigned int*)(0xBB0002C4)=0x00000100;
	*(volatile unsigned int*)(0xBB041000)=0x00000008;
	*(volatile unsigned int*)(0xBB000274)=0x00000022;
	*(volatile unsigned int*)(0xBB000354)=0x24000000;
	*(volatile unsigned int*)(0xBB041038)=0x0000AC68;
	*(volatile unsigned int*)(0xBB041034)=0x0000C001;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000276D;
	*(volatile unsigned int*)(0xBB041034)=0x0000C003;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x00005000;
	*(volatile unsigned int*)(0xBB041034)=0x0000C004;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x00006313;
	*(volatile unsigned int*)(0xBB041034)=0x0000C005;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000001C;
	*(volatile unsigned int*)(0xBB041034)=0x0000C006;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000521C;
	*(volatile unsigned int*)(0xBB041034)=0x0000C009;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000B628;
	*(volatile unsigned int*)(0xBB041034)=0x0000C00A;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000E714;
	*(volatile unsigned int*)(0xBB041034)=0x0000C00D;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x000050A2;
	*(volatile unsigned int*)(0xBB041034)=0x0000C100;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000886A;
	*(volatile unsigned int*)(0xBB041034)=0x0000C101;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x00000053;
	*(volatile unsigned int*)(0xBB041034)=0x0000C102;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x00002B66;
	*(volatile unsigned int*)(0xBB041034)=0x0000C103;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000450C;
	*(volatile unsigned int*)(0xBB041034)=0x0000C104;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000F0F2;
	*(volatile unsigned int*)(0xBB041034)=0x0000C109;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x000080A1;
	*(volatile unsigned int*)(0xBB041034)=0x0000C10B;
	mdelay(10);
	*(volatile unsigned int*)(0xBB000274)=0x0000003F;
	*(volatile unsigned int*)(0xBB000274)=0x00000022;
	*(volatile unsigned int*)(0xBB0002C4)=0x00000000;
	*(volatile unsigned int*)(0xbb0001e4)=0x76;
}

void set_hisgmii(void)
{
	printf("Set hisgmii\n");
	*(volatile unsigned int*)(0xb8000600)=0x20000002;
	*(volatile unsigned int*)(0xbb0001e4)=0x0;
	*(volatile unsigned int*)(0xbb000250)=0xbfff;
	*(volatile unsigned int*)(0xBB0002C4)=0x00000100;
	*(volatile unsigned int*)(0xBB041000)=0x00000008;
	*(volatile unsigned int*)(0xBB000274)=0x00000032;
	*(volatile unsigned int*)(0xBB000354)=0x48000000;
	*(volatile unsigned int*)(0xBB041038)=0x0000AC4C;
	*(volatile unsigned int*)(0xBB041034)=0x0000C001;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000276D;
	*(volatile unsigned int*)(0xBB041034)=0x0000C003;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x00005000;
	*(volatile unsigned int*)(0xBB041034)=0x0000C004;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x000063DB;
	*(volatile unsigned int*)(0xBB041034)=0x0000C005;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000001C;
	*(volatile unsigned int*)(0xBB041034)=0x0000C006;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000521C;
	*(volatile unsigned int*)(0xBB041034)=0x0000C009;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000B628;
	*(volatile unsigned int*)(0xBB041034)=0x0000C00A;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000E714;
	*(volatile unsigned int*)(0xBB041034)=0x0000C00D;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x000050A5;
	*(volatile unsigned int*)(0xBB041034)=0x0000C100;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000888A;
	*(volatile unsigned int*)(0xBB041034)=0x0000C101;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x00000053;
	*(volatile unsigned int*)(0xBB041034)=0x0000C102;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x00008B66;
	*(volatile unsigned int*)(0xBB041034)=0x0000C103;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000450C;
	*(volatile unsigned int*)(0xBB041034)=0x0000C104;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x0000F0F2;
	*(volatile unsigned int*)(0xBB041034)=0x0000C109;
	mdelay(10);
	*(volatile unsigned int*)(0xBB041038)=0x000080A1;
	*(volatile unsigned int*)(0xBB041034)=0x0000C10B;
	mdelay(10);
	*(volatile unsigned int*)(0xBB000274)=0x0000003F;
	*(volatile unsigned int*)(0xBB000274)=0x00000032;
	*(volatile unsigned int*)(0xBB0002C4)=0x00000000;
	*(volatile unsigned int*)(0xbb0001e4)=0x1075;	

}

int do_sds (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	uint32_t mode;
	
	if (argc != 2) return cmd_usage(cmdtp);
	
	mode = simple_strtoul(argv[1], NULL, 10);
	switch(mode){
		case 0: //disable
			set_disable();
		break;	
		case 1: //SGMII
			set_sgmii();
		break;
		case 2: //HISGMII
			set_hisgmii();
		break;
		
		default:
			cmd_usage(cmdtp);
	}	
	return 0;
}

U_BOOT_CMD(
	sdsmode,  2,      1,      do_sds,
	"Set serdes mode",
	"[mode]\n"
	"0: Disable, 1: SGMII, 2: HISGMII"
);

