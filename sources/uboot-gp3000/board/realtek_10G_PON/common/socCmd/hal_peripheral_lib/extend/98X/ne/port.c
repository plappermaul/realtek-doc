#include <generated/ca_ne_autoconf.h>
#include "port.h"
#include "vlan.h"
#include "aal_port.h"
#include "aal_ni_l2.h"
#include "aal_ni.h"
#include "aal_phy.h"
#include "eth_port.h"
#include "aal_l2_tm.h"
#include "aal_l3_tm.h"
#include "osal_common.h"
#include "scfg.h"
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
#include "aal_xfi.h"
#endif
#ifdef CA_HAVE_EPON
#include "aal_epon_mac.h"
#endif

ca_uint __port_lock;

#define __PORT_LOCK(dev)           ca_spin_lock(__port_lock)
#define __PORT_UNLOCK(dev)         ca_spin_unlock(__port_lock)

typedef struct {
    ca_boolean_t valid;    /* created or not */
    ca_uint32_t active_members_num;
    ca_uint32_t active_members[CA_PORT_MAX_NUM_TRUNK_MEMBER];    /* the lpid of active members, used for trunk dest map table update */
    ca_uint64_t members_bmp;
} __port_lag_attr_t;

__port_lag_attr_t __port_lag_attr[CA_PORT_MAX_NUM_TRUNK];    /* array index 0 used for group0, and index 1 used for group1   */

ca_uint32_t __port_tpid_index[CA_PORT_MAX_NUM];     /* pvid tpid set */

ca_boolean_t __port_stp_mode[CA_PORT_MAX_NUM];       /* stp is disable or enable */

ca_uint32_t __port_ipg_value = 12; /* for NI5 XAUI port */

#if 1
/* Special packet behavior table index in ILPB Table */
#define __PORT_SPB_RSVD  0
#define __PORT_SPB_WAN   1
#define __PORT_SPB_LAN   2

#define __PORT_IS_LAN(port) (((port) < CA_PORT_MAX_NUM_ETHERNET) && ((port) != AAL_LPORT_ETH_WAN))
#define __PORT_IS_WAN(port) ((port) == AAL_LPORT_ETH_WAN)
#endif

/* get ca_port_id_t (port_type + port_index) based on lpid */
ca_port_id_t ca_port_id_get(ca_uint32_t lpid)
{
    ca_uint16_t port_type = CA_PORT_TYPE_INVALID;

    if (lpid >= CA_PORT_ID_NI0 && lpid <= CA_PORT_ID_NI7) {
        port_type = CA_PORT_TYPE_ETHERNET;

    }else if (lpid == CA_PORT_ID_L3_LAN || CA_PORT_ID_L3_WAN) {
        port_type = CA_PORT_TYPE_L2RP;

    } else if (lpid >= CA_PORT_ID_CPU0 && lpid <= CA_PORT_ID_CPU7) {
        port_type = CA_PORT_TYPE_CPU;

        //} else if (lpid >= CA_PORT_ID_GROUP0 && lpid <= CA_PORT_ID_GROUP1) {
        //port_type = CA_PORT_TYPE_TRUNK;

        //} else if (lpid == CA_PORT_ID_MC) {
        //port_type = CA_PORT_TYPE_MC;

    }

    return CA_PORT_ID(port_type, lpid);
}

/* only support ethernet ports enable/disable */
ca_status_t ca_port_enable_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_boolean_t enable)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t ppid = 0;
    aal_ni_eth_mac_config_mask_t ge_msk;
    aal_ni_eth_mac_config_t ge_cfg;
    aal_ni_xaui_mac_tx_cfg_mask_t xaui_tx_msk;
    aal_ni_xaui_mac_tx_cfg_t xaui_tx_cfg;
    aal_ni_xaui_mac_rx_cfg_mask_t xaui_rx_msk;
    aal_ni_xaui_mac_rx_cfg_t xaui_rx_cfg;
#ifdef CA_HAVE_EPON
	aal_epon_mac_ctrl_msk_t epon_msk;
	aal_epon_mac_ctrl_t epon_cfg;
#endif

    ca_uint8_t phy_addr[8] = {/* FPGA board has 8 Eth Ports */
        1, 2, 3, 4, 16, 17, 18, 19
    };
#ifndef CONFIG_CORTINA_BOARD_FPGA
    ca_device_config_tlv_t scfg_tlv;
#endif
    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ETHERNET_CHECK(port_id);
    CA_BOOL_VAL_CHECK(enable);

    ppid = PORT_ID(port_id);    /* for ethernet ports, the ppid is the same as lpid */

#ifndef CONFIG_CORTINA_BOARD_FPGA
    if (ppid <= AAL_LPORT_ETH_NI4) {
        /* Read PHY MDIO address from STARTUP config */
        memset(&scfg_tlv, 0, sizeof(scfg_tlv));
        scfg_tlv.type = CA_CFG_ID_PHY_BASE_ADDR;
        scfg_tlv.len = CFG_ID_PHY_BASE_ADDR_LEN;

        ret = ca_device_config_tlv_get(device_id, &scfg_tlv);
        if (CA_E_OK == ret) {
#ifdef _DEBUG_
            ca_printf("get phy addr from scfg \r\n");
#endif
            memcpy(phy_addr, scfg_tlv.value, CFG_ID_PHY_BASE_ADDR_LEN);
        }
    }
#endif

#if 0
    /* Leave GPHY in power on mode always */
    ca_printf("set port %d (phy addr 0x%x) power enable to %d\r\n", ppid, phy_addr[ppid], enable);
    ret = aal_eth_phy_power_set(device_id, phy_addr[ppid], enable);
    if (ret != CA_E_OK) {
        ca_printf("set port %d (phy addr 0x%x) power enable failed. ret = %d\r\n", ppid, phy_addr[ppid], ret);
        return ret;
    }
#endif

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    if(ppid >= AAL_LPORT_ETH_NI3 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
#else
    if(ppid >= AAL_LPORT_ETH_NI0 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
#endif
        memset(&ge_msk, 0, sizeof(aal_ni_eth_mac_config_mask_t));
        memset(&ge_cfg, 0, sizeof(aal_ni_eth_mac_config_t));

        ge_msk.s.mac_rx_rst = 1;
        ge_msk.s.rx_en = 1;
        ge_msk.s.tx_en = 1;
        ge_cfg.mac_rx_rst = !enable;
        ge_cfg.rx_en = enable;
        ge_cfg.tx_en = enable;

        ret = aal_ni_eth_port_mac_set(device_id, ppid, ge_msk, &ge_cfg);
        if (ret != CA_E_OK) {
            ca_printf("set port %d mac enable failed. ret = %d\r\n", ppid, ret);
            return ret;
        }
    }
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    else if (ppid >= AAL_LPORT_ETH_NI5 && ppid <= AAL_LPORT_ETH_NI6) {

        aal_xfi_cfg_mask_t   cfg_mask;
        aal_xfi_cfg_t        cfg;

        cfg_mask.data32 = 0;
        memset(&cfg, 0, sizeof(cfg));

        cfg_mask.s.pdown = 1;
        cfg_mask.s.rx_en = 1;

        cfg.pdown = !enable;
        cfg.rx_en = enable ? 1 : 0;
        //ca_printf("%s(): %d\n", __func__, __LINE__);
        ret = aal_xfi_cfg_set(device_id, ppid - AAL_LPORT_ETH_NI5,
            cfg_mask, &cfg);
        if (ret != CA_E_OK) {
            ca_printf("aal_xfi_cfg_set %d failed. ret = %d\r\n", ppid, ret);
            return ret;
        }
        else {
            ca_printf("XGE %d %s!\n", ppid, enable ? "enable" : "disable");
        }
    }else if(ppid == AAL_LPORT_ETH_NI7){ /* PON port */
#ifdef CA_HAVE_EPON
        epon_msk.u32 = 0;
		epon_msk.s.rx_xg_ena = 1;
		epon_msk.s.tx_xg_ena = 1;

		memset(&epon_cfg, 0, sizeof(aal_epon_mac_ctrl_t));
		epon_cfg.rx_xg_ena = enable;
		epon_cfg.tx_xg_ena = enable;

		return aal_epon_mac_cfg_set(device_id, port_id, epon_msk, &epon_cfg);
#endif
    }
#elif defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    else if(ppid == AAL_LPORT_ETH_NI5){    /* XAUI */
        memset(&xaui_tx_msk, 0, sizeof(aal_ni_xaui_mac_tx_cfg_mask_t));
        memset(&xaui_tx_cfg, 0, sizeof(aal_ni_xaui_mac_tx_cfg_t));

        xaui_tx_msk.s.mac_en  = 1;
        xaui_tx_cfg.mac_en = enable;
        ret = aal_ni_xaui_mac_tx_config_set(device_id, xaui_tx_msk, &xaui_tx_cfg);
        if (ret != CA_E_OK) {
            ca_printf("set port %d mac tx enable failed. ret = %d\r\n", ppid, ret);
            return ret;
        }
        memset(&xaui_rx_msk, 0, sizeof(aal_ni_xaui_mac_rx_cfg_mask_t));
        memset(&xaui_rx_cfg, 0, sizeof(aal_ni_xaui_mac_rx_cfg_t));

        xaui_rx_msk.s.mac_en = 1;
        xaui_rx_cfg.mac_en = enable;
        ret = aal_ni_xaui_mac_rx_config_set(device_id, xaui_rx_msk, &xaui_rx_cfg);
        if (ret != CA_E_OK) {
            ca_printf("set port %d mac rx enable failed. ret = %d\r\n", ppid, ret);
            return ret;
        }

    }else if(ppid == AAL_LPORT_ETH_NI6 || ppid == AAL_LPORT_ETH_NI7){ /* XGE */
        aal_ni_xge_cmn_cfg_mask_t   cfg_mask;
        aal_ni_xge_cmn_cfg_t        cfg;

        cfg_mask.u32 = 0;
        memset(&cfg, 0, sizeof(cfg));

        cfg_mask.s.rx_rst = 1;
        cfg_mask.s.tx_rst = 1;
        cfg.rx_rst = !enable;
        cfg.tx_rst = !enable;
        ret = aal_ni_xge_cmn_config_set(device_id, ppid - AAL_LPORT_ETH_NI6,
            cfg_mask, &cfg);
        if (ret != CA_E_OK) {
            ca_printf("set port %d mac enable failed. ret = %d\r\n", ppid, ret);
            return ret;
        }
    }
#endif

    return ret;
}

ca_status_t ca_port_enable_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_boolean_t * enable)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t ppid = 0;
    aal_ni_eth_mac_config_t ge_cfg;
    aal_ni_xaui_mac_tx_cfg_t xaui_tx_cfg;
    aal_ni_xaui_mac_rx_cfg_t xaui_rx_cfg;
    ca_boolean mac_en, mac_rx_en, mac_tx_en, phy_en;
#ifdef CA_HAVE_EPON
	aal_epon_mac_ctrl_t epon_cfg;
#endif

    ca_uint8_t phy_addr[8] = {/* FPGA board has 8 Eth Ports */
        1, 2, 3, 4, 16, 17, 18, 19
    };
