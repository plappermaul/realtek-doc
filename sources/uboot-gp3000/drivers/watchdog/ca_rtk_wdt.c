/*
RTK: Not init by default, use RTK command : watchdog
to init and enable watchdog.	
Use: boot_by_tryactive call "run en_wdt". ( en_wdt=watchdog 60)
*/

#include <common.h>
#include <asm/io.h>
#include <asm/processor.h>
#include <watchdog.h>
#ifndef BIT
#define BIT(nr)            (1 << (nr))
#endif
/* PER_WDT Registers */
#define CORTINA_WDT_CTRL	0x00		/* control */
#define   WDT_CTRL_WDTEN	  BIT(0)
#define   WDT_CTRL_RSTEN	  BIT(1)
#define   WDT_CTRL_CLKSEL	  BIT(2)
#define CORTINA_WDT_PS		0x04		/* pre-scaler load value */
#define CORTINA_WDT_DIV		0x08		/* divider for the pre-scaler */
#define CORTINA_WDT_LD		0x0C		/* load value */
#define CORTINA_WDT_LOADE	0x10		/* load enable */
#define   WDT_LOADE_WDT		  BIT(0)
#define   WDT_LOADE_PRE		  BIT(1)
#define CORTINA_WDT_CNT		0x14		/* instantaneous value */
#define CORTINA_WDT_IE_0	0x18		/* interrupt enable */
#define   WDT_IE_WDTE		  BIT(0)
#define CORTINA_WDT_INT_0	0x1C		/* interrupt */
#define   WDT_INT_WDTI		  BIT(0)
#define CORTINA_WDT_STAT_0	0x20		/* interrupt status */
#define   WDT_STAT_WDTS		  BIT(0)

/* Global Config Register */
#define WD_RESET_SUBSYS_ENABLE	BIT(4)
#define WD_RESET_ALL_BLOCKS	BIT(6)
#define WD_RESET_REMAP		BIT(7)
#define WD_RESET_EXT_RESET	BIT(8)
#define EXT_RESET		BIT(9)

#define CORTINA_CA_REG_READ(off)                CA_REG_READ(off)
#define CORTINA_CA_REG_WRITE(data, off)         CA_REG_WRITE(data, off)
#define WDT_BASE    PER_WDT_CTRL
#ifdef CORTINA_PER_IO_FREQ
#define wdt_clock_f  CORTINA_PER_IO_FREQ
#else
#define wdt_clock_f  125000000
#endif
#define WDT_DEFAULT_TIMEOUT   30
struct wdt_setting {
	int prescaler ;
	int ps_div;
	unsigned int wdt_timeout;
};

struct wdt_setting wdt = {0, 0, WDT_DEFAULT_TIMEOUT};
void hw_watchdog_reset(void);
static int get_wdt_clock(void){
	return wdt_clock_f;
}

void  hw_watchdog_settimeout(u32 timeout){
	if( timeout > 0)
		wdt.wdt_timeout = timeout;	
}

static int hw_watchdog_start(void)
{
	
	unsigned int data = 0;

	CORTINA_CA_REG_WRITE(wdt.prescaler, WDT_BASE + CORTINA_WDT_PS);

	CORTINA_CA_REG_WRITE(wdt.ps_div, WDT_BASE + CORTINA_WDT_DIV);

	CORTINA_CA_REG_WRITE(wdt.wdt_timeout, WDT_BASE + CORTINA_WDT_LD);
	/*
	printf("hw_watchdog_start: timeout=%u\n", wdt.wdt_timeout);
	*/
	CORTINA_CA_REG_WRITE(WDT_IE_WDTE, WDT_BASE + CORTINA_WDT_IE_0);
	/* Reset on timeout if specified in device tree. */

	data |= WDT_CTRL_RSTEN;

	data |= WDT_CTRL_WDTEN;
	CORTINA_CA_REG_WRITE(data, WDT_BASE + CORTINA_WDT_CTRL);
	
	return 0;
}

void watchdog_reset(void){
#ifdef CONFIG_HW_WATCHDOG
	hw_watchdog_reset();
#endif
}
						
void hw_watchdog_reset(void)
{
	u32 val;

	val = WDT_LOADE_WDT | WDT_LOADE_PRE;
	CORTINA_CA_REG_WRITE(val, WDT_BASE + CORTINA_WDT_LOADE);
}

void hw_watchdog_disable(void)
{
	u32 val;

	val = CORTINA_CA_REG_READ(WDT_BASE + CORTINA_WDT_CTRL);
	val &= ~WDT_CTRL_WDTEN;
	CORTINA_CA_REG_WRITE(val, WDT_BASE + CORTINA_WDT_CTRL);
}


void hw_watchdog_init(void)
{
	u32 val;
	val = CORTINA_CA_REG_READ(GLOBAL_GLOBAL_CONFIG);
	val |= WD_RESET_SUBSYS_ENABLE | WD_RESET_ALL_BLOCKS |
		       WD_RESET_REMAP | WD_RESET_EXT_RESET;

	CORTINA_CA_REG_WRITE(val, GLOBAL_GLOBAL_CONFIG);
		
	wdt.prescaler =  get_wdt_clock() / 1000 - 1; /* 1ms */
	wdt.ps_div = 1000; /* 1s */
	hw_watchdog_start();
	/* reset count to load value */
	hw_watchdog_reset();
}

