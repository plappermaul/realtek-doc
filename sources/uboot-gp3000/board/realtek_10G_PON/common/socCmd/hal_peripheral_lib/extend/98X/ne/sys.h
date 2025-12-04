#ifndef __SYS_H__
#define __SYS_H__

#include "ca_types.h"

#define CA_MAX_PORT_NUM_GPON			0
#define CA_MAX_PORT_NUM_EPON			0
#define CA_MAX_PORT_NUM_ETHERNET		8
#define CA_MAX_PORT_NUM_CPU			8
#define CA_MAX_PORT_NUM_SUBPORT			0
#define CA_MAX_PORT_NUM_OFFLOAD			2
#define CA_MAX_PORT_NUM_BLACKHOLE		1
#define CA_MAX_PORT_NUM_L2RP			2
#define CA_MAX_PORT_NUM				64

typedef enum {
	CA_DEV_MODE_L3			= 0,
	CA_DEV_MODE_L2			= 1,
} ca_sys_dev_mode_t;

typedef enum {
	CA_INIT_SRAM_FAIL		= 0,	// Failed to initialize SRAM
	CA_CSR_ACCESS_FAIL		= 1,	// Failed to have access to hardware register
	CA_INIT_SERDES_FAIL		= 2,	// Failed to initialize SerDes
	CA_INIT_MISSING_STARTUP_CONF	= 3,	// Startup config file missing
	CA_INIT_QM_INIT_FAILED		= 4,	// Failed to initialize buffer manager
	CA_INIT_PON_INIT_FAILED		= 5,	// Failed to initialize buffer manager
	CA_INIT_HW_VERSION_INVALID	= 6,	// Unexpected HW version
} ca_init_err_t;

typedef enum {
	CA_SYS_RESET_MODE_COLD		= 0,	// reset the whole system, cold reboot
	CA_SYS_RESET_MODE_WARM		= 1,	// keep data path configurations, just reset CPU
	CA_SYS_RESET_MODE_DATAPATH	= 2,	// just reset data path, keep CPU as is
} ca_sys_reset_mode_t;

typedef struct {
	/* Notation of gpio pins: 0x1 means gpio0(pin0), 0x2 gpio1(pin1), etc.
	 * All gpio configurations are specified using this notation.
	 */

	/* Valid gpio pins used at the start-up time.
	 * All other bits in the below configurations are ignored.
	 * Other pins can be configured during run-time.
	 */
	ca_uint32_t	gpio_valid;

	/* GPIO interrupt pins to enable at the start-up time. Other pins can
	 * be enabled during run-time. */
	ca_uint32_t	gpio_intpin;

	/* GPIO Interrupt level: 0 - failing edge, 1 - rising edge for each GPIO.
	 * Only pins specified in gpio_intr_pins are valid. All other bits are
	 * ignored. */
	ca_uint32_t	gpio_intlvl;

	/* Direction of GPIO pins : 0 - output, 1 - input
	 */
	ca_uint32_t	gpio_config;

	/* GPIO output value */
	ca_uint32_t	gpio_outval;
} __attribute__((packed)) ca_gpio_startup_config_t;

typedef struct {
	/* ONU queue set report mode
	   0: 1 queue set report mode
	   1: 2 queue set report mode (CTC)
	   2: 1 queue set queue 0 mode (KT-1)
	   3: 1 queue set queue 7 mode (KT-2)
	 */
	ca_uint8_t	qset_report_mode;
} ca_epon_startup_config_t;

typedef struct {
	ca_sys_dev_mode_t		dev_mode;				/* device working mode indicator */
	ca_mac_addr_t			dev_mac;				/* MAC address */

	/* PHY MDIO base address */
	ca_uint8_t			phy_base_addr[CA_MAX_PORT_NUM_ETHERNET];

	/* PHY interface  0: RGMII, 1: RMII */
	ca_uint8_t			phy_interface[CA_MAX_PORT_NUM_ETHERNET];

	/* UNI connection mode
	 * 0: connected to MAC
	 * 1: connected with PHY
	 */
	ca_uint8_t			uni_connection[CA_MAX_PORT_NUM_ETHERNET];

	/* MAC uses local clock or peer clock */
	ca_uint8_t			mac_clock_selector[CA_MAX_PORT_NUM_ETHERNET];

	ca_gpio_startup_config_t	gpio_config;
	ca_epon_startup_config_t	epon_config;

	ca_uint16_t			ipv4_max_route;				/* IPv4 Route Table size */
	ca_uint16_t			ipv6_max_route;				/* IPv6 Route Table size */
	ca_uint16_t			ipv4_max_nexthop;			/* IPv4 Nexthop Table size */
	ca_uint16_t			ipv6_max_nexthop;			/* IPv6 Nexthop Table size */
	ca_uint16_t			nexthop_aging_time;			/* Nexthop Aging Time in Seconds */
} ca_init_config_t;

typedef struct {
	ca_init_config_t		common;					/* common to all Cortina ASIC*/

	ca_mac_addr_t			ca_ne_mac_addr0;			/* MAC address of WAN port */
	ca_mac_addr_t			ca_ne_mac_addr1;			/* MAC address of LAN port */
	ca_uint8_t			*ca_ne_sram_base;			/* Virtual base address of SRAM reserved area */
	ca_uint32_t			ca_ne_ddr_coherent_phy_base;		/* Physical base address of DDR coherent reserved area */
	ca_uint8_t			*ca_ne_ddr_noncache_base;		/* Virtual base address of DDR non-cache reserved area */
	ca_uint32_t			ca_ne_sram_phy_base;			/* Physical base address of SRAM reserved area */
	ca_uint32_t			ca_ne_ddr_noncache_phy_base;		/* Physical base address of DDR non-cache reserved area */

	ca_uint8_t			ca_ni_mgmt_mode;			/* flag to indicate whether NI is in management mode */
	ca_uint8_t			ca_ne_port_id;				/* port id of WAN port */
} ca_g3_init_config_t;

typedef struct {
	/* Chip mode */
	ca_uint32_t	hardware_id;

	/* Chip revision No. */
	ca_uint32_t	hardware_rev;

	/* Chip release date.
	 * For example, hardware_data will be 0x130901 if the release date is Sep. 1, 2013.
	 */
	ca_uint32_t	hardware_date;

	/* API version */
	ca_uint32_t	api_version;
} ca_info_t;

ca_status_t ca_init (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_init_config_t	*p_cfg,
	CA_OUT	ca_init_err_t		*err_code
);

ca_status_t ca_reset (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_sys_reset_mode_t	mode
);

ca_status_t ca_info_get (
	CA_IN	ca_device_id_t		device_id,
	CA_OUT	ca_info_t		*info
);

ca_status_t ca_info_set (
	CA_IN	ca_device_id_t		device_id,
	CA_OUT	ca_info_t		*info
);

#endif /* __SYS_H__ */