#ifndef CONFIG_CORTINA_BOARD_FPGA
    ca_device_config_tlv_t scfg_tlv;
#endif
    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ETHERNET_CHECK(port_id);

    ppid = PORT_ID(port_id);    /* for ethernet ports, the ppid is the same as lpid */

#ifndef CONFIG_CORTINA_BOARD_FPGA
    /* Read PHY MDIO address from STARTUP config */
    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_PHY_BASE_ADDR;
    scfg_tlv.len = CFG_ID_PHY_BASE_ADDR_LEN;

    ret = ca_device_config_tlv_get(device_id, &scfg_tlv);
    if (CA_E_OK == ret) {
        ca_printf("get phy addr from scfg \r\n");
        memcpy(phy_addr, scfg_tlv.value, CFG_ID_PHY_BASE_ADDR_LEN);
    }
#endif
    ret = aal_eth_phy_power_get(device_id, phy_addr[ppid], &phy_en);
    if (ret != CA_E_OK) {
        ca_printf("get port %d (phy addr 0x%x) power enable failed. ret = %d\r\n", ppid, phy_addr[ppid], ret);
        return ret;
    }
    //ca_printf("get port %d (phy addr 0x%x) power enable: %d\r\n", ppid, phy_addr[ppid], phy_en);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    if(ppid >= AAL_LPORT_ETH_NI3 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
#else
    if(ppid >= AAL_LPORT_ETH_NI0 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
#endif
        memset(&ge_cfg, 0, sizeof(aal_ni_eth_mac_config_t));

        ret = aal_ni_eth_port_mac_get(device_id, ppid, &ge_cfg);
        if(ret != CA_E_OK){
            ca_printf("get port %d mac enable failed. ret = %d\r\n", ppid, ret);
            return ret;
        }
        mac_en = ge_cfg.rx_en;
    }
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    else if (ppid >= AAL_LPORT_ETH_NI5 && ppid <= AAL_LPORT_ETH_NI6) {
        aal_xfi_cfg_t        cfg;

        memset(&cfg, 0, sizeof(cfg));
        ret = aal_xfi_cfg_get(device_id, ppid - AAL_LPORT_ETH_NI5,
            &cfg);
        if (ret != CA_E_OK) {
            ca_printf("get port %d mac enable failed. ret = %d\r\n", ppid, ret);
            return ret;
        }

        mac_en = !cfg.pdown;
    }else if(ppid == AAL_LPORT_ETH_NI7){ /* PON port */
#ifdef CA_HAVE_EPON
		memset(&epon_cfg, 0, sizeof(aal_epon_mac_ctrl_t));
		aal_epon_mac_cfg_get(device_id, port_id, epon_msk, &epon_cfg);
		if((epon_cfg.rx_xg_ena == 1) && (epon_cfg.tx_xg_ena == 1)){
			*enable = 1;
		}else{
			*enable = 0;
		}
#endif
    }
#elif defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    else if(ppid == AAL_LPORT_ETH_NI5){    /* XAUI */
        memset(&xaui_tx_cfg, 0, sizeof(aal_ni_xaui_mac_tx_cfg_t));
        ret = aal_ni_xaui_mac_tx_config_get(device_id, &xaui_tx_cfg);
        if(ret != CA_E_OK){
            ca_printf("get port %d mac tx enable failed. ret = %d\r\n", ppid, ret);
            return ret;
        }
        memset(&xaui_rx_cfg, 0, sizeof(aal_ni_xaui_mac_rx_cfg_t));
        ret = aal_ni_xaui_mac_rx_config_get(device_id, &xaui_rx_cfg);
        if(ret != CA_E_OK){
            ca_printf("get port %d mac rx enable failed. ret = %d\r\n", ppid, ret);
            return ret;
        }
        mac_en = (xaui_tx_cfg.mac_en && xaui_rx_cfg.mac_en);

    }else if(ppid == AAL_LPORT_ETH_NI6 || ppid == AAL_LPORT_ETH_NI7){ /* XGE */
        mac_en = 1;
    }
    //ca_printf("get port %d mac enable: %d\r\n", ppid, mac_en);
#endif
    *enable = mac_en & phy_en;

    return ret;
}

/* only support ethernet ports MTU config globally, port_id is ignored */
ca_status_t ca_port_max_frame_size_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t size)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t ppid = 0;
    aal_ni_pkt_len_config_mask_t ge_msk;
    aal_ni_pkt_len_config_t ge_cfg;
    aal_ni_xaui_mac_rx_cfg_mask_t xaui_rx_msk;
    aal_ni_xaui_mac_rx_cfg_t xaui_rx_cfg;
    aal_ni_xge_mac_frame_len_config_mask_t xge_msk;
    aal_ni_xge_mac_frame_len_config_t xge_cfg;
#ifdef CA_HAVE_EPON
	aal_epon_mac_ctrl_msk_t epon_msk;
	aal_epon_mac_ctrl_t epon_cfg;
#endif

    CA_DEV_ID_CHECK(device_id);
    //CA_PORT_ETHERNET_CHECK(port_id);

    ppid = PORT_ID(port_id);    /* for ethernet ports, the ppid is the same as lpid */

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    if(ppid >= AAL_LPORT_ETH_NI3 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
#else
    if(ppid >= AAL_LPORT_ETH_NI0 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
#endif
        memset(&ge_msk, 0, sizeof(aal_ni_pkt_len_config_mask_t));
        memset(&ge_cfg, 0, sizeof(aal_ni_pkt_len_config_t));

        ge_msk.s.max_pkt_size = 1;
        ge_cfg.max_pkt_size = size;
        ret = aal_ni_pkt_len_set(device_id, ge_msk, &ge_cfg);
    }
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    else if(ppid == AAL_LPORT_ETH_NI5 || ppid == AAL_LPORT_ETH_NI6){ /* XGE */
        memset(&xge_msk, 0, sizeof(aal_ni_xge_mac_frame_len_config_mask_t));
        memset(&xge_cfg, 0, sizeof(aal_ni_xge_mac_frame_len_config_t));

        xge_msk.s.max_len = 1;
        xge_cfg.max_len = size;
        ret = aal_ni_xge_mac_frame_len_set(device_id, ppid-AAL_LPORT_ETH_NI5, xge_msk, &xge_cfg);
    }
    else if(ppid == AAL_LPORT_ETH_NI7){ /* PON port */
#ifdef CA_HAVE_EPON
        epon_msk.u32 = 0;
		epon_msk.s.rx_psize_max = 1;

		memset(&epon_cfg, 0, sizeof(aal_epon_mac_ctrl_t));
		epon_cfg.rx_psize_max = size;

	return aal_epon_mac_cfg_set(device_id, port_id, epon_msk, &epon_cfg);
#endif
    }
#elif defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    else if(ppid == AAL_LPORT_ETH_NI5){    /* XAUI */
        memset(&xaui_rx_msk, 0, sizeof(aal_ni_xaui_mac_rx_cfg_mask_t));
        memset(&xaui_rx_cfg, 0, sizeof(aal_ni_xaui_mac_rx_cfg_t));

        xaui_rx_msk.s.max_size = 1;
        xaui_rx_cfg.max_size = size;
        ret = aal_ni_xaui_mac_rx_config_set(device_id, xaui_rx_msk, &xaui_rx_cfg);

    }else if(ppid == AAL_LPORT_ETH_NI6 || ppid == AAL_LPORT_ETH_NI7){ /* XGE */
        memset(&xge_msk, 0, sizeof(aal_ni_xge_mac_frame_len_config_mask_t));
        memset(&xge_cfg, 0, sizeof(aal_ni_xge_mac_frame_len_config_t));

        xge_msk.s.max_len = 1;
        xge_cfg.max_len = size;
        ret = aal_ni_xge_mac_frame_len_set(device_id, ppid-AAL_LPORT_ETH_NI6, xge_msk, &xge_cfg);
    }
#endif

    return ret;
}

ca_status_t ca_port_max_frame_size_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_uint32_t * size)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t ppid = 0;
    aal_ni_pkt_len_config_t ge_cfg;
    aal_ni_xaui_mac_rx_cfg_t xaui_rx_cfg;
    aal_ni_xge_mac_frame_len_config_t xge_cfg;
#ifdef CA_HAVE_EPON
	aal_epon_mac_ctrl_t epon_cfg;
#endif

    CA_DEV_ID_CHECK(device_id);

    CA_ASSERT_RET(size, CA_E_PARAM);
    //CA_PORT_ETHERNET_CHECK(port_id);

    ppid = PORT_ID(port_id);    /* for ethernet ports, the ppid is the same as lpid */

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
        if(ppid >= AAL_LPORT_ETH_NI3 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
#else
        if(ppid >= AAL_LPORT_ETH_NI0 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
#endif

        memset(&ge_cfg, 0, sizeof(aal_ni_pkt_len_config_t));
        ret = aal_ni_pkt_len_get(device_id, &ge_cfg);
        if(ret == CA_E_OK){
            *size = ge_cfg.max_pkt_size;
        }
    }
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    else if(ppid == AAL_LPORT_ETH_NI5 || ppid == AAL_LPORT_ETH_NI6){ /* XGE */
        memset(&xge_cfg, 0, sizeof(aal_ni_xge_mac_frame_len_config_t));
        ret = aal_ni_xge_mac_frame_len_get(device_id, ppid-AAL_LPORT_ETH_NI5, &xge_cfg);
        if(ret == CA_E_OK){
            *size = xge_cfg.max_len;
        }
    }
    else if(ppid == AAL_LPORT_ETH_NI7){ /* PON port */
#ifdef CA_HAVE_EPON
		memset(&epon_cfg, 0, sizeof(aal_epon_mac_ctrl_t));
		aal_epon_mac_cfg_get(device_id, port_id, &epon_cfg);
		*size = epon_cfg.rx_psize_max;
#endif
    }
#elif defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    else if(ppid == AAL_LPORT_ETH_NI5){    /* XAUI */
        memset(&xaui_rx_cfg, 0, sizeof(aal_ni_xaui_mac_rx_cfg_t));
        ret = aal_ni_xaui_mac_rx_config_get(device_id, &xaui_rx_cfg);
        if(ret == CA_E_OK){
            *size = xaui_rx_cfg.max_size;
        }

    }else if(ppid == AAL_LPORT_ETH_NI6 || ppid == AAL_LPORT_ETH_NI7){ /* XGE */
        memset(&xge_cfg, 0, sizeof(aal_ni_xge_mac_frame_len_config_t));
        ret = aal_ni_xge_mac_frame_len_get(device_id, ppid-AAL_LPORT_ETH_NI6, &xge_cfg);
        if(ret == CA_E_OK){
            *size = xge_cfg.max_len;
        }
    }
#endif

    return ret;
}

