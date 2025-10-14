//#ifdef CONFIG_TP_EXT_SWITCH_RTL8367S
#include <linux/proc_fs.h>

#define RTL8367S_HW_LED_FUNCTION
//#define RTL8367S_HW_BRIDGE_PORT_VLAN_DETECT

#ifdef CONFIG_TP_EXT_SWITCH_RTL8367S_SINGLE
#include "rtl8367s/rtl8367c_asicdrv_port.h"
#include "rtl8367s/rtk_switch.h"
#include "rtl8367s/port.h"
#include "rtl8367s/vlan.h"
#include "rtl8367s/acl.h"
#include "rtl8367s/svlan.h"
#include "rtl8367s/vlan.h"
#include "rtl8367s/l2.h"
#include "rtl8367s/stat.h"
#include "rtl8367s/cpu.h"
#include "rtl8367s/trap.h"
#include "rtl8367s/igmp.h"
#include "rtl8367s/mirror.h"
#ifdef RTL8367S_HW_LED_FUNCTION
#include "rtl8367s/led.h"
#endif /*RTL8367S_HW_LED_FUNCTION */
#else
#include "rtk_types.h"
#include "rtk_error.h"
#include "rtk_switch.h"
#include "port.h"
#include "vlan.h"
#include "acl.h"
#include "svlan.h"
#include "l2.h"
#include "stat.h"
#include "cpu.h"
#include "trap.h"
#include "igmp.h"
#include "mirror.h"
#include "dal/dal_mgmt.h"
#ifdef RTL8367S_HW_LED_FUNCTION
#include "led.h"
#endif /*RTL8367S_HW_LED_FUNCTION */
#include "chip.h"
#include "sw.h"
#include "fal_port_ctrl.h"
#include "hsl_api.h"
#include "hsl.h"
#include "hsl_phy.h"
#include "ssdk_plat.h"
#include "qca808x_phy.h"
#include "rtl8367s.h"
#include "eee.h"

#include <linux/of_mdio.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/proc_fs.h>
#include <linux/mdio.h>
#include <linux/mutex.h>
#endif

#include <generated/autoconf.h>
#include <linux/if_arp.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/version.h>

#if 1
#define RTL8367S_RESET_GPIO_NUM (30) 
#else
#define RTL8367S_RESET_GPIO_NUM (33)
#endif

#define RTL8367S_PHY_LINK_CHECK_INTERVAL (1000) /*ms*/

#define RTL8367S_FID_RESERVED_NUM  (2)  /* fid 0 & 1 not used */

#define RTL8367S_VID_RESERVED_NUM  (3)  /* vid 0 & 1 not used */

typedef enum {
	PORT_IDX0 = 0,
	PORT_IDX1 = 1,
	PORT_IDX2 = 2,
	PORT_IDX3 = 3,
	PORT_IDX4 = 4,
	PORT_MAX_NUM,
} port_idx;

#define RTL8367S_REVERSE_PORT_MAPPING

static const rtk_port_t rtl8367s_utp_port[PORT_MAX_NUM] = {
#ifdef RTL8367S_REVERSE_PORT_MAPPING
	UTP_PORT4,
	UTP_PORT3,
	UTP_PORT2,
	UTP_PORT1,
	UTP_PORT0,
#else
	UTP_PORT0,
	UTP_PORT1,
	UTP_PORT2,
	EXT_PORT1,
#endif /* RTL8367S_REVERSE_PORT_MAPPING */
};

static const rtk_port_t rtl8367s_cpu_port = EXT_PORT1;

static const rtk_port_t rtl8367s_lan_2P5G_port = EXT_PORT0;


static const char* rtl8367s_dev_name[PORT_MAX_NUM] = {
	"eth0.2",
	"eth0.3",
	"eth0.4",
	"eth0.5",
	"eth0.6",
};

static int rtl8367s_br_group[PORT_MAX_NUM] = {
	-1,
	-1,
	-1,
	-1,
	-1,
};

static int rtl8367s_iso_mode[PORT_MAX_NUM] = {
	0,
	0,
	0,
	0,
	0,
};

static int rtl8368s_mc_router[PORT_MAX_NUM] = {
	1,
	1,
	1,
	1,
	1,
};

static int rtl8368s_link_status[PORT_MAX_NUM] = {
	-1,
	-1,
	-1,
	-1,
	-1,
};

static int rtl8367s_hw_bridge_port[PORT_MAX_NUM] = {
	1,
	1,
	1,
	1,
	1,
};

#ifdef RTL8367S_HW_BRIDGE_PORT_VLAN_DETECT
static int rtl8367s_hw_bridge_vlan_port[PORT_MAX_NUM] = {
	0,
	0,
	0,
	0,
	0,
};
#endif /**/

static int rtl8367s_mii_type = 2;

static int rtl8367s_mcast_snoop = 1;

static int rtl8367s_port_mirror = 0;

static int rtl8367s_hw_bridge = 1;

static int rtl8367s_eth_forward = 0; /* 0: init forbid forwarding */

#ifdef RTL8367S_HW_LED_FUNCTION
static int rtl8367s_hw_led = 1;
#endif /* RTL8367S_HW_LED_FUNCTION */

unsigned char rtl8367s_cpu_mac[6] = { 0, 0, 0, 0, 0, 0 };

static int rtl8367s_debug_info = 1;

switch_chip_t g_switch_chip = CHIP_END;

#ifdef USE_SPIN_LOCK
static spinlock_t rtl8367s_lock;

#define RTL8367S_LOCK_INIT()    \
	do {                    \
		spin_lock_init(&rtl8367s_lock);\
	} while(0)

#define RTL8367S_LOCK()           \
	do {                          \
		spin_lock_bh(&rtl8367s_lock);\
	} while(0)

#define RTL8367S_UNLOCK()           \
	do {                            \
		spin_unlock_bh(&rtl8367s_lock);\
	} while(0)

#else
static struct mutex rtl8367s_lock;
	
#define RTL8367S_LOCK_INIT()    \
		do {					\
			mutex_init(&rtl8367s_lock);\
		} while(0)
	
#define RTL8367S_LOCK()           \
		do {						  \
			mutex_lock(&rtl8367s_lock);\
		} while(0)
	
#define RTL8367S_UNLOCK()           \
		do {							\
			mutex_unlock(&rtl8367s_lock);\
		} while(0)

#endif

#define RTL8367S_LOG_ERR(fmt, ...)    \
	do {                              \
		printk("[RTL8367S ERROR] (%s %d) " fmt,  __func__, __LINE__, ##__VA_ARGS__);\
	} while(0)

#define RTL8367S_LOG_INFO(fmt, ...)    \
	do {							  \
		printk("[RTL8367S INFO] " fmt, ##__VA_ARGS__);\
	} while(0)

#define RTL8367S_LOG_DEBUG(fmt, ...)    \
	do {                               \
		if(rtl8367s_debug_info)        \
		{                              \
			printk("[RTL8367S DEBUG] (%s %d) " fmt, __func__, __LINE__, ##__VA_ARGS__);\
		}                              \
	}while(0)


static struct mii_bus *g_mii_bus = NULL;

int rtl8367s_mdio_write(unsigned int phy_addr, unsigned int reg, unsigned int data)
{
	if (!g_mii_bus)
	{
		return -1;
	}

	mutex_lock(&g_mii_bus->mdio_lock);
	__mdiobus_write(g_mii_bus, 29, reg, data);
	mutex_unlock(&g_mii_bus->mdio_lock);
	
	return 0;
}

int rtl8367s_mdio_read(unsigned int phy_addr, unsigned int reg, unsigned int* data)
{
	if (!g_mii_bus)
	{
		return -1;
	}

	mutex_lock(&g_mii_bus->mdio_lock);
	*data = __mdiobus_read(g_mii_bus, 29, reg);
	mutex_unlock(&g_mii_bus->mdio_lock);

	return 0;
}

static void rtl8367s_gpio_reset(int gpio_num)
{
	gpio_direction_output(gpio_num, 1);
	msleep(500);
	gpio_set_value(gpio_num, 0);
	msleep(200);
	gpio_set_value(gpio_num, 1);
	msleep(1000);

	return;
}


static int _rtl8367s_acl_refresh(void)
{
	rtk_api_ret_t ret = RT_ERR_OK;

	rtk_filter_field_t filter_field;
	rtk_filter_cfg_t cfg;
	rtk_filter_action_t act;
	rtk_filter_number_t ruleNum;

	memset(&cfg, 0, sizeof(rtk_filter_cfg_t));
	memset(&act, 0, sizeof(rtk_filter_action_t));
	memset(&filter_field, 0, sizeof(rtk_filter_field_t));
	
	filter_field.fieldType = FILTER_FIELD_DMAC;
	filter_field.filter_pattern_union.dmac.dataType = FILTER_FIELD_DATA_MASK;
	filter_field.filter_pattern_union.dmac.value.octet[0] = 0x01; /* ieee 1905 message */
	filter_field.filter_pattern_union.dmac.value.octet[1] = 0x80;
	filter_field.filter_pattern_union.dmac.value.octet[2] = 0xC2;
	filter_field.filter_pattern_union.dmac.value.octet[3] = 0x00;
	filter_field.filter_pattern_union.dmac.value.octet[4] = 0x00;
	filter_field.filter_pattern_union.dmac.value.octet[5] = 0x13;
	
	filter_field.filter_pattern_union.dmac.mask.octet[0] = 0xFF;
	filter_field.filter_pattern_union.dmac.mask.octet[1] = 0xFF;
	filter_field.filter_pattern_union.dmac.mask.octet[2] = 0xFF;
	filter_field.filter_pattern_union.dmac.mask.octet[3] = 0xFF;
	filter_field.filter_pattern_union.dmac.mask.octet[4] = 0xFF;
	filter_field.filter_pattern_union.dmac.mask.octet[5] = 0xFF;
	
	if ((ret = rtk_filter_igrAcl_field_add(&cfg, &filter_field)) != RT_ERR_OK)
	{
		RTL8367S_LOG_ERR("rtk_filter_igrAcl_field_add fail...ret=%d\n", ret);
		return ret;
	}
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, EXT_PORT1);
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, UTP_PORT0);
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, UTP_PORT1);
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, UTP_PORT2);
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, UTP_PORT3);
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, UTP_PORT4);
	RTK_PORTMASK_ALLPORT_SET(cfg.activeport.mask);
	cfg.invert = FALSE;
	act.actEnable[FILTER_ENACT_REDIRECT] = TRUE;
	act.filterPortmask.bits[0] = (1<<rtl8367s_cpu_port);
	
	if ((ret = rtk_filter_igrAcl_cfg_add(0, &cfg, &act, &ruleNum)) != RT_ERR_OK)
	{
		RTL8367S_LOG_ERR("rtk_filter_igrAcl_cfg_add fail...ret=%d\n", ret);
		return ret;
	}

	return ret;
}

static int rtl8367s_acl_init(void)
{
	int ret = 0;

	RTL8367S_LOG_DEBUG("init\r\n");

	if((ret=rtk_filter_igrAcl_init()) != RT_ERR_OK){
		RTL8367S_LOG_ERR("rtk_filter_igrAcl_init fail...ret=%d\n", ret);
		return ret;
	} 

	_rtl8367s_acl_refresh();

	return 0;
}

static rtk_uint32 _rtl8367s_mii_type_refresh(void)
{
	int ret = 0;
	rtk_port_mac_ability_t ability;
	if(rtl8367s_mii_type == 0)
	{
		/* RGMII, TODO */
	}
	else if(rtl8367s_mii_type == 1)
	{
		/* SGMII */
		memset(&ability, 0x0, sizeof(ability));
		
		ability.speed = PORT_SPEED_1000M;
#ifdef CONFIG_TP_EXT_SWITCH_RTL8367S_SINGLE
		ability.forcemode = MAC_FORCE;
		ability.duplex = FULL_DUPLEX;
#else
		ability.forcemode = PORT_MAC_FORCE;
		ability.duplex = PORT_FULL_DUPLEX;
#endif

		ability.link = PORT_LINKUP;
		ability.nway = DISABLED;
		ability.txpause = ENABLED;
		ability.rxpause = ENABLED;
		
		if ((ret = rtk_port_macForceLinkExt_set(rtl8367s_cpu_port, MODE_EXT_SGMII, &ability)) != RT_ERR_OK) 
		{
			RTL8367S_LOG_ERR("rtk_port_macForceLinkExt_set failed...ret=%d\n", ret);
			return ret;
		}
		
		if ((ret = rtk_port_sgmiiNway_set(rtl8367s_cpu_port, DISABLED)) != RT_ERR_OK) 
		{
			RTL8367S_LOG_ERR("rtk_port_sgmiiNway_set failed...ret=%d\n", ret);
			return ret;
		}

		if ((ret = rtk_port_macForceLinkExt_set(rtl8367s_lan_2P5G_port, MODE_EXT_SGMII, &ability)) != RT_ERR_OK) 
		{
			RTL8367S_LOG_ERR("rtk_port_macForceLinkExt_set failed...ret=%d\n", ret);
			return ret;
		}
		
		if ((ret = rtk_port_sgmiiNway_set(rtl8367s_lan_2P5G_port, DISABLED)) != RT_ERR_OK) 
		{
			RTL8367S_LOG_ERR("rtk_port_sgmiiNway_set failed...ret=%d\n", ret);
			return ret;
		}
	}
	else if(rtl8367s_mii_type == 2)
	{
		/* HSGMII */
		memset(&ability, 0x0, sizeof(ability));
		
		ability.speed = PORT_SPEED_2500M;
#ifdef CONFIG_TP_EXT_SWITCH_RTL8367S_SINGLE
		ability.forcemode = MAC_FORCE;
		ability.duplex = FULL_DUPLEX;
#else		
		ability.forcemode = PORT_MAC_FORCE;
		ability.duplex = PORT_FULL_DUPLEX;
#endif
		ability.link = PORT_LINKUP;
		ability.nway = DISABLED;
		ability.txpause = ENABLED;
		ability.rxpause = ENABLED;
		
		if ((ret = rtk_port_macForceLinkExt_set(rtl8367s_cpu_port, MODE_EXT_HSGMII, &ability)) != RT_ERR_OK) 
		{
			RTL8367S_LOG_ERR("rtk_port_macForceLinkExt_set failed...ret=%d\n", ret);
			return ret;
		}
		
		if ((ret = rtk_port_sgmiiNway_set(rtl8367s_cpu_port, DISABLED)) != RT_ERR_OK) 
		{
			RTL8367S_LOG_ERR("rtk_port_sgmiiNway_set failed...ret=%d\n", ret);
			return ret;
		}

		if ((ret = rtk_port_macForceLinkExt_set(rtl8367s_lan_2P5G_port, MODE_EXT_HSGMII, &ability)) != RT_ERR_OK) 
		{
			RTL8367S_LOG_ERR("rtk_port_macForceLinkExt_set failed...ret=%d\n", ret);
			return ret;
		}
		
		if ((ret = rtk_port_sgmiiNway_set(rtl8367s_lan_2P5G_port, DISABLED)) != RT_ERR_OK) 
		{
			RTL8367S_LOG_ERR("rtk_port_sgmiiNway_set failed...ret=%d\n", ret);
			return ret;
		}
	}
	else
	{
		/* Unknown, skip here. Maybe config by proc later.*/
		RTL8367S_LOG_ERR("Unknown type %d, skip it\n", rtl8367s_mii_type);
	}

	return 0;
}

static rtk_uint32 _rtl8367s_lookup_calc_permit(int pidx)
{
	int p;
	
	rtk_uint32 ret = 0;

	if(!rtl8367s_eth_forward)
	{
		/* forbid forward */
		ret = 0;
		RTL8367S_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}
	
	ret = (1<<rtl8367s_cpu_port);

	if(!rtl8367s_hw_bridge)
	{
		ret |= (1<<rtl8367s_utp_port[pidx]);
		RTL8367S_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}

	if(!rtl8367s_hw_bridge_port[pidx])
	{
		ret |= (1<<rtl8367s_utp_port[pidx]);
		RTL8367S_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}

#ifdef RTL8367S_HW_BRIDGE_PORT_VLAN_DETECT
	if(rtl8367s_hw_bridge_vlan_port[pidx])
	{
		ret |= (1<<rtl8367s_utp_port[pidx]);
		RTL8367S_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}
#endif /**/

	if(rtl8367s_br_group[pidx] < 0 ||
		rtl8367s_iso_mode[pidx])
	{
		ret |= (1<<rtl8367s_utp_port[pidx]);
		RTL8367S_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}

	for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
	{
#if 0 /* xmit to source port? */
		if(p == pidx)
		{
			continue;
		}
#endif
		if(rtl8367s_br_group[p] != rtl8367s_br_group[pidx])
		{
			continue;
		}

		if(!rtl8367s_hw_bridge_port[p])
		{
			continue;
		}

#ifdef RTL8367S_HW_BRIDGE_PORT_VLAN_DETECT
		if(rtl8367s_hw_bridge_vlan_port[p])
		{
			continue;
		}
#endif /**/

		if(rtl8367s_iso_mode[p])
		{
			continue;
		}

		ret |= (1<<rtl8367s_utp_port[p]);
	}

	RTL8367S_LOG_DEBUG("ret=%x\n", ret);

	return ret;
}

