#include <common.h>
#include <command.h>
#include <asm/types.h>
#include <asm/io.h>
#include <config.h>
#include <dm.h>
#include <asm-generic/gpio.h>
#include <linux/mii.h>
#include <linux/list.h>
#include <i2c.h>
#include <miiphy.h>
#include <net.h>
#include <sound.h>
#include "ft2_cfg.h"
#include "ft2.h"

#if defined(CONFIG_TARGET_SATURN_ASIC)
	#if defined(CONFIG_SATURN2_SOC)
	#include "ca8271n_profile.c"
	#else
	#include "ca8271a_profile.c"
	#endif
#elif defined(CONFIG_TARGET_VENUS)
	#include "ca8277b_profile.c"
#else
	#include <pci.h>
	#if defined(CONFIG_CA8279_FT2)
	#include "ca8279_profile.c"
	#else
	#include "ca7774_profile.c"
	#endif
#endif

#if defined(CONFIG_TARGET_SATURN_FPGA) || defined(CONFIG_TARGET_SATURN_ASIC)
#define CORTINA_CA_REG_READ(off)		CA_REG_READ(KSEG1_ATU_XLAT(off))
#define CORTINA_CA_REG_WRITE(data, off)         CA_REG_WRITE(data, KSEG1_ATU_XLAT(off))
#else
#define CORTINA_CA_REG_READ(off)                CA_REG_READ(off)
#define CORTINA_CA_REG_WRITE(data, off)         CA_REG_WRITE(data, off)
#endif

#define CA_OTP_REG_READ(value, addr) {\
	OTP_STATUS_t otp_stat;\
	int16_t timeout = 400;\
	do {\
		otp_stat.wrd = CORTINA_CA_REG_READ(OTP_STATUS);\
		if (otp_stat.bf.RDOPEN_STATE)\
			break;\
		udelay(500);\
	} while(timeout-- > 0);\
	value = readb(addr);\
} while(0)

#define CA_OTP_REG_WRITE(value, addr) {\
	OTP_STATUS_t otp_stat;\
	int16_t timeout = 400;\
	do {\
		otp_stat.wrd = CORTINA_CA_REG_READ(OTP_STATUS);\
		if (otp_stat.bf.WROPEN_STATE)\
			break;\
		udelay(500);\
	} while(timeout-- > 0);\
	writeb(value, addr);\
} while(0)

#ifdef CONFIG_CA_EFUSE
#define FT2_CFG_ACCESS	ca_ft2_efuse_config
extern int cortina_efuse_access(int group, uint32_t * val, int op);
#else
#define FT2_CFG_ACCESS	ca_ft2_otp_config
#endif

//#define gpio_direction_output ca_gpio_direction_output

/* ADS1015 registers */
enum {
	ADS1015_CONVERSION = 0,
	ADS1015_CONFIG = 1,
};

struct cortina_efuse_context {
	void *base;
	unsigned int nregs;
	bool redundant;
};

typedef enum {
	SERDES_MOD_MIN = 1,
	/* EPON RX 10.3125G/TX 10.3125G; XFI0 10.3125G; XFI1 10.3125G */
	SERDES_MOD_1 = SERDES_MOD_MIN,
	/* GPON RX 9.98325G/TX 9.98325G; XFI0 3.125G; XFI1 3.125G */
	SERDES_MOD_2,
	/* For G3HGU, INTRA XFI1 10.3125G */
	SERDES_MOD_INTRA_XFI,
	SERDES_MOD_MAX
} serdes_test_mode_t;

#define ADS1015_DEFAULT_PGA 2
#define ADS1015_DEFAULT_DATA_RATE 4

unsigned long ft2_start;
unsigned long ft2_finish;
static uint8_t ft2_dbg = 0;
static ft2_record_t ft2_records[FT2_PARAM_MAX];

extern ca_status_t ni_eth_port_rx_mib_get(
	ca_uint32_t port_id,
	ca_boolean_t read_clear,
	ca_uint32_t stats[__NI_MIB_RX_NUM]
);
extern ca_status_t ni_eth_port_tx_mib_get(
	ca_uint32_t port_id,
	ca_boolean_t read_clear,
	ca_uint32_t stats[__NI_MIB_RX_NUM]
);

/*******************************************************************************
 * GPHY Calibration
 ******************************************************************************/
static uint16_t phy_k_result[PHY_K_PORT_NUM][PHY_K_MAX];
static int16_t phy_k_mdi[PHY_K_PORT_NUM][PHY_CHANNEL_NUM*2];
static int16_t phy_k_amp[PHY_K_PORT_NUM][PHY_CHANNEL_NUM*2];

//#define FT2_DBG(fmt, args...) if (ft2_dbg & 0x1) printf(fmt, ##args)
#define FT2_DBG(fmt, args...) printf(fmt, ##args)
//#define FT2_MOD(fmt, args...) if (ft2_dbg & 0x2) printf(fmt, ##args)
#define FT2_MOD(fmt, args...) printf(fmt, ##args)

static char *ft2_module_name[] = {
	[FT2_PARAM_GPHY_K] = "GPHY",
	[FT2_PARAM_RC_K_DIFF] = "",
	[FT2_PARAM_R_K_DIFF] = "",
	[FT2_PARAM_AMP_AID] = "",
	[FT2_PARAM_GPHY_SNR] = "GPHY SNR",
	[FT2_PARAM_XFI_PORT_LOOPBACK] = "XFI",
	[FT2_PARAM_GMII_PORT_LOOPBACK] = "GMII",
	[FT2_PARAM_GMII_PORT_OPEN_LOOPBACK] = "GMII",
	[FT2_PARAM_RGMII_PORT_LOOPBACK] = "RGMII",
	[FT2_PARAM_SERDES] = "SERDES",
	[FT2_PARAM_PON_BEN] = "PON BEN",
	[FT2_PARAM_PON_SRAM] = "PON SRAM",
	[FT2_PARAM_INTRA_XFI] = "INTRA XFI",
	[FT2_PARAM_CPU] = "CPU",
	[FT2_PARAM_PCIE] = "PCIE",
	[FT2_PARAM_USB] = "USB",
	[FT2_PARAM_SATA] = "SATA",
	[FT2_PARAM_RTC] = "RTC",
	[FT2_PARAM_OTP] = "OTP",
	[FT2_PARAM_TAROKO] = "TAROKO",
	[FT2_PARAM_GPIO] = "GPIO",
	[FT2_PARAM_LDMA] = "LDMA",
	[FT2_PARAM_DDR_ZQ] = "DDR ZQ",
	[FT2_PARAM_UUID] = "UUID",
	[FT2_PARAM_NAND] = "NAND",
	[FT2_PARAM_CFG_W] = "CFG",
	[FT2_PARAM_SSP] = "SSP",
	[FT2_PARAM_DYINGASP] = "DYINGASP",
	[FT2_PARAM_PON_BEN] = "PON BEN",
	[FT2_PARAM_DBG] = "",
//	FT2_PARAM_MAX
};

ft2_ret_t ca_ft2_gpio_mux_set(uint32_t gpio_num)
{
	uint32_t gpio_group, gpio_pin;
	GLOBAL_GPIO_MUX_0_t gpio_mux_0;
	uint32_t addr;

	gpio_group = gpio_num / 32;
	gpio_pin = gpio_num % 32;
	addr = GLOBAL_GPIO_MUX_0 + gpio_group*4;
	printf("%s: gpio_mux_addr = 0x%08x, gpio_group = %d, gpio_pin = %d\n", __func__, addr, gpio_group, gpio_pin);
	gpio_mux_0.wrd = CORTINA_CA_REG_READ(addr);
	gpio_mux_0.wrd |= (0x1 << gpio_pin);
	CORTINA_CA_REG_WRITE(gpio_mux_0.wrd, addr);

	return FT2_OK;
}

void ca_ft2_active_gphy_network(void)
{
	GLOBAL_PIN_MUX_t g_pin_mux;

#if defined(CONFIG_TARGET_PRESIDIO_ASIC)
	#if defined(CONFIG_CA8279_FT2)
	CORTINA_CA_REG_WRITE(0xffffdbff, PER_GPIO0_CFG);
	CORTINA_CA_REG_WRITE(0x2028, PER_GPIO0_OUT);
	CORTINA_CA_REG_WRITE(0x80088002, GLOBAL_PIN_MUX);
	#else
	/* set gpio13 output 1, and gpio10 output 0 */
	g_pin_mux.wrd = CORTINA_CA_REG_READ(GLOBAL_PIN_MUX);
	g_pin_mux.bf.iomux_sd_mmc_resetn  = 0;
	CORTINA_CA_REG_WRITE(g_pin_mux.wrd, GLOBAL_PIN_MUX);
	ca_ft2_gpio_mux_set(GPIO_LAN_SWITCH_SEL);
	gpio_direction_output(GPIO_LAN_SWITCH_SEL, 1);
	ca_ft2_gpio_mux_set(GPIO_LAN_SWITCH_LP);
	gpio_direction_output(GPIO_LAN_SWITCH_LP, 0);
	#endif
#elif defined(CONFIG_TARGET_SATURN_ASIC)
	CORTINA_CA_REG_WRITE(0xfffffddf, PER_GPIO0_CFG);
	CORTINA_CA_REG_WRITE(0x200, PER_GPIO0_OUT);
	CORTINA_CA_REG_WRITE(0x3000004, GLOBAL_PIN_MUX);
	CORTINA_CA_REG_WRITE(0x220, GLOBAL_GPIO_MUX_0);
#elif defined(CONFIG_TARGET_VENUS)
	/* set gpio3 output 1, and gpio10 output 0 */
	g_pin_mux.wrd = CORTINA_CA_REG_READ(GLOBAL_PIN_MUX);
	g_pin_mux.bf.iomux_uart_3_enable = 0;
	g_pin_mux.bf.iomux_sd_mmc_resetn  = 0;
	CORTINA_CA_REG_WRITE(g_pin_mux.wrd, GLOBAL_PIN_MUX);
	ca_ft2_gpio_mux_set(GPIO_LAN_SWITCH_SEL);
	gpio_direction_output(GPIO_LAN_SWITCH_SEL, 1);
	ca_ft2_gpio_mux_set(GPIO_LAN_SWITCH_LP);
	gpio_direction_output(GPIO_LAN_SWITCH_LP, 0);
#endif
	return;
}

static ft2_ret_t ca_ft2_lan_switch(
	lan_switch_t func)
{
	int rc = 0;

	ca_ft2_gpio_mux_set(GPIO_LAN_SWITCH_SEL);
	ca_ft2_gpio_mux_set(GPIO_LAN_SWITCH_LP);

	switch (func) {
	case LAN_SWITCH_BUS0:
		rc |= gpio_direction_output(GPIO_LAN_SWITCH_LP, 0);
		rc |= gpio_direction_output(GPIO_LAN_SWITCH_SEL, 0);
		break;
	case LAN_SWITCH_BUS1:
		rc |= gpio_direction_output(GPIO_LAN_SWITCH_LP, 0);
		rc |= gpio_direction_output(GPIO_LAN_SWITCH_SEL, 1);
		break;
	case LAN_SWITCH_OPEN:
		rc |= gpio_direction_output(GPIO_LAN_SWITCH_LP, 1);
		rc |= gpio_direction_output(GPIO_LAN_SWITCH_SEL, 0);
		break;
	}

	return rc ? FT2_GPIO : FT2_OK;
}

static ft2_ret_t ca_ft2_phy_regfield_read(
	uint8_t   phy_dev,
	uint8_t   reg,
	uint8_t   offset,
	uint8_t   length,
	uint16_t *p_val16)
{
	ca_uint16_t data = 0;
	const char	*devname;

	if (length == 0 || offset >= PHY_REG_MAX_LEN ||
	    length > (PHY_REG_MAX_LEN - offset) || !p_val16) {
		printf("%s: Input parameters error\n", __func__);
		return FT2_ERROR;
	}

	devname = miiphy_get_current_dev();
	
	if (miiphy_read(devname, phy_dev, reg, &data)) {
		printf("Error reading from the PHY addr=%02x reg=%02x\n", phy_dev, reg);
		return FT2_MDIO;
	}

//	printf("ca_ft2_phy_regfield_read: PHY addr=0x%x reg=0x%x, data = 0x%x\n", phy_dev, reg, data);

	if ((0 == offset) && (16 == length)) {
		*p_val16 = data & 0xffff;
	} else {
		*p_val16 = (data >> offset) & ((0x1 << length) -1);
	}

	return FT2_OK;
}

static ft2_ret_t ca_ft2_phy_regfield_write(
	uint8_t   phy_dev,
	uint8_t   reg,
	uint8_t   offset,
	uint8_t   length,
	uint16_t  val16)
{
	ca_uint16_t data = 0;
	ca_uint16_t data16 = 0;
	const char	*devname;

	if (length == 0 || offset >= PHY_REG_MAX_LEN ||
	    length > (PHY_REG_MAX_LEN - offset)){
		printf("%s: Input parameters error\n", __func__);
		return FT2_ERROR;
	}
	
	devname = miiphy_get_current_dev();
	
	if (miiphy_read(devname, phy_dev, reg, &data)) {
		printf("Error reading from the PHY addr=%02x reg=%02x\n", phy_dev, reg);
		return FT2_MDIO;
	}

	data16 = (data & ~(((0x1 << length) -1) << offset)) | (val16 << offset);

//	printf("ca_ft2_phy_regfield_write: PHY addr=0x%x reg=0x%x, data = 0x%x, data16 = 0x%x\n", phy_dev, reg, data, data16);

	if ((data & 0xffff) == data16) {
		return FT2_OK;
	}
	
	if (miiphy_write(devname, phy_dev, reg, data16)) {
		printf("Error writing to the PHY addr=%02x reg=%02x\n", phy_dev, reg);
		return FT2_MDIO;
	}

	return FT2_OK;
}

static int sysfs_hwmon_get(
	uint8_t dev,
	uint8_t chan,
	int *val)
{
#define SWAPb16(x) (x >> 8 | (x & 0xff) << 8)

	uint16_t buf, config, vol, neg=0;
	int ret;

	/* get channel parameters */
	ret = i2c_read(dev, ADS1015_CONFIG, 1, (uint8_t *)&buf, 2);
	if (ret) {
		printf("Error reading the dev=0x%x, reg=0x%x: %d\n", dev, ADS1015_CONFIG, ret);
		return FT2_I2C;
	}
	
	config = SWAPb16(buf);

	/* setup and start single conversion */
	config &= 0x001f;
	config |= (1 << 15) | (1 << 8);
	config |= (chan & 0x0007) << 12;
	config |= (0x1/*ADS1015_DEFAULT_PGA*/ & 0x0007) << 9;
	config |= (0x3/*ADS1015_DEFAULT_DATA_RATE*/ & 0x0007) << 5;

	buf = SWAPb16(config);

	//printf("%d: buf = %04x/%04x\n", __LINE__, buf, config);
	ret = i2c_write(dev, ADS1015_CONFIG, 1, (uint8_t *)&buf, 2);
	if (ret) {
		printf("Error writing the dev=0x%x, reg=0x%x, value=0x%04x: %d\n", dev, ADS1015_CONFIG, buf, ret);
		return FT2_I2C;
	}

	/* wait until conversion finished */
	udelay(2000);
	ret = i2c_read(dev, ADS1015_CONFIG, 1, (uint8_t *)&buf, 2);
	if (ret) {
		printf("Error reading the dev=0x%x, reg=0x%x: %d\n", dev, ADS1015_CONFIG, ret);
		return FT2_I2C;
	}

	config = SWAPb16(buf);
	//printf("%d: config = %04x/%04x\n", __LINE__, config, buf);
	if (!(config & (1 << 15))) {
		/* conversion not finished in time */
		printf("Conversion not finished in time");
		return FT2_I2C;
	}

	ret = i2c_read(dev, ADS1015_CONVERSION, 1, (uint8_t *)&buf, 2);
	if (ret) {
		printf("Error reading the dev=0x%x: %d\n", dev, ret);
		return FT2_I2C;
	}
	vol = SWAPb16(buf);
	//printf("%d: vol = %04x/%04x\n", __LINE__, vol, buf);
	vol = (vol >> 4);
	if (vol & 0x800) {
		vol = ((~vol) + 1) & 0xfff;
		neg = 1;
	}
	vol *= 2; 
	*val = (neg) ? -vol : vol;
	//printf("%d: val = 0x%x/%d\n", __LINE__, *val, *val);

	return FT2_OK;
}

static ft2_ret_t ca_ft2_voltage_get(
	uint8_t port,
	phy_channel_t channel_idx,
	int16_t *voltage)
{
	int val;
	uint8_t buf;
	int ret;

	/* change to the correct i2c mux channel */
	buf = 0x4 | ads_layout[port][channel_idx][I2C_MUX_CHANNEL];
	ret = i2c_write(I2C_MUX_ADDR, 0, 0, &buf, 1);
	if (ret) {
		printf("I2C MUX change channel fail, dev=0x%x, value=0x%02x: %d\n", I2C_MUX_ADDR, buf, ret);
		return FT2_I2C;
	}

	if (sysfs_hwmon_get(ads_layout[port][channel_idx][HWMON_ENTRY],
			    ads_layout[port][channel_idx][INPUT_MUX_ENTRY],
			    &val))
			    return FT2_I2C;

	if (ads_layout[port][channel_idx][REVERT_ENTRY])
		val = -val;

	*voltage = val;

	return FT2_OK;
}

static ft2_ret_t ca_ft2_phy_ado_current(
	uint8_t port,
	phy_ado_current_t ado_current)
{
	uint8_t phy_dev;

	/* get phy addr */
	phy_dev = phy_addr[port];

	switch (ado_current) {
		case PHY_ADO_CURR_LOW:
			ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8011);
			ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x7577);

			/* rg_ana_ctl.xls - select extension page */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcc);
			ca_ft2_phy_regfield_write(phy_dev, 23, 0, 16, 0x0f0);

			/* rg_pcs.xls - select extension page */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcd);
			ca_ft2_phy_regfield_write(phy_dev, 19, 0, 16, 0xb0e0);
			/* back to page 0 */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
			break;
		case PHY_ADO_CURR_MED:
			ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8011);
			ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x7577);

			/* rg_ana_ctl.xls - select extension page */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcc);
			ca_ft2_phy_regfield_write(phy_dev, 23, 0, 16, 0x0e0);

			/* rg_pcs.xls - select extension page */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcc);
			ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, 0xa408);

			/* rg_pcs.xls - select extension page */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bca);
			ca_ft2_phy_regfield_write(phy_dev, 23, 0, 16, 0x4000);

			/* rg_pcs.xls - select extension page */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcd);
			ca_ft2_phy_regfield_write(phy_dev, 19, 0, 16, 0xb0e1);
			/* back to page 0 */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
			break;
		case PHY_ADO_CURR_HIGH:
			ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8011);
			ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x7577);

			/* rg_ana_ctl.xls - select extension page */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcc);
			ca_ft2_phy_regfield_write(phy_dev, 23, 0, 16, 0x0e0);

			/* rg_pcs.xls - select extension page */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcc);
			ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, 0xa408);

			/* rg_pcs.xls - select extension page */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bca);
			ca_ft2_phy_regfield_write(phy_dev, 23, 0, 16, 0x4000);

			/* rg_pcs.xls - select extension page */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcd);
			ca_ft2_phy_regfield_write(phy_dev, 19, 0, 16, 0xb0e7);
			/* back to page 0 */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
			break;
		default:
			ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8011);
			ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x7577);

			/* rg_ana_ctl.xls - select extension page */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcc);
			ca_ft2_phy_regfield_write(phy_dev, 23, 0, 16, 0x0e0);

			/* rg_pcs.xls - select extension page */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcd);
			ca_ft2_phy_regfield_write(phy_dev, 19, 0, 16, 0xb0e3);
			/* back to page 0 */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
	}

	return FT2_OK;
}

#if defined(CONFIG_TARGET_SATURN_ASIC)
static ft2_ret_t ca_ft2_phy_calibration_prepare(
	uint8_t port,
	phy_k_item_t item)
{
	uint8_t phy_dev;

	/* get phy addr */
	phy_dev = phy_addr[port];

	if (item == PHY_K_R_TAPBIN_TX|| item == PHY_K_R_TAPBINRX_PM) {
		/* ROM code R_cal remapping */
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8121);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0xcbd4);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8123);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0xdce4);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8125);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0xebf2);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8127);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0xf8fe);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8129);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x0308);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x812b);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x0d12);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x812d);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x171c);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x812f);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x2126);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8131);
		ca_ft2_phy_regfield_write(phy_dev, 28, 8, 8, 0x2b);

		/* back to page 0 */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
	}

	return FT2_OK;
}
#elif defined(CONFIG_TARGET_VENUS)
static ft2_ret_t ca_ft2_phy_calibration_prepare(
	uint8_t port,
	phy_k_item_t item)
{
	uint8_t phy_dev;
	uint16_t value;
	int timeout = 100;

	/* get phy addr */
	phy_dev = phy_addr[port];

	if (item == PHY_K_RC_LEN || item == PHY_K_RC_RLEN) {
		/* port 0 set patch request/lock flow */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0a42);

		/* wait page 0x0a42 reg.16[1:0] = 2'b11 */
		while (timeout--) {
			ca_ft2_phy_regfield_read(phy_dev, 16, 0, 16, &value);
			if ((value & 0x3) == 3) {
				break;
			}
			mdelay(1);
		}
		if (timeout <= 0) {
			printf("%s: port=%d, phy_dev=%d, page=0xa42, reg=16, value=%d, expect 0x3, Timeout!!\n", __func__, port, phy_dev, value);
		}

		/* set patch request (0xb820[4] = 1) */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0b82);
		ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, 0x0010);

		/* polling patch rdy = 1 (0xb800[6] = 1)  */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0b80);
		timeout = 100;
		while (timeout--) {
			ca_ft2_phy_regfield_read(phy_dev, 16, 0, 16, &value);
			if ((value & 0x40) == 0x40) {
				break;
			}
			mdelay(1);
		}
		if (timeout <= 0) {
			printf("%s: port=%d, phy_dev=%d, page=0xb80, reg=16, value=%d, expect 0x40, Timeout!!\n", __func__, port, phy_dev, value);
		}

		/* set patch_key to SRAM (W 27 0x8028, W 28 0x6200) */
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8028);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x6200);

		/* set patch_lock = 1 */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0b82);
		ca_ft2_phy_regfield_write(phy_dev, 23, 0, 16, 0x0001);

		/* indirect access sram patch data */
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x83ed);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0xaf83);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x83ef);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0xf9af);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x83f1);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x8402);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x83f3);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0xaf84);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x83f5);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x02af);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x83f7);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x8402);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x83f9);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0xe082);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x83fb);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x5ee1);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x83fd);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x825f);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x83ff);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0xaf35);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8401);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x6b00);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0xb818);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x3565);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0xb81a);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x0000);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0xb81c);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x0000);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0xb81e);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x0000);
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0xb832);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x0001);

		/* port 0 set patch clear/release flow */
		/* clear patch key (W 27 0x8028, W 28 0x0000) */
		ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8028);
		ca_ft2_phy_regfield_write(phy_dev, 28, 0, 16, 0x0000);

		/* clear patch lock (W 31 0xb82e, W 23 0x0000) */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0b82);
		ca_ft2_phy_regfield_write(phy_dev, 23, 0, 16, 0x0000);

		/* release patch request (0xb820[4] = 0)   */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0b82);
		ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, 0x0000);

		/* polling patch rdy = 0 (0xb800[6] = 0)  */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0b80);
		timeout = 100;
		while (timeout--) {
			ca_ft2_phy_regfield_read(phy_dev, 16, 0, 16, &value);
			if ((value & 0x40) == 0x00) {
				break;
			}
			mdelay(1);
		}
		if (timeout <= 0) {
			printf("%s: port=%d, phy_dev=%d, page=0xb80, reg=16, value=%d, expect 0x0, Timeout!!\n", __func__, port, phy_dev, value);
		}
	}

	/* uc_sram_table_RL6386.xls - disable spdChg : 0x8011 bit[1], -->data16 bit[9] */
	ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8011);
	/* 0 = disable spdChg */
	ca_ft2_phy_regfield_write(phy_dev, 28, 9, 1, 0x0);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/* rg_ana_ctl.xls - select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcc);
	/* modify gdac_ib_dn_up */
	if (item == PHY_K_R_TAPBINRX_PM)
		ca_ft2_phy_regfield_write(phy_dev, 18, 0, 16, 0x0000);
	else
		ca_ft2_phy_regfield_write(phy_dev, 18, 0, 16, 0x00AA);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/* rg_pcs.xls - select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0a43);
	/* disable ALDPS */
	ca_ft2_phy_regfield_write(phy_dev, 24, 2, 1, 0x0);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/* change ado current */
	if (ado_curr == PHY_ADO_CURR_LOW) {
		if (item == PHY_K_RC_LEN || item == PHY_K_RC_RLEN ||
			item == PHY_K_R_TAPBIN_TX || item == PHY_K_R_TAPBINRX_PM)
			/* change ado current to normal state */
			ca_ft2_phy_ado_current(port, PHY_ADO_CURR_NORM);
		else
			/* change ado current to low state */
			ca_ft2_phy_ado_current(port, PHY_ADO_CURR_LOW);
	} else {
		/* change ado current state */
		ca_ft2_phy_ado_current(port, ado_curr);
	}

	return FT2_OK;
}
#else
static ft2_ret_t ca_ft2_phy_calibration_prepare(
	uint8_t port,
	phy_k_item_t item)
{
	uint8_t phy_dev;

	/* get phy addr */
	phy_dev = phy_addr[port];

	/* uc_sram_table_RL6386.xls - disable spdChg : 0x8011 bit[1], -->data16 bit[9] */
	ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8011);
	/* 0 = disable spdChg */
	ca_ft2_phy_regfield_write(phy_dev, 28, 9, 1, 0x0);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/* rg_ana_ctl.xls - select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcc);
	/* modify gdac_ib_dn_up */
	if (item == PHY_K_R_TAPBINRX_PM)
		ca_ft2_phy_regfield_write(phy_dev, 18, 0, 16, 0x0000);
	else
		ca_ft2_phy_regfield_write(phy_dev, 18, 0, 16, 0x00FF);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/* rg_pcs.xls - select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0a43);
	/* disable ALDPS */
	ca_ft2_phy_regfield_write(phy_dev, 24, 2, 1, 0x0);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	return FT2_OK;
}
#endif

static ft2_ret_t ca_ft2_phy_k_result_set(
	uint8_t port,
	phy_k_item_t item,
	uint16_t value)
{
	if (port >= PHY_K_PORT_NUM || item >= PHY_K_MAX) {
		printf("%s: parameter error\n", __func__);
		return FT2_ERROR;
	}

	phy_k_result[port][item] = value;
	return FT2_OK;
}

static int is_ch_outputV_in_range_ado(unsigned int* checkV, int *tap, unsigned short min, unsigned short max, int *sign)
{
	int i, ret = 1;

	if (tap) {
		for (i = 0; i < 4; i++) {
			/*reset tap*/
			tap[i] = 0;

			if (checkV[i] >= min && checkV[i] <= max) {
				continue;
			}
			else if (checkV[i] > max) {
				if(sign[i])
					tap[i] = -1;
				else
					tap[i] = 1;
				FT2_DBG("BB: %s:%d, output V of ch[%d] is %d, > %d mv tap=%d sign=%d\n", __func__,__LINE__, i, checkV[i], max,tap[i],sign[i]);
				ret = 0;
			}
/*
			else if (checkV[i] < min) {
				dbg_printk("BB: %s:%d, output V of ch[%d] is %d, < %d mv\n", __func__,__LINE__, i, checkV[i], min);
				tap[i] = 1;
				ret = 0;
			}
*/
		}
	}
	else {
		for (i = 0; i < 4; i++) {

			if (checkV[i] >= min && checkV[i] <= max) {
				continue;
			}
			else if (checkV[i] > max) {
				FT2_DBG("EE: %s:%d, output V of ch[%d] is %d, > %d mv\n", __func__,__LINE__, i, checkV[i], max);
				if(sign[i])
					tap[i] = -1;
				else
					tap[i] = 1;
				ret = 0;
			}
/*
			else if (checkV[i] < min) {
				printk("EE: %s:%d, output V of ch[%d] is %d, < %d mv\n", __func__,__LINE__, i, checkV[i], min);
				ret = 0;
			}
*/
		}
	}

	return ret;
}

#if defined(CONFIG_TARGET_VENUS)
static ft2_ret_t ca_ft2_phy_ado_verify(uint8_t port, unsigned int* avg_volt, int *avg_sign)
{
	uint8_t phy_dev;
	uint32_t cal_times, diff, cnt[PHY_CHANNEL_MAX] = {0};
	int i, sign = 0;
	uint16_t value;
	ft2_ret_t ret = FT2_OK;

	/* get phy addr */
	phy_dev = phy_addr[port];

	/*get ado_times of ado V*/
	for (cal_times = 1; cal_times <= ado_verify_times; cal_times++) {
		FT2_DBG("BB: %s-%d, Times#%d\n", __func__,__LINE__, cal_times);
		for (i = 0; i < PHY_CHANNEL_MAX; i++) {
			uint32_t voltmp = 0;
			/* voltmp = read_adoV_of_CH_X(i); */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, (0xa8c + (i * 0x10)));
			ca_ft2_phy_regfield_read(phy_dev, 18, 0, 16, &value);
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
			voltmp = value;
			if (value & 0x100) {
				/* negative value */
				voltmp = 0x200 - value;
				sign = 0;
			} else {
				/* postive value */
				sign = 1;
			}
			FT2_DBG("BB: %s-%d, ado V of ch[%d] = %d mv(0x%04x/0x%04x) sign=%d\n", __func__, __LINE__, i, voltmp, voltmp, value, sign);
			diff = (sign^avg_sign[i]) ? (voltmp + avg_volt[i]) : (abs(voltmp -avg_volt[i]));
			if (diff > ado_volt_diff) {
				cnt[i]++;
				FT2_DBG("BB: %s-%d, ado V of ch[%d] diff = %d\n", __func__,__LINE__, i, diff);
			}
		}
	}

	for (i = 0; i < PHY_CHANNEL_MAX; i++) {
		if (cnt[i] <= ado_over_times)
			continue;
		FT2_DBG("EE: %s:%d, ado V of ch[%d] is %d times over the range\n", __func__,__LINE__, i, cnt[i]);
		ret = FT2_ERROR;
	}

	return ret;
}
#endif

