#include <generated/ca_ne_autoconf.h>
#include "ca_types.h"
#include "ca_module.h"
#include "osal_common.h"
#include "aal_common.h"
#include "aal_ni_l2.h"
#include "aal_ni.h"
#include "aal_phy.h"
#include "port.h"
#include "eth_port.h"
#include "sys.h"
#include "scfg.h"
#include "log.h"

#if !defined(CONFIG_CORTINA_BOARD_FPGA) && defined(CONFIG_LEDS_CA77XX)
#include <linux/leds-ca77xx.h>

#define  LED_SPEED_SUPPORT 1
#define  LED_DIRECTION_NUM 2 /* 0 for Rx LED, 1 for Tx LED */

/* LED index mapping for Ethernet ports, refer to ca7774-engboard.dts */
static ca_uint8_t  led_map[CA_ETH_PORT_NUM][LED_DIRECTION_NUM] = {
    {8, 15},
    {9, 14},
    {10, 13},
    {11, 12},
    {0, 7},
    {1, 6},
    {2, 5},
    {3, 4}
};

#ifdef LED_SPEED_SUPPORT
#define __ETH_LED_ON(port,speed)   do { \
    if ((speed) >= AAL_PHY_SPEED_1000) { \
        ca77xx_led_enable(led_map[port][0], TRUE); \
        ca77xx_led_sw_on(led_map[port][0], FALSE); \
        ca77xx_led_enable(led_map[port][1], TRUE); \
        ca77xx_led_sw_on(led_map[port][1], TRUE); \
    } else { \
        ca77xx_led_enable(led_map[port][0], TRUE); \
        ca77xx_led_sw_on(led_map[port][0], TRUE); \
        ca77xx_led_enable(led_map[port][1], TRUE); \
        ca77xx_led_sw_on(led_map[port][1], FALSE); \
    } }while(0)
#else
#define __ETH_LED_ON(port,speed)   do { ca77xx_led_enable(led_map[port][0], TRUE); ca77xx_led_enable(led_map[port][1], TRUE);}while(0)
#endif /* LED_SPEED_SUPPORT */

#define __ETH_LED_OFF(port)  do { ca77xx_led_enable(led_map[port][0], FALSE); ca77xx_led_enable(led_map[port][1], FALSE);}while(0)

#else

#define __ETH_LED_ON(port,speed)
#define __ETH_LED_OFF(port)

#endif /* !defined(CONFIG_CORTINA_BOARD_FPGA) && defined(CONFIG_ARCH_CORTINA_G3) */

#if defined(CONFIG_CA_NE_AAL_XFI)
#include "aal_xfi.h"
#endif

/******************************************************************************/
/*                             Import Variables and Functions                 */
/******************************************************************************/

/******************************************************************************/
/*                             Local MACROs                                   */
/******************************************************************************/

/* Local MACROs */
#ifdef CONFIG_CORTINA_BOARD_FPGA
#define __ETH_PORT_LINK_SCAN_INTERVAL  100
#else    /* ASIC GHPY Polling */
#define __ETH_PORT_LINK_SCAN_INTERVAL  1000 /* 1 seconds */
#endif

#define __ETH_PORT_LOCK(dev)        ca_spin_lock(g_eth_port_drv.eth_port_lock)
#define __ETH_PORT_UNLOCK(dev)      ca_spin_unlock(g_eth_port_drv.eth_port_lock)
#define __ETH_PHY_ADDR(aal_port)    (g_eth_port_drv.eth_mdio_addr[aal_port])
#define __MDIO_LOCK(device_id)  // TODO: because UBoot will use, so donnt add spin lock now
#define __MDIO_UNLOCK(device_id)

#define __FOR_ALL_GE_DO(port)   \
                       for((port)=__GE_PORT_START; (port)<=__GE_PORT_END; ++(port))

#define __FOR_ALL_GE_PORTS_DO(port)   \
                       for((port) = AAL_PORT_NUM_FIRST_ETH; (port)< AAL_NI_PORT_NUM; ++(port))

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
123fsdfdsf
#define __FOR_ALL_ETH_PORTS_DO(port)   \
                       for((port)=AAL_PORT_NUM_FIRST_ETH; (port)<(AAL_PORT_NUM_FIRST_ETH + CA_PORT_MAX_NUM_ETHERNET); ++(port))
#else
#define __FOR_ALL_ETH_PORTS_DO(port)   \
                       for((port)=0; (port)<CA_PORT_MAX_NUM_ETHERNET; ++(port))
#endif
/* Debug MACROs */
#define __ETHPORT_MOD_ID    CA_MOD_DRVR_DEV

#define __ETH_DBG(fmt,...)  do { \
                                if (g_eth_port_debug) \
                                cap_log_msg(0,__ETHPORT_MOD_ID, CAP_LOG_DEBUG,\
                                "%s():"fmt, __FUNCTION__,##__VA_ARGS__);\
                            }while(0)

#define __ETH_INIT_PRINT    printf
#define __ETH_IS_XGE(aal_port)  ((aal_port) >= AAL_NI_PORT_XGE0 && (aal_port) <= AAL_NI_PORT_XGE2)
#define __ETH_IS_CONNECT_SWITCH(aal_port)  ((aal_port) == AAL_NI_PORT_GE4)

                        //g_eth_port_drv.eth_if_mode[port] == AAL_NI_IF_MODE_RXAUI
/******************************************************************************/
/*                             Local global Var                               */
/******************************************************************************/
#define __ETH_PORT_NUM   (CA_PORT_MAX_NUM_ETHERNET + 8)
typedef struct {
    ca_uint eth_port_lock;
    ca_uint32_t eth_port_scan_timer;
    ca_uint32_t eth_port_scan_enable;
    ca_uint8_t eth_mdio_addr[__ETH_PORT_NUM];
    ca_uint8_t eth_if_mode[__ETH_PORT_NUM];
    ca_uint8_t autoneg[__ETH_PORT_NUM];
} __eth_port_drv_t;

__eth_port_drv_t g_eth_port_drv;
ca_uint32_t g_eth_port_debug = 0;

#if defined(CONFIG_ARCH_RTL8198F)
    #if defined(CONFIG_RTL_83XX_SUPPORT)
    ca_uint32_t mac_speed[] = { 
    	AAL_NI_MAC_SPEED_100M, AAL_NI_MAC_SPEED_100M, AAL_NI_MAC_SPEED_100M, AAL_NI_MAC_SPEED_100M,
    	AAL_NI_MAC_SPEED_1000M, AAL_NI_MAC_SPEED_100M, AAL_NI_MAC_SPEED_100M, AAL_NI_MAC_SPEED_100M
    };
    #else
    ca_uint32_t mac_speed[] = { 
    	AAL_NI_MAC_SPEED_100M, AAL_NI_MAC_SPEED_100M, AAL_NI_MAC_SPEED_100M, AAL_NI_MAC_SPEED_100M,
    	AAL_NI_MAC_SPEED_100M, AAL_NI_MAC_SPEED_100M, AAL_NI_MAC_SPEED_100M, AAL_NI_MAC_SPEED_100M
    };
    #endif	
#endif

/******************************************************************************/
/*                       Local Function Implementation                     */
/******************************************************************************/

static ca_status_t __xge_phy_link_status_get(
    ca_device_id_t device_id,
    ca_uint32_t aal_port,
    aal_phy_link_status_t *status)
{
    ca_status_t  ret = CA_E_OK;

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)

    if (aal_port == AAL_NI_PORT_XGE0) {
        aal_ni_xaui_xgxs_intr_status_t  xaui_status;

        memset(&xaui_status, 0, sizeof(xaui_status));

        ret = aal_ni_xaui_xgxs_intr_status_get(
            device_id, &xaui_status);
        if (CA_E_OK == ret) {
            status->link_up = xaui_status.Tx_xgxs_align_det;
            status->duplex = TRUE;
            status->speed  = AAL_PHY_SPEED_10000;
        }
    } else {
        aal_ni_xge_pcs_status_t  xge_pcs;

        memset(&xge_pcs, 0, sizeof(xge_pcs));

        ret = aal_ni_xge_pcs_status_get(device_id,
            aal_port - AAL_NI_PORT_XGE1, &xge_pcs);
        if (CA_E_OK == ret) {
            status->link_up = xge_pcs.rxlinksts;
            status->duplex = TRUE;
            status->speed  = AAL_PHY_SPEED_10000;
        }

    }
#elif  defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    if (aal_port >= AAL_NI_PORT_XGE0 && aal_port <= AAL_NI_PORT_XGE1) {
#if defined(CONFIG_CA_NE_AAL_XFI)
        ca_boolean_t  link_up = 0;
        ca_uint32_t   speed   = 0;
        ret = aal_xfi_link_status_get(device_id,
            aal_port - AAL_NI_PORT_XGE0, &link_up);
        CA_RET_VALUE_CHECK(ret);
        ret = aal_xfi_speed_get(device_id,
            aal_port - AAL_NI_PORT_XGE0, &speed);
        CA_RET_VALUE_CHECK(ret);

        status->link_up = link_up;
        status->speed = (aal_phy_speed_mode_t)speed;
        status->duplex = TRUE;
#endif
    } else {
#ifdef _DEBUG_
        ca_printf("error: port %d is NOT XGE\n", aal_port);
#endif
        return CA_E_PARAM;
    }
#endif

    return ret;

}

/*
 * timer handle for polling etherent port link status change,
 * and sync PHY speed/duplex to MAC
 */
static void __eth_port_link_scan_timer_func(void *cookie)
{
    ca_device_id_t device_id = 0;
    ca_port_id_t port_id = 0;
    ca_boolean_t autoneg_enable = 0;
    aal_phy_link_status_t link_status;
    aal_ni_eth_mac_config_mask_t eth_mac_mask;
    aal_ni_eth_mac_config_t eth_mac_config;
    static ca_boolean_t port_link_up[16] = { 0 };
    static ca_boolean_t port_link_speed[16] = { 0 };
    static ca_boolean_t port_link_duplex[16] = { 0 };

    eth_mac_mask.u32 = 0;
    eth_mac_mask.s.speed = 1;
    eth_mac_mask.s.duplex = 1;
    memset(&eth_mac_config, 0, sizeof(eth_mac_config));


    __FOR_ALL_ETH_PORTS_DO(port_id) {
        if (!(0x1 & (g_eth_port_drv.eth_port_scan_enable >> port_id))) {
            continue;
        }

#if defined(CONFIG_ARCH_CORTINA_G3LITE)
        if (__ETH_IS_CONNECT_SWITCH(port_id)) {
            continue;
        }
#endif
        if (__ETH_IS_XGE(port_id)) {
            if (CA_E_OK !=__xge_phy_link_status_get(device_id, port_id, &link_status))
                continue;

            if (port_link_up[port_id] != link_status.link_up) {
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
                if (port_id != 6)
#endif
                    if (link_status.link_up)
                        ca_printf("INFO: XGE Port %u Link UP %s bps, FULL duplex\n",
                            port_id,
                        (link_status.speed == AAL_PHY_SPEED_10000) ? "10G"
                        : ((link_status.speed == AAL_PHY_SPEED_2500) ? "2.5G"
                        : "1G"));
                    else
                        ca_printf("INFO: XGE Port %u Link Down \n", port_id);

                }
                port_link_up[port_id] = link_status.link_up;

            continue;
        }
        else if (CA_E_OK != aal_eth_phy_link_status_get(device_id, __ETH_PHY_ADDR(port_id),&link_status)) {
            continue;
        }

        if (link_status.link_up) {
            if (port_link_up[port_id]) {
                if (port_link_speed[port_id] == link_status.speed && port_link_duplex[port_id] == link_status.duplex) {
                    continue; /* Link status and speed/duplex NOT change */
                }
            }
        }
        else {
            if (!port_link_up[port_id]) { /* Link status NOT change*/
                continue;
            }
        }

        /*
         * Handle the port link change in the following
         */
        if (link_status.link_up && !__ETH_IS_XGE(port_id)) {
            (void)aal_eth_phy_auto_neg_get(device_id,
                       __ETH_PHY_ADDR(port_id), &autoneg_enable);

            if (autoneg_enable) {
                //ca_printf("INFO: Sync Eth PHY status to MAC Port %u\n", port_id);
                /* sync MAC speed/duplex with PHY */
                eth_mac_config.speed = link_status.speed;
                eth_mac_config.duplex = link_status.duplex;
                aal_ni_eth_port_mac_set(device_id, port_id, eth_mac_mask,
                            &eth_mac_config);
            }
        }

        /* Link change found, then report the event */
        port_link_up[port_id] = link_status.link_up;

        if (link_status.link_up) {
            char speed_str[4][10] = {"10M", "100M", "1G", "10G"}; /* NOTE: Must match aal_phy_speed_mode_t */

            __ETH_LED_ON(port_id, link_status.speed);
            ca_printf("INFO: Ethernet Port %u Link UP speed=%sbps, %s duplex\n", port_id, speed_str[link_status.speed], link_status.duplex ? "FULL" : "HALF");

            port_link_speed[port_id] = link_status.speed;
            port_link_duplex[port_id] = link_status.duplex;
        }
        else {

            __ETH_LED_OFF(port_id);

            ca_printf("INFO: Ethernet Port %u Link DOWN\n", port_id);
        }
    }

}