static rtk_uint32 _rtl8367s_lookup_calc_fid(int pidx)
{
	int p;
	rtk_uint32 ret = 0;

	if(!rtl8367s_hw_bridge)
	{
		ret = (pidx + RTL8367S_FID_RESERVED_NUM);
		RTL8367S_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}

	if(!rtl8367s_hw_bridge_port[pidx])
	{
		ret = (pidx + RTL8367S_FID_RESERVED_NUM);
		RTL8367S_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}

#ifdef RTL8367S_HW_BRIDGE_PORT_VLAN_DETECT
	if(rtl8367s_hw_bridge_vlan_port[pidx])
	{
		ret = (pidx + RTL8367S_FID_RESERVED_NUM);
		RTL8367S_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}
#endif /**/

	if(rtl8367s_br_group[pidx] < 0 ||
		rtl8367s_iso_mode[pidx])
	{
		ret = (pidx + RTL8367S_FID_RESERVED_NUM);
		RTL8367S_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}

	/* find first port in the same br group */
	for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
	{
		if(rtl8367s_br_group[p] != rtl8367s_br_group[pidx])
		{
			continue;
		}
		
		if(!rtl8367s_hw_bridge_port[p])
		{
			continue;
		}

#ifdef RTL8367S_HW_BRIDGE_PORT_VLAN_DETECT
		if(rtl8367s_hw_bridge_vlan_port[p])
		{
			continue;
		}
#endif /**/

		if(rtl8367s_iso_mode[p])
		{
			continue;
		}

		ret = (p + RTL8367S_FID_RESERVED_NUM);
		break;
	}

	RTL8367S_LOG_DEBUG("ret=%x\n", ret);

	return ret;
}

static int _rtl8367s_lookup_config_port(int pidx, rtk_uint32 fid, rtk_uint32 permit)
{
	rtk_portmask_t permitPort;
	rtk_svlan_memberCfg_t svlanCfg;
	rtk_uint32 vid = (pidx + RTL8367S_VID_RESERVED_NUM);
	int ret = 0;

	/* set iso of the port */
	memset(&permitPort, 0x0, sizeof(permitPort));
	permitPort.bits[0] |= permit;
	RTL8367S_LOG_DEBUG("rtk_port_isolation_set %d %x\r\n", rtl8367s_utp_port[pidx], permit);
	if((ret = rtk_port_isolation_set(rtl8367s_utp_port[pidx], &permitPort)) != RT_ERR_OK) 
	{
		RTL8367S_LOG_ERR("rtk_port_isolation_set fail...ret=%d\n", ret);
		return ret;
	}
	
	/* set fid of the port */
	if(CHIP_RTL8367D != g_switch_chip)
	{
		/* set fid of the port */
		RTL8367S_LOG_DEBUG("rtk_port_efid_set port=%d fid=%d\r\n", rtl8367s_utp_port[pidx], fid);
		if((ret = rtk_port_efid_set(rtl8367s_utp_port[pidx], fid)) != RT_ERR_OK)
		{
			RTL8367S_LOG_ERR("rtk_port_efid_set fail...ret=%d\n", ret);
			return ret;
		}
	}

	/* set vlan of the port */
	memset(&svlanCfg, 0x0, sizeof(svlanCfg));
	svlanCfg.svid = vid;
	if(CHIP_RTL8367D != g_switch_chip)
	{
		svlanCfg.efiden = ENABLED;
		svlanCfg.efid = fid;
	}
	else
	{	
		svlanCfg.chk_ivl_svl = 0;
		svlanCfg.ivl_svl = 1;
	}

	svlanCfg.memberport.bits[0] = ((1<<rtl8367s_cpu_port) | (1<<(rtl8367s_utp_port[pidx])));
	svlanCfg.untagport.bits[0]	= (1<<(rtl8367s_utp_port[pidx]));
	RTL8367S_LOG_DEBUG("rtk_svlan_memberPortEntry_set vid=%d fid=%d\r\n", vid, fid);
	if((ret = rtk_svlan_memberPortEntry_set(vid, &svlanCfg)) != RT_ERR_OK) 
	{
		RTL8367S_LOG_ERR("rtk_svlan_memberPortEntry_set fail...ret=%d\n", ret);
		return ret;
	}

	/* set vid of the port */
	RTL8367S_LOG_DEBUG("rtk_svlan_defaultSvlan_set port=%d vid=%d\r\n", rtl8367s_utp_port[pidx], vid);
	if((ret = rtk_svlan_defaultSvlan_set(rtl8367s_utp_port[pidx], vid)) != RT_ERR_OK) 
	{
		RTL8367S_LOG_ERR("rtk_svlan_defaultSvlan_set fail...ret=%d\n", ret);
		return ret;
	}
	
	/* set pvid for 8367d */
	if(CHIP_RTL8367D == g_switch_chip)
	{
		if ((ret=rtk_vlan_portPvid_set(rtl8367s_utp_port[pidx], vid, 0)) != RT_ERR_OK)
		{
			RTL8367S_LOG_ERR("rtk_vlan_portPvid_set fail...ret=%d\n", ret);
			return ret;
		}
	}

	return ret;
}

static int _rtl8367s_set_port_acl(int pidx, rtk_filter_id_t acl_filter_id, rtk_uint32 fid)
{
	rtk_filter_field_t field1;
	rtk_filter_cfg_t cfg;
	rtk_filter_action_t act;
	rtk_filter_number_t ruleNum = 0;
	int ret = 0;
	memset(&field1, 0x00, sizeof(rtk_filter_field_t));
	memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
	memset(&act, 0x00, sizeof(rtk_filter_action_t));
	/*All packets will be matched*/
	field1.fieldType = FILTER_FIELD_DMAC;
	if((ret = rtk_filter_igrAcl_field_add(&cfg, &field1)) != RT_ERR_OK)
	{
		RTL8367S_LOG_ERR("rtk_filter_igrAcl_field_add fail...ret=%d\n", ret);
		return ret;
	}
                
	/* Set port1 to active port */
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, rtl8367s_utp_port[pidx]);
	RTK_PORTMASK_ALLPORT_SET(cfg.activeport.mask);
                
	cfg.invert = FALSE;
	act.actEnable[FILTER_ENACT_CVLAN_INGRESS] = TRUE;
	act.filterCvlanVid = fid; /* CVID = fid */

	if((ret = rtk_filter_igrAcl_cfg_add(acl_filter_id, &cfg, &act, &ruleNum)) != RT_ERR_OK)
	{
		RTL8367S_LOG_ERR("rtk_filter_igrAcl_cfg_add fail...ret=%d\n", ret);
		return ret;
	}
	return ret;
}

static int _rtl8367s_set_cpuport_acl(int pidx, rtk_filter_id_t acl_filter_id, rtk_uint32 fid)
{
	rtk_filter_field_t field1;
	rtk_filter_cfg_t cfg;
	rtk_filter_action_t act;
	rtk_filter_number_t ruleNum = 0;
	rtk_uint32 vid = (pidx + RTL8367S_VID_RESERVED_NUM);
	int ret = 0;
	memset(&field1, 0x00, sizeof(rtk_filter_field_t));
	memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
	memset(&act, 0x00, sizeof(rtk_filter_action_t));
	/*All packets will be matched*/
	field1.fieldType = FILTER_FIELD_STAG;
	field1.filter_pattern_union.stag.vid.value = vid;
	field1.filter_pattern_union.stag.vid.mask = 0xfff;
	
	if((ret = rtk_filter_igrAcl_field_add(&cfg, &field1)) != RT_ERR_OK)
	{
		RTL8367S_LOG_ERR("rtk_filter_igrAcl_field_add fail...ret=%d\n", ret);
		return ret;
	}
                
	/* Set port1 to active port */
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, rtl8367s_cpu_port);
	RTK_PORTMASK_ALLPORT_SET(cfg.activeport.mask);
                
	cfg.invert = FALSE;
	act.actEnable[FILTER_ENACT_CVLAN_INGRESS] = TRUE;
	act.filterCvlanVid = fid; /* CVID = fid */

	if((ret = rtk_filter_igrAcl_cfg_add(acl_filter_id, &cfg, &act, &ruleNum)) != RT_ERR_OK)
	{
		RTL8367S_LOG_ERR("rtk_filter_igrAcl_cfg_add fail...ret=%d\n", ret);
		return ret;
	}
	return ret;
}

static int _rtl8367s_lookup_refresh(void)
{
	int p;
	rtk_uint32 fid;
	rtk_uint32 permit;
	rtk_filter_id_t acl_filter_id = 1;
	int ret = 0;

	/* init ACL function for 8367d */
	if(CHIP_RTL8367D == g_switch_chip)
	{
		rtl8367s_acl_init();
	}
	
	for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
	{
		permit = _rtl8367s_lookup_calc_permit(p);
		fid = _rtl8367s_lookup_calc_fid(p);

		RTL8367S_LOG_DEBUG("permit=%x fid=%d\n", permit, fid);

		if((ret = _rtl8367s_lookup_config_port(p, fid, permit)) !=  RT_ERR_OK)
		{
			return ret;
		}

		/* set acl forward rule for every svlan in 8367d */
		if(CHIP_RTL8367D == g_switch_chip)
		{
			if((ret = _rtl8367s_set_port_acl(p, acl_filter_id, fid)) !=  RT_ERR_OK)
			{
				return ret;
			}else{
				acl_filter_id++;
			}

			if((ret = _rtl8367s_set_cpuport_acl(p, acl_filter_id, fid)) !=  RT_ERR_OK)
			{
				return ret;
			}else{
				acl_filter_id++;
			}
		}
	}

	return ret;
}

static int _rtl8367s_mcast_mcr_refresh(void)
{
	rtk_portmask_t portmask;
	int p;
	
	memset(&portmask, 0x0, sizeof(portmask));
	
	/* igmp static router port is not supported in 8367d */
	if(CHIP_RTL8367D != g_switch_chip)
	{
		if(rtl8367s_mcast_snoop)
		{
			portmask.bits[0] |= (1<<rtl8367s_cpu_port);
			
			for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
			{
				if(rtl8368s_mc_router[p] == 2)
				{
					portmask.bits[0] |= (1<<rtl8367s_utp_port[p]);
				}
			}
		}
		
		if(RT_ERR_OK != rtk_igmp_static_router_port_set(&portmask))
		{
			RTL8367S_LOG_ERR("rtk_igmp_static_router_port_set error\r\n");
		}
	}

	
	if(RT_ERR_OK != rtk_igmp_static_router_port_set(&portmask))
	{
		RTL8367S_LOG_ERR("rtk_igmp_static_router_port_set error\r\n");
	}

	return 0;
}

static int _rtl8367s_mcast_refresh(void)
{
//	rtk_api_ret_t ret;
//	rtk_portmask_t portmask;
	int p;

	if(rtl8367s_mcast_snoop)
	{
		if(CHIP_RTL8367D != g_switch_chip)
		{
			rtk_igmp_state_set(ENABLED);

			if(RT_ERR_OK != rtk_l2_ipMcastForwardRouterPort_set(ENABLED))
			{
				RTL8367S_LOG_ERR("rtk_l2_ipMcastForwardRouterPort_set error\r\n");
			}
			
			for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
			{
				if(RT_ERR_OK != rtk_trap_unknownMcastPktAction_set(
					rtl8367s_utp_port[p], MCAST_IPV4, MCAST_ACTION_ROUTER_PORT))
				{
					RTL8367S_LOG_ERR("rtk_trap_unknownMcastPktAction_set error\r\n");
				}

				if(RT_ERR_OK != rtk_trap_unknownMcastPktAction_set(
					rtl8367s_utp_port[p], MCAST_IPV6, MCAST_ACTION_ROUTER_PORT))
				{
					RTL8367S_LOG_ERR("rtk_trap_unknownMcastPktAction_set error\r\n");
				}
			}
		}
	}
	else
	{
		/* not supported in 8367d */
		if(CHIP_RTL8367D != g_switch_chip)
		{
			rtk_igmp_state_set(DISABLED);

			if(RT_ERR_OK != rtk_l2_ipMcastForwardRouterPort_set(DISABLED))
			{
				RTL8367S_LOG_ERR("rtk_l2_ipMcastForwardRouterPort_set error\r\n");
			}

			for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
			{
				if(RT_ERR_OK != rtk_trap_unknownMcastPktAction_set(
					rtl8367s_utp_port[p], MCAST_IPV4, MCAST_ACTION_FORWARD))
				{
					RTL8367S_LOG_ERR("rtk_trap_unknownMcastPktAction_set error\r\n");
				}

				if(RT_ERR_OK != rtk_trap_unknownMcastPktAction_set(
					rtl8367s_utp_port[p], MCAST_IPV6, MCAST_ACTION_FORWARD))
				{
					RTL8367S_LOG_ERR("rtk_trap_unknownMcastPktAction_set error\r\n");
				}
			}
		}
	}

	_rtl8367s_mcast_mcr_refresh();

	return 0;
}


static int _rtl8367s_cpumac_config_port(int pidx, rtk_uint32 fid, rtk_uint32 permit)
{
	rtk_l2_ucastAddr_t l2_entry;
	rtk_mac_t mac;
	int ret = 0;

	memset(&mac, 0x00, sizeof(mac));
	mac.octet[0] = rtl8367s_cpu_mac[0];
	mac.octet[1] = rtl8367s_cpu_mac[1];
	mac.octet[2] = rtl8367s_cpu_mac[2];
	mac.octet[3] = rtl8367s_cpu_mac[3];
	mac.octet[4] = rtl8367s_cpu_mac[4];
	mac.octet[5] = rtl8367s_cpu_mac[5];

	memset(&l2_entry, 0x00, sizeof(l2_entry));
	l2_entry.port = rtl8367s_cpu_port;
	
	if(CHIP_RTL8367D == g_switch_chip)
	{
		l2_entry.cvid = fid;
	}
	else
	{
		l2_entry.efid = fid;
	}
	
	l2_entry.is_static = 1;

	if((ret = rtk_l2_addr_add(&mac, &l2_entry)) != RT_ERR_OK) 
	{
		RTL8367S_LOG_ERR("rtk_l2_addr_add fail...ret=%d\n", ret);
		return ret;
	}
	
	return ret;
}

static int _rtl8367s_cpumac_refresh(void)
{
	int p;
	rtk_uint32 fid;
	rtk_uint32 permit;
	int ret = 0;

	for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
	{
		if(rtl8367s_br_group[p] < 0)
		{
			/* skip none bridge port */
			continue;
		}
		
		permit = _rtl8367s_lookup_calc_permit(p);
		fid = _rtl8367s_lookup_calc_fid(p);

		RTL8367S_LOG_DEBUG("permit=%x fid=%d\n", permit, fid);

		if((ret = _rtl8367s_cpumac_config_port(p, fid, permit)) !=  RT_ERR_OK)
		{
			return ret;
		}
	}

	return ret;
}

static inline int _rtl8367s_is_switch_device(struct net_device * dev)
{
#if 0
	if(dev->priv_switch_flags & IFF_TP_EXT_SWITCH_DEV)
	{
		return 1;
	}
#endif
	return 0;
}

#if 0
static int _rtl8367s_device_to_pidx(struct net_device * dev)
{
	int ret = -1;

	if(!_rtl8367s_is_switch_device(dev))
	{
		return -1;
	}

	ret = (dev->priv_switch_flags & IFF_TP_EXT_SWITCH_IDX_MASK);
	if(ret >= PORT_MAX_NUM)
	{
		return -1;
	}

	return ret;
}
#endif

#ifdef RTL8367S_HW_LED_FUNCTION
static int _rtl8367s_hw_led_refresh(void)
{
	int ret = 0;
	int p;

	rtk_led_force_mode_t mode = 0;
	
	switch(rtl8367s_hw_led)
	{
		case 0:
			mode = LED_FORCE_OFF;
			break;
		case 1:
			mode = LED_FORCE_NORMAL;
			break;
		case 2:
			mode = LED_FORCE_ON;
			break;
		default:
			RTL8367S_LOG_ERR("wrong hw_led %d\n", rtl8367s_hw_led);
			return -1;
	}
	
	for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
	{
		if((ret = rtk_led_modeForce_set(rtl8367s_utp_port[p], LED_GROUP_1, mode)) != RT_ERR_OK)
		{
			RTL8367S_LOG_ERR("rtk_led_modeForce_set failed...ret=%d\n", ret);
			return -1;
		}
	}

	return 0;
}
#endif /* RTL8367S_HW_LED_FUNCTION */
#if 0
static int rtl8367s_br_should_forward(struct net_device * in, struct net_device * out)
{
	int pin, pout;
	
	if(!in || !out)
	{
		RTL8367S_LOG_ERR("in or out null\r\n");
		return 1;
	}
	
	if(!rtl8367s_hw_bridge)
	{
		return 1;
	}
	
	pin = _rtl8367s_device_to_pidx(in);
	pout =  _rtl8367s_device_to_pidx(out);

	if(pin<0 || pout<0)
	{
		return 1;
	}

	if(!rtl8367s_hw_bridge_port[pin] || !rtl8367s_hw_bridge_port[pout])
	{
		return 1;
	}

#ifdef RTL8367S_HW_BRIDGE_PORT_VLAN_DETECT
	if(rtl8367s_hw_bridge_vlan_port[pin] || rtl8367s_hw_bridge_vlan_port[pout])
	{
		return 1;
	}
#endif /**/

	/* forbid tx/rx between hw switch ports by linux bridge */
	return 0;
}