static ft2_ret_t ca_ft2_phy_ado_calibration(uint8_t port)
{
	uint8_t phy_dev;
	uint16_t value;

	uint32_t cal_times = 0, adj_cnt = PHY_ADJ_TIMEOUT;
	uint32_t adoV[4] = {0}, avgV[4] = {0};
	uint16_t adoadj = 0, new_adoadj = 0;
	int adjTap[4] = {0};
	int i, ret = FT2_OK;
	int sign[4]={0};

	printf("II: GPHY Port %d ADO calibration\n", port);

	ca_ft2_lan_switch(LAN_SWITCH_OPEN);

	/* get phy addr */
	phy_dev = phy_addr[port];

	ca_ft2_phy_calibration_prepare(port, PHY_K_ADO);

	/* data = Giga_Eth_ReadPhyMdio(1, 0xa46, 21) | (1 << 1);
	   Giga_Eth_SetPhyMdio(1, 0xa46, 21, data); */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa46);
	ca_ft2_phy_regfield_write(phy_dev, 21, 1, 1, 1);
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/* check_cnt = GPHY_CAL_TIMEOUT;
	while((Giga_Eth_ReadPhyMdio(1, 0xa60, 16) & 0xff) != 0x1) {
		check_cnt--;
		if (check_cnt == 0) {
			printk("EE: %s check PCS state timeout (0x%04x)!\n", __func__, Giga_Eth_ReadPhyMdio(1, 0xa60, 16));
		}
	} */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa60);
	i = PHY_READY_TIMEOUT;
	do {
		ca_ft2_phy_regfield_read(phy_dev, 16, 0, 8, &value);
		if (value == 0x1)
			break;
	} while ((--i) > 0);

	if (i == 0) {
		printf("%s: EE check PCS state timeout (0x%04x)!\n", __func__, value);
		ca_ft2_lan_switch(LAN_SWITCH_BUS1);
		return FT2_TIMEOUT;
	}
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/* Giga_Eth_SetPhyMdio(1, 0xa43, 19, 0x8010); */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa43);
	ca_ft2_phy_regfield_write(phy_dev, 19, 0, 16, 0x8010);
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/*
	//page 0xa43 reg 20 bit[11] = 0
	data = Giga_Eth_ReadPhyMdio(1, 0xa43, 20) & ~(1 << 11);
	Giga_Eth_SetPhyMdio(1, 0xa43, 20, data); */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa43);
	ca_ft2_phy_regfield_write(phy_dev, 20, 11, 1, 0);
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/* Giga_Eth_SetPhyMdio(1, 0xa40, 16, 0x0040); */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa40);
	ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, 0x0040);
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/*
	//page 0xa4a reg 19 bit[7:6] = 2'b10
	data = (Giga_Eth_ReadPhyMdio(1, 0xa4a, 19) | (1 << 7)) & ~(1 << 6);
	Giga_Eth_SetPhyMdio(1, 0xa4a, 19, data); */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa4a);
	ca_ft2_phy_regfield_write(phy_dev, 19, 6, 2, 0x2);
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/*
	//page 0xa44 reg 20 bit[3:2] = 2'b01
	data = (Giga_Eth_ReadPhyMdio(1, 0xa44, 20) | (1 << 2)) & ~(1 << 3);
	Giga_Eth_SetPhyMdio(1, 0xa44, 20, data); */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa44);
	ca_ft2_phy_regfield_write(phy_dev, 20, 2, 2, 0x1);
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/*
	//page 0xa46 reg 21 bit[1] = 0
	data = Giga_Eth_ReadPhyMdio(1, 0xa46, 21) & ~(1 << 1);
	Giga_Eth_SetPhyMdio(1, 0xa46, 21, data); */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa46);
	ca_ft2_phy_regfield_write(phy_dev, 21, 1, 1, 0);
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/*
	//page 0xbc0 reg 20 bit[2] = 1
	data = Giga_Eth_ReadPhyMdio(1, 0xbc0, 20) | (1 << 2);
	Giga_Eth_SetPhyMdio(1, 0xbc0, 20, data); */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xbc0);
	ca_ft2_phy_regfield_write(phy_dev, 20, 2, 1, 1);
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
	do {
		/* Read adoadj */
		udelay(10000);

		/* adoadj = Giga_Eth_ReadPhyMdio(1, 0xbcf, 22); */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xbcf);
		ca_ft2_phy_regfield_read(phy_dev, 22, 0, 16, &adoadj);
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
		new_adoadj = adoadj;

		FT2_DBG("BB: %s-%d, current adoadj = 0x%04x\n", __func__, __LINE__, adoadj);

		/*check if we need to adjust adoadj*/
		for (i = 0; i < 4; i++) {
			FT2_DBG("BB: %s-%d, adjTap[%d]=%d\n", __func__, __LINE__, i, adjTap[i]);

			if (adjTap[i]) {
				if (((adoadj & (0xf << i*4)) == 2) && adjTap[i] < 0) {
					FT2_DBG("BB: %s-%d, adoadj of ch[%d] already reaches it's lower bound, 0x%x\n", __func__,__LINE__, i, ((adoadj & (0xf << i*4)) >> (i*4)));
					continue; //adoadj already reaches it's lower bound
				}
				else if (((adoadj & (0xf << i*4))) == (0xc << i*4) && adjTap[i] > 0) {
					FT2_DBG("BB: %s-%d, adoadj of ch[%d] already reaches it's upper bound, 0x%x\n", __func__,__LINE__, i, ((adoadj & (0xf << i*4)) >> (i*4)));
					continue; //adoadj already reaches it's upper bound
				}
				else {
					new_adoadj += (adjTap[i] << i*4);
					FT2_DBG("BB: %s-%d, new adoadj of ch[%d] will be 0x%04x\n", __func__,__LINE__, i, new_adoadj);
				}
			}
		}

		if (((adjTap[0] != 0) | (adjTap[1] != 0) | (adjTap[2] != 0) | (adjTap[3] != 0)) && (new_adoadj == adoadj)) {
			FT2_DBG("EE: %s, adoadj reaches it's limitation (0x%04x)\n", __func__, adoadj);
			ret = 1;
			break;
		}

		if (new_adoadj != adoadj) {
			/* Giga_Eth_SetPhyMdio(1, 0xbcf, 22, new_adoadj); */
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xbcf);
			ca_ft2_phy_regfield_write(phy_dev, 22, 0, 16, new_adoadj);
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
		}

		udelay(10000);

		/*get PHY_ADO_CAL_TIMES of ado V*/
		for (cal_times = 0; cal_times < PHY_ADO_CAL_TIMES; cal_times++) {
			for (i = 0; i < 4; i++) {
				uint32_t voltmp = 0;
				/* voltmp = read_adoV_of_CH_X(i); */
				ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, (0xa8c + (i * 0x10)));
				ca_ft2_phy_regfield_read(phy_dev, 18, 0, 16, &value);
				ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
				voltmp = value;
				FT2_DBG("BB: %s-%d, V of ch[%d] = %x \n", __func__,__LINE__, i, voltmp);
				if(voltmp & 0x100)
					voltmp |= 0x1ff00;
				adoV[i] += voltmp;
				FT2_DBG("BB: %s-%d, V of adoV[%d] = %x \n", __func__,__LINE__, i, adoV[i]);
			}
		}

		/*get average of ado V*/
		for (i = 0; i < 4; i++) {
			adoV[i] &= 0x01ffff;
			if(adoV[i] & 0x10000){
				//negative
				adoV[i] = 0x20000 - adoV[i];
				sign[i] = 0;
			}else{
				//postive
				sign[i] = 1;
			}
			avgV[i] = adoV[i] / PHY_ADO_CAL_TIMES;
			//reset ado vol for next round
			adoV[i] = 0;
			FT2_DBG("BB: %s, average ado V of ch[%d] = %d mv(0x%04x) sign=%d\n", __func__, i, avgV[i], avgV[i], sign[i]);
		}

		adj_cnt--;
		if (adj_cnt == 0) {
			printf("EE: %s, timeout!\n", __func__);
			ret = 1;
			break;
		}

	} while(!is_ch_outputV_in_range_ado(avgV, adjTap, 0, 5, sign));

#if defined(CONFIG_TARGET_VENUS)
	/* workaround for analog programmable gain amplifier citcuit stability issue
		kill chip with ado over range
	*/
	if ((ret == FT2_OK) && (FT2_OK != ca_ft2_phy_ado_verify(port, avgV, sign)))
		ret = 1;
#endif

	/*
	REG32(GPHY_CAL_RESULT_2) |= Giga_Eth_ReadPhyMdio(1, 0xbcf, 22);
	efuse[ADO_CAL] = Giga_Eth_ReadPhyMdio(1, 0xbcf, 22); */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xbcf);
	ca_ft2_phy_regfield_read(phy_dev, 22, 0, 16, &adoadj);
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	ca_ft2_lan_switch(LAN_SWITCH_BUS1);

	/*
	vol_res[ADO_V_A] = avgV[0];
	vol_res[ADO_V_B] = avgV[1];
	vol_res[ADO_V_C] = avgV[2];
	vol_res[ADO_V_D] = avgV[3]; */
	if (FT2_OK != ca_ft2_phy_k_result_set(port, PHY_K_ADO, adoadj))
		return FT2_ERROR;

	return ret;
}

static ft2_ret_t ca_ft2_phy_rc_calibration(
	uint8_t port)
{
	uint8_t phy_dev;
	uint16_t value, i;
	uint8_t len_avg[PHY_CHANNEL_MAX] = {0}, rlen_avg[PHY_CHANNEL_MAX] = {0};
	uint16_t len_sum[PHY_CHANNEL_MAX] = {0}, rlen_sum[PHY_CHANNEL_MAX] = {0};
	uint16_t len_val = 0, rlen_val = 0;
	uint16_t times = 0;

	printf("II: GPHY Port %d RC Calibration\n", port);

	ca_ft2_lan_switch(LAN_SWITCH_OPEN);

	/* get phy addr */
	phy_dev = phy_addr[port];

	ca_ft2_phy_calibration_prepare(port, PHY_K_RC_LEN);

	/* process rc calibration */
	for (times = 1; times <= PHY_RC_K_TIMES; times++) {

		/* select extension page */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa47);
		/* enable rc_cal_en */
		ca_ft2_phy_regfield_write(phy_dev, 17, 0, 16, 0x0200);

		/* check if rc calibration finish */
		i = PHY_READY_TIMEOUT;
		do {
			ca_ft2_phy_regfield_read(phy_dev, 17, 9, 1, &value);
			/* rc calibration complete */
			if (value == 0)
				break;
		} while ((--i) > 0);

		if (i == 0) {
			printf("%s: RC calibration timeout\n", __func__);
			return FT2_TIMEOUT;
		}
		/* back to page 0 */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		/* select extension page */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xbcd);
		/* read RC len */
		ca_ft2_phy_regfield_read(phy_dev, 22, 0, 16, &len_val);
		/* read RC r_len */
		ca_ft2_phy_regfield_read(phy_dev, 23, 0, 16, &rlen_val);
		/* back to page 0 */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		FT2_DBG("%s: Times #%d len value = 0x%04x, rlen value = 0x%04x\n",
			   __func__, times, len_val, rlen_val);

		for (i = PHY_CHANNEL_A; i < PHY_CHANNEL_MAX; i++) {
			len_sum[i] += (len_val >> i*4) & 0xf;
			rlen_sum[i] += (rlen_val >> i*4) & 0xf;
		}

	}

	/* get average value of len and rlen */
	for (i = PHY_CHANNEL_A; i < PHY_CHANNEL_MAX; i++) {
		len_avg[i] = (len_sum[i] / PHY_RC_K_TIMES) & 0xf;
		rlen_avg[i] = (rlen_sum[i] / PHY_RC_K_TIMES) & 0xf;
		/* check if the value out off the range */
		if ((len_avg[i] > rc_k_max[i]) || (len_avg[i] < rc_k_min[i]))
			return FT2_ERROR;
		if ((rlen_avg[i] > rc_k_max[i]) || (rlen_avg[i] < rc_k_min[i]))
			return FT2_ERROR;
	}

	len_val = (len_avg[PHY_CHANNEL_D] << 12 | len_avg[PHY_CHANNEL_C] << 8 |
		   len_avg[PHY_CHANNEL_B] << 4 | len_avg[PHY_CHANNEL_A]);
	rlen_val = (rlen_avg[PHY_CHANNEL_D] << 12 | rlen_avg[PHY_CHANNEL_C] << 8 |
		    rlen_avg[PHY_CHANNEL_B] << 4 | rlen_avg[PHY_CHANNEL_A]);

	if (rc_k_diff != 0) {
		FT2_DBG("%s: Avg. len value = 0x%04x, Avg. rlen value = 0x%04x\n",
			   __func__, len_val, rlen_val);

		len_val = ((len_avg[PHY_CHANNEL_D] + rc_k_diff) << 12 |
			   (len_avg[PHY_CHANNEL_C] + rc_k_diff) << 8 |
			   (len_avg[PHY_CHANNEL_B] + rc_k_diff) << 4 |
			   (len_avg[PHY_CHANNEL_A] + rc_k_diff));
		rlen_val = ((rlen_avg[PHY_CHANNEL_D] + rc_k_diff) << 12 |
			    (rlen_avg[PHY_CHANNEL_C] + rc_k_diff) << 8 |
			    (rlen_avg[PHY_CHANNEL_B] + rc_k_diff) << 4 |
			    (rlen_avg[PHY_CHANNEL_A] + rc_k_diff));

		FT2_DBG("%s: RC calibration diff value %d, Avg. len value = 0x%04x, Avg. rlen value = 0x%04x\n",
			   __func__, rc_k_diff, len_val, rlen_val);
	}

	/* select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xbcd);
	/* set RC len */
	ca_ft2_phy_regfield_write(phy_dev, 22, 0, 16, len_val);
	/* set RC r_len */
	ca_ft2_phy_regfield_write(phy_dev, 23, 0, 16, rlen_val);
	/* read RC len */
	ca_ft2_phy_regfield_read(phy_dev, 22, 0, 16, &len_val);
	/* read RC r_len */
	ca_ft2_phy_regfield_read(phy_dev, 23, 0, 16, &rlen_val);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	ca_ft2_lan_switch(LAN_SWITCH_BUS1);

	FT2_DBG("%s: RC calibration len value = 0x%04x, rlen value = 0x%04x\n",
		   __func__, len_val, rlen_val);

	if (FT2_OK != ca_ft2_phy_k_result_set(port, PHY_K_RC_LEN, len_val))
		return FT2_ERROR;
	if (FT2_OK != ca_ft2_phy_k_result_set(port, PHY_K_RC_RLEN, rlen_val))
		return FT2_ERROR;

	return FT2_OK;
}

static ft2_ret_t ca_ft2_phy_r_calibration(
	uint8_t port)
{
	uint8_t phy_dev;
	uint16_t value, i;
	uint8_t tapbintx_avg[PHY_CHANNEL_MAX] = {0}, tapbinrx_pm_avg[PHY_CHANNEL_MAX] = {0};
	uint16_t tapbintx_sum[PHY_CHANNEL_MAX] = {0}, tapbinrx_pm_sum[PHY_CHANNEL_MAX] = {0};
	uint16_t tapbintx = 0, tapbinrx_pm = 0;
	uint16_t times = 0;

	printf("II: GPHY Port %d R Calibration\n", port);

	ca_ft2_lan_switch(LAN_SWITCH_OPEN);

	/* get phy addr */
	phy_dev = phy_addr[port];

	ca_ft2_phy_calibration_prepare(port, PHY_K_R_TAPBINRX_PM);

	/* process rc calibration */
	for (times = 1; times <= PHY_R_K_TIMES; times++) {

		/* select extension page */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa47);
		/* enable r_cal_en */
		ca_ft2_phy_regfield_write(phy_dev, 17, 0, 16, 0x0400);

		/* check if rc calibration finish */
		i = PHY_READY_TIMEOUT;
		do {
			ca_ft2_phy_regfield_read(phy_dev, 17, 10, 1, &value);
			/* rc calibration complete */
			if (value == 0)
				break;
		} while ((--i) > 0);

		if (i == 0) {
			printf("%s: R calibration timeout\n", __func__);
			return FT2_TIMEOUT;
		}
		/* back to page 0 */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		/* select extension page */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xbce);
		/* read  tapbintx */
		ca_ft2_phy_regfield_read(phy_dev, 16, 0, 16, &tapbintx);
		/* read tapbinrx_pm */
		ca_ft2_phy_regfield_read(phy_dev, 17, 0, 16, &tapbinrx_pm);
		/* back to page 0 */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		FT2_DBG("%s: Times #%d tapbintx value = 0x%04x, tapbinrx_pm value = 0x%04x\n",
			   __func__, times, tapbintx, tapbinrx_pm);

		for (i = PHY_CHANNEL_A; i < PHY_CHANNEL_MAX; i++) {
			tapbintx_sum[i] += (tapbintx >> i*4) & 0xf;
			tapbinrx_pm_sum[i] += (tapbinrx_pm >> i*4) & 0xf;
		}

	}

	/* get average value of tapbintx and tapbinrx_pm */
	for (i = PHY_CHANNEL_A; i < PHY_CHANNEL_MAX; i++) {
		tapbintx_avg[i] = (tapbintx_sum[i] / PHY_R_K_TIMES) & 0xf;
		tapbinrx_pm_avg[i] = (tapbinrx_pm_sum[i] / PHY_R_K_TIMES) & 0xf;
	}

	tapbintx = (tapbintx_avg[PHY_CHANNEL_D] << 12 |
		    tapbintx_avg[PHY_CHANNEL_C] << 8 |
		    tapbintx_avg[PHY_CHANNEL_B] << 4 |
		    tapbintx_avg[PHY_CHANNEL_A]);
	tapbinrx_pm = (tapbinrx_pm_avg[PHY_CHANNEL_D] << 12 |
		       tapbinrx_pm_avg[PHY_CHANNEL_C] << 8 |
		       tapbinrx_pm_avg[PHY_CHANNEL_B] << 4 |
		       tapbinrx_pm_avg[PHY_CHANNEL_A]);

	if (r_k_diff != 0) {
		FT2_DBG("%s: Avg. tapbintx value = 0x%04x, Avg. tapbinrx_pm value = 0x%04x\n",
			   __func__, tapbintx, tapbinrx_pm);

		tapbintx = ((tapbintx_avg[PHY_CHANNEL_D] + r_k_diff) << 12 |
			    (tapbintx_avg[PHY_CHANNEL_C] + r_k_diff) << 8 |
			    (tapbintx_avg[PHY_CHANNEL_B] + r_k_diff) << 4 |
			    (tapbintx_avg[PHY_CHANNEL_A] + r_k_diff));
		tapbinrx_pm = ((tapbinrx_pm_avg[PHY_CHANNEL_D] + r_k_diff) << 12 |
			       (tapbinrx_pm_avg[PHY_CHANNEL_C] + r_k_diff) << 8 |
			       (tapbinrx_pm_avg[PHY_CHANNEL_B] + r_k_diff) << 4 |
			       (tapbinrx_pm_avg[PHY_CHANNEL_A] + r_k_diff));

		FT2_DBG("%s: R calibration diff value %d, Avg. tapbintx value = 0x%04x, Avg. tapbinrx_pm value = 0x%04x\n",
		           __func__, r_k_diff, tapbintx, tapbinrx_pm);
	}

	/* select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xbce);
	/* set tapbintx */
	ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, tapbintx);
	/* set tapbinrx_pm */
	ca_ft2_phy_regfield_write(phy_dev, 17, 0, 16, tapbinrx_pm);
	/* read  tapbintx */
	ca_ft2_phy_regfield_read(phy_dev, 16, 0, 16, &tapbintx);
	/* read tapbinrx_pm */
	ca_ft2_phy_regfield_read(phy_dev, 17, 0, 16, &tapbinrx_pm);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	ca_ft2_lan_switch(LAN_SWITCH_BUS1);

	FT2_DBG("%s: R calibration tapbintx value = 0x%04x, tapbinrx_pm value = 0x%04x\n",
		   __func__, tapbintx, tapbinrx_pm);

	if (FT2_OK != ca_ft2_phy_k_result_set(port, PHY_K_R_TAPBIN_TX, tapbintx))
		return FT2_ERROR;
	if (FT2_OK != ca_ft2_phy_k_result_set(port, PHY_K_R_TAPBINRX_PM, tapbinrx_pm))
		return FT2_ERROR;

	return FT2_OK;
}

static ft2_ret_t ca_ft2_phy_amp_calibration(
	uint8_t port)
{
	uint8_t phy_dev;
	uint16_t ibadj, ibadj_sel[PHY_CHANNEL_MAX] = {0};
	int16_t posV[PHY_CHANNEL_MAX][16], negV[PHY_CHANNEL_MAX][16];
	uint16_t aid_diff[PHY_CHANNEL_MAX], diff;
	int i, j;
	ft2_ret_t rc = FT2_OK;

	printf("II: GPHY Port %d AMP Calibration\n", port);

	ca_ft2_lan_switch(LAN_SWITCH_OPEN);

	/* get phy addr */
	phy_dev = phy_addr[port];

	for (i = 0; i < PHY_CHANNEL_MAX; i++)
		aid_diff[i] = 2 * amp_volt_aid;

	ca_ft2_phy_calibration_prepare(port, PHY_K_AMP_IBADJ);

	/* select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bca);
	/* read ibadj */
	ca_ft2_phy_regfield_read(phy_dev, 22, 0, 16, &ibadj);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	FT2_DBG("%s: Inital ibadj value = 0x%04x\n", __func__, ibadj);

	for (i = 0; i < PHY_CHANNEL_MAX; i++) {
		FT2_DBG("%s: Channel %c ibadj value = 0x%02x\n", __func__,
			   'A' + i, (ibadj >> (i * 4)) & 0xf);
	}

	for (i = 0; i < 16; i++) {
		ibadj = i | (i << 4) | (i << 8) | (i << 12);
		FT2_DBG("%s: new ibadj value = 0x%04x\n", __func__, ibadj);

		/* select extension page */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bca);
		/* write ibadj */
		ca_ft2_phy_regfield_write(phy_dev, 22, 0, 16, ibadj);
		/* back to page 0 */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		/* Set DC output voltage +1V */
		/* select extension page */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa46);
		/* read ibadj */
		ca_ft2_phy_regfield_write(phy_dev, 21, 0, 16, 0x0302);
		/* back to page 0 */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		/* select extension page */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa58);
		/* enable DAC test */
		ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, 0x1000);
		/* back to page 0 */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		/* select extension page */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa46);
		/* read ibadj */
		ca_ft2_phy_regfield_write(phy_dev, 21, 0, 16, 0x0300);
		/* back to page 0 */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		udelay(3000);

		/* Read channel volatage */
		for (j = PHY_CHANNEL_A; j < PHY_CHANNEL_MAX; j++) {
			ca_ft2_voltage_get(port, j, &posV[j][i]);
		}

		/* Set DC output voltage -1V */
		/* select extension page */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa58);
		/* enable DAC test */
		ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, 0x1010);
		/* back to page 0 */
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		udelay(3000);

		/* Read channel volatage */
		for (j = PHY_CHANNEL_A; j < PHY_CHANNEL_MAX; j++) {
			ca_ft2_voltage_get(port, j, &negV[j][i]);
		}

		for (j = PHY_CHANNEL_A; j < PHY_CHANNEL_MAX; j++) {
			diff = abs((abs(posV[j][i]) + abs(negV[j][i])) / 2 - amp_volt_aid);

			FT2_DBG("%s: Channel %c +1V volt %d, -1V volt %d, diff %d\n",
				   __func__, 'A' + j, posV[j][i], negV[j][i], diff);

			if (diff < aid_diff[j]) {
				aid_diff[j] = diff;
				ibadj_sel[j] = i;
			}
		}
	}

	for (j = PHY_CHANNEL_A; j < PHY_CHANNEL_MAX; j++) {
		phy_k_amp[port][j*2] = negV[j][ibadj_sel[j]];
		phy_k_amp[port][j*2+1] = posV[j][ibadj_sel[j]];
	}

	ibadj = ibadj_sel[0] | (ibadj_sel[1] << 4) | (ibadj_sel[2] << 8) |
		(ibadj_sel[3] << 12);

	/* select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bca);
	/* write ibadj */
	ca_ft2_phy_regfield_write(phy_dev, 22, 0, 16, ibadj);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa58);
	ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, 0x0000);
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	ca_ft2_lan_switch(LAN_SWITCH_BUS1);

	FT2_DBG("%s: AMP calibration ibadj value = 0x%04x\n", __func__, ibadj);

	for (i = PHY_CHANNEL_A; i < PHY_CHANNEL_MAX; i++) {
		if (aid_diff[i] > amp_volt_range) {
			FT2_DBG("%s: Channel %c min diff %d - AMP-K FAIL\n",
				   __func__, 'A' + i, aid_diff[i]);
			rc = FT2_ERROR;
		} else {
			FT2_DBG("%s: Channel %c min diff %d - AMP-K PASS\n",
				   __func__, 'A' + i, aid_diff[i]);
		}
	}

	if (FT2_OK != ca_ft2_phy_k_result_set(port, PHY_K_AMP_IBADJ, ibadj))
		return FT2_ERROR;

	return rc;
}

static ft2_ret_t ca_ft2_phy_mdi_calibration(
	uint8_t port)
{
	uint8_t phy_dev, i;
	int16_t posV[PHY_CHANNEL_MAX] = {0}, negV[PHY_CHANNEL_MAX] = {0};
	int value = 0;

	printf("II: GPHY Port %d MDI Calibration\n", port);

	ca_ft2_lan_switch(LAN_SWITCH_BUS0);

	/* get phy addr */
	phy_dev = phy_addr[port];

	ca_ft2_phy_calibration_prepare(port, PHY_K_MDI);

	/*Set DC output voltage +1V*/
	/* select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa46);
	/* read ibadj */
	ca_ft2_phy_regfield_write(phy_dev, 21, 0, 16, 0x0302);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/* select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa58);
	/* enable DAC test */
	ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, 0x1000);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/* select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa46);
	/* read ibadj */
	ca_ft2_phy_regfield_write(phy_dev, 21, 0, 16, 0x0300);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	udelay(3000);

	/*Read channel volatage*/
	for (i = PHY_CHANNEL_A; i < PHY_CHANNEL_MAX; i++) {
		ca_ft2_voltage_get(port, i, &posV[i]);
	}

	/* Set DC output voltage -1V */
	/* select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa58);
	/* enable DAC test */
	ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, 0x1010);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	udelay(3000);

	/*Read channel volatage*/
	for (i = PHY_CHANNEL_A; i < PHY_CHANNEL_MAX; i++) {
		ca_ft2_voltage_get(port, i, &negV[i]);
	}

	/* check if channel voltage is in range */
	for (i = PHY_CHANNEL_A; i < PHY_CHANNEL_MAX; i++) {
		int16_t volt;
		bool pass;

		volt = abs(posV[i]);
		phy_k_mdi[port][i*2] = posV[i];
		if (volt >= ch_mdi_min[i] && volt <= ch_mdi_max[i]) {
			pass = true;
		} else {
			pass = false;
			value |= 1 << i;
		}
		FT2_DBG("%s: Channel %c +1V volt %d %s range(%d - %d)\n",
			   __func__, 'A' + i, posV[i],
			   pass == true ? "in" : "out of",
			   ch_mdi_min[i], ch_mdi_max[i]);

		volt = abs(negV[i]);
		phy_k_mdi[port][i*2 + 1] = negV[i];
		if (volt >= ch_mdi_min[i] && volt <= ch_mdi_max[i]) {
			pass = true;
		} else {
			pass = false;
			value |= 1 << i;
		}
		FT2_DBG("%s: Channel %c -1V volt %d %s range(%d - %d)\n",
			   __func__, 'A' + i, negV[i],
			   pass == true ? "in" : "out of",
			   ch_mdi_min[i], ch_mdi_max[i]);
	}

	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa58);
	ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, 0x0000);
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	ca_ft2_lan_switch(LAN_SWITCH_BUS1);

	if (FT2_OK != ca_ft2_phy_k_result_set(port, PHY_K_MDI, value))
		return FT2_ERROR;

	return value ? FT2_ERROR : FT2_OK;
}

static ft2_ret_t ca_ft2_phy_k_result_get(
	uint8_t port,
	phy_k_item_t item,
	uint16_t *value)
{
	if (port >= PHY_K_PORT_NUM || item >= PHY_K_MAX) {
		printf("%s: parameter error\n", __func__);
		return FT2_ERROR;
	}

	*value = phy_k_result[port][item];
	return FT2_OK;
}

static ft2_ret_t ca_ft2_phy_calibration_result_dump(
	uint8_t port)
{
	uint16_t item, value;
	int i, len;
	char buf[256];

	printf("\nGPHY Port %d (0x%x) Calibration Result:\n", port, phy_addr[port]);

	for (item = PHY_K_ADO; item < PHY_K_MAX; item++) {
		if (FT2_OK != ca_ft2_phy_k_result_get(port, item, &value))
			return FT2_ERROR;

		switch (item) {
		case PHY_K_ADO:
			printf("ADO-K = 0x%04x\n", value);
			break;
		case PHY_K_RC_LEN:
			printf("RC-K Len = 0x%04x\n", value);
			break;
		case PHY_K_RC_RLEN:
			printf("RC-K RLen = 0x%04x\n", value);
			break;
		case PHY_K_R_TAPBIN_TX:
			printf("R-K Tapbin = 0x%04x\n", value);
			break;
		case PHY_K_R_TAPBINRX_PM:
			printf("R-K Tapbin PM = 0x%04x\n", value);
			break;
		case PHY_K_AMP_IBADJ:
			printf("AMP-K IBADJ = 0x%04x", value);
			printf(" #");
			for (len=0, i = (PHY_CHANNEL_NUM * 2 - 1); i >= 0; i--)
				len += sprintf(buf+len, " %d", phy_k_amp[port][i]);
			printf(buf);
			printf("\n");
			break;
		case PHY_K_MDI:
			printf("MDI-K Result = 0x%04x", value);
			printf(" #");
			for (len=0, i = (PHY_CHANNEL_NUM * 2 - 1); i >= 0; i--)
				len += sprintf(buf+len, " %d", phy_k_mdi[port][i]);
			printf(buf);
			printf("\n");
			break;
		default:
			printf("Invalid Calibration Item!!\n");
			break;
		}
	}
	printf("\n");
	return FT2_OK;
}

static ft2_ret_t ca_ft2_phy_recover(
	uint8_t port)
{
	uint8_t phy_dev;

	/* get phy addr */
	phy_dev = phy_addr[port];

	/* uc_sram_table_RL6386.xls - disable spdChg : 0x8011 bit[1], -->data16 bit[9] */
	ca_ft2_phy_regfield_write(phy_dev, 27, 0, 16, 0x8011);
	/* 1 = enable spdChg */
	ca_ft2_phy_regfield_write(phy_dev, 28, 9, 1, 0x1);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/* rg_ana_ctl.xls - select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bcc);
	/* modify gdac_ib_dn_up */
	ca_ft2_phy_regfield_write(phy_dev, 18, 0, 16, 0xFF00);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

	/* rg_pcs.xls - select extension page */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0a43);
	/* enable ALDPS */
	ca_ft2_phy_regfield_write(phy_dev, 24, 2, 1, 0x1);
	/* back to page 0 */
	ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
	return FT2_OK;
}

static ft2_ret_t ca_ft2_phy_calibration(ft2_record_t *record)
{
	ft2_ret_t rc = FT2_OK, rc1;
	uint8_t i, port;

	FT2_MOD("FT2_PARAM_GPHY_K test ...\n");

	record->entry_mask = GMII_PORT_MASK;
	record->start = get_timer(0);

	memset(phy_k_result, 0, sizeof(phy_k_result));

	for (i = 0; (port = ge_ports[i]) != INVALID_PORT; i++) {
		record->entry_test[i] = TEST_ALL_CALIBRATION;

		/* process ADO Calibration */
		rc1 = ca_ft2_phy_ado_calibration(port);
		if (rc1 == FT2_OK) {
			printf("ADO PASS\n");
		} else {
			printf("ADO FAIL\n");
			rc = rc1;
			record->entry_rlt[i] |= TEST_ADO_CALIBRATION;
		}

		/* process RC Calibration */
		rc1 = ca_ft2_phy_rc_calibration(port);
		if (rc1 == FT2_OK) {
			printf("RC-K PASS\n");
		} else {
			printf("RC-K FAIL\n");
			rc = rc1;
			record->entry_rlt[i] |= TEST_RC_CALIBRATION;
		}

		/* process R Calibration */
		rc1 = ca_ft2_phy_r_calibration(port);
		if (rc1 == FT2_OK) {
			printf("R-K PASS\n");
		} else {
			printf("R-K FAIL\n");
			rc = rc1;
			record->entry_rlt[i] |= TEST_RC_CALIBRATION;
		}

		/* process AMP Calibration */
		rc1 = ca_ft2_phy_amp_calibration(port);
		if (rc1== FT2_OK) {
			printf("AMP-K PASS\n");
		} else {
			printf("AMP-K FAIL\n");
			rc = rc1;
			record->entry_rlt[i] |= TEST_AMP_CALIBRATION;
		}

		/* process MDI Calibration */
		rc1 = ca_ft2_phy_mdi_calibration(port);
		if (rc1 == FT2_OK) {
			printf("MDI-K PASS\n");
		} else {
			printf("MDI-K FAIL\n");
			record->entry_rlt[i] |= TEST_MDI_CALIBRATION;
			rc = rc1;
		}

		ca_ft2_phy_calibration_result_dump(port);
		ca_ft2_phy_recover(port);
	}

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	FT2_MOD("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return rc;
}

ft2_ret_t ca_ft2_phy_calibration_reload(ca_ft2_cfg_t *cfg_data)
{
	uint16_t adoadj, rlen_val, tapbinrx_pm, ibadj;
	uint8_t i, port;
	uint8_t phy_dev;

	for (i = 0; (port = ge_ports[i]) != INVALID_PORT; i++) {
		phy_dev = phy_addr[port];

		if (cfg_data->param.sw_patch)
			continue;

		if (cfg_data->param.en_ado_cal) {
			adoadj = cfg_data->phy_k_data[i].adc0_cal;
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xbcf);
			ca_ft2_phy_regfield_write(phy_dev, 22, 0, 16, adoadj);
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
		}

		if (cfg_data->param.en_rc_cal) {
			rlen_val = cfg_data->phy_k_data[i].rc_cal_len;
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xbcd);
			ca_ft2_phy_regfield_write(phy_dev, 22, 0, 16, rlen_val);
			ca_ft2_phy_regfield_write(phy_dev, 23, 0, 16, rlen_val);
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
		}

		if (cfg_data->param.en_r_cal) {
			tapbinrx_pm = cfg_data->phy_k_data[i].r_cal;
			tapbinrx_pm = tapbinrx_pm | (tapbinrx_pm << 4) | (tapbinrx_pm << 8) | (tapbinrx_pm << 12);
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xbce);
			ca_ft2_phy_regfield_write(phy_dev, 16, 0, 16, tapbinrx_pm);
			ca_ft2_phy_regfield_write(phy_dev, 17, 0, 16, tapbinrx_pm);
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
		}

		if (cfg_data->param.en_amp_cal) {
			ibadj = cfg_data->phy_k_data[i].amp_cal;
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0x0bca);
			ca_ft2_phy_regfield_write(phy_dev, 22, 0, 16, ibadj);
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
		}

		if (cfg_data->param.disable_500m) {
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa4a);
			ca_ft2_phy_regfield_write(phy_dev, 17, 8, 1, 0);
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
		} else {
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0xa4a);
			ca_ft2_phy_regfield_write(phy_dev, 17, 8, 1, 1);
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
		}
	}

	return FT2_OK;
}