static ca_status_t __eth_port_scfg_load(ca_device_id_t device_id)
{
    ca_device_config_tlv_t scfg_tlv;
    ca_status_t ret = CA_E_OK;
    ca_uint32_t phy_id = 0;
    ca_uint32_t default_mdio[] = {/* FPGA board has 8 Eth Ports */
#if defined(CONFIG_ARCH_CORTINA_G3LITE)
        1, 2, 3, 4, 31, 31, 31, 31
#else
        1, 2, 3, 4, 16, 17, 18, 19
#endif
    };

/**
    Port Interface Configuration
FOR G3:
    LAN Port 0~3 ASIC: support GMII_GE/MII mode1.
    In GMII_GE mode, support 1Gbps and full-duplex2.
    In MII mode, support 10/100Mbps and full/half-duplex
    LAN Port 0~3 FPGA: support RGMII_GE/RGMII_FE1.
    In RGMII_GE mode, support 1Gbps and full-duplex2.
    In RGMII_FE mode, support 10/100Mpbs and half/full-duplex
    LAN Port 4 ASIC: support GMII_GE/RGMII_GE/RGMII_FE mode
    1. In GMII_GE mode, support 1000/100/10Mbps;
    2. In GMII_GE mode, support full-duplex at 1000Mbps;
    3. In GMII_GE mode, support half/full-duplex at 10/100Mbps;
    4. In RGMII_GE mode, support full-duplex at 1000Mbps;
    5. In RGMII_FE mode, support 10/100Mbps and full/half-duplex
    LAN Port 4 FPGA: support RGMII_GE/RGMII_FE mode
    1. In RGMII_GE mode, support full-duplex at 1000Mbps;
    2. In RGMII_FE mode, support 10/100Mbps and full/half-duplex
    Note: when Port 4 connects to SGMII PCS, it should set to GMII_GE mode.
    In thismode,port 4 can run 1000Mbps/full-duplex, 100Mbps/10Mbps/full or half-duplex.
FOR Raptor:Port 0~3 can be configured as 10/100Mbps MII mode or 1000Mbps GMII-like SerDesmode
    set int_cfg == GMII_GE for GMII-like SerDes mode
    set int_cfg == MII for 10/100M MII mode
    Port 4 can be configured as 10/100Mbps MII mode or RGMII_GE/RGMII_FE mode
    set int_cfg == MII for 10/100M MII mode
    set int_cfg == RGMII_GE for 1000M RGMII_GE mode
    set int_cfg == RGMII_FE for 100/10M RGMII_FE mode
*/
#ifdef CONFIG_CORTINA_BOARD_FPGA
    ca_uint32_t default_if_mode[] = {
        AAL_NI_IF_MODE_FE_RGMII, /* p0 */
        AAL_NI_IF_MODE_FE_RGMII,
        AAL_NI_IF_MODE_FE_RGMII,
        AAL_NI_IF_MODE_FE_RGMII,
        AAL_NI_IF_MODE_FE_RGMII, /* p4 FE */
        AAL_NI_IF_MODE_RXAUI   ,
        AAL_NI_IF_MODE_RXAUI   ,
        AAL_NI_IF_MODE_RXAUI    /* p7 */
    };
#else  /* CONFIG_CORTINA_BOARD_FPGA */
    ca_uint32_t default_if_mode[] = {
        AAL_NI_IF_MODE_GE_GMII, /* p0 */
        AAL_NI_IF_MODE_GE_GMII,
        AAL_NI_IF_MODE_GE_GMII,
        AAL_NI_IF_MODE_GE_GMII,
    #if defined(CONFIG_ARCH_CORTINA_G3LITE)
        AAL_NI_IF_MODE_GE_RGMII, /* p4 RGMII */
    #else
        AAL_NI_IF_MODE_GE_GMII, /* p4 GMII */
    #endif
        AAL_NI_IF_MODE_RXAUI   ,
        AAL_NI_IF_MODE_RXAUI   ,
        AAL_NI_IF_MODE_RXAUI    /* p7 */
    };
#endif /* CONFIG_CORTINA_BOARD_FPGA */



#if defined(CONFIG_ARCH_CORTINA_G3LITE)

#if CONFIG_98F_UBOOT_NE_DBG
    __ETH_INIT_PRINT("%s(%d) %s\n", __func__, __LINE__, __FILE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

#if CONFIG_98F_UBOOT_NE_DBG
    __ETH_INIT_PRINT("Init G3Lite chipset ether ports.\n");
#endif /* CONFIG_98F_UBOOT_NE_DBG */

#elif defined(CONFIG_ARCH_CORTINA_G3HGU)
    __ETH_INIT_PRINT("Init Saturn HGU chipset ether ports.\n");
#elif defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    __ETH_INIT_PRINT("Init Saturn SFU chipset ether ports.\n");
#elif defined(CONFIG_ARCH_CORTINA_G3)
    __ETH_INIT_PRINT("Init G3 chipset ether ports.\n");
#else
    __ETH_INIT_PRINT("Init Unk chipset ether ports.\n");
#endif



    /* ==== Read PHY MDIO address from STARTUP config ==== */
    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_PHY_BASE_ADDR;
    scfg_tlv.len = CFG_ID_PHY_BASE_ADDR_LEN;

#ifdef CONFIG_CORTINA_BOARD_FPGA
    __ETH_INIT_PRINT("FPGA Board: use default MDIO configurations.\n");
    __FOR_ALL_ETH_PORTS_DO(phy_id) {
        g_eth_port_drv.eth_mdio_addr[phy_id] = default_mdio[phy_id];
    }
#else /* !CONFIG_CORTINA_BOARD_FPGA */

#if 0//yocto: in scfg.c
    ret = ca_device_config_tlv_get(device_id, &scfg_tlv);
#else//sd1 uboot for 98f
    scfg_tlv.value[0] = 0x08;
    scfg_tlv.value[1] = 0x01;
    scfg_tlv.value[2] = 0x02;
    scfg_tlv.value[3] = 0x03;
    scfg_tlv.value[4] = 0x04;
    scfg_tlv.value[5] = 0x02;
    scfg_tlv.value[6] = 0x03;
    ret = CA_E_OK;
#endif

    if (CA_E_OK != ret) {
        __ETH_INIT_PRINT("!Err: read PHY_BASE_ADDR fail %#x!.\n", ret);
        /* Use following default MDIO configuration for ETH ports */
        __FOR_ALL_ETH_PORTS_DO(phy_id) {
            g_eth_port_drv.eth_mdio_addr[phy_id] = default_mdio[phy_id];
        }
    } else {
        memcpy(g_eth_port_drv.eth_mdio_addr,
               scfg_tlv.value, CFG_ID_PHY_BASE_ADDR_LEN);
    }
#endif /* !CONFIG_CORTINA_BOARD_FPGA */

    /* ==== Read ETH Interface mode ==== */
    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_PHY_INTERFACE;
    scfg_tlv.len = CFG_ID_PHY_INTERFACE_LEN;

#ifdef CONFIG_CORTINA_BOARD_FPGA
    __ETH_INIT_PRINT("FPGA Board: use default I/F mode configurations.\n");
    __FOR_ALL_ETH_PORTS_DO(phy_id) {
        g_eth_port_drv.eth_if_mode[phy_id] = default_if_mode[phy_id];
    }
#else /* !CONFIG_CORTINA_BOARD_FPGA */

#if 0//yocto: in scfg.c
    ret = ca_device_config_tlv_get(device_id, &scfg_tlv);
#else//sd1 uboot for 98f
    scfg_tlv.value[0] = 0x01;
    scfg_tlv.value[1] = 0x01;
    scfg_tlv.value[2] = 0x01;
    scfg_tlv.value[3] = 0x01;
    scfg_tlv.value[4] = 0x01;
    scfg_tlv.value[5] = 0x03;
    scfg_tlv.value[6] = 0x03;
    ret = CA_E_OK;
#endif

    if (CA_E_OK != ret) {
        __ETH_INIT_PRINT("!Err: read CA_CFG_ID_PHY_INTERFACE fail %#x!.\n", ret);
        /* Use following default I/F configuration for ETH ports */
        __FOR_ALL_ETH_PORTS_DO(phy_id) {
            g_eth_port_drv.eth_if_mode[phy_id] = AAL_NI_IF_MODE_GE_RGMII;
        }
    } else {
        memcpy(g_eth_port_drv.eth_if_mode,
               scfg_tlv.value, CFG_ID_PHY_INTERFACE_LEN);
    }
#endif /* !CONFIG_CORTINA_BOARD_FPGA */

    return ret;

}

static ca_status_t __eth_port_global_init(ca_device_id_t device_id)
{
    g_eth_port_debug = 0;

    //ca_printf("!!!! NEW ETH PORT DRIVER support XGE!!!\r\n");
    memset(&g_eth_port_drv, 0, sizeof(g_eth_port_drv));

    /* enable ethernet port scan for G3 ASIC */
    g_eth_port_drv.eth_port_scan_enable = 0xFF;

#if 0//yocto
    ca_spin_lock_init(&g_eth_port_drv.eth_port_lock, SPINLOCK_BH);
#else//sd1 uboot for 98f
#endif

    aal_mdio_global_init(device_id);

    /* Load startup configuration into global var */
    __eth_port_scfg_load(device_id);

    return CA_E_OK;
}

void ca_eth_port_exit(CA_IN ca_device_id_t device_id)
{
    if (g_eth_port_drv.eth_port_lock)
        ca_spin_lock_destroy(g_eth_port_drv.eth_port_lock);

    if (g_eth_port_drv.eth_port_scan_timer) {
        ca_timer_del(g_eth_port_drv.eth_port_scan_timer);
    }

    aal_mdio_global_exit(device_id);

    memset(&g_eth_port_drv, 0, sizeof(g_eth_port_drv));

}

static ca_status_t __eth_port_mac_init(ca_device_id_t device_id)
{
    aal_ni_eth_mac_config_mask_t eth_mac_mask;
    aal_ni_eth_mac_config_t eth_mac_config;
    aal_ni_eth_if_config_mask_t eth_if_mask;
    aal_ni_eth_if_config_t eth_if_cfg;
    ca_uint32_t port_id = 0;
    ca_status_t ret = CA_E_OK;

    eth_mac_mask.u32 = 0;
    eth_mac_mask.s.duplex = 1;
    eth_mac_mask.s.speed = 1;
    eth_mac_mask.s.mac_crc_calc_en = 1;
    eth_mac_mask.s.tx_en = 1;
    eth_mac_mask.s.rx_pfc_disable = 1;

    memset(&eth_mac_config, 0, sizeof(eth_mac_config));
#ifdef CONFIG_CORTINA_BOARD_FPGA
    eth_mac_config.speed = AAL_NI_MAC_SPEED_100M;  /* 100Mbps if FPGA board */
#else
    eth_mac_config.speed = AAL_NI_MAC_SPEED_1000M; /* 1G */
#endif

    eth_mac_config.duplex = AAL_NI_MAC_DUPLEX_FULL; /* FULL Duplex */
    eth_mac_config.mac_crc_calc_en = 1; /* enable CRC caculation */
    eth_mac_config.tx_en = 1;   /* Enable */
    eth_mac_config.rx_pfc_disable = 1;  /* Disable PFC function */

    eth_if_mask.u32 = 0;
    eth_if_mask.s.int_cfg = 1;
    eth_if_mask.s.phy_mode = 1;

    memset(&eth_if_cfg, 0, sizeof(eth_if_cfg));
    eth_if_cfg.phy_mode = AAL_NI_PHY_MODE_MAC;  /* connect to PHY */

    /* ASIC NI block init */
    //aal_ni_init(device_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
        __FOR_ALL_GE_PORTS_DO(port_id) {

        eth_if_cfg.int_cfg = g_eth_port_drv.eth_if_mode[port_id];

        ret = aal_ni_eth_if_set(device_id, port_id,
                    eth_if_mask, &eth_if_cfg);

        if (CA_E_OK != ret) {
            ca_printf("port %u interface init fail %#x.", port_id,
                  ret);
            return ret;
        }

        ret = aal_ni_eth_port_mac_set(device_id, port_id,
                          eth_mac_mask, &eth_mac_config);

        if (CA_E_OK != ret) {
            ca_printf("port %u MAC init fail %#x.", port_id, ret);
            return ret;
        }
    }
#else
    for (port_id = 0; port_id < AAL_NI_PORT_NUM; port_id++){

#if defined(CONFIG_ARCH_RTL8198F)
        eth_mac_config.speed = mac_speed[port_id];
#endif
        eth_if_cfg.int_cfg = g_eth_port_drv.eth_if_mode[port_id];

        ret = aal_ni_eth_if_set(device_id, port_id,
                    eth_if_mask, &eth_if_cfg);

        if (CA_E_OK != ret) {
            ca_printf("port %u interface init fail %#x.", port_id,
                  ret);
            return ret;
        }

        ret = aal_ni_eth_port_mac_set(device_id, port_id,
                          eth_mac_mask, &eth_mac_config);

        if (CA_E_OK != ret) {
            ca_printf("port %u MAC init fail %#x.", port_id, ret);
            return ret;
        }
    }
#endif
    return ret;

}

static ca_status_t __eth_port_misc_init(ca_device_id_t device_id)
{
#if 0//yocto
    /* polling timer init */
    g_eth_port_drv.eth_port_scan_timer =
        ca_circle_timer_add(__ETH_PORT_LINK_SCAN_INTERVAL,
                __eth_port_link_scan_timer_func, NULL);

    if (CA_INVALID_TIMER == g_eth_port_drv.eth_port_scan_timer)
        return CA_E_ERROR;

#else//sd1 uboot for 98f - TBD
#endif

    return CA_E_OK;
}

ca_status_t cap_mdio_speed_set(CA_IN ca_device_id_t device_id,
                  CA_IN ca_uint32_t addr, CA_IN ca_uint32_t clock)
{
    ca_status_t ret = CA_E_OK;
    CA_ASSERT_RET(addr >= CA_MDIO_ADDR_MIN, CA_E_PARAM);
    CA_ASSERT_RET(addr <= CA_MDIO_ADDR_MAX, CA_E_PARAM);
    CA_ASSERT_RET(clock, CA_E_PARAM);

    __MDIO_LOCK(device_id);
    ret = aal_mdio_speed_set(device_id, addr, clock);
    __MDIO_UNLOCK(device_id);

    return ret;
}

ca_status_t cap_mdio_speed_get(CA_IN ca_device_id_t device_id,
                  CA_IN ca_uint32_t addr,
                  CA_OUT ca_uint32_t * clock)
{
    ca_status_t ret = CA_E_OK;

    CA_ASSERT_RET(addr >= CA_MDIO_ADDR_MIN, CA_E_PARAM);
    CA_ASSERT_RET(addr <= CA_MDIO_ADDR_MAX, CA_E_PARAM);
    CA_ASSERT_RET(clock, CA_E_PARAM);

    __MDIO_LOCK(device_id);
    ret = aal_mdio_speed_get(device_id, addr, clock);
    __MDIO_UNLOCK(device_id);

    return ret;

}

ca_status_t cap_mdio_write(CA_IN ca_device_id_t device_id,
CA_IN      ca_uint8_t              st_code,
CA_IN      ca_uint8_t              phy_port_addr,
CA_IN      ca_uint8_t              reg_dev_addr,
CA_IN      ca_uint16_t             addr,
CA_IN      ca_uint16_t            data)
{
    ca_status_t ret = CA_E_OK;

    CA_ASSERT_RET(phy_port_addr >= CA_MDIO_ADDR_MIN, CA_E_PARAM);
    CA_ASSERT_RET(phy_port_addr <= CA_MDIO_ADDR_MAX, CA_E_PARAM);

    __MDIO_LOCK(device_id);
    ret = aal_mdio_write(device_id, st_code, phy_port_addr, reg_dev_addr, addr, data);
    __MDIO_UNLOCK(device_id);

    return ret;

}

ca_status_t cap_mdio_read(CA_IN ca_device_id_t device_id,
CA_IN      ca_uint8_t              st_code,
CA_IN      ca_uint8_t              phy_port_addr,
CA_IN      ca_uint8_t              reg_dev_addr,
CA_IN      ca_uint16_t             addr,
CA_OUT   ca_uint16_t         *data)
{
    ca_status_t ret = CA_E_OK;

    CA_ASSERT_RET(phy_port_addr >= CA_MDIO_ADDR_MIN, CA_E_PARAM);
    CA_ASSERT_RET(phy_port_addr <= CA_MDIO_ADDR_MAX, CA_E_PARAM);
    CA_ASSERT_RET(data, CA_E_PARAM);

    __MDIO_LOCK(device_id);
    ret = aal_mdio_read(device_id, st_code, phy_port_addr, reg_dev_addr, addr, data);
    __MDIO_UNLOCK(device_id);

    return ret;
}


/******************************************************************************/
/*                       Ethernet Port API Implementation                     */
/******************************************************************************/

static ca_status_t __ge_pause_set(
        ca_device_id_t  device_id,
        ca_uint32_t  aal_port,
        ca_boolean_t pfc_enable,
        ca_boolean_t pause_rx,
        ca_boolean_t pause_tx)
{
    aal_ni_eth_mac_config_t      aal_config;
    aal_ni_eth_mac_config_mask_t aal_mask;
    aal_phy_autoneg_adv_t adv;

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    if (aal_port >= AAL_NI_PORT_NUM || aal_port < AAL_PORT_NUM_FIRST_ETH)
        return CA_E_PARAM;
#else
    if (aal_port >= AAL_NI_PORT_NUM)
        return CA_E_PARAM;
#endif
    /* Set PHY pause */
    aal_eth_phy_ability_adv_get(device_id,
                      __ETH_PHY_ADDR(aal_port), &adv);

    adv.adv_pause = (pause_rx && pause_tx);
    adv.adv_asym_pause = (pause_rx || pause_tx);

    aal_eth_phy_ability_adv_set(device_id,
                      __ETH_PHY_ADDR(aal_port), adv);

    memset(&aal_config, 0, sizeof(aal_ni_eth_mac_config_t));

    /* Set MAC pause */
    aal_config.tx_pause_en      = pause_tx;
    aal_config.rx_pause_en      = pause_rx;
    /* enable stop tx request to Tx MAC when recevie PAUSE */
    aal_config.rx_flow_to_tx_en = pause_rx;

    if (pfc_enable) {/* Enable PFC should disable FC according to ASIC */
        aal_config.rx_pfc_disable   = 0;//!pause_rx; enable signal to TE
        aal_config.tx_pfc_disable   = !pause_tx;
        aal_config.rx_flow_disable  = 1;
        aal_config.tx_flow_disable  = 1;
    } else {
        aal_config.rx_pfc_disable   = 1;
        aal_config.tx_pfc_disable   = 1;
        aal_config.rx_flow_disable  = !pause_rx;
        aal_config.tx_flow_disable  = !pause_tx;
    }

    aal_mask.u32 = 0;
    aal_mask.s.rx_flow_disable  = 1;
    aal_mask.s.tx_flow_disable  = 1;
    aal_mask.s.rx_pause_en      = 1;
    aal_mask.s.tx_pause_en      = 1;
    aal_mask.s.rx_flow_to_tx_en = 1;
    aal_mask.s.rx_pfc_disable   = 1;
    aal_mask.s.tx_pfc_disable   = 1;

    return aal_ni_eth_port_mac_set(device_id, aal_port,
                      aal_mask, &aal_config);
}

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
static ca_status_t __xge_pause_set(ca_device_id_t device_id,
    ca_uint32_t aal_port, ca_boolean_t pfc_enable,
    ca_boolean_t pause_rx, ca_boolean_t pause_tx)
{
    ca_status_t ret = CA_E_OK;
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    ca_port_id_t xge_start = AAL_NI_PORT_XGE1;
#elif defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    ca_port_id_t xge_start = AAL_NI_PORT_XGE0;
#endif
    aal_ni_xaui_mac_tx_cfg_mask_t  xaui_mac_tx_mask;
    aal_ni_xaui_mac_tx_cfg_t       xaui_mac_tx;
    aal_ni_xaui_mac_rx_cfg_mask_t  xaui_mac_rx_mask;
    aal_ni_xaui_mac_rx_cfg_t       xaui_mac_rx;
    aal_ni_xaui_mac_rx_ctrl_mask_t xaui_mac_rx_ctrl_mask;
    aal_ni_xaui_mac_tx_ctrl_mask_t xaui_mac_tx_ctrl_mask;
    aal_ni_xaui_mac_rx_ctrl_t      xaui_mac_rx_ctrl;
    aal_ni_xaui_mac_tx_ctrl_t      xaui_mac_tx_ctrl;
    aal_ni_eth_mac_config_mask_t   ni_config_mask;
    aal_ni_eth_mac_config_t        ni_config;

    memset(&xaui_mac_tx, 0, sizeof(aal_ni_xaui_mac_tx_cfg_t));
    memset(&xaui_mac_rx, 0, sizeof(aal_ni_xaui_mac_rx_cfg_t));
    memset(&xaui_mac_rx_ctrl, 0, sizeof(aal_ni_xaui_mac_rx_ctrl_t));
    memset(&xaui_mac_tx_ctrl, 0, sizeof(aal_ni_xaui_mac_tx_ctrl_t));

    ni_config_mask.u32 = 0;
    memset(&ni_config, 0, sizeof(ni_config));
    ni_config_mask.s.tx_pause_en = 1;
    ni_config_mask.s.rx_pause_en = 1;

    ni_config.tx_pause_en = pause_tx;
    ni_config.rx_pause_en = pause_rx;

    ret = aal_ni_eth_port_mac_set(device_id, aal_port,
        ni_config_mask, &ni_config);
    CA_RET_VALUE_CHECK(ret);

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    if (aal_port == AAL_NI_PORT_XGE0) {
        xaui_mac_tx_mask.u32 = 0;
        xaui_mac_rx_mask.u32 = 0;
        xaui_mac_rx_ctrl_mask.u32 = 0;
        xaui_mac_tx_ctrl_mask.u32 = 0;
        xaui_mac_tx_mask.s.flow_en  = 1;
        xaui_mac_rx_mask.s.pause_en = 1;
        xaui_mac_rx_ctrl_mask.s.qbb_pause_det_en  = 1;
        xaui_mac_tx_ctrl_mask.s.qbb_flow_en = 1;
        xaui_mac_rx_ctrl_mask.s.frame_pause_drop_en  = 1;
        xaui_mac_rx_ctrl_mask.s.qbb_pause_drop_en = 1;
        xaui_mac_tx_ctrl_mask.s.qbb_qen = 1;

        if (pfc_enable) { /* PFC and FC should not enable at the same time according to ASIC */
            xaui_mac_tx_ctrl.qbb_qen = 0xFF;
            xaui_mac_tx.flow_en  = 0;
            xaui_mac_rx.pause_en = 0;
            xaui_mac_rx_ctrl.qbb_pause_det_en = 1; /* Enable PFC detect */
            xaui_mac_rx_ctrl.frame_pause_drop_en = 1;
            xaui_mac_rx_ctrl.qbb_pause_drop_en = !pause_rx;
            xaui_mac_tx_ctrl.qbb_flow_en = pause_tx;
        } else {
            xaui_mac_tx_ctrl.qbb_qen = 0;
            xaui_mac_tx.flow_en  = pause_tx;
            xaui_mac_rx.pause_en = pause_rx;
            xaui_mac_rx_ctrl.frame_pause_drop_en = !pause_rx;
            xaui_mac_rx_ctrl.qbb_pause_drop_en = 1;
            xaui_mac_rx_ctrl.qbb_pause_det_en = 0; /* Disable PFC detect */
            xaui_mac_tx_ctrl.qbb_flow_en = 0;
        }

        ret = aal_ni_xaui_mac_tx_ctrl_set(device_id, xaui_mac_tx_ctrl_mask, &xaui_mac_tx_ctrl);
        CA_RET_VALUE_CHECK(ret);


        ret = aal_ni_xaui_mac_rx_ctrl_set(device_id, xaui_mac_rx_ctrl_mask, &xaui_mac_rx_ctrl);
        CA_RET_VALUE_CHECK(ret);


        ret = aal_ni_xaui_mac_tx_config_set(device_id, xaui_mac_tx_mask, &xaui_mac_tx);
        CA_RET_VALUE_CHECK(ret);

        return aal_ni_xaui_mac_rx_config_set(device_id, xaui_mac_rx_mask, &xaui_mac_rx);
    }else
#endif
    if (aal_port <= AAL_NI_PORT_XGE2){
        aal_ni_xge_flowctrl_mask_t      xge_fc_mask;
        aal_ni_xge_flowctrl_t           xge_fc_config;
        aal_ni_xge_cfg_tx_mask_t        xge_tx_mask;
        aal_ni_xge_cfg_tx_t             xge_tx_cfg;

        xge_tx_mask.u32 = 0;
        xge_tx_mask.s.pause_req_en = 1;
        xge_tx_mask.s.pause_req_auto_xon = 1;
        memset(&xge_tx_cfg, 0, sizeof(aal_ni_xge_cfg_tx_t));

        xge_fc_mask.u32 = 0;
        xge_fc_mask.s.pau_tx_en = 1;
        xge_fc_mask.s.pau_rx_en = 1;
        xge_fc_mask.s.qbb_tx_en = 1;
        xge_fc_mask.s.qbb_rx_en = 1;
        xge_fc_mask.s.qbb_qen   = 1;

        memset(&xge_fc_config, 0, sizeof(aal_ni_xge_flowctrl_t));
        if (pfc_enable) {
            xge_fc_config.qbb_qen   = 0xFF;
            xge_fc_config.pau_tx_en = 0;
            xge_fc_config.pau_rx_en = 0;
            xge_fc_config.qbb_tx_en = pause_tx;
            xge_fc_config.qbb_rx_en = pause_rx;
            xge_tx_cfg.pause_req_en = 0;
            xge_tx_cfg.pause_req_auto_xon = 0;
        } else {
            xge_fc_config.qbb_qen   = 0;
            xge_fc_config.pau_tx_en = pause_tx;
            xge_fc_config.pau_rx_en = pause_rx;
            xge_fc_config.qbb_tx_en = 0;
            xge_fc_config.qbb_rx_en = 0;
            xge_tx_cfg.pause_req_en = pause_tx;
            xge_tx_cfg.pause_req_auto_xon = pause_tx;
        }
        ret = aal_ni_xge_flowctrl_set(device_id, aal_port - xge_start, xge_fc_mask, &xge_fc_config);
        CA_RET_VALUE_CHECK(ret);

        ret = aal_ni_xge_tx_config_set(device_id, aal_port - xge_start, xge_tx_mask, &xge_tx_cfg);

    }

    return ret;

}
#endif

ca_status_t ca_eth_port_pause_set(CA_IN ca_device_id_t device_id,
                  CA_IN ca_port_id_t port_id,
                  CA_IN ca_boolean_t pfc_enable,
                  CA_IN ca_boolean_t pause_rx,
                  CA_IN ca_boolean_t pause_tx)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x pause_rx = %u pause_tx = %u.\n",
        device_id, port_id, pause_rx, pause_tx);

    CA_PORT_ETHERNET_CHECK(port_id);
    CA_BOOL_VAL_CHECK(pause_rx);
    CA_BOOL_VAL_CHECK(pause_tx);

    __ETH_PORT_LOCK(device_id);

    if (__ETH_IS_XGE(aal_port)) {/* XGE */
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
        ret = __xge_pause_set(device_id, aal_port,
            pfc_enable, pause_rx, pause_tx);
#endif
    }
    else { /* GE/FE */
        ret = __ge_pause_set(device_id, aal_port,
            pfc_enable, pause_rx, pause_tx);
    }
    __ETH_PORT_UNLOCK(device_id);

    return ret;

}