static int rtl8367s_notify_if_change(struct net_device *br, struct net_device *dev, int add)
{
	int p;
	int old_group;
	int new_group;
	int ret = 0;

	if(!br || !dev)
	{
		RTL8367S_LOG_ERR("br or dev null\r\n");
		return -1;
	}

	if(!_rtl8367s_is_switch_device(dev))
	{
		return 0;
	}

	RTL8367S_LOG_DEBUG("%s %s %d\r\n", br->name, dev->name, add);

	RTL8367S_LOCK();

	p = _rtl8367s_device_to_pidx(dev);

	if(p < 0)
	{
		RTL8367S_LOG_DEBUG("wrong dev %s\r\n", dev->name);
		ret = -1;
		goto error;
	}
	
	old_group = rtl8367s_br_group[p];
	
	if(add)
	{
		if(old_group >= 0)
		{
			RTL8367S_LOG_ERR("wrong old group index %d\r\n", old_group);
			ret = -1;
			goto error;
		}
		new_group = br->ifindex;
	}
	else
	{
		if(old_group < 0)
		{
			RTL8367S_LOG_ERR("wrong old group index %d\r\n", old_group);
			ret = -1;
			goto error;
		}
		new_group = -1;
	}
	
	if(old_group == new_group)
	{
		RTL8367S_LOG_ERR("same group %d\r\n", old_group);
		ret = -1;
		goto error;
	}
	
	rtl8367s_br_group[p] = new_group;
	rtl8367s_iso_mode[p] = 0;
	rtl8368s_mc_router[p] = 1;
	
	if(rtl8367s_hw_bridge)
	{
		_rtl8367s_lookup_refresh();
	}
	
	if(rtl8367s_mcast_snoop && CHIP_RTL8367D != g_switch_chip)
	{
		_rtl8367s_mcast_mcr_refresh();
	}

	rtk_l2_table_clear();

	if(rtl8367s_cpu_mac[0] !=0 ||
		rtl8367s_cpu_mac[1] !=0 ||
		rtl8367s_cpu_mac[2] !=0 ||
		rtl8367s_cpu_mac[3] !=0 ||
		rtl8367s_cpu_mac[4] !=0 ||
		rtl8367s_cpu_mac[5] !=0)
	{
		_rtl8367s_cpumac_refresh();
	}

error:
	RTL8367S_UNLOCK();

	return ret;
}

static int rtl8367s_notify_iso_change(struct net_device *br, struct net_device *dev, int iso)
{
	int p;
	int cur_group;
	int tmp_group;
	int ret = 0;

	if(!br || !dev)
	{
		RTL8367S_LOG_ERR("br or dev null\r\n");
		return -1;
	}

	if(!_rtl8367s_is_switch_device(dev))
	{
		return 0;
	}

	RTL8367S_LOG_DEBUG("%s %s %d\r\n", br->name, dev->name, iso);

	RTL8367S_LOCK();

	p = _rtl8367s_device_to_pidx(dev);

	if(p < 0)
	{
		RTL8367S_LOG_DEBUG("wrong dev %s\r\n", dev->name);
		ret = -1;
		goto error;
	}

	cur_group = rtl8367s_br_group[p];
	
	tmp_group = br->ifindex;

	if(tmp_group != cur_group)
	{
		RTL8367S_LOG_ERR("not same group %d\r\n", cur_group);
		ret = -1;
		goto error;
	}

	if(rtl8367s_iso_mode[p] == iso)
	{
		RTL8367S_LOG_ERR("same iso %d\r\n", iso);
		ret = -1;
		goto error;
	}

	rtl8367s_iso_mode[p] = iso;

	if(rtl8367s_hw_bridge)
	{
		_rtl8367s_lookup_refresh();
	}

	rtk_l2_table_clear();
	
	if(rtl8367s_cpu_mac[0] !=0 ||
		rtl8367s_cpu_mac[1] !=0 ||
		rtl8367s_cpu_mac[2] !=0 ||
		rtl8367s_cpu_mac[3] !=0 ||
		rtl8367s_cpu_mac[4] !=0 ||
		rtl8367s_cpu_mac[5] !=0)
	{
		_rtl8367s_cpumac_refresh();
	}
error:
	RTL8367S_UNLOCK();

	return ret;
}


static int rtl8367s_notify_mcr_change(struct net_device *br, struct net_device *dev, int mcr)
{
	int p;
	int cur_group;
	int tmp_group;
	int ret = 0;

//	rtk_portmask_t portmask;

	if(!br || !dev)
	{
		RTL8367S_LOG_ERR("br or dev null\r\n");
		return -1;
	}

	if(!_rtl8367s_is_switch_device(dev))
	{
		return 0;
	}

	RTL8367S_LOG_DEBUG("%s %s %d\r\n", br->name, dev->name, mcr);

	RTL8367S_LOCK();

	p = _rtl8367s_device_to_pidx(dev);

	if(p < 0)
	{
		RTL8367S_LOG_DEBUG("wrong dev %s\r\n", dev->name);
		ret = -1;
		goto error;
	}
	
	cur_group = rtl8367s_br_group[p];

	tmp_group = br->ifindex;

	if(tmp_group != cur_group)
	{
		RTL8367S_LOG_ERR("not same bridge %d\r\n", cur_group);
		ret = -1;
		goto error;
	}
	
	if(rtl8368s_mc_router[p] == mcr)
	{
		RTL8367S_LOG_ERR("same mcr %d\r\n", mcr);
		ret = -1;
		goto error;
	}
	
	rtl8368s_mc_router[p] = mcr;

	if(rtl8367s_mcast_snoop)
	{
		_rtl8367s_mcast_mcr_refresh();
	}
	
error:
	RTL8367S_UNLOCK();

	return ret;
}
#endif
#ifdef RTL8367S_HW_BRIDGE_PORT_VLAN_DETECT
static int rtl8367s_notify_vlan_change(struct net_device *dev, int add)
{
	int p;
	int ret = 0;

	if(!dev)
	{
		RTL8367S_LOG_ERR("br or dev null\r\n");
		return -1;
	}

	if(!_rtl8367s_is_switch_device(dev))
	{
		return 0;
	}

	RTL8367S_LOG_DEBUG("%s %d\r\n", dev->name, add);

	RTL8367S_LOCK();

	p = _rtl8367s_device_to_pidx(dev);

	if(p < 0)
	{
		RTL8367S_LOG_DEBUG("wrong dev %s\r\n", dev->name);
		ret = -1;
		goto error;
	}
	
	if(add)
	{
		rtl8367s_hw_bridge_vlan_port[p]++;
	}
	else
	{
		rtl8367s_hw_bridge_vlan_port[p]--;
		if(rtl8367s_hw_bridge_vlan_port[p] < 0)
		{
			rtl8367s_hw_bridge_vlan_port[p] = 0;
			RTL8367S_LOG_ERR("vlan_port %d below zero\r\n", p);
		}
	}
	
	if(rtl8367s_hw_bridge)
	{
		_rtl8367s_lookup_refresh();
	}
	
	if(rtl8367s_mcast_snoop)
	{
		_rtl8367s_mcast_mcr_refresh();
	}

	rtk_l2_table_clear();

	if(rtl8367s_cpu_mac[0] !=0 ||
		rtl8367s_cpu_mac[1] !=0 ||
		rtl8367s_cpu_mac[2] !=0 ||
		rtl8367s_cpu_mac[3] !=0 ||
		rtl8367s_cpu_mac[4] !=0 ||
		rtl8367s_cpu_mac[5] !=0)
	{
		_rtl8367s_cpumac_refresh();
	}

error:
	RTL8367S_UNLOCK();

	return ret;
}
#endif /* RTL8367S_HW_BRIDGE_PORT_VLAN_DETECT */
#if 0
static int rtl8367s_ethtool_get_settings(struct net_device * dev, struct ethtool_cmd * cmd)
{
	rtk_api_ret_t ret;
	rtk_port_mac_ability_t ability;
	int p;
	
	if(!dev || !cmd)
	{
		RTL8367S_LOG_ERR("dev or cmd null\r\n");
		return -1;
	}

	if(!_rtl8367s_is_switch_device(dev))
	{
		return 0;
	}
	
	RTL8367S_LOG_DEBUG("dev=%s\r\n", dev->name);

	p = _rtl8367s_device_to_pidx(dev);

	if(p < 0)
	{
		RTL8367S_LOG_DEBUG("wrong dev %s\r\n", dev->name);
		return -1;
	}
	
	if((ret = rtk_port_macStatus_get(rtl8367s_utp_port[p], &ability) != RT_ERR_OK))
	{
		RTL8367S_LOG_ERR("rtk_port_macStatus_get failed...ret=%d\n", ret);
		return -1;
	}

	if(ability.link == 0)
	{
		cmd->speed = 0;
		cmd->duplex = DUPLEX_UNKNOWN;
		return 0;
	}
	
	switch(ability.speed)
	{
		case PORT_SPEED_10M:
			cmd->speed = SPEED_10;
			break;
			
		case PORT_SPEED_100M:
			cmd->speed = SPEED_100;
			break;

		case PORT_SPEED_1000M:
			cmd->speed = SPEED_1000;
			break;

		case PORT_SPEED_500M:
			cmd->speed = 500; /* SPEED_500 not defined */
			break;

		case PORT_SPEED_2500M:
			cmd->speed = SPEED_2500;
			break;
		
		default:
			RTL8367S_LOG_ERR("unknown ethernet speed (%d)\n", ability.speed);
			cmd->speed = 0;
			return -1;
	}

	switch(ability.duplex)
	{
		case PORT_HALF_DUPLEX:
			cmd->duplex = DUPLEX_HALF;
			break;
			
		case PORT_FULL_DUPLEX:
			cmd->duplex = DUPLEX_FULL;
			break;

		default:
			cmd->duplex = DUPLEX_UNKNOWN;
			return -1;
	}

	return 0;
}

static int rtl8367s_dev_get_stats(struct net_device *dev,
					struct rtnl_link_stats64 *storage)
{
	int p;
	rtk_stat_counter_t cnt;

	if(!dev)
	{
		RTL8367S_LOG_ERR("dev or cmd null\r\n");
		return -1;
	}

	if(!_rtl8367s_is_switch_device(dev))
	{
		return 0;
	}
	
	RTL8367S_LOG_DEBUG("dev=%s\r\n", dev->name);

	p = _rtl8367s_device_to_pidx(dev);

	if(p < 0)
	{
		RTL8367S_LOG_DEBUG("wrong dev %s\r\n", dev->name);
		return -1;
	}

	memset(storage, 0x0, sizeof(*storage));
	
	if(RT_ERR_OK != rtk_stat_port_get(rtl8367s_utp_port[p], STAT_IfInUcastPkts, &cnt))
	{
		RTL8367S_LOG_ERR("rtk_stat_port_get fail\r\n");
		return -1;
	}

	storage->rx_packets += cnt;

	if(RT_ERR_OK != rtk_stat_port_get(rtl8367s_utp_port[p], STAT_IfInMulticastPkts, &cnt))
	{
		RTL8367S_LOG_ERR("rtk_stat_port_get fail\r\n");
		return -1;
	}

	storage->rx_packets += cnt;
	storage->multicast += cnt;

	if(RT_ERR_OK != rtk_stat_port_get(rtl8367s_utp_port[p], STAT_IfInBroadcastPkts, &cnt))
	{
		RTL8367S_LOG_ERR("rtk_stat_port_get fail\r\n");
		return -1;
	}

	storage->rx_packets += cnt;

	if(RT_ERR_OK != rtk_stat_port_get(rtl8367s_utp_port[p], STAT_IfInOctets, &cnt))
	{
		RTL8367S_LOG_ERR("rtk_stat_port_get fail\r\n");
		return -1;
	}
	
	storage->rx_bytes += cnt;

	if(RT_ERR_OK != rtk_stat_port_get(rtl8367s_utp_port[p], STAT_IfOutUcastPkts, &cnt))
	{
		RTL8367S_LOG_ERR("rtk_stat_port_get fail\r\n");
		return -1;
	}

	storage->tx_packets += cnt;

	if(RT_ERR_OK != rtk_stat_port_get(rtl8367s_utp_port[p], STAT_IfOutMulticastPkts, &cnt))
	{
		RTL8367S_LOG_ERR("rtk_stat_port_get fail\r\n");
		return -1;
	}

	storage->tx_packets += cnt;

	if(RT_ERR_OK != rtk_stat_port_get(rtl8367s_utp_port[p], STAT_IfOutBroadcastPkts, &cnt))
	{
		RTL8367S_LOG_ERR("rtk_stat_port_get fail\r\n");
		return -1;
	}

	storage->tx_packets += cnt;

	if(RT_ERR_OK != rtk_stat_port_get(rtl8367s_utp_port[p], STAT_IfOutOctets, &cnt))
	{
		RTL8367S_LOG_ERR("rtk_stat_port_get fail\r\n");
		return -1;
	}
	
	storage->tx_bytes += cnt;

	return 0;

}
#endif

static int rtl8367s_phy_link_change_notify(void)
{
	rtk_api_ret_t ret;
	rtk_port_mac_ability_t ability;
	int link_status = 0;
	int p;
	
	struct net_device* port_dev;
	
	for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
	{
		if((ret = rtk_port_macStatus_get(rtl8367s_utp_port[p], &ability) != RT_ERR_OK))
		{
			RTL8367S_LOG_ERR("rtk_port_macStatus_get failed...ret=%d, port=%d\n", ret, rtl8367s_utp_port[p]);
		}
		
		link_status = ability.link;
		
		if(link_status != rtl8368s_link_status[p])
		{
			RTL8367S_LOG_INFO("PORT%d (%s) link %s\r\n",
				p, rtl8367s_dev_name[p], link_status? "up":"down");
			
			rtl8368s_link_status[p] = link_status;
		}
		
		port_dev = dev_get_by_name(&init_net, rtl8367s_dev_name[p]);
		
		if(port_dev)
		{
			if(_rtl8367s_is_switch_device(port_dev))
			{
				if(link_status)
				{
					netif_carrier_on(port_dev);
				}
				else
				{
					netif_carrier_off(port_dev);
				}
			}
			else
			{
				//RTL8367S_LOG_DEBUG("not switch device: %s\n", port_dev->name);
			}
			dev_put(port_dev);
		}
	}
	return 0;
}

static struct timer_list rtl8367s_phy_link_timer;

static void rtl8367s_phy_link_work_cb(struct work_struct *work)
{
    rtl8367s_phy_link_change_notify();
    mod_timer(&rtl8367s_phy_link_timer, jiffies + msecs_to_jiffies(RTL8367S_PHY_LINK_CHECK_INTERVAL));
}

DECLARE_WORK(_rtl8367s_phy_link_work, rtl8367s_phy_link_work_cb);

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
static void rtl8367s_phy_link_timer_cb(unsigned long data)
#else
static void rtl8367s_phy_link_timer_cb(struct timer_list *tl)
#endif
{
    schedule_work(&_rtl8367s_phy_link_work);
}

static void rtl8367s_phy_link_timer_start(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
    init_timer(&rtl8367s_phy_link_timer);
    rtl8367s_phy_link_timer.function = rtl8367s_phy_link_timer_cb;
#else
    timer_setup(&rtl8367s_phy_link_timer, rtl8367s_phy_link_timer_cb, 0);
#endif
    rtl8367s_phy_link_timer.expires = jiffies + msecs_to_jiffies(RTL8367S_PHY_LINK_CHECK_INTERVAL);
    add_timer(&rtl8367s_phy_link_timer);
}

static ssize_t rtl8367s_switch_reg_write(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 reg, val;
	int argc=0;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	argc=sscanf(val_string, "%x %x", &reg, &val);
	if(argc!=1 && argc!=2)
	{
		printk("usage: <action> reg [val]\n");
		return count;
	}

	if(argc==2)
	{
#ifdef CONFIG_TP_EXT_SWITCH_RTL8367S_SINGLE
		if(RT_ERR_OK != rtl8367c_setAsicReg(reg, val))
#else
		if(RT_ERR_OK != RT_MAPPER->asic_setAsicReg(reg, val))
#endif

		{
			RTL8367S_LOG_ERR("rtl8367c_setAsicReg error\r\n");
		}
	}
	
#ifdef CONFIG_TP_EXT_SWITCH_RTL8367S_SINGLE
	if(RT_ERR_OK != rtl8367c_getAsicReg(reg, &val))
#else
	if(RT_ERR_OK != RT_MAPPER->asic_getAsicReg(reg, &val))
#endif
	{
		RTL8367S_LOG_ERR("rtl8367c_getAsicReg error\r\n");
	}
	
	RTL8367S_LOG_INFO("reg[%08x]=%08x\r\n", reg, val);
	
	return count;
}

static struct file_operations rtl8367s_switch_reg_fops = {
	.owner 		= THIS_MODULE,
	.open		= NULL,
	.read	 	= NULL,
	.write		= rtl8367s_switch_reg_write,
	.llseek 	= NULL,  
	.release	= NULL,
};


static ssize_t rtl8367s_phy_reg_write(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 phy, reg, val;
	int argc=0;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	argc=sscanf(val_string, "%d %x %x", &phy, &reg, &val);
	if(argc!=2 && argc!=3)
	{
		printk("usage: <action> phy reg [val]\n");
		return count;
	}

	if(argc==3)
	{
		if(RT_ERR_OK != rtk_port_phyReg_set(rtl8367s_utp_port[phy], reg, val))
		{
			RTL8367S_LOG_ERR("rtk_port_phyReg_set error\r\n");
		}
	}
	
	if(RT_ERR_OK != rtk_port_phyReg_get(rtl8367s_utp_port[phy], reg, &val))
	{
		RTL8367S_LOG_ERR("rtk_port_phyReg_get error\r\n");
	}
	
	RTL8367S_LOG_INFO("phy=%d reg[%08x]=%08x\r\n", phy, reg, val);
	
	return count;
}

