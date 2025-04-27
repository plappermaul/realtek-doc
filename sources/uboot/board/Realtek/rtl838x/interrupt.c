#include <common.h>
#include <asm/mipsregs.h>
#include <interrupt.h>
#include "rtl8380sreg.h"
#include <rtk_type.h>
#include "interrupt_reg.h"


#ifdef CONFIG_INTERRUPT_SUPPORT

#if 0
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

#endif

#define SHOWMESSAGE(msg, argv...) printf("[%s][%d]" msg, __FILE__, __LINE__, ##argv) 

#define DIVF_OFFSET		16
#define DIVISOR			55
#define HZ			100
#define MHZ			200
#define TCD_OFFSET		8


/*
 *   RTL8380 Interrupt Scheme (Subject to change)
 *
 *   Source		EXT_INT   IRQ      CPU INT
 *   --------	-------   ------   -------
 *   UART0        31        31       2
 *   UART1        30        30       1
 *   TIMER0       29        29       5
 *   TIMER1       28        28       1
 *   OCPTO        27        27       1
 *   HLXTO        26        26       1
 *   SLXTO        25        25       1
 *   NIC          24        24       4
 *   GPIO_ABC     23        23       4
 *   SECURITY_IE  22        22
 *   SWCORE_IE    20        20
 *   WDT_PH1TO_IE 19
 *   WDT_PH2TO_IE 18
 */

static rtk_dev_t isr_vector[RTK_DEV_MAX];