ca_status_t ca_eth_port_pause_get(CA_IN ca_device_id_t device_id,
                  CA_IN ca_port_id_t   port_id,
                  CA_OUT ca_boolean_t *pfc_enable,
                  CA_OUT ca_boolean_t *pause_rx,
                  CA_OUT ca_boolean_t *pause_tx)
{
    ca_uint32_t aal_port = PORT_ID(port_id);
    ca_status_t ret = CA_E_OK;

    __ETH_DBG("device_id = %u port_id = %#x pause_rx =% p pause_tx = %p.\n",
        device_id, port_id, pause_rx, pause_tx);

    CA_ASSERT_RET(pause_rx != NULL && pause_tx != NULL, CA_E_PARAM);
    CA_PORT_ETHERNET_CHECK(port_id);

    __ETH_PORT_LOCK(device_id);

    if (__ETH_IS_XGE(aal_port)) {/* XGE */
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
        if (aal_port == AAL_NI_PORT_XGE0) {
            aal_ni_xaui_mac_tx_cfg_t  xaui_mac_tx;
            aal_ni_xaui_mac_rx_cfg_t  xaui_mac_rx;
            aal_ni_xaui_mac_rx_ctrl_t xaui_mac_rx_ctrl;
            aal_ni_xaui_mac_tx_ctrl_t xaui_mac_tx_ctrl;

            memset(&xaui_mac_tx, 0, sizeof(aal_ni_xaui_mac_tx_cfg_t));
            memset(&xaui_mac_rx, 0, sizeof(aal_ni_xaui_mac_rx_cfg_t));
            memset(&xaui_mac_rx_ctrl, 0, sizeof(aal_ni_xaui_mac_rx_ctrl_t));
            memset(&xaui_mac_tx_ctrl, 0, sizeof(aal_ni_xaui_mac_tx_ctrl_t));

            ret = aal_ni_xaui_mac_tx_config_get(device_id, &xaui_mac_tx);
            if (CA_E_OK != ret) {
                goto __api_end;
            }

            ret = aal_ni_xaui_mac_rx_config_get(device_id, &xaui_mac_rx);
            if (CA_E_OK != ret) {
                goto __api_end;
            }

            ret = aal_ni_xaui_mac_tx_ctrl_get(device_id, &xaui_mac_tx_ctrl);
            if (CA_E_OK != ret) {
                goto __api_end;
            }

            ret = aal_ni_xaui_mac_rx_ctrl_get(device_id, &xaui_mac_rx_ctrl);
            if (CA_E_OK != ret) {
                goto __api_end;
            }

            if (xaui_mac_rx_ctrl.qbb_pause_det_en) {
                *pfc_enable = TRUE;
                *pause_tx = xaui_mac_tx_ctrl.qbb_flow_en;
                *pause_rx = !xaui_mac_rx_ctrl.qbb_pause_drop_en;
            } else {
                *pfc_enable = FALSE;
                *pause_tx = xaui_mac_tx.flow_en;
                *pause_rx = xaui_mac_rx.pause_en;
            }

        }
        else if (aal_port <= AAL_NI_PORT_XGE2){

            aal_ni_xge_flowctrl_t  xge_fc_config;

            memset(&xge_fc_config, 0, sizeof(aal_ni_xge_flowctrl_t));

            ret = aal_ni_xge_flowctrl_get(device_id, aal_port - AAL_NI_PORT_XGE1, &xge_fc_config);
            if (CA_E_OK != ret) {
                goto __api_end;
            }

            if (xge_fc_config.qbb_qen) {
                *pause_tx = xge_fc_config.qbb_tx_en;
                *pause_rx = xge_fc_config.qbb_rx_en;
                *pfc_enable = TRUE;
            } else {
                *pause_tx = xge_fc_config.pau_tx_en;
                *pause_rx = xge_fc_config.pau_rx_en;
                *pfc_enable = FALSE;
            }

        }
#elif defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
        if (aal_port >= AAL_NI_PORT_XGE0 && aal_port <= AAL_NI_PORT_XGE1){

            aal_ni_xge_flowctrl_t  xge_fc_config;

            memset(&xge_fc_config, 0, sizeof(aal_ni_xge_flowctrl_t));

            ret = aal_ni_xge_flowctrl_get(device_id, aal_port - AAL_NI_PORT_XGE0, &xge_fc_config);
            if (CA_E_OK != ret) {
                goto __api_end;
            }

            if (xge_fc_config.qbb_qen) {
                *pause_tx = xge_fc_config.qbb_tx_en;
                *pause_rx = xge_fc_config.qbb_rx_en;
                *pfc_enable = TRUE;
            } else {
                *pause_tx = xge_fc_config.pau_tx_en;
                *pause_rx = xge_fc_config.pau_rx_en;
                *pfc_enable = FALSE;
            }
        }
#endif
    }
    else { /* GE/FE */
        aal_ni_eth_mac_config_t aal_config;

        ret = aal_ni_eth_port_mac_get(device_id,
                          aal_port, &aal_config);

        if (CA_E_OK != ret) {
            goto __api_end;
        }

        if (aal_config.rx_pfc_disable && aal_config.tx_pfc_disable) {
            *pfc_enable = FALSE;
            *pause_rx = !aal_config.rx_flow_disable;
            *pause_tx = !aal_config.tx_flow_disable;
        } else {
            *pfc_enable = TRUE;
            *pause_rx = !aal_config.rx_pfc_disable;
            *pause_tx = !aal_config.tx_pfc_disable;
        }

    }

  __api_end:

    __ETH_PORT_UNLOCK(device_id);

    return ret;

}

