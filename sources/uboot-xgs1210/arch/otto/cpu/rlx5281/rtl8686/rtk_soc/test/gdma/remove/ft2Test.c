/* Copyright c                  Realtek Semiconductor Corporation, 2006
* All rights reserved.
*
* Program : Test Code Header File For IC-FT2 Testing
* Abstract : test Imem Dmem 
* Author : Michael Huang (michaelhuang@realtek.com.tw)
*/

#include <asm/uaccess.h>
#include <asm/delay.h>
#include "rtl_types.h"
#include "asicRegs.h"
#ifdef CONFIG_RTL865XC
#include "rtl865xC_tblAsicDrv.h"
#else
#include "rtl8651_tblAsicDrv.h"
#endif
#include "virtualMac.h"
#include "rtl_glue.h"
#include "rtl_utils.h"
#include "drvTest.h"
#include "icTest.h"
#include "l2Test.h"
//#include "l2PacketTest.h"
#include "l34Test.h"
#include "hsModel.h"
#include "rtl8651_aclLocal.h"
#include "rtl8651_layer2local.h"
#include "drvTest.h"
#include "fwdTest.h"
#include "rtl8651_tblDrvFwd.h"
#include "icModel.h" /* for FIRST_CPURPDCR? */
#include "pktForm.h"
#include "rtl865xc_asicregs.h"
#include "crc32.h"
#include "pktGen.h"
#include "linux/delay.h"

/*
#if defined(RTL865X_TEST)||defined(RTL865X_MODEL_USER)
#define  CONFIG_RTL865XB
#include "utility.h"
#include <cle_userglue.h>
#else
#include "utility.h"
#endif
*/

uint32 ft2_get_cpu_clock_speed_proc (void)
{	
/*	rtlglue_printf("TEST BogoMIPS\t\t: %lu.%02lu\n",
         loops_per_jiffy / (500000/HZ), (loops_per_jiffy / (5000/HZ)) % 100);*/
	
	 return loops_per_jiffy / (500000/HZ);	 
}

uint32 ft2_get_cpu_clock_speed_dpll(void)
{
	uint32 clock_speed = 0;
					
	//REG32(0xB8003204) |= EN_S0DPLL; 	/* DO NOT enable EN_S0DPLL or EN_S1DPLL if read only */			
	if(REG32(0xB8003204)) {
		uint32 s0updpll = (REG32(0xB8003204)>>S0UPDPLL_OFFSET) & S0UPDPLL_MASK;
		uint32 s0downdpll = (REG32(0xB8003204)>>S0DOWNDPLL_OFFSET) & S0DOWNDPLL_MASK;	
		clock_speed = 125*(s0updpll+1)/(s0downdpll+1);		
	} else {		
		 uint32 scr = (REG32(0xB8003200)>>SCCR_CPU_OFFSET) & SCCR_STATUS_MASK;			
		 switch( scr )
    		{
        		case 0: clock_speed = 100; break;
        		case 1: clock_speed = 200; break;
        		case 2: clock_speed = 250; break;
        		case 3: clock_speed = 260; break;
        		case 4: clock_speed = 270; break;
        		case 5: clock_speed = 280; break;
        		case 6: clock_speed = 290; break;
        		case 7: clock_speed = 300; break;
        		default: clock_speed = 0; break;
    		}		
	}
	return clock_speed;	 
}


int32 ft2_testCPU_MIPS(uint32 caseNo)
{	
	int32 dpll_val = ft2_get_cpu_clock_speed_dpll();
	int32 proc_val = ft2_get_cpu_clock_speed_proc();	
	rtlglue_printf("debug: CPU Clock -- DPLL=%dMHz, LINUX=%dMHz, EP=%d\n", dpll_val, proc_val, dpll_val-proc_val);	

	/* error percent should be around +-3MHz only*/
	if((dpll_val-proc_val)<3 && (dpll_val-proc_val)>-3) return SUCCESS;
	else return FAILED;
}

#define ARITHMETIC_TEST_CASE 10
#define TOTAL_ARITHMETIC_TEST_LOOP 1000