ft2_ret_t ca_ft2_gphy_snr_test(ft2_record_t *record)
{
#define CHECK_SNR_TIMES		10

	ft2_ret_t rc = FT2_OK;
	uint8_t i, j, phy_dev, port, times, reg;
	uint32_t gphy_snr[PHY_K_PORT_NUM][PHY_CHANNEL_NUM];
	uint16_t page, value;

#if (GPHY_SNR_MASK > 0)
	printf("FT2_PARAM_GPHY_SNR test ...\n");

	record->entry_mask = GMII_PORT_MASK;
	record->start = get_timer(0);

	memset(gphy_snr, 0, sizeof(gphy_snr));
	for (times = 1; times <= CHECK_SNR_TIMES; times++) {
		for (i = 0; (port = ge_ports[i]) != INVALID_PORT; i++) {
			record->entry_test[i] = TEST_GPHY_SNR;
			FT2_DBG("%s: Times #%d, Port #%d\n", __func__, times, port);

			/* get phy addr */
			phy_dev = phy_addr[port];
			/* check gphy snr */
			for (j = PHY_CHANNEL_A; j < PHY_CHANNEL_MAX; j++) {
				/* read channel snr */
				page = 0xa8c + j * 0x10;
				reg = 16;
				ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, page);
				ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
				FT2_DBG("%s: Channel #%c page = 0x%04x, reg = %d, value = 0x%04x\n", __func__, (65+j), page, reg, value);
				gphy_snr[i][j] += value;

				/* check gphy channel snr */
				if (times == CHECK_SNR_TIMES) {
					gphy_snr[i][j] = gphy_snr[i][j]/CHECK_SNR_TIMES;
					if (gphy_snr[i][j] > gphy_avg_snr) {
						FT2_DBG("%s: Port #%d Channel #%c  Average SNR = 0x%04x over 0x%04x\n", __func__, port, (65+j), gphy_snr[i][j], gphy_avg_snr);
						record->entry_rlt[i] |= TEST_GPHY_SNR;
						rc |= FT2_GPHY_SNR;
					} else {
						FT2_DBG("%s: Port #%d Channel #%c  Average SNR = 0x%04x\n", __func__, port, (65+j), gphy_snr[i][j]);
					}
				}
			}
			ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
		}
	}

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return rc;
#else
	printf("No GPHY SNR test\n");
	return FT2_GPHY_SNR;
#endif
}


#define OP_READ         0
#define OP_WRITE        1

#define FT2_CONFIG_ALL_LOAD			(1 << 0)
#define FT2_CONFIG_ALL_UPDATE	(1 << 1)
#define FT2_CONFIG_CMPLT_BIT_UPDATE	(1 << 2)

static void ca_ft2_cfg_dump(ca_ft2_cfg_t *cfg_data)
{
	int i;

	printf("########################\n");
	printf("uuid:        0x%08x\n", cfg_data->uuid);
	printf("hvsid:       0x%02x\n", cfg_data->hvsid);
	printf("reserved1_0: 0x%06x\n", cfg_data->reserved1_0);
	printf("reserved2:   0x%08x\n", cfg_data->reserved2);
	printf("########################\n");
	printf("sw_patch:       %d\n", cfg_data->param.sw_patch);
	printf("phy_cmd:        %d\n", cfg_data->param.phy_cmd);
	printf("reserved3_0:    0x%01x\n", cfg_data->param.reserved3_0);
	printf("gphy_cal_cmplt: %d\n", cfg_data->param.gphy_cal_cmplt);
	printf("dgac_lb_dn_up:  %d\n", cfg_data->param.dgac_lb_dn_up);
	printf("spd_chg:        %d\n", cfg_data->param.spd_chg);
	printf("reserved3_1:    0x%03x\n", cfg_data->param.reserved3_1);
	printf("en_ado_cal:     %d\n", cfg_data->param.en_ado_cal);
	printf("en_rc_cal:      %d\n", cfg_data->param.en_rc_cal);
	printf("en_r_cal:       %d\n", cfg_data->param.en_r_cal);
	printf("en_amp_cal:     %d\n", cfg_data->param.en_amp_cal);
	printf("disable_500m:   %d\n", cfg_data->param.disable_500m);
	printf("reserved3_2:    0x%03x\n", cfg_data->param.reserved3_2);
	printf("########################\n");
	for (i = 0; i < 3; i++)
		printf("phy_patch(%d): 0x%08x\n", i, cfg_data->phy_patch[i]);
	printf("########################\n");
	printf("test_prog_ver: 0x%08x\n", cfg_data->test_prog_ver);
	printf("########################\n");
	for (i = 0; i < PHY_K_PORT_NUM; i++) {
		printf("rc_cal_len(%d): 0x%04x\n", i, cfg_data->phy_k_data[i].rc_cal_len);
		printf("amp_cal(%d):    0x%04x\n", i, cfg_data->phy_k_data[i].amp_cal);
		printf("adc0_cal(%d):   0x%04x\n", i, cfg_data->phy_k_data[i].adc0_cal);
		printf("r_cal(%d):      0x%04x\n", i, cfg_data->phy_k_data[i].r_cal);
		printf("reserved(%d):   0x%04x\n", i, cfg_data->phy_k_data[i].reserved);
	}
}

#ifdef CONFIG_CA_EFUSE
static ft2_ret_t ca_ft2_efuse_config(uint32_t action, ca_ft2_cfg_t *cfg_data)
{
	struct cortina_efuse_context context;
	int i, group, red_group;
	void *buf;
	uint32_t val;
	ca_phy_param_t phy_param;

	FT2_DBG("%s: action = %d\n", __func__, action);

	context.nregs = 32;
	context.redundant = 1;

	/* save all ft2 data */
	if (action & FT2_CONFIG_ALL_UPDATE) {
		if (cfg_data == NULL)
			return FT2_ERROR;
		/* save phy param */
		group = FT2_CFG_PHY_PARAM_OFFSET / 4;
		buf = &cfg_data->param;
		cortina_efuse_access(group, buf, OP_WRITE);
		if (context.redundant) {
			red_group = group + context.nregs / 2;
			cortina_efuse_access(red_group, buf, OP_WRITE);
		}
		/* save sw version */
		buf = &cfg_data->test_prog_ver;
		group = FT2_CFG_TEST_PROG_VER_OFFSET / 4;
		cortina_efuse_access(group, buf, OP_WRITE);
		if (context.redundant) {
			red_group = group + context.nregs / 2;
			cortina_efuse_access(red_group, buf, OP_WRITE);
		}
		/* save phy_k data */
		buf = cfg_data->phy_k_data;
		for (i = 0; i < sizeof(cfg_data->phy_k_data) / 4; i++, buf += 4) {
			//printf("i = %d, buf = 0x%08x\n", i, *(uint32_t *)buf);
			group = FT2_CFG_PHY_CALIB_OFFSET / 4 + i;
			cortina_efuse_access(group, buf, OP_WRITE);
			if (context.redundant) {
				red_group = group + context.nregs / 2;
				cortina_efuse_access(red_group, buf, OP_WRITE);
			}
		}
	}

	/* program efuse.gphy_cal_cmplt=1 (efuse group 3, bit 4), after efuse test success */
	if (action & FT2_CONFIG_CMPLT_BIT_UPDATE) {
		memset(&phy_param, 0, sizeof(ca_phy_param_t));
		phy_param.gphy_cal_cmplt = 1;
		buf = &phy_param;
		group = FT2_CFG_PHY_PARAM_OFFSET / 4;
		cortina_efuse_access(group, buf, OP_WRITE);
		if (context.redundant) {
			red_group = group + context.nregs / 2;
			cortina_efuse_access(red_group, buf, OP_WRITE);
		}
	}

	/* read all ft2 data */
	if (action & FT2_CONFIG_ALL_LOAD) {
		if (cfg_data == NULL)
			return FT2_ERROR;
		buf = cfg_data;
		for (i = 0; i < sizeof(*cfg_data) / 4; i++, buf += 4) {
			cortina_efuse_access(i, buf, OP_READ);
			if (context.redundant) {
				cortina_efuse_access(i + context.nregs / 2,
						     &val, OP_READ);
				*(uint32_t *)buf = *(uint32_t *)buf | val;
			}
		}
	}

	return FT2_OK;
}

#else
static ft2_ret_t ca_ft2_otp_config(uint32_t action, ca_ft2_cfg_t *cfg_data)
{
	GLOBAL_SECURITY_OVERRIDE_t glb_scur;
	OTP_PROTEN_t otp_proten;
	uint8_t *p_value = NULL;
	uint32_t i, addr;
	ca_phy_param_t phy_param;

	FT2_DBG("%s: action = %d\n", __func__, action);

	/*  turn off otp register protection */
	glb_scur.wrd = CORTINA_CA_REG_READ(GLOBAL_SECURITY_OVERRIDE);
	glb_scur.bf.otp_lock_prot_fields = 0;
	CORTINA_CA_REG_WRITE(glb_scur.wrd, GLOBAL_SECURITY_OVERRIDE);
	/* turn off local otp protection bits */
	otp_proten.wrd = CORTINA_CA_REG_READ(OTP_PROTEN);
	otp_proten.bf.EN = 0;
	otp_proten.bf.PROG = 0;
	CORTINA_CA_REG_WRITE(otp_proten.wrd, OTP_PROTEN);

	/* write data to otp */
	/* save all ft2 data */
	if (action & FT2_CONFIG_ALL_UPDATE) {
		if (cfg_data == NULL)
			return FT2_ERROR;
		/* save phy param */
		addr = OTP_FT2_CFG_BASE + FT2_CFG_PHY_PARAM_OFFSET;
		p_value = (uint8_t *)&cfg_data->param;
		for (i = 0; (i < sizeof(ca_phy_param_t)) && (addr <= OTP_FT2_CFG_END); i++, addr++, p_value++) {
			FT2_DBG("%s: WRITE addr = 0x%08x, value = 0x%02x\n", __func__, addr, *p_value);
			CA_OTP_REG_WRITE(*p_value, addr);
		}
		/* save sw version */
		addr = OTP_FT2_CFG_BASE + FT2_CFG_TEST_PROG_VER_OFFSET;
		p_value = (uint8_t *)&cfg_data->test_prog_ver;
		for (i = 0; (i < 4) && (addr <= OTP_FT2_CFG_END); i++, addr++, p_value++) {
			FT2_DBG("%s: WRITE addr = 0x%08x, value = 0x%02x\n", __func__, addr, *p_value);
			CA_OTP_REG_WRITE(*p_value, addr);
		}
		/* save phy_k data */
		addr = OTP_FT2_CFG_BASE + FT2_CFG_PHY_CALIB_OFFSET;
		p_value = (uint8_t *)&cfg_data->phy_k_data;
		for (i = 0; (i < 4*sizeof(ca_phy_k_t)) && (addr <= OTP_FT2_CFG_END); i++, addr++, p_value++) {
			FT2_DBG("%s: WRITE addr = 0x%08x, value = 0x%02x\n", __func__, addr, *p_value);
			CA_OTP_REG_WRITE(*p_value, addr);
		}
	}

	/* program cfg.gphy_cal_cmplt=1, after cfg test success */
	if (action & FT2_CONFIG_CMPLT_BIT_UPDATE) {
		memset(&phy_param, 0, sizeof(ca_phy_param_t));
		phy_param.gphy_cal_cmplt = 1;
		addr = OTP_FT2_CFG_BASE + FT2_CFG_PHY_PARAM_OFFSET;
		p_value = (uint8_t *)&phy_param;
		FT2_DBG("%s: WRITE addr = 0x%08x, value = 0x%02x\n", __func__, addr, *p_value);
		CA_OTP_REG_WRITE(*p_value, addr);
	}

	/* read all ft2 data */
	if (action & FT2_CONFIG_ALL_LOAD) {
		if (cfg_data == NULL)
			return FT2_ERROR;
		addr = OTP_FT2_CFG_BASE;
		p_value = (uint8_t *)cfg_data;
		for (i = 0; i < sizeof(ca_ft2_cfg_t); i++, addr++, p_value++) {
			CA_OTP_REG_READ(*p_value, addr);
			FT2_DBG("%s: READ addr = 0x%08x, value = 0x%02x\n", __func__, addr, *p_value);
		}
	}

	/* turn on local otp protection bits */
	otp_proten.wrd = CORTINA_CA_REG_READ(OTP_PROTEN);
	otp_proten.bf.EN = 1;
	otp_proten.bf.PROG = 1;
	CORTINA_CA_REG_WRITE(otp_proten.wrd, OTP_PROTEN);
	/*  turn on otp register protection */
	glb_scur.wrd = CORTINA_CA_REG_READ(GLOBAL_SECURITY_OVERRIDE);
	glb_scur.bf.otp_lock_prot_fields = 1;
	CORTINA_CA_REG_WRITE(glb_scur.wrd, GLOBAL_SECURITY_OVERRIDE);

	return FT2_OK;
}
#endif

ft2_ret_t ca_ft2_cfg_confirm(ca_ft2_cfg_t *cfg_data)
{
	int rc = 0;

	if ((cfg_data->param.gphy_cal_cmplt != 1) ||
	    (cfg_data->param.en_ado_cal != 1) ||
	    (cfg_data->param.en_rc_cal != 1) ||
	    (cfg_data->param.en_r_cal != 1) ||
	    (cfg_data->param.en_amp_cal != 1))
	    rc = -1;

	printf("{%s:%s}\n", ft2_module_name[FT2_PARAM_CFG_W],
	       rc ? "FAIL" : "PASS");
	return rc ? FT2_CFG : FT2_OK;
}

ft2_ret_t ca_ft2_cfg_save(ft2_record_t *record)
{
	ca_ft2_cfg_t *cfg_data = record->ptr_data;
	ca_ft2_cfg_t new_cfg_data;
	uint16_t value, retry=0;
	int rc = 0;
	int i;
	uint32_t *p_org_data, *p_new_data;

	record->entry_mask = 1;
	record->start = get_timer(0);
	record->entry_test[0] |= 1;

	printf("FT2_PARAM_CFG_W test ...\n");

	/* set test program version */
	cfg_data->test_prog_ver = TEST_PROG_VER;

	cfg_data->param.gphy_cal_cmplt = 0;
#if defined(CONFIG_TARGET_SATURN_ASIC)
	cfg_data->param.dgac_lb_dn_up = 0;
#elif defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_VENUS)
	cfg_data->param.dgac_lb_dn_up = 1;
#endif
	cfg_data->param.en_ado_cal = 1;
	cfg_data->param.en_rc_cal = 1;
	cfg_data->param.en_r_cal = 1;
	cfg_data->param.en_amp_cal = 1;
	cfg_data->param.disable_500m = 1;

	for (i = 0; i < PHY_K_PORT_NUM; i++) {
		ca_ft2_phy_k_result_get(i, PHY_K_ADO, &value);
		cfg_data->phy_k_data[i].adc0_cal = value;
		ca_ft2_phy_k_result_get(i, PHY_K_RC_RLEN, &value);
		cfg_data->phy_k_data[i].rc_cal_len = value;
		ca_ft2_phy_k_result_get(i, PHY_K_R_TAPBINRX_PM, &value);
		cfg_data->phy_k_data[i].r_cal = value & 0x000F;
		ca_ft2_phy_k_result_get(i, PHY_K_AMP_IBADJ, &value);
		cfg_data->phy_k_data[i].amp_cal = value;
	}

	do {
		/* don't touch cfg uuid/hvsid */
		cfg_data->uuid = 0;
		cfg_data->hvsid = 0;
		FT2_CFG_ACCESS(FT2_CONFIG_ALL_UPDATE, cfg_data);

		FT2_CFG_ACCESS(FT2_CONFIG_ALL_LOAD, &new_cfg_data);
		/* don't compare cfg uuid/hvsid */
		new_cfg_data.uuid = 0;
		new_cfg_data.hvsid = 0;

		if (memcmp(cfg_data, &new_cfg_data, sizeof(ca_ft2_cfg_t))) {
			/* cfg data compare fail */
			record->entry_rlt[0] = 1;

			p_org_data = (uint32_t *)cfg_data;
			p_new_data = (uint32_t *)&new_cfg_data;
			for (i = 0; i < sizeof(ca_ft2_cfg_t)/4; i++, p_org_data++, p_new_data++) {
				FT2_MOD("group %02d, org = 0x%08x, new = 0x%08x [%s]\n",
					i, *p_org_data, *p_new_data, (*p_org_data != *p_new_data)?"NOK":"OK");
			}
		} else {
			/* program gphy_cal_cmplt=1 after ft2 data save success */
			FT2_CFG_ACCESS(FT2_CONFIG_CMPLT_BIT_UPDATE, NULL);
			FT2_CFG_ACCESS(FT2_CONFIG_ALL_LOAD, &new_cfg_data);
			p_new_data = (uint32_t *)&new_cfg_data;
			for (i = 0; i < sizeof(ca_ft2_cfg_t)/4; i++, p_new_data++) {
				FT2_DBG("group %02d, value = 0x%08x\n", i, *p_new_data);
			}
			/* check if gphy_cal_cmplt=1 */
			if (!new_cfg_data.param.gphy_cal_cmplt) {
				/* retry to update gphy_cal_cmplt */
				FT2_CFG_ACCESS(FT2_CONFIG_CMPLT_BIT_UPDATE, NULL);
				FT2_CFG_ACCESS(FT2_CONFIG_ALL_LOAD, &new_cfg_data);
				/* cfg.gphy_cal_cmplt program fail */
				if (!new_cfg_data.param.gphy_cal_cmplt)
					record->entry_rlt[0] = 1;
			}
			break;
		}
		FT2_MOD("CFG retry times: %d\n", retry);
	}while(++retry < 3);

	rc = record->entry_rlt[0];

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc ? FT2_CFG : FT2_OK;
}

ft2_ret_t ca_ft2_cfg_resorting_test(ft2_record_t *record,  int have_record)
{
	ca_ft2_cfg_t *cfg_data = record->ptr_data;
	int rc = 0;

	record->entry_mask = 1;
	record->start = get_timer(0);
	record->entry_test[0] |= 1;

	printf("FT2_PARAM_CFG_RESORT test ...\n");

	/* check if cfg.gphy_cal_cmplt=1 (cfg group 3, bit 4) */
	if (have_record && cfg_data->param.gphy_cal_cmplt)
		record->entry_rlt[0] = 0;
	else
		record->entry_rlt[0] = 1;

	rc = record->entry_rlt[0];

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc ? FT2_CFG : FT2_OK;
}

#if (SERDES_PORT_MASK > 0)
#define GLD_PRBS_INIT(addr, rc) {\
	uint32_t timeout = 50;\
	uint8_t i2c_mux, data=6;\
	rc = FT2_OK;\
	if (addr == GLD_B_PROGRAM_DEV)\
		i2c_mux = 5;\
	else if (addr == GLD_C_PROGRAM_DEV)\
		i2c_mux = 6;\
	else {\
		i2c_mux = 4;\
		data=5;\
	}\
	if (CA_E_OK != (ret = i2c_write(GLD_I2C_MUX_ADDR, 0, 0, &i2c_mux, 1))) {\
		FT2_DBG("%s - %d: Slave addr 0x%x select I2C channel %d, ret = %d\n", __func__, __LINE__, addr, i2c_mux, ret);\
		rc = FT2_I2C;\
	}\
	do {\
		if (CA_E_OK != (ret = i2c_read(GLD_A_PROGRAM_DEV/*addr*/, 0, 1, (ca_uint8_t *)&value, 4))) {\
			FT2_DBG("%s - %d: Read GLD ret = %d\n", __func__, __LINE__, ret);\
		} else\
			FT2_DBG("%s - %d: Read GLD value = 0x%08x\n", __func__, __LINE__, value);\
		if (value != 0xffffffff) {\
			FT2_DBG("%s - %d: GLD is up\n", __func__, __LINE__);\
			break;\
		}\
		udelay(500000);\
	} while (timeout--);\
	timeout = 40;\
	do {\
		if (CA_E_OK != (ret = i2c_write(GLD_FIX_DEV, 0, 1, &data, 1))) {\
			FT2_DBG("%s - %d: Init GLD PRBS, ret = %d\n", __func__, __LINE__, ret);\
		} else\
			break;\
		udelay(200000);\
	} while (timeout--);\
	if (!timeout)\
		rc = FT2_TIMEOUT;\
} while(0)

#define GLD_PRBS_CONFIG(addr, data, rc) {\
	uint32_t timeout = 30;\
	uint8_t i2c_mux;\
	rc = FT2_OK;\
	if (addr == GLD_B_PROGRAM_DEV)\
		i2c_mux = 5;\
	else if (addr == GLD_C_PROGRAM_DEV)\
		i2c_mux = 6;\
	else\
		i2c_mux = 4;\
	if (CA_E_OK != (ret = i2c_write(GLD_I2C_MUX_ADDR, 0, 0, &i2c_mux, 1))) {\
		FT2_DBG("%s - %d: Slave addr 0x%x select I2C channel %d, ret = %d\n", __func__, __LINE__, addr, i2c_mux, ret);\
		rc = FT2_I2C;\
	}\
	do {\
		if (CA_E_OK != (ret = i2c_write(GLD_FIX_DEV, 0, 1, &data, 1))) {\
			rc = FT2_I2C;\
			FT2_DBG("%s - %d: Start GLD PRBS, ret = %d\n", __func__, __LINE__, ret);\
		} else\
			break;\
		udelay(200000);\
	} while (timeout--);\
	if (!timeout)\
		rc = FT2_TIMEOUT;\
} while(0)

#define GLD_PRBS_START(addr, data, rc) {\
	uint32_t timeout = 30;\
	uint8_t i2c_mux;\
	rc = FT2_OK;\
	if (addr == GLD_B_PROGRAM_DEV)\
		i2c_mux = 5;\
	else if (addr == GLD_C_PROGRAM_DEV)\
		i2c_mux = 6;\
	else\
		i2c_mux = 4;\
	if (CA_E_OK != (ret = i2c_write(GLD_I2C_MUX_ADDR, 0, 0, &i2c_mux, 1))) {\
		FT2_DBG("%s - %d: Slave addr 0x%x select I2C channel %d, ret = %d\n", __func__, __LINE__, addr, i2c_mux, ret);\
		rc = FT2_I2C;\
	}\
	do {\
		if (CA_E_OK != (ret = i2c_write(GLD_FIX_DEV, 0, 1, &data, 1))) {\
			rc = FT2_I2C;\
			FT2_DBG("%s - %d: Start GLD PRBS, ret = %d\n", __func__, __LINE__, ret);\
		} else\
			break;\
		udelay(200000);\
	} while (timeout--);\
	if (!timeout)\
		rc = FT2_TIMEOUT;\
} while(0)

#define GLD_PRBS_STOP(addr, rc) {\
	uint32_t timeout = 30;\
	uint8_t i2c_mux, data=0;\
	rc = FT2_OK;\
	if (addr == GLD_B_PROGRAM_DEV)\
		i2c_mux = 5;\
	else if (addr == GLD_C_PROGRAM_DEV)\
		i2c_mux = 6;\
	else\
		i2c_mux = 4;\
	if (CA_E_OK != (ret = i2c_write(GLD_I2C_MUX_ADDR, 0, 0, &i2c_mux, 1))) {\
		FT2_DBG("%s - %d: Slave addr 0x%x select I2C channel %d, ret = %d\n", __func__, __LINE__, addr, i2c_mux, ret);\
		rc = FT2_I2C;\
	}\
	do {\
		if (CA_E_OK != (ret = i2c_write(GLD_FIX_DEV, 0, 1, &data, 1))) {\
			rc = FT2_I2C;\
			FT2_DBG("%s - %d: Stop GLD PRBS, ret = %d\n", __func__, __LINE__, ret);\
		} else\
			break;\
		udelay(200000);\
	} while (timeout--);\
	if (!timeout)\
		rc = FT2_TIMEOUT;\
} while(0)

#define GLD_PRBS_STATUS_GET(addr, value, rc) {\
	uint32_t timeout = 30;\
	uint8_t i2c_mux;\
	rc = FT2_OK;\
	value = 0;\
	if (addr == GLD_B_PROGRAM_DEV)\
		i2c_mux = 5;\
	else if (addr == GLD_C_PROGRAM_DEV)\
		i2c_mux = 6;\
	else\
		i2c_mux = 4;\
	if (CA_E_OK != (ret = i2c_write(GLD_I2C_MUX_ADDR, 0, 0, &i2c_mux, 1))) {\
		FT2_DBG("%s - %d: Slave addr 0x%x select I2C channel %d, ret = %d\n", __func__, __LINE__, addr, i2c_mux, ret);\
		rc = FT2_I2C;\
	}\
	do {\
		udelay(800000);\
		if (CA_E_OK != (ret = i2c_read(GLD_A_PROGRAM_DEV/*addr*/, 0, 1, (ca_uint8_t *)&value, 4))) {\
			FT2_DBG("%s - %d: Read GLD ret = %d\n", __func__, __LINE__, ret);\
		} else\
			FT2_DBG("%s - %d: Read GLD value = 0x%08x\n", __func__, __LINE__, value);\
		if (!timeout--) {\
			rc = FT2_TIMEOUT;\
			break;\
		}\
	} while (!(value >> 7));\
} while(0)

#define GLD_WAIT_READY(addr, value, rc) {\
	uint32_t timeout = 30;\
	uint8_t i2c_mux;\
	rc = FT2_OK;\
	value = 0;\
	if (addr == GLD_B_PROGRAM_DEV)\
		i2c_mux = 5;\
	else if (addr == GLD_C_PROGRAM_DEV)\
		i2c_mux = 6;\
	else\
		i2c_mux = 4;\
	if (CA_E_OK != (ret = i2c_write(GLD_I2C_MUX_ADDR, 0, 0, &i2c_mux, 1))) {\
		FT2_DBG("%s - %d: Slave addr 0x%x select I2C channel %d, ret = %d\n", __func__, __LINE__, addr, i2c_mux, ret);\
		rc = FT2_I2C;\
	}\
	do {\
		udelay(200000);\
		if (CA_E_OK != (ret = i2c_read(GLD_A_PROGRAM_DEV/*addr*/, 0, 1, (ca_uint8_t *)&value, 4))) {\
			FT2_DBG("%s - %d: Read GLD ret = %d\n", __func__, __LINE__, ret);\
		} else\
			FT2_DBG("%s - %d: Read GLD value = 0x%08x\n", __func__, __LINE__, value);\
		if (!timeout--) {\
			rc = FT2_TIMEOUT;\
			break;\
		}\
	} while (!(value >> 7));\
} while(0)

ft2_ret_t ca_ft2_pon_serdes_cfg(
	serdes_test_mode_t mode)
{
	uint16_t i;
	serdes_cfg_t *cfg_tbl=NULL;

	switch (mode) {
		case SERDES_MOD_1:
			cfg_tbl = mode_1_pon_serdes_cfg;
			break;
		case SERDES_MOD_2:
			cfg_tbl = mode_2_pon_serdes_cfg;
			break;
		default:
			return FT2_ERROR;
	}

	for (i = 0; cfg_tbl[i].addr != 0; i++) {
		FT2_DBG("%s - %d: addr = 0x%08x, value = 0x%08x\n",
			__func__, __LINE__, cfg_tbl[i].addr, cfg_tbl[i].value);
		if (cfg_tbl[i].addr == 0xffffffff) {
			udelay(cfg_tbl[i].value);
			continue;
		}
		CA_REG_WRITE(cfg_tbl[i].value, cfg_tbl[i].addr);
	}

	return FT2_OK;
}

ft2_ret_t ca_ft2_xfi_serdes_cfg(
	serdes_test_mode_t mode)
{
	uint16_t i;
	serdes_cfg_t *cfg_tbl=NULL;

	if (mode == SERDES_MOD_1)
		cfg_tbl = mode_1_xfi_serdes_cfg;
#if (INTRA_XFI_PORT_MASK > 0)
	else if (mode == SERDES_MOD_INTRA_XFI)
		cfg_tbl = mode_1_intra_xfi_serdes_cfg;
#endif
	else
		return FT2_ERROR;

	for (i = 0; cfg_tbl[i].addr != 0; i++) {
		FT2_DBG("%s - %d: addr = 0x%08x, value = 0x%08x\n",
			__func__, __LINE__, cfg_tbl[i].addr, cfg_tbl[i].value);
		if (cfg_tbl[i].addr == 0xffffffff) {
			udelay(cfg_tbl[i].value);
			continue;
		}
		CA_REG_WRITE(cfg_tbl[i].value, cfg_tbl[i].addr);
	}

	return FT2_OK;
}

ft2_ret_t ca_ft2_serdes_cfg(uint8_t port, serdes_test_mode_t mode)
{
	ft2_ret_t rc = FT2_OK;

	/* config serdes */
	switch (port) {
		case CA_PORT_ID_NI7:
			if (FT2_OK != (rc = ca_ft2_pon_serdes_cfg(mode)))
				FT2_DBG("%s - %d: PON serdes config fail\n", __func__, __LINE__);
			break;
		case CA_PORT_ID_NI5:
		//case CA_PORT_ID_NI6:
			/* xfi_serdes_cfg usually includes port#5 and #6 */
			if (FT2_OK != (rc = ca_ft2_xfi_serdes_cfg(mode)))
				FT2_DBG("%s - %d: XFI serdes config fail\n", __func__, __LINE__);
			break;
		default:
			rc = FT2_ERROR;
	}
	return rc;
}