ca_status_t ca_eth_port_link_scan_set(CA_IN ca_device_id_t device_id,
                      CA_IN ca_port_id_t port_id,
                      CA_IN ca_boolean_t enable)
{
    ca_uint32_t aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x enable = %u.\n",
        device_id, port_id, enable);

    CA_PORT_ETHERNET_CHECK(port_id);

    __ETH_PORT_LOCK(device_id);

    if (enable) {
        /* enable the port-mapping bit */
        g_eth_port_drv.eth_port_scan_enable |= 0x1 << aal_port;
    } else {
        g_eth_port_drv.eth_port_scan_enable &= ~(0x1 << aal_port);
    }

    __ETH_PORT_UNLOCK(device_id);

    return CA_E_OK;

}

ca_status_t ca_eth_port_link_scan_get(CA_IN ca_device_id_t device_id,
                      CA_IN ca_port_id_t port_id,
                      CA_OUT ca_boolean_t * enable)
{
    ca_uint32_t aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x enable = %p.\n",
        device_id, port_id, enable);

    CA_PORT_ETHERNET_CHECK(port_id);
    CA_ASSERT_RET(enable != NULL, CA_E_PARAM);

    __ETH_PORT_LOCK(device_id);

    *enable = (g_eth_port_drv.eth_port_scan_enable >> aal_port) & 0x1;

    __ETH_PORT_UNLOCK(device_id);

    return CA_E_OK;
}

ca_status_t ca_eth_port_link_status_get(CA_IN ca_device_id_t device_id,
            CA_IN ca_port_id_t port_id,
            CA_OUT ca_eth_port_link_status_t *status)
{
    ca_uint32_t           aal_port = PORT_ID(port_id);
    ca_status_t           ret = CA_E_OK;
    aal_phy_link_status_t phy_lnk;

    __ETH_DBG("device_id = %u port_id = %#x status = %p.\n",
        device_id, port_id, status);

    CA_ASSERT_RET(status != NULL, CA_E_PARAM);
    CA_PORT_ETHERNET_CHECK(port_id);

    memset(&phy_lnk, 0, sizeof(phy_lnk));

    __ETH_PORT_LOCK(device_id);

    if (__ETH_IS_XGE(aal_port)) {/* XGE */
        ret = __xge_phy_link_status_get(device_id, aal_port, &phy_lnk);
    }
    else {
        ret = aal_eth_phy_link_status_get(device_id,
                        __ETH_PHY_ADDR(aal_port),
                        &phy_lnk);
    }
    __ETH_PORT_UNLOCK(device_id);

    if (CA_E_OK == ret) {
        *status = phy_lnk.link_up ? CA_PORT_LINK_UP : CA_PORT_LINK_DOWN;
    }

    return ret;

}


static ca_status_t __eth_stat_get(ca_device_id_t device_id,
                 ca_uint32_t aal_port,
                 ca_boolean_t read_keep,
                 ca_eth_port_stat_t * data)
{
    aal_ni_mib_stats_t ni_mib;
    ca_status_t ret = CA_E_OK;

    memset(&ni_mib, 0, sizeof(aal_ni_mib_stats_t));

    ret = aal_ni_eth_port_mib_get(device_id, aal_port, !read_keep, &ni_mib);
    CA_RET_VALUE_CHECK(ret);

            data->rx_good_octets = ((ca_uint64_t)ni_mib.rx_byte_cnt_high << 32)
                            + (ca_uint64_t)ni_mib.rx_byte_cnt_low;
            data->rx_uc_frames = ni_mib.rx_uc_pkt_cnt;
            data->rx_mc_frames = ni_mib.rx_mc_frame_cnt;
            data->rx_bc_frames = ni_mib.rx_bc_frame_cnt;
            data->rx_fcs_error_frames = ni_mib.rx_crc_error_frame_cnt;
            data->rx_pause_frames = ni_mib.rx_pause_frame_cnt;
            data->rx_undersize_frames = ni_mib.rx_undersize_frame_cnt;
            data->rx_runt_frames = ni_mib.rx_runt_frame_cnt;
            data->rx_gaint_frames = ni_mib.rx_ovsize_frame_cnt;
            data->rx_64_frames = ni_mib.rx_frame_64_cnt;
            data->rx_65_127_frames = ni_mib.rx_frame_65to127_cnt;
            data->rx_128_255_frames = ni_mib.rx_frame_128to255_cnt;
            data->rx_256_511_frames = ni_mib.rx_frame_256to511_cnt;
            data->rx_512_1023_frames = ni_mib.rx_frame_512to1023_cnt;
            data->rx_1024_1518_frames = ni_mib.rx_frame_1024to1518_cnt;
            data->rx_1519_max_frames = ni_mib.rx_frame_1519to2100_cnt +
                ni_mib.rx_frame_2101to9200_cnt + ni_mib.rx_frame_9201tomax_cnt;
            data->tx_good_octets = ((ca_uint64_t)ni_mib.tx_byte_cnt_high << 32)
                            + (ca_uint64_t)ni_mib.tx_byte_cnt_low;
            data->tx_uc_frames = ni_mib.tx_uc_pkt_cnt;
            data->tx_mc_frames = ni_mib.tx_mc_frame_cnt;
            data->tx_bc_frames = ni_mib.tx_bc_frame_cnt;
            data->tx_pause_frames = ni_mib.tx_pause_frame_cnt;
            data->tx_64_frames = ni_mib.tx_frame_64_cnt;
            data->tx_65_127_frames = ni_mib.tx_frame_65to127_cnt;
            data->tx_128_255_frames = ni_mib.tx_frame_128to255_cnt;
            data->tx_256_511_frames = ni_mib.tx_frame_256to511_cnt;
            data->tx_512_1023_frames = ni_mib.tx_frame_512to1023_cnt;
            data->tx_1024_1518_frames = ni_mib.tx_frame_1024to1518_cnt;
            data->tx_1519_max_frames = ni_mib.tx_frame_1519to2100_cnt +
                ni_mib.tx_frame_2101to9200_cnt + ni_mib.tx_frame_9201tomax_cnt;

    return ret;

}

