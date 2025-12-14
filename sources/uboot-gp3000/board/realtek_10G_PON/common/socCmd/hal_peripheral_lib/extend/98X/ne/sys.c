#if 0//yocto
#include <generated/ca_ne_autoconf.h>
#else//sd1 uboot
#include "ca_ne_autoconf.h"
#endif
#include "sys.h"
#include "osal_common.h"
#include "ca_aal.h"
#include "aal_l3qm.h"
#include "aal_common.h"
#include "scfg.h"
#include "port.h"

#include "pkt_tx.h"
#include "peri.h"
#include "aal_l3fe.h"

extern ca_status_t  ca_dp_init(/*CODEGEN_IGNORE_TAG*/CA_IN ca_device_id_t device_id);

extern ca_status_t ca_eth_ports_restore(ca_device_id_t device_id);
extern ca_status_t ca_eth_port_init(ca_device_id_t device_id, ca_init_config_t * p_cfg);
extern ca_status_t ca_eth_ports_enable(ca_device_id_t device_id, ca_boolean_t enable);
extern ca_status_t ca_port_init(ca_device_id_t device_id);
extern ca_status_t ca_special_packet_init(ca_device_id_t device_id, ca_init_config_t * p_cfg);
extern ca_status_t ca_qos_init(ca_device_id_t device_id, ca_init_config_t * p_cfg);
extern ca_status_t ca_l2_fdb_init(ca_device_id_t device_id);
extern ca_status_t ca_l2_vlan_init(ca_device_id_t device_id);
extern void ca_l2_classifier_init(void);

extern ca_status_t ca_l3_intf_init(ca_device_id_t device_id);
extern ca_status_t ca_l3_route_init(ca_device_id_t device_id);
extern ca_status_t ca_tunnel_init(ca_device_id_t device_id);
extern ca_status_t ca_nat_init(ca_device_id_t device_id);
extern ca_status_t ca_flow_init(ca_device_id_t device_id);
extern void ca_ni_init_reserved_mem(void);
extern int ca_ni_init_l3qm(void);
extern void ca_ni_init_fbm(void);
extern ca_status_t ca_tx_rx_init(void);


#ifdef CONFIG_CA_NE_SW_SHAPER
ca_status_t ca_sw_shaper_init(void);
#endif

#ifdef CONFIG_CA_NE_US_RATE_MANAGER
ca_status_t ca_us_rate_manager_init(void);
#endif