int32 ft2_testCPU_Arithmetic(uint32 caseNo)
{
	int i, j, pass = 0;
	int val[ARITHMETIC_TEST_CASE][6] = {
		{31415, 337, 1025, 15, 123456789, 124162511},
		{995678, 1024, 70072, 200, 3456789, 8554310},
		{314, 963852, 41528, 8000, 98765432, 98803258},
		{1234, 5678, 6652, 50000, 777777777, 777777917},
		{999, 20480, 80000, 20, 556677, 1575653},
		{73197, 3888, 84500936, 25, 123456999, 131460559},
		{119978, 63, 698, 4, 2561024, 4450503},
		{78, 12345678, 741852, 36, 32165499, 58893861},
		{12345678, 23, 594, 50000, 388, 6067},		
		{831, 987654, 987654, 20, 99999988, 140987629}
	};

	int val2[ARITHMETIC_TEST_CASE][6] = {
		{22, 33, 44, 55, 20, 87846},
		{123, 456, 789, 23, 8, 127228617},
		{741, 852, 963, 2, 6, 202657572},
		{987, 654, 333, 5, 2, 537377085},
		{10, 11, 12, 13, 3, 5720},
		{333, 654, 1234, 2, 6, 89580996},
		{999999, 14, 25, 3, 50, 20999979},
		{99, 98, 97, 96, 4, 22586256},
		{43, 45, 47, 49, 15, 297087},		
		{61, 620, 6300, 4, 4000, 238266}
	};

	for(j=0; j<TOTAL_ARITHMETIC_TEST_LOOP; j++) {
		for(i=0; i<ARITHMETIC_TEST_CASE; i++) {
			if((val[i][5]==((val[i][0]*val[i][1]-val[i][2])/val[i][3]+val[i][4])) &&
				(val2[i][5]==(val2[i][0]*val2[i][1]*val2[i][2]*val2[i][3]/val2[i][4]))) pass++;
			else {
				rtlglue_printf("debug: [FAIL] arithmetic test result %d/%d (pass/total)\n", pass, ARITHMETIC_TEST_CASE*TOTAL_ARITHMETIC_TEST_LOOP);	
				return FAILED;		
			}			
		}
	}	
	rtlglue_printf("debug: [PASS] arithmetic test result %d/%d (pass/total)\n", pass, ARITHMETIC_TEST_CASE*TOTAL_ARITHMETIC_TEST_LOOP);	
	return SUCCESS;	
}

static void ft2_getImem(unsigned int *pIW_base, unsigned int *pIW_top )
{
	__asm__ __volatile__(
		".set\tnoreorder\n\t"
		/* Initialize */
		"mfc0\t$8,$12\n\t"
		"nop\n\t"
		"nop\n\t"
		"or\t$8,0x80000000\n\t"
		"mtc0\t$8,$12\n\t"
		"nop\n\t"
		"nop\n\t"
		/* IW Base */
		"mfc3\t$8,$0\n\t"
		"nop\n\t"
		"nop\n\t"
		"sw\t$8,0($4)\n\t"
		"nop\n\t"
		/* IW Top */
		"mfc3\t$8,$1\n\t"
		"nop\n\t"
		"nop\n\t"
		"sw\t$8,0($5)\n\t"
		"nop\n\t"
		/* return */
		"jr\t$31\n\t"
		"nop\n\t"
		".set\treorder\n\t");
}

static void ft2_setImem(unsigned int base, unsigned int size)
{
	__asm__ __volatile__(
		".set\tnoreorder\n\t"
		/* --- initialize and start COP3 */
		"mfc0\t$8,$12\n\t"
		"nop\n\t"
		"nop\n\t"
		"or\t$8,0x80000000\n\t"
		"mtc0\t$8,$12\n\t"
		"nop\n\t"
		"nop\n\t"
		/* --- invalidate the IRAM with a 0->1 transition */
		"mtc0\t$0,$20\n\t"	/* CCTL */
		"nop\n\t"
		"nop\n\t"
		"li\t$8,0x20\n\t"
		"mtc0\t$8,$20\n\t"
		"nop\n\t"
		"nop\n\t"        
		/* --- invalidate the icache with a 0->1 transition */
		"mtc0\t$0,$20\n\t"	/* CCTL */
		"nop\n\t"
		"nop\n\t"
		"li\t$8,0x00000202\n\t"	/* Invalid ICACHE */
		"mtc0\t$8,$20\n\t"
		"nop\n\t"
		"nop\n\t"
		/* --- load iram base and top */
		"move\t$8,$4\n\t"
		"la\t$9,0x0fffc000\n\t"
		"and\t$8,$8,$9\n\t"
		"mtc3\t$8,$0\n\t"	/* IW base */
		"nop\n\t"
		"nop\n\t"
		"add\t$8,$8,$5\n\t"
		"addiu\t$8,$8,-1\n\t"
		"mtc3\t$8,$1\n\t"	/* IW top */
		"nop\n\t"
		"nop\n\t"
		/* --- Refill the IRAM with a 0->1 transition */
		"mtc0\t$8,$20\n\t"	/* CCTL */
		"nop\n\t"
		"nop\n\t"
		"li\t$8,0x00000010\n\t"	/* IRAM Fill */
		"mtc0\t$8,$20\n\t"
		"nop\n\t"
		"nop\n\t"
		"jr\t$31\n\t"
		"nop\n\t"
		".set\treorder\n\t");
}

