#include <common.h>
//#include <soc.h>
#include <misc_setting.h>
#include <swp.h>


//#define OTTO_CID (((*(soc_t *)(0x9F000020)).cid) & 0xFFFF)

/***************************************************************************
 * GPIO Function 
 ***************************************************************************/
//#define DEBUG_GPIO
#define GPIO_MAX_NUM			(95)

#define SWP_GPIO_INPUT			(0)
#define SWP_GPIO_OUTPUT			(1)

#define SWCORE_APRO_GPIO_EN             (0xBB000038)
#define SWCORE_6608_GPIO_EN             (0xBB00003C)

#define SOC_GPIO_BASE			(0xB8003300)
#define SOC_GPIO_PABCD_DIR_OFFSET	(0x08)
#define SOC_GPIO_PABCD_DAT_OFFSET	(0x0C)
#define SOC_GPIO_PEFGH_DIR_OFFSET	(0x24)
#define SOC_GPIO_PEFGH_DAT_OFFSET	(0x28)
#define SOC_GPIO_PJKMN_DIR_OFFSET	(0x48)
#define SOC_GPIO_PJKMN_DAT_OFFSET	(0x4C)
static int swcore_gpio_en_reg = 0;

void swp_gpioMode (int gpio_num, int gpio_dir)
{
	int regGroup, regAddr, regMask;

#ifdef DEBUG_GPIO  
	printf("Set GPIO #%d dir. as %s\n", gpio_num, (gpio_dir?"Output":"Input"));
#endif

	if ((gpio_num <= GPIO_MAX_NUM) && (gpio_num >= 0)) {
		regGroup = gpio_num >> 5;
		regMask = 1 << (gpio_num % 32);

		/* Swicth Core */
		if (swcore_gpio_en_reg == 0) {
#ifdef DEBUG_GPIO
			printf("Please define SWCORE_GPIO_EN reg. !!!\n");
#endif
			return;
		}
		else {
			REG32((swcore_gpio_en_reg + (regGroup << 2))) |= regMask;
		}

		if (regGroup == 0)
			regAddr = SOC_GPIO_BASE + SOC_GPIO_PABCD_DIR_OFFSET;
		else if (regGroup == 1)
			regAddr = SOC_GPIO_BASE + SOC_GPIO_PEFGH_DIR_OFFSET;
		else if (regGroup == 2)
			regAddr = SOC_GPIO_BASE + SOC_GPIO_PJKMN_DIR_OFFSET;

#ifdef DEBUG_GPIO 
		printf("GPIO: Addr: 0x%08x; Bit: 0x%08x\n", regAddr, regMask);
#endif

		/* Select mode */
		if (SWP_GPIO_INPUT == gpio_dir)
			REG32(regAddr) &= ~regMask;
		else
			REG32(regAddr) |= regMask;
	}
	else {
		printf("ERROR: GPIO %d is not supported!!!\n", gpio_num);
	}
}

void swp_gpioData(int gpio_num, int gpio_data)
{
	int regGroup, regAddr, regMask;

#ifdef DEBUG_GPIO  
	printf("Set GPIO #%d data as %d\n", gpio_num, gpio_data);
#endif

	if ((gpio_num <= GPIO_MAX_NUM) && (gpio_num >= 0)) {
		regGroup = gpio_num >> 5;
		regMask = 1 << (gpio_num % 32);

		if (regGroup == 0)
			regAddr = SOC_GPIO_BASE + SOC_GPIO_PABCD_DAT_OFFSET;
		else if (regGroup == 1)
			regAddr = SOC_GPIO_BASE + SOC_GPIO_PEFGH_DAT_OFFSET;
		else if (regGroup == 2)
			regAddr = SOC_GPIO_BASE + SOC_GPIO_PJKMN_DAT_OFFSET;

#ifdef DEBUG_GPIO 
		printf("GPIO: Addr: 0x%08x; Bit: 0x%08x\n", regAddr, regMask);
#endif

		if(gpio_data == 0)
			REG32(regAddr) &= ~regMask;
		else
			REG32(regAddr) |= regMask;
	}
	else {
		printf("ERROR: GPIO %d is not supported!!!\n", gpio_num);
	}
}

