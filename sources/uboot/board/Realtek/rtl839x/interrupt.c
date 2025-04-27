#include <common.h>
//#include <asm/arch-rtl839x/rtk_soc_common.h>
#include <asm/mipsregs.h>
#include <interrupt.h>
#include <rtk_reg.h>
#include "rtl8390_soc_reg.h"
#include <rtk_type.h>

#ifdef CONFIG_INTERRUPT_SUPPORT

/*
 * Interrupt Routing Selection
 */
#define UART0_RS       2
#define UART1_RS       1
#define TC0_RS         5
#define TC1_RS         1
#define OCPTO_RS       1
#define HLXTO_RS       1
#define SLXTO_RS       1
#define NIC_RS         4
#define GPIO_ABCD_RS   4
#define	SWCORE_RS      3

#define IRR0_SETTING    ((UART0_RS  << RTL8390_SOC_VPE0_IRR0_UART0_RS_OFFSET) | \
                         (UART1_RS  << RTL8390_SOC_VPE0_IRR0_UART1_RS_OFFSET) | \
                         (TC0_RS    << RTL8390_SOC_VPE0_IRR0_TC0_RS_OFFSET) | \
                         (TC1_RS    << RTL8390_SOC_VPE0_IRR0_TC1_RS_OFFSET) | \
                         (OCPTO_RS  << RTL8390_SOC_VPE0_IRR0_OCPTO_RS_OFFSET) | \
                         (HLXTO_RS  << RTL8390_SOC_VPE0_IRR0_HLXTO_RS_OFFSET)  | \
                         (SLXTO_RS  << RTL8390_SOC_VPE0_IRR0_SLXTO_RS_OFFSET)  | \
                         (NIC_RS    << RTL8390_SOC_VPE0_IRR0_NIC_RS_OFFSET)    \
                        )

#define IRR1_SETTING    ((GPIO_ABCD_RS << RTL8390_SOC_VPE0_IRR1_GPIO_ABCD_RS_OFFSET) | \
                         (SWCORE_RS    << RTL8390_SOC_VPE0_IRR1_SWCORE_RS_OFFSET)   \
                        )

#define IRR2_SETTING    0
#define IRR3_SETTING    0

#define SHOWMESSAGE(msg, argv...) printf("[%s][%d]" msg, __FILE__, __LINE__, ##argv) 

#define DIVF_OFFSET		16
#define DIVISOR			55
#define HZ			100
#define MHZ			200
#define TCD_OFFSET		8


/*
 *   RTL8390 Interrupt Scheme (Subject to change)
 *
 *   Source     EXT_INT   IRQ      CPU INT
 *   --------   -------   ------   -------
 *   UART0      31        31       2
 *   UART1      30        30       1
 *   TIMER0     29        29       5
 *   TIMER1     28        28       1
 *   OCPTO      27        27       1
 *   HLXTO      26        26       1
 *   SLXTO      25        25       1
 *   NIC        24        24       4
 *   GPIO_ABCD  23        23       4
 *   SWCORE     20        20       3
 */

static rtk_dev_t isr_vector[RTK_DEV_MAX];

#ifdef CONFIG_INTERRUPT_SUPPORT
__attribute__((nomips16))
#endif
void common_enable_interrupt(void){
	unsigned int status;
//	status = read_32bit_cp0_register(CP0_STATUS);
	status = read_c0_status();
	status |= ( 1 );
	status &= ~(ST0_BEV);             //set BEV = 0
//	write_32bit_cp0_register(CP0_STATUS, status);
	write_c0_status(status);
}

#ifdef CONFIG_INTERRUPT_SUPPORT
__attribute__((nomips16))
#endif
void common_disable_interrupt(void){
	unsigned int status;
//	status = read_32bit_cp0_register(CP0_STATUS);
	status = read_c0_status();
	status &= ( ~1 );
	status |= (ST0_BEV);             //set BEV = 1
//	write_32bit_cp0_register(CP0_STATUS, status);
	write_c0_status(status);
}