/* only support ethernet ports IPG config */
ca_status_t ca_port_ipg_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t ipg)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t ppid = 0;
    aal_ni_eth_mac_config_mask_t ge_msk;
    aal_ni_eth_mac_config_t ge_cfg;
    aal_ni_xaui_mac_tx_cfg_mask_t xaui_tx_msk;
    aal_ni_xaui_mac_tx_cfg_t xaui_tx_cfg;
    aal_ni_xge_cfg_tx_mask_t xge_tx_msk;
    aal_ni_xge_cfg_tx_t xge_tx_cfg;
    aal_ni_xaui_ifg_mode_t dic_mode;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ETHERNET_CHECK(port_id);

    ppid = PORT_ID(port_id);    /* for ethernet ports, the ppid is the same as lpid */

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    if(ppid >= AAL_LPORT_ETH_NI3 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
#else
    if(ppid >= AAL_LPORT_ETH_NI0 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
#endif

        if (ipg < CA_PORT_IPG_GE_VAL_MIN || ipg > CA_PORT_IPG_GE_VAL_MAX) {
            return CA_E_PARAM;
        }
        memset(&ge_msk, 0, sizeof(aal_ni_eth_mac_config_mask_t));
        memset(&ge_cfg, 0, sizeof(aal_ni_eth_mac_config_t));

        ge_msk.s.tx_ipg_sel = 1;
        ge_cfg.tx_ipg_sel = ipg - 8;

        ret = aal_ni_eth_port_mac_set(device_id, ppid, ge_msk, &ge_cfg);

    }
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    else if(ppid == AAL_LPORT_ETH_NI5 || ppid == AAL_LPORT_ETH_NI6){ /* XGE */

        if (ipg < CA_PORT_IPG_XGE_VAL_MIN || ipg > CA_PORT_IPG_XGE_VAL_MAX) {
            return CA_E_PARAM;
        }
        memset(&xge_tx_msk, 0, sizeof(aal_ni_xge_cfg_tx_mask_t));
        memset(&xge_tx_cfg, 0, sizeof(aal_ni_xge_cfg_tx_t));

        xge_tx_msk.s.ifg  = 1;
        xge_tx_cfg.ifg = ipg;
        ret = aal_ni_xge_tx_config_set(device_id, ppid-AAL_LPORT_ETH_NI5, xge_tx_msk, &xge_tx_cfg);
    }
#elif defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    else if(ppid == AAL_LPORT_ETH_NI5){    /* XAUI */

        /* 5~11 DIC8 mode; 12~15 DIC12 mode; 15~ Disable DIC */
        if(ipg < AAL_NI_XAUI_IFG_DIC8_MIN_5){
            return CA_E_PARAM;

        }else if(ipg >= AAL_NI_XAUI_IFG_DIC8_MIN_5 && ipg > AAL_NI_XAUI_IFG_DIC8_MAX_11){
            dic_mode = AAL_NI_XAUI_IFG_MODE_DIC8;

        }else if(ipg >= AAL_NI_XAUI_IFG_DIC12_MIN_12 && ipg > AAL_NI_XAUI_IFG_DIC12_MAX_15){
            dic_mode = AAL_NI_XAUI_IFG_MODE_DIC12;

        }else{
            dic_mode = AAL_NI_XAUI_IFG_MODE_DISABLE;
        }

        memset(&xaui_tx_msk, 0, sizeof(aal_ni_xaui_mac_tx_cfg_mask_t));
        memset(&xaui_tx_cfg, 0, sizeof(aal_ni_xaui_mac_tx_cfg_t));

        xaui_tx_msk.s.dic_mode  = 1;
        xaui_tx_cfg.dic_mode = dic_mode;
        ret = aal_ni_xaui_mac_tx_config_set(device_id, xaui_tx_msk, &xaui_tx_cfg);
        if (ret == CA_E_OK) {
            __port_ipg_value = ipg;
        }

    }else if(ppid == AAL_LPORT_ETH_NI6 || ppid == AAL_LPORT_ETH_NI7){ /* XGE */

        if (ipg < CA_PORT_IPG_XGE_VAL_MIN || ipg > CA_PORT_IPG_XGE_VAL_MAX) {
            return CA_E_PARAM;
        }
        memset(&xge_tx_msk, 0, sizeof(aal_ni_xge_cfg_tx_mask_t));
        memset(&xge_tx_cfg, 0, sizeof(aal_ni_xge_cfg_tx_t));

        xge_tx_msk.s.ifg  = 1;
        xge_tx_cfg.ifg = ipg;
        ret = aal_ni_xge_tx_config_set(device_id, ppid-AAL_LPORT_ETH_NI6, xge_tx_msk, &xge_tx_cfg);
    }
#endif
    return ret;

}

ca_status_t ca_port_ipg_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_uint32_t * ipg)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t ppid = 0;
    aal_ni_eth_mac_config_t ge_cfg;
    aal_ni_xaui_mac_tx_cfg_t xaui_tx_cfg;
    aal_ni_xge_cfg_tx_t xge_tx_cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ETHERNET_CHECK(port_id);

    CA_ASSERT_RET(ipg, CA_E_PARAM);

    ppid = PORT_ID(port_id);    /* for ethernet ports, the ppid is the same as lpid */

    if(ppid >= AAL_LPORT_ETH_NI0 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
        memset(&ge_cfg, 0, sizeof(aal_ni_eth_mac_config_t));
        ret = aal_ni_eth_port_mac_get(device_id, ppid, &ge_cfg);
        if (ret == CA_E_OK) {
            *ipg = ge_cfg.tx_ipg_sel + 8;
        }
    }
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    else if(ppid == AAL_LPORT_ETH_NI5 || ppid == AAL_LPORT_ETH_NI6){ /* XGE */
        memset(&xge_tx_cfg, 0, sizeof(aal_ni_xge_cfg_tx_t));
        ret = aal_ni_xge_tx_config_get(device_id, ppid-AAL_LPORT_ETH_NI5, &xge_tx_cfg);
        if (ret == CA_E_OK) {
            *ipg = xge_tx_cfg.ifg;
        }
    }
#elif defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    else if(ppid == AAL_LPORT_ETH_NI5){    /* XAUI */
        *ipg = __port_ipg_value;

    }else if(ppid == AAL_LPORT_ETH_NI6 || ppid == AAL_LPORT_ETH_NI7){ /* XGE */
        memset(&xge_tx_cfg, 0, sizeof(aal_ni_xge_cfg_tx_t));
        ret = aal_ni_xge_tx_config_get(device_id, ppid-AAL_LPORT_ETH_NI6, &xge_tx_cfg);
        if (ret == CA_E_OK) {
            *ipg = xge_tx_cfg.ifg;
        }
    }
#endif
    return ret;
}

/* only support ethernet ports loopback config */
ca_status_t ca_port_loopback_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_port_loopback_mode_t loopback)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t ppid = 0;
    aal_ni_eth_if_config_mask_t ge_msk;
    aal_ni_eth_if_config_t ge_cfg;
    aal_ni_xaui_mac_ctrl_mask_t xaui_msk;
    aal_ni_xaui_mac_ctrl_t xaui_cfg;
    aal_ni_xge_cmn_cfg_mask_t xge_msk;
    aal_ni_xge_cmn_cfg_t xge_cfg;

    ca_uint8_t phy_addr[8] = {/* FPGA board has 8 Eth Ports */
        1, 2, 3, 4, 16, 17, 18, 19
    };
#ifndef CONFIG_CORTINA_BOARD_FPGA
    ca_device_config_tlv_t scfg_tlv;
#endif
    aal_ni_mac_lpbk_mode_t mac_lpbk = AAL_NI_MAC_LPBK_MODE_NONE;
    aal_phy_lpbk_mode_t phy_lpbk;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ETHERNET_CHECK(port_id);

    ppid = PORT_ID(port_id);    /* for ethernet ports, the ppid is the same as lpid */

    switch (loopback) {
    case CA_PORT_LOOPBACK_NONE:
        mac_lpbk = AAL_NI_MAC_LPBK_MODE_NONE;
        phy_lpbk.rx2tx_enable = 0;
        phy_lpbk.tx2rx_enable = 0;
        break;

    case CA_PORT_LOOPBACK_MAC_LOCAL:
        mac_lpbk = AAL_NI_MAC_LPBK_MODE_TX2RX;
        phy_lpbk.rx2tx_enable = 0;
        phy_lpbk.tx2rx_enable = 0;
        break;

    case CA_PORT_LOOPBACK_MAC_REMOTE:
        mac_lpbk = AAL_NI_MAC_LPBK_MODE_RX2TX;
        phy_lpbk.rx2tx_enable = 0;
        phy_lpbk.tx2rx_enable = 0;
        break;

    case CA_PORT_LOOPBACK_PHY_LOCAL:
        mac_lpbk = AAL_NI_MAC_LPBK_MODE_NONE;
        phy_lpbk.rx2tx_enable = 0;
        phy_lpbk.tx2rx_enable = 1;
        break;

    case CA_PORT_LOOPBACK_PHY_REMOTE:
        mac_lpbk = AAL_NI_MAC_LPBK_MODE_NONE;
        phy_lpbk.rx2tx_enable = 1;
        phy_lpbk.tx2rx_enable = 0;
        break;

    default:
        mac_lpbk = AAL_NI_MAC_LPBK_MODE_NONE;
        phy_lpbk.rx2tx_enable = 0;
        phy_lpbk.tx2rx_enable = 0;
        break;

    }

#ifndef CONFIG_CORTINA_BOARD_FPGA
    /* Read PHY MDIO address from STARTUP config */
    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_PHY_BASE_ADDR;
    scfg_tlv.len = CFG_ID_PHY_BASE_ADDR_LEN;

    ret = ca_device_config_tlv_get(device_id, &scfg_tlv);
    if (CA_E_OK == ret) {
        memcpy(phy_addr, scfg_tlv.value, CFG_ID_PHY_BASE_ADDR_LEN);
    }
#endif
    ca_printf("set port %d (phy addr 0x%x) loopback tx2rx %d, rx2tx %d\r\n",
              ppid, phy_addr[ppid], phy_lpbk.tx2rx_enable, phy_lpbk.rx2tx_enable);

    ret = aal_eth_phy_loopback_set(device_id, phy_addr[ppid], phy_lpbk);
    if (ret != CA_E_OK) {
        ca_printf("phy loopback set error, ret %d\r\n", ret);
        return ret;
    }

    if(ppid >= AAL_LPORT_ETH_NI0 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
        memset(&ge_msk, 0, sizeof(aal_ni_eth_if_config_mask_t));
        memset(&ge_cfg, 0, sizeof(aal_ni_eth_if_config_t));

        ge_msk.s.lpbk_mode = 1;
        ge_cfg.lpbk_mode = mac_lpbk;

        ret = aal_ni_eth_if_set(device_id, ppid, ge_msk, &ge_cfg);

    }
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    else if(ppid == AAL_LPORT_ETH_NI5 || ppid == AAL_LPORT_ETH_NI6){ /* XGE */

        memset(&xge_msk, 0, sizeof(aal_ni_xge_cmn_cfg_mask_t));
        memset(&xge_cfg, 0, sizeof(aal_ni_xge_cmn_cfg_t));

        xge_msk.s.core_tx2rx_lb = 1;
        xge_msk.s.core_rx2tx_lb = 1;
        if(mac_lpbk == AAL_NI_MAC_LPBK_MODE_TX2RX){
            xge_cfg.core_tx2rx_lb = 1;
        }else if(mac_lpbk == AAL_NI_MAC_LPBK_MODE_TX2RX){
            xge_cfg.core_rx2tx_lb = 1;
        }

        ret = aal_ni_xge_cmn_config_set(device_id, ppid-AAL_LPORT_ETH_NI5, xge_msk, &xge_cfg);
    }