/***************************************************************************
 * LED Function 
 ***************************************************************************/
#define POWER_LED_GPIO_HIGH_ACTIVE	1

static int gpio_led_power = 0;
static int gpio_led_power_2 = 0;
static int gpio_led_power_3 = 0;
static int gpio_led_los = 0;
static int gpio_led_pon = 0;

#ifdef CONFIG_MULTICAST_UPGRADE
void led_multicastUP_rx_data(void) {
	static int cnt = 0;
	static int check_gpio_set = 0;
	static int op = 0;

	if(cnt++ >= 10) {
		if(check_gpio_set == 0) {
			swp_gpioMode(gpio_led_pon, SWP_GPIO_OUTPUT);
			check_gpio_set = 1;
		}
		if(op == 0)
			swp_gpioData(gpio_led_pon, 1);
		else	
			swp_gpioData(gpio_led_pon, 0);
		op = !op;
		cnt = 0;
	}
}

void led_multicastUP_rx_drop(void) {
	static int cnt = 0;
	static int check_gpio_set = 0;
	static int op = 0;

	if(cnt++ >= 50) {
		if(check_gpio_set == 0) {
			swp_gpioMode(gpio_led_pon, SWP_GPIO_OUTPUT);
			check_gpio_set = 1;
		}
		if(op == 0)
			swp_gpioData(gpio_led_pon, 1);
		else	
			swp_gpioData(gpio_led_pon, 0);
		op = !op;
		cnt = 0;
	}
}

void led_multicastUP_updating(void) {
	swp_gpioData(gpio_led_pon, 0);
}

void led_multicastUP_error_hang(void) {
	swp_gpioData(gpio_led_pon, 1);
	swp_gpioMode(gpio_led_los, SWP_GPIO_OUTPUT);

	while(1) {
		swp_gpioData(gpio_led_los, 0);
		udelay (1000000);
		swp_gpioData(gpio_led_los, 1);
		udelay (1000000);
	};
}

void led_multicastUP_done(void) {
	swp_gpioMode(gpio_led_power, SWP_GPIO_OUTPUT);
	swp_gpioMode(gpio_led_los, SWP_GPIO_OUTPUT);
	swp_gpioMode(gpio_led_pon, SWP_GPIO_OUTPUT);

	while(1) {
		(POWER_LED_GPIO_HIGH_ACTIVE) ? swp_gpioData(gpio_led_power, 1) : swp_gpioData(gpio_led_power, 0);
		swp_gpioData(gpio_led_power_2, 0);
		swp_gpioData(gpio_led_power_3, 0);
		swp_gpioData(gpio_led_los, 0);
		swp_gpioData(gpio_led_pon, 0);
		udelay (1000000);
		(POWER_LED_GPIO_HIGH_ACTIVE) ? swp_gpioData(gpio_led_power,0) : swp_gpioData(gpio_led_power,1);
		swp_gpioData(gpio_led_power_2, 1);
		swp_gpioData(gpio_led_power_3, 1);
		swp_gpioData(gpio_led_los, 1);
		swp_gpioData(gpio_led_pon, 1);
		udelay (1000000);
	};
}

#endif