#ifdef CONFIG_INTERRUPT_SUPPORT
__attribute__((nomips16))
#endif
void common_enable_irq(unsigned int irq){
	unsigned int status;
    unsigned int32 val;
	//lock
        val = MEM32_READ(SOC_BASE_ADDR | RTL8390_SOC_TCIR_ADDR);
        val |= RTL8390_SOC_TCIR_TC0IP_MASK;
        MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_TCIR_ADDR, val);
        MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_VPE0_GIMR_ADDR, 0);
        MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_VPE0_GIMR_ADDR, MEM32_READ(SOC_BASE_ADDR | RTL8390_SOC_VPE0_GIMR_ADDR) | (1 << irq));
//	status = read_32bit_cp0_register(CP0_STATUS);
	status = read_c0_status();

	status &= ~( 1<<(2) ); /*Clear ERL (Error Level) bit*/
	status &= ~( 1<<(1) ); /*Clear EXL (Exception Level) bit*/
	status &= ~( 1<<(15) );	 /*Clear IM7 bit*/
	switch(irq){
		case RTL8390_SOC_VPE1_GISR_TC0_IP_OFFSET:
			status |= ( 1<<(8+6) );
			break;
		case RTL8390_SOC_VPE1_GISR_NIC_IP_OFFSET:
			status |= ( 1<<(8+5) );
			break;
		default:
			break;
	}
//	write_32bit_cp0_register(CP0_STATUS, status);
	write_c0_status(status);
	//unlock
}

void common_disable_irq(unsigned int irq){
	//lock
	MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_VPE0_GIMR_ADDR, MEM32_READ(SOC_BASE_ADDR | RTL8390_SOC_VPE0_GIMR_ADDR) & (~(1 << irq)));
	//unlock
}

unsigned int uboot_isr_register(unsigned int dev_id, uboot_isr_t isr, \
		                       void* isr_param){
	rtk_dev_t* _dev;

    if (!IS_VALID_DEVICE_ID(dev_id)) {
        printf(" uboot_isr_register: Invalid device index %d\n", dev_id);
        return FAILED;
    }

    _dev = &isr_vector[dev_id];
    _dev->isr = isr;
    _dev->isr_param = isr_param;

    if ( _dev->irq != -1) {
    }
    else {
		printf("invalid irq number %d\n", _dev->irq);
	return FAILED;
    }

    return SUCCESS;
}


void common_irq_init(void){
        uint32  val;

	MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_TCCNR_ADDR, 0);

	val = MEM32_READ(SOC_BASE_ADDR | RTL8390_SOC_TCIR_ADDR);
       val |= (0x1 << RTL8390_SOC_TCIR_TC0IP_OFFSET);
       MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_TCIR_ADDR, val);


	isr_vector[RTK_DEV_NIC].irq = RTL8390_SOC_VPE1_GISR_NIC_IP_OFFSET;
	isr_vector[RTK_DEV_TC0].irq = RTL8390_SOC_VPE1_GISR_TC0_IP_OFFSET;


	/* Clear Timer IP status */
	if (MEM32_READ(SOC_BASE_ADDR | RTL8390_SOC_TCIR_ADDR) & (0x1 << RTL8390_SOC_VPE1_GISR_TC0_IP_OFFSET))
	{
	        val = MEM32_READ(SOC_BASE_ADDR | RTL8390_SOC_TCIR_ADDR);
               val |= (0x1 << RTL8390_SOC_VPE1_GISR_TC0_IP_OFFSET);
               MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_TCIR_ADDR, val);
	}

	/* Here irq->handler is passed from outside */

	MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_TCCNR_ADDR, 0); /* disable timer before setting CDBR */
	MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_CDBR_ADDR, (DIVISOR) << DIVF_OFFSET);
	MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_TC0DATA_ADDR, (((MHZ * 1000000)/(DIVISOR * HZ)) << TCD_OFFSET));

	MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_TCCNR_ADDR, (0x1 << RTL8390_SOC_TCCNR_TC0EN_OFFSET) | (0x1 << RTL8390_SOC_TCCNR_TC0MODE_OFFSET));
	MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_TCIR_ADDR, (0x1 << RTL8390_SOC_TCIR_TC0IE_OFFSET));

#if 0
	/* Set GIMR, IRR */
	REG32(GIMR) = TC0_IE;