static ca_status_t __xge_stat_get(ca_device_id_t device_id,
                 ca_uint32_t aal_port,
                 ca_boolean_t read_keep,
                 ca_eth_port_stat_t * data)
{
    ca_status_t ret = CA_E_OK;
    aal_ni_mib_stats_t ni_mib;

    memset(&ni_mib, 0, sizeof(aal_ni_mib_stats_t));

#if defined(CONFIG_ARCH_CORTINA_G3)
    if (AAL_NI_PORT_XGE0 == aal_port) {/* XGE */
        aal_ni_xaui_mac_ctrl_tx_mib_t  xaui_tx_mib;
        aal_ni_xaui_mac_ctrl_rx_mib_t  xaui_rx_mib;

        memset(&xaui_tx_mib, 0, sizeof(aal_ni_xaui_mac_ctrl_tx_mib_t));
        memset(&xaui_tx_mib, 0, sizeof(aal_ni_xaui_mac_ctrl_rx_mib_t));

        ret = aal_ni_xaui_mac_ctrl_tx_mib_get(device_id, &xaui_tx_mib);
        CA_RET_VALUE_CHECK(ret);

        ret = aal_ni_xaui_mac_ctrl_rx_mib_get(device_id, &xaui_rx_mib);
        CA_RET_VALUE_CHECK(ret);

        data->rx_good_octets      = ((ca_uint64_t)xaui_rx_mib.byte_hi << 32)
                                    + (ca_uint64_t)xaui_rx_mib.byte_lo;
        data->rx_uc_frames        = xaui_rx_mib.uc_cnt;
        data->rx_mc_frames        = xaui_rx_mib.mc_cnt;
        data->rx_bc_frames        = xaui_rx_mib.bc_cnt;
        data->rx_fcs_error_frames = xaui_rx_mib.crc_err_cnt;
        data->rx_pause_frames     = xaui_rx_mib.pause_cnt;
        data->rx_undersize_frames = xaui_rx_mib.undersize_cnt;
        data->rx_gaint_frames     = xaui_rx_mib.oversize_cnt;
        data->rx_64_frames        = xaui_rx_mib.rmon64_cnt ;
        data->rx_65_127_frames    = xaui_rx_mib.rmon127_cnt;
        data->rx_128_255_frames   = xaui_rx_mib.rmon128_255_cnt ;
        data->rx_256_511_frames   = xaui_rx_mib.rmon256_511_cnt ;
        data->rx_512_1023_frames  = xaui_rx_mib.rmon512_1023_cnt ;
        data->rx_1024_1518_frames = xaui_rx_mib.rmon1024_1518_cnt;
        data->rx_1519_max_frames  = xaui_rx_mib.rmon1519_max_cnt;
        data->tx_good_octets      = ((ca_uint64_t)xaui_tx_mib.byte_hi << 32)
                                    + (ca_uint64_t)xaui_tx_mib.byte_lo;
        data->tx_uc_frames        = xaui_tx_mib.uc_cnt;
        data->tx_mc_frames        = xaui_tx_mib.mc_cnt;
        data->tx_bc_frames        = xaui_tx_mib.bc_cnt;
        data->tx_pause_frames     = xaui_tx_mib.pause_cnt;
        data->tx_64_frames        = xaui_tx_mib.rmon64_cnt ;
        data->tx_65_127_frames    = xaui_tx_mib.rmon127_cnt;
        data->tx_128_255_frames   = xaui_tx_mib.rmon128_255_cnt ;
        data->tx_256_511_frames   = xaui_tx_mib.rmon256_511_cnt ;
        data->tx_512_1023_frames  = xaui_tx_mib.rmon512_1023_cnt ;
        data->tx_1024_1518_frames = xaui_tx_mib.rmon1024_1518_cnt;
        data->tx_1519_max_frames  = xaui_tx_mib.rmon1519_max_cnt;

        return ret;
    }
    else if (aal_port >= AAL_NI_PORT_XGE1) {

        ret = aal_ni_xge_mac_mib_get(device_id,
                aal_port - AAL_NI_PORT_XGE1, &ni_mib);
        CA_RET_VALUE_CHECK(ret);
    } else {
        return CA_E_PARAM;
    }

#elif defined(CONFIG_ARCH_CORTINA_G3HGU)
    if (aal_port >= AAL_NI_PORT_XGE1) {

        ret = aal_ni_xge_mac_mib_get(1,
                aal_port - AAL_NI_PORT_XGE1, &ni_mib);
        CA_RET_VALUE_CHECK(ret);
    } else {
        return CA_E_PARAM;
    }
#elif defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    if (aal_port >= AAL_NI_PORT_XGE0) {

        ret = aal_ni_xge_mac_mib_get(1,
                aal_port - AAL_NI_PORT_XGE0, &ni_mib);
        CA_RET_VALUE_CHECK(ret);

    } else {
        return CA_E_PARAM;
    }
#endif
    data->rx_good_octets = ((ca_uint64_t)ni_mib.rx_byte_cnt_high << 32)
                    + (ca_uint64_t)ni_mib.rx_byte_cnt_low;
    data->rx_uc_frames = ni_mib.rx_uc_pkt_cnt;
    data->rx_mc_frames = ni_mib.rx_mc_frame_cnt;
    data->rx_bc_frames = ni_mib.rx_bc_frame_cnt;
    data->rx_fcs_error_frames = ni_mib.rx_crc_error_frame_cnt;
    data->rx_pause_frames = ni_mib.rx_pause_frame_cnt;
    data->rx_undersize_frames = ni_mib.rx_undersize_frame_cnt;
    data->rx_runt_frames = ni_mib.rx_runt_frame_cnt;
    data->rx_gaint_frames = ni_mib.rx_ovsize_frame_cnt;
    data->rx_64_frames = ni_mib.rx_frame_64_cnt;
    data->rx_65_127_frames = ni_mib.rx_frame_65to127_cnt;
    data->rx_128_255_frames = ni_mib.rx_frame_128to255_cnt;
    data->rx_256_511_frames = ni_mib.rx_frame_256to511_cnt;
    data->rx_512_1023_frames = ni_mib.rx_frame_512to1023_cnt;
    data->rx_1024_1518_frames = ni_mib.rx_frame_1024to1518_cnt;
    data->rx_1519_max_frames = ni_mib.rx_frame_1519to2100_cnt +
        ni_mib.rx_frame_2101to9200_cnt + ni_mib.rx_frame_9201tomax_cnt;
    data->tx_good_octets = ((ca_uint64_t)ni_mib.tx_byte_cnt_high << 32)
                    + (ca_uint64_t)ni_mib.tx_byte_cnt_low;
    data->tx_uc_frames = ni_mib.tx_uc_pkt_cnt;
    data->tx_mc_frames = ni_mib.tx_mc_frame_cnt;
    data->tx_bc_frames = ni_mib.tx_bc_frame_cnt;
    data->tx_pause_frames = ni_mib.tx_pause_frame_cnt;
    data->tx_64_frames = ni_mib.tx_frame_64_cnt;
    data->tx_65_127_frames = ni_mib.tx_frame_65to127_cnt;
    data->tx_128_255_frames = ni_mib.tx_frame_128to255_cnt;
    data->tx_256_511_frames = ni_mib.tx_frame_256to511_cnt;
    data->tx_512_1023_frames = ni_mib.tx_frame_512to1023_cnt;
    data->tx_1024_1518_frames = ni_mib.tx_frame_1024to1518_cnt;
    data->tx_1519_max_frames = ni_mib.tx_frame_1519to2100_cnt +
        ni_mib.tx_frame_2101to9200_cnt + ni_mib.tx_frame_9201tomax_cnt;

    return ret;
}

ca_status_t ca_eth_port_stat_get(CA_IN ca_device_id_t device_id,
                 CA_IN ca_port_id_t port_id,
                 CA_IN ca_boolean_t read_keep,
                 CA_OUT ca_eth_port_stat_t * data)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x read_keep = %u data = %p.\n",
        device_id, port_id, read_keep, data);

    CA_ASSERT_RET(data != NULL, CA_E_PARAM);
    CA_PORT_ETHERNET_CHECK(port_id);
    CA_BOOL_VAL_CHECK(read_keep);

    __ETH_PORT_LOCK(device_id);

    if (__ETH_IS_XGE(aal_port)) {/* XGE */
        ret = __xge_stat_get(device_id, aal_port, read_keep, data);
    } else {
        ret = __eth_stat_get(device_id, aal_port, read_keep, data);
    }

#if 0
    if (AAL_NI_PORT_XGE0 == aal_port) {/* XGE */
        aal_ni_xaui_mac_ctrl_tx_mib_t  xaui_tx_mib;
        aal_ni_xaui_mac_ctrl_rx_mib_t  xaui_rx_mib;

        memset(&xaui_tx_mib, 0, sizeof(aal_ni_xaui_mac_ctrl_tx_mib_t));
        memset(&xaui_tx_mib, 0, sizeof(aal_ni_xaui_mac_ctrl_rx_mib_t));

        ret = aal_ni_xaui_mac_ctrl_tx_mib_get(device_id, &xaui_tx_mib);
        if (CA_E_OK != ret)
            goto __api_exit;

        ret = aal_ni_xaui_mac_ctrl_rx_mib_get(device_id, &xaui_rx_mib);
        if (CA_E_OK == ret) {
           data->rx_good_octets      = ((ca_uint64_t)xaui_rx_mib.byte_hi << 32)
                                        + (ca_uint64_t)xaui_rx_mib.byte_lo;
           data->rx_uc_frames        = xaui_rx_mib.uc_cnt;
           data->rx_mc_frames        = xaui_rx_mib.mc_cnt;
           data->rx_bc_frames        = xaui_rx_mib.bc_cnt;
           data->rx_fcs_error_frames = xaui_rx_mib.crc_err_cnt;
           data->rx_pause_frames     = xaui_rx_mib.pause_cnt;
           data->rx_undersize_frames = xaui_rx_mib.undersize_cnt;
           data->rx_gaint_frames     = xaui_rx_mib.oversize_cnt;
           data->rx_64_frames        = xaui_rx_mib.rmon64_cnt ;
           data->rx_65_127_frames    = xaui_rx_mib.rmon127_cnt;
           data->rx_128_255_frames   = xaui_rx_mib.rmon128_255_cnt ;
           data->rx_256_511_frames   = xaui_rx_mib.rmon256_511_cnt ;
           data->rx_512_1023_frames  = xaui_rx_mib.rmon512_1023_cnt ;
           data->rx_1024_1518_frames = xaui_rx_mib.rmon1024_1518_cnt;
           data->rx_1519_max_frames  = xaui_rx_mib.rmon1519_max_cnt;
           data->tx_good_octets      = ((ca_uint64_t)xaui_tx_mib.byte_hi << 32)
                                        + (ca_uint64_t)xaui_tx_mib.byte_lo;
           data->tx_uc_frames        = xaui_tx_mib.uc_cnt;
           data->tx_mc_frames        = xaui_tx_mib.mc_cnt;
           data->tx_bc_frames        = xaui_tx_mib.bc_cnt;
           data->tx_pause_frames     = xaui_tx_mib.pause_cnt;
           data->tx_64_frames        = xaui_tx_mib.rmon64_cnt ;
           data->tx_65_127_frames    = xaui_tx_mib.rmon127_cnt;
           data->tx_128_255_frames   = xaui_tx_mib.rmon128_255_cnt ;
           data->tx_256_511_frames   = xaui_tx_mib.rmon256_511_cnt ;
           data->tx_512_1023_frames  = xaui_tx_mib.rmon512_1023_cnt ;
           data->tx_1024_1518_frames = xaui_tx_mib.rmon1024_1518_cnt;
           data->tx_1519_max_frames  = xaui_tx_mib.rmon1519_max_cnt;
        }
    }
    else {
        aal_ni_mib_stats_t ni_mib;

        memset(&ni_mib, 0, sizeof(aal_ni_mib_stats_t));

        if (aal_port >= AAL_NI_PORT_XGE1) {
            ret = aal_ni_xge_mac_mib_get(device_id,
                          aal_port - AAL_NI_PORT_XGE1, &ni_mib);

        } else {
            ret = aal_ni_eth_port_mib_get(device_id,
                          aal_port, !read_keep, &ni_mib);
        }
        if (CA_E_OK == ret) {
            data->rx_good_octets = ((ca_uint64_t)ni_mib.rx_byte_cnt_high << 32)
                            + (ca_uint64_t)ni_mib.rx_byte_cnt_low;
            data->rx_uc_frames = ni_mib.rx_uc_pkt_cnt;
            data->rx_mc_frames = ni_mib.rx_mc_frame_cnt;
            data->rx_bc_frames = ni_mib.rx_bc_frame_cnt;
            data->rx_fcs_error_frames = ni_mib.rx_crc_error_frame_cnt;
            data->rx_pause_frames = ni_mib.rx_pause_frame_cnt;
            data->rx_undersize_frames = ni_mib.rx_undersize_frame_cnt;
            data->rx_runt_frames = ni_mib.rx_runt_frame_cnt;
            data->rx_gaint_frames = ni_mib.rx_ovsize_frame_cnt;
            data->rx_64_frames = ni_mib.rx_frame_64_cnt;
            data->rx_65_127_frames = ni_mib.rx_frame_65to127_cnt;
            data->rx_128_255_frames = ni_mib.rx_frame_128to255_cnt;
            data->rx_256_511_frames = ni_mib.rx_frame_256to511_cnt;
            data->rx_512_1023_frames = ni_mib.rx_frame_512to1023_cnt;
            data->rx_1024_1518_frames = ni_mib.rx_frame_1024to1518_cnt;
            data->rx_1519_max_frames = ni_mib.rx_frame_1519to2100_cnt +
                ni_mib.rx_frame_2101to9200_cnt + ni_mib.rx_frame_9201tomax_cnt;
            data->tx_good_octets = ((ca_uint64_t)ni_mib.tx_byte_cnt_high << 32)
                            + (ca_uint64_t)ni_mib.tx_byte_cnt_low;
            data->tx_uc_frames = ni_mib.tx_uc_pkt_cnt;
            data->tx_mc_frames = ni_mib.tx_mc_frame_cnt;
            data->tx_bc_frames = ni_mib.tx_bc_frame_cnt;
            data->tx_pause_frames = ni_mib.tx_pause_frame_cnt;
            data->tx_64_frames = ni_mib.tx_frame_64_cnt;
            data->tx_65_127_frames = ni_mib.tx_frame_65to127_cnt;
            data->tx_128_255_frames = ni_mib.tx_frame_128to255_cnt;
            data->tx_256_511_frames = ni_mib.tx_frame_256to511_cnt;
            data->tx_512_1023_frames = ni_mib.tx_frame_512to1023_cnt;
            data->tx_1024_1518_frames = ni_mib.tx_frame_1024to1518_cnt;
            data->tx_1519_max_frames = ni_mib.tx_frame_1519to2100_cnt +
                ni_mib.tx_frame_2101to9200_cnt + ni_mib.tx_frame_9201tomax_cnt;
        }

    }