#elif defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    else if(ppid == AAL_LPORT_ETH_NI5){    /* XAUI */

        memset(&xaui_msk, 0, sizeof(aal_ni_xaui_mac_ctrl_mask_t));
        memset(&xaui_cfg, 0, sizeof(aal_ni_xaui_mac_ctrl_t));

        xaui_msk.s.tx2rx_lpbk_en = 1;
        xaui_msk.s.rx2tx_lpbk_en = 1;
        if(mac_lpbk == AAL_NI_MAC_LPBK_MODE_TX2RX){
            xaui_cfg.tx2rx_lpbk_en = 1;
        }else if(mac_lpbk == AAL_NI_MAC_LPBK_MODE_TX2RX){
            xaui_cfg.rx2tx_lpbk_en = 1;
        }

        ret = aal_ni_xaui_mac_ctrl_set(device_id, xaui_msk, &xaui_cfg);

    }else if(ppid == AAL_LPORT_ETH_NI6 || ppid == AAL_LPORT_ETH_NI7){ /* XGE */

        memset(&xge_msk, 0, sizeof(aal_ni_xge_cmn_cfg_mask_t));
        memset(&xge_cfg, 0, sizeof(aal_ni_xge_cmn_cfg_t));

        xge_msk.s.core_tx2rx_lb = 1;
        xge_msk.s.core_rx2tx_lb = 1;
        if(mac_lpbk == AAL_NI_MAC_LPBK_MODE_TX2RX){
            xge_cfg.core_tx2rx_lb = 1;
        }else if(mac_lpbk == AAL_NI_MAC_LPBK_MODE_TX2RX){
            xge_cfg.core_rx2tx_lb = 1;
        }

        ret = aal_ni_xge_cmn_config_set(device_id, ppid-AAL_LPORT_ETH_NI6, xge_msk, &xge_cfg);
    }
#endif
    return ret;
}

ca_status_t ca_port_loopback_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_port_loopback_mode_t * loopback)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t ppid = 0;
    aal_ni_eth_if_config_t ge_cfg;
    aal_ni_xaui_mac_ctrl_t xaui_cfg;
    aal_ni_xge_cmn_cfg_t xge_cfg;
    ca_uint8_t phy_addr[8] = {/* FPGA board has 8 Eth Ports */
        1, 2, 3, 4, 16, 17, 18, 19
    };
#ifndef CONFIG_CORTINA_BOARD_FPGA
    ca_device_config_tlv_t scfg_tlv;
#endif
    aal_ni_mac_lpbk_mode_t mac_lpbk = AAL_NI_MAC_LPBK_MODE_NONE;
    aal_phy_lpbk_mode_t phy_lpbk;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ETHERNET_CHECK(port_id);

    ppid = PORT_ID(port_id);    /* for ethernet ports, the ppid is the same as lpid */

#ifndef CONFIG_CORTINA_BOARD_FPGA
    /* Read PHY MDIO address from STARTUP config */
    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_PHY_BASE_ADDR;
    scfg_tlv.len = CFG_ID_PHY_BASE_ADDR_LEN;

    ret = ca_device_config_tlv_get(device_id, &scfg_tlv);
    if (CA_E_OK == ret) {
        memcpy(phy_addr, scfg_tlv.value, CFG_ID_PHY_BASE_ADDR_LEN);
    }
#endif

    ret = aal_eth_phy_loopback_get(device_id, phy_addr[ppid], &phy_lpbk);
    if (ret != CA_E_OK) {
        ca_printf("phy loopback get error, ret %d\r\n", ret);
        return ret;
    }
    ca_printf("get port %d (phy addr 0x%x) loopback tx2rx %d, rx2tx %d\r\n",
              ppid, phy_addr[ppid], phy_lpbk.tx2rx_enable, phy_lpbk.rx2tx_enable);

    if(ppid >= AAL_LPORT_ETH_NI0 && ppid <= AAL_LPORT_ETH_NI4){ /* GE */
        memset(&ge_cfg, 0, sizeof(aal_ni_eth_if_config_t));
        ret = aal_ni_eth_if_get(device_id, ppid, &ge_cfg);
        if (ret != CA_E_OK) {
            ca_printf("ni %d mac loopback get error, ret %d\r\n", ppid, ret);
            return ret;
        }
        mac_lpbk = ge_cfg.lpbk_mode;
    }
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    else if(ppid == AAL_LPORT_ETH_NI5 || ppid == AAL_LPORT_ETH_NI6){ /* XGE */

        memset(&xge_cfg, 0, sizeof(aal_ni_xge_cmn_cfg_t));

        ret = aal_ni_xge_cmn_config_get(device_id, ppid-AAL_LPORT_ETH_NI5, &xge_cfg);
        if (ret != CA_E_OK) {
            ca_printf("ni %d mac loopback get error, ret %d\r\n", ppid, ret);
            return ret;
        }
        if(xge_cfg.core_tx2rx_lb == 1){
            mac_lpbk = AAL_NI_MAC_LPBK_MODE_TX2RX;

        }else if(xge_cfg.core_rx2tx_lb == 1){
            mac_lpbk = AAL_NI_MAC_LPBK_MODE_RX2TX;

        }else{
            mac_lpbk = AAL_NI_MAC_LPBK_MODE_NONE;
        }
    }
#elif defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    else if(ppid == AAL_LPORT_ETH_NI5){    /* XAUI */

        memset(&xaui_cfg, 0, sizeof(aal_ni_xaui_mac_ctrl_t));

        ret = aal_ni_xaui_mac_ctrl_get(device_id, &xaui_cfg);
        if (ret != CA_E_OK) {
            ca_printf("ni %d mac loopback get error, ret %d\r\n", ppid, ret);
            return ret;
        }
        if(xaui_cfg.tx2rx_lpbk_en == 1){
            mac_lpbk = AAL_NI_MAC_LPBK_MODE_TX2RX;

        }else if(xaui_cfg.rx2tx_lpbk_en == 1){
            mac_lpbk = AAL_NI_MAC_LPBK_MODE_RX2TX;

        }else{
            mac_lpbk = AAL_NI_MAC_LPBK_MODE_NONE;
        }

    }else if(ppid == AAL_LPORT_ETH_NI6 || ppid == AAL_LPORT_ETH_NI7){ /* XGE */

        memset(&xge_cfg, 0, sizeof(aal_ni_xge_cmn_cfg_t));

        ret = aal_ni_xge_cmn_config_get(device_id, ppid-AAL_LPORT_ETH_NI6, &xge_cfg);
        if (ret != CA_E_OK) {
            ca_printf("ni %d mac loopback get error, ret %d\r\n", ppid, ret);
            return ret;
        }
        if(xge_cfg.core_tx2rx_lb == 1){
            mac_lpbk = AAL_NI_MAC_LPBK_MODE_TX2RX;

        }else if(xge_cfg.core_rx2tx_lb == 1){
            mac_lpbk = AAL_NI_MAC_LPBK_MODE_RX2TX;

        }else{
            mac_lpbk = AAL_NI_MAC_LPBK_MODE_NONE;
        }
    }
#endif

    if(mac_lpbk == AAL_NI_MAC_LPBK_MODE_NONE && !phy_lpbk.rx2tx_enable && !phy_lpbk.tx2rx_enable){
        *loopback = CA_PORT_LOOPBACK_NONE;

    }else if(mac_lpbk == AAL_NI_MAC_LPBK_MODE_TX2RX && !phy_lpbk.rx2tx_enable && !phy_lpbk.tx2rx_enable){
        *loopback = CA_PORT_LOOPBACK_MAC_LOCAL;

    }else if(mac_lpbk == AAL_NI_MAC_LPBK_MODE_RX2TX && !phy_lpbk.rx2tx_enable && !phy_lpbk.tx2rx_enable){
        *loopback = CA_PORT_LOOPBACK_MAC_REMOTE;

    }else if(mac_lpbk == AAL_NI_MAC_LPBK_MODE_NONE && !phy_lpbk.rx2tx_enable && phy_lpbk.tx2rx_enable){
        *loopback = CA_PORT_LOOPBACK_PHY_LOCAL;

    }else if(mac_lpbk == AAL_NI_MAC_LPBK_MODE_NONE && phy_lpbk.rx2tx_enable && !phy_lpbk.tx2rx_enable){
        *loopback = CA_PORT_LOOPBACK_PHY_REMOTE;

    }else{
        *loopback = CA_PORT_LOOPBACK_NONE;
    }

    return ret;
}

ca_status_t ca_port_untagged_priority_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t priority)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    aal_ilpb_cfg_msk_t msk;
    aal_ilpb_cfg_t cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);
    CA_COS_CHECK(priority);

    lpid = PORT_ID(port_id);
#if 0
    if (__PORT_IS_WAN(lpid))
        lpid = CA_PORT_ID_L3_WAN;
#endif
    memset(&msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&cfg, 0, sizeof(aal_ilpb_cfg_t));

    msk.s.default_cos = 1;
    cfg.default_cos = priority;

    ret = aal_port_ilpb_cfg_set(device_id, lpid, msk, &cfg);
    if (ret != CA_E_OK) {
        return ret;
    }

    return ret;
}

ca_status_t ca_port_untagged_priority_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_uint32_t * priority)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    aal_ilpb_cfg_t cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);
    CA_ASSERT_RET(priority, CA_E_PARAM);

    lpid = PORT_ID(port_id);
#if 0
    if (__PORT_IS_WAN(lpid))
        lpid = CA_PORT_ID_L3_WAN;
#endif
    ret = aal_port_ilpb_cfg_get(device_id, lpid, &cfg);
    if (ret != CA_E_OK) {
        return ret;
    }
    *priority = cfg.default_cos;

    return ret;
}

ca_status_t ca_port_tpid_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t tpid)
{
    return CA_E_OK;
}

ca_status_t ca_port_tpid_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_uint32_t * tpid)
{
    return CA_E_OK;
}

ca_status_t ca_port_stp_state_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_port_stp_state_t stp_state)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    ca_uint32_t egr_status = 0;
    aal_stp_state_t igr_state = AAL_STP_STATE_FWD_AND_LEARN;
    aal_ilpb_cfg_msk_t igr_msk;
    aal_ilpb_cfg_t igr_cfg;
    aal_elpb_cfg_msk_t egr_msk;
    aal_elpb_cfg_t egr_cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);

    lpid = PORT_ID(port_id);

    __PORT_LOCK(device_id);

    memset(&igr_msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&igr_cfg, 0, sizeof(aal_ilpb_cfg_t));

    switch (stp_state) {
    case CA_PORT_STP_DISABLED:
        igr_state = AAL_STP_STATE_DROP;
        //TODO. special packet set BPDU_PKT=drop
        egr_status = 0;
        break;

    case CA_PORT_STP_BLOCKING:
        igr_state = AAL_STP_STATE_DROP;
        egr_status = 0;
        break;

    case CA_PORT_STP_LEARNING:
        igr_state = AAL_STP_STATE_LEARN_NO_FWD;
        egr_status = 0;
        break;

    case CA_PORT_STP_FORWARDING:
        igr_state = AAL_STP_STATE_FWD_AND_LEARN;
        egr_status = 1;
        break;

    default:
        igr_state = CA_PORT_STP_FORWARDING;
        egr_status = 1;
        break;

    }

    igr_msk.s.stp_mode = 1;
    igr_cfg.stp_mode = igr_state;

    ret = aal_port_ilpb_cfg_set(device_id, lpid, igr_msk, &igr_cfg);
    if (ret != CA_E_OK) {
        goto end;
    }

    egr_msk.s.egr_port_stp_status = 1;
    egr_cfg.egr_port_stp_status = egr_status;

    ret = aal_port_elpb_cfg_set(device_id, lpid, egr_msk, &egr_cfg);
    if (ret != CA_E_OK) {
        goto end;
    }

    if(stp_state == CA_PORT_STP_DISABLED){
        __port_stp_mode[lpid] = FALSE;
    }else{
        __port_stp_mode[lpid] = TRUE;
    }