#ifdef CONFIG_INTERRUPT_SUPPORT
__attribute__((nomips16))
#endif /*CONFIG_INTERRUPT_SUPPORT*/
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
#endif /*CONFIG_INTERRUPT_SUPPORT*/
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
#endif /*CONFIG_INTERRUPT_SUPPORT*/
void common_enable_irq(unsigned int irq){
	unsigned int status;
        uint32  val;
	//lock
    val = MEM32_READ(0xb8003114);
    val |= (1 << 29);
    MEM32_WRITE(0xb8003114, val);

	val = MEM32_READ(GIMR);
       val |= (1 << irq);
       MEM32_WRITE(GIMR, val);

	status = read_c0_status();

	status &= ~( 1<<(2) ); /*Clear ERL (Error Level) bit*/
	status &= ~( 1<<(1) ); /*Clear EXL (Exception Level) bit*/
	status &= ~( 1<<(15) );	 /*Clear IM7 bit*/
	switch(irq){
		case TC0_IRQ:
			val = MEM32_READ(TCCNR);
                       val |= TC0EN | TC0MODE_TIMER;
                       MEM32_WRITE(TCCNR, val);

			val = MEM32_READ(TCIR);
                       val |= TC0IE;
                       MEM32_WRITE(TCIR, val);

			status |= ( 1<<(8+6) );
			break;
		case NIC_IRQ:
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
	MEM32_WRITE(GIMR, MEM32_READ(GIMR) & (~(1 << irq)));
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

    unsigned int original_data_intRd;
    unsigned int original_data_chipRd;
    unsigned int temp = 0;
    unsigned int temp_chip_info = 0;
    uint32      val;

    original_data_intRd = MEM32_READ(0xBB000058);
    MEM32_WRITE(0xBB000058, (original_data_intRd | 0x3));
    original_data_chipRd = MEM32_READ(0xBB0000D8);
    MEM32_WRITE(0xBB0000D8, (original_data_chipRd | 0xA0000000));
    temp = MEM32_READ(0xBB0000D4);
    temp_chip_info = MEM32_READ(0xBB0000D8);
    MEM32_WRITE(0xBB0000D8, original_data_chipRd);
    MEM32_WRITE(0xBB000058, original_data_intRd);


	MEM32_WRITE(GIMR, 0);
#if 0
	REG32(TCCNR) = 0;
	REG32(TCIR) |= TC0IP;
#endif
	isr_vector[RTK_DEV_NIC].irq = NIC_IRQ;
	isr_vector[RTK_DEV_TC0].irq = TC0_IRQ;


	/* Clear Timer IP status */
    if ((temp_chip_info & 0xFFFF) == 0x0477)
    {
    	if (MEM32_READ(RTL8380TC_TCIR) & RTL8380TC_TC0IP)
    	{
    	       val = MEM32_READ(RTL8380TC_TCIR);
               val |= RTL8380TC_TC0IP;
    		MEM32_WRITE(RTL8380TC_TCIR, val);
    	}

    	MEM32_WRITE(RTL8380TC_TCCNR, 0); /* disable timer before setting CDBR */

    	MEM32_WRITE(RTL8380TC_CDBR, (DIVISOR) << RTL8380TC_DIVF_OFFSET);
    	MEM32_WRITE(TC0DATA, ((MHZ * 1000000)/(DIVISOR * HZ)) << RTL8380TC_TCD_OFFSET);

    	MEM32_WRITE(RTL8380TC_TCCNR, RTL8380TC_TC0EN | RTL8380TC_TC0MODE_TIMER);
    	MEM32_WRITE(RTL8380TC_TCIR, RTL8380TC_TC0IE);
    }
    else
    {
    	if (MEM32_READ(RTL8380MP_TC0INT) & RTL8380MP_TCIP)
    	{
    	    val = MEM32_READ(RTL8380MP_TC0INT);
            val |= RTL8380MP_TCIP;
    	    MEM32_WRITE(RTL8380MP_TC0INT, val);
    	}

    	MEM32_WRITE(RTL8380MP_TC0CTL, 0); /* disable timer before setting CDBR */

        MEM32_WRITE(TC0DATA, ((MHZ * 1000000)/(DIVISOR * HZ)));
        MEM32_WRITE(RTL8380MP_TC0CTL, RTL8380MP_TCEN | RTL8380MP_TCMODE_TIMER | DIVISOR);
        MEM32_WRITE(RTL8380MP_TC0INT, RTL8380MP_TCIE);
    }

#if 0
	/* Clear Timer IP status */
	if (REG32(TCIR) & TC0IP)
		REG32(TCIR) |= TC0IP;

	/* Here irq->handler is passed from outside */

	REG32(TCCNR) = 0; /* disable timer before setting CDBR */
	REG32(CDBR) = (DIVISOR) << DIVF_OFFSET;
	REG32(TC0DATA) = ((MHZ * 1000000)/(DIVISOR * HZ)) << TCD_OFFSET;

	REG32(TCCNR) = TC0EN | TC0MODE_TIMER;
	REG32(TCIR) = TC0IE;
#endif
#if 0
	/* Set GIMR, IRR */
	REG32(GIMR) = TC0_IE;
#endif

	MEM32_WRITE(IRR0, IRR0_SETTING);
	MEM32_WRITE(IRR1, IRR1_SETTING);
	/* Reserved Interrupt routine*/
	/* REG32(IRR2) = IRR2_SETTING; */
	/* REG32(IRR3) = IRR3_SETTING; */
}


//INTERRUPT_SEGMENT
#ifdef CONFIG_INTERRUPT_SUPPORT
__attribute__((nomips16))
#endif /*CONFIG_INTERRUPT_SUPPORT*/
void common_irq_dispatch(void){
	unsigned int cpuint_ip ;

	//Read Cause
//	cpuint_ip  = read_32bit_cp0_register(CP0_STATUS);
	cpuint_ip  = read_c0_status();				
//	cpuint_ip &= read_32bit_cp0_register(CP0_CAUSE);
	cpuint_ip &= read_c0_cause();
	cpuint_ip &= ST0_IM;

	if( cpuint_ip & CAUSEF_IP6 ){
		unsigned int extint_ip = MEM32_READ(GIMR) & MEM32_READ(GISR);
		uboot_isr_t action = NULL;
		if (extint_ip & TC0_IP){
			action = (uboot_isr_t) isr_vector[RTK_DEV_TC0].isr;
			if( NULL!=action ){
				action(isr_vector[RTK_DEV_TC0].isr_param);
			}
		}
	}else if( cpuint_ip & CAUSEF_IP5 ){
		unsigned int extint_ip = MEM32_READ(GIMR) & MEM32_READ(GISR);
		uboot_isr_t action = NULL;
		if (extint_ip & NIC_IP){
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
//	SHOWMESSAGE("IRQ Exit!\n");
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
    /* This timer ISR executes every 110 ms */


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