__api_exit:
#endif

    __ETH_PORT_UNLOCK(device_id);

    return ret;

}

ca_status_t ca_eth_port_autoneg_set(CA_IN ca_device_id_t device_id,
                    CA_IN ca_port_id_t port_id,
                    CA_IN ca_boolean_t enable)
{
    ca_boolean_t prev_cfg = FALSE;
    ca_status_t  ret = CA_E_OK;
    ca_uint32_t  aal_port = PORT_ID(port_id);


    __ETH_DBG("device_id = %u port_id = %#x enable = %u.\n",
        device_id, port_id, enable);

    CA_PORT_ETHERNET_CHECK(port_id);
    CA_BOOL_VAL_CHECK(enable);

#ifndef CONFIG_CORTINA_BOARD_FPGA
    if (__ETH_IS_XGE(aal_port)) {/* XGE */

        aal_phy_sxgmii_pcs_control_index_t pcs_index;
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
        if (aal_port == CA_PORT_ID_NI5)
            pcs_index = AAL_SXGMII_PCS_CONTROL_INDEX_0;
        else if (aal_port == CA_PORT_ID_NI6)
            pcs_index = AAL_SXGMII_PCS_CONTROL_INDEX_1;
        else
            return CA_E_NOT_SUPPORT;
#else
        if (aal_port == CA_PORT_ID_NI6)
            pcs_index = AAL_SXGMII_PCS_CONTROL_INDEX_0;
        else if (aal_port == CA_PORT_ID_NI7)
            pcs_index = AAL_SXGMII_PCS_CONTROL_INDEX_1;
        else
            return CA_E_NOT_SUPPORT;
#endif

#if defined(CONFIG_CA_NE_AAL_XFI)
        if (enable)
            ret= aal_xfi_do_hw_autoneg(device_id, pcs_index);
        CA_RET_VALUE_CHECK(ret);
        g_eth_port_drv.autoneg[aal_port] = enable;
        return ret;
#endif
    }
#endif /* CONFIG_CORTINA_BOARD_FPGA */

    __ETH_PORT_LOCK(device_id);

    ret = aal_eth_phy_auto_neg_get(device_id,
                       __ETH_PHY_ADDR(aal_port), &prev_cfg);

    if (CA_E_OK == ret) {
        if (prev_cfg && enable) {
            ret = aal_eth_phy_auto_neg_restart(device_id,
                               __ETH_PHY_ADDR(aal_port));
        } else {
            ret = aal_eth_phy_auto_neg_set(device_id,
                               __ETH_PHY_ADDR(aal_port), enable);
        }
    }

    __ETH_PORT_UNLOCK(device_id);

    return ret;

}

ca_status_t ca_eth_port_autoneg_get(CA_IN ca_device_id_t device_id,
                    CA_IN ca_port_id_t port_id,
                    CA_OUT ca_boolean_t * enable)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t  aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x enable = %p.\n",
        device_id, port_id, enable);

    CA_PORT_ETHERNET_CHECK(port_id);
    CA_ASSERT_RET(enable != NULL, CA_E_PARAM);

    if (__ETH_IS_XGE(aal_port)) {/* XGE */
        __ETH_DBG("XGE port cannot support autoneg now.\n");
        *enable = g_eth_port_drv.autoneg[aal_port];
        return CA_E_OK;
    }

    __ETH_PORT_LOCK(device_id);

    ret = aal_eth_phy_auto_neg_get(device_id,
                       __ETH_PHY_ADDR(aal_port), enable);
    __ETH_PORT_UNLOCK(device_id);

    return ret;
}

ca_status_t ca_eth_port_advert_set(CA_IN ca_device_id_t device_id,
                   CA_IN ca_port_id_t port_id,
                   CA_IN ca_eth_port_ability_t ability)
{
    ca_status_t ret = CA_E_OK;
    aal_phy_autoneg_adv_t adv;
    ca_uint32_t  aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x "\
        "ability.half_10 = %u "\
        "ability.full_10 = %u "\
        "ability.half_100 = %u "\
        "ability.full_100 = %u "\
        "ability.full_1g = %u "\
        "ability.full_10g = %u "\
        "ability.flow_ctrl = %u "\
        "ability.asym_flow_ctrl = %u .\n",
        device_id, port_id,
        ability.half_10,
        ability.full_10,
        ability.half_100,
        ability.full_100,
        ability.full_1g,
        ability.full_10g,
        ability.flow_ctrl,
        ability.asym_flow_ctrl);

    CA_PORT_ETHERNET_CHECK(port_id);

    if (__ETH_IS_XGE(aal_port)) {/* XGE */
        __ETH_DBG("XGE port cannot support autoneg now.\n");
        return CA_E_NOT_SUPPORT;
    }

    memset(&adv, 0, sizeof(adv));

    if (ability.full_10g) {
        __ETH_DBG("GE port cannot support 10G ability now.\n");
        return CA_E_PARAM;
    }

    adv.adv_10base_t_half   = ability.half_10;
    adv.adv_10base_t_full   = ability.full_10;
    adv.adv_100base_tx_half = ability.half_100;
    adv.adv_100base_tx_full = ability.full_100;
    adv.adv_1000base_t_full = ability.full_1g;
    adv.adv_pause           = ability.flow_ctrl;
    adv.adv_asym_pause      = ability.asym_flow_ctrl;

    __ETH_PORT_LOCK(device_id);
    ret = aal_eth_phy_ability_adv_set(device_id,
                      __ETH_PHY_ADDR(aal_port), adv);
    __ETH_PORT_UNLOCK(device_id);

    return CA_E_OK;
}

ca_status_t ca_eth_port_advert_get(CA_IN ca_device_id_t device_id,
                   CA_IN ca_port_id_t port_id,
                   CA_OUT ca_eth_port_ability_t * ability)
{
    ca_status_t ret = CA_E_OK;
    aal_phy_autoneg_adv_t adv;
    ca_uint32_t  aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x ability = %p.\n",
        device_id, port_id, ability);

    CA_PORT_ETHERNET_CHECK(port_id);
    CA_ASSERT_RET(ability != NULL, CA_E_PARAM);

    if (__ETH_IS_XGE(aal_port)) {/* XGE */
        __ETH_DBG("XGE port cannot support autoneg now.\n");
        return CA_E_NOT_SUPPORT;
    }

    memset(&adv, 0, sizeof(adv));

    __ETH_PORT_LOCK(device_id);

    ret = aal_eth_phy_ability_adv_get(device_id,
                      __ETH_PHY_ADDR(aal_port), &adv);

    __ETH_PORT_UNLOCK(device_id);

    ability->half_10        = adv.adv_10base_t_half;
    ability->full_10        = adv.adv_10base_t_full;
    ability->half_100       = adv.adv_100base_tx_half;
    ability->full_100       = adv.adv_100base_tx_full;
    ability->full_1g        = adv.adv_1000base_t_full;
    ability->full_10g       = 0;
    ability->flow_ctrl      = adv.adv_pause;
    ability->asym_flow_ctrl = adv.adv_asym_pause;

    return ret;

}

ca_status_t ca_eth_port_advert_remote_get(CA_IN ca_device_id_t device_id,
                      CA_IN ca_port_id_t port_id,
                      CA_OUT ca_eth_port_ability_t *ability)
{
    ca_status_t ret = CA_E_OK;
    aal_phy_autoneg_adv_t adv;
    ca_uint32_t  aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x ability = %p.\n",
        device_id, port_id, ability);

    CA_PORT_ETHERNET_CHECK(port_id);
    CA_ASSERT_RET(ability != NULL, CA_E_PARAM);

    if (__ETH_IS_XGE(aal_port)) {/* XGE */
        __ETH_DBG("XGE port cannot support autoneg now.\n");
        return CA_E_NOT_SUPPORT;
    }

    memset(&adv, 0, sizeof(adv));

    __ETH_PORT_LOCK(device_id);
    ret = aal_eth_phy_partner_ability_adv_get(device_id,
                          __ETH_PHY_ADDR(aal_port),
                          &adv);
    __ETH_PORT_UNLOCK(device_id);

    ability->half_10        = adv.adv_10base_t_half;
    ability->full_10        = adv.adv_10base_t_full;
    ability->half_100       = adv.adv_100base_tx_half;
    ability->full_100       = adv.adv_100base_tx_full;
    ability->full_1g        = adv.adv_1000base_t_half;
    ability->full_10g       = 0;
    ability->flow_ctrl      = adv.adv_pause;
    ability->asym_flow_ctrl = adv.adv_asym_pause;

    return ret;

}

#define _SPEED2PHY(speed)     (aal_phy_speed_mode_t)((speed)-1)
#define _PHY2SPEED(phy_speed) (ca_eth_port_speed_t)((phy_speed)+1)

ca_status_t ca_eth_port_speed_set(CA_IN ca_device_id_t device_id,
                  CA_IN ca_port_id_t port_id,
                  CA_IN ca_eth_port_speed_t speed)
{
    ca_status_t ret = CA_E_OK;
    aal_ni_eth_mac_config_mask_t eth_mac_mask;
    aal_ni_eth_mac_config_t eth_mac_config;
    ca_uint32_t aal_port = PORT_ID(port_id);
    ca_boolean_t autoneg_ena = FALSE;
    aal_phy_speed_mode_t aal_speed = 0;

    __ETH_DBG("device_id = %u port_id = %#x speed = %u.\n",
        device_id, port_id, speed);

    CA_ASSERT_RET(speed > CA_PORT_SPEED_INVALID
              && speed <= CA_PORT_SPEED_10G, CA_E_PARAM);

    CA_PORT_ETHERNET_CHECK(port_id);

#ifndef CONFIG_CORTINA_BOARD_FPGA
    if (__ETH_IS_XGE(aal_port)) {/* XGE */

    aal_phy_sxgmii_pcs_control_index_t pcs_index;
    aal_phy_speed_mode_t pcs_speed;

    if (aal_port == CA_PORT_ID_NI6)
        pcs_index = AAL_SXGMII_PCS_CONTROL_INDEX_0;
    else if (aal_port == CA_PORT_ID_NI7)
        pcs_index = AAL_SXGMII_PCS_CONTROL_INDEX_1;
    else
        return CA_E_NOT_SUPPORT;

    if (speed == CA_PORT_SPEED_1G)
        pcs_speed = AAL_PHY_SPEED_1000;
    else if (speed == CA_PORT_SPEED_10G)
        pcs_speed = AAL_PHY_SPEED_10000;
    else
        return CA_E_NOT_SUPPORT;
#if defined(CONFIG_CA_NE_AAL_XFI)
    return aal_xfi_speed_set(device_id, pcs_index, pcs_speed);
#endif
    }
#endif /* CONFIG_CORTINA_BOARD_FPGA */

    __ETH_PORT_LOCK(device_id);

    ret = aal_eth_phy_auto_neg_get(device_id,
                 __ETH_PHY_ADDR(aal_port), &autoneg_ena);
    if (CA_E_OK != ret)
        goto __end;

    if (autoneg_ena) {
        aal_eth_phy_auto_neg_set(device_id,
                 __ETH_PHY_ADDR(aal_port), FALSE);
    }

    ret = aal_eth_phy_speed_get(device_id, __ETH_PHY_ADDR(aal_port), &aal_speed);
    if (CA_E_OK != ret)
        goto __end;

    if (aal_speed == _SPEED2PHY(speed)) { /* not change config */
        goto __end;
    }

    eth_mac_mask.u32 = 0;
    eth_mac_mask.s.speed = 1;
    memset(&eth_mac_config, 0, sizeof(eth_mac_config));

    /* ONLY impact 10M/100M, for 1G, always set to be 100M */
    eth_mac_config.speed = (speed == CA_PORT_SPEED_10M)
            ? AAL_NI_MAC_SPEED_10M : AAL_NI_MAC_SPEED_100M;

    ret = aal_ni_eth_port_mac_set(device_id, aal_port,
                          eth_mac_mask, &eth_mac_config);

    if (CA_E_OK == ret) {
        ret = aal_eth_phy_speed_set(device_id,
                    __ETH_PHY_ADDR(aal_port),
                    _SPEED2PHY(speed));
    }

__end:

    __ETH_PORT_UNLOCK(device_id);

    return ret;

}