int32 ft2_testImem(uint32 caseNo)
{
	int (*app_start)(void);
	unsigned int IW_base;
	unsigned int IW_top;
	unsigned int *p_buf;
	unsigned int imem;
	unsigned int base;
	unsigned int ofst;
	unsigned int size = 16*1024;	/* for 865xC (IMem size = 16k) */
	u8 result = 0;
	
	ft2_getImem(&IW_base, &IW_top);
	imem = IW_base | 0x80000000;
	/*
	rtlglue_printf("debug: ft2_getImem # pIW_base = 0x%x, pIW_top = 0x%x, Imem = 0x%x \n", IW_base, IW_top, imem);
	*/

	p_buf = rtlglue_malloc(size*2);	/* alloc double size(32k) for alignment */
	if (p_buf == NULL)
		return FAILED;
	
	base = ((unsigned int)p_buf+size) & 0xffff8000;	/* mask 16k alignment space */
    
	for (ofst=0; ofst<size; ofst+=4)
		*(unsigned int*) (base + ofst) = 0x20420001; /* opcode 'addi r2,r2,1' */
	*(unsigned int*) base = 0x24020000; /* opcode 'addiu r2,zero,0x0000' */
	*(unsigned int*) (base + size - 8) = 0x03e00008; /* opcode 'jr $ra' */
	
	ft2_setImem(base, size);
	
	app_start = (int (*)(void)) base;
	if (app_start() == ((size / 4) - 2)) /* exclude 'addiu r2,zero,0' and 'jr ra'. */
		result = 1;	// Pass
	else
		result = 2;	// Fail
	
	ft2_setImem(imem, size);	/* recovery original Imem */
    
	rtlglue_free(p_buf);	/* release sdram */
	/*
	rtlglue_printf("TEST I-mem ...end!\n");
	*/
	
	if (result == 1) {
		return SUCCESS;
	}
	
	return FAILED;
}


static void ft2_getDmem(unsigned int *pDW_base, unsigned int *pDW_top)
{
	__asm__ __volatile__(
		".set\tnoreorder\n\t"	
		/* initialize */
		"mfc0\t$8,$12\n\t"
		"nop\n\t"
		"nop\n\t"
		"or\t$8,0x80000000\n\t"
		"mtc0\t$8,$12\n\t"
		"nop\n\t"
		"nop\n\t"
		/* DW base */
		"mfc3\t$8,$4\n\t"
		"nop\n\t"
		"nop\n\t"
		"sw\t$8,0($4)\n\t"
		"nop\n\t"
		/* DW top */
		"mfc3\t$8,$5\n\t"
		"nop\n\t"
		"nop\n\t"
		"sw\t$8,0($5)\n\t"
		"nop\n\t"
		/* return */
		"jr\t$31\n\t"
		"nop\n\t"
		".set\treorder\n\t");
}
		
static void ft2_setDmem(unsigned int base, unsigned int size)
{
	__asm__ __volatile__(
		".set\tnoreorder\n\t"
		/* --- invalidate the icache and dcache with a 0->1 transition */
		"mfc0\t$0,$20\n\t"
		"nop\n\t"
		"nop\n\t"
		"li\t$8,0x00000202\n\t"
		"mtc0\t$8,$20\n\t"
		"nop\n\t"
		"nop\n\t"
		/* --- initialize and start COP3 */
		"mfc0\t$8,$12\n\t"			
		"nop\n\t"
		"nop\n\t"
		"or\t$8,0x80000000\n\t"
		"mtc0\t$8,$12\n\t"
		"nop\n\t"
		"nop\n\t"
 		/* --- load dram base and top */
		"move\t$8,$4\n\t"
		"la\t$9,0x0fffe000\n\t"
		"and\t$8,$8,$9\n\t"
		"mtc3\t$8,$4\n\t"
		"nop\n\t"
		"nop\n\t"
		"add\t$8,$8,$5\n\t"
		"addiu\t$8,$8,-1\n\t"
		"mtc3\t$8,$5\n\t"
		"nop\n\t"
		"nop\n\t"
		/* --- enable icache and dcache */
		"mtc0\t$0,$20\n\t"	/* CCTL */
		"nop\n\t"
		"nop\n\t"
		"jr\t$31\n\t"
		"nop\n\t"
		".set\treorder\n\t");
}

