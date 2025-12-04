

//set up tlb mmu for emmc test.
//test interrupt for emmc

//#define SOC_8197F 1
//#define SOC_8198F 1
#define SOC_8198X 1

//#include "basic_types.h"
//#include "diag.h"
#ifdef SOC_8197F
#include <asm/mipsregs.h>
extern void write_one_tlb(int index, u32 pagemask, u32 hi, u32 low0, u32 low1);
#endif

#include <common.h>
//#include "bspchip.h"

void setup_mmu_emmc( void )
{
#ifdef SOC_8197F
	int a, b, c;
	a=read_c0_config();
	b=read_c0_cause();
	c=read_c0_status();
	printf("cpu0 config=%x, cause=%x, status=%x\n", a,b, c);
	printf("date=%s, time=%s\n", __DATE__, __TIME__);
	// 0~64k-1 sd-ram is mapped
	write_one_tlb(0, PM_4K, 0<<PL_4K, (0<<6)|(2<<3)|7, (1<<6)|(2<<3)|7);
	__asm__ __volatile__("ssnop\t\n");
	__asm__ __volatile__("ssnop\t\n");
	write_one_tlb(1, PM_4K, 2<<PL_4K, (2<<6)|(2<<3)|7, (3<<6)|(2<<3)|7);
	__asm__ __volatile__("ssnop\t\n");
	__asm__ __volatile__("ssnop\t\n");
	write_one_tlb(2, PM_4K, 4<<PL_4K, (4<<6)|(2<<3)|7, (5<<6)|(2<<3)|7);
	__asm__ __volatile__("ssnop\t\n");
	__asm__ __volatile__("ssnop\t\n");
	write_one_tlb(3, PM_4K, 6<<PL_4K, (6<<6)|(2<<3)|7, (7<<6)|(2<<3)|7);
	__asm__ __volatile__("ssnop\t\n");
	__asm__ __volatile__("ssnop\t\n");
	write_one_tlb(4, PM_4K, 8<<PL_4K, (8<<6)|(2<<3)|7, (9<<6)|(2<<3)|7);
	__asm__ __volatile__("ssnop\t\n");
	__asm__ __volatile__("ssnop\t\n");
	write_one_tlb(5, PM_4K, 10<<PL_4K, (10<<6)|(2<<3)|7, (11<<6)|(2<<3)|7);
	__asm__ __volatile__("ssnop\t\n");
	__asm__ __volatile__("ssnop\t\n");
	write_one_tlb(6, PM_4K, 12<<PL_4K, (12<<6)|(2<<3)|7, (13<<6)|(2<<3)|7);
	__asm__ __volatile__("ssnop\t\n");
	__asm__ __volatile__("ssnop\t\n");
	write_one_tlb(7, PM_4K, 14<<PL_4K, (14<<6)|(2<<3)|7, (15<<6)|(2<<3)|7);
	__asm__ __volatile__("ssnop\t\n");
	__asm__ __volatile__("ssnop\t\n");

	// 0~16k-1 sram is mapped (address 1fe00000 >> 12 = 1fe00)
	write_one_tlb(8, PM_4K, 0x1fe00<<PL_4K, (0x1fe00<<6)|(2<<3)|7, (0x1fe01<<6)|(2<<3)|7);
	__asm__ __volatile__("ssnop\t\n");
	__asm__ __volatile__("ssnop\t\n");
	write_one_tlb(9, PM_4K, 0x1fe02<<PL_4K, (0x1fe02<<6)|(2<<3)|7, (0x1fe03<<6)|(2<<3)|7);
	__asm__ __volatile__("ssnop\t\n");
	__asm__ __volatile__("ssnop\t\n");
	write_one_tlb(10, PM_4K, 0x1fe04<<PL_4K, (0x1fe04<<6)|(2<<3)|7, (0x1fe05<<6)|(2<<3)|7);
	__asm__ __volatile__("ssnop\t\n");
	__asm__ __volatile__("ssnop\t\n");
	write_one_tlb(11, PM_4K, 0x1fe06<<PL_4K, (0x1fe06<<6)|(2<<3)|7, (0x1fe07<<6)|(2<<3)|7);
	__asm__ __volatile__("ssnop\t\n");
	__asm__ __volatile__("ssnop\t\n");
#endif

}

/* Register Macro */
#if defined(SOC_8198F) || defined(SOC_8198X)
#ifndef REG32
#define REG32(reg)      (*(volatile u32 *)(reg))
#endif
#ifndef REG8
#define REG8(reg)       (*(volatile u8 *)(reg))
#endif
#endif
	