ca_status_t ca_eth_port_speed_get(CA_IN ca_device_id_t device_id,
                  CA_IN ca_port_id_t port_id,
                  CA_OUT ca_eth_port_speed_t * speed)
{
    ca_status_t ret = CA_E_OK;
    aal_phy_link_status_t phy_lnk;
    ca_uint32_t aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x speed = %p.\n",
        device_id, port_id, speed);

    CA_ASSERT_RET(speed != NULL, CA_E_PARAM);

    CA_PORT_ETHERNET_CHECK(port_id);
    if (__ETH_IS_XGE(aal_port)) {/* XGE */
        *speed = CA_PORT_SPEED_10G;
        return CA_E_OK;
    }

    __ETH_PORT_LOCK(device_id);

    ret = aal_eth_phy_link_status_get(device_id,
         __ETH_PHY_ADDR(aal_port),
         &phy_lnk);

    __ETH_PORT_UNLOCK(device_id);

    if (CA_E_OK == ret) {

        if (phy_lnk.speed < AAL_PHY_SPEED_10000) {
            *speed = _PHY2SPEED(phy_lnk.speed);
        } else {
            *speed = CA_PORT_SPEED_INVALID;
        }
    }

    return ret;

}

ca_status_t ca_eth_port_duplex_set(CA_IN ca_device_id_t device_id,
                   CA_IN ca_port_id_t port_id,
                   CA_IN ca_eth_port_duplex_t duplex)
{
    ca_status_t ret = CA_E_OK;
    aal_ni_eth_mac_config_mask_t eth_mac_mask;
    aal_ni_eth_mac_config_t eth_mac_config;
    ca_uint32_t aal_port = PORT_ID(port_id);
    ca_boolean_t is_full = 0;
    ca_boolean_t autoneg_ena = FALSE;

    __ETH_DBG("device_id = %u port_id = %#x duplex = %u.\n",
        device_id, port_id, duplex);

    CA_PORT_ETHERNET_CHECK(port_id);
    CA_ASSERT_RET(duplex < CA_PORT_DUPLEX_AUTO, CA_E_PARAM);

    if (__ETH_IS_XGE(aal_port)) {/* XGE */

        if (duplex != CA_PORT_DUPLEX_FULL)
            return CA_E_NOT_SUPPORT;
        else
            return CA_E_OK;
    }

    __ETH_PORT_LOCK(device_id);

    ret = aal_eth_phy_auto_neg_get(device_id,
                 __ETH_PHY_ADDR(aal_port), &autoneg_ena);
    if (CA_E_OK != ret)
        goto __end;

    if (autoneg_ena) {
        aal_eth_phy_auto_neg_set(device_id,
                 __ETH_PHY_ADDR(aal_port), FALSE);
    }

    ret = aal_eth_phy_duplex_get(device_id,
                 __ETH_PHY_ADDR(aal_port), &is_full);
    if (CA_E_OK != ret)
        goto __end;

    if (is_full == (ca_boolean_t)duplex) { /* not change config */
        goto __end;
    }

    eth_mac_mask.u32 = 0;
    eth_mac_mask.s.duplex = 1;
    memset(&eth_mac_config, 0, sizeof(eth_mac_config));

    eth_mac_config.duplex = duplex;
    ret = aal_ni_eth_port_mac_set(device_id, aal_port,
                      eth_mac_mask, &eth_mac_config);

    if (CA_E_OK == ret) {
        ret = aal_eth_phy_duplex_set(device_id,
                     __ETH_PHY_ADDR(aal_port),
                     CA_PORT_DUPLEX_FULL == duplex);
    }

__end:
    __ETH_PORT_UNLOCK(device_id);

    return ret;

}

ca_status_t ca_eth_port_duplex_get(CA_IN ca_device_id_t device_id,
                   CA_IN ca_port_id_t port_id,
                   CA_OUT ca_eth_port_duplex_t * duplex)
{
    ca_status_t ret = CA_E_OK;
    aal_phy_link_status_t phy_lnk;
    ca_uint32_t aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x duplex = %p.\n",
        device_id, port_id, duplex);

    CA_ASSERT_RET(duplex != NULL, CA_E_PARAM);
    CA_PORT_ETHERNET_CHECK(port_id);

    if (__ETH_IS_XGE(aal_port)) {/* XGE */
        *duplex = CA_PORT_DUPLEX_FULL;
        return CA_E_OK;
    }
    __ETH_PORT_LOCK(device_id);

    ret = aal_eth_phy_link_status_get(device_id,
                      __ETH_PHY_ADDR(aal_port), &phy_lnk);

    __ETH_PORT_UNLOCK(device_id);

    if (CA_E_OK == ret) {
        *duplex = (phy_lnk.duplex ? CA_PORT_DUPLEX_FULL : CA_PORT_DUPLEX_HALF);
    }

    return ret;

}

static ca_status_t _ge_pause_quanta_set(
    CA_IN ca_device_id_t       device_id,
    CA_IN ca_uint32_t       aal_port,
    CA_IN ca_uint16_t       pause_quanta)
{
    ca_status_t ret = CA_E_OK;
    ca_uint8_t  cos = 0;

    for (cos = 0; cos < 8; ++cos){
        ret = aal_ni_eth_pfc_quanta_set(device_id, aal_port, cos, pause_quanta);
        if (CA_E_OK != ret) {
            __ETH_DBG("aal_ni_eth_pfc_quanta_set port %d cos %d quanta %d fail\n",
                aal_port, cos, pause_quanta);
            return ret;
        }
    }

    return CA_E_OK;
}

static ca_status_t _xge_pause_quanta_set(
    CA_IN ca_device_id_t       device_id,
    CA_IN ca_uint32_t       aal_port,
    CA_IN ca_uint16_t       pause_quanta)
{
    ca_status_t                          ret = CA_E_OK;
    aal_ni_xaui_mac_ctrl_flowctrl_mask_t xaui_mask;
    aal_ni_xaui_mac_ctrl_flowctrl_t      xaui_cfg;

    if (AAL_NI_PORT_XGE0 == aal_port) {/* XGE0 */
        xaui_mask.u32 = 0;
        xaui_mask.s.qbb_pause_quanta0 = 1;
        xaui_mask.s.qbb_pause_quanta1 = 1;
        xaui_mask.s.qbb_pause_quanta2 = 1;
        xaui_mask.s.qbb_pause_quanta3 = 1;
        xaui_mask.s.qbb_pause_quanta4 = 1;
        xaui_mask.s.qbb_pause_quanta5 = 1;
        xaui_mask.s.qbb_pause_quanta6 = 1;
        xaui_mask.s.qbb_pause_quanta7 = 1;
        xaui_cfg.qbb_pause_quanta0 = pause_quanta;
        xaui_cfg.qbb_pause_quanta1 = pause_quanta;
        xaui_cfg.qbb_pause_quanta2 = pause_quanta;
        xaui_cfg.qbb_pause_quanta3 = pause_quanta;
        xaui_cfg.qbb_pause_quanta4 = pause_quanta;
        xaui_cfg.qbb_pause_quanta5 = pause_quanta;
        xaui_cfg.qbb_pause_quanta6 = pause_quanta;
        xaui_cfg.qbb_pause_quanta7 = pause_quanta;

        ret = aal_ni_xaui_mac_ctrl_flowctrl_set(device_id,
            xaui_mask, &xaui_cfg);

    } else if (aal_port <= AAL_NI_PORT_XGE2) { /* XGE1, XGE2 */
        ret = aal_ni_xge_pause_quanta_set(device_id,
            aal_port - AAL_NI_PORT_XGE1, pause_quanta);
    }

    return ret;
}

ca_status_t ca_eth_port_pause_quanta_set(
    CA_IN ca_device_id_t       device_id,
    CA_IN ca_port_id_t      port_id,
    CA_IN ca_uint16_t       pause_quanta
)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x pause_quanta = %u.\n",
        device_id, port_id, pause_quanta);

    CA_PORT_ETHERNET_CHECK(port_id);

    __ETH_PORT_LOCK(device_id);
    if (__ETH_IS_XGE(aal_port)) {/* XGE */
        _xge_pause_quanta_set(device_id, aal_port, pause_quanta);
    }
    else { /* GE/FE */
        _ge_pause_quanta_set(device_id, aal_port, pause_quanta);
    }

    __ETH_PORT_UNLOCK(device_id);

    return ret;

}

ca_status_t ca_eth_port_pause_quanta_get(
    CA_IN  ca_device_id_t      device_id,
    CA_IN  ca_port_id_t     port_id,
    CA_OUT ca_uint16_t     *pause_quanta
)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x pause_quanta = %p.\n",
        device_id, port_id, pause_quanta);

    CA_PORT_ETHERNET_CHECK(port_id);
    CA_ASSERT_RET(pause_quanta != NULL, CA_E_PARAM);

    __ETH_PORT_LOCK(device_id);
    if (__ETH_IS_XGE(aal_port)) {/* XGE */
        if (AAL_NI_PORT_XGE0 == aal_port) {
            aal_ni_xaui_mac_ctrl_flowctrl_t  xaui_cfg;
            memset(&xaui_cfg, 0, sizeof(xaui_cfg));
            ret = aal_ni_xaui_mac_ctrl_flowctrl_get(device_id, &xaui_cfg);
            if (CA_E_OK == ret)
                *pause_quanta = xaui_cfg.qbb_pause_quanta0;
        } else { /* XGE1, XGE2 */
            ret = aal_ni_xge_pause_quanta_get(device_id,
                aal_port - AAL_NI_PORT_XGE1, pause_quanta);
        }
    }
    else { /* GE/FE */
        ret = aal_ni_eth_pfc_quanta_get(device_id, aal_port, 0, pause_quanta);
    }

    __ETH_PORT_UNLOCK(device_id);

    __ETH_DBG("RETURN device_id = %u port_id = %#x pause_quanta = %d.\n",
        device_id, port_id, *pause_quanta);

    return ret;

}


ca_status_t ca_eth_port_mdix_set (
    CA_IN  ca_device_id_t          device_id,
    CA_IN  ca_port_id_t         port_id,
    CA_IN  ca_eth_port_mdix_t   mode)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t aal_port = PORT_ID(port_id);
    aal_phy_mdi_mode_t  old_mode = AAL_PHY_MDI_MDI;

    __ETH_DBG("device_id = %u port_id = %#x mode = %u.\n",
        device_id, port_id, mode);

    CA_PORT_ETHERNET_CHECK(port_id);
    CA_ASSERT_RET(mode <= CA_PORT_MDIX_AUTO, CA_E_PARAM);

    if (__ETH_IS_XGE(aal_port)) {
        if (mode != CA_PORT_MDIX_AUTO) {/* XGE */
            __ETH_DBG("XGE port not support force MDIX configuration!\n");
            return CA_E_NOT_SUPPORT;
        } else {
            return CA_E_OK;
        }
    }

    __ETH_PORT_LOCK(device_id);
    ret = aal_eth_phy_mdix_get(device_id, __ETH_PHY_ADDR(aal_port), &old_mode);

    if (CA_E_OK == ret && old_mode != (aal_phy_mdi_mode_t)mode) {
        ret = aal_eth_phy_mdix_set(device_id, __ETH_PHY_ADDR(aal_port), (aal_phy_mdi_mode_t)mode);
    }
    __ETH_PORT_UNLOCK(device_id);

    return ret;

}

ca_status_t ca_eth_port_mdix_get (
    CA_IN  ca_device_id_t          device_id,
    CA_IN  ca_port_id_t         port_id,
    CA_OUT ca_eth_port_mdix_t   *mode)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t aal_port = PORT_ID(port_id);

    __ETH_DBG("device_id = %u port_id = %#x mode = %p.\n",
        device_id, port_id, mode);

    CA_PORT_ETHERNET_CHECK(port_id);
    CA_ASSERT_RET(mode != NULL, CA_E_PARAM);

    if (__ETH_IS_XGE(aal_port)) {/* XGE */
        *mode = CA_PORT_MDIX_AUTO;
        return CA_E_OK;
    }

    __ETH_PORT_LOCK(device_id);
    ret = aal_eth_phy_mdix_get(device_id, __ETH_PHY_ADDR(aal_port), (aal_phy_mdi_mode_t *)mode);
    __ETH_PORT_UNLOCK(device_id);

    return ret;

}