static ft2_ret_t ca_ft2_serdes_epon_test(ft2_record_t *record, uint8_t serdes_ports[])
{
	#define RETRY_TIMES 3
	ft2_ret_t rc = FT2_OK, rc1;
	uint32_t value, value1, dut_cnt_err[SERDES_PORT_NUM][RETRY_TIMES]={0};
	uint8_t i, j, port, data, gld_st_err[SERDES_PORT_NUM]={0};
	ca_status_t ret;

	/* Choose EPON RX 10.3125G/TX 10.3125G; XFI0 10.3125G; XFI1 10.3125G */
	FT2_DBG("%s - %d: Start Mode 1 EPON PRBS test\n", __func__, __LINE__);

	/* init GLD */
	for (i = 0; (port = serdes_ports[i]) != INVALID_PORT; i++) {
		if (port == NONE_PORT)
			continue;
		FT2_DBG("%s - %d: Init GLD %d (Port#%d)\n", __func__, __LINE__, i, port);
		record->entry_name[i] = serdes_port_name[i];
		record->entry_test[i] |= E_SERDES_EPON | E_SERDES_I2C;
		GLD_PRBS_INIT(gld_dev[i], rc1);
		if (rc1 != FT2_OK) {
			FT2_DBG("%s - %d: Init GLD %d fail, rc1 = %d\n", __func__, __LINE__, i, rc1);
			rc = FT2_SERDES;
			if (rc1 == FT2_I2C)
				record->entry_rlt[i] |= E_SERDES_I2C;
			else
				record->entry_rlt[i] |= E_SERDES_EPON;
		}
	}
	/* config GLD */
	for (i = 0; (port = serdes_ports[i]) != INVALID_PORT; i++) {
		if (port == NONE_PORT)
			continue;
		FT2_DBG("%s - %d: Config GLD %d (Port#%d) PRBS\n", __func__, __LINE__, i, port);
		GLD_WAIT_READY(gld_dev[i], value, rc1);
		if (rc1 != FT2_OK || value != 0x80) {
			FT2_DBG("%s - %d: Wait GLD %d ready, rc1 = %d, value = 0x%x\n", __func__, __LINE__, i, rc1, value);
			rc = FT2_SERDES;
			if (rc1 == FT2_I2C)
				record->entry_rlt[i] |= E_SERDES_I2C;
			else
				record->entry_rlt[i] |= E_SERDES_EPON;
		}
		data = 1;
		GLD_PRBS_CONFIG(gld_dev[i], data, rc1);
		if (rc1 != FT2_OK) {
			FT2_DBG("%s - %d: Config GLD %d PRBS fail, rc1 = %d\n", __func__, __LINE__, i, rc1);
			rc = FT2_SERDES;
			if (rc1 == FT2_I2C)
				record->entry_rlt[i] |= E_SERDES_I2C;
			else
				record->entry_rlt[i] |= E_SERDES_EPON;
		}
	}
	/* config DUT */
	for (i = 0; (port = serdes_ports[i]) != INVALID_PORT; i++) {
		if (port == NONE_PORT)
			continue;
		FT2_DBG("%s - %d: Config DUT %d (Port#%d) PRBS\n", __func__, __LINE__, i, port);
		GLD_WAIT_READY(gld_dev[i], value, rc1);
		if (rc1 != FT2_OK || value != 0x80) {
			FT2_DBG("%s - %d: Wait GLD %d ready, rc1 = %d, value = 0x%x\n", __func__, __LINE__, i, rc1, value);
			rc = FT2_SERDES;
			if (rc1 == FT2_I2C)
				record->entry_rlt[i] |= E_SERDES_I2C;
			else
				record->entry_rlt[i] |= E_SERDES_EPON;
		}
		ca_ft2_serdes_cfg(port, SERDES_MOD_1);
	}
	/* start GLD PRBS */
	for (i = 0; (port = serdes_ports[i]) != INVALID_PORT; i++) {
		if (port == NONE_PORT)
			continue;
		FT2_DBG("%s - %d: Start GLD %d (Port#%d) PRBS\n", __func__, __LINE__, i, port);
		data = 0x10;
		GLD_PRBS_START(gld_dev[i], data, rc1);
		if (rc1 != FT2_OK) {
			FT2_DBG("%s - %d: Start GLD %d PRBS fail, rc1 = %d\n", __func__, __LINE__, i, rc1);
			rc = FT2_SERDES;
			if (rc1 == FT2_I2C)
				record->entry_rlt[i] |= E_SERDES_I2C;
			else
				record->entry_rlt[i] |= E_SERDES_EPON;
		}
	}
	/* get and check GLD/DUT counters */
	for (i = 0; (port = serdes_ports[i]) != INVALID_PORT; i++) {
		if (port == NONE_PORT)
			continue;
		FT2_DBG("%s - %d: Get GLD/DUT %d (Port#%d) counter\n", __func__, __LINE__, i, port);
		GLD_PRBS_STATUS_GET(gld_dev[i], value, rc1);
		if (rc1 != FT2_OK || (value & 0x3) != 2) {
			FT2_DBG("%s - %d: Get GLD %d status, rc1 = %d, data = 0x%x\n", __func__, __LINE__, i, rc1, value);
			gld_st_err[i] = 1;
		} else
			gld_st_err[i] = 0;

		/* get and check DUT counter */
		for (j = 0; j < RETRY_TIMES; j++) {
			/* check dut prbs counter */
			if (port == CA_PORT_ID_NI7) {
				value = CORTINA_CA_REG_READ(PSDS_PRBS_ERR_CNT_L);
				value1 = CORTINA_CA_REG_READ(PSDS_PRBS_ERR_CNT_H);
				dut_cnt_err[i][j] = value | value1;
				/* clear dut pon prbs counter */
				value = CORTINA_CA_REG_READ(PSDS_PRBS_CTRL);
				CORTINA_CA_REG_WRITE(value | 0x800, PSDS_PRBS_CTRL);
				CORTINA_CA_REG_WRITE(value, PSDS_PRBS_CTRL);
			}
			if (port == CA_PORT_ID_NI5)
				dut_cnt_err[i][j] = CORTINA_CA_REG_READ(NI_HV_SXGMII_PCS_PCSTSTPTRNERRCNTR);
			if (port == CA_PORT_ID_NI6)
				dut_cnt_err[i][j] = CORTINA_CA_REG_READ(NI_HV_SXGMII_PCS_PCSTSTPTRNERRCNTR + APB0_NI_HV_SXGMII_STRIDE);
			udelay(100000);
		}

		FT2_DBG("%s - %d: Get GLD/DUT %d counter [%d, %d, %d]\n",
			__func__, __LINE__, i, gld_st_err[i], dut_cnt_err[i][0], dut_cnt_err[i][RETRY_TIMES-1]);
		if ((gld_st_err[i] == 0) && (dut_cnt_err[i][0] != 0) && (dut_cnt_err[i][RETRY_TIMES-1] == 0)) {
			FT2_MOD("%s - %d: Get GLD/DUT %d PRBS PASS\n", __func__, __LINE__, i);
		} else {
			FT2_MOD("%s - %d: Get GLD/DUT %d PRBS FAIL\n", __func__, __LINE__, i);
			record->entry_rlt[i] |= E_SERDES_EPON;
			rc = FT2_SERDES;
		}
	}

	/* stop GLD/DUT PRBS */
	for (i = 0; (port = serdes_ports[i]) != INVALID_PORT; i++) {
		if (port == NONE_PORT)
			continue;
		FT2_DBG("%s - %d: STOP GLD/DUT %d (Port#%d) PRBS\n", __func__, __LINE__, i, port);
		GLD_PRBS_STOP(gld_dev[i], rc1);
		/* disable dut pon prbs */
		if (port == CA_PORT_ID_NI7) {
			CORTINA_CA_REG_WRITE(0, PSDS_PRBS_CTRL);
			CORTINA_CA_REG_WRITE(0x50, PSDS_GBOX_CTRL);
		}
		/* disable DUT XFI0 prbs */
		if (port == CA_PORT_ID_NI5) {
			CORTINA_CA_REG_WRITE(0, NI_HV_SXGMII_PCS_PCSTSTPTRNCTL);
			CORTINA_CA_REG_WRITE(0x2, NI_HV_SXGMII_PCS_PCSREG32768);
		}
		/* disable DUT XFI1 prbs */
		if (port == CA_PORT_ID_NI6) {
			CORTINA_CA_REG_WRITE(0, NI_HV_SXGMII_PCS_PCSTSTPTRNCTL + APB0_NI_HV_SXGMII_STRIDE);
			CORTINA_CA_REG_WRITE(0x2, NI_HV_SXGMII_PCS_PCSREG32768 + APB0_NI_HV_SXGMII_STRIDE);
		}
	}

	return rc;
}

static ft2_ret_t ca_ft2_serdes_gpon_test(ft2_record_t *record, uint8_t serdes_ports[])
{
	#define RETRY_TIMES 3
	ft2_ret_t rc = FT2_OK, rc1;
	uint32_t value, value1, dut_cnt_err[SERDES_PORT_NUM][RETRY_TIMES]={0};
	uint8_t i, j, port, data, gld_st_err[SERDES_PORT_NUM]={0};
	ca_status_t ret;

	/* Choose GPON RX 9.98325G/TX 9.98325G; XFI0 3.125G; XFI1 3.125G */
	FT2_DBG("%s - %d: Start Mode 2 GPON PRBS test\n", __func__, __LINE__);
	/* init GLD */
	for (i = 0; (port = serdes_ports[i]) != INVALID_PORT; i++) {
		if (port == NONE_PORT)
			continue;
		FT2_DBG("%s - %d: Init GLD %d (Port#%d)\n", __func__, __LINE__, i, port);
		record->entry_name[i] = serdes_port_name[i];
		record->entry_test[i] |= E_SERDES_GPON | E_SERDES_I2C;
		GLD_PRBS_INIT(gld_dev[i], rc1);
		if (rc1 != FT2_OK) {
			FT2_DBG("%s - %d: Init GLD %d fail, rc1 = %d\n", __func__, __LINE__, i, rc1);
			rc = FT2_SERDES;
			if (rc1 == FT2_I2C)
				record->entry_rlt[i] |= E_SERDES_I2C;
			else
				record->entry_rlt[i] |= E_SERDES_GPON;
		}
	}
	/* config GLD */
	for (i = 0; (port = serdes_ports[i]) != INVALID_PORT; i++) {
		if (port == NONE_PORT)
			continue;
		FT2_DBG("%s - %d: Config GLD %d (Port#%d) PRBS\n", __func__, __LINE__, i, port);
		GLD_WAIT_READY(gld_dev[i], value, rc1);
		if (rc1 != FT2_OK || value != 0x80) {
			FT2_DBG("%s - %d: Wait GLD %d ready, rc1 = %d, value = 0x%x\n", __func__, __LINE__, i, rc1, value);
			rc = FT2_SERDES;
			if (rc1 == FT2_I2C)
				record->entry_rlt[i] |= E_SERDES_I2C;
			else
				record->entry_rlt[i] |= E_SERDES_GPON;
		}
		data = 2;
		GLD_PRBS_CONFIG(gld_dev[i], data, rc1);
		if (rc1 != FT2_OK) {
			FT2_DBG("%s - %d: Config GLD %d PRBS fail, rc1 = %d\n", __func__, __LINE__, i, rc1);
			rc = FT2_SERDES;
			if (rc1 == FT2_I2C)
				record->entry_rlt[i] |= E_SERDES_I2C;
			else
				record->entry_rlt[i] |= E_SERDES_GPON;
		}
	}
	/* config DUT */
	for (i = 0; (port = serdes_ports[i]) != INVALID_PORT; i++) {
		if (port == NONE_PORT)
			continue;
		FT2_DBG("%s - %d: Config DUT %d (Port#%d) PRBS\n", __func__, __LINE__, i, port);
		GLD_WAIT_READY(gld_dev[i], value, rc1);
		if (rc1 != FT2_OK || value != 0x80) {
			FT2_DBG("%s - %d: Wait GLD %d ready, rc1 = %d, value = 0x%x\n", __func__, __LINE__, i, rc1, value);
			rc = FT2_SERDES;
			if (rc1 == FT2_I2C)
				record->entry_rlt[i] |= E_SERDES_I2C;
			else
				record->entry_rlt[i] |= E_SERDES_GPON;
		}
		ca_ft2_serdes_cfg(port, SERDES_MOD_2);
	}
	/* start GLD PRBS */
	for (i = 0; (port = serdes_ports[i]) != INVALID_PORT; i++) {
		if (port == NONE_PORT)
			continue;
		FT2_DBG("%s - %d: Start GLD %d (Port#%d) PRBS\n", __func__, __LINE__, i, port);
		data = 0x10;
		GLD_PRBS_START(gld_dev[i], data, rc1);
		if (rc1 != FT2_OK) {
			FT2_DBG("%s - %d: Start GLD %d PRBS fail, rc1 = %d\n", __func__, __LINE__, i, rc1);
			rc = FT2_SERDES;
			if (rc1 == FT2_I2C)
				record->entry_rlt[i] |= E_SERDES_I2C;
			else
				record->entry_rlt[i] |= E_SERDES_GPON;
		}
	}
	/* get and check GLD/DUT counters */
	for (i = 0; (port = serdes_ports[i]) != INVALID_PORT; i++) {
		if (port == NONE_PORT)
			continue;
		FT2_DBG("%s - %d: Get GLD/DUT %d (Port#%d) counter\n", __func__, __LINE__, i, port);
		GLD_PRBS_STATUS_GET(gld_dev[i], value, rc1);
		if (rc1 != FT2_OK || (value & 0x3) != 2) {
			FT2_DBG("%s - %d: Get GLD %d status, rc1 = %d, data = 0x%x\n", __func__, __LINE__, i, rc1, value);
			gld_st_err[i] = 1;
		} else
			gld_st_err[i] = 0;

		/* get and check DUT counter */
		for (j = 0; j < RETRY_TIMES; j++) {
			/* check dut prbs counter */
			if (port == CA_PORT_ID_NI7) {
				value = CORTINA_CA_REG_READ(PSDS_PRBS_ERR_CNT_L);
				value1 = CORTINA_CA_REG_READ(PSDS_PRBS_ERR_CNT_H);
				dut_cnt_err[i][j] = value | value1;
				/* clear dut pon prbs counter */
				value = CORTINA_CA_REG_READ(PSDS_PRBS_CTRL);
				CORTINA_CA_REG_WRITE(value | 0x800, PSDS_PRBS_CTRL);
				CORTINA_CA_REG_WRITE(value, PSDS_PRBS_CTRL);
			}
			if (port == CA_PORT_ID_NI5)
				dut_cnt_err[i][j] = CORTINA_CA_REG_READ(NI_HV_SXGMII_PCS_PCSTSTPTRNERRCNTR);
			if (port == CA_PORT_ID_NI6)
				dut_cnt_err[i][j] = CORTINA_CA_REG_READ(NI_HV_SXGMII_PCS_PCSTSTPTRNERRCNTR + APB0_NI_HV_SXGMII_STRIDE);
			udelay(100000);
		}

		FT2_DBG("%s - %d: Get GLD/DUT %d counter [%d, %d, %d]\n",
			__func__, __LINE__, i, gld_st_err[i], dut_cnt_err[i][0], dut_cnt_err[i][RETRY_TIMES-1]);
		if ((gld_st_err[i] == 0) && (dut_cnt_err[i][0] != 0) && (dut_cnt_err[i][RETRY_TIMES-1] == 0)) {
			FT2_MOD("%s - %d: Get GLD/DUT %d PRBS PASS\n", __func__, __LINE__, i);
		} else {
			FT2_MOD("%s - %d: Get GLD/DUT %d PRBS FAIL\n", __func__, __LINE__, i);
			record->entry_rlt[i] |= E_SERDES_GPON;
			rc = FT2_SERDES;
		}
	}

	/* stop GLD/DUT PRBS */
	for (i = 0; (port = serdes_ports[i]) != INVALID_PORT; i++) {
		if (port == NONE_PORT)
			continue;
		FT2_DBG("%s - %d: STOP GLD/DUT %d (Port#%d) PRBS\n", __func__, __LINE__, i, port);
		GLD_PRBS_STOP(gld_dev[i], rc1);
		/* disable dut pon prbs */
		if (port == CA_PORT_ID_NI7) {
			CORTINA_CA_REG_WRITE(0, PSDS_PRBS_CTRL);
			CORTINA_CA_REG_WRITE(0x50, PSDS_GBOX_CTRL);
		}
		/* disable DUT XFI0 prbs */
		if (port == CA_PORT_ID_NI5) {
			CORTINA_CA_REG_WRITE(0, NI_HV_SXGMII_PCS_PCSTSTPTRNCTL);
			CORTINA_CA_REG_WRITE(0x2, NI_HV_SXGMII_PCS_PCSREG32768);
		}
		/* disable DUT XFI1 prbs */
		if (port == CA_PORT_ID_NI6) {
			CORTINA_CA_REG_WRITE(0, NI_HV_SXGMII_PCS_PCSTSTPTRNCTL + APB0_NI_HV_SXGMII_STRIDE);
			CORTINA_CA_REG_WRITE(0x2, NI_HV_SXGMII_PCS_PCSREG32768 + APB0_NI_HV_SXGMII_STRIDE);
		}
	}

	return rc;
}
#endif

ft2_ret_t ca_ft2_serdes_test(ft2_record_t *record)
{
	ft2_ret_t rc = FT2_OK;

#if (SERDES_PORT_MASK > 0)

	printf("FT2_PARAM_SERDES test ...\n");

	record->entry_mask = SERDES_PORT_MASK;
	record->start = get_timer(0);

	if (mod_1_serdes_ports[0] != INVALID_PORT) {
		if (FT2_OK != ca_ft2_serdes_epon_test(record, mod_1_serdes_ports))
			rc = FT2_SERDES;
	}
	if (mod_2_serdes_ports[0] != INVALID_PORT) {
		if (FT2_OK != ca_ft2_serdes_gpon_test(record, mod_2_serdes_ports))
			rc = FT2_SERDES;
	}

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return rc;
#else
	printf("No SERDES test port\n");
	return FT2_SERDES;
#endif
}

ft2_ret_t ca_ft2_pon_ben_test(ft2_record_t *record)
{
	ft2_ret_t rc = FT2_OK;
	uint8_t value, value1;

#if (PON_BEN_MASK > 0)

	printf("FT2_PARAM_PON_BEN test ...\n");

	record->entry_mask = PON_BEN_MASK;
	record->start = get_timer(0);
	record->entry_test[0] = 1;

	/* set gpio mux */
	ca_ft2_gpio_mux_set(GPIO_PON_BEN);

	/* Turn on BEN output */
	CORTINA_CA_REG_WRITE(0x31, CLKGEN_PSDS_INIT_CNTL);
	udelay(10000);
	gpio_direction_input(GPIO_PON_BEN);
	value = gpio_get_value(GPIO_PON_BEN);
	FT2_DBG("%s: Turn on BEN output, value = %d\n", __func__, value);

	/* Change BEN */
	CORTINA_CA_REG_WRITE(0x8050, PSDS_GBOX_CTRL);
	udelay(10000);
	gpio_direction_input(GPIO_PON_BEN);
	value = gpio_get_value(GPIO_PON_BEN);
	FT2_DBG("%s: Change BEN, value = %d\n", __func__, value);

	/* Change BEN */
	CORTINA_CA_REG_WRITE(0x0050, PSDS_GBOX_CTRL);
	udelay(10000);
	gpio_direction_input(GPIO_PON_BEN);
	value1 = gpio_get_value(GPIO_PON_BEN);
	if (value == value1) {
		FT2_DBG("%s: Change BEN again, value = %d\n", __func__, value);
		rc = FT2_PON_BEN;
		goto End;
	}

End:
	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;
	record->entry_rlt[0] = rc ? 1 : 0;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return rc;
#else
	printf("No PON BEN test\n");
	return FT2_PON_BEN;
#endif
}

#if (PON_SRAM_MASK > 0)
#define PON_SRAM_TIMEOUT_CNT 10

#define PON_REG_CONTROL_0(addr, value, rc) {\
	uint32_t timeout=PON_SRAM_TIMEOUT_CNT;\
	CORTINA_CA_REG_WRITE(value, addr);\
	do {\
		value = CORTINA_CA_REG_READ(addr);\
		if (!timeout--) {\
			rc = FT2_TIMEOUT;\
			break;\
		}\
	} while (value & 0x1);\
} while(0)

#define PON_REG_CONTROL_31(addr, value, rc) {\
	uint32_t timeout=PON_SRAM_TIMEOUT_CNT;\
	CORTINA_CA_REG_WRITE(value, addr);\
	do {\
		value = CORTINA_CA_REG_READ(addr);\
		if (!timeout--) {\
			rc = FT2_TIMEOUT;\
			break;\
		}\
	} while (value >> 31);\
} while(0)

ft2_ret_t ca_ft2_epon_xgepn_coresec10_top_nandi_sadb_test(
	uint8_t mem_idx,
	uint16_t said_cnt)
{
	ft2_ret_t rc = 0;
	uint32_t reg, value;
	uint32_t addr, cmd_addr, data_addr, ctrl_addr, int_addr_0, vld_addr, en_int_addr_0, en_int_addr_1, int_addr_1;
	int32_t loop;

	FT2_MOD("Start %s\n", __func__);

	if (mem_idx == 1) {
		vld_addr = EPON_XGEPN_CORESEC10_TOP_NANDI_DATAFIELD_VALID + APB1_EPON_XGEPN_CORESEC10_TOP_STRIDE;
		cmd_addr = EPON_XGEPN_CORESEC10_TOP_NANDI_COMMAND_CONTROL + APB1_EPON_XGEPN_CORESEC10_TOP_STRIDE;
		data_addr = EPON_XGEPN_CORESEC10_TOP_NANDI_SADB_KEY0_3 + APB1_EPON_XGEPN_CORESEC10_TOP_STRIDE;
		int_addr_0 = EPON_XGEPN_CORESEC10_TOP_NANDI_INTERRUPT_0 + APB1_EPON_XGEPN_CORESEC10_TOP_STRIDE;
		en_int_addr_0 = EPON_XGEPN_CORESEC10_TOP_NANDI_INTENABLE_0 + APB1_EPON_XGEPN_CORESEC10_TOP_STRIDE;
		int_addr_1 = EPON_XGEPN_CORESEC10_TOP_NANDI_INTERRUPT_1 + APB1_EPON_XGEPN_CORESEC10_TOP_STRIDE;
		en_int_addr_1 = EPON_XGEPN_CORESEC10_TOP_NANDI_INTENABLE_1 + APB1_EPON_XGEPN_CORESEC10_TOP_STRIDE;
		ctrl_addr = EPON_XGEPN_CORESEC10_TOP_NANDI_SADB_Control + APB1_EPON_XGEPN_CORESEC10_TOP_STRIDE;
	} else {
		vld_addr = EPON_XGEPN_CORESEC10_TOP_NANDI_DATAFIELD_VALID;
		cmd_addr = EPON_XGEPN_CORESEC10_TOP_NANDI_COMMAND_CONTROL;
		data_addr = EPON_XGEPN_CORESEC10_TOP_NANDI_SADB_KEY0_3;
		int_addr_0 = EPON_XGEPN_CORESEC10_TOP_NANDI_INTERRUPT_0;
		en_int_addr_0 = EPON_XGEPN_CORESEC10_TOP_NANDI_INTENABLE_0;
		int_addr_1 = EPON_XGEPN_CORESEC10_TOP_NANDI_INTERRUPT_1;
		en_int_addr_1 = EPON_XGEPN_CORESEC10_TOP_NANDI_INTENABLE_1;
		ctrl_addr = EPON_XGEPN_CORESEC10_TOP_NANDI_SADB_Control;
	}

	CORTINA_CA_REG_WRITE(0x3ff, vld_addr);
	value = CORTINA_CA_REG_READ(int_addr_0);
	if (value)
		CORTINA_CA_REG_WRITE(0xff, int_addr_0);

	CORTINA_CA_REG_WRITE(0, en_int_addr_0);
	value = CORTINA_CA_REG_READ(int_addr_1);
	if (value)
		CORTINA_CA_REG_WRITE(0xff, int_addr_1);

	CORTINA_CA_REG_WRITE(0, en_int_addr_1);

	/* fill the memory */
	for (loop=0; loop<said_cnt; loop++) {
		for (reg=0; reg<18; reg++) {
			/* fill the register data */
			addr = data_addr+4*reg;
			value = 0xa5a5a5a5+loop+reg;
			CORTINA_CA_REG_WRITE(value, addr);
		}
		value = 0x3+loop*4;
		PON_REG_CONTROL_0(cmd_addr, value, rc);
	}

	FT2_DBG("%s - %d: rc = %d\n", __func__, __LINE__, rc);

	/* read the memory, fill with different data */
	for (loop=0; loop<said_cnt; loop++) {
		value = 0x1+loop*4;
		PON_REG_CONTROL_0(cmd_addr, value, rc);

		for (reg=0; reg<18; reg++) {
			addr = data_addr+4*reg;
			if (reg == 17) {
				/* Only 1 entry is needed to compare, others are protected by ECC */
				value = CORTINA_CA_REG_READ(addr);
				if (value != (0xa5a5a5a5+loop+reg)) {
					rc = FT2_PON_SRAM;
					FT2_DBG("%s - %d: loop = %d, reg = %d, rc = %d, addr=0x%x, value=0x%x\n",
						__func__, __LINE__, loop, reg, rc, addr, value);
					goto End;
				}
			}
			value = ~(0xa5a5a5a5+loop+reg);
			CORTINA_CA_REG_WRITE(value, addr);
		}

		value = ~(0xa5a5a5a5+loop+18);
		CORTINA_CA_REG_WRITE(value, ctrl_addr);

		value = 0x3+loop*4;
		PON_REG_CONTROL_0(cmd_addr, value, rc);
	}

	FT2_DBG("%s - %d: rc = %d\n", __func__, __LINE__, rc);

	/* read the memory */
	for (loop=(said_cnt-1); loop>=0; loop--) {
		value = 0x1+loop*4;
		PON_REG_CONTROL_0(cmd_addr, value, rc);

		for (reg=17; reg<18; reg++) {
			/* Only 1 entry is needed to compare, others are protected by ECC */
			addr = data_addr+4*reg;
			value = CORTINA_CA_REG_READ(addr);
			if (value != ~(0xa5a5a5a5+loop+reg)) {
				rc = FT2_PON_SRAM;
				FT2_DBG("%s - %d: loop = %d, reg = %d, rc = %d, addr=0x%x, value=0x%x\n",
					__func__, __LINE__, loop, reg, rc, addr, value);
				goto End;
			}
		}
	}

	value = CORTINA_CA_REG_READ(int_addr_0);
	if (value >> 2) {
		rc = FT2_PON_SRAM;
		FT2_DBG("%s - %d: rc = %d\n", __func__, __LINE__, rc);
		goto End;
	}
	FT2_DBG("%s - %d: rc = %d\n", __func__, __LINE__, rc);

End:
	FT2_MOD("%s\n", rc ? "FAIL" : "PASS");

	return rc;
}

ft2_ret_t ca_ft2_pon_mib_init(
	uint64_t addr,
	uint16_t mem_loc_cnt)
{
	ft2_ret_t rc = FT2_OK;
	uint32_t value;
	int32_t loop;

	/* Workaround for HW issue in EPON Coresec10 EN memories 256x150 */
	for (loop=65; loop<mem_loc_cnt; loop++) {
		value = 0x80000000+loop;
		PON_REG_CONTROL_31(addr, value, rc);
	}

	return rc;
}

ft2_ret_t ca_ft2_epon_xgepn_coresec10_top_coresec10_tx_rx_mib_mem_test(
	uint8_t mem_idx,
	uint16_t mem_loc_cnt)
{
	ft2_ret_t rc = FT2_OK;
	uint32_t reg, value;
	uint32_t addr, cmd_addr, data_addr;
	int32_t loop;

	FT2_MOD("Start %s\n", __func__);

	if (mem_idx == 1) {
		cmd_addr = EPON_XGEPN_CORESEC10_TOP_CORESEC10_TX_RX_MIB_MEM_ACCESS + APB1_EPON_XGEPN_CORESEC10_TOP_STRIDE;
		data_addr = EPON_XGEPN_CORESEC10_TOP_CORESEC10_TX_RX_MIB_MEM_DATA10 + APB1_EPON_XGEPN_CORESEC10_TOP_STRIDE;
	} else {
		cmd_addr = EPON_XGEPN_CORESEC10_TOP_CORESEC10_TX_RX_MIB_MEM_ACCESS;
		data_addr = EPON_XGEPN_CORESEC10_TOP_CORESEC10_TX_RX_MIB_MEM_DATA10;
	}

	/* Workaround for HW issue in EPON Coresec10 EN memories 256x150 */
	ca_ft2_pon_mib_init(cmd_addr, mem_loc_cnt);

	/* EPON Coresec10 EN memories 256x150, read only */
	for (loop=0; loop<mem_loc_cnt; loop++) {
		value = 0x80000000+loop;
		PON_REG_CONTROL_31(cmd_addr, value, rc);

		for (reg=0; reg<11; reg++) {
			addr = data_addr+4*reg;
			value = CORTINA_CA_REG_READ(addr);
			if (value) {
				rc = FT2_PON_SRAM;
				FT2_DBG("%s - %d: loop = %d, reg = %d, rc = %d, addr=0x%x, value=0x%x\n",
					__func__, __LINE__, loop, reg, rc, addr, value);
				goto End;
			}
		}
	}
	FT2_DBG("%s - %d: rc = %d\n", __func__, __LINE__, rc);

End:
	FT2_MOD("%s\n", rc ? "FAIL" : "PASS");

	return rc;
}

ft2_ret_t ca_ft2_onu_aes_dec_key_tbl_test(void)
{
	ft2_ret_t rc = FT2_OK;
	uint32_t reg, value;
	uint32_t addr;
	int32_t loop;

	FT2_MOD("Start %s\n", __func__);

	/* fill the memory */
	for (loop=0; loop<16; loop++) {
		for (reg=0; reg<4; reg++) {
			/* fill the register data */
			addr = EPON_ONU_EPON_AES_DEC_KEY_TBL_DATA3+4*reg;
			value = 0xa5a5a5a5+loop+reg;
			CORTINA_CA_REG_WRITE(value, addr);
		}
		value = 0xc0000000+loop;
		PON_REG_CONTROL_31(EPON_ONU_EPON_AES_DEC_KEY_TBL_ACCESS, value, rc);
	}

	FT2_DBG("%s - %d: rc = %d\n", __func__, __LINE__, rc);

	/* read the memory, fill with different data */
	for (loop=0; loop<16; loop++) {
		value = 0x80000000+loop;
		PON_REG_CONTROL_31(EPON_ONU_EPON_AES_DEC_KEY_TBL_ACCESS, value, rc);

		for (reg=0; reg<4; reg++) {
			addr = EPON_ONU_EPON_AES_DEC_KEY_TBL_DATA3+4*reg;
			value = CORTINA_CA_REG_READ(addr);
			if (value != (0xa5a5a5a5+loop+reg)) {
				rc = FT2_PON_SRAM;
				FT2_DBG("%s - %d: loop = %d, reg = %d, rc = %d, addr=0x%x, value=0x%x\n",
					__func__, __LINE__, loop, reg, rc, addr, value);
				goto End;
			}
			value = ~(0xa5a5a5a5+loop+reg);
			CORTINA_CA_REG_WRITE(value, addr);
		}

		value = 0xc0000000+loop;
		PON_REG_CONTROL_31(EPON_ONU_EPON_AES_DEC_KEY_TBL_ACCESS, value, rc);
	}

	FT2_DBG("%s - %d: rc = %d\n", __func__, __LINE__, rc);

	/* read the memory */
	for (loop=15; loop>=0; loop--) {
		value = 0x80000000+loop;
		PON_REG_CONTROL_31(EPON_ONU_EPON_AES_DEC_KEY_TBL_ACCESS, value, rc);

		for (reg=0; reg<4; reg++) {
			addr = EPON_ONU_EPON_AES_DEC_KEY_TBL_DATA3+4*reg;
			value = CORTINA_CA_REG_READ(addr);
			if (value != ~(0xa5a5a5a5+loop+reg)) {
				rc = FT2_PON_SRAM;
				FT2_DBG("%s - %d: loop = %d, reg = %d, rc = %d, addr=0x%x, value=0x%x\n",
					__func__, __LINE__, loop, reg, rc, addr, value);
				goto End;
			}
		}
	}
	FT2_DBG("%s - %d: rc = %d\n", __func__, __LINE__, rc);

End:
	FT2_MOD("%s\n", rc ? "FAIL" : "PASS");

	return rc;
}
#endif

ft2_ret_t ca_ft2_pon_sram_test(ft2_record_t *record)
{
	ft2_ret_t rc = FT2_OK;

#if (PON_SRAM_MASK > 0)

	printf("FT2_PARAM_PON_SRAM test ...\n");

	record->start = get_timer(0);
	record->entry_mask = PON_SRAM_MASK;
	record->entry_test[0] |= TEST_PON_SRAM;

#if defined(CONFIG_CA8279_FT2)
	CA_REG_WRITE(0x30f, GLOBAL_PON_CNTL);
	CA_REG_WRITE(0x30f6f, GLOBAL_EPON_CNTL);
#endif

	/* pon sram test only in epon mode */
	if (FT2_OK != ca_ft2_pon_serdes_cfg(SERDES_MOD_1))
		FT2_DBG("%s - %d: PON serdes config fail\n", __func__, __LINE__);

	if (record->entry_test[0] & E_PON_SRAM_DN_NANDI_SADB) {
		if (FT2_OK != ca_ft2_epon_xgepn_coresec10_top_nandi_sadb_test(0, 64)) {
			record->entry_rlt[0] |= E_PON_SRAM_DN_NANDI_SADB;
			rc = FT2_PON_SRAM;
		}
	}

	if (record->entry_test[0] & E_PON_SRAM_UP_NANDI_SADB) {
		if (FT2_OK != ca_ft2_epon_xgepn_coresec10_top_nandi_sadb_test(1, 32)) {
			record->entry_rlt[0] |= E_PON_SRAM_UP_NANDI_SADB;
			rc = FT2_PON_SRAM;
		}
	}

	if (record->entry_test[0] & E_PON_SRAM_DN_TX_RX_MIB) {
		if (FT2_OK != ca_ft2_epon_xgepn_coresec10_top_coresec10_tx_rx_mib_mem_test(0, 256)) {
			record->entry_rlt[0] |= E_PON_SRAM_DN_TX_RX_MIB;
			rc = FT2_PON_SRAM;
		}
	}

	if (record->entry_test[0] & E_PON_SRAM_UP_TX_RX_MIB) {
		if (FT2_OK != ca_ft2_epon_xgepn_coresec10_top_coresec10_tx_rx_mib_mem_test(1, 128)) {
			record->entry_rlt[0] |= E_PON_SRAM_UP_TX_RX_MIB;
			rc = FT2_PON_SRAM;
		}
	}

	if (record->entry_test[0] & E_PON_SRAM_AES_DEC_KEY) {
		if (FT2_OK != ca_ft2_onu_aes_dec_key_tbl_test()) {
			record->entry_rlt[0] |= E_PON_SRAM_AES_DEC_KEY;
			rc = FT2_PON_SRAM;
		}
	}

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return rc;
#else
		printf("No PON SRAM test\n");
		return FT2_PON_SRAM;
#endif
}