void led_power_on(void) {

	if ((OTTO_SID == PLR_SID_APRO) || (OTTO_SID == PLR_SID_APRO_GEN2)) {
		swcore_gpio_en_reg = SWCORE_APRO_GPIO_EN;
	switch(OTTO_CID) {
	case ST_RTL9603C:
	case ST_RTL9607C:
	case ST_RTL9606C:
		gpio_led_power = 60;
		gpio_led_power_2 = 15;
		gpio_led_power_3 = 6;
		gpio_led_los = 21;
		gpio_led_pon = 10;
		swp_gpioMode(gpio_led_power, SWP_GPIO_OUTPUT);
		swp_gpioMode(gpio_led_power_2, SWP_GPIO_OUTPUT);
		swp_gpioMode(gpio_led_power_3, SWP_GPIO_OUTPUT);
		swp_gpioMode(66, SWP_GPIO_OUTPUT);
		(POWER_LED_GPIO_HIGH_ACTIVE) ? swp_gpioData(gpio_led_power, 1) : swp_gpioData(gpio_led_power, 0);
		swp_gpioData(gpio_led_power_2, 0);
		swp_gpioData(gpio_led_power_3, 0);
		swp_gpioData(66, 0);
		break;
	case ST_RTL9603CP:
		break;
	case ST_RTL9607CP:
		gpio_led_power = 19;
		gpio_led_los = 21;
		gpio_led_pon = 10;
		swp_gpioMode(gpio_led_power, SWP_GPIO_OUTPUT);
		swp_gpioData(gpio_led_power, 0);
		break;
	case ST_RTL9607E:
		break;
	case ST_RTL9607EP:
		break;
	default:
		break;            
	}
	} else if (OTTO_SID == PLR_SID_9603CVD){
		swcore_gpio_en_reg = SWCORE_6608_GPIO_EN;
		switch(OTTO_CID) {
			case ST_RTL9603CVD4CG:
			case ST_RTL9603CVD5CG:
			case ST_RTL9603CVD6CG:
				gpio_led_power = 15;
				gpio_led_los = 21;
				gpio_led_pon = 10;
				swp_gpioMode(gpio_led_power, SWP_GPIO_OUTPUT);
				swp_gpioData(gpio_led_power, 0);
			break;
			case ST_RTL9601DVD3CG:
			case ST_RTL9601DVD4CG:
				gpio_led_power = 35;
				gpio_led_los = 21;
				gpio_led_pon = 10;
				swp_gpioMode(gpio_led_power, SWP_GPIO_OUTPUT);
				swp_gpioData(gpio_led_power, 0);
				break;
			default:
				break;
		}
	}
}

void led_power_off(void) {
	
	if ((OTTO_SID == PLR_SID_APRO) || (OTTO_SID == PLR_SID_APRO_GEN2)) {
	switch(OTTO_CID) {
	case ST_RTL9603C:
	case ST_RTL9607C:
	case ST_RTL9606C:
		swp_gpioMode(gpio_led_power, SWP_GPIO_OUTPUT);
		(POWER_LED_GPIO_HIGH_ACTIVE) ? swp_gpioData(gpio_led_power, 0) : swp_gpioData(gpio_led_power, 1);
		swp_gpioData(gpio_led_power_2, 1);
		swp_gpioData(gpio_led_power_3, 1);
		break;
	case ST_RTL9603CP:
		break;
	case ST_RTL9607CP:
		swp_gpioMode(gpio_led_power, SWP_GPIO_OUTPUT);
		swp_gpioData(gpio_led_power, 1);
		break;
	case ST_RTL9607E:
		break;
	case ST_RTL9607EP:
		break;
	default: 
		break;            
	}
	} else if (OTTO_SID == PLR_SID_9603CVD){
		switch(OTTO_CID) {
			case ST_RTL9603CVD4CG:
			case ST_RTL9603CVD5CG:
			case ST_RTL9603CVD6CG:
				swp_gpioMode(gpio_led_power, SWP_GPIO_OUTPUT);
				swp_gpioData(gpio_led_power, 1);
			break;
			case ST_RTL9601DVD3CG:
			case ST_RTL9601DVD4CG:
				swp_gpioMode(gpio_led_power, SWP_GPIO_OUTPUT);
				swp_gpioData(gpio_led_power, 1);
				break;
			default:
				break;
		}
	}
}	

PATCH_REG(led_power_on, 0);