ca_status_t ca_eth_ports_restore(CA_IN ca_device_id_t device_id)
{
    ca_port_id_t port_id = 0;
    ca_boolean_t enable = 0;
#if  defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    ca_port_id_t xge_start = AAL_NI_PORT_XGE1;
#elif defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    ca_port_id_t xge_start = AAL_NI_PORT_XGE0;
#else
    ca_port_id_t xge_start = 0xF;
#endif

    ca_status_t ret = CA_E_OK;
    ca_status_t ret1 = CA_E_OK;
    ca_status_t ret2 = CA_E_OK;
    aal_ni_eth_mac_config_mask_t eth_mac_mask;
    aal_ni_eth_mac_config_t eth_mac_config;
    aal_ni_xge_cmn_cfg_mask_t      xge_mask;
    aal_ni_xge_cmn_cfg_t           xge_config;
    ca_uint8_t uni_connect[CFG_ID_UNI_CONNECTION_LEN] = {0};

    ret = scfg_read( device_id, CFG_ID_UNI_CONNECTION, CFG_ID_UNI_CONNECTION_LEN, uni_connect);
    if (CA_E_OK != ret) {
        ca_printf("Load %s fail, ret = %d, so set UNI default to be enable\n", CFG_ID_UNI_CONNECTION, ret);
        memset(uni_connect, 0x01, CFG_ID_UNI_CONNECTION_LEN);
    }

    /* Only set rx, when tx is always enable to prevent buffer hang */
    eth_mac_mask.u32 = 0;
    eth_mac_mask.s.mac_rx_rst = 1;
    eth_mac_mask.s.rx_en = 1;
    memset(&eth_mac_config, 0, sizeof(eth_mac_config));

    xge_mask.u32 = 0;
    xge_mask.s.tx_rst = 1;
    xge_mask.s.rx_rst = 1;

    __FOR_ALL_ETH_PORTS_DO(port_id) {
        enable = uni_connect[port_id];
#ifdef __DEBUG__
        ca_printf("%s: port_id=%d enable = %d \n", __func__, port_id, enable);
#endif
        if (__ETH_IS_XGE(port_id)) {/* XGE */

#if defined(CONFIG_CA_NE_AAL_XFI)
            aal_xfi_cfg_mask_t   cfg_mask;
            aal_xfi_cfg_t        cfg;

            cfg_mask.data32 = 0;
            memset(&cfg, 0, sizeof(cfg));

            cfg_mask.s.pdown = 1;
            cfg_mask.s.rx_en = 1;
            cfg.pdown = !enable;
            cfg.rx_en = enable ? 1 : 0;
            ret1 = aal_xfi_cfg_set(device_id,
                port_id - xge_start,
                cfg_mask, &cfg);

#endif

#if  defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
            if (AAL_NI_PORT_XGE0 == port_id) {
                aal_ni_xaui_mac_tx_cfg_mask_t  xaui_mac_tx_mask;
                aal_ni_xaui_mac_tx_cfg_t       xaui_mac_tx;
                aal_ni_xaui_mac_rx_cfg_mask_t  xaui_mac_rx_mask;
                aal_ni_xaui_mac_rx_cfg_t       xaui_mac_rx;
                memset(&xaui_mac_tx, 0, sizeof(xaui_mac_tx));
                memset(&xaui_mac_rx, 0, sizeof(xaui_mac_rx));
                xaui_mac_tx_mask.u32 = 0;
                xaui_mac_rx_mask.u32 = 0;
                xaui_mac_tx_mask.s.mac_en = 1;
                xaui_mac_rx_mask.s.mac_en = 1;
                xaui_mac_tx.mac_en = enable;
                xaui_mac_rx.mac_en = enable;

                ret1 = aal_ni_xaui_mac_tx_config_set(device_id, xaui_mac_tx_mask, &xaui_mac_tx);
                ret2 = aal_ni_xaui_mac_rx_config_set(device_id, xaui_mac_rx_mask, &xaui_mac_rx);
            } else {
                ret1 = aal_ni_xge_cmn_config_set(device_id, port_id - xge_start, xge_mask, &xge_config);
            }
#elif defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
            xge_config.tx_rst  = !enable;
            xge_config.rx_rst  = !enable;
            ret1 = aal_ni_xge_cmn_config_set(device_id, port_id - xge_start, xge_mask, &xge_config);
#endif
        }
        else {
            /* Leave GPHY power on always */
            eth_mac_config.mac_rx_rst = !enable;
            eth_mac_config.rx_en = enable;

            ret2 = aal_ni_eth_port_mac_set(device_id, port_id,
                               eth_mac_mask, &eth_mac_config);
        }

        if (ret1 != CA_E_OK || ret2 != CA_E_OK) {
            ca_printf("%s: ret1=%d, ret2=%d\n", __func__, ret1, ret2);
        }
    }

    return CA_E_OK;

}

ca_status_t ca_eth_ports_enable(CA_IN ca_device_id_t device_id,
                CA_IN ca_boolean_t enable)
{
    ca_port_id_t port_id = 0;
    ca_status_t ret1 = CA_E_OK;
    ca_status_t ret2 = CA_E_OK;
    aal_ni_eth_mac_config_mask_t eth_mac_mask;
    aal_ni_eth_mac_config_t eth_mac_config;
    aal_ni_xaui_mac_tx_cfg_mask_t  xaui_mac_tx_mask;
    aal_ni_xaui_mac_tx_cfg_t       xaui_mac_tx;
    aal_ni_xaui_mac_rx_cfg_mask_t  xaui_mac_rx_mask;
    aal_ni_xaui_mac_rx_cfg_t       xaui_mac_rx;
    aal_ni_xge_cmn_cfg_mask_t      xge_mask;
    aal_ni_xge_cmn_cfg_t           xge_config;

    /* Only set rx, when tx is always enable to prevent buffer hang */
    eth_mac_mask.u32 = 0;
    eth_mac_mask.s.mac_rx_rst = 1;
    eth_mac_mask.s.rx_en = 1;

    memset(&eth_mac_config, 0, sizeof(eth_mac_config));
    eth_mac_config.mac_rx_rst = !enable;
    eth_mac_config.rx_en = enable;

    xaui_mac_tx_mask.u32 = 0;
    xaui_mac_rx_mask.u32 = 0;
    xge_mask.u32 = 0;

    memset(&xaui_mac_tx, 0, sizeof(xaui_mac_tx));
    memset(&xaui_mac_rx, 0, sizeof(xaui_mac_rx));

    xaui_mac_tx_mask.s.mac_en  = 1;
    xaui_mac_rx_mask.s.mac_en = 1;
    xge_mask.s.tx_rst = 1;
    xge_mask.s.rx_rst = 1;
    xaui_mac_tx.mac_en = enable;
    xaui_mac_rx.mac_en = enable;
    xge_config.tx_rst  = !enable;
    xge_config.rx_rst  = !enable;

    __FOR_ALL_ETH_PORTS_DO(port_id) { /* P0 ~ P7 */

#if CONFIG_98F_UBOOT_NE_DBG
        __ETH_INIT_PRINT("%s: port_id=%d enable = %d \n", __func__, port_id, enable);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        if (__ETH_IS_XGE(port_id)) { /* XGE (P5 ~ P7 of G3) */

#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
            ret1 = aal_ni_xge_cmn_config_set(device_id,
                port_id - AAL_NI_PORT_XGE0, xge_mask, &xge_config);

#else
            if (AAL_NI_PORT_XGE0 == port_id) {
                ret1 = aal_ni_xaui_mac_tx_config_set(device_id, xaui_mac_tx_mask, &xaui_mac_tx);

                ret2 = aal_ni_xaui_mac_rx_config_set(device_id, xaui_mac_rx_mask, &xaui_mac_rx);
            } else {
                ret1 = aal_ni_xge_cmn_config_set(device_id, port_id - AAL_NI_PORT_XGE1, xge_mask, &xge_config);
            }
#endif
        } /* XGE (P5~P7 of G3) */
        else { /* P0 ~ P4 */
        /* Leave GPHY power on always */
#if 0
            ret1 = aal_eth_phy_power_set(device_id,
                             __ETH_PHY_ADDR(port_id), enable);
#endif
            ret2 = aal_ni_eth_port_mac_set(device_id, port_id,
                               eth_mac_mask, &eth_mac_config);
        } /* P0 ~ P4 */

        if (ret1 != CA_E_OK || ret2 != CA_E_OK) {
            __ETH_INIT_PRINT("%s(%d): ret1=%d, ret2=%d\n", __func__, __LINE__, ret1, ret2);
        }
    } /* P0 ~ P7 */

    return CA_E_OK;

}

static inline ca_status_t __eth_phy_auto_detect(ca_device_id_t device_id)
{
    ca_status_t ret = CA_E_OK;
    ca_uint8_t addr = 0, phy_count = 0;

    for (addr = 1; addr <= CA_MDIO_ADDR_MAX; ++addr) {
        ret = aal_eth_phy_init(device_id, addr);

        if (CA_E_OK == ret)
            phy_count++;
    }

#if CONFIG_98F_UBOOT_NE_DBG
    __ETH_INIT_PRINT("Detect Ethernet PHY finish, find %d FE-PHY. \n", phy_count);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    if(phy_count != RTL8198F_FE_PHY_NUM){
        __ETH_INIT_PRINT("Detect FE-PHY number error: %d", phy_count);

        return CA_E_ERROR;
    }else{
        return CA_E_OK;
    }

}

static ca_status_t __eth_port_phy_init(ca_device_id_t device_id)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t port_id = 0;
    aal_phy_autoneg_adv_t  adv;

    __FOR_ALL_ETH_PORTS_DO(port_id) {
/* #if defined(CONFIG_ARCH_CORTINA_G3LITE) */
#if defined(CONFIG_RTL_83XX_SUPPORT)
        if (__ETH_IS_CONNECT_SWITCH(port_id)) { /* Connect to RTL switch chipset */
            continue;
        }
#endif
        aal_eth_phy_ability_adv_get(device_id, __ETH_PHY_ADDR(port_id), &adv);

        adv.adv_100base_tx_half = 1;
        adv.adv_100base_tx_full = 1;
        adv.adv_10base_t_half   = 1;
        adv.adv_10base_t_full   = 1;

#ifdef CONFIG_CORTINA_BOARD_FPGA
        adv.adv_1000base_t_half = 0;
        adv.adv_1000base_t_full = 0;
#else
        adv.adv_1000base_t_half = 1;
        adv.adv_1000base_t_full = 1;
#endif
        aal_eth_phy_ability_adv_set(device_id, __ETH_PHY_ADDR(port_id), adv);
        aal_eth_phy_duplex_set(device_id, __ETH_PHY_ADDR(port_id), TRUE); /* FULL */

#ifdef CONFIG_CORTINA_BOARD_FPGA
        aal_eth_phy_auto_neg_set(device_id, __ETH_PHY_ADDR(port_id), FALSE);
        aal_eth_phy_speed_set(device_id, __ETH_PHY_ADDR(port_id), AAL_PHY_SPEED_100);
#else
        aal_eth_phy_auto_neg_set(device_id, __ETH_PHY_ADDR(port_id), TRUE);
        aal_eth_phy_speed_set(device_id, __ETH_PHY_ADDR(port_id), AAL_PHY_SPEED_1000);
#endif
    }

#ifndef CONFIG_CORTINA_BOARD_FPGA /* ASIC internal GPHY init */
    aal_internal_phy_init(device_id);
#endif
#if defined(CONFIG_CA_NE_AAL_XFI)
    aal_xfi_init(device_id);
#endif

    return ret;
}

/*
 * ONLY used for ca_init invoking
 */
ca_status_t ca_eth_port_init(CA_IN ca_device_id_t device_id,
                 CA_IN ca_init_config_t * p_cfg)
{
    ca_status_t ret = CA_E_OK;

    /* Global data init */
    __eth_port_global_init(device_id);

    /* Disable all ports' Rx */
    ret = ca_eth_ports_enable(device_id, FALSE);
    if (CA_E_OK != ret) {
        __ETH_INIT_PRINT("ca_eth_ports_enable fail, return code %#x.\n", ret);
    }

#if 1//yocto
#else//sd1 uboot for 98f dbg
    /* ETH PHY init */
    __ETH_INIT_PRINT("ETH PHY init...%s(%d) %s\n", __func__, __LINE__, __FILE__);
#endif

    ret = __eth_phy_auto_detect(device_id);
    if (CA_E_OK != ret) {
        __ETH_INIT_PRINT("%s:%d __eth_phy_auto_detect() returns error, ret = %d.\n", __func__, __LINE__, ret);
        return ret;
    }

    __eth_port_phy_init(device_id);

    /* ETH MAC init */
    ret = __eth_port_mac_init(device_id);
    if (CA_E_OK != ret) {
        __ETH_INIT_PRINT("__eth_port_mac_init fail, return code %#x.\n", ret);
    }

    /* Misc functions init, includes polling timer, */
    ret = __eth_port_misc_init(device_id);
    if (CA_E_OK != ret) {
        __ETH_INIT_PRINT("__eth_port_misc_init fail, return code %#x.\n", ret);
    }

    return ret;

}

#ifdef __KERNEL__

EXPORT_SYMBOL(g_eth_port_debug);
EXPORT_SYMBOL(cap_mdio_write);
EXPORT_SYMBOL(cap_mdio_read);
EXPORT_SYMBOL(cap_mdio_speed_set);
EXPORT_SYMBOL(cap_mdio_speed_get);
EXPORT_SYMBOL(ca_eth_port_link_status_get);
EXPORT_SYMBOL(ca_eth_port_link_scan_get);
EXPORT_SYMBOL(ca_eth_port_link_scan_set);
EXPORT_SYMBOL(ca_eth_port_advert_remote_get);
EXPORT_SYMBOL(ca_eth_port_autoneg_set);
EXPORT_SYMBOL(ca_eth_port_autoneg_get);
EXPORT_SYMBOL(ca_eth_port_pause_set);
EXPORT_SYMBOL(ca_eth_port_pause_get);
EXPORT_SYMBOL(ca_eth_port_speed_set);
EXPORT_SYMBOL(ca_eth_port_speed_get);
EXPORT_SYMBOL(ca_eth_port_advert_get);
EXPORT_SYMBOL(ca_eth_port_advert_set);
EXPORT_SYMBOL(ca_eth_port_stat_get);
EXPORT_SYMBOL(ca_eth_port_duplex_set);
EXPORT_SYMBOL(ca_eth_port_duplex_get);
EXPORT_SYMBOL(ca_eth_port_pause_quanta_set);
EXPORT_SYMBOL(ca_eth_port_pause_quanta_get);
EXPORT_SYMBOL(ca_eth_port_mdix_set);
EXPORT_SYMBOL(ca_eth_port_mdix_get);
EXPORT_SYMBOL(ca_eth_port_exit);

#endif