ft2_ret_t  ca_ft2_test_pkt_send(uint16_t pkt_cnt)
{
	uint8_t dst_mac[6] = {0x0, 0x0, 0x0, 0xaa,0xbb, 0xcc}, pkt[128];
	uint16_t i;

	//add new define for out environment
	static struct in_addr addr;
	addr.s_addr = 0;
	///////

	printf("%s\n", __func__);

	memset(pkt, 0, 128);
	for (i=0; i < pkt_cnt; i++)
		//NetSendUDPPacket(dst_mac, 0, 555, 666, 128);
		net_send_udp_packet(dst_mac, addr, 555, 666, 128);

	return FT2_OK;
}

ft2_ret_t ca_ft2_gmii_loopback(ft2_record_t *record)
{
#define LOOPBACK_PKT_CNT	1000

	uint8_t i, port;
	ft2_ret_t rc = FT2_OK;
	NI_HV_GLB_STATIC_CFG_t cfg;
	NI_HV_PT_PORT_STATIC_CFG_t port_cfg;
	NI_HV_PT_PORT_GLB_CFG_t port_glb;
	ca_uint32_t rx_stats[__NI_MIB_RX_NUM];
	ca_uint32_t tx_stats[__NI_MIB_TX_NUM];

	printf("FT2_PARAM_GMII_PORT_LOOPBACK test ...\n");

	record->start = get_timer(0);
	record->entry_mask = GMII_PORT_MASK;

	/* release port reset */
	CORTINA_CA_REG_WRITE(0, NI_HV_GLB_INTF_RST_CONFIG);

	/* just init ether */
	//NetLoop(FT2TEST);
	net_loop(FT2TEST);

	/* set mac loopback */
	for (i = 0; (port = gmii_ports[i]) != INVALID_PORT; i++) {
		if (!(GMII_PORT_MASK & (1 << i)))
			continue;

		record->entry_test[i] = 1;
		/* all port ingress goes to L2FE */
		cfg.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_STATIC_CFG);
		cfg.bf.port_to_cpu = port;
		CORTINA_CA_REG_WRITE(cfg.wrd, NI_HV_GLB_STATIC_CFG);
		/* set to  GMII mode */
		port_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		port_cfg.bf.int_cfg = 0;
		port_cfg.bf.mac_addr6 = 0x2;
		/* enable  mac loopback: Ethernet Tx-to-Rx (Deep) Loopback (after MAC) */
		port_cfg.bf.lpbk_mode = 2;
		CORTINA_CA_REG_WRITE(port_cfg.wrd, (NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		/* set speed/duplex */
		port_glb.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * port));
		port_glb.bf.duplex = 1;
		CORTINA_CA_REG_WRITE(port_glb.wrd, (NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * port));

		/* wait port link state change */
		udelay(10000);

		/* clear mib counter */
		ni_eth_port_rx_mib_get(port, 1, rx_stats);
		ni_eth_port_tx_mib_get(port, 1, tx_stats);

		/* send test packets */
		ca_ft2_test_pkt_send(LOOPBACK_PKT_CNT);

		/* read mib counter */
		ni_eth_port_rx_mib_get(port, 0, rx_stats);
		ni_eth_port_tx_mib_get(port, 0, tx_stats);

		/* check counters */
		if (rx_stats[__NI_MIB_RX_STATS_FRM_128_TO_255_OCT] < LOOPBACK_PKT_CNT || tx_stats[__NI_MIB_TX_STATS_FRM_128_TO_255_OCT] < LOOPBACK_PKT_CNT ||
			rx_stats[__NI_MIB_RX_CRC_ERR_FRM_CNT] != 0) {
			printf("%s: Port %d loopback fail: tx_frame_128_to_255_cnt = %d, rx_frame_128_to_255_cnt = %d, rx_fcs_error_frames = %d\n",
				__func__, port, tx_stats[__NI_MIB_TX_STATS_FRM_128_TO_255_OCT], rx_stats[__NI_MIB_RX_STATS_FRM_128_TO_255_OCT],
				rx_stats[__NI_MIB_RX_CRC_ERR_FRM_CNT]);
			rc = FT2_ERROR;
		}

		/* disable  mac loopback: Ethernet Tx-to-Rx (Deep) Loopback (after MAC) */
		port_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		port_cfg.bf.lpbk_mode = 0;
		CORTINA_CA_REG_WRITE(port_cfg.wrd, (NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));

		record->entry_rlt[i] = rc ? 1 : 0;
	}

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return rc;
}

ft2_ret_t ca_ft2_gmii_open_loopback(ft2_record_t *record)
{
#define CNT	100

	uint8_t i, port, phy_dev, reg;
	ft2_ret_t rc = FT2_OK;
	uint16_t page, value;
	int16_t cnt;

	printf("FT2_PARAM_GMII_PORT_OPEN_LOOPBACK test ...\n");

	record->start = get_timer(0);
	record->entry_mask = GMII_PORT_MASK;

	/* set mac loopback */
	for (i = 0; (port = gmii_ports[i]) != INVALID_PORT; i++) {
		if (!(GMII_PORT_MASK & (1 << i)))
			continue;
		FT2_MOD("GPHY Port %d: \n", port);
		record->entry_test[i] = 1;
		phy_dev = phy_addr[port];

		/*
		check gphy phy_status = 2'b11 (LAN_ON), status =LAN_ON
		can run open_loopback test
		check page.0x0a42 reg.16[2:0] = 2'b11"
		*/
		page = 0x0a42;
		reg = 16;
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, page);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 3, &value);
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
		if (value != 0x3)
			FT2_DBG("%s: page = 0x%04x, reg = 0x%02x (%d), phy_status = 0x%x fail\n", __func__, page, reg, reg, value);

		/*
		write rg_dly_linkok10 = 1'b0
		page.0x0a4e reg.16[11] = 1'b0"
		*/
		page = 0x0a4e;
		reg = 16;
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, page);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
		FT2_DBG("%s: page = 0x%04x, reg = 0x%02x (%d), value = 0x%x\n", __func__, page, reg, reg, value);
		ca_ft2_phy_regfield_write(phy_dev, reg, 11, 1, 0);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
		FT2_DBG("%s: page = 0x%04x, reg = 0x%02x (%d), value = 0x%x\n", __func__, page, reg, reg, value);
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		/*
		write rg_en_idletimer_before_chest = 1'b0
		page.0x0a84 reg.19[4] = 1'b0"
		*/
		page = 0x0a84;
		reg = 19;
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, page);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
		FT2_DBG("%s: page = 0x%04x, reg = 0x%02x (%d), value = 0x%x\n", __func__, page, reg, reg, value);
		ca_ft2_phy_regfield_write(phy_dev, reg, 4, 1, 0);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
		FT2_DBG("%s: page = 0x%04x, reg = 0x%02x (%d), value = 0x%x\n", __func__, page, reg, reg, value);
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		/*
		select MPBIST test item, mpbist_item_sel
		write sram address, reg.27[15:0] = 16'h8104
		write sram data, reg.28[15:0] = 16'h0f00"
		*/
		reg = 27;
		ca_ft2_phy_regfield_write(phy_dev, reg, 0, 16, 0x8104);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
		FT2_DBG("%s: reg = 0x%02x (%d), value = 0x%x\n", __func__, reg, reg, value);
		reg = 28;
		ca_ft2_phy_regfield_write(phy_dev, reg, 0, 16, 0x0f00);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
		FT2_DBG("%s: reg = 0x%02x (%d), value = 0x%x\n", __func__, reg, reg, value);

		/*
		write mpbist_en = 1'b1
		page. 0x0a47 reg.17[8] = 1'b1"
		*/
		page = 0x0a47;
		reg = 17;
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, page);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
		FT2_DBG("%s: page = 0x%04x, reg = 0x%02x (%d), value = 0x%x\n", __func__, page, reg, reg, value);
		ca_ft2_phy_regfield_write(phy_dev, reg, 8, 1, 1);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
		FT2_DBG("%s: page = 0x%04x, reg = 0x%02x (%d), value = 0x%x\n", __func__, page, reg, reg, value);
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		/*
		check mpbist_en = 1'b0 to ensure mpbist is finished
		page. 0x0a47 reg.17[8] = 1'b0"
		*/
		page = 0x0a47;
		reg = 17;
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, page);
		cnt = CNT;
		do {
			ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
			/* check mpbist_en = 1'b0 to ensure mpbist is finished */
			if (!value)
				break;
			mdelay(10);
		} while ((cnt--) >  0);
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		/* check timeout */
		if (cnt <=  0) {
			FT2_DBG("%s: mpbist timeout (cnt = %d) !page = 0x%04x, reg = 0x%02x (%d), value = 0x%x\n", __func__, cnt, page, reg, reg, value);
			rc = FT2_ERROR;
			record->entry_rlt[i] = 1;
		}
	}

	return rc;
}

ft2_ret_t ca_ft2_gmii_open_loopback_result(ft2_record_t *record)
{
#define CHK_CNT	5

	uint8_t i, port, phy_dev, reg;
	ft2_ret_t rc = FT2_OK;
	uint16_t page, value, value1;
	int16_t cnt, rslt, fin;

	printf("FT2_PARAM_GMII_PORT_OPEN_LOOPBACK test result ...\n");

	/* read test results */
	cnt = 1;
	do {
		FT2_MOD("CNT: %d\n", cnt);
		rslt = 0;
		fin = 0;
		for (i = 0; (port = gmii_ports[i]) != INVALID_PORT; i++) {
			if (!(GMII_PORT_MASK & (1 << i)))
				continue;
			FT2_MOD("GPHY Port %d Results: \n", port);
			phy_dev = phy_addr[port];

			/*
			read mpbist_pass = 16'h0f00
			write sram address, reg.27[15:0] = 16'h8106
			read sram data, reg.28[15:0] = 16'h0f00"
			*/
			reg = 27;
			ca_ft2_phy_regfield_write(phy_dev, reg, 0, 16, 0x8106);
			ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
			FT2_DBG("%s: reg = 0x%02x (%d), value = 0x%x\n", __func__, reg, reg, value);
			reg = 28;
			ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
			FT2_DBG("%s: reg = 0x%02x (%d), value = 0x%x\n", __func__, reg, reg, value);

			/*
			read mpbist_fail = 16'h0000
			write sram address, reg.27[15:0] = 16'h8108
			read sram data, reg.28[15:0] = 16'h0000"
			*/
			reg = 27;
			ca_ft2_phy_regfield_write(phy_dev, reg, 0, 16, 0x8108);
			ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value1);
			FT2_DBG("%s: reg = 0x%02x (%d), value = 0x%x\n", __func__, reg, reg, value1);
			reg = 28;
			ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value1);
			FT2_DBG("%s: reg = 0x%02x (%d), value = 0x%x\n", __func__, reg, reg, value1);

			if (value != 0x0f00) {
				rslt |= (0x1 << i);
				FT2_DBG("%s: mpbist_pass = 0x%04x, mpbist_fail = 0x%04x\n", __func__, value, value1);
			}
			if ((value | value1) != 0xf00)
				fin |= (0x1 << i);
		}

		/* all ports pass */
		if (!rslt)
			break;
		/* all port test complete */
		if (!fin)
			break;
		mdelay(3000);
	} while ((cnt++) < CHK_CNT);

	/* recover setting */
	for (i = 0; (port = gmii_ports[i]) != INVALID_PORT; i++) {
		if (!(GMII_PORT_MASK & (1 << i)))
			continue;
		FT2_MOD("GPHY Port %d recover setting\n", port);
		phy_dev = phy_addr[port];

		/* fill result */
		if (rslt & (0x1 << i)) {
			record->entry_rlt[i] |= 1;
			rc = FT2_ERROR;
		} else
			record->entry_rlt[i] |= 0;

		FT2_MOD("%s\n", record->entry_rlt[i] ? "FAIL" : "PASS");

		/*
		write rg_dly_linkok10 = 1'b1
		page. 0x0a4e reg.16[11] = 1'b1
		*/
		page = 0x0a4e;
		reg = 16;
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, page);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
		FT2_DBG("%s: page = 0x%04x, reg = 0x%02x (%d), value = 0x%x\n", __func__, page, reg, reg, value);
		ca_ft2_phy_regfield_write(phy_dev, reg, 11, 1, 1);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
		FT2_DBG("%s: page = 0x%04x, reg = 0x%02x (%d), value = 0x%x\n", __func__, page, reg, reg, value);
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);

		/*
		write rg_en_idletimer_before_chest = 1'b1
		page.0x0a84 reg.19[4] = 1'b1
		*/
		page = 0x0a84;
		reg = 19;
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, page);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
		FT2_DBG("%s: page = 0x%04x, reg = 0x%02x (%d), value = 0x%x\n", __func__, page, reg, reg, value);
		ca_ft2_phy_regfield_write(phy_dev, reg, 4, 1, 1);
		ca_ft2_phy_regfield_read(phy_dev, reg, 0, 16, &value);
		FT2_DBG("%s: page = 0x%04x, reg = 0x%02x (%d), value = 0x%x\n", __func__, page, reg, reg, value);
		ca_ft2_phy_regfield_write(phy_dev, 31, 0, 16, 0);
	}


	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return rc;
}

ft2_ret_t ca_ft2_rgmii_loopback(ft2_record_t *record)
{
#define LOOPBACK_PKT_CNT	1000

	uint8_t i, port;
	ft2_ret_t rc = FT2_OK;
	NI_HV_GLB_STATIC_CFG_t cfg;
	NI_HV_PT_PORT_STATIC_CFG_t port_cfg;
	NI_HV_PT_PORT_GLB_CFG_t port_glb;
	ca_uint32_t rx_stats[__NI_MIB_RX_NUM];
	ca_uint32_t tx_stats[__NI_MIB_TX_NUM];

	printf("FT2_PARAM_RGMII_PORT_LOOPBACK test ...\n");

	record->start = get_timer(0);
	record->entry_mask = RGMII_PORT_MASK;

	/* release port reset */
	CORTINA_CA_REG_WRITE(0, NI_HV_GLB_INTF_RST_CONFIG);

	/* just init ether */
	//NetLoop(FT2TEST);
	net_loop(FT2TEST);

	/* set mac loopback */
	for (i = 0; (port = rgmii_ports[i]) != INVALID_PORT; i++) {
		if (!(RGMII_PORT_MASK & (1 << i)))
			continue;

		record->entry_test[i] = 1;
		/* all port ingress goes to L2FE */
		cfg.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_STATIC_CFG);
		cfg.bf.port_to_cpu = port;
		CORTINA_CA_REG_WRITE(cfg.wrd, NI_HV_GLB_STATIC_CFG);
		/* set to  RGMII mode */
		port_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		port_cfg.bf.int_cfg = 2;
		port_cfg.bf.mac_addr6 = 0x2;
		/* enable  mac loopback: Ethernet Tx-to-Rx (Deep) Loopback (after MAC) */
		port_cfg.bf.lpbk_mode = 2;
		CORTINA_CA_REG_WRITE(port_cfg.wrd, (NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		/* set speed/duplex */
		port_glb.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * port));
		port_glb.bf.duplex = 1;
		CORTINA_CA_REG_WRITE(port_glb.wrd, (NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * port));

		/* wait port link state change */
		udelay(10000);

		/* clear mib counter */
		ni_eth_port_rx_mib_get(port, 1, rx_stats);
		ni_eth_port_tx_mib_get(port, 1, tx_stats);

		/* send test packets */
		ca_ft2_test_pkt_send(LOOPBACK_PKT_CNT);

		/* read mib counter */
		ni_eth_port_rx_mib_get(port, 0, rx_stats);
		ni_eth_port_tx_mib_get(port, 0, tx_stats);

		/* check counters */
		if (rx_stats[__NI_MIB_RX_STATS_FRM_128_TO_255_OCT] < LOOPBACK_PKT_CNT || tx_stats[__NI_MIB_TX_STATS_FRM_128_TO_255_OCT] < LOOPBACK_PKT_CNT ||
			rx_stats[__NI_MIB_RX_CRC_ERR_FRM_CNT] != 0) {
			FT2_DBG("%s: Port %d loopback fail: tx_frame_128_to_255_cnt = %d, rx_frame_128_to_255_cnt = %d, rx_fcs_error_frames = %d\n",
				__func__, port, tx_stats[__NI_MIB_TX_STATS_FRM_128_TO_255_OCT], rx_stats[__NI_MIB_RX_STATS_FRM_128_TO_255_OCT],
				rx_stats[__NI_MIB_RX_CRC_ERR_FRM_CNT]);
			rc = FT2_ERROR;
		}

		/* disable  mac loopback: Ethernet Tx-to-Rx (Deep) Loopback (after MAC) */
		port_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		port_cfg.bf.lpbk_mode = 0;
		CORTINA_CA_REG_WRITE(port_cfg.wrd, (NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));

		record->entry_rlt[i] = rc ? 1 : 0;
	}

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return rc;
}

ft2_ret_t ca_ft2_intra_xfi_test(ft2_record_t *record)
{
	#define RETRY_TIMES 3
	ft2_ret_t rc = FT2_OK;
	int j;
	uint32_t cnt[2][RETRY_TIMES] = {0};

#if (INTRA_XFI_PORT_MASK > 0)

	printf("FT2_PARAM_INTRA_XFI test ...\n");

	record->start = get_timer(0);\
	record->entry_mask = INTRA_XFI_PORT_MASK;

	/* g3hgu intra-XFI1 prbs loopback */
	record->entry_test[0] = 1;

	if (FT2_OK != (rc = ca_ft2_xfi_serdes_cfg(SERDES_MOD_INTRA_XFI)))
		FT2_DBG("%s - %d: intra XFI serdes config fail\n", __func__, __LINE__);

	udelay(100000);
	/* get and check DUT counter */
	for (j = 0; j < RETRY_TIMES; j++) {
		/* check intra xfi prbs counter */
		udelay(100000);
		cnt[0][j] = CORTINA_CA_REG_READ(NI_HV_SXGMII_PCS_PCSTSTPTRNERRCNTR + APB0_NI_HV_SXGMII_STRIDE);
		cnt[1][j] = CORTINA_CA_REG_READ(SATURN_NI_HV_SXGMII_PCS_PCSTSTPTRNERRCNTR);
	}

	FT2_DBG("%s - %d: Get intra XFI counter [%d, %d]\n", __func__, __LINE__, cnt[0][0], cnt[0][RETRY_TIMES-1]);
	FT2_DBG("%s - %d: Get intra Saturn XFI counter [%d, %d]\n", __func__, __LINE__, cnt[1][0], cnt[1][RETRY_TIMES-1]);

	if ((cnt[0][0] != 0) && (cnt[0][RETRY_TIMES-1] == 0) && (cnt[1][0] != 0) && (cnt[1][RETRY_TIMES-1] == 0)) {
		FT2_MOD("%s - %d: Intra XFI PRBS PASS\n", __func__, __LINE__);
	} else {
		FT2_MOD("%s - %d: Intra XFI PRBS FAIL\n", __func__, __LINE__);
		rc = FT2_ERROR;
	}

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->entry_rlt[0] = rc ? 1 : 0;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return rc;
#else
	printf("No INTRA XFI test\n");
	return FT2_ERROR;
#endif
}

#define GPIO_PAIR_TEST(in, out, exp, rc) {\
	uint8_t value;\
	FT2_DBG("%s - %d: in GPIO%d, out GPIO%d, exp value = %d\n", __func__, __LINE__, in, out, exp);\
	gpio_direction_output(out, exp);\
	gpio_direction_input(in);\
	value = gpio_get_value(in);\
	if (exp != value) {\
		rc = FT2_GPIO;\
		FT2_DBG("%s - %d: in GPIO%d, out GPIO%d, value = %d, rc = %d\n", __func__, __LINE__, in, out, value, rc);\
	}\
} while(0)

ft2_ret_t ca_ft2_gpio_test(ft2_record_t *record)
{
	ft2_ret_t rc = FT2_OK;
	int i, j;
	GLOBAL_PIN_MUX_t g_pin_mux;

#if (TEST_GPIO_MASK > 0)

	printf("FT2_PARAM_GPIO test ...\n");

	record->entry_mask = TEST_GPIO_MASK;
	record->start = get_timer(0);
	record->entry_test[0] |= 1;

#if defined(CONFIG_TARGET_PRESIDIO_ASIC)
	/* for gpio test pin */
	g_pin_mux.wrd = CORTINA_CA_REG_READ(GLOBAL_PIN_MUX);
	g_pin_mux.bf.iomux_led_enable = 0;
	g_pin_mux.bf.iomux_sd_volt_reg = 0;
	CORTINA_CA_REG_WRITE(g_pin_mux.wrd, GLOBAL_PIN_MUX);
	#if defined(CONFIG_CA8279_FT2)
	CORTINA_CA_REG_WRITE(0x02800000, SATURN_GLOBAL_PIN_MUX);
	#endif
#elif defined(CONFIG_TARGET_VENUS)
	/* for gpio test pin */
	g_pin_mux.wrd = CORTINA_CA_REG_READ(GLOBAL_PIN_MUX);
	g_pin_mux.bf.iomux_spi_cs_2_enable = 0;
	g_pin_mux.bf.iomux_spi_cs_3_enable = 0;
	g_pin_mux.bf.iomux_spi_cs_4_enable = 0;
	g_pin_mux.bf.iomux_sd_volt_reg = 0;
	g_pin_mux.bf.iomux_led_enable = 0;
	CORTINA_CA_REG_WRITE(g_pin_mux.wrd, GLOBAL_PIN_MUX);
#endif

	for (i = 0; i < GPIO_PAIR_NUM; i++) {
		/* set gpio mux */
		for (j = 0; j <=1; j++)
			ca_ft2_gpio_mux_set(gpio_pair[i][j]);

#if defined(CONFIG_TARGET_PRESIDIO_ASIC)
		/* workaround for hw bug */
		if (gpio_pair[i][1] == 66)
			CORTINA_CA_REG_WRITE(0xfffffffb, PER_GPIO1_CFG);
#endif
		GPIO_PAIR_TEST(gpio_pair[i][0], gpio_pair[i][1], 1, rc);
		GPIO_PAIR_TEST(gpio_pair[i][0], gpio_pair[i][1], 0, rc);
#if defined(CONFIG_TARGET_PRESIDIO_ASIC)
		/* workaround for hw bug */
		if (gpio_pair[i][1] == 66)
			CORTINA_CA_REG_WRITE(0xffffffff, PER_GPIO1_CFG);
#endif
	}

	if (!record->module_rlt)
		record->module_rlt = rc;
	record->entry_rlt[0] = rc ? 1 : 0;
	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return rc;
#else
	printf("No GPIO test\n");
	return FT2_GPIO;
#endif
}

ft2_ret_t ca_ft2_otp_test(ft2_record_t *record)
{
	ft2_ret_t rc = FT2_OK;
	uint32_t value;

	record->entry_mask = TEST_OTP_CHECK;
	record->start = get_timer(0);

	record->entry_test[0] |= TEST_OTP_CHECK;

	value = CORTINA_CA_REG_READ(BL2_TEST_RLT);
	if (value & BL2_TEST_OTP) {
		record->entry_rlt[0] = TEST_OTP_CHECK;
		rc = FT2_OTP;
	}

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	FT2_MOD("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc ? FT2_OTP : FT2_OK;
}

ft2_ret_t ca_ft2_cpu_test(ft2_record_t *record)
{
	char cmd[64];
	uint32_t cpu, addr, value, end_addr;
	int cmd_ret;
	ft2_ret_t rc = FT2_OK;

#if (CPU_CORE_MASK > 0)

	printf("FT2_PARAM_CPU test ...\n");

	record->entry_mask = CPU_CORE_MASK;
	record->start = get_timer(0);
	record->entry_test[0] = 1;

	/* wake up cpu */
	if (CPU_CORE_NUM > 1) {
		sprintf(cmd, "cpu_up 0x%x", CPU_CORE_MASK);
		FT2_DBG("%s\n", cmd);
		if ((cmd_ret = run_command(cmd, 0)))
			rc = FT2_CPU;
	}
	/* test and check cpu 0 result */
	end_addr =  CONFIG_SYS_MEMTEST_START + ((CONFIG_SYS_MEMTEST_END - CONFIG_SYS_MEMTEST_START) >> 5);
	sprintf(cmd, "mtest 0x%x 0x%x 0xa5 1", CONFIG_SYS_MEMTEST_START, end_addr);
	FT2_DBG("%s\n", cmd);
	if ((cmd_ret = run_command(cmd, 0))) {
		rc = FT2_CPU;
		record->entry_rlt[0] = 1;
	}
	FT2_DBG("%s: cmd_ret = %d, rc = 0x%x\n", __func__, cmd_ret, rc);

	/* check cpu 1,2,3 results */
	for (cpu = 1; cpu < CPU_CORE_NUM; cpu++) {
		if (!((CPU_CORE_MASK >> cpu) & 0x1))
			continue;
		record->entry_test[cpu] = 1;
		addr = CPU_CORE_VERIFY_ADDR + 4*cpu;
		value = CORTINA_CA_REG_READ(addr);
		if (value != 0x900d) {
			rc = FT2_CPU;
			record->entry_rlt[cpu] = 1;
		}
		FT2_DBG("%s: cpu#%d, addr = 0x%04x, value = 0x%04x\n", __func__, cpu, addr, value);
	}

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc ? FT2_CPU : FT2_OK;
#else
	printf("No CPU test\n");
	return FT2_CPU;
#endif
}

#define HEX2DEC(x) ((x >> 4) * 10 + (x & 0xf))
#define RTC_DATE_TO_SEC(d, h, m, s, out) {\
	out = (HEX2DEC(s) + HEX2DEC(m)*60 + HEX2DEC(h)*60*60 + HEX2DEC(d)*24*60*60);\
} while(0)

ft2_ret_t ca_ft2_rtc_test(ft2_record_t *record)
{
	uint32_t sec, min, hr, day;

#if (RTC_MASK > 0)
	printf("FT2_PARAM_RTC test ...\n");

	record->entry_mask = RTC_MASK;
	record->start = get_timer(0);
	record->entry_test[0] = TEST_RTC_TIME | TEST_RTC_ACCESS;

	/* get rtc time */
	sec = CORTINA_CA_REG_READ(RTCLK_BCD_SEC);
	min = CORTINA_CA_REG_READ(RTCLK_BCD_MIN);
	hr = CORTINA_CA_REG_READ(RTCLK_BCD_HOUR);
	day = CORTINA_CA_REG_READ(RTCLK_BCD_DAY);
	RTC_DATE_TO_SEC(day, hr, min, sec, record->data);

	FT2_DBG("%s: sec = %x, min = %x, hr = %x, day = %x\n", __func__, sec, min, hr, day);
	FT2_DBG("%s: time_start = %d\n", __func__, record->data);

	return  FT2_OK;
#else
		printf("No RTC test\n");
		return FT2_RTC;
#endif
}

ft2_ret_t ca_ft2_rtc_result(ft2_record_t *record)
{
	uint32_t value, sec, min, hr, day, t_end, t_diff;
	ft2_ret_t rc = FT2_OK;

#if (RTC_MASK > 0)
	printf("FT2_PARAM_RTC test result ...\n");

	/* get rtc access result */
	value = CORTINA_CA_REG_READ(BL2_TEST_RLT);
	FT2_DBG("BL2_TEST_RLT = 0x%08x\n", value);
	if ((value >> 24) == 0xf2)
		record->entry_rlt[0] = value & BL2_TEST_RTC ? TEST_RTC_ACCESS : 0;
	else
		record->entry_rlt[0] = TEST_RTC_ACCESS;

	/* get rtc time */
	sec = CORTINA_CA_REG_READ(RTCLK_BCD_SEC);
	min = CORTINA_CA_REG_READ(RTCLK_BCD_MIN);
	hr = CORTINA_CA_REG_READ(RTCLK_BCD_HOUR);
	day = CORTINA_CA_REG_READ(RTCLK_BCD_DAY);
	RTC_DATE_TO_SEC(day, hr, min, sec, t_end);
	t_diff = (t_end -record->data)/RTC_SPEEDUP_SCALE;

	FT2_DBG("%s: sec = %x, min = %x, hr = %x, day = %x\n", __func__, sec, min, hr, day);
	FT2_DBG("%s: time_diff = %d, tme_start = %d, time_end = %d\n", __func__, t_diff, record->data, t_end);

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;

	if (abs(t_diff-record->finish/1000) > RTC_TIME_TOLERANCE)
		record->entry_rlt[0] |= TEST_RTC_TIME;

	rc = record->entry_rlt[0] ? FT2_RTC: FT2_OK;
	record->module_rlt = record->entry_rlt[0];

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc;
#else
	printf("No RTC test\n");
	return FT2_RTC;
#endif
}

