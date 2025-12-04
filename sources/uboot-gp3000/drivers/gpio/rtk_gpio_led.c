/*
    RTK LED Spec Requirement
    IC: CA77xx(G3HGU), CA8277X(Venus), and xxxx
    Desc: GPIO offset is the same, just baseadress is different.

 */

#include <common.h>
#include <asm/io.h>
#include <asm/gpio.h>


/* GPIO NUM of PWR LED  */
/* 8277AB               */
/** Power LED GPIO NUM */
//#define LED_PWR_G_GPIO 11
static unsigned int LED_PWR_G_GPIO = 11;
//#define LED_PWR_G_GPIO_ACTIVE 1
static unsigned int LED_PWR_G_GPIO_ACTIVE =1;

static unsigned int prev_time = 0;
/* n- ms to turn on/of LED */
#define LED_TIMER_INTERVAL 500  //ms

/**************************
u-boot env can control
1. flash time or always on
2. gpio num and Active High/Low
*****************************/
#define RTK_LED_ENV_CONTROL    1

#define LED_ON 1
#define LED_OFF 0
#define BIT(nr) (1UL << (nr))

/* GPIO Register Map */
#define CA77XX_GPIO_CFG 0x00
#define CA77XX_GPIO_OUT 0x04
#define CA77XX_GPIO_IN 0x08
#define CA77XX_GPIO_LVL 0x0C
#define CA77XX_GPIO_EDGE 0x10
#define CA77XX_GPIO_BOTHEDGE 0x14
#define CA77XX_GPIO_IE 0x18
#define CA77XX_GPIO_INT 0x1C
#define CA77XX_GPIO_STAT 0x20

/* ca77xx_GPIO_CFG BIT */
#define CA77XX_GPIO_CFG_OUT 0
#define CA77XX_GPIO_CFG_IN 1

#define GPIO_MAX_BANK_NUM 5
#define GPIO_BANK_SIZE 32
#define GPIO_BANK_REG_OFFSET 0x24
#define GPIO_BANK_STATUS_OK 1
#ifdef CONFIG_RTK_PWR_BOOT_ON
static unsigned int led_flash_ms = 1;
#else
static unsigned int led_flash_ms = LED_TIMER_INTERVAL;
#endif

static u8 led_has_on = 0;
#define LED_RATIO  100000
static u32 led_check_cnt = 0;

static unsigned int leds_init_flag = 0;

static void rtk_gpio_led_out_value(int gpio_num, u8 value)
{
    unsigned long base_reg;
    u32 reg;
    u32 offset;

    offset = gpio_num % GPIO_BANK_SIZE;
    base_reg =(unsigned long)(
        PER_GPIO0_CFG + (gpio_num / GPIO_BANK_SIZE) * GPIO_BANK_REG_OFFSET);
    reg = readl(base_reg + CA77XX_GPIO_OUT);
    if(value){
        reg |= (value << offset);
    }else{
        reg &= ~(1<<offset);
    }
    writel(reg, base_reg + CA77XX_GPIO_OUT);
}

void
rtk_gpio_led_out(int gpio_num, unsigned char value)
{
	if (value) {
		rtk_gpio_led_out_value(gpio_num, 1);
	} else {
		rtk_gpio_led_out_value(gpio_num, 0);
	}
}


static void rtk_gpio_led_pwr_on(void)
{
    rtk_gpio_led_out_value(LED_PWR_G_GPIO, LED_PWR_G_GPIO_ACTIVE);
}

static void rtk_gpio_leds_cfg(int gpio_num, int dir)
{
    unsigned long base_reg;
    u32 reg;
    u32 offset;

    offset = gpio_num % GPIO_BANK_SIZE;
    base_reg = (unsigned long)PER_GPIO0_CFG;
    base_reg += (gpio_num / GPIO_BANK_SIZE) * GPIO_BANK_REG_OFFSET;
    reg = readl(base_reg + CA77XX_GPIO_CFG);
    if (dir == CA77XX_GPIO_CFG_OUT)
        reg &= ~(1 << offset);
    else
        reg |= 1 << offset;

    writel(reg, base_reg + CA77XX_GPIO_CFG);

	base_reg = (unsigned long)GLOBAL_GPIO_MUX_0;
	base_reg += ((gpio_num / GPIO_BANK_SIZE) * 4);
	reg = readl(base_reg) | (1<<offset);
	writel(reg, base_reg);

}