ca_status_t ca_init (
    CA_IN    ca_device_id_t        device_id,
    CA_IN    ca_init_config_t    *p_cfg,
    CA_OUT    ca_init_err_t        *err_code
)
{
    ca_status_t ret = CA_E_OK;
    ca_g3_init_config_t *p_g3_cfg = (ca_g3_init_config_t *) p_cfg;
    ca_init_err_t err = CA_INIT_SRAM_FAIL;

#if 0//sd1 uboot for 98f - TBD
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
	ca_macsec_global_config_t macsec_glb_cfg;
#endif
    /* load statup config and initialize reserved memory for L3QM */
    ca_ni_init_reserved_mem();
#endif

    /* should be for all of L2/L3 init */
    if ((ret = aal_init(device_id)) != CA_E_OK) {
        ca_printf("%s:%d aal_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
        goto end_ca_init;
    }

#if 0//yocto
#if CONFIG_98F_UBOOT_NE_DBG//yocto test
ca_printf("%s(%d) %s\tp_g3_cfg->ca_ni_mgmt_mode: 0x%x\n", __func__, __LINE__, __FILE__, p_g3_cfg->ca_ni_mgmt_mode);//yocto: 0x0
ca_printf("%s(%d) %s\tp_g3_cfg->ca_ne_port_id: 0x%x\n", __func__, __LINE__, __FILE__, p_g3_cfg->ca_ne_port_id);//yocto: 0x3
#endif
#else//sd1 uboot for 98f
    p_g3_cfg->ca_ni_mgmt_mode = 0;
#endif//sd1 uboot for 98f

    aal_ni_init_ni(p_g3_cfg->ca_ni_mgmt_mode, p_g3_cfg->ca_ne_port_id);

#if 0//sd1 uboot for 98f - TBD
    aal_ni_init_interrupt_cfg(p_g3_cfg->ca_ni_mgmt_mode);

    /* L3QM should be initialzed after port initialized */
    //ca_ni_init_l3qm();
    //ca_ni_init_fbm();
#endif//sd1 uboot for 98f - TBD

    ret = ca_eth_port_init(device_id, p_cfg);
    if (CA_E_OK != ret) {
        ca_printf("%s:%d ca_eth_port_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
        goto end_ca_init;
    }

    ret = ca_eth_ports_enable(device_id, FALSE);

    if (CA_E_OK != ret) {
        ca_printf("%s:%d ca_eth_ports_enable() returns error, ret = %d.\n", __func__, __LINE__, ret);
        goto end_ca_init;
    }

    ret = ca_port_init(device_id);
    if (CA_E_OK != ret) {
        ca_printf("%s:%d ca_port_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
        goto end_ca_init;
    }

    ret = ca_l2_fdb_init(device_id);
    if (CA_E_OK != ret) {
        ca_printf("%s:%d ca_l2_fdb_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
        goto end_ca_init;
    }

#if 0//sd1 uboot for 98f - TBD
    ret = ca_qos_init(device_id, p_cfg);
    if (CA_E_OK != ret) {
        ca_printf("%s:%d ca_qos_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
        goto end_ca_init;
    }
#endif//sd1 uboot for 98f - TBD

#if 1//dbg
    ret = ca_l2_vlan_init(device_id);
    if (CA_E_OK != ret) {
        ca_printf("%s:%d ca_l2_vlan_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
        goto end_ca_init;
    }
#endif//dbg

#if 0//sd1 uboot for 98f - TBD
    // TODO: Should check if L3FE is enabled; L3FE also contains special packet
    ret = ca_special_packet_init(device_id, p_cfg);
    if (CA_E_OK != ret) {
        ca_printf("%s:%d ca_special_packet_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
        goto end_ca_init;
    }
#if 0 /* move to last */
    ret = ca_eth_ports_enable(device_id, TRUE);
    if (CA_E_OK != ret) {
        ca_printf("%s:%d ca_eth_ports_enable() returns error, ret = %d.\n", __func__, __LINE__, ret);
        goto end_ca_init;
    }
#endif
    ca_l2_classifier_init();

    ca_ni_init_l3qm();
    ca_ni_init_fbm();

    if (SOC_HAS_L3FE()) {
#ifndef CONFIG_ARCH_CORTINA_SATURN_SFU
        if ((ret = ca_tunnel_init(device_id)) != CA_E_OK) {
            ca_printf("%s:%d ca_tunnel_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
            goto end_ca_init;
        }

        ret = ca_l3_intf_init(device_id);
        if (CA_E_OK != ret) {
            ca_printf("%s:%d ca_l3_intf_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
            goto end_ca_init;
        }

        ret = ca_l3_route_init(device_id);
        if (CA_E_OK != ret) {
            ca_printf("%s:%d ca_l3_route_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
            goto end_ca_init;
        }

        ret = ca_nat_init(device_id);
        if (CA_E_OK != ret) {
            ca_printf("%s:%d ca_nat_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
            goto end_ca_init;
        }

        ret = ca_flow_init(device_id);
        if (CA_E_OK != ret) {
            ca_printf("%s:%d ca_flow_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
            goto end_ca_init;
        }
#endif
    }

#if defined(CONFIG_ARCH_CORTINA_G3HGU) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    ret = ca_dp_init(device_id);
    if(CA_E_OK != ret) {
        ca_printf("%s:%d ca_dp_init() returns error, ret = %d\n",__func__, __LINE__, ret);
        goto end_ca_init;
    }

    /* Saturn PON init */
    ret = ca_pon_init(device_id);
    if(CA_E_OK != ret) {
        ca_printf("%s:%d ca_pon_init() returns error, ret = %d\n",__func__, __LINE__, ret);
        goto end_ca_init;
    }

    /* register packet tx handler */
    ca_printf("<%s, %d>ca_tx:%p\r\n", __FUNCTION__, __LINE__, ca_tx);
    ret = ca_pon_tx_register(ca_tx);
    if(CA_E_OK != ret) {
        ca_printf("%s:%d ca_pon_init() returns error, ret = %d\n",__func__, __LINE__, ret);
        goto end_ca_init;
    }

#endif

#ifdef CONFIG_CA_NE_SW_SHAPER
    ret = ca_sw_shaper_init();
    if (CA_E_OK != ret) {
        ca_printf("%s:%d ca_sw_shaper_init() return error, ret = %d\n", __func__, __LINE__, ret);
        goto end_ca_init;
    }
#endif
#ifdef CONFIG_CA_NE_US_RATE_MANAGER
    ret = ca_us_rate_manager_init();
    if (CA_E_OK != ret) {
        ca_printf("%s:%d ca_us_rate_manager_init() return error, ret = %d\n", __func__, __LINE__, ret);
        goto end_ca_init;
    }
#endif
#endif//sd1 uboot for 98f - TBD

    /* do not enable ethernet ports let NI driver enable it */
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3LITE)
    ret = ca_eth_ports_enable(device_id, TRUE);
    if(CA_E_OK != ret) {
        ca_printf("%s:%d ca_eth_ports_enable() returns error, ret = %d\n",__func__, __LINE__, ret);
        goto end_ca_init;
    }

#elif defined(CONFIG_ARCH_CORTINA_SATURN_SFU)

	//ca_pon_policer_callback = ca_flow_policer_set;
#if 0
	memset(&macsec_glb_cfg, 0 ,sizeof(ca_macsec_global_config_t));
	macsec_glb_cfg.fwd_untagged_oam = 1;
	macsec_glb_cfg.fwd_untagged_mpcp = 1;
	macsec_glb_cfg.fwd_untagged_ethertype= 0;
	macsec_glb_cfg.co_error = 0;
	macsec_glb_cfg.dp_reset = 0;
	macsec_glb_cfg.KaYReserved = 1;
	macsec_glb_cfg.validateFrames = 2;

	macsec_glb_cfg.skip_mpcp = 1;
	macsec_glb_cfg.skip_oam = 1;
	macsec_glb_cfg.skip_untagged_ethertype = 0;
	macsec_glb_cfg.untagged_ethertype = 0;

	ret = ca_macsec_init(device_id,&macsec_glb_cfg);
    ca_printf("ca-ne: ca_macsec_init ... %s\n", ret ? "FAIL" : "OK");

	if (CA_E_OK != ret) {
		ca_printf("%s:%d ca_macsec_init() returns error, ret = %d.\n", __func__, __LINE__, ret);
		goto end_ca_init;
	}
#endif

    ret = ca_eth_ports_restore(device_id);
    if(CA_E_OK != ret) {
        ca_printf("%s:%d ca_eth_ports_restore() returns error, ret = %d\n",__func__, __LINE__, ret);
        goto end_ca_init;
    }
#endif

#if 0//sd1 uboot for 98f - TBD
    /* init routine for kernel mode ca_tx and ca_rx */
    ca_tx_rx_init();
#endif//sd1 uboot for 98f - TBD

end_ca_init:

    if (err_code != NULL) *err_code = err;
    return ret;
}

ca_status_t ca_reset (
    CA_IN    ca_device_id_t        device_id,
    CA_IN    ca_sys_reset_mode_t    mode
)
{
    cap_watchdog_config_t wdt_cfg;
    /*ONLY Support watchdog reset*/
    wdt_cfg.enable = 1;
    wdt_cfg.threshold = 7;
    wdt_cfg.action = CAP_WATCHDOG_ACTION_REBOOT;
    cap_watchdog_set(device_id, &wdt_cfg);

    return CA_E_OK;
}

#if 0//yocto
ca_status_t ca_info_get (
    CA_IN    ca_device_id_t        device_id,
    CA_OUT    ca_info_t        *info
)
{
    int ret;
    aal_info_t aal_info;
    ca_uint32_t api_version;

    if (info == NULL)
        return CA_E_PARAM;

    ret = aal_hw_info_get(&aal_info);
    if (ret != AAL_E_OK)
        printf("%s: Fail to get HW info. (ret=%d)\n", __func__, ret);

    ret = aal_scfg_read(CFG_ID_API_VERSION, sizeof(api_version), &api_version);
    if (ret != CA_E_OK)
        printf("%s: Fail to read CFG_ID_API_VERSION. (ret=%d)\n", __func__, ret);

    info->hardware_id = aal_info.hardware_id;
    info->hardware_rev = aal_info.hardware_rev;
    info->hardware_date = aal_info.hardware_date;
    info->api_version = api_version;

    return CA_E_OK;
}
#else//sd1 uboot for 98f
#endif

ca_status_t ca_info_set (
    CA_IN    ca_device_id_t        device_id,
    CA_OUT    ca_info_t        *info
)
{
    return CA_E_OK;
}

EXPORT_SYMBOL(ca_init);
EXPORT_SYMBOL(ca_reset);
EXPORT_SYMBOL(ca_info_get);
EXPORT_SYMBOL(ca_info_set);