#if (PCIE_PORT_MASK > 0)
#define CA_PCIE_WRITE(addr, value)      writel(value, (u64)addr)
int pcie_test(u32 *link_status, u32 *link_speed, int *ret, u32 *vendor_id)
{
	u32 status0, status1, status2;
	u32 speed0, speed1, speed2;
	u32 val;
	GLOBAL_BLOCK_RESET_t g_blk_reset;
	int rc = 0;

	// gpio output
	gpio_direction_output(0, 0);
	gpio_direction_output(1, 0);
	gpio_direction_output(2, 0);
	mdelay(150);

	// block reset assert
	g_blk_reset.wrd = CORTINA_CA_REG_READ(GLOBAL_BLOCK_RESET);
	g_blk_reset.bf.reset_pcie0 = 1;
	g_blk_reset.bf.reset_pcie1  = 1;
	g_blk_reset.bf.reset_pcie2  = 1;
	CORTINA_CA_REG_WRITE(g_blk_reset.wrd, GLOBAL_BLOCK_RESET);
	mdelay(1);
	// dphy reset assert
	CA_PCIE_WRITE(0xf432002c,0x000001bf);
	mdelay(1);
	// serdes power off
	CA_PCIE_WRITE(0xf4320050,0x00008803);
	mdelay(1);

	// serdes_phy
	CA_PCIE_WRITE(0xf4333068,0x00000000);
	CA_PCIE_WRITE(0xf4333004,0x0000a852);
	CA_PCIE_WRITE(0xf4333008,0x00006042);
	CA_PCIE_WRITE(0xf4333010,0x00008000);
	CA_PCIE_WRITE(0xf4333014,0x0000062c);
	CA_PCIE_WRITE(0xf4333018,0x00000007);
	CA_PCIE_WRITE(0xf4333028,0x0000d210);
	CA_PCIE_WRITE(0xf433302c,0x0000b905);
	CA_PCIE_WRITE(0xf4333030,0x0000c008);
	CA_PCIE_WRITE(0xf4333034,0x0000f712);
	CA_PCIE_WRITE(0xf4333038,0x00001000);
	CA_PCIE_WRITE(0xf4333080,0x0000d466);
	CA_PCIE_WRITE(0xf4333084,0x000066aa);
	CA_PCIE_WRITE(0xf4333088,0x00000013);
	CA_PCIE_WRITE(0xf433308c,0x0000bb66);
	CA_PCIE_WRITE(0xf4333090,0x00004f0c);
	CA_PCIE_WRITE(0xf43330ac,0x0000b003);
	CA_PCIE_WRITE(0xf43330b8,0x0000fffe);
	CA_PCIE_WRITE(0xf4333024,0x0000500c);
	CA_PCIE_WRITE(0xf4333024,0x0000520c);

	CA_PCIE_WRITE(0xf4333168,0x00000000);
	CA_PCIE_WRITE(0xf4333104,0x0000a84a);
	CA_PCIE_WRITE(0xf4333108,0x00006042);
	CA_PCIE_WRITE(0xf4333110,0x00008000);
	CA_PCIE_WRITE(0xf4333114,0x0000062c);
	CA_PCIE_WRITE(0xf4333118,0x00000007);
	CA_PCIE_WRITE(0xf4333128,0x0000d210);
	CA_PCIE_WRITE(0xf433312c,0x0000b905);
	CA_PCIE_WRITE(0xf4333130,0x0000c008);
	CA_PCIE_WRITE(0xf4333134,0x0000f712);
	CA_PCIE_WRITE(0xf4333138,0x00001000);
	CA_PCIE_WRITE(0xf4333184,0x000066aa);
	CA_PCIE_WRITE(0xf4333188,0x00000013);
	CA_PCIE_WRITE(0xf433318c,0x0000bb66);
	CA_PCIE_WRITE(0xf4333190,0x00004f10);
	CA_PCIE_WRITE(0xf43331ac,0x0000b003);
	CA_PCIE_WRITE(0xf43331b8,0x0000fffe);
	CA_PCIE_WRITE(0xf43331bc,0x00005100);
	CA_PCIE_WRITE(0xf4333124,0x0000500c);
	CA_PCIE_WRITE(0xf4333124,0x0000520c);

	CA_PCIE_WRITE(0xf4334068,0x00000000);
	CA_PCIE_WRITE(0xf4334004,0x0000a852);
	CA_PCIE_WRITE(0xf4334008,0x00006042);
	CA_PCIE_WRITE(0xf4334010,0x00008000);
	CA_PCIE_WRITE(0xf4334014,0x0000062c);
	CA_PCIE_WRITE(0xf4334018,0x00000007);
	CA_PCIE_WRITE(0xf4334028,0x0000d210);
	CA_PCIE_WRITE(0xf433402c,0x0000b905);
	CA_PCIE_WRITE(0xf4334030,0x0000c008);
	CA_PCIE_WRITE(0xf4334034,0x0000f712);
	CA_PCIE_WRITE(0xf4334038,0x00001000);
	CA_PCIE_WRITE(0xf4334080,0x0000d466);
	CA_PCIE_WRITE(0xf4334084,0x000066aa);
	CA_PCIE_WRITE(0xf4334088,0x00000013);
	CA_PCIE_WRITE(0xf433408c,0x0000bb66);
	CA_PCIE_WRITE(0xf4334090,0x00004f0c);
	CA_PCIE_WRITE(0xf43340ac,0x0000b003);
	CA_PCIE_WRITE(0xf43340b8,0x0000fffe);
	CA_PCIE_WRITE(0xf4334024,0x0000500c);
	CA_PCIE_WRITE(0xf4334024,0x0000520c);

	CA_PCIE_WRITE(0xf4334168,0x00000000);
	CA_PCIE_WRITE(0xf4334104,0x0000a84a);
	CA_PCIE_WRITE(0xf4334108,0x00006042);
	CA_PCIE_WRITE(0xf4334110,0x00008000);
	CA_PCIE_WRITE(0xf4334114,0x0000062c);
	CA_PCIE_WRITE(0xf4334118,0x00000007);
	CA_PCIE_WRITE(0xf4334128,0x0000d210);
	CA_PCIE_WRITE(0xf433412c,0x0000b905);
	CA_PCIE_WRITE(0xf4334130,0x0000c008);
	CA_PCIE_WRITE(0xf4334134,0x0000f712);
	CA_PCIE_WRITE(0xf4334138,0x00001000);
	CA_PCIE_WRITE(0xf4334184,0x000066aa);
	CA_PCIE_WRITE(0xf4334188,0x00000013);
	CA_PCIE_WRITE(0xf433418c,0x0000bb66);
	CA_PCIE_WRITE(0xf4334190,0x00004f10);
	CA_PCIE_WRITE(0xf43341ac,0x0000b003);
	CA_PCIE_WRITE(0xf43341b8,0x0000fffe);
	CA_PCIE_WRITE(0xf43341bc,0x00005100);
	CA_PCIE_WRITE(0xf4334124,0x0000500c);
	CA_PCIE_WRITE(0xf4334124,0x0000520c);

	CA_PCIE_WRITE(0xf4335068,0x00000000);
	CA_PCIE_WRITE(0xf4335004,0x0000a852);
	CA_PCIE_WRITE(0xf4335008,0x00006042);
	CA_PCIE_WRITE(0xf4335010,0x00008000);
	CA_PCIE_WRITE(0xf4335014,0x0000062c);
	CA_PCIE_WRITE(0xf4335018,0x00000007);
	CA_PCIE_WRITE(0xf4335028,0x0000d210);
	CA_PCIE_WRITE(0xf433502c,0x0000b905);
	CA_PCIE_WRITE(0xf4335030,0x0000c008);
	CA_PCIE_WRITE(0xf4335034,0x0000f712);
	CA_PCIE_WRITE(0xf4335038,0x00001000);
	CA_PCIE_WRITE(0xf4335080,0x0000d466);
	CA_PCIE_WRITE(0xf4335084,0x000066aa);
	CA_PCIE_WRITE(0xf4335088,0x00000013);
	CA_PCIE_WRITE(0xf433508c,0x0000bb66);
	CA_PCIE_WRITE(0xf4335090,0x00004f0c);
	CA_PCIE_WRITE(0xf43350ac,0x0000b003);
	CA_PCIE_WRITE(0xf43350b8,0x0000fffe);
	CA_PCIE_WRITE(0xf4335024,0x0000500c);
	CA_PCIE_WRITE(0xf4335024,0x0000520c);

	CA_PCIE_WRITE(0xf4335168,0x00000000);
	CA_PCIE_WRITE(0xf4335104,0x0000a84a);
	CA_PCIE_WRITE(0xf4335108,0x00006042);
	CA_PCIE_WRITE(0xf4335110,0x00008000);
	CA_PCIE_WRITE(0xf4335114,0x0000062c);
	CA_PCIE_WRITE(0xf4335118,0x00000007);
	CA_PCIE_WRITE(0xf4335128,0x0000d210);
	CA_PCIE_WRITE(0xf433512c,0x0000b905);
	CA_PCIE_WRITE(0xf4335130,0x0000c008);
	CA_PCIE_WRITE(0xf4335134,0x0000f712);
	CA_PCIE_WRITE(0xf4335138,0x00001000);
	CA_PCIE_WRITE(0xf4335184,0x000066aa);
	CA_PCIE_WRITE(0xf4335188,0x00000013);
	CA_PCIE_WRITE(0xf433518c,0x0000bb66);
	CA_PCIE_WRITE(0xf4335190,0x00004f10);
	CA_PCIE_WRITE(0xf43351ac,0x0000b003);
	CA_PCIE_WRITE(0xf43351b8,0x0000fffe);
	CA_PCIE_WRITE(0xf43351bc,0x00005100);
	CA_PCIE_WRITE(0xf4335124,0x0000500c);
	CA_PCIE_WRITE(0xf4335124,0x0000520c);

	CA_PCIE_WRITE(0xf4336068,0x00000000);
	CA_PCIE_WRITE(0xf4336004,0x0000a852);
	CA_PCIE_WRITE(0xf4336008,0x00006042);
	CA_PCIE_WRITE(0xf4336010,0x00008000);
	CA_PCIE_WRITE(0xf4336014,0x0000062c);
	CA_PCIE_WRITE(0xf4336018,0x00000007);
	CA_PCIE_WRITE(0xf4336028,0x0000d210);
	CA_PCIE_WRITE(0xf433602c,0x0000b905);
	CA_PCIE_WRITE(0xf4336030,0x0000c008);
	CA_PCIE_WRITE(0xf4336034,0x0000f712);
	CA_PCIE_WRITE(0xf4333038,0x00001000);
	CA_PCIE_WRITE(0xf4336080,0x0000d466);
	CA_PCIE_WRITE(0xf4336084,0x000066aa);
	CA_PCIE_WRITE(0xf4336088,0x00000013);
	CA_PCIE_WRITE(0xf433608c,0x0000bb66);
	CA_PCIE_WRITE(0xf4336090,0x00004f0c);
	CA_PCIE_WRITE(0xf43360ac,0x0000b003);
	CA_PCIE_WRITE(0xf43360b8,0x0000fffe);
	CA_PCIE_WRITE(0xf4336024,0x0000500c);
	CA_PCIE_WRITE(0xf4336024,0x0000520c);

	CA_PCIE_WRITE(0xf4336168,0x00000000);
	CA_PCIE_WRITE(0xf4336104,0x0000a84a);
	CA_PCIE_WRITE(0xf4336108,0x00006042);
	CA_PCIE_WRITE(0xf4336110,0x00008000);
	CA_PCIE_WRITE(0xf4336114,0x0000062c);
	CA_PCIE_WRITE(0xf4336118,0x00000007);
	CA_PCIE_WRITE(0xf4336128,0x0000d210);
	CA_PCIE_WRITE(0xf433612c,0x0000b905);
	CA_PCIE_WRITE(0xf4336130,0x0000c008);
	CA_PCIE_WRITE(0xf4336134,0x0000f712);
	CA_PCIE_WRITE(0xf4336138,0x00001000);
	CA_PCIE_WRITE(0xf4336184,0x000066aa);
	CA_PCIE_WRITE(0xf4336188,0x00000013);
	CA_PCIE_WRITE(0xf433618c,0x0000bb66);
	CA_PCIE_WRITE(0xf4336190,0x00004f10);
	CA_PCIE_WRITE(0xf43361ac,0x0000b003);
	CA_PCIE_WRITE(0xf43361b8,0x0000fffe);
	CA_PCIE_WRITE(0xf43361bc,0x00005100);
	CA_PCIE_WRITE(0xf4336124,0x0000500c);
	CA_PCIE_WRITE(0xf4336124,0x0000520c);

	CA_PCIE_WRITE(0xf4337004,0x0000a852);
	CA_PCIE_WRITE(0xf4337008,0x00006042);
	CA_PCIE_WRITE(0xf4337010,0x00009000);
	CA_PCIE_WRITE(0xf4337014,0x0000c314);
	CA_PCIE_WRITE(0xf4337018,0x00000007);
	CA_PCIE_WRITE(0xf4337028,0x0000d210);
	CA_PCIE_WRITE(0xf433702c,0x0000b905);
	CA_PCIE_WRITE(0xf4337030,0x0000c008);
	CA_PCIE_WRITE(0xf4337034,0x0000f712);
	CA_PCIE_WRITE(0xf4337038,0x00001000);
	CA_PCIE_WRITE(0xf4337080,0x0000c466);
	CA_PCIE_WRITE(0xf4337084,0x000066aa);
	CA_PCIE_WRITE(0xf4337088,0x00000013);
	CA_PCIE_WRITE(0xf433708c,0x0000bb66);
	CA_PCIE_WRITE(0xf4337090,0x00004f0c);
	CA_PCIE_WRITE(0xf43370ac,0x0000b001);
	CA_PCIE_WRITE(0xf43370b8,0x0000fffe); // #CDR timer
	CA_PCIE_WRITE(0xf4337024,0x0000500c);
	CA_PCIE_WRITE(0xf4337024,0x0000520c);

	CA_PCIE_WRITE(0xf4337104,0x0000a84a);
	CA_PCIE_WRITE(0xf4337108,0x00006042);
	CA_PCIE_WRITE(0xf4337110,0x00009000);
	CA_PCIE_WRITE(0xf4337114,0x0000c314);
	CA_PCIE_WRITE(0xf4337118,0x00000007);
	CA_PCIE_WRITE(0xf4337128,0x0000d210);
	CA_PCIE_WRITE(0xf433712c,0x0000b905);
	CA_PCIE_WRITE(0xf4337130,0x0000c008);
	CA_PCIE_WRITE(0xf4337134,0x0000f712);
	CA_PCIE_WRITE(0xf4337138,0x00001000);
	CA_PCIE_WRITE(0xf4337184,0x000066aa);
	CA_PCIE_WRITE(0xf4337188,0x00000013);
	CA_PCIE_WRITE(0xf433718c,0x0000bb66);
	CA_PCIE_WRITE(0xf4337190,0x00004f10);
	CA_PCIE_WRITE(0xf43371ac,0x0000b001);
	CA_PCIE_WRITE(0xf43371b8,0x0000fffe); // #CDR timer
	CA_PCIE_WRITE(0xf43371bc,0x0000a046);
	CA_PCIE_WRITE(0xf4337124,0x0000500c);
	CA_PCIE_WRITE(0xf4337124,0x0000520c);

	// serdes power on
	CA_PCIE_WRITE(0xf4320050,0x00009aab);
	mdelay(1);

	// dphy reset deassert
	CA_PCIE_WRITE(0xf432002c,0x000001b8);
	mdelay(1);
	// block reset deassert
	g_blk_reset.bf.reset_pcie0 = 0;
	g_blk_reset.bf.reset_pcie1  = 0;
	g_blk_reset.bf.reset_pcie2  = 0;
	CORTINA_CA_REG_WRITE(g_blk_reset.wrd, GLOBAL_BLOCK_RESET);
	mdelay(1);

	// check ckusable
	val = readl(0xf4320058);
	if ((val & 0x68) != 0x68)
		printf("0xf4320058 is %x but should be 0x68\n", val);

	int cnt = 0;
	do {
	//	mdelay(100);

		val = 1;
		val &= readl(0xf433317c);
		val &= readl(0xf433517c);
		//val &= readl(0xf433717c);

		if (val)
			break;
	} while (cnt++ < 50);

	if (cnt >= 50) {
		printf("BER notify fail!!!\n");
		printf("0xf433317c = %x\n", readl(0xf433317c));
		printf("0xf433517c = %x\n", readl(0xf433517c));
		printf("0xf433717c = %x\n", readl(0xf433717c));
	}

	gpio_direction_output(0, 1);
	gpio_direction_output(1, 1);
	gpio_direction_output(2, 1);
	mdelay(100);

	//,setup pcie0 rc
	CA_PCIE_WRITE(0xf4325040,0xa0000000);
	CA_PCIE_WRITE(0xf4325044,0xa0000FFF);
	CA_PCIE_WRITE(0xf4325048,0xa0300000);
	CA_PCIE_WRITE(0xa0000710,0x00030120);
	CA_PCIE_WRITE(0xa000080C,0x0003020F);
	CA_PCIE_WRITE(0xa00008CC,0x32000000);
	CA_PCIE_WRITE(0xa0000010,0x00000004);
	CA_PCIE_WRITE(0xa0000014,0x00000000);
	CA_PCIE_WRITE(0xa000003C,0x000001FF);
	CA_PCIE_WRITE(0xa0000018,0x00000000);
	CA_PCIE_WRITE(0xa0000018,0x00010100);
	CA_PCIE_WRITE(0xa0000020,0xa000a000);
	CA_PCIE_WRITE(0xa0000004,0x00100000);
	CA_PCIE_WRITE(0xa0000004,0x00100007);

	// pcie0 outbound atu
	CA_PCIE_WRITE(0xa0300008, 0xa0400000);
	CA_PCIE_WRITE(0xa030000c, 0x0);
	CA_PCIE_WRITE(0xa0300010, 0xafffffff);
	CA_PCIE_WRITE(0xa0300014, 0xa0400000);
	CA_PCIE_WRITE(0xa0300018, 0x0);
	CA_PCIE_WRITE(0xa0300000, 0x0);
	CA_PCIE_WRITE(0xa0300004, 0x80000000);
	CA_PCIE_WRITE(0xa0300004, 0x80000000);
	CA_PCIE_WRITE(0xa0000010, 0x0);
	CA_PCIE_WRITE(0xa00008bc, 0x1);
	writew(0x604, 0xa000000a);
	CA_PCIE_WRITE(0xa00008bc, 0x0);
	CA_PCIE_WRITE(0xa000080c, 0x3020f);

	// setup pcie1 rc
	CA_PCIE_WRITE(0xf4326040,0xb0000000);
	CA_PCIE_WRITE(0xf4326044,0xb0000FFF);
	CA_PCIE_WRITE(0xf4326048,0xb0300000);
	CA_PCIE_WRITE(0xb0000710,0x00030120);
	CA_PCIE_WRITE(0xb000080C,0x0003020F);
	CA_PCIE_WRITE(0xb00008CC,0x32000000);
	CA_PCIE_WRITE(0xb0000010,0x00000004);
	CA_PCIE_WRITE(0xb0000014,0x00000000);
	CA_PCIE_WRITE(0xb000003C,0x000001FF);
	CA_PCIE_WRITE(0xb0000018,0x00000000);
	CA_PCIE_WRITE(0xb0000018,0x00010100);
	CA_PCIE_WRITE(0xb0000020,0xb000b000);
	CA_PCIE_WRITE(0xb0000004,0x00100000);
	CA_PCIE_WRITE(0xb0000004,0x00100107);

	// pcie1 outbound atu
	CA_PCIE_WRITE(0xb0300008, 0xb0400000);
	CA_PCIE_WRITE(0xb030000c, 0x0);
	CA_PCIE_WRITE(0xb0300010, 0xbfffffff);
	CA_PCIE_WRITE(0xb0300014, 0xb0400000);
	CA_PCIE_WRITE(0xb0300018, 0x0);
	CA_PCIE_WRITE(0xb0300000, 0x0);
	CA_PCIE_WRITE(0xb0300004, 0x80000000);
	CA_PCIE_WRITE(0xb0300004, 0x80000000);
	CA_PCIE_WRITE(0xb0000010, 0x0);
	CA_PCIE_WRITE(0xb00008bc, 0x1);
	writew(0x604, 0xb000000a);
	CA_PCIE_WRITE(0xb00008bc, 0x0);
	CA_PCIE_WRITE(0xb000080c, 0x3020f);

	// setup pcie2 rc
	CA_PCIE_WRITE(0xf4327040,0xc0000000);
	CA_PCIE_WRITE(0xf4327044,0xc0000FFF);
	CA_PCIE_WRITE(0xf4327048,0xc0300000);
	CA_PCIE_WRITE(0xc0000710,0x00030120);
	CA_PCIE_WRITE(0xc000080C,0x0003020F);
	CA_PCIE_WRITE(0xc00008CC,0x32000000);
	CA_PCIE_WRITE(0xc0000010,0x00000004);
	CA_PCIE_WRITE(0xc0000014,0x00000000);
	CA_PCIE_WRITE(0xc000003C,0x000001FF);
	CA_PCIE_WRITE(0xc0000018,0x00000000);
	CA_PCIE_WRITE(0xc0000018,0x00010100);
	CA_PCIE_WRITE(0xc0000020,0xc000c000);
	CA_PCIE_WRITE(0xc0000004,0x00100000);
	CA_PCIE_WRITE(0xc0000004,0x00100107);

	// pcie2 outbound atu
	CA_PCIE_WRITE(0xc0300008, 0xc0400000);
	CA_PCIE_WRITE(0xc030000c, 0x0);
	CA_PCIE_WRITE(0xc0300010, 0xcfffffff);
	CA_PCIE_WRITE(0xc0300014, 0xc0400000);
	CA_PCIE_WRITE(0xc0300018, 0x0);
	CA_PCIE_WRITE(0xc0300000, 0x0);
	CA_PCIE_WRITE(0xc0300004, 0x80000000);
	CA_PCIE_WRITE(0xc0300004, 0x80000000);
	CA_PCIE_WRITE(0xc0000010, 0x0);
	CA_PCIE_WRITE(0xc00008bc, 0x1);
	writew(0x604, 0xc000000a);
	CA_PCIE_WRITE(0xc00008bc, 0x0);
	CA_PCIE_WRITE(0xc000080c, 0x3020f);

	CA_PCIE_WRITE(0xf4325018,0x00000021);
	CA_PCIE_WRITE(0xf4326018,0x00000021);
	CA_PCIE_WRITE(0xf4327018,0x00000021);
	mdelay(20);

	status0 = readl(0xf4325000);
	speed0 = readl(0xa0000080);
	status1 = readl(0xf4326000);
	speed1 = readl(0xb0000080);
	status2 = readl(0xf4327000);
	speed2 = readl(0xc0000080);

	printf("pcie0 link status %x, speed %x\n", status0, speed0);
	printf("pcie1 link status %x, speed %x\n", status1, speed1);
	printf("pcie2 link status %x, speed %x\n", status2, speed2);

	link_status[0] = status0;
	link_status[1] = status1;
	link_status[2] = status2;

	link_speed[0] = speed0;
	link_speed[1] = speed1;
	link_speed[2] = speed2;

	if (status0 != 0x200) {
		printf("pcie0 not linked\n");
		rc = -1;
		ret[0] = -1;
	}

	if (status1 != 0x200) {
		printf("pcie1 not linked\n");
		rc = -1;
		ret[1] = -1;
	}
/*
	if (status2 != 0x200) {
		printf("pcie2 not linked\n");
		rc = -1;
		ret[2] = -1;
	}
*/
	if ((speed0 & 0x00ff0000) != 0x00220000) {
		printf("pcie0 speed not correct\n");
		rc = -1;
		ret[0] += -2;
	}
	if ((speed1 & 0x00ff0000) != 0x00220000) {
		printf("pcie1 speed not correct\n");
		rc = -1;
		ret[1] += -2;
	}
/*	if ((speed2 & 0x00ff0000) != 0x00120000) {
		printf("pcie2 speed not correct\n");
		rc = -1;
		ret[2] += -2;
	}
*/
	CA_PCIE_WRITE(0xa0000820, 0x3e6ef000);
	CA_PCIE_WRITE(0xa0000824, 0);
	writel(0xfc1fc00, 0xf4325004);
	writel(0xfc1fd00, 0xf4325004);

	CA_PCIE_WRITE(0xb0000820, 0x3e6ef000);
	CA_PCIE_WRITE(0xb0000824, 0);
	writel(0xfc1fc00, 0xf4326004);
	writel(0xfc1fd00, 0xf4326004);

	CA_PCIE_WRITE(0xc0000820, 0x3e6ef000);
	CA_PCIE_WRITE(0xc0000824, 0);
	writel(0xfc1fc00, 0xf4327004);
	writel(0xfc1fd00, 0xf4327004);

	CA_PCIE_WRITE(0xa0300208, 0xa0001000);
	CA_PCIE_WRITE(0xa030020c, 0x0);
	CA_PCIE_WRITE(0xa0300210, 0xa0001fff);
	CA_PCIE_WRITE(0xa0300214, 0x1000000);
	CA_PCIE_WRITE(0xa0300218, 0x0);
	CA_PCIE_WRITE(0xa0300200, 0x4);
	CA_PCIE_WRITE(0xa0300204, 0x80000000);
	val = readl(0xa0001000);
	vendor_id[0] = val & 0xffff;
	printf("read pcie0 device id %x\n", val);

	CA_PCIE_WRITE(0xa0300208, 0xa0003000);
	CA_PCIE_WRITE(0xa030020c, 0x0);
	CA_PCIE_WRITE(0xa0300210, 0xa02fffff);
	CA_PCIE_WRITE(0xa0300214, 0x0);
	CA_PCIE_WRITE(0xa0300218, 0x0);
	CA_PCIE_WRITE(0xa0300200, 0x2);
	CA_PCIE_WRITE(0xa0300204, 0x80000000);

	CA_PCIE_WRITE(0xb0300208, 0xb0001000);
	CA_PCIE_WRITE(0xb030020c, 0x0);
	CA_PCIE_WRITE(0xb0300210, 0xb0001fff);
	CA_PCIE_WRITE(0xb0300214, 0x1000000);
	CA_PCIE_WRITE(0xb0300218, 0x0);
	CA_PCIE_WRITE(0xb0300200, 0x4);
	CA_PCIE_WRITE(0xb0300204, 0x80000000);
	val = readl(0xb0001000);
	vendor_id[1] = val & 0xffff;
	printf("read pcie1 device id %x\n", val);

	CA_PCIE_WRITE(0xb0300208, 0xb0003000);
	CA_PCIE_WRITE(0xb030020c, 0x0);
	CA_PCIE_WRITE(0xb0300210, 0xb02fffff);
	CA_PCIE_WRITE(0xb0300214, 0x0);
	CA_PCIE_WRITE(0xb0300218, 0x0);
	CA_PCIE_WRITE(0xb0300200, 0x2);
	CA_PCIE_WRITE(0xb0300204, 0x80000000);

	CA_PCIE_WRITE(0xc0300208, 0xc0001000);
	CA_PCIE_WRITE(0xc030020c, 0x0);
	CA_PCIE_WRITE(0xc0300210, 0xc0001fff);
	CA_PCIE_WRITE(0xc0300214, 0x1000000);
	CA_PCIE_WRITE(0xc0300218, 0x0);
	CA_PCIE_WRITE(0xc0300200, 0x4);
	CA_PCIE_WRITE(0xc0300204, 0x80000000);
	val = readl(0xc0001000);
	vendor_id[2] = val & 0xffff;
	printf("read pcie2 device id %x\n", val);

	CA_PCIE_WRITE(0xc0300208, 0xc0003000);
	CA_PCIE_WRITE(0xc030020c, 0x0);
	CA_PCIE_WRITE(0xc0300210, 0xc02fffff);
	CA_PCIE_WRITE(0xc0300214, 0x0);
	CA_PCIE_WRITE(0xc0300218, 0x0);
	CA_PCIE_WRITE(0xc0300200, 0x2);
	CA_PCIE_WRITE(0xc0300204, 0x80000000);

	return rc;
}
#endif

ft2_ret_t pcie_device_scan(int bus, uint8_t *link, uint16_t *gen, uint16_t *lane, uint16_t *vendor_id)
{
	pci_dev_t bdf;
	uint16_t speed, device_id;

	FT2_DBG("BUS #%d:\n", bus);

	bdf = PCI_BDF(bus, 0, 0); /* PCIe#0 host */
	pci_read_config_word(bdf, 0x82, &speed);
	*link = speed == 0xffff ? 0 : 1;
	*gen = speed & 0x000f;
	*lane = (speed & 0x03f0) >> 4;
	FT2_DBG("speed = 0x%04x, link = 0x%x, gen = 0x%x, lane = 0x%x\n", speed, *link, *gen, *lane);

	bdf = PCI_BDF(bus + 1, 0, 0); /* PCIe#0 device */
	pci_read_config_word(bdf, PCI_VENDOR_ID, vendor_id);
	pci_read_config_word(bdf, PCI_DEVICE_ID, &device_id);
	FT2_DBG("vendor_id = 0x%04x, device_id = 0x%04x\n", *vendor_id, device_id);

	return FT2_OK;
}

ft2_ret_t ca_ft2_pcie_test(ft2_record_t *record)
{
	ft2_ret_t rc = FT2_OK;
	uint32_t i, link_status[3], link_speed[3], vendor_id[3];
	int ret[3];
	uint8_t link;
	uint16_t gen, lane, vendor;

#if (PCIE_PORT_MASK > 0)
	printf("FT2_PARAM_PCIE test ...\n");

	record->entry_mask = PCIE_PORT_MASK;
	record->start = get_timer(0);

#if defined(CONFIG_TARGET_VENUS)
	link_status[0] = link_status[1] = link_status[2] = 0;
	link_speed[0] = link_speed[1] = link_speed[2] = 0;
	ret[0] = ret[1] = ret[2] = 0;
	vendor_id[0] = vendor_id[1] = vendor_id[2] = 0;

	rc = pcie_test(link_status, link_speed, ret, vendor_id);

	FT2_DBG("%s: link_status0-2 = 0x%x-0x%x-0x%x\n", __func__, link_status[0], link_status[1], link_status[2]);
	FT2_DBG("%s: link_speed0-2 = 0x%x-0x%x-0x%x\n", __func__, link_speed[0], link_speed[1], link_speed[2]);
	FT2_DBG("%s: ret0-2 = %d-%d-%d\n", __func__,  ret[0], ret[1], ret[2]);
	FT2_DBG("PCIE0 link status %s\n", (ret[0] == -1) ? "FAIL" : "PASS");
	FT2_DBG("PCIE1 link status %s\n", (ret[1] == -1) ? "FAIL" : "PASS");
	FT2_DBG("PCIE2 link status %s\n", (ret[2] == -1) ? "FAIL" : "PASS");

	FT2_DBG("PCIE0 link speed %s\n", (ret[0] < -1) ? "FAIL" : "PASS");
	FT2_DBG("PCIE1 link speed %s\n", (ret[1] < -1)  ? "FAIL" : "PASS");
	FT2_DBG("PCIE2 link speed %s\n", (ret[2] < -1)? "FAIL" : "PASS");

	for (i = 0; i < PCIE_PORT_NUM; i++) {
		if (!((record->entry_mask >> i) & 0x1))
			continue;
		record->entry_name[i] = pcie_port_name[i];
		record->entry_test[i] = pcie_test_items[i];

		/* check link */
		if (record->entry_test[i] & E_PCIE_LINK)
			record->entry_rlt[i] |= (ret[i] == -1) ? E_PCIE_LINK : 0;
		/* check speed */
		if (record->entry_test[i] & E_PCIE_LANE)
			record->entry_rlt[i] |= (ret[i] < -1) ? E_PCIE_LANE : 0;
		/* check vendor id */
		if (record->entry_test[i] & E_PCIE_VENDOR)
			record->entry_rlt[i] |= (vendor_id[i] != pcie_vendor_id[i]) ? E_PCIE_VENDOR: 0;

		rc |= (record->entry_rlt[i]) ? FT2_PCIE: FT2_OK;
	}
#else
	for (i = 0; i < PCIE_PORT_NUM; i++) {
		if (!((record->entry_mask >> i) & 0x1))
			continue;
		record->entry_name[i] = pcie_port_name[i];
		record->entry_test[i] = pcie_test_items[i];

		pcie_device_scan(i*2, &link, &gen, &lane, &vendor);
		/* check link */
		if (record->entry_test[i] & E_PCIE_LINK)
			record->entry_rlt[i] |= (link) ? 0 : E_PCIE_LINK;
		/* check gen */
		if (record->entry_test[i] & E_PCIE_GEN)
			record->entry_rlt[i] |= (gen == pcie_target_link[i][0]) ? 0 : E_PCIE_GEN;
		/* check lane */
		if (record->entry_test[i] & E_PCIE_LANE)
			record->entry_rlt[i] |= (lane == pcie_target_link[i][1]) ? 0 : E_PCIE_LANE;
		/* check vendor id */
		if (record->entry_test[i] & E_PCIE_VENDOR)
			record->entry_rlt[i] |= (vendor == pcie_vendor_id[i]) ? 0 : E_PCIE_VENDOR;

		rc |= (record->entry_rlt[i]) ? FT2_PCIE: FT2_OK;
	}
#endif

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc ? FT2_PCIE : FT2_OK;
#else
	printf("No PCIE test port\n");
	return FT2_PCIE;
#endif
}

static ft2_ret_t ca_ft2_ldma_test(ft2_record_t *record)
{
	/*  cortex cores and tensilica cores */
#define LDMA_QUEUE_COUNT 16
#define LDMA_BASE_ADDR LDMA_LDMA_QUEUE_DESCRIPTOR_ACCESS
#define RCPU_LDMA_BASE_ADDR LDMA_RCPU_LDMA_QUEUE_DESCRIPTOR_ACCESS
#define LDMA_QUEUE_DESCRIPTOR_DATA2_OFFSET 0x4
#define LDMA_QUEUE_DESCRIPTOR_DATA1_OFFSET 0x8
#define LDMA_QUEUE_DESCRIPTOR_DATA0_OFFSET 0xc
#define LDMA_QUEUE_CONTROL_OFFSET 0x10
#define LDMA_QUEUE_POINTER_REGISTER_OFFSET 0x20
#define DEST_MEM_ADDR_BASE 0x03001100
#define SRC_MEM_ADDR_BASE 0x03001000
#define DATA_BUF_SIZE 0x20

	ft2_ret_t rc = FT2_OK;
	uint8_t core, queue;
	uint32_t value, q_base_addr, addr;

#if (LDMA_CORE_MASK > 0)

	printf("FT2_PARAM_LDMA test ...\n");

	record->entry_mask = LDMA_CORE_MASK;
	record->start = get_timer(0);

	/* ldma write data */
	memset((void *)SRC_MEM_ADDR_BASE, 0x5a, DATA_BUF_SIZE);
	for (core = 0; core < LDMA_CORE_NUM; core++) {
		/* assign all queues to tensilica cores( core = 1) or cortex cores(core = 0) */
		value = (core) ? 0xffff : 0;
		CORTINA_CA_REG_WRITE(value, LDMA_LDMA_GLB_QUEUE_ASSIGN_REGISTER);
		for (queue = 0; queue < LDMA_QUEUE_COUNT; queue++) {
			record->entry_test[core] |= 0x1 << queue;
			q_base_addr =  (core) ? (RCPU_LDMA_BASE_ADDR + APB1_LDMA_RCPU_LDMA_QUEUE_STRIDE * queue) : (LDMA_BASE_ADDR + APB1_LDMA_LDMA_QUEUE_STRIDE * queue);
			/* enable queue */
			addr = q_base_addr + LDMA_QUEUE_CONTROL_OFFSET;
			CORTINA_CA_REG_WRITE(0x80000000, addr);
			/* set buffer size in bytes */
			addr = q_base_addr + LDMA_QUEUE_DESCRIPTOR_DATA2_OFFSET;
			CORTINA_CA_REG_WRITE(DATA_BUF_SIZE, addr);
			/* set destination addres */
			addr = q_base_addr + LDMA_QUEUE_DESCRIPTOR_DATA1_OFFSET;
			CORTINA_CA_REG_WRITE(DEST_MEM_ADDR_BASE + DATA_BUF_SIZE * (queue + LDMA_QUEUE_COUNT*core), addr);
			/* set source addres */
			addr = q_base_addr + LDMA_QUEUE_DESCRIPTOR_DATA0_OFFSET;
			CORTINA_CA_REG_WRITE(SRC_MEM_ADDR_BASE, addr);
		}
	}
	/* check data */
	for (core = 0; core < LDMA_CORE_NUM; core++) {
		for (queue = 0; queue < LDMA_QUEUE_COUNT; queue++) {
			addr = DEST_MEM_ADDR_BASE + DATA_BUF_SIZE * (queue + LDMA_QUEUE_COUNT*core);
			if (memcmp((void *)SRC_MEM_ADDR_BASE, addr, DATA_BUF_SIZE)) {
				record->entry_rlt[core] |= 0x1 << queue;
				FT2_MOD("data fail: core %d, queue %d, dest_addr = 0x%08x\n", core, queue, addr);
			}
		}
	}

	rc = record->entry_rlt[0] | record->entry_rlt[1];

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc ? FT2_LDMA: FT2_OK;
#else
	printf("No LDMA test\n");
	return FT2_LDMA;
#endif
}