end:
    __PORT_UNLOCK(device_id);
    return ret;

}

ca_status_t ca_port_stp_state_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_port_stp_state_t * stp_state)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    ca_uint32_t egr_state = 0;
    aal_stp_state_t igr_state = AAL_STP_STATE_FWD_AND_LEARN;
    aal_ilpb_cfg_t igr_cfg;
    aal_elpb_cfg_t egr_cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);

    lpid = PORT_ID(port_id);

    memset(&igr_cfg, 0, sizeof(aal_ilpb_cfg_t));

    igr_cfg.stp_mode = igr_state;

    ret = aal_port_ilpb_cfg_get(device_id, lpid, &igr_cfg);
    if (ret != CA_E_OK) {
        return ret;
    }
    igr_state = igr_cfg.stp_mode;

    ret = aal_port_elpb_cfg_get(device_id, lpid, &egr_cfg);
    if (ret != CA_E_OK) {
        return ret;
    }
    egr_state = egr_cfg.egr_port_stp_status;

    if(igr_state == AAL_STP_STATE_DROP && egr_state == 0){
        if(!__port_stp_mode[lpid]){
            *stp_state = CA_PORT_STP_DISABLED;
        }else{
            *stp_state = CA_PORT_STP_BLOCKING;
        }
    }else if(igr_state == AAL_STP_STATE_LEARN_NO_FWD && egr_state == 0){
        *stp_state = CA_PORT_STP_LEARNING;

    }else if(igr_state == AAL_STP_STATE_FWD_AND_LEARN && egr_state == 1){
        *stp_state = CA_PORT_STP_FORWARDING;

    }else{
        *stp_state = CA_PORT_STP_FORWARDING;

    }
    return ret;

}

ca_status_t ca_port_isolation_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint8_t port_count,
    CA_IN ca_port_id_t * dst_ports)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    ca_uint64_t isolation_bmp = 0;
    ca_uint64_t mmshp_bmp = -1;
    ca_uint32 i = 0;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);
    //CA_ASSERT_RET(dst_ports, CA_E_PARAM); /* if port_count = 0, dst_ports can be NULL */

    if (port_count > CA_PORT_MAX_NUM) {
        return CA_E_PARAM;
    }

    for (i = 0; i < port_count; i++) {
        lpid = PORT_ID(dst_ports[i]);
        isolation_bmp |= (ca_uint64) 1 << lpid;
    }

    lpid = PORT_ID(port_id);

    __PORT_LOCK(device_id);

    ca_printf("port isolation set port 0x%x, count %d, bmp 0x%llx\r\n", port_id, port_count, isolation_bmp);
    mmshp_bmp &= ~isolation_bmp;
    ret = aal_port_mmshp_check_set(device_id, lpid, mmshp_bmp);
    if (ret != CA_E_OK) {
        goto end;
    }

end:
    __PORT_UNLOCK(device_id);
    return ret;

}

ca_status_t ca_port_isolation_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_uint8_t * port_count,
    CA_OUT ca_port_id_t * dst_ports)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    ca_uint64_t mmshp_bmp = 0;
    ca_uint32_t isolation_bmp = -1;
    ca_uint32 count = 0;
    ca_uint32 i = 0;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);

    CA_ASSERT_RET(port_count, CA_E_PARAM);
    CA_ASSERT_RET(dst_ports, CA_E_PARAM);

    lpid = PORT_ID(port_id);
    ret = aal_port_mmshp_check_get(device_id, lpid, &mmshp_bmp);
    isolation_bmp &= ~mmshp_bmp;

    for (i = 0; i < 64; i++) {
        if (isolation_bmp & (ca_uint64) 1 << i) {
            dst_ports[count] = ca_port_id_get(i);
            count++;
        }
    }
    *port_count = count;
    return CA_E_OK;
}

ca_status_t ca_port_discard_control_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_port_discard_control_t *control)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    aal_ilpb_cfg_msk_t msk;
    aal_ilpb_cfg_t cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);

    CA_ASSERT_RET(control, CA_E_PARAM);

    lpid = PORT_ID(port_id);

    memset(&msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&cfg, 0, sizeof(aal_ilpb_cfg_t));

    msk.s.drop_untagged_packet = 1;
    msk.s.drop_pritagged_packet = 1;
    msk.s.drop_doubletagged_packet = 1;
    msk.s.drop_singletagged_packet = 1;

    if (control->drop_untag) {
        cfg.drop_untagged_packet = TRUE;
    }

    if (control->drop_pritag) {
        cfg.drop_pritagged_packet = TRUE;
    }

    if (control->drop_mtag) {
        cfg.drop_doubletagged_packet = TRUE;
    }

    if (control->drop_stag) {    /* drop single tagged packets */
        cfg.drop_singletagged_packet = TRUE;
    }

    ret = aal_port_ilpb_cfg_set(device_id, lpid, msk, &cfg);
    if (ret != CA_E_OK) {
        return ret;
    }

    return ret;

}

ca_status_t ca_port_discard_control_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_port_discard_control_t *control)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    aal_ilpb_cfg_t cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);

    CA_ASSERT_RET(control, CA_E_PARAM);

    lpid = PORT_ID(port_id);

    memset(&cfg, 0, sizeof(aal_ilpb_cfg_t));

    ret = aal_port_ilpb_cfg_get(device_id, lpid, &cfg);
    if (ret != CA_E_OK) {
        return ret;
    }

    memset(control, 0, sizeof(ca_port_discard_control_t));
    if (cfg.drop_untagged_packet) {
        control->drop_untag = TRUE;
    }
    if(cfg.drop_pritagged_packet){
        control->drop_pritag = TRUE;
    }

    if (cfg.drop_doubletagged_packet) {
        control->drop_mtag = TRUE;
    }

    if (cfg.drop_singletagged_packet) {
        control->drop_stag = TRUE;
    }
    return ret;
}

ca_status_t ca_port_station_move_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_boolean_t enable)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    aal_ilpb_cfg_msk_t msk;
    aal_ilpb_cfg_t cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);
    CA_BOOL_VAL_CHECK(enable);

    lpid = PORT_ID(port_id);

    memset(&msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&cfg, 0, sizeof(aal_ilpb_cfg_t));

    msk.s.station_move_en = 1;
    cfg.station_move_en = enable;

    ret = aal_port_ilpb_cfg_set(device_id, lpid, msk, &cfg);
    if (ret != CA_E_OK) {
        return ret;
    }

    return ret;

}

ca_status_t ca_port_station_move_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_boolean_t * enable)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    aal_ilpb_cfg_t cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);

    CA_ASSERT_RET(enable, CA_E_PARAM);

    lpid = PORT_ID(port_id);

    memset(&cfg, 0, sizeof(aal_ilpb_cfg_t));

    ret = aal_port_ilpb_cfg_get(device_id, lpid, &cfg);
    if (ret != CA_E_OK) {
        return ret;
    }

    *enable = cfg.station_move_en;
    return ret;
}

ca_status_t ca_port_pvid_vlan_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t vid)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    aal_ilpb_cfg_msk_t msk;
    aal_ilpb_cfg_t cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);
    CA_VLAN_ID_CHECK(vid);

    lpid = PORT_ID(port_id);

    memset(&msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&cfg, 0, sizeof(aal_ilpb_cfg_t));

    msk.s.default_vlanid = 1;
    msk.s.default_untag_cmd = 1;
    cfg.default_vlanid = vid;

    if(vid != 0){
        cfg.default_untag_cmd = AAL_VLAN_CMD_PUSH;
    }else{
        cfg.default_untag_cmd = AAL_VLAN_CMD_NOP;
    }

    ret = aal_port_ilpb_cfg_set(device_id, lpid, msk, &cfg);
    if (ret != CA_E_OK) {
        return ret;
    }

    return ret;

}

ca_status_t ca_port_pvid_vlan_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_uint32_t * vid)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    aal_ilpb_cfg_t cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);

    CA_ASSERT_RET(vid, CA_E_PARAM);

    lpid = PORT_ID(port_id);

    memset(&cfg, 0, sizeof(aal_ilpb_cfg_t));

    ret = aal_port_ilpb_cfg_get(device_id, lpid, &cfg);
    if (ret != CA_E_OK) {
        return ret;
    }

    *vid = cfg.default_vlanid;
    return ret;
}

ca_boolean_t __port_is_svlan_tpid(ca_port_id_t port_id, ca_uint32_t tpid_index)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t i = 0;
    ca_uint32_t num = 0;
    ca_uint32_t tpid[4];

    memset(&tpid[0], 0, sizeof(tpid));
    ret = ca_l2_vlan_outer_tpid_list(0, port_id, &num, tpid);
    if(ret != CA_E_OK){
        return CA_FALSE;
    }
    for(i = 0; i < num; i++){
        if(tpid[i] == tpid_index){
            return CA_TRUE;
        }
    }
    return CA_FALSE;
}

/* index 0: 0x8100; 1: 0x9100; 2: 0x88a8; 3: user defined*/
ca_status_t ca_port_pvid_tpid_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t tpid_index)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    aal_ilpb_cfg_msk_t msk;
    aal_ilpb_cfg_t cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);

    if (tpid_index > 3) {    /* support index 0, 1, 2, 3 */
        return CA_E_PARAM;
    }

    __PORT_LOCK(device_id);
    lpid = PORT_ID(port_id);

    memset(&msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&cfg, 0, sizeof(aal_ilpb_cfg_t));
    if(__port_is_svlan_tpid(port_id, tpid_index)){
        msk.s.sc_ind= 1;
        cfg.sc_ind = 1;
    }else{
        msk.s.sc_ind= 1;
        cfg.sc_ind = 0;
    }

    ret = aal_port_ilpb_cfg_set(device_id, lpid, msk, &cfg);
    if (ret != CA_E_OK) {
        goto end;
        return ret;
    }

    __port_tpid_index[lpid] = tpid_index;
end:
    __PORT_UNLOCK(device_id);
    return ret;

}

ca_status_t ca_port_pvid_tpid_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_uint32_t * tpid_index)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);

    CA_ASSERT_RET(tpid_index, CA_E_PARAM);

    lpid = PORT_ID(port_id);

    *tpid_index = __port_tpid_index[lpid];

    return ret;
}