static struct file_operations rtl8367s_phy_reg_fops = {
	.owner 		= THIS_MODULE,
	.open		= NULL,
	.read	 	= NULL,
	.write		= rtl8367s_phy_reg_write,
	.llseek 	= NULL,  
	.release	= NULL,
};

static int rtl8367s_phy_status_get(struct seq_file *s, void *unused)
{
	char *speed_str[] = {"10M", "100M", "1000M", "500M", "2.5G", "None"};
	char *duplex_str[] = {"HD", "FD", "None"};
	
	rtk_port_linkStatus_t pLinkStatus[PORT_MAX_NUM] = {0};
	rtk_port_speed_t pSpeed[PORT_MAX_NUM] = {0};
	rtk_port_duplex_t pDuplex[PORT_MAX_NUM] = {0};
	int ret = 0;
	int p = 0;

	for(p=0; p<PORT_MAX_NUM; p++)
	{
		ret =  rtk_port_phyStatus_get(rtl8367s_utp_port[p], &pLinkStatus[p], &pSpeed[p], &pDuplex[p]);
		if (RT_ERR_OK != ret)
		{
			RTL8367S_LOG_ERR("rtk_port_phyStatus_get failed..., ret is %d, port is %d\n", ret, rtl8367s_utp_port[p]);
			goto error;
		}
	}

	for(p=0; p<PORT_MAX_NUM; p++)
	{
		seq_printf(s, "%u ", pLinkStatus[p]);
	}
	seq_printf(s, "\n");
	
	for(p=0; p<PORT_MAX_NUM; p++)
	{
		seq_printf(s, "%s ", speed_str[pSpeed[p]]);
	}
	seq_printf(s, "\n");

	for(p=0; p<PORT_MAX_NUM; p++)
	{
		seq_printf(s, "%s ", duplex_str[pDuplex[p]]);
	}
	seq_printf(s, "\n");

error:

	return 0;
}

static int rtl8367s_phy_status_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_phy_status_get, file->f_path.dentry->d_iname);
}

static struct file_operations rtl8367s_phy_status_fops = {
	.owner		= THIS_MODULE,
	.open		= rtl8367s_phy_status_open,
	.read		= seq_read,
	.write		= NULL,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};




static rtk_api_ret_t rtl8367s_phy_Patch_set(rtk_uint32 opt)
{
#define RTL8367C_PHY_CONTROL                 0

	rtk_api_ret_t retVal = -1;
	rtk_uint32 port = 0;
	rtk_uint32 data = 0;

	if (opt != 0 && opt != 1)
		return RT_ERR_FAILED;

	for (port = 0; port <= 4; port++) {
#ifdef CONFIG_TP_EXT_SWITCH_RTL8367S_SINGLE
		if ((retVal = rtl8367c_getAsicPHYOCPReg(port, 0xA42C, &data)) != RT_ERR_OK)
#else
		if ((retVal = RT_MAPPER->asic_getAsicPHYOCPReg(port, 0xA42C, &data)) != RT_ERR_OK)
#endif
		return retVal;

		data |= 0x0010;
#ifdef CONFIG_TP_EXT_SWITCH_RTL8367S_SINGLE
		if ((retVal = rtl8367c_setAsicPHYOCPReg(port, 0xA42C, data)) != RT_ERR_OK)
#else
		if ((retVal = RT_MAPPER->asic_setAsicPHYOCPReg(port, 0xA42C, data)) != RT_ERR_OK)
#endif
		return retVal;

		if (opt == 0) {
#ifdef CONFIG_TP_EXT_SWITCH_RTL8367S_SINGLE
			if((retVal = rtl8367c_setAsicPHYOCPReg(port, 0xBC02, 0x00D0)) != RT_ERR_OK)
#else
			if((retVal = RT_MAPPER->asic_setAsicPHYOCPReg(port, 0xBC02, 0x00D0)) != RT_ERR_OK)
#endif
			return retVal;
		}
		else {
#ifdef CONFIG_TP_EXT_SWITCH_RTL8367S_SINGLE
			if((retVal = rtl8367c_setAsicPHYOCPReg(port, 0xBC02, 0x00F0)) != RT_ERR_OK)
#else
			if((retVal = RT_MAPPER->asic_setAsicPHYOCPReg(port, 0xBC02, 0x00F0)) != RT_ERR_OK)
#endif
			return retVal;
		}

#ifdef CONFIG_TP_EXT_SWITCH_RTL8367S_SINGLE
		if((retVal = rtl8367c_getAsicPHYReg(port, RTL8367C_PHY_CONTROL, &data)) != RT_ERR_OK)
#else
		if ((retVal = RT_MAPPER->asic_getAsicPHYReg(port, RTL8367C_PHY_CONTROL, &data)) != RT_ERR_OK)
#endif
		return retVal;

		data |= (0x0001 << 9);
#ifdef CONFIG_TP_EXT_SWITCH_RTL8367S_SINGLE
		if((retVal = rtl8367c_setAsicPHYReg(port, RTL8367C_PHY_CONTROL, data)) != RT_ERR_OK)
#else
		if ((retVal = RT_MAPPER->asic_setAsicPHYReg(port, RTL8367C_PHY_CONTROL, data)) != RT_ERR_OK)
#endif
		return retVal;
    }

    return RT_ERR_OK;
}

static int rtl8367s_mii_type_get(struct seq_file *s, void *unused)
{
	seq_printf(s, "%d\n", rtl8367s_mii_type);
	return 0;
}

static int rtl8367s_mii_type_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_mii_type_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8367s_mii_type_write(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 val;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}

	RTL8367S_LOCK();
	
	if(rtl8367s_mii_type == val)
	{
		RTL8367S_LOG_ERR("same mii_type %d\n", val);
		goto error;
	}

	rtl8367s_mii_type = val;

	_rtl8367s_mii_type_refresh();

error:	
	RTL8367S_UNLOCK();

	return count;
}


static struct file_operations rtl8367s_mii_type_fops = {
	.owner		= THIS_MODULE,
	.open		= rtl8367s_mii_type_open,
	.read		= seq_read,
	.write		= rtl8367s_mii_type_write,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};

static int rtl8367s_ucast_tbl_get(struct seq_file *s, void *unused)
{
	rtk_uint32 address=0;
	rtk_l2_ucastAddr_t ucastAddr;
	
	while(1)
	{
		if(RT_ERR_OK != rtk_l2_addr_next_get(READMETHOD_NEXT_L2UC, UTP_PORT0, &address, &ucastAddr))
		{
			break;
		}
		seq_printf(s, 
			"%04d) mac=%02x-%02x-%02x-%02x-%02x-%02x, ivl=%d, fid=%d, efid=%d, cvid=%04x, static=%d, prio=%d, port=%d\n", 
			ucastAddr.address,
			ucastAddr.mac.octet[0], 
			ucastAddr.mac.octet[1],
			ucastAddr.mac.octet[2],
			ucastAddr.mac.octet[3],
			ucastAddr.mac.octet[4],
			ucastAddr.mac.octet[5],
			ucastAddr.ivl,
			ucastAddr.fid,
			ucastAddr.efid,
			ucastAddr.cvid,
			ucastAddr.is_static,
			ucastAddr.priority,
			ucastAddr.port);

		address++;
	}

	return 0;
}

static int rtl8367s_ucast_tbl_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_ucast_tbl_get, file->f_path.dentry->d_iname);
}


static ssize_t rtl8367s_ucast_tbl_write(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 val;
	rtk_uint32 address=0;
	rtk_l2_ucastAddr_t ucastAddr;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action> -1: delete all entry, (0~...): delete specific entry\n");
		return count;
	}

	if(val < 0)
	{
		while(1)
		{
			address=0;
			if(RT_ERR_OK != rtk_l2_addr_next_get(READMETHOD_NEXT_L2UC, UTP_PORT0, &address, &ucastAddr))
			{
				RTL8367S_LOG_ERR("rtk_l2_addr_next_get fail\r\n");
				break;
			}
			rtk_l2_addr_del(&ucastAddr.mac, &ucastAddr);
		}
		printk("rtk_l2_addr_del delete all done\r\n");
	}
	else
	{
		address=val;
		if(RT_ERR_OK != rtk_l2_addr_next_get(READMETHOD_NEXT_L2UC, UTP_PORT0, &address, &ucastAddr))
		{
			RTL8367S_LOG_ERR("rtk_l2_addr_next_get fail\r\n");
			return count;
		}
		rtk_l2_addr_del(&ucastAddr.mac, &ucastAddr);

		printk("rtk_l2_addr_del %02x-%02x-%02x-%02x-%02x-%02x done\r\n",
			ucastAddr.mac.octet[0], ucastAddr.mac.octet[1], ucastAddr.mac.octet[2], \
			ucastAddr.mac.octet[3], ucastAddr.mac.octet[4], ucastAddr.mac.octet[5]);
	}
	
	return count;
}


static struct file_operations rtl8367s_ucast_tbl_fops = {
	.owner		= THIS_MODULE,
	.open		= rtl8367s_ucast_tbl_open,
	.read		= seq_read,
	.write		= rtl8367s_ucast_tbl_write,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};

static int rtl8367s_mcast_tbl_get(struct seq_file *s, void *unused)
{
	rtk_uint32 address=0;
	rtk_l2_ipMcastAddr_t ipMcastAddr;
#if 0
	rtk_l2_ipVidMcastAddr_t ipvidMcastAddr;
#endif
	rtk_l2_mcastAddr_t l2McastAddr;

	seq_printf(s, "================IPv4 Mcast Address===========\r\n");

	while(1)
	{
		if(RT_ERR_OK != rtk_l2_ipMcastAddr_next_get(&address, &ipMcastAddr))
		{
			break;
		}

		ipMcastAddr.dip |= 0xe0000000;
		
		seq_printf(s, "dip=0x%08x, sip=0x%08x, asic=%d index=%d ports=0x%08x\n", 
			ipMcastAddr.dip, ipMcastAddr.sip, ipMcastAddr.igmp_asic, ipMcastAddr.igmp_index, ipMcastAddr.portmask.bits[0]);
		
		address++;
	}

#if 0
	seq_printf(s, "================IPv4 VID Mcast Address===========\r\n");

	address=0;
	while(1)
	{
		if(RT_ERR_OK != rtk_l2_ipVidMcastAddr_next_get(&address, &ipvidMcastAddr))
		{
			break;
		}

		ipvidMcastAddr.dip |= 0xe0000000;
		
		seq_printf(s, "dip=0x%08x, sip=0x%08x, vid=%d port=0x%08x\n", 
			ipvidMcastAddr.dip, ipvidMcastAddr.sip, ipvidMcastAddr.vid, ipvidMcastAddr.portmask.bits[0]);
		
		address++;
	}
#endif
	
	seq_printf(s, "================IPv6 Mcast Address===========\r\n");
	
	address=0;
	while(1)
	{
		if(RT_ERR_OK != rtk_l2_mcastAddr_next_get(&address, &l2McastAddr))
		{
			break;
		}
		
		seq_printf(s, "mac=%02x-%02x-%02x-%02x-%02x-%02x fid=%d ivl=%d vid=%04x asic=%d index=%d ports=0x%08x\n", 
			l2McastAddr.mac.octet[0], l2McastAddr.mac.octet[1], l2McastAddr.mac.octet[2], 
			l2McastAddr.mac.octet[3], l2McastAddr.mac.octet[4], l2McastAddr.mac.octet[5], 
			l2McastAddr.fid, l2McastAddr.ivl, l2McastAddr.vid, l2McastAddr.igmp_asic, l2McastAddr.igmp_index,
			l2McastAddr.portmask.bits[0]);
		
		address++;
	}
	
	return 0;

}

static int rtl8367s_mcast_tbl_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_mcast_tbl_get, file->f_path.dentry->d_iname);
}


static ssize_t rtl8367s_mcast_tbl_write(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 val;

	rtk_uint32 address=0;
	rtk_l2_ipMcastAddr_t ipMcastAddr;
	rtk_l2_mcastAddr_t l2McastAddr;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}
	
	memset(&ipMcastAddr, 0x0, sizeof(ipMcastAddr));
	
	while(1)
	{
		address = 0;
		if(RT_ERR_OK != rtk_l2_ipMcastAddr_next_get(&address, &ipMcastAddr))
		{
			break;
		}

		ipMcastAddr.dip |= 0xe0000000;
		
		if(RT_ERR_OK != rtk_l2_ipMcastAddr_del(&ipMcastAddr))
		{
			RTL8367S_LOG_ERR("rtk_l2_ipMcastAddr_del fail\r\n");
		}
	}

	while(1)
	{
		address = 0;
		if(RT_ERR_OK != rtk_l2_mcastAddr_next_get(&address, &l2McastAddr))
		{
			break;
		}
		
		if(RT_ERR_OK != rtk_l2_mcastAddr_del(&l2McastAddr))
		{
			RTL8367S_LOG_ERR("rtk_l2_mcastAddr_del fail\r\n");
		}
	}

	return count;
}


static struct file_operations rtl8367s_mcast_tbl_fops = {
	.owner 		= THIS_MODULE,
	.open		= rtl8367s_mcast_tbl_open,
	.read		= seq_read,
	.write		= rtl8367s_mcast_tbl_write,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};


static int rtl8367s_mcast_snoop_get(struct seq_file *s, void *unused)
{
	int p;
	
	seq_printf(s, "mcast_snoop = %d\n", rtl8367s_mcast_snoop);

	seq_printf(s,"============================\n");
	
	for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
	{
		seq_printf(s,"port=%d dev=%s mcr=%d\n", 
			p, rtl8367s_dev_name[p], rtl8368s_mc_router[p]);
	}
	
    return 0;
}

static int rtl8367s_mcast_snoop_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_mcast_snoop_get, file->f_path.dentry->d_iname);
}


static ssize_t rtl8367s_mcast_snoop_write(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 val, pidx;
	int argc=0;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	argc= sscanf(val_string, "%d %d", &val, &pidx);

	if (argc != 1 && argc !=2)
	{
		printk("usage: <action> enable [ port ]\n");
		return count;
	}

	RTL8367S_LOCK();

	if(argc == 1)
	{
		if(rtl8367s_mcast_snoop == val)
		{
			RTL8367S_LOG_ERR("same mcast_snoop %d\r\n", val);
			goto error;
		}

		rtl8367s_mcast_snoop = val;

		_rtl8367s_mcast_refresh();
	}
	else if(argc == 2)
	{
		if(pidx <0 || pidx >= PORT_MAX_NUM)
		{
			RTL8367S_LOG_ERR("pidx out of range %d\n", pidx);
			goto error;
		}

		if(rtl8367s_br_group[pidx] >= 0)
		{
			RTL8367S_LOG_ERR("do not change bridge port's mc_router\n");
			goto error;
		}
		
		if(rtl8368s_mc_router[pidx] == val)
		{
			RTL8367S_LOG_ERR("same mc_router %d\n", val);
			goto error;
		}

		rtl8368s_mc_router[pidx] = val;

		if(rtl8367s_mcast_snoop)
		{
			_rtl8367s_mcast_mcr_refresh();
		}
	}
	else
	{
		RTL8367S_LOG_ERR("wrong argc %d\n", argc);
		goto error;
	}

error:
	RTL8367S_UNLOCK();
	
	return count;
}


static struct file_operations rtl8367s_mcast_snoop_fops = {
	.owner 		= THIS_MODULE,
	.open		= rtl8367s_mcast_snoop_open,
	.read	 	= seq_read,
	.write		= rtl8367s_mcast_snoop_write,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};



static int rtl8367s_port_mirror_get(struct seq_file *s, void *unused)
{
	seq_printf(s, "port_mirror = %d\n", rtl8367s_port_mirror);
	
    return 0;
}

static int rtl8367s_port_mirror_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_port_mirror_get, file->f_path.dentry->d_iname);
}


static ssize_t rtl8367s_port_mirror_write(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	int argc;
	
	int val, ming_port, mred_port, isolation;
	rtk_portmask_t txmask, rxmask;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	argc = sscanf(val_string, "%d %d %d %d", &val, &ming_port, &mred_port, &isolation);
	if(argc!=1 && argc!=4)
	{
		printk("usage: <action> mirror_en [mirroring_port] [mirrored_port] [isolation]\n");
		return count;
	}

	if(val)
	{
		if(argc < 4)
		{
			RTL8367S_LOG_ERR("missing mirroring_port mirrored_port isolation\r\n");
			return count;
		}
		
		if(ming_port == mred_port)
		{
			RTL8367S_LOG_ERR("mirroring_port same with mirrored_port\r\n");
			return count;
		}

		if(ming_port < PORT_IDX0 || ming_port >= PORT_MAX_NUM)
		{
			RTL8367S_LOG_ERR("mirroring_port out of range\r\n");
			return count;
		}

		memset(&txmask, 0x0, sizeof(txmask));
		memset(&rxmask, 0x0, sizeof(rxmask));
		if(mred_port < PORT_IDX0 || mred_port >= PORT_MAX_NUM)
		{
			RTL8367S_LOG_INFO("mirror cpu port -> utp port%d\r\n", ming_port);
			
			txmask.bits[0] |= (1<<rtl8367s_cpu_port);
			rxmask.bits[0] |= (1<<rtl8367s_cpu_port);
		}
		else
		{
			RTL8367S_LOG_INFO("mirror utp port%d -> utp port%d\r\n", mred_port, ming_port);
			txmask.bits[0] |= (1<<rtl8367s_utp_port[mred_port]);
			rxmask.bits[0] |= (1<<rtl8367s_utp_port[mred_port]);
		}

		if(RT_ERR_OK != rtk_mirror_portBased_set(rtl8367s_utp_port[ming_port], &txmask, &rxmask))
		{
			RTL8367S_LOG_ERR("rtk_mirror_portBased_set fail\r\n");
		}

		if(RT_ERR_OK != rtk_mirror_portIso_set(isolation?ENABLED:DISABLED))
		{
			RTL8367S_LOG_ERR("rtk_mirror_portIso_set fail\r\n");
		}

		if(RT_ERR_OK != rtk_mirror_isolationLeaky_set(ENABLED, ENABLED))
		{
			RTL8367S_LOG_ERR("rtk_mirror_isolationLeaky_set fail\r\n");
		}

	}
	else
	{
		memset(&txmask, 0x0, sizeof(txmask));
		memset(&rxmask, 0x0, sizeof(rxmask));
		if(RT_ERR_OK != rtk_mirror_portBased_set(rtl8367s_utp_port[0], &txmask, &rxmask))
		{
			RTL8367S_LOG_ERR("rtk_mirror_portBased_set fail\r\n");
		}

		if(RT_ERR_OK != rtk_mirror_portIso_set(DISABLED))
		{
			RTL8367S_LOG_ERR("rtk_mirror_portIso_set fail\r\n");
		}

		if(RT_ERR_OK != rtk_mirror_isolationLeaky_set(DISABLED, DISABLED))
		{
			RTL8367S_LOG_ERR("rtk_mirror_isolationLeaky_set fail\r\n");
		}
	}
	
	rtl8367s_port_mirror = val;

	return count;
}