static ft2_ret_t ca_ft2_uuid_test(ft2_record_t *record, ca_uint64 uuid)
{
	ft2_ret_t rc = FT2_OK;
	uint8_t idx;
	ca_uint64 crc, wafer_id, x, y, crc_exp;
	ca_ft2_cfg_t cfg_data;

#if (UUID_MASK > 0)
	printf("FT2_PARAM_UUID test ...\n");

	record->start = get_timer(0);
	if (uuid) {
		idx = 0;
		record->entry_mask = 0x1 << idx;
	} else {
		/* get uuid */
		idx = 1;
		FT2_CFG_ACCESS(FT2_CONFIG_ALL_LOAD, &cfg_data);
		uuid = cfg_data.uuid;
		record->entry_mask |= 0x1 << idx;
	}
	record->entry_test[idx] = 1;

	/* caculate crc */
	crc = uuid >> 21;
	wafer_id = (uuid >> 16) & 0x1f;
	x = uuid & 0xff;
	y = (uuid >> 8) & 0xff;
	crc_exp = (2039+1) - ((x*100000+y*100+wafer_id)*10000)%2039;
	FT2_DBG("uuid = 0x%08x, wafer_id = %d, x = %d, y = %d, crc = %d, crc_exp = %d\n",
		(uint32_t) uuid, (uint32_t)wafer_id, (uint32_t)x, (uint32_t)y, (uint32_t)crc, (uint32_t)crc_exp);

	/* check crc */
	if (crc != crc_exp) {
		FT2_MOD("crc fail: uuid = 0x%08x, wafer_id = %d, x = %d, y = %d, crc = %d, crc_exp = %d\n",
			(uint32_t)uuid, (uint32_t)wafer_id, (uint32_t)x, (uint32_t)y, (uint32_t)crc, (uint32_t)crc_exp);
		record->entry_rlt[idx] = 0x1;
	}

	rc = record->entry_rlt[0] | record->entry_rlt[1];

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc ? FT2_UUID: FT2_OK;
#else
	printf("No UUID test\n");
	return FT2_UUID;
#endif
}

ft2_ret_t ca_ft2_nand_test(ft2_record_t *record)
{
	int rc = 0;
	char *value;

#if (NAND_MASK > 0)

	printf("FT2_PARAM_NAND test ...\n");

	record->entry_mask = NAND_MASK;
	record->start = get_timer(0);

	record->entry_test[0] = 1;

	//if ((value = getenv("FT_CHK")) == NULL)
		record->entry_rlt[0] = 1;

	FT2_DBG("FT_CHK = %s\n", value);
	if ((!record->entry_rlt[0])  && strcmp("OK", value))
		record->entry_rlt[0] = 1;

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;

	rc = record->entry_rlt[0];
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc ? FT2_NAND : FT2_OK;
#else
	printf("No NAND test\n");
	return FT2_NAND;
#endif
}

ft2_ret_t ca_ft2_ssp_test(ft2_record_t *record)
{
	int cmd_ret;
	ft2_ret_t rc = FT2_OK;

#if (SSP_MASK > 0)

	printf("FT2_PARAM_SSP test ...\n");

	record->entry_mask = 1;
	record->start = get_timer(0);
	record->entry_test[0] = 1;

	/* SSP init here ??? */
	/*
	sound_init(NULL);
	if ((cmd_ret = sound_play(1000, 49152000))) { //sound_beep()
		rc = FT2_SSP;
		record->entry_rlt[0] = 0x1;
	}
	*/

	FT2_DBG("%s: cmd_ret = %d, rc = 0x%x\n", __func__, cmd_ret, rc);

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc ? FT2_SSP : FT2_OK;
#else
	printf("No SSP test\n");
	return FT2_SSP;
#endif
}

ft2_ret_t ca_ft2_dyingasp_test(ft2_record_t *record)
{
	uint32_t val;
	ft2_ret_t rc = FT2_OK;

#if (DYINGASP_MASK > 0)

	printf("FT2_PARAM_DYINGASP test ...\n");

	record->entry_mask = 1;
	record->start = get_timer(0);
	record->entry_test[0] = 1;

	/* enable dying gasp here ??? */
	CORTINA_CA_REG_WRITE(0x105, PER_DYING_GASP);

	/* clear dying gasp interrupt */
	CORTINA_CA_REG_WRITE(0x1, PER_DG_INT_0);

	gpio_direction_output(15, 0);
	mdelay(10);
	gpio_direction_output(15, 1);

	mdelay(10);
	val = CORTINA_CA_REG_READ(PER_DG_INT_0);

	if (val)
		CORTINA_CA_REG_WRITE(0x1, PER_DG_INT_0);
	else {
		record->entry_rlt[0] = 0x1;
		rc = FT2_DYINGASP;
	}

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc ? FT2_DYINGASP : FT2_OK;
#else
	printf("No DYINGASP test\n");
	return FT2_DYINGASP;
#endif
}

ft2_ret_t ca_ft2_ddr_zq_test(ft2_record_t *record)
{
	int rc = 0;
	uint32_t value;

#if (DDR_ZQ_MASK > 0)

	printf("FT2_PARAM_DDR_ZQ test ...\n");

	record->entry_mask = DDR_ZQ_MASK;
	record->start = get_timer(0);

	record->entry_test[0] = TEST_DDR_R480_ZQ | TEST_DDR_CMD_ZQ | TEST_DDR_DQ_ZQ;

	value = CORTINA_CA_REG_READ(BL2_TEST_RLT);
	FT2_DBG("BL2_TEST_RLT = 0x%08x", value);
	if ((value >> 24) == 0xf2) {
		record->entry_rlt[0] |= value & BL2_TEST_DDR_R480_ZQ ? TEST_DDR_R480_ZQ : 0;
		record->entry_rlt[0] |= value & BL2_TEST_DDR_CMD_ZQ ? TEST_DDR_CMD_ZQ : 0;
		record->entry_rlt[0] |= value & BL2_TEST_DDR_DQ_ZQ ? TEST_DDR_DQ_ZQ : 0;
	} else {
		record->entry_rlt[0] = BL2_TEST_DDR_R480_ZQ | BL2_TEST_DDR_CMD_ZQ | BL2_TEST_DDR_DQ_ZQ;
	}
	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;

	rc = record->entry_rlt[0];
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc ? FT2_DDR_ZQ : FT2_OK;
#else
	printf("No DDR ZQ test\n");
	return FT2_DDR_ZQ;
#endif
}

void test_prepare(ft2_record_t *records, int record_num, /*uint32_t test,*/ ca_ft2_cfg_t *cfg_data, int *have_record)
{
	static int gpio_init = 0;
	int i;
	uint8_t port, phy_dev;
	ca_ft2_cfg_t cfg_zero;
	GLOBAL_PIN_MUX_t g_pin_mux;

	printf("*** test prepare ...\n\n");

	memset(records, 0, record_num * sizeof(ft2_record_t));
	for (i = 0; i < record_num; i++)
		(records + i)->name = ft2_module_name[i];

#if defined(CONFIG_TARGET_SATURN_ASIC)
	printf("CONFIG_TARGET_SATURN_ASIC\n");
	CORTINA_CA_REG_WRITE(0x3000004, GLOBAL_PIN_MUX);
	CORTINA_CA_REG_WRITE(0x220, GLOBAL_GPIO_MUX_0);
#endif

#if defined(CONFIG_CA8279_FT2)
	printf("CONFIG_CA8279_FT2\n");
	CORTINA_CA_REG_WRITE(0x02800000, SATURN_GLOBAL_PIN_MUX);
	CORTINA_CA_REG_WRITE(0x80088002, GLOBAL_PIN_MUX);
	CORTINA_CA_REG_WRITE(0x2400, GLOBAL_GPIO_MUX_0);
	/* init saturn gpio */
	if (!gpio_init) {
		ca77xx_gpio_bank_init(SATURN_PER_GPIO0_CFG, SATURN_GLOBAL_GPIO_MUX_0, 4);
		gpio_init = 1;
	}
#endif

#if defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_VENUS)
	/* set pins as gpio 3, 10 */
	g_pin_mux.wrd = CORTINA_CA_REG_READ(GLOBAL_PIN_MUX);
	g_pin_mux.bf.iomux_uart_3_enable = 0;
	g_pin_mux.bf.iomux_sd_mmc_resetn  = 0;
	CORTINA_CA_REG_WRITE(g_pin_mux.wrd, GLOBAL_PIN_MUX);
	/* turn off usb power */
	ca_ft2_gpio_mux_set(GPIO_USB_PWR);
	gpio_direction_output(GPIO_USB_PWR, 0);

#endif

	if (cfg_data != NULL && have_record != NULL) {
		FT2_CFG_ACCESS(FT2_CONFIG_ALL_LOAD, cfg_data);

		memset(&cfg_zero, 0, sizeof(ca_ft2_cfg_t));

		if (memcmp(&cfg_zero.param, &cfg_data->param, sizeof(ca_phy_param_t)))
			*have_record = 1;
		else if (memcmp(&cfg_zero.phy_patch, &cfg_data->phy_patch, sizeof(cfg_zero.phy_patch)))
			*have_record = 1;
		else if (memcmp(&cfg_zero.phy_k_data, &cfg_data->phy_k_data, sizeof(cfg_zero.phy_k_data)))
			*have_record = 1;
		else
			*have_record = 0;
	}

	return;
}

void system_info(uint32_t uuid)
{
	uint32_t jtagid = 0;
	uint32_t mem_type = 0, mem_size = 0, mem_mode = 0, mem_freq = 0;

	printf("### SYSTEM INFO:\n");
	/* get hw id */
	jtagid = CORTINA_CA_REG_READ(GLOBAL_JTAG_ID);
	/* get ddr info */
	mem_type = CORTINA_CA_REG_READ(MEM_DDR_TYPE);
	mem_freq = CORTINA_CA_REG_READ(MEM_DDR_FREQ);
	mem_size = CORTINA_CA_REG_READ(MEM_DDR_SIZE);
	mem_mode = CORTINA_CA_REG_READ(MEM_BIT_MODE);

	printf("[UUID: 0x%08X]\n", uuid);
	printf("{HW ID: 0x%04X}\n", CHIP_ID(jtagid));
	printf("{SW Ver: %d.%d.%d}\n", (TEST_PROG_VER >> 16) & 0xf, (TEST_PROG_VER>>8) & 0xf, TEST_PROG_VER & 0xff);
	printf("{Memory: DDR%d/%dMHz/%dMB/%dbits}\n", mem_type, mem_freq, mem_size, mem_mode*8);

	return;
}

void test_result(ft2_record_t *records, int record_num, uint32_t test, ca_ft2_cfg_t *cfg_data)
{
	uint32_t module = 0;
	uint32_t result = 0;
	uint32_t error = 0, *ptr;
	int i, j;

	if (cfg_data != NULL)
		system_info(cfg_data->uuid);
	printf("### TEST RESULT:\n");

	for (i = 0; i < record_num; i++) {
		if ((records + i)->entry_mask)
			module++;
		else
			continue;

		if ((records + i)->module_rlt) {
			result |= (1 << i);
			error++;
		}
	}
	if (module > 0)
		printf("{CHIP:%s}\n", result ? "FAIL" : "PASS");
	printf("%s-T:0x%08X/E:0x%08X\n", CHIP_NAME, test, result);
	printf("=== Total %d %s %ld.%03ld secs\n", module,
	       module > 1 ? "tests take" : "test takes",
	       ft2_finish/1000, ft2_finish%1000);

	for (i = 0; i < record_num; i++) {
		if ((records + i)->entry_mask)
			printf("{%s:%s}\n", (records + i)->name,
			       (records + i)->module_rlt ? "FAIL" : "PASS");

		for (j = 0; j < MODULE_MAX_ENTRY; j++)
			if ((records + i)->entry_mask & (1 << j)) {
				printf("%s-T:0x%08X-E:0x%08X\n",
				       ((records + i)->entry_name[j] == NULL)?(records + i)->name:(records + i)->entry_name[j],
				       (records + i)->entry_test[j],
				       (records + i)->entry_rlt[j]);
			}

		if ((records + i)->entry_mask) {
			printf("=== %s test takes %ld.%03d secs\n",
			       (records + i)->name,
			       (records + i)->finish/1000, (int)(records + i)->finish%1000);
		}
	}

	/* load ft2 cfg entry 3 */
	if (cfg_data != NULL) {
		FT2_CFG_ACCESS(FT2_CONFIG_ALL_LOAD, cfg_data);
		ptr = (uint32_t *)cfg_data;
		printf("{CFG ENTRY 3: 0x%08x}\n", ptr[3]);
	}

	if (module > 0) {
		if (error)
			printf("%d Failure\n", error);
		else
			printf("No Failure\n");
	}

	return;
}

int do_ft2_phy_calibration(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_phy_calibration(&ft2_records[FT2_PARAM_GPHY_K]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_GPHY_K), NULL);
	return 0;
}

int do_ft2_rc_k_diff(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc == 1) {
		printf("rc_k_diff = %d\n", rc_k_diff);
	} else {
		rc_k_diff = simple_strtol(argv[1], NULL, 10);
	}
	return 0;
}

int do_ft2_r_k_diff(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc == 1) {
		printf("r_k_diff = %d\n", r_k_diff);
	} else {
		r_k_diff = simple_strtol(argv[1], NULL, 10);
	}
	return 0;
}

int do_ft2_amp_volt_aid(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc == 1) {
		printf("amp_volt_aid = %d\n", amp_volt_aid);
	} else {
		amp_volt_aid = simple_strtol(argv[1], NULL, 10);
	}
	return 0;
}

int do_ft2_ado_verify_times(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc == 1) {
		printf("ado_verify_times = %d\n", ado_verify_times);
	} else {
		ado_verify_times = simple_strtol(argv[1], NULL, 10);
	}
	return 0;
}

int do_ft2_ado_volt_diff(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc == 1) {
		printf("ado_volt_diff = %d\n", ado_volt_diff);
	} else {
		ado_volt_diff = simple_strtol(argv[1], NULL, 10);
	}
	return 0;
}

int do_ft2_ado_over_times(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc == 1) {
		printf("ado_over_times = %d\n", ado_over_times);
	} else {
		ado_over_times = simple_strtol(argv[1], NULL, 10);
	}
	return 0;
}

int do_ft2_ado_current(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	phy_ado_current_t curr_type;
	char *current[PHY_ADO_CURR_MAX] = {"norm", "low", "med", "high"};
	uint8_t i, port;

	if (argc > 1) {
		for (curr_type = PHY_ADO_CURR_NORM; curr_type < PHY_ADO_CURR_MAX; curr_type++) {
			if (0 == strcmp(argv[1], current[curr_type])) {
				ado_curr = curr_type;
				break;
			}
		}
	}

	printf("ado_curr = %s\n", current[ado_curr]);
	for (i = 0; (port = ge_ports[i]) != INVALID_PORT; i++)
		ca_ft2_phy_ado_current(port, ado_curr);

	return 0;
}

int do_ft2_gphy_snr_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_gphy_snr_test(&ft2_records[FT2_PARAM_GPHY_SNR]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_GPHY_SNR), NULL);
	return 0;
}

int do_ft2_ldma_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_ldma_test(&ft2_records[FT2_PARAM_LDMA]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_LDMA), NULL);
	return 0;
}

int do_ft2_serdes_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_serdes_test(&ft2_records[FT2_PARAM_SERDES]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_SERDES), NULL);
	return 0;
}

int do_ft2_pon_ben_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_pon_ben_test(&ft2_records[FT2_PARAM_PON_BEN]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_PON_BEN), NULL);
	return 0;
}

int do_ft2_pon_sram_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_pon_sram_test(&ft2_records[FT2_PARAM_PON_SRAM]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_PON_SRAM), NULL);
	return 0;
}

int do_ft2_intra_xfi_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_intra_xfi_test(&ft2_records[FT2_PARAM_INTRA_XFI]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_INTRA_XFI), NULL);
	return 0;
}

int do_ft2_gmii_loopback_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_gmii_loopback(&ft2_records[FT2_PARAM_GMII_PORT_LOOPBACK]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_GMII_PORT_LOOPBACK), NULL);
	return 0;
}

int do_ft2_gmii_open_loopback_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_gmii_open_loopback(&ft2_records[FT2_PARAM_GMII_PORT_OPEN_LOOPBACK]);
	printf("%s: WAIT test complete...\n", __func__);
	mdelay(10000);
	ca_ft2_gmii_open_loopback_result(&ft2_records[FT2_PARAM_GMII_PORT_OPEN_LOOPBACK]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_GMII_PORT_OPEN_LOOPBACK), NULL);
	return 0;
}

int do_ft2_rgmii_loopback_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_rgmii_loopback(&ft2_records[FT2_PARAM_RGMII_PORT_LOOPBACK]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_RGMII_PORT_LOOPBACK), NULL);
	return 0;
}

int do_ft2_gpio_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_gpio_test(&ft2_records[FT2_PARAM_GPIO]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_GPIO), NULL);
	return 0;
}

int do_ft2_otp_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_otp_test(&ft2_records[FT2_PARAM_OTP]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_OTP), NULL);
	return 0;
}

int do_ft2_cpu_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_cpu_test(&ft2_records[FT2_PARAM_CPU]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_CPU), NULL);
	return 0;
}

int do_ft2_rtc_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_rtc_test(&ft2_records[FT2_PARAM_RTC]);
	udelay(5000000);
	ca_ft2_rtc_result(&ft2_records[FT2_PARAM_RTC]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_RTC), NULL);
	return 0;
}

int do_ft2_ddr_zq_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_ddr_zq_test(&ft2_records[FT2_PARAM_DDR_ZQ]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_DDR_ZQ), NULL);
	return 0;
}

int do_ft2_pcie_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_pcie_test(&ft2_records[FT2_PARAM_PCIE]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_PCIE), NULL);
	return 0;
}

#if (USB_PORT_MASK > 0)

#define BIT(nr)			(1UL << (nr))
#define USB2CFG_CNTRL_OFFSET				0x00
#define USB2CFG_PHY_VAUX_RESET					BIT(31)
#define USB2CFG_BUS_CLKEN_GLAVE					BIT(30)
#define USB2CFG_BUS_CLKEN_GMASTER				BIT(29)
#define USB2CFG_BIGENDIAN_GSLAVE				BIT(28)
#define USB2CFG_HOST_PORT_POWER_CTRL_PRESENT	BIT(27)
#define USB2CFG_HOST_MSI_ENABLE					BIT(26)
#define USB2CFG_HOST_LEGACY_SMI_PCI_CMD_REG_WR	BIT(25)
#define USB2CFG_HOST_LEGACY_SMI_BAR_WR			BIT(24)
#define USB2CFG_FLADJ_30MHZ_REG_MASK(x)			((0x3f & (x)) << 16)
#define USB2CFG_PHY_VAUX_RESET_PORT2            BIT(10)
#define USB2CFG_PHY_VAUX_RESET_PORT1            BIT(9)
#define USB2CFG_PHY_VAUX_RESET_PORT0            BIT(8)
#define USB2CFG_XHCI_BUS_MASTER_ENABLE			BIT(4)

#define USB2CFG_STATUS_OFFSET				0x04
#define USB2CFG_HOST_SYSTEM_ERR					BIT(31)
#define USB2CFG_LEGACY_SMI_INTERRUPT			BIT(16)
#define USB2CFG_HOST_CURRENT_BELT_MASK(x)		(0xfff & (x))

#define USB2CFG_PORT_CONFIG_OFFSET			0x08
#define USB2CFG_HOST_DISABLE					BIT(2)
#define USB2CFG_HUB_PORT_OVERCURRENT			BIT(1)
#define USB2CFG_HUB_PORT_PERM_ATTACH			BIT(0)

#define USB2CFG_PORT_STATUS_OFFSET			0x0c
#define USB2CFG_UTMI_FSLS_LOW_POWER				BIT(1)
#define USB2CFG_HUB_VBUS_CTRL					BIT(0)

#define USB2CFG_PHY_PORT0_CONFIG_OFFSET		0x10
#define USB2CFG_PHY_PORT1_CONFIG_OFFSET		0x18
#define USB2CFG_PHY_PORT2_CONFIG_OFFSET		0x20
#define USB2CFG_PHY_PORT_VSTATUS_IN_MASK(x)		((0xff & (x)) << 24)
#define USB2CFG_PHY_PORT_VCNTRL_MASK(x)		((0xf & (x)) << 20)
#define USB2CFG_PHY_PORT_VLOADM					BIT(19)
#define USB2CFG_PHY_PORT_BY_PASS_ON				BIT(18)
#define USB2CFG_PHY_PORT_LF_PD_R_EN				BIT(9)
#define USB2CFG_PHY_PORT_CLKTSTEN				BIT(8)
#define USB2CFG_PHY_PORT_DPPULLDOWN				BIT(6)
#define USB2CFG_PHY_PORT_DMPULLDOWN				BIT(5)
#define USB2CFG_PHY_PORT_TXBITSTUFF_ENABLE		BIT(4)
#define USB2CFG_PHY_PORT_TXBITSTUFF_ENABLE_H	BIT(3)
#define USB2CFG_PHY_PORT_TX_ENABLE_N			BIT(2)
#define USB2CFG_PHY_PORT_TX_DAT					BIT(1)
#define USB2CFG_PHY_PORT_TX_SE0					BIT(0)

#define USB2CFG_PHY_PORT0_STATUS_OFFSET		0x14
#define USB2CFG_PHY_PORT1_STATUS_OFFSET		0x1c
#define USB2CFG_PHY_PORT2_STATUS_OFFSET		0x24
#define USB2CFG_PHY_PORT_VSTATUS_OUT_MASK(x)	((0xff & (x)) << 24)
#define USB2CFG_PHY_PORT_DEBUG_MASK(x)			(0xff & (x))


static int debug = 0;

static void ca_usb2_phy_write(
	u8 vstat_in_mask, u8 vctrl_mask, int port_id)
{
	u32 offset = 0;
	u32 reg_val = 0;
	u8 vctrl_mask1, vctrl_mask2;

	vctrl_mask1 = vctrl_mask & 0x0F;
	vctrl_mask2 = (vctrl_mask >> 4) & 0x0F;
	reg_val |= (USB2CFG_PHY_PORT_VSTATUS_IN_MASK(vstat_in_mask)
		| USB2CFG_PHY_PORT_VCNTRL_MASK(vctrl_mask1)
		| USB2CFG_PHY_PORT_DPPULLDOWN
		| USB2CFG_PHY_PORT_DMPULLDOWN
		| USB2CFG_PHY_PORT_VLOADM
		| USB2CFG_PHY_PORT_TX_ENABLE_N);

	if (port_id == 0)
		offset = USB2CFG_PHY_PORT0_CONFIG_OFFSET;
	else if (port_id == 1)
		offset = USB2CFG_PHY_PORT1_CONFIG_OFFSET;
	else if (port_id == 2)
		offset = USB2CFG_PHY_PORT2_CONFIG_OFFSET;
	else
		printf("The usb2phy port num is unvalid.\n");

	//printf("ca_usb2_phy_write %x %x %x\n",USB2PHY_REG + offset,vctrl_mask,vstat_in_mask);

	writel(reg_val, USB2PHY_REG + offset);
	mdelay(1);

#if 1
	if (debug)
		printf("1read U2PHY_P%d_CFG reg_val = 0x%08x\n",
			port_id, readl(USB2PHY_REG + offset));
#endif
	reg_val &= ~USB2CFG_PHY_PORT_VLOADM;
	writel(reg_val, USB2PHY_REG + offset);
#if 1
	if (debug)
		printf( "2read U2PHY_P%d_CFG reg_val = 0x%08x\n",
			port_id, readl(USB2PHY_REG + offset));
#endif
	reg_val |= USB2CFG_PHY_PORT_VLOADM;
	writel(reg_val, USB2PHY_REG + offset);
#if 1
	if (debug)
		printf("3read U2PHY_P%d_CFG reg_val = 0x%08x\n",
			port_id, readl(USB2PHY_REG + offset));
#endif
	/* Clear VCONTROL field before refilling */
	reg_val &= ~USB2CFG_PHY_PORT_VCNTRL_MASK(0xF);
	reg_val |= USB2CFG_PHY_PORT_VCNTRL_MASK(vctrl_mask2);
	writel(reg_val, USB2PHY_REG + offset);
#if 1
	if (debug)
		printf("4read U2PHY_P%d_CFG reg_val = 0x%08x\n",
			port_id, readl(USB2PHY_REG + offset));
#endif
	reg_val &= ~USB2CFG_PHY_PORT_VLOADM;
	writel(reg_val, USB2PHY_REG + offset);
#if 1
	if (debug)
		printf("5read U2PHY_P%d_CFG reg_val = 0x%08x\n",
			port_id, readl(USB2PHY_REG + offset));
#endif
	reg_val |= USB2CFG_PHY_PORT_VLOADM;
	writel(reg_val, USB2PHY_REG + offset);
#if 1
	if (debug)
		printf("6read U2PHY_P%d_CFG reg_val = 0x%08x\n",
			port_id, readl(USB2PHY_REG + offset));
#endif

}

static inline void ca_usb3_phy_write(unsigned addr, u16 data, int port_id)
{
	uint tmp_addr;
	if (port_id == 0)
		writel(data, S2USBPHY_U3PORT0 + (addr << 2));
	if (port_id == 1)
		writel(data, S3USBPHY_U3PORT1 + (addr << 2));

	mdelay(1);

#if 0
	if (port_id == 0)
		tmp_addr = S2USBPHY_U3PORT0 + (addr << 2);
	if (port_id == 1)
		tmp_addr = S3USBPHY_U3PORT1 + (addr << 2);

	printf("ca_usb3_phy_write 0x%x  0x%x 0x%x\n",tmp_addr,data,readl(tmp_addr));
#endif

}

static inline void ca_usb3_phy_read(unsigned addr, int port_id)
{
	uint tmp_addr;

#if 1
	if (port_id == 0)
		tmp_addr = S2USBPHY_U3PORT0 + (addr << 2);
	if (port_id == 1)
		tmp_addr = S3USBPHY_U3PORT1 + (addr << 2);

	printf("ca_usb3_phy_read 0x%x 0x%x\n",tmp_addr,readl(tmp_addr));
#endif

}

#define XHCI_CTRL0_MAXPOART 4
#define XHCI_CTRL1_MAXPOART 1
#define USB_DEVUCE_MAX_NUM (XHCI_CTRL0_MAXPOART+XHCI_CTRL1_MAXPOART)
#define PORTSC_PORT0 0x420
#define PORTSC_PORT1 0x430
#define PORTSC_PORT2 0x440
#define PORTSC_PORT3 0x450
#include <usb.h>

struct ft_usb_device_info_st {
	int	speed;			/* full/low/high */
	char	device_name[32];			/* manufacturer */
	u32	link;
};

struct ft_usb_device_info_st ft_usb_device_info[USB_DEVUCE_MAX_NUM];