void rtk_gpio_leds_cfg_mode(int gpio_num, int dir)
{
	rtk_gpio_leds_cfg(gpio_num, dir ? 1 : 0);
}

#ifdef CONFIG_PARALLEL_LED_EXTENSION
static void rtk_spare_reg_set(unsigned int bit){
    unsigned long reg;
    u32 val;

    reg = (unsigned long)PER_SPARE;
    val = readl(reg);
    val |= (1 << bit);
    writel(val, reg);
}

static void rtk_parallel_led_blink_set(unsigned int led, unsigned int ms){
    unsigned long reg;
    u32 val;

    if (led > 15)
       return;

    reg = (unsigned long)GLOBAL_LED_CONTROL;
    val = (ms/16-1)&(0xff);
    writel(val, reg);

    reg = (unsigned long)GLOBAL_LED_CONFIG_0 + led*0x4;
    if (ms > 1)
        val = 0x8020;
    else
        val = 0x200;

	writel(val, reg);
}
#endif /* CONFIG_PARALLEL_LED_EXTENSION */

static void __rtk_gpio_leds_init(void){
    if(LED_PWR_G_GPIO < 256){
        rtk_gpio_leds_cfg(LED_PWR_G_GPIO, CA77XX_GPIO_CFG_OUT);
        leds_init_flag = 1;
    }

#ifdef CONFIG_PARALLEL_LED_EXTENSION
    if(LED_PWR_G_GPIO < 16){
        rtk_spare_reg_set(LED_PWR_G_GPIO);
        rtk_parallel_led_blink_set(LED_PWR_G_GPIO, led_flash_ms);
    }
#endif
}

int rtk_gpio_leds_init(void)
{
    /* pinctrl */
    {
        GLOBAL_PIN_MUX_t pin_mux;
        pin_mux.wrd = CA_REG_READ(GLOBAL_PIN_MUX);
        /* disable CA LED */
        pin_mux.bf.iomux_led_enable = 0;
        CA_REG_WRITE(pin_mux.wrd, GLOBAL_PIN_MUX);
    }

    if (IS_ENABLED(CONFIG_EARLY_PWR_LED_INIT))
        __rtk_gpio_leds_init();

    return 0;
}

/*
    CONFIG_RTK_PWR_BOOT_ON=y
    PWR_LED always is ON, not flash.
 */


/* Must flash every LED_TIMER_INTERVAL */


static void rtk_gpio_led_toggle(int gpio_num)
{
    unsigned long base_reg;
    u32 reg;
    u32 offset;
	
    offset = gpio_num % GPIO_BANK_SIZE;
    base_reg =(unsigned long)
        (PER_GPIO0_CFG + (gpio_num / GPIO_BANK_SIZE) * GPIO_BANK_REG_OFFSET);
    reg = readl(base_reg + CA77XX_GPIO_OUT);
    if (reg & (1 << offset))
        reg &= ~(1 << offset);
    else
        reg |= 1 << offset;

    writel(reg, base_reg + CA77XX_GPIO_OUT);
}

static void rtk_led_act(void)
{
    ulong msecs;
    if (leds_init_flag > 0) {
	if ((led_check_cnt % LED_RATIO) == 0) {
#if 0
printf(" %u\n", get_timer(0) * 1000);
        printf("[%s][%d]rtk_led_act_flag=%d\n", __func__, __LINE__,
               rtk_led_act_flag);
        printf("PER_GPIO0_CFG : 0x%x\n", readl(0xf4329280));
        printf("PER_GPIO0_OUT : 0x%x\n", readl(0xf4329284));
        printf("PER_GPIO0_IN : 0x%x\n", readl(0xf4329288));
        printf("PER_GPIO0_LVL : 0x%x\n", readl(0xf432928c));
        printf("PER_GPIO0_IE : 0x%x\n", readl(0xf4329288));
        printf("PER_GPIO0_STAT : 0x%x\n", readl(0xf43292a0));
#endif
            if ((led_flash_ms == 1) && (led_has_on==0)) {
            rtk_gpio_led_pwr_on();
                led_has_on = 1;
        } else {
                msecs = get_timer(0) * 1000 / CONFIG_SYS_HZ;
            if ((msecs - prev_time) >= led_flash_ms) {
                rtk_gpio_led_toggle(LED_PWR_G_GPIO);
                prev_time = msecs;
            }
        }
    }
        led_check_cnt++;
    }
}