static struct file_operations rtl8367s_port_mirror_fops = {
	.owner 		= THIS_MODULE,
	.open		= rtl8367s_port_mirror_open,
	.read	 	= seq_read,
	.write		= rtl8367s_port_mirror_write,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};

static int rtl8367s_mib_cnt_get(struct seq_file *s, void *unused)
{
	const char* port_strs[] = {
		"UTP0",
		"UTP1",
		"UTP2",
		"UTP3",
		"UTP4",
		"EXT0",
		"EXT1",
	};

	const rtk_port_t ports[] = {
		UTP_PORT0,
		UTP_PORT1,
		UTP_PORT2,
		UTP_PORT3,
		UTP_PORT4,
		EXT_PORT0,
		EXT_PORT1,
	};
	
	const char* ptype_strs[] = {
		"InUcast     ",
		"InMulticast ",
		"InBroadcast ",

		"OutUcast    ",
		"OutMulticast",
		"OutBroadcast",

		"InMLDSQuery ",
		"InMLDGQuery ",
		"InMLDLeaves ",
		"InMLDJoinOK ",
		"InMLDJoinFai",
		"InMLDChkErr",

		"OutMLDReport",
		"OutMLDLeaves",
		"OutMLDGQuery",
		"OutMLDSQuery",
		
		"InIGMPSQuery",
		"InIGMPGQuery",
		"InIGMPLeaves",
		"InIGMPJoinOK",
		"InIGMPJoinFai",
		"InIGMPChkErr",

		"OutIGMPReport",
		"OutIGMPLeaves",
		"OutIGMPGQuery",
		"OutIGMPSQuery",

		"InKnownMcast",

		"FCSErrors",
		"SymbolErrors",
		"SingleCollision",
		"MultiCollision",
		"DeferredTrans",
		"LateCollision",
		"ExcesCollisions",

		"InPauseFrames",
		"OutPauseFrames",
	};
	
	const rtk_stat_port_type_t ptypes[] = {
		STAT_IfInUcastPkts,
		STAT_IfInMulticastPkts,
		STAT_IfInBroadcastPkts,
		
		STAT_IfOutUcastPkts,
		STAT_IfOutMulticastPkts,
		STAT_IfOutBroadcastPkts,

		STAT_InMldSpecificQuery,
		STAT_InMldGeneralQuery,
		STAT_InMldLeaves,
		STAT_InMldJoinsSuccess,
		STAT_InMldJoinsFail,
		STAT_InMldChecksumError,
		
		STAT_OutMldReports,
		STAT_OutMldLeaves,
		STAT_OutMldGeneralQuery,
		STAT_OutMldSpecificQuery,
		
		STAT_InIgmpSpecificQuery,
		STAT_InIgmpGeneralQuery,
		STAT_InIgmpInterfaceLeaves,
		STAT_InIgmpJoinsSuccess,
		STAT_InIgmpJoinsFail,
		STAT_InIgmpChecksumError,
		
		STAT_OutIgmpReports,
		STAT_OutIgmpLeaves,
		STAT_OutIgmpGeneralQuery,
		STAT_OutIgmpSpecificQuery,
		
		STAT_InKnownMulticastPkts,
		
		STAT_Dot3StatsFCSErrors,
		STAT_Dot3StatsSymbolErrors,
		STAT_Dot3StatsSingleCollisionFrames,
		STAT_Dot3StatsMultipleCollisionFrames,
		STAT_Dot3StatsDeferredTransmissions,
		STAT_Dot3StatsLateCollisions,
		STAT_Dot3StatsExcessiveCollisions,
		
		STAT_Dot3InPauseFrames,
		STAT_Dot3OutPauseFrames,
	};

	static rtk_stat_counter_t counters[sizeof(ports)/sizeof(ports[0])]
		[sizeof(ptypes)/sizeof(ptypes[0])] = { 0 };
	
	int p = 0;
	int t = 0;
	
	rtk_stat_counter_t cnt;

	seq_printf(s, "MIB Counter\t\t");
	
	for(p=0; p<(sizeof(ports)/sizeof(ports[0])); p++)
	{
		seq_printf(s, "%s\t\t", port_strs[p]);
	}
	seq_printf(s, "\r\n");

	for(t=0; t<(sizeof(ptypes)/sizeof(ptypes[0])); t++)
	{
		seq_printf(s, "%s\t\t", ptype_strs[t]);

		for(p=0; p<(sizeof(ports)/sizeof(ports[0])); p++)
		{
			if(RT_ERR_OK != rtk_stat_port_get(ports[p], ptypes[t], &cnt))
			{
				RTL8367S_LOG_ERR("rtk_stat_port_get fail %s %s\r\n", port_strs[p], ptype_strs[t]);
				goto next;
			}

			seq_printf(s, "%lld\t\t", (rtk_int64)(cnt-counters[p][t]));
			counters[p][t] = cnt;
		}

		seq_printf(s, "\r\n");
	}

next:

#if 0
	/* reset counter */
	for(p=0; p<(sizeof(ports)/sizeof(ports[0])); p++)
	{
		if(RT_ERR_OK != rtk_stat_port_reset(ports[p]))
		{
			RTL8367S_LOG_ERR("rtk_stat_port_reset fail %s\r\n", port_strs[p]);
		}
	}
#endif

    return 0;
}

static int rtl8367s_mib_cnt_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_mib_cnt_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8367s_mib_cnt_write(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 val;

	rtk_port_t p;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}

	/* reset counter */
	RTK_SCAN_ALL_LOG_PORT(p)
	{
		if(RT_ERR_OK != rtk_stat_port_reset(p))
		{
			RTL8367S_LOG_ERR("rtk_stat_port_reset fail\r\n");
			return 0;
		}
	}
	RTL8367S_LOG_ERR("rtk_stat_port_reset success\r\n");
	return count;
}


static struct file_operations rtl8367s_mib_cnt_fops = {
	.owner 		= THIS_MODULE,
	.open		= rtl8367s_mib_cnt_open,
	.read	 	= seq_read,
	.write		= rtl8367s_mib_cnt_write,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};

static int rtl8367s_mib_cnt_partila_get(struct seq_file *s, void *unused)
{
	const char* port_strs[] = {
		"UTP0",
		"UTP1",
		"UTP2",
		"UTP3",
		"UTP4",
		"EXT0",
		"EXT1",
	};

	const rtk_port_t ports[] = {
		UTP_PORT0,
		UTP_PORT1,
		UTP_PORT2,
		UTP_PORT3,
		UTP_PORT4,
		EXT_PORT0,
		EXT_PORT1,
	};
	
	const char* ptype_strs[] = {
		"InUcast     ",
		"InMulticast ",
		"InBroadcast ",

		"OutUcast    ",
		"OutMulticast",
		"OutBroadcast",
	#if 0
		"InMLDSQuery ",
		"InMLDGQuery ",
		"InMLDLeaves ",
		"InMLDJoinOK ",
		"InMLDJoinFai",
		"InMLDChkErr",

		"OutMLDReport",
		"OutMLDLeaves",
		"OutMLDGQuery",
		"OutMLDSQuery",
		
		"InIGMPSQuery",
		"InIGMPGQuery",
		"InIGMPLeaves",
		"InIGMPJoinOK",
		"InIGMPJoinFai",
		"InIGMPChkErr",

		"OutIGMPReport",
		"OutIGMPLeaves",
		"OutIGMPGQuery",
		"OutIGMPSQuery",

		"InKnownMcast",

		"FCSErrors",
		"SymbolErrors",
		"SingleCollision",
		"MultiCollision",
		"DeferredTrans",
		"LateCollision",
		"ExcesCollisions",

		"InPauseFrames",
		"OutPauseFrames",
	#endif
		"InOctets",
		"OutOctets",
	};
	
	const rtk_stat_port_type_t ptypes[] = {
		STAT_IfInUcastPkts,
		STAT_IfInMulticastPkts,
		STAT_IfInBroadcastPkts,
		
		STAT_IfOutUcastPkts,
		STAT_IfOutMulticastPkts,
		STAT_IfOutBroadcastPkts,
	#if 0
		STAT_InMldSpecificQuery,
		STAT_InMldGeneralQuery,
		STAT_InMldLeaves,
		STAT_InMldJoinsSuccess,
		STAT_InMldJoinsFail,
		STAT_InMldChecksumError,
		
		STAT_OutMldReports,
		STAT_OutMldLeaves,
		STAT_OutMldGeneralQuery,
		STAT_OutMldSpecificQuery,
		
		STAT_InIgmpSpecificQuery,
		STAT_InIgmpGeneralQuery,
		STAT_InIgmpInterfaceLeaves,
		STAT_InIgmpJoinsSuccess,
		STAT_InIgmpJoinsFail,
		STAT_InIgmpChecksumError,
		
		STAT_OutIgmpReports,
		STAT_OutIgmpLeaves,
		STAT_OutIgmpGeneralQuery,
		STAT_OutIgmpSpecificQuery,
		
		STAT_InKnownMulticastPkts,
		
		STAT_Dot3StatsFCSErrors,
		STAT_Dot3StatsSymbolErrors,
		STAT_Dot3StatsSingleCollisionFrames,
		STAT_Dot3StatsMultipleCollisionFrames,
		STAT_Dot3StatsDeferredTransmissions,
		STAT_Dot3StatsLateCollisions,
		STAT_Dot3StatsExcessiveCollisions,
		
		STAT_Dot3InPauseFrames,
		STAT_Dot3OutPauseFrames,
	#endif
		STAT_IfInOctets,
		STAT_IfOutOctets,
	};

	static rtk_stat_counter_t counters[sizeof(ports)/sizeof(ports[0])]
		[sizeof(ptypes)/sizeof(ptypes[0])] = { 0 };
	
	int p = 0;
	int t = 0;
	
	rtk_stat_counter_t cnt;

	seq_printf(s, "MIB Counter\t\t");
	
	for(p=0; p<(sizeof(ports)/sizeof(ports[0])); p++)
	{
		seq_printf(s, "%s\t\t", port_strs[p]);
	}
	seq_printf(s, "\r\n");

	for(t=0; t<(sizeof(ptypes)/sizeof(ptypes[0])); t++)
	{
		seq_printf(s, "%s\t\t", ptype_strs[t]);

		for(p=0; p<(sizeof(ports)/sizeof(ports[0])); p++)
		{
			if(RT_ERR_OK != rtk_stat_port_get(ports[p], ptypes[t], &cnt))
			{
				RTL8367S_LOG_ERR("rtk_stat_port_get fail %s %s\r\n", port_strs[p], ptype_strs[t]);
				goto next;
			}

			seq_printf(s, "%lld\t\t", (rtk_int64)(cnt-counters[p][t]));
			counters[p][t] = cnt;
		}

		seq_printf(s, "\r\n");
	}

next:

#if 0
	/* reset counter */
	for(p=0; p<(sizeof(ports)/sizeof(ports[0])); p++)
	{
		if(RT_ERR_OK != rtk_stat_port_reset(ports[p]))
		{
			RTL8367S_LOG_ERR("rtk_stat_port_reset fail %s\r\n", port_strs[p]);
		}
	}
#endif

    return 0;
}

static int rtl8367s_mib_cnt_partial_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_mib_cnt_partila_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8367s_mib_cnt_partial_write(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 val;

	rtk_port_t p;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}

	/* reset counter */
	RTK_SCAN_ALL_LOG_PORT(p)
	{
		if(RT_ERR_OK != rtk_stat_port_reset(p))
		{
			RTL8367S_LOG_ERR("rtk_stat_port_reset fail\r\n");
			return 0;
		}
	}
	
	return count;
}

static struct file_operations rtl8367s_mib_cnt_partial_fops = {
	.owner 		= THIS_MODULE,
	.open		= rtl8367s_mib_cnt_partial_open,
	.read	 	= seq_read,
	.write		= rtl8367s_mib_cnt_partial_write,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};

static int rtl8367s_hw_bridge_get(struct seq_file *s, void *unused)
{
	rtk_portmask_t permitPort;
	rtk_svlan_memberCfg_t svlanCfg;
	rtk_uint32 vid;
	rtk_uint32 fid;

	int p;
	int ret = 0;

	seq_printf(s, "hw_bridge=%d\n", rtl8367s_hw_bridge);
	seq_printf(s, "hw_bridge_port=");
	for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
	{
		seq_printf(s, "%d ", rtl8367s_hw_bridge_port[p]);
	}
	seq_printf(s, "\n");
#ifdef RTL8367S_HW_BRIDGE_PORT_VLAN_DETECT
	seq_printf(s, "hw_bridge_vlan_port=");
	for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
	{
		seq_printf(s, "%d ", rtl8367s_hw_bridge_vlan_port[p]);
	}
	seq_printf(s, "\n");
#endif /**/
	seq_printf(s, "============================\n");

	/* can not show efid in 8367d */
	if(CHIP_RTL8367D != g_switch_chip)
	{
		seq_printf(s, "UTP\tpermit\t\tpfid\tvfid\n");
	}
	else
	{
		seq_printf(s,"UTP\tpermit\t\tfid\n");
	}

	for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
	{
		seq_printf(s, "UTP%d\t", p);
		
		/* get iso of the port */
		memset(&permitPort, 0x0, sizeof(permitPort));
		if((ret = rtk_port_isolation_get(rtl8367s_utp_port[p], &permitPort)) != RT_ERR_OK) 
		{
			RTL8367S_LOG_ERR("rtk_port_isolation_set fail...ret=%d\n", ret);
		}
		seq_printf(s, "0x%08x\t", permitPort.bits[0]);

		/* get fid of the port */
		/* efid is not support in 8367d */
		if(CHIP_RTL8367D != g_switch_chip)
		{
			if((ret = rtk_port_efid_get(rtl8367s_utp_port[p], &fid)) != RT_ERR_OK)
			{
				RTL8367S_LOG_ERR("rtk_port_efid_get fail...ret=%d\n", ret);
				return ret;
			}
			seq_printf(s, "%0d\t", fid);
		}
		/* get vlan of the port */
		vid = (p + RTL8367S_VID_RESERVED_NUM);
		memset(&svlanCfg, 0x0, sizeof(svlanCfg));
		if((ret = rtk_svlan_memberPortEntry_get(vid, &svlanCfg)) != RT_ERR_OK) 
		{
			RTL8367S_LOG_ERR("rtk_svlan_memberPortEntry_get fail...ret=%d\n", ret);
		}
		
		/* show efid in 8367c */
		if(CHIP_RTL8367D != g_switch_chip)
		{
			seq_printf(s, "%0d\n", svlanCfg.efid);
		}
		else
		{
			seq_printf(s, "%0d\n", svlanCfg.fid);
		}
		
	}

	seq_printf(s,"============================\n");
	
	for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
	{
		seq_printf(s, "port=%d dev=%s group=%d iso=%d\n", 
			p, rtl8367s_dev_name[p], rtl8367s_br_group[p], rtl8367s_iso_mode[p]);
	}
	
	return 0;
}

static int rtl8367s_hw_bridge_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_hw_bridge_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8367s_hw_bridge_write(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 val, pidx;
	int argc=0;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	
	argc= sscanf(val_string, "%d %d", &val, &pidx);
	if (argc != 1 && argc !=2)
	{
		printk("usage: <action> enable [ port ]\n");
		return count;
	}

	RTL8367S_LOCK();

	if(argc == 1)
	{
		if(rtl8367s_hw_bridge == val)
		{
			RTL8367S_LOG_ERR("same hw_bridge %d\n", val);
			goto error;
		}
		
		rtl8367s_hw_bridge = val;
	}
	else if(argc == 2)
	{
		if(pidx <0 || pidx >= PORT_MAX_NUM)
		{
			RTL8367S_LOG_ERR("pidx out of range %d\n", pidx);
			goto error;
		}

		if(rtl8367s_hw_bridge_port[pidx] == val)
		{
			RTL8367S_LOG_ERR("same hw_bridge_port %d\n", val);
			goto error;
		}

		rtl8367s_hw_bridge_port[pidx] = val;
	}
	else
	{
		RTL8367S_LOG_ERR("wrong argc %d\n", argc);
		goto error;
	}
	
	_rtl8367s_lookup_refresh();

	rtk_l2_table_clear();

	if(rtl8367s_cpu_mac[0] !=0 ||
		rtl8367s_cpu_mac[1] !=0 ||
		rtl8367s_cpu_mac[2] !=0 ||
		rtl8367s_cpu_mac[3] !=0 ||
		rtl8367s_cpu_mac[4] !=0 ||
		rtl8367s_cpu_mac[5] !=0)
	{
		_rtl8367s_cpumac_refresh();
	}

error:	
	RTL8367S_UNLOCK();

	return count;
}