int32 ft2_testDmem(uint32 caseNo)
{
	unsigned int *p_buf;
	unsigned int DW_base;
	unsigned int DW_top;
	unsigned int dmem;
	unsigned int base;
	unsigned int ofst;
	unsigned int size = 8*1024;	/* for 865xC (DMem size = 8K) */
	u8 result = 0;
	
	ft2_getDmem(&DW_base, &DW_top);
	dmem = DW_base | 0x80000000;
	/*
	rtlglue_printf("debug: ft2_getDmem # pDW_base = 0x%x, pDW_top = 0x%x, Dmem = 0x%x \n", DW_base, DW_top, dmem);
	*/
	
	p_buf = rtlglue_malloc(size*2);	/* alloc double size(16k) for alignment */
	if (p_buf == NULL)
		return FAILED;
	
	base = ((unsigned int)p_buf+size) & 0xffffc000;	/* mask 8k alignment space */
	
	/* store dmem */
	for (ofst=0; ofst<size; ofst+=4)
		*(unsigned int*) (base + ofst) = *(unsigned int*) (dmem + ofst);
	
	ft2_setDmem(base, size);
	
	for (ofst=0; ofst<size; ofst+=4)
		*(unsigned int*) (base + ofst) = 0x55aa33cc + ofst;
	
	for (ofst=0; ofst<size; ofst+=4)
		if (*(unsigned int*) (base + ofst) != 0x55aa33cc + ofst)
			break;
	
	if (ofst >= size)
		result = 1;
	else
		result = 2;
	
	ft2_setDmem(dmem, size);
	
	/* recovery dmem */
	for (ofst=0; ofst<size; ofst+=4)
		*(unsigned int*) (dmem + ofst) = *(unsigned int*) (base + ofst);
	
	rtlglue_free(p_buf);	/* release sdram */
	/*
	rtlglue_printf("TEST D-mem ...end!\n");
	*/

	if (result == 1) {
		return SUCCESS;
	}
	
	return FAILED;
}


int32 ft2_testSram_BIST_Status(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */
	
	rtlglue_printf("debug: &BISTCR = %08x, BISTCR = %08x\n", BISTCR, REG32(BISTCR));
	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));

	if ((REG32(BISTCR) & 0x000007ff) == 0)	/* BIST testing finished */
		return SUCCESS;

	return FAILED;
}


int32 ft2_testSram_BIST_RLMRAM(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */

	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 25))
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_MIBRAM(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */

	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 24))
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_AMI(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */

	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 23))
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_HTRAM(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */

	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 22))
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_L2RAM(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */

	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 21))
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_L4RAM(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */

	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 20))
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_ACLRAM126(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */
	
	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 19))
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_ACLRAM125(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */
	
	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 18))
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_NICTXRAM(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */
	
	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 17))
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_NICRXRAM(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */
	
	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 16))
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_V4KRAM(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */
	
	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 15))
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_OQRAM(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */
	
	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (0x7f << 8))	/* 14~08: OQRAM failed */
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_IQRAM(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */
	
	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (0x3 << 6))	/* 07~06: IQRAM(256x165_1, 256x165_0) failed */
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_HSARAM125(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */
	
	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 5))	/* 05: HSARAM(512x125) failed */
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_HSARAM121(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */
	
	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (1 << 4))	/* 04: HSARAM(512x121) failed */
		return FAILED;

	return SUCCESS;
}


int32 ft2_testSram_BIST_PBRAM(uint32 caseNo)
{
	if ( model_setTestTarget( IC_TYPE_REAL ) == VMAC_ERROR_NON_AVAILABLE ) return SUCCESS; /* Not support, we don't compare. */
	
	rtlglue_printf("debug: &BISTTSDR0 = %08x, BISTTSDR0 = %08x\n", BISTTSDR0, REG32(BISTTSDR0));
	
	if (REG32(BISTTSDR0) & (0xf << 0))	/* 03~00: PBRAM failed */
		return FAILED;

	return SUCCESS;
}

int32 ft2_testPCI_S0(uint32 caseNo)
{
	int fd;
	char read_buf;
	int result = 0;
	
	fd = rtlglue_open("/proc/net/rtl8169/rtl0/ft2_pci", O_RDONLY, 0);
	if (fd >= 0) {
		int ret;
		ret = rtlglue_read(fd, &read_buf, 1);
		if (ret == 1 && read_buf == '1')
			result = 1;
		else
			result = 2;
		rtlglue_close(fd);
	}

	if (result == 1) {
		return SUCCESS;
	}
	
	return FAILED;
}