ca_status_t ca_port_pvid_dot1p_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t priority)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    aal_ilpb_cfg_msk_t msk;
    aal_ilpb_cfg_t cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);
    CA_DOT_1P_CHECK(priority);

    lpid = PORT_ID(port_id);

    memset(&msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&cfg, 0, sizeof(aal_ilpb_cfg_t));

    msk.s.default_802_1p = 1;
    cfg.default_802_1p = priority;

    ret = aal_port_ilpb_cfg_set(device_id, lpid, msk, &cfg);
    if (ret != CA_E_OK) {
        return ret;
    }

    return ret;

}

ca_status_t ca_port_pvid_dot1p_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t port_id,
    CA_OUT ca_uint32_t * priority)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    aal_ilpb_cfg_t cfg;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);

    CA_ASSERT_RET(priority, CA_E_PARAM);

    lpid = PORT_ID(port_id);

    memset(&cfg, 0, sizeof(aal_ilpb_cfg_t));

    ret = aal_port_ilpb_cfg_get(device_id, lpid, &cfg);
    if (ret != CA_E_OK) {
        return ret;
    }

    *priority = cfg.default_802_1p;
    return ret;
}

static ca_status_t __port_lag_hash_set(ca_device_id_t device_id, ca_uint8_t lag_group_id, ca_classifier_key_mask_t *lag_hash_keys)
{
    aal_trunk_hash_key_t cfg;

    memset((void *)&cfg, 0, sizeof(aal_trunk_hash_key_t));
    //if(lag_hash_keys.l2 == 1 && lag_hash_keys.l2_mask. == 1){
    //cfg.field_sel_cos = 1;
    //}

    if (lag_hash_keys->l4 == 1 && lag_hash_keys->l4_mask.dst_port == 1) {
        cfg.field_sel_l4_dp = 1;
    }

    if (lag_hash_keys->l4 == 1 && lag_hash_keys->l4_mask.src_port == 1) {
        cfg.field_sel_l4_sp = 1;
    }

    if (lag_hash_keys->ip == 1 && lag_hash_keys->ip_mask.ip_da == 1) {
        cfg.field_sel_ip_da = 1;
    }

    if (lag_hash_keys->ip == 1 && lag_hash_keys->ip_mask.ip_sa == 1) {
        cfg.field_sel_ip_sa = 1;
    }

    if (lag_hash_keys->l2 == 1 && lag_hash_keys->l2_mask.vlan_otag == 1
            && lag_hash_keys->l2_mask.vlan_otag_mask.vid == 1) {
        cfg.field_sel_tx_top_vid = 1;
    }

    if (lag_hash_keys->l2 == 1 && lag_hash_keys->l2_mask.dst_mac == 1) {
        cfg.field_sel_mac_da = 1;
    }

    if (lag_hash_keys->l2 == 1 && lag_hash_keys->l2_mask.src_mac == 1) {
        cfg.field_sel_mac_sa = 1;
    }
    return aal_port_trunk_group_hash_set(device_id, lag_group_id, cfg);

}

static ca_status_t __port_lag_hash_get(ca_device_id_t device_id, ca_uint8_t lag_group_id, ca_classifier_key_mask_t *lag_hash_keys)
{
    aal_trunk_hash_key_t cfg;

    aal_port_trunk_group_hash_get(device_id, lag_group_id, &cfg);

    memset(lag_hash_keys, 0, sizeof(ca_classifier_key_mask_t));

    //if(cfg.bf.trunk_hash_field_sel_cos == 1){
    //lag_hash_keys = 1;
    //}

    if (cfg.field_sel_l4_dp == 1) {
        lag_hash_keys->l4 = 1;
        lag_hash_keys->l4_mask.dst_port = 1;
    }

    if (cfg.field_sel_l4_sp == 1) {
        lag_hash_keys->l4 = 1;
        lag_hash_keys->l4_mask.src_port = 1;
    }

    if (cfg.field_sel_ip_da == 1) {
        lag_hash_keys->ip = 1;
        lag_hash_keys->ip_mask.ip_da = 1;
    }

    if (cfg.field_sel_ip_sa == 1) {
        lag_hash_keys->ip = 1;
        lag_hash_keys->ip_mask.ip_sa = 1;
    }

    if (cfg.field_sel_tx_top_vid == 1) {
        lag_hash_keys->l2 = 1;
        lag_hash_keys->l2_mask.vlan_otag = 1;
        lag_hash_keys->l2_mask.vlan_otag_mask.vid = 1;
    }

    if (cfg.field_sel_mac_da == 1) {
        lag_hash_keys->l2 = 1;
        lag_hash_keys->l2_mask.dst_mac = 1;
    }

    if (cfg.field_sel_mac_sa == 1) {
        lag_hash_keys->l2 = 1;
        lag_hash_keys->l2_mask.src_mac = 1;
    }

    return CA_E_OK;
}

static ca_status_t __port_lag_dest_map_update(CA_IN ca_device_id_t dev,
        CA_IN ca_uint32_t group_id,
        CA_IN ca_uint32_t port_num,
        CA_IN ca_uint32_t active_ports[])
{
    ca_uint32_t i = 0;
    ca_uint32_t ldpid = 0;

    for (i = 0; i < 32; i++) {
        if (port_num == 0) {
            ldpid = 0;
        } else {
            ldpid = active_ports[i % port_num];
        }
        aal_port_trunk_dest_map_set(dev, 32 * group_id + i, ldpid);
    }

    return CA_E_OK;
}

/* return 0xff means not found */
ca_uint8_t __port_lag_member_of_group(ca_uint32_t lpid)
{
    ca_uint32_t group_id = 0;
    for(group_id = 0; group_id < CA_PORT_MAX_NUM_TRUNK; group_id++){
        if(__port_lag_attr[group_id].valid){
            if(__port_lag_attr[group_id].members_bmp & ((ca_uint64)1<<lpid)){
                return group_id;
            }
        }
    }
    return 0xff;
}

ca_status_t ca_port_link_aggregation_group_create(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint8_t lag_group_id,
    CA_IN ca_uint8_t member_port_count,
    CA_IN ca_port_id_t *member_ports,
    CA_IN ca_classifier_key_mask_t *lag_hash_keys)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t i = 0;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    ca_uint8_t group_id = 0;
    ca_uint64_t members_bmp = 0;
    ca_eth_port_link_status_t status = CA_PORT_LINK_UP;
    ca_uint32_t active_ldpid_num = 0;
    ca_uint32_t active_ldpid[CA_PORT_MAX_NUM_TRUNK_MEMBER];

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_LAG_ID_CHECK(lag_group_id);

    if (member_port_count > CA_PORT_MAX_NUM_TRUNK_MEMBER)
        return CA_E_PARAM;

    if (__port_lag_attr[lag_group_id].valid) {
        return CA_E_EXISTS;
    }

    __PORT_LOCK(device_id);

    /* note: set hash key and dest maping table before the group member set */
    ret = __port_lag_hash_set(device_id, lag_group_id, lag_hash_keys);
    if (ret != CA_E_OK) {
        goto end;
    }

    /* get the active ports */
    memset(active_ldpid, 0, sizeof(active_ldpid));
    for (i = 0; i < member_port_count; i++) {
        lpid = PORT_ID(member_ports[i]);
        group_id = __port_lag_member_of_group(lpid);
        if(group_id != lag_group_id && group_id != 0xff){
            ca_printf("port 0x%x is a member of group %d\r\n", member_ports[i], group_id);
            ret = CA_E_CONFLICT;
            goto end;
        }

        if (CA_PORT_IS_ETHERNET(member_ports[i])) {    /* ethernet port need to check link status */
            ret = ca_eth_port_link_status_get(device_id, member_ports[i], &status);
            if (ret != CA_E_OK || status != CA_PORT_LINK_UP) {
                continue;
            }
        }
        active_ldpid[active_ldpid_num] = lpid;
        active_ldpid_num++;
    }

    /* only add the linkup ports to the dest map */
    ret = __port_lag_dest_map_update(device_id, lag_group_id, active_ldpid_num, active_ldpid);
    if (ret != CA_E_OK) {
        goto end;
    }
    memset(__port_lag_attr[lag_group_id].active_members, 0,
           CA_PORT_MAX_NUM_TRUNK_MEMBER * sizeof(ca_uint32_t));
    memcpy(__port_lag_attr[lag_group_id].active_members, active_ldpid,
           active_ldpid_num * sizeof(ca_uint32_t));
    __port_lag_attr[lag_group_id].active_members_num = active_ldpid_num;

    /* set the members  for group */
    for (i = 0; i < member_port_count; i++) {
        lpid = PORT_ID(member_ports[i]);
        members_bmp |= (ca_uint64) 1 << lpid;
    }

    ret = aal_port_trunk_group_member_set(device_id, lag_group_id, members_bmp);
    if (ret != CA_E_OK) {
        goto end;
    }
    __port_lag_attr[lag_group_id].members_bmp = members_bmp;
    __port_lag_attr[lag_group_id].valid = 1;

end:
    __PORT_UNLOCK(device_id);
    return ret;

}

ca_status_t ca_port_link_aggregation_group_update(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint8_t lag_group_id,
    CA_IN ca_uint8_t member_port_count,
    CA_IN ca_port_id_t *member_ports,
    CA_IN ca_classifier_key_mask_t *lag_hash_keys)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t i = 0;
    ca_uint32_t lpid = AAL_LPORT_INVALID;
    ca_uint8_t group_id = 0;
    ca_uint64_t members_bmp = 0;
    ca_eth_port_link_status_t status = CA_PORT_LINK_UP;
    ca_uint32_t active_ldpid_num = 0;
    ca_uint32_t active_ldpid[CA_PORT_MAX_NUM_TRUNK_MEMBER];

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_LAG_ID_CHECK(lag_group_id);

    if (member_port_count > CA_PORT_MAX_NUM_TRUNK_MEMBER)
        return CA_E_PARAM;

    if (!__port_lag_attr[lag_group_id].valid) {
        return CA_E_NOT_FOUND;
    }

    __PORT_LOCK(device_id);
    ret = __port_lag_hash_set(device_id, lag_group_id, lag_hash_keys);
    if (ret != CA_E_OK) {
        goto end;
    }
    /* get the active ports */
    memset(active_ldpid, 0, sizeof(active_ldpid));
    for (i = 0; i < member_port_count; i++) {

        lpid = PORT_ID(member_ports[i]);
        group_id = __port_lag_member_of_group(lpid);
        if(group_id != lag_group_id && group_id != 0xff){
            ca_printf("port 0x%x is a member of group %d\r\n", member_ports[i], group_id);
            ret = CA_E_CONFLICT;
            goto end;
        }

        if (CA_PORT_IS_ETHERNET(member_ports[i])) {    /* only suuport ethernet port aggregation */
            ret = ca_eth_port_link_status_get(device_id, member_ports[i], &status);
            if (ret != CA_E_OK || status != CA_PORT_LINK_UP) {
                continue;
            }

            active_ldpid[active_ldpid_num] = PORT_ID(member_ports[i]);
            active_ldpid_num++;
        }

    }

    if (active_ldpid_num != __port_lag_attr[lag_group_id].active_members_num
            || memcmp(__port_lag_attr[lag_group_id].active_members,
                      active_ldpid, active_ldpid_num * sizeof(ca_port_id_t))) {
        /* only add the linkup ports to the dest map */
        ret = __port_lag_dest_map_update(device_id, lag_group_id, active_ldpid_num, active_ldpid);
        if (ret != CA_E_OK) {
            goto end;
        }
        memset(__port_lag_attr[lag_group_id].active_members, 0,
               CA_PORT_MAX_NUM_TRUNK_MEMBER * sizeof(ca_uint32_t));
        memcpy(__port_lag_attr[lag_group_id].active_members,
               active_ldpid, active_ldpid_num * sizeof(ca_uint32_t));
        __port_lag_attr[lag_group_id].active_members_num = active_ldpid_num;
    }

    /* set the members for group */
    for (i = 0; i < member_port_count; i++) {
        lpid = PORT_ID(member_ports[i]);
        members_bmp |= (ca_uint64) 1 << lpid;
    }
    ret = aal_port_trunk_group_member_set(device_id, lag_group_id, members_bmp);
    if (ret != CA_E_OK) {
        goto end;
    }
    __port_lag_attr[lag_group_id].members_bmp = members_bmp;