static void ca_usb_phy_init(void)
{

	u8 *addr = phy_data_addr;
	u16 *data = phy_data_array;
	int size = PHY_DATA_SIZE;
	int index,port_id;
	GLOBAL_BLOCK_RESET_t g_blk_reset;
	GLOBAL_DPHY_RESET_t g_dphy_reset;
	GLOBAL_PHY_CONTROL_t g_phy_ctrl;

	u8 *u2_addr = u2_phy_data_addr;
	u8 *u2_data = u2_phy_data_array;

	int time = 1;
	uint32_t usb;

	/* ============================== */
	/* force reset, S2/S3 */
	#if defined(CONFIG_TARGET_PRESIDIO_ASIC)
	gpio_direction_output(GPIO_USB_PWR, 0);
	mdelay(1500);
	gpio_direction_output(GPIO_USB_PWR, 1);
	#endif

#if defined(USB_S2S3_FORCE_RESET)
	/* GLOBAL_BLOCK_RESET */
	g_blk_reset.wrd = CORTINA_CA_REG_READ(GLOBAL_BLOCK_RESET);
	g_blk_reset.bf.reset_usb = 1;
	CORTINA_CA_REG_WRITE(g_blk_reset.wrd, GLOBAL_BLOCK_RESET);
	mdelay(20);
	/* GLOBAL_DPHY_RESET */
	g_dphy_reset.wrd = CORTINA_CA_REG_READ(GLOBAL_DPHY_RESET);
	g_dphy_reset.bf.reset_dphy_s2_usb = 1;
	g_dphy_reset.bf.reset_dphy_s3_usb = 1;
	CORTINA_CA_REG_WRITE(g_dphy_reset.wrd, GLOBAL_DPHY_RESET);
	mdelay(20);
	/* GLBAL_PHY_CONTROL */
	g_phy_ctrl.wrd = CORTINA_CA_REG_READ(GLOBAL_PHY_CONTROL);
	g_phy_ctrl.bf.s2_POW_USB3 = 0;
	g_phy_ctrl.bf.s3_POW_USB3 = 0;
	g_phy_ctrl.bf.s2_combo_sel = 0;
	g_phy_ctrl.bf.s3_combo_sel = 0;
	CORTINA_CA_REG_WRITE(g_phy_ctrl.wrd, GLOBAL_PHY_CONTROL);
	mdelay(20);
#endif

	/* USB2 DPPULLDOWN/DMPULLDOWN */
	writel(0x00080064, USBCFG_U2PHY_PORT0_CONFIG);
	//udelay(time);
	writel(0x00080064, USBCFG_U2PHY_PORT1_CONFIG);
	writel(0x00080064, USBCFG_U2PHY_PORT2_CONFIG);
	mdelay(time);


#if 1
	for (index = 0; index < size; index++) {
		ca_usb3_phy_write( *(addr + index),
			*(data + index), 0);
		ca_usb3_phy_write( *(addr + index),
			*(data + index), 1);
	}

#endif
	udelay(time);

	/* ============================== */
	/* release reset */
	/* GLBAL_PHY_CONTROL */
	g_phy_ctrl.wrd = CORTINA_CA_REG_READ(GLOBAL_PHY_CONTROL);
	g_phy_ctrl.bf.s2_POW_USB3 = 1;
	g_phy_ctrl.bf.s3_POW_USB3 = 1;
	g_phy_ctrl.bf.s2_combo_sel = 1;
	g_phy_ctrl.bf.s3_combo_sel = 1;
	CORTINA_CA_REG_WRITE(g_phy_ctrl.wrd, GLOBAL_PHY_CONTROL);
	udelay(time);
	/* GLOBAL_DPHY_RESET */
	g_dphy_reset.wrd = CORTINA_CA_REG_READ(GLOBAL_DPHY_RESET);
	g_dphy_reset.bf.reset_dphy_s2_usb = 0;
	g_dphy_reset.bf.reset_dphy_s3_usb = 0;
	CORTINA_CA_REG_WRITE(g_dphy_reset.wrd, GLOBAL_DPHY_RESET);
	udelay(time);
	/* GLOBAL_BLOCK_RESET */
	g_blk_reset.wrd = CORTINA_CA_REG_READ(GLOBAL_BLOCK_RESET);
	g_blk_reset.bf.reset_usb = 0;
	CORTINA_CA_REG_WRITE(g_blk_reset.wrd, GLOBAL_BLOCK_RESET);
	udelay(time);


	/* ============================== */
	/* USBCFG_U3_CONTROL (def 0x68200010) */
	writel(0xe8200010, USBCFG_U3_CONTROL);
	udelay(time);
    /*# USBCFG_U2_CONTROL*/
	writel(0xe8200710, USBCFG_U2_CONTROL);
	udelay(time);
	mdelay(200);

	size = U2_PHY_DATA_SIZE;
	/* USB2 PHY port[0:3] calibration and initialization */
	for (port_id = 0; port_id < 3; port_id++) {
		for (index = 0; index < size ; index++) {
			ca_usb2_phy_write(*(u2_data + index),
				*(u2_addr + index), port_id);
#if defined(CONFIG_TARGET_PRESIDIO_ASIC)
			/* Verify bug for G3 REV-D chip for USB2 Port2 init */
			if (index == 10 && port_id == 1) {
				ca_usb2_phy_write(0x8D,
					*(u2_addr + index), port_id);
				}
			if (index == 11 && port_id == 1) {
				ca_usb2_phy_write(0xA1,
					*(u2_addr + index), port_id);
			}
#endif
		}
	}

	usb_host_early_init();
	mdelay(200);


	/* ============================== */
	/* check U3 LTSSM state */
	/* portsc[8:5] link state=0, [13:10] port speed */
	/* port S2 link status (port 3, SS 0) */
	/* 0xf0200440 expect 0x00xx1203 */
	/* port S3 link status (port 4, SS 1) */
	/* 0xf0200450 expect 0x00xx1203 */
	#if 0
	value = readl(0xf0200440);
	udelay(time);
	printf("0xf0200440: 0x%x\n", value);
	value = readl(0xf0200450);
	udelay(time);
	printf("0xf0200450: 0x%x\n\n", value);
	#endif
	/* =============================== */
	/* USB2 port, program after device inserted */
	/* should be programmed by the xHCI driver */
	/* PORTSC, enable port */
	//writel(0x000206f3, 0xf0200420);
	//udelay(time);
	/* PORTSC, enable port */
	//writel(0x000206f3, 0xf0200430);
    //writel(0x000206f3, 0xf0400430);
	//udelay(time);
	/* PORTSC, expect 0x00200e03, [8:5] link state=0 */
    //writel(0x000206f3, 0xf0200420);
    //writel(0x000206f3, 0xf0200430);
	#if defined(CONFIG_TARGET_PRESIDIO_ASIC)
	//gpio_direction_output(GPIO_USB_PWR, 0);
	//mdelay(300);
	#endif

	gpio_set_value(GPIO_USB_PWR, 1);
	mdelay(2000);

	writel(0x000206f3, USB_0_BASE+PORTSC_PORT0);
	writel(0x000206f3, USB_0_BASE+PORTSC_PORT1);

	writel(0x000206f3, USB_1_BASE+PORTSC_PORT0);


    mdelay(200);


	usb = readl(USB_0_BASE+PORTSC_PORT2);

	if ((usb & 0x0000FFFF) == 0x1203) {
		printf("USB3-port0:Link PASS\n\n");
		ft_usb_device_info[2].link = usb;
	} else {
		printf("USB3-port0:Link FAIL\n");
		ft_usb_device_info[2].link = 0;
		printf("0x%x : 0x%x\n\n", USB_0_BASE+PORTSC_PORT2,usb);
	}

	usb = readl(USB_0_BASE+PORTSC_PORT3);

	if ((usb & 0x0000FFFF) == 0x1203) {
		printf("USB3-port1:Link PASS\n\n");
		ft_usb_device_info[3].link = usb;
	} else {
		printf("USB3-port1:Link FAIL\n");
		ft_usb_device_info[3].link = 0;
		printf("0x%x : 0x%x\n\n", USB_0_BASE+PORTSC_PORT3,usb);
	}

	usb = readl(USB_0_BASE+PORTSC_PORT0);

	if ((usb & 0x0000FFFF) == 0x0e03) {
		printf("USB2-port0:Link PASS\n\n");
		ft_usb_device_info[0].link = usb;
	} else {
		printf("USB2-port0:Link FAIL\n");
		ft_usb_device_info[0].link = 0;
		printf("0x%x : 0x%x\n\n", USB_0_BASE+PORTSC_PORT0,usb);
	}

	usb = readl(USB_0_BASE+PORTSC_PORT1);

	if ((usb & 0x0000FFFF) == 0x0e03) {
		printf("USB2-port1:Link PASS\n\n");
		ft_usb_device_info[1].link = usb;
	} else {
		printf("USB2-port1:Link FAIL\n");
		ft_usb_device_info[1].link = 0;
		printf("0x%x : 0x%x\n\n", USB_0_BASE+PORTSC_PORT1,usb);
	}

	usb = readl(USB_1_BASE+PORTSC_PORT0);

	if ((usb & 0x0000FFFF) == 0x0e03) {
		printf("USB2-port2:Link PASS\n\n");
		ft_usb_device_info[4].link = usb;
	} else {
		printf("USB2-port2:Link FAIL\n");
		printf("0x%x : 0x%x\n\n", USB_1_BASE+PORTSC_PORT0,usb);
		ft_usb_device_info[4].link = 0;
	}

	return;
}

int usb_test(void)
{
	struct usb_device *dev = NULL;
	int rc = FT2_OK;
	int d;
	int i,index;

	memset(ft_usb_device_info,0,sizeof(ft_usb_device_info));

	usb_stop();

	ca_usb_phy_init();

	if (usb_init() < 0)
		return FT2_ERROR;

	for (d = 0; d < USB_MAX_DEVICE; d++) {
		dev = usb_get_dev_index(d);
		if (dev == NULL)
			break;

		if (dev->maxchild != 0){

			if(dev->maxchild == XHCI_CTRL0_MAXPOART){
				printf("usb controler 0 :\n");
				for (i = 0; i < dev->maxchild; i++) {
					if (dev->children[i] != NULL){
						index = dev->children[i]->portnr -1;
						printf("   port %d , vendor = %s\n",i,dev->children[i]->mf);
						memcpy(ft_usb_device_info[index].device_name,dev->children[i]->mf,32);
						printf("   port %d , speed = %d\n",i, dev->children[i]->speed);
						ft_usb_device_info[index].speed = dev->children[i]->speed;
					}

				}
			}

			if(dev->maxchild == XHCI_CTRL1_MAXPOART){
				printf("usb controler 1 :\n");

				for (i = 0; i < dev->maxchild; i++) {
					if (dev->children[i] != NULL){
						index = dev->children[i]->portnr -1 + XHCI_CTRL0_MAXPOART;
						printf("   port %d , vendor = %s\n",i,dev->children[i]->mf);
						memcpy(ft_usb_device_info[index].device_name,dev->children[i]->mf,32);
						printf("   port %d , speed = %d\n",i, dev->children[i]->speed);
						ft_usb_device_info[index].speed = dev->children[i]->speed;
					}

				}

			}

		}




		//if (strlen(dev->mf) || strlen(dev->prod) ||
		//	strlen(dev->serial))
		//	printf("%d - %s %s %s\n",d, dev->mf, dev->prod,
		//		dev->serial);
		//	printf("maxchild %d  portnr = %d \n",dev->maxchild,dev->portnr);
	}

	return rc;
}
#endif

ft2_ret_t ca_ft2_usb_test(ft2_record_t *record)
{
	int i;
	ft2_ret_t rc = FT2_OK;

#if (USB_PORT_MASK > 0)
	printf("FT2_PARAM_USB test ...\n");

	record->entry_mask = USB_PORT_MASK;
	record->start = get_timer(0);

	rc = usb_test();

	for (i = 0; i < USB_PORT_NUM; i++) {
		if (!((record->entry_mask >> i) & 0x1))
			continue;
		record->entry_name[i] = usb_port_name[i];
		record->entry_test[i] = usb_test_items[i];

		/* check link */
		if (record->entry_test[i] & E_USB_LINK)
			record->entry_rlt[i] |= (ft_usb_device_info[i].link) ? 0 : E_USB_LINK;
		/* check speed */
		if (record->entry_test[i] & E_USB_GEN)
			record->entry_rlt[i] |= (ft_usb_device_info[i].speed == usb_target_link[i]) ? 0 : E_USB_GEN;
		/* vendor id */
		if (record->entry_test[i] & E_USB_MANU)
			record->entry_rlt[i] |=  (strncmp(ft_usb_device_info[i].device_name, "SanDisk", 7)) ? E_USB_MANU : 0;

		rc |= (record->entry_rlt[i]) ? FT2_USB: FT2_OK;
	}

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return rc;
#else
	printf("No USB test port\n");
	return FT2_USB;
#endif
}

int do_ft2_usb_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_usb_test(&ft2_records[FT2_PARAM_USB]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_USB), NULL);
	return 0;
}

#if defined(CONFIG_TARGET_SATURN_ASIC)
ft2_ret_t ca_ft2_taroko_test(ft2_record_t *record)
{
	char cmd[64];
	int cmd_ret;
	ft2_ret_t rc = FT2_OK;

	record->entry_mask = TAROKO_CORE_MASK;
	record->start = get_timer(0);
	record->entry_test[0] = TEST_TAROKO_LOAD;

	/* clear counter */
	CORTINA_CA_REG_WRITE(0, TAROKO_VERIFY_ADDR);

	sprintf(cmd, "%s", "rboot");
	FT2_DBG("%s: run taroko cmd = %s\n", __func__, cmd);
	if (cmd_ret = run_command(cmd, 0)) {
		record->entry_rlt[0] = TEST_TAROKO_LOAD;
		rc = FT2_TAROKO;
	}
	FT2_DBG("%s: cmd_ret = %d, rc = 0x%x\n", __func__, cmd_ret, rc);

	return  rc ? FT2_TAROKO : FT2_OK;
}
#else
int ca_ft2_taroko_run(void)
{
	GLOBAL_BLOCK_RESET_t g_blk_reset;

	printf("%s\n", __func__);

	/* reset taroko */
	g_blk_reset.wrd = CORTINA_CA_REG_READ(GLOBAL_BLOCK_RESET);
	g_blk_reset.bf.reset_rcpu1 = 1;
	g_blk_reset.bf.reset_rcpu0  = 1;
	CORTINA_CA_REG_WRITE(g_blk_reset.wrd, GLOBAL_BLOCK_RESET);
	memcpy((void *)TAROKO_LOAD_ADDR, taroko_firmware, sizeof(taroko_firmware));
	mdelay(100);
	/* release taroko */
	g_blk_reset.bf.reset_rcpu1 = 0;
	g_blk_reset.bf.reset_rcpu0  = 0;
	CORTINA_CA_REG_WRITE(g_blk_reset.wrd, GLOBAL_BLOCK_RESET);

	return 0;
}

ft2_ret_t ca_ft2_taroko_test(ft2_record_t *record)
{
	int i;
	ft2_ret_t rc = FT2_OK;

#if (TAROKO_CORE_MASK > 0)

	printf("FT2_PARAM_TAROKO test ...\n");

	record->entry_mask = TAROKO_CORE_MASK;
	record->start = get_timer(0);
	for (i = 0; i < TAROKO_CORE_NUM; i++) {
		if (!((record->entry_mask >> i) & 0x1))
			continue;

		/* clean counters */
		CORTINA_CA_REG_WRITE(0, (TAROKO_VERIFY_ADDR + i*4));
		record->entry_test[i] |= TEST_TAROKO_LOAD|TEST_TAROKO_RUN;
	}

	ca_ft2_taroko_run();

	return  rc ? FT2_TAROKO : FT2_OK;
#else
		printf("No TAROKO test\n");
		return FT2_TAROKO;
#endif
}
#endif

ft2_ret_t ca_ft2_taroko_result(ft2_record_t *record)
{
	uint32_t i, cnt;
	ft2_ret_t rc = FT2_OK;

#if (TAROKO_CORE_MASK > 0)

	printf("FT2_PARAM_TAROKO result ...\n");

	for (i = 0; i < TAROKO_CORE_NUM; i++) {
		if (!((record->entry_mask >> i) & 0x1))
			continue;

		cnt = CORTINA_CA_REG_READ((TAROKO_VERIFY_ADDR + i*4));
		FT2_DBG("%s: taroko#%d run cnt = %d\n", __func__, i, cnt);
		if (cnt < 5) {
			record->entry_rlt[i] |= TEST_TAROKO_RUN;
			rc = FT2_TAROKO;
		}
	}

	record->finish = get_timer(record->start) * 1000 / CONFIG_SYS_HZ;
	record->module_rlt = rc;

	printf("%s\n", rc ? "FAIL" : "PASS");
	FT2_MOD("test takes %ld.%03d secs\n", record->finish/1000, (int)(record->finish%1000));

	return  rc ? FT2_TAROKO : FT2_OK;
#else
	printf("No TAROKO test\n");
	return FT2_TAROKO;
#endif
}

int do_ft2_taroko_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_taroko_test(&ft2_records[FT2_PARAM_TAROKO]);
	udelay(1000000);
	ca_ft2_taroko_result(&ft2_records[FT2_PARAM_TAROKO]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_TAROKO), NULL);
	return 0;
}

int do_ft2_uuid_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	ca_ft2_cfg_t cfg_data;
	int have_cfg_record = 0;

	test_prepare(ft2_records, FT2_PARAM_MAX, /*test,*/ &cfg_data, &have_cfg_record);
	ca_ft2_uuid_test(&ft2_records[FT2_PARAM_UUID], cfg_data.uuid);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_UUID), NULL);
	return 0;
}

int do_ft2_nand_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_nand_test(&ft2_records[FT2_PARAM_NAND]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_NAND), NULL);
	return 0;
}

int do_ft2_cfg_w(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	ca_ft2_cfg_t cfg_data;
	int i, rc, have_cfg_record = 0, f_save = 0;
	uint32_t cfg_option;

	test_prepare(&ft2_records[FT2_PARAM_CFG_W], 1, /*test,*/ &cfg_data, &have_cfg_record);

	if (argc == 1) {
		ca_ft2_cfg_dump(&cfg_data);
	} else {
		cfg_option = simple_strtoul(argv[1], NULL, 10);

		if (cfg_option == FT2_CFG_GALIB_LOAD) {
			rc = ca_ft2_cfg_confirm(&cfg_data);
			ca_ft2_cfg_dump(&cfg_data);
			if (!rc)
				ca_ft2_phy_calibration_reload(&cfg_data);

		} else if (cfg_option == FT2_CFG_GALIB_CHECK) {
			printf("\n{SORTING:%s}\n\n\n", (have_cfg_record && cfg_data.param.gphy_cal_cmplt) ? "DONE" : "NO");

		} else if ((cfg_option == FT2_CFG_GALIB_WRITE) && (ft2_records[FT2_PARAM_GPHY_K].module_rlt == 0)) {

			for (i = 0; ge_ports[i] != INVALID_PORT; i++) {
				if ((ft2_records[FT2_PARAM_GPHY_K].entry_test[i] != TEST_ALL_CALIBRATION) ||
					(ft2_records[FT2_PARAM_GPHY_K].entry_rlt[i] != 0)) {
					break;
				}
				f_save = 1;
			}
			/* save gphy_k results */
			if (f_save && (!have_cfg_record)) {
				ft2_records[FT2_PARAM_CFG_W].ptr_data = (void *)&cfg_data;
				ca_ft2_cfg_save(&ft2_records[FT2_PARAM_CFG_W]);
			}
		}
	}
	return 0;
}

int do_ft2_ssp_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_ssp_test(&ft2_records[FT2_PARAM_SSP]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_SSP), NULL);
	return 0;
}

int do_ft2_dyingasp_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	test_prepare(ft2_records, FT2_PARAM_MAX, /*DEFAULT_TESTS,*/ NULL, NULL);
	ca_ft2_dyingasp_test(&ft2_records[FT2_PARAM_DYINGASP]);
	test_result(ft2_records, FT2_PARAM_MAX, (1 << FT2_PARAM_DYINGASP), NULL);
	return 0;
}

int do_ft2_otp_access(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	ca_uint32_t op = 0, addr, value, tmp = 0;
	GLOBAL_SECURITY_OVERRIDE_t glb_scur;
	OTP_PROTEN_t otp_proten;

	if (argc <= 1) {
		return 0;
	} else {
		/* get addr */
		addr = simple_strtoul(argv[1], NULL, 16);
		/* get value */
		if (NULL != argv[2]) {
			value = simple_strtoul(argv[2], NULL, 16);
			op = 1;
		}
	}

	/*  turn off otp register protection */
	glb_scur.wrd = CORTINA_CA_REG_READ(GLOBAL_SECURITY_OVERRIDE);
	glb_scur.bf.otp_lock_prot_fields = 0;
	CORTINA_CA_REG_WRITE(glb_scur.wrd, GLOBAL_SECURITY_OVERRIDE);
	/* turn off local otp protection bits */
	otp_proten.wrd = CORTINA_CA_REG_READ(OTP_PROTEN);
	otp_proten.bf.EN = 0;
	otp_proten.bf.PROG = 0;
	CORTINA_CA_REG_WRITE(otp_proten.wrd, OTP_PROTEN);

	if (op) {
		/* write data to otp */
		CA_OTP_REG_READ(tmp, 0xfffdfffc);
		printf("0xfffdfffc: 0x%02x\n", tmp);
		if (tmp == 0x0){
			value |= 0x0;
			printf("{0} A-cut\n");
                } else if (tmp == 0xb) {
                        value |= 0x1;
			printf("{1} B-cut\n");
                } else if (tmp == 0xc) {
                        value |= 0x2;
			printf("{2} C-cut\n");
                } else if (tmp == 0xd) {
                        value |= 0x3;
			printf("{3} D-cut\n");
		} else {
			printf("wrong ECO\n");
		}
		//printf("0x%08x: 0x%02x\n", addr, value);
		CA_OTP_REG_WRITE(value, addr);
	} else {
		CA_OTP_REG_READ(tmp, 0xfffdfffc);
                if (tmp == 0x0){
                        printf("{0} A-cut\n");
                } else if (tmp == 0xb) {
                        printf("{1} B-cut\n");
                } else if (tmp == 0xc) {
                        printf("{2} C-cut\n");
                } else if (tmp == 0xd) {
                        printf("{3} D-cut\n");
                } else {
                        printf("wrong ECO\n");
                }

		/* read data from otp */
		CA_OTP_REG_READ(value, addr);
		printf("0x%08x: 0x%02x\n", addr, value);
	}

	FT2_DBG("OTP %s 0x%08x: 0x%02x\n", ((op) ? "write" : "read"), addr, value);

	/* turn on local otp protection bits */
	otp_proten.wrd = CORTINA_CA_REG_READ(OTP_PROTEN);
	otp_proten.bf.EN = 1;
	otp_proten.bf.PROG = 1;
	CORTINA_CA_REG_WRITE(otp_proten.wrd, OTP_PROTEN);
	/*  turn on otp register protection */
	glb_scur.wrd = CORTINA_CA_REG_READ(GLOBAL_SECURITY_OVERRIDE);
	glb_scur.bf.otp_lock_prot_fields = 1;
	CORTINA_CA_REG_WRITE(glb_scur.wrd, GLOBAL_SECURITY_OVERRIDE);

	return 0;
}

int do_ft2_dbg(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc == 1) {
		printf("ft2_dbg = %d\n", ft2_dbg);
	} else {
		ft2_dbg = simple_strtoul(argv[1], NULL, 10);
	}
	return 0;
}

int do_ft2_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	uint32_t test = DEFAULT_TESTS;
	ca_ft2_cfg_t cfg_data;
	int rc, have_cfg_record = 0;
	uint32_t cfg_option = FT2_CFG_GALIB_CHECK;

	ft2_start = get_timer(0);

	test_prepare(ft2_records, FT2_PARAM_MAX, /*test,*/ &cfg_data, &have_cfg_record);

	if (argc == 1) {
		test = (1 << FT2_PARAM_GPHY_K);
	} else {
		cfg_option = simple_strtoul(argv[1], NULL, 10);
	}

	if (test & (0x1 << FT2_PARAM_CFG_W)) {
		if (cfg_option == FT2_CFG_GALIB_LOAD) {
			rc = ca_ft2_cfg_confirm(&cfg_data);
			ca_ft2_cfg_dump(&cfg_data);
			if (!rc)
				ca_ft2_phy_calibration_reload(&cfg_data);
		} else if (cfg_option == FT2_CFG_GALIB_CHECK) {
			printf("\n{SORTING:%s}\n\n\n", (have_cfg_record && cfg_data.param.gphy_cal_cmplt) ? "DONE" : "NO");

			if (have_cfg_record && cfg_data.param.gphy_cal_cmplt)
				test &= ~(1 << FT2_PARAM_GPHY_K);
			else
				test = 0;
		}
	}

	if (test & (1 << FT2_PARAM_RTC)) {
		ca_ft2_rtc_test(&ft2_records[FT2_PARAM_RTC]);
	}

	if (test & (1 << FT2_PARAM_TAROKO)) {
		ca_ft2_taroko_test(&ft2_records[FT2_PARAM_TAROKO]);
	}

	if (test & (1 << FT2_PARAM_GPHY_K)) {
		ca_ft2_phy_calibration(&ft2_records[FT2_PARAM_GPHY_K]);
	}

	if (test & (1 << FT2_PARAM_CPU)) {
		ca_ft2_cpu_test(&ft2_records[FT2_PARAM_CPU]);
	}

	if (test & (1 << FT2_PARAM_GMII_PORT_OPEN_LOOPBACK)) {
		ca_ft2_gmii_open_loopback(&ft2_records[FT2_PARAM_GMII_PORT_OPEN_LOOPBACK]);
	}

	if (test & (1 << FT2_PARAM_PON_SRAM)) {
		ca_ft2_pon_sram_test(&ft2_records[FT2_PARAM_PON_SRAM]);
	}

	if (test & (1 << FT2_PARAM_GMII_PORT_LOOPBACK)) {
		ca_ft2_gmii_loopback(&ft2_records[FT2_PARAM_GMII_PORT_LOOPBACK]);
	}

	if (test & (1 << FT2_PARAM_RGMII_PORT_LOOPBACK)) {
		ca_ft2_rgmii_loopback(&ft2_records[FT2_PARAM_RGMII_PORT_LOOPBACK]);
	}

	if (test & (1 << FT2_PARAM_OTP)) {
		ca_ft2_otp_test(&ft2_records[FT2_PARAM_OTP]);
	}

	if (test & (1 << FT2_PARAM_LDMA)) {
		ca_ft2_ldma_test(&ft2_records[FT2_PARAM_LDMA]);
	}

	if (test & (1 << FT2_PARAM_PCIE)) {
		ca_ft2_pcie_test(&ft2_records[FT2_PARAM_PCIE]);
	}

	if (test & (1 << FT2_PARAM_USB)) {
		ca_ft2_usb_test(&ft2_records[FT2_PARAM_USB]);
	}

	if (test & (1 << FT2_PARAM_DDR_ZQ)) {
		ca_ft2_ddr_zq_test(&ft2_records[FT2_PARAM_DDR_ZQ]);
	}

	if (test & (1 << FT2_PARAM_SSP)) {
		ca_ft2_ssp_test(&ft2_records[FT2_PARAM_SSP]);
	}

	if (test & (1 << FT2_PARAM_DYINGASP)) {
		ca_ft2_dyingasp_test(&ft2_records[FT2_PARAM_DYINGASP]);
	}

	if (test & (1 << FT2_PARAM_UUID)) {
		ca_ft2_uuid_test(&ft2_records[FT2_PARAM_UUID], cfg_data.uuid);
	}

	if (test & (1 << FT2_PARAM_SERDES)) {
		ca_ft2_serdes_test(&ft2_records[FT2_PARAM_SERDES]);
	}

	if (test & (1 << FT2_PARAM_PON_BEN)) {
		ca_ft2_pon_ben_test(&ft2_records[FT2_PARAM_PON_BEN]);
	}

	if (test & (1 << FT2_PARAM_INTRA_XFI)) {
		ca_ft2_intra_xfi_test(&ft2_records[FT2_PARAM_INTRA_XFI]);
	}

	if (test & (1 << FT2_PARAM_GMII_PORT_OPEN_LOOPBACK)) {
		ca_ft2_gmii_open_loopback_result(&ft2_records[FT2_PARAM_GMII_PORT_OPEN_LOOPBACK]);
	}

	if (test & (1 << FT2_PARAM_RTC)) {
		ca_ft2_rtc_result(&ft2_records[FT2_PARAM_RTC]);
	}

	if (test & (1 << FT2_PARAM_TAROKO)) {
		ca_ft2_taroko_result(&ft2_records[FT2_PARAM_TAROKO]);
	}

	if (test & (1 << FT2_PARAM_NAND)) {
		ca_ft2_nand_test(&ft2_records[FT2_PARAM_NAND]);
	}
	/* it's the last test item. gpio setting will be incorrect after run gpio test*/
	if (test & (1 << FT2_PARAM_GPIO)) {
		ca_ft2_gpio_test(&ft2_records[FT2_PARAM_GPIO]);
	}

	if (test & 0x1 << FT2_PARAM_CFG_W) {

		if ((cfg_option == FT2_CFG_GALIB_WRITE) &&
			(test & (1 << FT2_PARAM_GPHY_K))) {
			ft2_records[FT2_PARAM_CFG_W].ptr_data = (void *)&cfg_data;

			if ((ft2_records[FT2_PARAM_GPHY_K].module_rlt == 0) &&
				(!have_cfg_record) && (!cfg_data.param.gphy_cal_cmplt))
				/* save gphy_k results */
				ca_ft2_cfg_save(&ft2_records[FT2_PARAM_CFG_W]);
			else
				ca_ft2_cfg_resorting_test(&ft2_records[FT2_PARAM_CFG_W], have_cfg_record);
		}

		if ((cfg_option != FT2_CFG_GALIB_LOAD) && (cfg_option != FT2_CFG_GALIB_WRITE))
			ca_ft2_cfg_dump(&cfg_data);

		/* check if uuid was written after cfg updated */
		if (test & (1 << FT2_PARAM_UUID)) {
			ca_ft2_uuid_test(&ft2_records[FT2_PARAM_UUID], 0);
		}
	}

	ft2_finish = get_timer(ft2_start) * 1000 / CONFIG_SYS_HZ;

	test_result(ft2_records, FT2_PARAM_MAX, test, &cfg_data);

	return 0;
}

static cmd_tbl_t cmd_ft2_sub[] = {
	U_BOOT_CMD_MKENT(test, 2, 1, do_ft2_test, "", ""),
	U_BOOT_CMD_MKENT(gphy_k, 1, 1, do_ft2_phy_calibration, "", ""),
	U_BOOT_CMD_MKENT(rc_k_diff, 2, 1, do_ft2_rc_k_diff, "", ""),
	U_BOOT_CMD_MKENT(r_k_diff, 2, 1, do_ft2_r_k_diff, "", ""),
	U_BOOT_CMD_MKENT(amp_volt_aid, 2, 1, do_ft2_amp_volt_aid, "", ""),
	U_BOOT_CMD_MKENT(ado_verify_times, 2, 1, do_ft2_ado_verify_times, "", ""),
	U_BOOT_CMD_MKENT(ado_volt_diff, 2, 1, do_ft2_ado_volt_diff, "", ""),
	U_BOOT_CMD_MKENT(ado_over_times, 2, 1, do_ft2_ado_over_times, "", ""),
	U_BOOT_CMD_MKENT(ado_current, 2, 1, do_ft2_ado_current, "", ""),
	U_BOOT_CMD_MKENT(gphy_snr, 1, 1, do_ft2_gphy_snr_test, "", ""),
	U_BOOT_CMD_MKENT(ldma, 1, 1, do_ft2_ldma_test, "", ""),
	U_BOOT_CMD_MKENT(serdes, 1, 1, do_ft2_serdes_test, "", ""),
	U_BOOT_CMD_MKENT(pon_ben, 1, 1, do_ft2_pon_ben_test, "", ""),
	U_BOOT_CMD_MKENT(pon_sram, 1, 1, do_ft2_pon_sram_test, "", ""),
	U_BOOT_CMD_MKENT(intra_xfi, 1, 1, do_ft2_intra_xfi_test, "", ""),
	U_BOOT_CMD_MKENT(gmii, 1, 1, do_ft2_gmii_loopback_test, "", ""),
	U_BOOT_CMD_MKENT(gmii_open_lb, 1, 1, do_ft2_gmii_open_loopback_test, "", ""),
	U_BOOT_CMD_MKENT(rgmii, 1, 1, do_ft2_rgmii_loopback_test, "", ""),
	U_BOOT_CMD_MKENT(gpio, 1, 1, do_ft2_gpio_test, "", ""),
	U_BOOT_CMD_MKENT(otp, 1, 1, do_ft2_otp_test, "", ""),
	U_BOOT_CMD_MKENT(cpu, 1, 1, do_ft2_cpu_test, "", ""),
	U_BOOT_CMD_MKENT(rtc, 1, 1, do_ft2_rtc_test, "", ""),
	U_BOOT_CMD_MKENT(uuid, 1, 1, do_ft2_uuid_test, "", ""),
	U_BOOT_CMD_MKENT(nand, 1, 1, do_ft2_nand_test, "", ""),
	//U_BOOT_CMD_MKENT(ssp, 1, 1, do_ft2_ssp_test, "", ""),
	U_BOOT_CMD_MKENT(dyingasp, 1, 1, do_ft2_dyingasp_test, "", ""),
	U_BOOT_CMD_MKENT(pcie, 1, 1, do_ft2_pcie_test, "", ""),
	U_BOOT_CMD_MKENT(usb, 1, 1, do_ft2_usb_test, "", ""),
	U_BOOT_CMD_MKENT(taroko, 1, 1, do_ft2_taroko_test, "", ""),
	U_BOOT_CMD_MKENT(ddr_zq, 1, 1, do_ft2_ddr_zq_test, "", ""),
	U_BOOT_CMD_MKENT(cfg_w, 2, 1, do_ft2_cfg_w, "", ""),
	U_BOOT_CMD_MKENT(otp_w, 2, 1, do_ft2_otp_access, "", ""),
	U_BOOT_CMD_MKENT(dbg, 2, 1, do_ft2_dbg, "", "")
};

int do_ft2(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* Strip off leading 'ft2' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_ft2_sub[0], ARRAY_SIZE(cmd_ft2_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

/***************************************************/
#ifdef CONFIG_SYS_LONGHELP
static char ft2_help_text[] =
	"test <value> - Run FT2 full test items\n"
	"ft2 gphy_k - Run GPHY calibration\n"
	"ft2 rc_k_diff <value> - Set RC-K correction value\n"
	"ft2 r_k_diff <value> - Set R-K correction value\n"
	"ft2 amp_volt_aid <value> - Set Aid voltage for AMP-K\n"
	"ft2 ado_verify_times <value> - Set verify times for ADO-K\n"
	"ft2 ado_volt_diff <value> - Set volt range for ADO-K\n"
	"ft2 ado_over_times <value> - Set threshold times over volt diff\n"
	"ft2 ado_curr <low|norm|med|high> - Set ADO current\n"
	"ft2 gphy_snr - Run GPHY SNR test\n"
	"ft2 ldma - Run LDMA test\n"
	"ft2 serdes - Run SERDES test\n"
	"ft2 pon_ben - Run PON BEN test\n"
	"ft2 pon_sram - Run PON SRAM test\n"
	"ft2 intra_xfi - Run INTRA XFI test\n"
	"ft2 gmii - Run GMII loopback test\n"
	"ft2 gmii_open_lb - Run GMII open loopback test\n"
	"ft2 rgmii - Run RGMII loopback test\n"
	"ft2 gpio - Run GPIO test\n"
	"ft2 otp - Run OTP test\n"
	"ft2 cpu - Run CPU test\n"
	"ft2 rtc - Run RTC test\n"
	"ft2 uuid - Run UUID test\n"
	"ft2 nand - Run NAND test\n"
	"ft2 ssp - Run SSP CLK test\n"
	"ft2 dyingasp - Run dyingasp test\n"
	"ft2 pcie - Run PCIe test\n"
	"ft2 usb - Run USB test\n"
	"ft2 taroko - Run taroko test\n"
	"ft2 ddr_zq - Run DDR ZQ test\n"
	"ft2 cfg_w <value> - Save GPHY calibration result to FT2_CFG\n"
	"ft2 otp_w <addr> <value> - Read/Write OTP\n"
	"ft2 dbg <value> - Dump more debug messages\n";
#endif

U_BOOT_CMD(
	ft2, 20, 1, do_ft2,
	"FT2 utility commands",
	ft2_help_text
);