static struct file_operations rtl8367s_hw_bridge_fops = {
	.owner		= THIS_MODULE,
	.open		= rtl8367s_hw_bridge_open,
	.read		= seq_read,
	.write		= rtl8367s_hw_bridge_write,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};


static int rtl8367s_eth_forward_get(struct seq_file *s, void *unused)
{
	seq_printf(s, "eth_forward=%d\n", rtl8367s_eth_forward);

	return 0;
}

static int rtl8367s_eth_forward_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_eth_forward_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8367s_eth_forward_write_proc(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 val;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}

	RTL8367S_LOCK();

	if(rtl8367s_eth_forward == val)
	{
		RTL8367S_LOG_ERR("same eth_forward %d\n", val);
		goto error;
	}

	rtl8367s_eth_forward = val;

	_rtl8367s_lookup_refresh();

	rtk_l2_table_clear();

error:	
	RTL8367S_UNLOCK();
	
	return count;
}

static struct file_operations rtl8367s_eth_forward_fops = {
	.owner 		= THIS_MODULE,
	.open		= rtl8367s_eth_forward_open,
	.read	 	= seq_read,
	.write		= rtl8367s_eth_forward_write_proc,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};

static int rtl8367s_cpu_mac_get(struct seq_file *s, void *unused)
{
	seq_printf(s, "cpu_mac=%02x:%02x:%02x:%02x:%02x:%02x\n", 
		rtl8367s_cpu_mac[0], rtl8367s_cpu_mac[1], rtl8367s_cpu_mac[2],
		rtl8367s_cpu_mac[3], rtl8367s_cpu_mac[4], rtl8367s_cpu_mac[5]);

	return 0;
}

static int rtl8367s_cpu_mac_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_cpu_mac_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8367s_cpu_mac_write_proc(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	unsigned char mac[6];

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	if (sscanf(val_string, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
		&mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5]) != 6)
	{
		printk("usage: <action>\n");
		return count;
	}

	RTL8367S_LOCK();
	
	if(memcmp(rtl8367s_cpu_mac, mac, sizeof(rtl8367s_cpu_mac)) == 0)
	{
		RTL8367S_LOG_ERR("same cpu_mac %02x%02x%02x%02x%02x%02x\n", 
			mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		goto error;
	}
	
	memcpy(rtl8367s_cpu_mac, mac, sizeof(rtl8367s_cpu_mac));
	
	rtk_l2_table_clear();
	
	if(rtl8367s_cpu_mac[0] !=0 ||
		rtl8367s_cpu_mac[1] !=0 ||
		rtl8367s_cpu_mac[2] !=0 ||
		rtl8367s_cpu_mac[3] !=0 ||
		rtl8367s_cpu_mac[4] !=0 ||
		rtl8367s_cpu_mac[5] !=0)
	{
		_rtl8367s_cpumac_refresh();
	}

error:	
	RTL8367S_UNLOCK();
	
	return count;
}

static struct file_operations rtl8367s_cpu_mac_fops = {
	.owner 		= THIS_MODULE,
	.open		= rtl8367s_cpu_mac_open,
	.read	 	= seq_read,
	.write		= rtl8367s_cpu_mac_write_proc,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};

#ifdef RTL8367S_HW_LED_FUNCTION

static int rtl8367s_hw_led_get(struct seq_file *s, void *unused)
{
	seq_printf(s,"hw_led=%d\n", rtl8367s_hw_led);
	
	return 0;
}

static int rtl8367s_hw_led_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_hw_led_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8367s_hw_led_write(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 val;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}

	RTL8367S_LOCK();

	if(rtl8367s_hw_led == val)
	{
		RTL8367S_LOG_ERR("same led config %d\n", val);
		goto error;
	}

	rtl8367s_hw_led = val;

	_rtl8367s_hw_led_refresh();

error:	
	RTL8367S_UNLOCK();

	return count;
}

static struct file_operations rtl8367s_hw_led_fops = {
	.owner 		= THIS_MODULE,
	.open		= rtl8367s_hw_led_open,
	.read	 	= seq_read,
	.write		= rtl8367s_hw_led_write,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};

#endif /* RTL8367S_HW_LED_FUNCTION */


static int rtl8367s_debug_info_get(struct seq_file *s, void *unused)
{
	seq_printf(s,"debug_info=%d\n", rtl8367s_debug_info);
	
	return 0;
}

static int rtl8367s_debug_info_open(struct inode *inode, struct file *file)  
{  
    return single_open(file, rtl8367s_debug_info_get, file->f_path.dentry->d_iname);
}


static ssize_t rtl8367s_debug_info_write(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 val;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}
	
	rtl8367s_debug_info = val;
	
	return count;
}

static struct file_operations rtl8367s_debug_info_fops = {
	.owner		= THIS_MODULE,
	.open		= rtl8367s_debug_info_open,
	.read		= seq_read,
	.write		= rtl8367s_debug_info_write,
	.llseek 	= seq_lseek,  
	.release	= single_release,
};


static ssize_t rtl8367s_vlan_write(struct file *filp, const char __user *ubuf,	
	size_t usize, loff_t * off)
{
		int ret = -1;
		//rtk_enable_t enable = RTK_ENABLE_END;
		rtk_uint32 port = 0;
		rtk_vlan_acceptFrameType_t type = ACCEPT_FRAME_TYPE_ALL;
		rtk_vlan_t vid = 0;
		rtk_vlan_cfg_t vlanCfg;
		rtk_uint32 member = 0;
		rtk_uint32 untag = 0;
		rtk_vlan_cfg_t vlan;
		char buf[16] = {0};
		char *pBuf = NULL;
		char *cmd = NULL;
		int cmdLen = 0;
		char *substr = NULL;


		char val_string[128];

		if (usize > sizeof(val_string) - 1)
			return -EINVAL;
		if (copy_from_user(val_string, ubuf, usize))
			return -EFAULT;

		printk("cmd is %s\n", val_string);

		switch (val_string[0]) {
		case 'i':
			cmd = "init";
			cmdLen = strlen(cmd);
			if (strncmp(val_string, cmd, cmdLen) != 0) {
				goto usage;
			}
			
			if ((ret = rtk_vlan_init()) != RT_ERR_OK) {
				printk("init vlan failed...ret=%d\n", ret);
				goto Error;
			}
			
			for (port = 0; port < RTK_PORT_MAX; port++) {
				if ((ret = rtk_vlan_portAcceptFrameType_set(port, type)) != RT_ERR_OK) {
					printk("set portAcceptFrameType failed...ret=%d\n", ret);
					goto Error;
				}
			}
			
			break;
		case 'r':
			cmd = "reset";
			cmdLen = strlen(cmd);
			if (strncmp(val_string, cmd, cmdLen) != 0) {
				goto usage;
			}
			
			if ((ret = rtk_vlan_reset()) != RT_ERR_OK) {
				printk("reset vlan failed...ret=%d\n", ret);
				goto Error;
			}
			
			break;
		case 'g':
			cmd = "get";
			cmdLen = strlen(cmd);
			if (strncmp(val_string, cmd, cmdLen) != 0) {
				goto usage;
			}
			
			cmdLen += 1;
			if (copy_from_user(buf, ubuf + cmdLen, usize - cmdLen)) {
				printk("copy_from_user failed...\n");
				goto Error;
			}
	
			pBuf = buf;
			substr = strsep(&pBuf, " ");
			if (!substr) {
				goto usage;
			}

			if(kstrtouint(substr, 0, &vid))
			{
				printk("error!!!!\n");
				goto Error;
			}
			
			if ((ret = rtk_vlan_get(vid, &vlanCfg)) != RT_ERR_OK) {
				printk("get vlan %d failed...ret=%d\n", vid, ret);
				goto Error;
			}
	
			printk("mbr=%x\n", vlanCfg.mbr.bits[0]);
			printk("untag=%x\n", vlanCfg.untag.bits[0]);
			printk("fid_msti=%hu\n", vlanCfg.fid_msti);
			printk("envlanpol=%hu\n", vlanCfg.envlanpol);
			printk("ivl_en=%hu\n", vlanCfg.ivl_en);
			printk("meteridx=%hu\n", vlanCfg.meteridx);
			printk("vbpen=%hu\n", vlanCfg.vbpen);
			printk("vbpri=%hu\n", vlanCfg.vbpri);
			
			break;
		case 's':
			cmd = "set";
			cmdLen = strlen(cmd);
			if (strncmp(val_string, cmd, cmdLen) != 0) {
				goto usage;
			}
	
			cmdLen += 1;
			if (copy_from_user(buf, ubuf + cmdLen, usize - cmdLen)) {
				printk("copy_from_user failed...\n");
				goto Error;
			}
	
			pBuf = buf;
			substr = strsep(&pBuf, " ");
			if (!substr) {
				goto usage;
			}
			if (kstrtouint(substr, 0, &vid))
				goto Error;

			substr = strsep(&pBuf, " ");
			if (!substr || !pBuf) {
				goto usage;
			}

			if (kstrtouint(substr, 0, &member))
				goto Error;

			if (kstrtouint(pBuf, 0, &untag))
				goto Error;	
	
			memset(&vlan, 0x0, sizeof(rtk_vlan_cfg_t));
			vlan.ivl_en = ENABLED;
			for (port = UTP_PORT0; port < RTK_PORT_MAX; port++) {
				if (member & (0x1 << port)) {
					RTK_PORTMASK_PORT_SET(vlan.mbr, port);
				}
			}
			
			for (port = UTP_PORT0; port < RTK_PORT_MAX; port++) {
				if (untag & (0x1 << port)) {
					RTK_PORTMASK_PORT_SET(vlan.untag, port);
				}
			}
			
			if ((ret = rtk_vlan_set(vid, &vlan)) != RT_ERR_OK) {
				printk("set vlan %d failed...ret=%d\n", vid, ret);
				goto Error;
			}
			
			break;
		default:
	usage:
			printk("Unknown cmd, vlan support list:\n");
			printk("init\nreset\nget <vid>\nset <vid> <membermap> <untagmap>\n");
			break;
		}
	
	Error:
		return usize;
}



static struct file_operations rtl8367s_vlan_fops = {
	.owner		= THIS_MODULE,
	.open		= NULL,
	.read		= NULL,
	.write		= rtl8367s_vlan_write,
	.llseek 	= NULL,  
	.release	= NULL,
};


static ssize_t rtl8367s_port_write(struct file *filp, const char __user *ubuf,
				size_t usize, loff_t *off)
{
	int ret = -1;
	rtk_enable_t enable = RTK_ENABLE_END;
	rtk_uint32 portid = 0;
	rtk_vlan_acceptFrameType_t type = ACCEPT_FRAME_TYPE_END;
	rtk_vlan_t pvid = 0;
	rtk_pri_t priority = 0;
	char buf[16] = {0};
	char *pBuf = NULL;
	char *cmd = NULL;
	int cmdLen = 0;
	int preCmdLen = 0;
	char *substr = NULL;

	char val_string[128];
	
	if (usize > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, ubuf, usize))
		return -EFAULT;

	printk("cmd is %s\n", val_string);

	switch (val_string[1]) {
	case 'v':
		cmd = "pvid";
		cmdLen = strlen(cmd);
		if (strncmp(val_string, cmd, cmdLen) != 0) {
			goto usage;
		}
		preCmdLen = cmdLen;

		preCmdLen += 1;
		cmd = "get";
		cmdLen = strlen(cmd);
		if (strncmp(val_string + preCmdLen, cmd, cmdLen) == 0) {
			preCmdLen += cmdLen;
			preCmdLen += 1;

			if (copy_from_user(buf, ubuf + preCmdLen, usize - preCmdLen)) {
				printk("copy_from_user failed...\n");
				goto Error;
			}

			pBuf = buf;
			substr = strsep(&pBuf, " ");
			if (!substr) {
				goto usage;
			}

			if (kstrtouint(substr, 0, &portid))
				goto Error;

			if ((ret = rtk_vlan_portPvid_get(portid, &pvid, &priority)) != RT_ERR_OK) {
				printk("get portPvid failed...ret=%d\n", ret);
				goto Error;
			}

			printk("pvid=%u priority=%d\n", pvid, priority);
		}
		else {
			cmd = "set";
			cmdLen = strlen(cmd);
			if (strncmp(val_string + preCmdLen, cmd, cmdLen) == 0) {
				preCmdLen += cmdLen;
				preCmdLen += 1;

				if (copy_from_user(buf, ubuf + preCmdLen, usize - preCmdLen)) {
					printk("copy_from_user failed...\n");
					goto Error;
				}

				pBuf = buf;
				substr = strsep(&pBuf, " ");
				if (!substr) {
					goto usage;
				}

				if (kstrtouint(substr, 0, &portid))
					goto Error;
				
				substr = strsep(&pBuf, " ");
				if (!substr || !pBuf) {
					goto usage;
				}

				if (kstrtouint(substr, 0, &pvid))
					goto Error;

				if (kstrtouint(pBuf, 0, &priority))
					goto Error;

				if ((ret = rtk_vlan_portPvid_set(portid, pvid, priority)) != RT_ERR_OK) {
					printk("set portPvid failed...ret=%d\n", ret);
					goto Error;
				}
			}
			else {
				goto usage;
			}
		}
		
		break;
	case 't':
		cmd = "ptype";
		cmdLen = strlen(cmd);
		if (strncmp(val_string, cmd, cmdLen) != 0) {
			goto usage;
		}
		preCmdLen = cmdLen;

		preCmdLen += 1;
		cmd = "get";
		cmdLen = strlen(cmd);
		if (strncmp(val_string + preCmdLen, cmd, cmdLen) == 0) {
			preCmdLen += cmdLen;
			preCmdLen += 1;

			if (copy_from_user(buf, ubuf + preCmdLen, usize - preCmdLen)) {
				printk("copy_from_user failed...\n");
				goto Error;
			}

			pBuf = buf;
			substr = strsep(&pBuf, " ");
			if (!substr) {
				goto usage;
			}

			if (kstrtouint(substr, 0, &portid))
				goto Error;

			if ((ret = rtk_vlan_portAcceptFrameType_get(portid, &type)) != RT_ERR_OK) {
				printk("get portAcceptFrameType failed...ret=%d\n", ret);
				goto Error;
			}

			printk("type=%d\n", type);
		}
		else {
			cmd = "set";
			cmdLen = strlen(cmd);
			if (strncmp(val_string + preCmdLen, cmd, cmdLen) == 0) {
				preCmdLen += cmdLen;
				preCmdLen += 1;

				if (copy_from_user(buf, ubuf + preCmdLen, usize - preCmdLen)) {
					printk("copy_from_user failed...\n");
					goto Error;
				}

				pBuf = buf;
				substr = strsep(&pBuf, " ");
				if (!substr || !pBuf) {
					goto usage;
				}

				if (kstrtouint(substr, 0, &portid))
					goto Error;

				if (kstrtouint(pBuf, 0, &type))
					goto Error;

				if ((ret = rtk_vlan_portAcceptFrameType_set(portid, type)) != RT_ERR_OK) {
					printk("set portAcceptFrameType failed...ret=%d\n", ret);
					goto Error;
				}
			}
			else {
				goto usage;
			}
		}
		
		break;
	case 'e':
		cmd = "eee";
		cmdLen = strlen(cmd);
		if (strncmp(val_string, cmd, cmdLen) != 0) {
			goto usage;
		}
		preCmdLen = cmdLen;

		preCmdLen += 1;
		cmd = "get";
		cmdLen = strlen(cmd);
		if (strncmp(val_string + preCmdLen, cmd, cmdLen) == 0) {
			preCmdLen += cmdLen;
			preCmdLen += 1;

			if (copy_from_user(buf, ubuf + preCmdLen, usize - preCmdLen)) {
				printk("copy_from_user failed...\n");
				goto Error;
			}

			pBuf = buf;
			substr = strsep(&pBuf, " ");
			if (!substr) {
				goto usage;
			}

			if (kstrtouint(substr, 0, &portid))
				goto Error;

			if ((ret = rtk_eee_portEnable_get(portid, &enable)) != RT_ERR_OK) {
				printk("get eee portEnable failed...ret=%d\n", ret);
				goto Error;
			}

			printk("enable=%d\n", enable);
		}
		else {
			cmd = "set";
			cmdLen = strlen(cmd);
			if (strncmp(val_string + preCmdLen, cmd, cmdLen) == 0) {
				preCmdLen += cmdLen;
				preCmdLen += 1;

				if (copy_from_user(buf, ubuf + preCmdLen, usize - preCmdLen)) {
					printk("copy_from_user failed...\n");
					goto Error;
				}

				pBuf = buf;
				substr = strsep(&pBuf, " ");
				if (!substr || !pBuf) {
					goto usage;
				}

				if (kstrtouint(substr, 0, &portid))
					goto Error;

				if (kstrtouint(pBuf, 0, &enable))
					goto Error;

				if ((ret = rtk_eee_portEnable_set(portid, enable)) != RT_ERR_OK) {
					printk("set eee portEnable failed...ret=%d\n", ret);
					goto Error;
				}
			}
			else {
				goto usage;
			}
		}
		break;
	case 'r':
		cmd = "green";
		cmdLen = strlen(cmd);
		if (strncmp(val_string, cmd, cmdLen) != 0) {
			goto usage;
		}
		preCmdLen = cmdLen;

		preCmdLen += 1;
		cmd = "get";
		cmdLen = strlen(cmd);
		if (strncmp(val_string + preCmdLen, cmd, cmdLen) == 0) {
			if ((ret = rtk_switch_greenEthernet_get(&enable)) != RT_ERR_OK) {
				if (ret == RT_ERR_DRIVER_NOT_FOUND) {
					printk("get switch greenEthernet failed...ret=%d(The driver can not found)," 
					"Green Ethernet support is removed in rtl8367d DAL since switch driver V1.4.1\n", ret);
				}
				else{
					printk("get switch greenEthernet failed...ret=%d\n", ret);
				}
				goto Error;
			}
			printk("greenEthernet.enable=%d\n", enable);
		}
		else {
			cmd = "set";
			cmdLen = strlen(cmd);
			if (strncmp(val_string + preCmdLen, cmd, cmdLen) == 0) {
				preCmdLen += cmdLen;
				preCmdLen += 1;

				if (copy_from_user(buf, ubuf + preCmdLen, usize - preCmdLen)) {
					printk("copy_from_user failed...\n");
					goto Error;
				}

				pBuf = buf;
				if (!pBuf) {
					goto usage;
				}

				if (kstrtouint(pBuf, 0, &enable))
					goto Error;

				if ((ret = rtk_switch_greenEthernet_set(enable)) != RT_ERR_OK) {
					if (ret == RT_ERR_DRIVER_NOT_FOUND) {
						printk("set switch greenEthernet failed...ret=%d(The driver can not found)," 
						"Green Ethernet support is removed in rtl8367d DAL since switch driver V1.4.1\n", ret);
					}
					else{
						printk("set switch greenEthernet failed...ret=%d\n", ret);
					}
					goto Error;
				}
			}
			else {
				goto usage;
			}
		}
		
		break;
	default:
usage:
		printk("Unknown cmd, port support list:\n");
		printk("pvid get <portid>\npvid set <portid> <pvid> <priority>\n");
		printk("ptype get <portid>\nptype set <portid> <type>\n");
		printk("eee get <portid>\neee set <portid> <enable>\n");
		printk("green get \ngreen set <enable>\n");
		break;
	}