// sd30/emmc interrupt start
#ifdef SOC_8197F
#define SD30_BASE                           0xb8015000
#elif defined(SOC_8198F) || defined(SOC_8198X)
#define SD30_BASE                           0xF4400000
#endif
#define wSRAM_CTL							(0x400 + SD30_BASE)       /* SRAM CTL */
#define wSD_ISR_REG                         (0x424 + SD30_BASE)       /* SD ISR */ 
#define wSD_ISREN_REG                       (0x428 + SD30_BASE)       /* SD ISREN */
#define bCARD_EXIST							(0x51f + SD30_BASE)       /* CARD EXIST */
#define bCARD_INT_EN_REG                    (0x520 + SD30_BASE)       /* CARD INT ENABLE */
#define bCARD_INT_PEND_REG                  (0x521 + SD30_BASE)       /* CARD INT PENDING */


#ifdef SOC_8197F
#define SD30_IRQ	(14)
#elif defined(SOC_8198F)
#define GIC_SPI_BASE 32
#define SD30_IRQ	(43+GIC_SPI_BASE)
#elif defined(SOC_8198X)
#define GIC_SPI_BASE 32
#define SD30_IRQ	(41+GIC_SPI_BASE)
#endif

void Emmc_ISR(void *data)
{
	unsigned char btemp, btemp1, btemp2;
	unsigned int wtemp;

	btemp = REG8(bCARD_INT_PEND_REG);
	btemp1 = (REG8(bCARD_EXIST))&0x24;
	if (btemp & 0x4) {
	    printf("card detect change, exist=%x\n", btemp1);
	}
/*	if (btemp1 & 0x4) {
		btemp1 = (REG8(bCARD_EXIST))&0x24;
		printf("exist=%x\n", btemp1);
	}
	if (btemp1 & 0x4) {
		btemp1 = (REG8(bCARD_EXIST))&0x24;
		printf("exist=%x\n", btemp1);
	}
	if (btemp1 & 0x4) {
		btemp1 = (REG8(bCARD_EXIST))&0x24;
		printf("exist=%x\n", btemp1);
	}
	if (btemp1 & 0x4) {
		btemp1 = (REG8(bCARD_EXIST))&0x24;
		printf("exist=%x\n", btemp1);
	}
	if (btemp1 & 0x4) {
		btemp1 = (REG8(bCARD_EXIST))&0x24;
		printf("exist=%x\n", btemp1);
	}
*/
	REG8(bCARD_INT_PEND_REG) = btemp;
	btemp2 = (REG8(bCARD_EXIST))&0x24;
	if ((btemp1 & 0x4) != (btemp2 & 0x4)) {
		printf("exist=%x\n", btemp2);
	}
	wtemp = REG32(wSD_ISR_REG);
	if (wtemp & 0x10) {
	    printf("emmc int4 dma transfer done\n");
	}
	if (wtemp & 0x4) {
	    printf("emmc int2 card error\n");
	}
	if (wtemp & 0x2) {
	    printf("emmc int1 card end\n");
	}
	REG32(wSD_ISR_REG) = wtemp;

}


void Start_emmc_carddetect(void)
{
	unsigned int wtemp;

	
	wtemp = REG32(wSRAM_CTL);
	wtemp |= 0x20;
	REG32(wSRAM_CTL) = wtemp;

	
	REG8(bCARD_INT_PEND_REG) = 0x4;
	REG8(bCARD_INT_EN_REG) = 0x4;

    /* Enable emmc interrupt */
	irq_install_handler(SD30_IRQ, Emmc_ISR, NULL);

	
}

void Start_emmc_int1(void)
{
	unsigned int wtemp;

	
	wtemp = REG32(wSRAM_CTL);
	wtemp |= 0x20;
	REG32(wSRAM_CTL) = wtemp;

	
	REG32(wSD_ISR_REG) = 0x2;
	REG32(wSD_ISREN_REG) = 0x3;
    /* Enable sd30 interrupt */

	irq_install_handler(SD30_IRQ, Emmc_ISR, NULL);

	REG32(wSD_ISR_REG) = 0x3;
}



void Start_emmc_int2(void)
{
	unsigned int wtemp;

	
	wtemp = REG32(wSRAM_CTL);
	wtemp |= 0x20;
	REG32(wSRAM_CTL) = wtemp;

	
	REG32(wSD_ISR_REG) = 0x4;
	REG32(wSD_ISREN_REG) = 0x5;
    /* Enable emmc interrupt */

	irq_install_handler(SD30_IRQ, Emmc_ISR, NULL);

	REG32(wSD_ISR_REG) = 0x5;
}


void Start_emmc_int4(void)
{
	unsigned int wtemp;

	
	wtemp = REG32(wSRAM_CTL);
	wtemp |= 0x20;
	REG32(wSRAM_CTL) = wtemp;

	
	REG32(wSD_ISR_REG) = 0x10;
	REG32(wSD_ISREN_REG) = 0x11;
    /* Enable emmc interrupt */

	irq_install_handler(SD30_IRQ, Emmc_ISR, NULL);

	REG32(wSD_ISR_REG) = 0x11;
}