int32 ft2_testUSB_P0_Detect(uint32 caseNo)
{
	int fd;
	
	//fd = rtlglue_open("/proc/net/rtl8187/wlan0/ft2_usb", O_RDONLY, 0);
	fd = rtlglue_open("/proc/net/rtl8187/wlan0/stats-rx", O_RDONLY, 0);

	if (fd >= 0) {
		rtlglue_close(fd);
		return SUCCESS;
	}
	
	return FAILED;
}

int32 ft2_testUSB_P0_Loopback(uint32 caseNo)
{
	int fd;
	char read_buf;
	int result = 0;
	
	fd = rtlglue_open("/proc/net/rtl8187/wlan0/ft2_usb", O_RDONLY, 0);
	if (fd >= 0) {
		int ret;
		ret = rtlglue_read(fd, &read_buf, 1);
		if (ret == 1 && read_buf == '1')
			result = 1;
		else
			result = 2;
		rtlglue_close(fd);
	}
	
	if (result == 1) {
		return SUCCESS;
	}
	
	return FAILED;
}

int32 ft2_testUSB_P1_Detect(uint32 caseNo)
{
	int fd;
	
	//fd = rtlglue_open("/proc/net/rtl8187/wlan1/ft2_usb", O_RDONLY, 0);
	fd = rtlglue_open("/proc/net/rtl8187/wlan1/stats-rx", O_RDONLY, 0);
	
	if (fd >= 0) {
		rtlglue_close(fd);
		return SUCCESS;
	}
	
	return FAILED;
}

int32 ft2_testUSB_P1_Loopback(uint32 caseNo)
{
	int fd;
	char read_buf;
	int result = 0;
	
	fd = rtlglue_open("/proc/net/rtl8187/wlan1/ft2_usb", O_RDONLY, 0);
	if (fd >= 0) {
		int ret;
		ret = rtlglue_read(fd, &read_buf, 1);
		if (ret == 1 && read_buf == '1')
			result = 1;
		else
			result = 2;
		rtlglue_close(fd);
	}
	
	if (result == 1) {
		return SUCCESS;
	}
	
	return FAILED;
}


int32 ft2_testUSB_PowerOn(uint32 caseNo)
{
#define	RTL865XC_EHCI_PORT0	0xb8021054
#define	RTL865XC_EHCI_PORT1	0xb8021058
		
	uint32 RegCS = 0;
	
	/* USB Port-0: PowerOn  */
	RegCS = REG32(RTL865XC_EHCI_PORT0);
	RegCS |= 0x00100000;
	REG32(RTL865XC_EHCI_PORT0) = RegCS;
	
	/* USB Port-1: PowerOn  */
	RegCS = REG32(RTL865XC_EHCI_PORT1);
	RegCS |= 0x00100000;
	REG32(RTL865XC_EHCI_PORT1) = RegCS;
	
	return SUCCESS;
}

int32 ft2_testUSB_PowerOff(uint32 caseNo)
{
#define	RTL865XC_EHCI_PORT0	0xb8021054
#define	RTL865XC_EHCI_PORT1	0xb8021058
	
	uint32 RegCS = 0;
	
	/* USB Port-0: PowerOff  */
	RegCS = REG32(RTL865XC_EHCI_PORT0);
	RegCS &= 0xffefffff;
	REG32(RTL865XC_EHCI_PORT0) = RegCS;
	
	/* USB Port-1: PowerOff  */
	RegCS = REG32(RTL865XC_EHCI_PORT1);
	RegCS &= 0xffefffff;
	REG32(RTL865XC_EHCI_PORT1) = RegCS;

	mdelay(100);
	
	return SUCCESS;
}

int32 ft2_testUSB_IncreaseVoltage(uint32 caseNo)
{
	uint32 phyReg = 0x3EA5;	
	
	/* 2007-06/28: Phy6 Reg24 => write 0x3ea5 */
	REG32(0xbb804004) = 0x80000000 | phyReg | (0x06<<24) | (0x18<<16);	/* Phy-ID = 6 */
	do {
		phyReg = REG32(0xbb804008);
	} while ((phyReg & 0x80000000) != 0);
	
	return SUCCESS;
}

int32 ft2_testUSB_DecreaseVoltage(uint32 caseNo)
{
	uint32 phyReg = 0x32A5;
	
	/* 2007-06/28: Phy6 Reg24 => write 0x3ea5 */
	REG32(0xbb804004) = 0x80000000 | phyReg | (0x06<<24) | (0x18<<16);	/* Phy-ID = 6 */
	do {
		phyReg = REG32(0xbb804008);
	} while ((phyReg & 0x80000000) != 0);
	
	return SUCCESS;
}