end:
    __PORT_UNLOCK(device_id);
    return ret;

}

ca_status_t ca_port_link_aggregation_group_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint8_t lag_group_id,
    CA_OUT ca_uint8_t *member_port_count,
    CA_OUT ca_port_id_t *member_ports,
    CA_OUT ca_classifier_key_mask_t *lag_hash_keys)
{
    ca_uint32_t i = 0;
    ca_uint32_t count = 0;
    ca_port_id_t port_id = 0;
    ca_uint64_t members_bmp = 0;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_LAG_ID_CHECK(lag_group_id);

    CA_ASSERT_RET(member_port_count, CA_E_PARAM);
    CA_ASSERT_RET(member_ports, CA_E_PARAM);
    CA_ASSERT_RET(lag_hash_keys, CA_E_PARAM);

    if (!__port_lag_attr[lag_group_id].valid) {
        return CA_E_NOT_FOUND;
    }

    __port_lag_hash_get(device_id, lag_group_id, lag_hash_keys);

    aal_port_trunk_group_member_get(device_id, lag_group_id, &members_bmp);
    for (i = 0; i < AAL_PORT_NUM_LOGICAL; i++) {
        if ((members_bmp & (ca_uint64) 1 << i) != 0) {
            port_id = ca_port_id_get(i);
            member_ports[count] = port_id;
            count++;
        }
    }
    *member_port_count = count;
    return CA_E_OK;
}

ca_status_t ca_port_link_aggregation_group_delete(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint8_t lag_group_id)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t active_ports_num = 0;
    ca_classifier_key_mask_t lag_hash_keys;
    ca_uint32_t active_ldpid[CA_PORT_MAX_NUM_TRUNK_MEMBER];

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_LAG_ID_CHECK(lag_group_id);

    if (!__port_lag_attr[lag_group_id].valid) {
        return CA_E_NOT_FOUND;
    }

    __PORT_LOCK(device_id);

    /* note: clear the group member before hash key and dest maping table clear */
    ret = aal_port_trunk_group_member_set(device_id, lag_group_id, 0);
    if (ret != CA_E_OK) {
        ca_printf("aal_port_trunk_group_member_set fail, ret %d \r\n", ret);
        goto end;
    }
    memset(&lag_hash_keys, 0, sizeof(ca_classifier_key_mask_t));
    ret = __port_lag_hash_set(device_id, lag_group_id, &lag_hash_keys);
    if (ret != CA_E_OK) {
        ca_printf("__port_lag_hash_set fail, ret %d \r\n", ret);
        goto end;
    }
    memset(active_ldpid, 0, sizeof(active_ldpid));
    ret = __port_lag_dest_map_update(device_id, lag_group_id, active_ports_num, active_ldpid);
    if (ret != CA_E_OK) {
        ca_printf("__port_lag_dest_map_update fail, ret %d \r\n", ret);
        goto end;
    }
    memset(&__port_lag_attr[lag_group_id], 0, sizeof(__port_lag_attr_t));

end:
    __PORT_UNLOCK(device_id);
    return ret;

}

static ca_status_t __port_spcl_pkt_init(ca_device_id_t device_id)
{
    ca_status_t        ret = CA_E_OK;
    aal_ilpb_cfg_msk_t igr_msk;
    aal_ilpb_cfg_t     igr_cfg;
    ca_uint32_t        lport = 0;


    memset(&igr_msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&igr_cfg, 0, sizeof(aal_ilpb_cfg_t));
    igr_msk.s.spcl_pkt_idx = 1;

    for (lport = 0; lport < AAL_PORT_NUM_LOGICAL; ++lport) {

        if (__PORT_IS_WAN(lport))
            igr_cfg.spcl_pkt_idx =  __PORT_SPB_WAN;
        else if (__PORT_IS_LAN(lport))
            igr_cfg.spcl_pkt_idx =  __PORT_SPB_LAN;
#if 0
        else if (__PORT_IS_CPU(lport))
            igr_cfg.spcl_pkt_idx =  __PORT_SPB_LAN;
        else if (lport == AAL_LPORT_L3_WAN)
            igr_cfg.spcl_pkt_idx =  __PORT_SPB_WAN;
        else if (lport == AAL_LPORT_L3_LAN)
            igr_cfg.spcl_pkt_idx =  __PORT_SPB_LAN;
#endif
        else
            igr_cfg.spcl_pkt_idx =  __PORT_SPB_RSVD;

        ret = aal_port_ilpb_cfg_set(device_id, lport, igr_msk, &igr_cfg);
        if (CA_E_OK != ret) {
            ca_printf("aal_port_ilpb_cfg_set port %u fail, return %#x.\n", lport, ret);
            break;
        }
    }

    return ret;

}

ca_status_t ca_port_tx_traffic_block_set(CA_IN ca_device_id_t device_id,
                                                 CA_IN ca_port_id_t port_id,
                                                 CA_IN ca_boolean_t  enable
                                         )
{
    ca_uint32_t logic_port_id = 0;
    ca_uint32_t new_phy_port_id = 0;
    ca_status_t ret = CA_E_OK;

    CA_DEV_ID_CHECK(device_id);
    //CA_PORT_ID_CHECK(port_id);

    logic_port_id = PORT_ID(port_id);
    if(enable){
        new_phy_port_id = AAL_PPORT_BLACKHOLE;
    }else{
        if(logic_port_id <= AAL_LPORT_ETH_NI7){
            new_phy_port_id = logic_port_id;
        }else if(logic_port_id <= AAL_LPORT_9QUEUE_NI7){
            new_phy_port_id = AAL_PPORT_OAM;
        }else if(logic_port_id <= AAL_LPORT_CPU_7){
            new_phy_port_id = AAL_PPORT_CPU;
        }else if(logic_port_id == AAL_LPORT_L3_WAN){
            new_phy_port_id = AAL_PPORT_L3_WAN;
        }else if(logic_port_id == AAL_LPORT_L3_LAN){
            new_phy_port_id = AAL_PPORT_L3_LAN;
        }else if(logic_port_id == AAL_LPORT_MC){
            new_phy_port_id = AAL_PPORT_MC;
        }else if(logic_port_id == AAL_LPORT_OAM){
            new_phy_port_id = AAL_PPORT_OAM;
        }
    }
    return aal_port_arb_ldpid_pdpid_map_set(device_id,0,0,logic_port_id,new_phy_port_id);
}

ca_status_t ca_port_tx_traffic_block_get(CA_IN ca_device_id_t device_id,
                                         CA_IN ca_port_id_t port_id,
                                         CA_OUT ca_boolean_t  *enable
                                         )
{
    ca_uint32_t  logic_port_id = 0;
    ca_uint32_t  phy_port_id = 0;
    ca_status_t ret = CA_E_OK;

    CA_DEV_ID_CHECK(device_id);
    CA_PORT_ID_CHECK(port_id);
    if(enable == NULL){
        return CA_E_PARAM;
    }

    logic_port_id = PORT_ID(port_id);
    ret = aal_port_arb_ldpid_pdpid_map_get(device_id,0,0,logic_port_id,&phy_port_id);
    if(CA_E_OK != ret){
        return ret;
    }
    if(phy_port_id == AAL_PPORT_BLACKHOLE){
        *enable = TRUE;
    }else{
        *enable = FALSE;
    }
    return CA_E_OK;
}

ca_status_t ca_port_egr_qm_block_set(CA_IN ca_device_id_t device_id,
                                                 CA_IN ca_port_id_t port_id,
                                                 CA_IN ca_boolean_t  enable
                                            )
{
    ca_uint32_t logic_port_id = 0;
    ca_uint32_t phy_port_id = 0;
    ca_status_t ret = CA_E_OK;

    CA_DEV_ID_CHECK(device_id);
   // CA_PORT_ID_CHECK(port_id);

    logic_port_id = PORT_ID(port_id);
    if(logic_port_id <= AAL_LPORT_ETH_NI7){
        phy_port_id = logic_port_id;
    }else if(logic_port_id <= AAL_LPORT_9QUEUE_NI7){
        phy_port_id = AAL_PPORT_OAM;
    }else if(logic_port_id <= AAL_LPORT_CPU_7){
        phy_port_id = AAL_PPORT_CPU;
    }else if(logic_port_id == AAL_LPORT_L3_WAN){
        phy_port_id = AAL_PPORT_L3_WAN;
    }else if(logic_port_id == AAL_LPORT_L3_LAN){
        phy_port_id = AAL_PPORT_L3_LAN;
    }else if(logic_port_id == AAL_LPORT_MC){
        phy_port_id = AAL_PPORT_MC;
    }else if(logic_port_id == AAL_LPORT_OAM){
        phy_port_id = AAL_PPORT_OAM;
    }

    ret = aal_l2_tm_es_port_ena_set(device_id,phy_port_id,enable == 0);
    if(ret != CA_E_OK){
        return ret;
    }
    if(phy_port_id == 7){
        return aal_l3_tm_es_ni_port_ena_set(0,7,enable == 0);
    }
    return CA_E_OK;
}

ca_status_t ca_port_egr_qm_block_get(CA_IN ca_device_id_t device_id,
                                         CA_IN ca_port_id_t port_id,
                                         CA_OUT ca_boolean_t  *enable
                                         )
{
    ca_uint32_t  logic_port_id = 0;
    ca_uint32_t  phy_port_id = 0;
    ca_status_t ret = CA_E_OK;
    ca_boolean_t es_port_ena = FALSE;

    CA_DEV_ID_CHECK(device_id);
    //CA_PORT_ID_CHECK(port_id);
    if(enable == NULL){
        return CA_E_PARAM;
    }

    logic_port_id = PORT_ID(port_id);
    ret = aal_port_arb_ldpid_pdpid_map_get(device_id,0,0,logic_port_id,&phy_port_id);
    if(CA_E_OK != ret){
        return ret;
    }
    ret = aal_l2_tm_es_port_ena_get(device_id,phy_port_id, &es_port_ena);
    *enable = (es_port_ena == 0);

    return CA_E_OK;
}