Error:
	return usize;
}

static struct file_operations rtl8367s_port_fops = {
	.owner		= THIS_MODULE,
	.open		= NULL,
	.read		= NULL,
	.write		= rtl8367s_port_write,
	.llseek 	= NULL,  
	.release	= NULL,
};

/********************************************************************/
/* unused function
static int rtl8367s_hw_init(void)
{
	int ret = 0;
#if 0
	RTL8367S_LOG_INFO("reset by gpio%d...", RTL8367S_RESET_GPIO_NUM);
	gpio_direction_output(RTL8367S_RESET_GPIO_NUM, SSDK_GPIO_RESET);
	msleep(200);
	gpio_set_value(RTL8367S_RESET_GPIO_NUM, SSDK_GPIO_RELEASE);
	msleep(200);
	RTL8367S_LOG_INFO(" done\r\n");
#endif
	return ret;
}
*/

static int rtl8367s_mii_init(void)
{
	int ret = 0;

	RTL8367S_LOG_DEBUG("init\r\n");
	
	if ((ret = rtk_switch_init()) != RT_ERR_OK) {
		RTL8367S_LOG_ERR("rtk_switch_init failed...ret=%d\n", ret);
		return ret;
	}

	if(CHIP_RTL8367D != g_switch_chip)
	{
		rtl8367s_phy_Patch_set(0);
	}
	_rtl8367s_mii_type_refresh();

	return ret;
}

/* unused function
static int rtl8367s_vlan_init(void)
{
	int ret = 0;

	RTL8367S_LOG_DEBUG("init\r\n");

	if ((ret = rtk_svlan_init()) != RT_ERR_OK) {
		RTL8367S_LOG_ERR("rtk_svlan_init fail...ret=%d\n", ret);
		return ret;
	}
	
	if((ret = rtk_svlan_tpidEntry_set(ETH_P_8021Q)) != RT_ERR_OK) 
	{
		RTL8367S_LOG_ERR("rtk_svlan_tpidEntry_set fail...ret=%d\n", ret);
		return ret;
	}

	if((ret = rtk_svlan_servicePort_add(rtl8367s_cpu_port)) != RT_ERR_OK) 
	{
		RTL8367S_LOG_ERR("rtk_svlan_servicePort_add fail...ret=%d\n", ret);
		return ret;
	}

	if((ret = rtk_svlan_untag_action_set(UNTAG_DROP, 0)) != RT_ERR_OK) 
	{
		RTL8367S_LOG_ERR("rtk_svlan_untag_action_set fail...ret=%d\n", ret);
		return ret;
	}

	if(CHIP_RTL8367D != g_switch_chip)
	{
		if((ret = rtk_svlan_unmatch_action_set(UNMATCH_DROP, 0)) != RT_ERR_OK) 
		{
			RTL8367S_LOG_ERR("rtk_svlan_unmatch_action_set fail...ret=%d\n", ret);
			return ret;
		}
	}


	_rtl8367s_lookup_refresh();

	return 0;
}
*/

static int rtl8367s_led_init(void)
{
#ifdef RTL8367S_HW_LED_FUNCTION

	rtk_portmask_t portmask;
	int ret;
	int p;

	memset(&portmask, 0x0, sizeof(portmask));

	for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
	{
		portmask.bits[0] |= (1<<rtl8367s_utp_port[p]);
	}

	if((ret = rtk_led_enable_set(LED_GROUP_1, &portmask)) != RT_ERR_OK)
	{
		RTL8367S_LOG_ERR("rtk_led_enable_set failed...ret=%d\n", ret);
		return -1;
	}
	
	if((ret = rtk_led_operation_set(LED_OP_PARALLEL)) != RT_ERR_OK)
	{
		RTL8367S_LOG_ERR("rtk_led_operation_set failed...ret=%d\n", ret);
		return -1;
	}
	
	if((ret = rtk_led_blinkRate_set(LED_BLINKRATE_128MS)) != RT_ERR_OK)
	{
		RTL8367S_LOG_ERR("rtk_led_blinkRate_set failed...ret=%d\n", ret);
		return -1;
	}

	if((ret = rtk_led_groupConfig_set(LED_GROUP_1, LED_CONFIG_LINK_ACT)) != RT_ERR_OK)
	{
		RTL8367S_LOG_ERR("rtk_led_groupConfig_set failed...ret=%d\n", ret);
		return -1;
	}

	_rtl8367s_hw_led_refresh();
#endif /* RTL8367S_HW_LED_FUNCTION */

	return 0;
}

static int rtl8367s_mcast_init(void)
{
	rtk_api_ret_t ret;
	int p;

	RTL8367S_LOG_DEBUG("init\r\n");

	/* not supported in 8367d */
	if(CHIP_RTL8367D != g_switch_chip)
	{

		if((ret = rtk_igmp_init() != RT_ERR_OK))
		{
			RTL8367S_LOG_ERR("rtk_igmp_init fail\r\n");
		}

#if 0
		if((ret = rtk_l2_ipMcastAddrLookup_set(LOOKUP_IP_VID)) != RT_ERR_OK) 
		{
			RTL8367S_LOG_ERR("rtk_l2_ipMcastAddrLookup_set fail...ret=%d\n", ret);
			return ret;
		}
#endif

		for(p=PORT_IDX0; p<PORT_MAX_NUM; p++)
		{
			if((ret = rtk_igmp_protocol_set(rtl8367s_utp_port[p], 
				PROTOCOL_IGMPv1, IGMP_ACTION_ASIC) != RT_ERR_OK))
			{
				RTL8367S_LOG_ERR("rtk_igmp_protocol_set fail\r\n");
				return ret;
			}
			
			if((ret = rtk_igmp_protocol_set(rtl8367s_utp_port[p], 
				PROTOCOL_IGMPv2, IGMP_ACTION_ASIC) != RT_ERR_OK))
			{
				RTL8367S_LOG_ERR("rtk_igmp_protocol_set fail\r\n");
				return ret;
			}
		
			if((ret = rtk_igmp_protocol_set(rtl8367s_utp_port[p], 
				PROTOCOL_IGMPv3, IGMP_ACTION_ASIC) != RT_ERR_OK))
			{
				RTL8367S_LOG_ERR("rtk_igmp_protocol_set fail\r\n");
				return ret;
			}
			
			if((ret = rtk_igmp_protocol_set(rtl8367s_utp_port[p], 
				PROTOCOL_MLDv1, IGMP_ACTION_ASIC) != RT_ERR_OK))
			{
				RTL8367S_LOG_ERR("rtk_igmp_protocol_set fail\r\n");
				return ret;
			}
			
			if((ret = rtk_igmp_protocol_set(rtl8367s_utp_port[p], 
				PROTOCOL_MLDv2, IGMP_ACTION_ASIC) != RT_ERR_OK))
			{
				RTL8367S_LOG_ERR("rtk_igmp_protocol_set fail\r\n");
				return ret;
			}
		}
	}
	_rtl8367s_mcast_refresh();

	return 0;
}

int rtl8367s_event_init(void)
{
#if 0
	extern int (*tp_ext_switch_notify_if_change)(struct net_device *br, struct net_device *dev, int add);
	extern int (*tp_ext_switch_notify_iso_change)(struct net_device *br, struct net_device *dev, int iso);
	extern int (*tp_ext_switch_notify_mcr_change)(struct net_device *br, struct net_device *dev, int mcr);
#ifdef RTL8367S_HW_BRIDGE_PORT_VLAN_DETECT
	extern int (*tp_ext_switch_notify_vlan_change)(struct net_device *dev, int add);
#endif /**/
	extern int (*tp_ext_switch_br_should_forward)(struct net_device * in, struct net_device * out);
	extern int (*tp_ext_switch_ethtool_get_settings)(struct net_device * dev, struct ethtool_cmd * cmd);
	extern int (*tp_ext_switch_dev_get_stats)(struct net_device *dev, struct rtnl_link_stats64 *storage);

	RTL8367S_LOG_DEBUG("init\r\n");

	tp_ext_switch_notify_if_change = rtl8367s_notify_if_change;
	tp_ext_switch_notify_iso_change = rtl8367s_notify_iso_change;
	tp_ext_switch_notify_mcr_change = rtl8367s_notify_mcr_change;
#ifdef RTL8367S_HW_BRIDGE_PORT_VLAN_DETECT
	tp_ext_switch_notify_vlan_change = rtl8367s_notify_vlan_change;
#endif /**/
	tp_ext_switch_br_should_forward = rtl8367s_br_should_forward;
	tp_ext_switch_ethtool_get_settings = rtl8367s_ethtool_get_settings;
	tp_ext_switch_dev_get_stats = rtl8367s_dev_get_stats;
#endif	
	rtl8367s_phy_link_timer_start();

	return 0;
}


static ssize_t rtl8367s_reset_write_proc(struct file *file, const char __user *buffer,	
	size_t count, loff_t * pPos)
{
	char val_string[128];
	rtk_uint32 val;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}

	//rtl8367s_hw_init();
	
	RTL8367S_LOCK();
	
	rtl8367s_mii_init();
	//rtl8367s_vlan_init();
	if(CHIP_RTL8367D != g_switch_chip)
	{
		rtl8367s_mcast_init();
	}
	
	rtl8367s_led_init();

	rtk_port_phyEnableAll_set(ENABLED);
	
	RTL8367S_UNLOCK();
	
	return count;
}

static struct file_operations rtl8367s_reset_fops = {
	.owner 		= THIS_MODULE,
	.open		= NULL,
	.read	 	= NULL,
	.write		= rtl8367s_reset_write_proc,
	.llseek 	= NULL,  
	.release	= NULL,
};

static int rtl8367s_proc_init(void)
{
	struct proc_dir_entry * procRegDir = NULL;

	RTL8367S_LOG_DEBUG("init\r\n");

	procRegDir = proc_mkdir("rtl8367s", NULL);

	proc_create("switch_reg", 0, procRegDir, &rtl8367s_switch_reg_fops);

	proc_create("phy_reg", 0, procRegDir, &rtl8367s_phy_reg_fops);

	proc_create("phy_status", 0, procRegDir, &rtl8367s_phy_status_fops);

	proc_create("mii_type", 0, procRegDir, &rtl8367s_mii_type_fops);

	proc_create("ucast_tbl", 0, procRegDir, &rtl8367s_ucast_tbl_fops);

	proc_create("mcast_tbl", 0, procRegDir, &rtl8367s_mcast_tbl_fops);

	proc_create("mib_cnt", 0, procRegDir, &rtl8367s_mib_cnt_fops);

	proc_create("mib_cnt_partial", 0, procRegDir, &rtl8367s_mib_cnt_partial_fops);
	
	if(CHIP_RTL8367D != g_switch_chip)
	{
		proc_create("mcast_snoop", 0, procRegDir, &rtl8367s_mcast_snoop_fops);
	}

	proc_create("port_mirror", 0, procRegDir, &rtl8367s_port_mirror_fops);

	proc_create("hw_bridge", 0, procRegDir, &rtl8367s_hw_bridge_fops);

	proc_create("eth_forward", 0, procRegDir, &rtl8367s_eth_forward_fops);

	proc_create("cpu_mac", 0, procRegDir, &rtl8367s_cpu_mac_fops);

#ifdef RTL8367S_HW_LED_FUNCTION
	proc_create("hw_led", 0, procRegDir, &rtl8367s_hw_led_fops);
#endif /* RTL8367S_HW_LED_FUNCTION */

	proc_create("reset", 0, procRegDir, &rtl8367s_reset_fops);

	proc_create("debug_info", 0, procRegDir, &rtl8367s_debug_info_fops);

	proc_create("vlan", 0, procRegDir, &rtl8367s_vlan_fops);

	proc_create("port", 0, procRegDir, &rtl8367s_port_fops);

	return 0;
}


#define RTL8367C_STATUS_LINK_10M			 0
#define RTL8367C_STATUS_LINK_100M			 1
#define RTL8367C_STATUS_LINK_1000M			 2
#define RTL8367C_CTRL_POWER_DOWN	0x0800
#define CONFIG_VLAN_PER_PORT 1
#define RTL8367C_PHY0_ADDR   0x0
#define RTL8367C_PHY1_ADDR   0x1
#define RTL8367C_PHY2_ADDR   0x2
#define RTL8367C_PHY3_ADDR   0x3
#define RTL8367C_PHY4_ADDR   0x4
#define NO_WAN_PORT -1
#define RTL_LAN_PORT_VLAN          1
#define RTL_WAN_PORT_VLAN          2
#define ENET_UNIT_GE1 1
#define ENET_UNIT_GE0 0 /* Connected to the switch */
#define ENET_UNIT_LAN ENET_UNIT_GE0
#define ENET_UNIT_WAN ENET_UNIT_GE1

#define RTL8367C_IS_ENET_PORT(phyUnit) (rtlPhyInfo[phyUnit].isEnetPort)
#define RTL8367C_IS_PHY_ALIVE(phyUnit) (rtlPhyInfo[phyUnit].isPhyAlive)
#define RTL8367C_ETHUNIT(phyUnit) (rtlPhyInfo[phyUnit].ethUnit)
#define RTL8367C_PHYBASE(phyUnit) (rtlPhyInfo[phyUnit].phyBase)
#define RTL8367C_PHYADDR(phyUnit) (rtlPhyInfo[phyUnit].phyAddr)
#define RTL8367C_VLAN_TABLE_SETTING(phyUnit) (rtlPhyInfo[phyUnit].VLANTableSetting)


typedef struct {
    bool   isEnetPort;       /* normal enet port */
    bool   isPhyAlive;       /* last known state of link */
    int    ethUnit;          /* MAC associated with this phy port */
   	unsigned int phyBase;
    unsigned int phyAddr;          /* PHY registers associated with this phy port */
    unsigned int VLANTableSetting; /* Value to be written to VLAN table */
} rtlPhyInfo_t;

static rtlPhyInfo_t rtlPhyInfo[] = {

    {TRUE,   /* port 1 -- LAN port 1 */
     FALSE,
#if defined(WAN_AT_P0)
     ENET_UNIT_WAN,
#else
	 ENET_UNIT_LAN,
#endif
     0,
     RTL8367C_PHY0_ADDR,
     RTL_LAN_PORT_VLAN
    },

    {TRUE,   /* port 2 -- LAN port 2 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     RTL8367C_PHY1_ADDR,
     RTL_LAN_PORT_VLAN
    },

    {TRUE,   /* port 3 -- LAN port 3 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     RTL8367C_PHY2_ADDR,
     RTL_LAN_PORT_VLAN
    },

    {TRUE,   /* port 4 --  LAN port 4 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     RTL8367C_PHY3_ADDR,
     RTL_LAN_PORT_VLAN   /* Send to all ports */
    },

    {TRUE,  /* port 5 -- WAN Port 5 */
     FALSE,
 #if defined(WAN_AT_P4)
     ENET_UNIT_WAN,
 #else
 	 ENET_UNIT_LAN,
 #endif
     0,
     RTL8367C_PHY4_ADDR,
     RTL_LAN_PORT_VLAN    /* Send to all ports */
    },

    {FALSE,   /* port 0 -- cpu port 0 */
     TRUE,
     ENET_UNIT_LAN,
     0,
     0x00,
     RTL_LAN_PORT_VLAN
    },
};


