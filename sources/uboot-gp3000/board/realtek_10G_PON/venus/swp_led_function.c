
#include <stdio.h>
void rtk_gpio_led_out(int gpio_num, unsigned char value);
void rtk_gpio_leds_cfg_mode(int gpio_num, int dir);
#define swp_gpioData rtk_gpio_led_out
#define swp_gpioMode rtk_gpio_leds_cfg_mode
#define SWP_GPIO_OUTPUT 0

#define gpio_led_power 11
#define gpio_led_pon 9
#define gpio_led_los 10
#define gpio_led_power_2 6 // LED_USB_1
#define gpio_led_power_3 7 // LED_USB_0
#define POWER_LED_GPIO_HIGH_ACTIVE 1

#ifdef CONFIG_MULTICAST_UPGRADE

void
led_multicastUP_rx_data(void)
{
	static int cnt = 0;
	static int check_gpio_set = 0;
	static int op = 0;

	if(cnt++ >= 10) {
		if(check_gpio_set == 0) {
			swp_gpioMode(gpio_led_pon, SWP_GPIO_OUTPUT);
			check_gpio_set = 1;
		}
		if(op == 0){
			swp_gpioData(gpio_led_pon, 1);
		} else {
			swp_gpioData(gpio_led_pon, 0);
		}
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
		if(op == 0){
			swp_gpioData(gpio_led_pon, 1);
		} else {
			swp_gpioData(gpio_led_pon, 0);
		}
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