ca_status_t ca_port_init(CA_IN ca_device_id_t device_id)
{
    ca_status_t ret = CA_E_OK;
    ca_uint8_t wan_lpid = CA_PORT_ID_NI7;
    ca_uint32_t i = 0, port_id = 0;
    //ca_uint64_t bmp = 0;
    //aal_ippb_cfg_msk_t ippb_msk;
    //aal_ippb_cfg_t ippb_cfg;
    aal_ilpb_cfg_msk_t igr_msk;
    aal_ilpb_cfg_t igr_cfg;
    //aal_dpb_cfg_msk_t dpb_msk;
    //aal_dpb_cfg_t dpb_cfg;

    CA_DEV_ID_CHECK(device_id);

    aal_port_init(device_id);

#if 0//yocto
    ca_spin_lock_init(&__port_lock, SPINLOCK_BH);
#else//sd1 uboot for 98f
#endif

    wan_lpid = AAL_LPORT_ETH_WAN;
    memset(&__port_lag_attr, 0, sizeof(__port_lag_attr));
    memset(&__port_tpid_index, 0, sizeof(__port_tpid_index));
    memset(&__port_stp_mode, 0, sizeof(__port_stp_mode));

    for (i = 0; i < CA_PORT_MAX_NUM; i++) {
        __port_stp_mode[i] = TRUE;     /* stp is enbled by default */
    }
    //if (wan_lpid >= CA_PORT_MAX_NUM_ETHERNET) {
        //return CA_E_ERROR;
    //}

    memset(&igr_msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&igr_cfg, 0, sizeof(aal_ilpb_cfg_t));
    igr_msk.s.wan_ind = 1;
    igr_cfg.wan_ind = 1;
    aal_port_ilpb_cfg_set(0, wan_lpid, igr_msk, &igr_cfg);


    ret = __port_spcl_pkt_init(device_id);

#if 0
    for (i = 0; i < CA_PORT_MAX_NUM_ETHERNET; i++) {
        port_id = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, i);
        ret = ca_port_enable_set(0, port_id, 1);
        if(ret != CA_E_OK){
            ca_printf("port enable failed! port_id 0x%x\r\n", port_id);
        }
    }
    /* cross connection setup: init ILPB and DPB table, refer to 7.20.1 in G3 Arch doc */
    /* 1. WAN NI6 (LPORT 0x06) -> L2RP WAN (LPORT 0x09) */
    igr_msk.s.wan_ind = 1;
    igr_msk.s.igr_port_l2uc_redir_en = 1;
    igr_msk.s.igr_port_l2uc_ldpid = 1;
    igr_msk.s.igr_port_l2mc_redir_en = 1;
    igr_msk.s.igr_port_l2mc_ldpid = 1;
    igr_msk.s.igr_port_l2bc_redir_en = 1;
    igr_msk.s.igr_port_l2bc_ldpid = 1;

    igr_cfg.wan_ind = 1;
    igr_cfg.igr_port_l2uc_redir_en = 1;
    igr_cfg.igr_port_l2uc_ldpid = AAL_LPORT_L2RP_WAN;    /* 0x09 */
    igr_cfg.igr_port_l2mc_redir_en = 1;
    igr_cfg.igr_port_l2mc_ldpid = AAL_LPORT_L2RP_WAN;
    igr_cfg.igr_port_l2bc_redir_en = 1;
    igr_cfg.igr_port_l2bc_ldpid = AAL_LPORT_L2RP_WAN;

    ret = aal_port_ilpb_cfg_set(device_id, wan_ni_index, igr_msk, &igr_cfg);    /* by default, wan_ni_index is 0x06 */
    if (ret != CA_E_OK) {
        return ret;
    }

    /* 2. L2RP WAN (LPORT 0x09) -> L3 WAN (LPORT 0x19): enable redirect and set ldpid via ILPB table */
    igr_msk.s.igr_port_l2uc_redir_en = 1;
    igr_msk.s.igr_port_l2uc_ldpid = 1;
    igr_msk.s.igr_port_l2mc_redir_en = 1;
    igr_msk.s.igr_port_l2mc_ldpid = 1;
    igr_msk.s.igr_port_l2bc_redir_en = 1;
    igr_msk.s.igr_port_l2bc_ldpid = 1;

    igr_cfg.igr_port_l2uc_redir_en = 1;
    igr_cfg.igr_port_l2uc_ldpid = AAL_LPORT_L3_WAN;    /* 0x19 */
    igr_cfg.igr_port_l2mc_redir_en = 1;
    igr_cfg.igr_port_l2mc_ldpid = AAL_LPORT_L3_WAN;
    igr_cfg.igr_port_l2bc_redir_en = 1;
    igr_cfg.igr_port_l2bc_ldpid = AAL_LPORT_L3_WAN;

    ret = aal_port_ilpb_cfg_set(device_id, AAL_LPORT_L2RP_WAN, igr_msk, &igr_cfg);    /* 0x09 */
    if (ret != CA_E_OK) {
        return ret;
    }

    /* 3. L2RP LAN (LPORT 0x08) -> L3 LAN (LPORT 0x18): enable redirect and set ldpid via ILPB table */
    igr_msk.s.igr_port_l2uc_redir_en = 1;
    igr_msk.s.igr_port_l2uc_ldpid = 1;
    igr_msk.s.igr_port_l2mc_redir_en = 1;
    igr_msk.s.igr_port_l2mc_ldpid = 1;
    igr_msk.s.igr_port_l2bc_redir_en = 1;
    igr_msk.s.igr_port_l2bc_ldpid = 1;

    igr_cfg.igr_port_l2uc_redir_en = 1;
    igr_cfg.igr_port_l2uc_ldpid = AAL_LPORT_L3_LAN;    /* 0x18 */
    igr_cfg.igr_port_l2mc_redir_en = 1;
    igr_cfg.igr_port_l2mc_ldpid = AAL_LPORT_L3_LAN;
    igr_cfg.igr_port_l2bc_redir_en = 1;
    igr_cfg.igr_port_l2bc_ldpid = AAL_LPORT_L3_LAN;

    ret = aal_port_ilpb_cfg_set(device_id, AAL_LPORT_L2RP_LAN, igr_msk, &igr_cfg);    /* 0x08 */
    if (ret != CA_E_OK) {
        return ret;
    }

    /* set DPB table for the traffic from L3 WAN (0x18), L3 LAN (0x19), OAM, MC, SPECL */
    ret = aal_port_dpb_ctrl_set(device_id, 3);    /* top 3 bits of ldpid 0x18~0x1f (0x011xxx)  = 3 */
    if (ret != CA_E_OK) {
        return ret;
    }

    /* 4. L3 WAN (LPORT 0x18) -> WAN NI (LPORT 0x07): enable L2FE bypass and set ldpid via DPB table */
    memset(&dpb_msk, 0, sizeof(aal_dpb_cfg_msk_t));
    memset(&dpb_cfg, 0, sizeof(aal_dpb_cfg_t));

    dpb_msk.s.bypass = 1;
    dpb_msk.s.ldpid = 1;
    dpb_msk.s.cos = 1;

    dpb_cfg.bypass = 1;
    dpb_cfg.ldpid = AAL_LPORT_ETH_WAN;    /* 0x07 */

    lpid_cos = ((0x011000) << 3) & 0x1f;        /* low 3  bits of lpid 0x18 (0x011000) << 3 | cos (0~7) = 0~7 */
    for (i = 0; i < 7; i++) {
        dpb_cfg.cos = i;
        aal_port_dpb_cfg_set(device_id, lpid_cos + i, dpb_msk, &dpb_cfg);
    }

    /* 5. L3 LAN (LPORT 0x19) -> L2FE: disable L2FE bypass via DPB table */
    memset(&dpb_msk, 0, sizeof(aal_dpb_cfg_msk_t));
    memset(&dpb_cfg, 0, sizeof(aal_dpb_cfg_t));

    dpb_msk.s.bypass = 1;
    dpb_cfg.bypass = 0;

    lpid_cos = ((0x011001) << 3) & 0x1f;        /* low 3  bits of lpid 0x19 (0x011001) << 3 | cos (0~7) = 8~15 */
    for (i = 0; i < 7; i++) {
        aal_port_dpb_cfg_set(device_id, lpid_cos + i, dpb_msk, &dpb_cfg);
    }
 #endif
    return CA_E_OK;

}


ca_status_t  ca_dp_init(ca_device_id_t device_id)
{
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    aal_port_arb_ldpid_pdpid_map_set(device_id, 0, 0, AAL_LPORT_9QUEUE_NI7, AAL_PPORT_OAM);
    aal_port_arb_ldpid_pdpid_map_set(device_id, 1, 0, AAL_LPORT_9QUEUE_NI7, AAL_PPORT_OAM);
#endif
    //ca_event_register(device_id,  event_id, CA_IN ca_event_cb_ptr event_cb, CA_IN ca_void * cookie)
    return CA_E_OK;
}

EXPORT_SYMBOL(ca_port_link_aggregation_group_delete);
EXPORT_SYMBOL(ca_port_max_frame_size_set);
EXPORT_SYMBOL(ca_port_discard_control_set);
EXPORT_SYMBOL(ca_port_stp_state_set);
EXPORT_SYMBOL(ca_port_pvid_dot1p_get);
EXPORT_SYMBOL(ca_port_isolation_set);
EXPORT_SYMBOL(ca_port_station_move_set);
EXPORT_SYMBOL(ca_port_ipg_get);
EXPORT_SYMBOL(ca_port_link_aggregation_group_create);
EXPORT_SYMBOL(ca_port_tpid_set);
EXPORT_SYMBOL(ca_port_pvid_vlan_set);
EXPORT_SYMBOL(ca_port_max_frame_size_get);
EXPORT_SYMBOL(ca_port_discard_control_get);
EXPORT_SYMBOL(ca_port_untagged_priority_get);
EXPORT_SYMBOL(ca_port_loopback_get);
EXPORT_SYMBOL(ca_port_stp_state_get);
EXPORT_SYMBOL(ca_port_pvid_dot1p_set);
EXPORT_SYMBOL(ca_port_pvid_tpid_get);
EXPORT_SYMBOL(ca_port_ipg_set);
EXPORT_SYMBOL(ca_port_station_move_get);
EXPORT_SYMBOL(ca_port_enable_get);
EXPORT_SYMBOL(ca_port_link_aggregation_group_update);
EXPORT_SYMBOL(ca_port_tpid_get);
EXPORT_SYMBOL(ca_port_loopback_set);
EXPORT_SYMBOL(ca_port_untagged_priority_set);
EXPORT_SYMBOL(ca_port_enable_set);
EXPORT_SYMBOL(ca_port_pvid_tpid_set);
EXPORT_SYMBOL(ca_port_pvid_vlan_get);
EXPORT_SYMBOL(ca_port_isolation_get);
EXPORT_SYMBOL(ca_port_link_aggregation_group_get);
EXPORT_SYMBOL(ca_port_tx_traffic_block_set);
EXPORT_SYMBOL(ca_port_tx_traffic_block_get);
EXPORT_SYMBOL(ca_port_egr_qm_block_set);
EXPORT_SYMBOL(ca_port_egr_qm_block_get);