#endif

	MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_VPE0_IRR0_ADDR, IRR0_SETTING);
	MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_VPE0_IRR1_ADDR, IRR1_SETTING);
	MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_VPE0_IRR2_ADDR, IRR2_SETTING);
	MEM32_WRITE(SOC_BASE_ADDR | RTL8390_SOC_VPE0_IRR3_ADDR, IRR3_SETTING);
}


//INTERRUPT_SEGMENT
#ifdef CONFIG_INTERRUPT_SUPPORT
__attribute__((nomips16))
#endif
void common_irq_dispatch(void){
	unsigned int cpuint_ip ;

	//Read Cause
//	cpuint_ip  = read_32bit_cp0_register(CP0_STATUS);
	cpuint_ip  = read_c0_status();				
//	cpuint_ip &= read_32bit_cp0_register(CP0_CAUSE);
	cpuint_ip &= read_c0_cause();
	cpuint_ip &= ST0_IM;

	if( cpuint_ip & CAUSEF_IP6 ){
		unsigned int extint_ip = MEM32_READ(SOC_BASE_ADDR | RTL8390_SOC_VPE0_GIMR_ADDR) & MEM32_READ(SOC_BASE_ADDR | RTL8390_SOC_VPE0_GISR_ADDR);
		uboot_isr_t action = NULL;
		if (extint_ip & (0x1 << RTL8390_SOC_VPE0_GISR_TC0_IP_OFFSET)){
			action = (uboot_isr_t) isr_vector[RTK_DEV_TC0].isr;
			if( NULL!=action ){
				action(isr_vector[RTK_DEV_TC0].isr_param);
			}
		}
	}else if( cpuint_ip & CAUSEF_IP5 ){
		unsigned int extint_ip = MEM32_READ(SOC_BASE_ADDR | RTL8390_SOC_VPE0_GIMR_ADDR) & MEM32_READ(SOC_BASE_ADDR | RTL8390_SOC_VPE0_GISR_ADDR);
		uboot_isr_t action = NULL;
		if (extint_ip & (0x1 << RTL8390_SOC_VPE0_GISR_NIC_IP_OFFSET)){
			action = (uboot_isr_t) isr_vector[RTK_DEV_NIC].isr;
			if( NULL!=action ){
				action(isr_vector[RTK_DEV_NIC].isr_param);
			}
		}
	}else if( cpuint_ip & CAUSEF_IP4 ){
	}else if( cpuint_ip & CAUSEF_IP3 ){
	}else if( cpuint_ip & CAUSEF_IP2 ){
	}else{
		SHOWMESSAGE("Unknow IRQ!\n");
		SHOWMESSAGE("Current IP:0x%08x\n", cpuint_ip);
	}
}

unsigned int volatile timer_flag_1sec = 0;
unsigned int volatile timer_uptime_sec = 0;

unsigned int volatile timer_flag_50ms = 0;
unsigned int volatile timer_flag_150ms = 0;
unsigned int volatile timer_flag_350ms = 0;


unsigned int volatile timer_cnt_150ms = 0;
unsigned int volatile timer_cnt_350ms = 0;
unsigned int volatile timer_cnt_1s = 0;

void timer_isr(void* noUse)
{
    unsigned int32 val;
    /* This timer ISR executes every 10 ms */


    /* Clear TC0 ISR */
    val = MEM32_READ(0xb8003114);
    val |= (1 << 29);
    MEM32_WRITE(0xb8003114, val);

    timer_cnt_150ms++;
    timer_cnt_350ms++;
    timer_cnt_1s++;

    timer_flag_50ms = 1;
       
    if (timer_cnt_150ms >= 25)
    {
        timer_cnt_150ms = 0;
        timer_flag_150ms = 1;

    }

    if (timer_cnt_350ms >= 35)
    {
        timer_cnt_350ms = 0;
        timer_flag_350ms = 1;

    }
    
    if(timer_cnt_1s >= 100)
    {
        timer_cnt_1s = 0;
        timer_flag_1sec = 1;
        timer_uptime_sec++;
    }

  //REG32(0xbb004718) = timer_uptime_sec;
}

#endif /*CONFIG_INTERRUPT_SUPPORT*/