/******************************************************************************
*
* rtl8211_phy_mii_read - mii register read
*
*/
static a_uint16_t
rtl8211_phy_reg_read (rtk_uint32 dev_id, rtk_uint32 phy_id, rtk_uint32 reg_id)
{
	return qca808x_phy_reg_read (dev_id, phy_id, reg_id);
}

/******************************************************************************
*
* rtl8211_phy_mii_write - mii register write
*
*/
static sw_error_t
rtl8211_phy_reg_write (rtk_uint32 dev_id, rtk_uint32 phy_id, rtk_uint32 reg_id,
	a_uint16_t reg_val)
{
	return qca808x_phy_reg_write (dev_id, phy_id, reg_id, reg_val);
}

/******************************************************************************
*
* rtl8211_phy_debug_read - debug port read
*
*/
static a_uint16_t
rtl8211_phy_debug_read(rtk_uint32 dev_id, rtk_uint32 phy_id, a_uint16_t reg_id)
{
	return qca808x_phy_debug_read(dev_id, phy_id, reg_id);
}

/******************************************************************************
*
* rtl8211_phy_debug_write - debug port write
*
*/
static sw_error_t
rtl8211_phy_debug_write(rtk_uint32 dev_id, rtk_uint32 phy_id, a_uint16_t reg_id,
	a_uint16_t reg_val)
{
	return qca808x_phy_debug_write (dev_id, phy_id, reg_id, reg_val);
}

/******************************************************************************
*
* rtl8211_phy_mmd_read -  PHY MMD register read
*
*/
static a_uint16_t
rtl8211_phy_mmd_read(rtk_uint32 dev_id, rtk_uint32 phy_id,
	a_uint16_t mmd_num, a_uint16_t reg_id)
{
	return qca808x_phy_mmd_read(dev_id, phy_id, mmd_num, reg_id);
}

/******************************************************************************
*
* rtl8211_phy_mmd_write - PHY MMD register write
*
*/
static sw_error_t
rtl8211_phy_mmd_write(rtk_uint32 dev_id, rtk_uint32 phy_id,
	a_uint16_t mmd_num, a_uint16_t reg_id, a_uint16_t reg_val)
{
	return qca808x_phy_mmd_write (dev_id, phy_id, mmd_num,
		reg_id, reg_val);
}


bool
rtl8367c_phy_is_link_alive(int phyUnit, int *plinkStatus, int *pspeed, int *pduplex)
{
	uint32_t phyBase;
	uint32_t phyAddr;
	rtk_api_ret_t retVal;
	int linkStatus, speed, duplex;

	phyBase = RTL8367C_PHYBASE(phyUnit);
	phyAddr = RTL8367C_PHYADDR(phyUnit);

	if (phyAddr > RTK_PHY_ID_MAX)
		return FALSE;

	retVal = rtk_port_phyStatus_get(phyAddr, (rtk_port_linkStatus_t *)&linkStatus,
		(rtk_port_speed_t *)&speed, (rtk_port_duplex_t *)&duplex);

	if (retVal != RT_ERR_OK)
	{
		printk("rtl8367c_phy_is_link_alive failed!!!\r\n");
		return FALSE;
	}
	*plinkStatus = linkStatus;
	switch(speed)
	{
	case PORT_SPEED_10M:
		*pspeed = FAL_SPEED_10;
		break;
	case PORT_SPEED_100M:
		*pspeed = FAL_SPEED_100;
		break;
	case PORT_SPEED_1000M:
		*pspeed = FAL_SPEED_1000;
		break;
	case PORT_SPEED_2500M:
		*pspeed = FAL_SPEED_2500;
		break;
	default:
		*pspeed = FAL_SPEED_10;
		break;
	}
	switch (duplex)
	{
	case PORT_HALF_DUPLEX:
		*pduplex = FAL_HALF_DUPLEX;
		break;
	case PORT_FULL_DUPLEX:
		*pduplex = FAL_FULL_DUPLEX;
		break;
	default:
		*pduplex = FAL_FULL_DUPLEX;
		break;
	}

	return linkStatus;
}



/******************************************************************************
*
* rtl8211_phy_get_status - get the phy status
*
*/
static sw_error_t
rtl8211_phy_get_status(rtk_uint32 dev_id, rtk_uint32 phy_id,
		struct port_phy_status *phy_status)
{
	rtk_port_linkStatus_t pLinkStatus[PORT_MAX_NUM] = {0};
	rtk_port_speed_t pSpeed[PORT_MAX_NUM] = {0};
	rtk_port_duplex_t pDuplex[PORT_MAX_NUM] = {0};
	
	int p = 0;

	for(p=0; p<PORT_MAX_NUM; p++)
	{
		if (RT_ERR_OK != rtk_port_phyStatus_get(rtl8367s_utp_port[p], &pLinkStatus[p], &pSpeed[p], &pDuplex[p]))
		{
			RTL8367S_LOG_ERR("rtk_port_phyStatus_get failed...\n");
			break;
		}
		SSDK_DEBUG("get rtl8211 linkstatus: %u, speed: %d, duplex: %d\n", pLinkStatus[p], pSpeed[p], pDuplex[p]);
		if (pLinkStatus[p])
		{
			switch(pSpeed[p])
			{
			case PORT_SPEED_10M:
				pSpeed[p] = FAL_SPEED_10;
				break;
			case PORT_SPEED_100M:
				pSpeed[p] = FAL_SPEED_100;
				break;
			case PORT_SPEED_1000M:
				pSpeed[p] = FAL_SPEED_1000;
				break;
			case PORT_SPEED_2500M:
				pSpeed[p] = FAL_SPEED_2500;
				break;
			default:
				pSpeed[p] = FAL_SPEED_10;
				break;
			}
			switch (pDuplex[p])
			{
			case PORT_HALF_DUPLEX:
				pDuplex[p] = FAL_HALF_DUPLEX;
				break;
			case PORT_FULL_DUPLEX:
				pDuplex[p] = FAL_FULL_DUPLEX;
				break;
			default:
				pDuplex[p] = FAL_FULL_DUPLEX;
				break;
			}
		
			phy_status->link_status = pLinkStatus[p];
			phy_status->speed = pSpeed[p];
			phy_status->duplex = pDuplex[p];
		}
	}
			
#if 0
	for (i = 0; i < 5; i++)
	{
		alive = rtl8367c_phy_is_link_alive(i, &linkStatus, &speed, &duplex);
		SSDK_DEBUG("[%d] linkstatus=%d speed=%d duplex=%d\n", 
			i, linkStatus, speed, duplex);
		if (linkStatus)
		{
			phy_status->link_status = linkStatus;
			phy_status->speed = speed;
			phy_status->duplex = duplex;
		}
	}
	
	SSDK_DEBUG("get rtl8211 linkstatus: %u, speed: %d, duplex: %d\n", phy_status->link_status, phy_status->speed, phy_status->duplex);
#endif

	return SW_OK;
}

/******************************************************************************
*
* rtl8211_set_autoneg_adv - set the phy autoneg Advertisement
*
*/
static sw_error_t
rtl8211_phy_set_autoneg_adv(rtk_uint32 dev_id, rtk_uint32 phy_id,
			   rtk_uint32 autoneg)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_set_autoneg_adv\n");

	return SW_OK;
}

/******************************************************************************
*
* rtl8211_get_autoneg_adv - get the phy autoneg Advertisement
*
*/
static sw_error_t
rtl8211_phy_get_autoneg_adv(rtk_uint32 dev_id, rtk_uint32 phy_id,
			   rtk_uint32 * autoneg)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_get_autoneg_adv\n");

	return SW_OK;
}

/******************************************************************************
*
* rtl8211_phy_get_speed - Determines the speed of phy ports associated with the
* specified device.
*/

static sw_error_t
rtl8211_phy_get_speed(rtk_uint32 dev_id, rtk_uint32 phy_id,
		     fal_port_speed_t * speed)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_get_speed\n");

	return SW_OK;
}

/******************************************************************************
*
* rtl8211_phy_get_duplex - Determines the duplex of phy ports associated with the
* specified device.
*/
static sw_error_t
rtl8211_phy_get_duplex(rtk_uint32 dev_id, rtk_uint32 phy_id,
		      fal_port_duplex_t * duplex)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_get_duplex\n");

	return SW_OK;
}

/******************************************************************************
*
* rtl8211_phy_set_speed - Set the speed of phy ports associated with the
* specified device.
*/
static sw_error_t
rtl8211_phy_set_speed(rtk_uint32 dev_id, rtk_uint32 phy_id,
		     fal_port_speed_t speed)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_set_speed\n");
	
	return SW_OK;
}

/******************************************************************************
*
* rtl8211_phy_set_duplex - Set the duplex of phy ports associated with the
* specified device.
*/
static sw_error_t
rtl8211_phy_set_duplex(rtk_uint32 dev_id, rtk_uint32 phy_id,
		      fal_port_duplex_t duplex)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_set_duplex\n");

	return SW_OK;
}

/******************************************************************************
*
* rtl8211_phy_enable_autoneg - enable the phy autoneg
*
*/
static sw_error_t
rtl8211_phy_enable_autoneg(rtk_uint32 dev_id, rtk_uint32 phy_id)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_enable_autoneg\n");
	 
	return SW_OK;

}

/******************************************************************************
*
* rtl8211_restart_autoneg - restart the phy autoneg
*
*/
static sw_error_t
rtl8211_phy_restart_autoneg(rtk_uint32 dev_id, rtk_uint32 phy_id)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_restart_autoneg\n");
 
	return SW_OK;
}

/******************************************************************************
*
* rtl8211_phy_autoneg_status - get the phy autoneg status
*
*/
static a_bool_t
rtl8211_phy_autoneg_status(rtk_uint32 dev_id, rtk_uint32 phy_id)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_autoneg_status\n");
	
	return A_FALSE;
}

/******************************************************************************
*
* rtl8211_phy_status - get the phy link status
*
* RETURNS:
*    A_TRUE  --> link is alive
*    A_FALSE --> link is down
*/
static a_bool_t
rtl8211_phy_get_link_status(rtk_uint32 dev_id, rtk_uint32 phy_id)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_get_link_status\n");
	
	return A_FALSE;
}

/******************************************************************************
*
* rtl8211_phy_reset - reset the phy
*
*/
static sw_error_t
rtl8211_phy_reset(rtk_uint32 dev_id, rtk_uint32 phy_id)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_reset\n");
 
	return SW_OK;
}


/******************************************************************************
*
* rtl8211_phy_get_phy_id - get the phy id
*
*/
static sw_error_t
rtl8211_phy_get_phy_id(rtk_uint32 dev_id, rtk_uint32 phy_id,
	rtk_uint32 *phy_data)
{
	return qca808x_phy_get_phy_id (dev_id, phy_id, phy_data);
}

/******************************************************************************
*
* rtl8211_phy_off - power off the phy
*
*/
static sw_error_t
rtl8211_phy_poweroff(rtk_uint32 dev_id, rtk_uint32 phy_id)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_poweroff\n");

	return SW_OK;
}

/******************************************************************************
*
* rtl8211_phy_on - power on the phy
*
*/
static sw_error_t
rtl8211_phy_poweron(rtk_uint32 dev_id, rtk_uint32 phy_id)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_poweron\n");

	return SW_OK;
}


/******************************************************************************
*
* rtl8211_interface_mode_set
*
*/
static sw_error_t
rtl8211_phy_interface_mode_set(rtk_uint32 dev_id, rtk_uint32 phy_id, fal_port_interface_mode_t interface_mode)
{
	return SW_OK;
}

static sw_error_t
rtl8211_phy_interface_mode_get(rtk_uint32 dev_id, rtk_uint32 phy_id, fal_port_interface_mode_t *interface_mode)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_interface_mode_get\n");
	*interface_mode = PORT_SGMII_PLUS;
	return SW_OK;
}

static sw_error_t
rtl8211_phy_interface_modestatus_get(rtk_uint32 dev_id, rtk_uint32 phy_id, fal_port_interface_mode_t *interface_mode)
{
	SSDK_ERROR("[ZC]===>rtl8211_phy_interface_modestatus_get\n");
	*interface_mode = PORT_SGMII_PLUS;
	return SW_OK;
}

static sw_error_t rtl8367_phy_api_ops_init(void)
{
	sw_error_t ret = SW_OK;
	hsl_phy_ops_t *rtl8367_phy_api_ops = NULL;

	rtl8367_phy_api_ops = kzalloc(sizeof(hsl_phy_ops_t), GFP_KERNEL);
	if (rtl8367_phy_api_ops == NULL)
	{
		SSDK_ERROR("rtl8211 phy ops kzalloc failed!\n");
		return -ENOMEM;
	}

	phy_api_ops_init(RTL8367S_PHY_CHIP);

	rtl8367_phy_api_ops->phy_reg_write = rtl8211_phy_reg_write;
	rtl8367_phy_api_ops->phy_reg_read = rtl8211_phy_reg_read;
	rtl8367_phy_api_ops->phy_debug_write = rtl8211_phy_debug_write;
	rtl8367_phy_api_ops->phy_debug_read = rtl8211_phy_debug_read;
	rtl8367_phy_api_ops->phy_mmd_write = rtl8211_phy_mmd_write;
	rtl8367_phy_api_ops->phy_mmd_read = rtl8211_phy_mmd_read;
	rtl8367_phy_api_ops->phy_get_status = rtl8211_phy_get_status;
	rtl8367_phy_api_ops->phy_speed_get = rtl8211_phy_get_speed;
	rtl8367_phy_api_ops->phy_speed_set = rtl8211_phy_set_speed;
	rtl8367_phy_api_ops->phy_duplex_get = rtl8211_phy_get_duplex;
	rtl8367_phy_api_ops->phy_duplex_set = rtl8211_phy_set_duplex;
	rtl8367_phy_api_ops->phy_autoneg_enable_set = rtl8211_phy_enable_autoneg;
	rtl8367_phy_api_ops->phy_restart_autoneg = rtl8211_phy_restart_autoneg;
	rtl8367_phy_api_ops->phy_autoneg_status_get = rtl8211_phy_autoneg_status;
	rtl8367_phy_api_ops->phy_autoneg_adv_set = rtl8211_phy_set_autoneg_adv;
	rtl8367_phy_api_ops->phy_autoneg_adv_get = rtl8211_phy_get_autoneg_adv;
	rtl8367_phy_api_ops->phy_link_status_get = rtl8211_phy_get_link_status;
	rtl8367_phy_api_ops->phy_reset = rtl8211_phy_reset;
	rtl8367_phy_api_ops->phy_id_get = rtl8211_phy_get_phy_id;
	rtl8367_phy_api_ops->phy_power_off = rtl8211_phy_poweroff;
	rtl8367_phy_api_ops->phy_power_on = rtl8211_phy_poweron;
	rtl8367_phy_api_ops->phy_interface_mode_set = rtl8211_phy_interface_mode_set;
	rtl8367_phy_api_ops->phy_interface_mode_get = rtl8211_phy_interface_mode_get;
	rtl8367_phy_api_ops->phy_interface_mode_status_get = rtl8211_phy_interface_modestatus_get;

	ret = hsl_phy_api_ops_register(RTL8367S_PHY_CHIP, rtl8367_phy_api_ops);

	if (ret == SW_OK)
	{
		SSDK_INFO("qca probe rtl8367 phy driver succeeded!\n");
	}
	else
	{
		SSDK_ERROR("qca probe rtl8367 phy driver failed! (code: %d)\n", ret);
	}

	return ret;
}


int rtl8367s_init(rtk_uint32 dev_id, rtk_uint32 port_bmp)
{
#if 0
	int saved = rtl8367s_debug_info;
	rtl8367s_debug_info = 1; /* open debug flag while init */
#endif
	static a_bool_t phy_ops_flag = A_FALSE;

	struct device_node *mdio;
	struct mii_bus *mdio_bus;

	SSDK_ERROR("Init RTL8367, dev_id: %d, port_bmp: %0x\n", dev_id, port_bmp);
	if(phy_ops_flag == A_FALSE &&
			rtl8367_phy_api_ops_init() == SW_OK) {
		phy_ops_flag = A_TRUE;
	}
	
	mdio = of_find_node_by_path("/soc/mdio@90000");

	if (!mdio)
	{
		SSDK_ERROR("RTL8367 init error -EINVAL\n");
		return -EINVAL;
	}

	mdio_bus = of_mdio_find_bus(mdio);

	if (!mdio_bus)
	{
		SSDK_ERROR("RTL8367 init error -EPROBE_DEFER\n");
		return -EPROBE_DEFER;
	}

	g_mii_bus = mdio_bus;

	rtl8367s_gpio_reset(30); //hardware reset

	RTL8367S_LOG_INFO("init wzx\r\n");

	RTL8367S_LOCK_INIT();

//	rtl8367s_hw_init();

	rtl8367s_mii_init();

	//rtl8367s_vlan_init();

	if(CHIP_RTL8367D != g_switch_chip)
	{
		rtl8367s_acl_init();
		rtl8367s_mcast_snoop = 0;
		rtl8367s_mcast_init();
	}

	rtl8367s_led_init();

	//rtl8367s_event_init();

	rtl8367s_proc_init();

	rtk_port_phyEnableAll_set(ENABLED);

	RTL8367S_LOG_INFO("done!\n");

#if 0
	rtl8367s_debug_info = saved; /* recover debug flag after init */
#endif

	return 0;
}

//#endif /* CONFIG_TP_EXT_SWITCH_RTL8367S */