void hw_watchdog_and_led_reset(void)
{
#ifdef CONFIG_HW_WATCHDOG
/* Default, not enable */
    extern void hw_watchdog_reset(void);
    hw_watchdog_reset();
#endif
#if defined(CONFIG_WATCHDOG)
/* Default, not enable */
    extern void watchdog_reset(void);
    watchdog_reset();
#endif

    /*
     * handle led after relocation since global/static variables
     * is unusable before relocation
     */
    if (gd->flags & GD_FLG_RELOC)
        rtk_led_act();
}

static void led_restore_original(void)
{
#ifdef CONFIG_RTK_PWR_BOOT_ON
    led_flash_ms = 1;
#else
    led_flash_ms = LED_TIMER_INTERVAL;
#endif
}

#ifdef RTK_LED_ENV_CONTROL
//#include <environment.h>
#include <env.h>
#include <env_callback.h>
#include <malloc.h>
static int pwr_led_gpio(const char *name, const char *value,
                        enum env_op op, int flags)
{
    unsigned int gpio_pin = 0;
    const char delim[] = ",";
    int active_high = 1;
    char *p = strdup(value);
    char *sstr = NULL;
    char *free_p = p;
    switch (op) {
        case env_op_create:
        case env_op_overwrite:
            sstr = strsep(&p, delim);
            if(sstr && p){
                gpio_pin = simple_strtoul(value, NULL, 10);
                //printf("[%d]gpio_pin=%d, active_high=%d\n", __LINE__, gpio_pin, active_high);
                if(gpio_pin >128){
                    goto exit;
                }
                if(strncmp("GPIO_ACTIVE_LOW", p, 16) == 0){
                    //LED_PWR_G_GPIO_ACTIVE = 0;
                    active_high = 0;
                }else{
                    //LED_PWR_G_GPIO_ACTIVE = 1;
                    active_high = 1;
                }

                if (leds_init_flag == 1) {
                    if(LED_PWR_G_GPIO != gpio_pin){
                        rtk_gpio_leds_cfg(LED_PWR_G_GPIO, CA77XX_GPIO_CFG_IN);
                    }
                    //printf("[%d]gpio_pin=%d, active_high=%d\n", __LINE__, gpio_pin, active_high);
                    if((LED_PWR_G_GPIO != gpio_pin) || (active_high != LED_PWR_G_GPIO_ACTIVE)){
                        LED_PWR_G_GPIO = gpio_pin;
                        LED_PWR_G_GPIO_ACTIVE = active_high;
                        __rtk_gpio_leds_init();
                    }
                } else {
                    LED_PWR_G_GPIO = gpio_pin;
                    LED_PWR_G_GPIO_ACTIVE = active_high;
                    __rtk_gpio_leds_init();
                }
            }

            break;
        case env_op_delete:
        default:
            led_restore_original();
            break;
    }
exit:
    if(free_p)
        free(free_p);

    return 0;
}
static int pwr_led_flash(const char *name, const char *value,
                        enum env_op op, int flags)
{
    unsigned int tmp = 0;

    switch (op) {
        case env_op_create:
        case env_op_overwrite:
            tmp = simple_strtoul(value, NULL, 10);
            //printf("[%s %d]%u\n", __func__, __LINE__, tmp);
                if (tmp > 0) {
                    led_has_on = 0;
                    led_flash_ms = tmp;
                } else {
                    led_restore_original();
                }

            break;
        case env_op_delete:
        default:
            led_restore_original();
            break;
    }

#ifdef CONFIG_PARALLEL_LED_EXTENSION
    rtk_parallel_led_blink_set(LED_PWR_G_GPIO, led_flash_ms);
#endif

    return 0;
}

U_BOOT_ENV_CALLBACK(pwr_led_flash, pwr_led_flash);
U_BOOT_ENV_CALLBACK(pwr_led_gpio, pwr_led_gpio);
#endif
