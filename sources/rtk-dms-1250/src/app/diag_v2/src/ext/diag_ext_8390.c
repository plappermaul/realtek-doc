/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those extension command and APIs in the SDK diagnostic shell.
 *
 */

/*
 * Include Files
 */


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/qos.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <private/drv/swcore/swcore_rtl8390.h>
#include <parser/cparser_priv.h>
#include <rtdrv/ext/rtdrv_netfilter_ext_8390.h>
#include <hal/common/halctrl.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/reg.h>
#include <ioal/mem32.h>
#ifdef CONFIG_SDK_MODEL_MODE
#include <virtualmac/vmac_target.h>
#endif
/*
 * SET
 */


/** MIB **/
/*
 * mib set reset-value ( 0 | 1 )
 */
cparser_result_t cparser_cmd_mib_set_reset_value_0_1(cparser_context_t *context)
{
    uint32  unit = 0;
    rtdrv_ext_mibCfg_t cntr_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cntr_cfg.unit = unit;
    switch(TOKEN_CHAR(3,0))
    {
        case '0':
            cntr_cfg.rst_val = 0;
            break;

        case '1':
            cntr_cfg.rst_val = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }
    SETSOCKOPT(RTDRV_EXT_MIB_RST_VAL_SET, &cntr_cfg, rtdrv_ext_mibCfg_t, 1);

    return CPARSER_OK;
}

/*
 *  sqa reset
 */
cparser_result_t cparser_cmd_sqa_reset(cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mac_t mac;
    uint32  addr, val, i, group_idx_max;
    rtk_qos_priSelWeight_t weight;
    rtdrv_ext_switchCfg_t switch_cfg;
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    rtk_enable_t inner_state, outer_state;
#endif
    rtdrv_ext_l2Cfg_t   l2_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    DIAG_UTIL_ERR_CHK(rtk_qos_1pPriRemap_set(unit, 0, 0), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_1pPriRemap_set(unit, 1, 1), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_1pPriRemap_set(unit, 2, 2), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_1pPriRemap_set(unit, 3, 3), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_1pPriRemap_set(unit, 4, 4), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_1pPriRemap_set(unit, 5, 5), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_1pPriRemap_set(unit, 6, 6), ret);
    DIAG_UTIL_ERR_CHK(rtk_qos_1pPriRemap_set(unit, 7, 7), ret);
#endif

    DIAG_UTIL_ERR_CHK(rtk_switch_pppoeIpParseEnable_set(unit, ENABLED), ret);

    /* reset switch MAC address */
    mac.octet[0] = 0x0;
    mac.octet[1] = 0x11;
    mac.octet[2] = 0x83;
    mac.octet[3] = 0x89;
    mac.octet[4] = 0x23;
    mac.octet[5] = 0x79;
    DIAG_UTIL_ERR_CHK(rtk_switch_mgmtMacAddr_set(unit, &mac), ret);

    /* enable L2 CAM */
    ioal_mem32_read(unit, RTL8390_L2_CTRL_0_ADDR, &val);
    ioal_mem32_write(unit, RTL8390_L2_CTRL_0_ADDR, (val | RTL8390_L2_CTRL_0_LUTCAM_EN_MASK));
    l2_cfg.unit = 0;
    l2_cfg.enable = ENABLED;
    SETSOCKOPT(RTDRV_EXT_L2_CMA_ENABLE_SET, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    /* set backpressure method to be JAM mode */
    ioal_mem32_read(unit, RTL8390_MAC_GLB_CTRL_ADDR, &val);
    val &= ~RTL8390_MAC_GLB_CTRL_BKPRES_MTHD_SEL_MASK;
    ioal_mem32_write(unit, RTL8390_MAC_GLB_CTRL_ADDR, val);

    /* adjust priority selection weight */
    osal_memset((void *)&weight,0x0,sizeof(rtk_qos_priSelWeight_t));
    weight.weight_of_innerTag = 1;
    DIAG_OM_GET_CHIP_CAPACITY(unit, group_idx_max, pri_sel_group_index_max);
    for (i=0; i<= group_idx_max; i++)
        rtk_qos_priSelGroup_set(unit, 0, &weight);

    DIAG_UTIL_ERR_CHK(rtk_qos_queueNum_set(unit, 8), ret);

    ioal_mem32_write(unit, RTL8390_FC_DROP_THR_ADDR, 0xfff);
    ioal_mem32_write(unit, RTL8390_FC_GLB_HI_THR_ADDR, 0x07780746);
    ioal_mem32_write(unit, RTL8390_FC_GLB_LO_THR_ADDR, 0x03520320);
    ioal_mem32_write(unit, RTL8390_FC_GLB_FCOFF_HI_THR_ADDR, 0x07780746);
    ioal_mem32_write(unit, RTL8390_FC_GLB_FCOFF_LO_THR_ADDR, 0x03520320);
    addr = RTL8390_FC_P_HI_THR_ADDR(0);
    ioal_mem32_write(unit, addr, 0x012c00fa);
    addr = RTL8390_FC_P_LO_THR_ADDR(0);
    ioal_mem32_write(unit, addr, 0x00140014);
    addr = RTL8390_FC_P_FCOFF_HI_THR_ADDR(0);
    ioal_mem32_write(unit, addr, 0x012c00fa);

    /* enable all ports */
    for (i=0; i<= 51; i++)
        rtk_port_adminEnable_set(0, i, ENABLED);


#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    /* vlan set egress port all inner keep-tag state enable
       vlan set ingress port all inner keep-tag state enable */
    for (i=0; i<= 51; i++)
    {
        rtk_vlan_portIgrTagKeepEnable_get(unit, i, &outer_state, &inner_state);
        rtk_vlan_portIgrTagKeepEnable_set(unit, i, outer_state, ENABLED);

        rtk_vlan_portEgrTagKeepEnable_get(unit, i, &outer_state, &inner_state);
        rtk_vlan_portEgrTagKeepEnable_set(unit, i, outer_state, ENABLED);
    }
#endif

    /* Disable 48pass1 */
    ioal_mem32_read(unit, RTL8390_MAC_GLB_CTRL_ADDR, &val);
    val &= ~RTL8390_MAC_GLB_CTRL_HALF_48PASS1_EN_MASK;
    ioal_mem32_write(unit, RTL8390_MAC_GLB_CTRL_ADDR, val);

    /* Command: switch set limit-pause state disable */
    switch_cfg.unit = unit;
    switch_cfg.limit_pause = DISABLED;
    SETSOCKOPT(RTDRV_EXT_SWITCH_LIMITPAUSE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    /* Command: switch set interrupt link-change state enable */
//    SETSOCKOPT(RTDRV_EXT_SWITCH_INTR_LINK_CHANGE_ENABLE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    /* Set pppoe-IP-parse = disable */
    rtk_switch_pppoeIpParseEnable_set(unit, DISABLED);

    /* Set inner-tag remarking source = internal-priority */
    rtk_qos_1pRemarkSrcSel_set(unit, PRI_SRC_INT_PRI);

    /* Set DSCP remarking source = internal-priority */
    rtk_qos_dscpRemarkSrcSel_set(unit, PRI_SRC_INT_PRI);

    /* Set default outer-tag priority = copy-internal-priority */
    for (i=0; i<= 52; i++)
    {
        rtk_qos_portOuter1pDfltPriSrcSel_set(unit, i, OUTER_1P_DFLT_SRC_INT_PRI);
    }

    return CPARSER_OK;
}

/** SWITCH **/
cparser_result_t cparser_cmd_switch_set_48_pass_1_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    switch_cfg.half_48pass1 = enable;
    SETSOCKOPT(RTDRV_EXT_SWITCH_48PASS1_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    return CPARSER_OK;
}

/*
 * switch set limit-pause state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_limit_pause_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    switch_cfg.limit_pause = enable;
    SETSOCKOPT(RTDRV_EXT_SWITCH_LIMITPAUSE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    return CPARSER_OK;
}

/*
 *  switch set ipg-compensation ( ge | 10g ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_ipg_compensation_ge_10g_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(5,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch(TOKEN_CHAR(3,0))
    {
        case 'g': /* ge */
            switch_cfg.unit = unit;
            switch_cfg.ipg_cmpstn = enable;
            SETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        case '1': /* 10g */
            switch_cfg.unit = unit;
            switch_cfg.ipg_cmpstn = enable;
            SETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 *  switch set ipg-compensation ( ge | 10g ) ( 65ppm | 90ppm )
 */
cparser_result_t cparser_cmd_switch_set_ipg_compensation_ge_10g_65ppm_90ppm(cparser_context_t *context)
{
    uint32 unit;
    uint32 ipg_cmpstn_sel = 1;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case '6':
            ipg_cmpstn_sel = 0;
            break;

        case '9':
            ipg_cmpstn_sel = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch(TOKEN_CHAR(3,0))
    {
        case 'g': /* ge */
            switch_cfg.unit = unit;
            switch_cfg.ipg_cmpstn_sel = ipg_cmpstn_sel;
            SETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_SEL_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        case '1': /* 10g */
            switch_cfg.unit = unit;
            switch_cfg.ipg_cmpstn_sel = ipg_cmpstn_sel;
            SETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SEL_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 *  switch set ipg-min-length port ( <PORT_LIST:ports> | all ) length <UINT:len>
 */
cparser_result_t cparser_cmd_switch_set_ipg_min_length_port_ports_all_length_len(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *len_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    rtdrv_ext_switchCfg_t switch_cfg;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.unit = unit;
        switch_cfg.port = port;
        switch_cfg.data = *len_ptr;
        SETSOCKOPT(RTDRV_EXT_SWITCH_IPGMINLEN_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 *  switch set interrupt link-change state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_interrupt_link_change_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch_cfg.unit = unit;
    switch (TOKEN_CHAR(5,0))
    {
        case 'd':
            SETSOCKOPT(RTDRV_EXT_SWITCH_INTR_LINK_CHANGE_DISABLE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            break;

        default:
            SETSOCKOPT(RTDRV_EXT_SWITCH_INTR_LINK_CHANGE_ENABLE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_switch_set_back_pressure_jam_defer(cparser_context_t *context)
{
    uint32 unit;
    uint32 bkpres = 1;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'j':
            bkpres = 0;
            break;

        case 'd':
            bkpres = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    switch_cfg.bkpres = bkpres;
    SETSOCKOPT(RTDRV_EXT_SWITCH_BKPRES_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    return CPARSER_OK;
}

/*
 * switch set pass-all-mode port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_pass_all_mode_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        switch_cfg.pass_all_mode = enable;
        SETSOCKOPT(RTDRV_EXT_SWITCH_PASSALLMODE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * switch set rx-check-crc port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_rx_check_crc_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        switch_cfg.rx_check_crc = enable;
        SETSOCKOPT(RTDRV_EXT_SWITCH_RXCHECKCRC_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * switch set bypass-tx-crc port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_bypass_tx_crc_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        switch_cfg.bypass_tx_crc = enable;
        SETSOCKOPT(RTDRV_EXT_SWITCH_BYPASSTXCRC_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * switch set padding-under-size port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_padding_under_size_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch_cfg.unit = unit;
    switch_cfg.enable = enable;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_SWITCH_PADDINGUNDSIZE_SET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    }

    return CPARSER_OK;
}

/** QoS **/
/*
 * qos set queue number port ( <PORT_LIST:ports> | all ) <UINT:queue_num>
 */
cparser_result_t cparser_cmd_qos_set_queue_number_port_ports_all_queue_num(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_num_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_qosCfg_t qos_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    qos_cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        qos_cfg.port = port;
        qos_cfg.qnum = *queue_num_ptr - 1;
        SETSOCKOPT(RTDRV_EXT_QOS_PORT_QUEUE_NUM_SET, &qos_cfg, rtdrv_ext_qosCfg_t, 1);
    }

    return CPARSER_OK;
}

/** Port **/
/*
 * port set special-congest drain-out-thresh <UINT:threshold> duplex ( full | half )
 */
cparser_result_t cparser_cmd_port_set_special_congest_drain_out_thresh_threshold_duplex_full_half(
    cparser_context_t *context,
    uint32_t *threshold_ptr)
{
    uint32 unit;
    rtdrv_ext_portCfg_t port_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    port_cfg.unit = unit;

    if (TOKEN_CHAR(6,0) == 'f')
    {
        port_cfg.full_th = *threshold_ptr;
        SETSOCKOPT(RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_SET, &port_cfg, rtdrv_ext_portCfg_t, 1);
    }
    else if (TOKEN_CHAR(6,0) == 'h')
    {
        port_cfg.half_th = *threshold_ptr;
        SETSOCKOPT(RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_HALF_SET, &port_cfg, rtdrv_ext_portCfg_t, 1);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 * port set special-congest port ( <PORT_LIST:ports> | all ) sustain-timer <UINT:second> duplex ( full | half )
 */
cparser_result_t cparser_cmd_port_set_special_congest_port_ports_all_sustain_timer_second_duplex_full_half(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *second_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    uint32 optid;
    diag_portlist_t portlist;
    rtdrv_ext_portCfg_t port_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    port_cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if (TOKEN_CHAR(8,0) == 'f')
    {
        optid = RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_SUSTAIN_TIMER_SET;
        port_cfg.full_sec = *second_ptr;
    }
    else if (TOKEN_CHAR(8,0) == 'h')
    {
        optid = RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_SUSTAIN_TIMER_HALF_SET;
        port_cfg.half_sec = *second_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        port_cfg.port = port;
        SETSOCKOPT(optid, &port_cfg, rtdrv_ext_portCfg_t, 1);
    }

    return CPARSER_OK;
}



/*
 * GET
 */


/** MIB **/
/*
 * mib get reset-value
 */
cparser_result_t cparser_cmd_mib_get_reset_value(cparser_context_t *context)
{
    uint32  unit = 0;
    rtdrv_ext_mibCfg_t cntr_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cntr_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_MIB_RST_VAL_GET, &cntr_cfg, rtdrv_ext_mibCfg_t, 1);

    diag_util_mprintf("\tReset MIB Value : %d \n", cntr_cfg.rst_val);

    return CPARSER_OK;
}


/** SWITCH **/
cparser_result_t cparser_cmd_switch_get_48_pass_1_state(cparser_context_t *context)
{
    uint32 unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_SWITCH_48PASS1_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    enable = switch_cfg.half_48pass1;

    diag_util_mprintf("\t48pass1 State: %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_switch_get_limit_pause_state(cparser_context_t *context)
{
    uint32 unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_SWITCH_LIMITPAUSE_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
    enable = switch_cfg.limit_pause;

    diag_util_mprintf("\tLimit Pause State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}

/*
 *  switch get ipg-compensation ( ge | 10g ) state
 */
cparser_result_t cparser_cmd_switch_get_ipg_compensation_ge_10g_state(cparser_context_t *context)
{
    uint32 unit;
    rtk_enable_t enable = DISABLED;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(3,0))
    {
        case 'g': /* ge */
            switch_cfg.unit = unit;
            GETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            enable = switch_cfg.ipg_cmpstn;
            diag_util_mprintf("\tGE IPG Compensation State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            break;

        case '1': /* 10g */
            switch_cfg.unit = unit;
            GETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_10G_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            enable = switch_cfg.ipg_cmpstn;
            diag_util_mprintf("\t10G IPG Compensation State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 *  switch get ipg-compensation ( ge | 10g )
 */
cparser_result_t cparser_cmd_switch_get_ipg_compensation_ge_10g(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(3,0))
    {
        case 'g': /* ge */
            switch_cfg.unit = unit;
            GETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_SEL_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            diag_util_mprintf("\tGE IPG Compensation : %s\n", (switch_cfg.ipg_cmpstn_sel == 1) ? "90ppm" : "65ppm");
            break;

        case '1': /* 10g */
            switch_cfg.unit = unit;
            GETSOCKOPT(RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SEL_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
            diag_util_mprintf("\t10G IPG Compensation : %s\n", (switch_cfg.ipg_cmpstn_sel == 1) ? "90ppm" : "65ppm");
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

/*
 * switch get ipg-min-length port ( <PORT_LIST:ports> | all ) length
 */
cparser_result_t cparser_cmd_switch_get_ipg_min_length_port_ports_all_length(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    rtdrv_ext_switchCfg_t switch_cfg;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("IPG Minimum Receive Length Configuration\n");
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.unit = unit;
        switch_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_SWITCH_IPGMINLEN_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %d-Byte\n", port, switch_cfg.data);
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_switch_get_back_pressure(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_SWITCH_BKPRES_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    diag_util_mprintf("\tBackpressure : %s\n", (switch_cfg.bkpres == 1) ? "Defer mode" : "Jam mode");

    return CPARSER_OK;
}

/*
 * switch get pass-all-mode port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_switch_get_pass_all_mode_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;


    diag_util_mprintf("Pass All Packets Mode Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_SWITCH_PASSALLMODE_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (switch_cfg.pass_all_mode == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}

/*
 * switch get rx-check-crc port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_switch_get_rx_check_crc_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;


    diag_util_mprintf("RX Check CRC Function Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_SWITCH_RXCHECKCRC_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (switch_cfg.rx_check_crc == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}

/*
 * switch get bypass-tx-crc port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_switch_get_bypass_tx_crc_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;


    diag_util_mprintf("Bypress TX Recalculating CRC Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_SWITCH_BYPASSTXCRC_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (switch_cfg.bypass_tx_crc == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}

/*
 * switch get padding-under-size port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_switch_get_padding_under_size_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_switchCfg_t switch_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;
    diag_util_mprintf("Padding Under Size State\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_SWITCH_PADDINGUNDSIZE_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (switch_cfg.enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}

/*
 * switch get info
 */
cparser_result_t cparser_cmd_switch_get_info(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_switchCfg_t switch_cfg;
    uint32 fpga_date = 0;
    uint32 rtl_version = 0;
    uint32 svn_revision = 0;
    FILE *fp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_SWITCH_INFO_GET, &switch_cfg, rtdrv_ext_switchCfg_t, 1);

    fpga_date = ((switch_cfg.data >> 16) & 0xFFFF);
    rtl_version = (switch_cfg.data & 0xFFFF);

    diag_util_mprintf(" FPGA Date : %04X\n", fpga_date);
    diag_util_mprintf("  RTL Ver. : %u\n", rtl_version);

    /* SVN revision */
    fp = fopen("/etc/revision", "r");
    if (NULL != fp)
    {
        fscanf(fp, "%10u", &svn_revision);
        fclose(fp);
        diag_util_mprintf("  F/W Ver. : %d\n", svn_revision);
    }

    return CPARSER_OK;
}

/** QoS **/
/*
 * qos get queue number port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_qos_get_queue_number_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_qosCfg_t qos_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    qos_cfg.unit = unit;

    diag_util_mprintf("Queue Number\n");
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        qos_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_QOS_PORT_QUEUE_NUM_GET, &qos_cfg, rtdrv_ext_qosCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %d\n", port, qos_cfg.qnum + 1);
    }

    return CPARSER_OK;
}

/*
 * qos get egress-port-rate port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_qos_get_egress_port_rate_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_qosCfg_t qos_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    qos_cfg.unit = unit;

    diag_util_mprintf("Egress Port Rate on Chip\n");
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        qos_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_QOS_PORT_RATE_GET, &qos_cfg, rtdrv_ext_qosCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %d (0x%x)\n", port, qos_cfg.data, qos_cfg.data);
    }

    return CPARSER_OK;
}

/*
 * qos set egress-port-rate port ( <PORT_LIST:ports> | all ) rate <UINT:rate>
 */
cparser_result_t cparser_cmd_qos_set_egress_port_rate_port_ports_all_rate_rate(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *rate_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_qosCfg_t qos_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    qos_cfg.unit = unit;
    qos_cfg.data = *rate_ptr;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        qos_cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_QOS_PORT_RATE_SET, &qos_cfg, rtdrv_ext_qosCfg_t, 1);
    }

    return CPARSER_OK;
}


/** Port **/

/*
 * port get special-congest drain-out-thresh
 */
cparser_result_t cparser_cmd_port_get_special_congest_drain_out_thresh(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_portCfg_t port_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&port_cfg, 0x00, sizeof(rtdrv_ext_portCfg_t));
    port_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_GET, &port_cfg, rtdrv_ext_portCfg_t, 1);
    diag_util_mprintf("\tFull Duplex Drain-Out Threshold : %d\n", port_cfg.full_th);
    diag_util_mprintf("\tHalf Duplex Drain-Out Threshold : %d\n", port_cfg.half_th);

    return CPARSER_OK;
}

/*
 * port get special-congest port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_port_get_special_congest_port_ports_all(cparser_context_t *context, char **ports_ptr)
{
    uint32      unit, port;
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtdrv_ext_portCfg_t port_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&port_cfg, 0x00, sizeof(rtdrv_ext_portCfg_t));
    port_cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        port_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_GET, &port_cfg, rtdrv_ext_portCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tFull Duplex Sustain Timer : %d\n", port_cfg.full_sec);
        diag_util_mprintf("\tHalf Duplex Sustain Timer : %d\n", port_cfg.half_sec);
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_get_tx_wake_mode(cparser_context_t *context)
{
    uint32  unit = 0;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_EEE_TX_WAKE_MODE_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    diag_util_printf("\tEEE TX Wake Mode : ");
    switch(eee_cfg.wakeMode)
    {
        case WAKE_MODE_ANY_PKT_TX:
            diag_util_mprintf("packet-tx\n");
            break;
        case WAKE_MODE_QOS_BASED:
            diag_util_mprintf("qos-based\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;

}

cparser_result_t cparser_cmd_eee_get_port_ports_all_status(cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t portlist;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("EEE or EEEP Status:\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        eee_cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_EEE_PORT_STATUS_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);
        diag_util_mprintf("\tPort %2d :\n", port);
        if (EEE_POWER_WAKE_STATE == eee_cfg.rxState)
            diag_util_mprintf("\t\tRX : Wake\n");
        else if (EEE_POWER_SLEEP_STATE == eee_cfg.rxState)
            diag_util_mprintf("\t\tRX : Sleep\n");

        if (EEE_POWER_WAKE_STATE == eee_cfg.txState)
            diag_util_mprintf("\t\tTX : Wake\n");
        else if (EEE_POWER_SLEEP_STATE == eee_cfg.txState)
            diag_util_mprintf("\t\tTX : Sleep\n");
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_get_link_up_delay(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_EEE_LINK_UP_DELAY_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    diag_util_printf("\tEEE Link Up Delay ");
    switch(eee_cfg.linkUpDelay)
    {
        case 0:
            diag_util_mprintf(": 1024 ms\n");
            break;
        case 1:
            diag_util_mprintf(": 512 ms\n");
            break;
        case 2:
            diag_util_mprintf(": 256 ms\n");
            break;
        case 3:
            diag_util_mprintf(": 1 ms\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_get_multi_wake_state(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_EEE_MULTI_WAKE_STATE_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    diag_util_printf("\tEEE Multi-wake State : ");
    switch(eee_cfg.enable)
    {
        case DISABLED:
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
            break;
        case ENABLED:
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_get_multi_wake_interval(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_EEE_MULTI_WAKE_INTERVAL_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    diag_util_printf("\tEEE Multi-wake Interval : ");
    switch(eee_cfg.interval)
    {
        case 0:
            diag_util_mprintf("1 us\n");
            break;
        case 1:
            diag_util_mprintf("2 us\n");
            break;
        case 2:
            diag_util_mprintf("3 us\n");
            break;
        case 3:
            diag_util_mprintf("4 us\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_get_multi_wake_port_num(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_EEE_MULTI_WAKE_PORT_NUM_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    diag_util_printf("\tEEE Multi-wake Port Number of a Group : ");
    switch(eee_cfg.portNum)
    {
        case 0:
            diag_util_mprintf("2 ports\n");
            break;
        case 1:
            diag_util_mprintf("4 ports\n");
            break;
        case 2:
            diag_util_mprintf("6 ports\n");
            break;
        case 3:
            diag_util_mprintf("8 ports\n");
            break;
        case 4:
            diag_util_mprintf("10 ports\n");
            break;
        case 5:
            diag_util_mprintf("12 ports\n");
            break;
        case 6:
            diag_util_mprintf("16 ports\n");
            break;
        case 7:
            diag_util_mprintf("20 ports\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_set_tx_wake_mode_packet_tx_qos_based(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'p':
            eee_cfg.wakeMode = WAKE_MODE_ANY_PKT_TX;
            break;

        case 'q':
            eee_cfg.wakeMode = WAKE_MODE_QOS_BASED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    eee_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_EEE_TX_WAKE_MODE_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_set_link_up_delay_1ms_256ms_512ms_1024ms(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(3,1))
    {
        case 'm': /*1ms*/
            eee_cfg.linkUpDelay = 3;
            break;
        case '5': /*256ms*/
            eee_cfg.linkUpDelay = 2;
            break;
        case '1': /*512ms*/
            eee_cfg.linkUpDelay = 1;
            break;
        case '0': /*1024ms*/
            eee_cfg.linkUpDelay = 0;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    eee_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_EEE_LINK_UP_DELAY_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_set_multi_wake_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'd': /*disable*/
            eee_cfg.enable = DISABLED;
            break;
        case 'e': /*enable*/
            eee_cfg.enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    eee_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_EEE_MULTI_WAKE_STATE_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_set_multi_wake_interval_1us_2us_3us_4us(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case '1': /*1us*/
            eee_cfg.interval = 0;
            break;
        case '2': /*2us*/
            eee_cfg.interval = 1;
            break;
        case '3': /*3us*/
            eee_cfg.interval = 2;
            break;
        case '4': /*4us*/
            eee_cfg.interval = 3;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    eee_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_EEE_MULTI_WAKE_INTERVAL_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eee_set_multi_wake_port_num_2port_4port_6port_8port_10port_12port_16port_20port(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case '2':
            if('p' == TOKEN_CHAR(4,1)) /*2port*/
                eee_cfg.portNum = 0;
            else /*20port*/
                eee_cfg.portNum = 7;
            break;
        case '4': /*4port*/
            eee_cfg.portNum = 1;
            break;
        case '6': /*6port*/
            eee_cfg.portNum = 2;
            break;
        case '8': /*8port*/
            eee_cfg.portNum = 3;
            break;
        case '1':
            if('0' == TOKEN_CHAR(4,1)) /*10port*/
                eee_cfg.portNum = 4;
            else if('2' == TOKEN_CHAR(4,1)) /*12port*/
                eee_cfg.portNum = 5;
            else
                eee_cfg.portNum = 6; /*16port*/
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    eee_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_EEE_MULTI_WAKE_PORT_NUM_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eeep_get_rx_tx_sleep_rate(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    eee_cfg.unit = unit;

    switch(TOKEN_CHAR(2,0))
    {
        case 't':
            GETSOCKOPT(RTDRV_EXT_EEEP_TX_SLEEP_RATE_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);
            diag_util_mprintf("\tEEEP TX Sleep Rate (unit:0.5Mbps): %d\n", eee_cfg.rate);
            break;

        case 'r':
            GETSOCKOPT(RTDRV_EXT_EEEP_RX_SLEEP_RATE_GET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);
            diag_util_mprintf("\tEEEP RX Sleep Rate (unit:0.5Mbps): %d\n", eee_cfg.rate);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_eeep_set_rx_tx_sleep_rate_rate(cparser_context_t *context,
    uint32_t *rate_ptr)
{
    uint32 unit;
    rtdrv_ext_eeeCfg_t  eee_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    eee_cfg.unit = unit;
    eee_cfg.rate = *rate_ptr;

    switch(TOKEN_CHAR(2,0))
    {
        case 't':
            SETSOCKOPT(RTDRV_EXT_EEEP_TX_SLEEP_RATE_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);
            break;

        case 'r':
            SETSOCKOPT(RTDRV_EXT_EEEP_RX_SLEEP_RATE_SET, &eee_cfg, rtdrv_ext_eeeCfg_t, 1);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_iol_dump(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    iol_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_IOL_COLLISION_MAX_ATTEMPT_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);
    diag_util_printf("Collision-max-attempt: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("retry\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    GETSOCKOPT(RTDRV_EXT_IOL_ERROR_LENGTH_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);
    diag_util_printf("Error-length: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("forward\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    GETSOCKOPT(RTDRV_EXT_IOL_INVALID_PAUSE_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);
    diag_util_printf("Invalid-pause: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("forward\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    GETSOCKOPT(RTDRV_EXT_IOL_LATE_COLLISION_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);
    diag_util_printf("Late-collision: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("re-transmit\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    GETSOCKOPT(RTDRV_EXT_IOL_MAX_LENGTH_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);
    diag_util_printf("Max-length: ");
    switch(iol_cfg.enable)
    {
        case DISABLED:
            diag_util_mprintf("disable\n");
            break;
        case ENABLED:
            diag_util_mprintf("enable\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_iol_get_collision_max_attempt(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    iol_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_IOL_COLLISION_MAX_ATTEMPT_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    diag_util_printf("Collision-max-attempt: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("retry\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_iol_get_error_length(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    iol_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_IOL_ERROR_LENGTH_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    diag_util_printf("Error-length: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("forward\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_iol_get_invalid_pause(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    iol_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_IOL_INVALID_PAUSE_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    diag_util_printf("Invalid-pause: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("forward\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_iol_get_late_collision(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    iol_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_IOL_LATE_COLLISION_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    diag_util_printf("Late-collision: ");
    switch(iol_cfg.action)
    {
        case 0:
            diag_util_mprintf("re-transmit\n");
            break;
        case 1:
            diag_util_mprintf("drop\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_iol_get_max_length(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    iol_cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_IOL_MAX_LENGTH_GET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    diag_util_printf("Max-length: ");
    switch(iol_cfg.enable)
    {
        case DISABLED:
            diag_util_mprintf("disable\n");
            break;
        case ENABLED:
            diag_util_mprintf("enable\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_iol_set_collision_max_attempt_retry_drop(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'r': /*retry*/
            iol_cfg.action = 0;
            break;
        case 'd': /*drop*/
            iol_cfg.action = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    iol_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_IOL_COLLISION_MAX_ATTEMPT_SET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_iol_set_error_length_forward_drop(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'f': /*forward*/
            iol_cfg.action = 0;
            break;
        case 'd': /*drop*/
            iol_cfg.action = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    iol_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_IOL_ERROR_LENGTH_SET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_iol_set_invalid_pause_forward_drop(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'f': /*retry*/
            iol_cfg.action = 0;
            break;
        case 'd': /*drop*/
            iol_cfg.action = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    iol_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_IOL_INVALID_PAUSE_SET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_iol_set_late_collision_re_transmit_drop(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'r': /*re-transmit*/
            iol_cfg.action = 0;
            break;
        case 'd': /*drop*/
            iol_cfg.action = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    iol_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_IOL_LATE_COLLISION_SET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    return CPARSER_OK;
}

cparser_result_t cparser_cmd_iol_set_max_length_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    rtdrv_ext_iolCfg_t  iol_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'd': /*disable*/
            iol_cfg.enable = DISABLED;
            break;
        case 'e': /*enable*/
            iol_cfg.enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    iol_cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_IOL_MAX_LENGTH_SET, &iol_cfg, rtdrv_ext_iolCfg_t, 1);

    return CPARSER_OK;
}

/*
 * pktgen get tx-cmd
 */
cparser_result_t
cparser_cmd_pktgen_get_tx_cmd(cparser_context_t *context)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_PKTGEN_TX_CMD_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);

    diag_util_mprintf("\tPacket generation TX command : ");

    switch (cfg.status)
    {
        case 0:
            diag_util_mprintf("None\n");
            break;
        case 1:
            diag_util_mprintf("start\n");
            break;
        case 2:
            diag_util_mprintf("stop and reset counter\n");
            break;
        case 3:
            diag_util_mprintf("stop and hold counter\n");
            break;
        default:
            break;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_tx_cmd */

/*
 * pktgen set tx-cmd ( start | stop-and-reset-counter | stop-and-hold-counter )
 */
cparser_result_t
cparser_cmd_pktgen_set_tx_cmd_start_stop_and_reset_counter_stop_and_hold_counter(
    cparser_context_t *context)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;

    if ('a' == TOKEN_CHAR(3, 2))
        cfg.status = 1;
    else if ('r' == TOKEN_CHAR(3, 9))
        cfg.status = 2;
    else
        cfg.status = 3;

    SETSOCKOPT(RTDRV_EXT_PKTGEN_TX_CMD_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_tx_cmd_start_stop_and_reset_counter_stop_and_hold_counter */

/*
 * pktgen get state
 */
cparser_result_t
cparser_cmd_pktgen_get_state(
    cparser_context_t *context)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_PKTGEN_STATE_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);

    diag_util_mprintf("\tPacket generation status : ");
    if (0 == cfg.enable)
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    else if (1 == cfg.enable)
        diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
    else
        diag_util_mprintf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_state */

/*
 * pktgen set state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_state_enable_disable(
    cparser_context_t *context)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;
    switch(TOKEN_CHAR(3,0))
    {
        case 'd': /*disable*/
            cfg.enable = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.enable = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }
    SETSOCKOPT(RTDRV_EXT_PKTGEN_STATE_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_state_enable_disable */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit, ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STATE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tTX state : ");
        if (0 == cfg.enable)
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        else if (1 == cfg.enable)
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_state */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;
    switch(TOKEN_CHAR(5,0))
    {
        case 'd': /*disable*/
            cfg.enable = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.enable = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STATE_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_state_enable_disable */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) tx-done state
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_tx_done_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_TX_DONE_STATE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tTX done state : ");
        if (0 == cfg.enable)
            diag_util_mprintf("Normal\n");
        else if (1 == cfg.enable)
            diag_util_mprintf("Finished\n");
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_tx_done_state */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) frag-pkt action
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_frag_pkt_action(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_FRAG_PKT_ACTION_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tFragment packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_frag_pkt_action */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) frag-pkt action ( drop | trap )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_frag_pkt_action_drop_trap(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;
    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            cfg.action = 0;
            break;
        case 't':
            cfg.action = 1;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_FRAG_PKT_ACTION_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_frag_pkt_action_drop_trap */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) oversize-pkt action
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_oversize_pkt_action(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_OVERSIZE_PKT_ACTION_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tOver size packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_oversize_pkt_action */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) oversize-pkt action ( drop | trap )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_oversize_pkt_action_drop_trap(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;
    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            cfg.action = 0;
            break;
        case 't':
            cfg.action = 1;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_OVERSIZE_PKT_ACTION_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_oversize_pkt_action_drop_trap */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) undersize-pkt action
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_undersize_pkt_action(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_UNDERSIZE_PKT_ACTION_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tUnder size packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_undersize_pkt_action */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) undersize-pkt action ( drop | trap )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_undersize_pkt_action_drop_trap(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;
    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            cfg.action = 0;
            break;
        case 't':
            cfg.action = 1;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_UNDERSIZE_PKT_ACTION_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_undersize_pkt_action_drop_trap */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) bad-crc-pkt action
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_bad_crc_pkt_action(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_BADCRC_PKT_ACTION_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d : \n", port);
        diag_util_mprintf("\tBad CRC packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_bad_crc_pkt_action */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) bad-crc-pkt action ( drop | trap )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_bad_crc_pkt_action_drop_trap(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;
    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            cfg.action = 0;
            break;
        case 't':
            cfg.action = 1;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_BADCRC_PKT_ACTION_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_bad_crc_pkt_action_drop_trap */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) tx-pkt-cnt
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_tx_pkt_cnt(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_TX_PKT_CNT_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d TX packet counter: %d\n", port, cfg.status);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_tx_pkt_cnt */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) tx-pkt-cnt <UINT:pktcnt>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_tx_pkt_cnt_pktcnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *pktcnt_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;
    cfg.status = *pktcnt_ptr;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_TX_PKT_CNT_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_tx_pkt_cnt_pktcnt */

#define DIAG_EXT_PKTGEN_STREAM_MAX      2
#define DIAG_EXT_PKTGEN_STREAM_RANDOM_OFFSET_MAX    31

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> bad-crc state
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_bad_crc_state(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_BADCRC_STATE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d TX bad CRC state: ",
                port, *stream_idx_ptr);

        if (0 == cfg.status)
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        else if (1 == cfg.status)
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_bad_crc_state */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> bad-crc state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_bad_crc_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    switch(TOKEN_CHAR(8,0))
    {
        case 'd': /*disable*/
            cfg.status = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.status = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_BADCRC_STATE_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_bad_crc_state_enable_disable */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> len-type
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_len_type(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_LENTYPE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d packet length type: ",
                port, *stream_idx_ptr);

        if (0 == cfg.status)
            diag_util_mprintf("Fixed\n");
        else if (1 == cfg.status)
            diag_util_mprintf("Random\n");
        else if (2 == cfg.status)
            diag_util_mprintf("Increamental\n");
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_len_type */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> len-type ( fixed | random | increamental )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_len_type_fixed_random_increamental(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    switch(TOKEN_CHAR(7,0))
    {
        case 'f':
            cfg.status = 0;
            break;
        case 'r':
            cfg.status = 1;
            break;
        case 'i':
            cfg.status = 2;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_LENTYPE_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_len_type_fixed_random_increamental */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> random-content state
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_random_content_state(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_CONTENT_STATE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d random content state: ",
                port, *stream_idx_ptr);

        if (0 == cfg.status)
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        else if (1 == cfg.status)
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_random_content_state */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> random-content state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_random_content_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    switch(TOKEN_CHAR(8,0))
    {
        case 'd': /*disable*/
            cfg.status = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.status = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_CONTENT_STATE_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_random_content_state_enable_disable */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> random-offset
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_random_offset(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_OFFSET_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d random content offset: %d\n",
                port, *stream_idx_ptr, cfg.status);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_random_offset */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> random-offset <UINT:offset>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_random_offset_offset(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *offset_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }
    if (*offset_ptr > DIAG_EXT_PKTGEN_STREAM_RANDOM_OFFSET_MAX)
    {
        diag_util_printf("random-offset config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.status = *offset_ptr;
    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_OFFSET_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_random_offset_offset */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> sa-inc state
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_sa_inc_state(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_SA_INC_STATE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d SA increament state: ",
                port, *stream_idx_ptr);

        if (0 == cfg.status)
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        else if (1 == cfg.status)
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_sa_inc_state */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> sa-inc state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_sa_inc_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    switch(TOKEN_CHAR(8,0))
    {
        case 'd': /*disable*/
            cfg.status = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.status = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_SA_INC_STATE_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_sa_inc_state_enable_disable */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> da-inc state
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_da_inc_state(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_DA_INC_STATE_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d DA increament state: ",
                port, *stream_idx_ptr);

        if (0 == cfg.status)
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        else if (1 == cfg.status)
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_da_inc_state */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> da-inc state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_da_inc_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    switch(TOKEN_CHAR(8,0))
    {
        case 'd': /*disable*/
            cfg.status = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.status = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_DA_INC_STATE_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_da_inc_state_enable_disable */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> tx-pkt-cnt
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_tx_pkt_cnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_TX_PKT_CNT_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d TX packet counter: %d\n",
                port, *stream_idx_ptr, cfg.status);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_tx_pkt_cnt */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> tx-pkt-cnt <UINT:pktcnt>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_tx_pkt_cnt_pktcnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *pktcnt_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.status = *pktcnt_ptr;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_TX_PKT_CNT_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_tx_pkt_cnt_pktcnt */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> pkt-len
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_pkt_len(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    int32                   ret;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_PKT_LEN_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d TX packet length start: %d end: %d\n",
                port, *stream_idx_ptr, cfg.pktlen_start, cfg.pktlen_end);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_pkt_len */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> pkt-len start <UINT:start_val> end <UINT:end_val>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_pkt_len_start_start_val_end_end_val(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *start_val_ptr,
    uint32_t *end_val_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.pktlen_start = *start_val_ptr;
    cfg.pktlen_end = *end_val_ptr;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_PKT_LEN_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_pkt_len_start_start_val_end_end_val */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> sa-repeat-cnt
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_sa_repeat_cnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_SA_REPEAT_CNT_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d SA repeat counter: %d\n",
                port, *stream_idx_ptr, cfg.status);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_sa_repeat_cnt */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> sa-repeat-cnt <UINT:repeat_cnt>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_sa_repeat_cnt_repeat_cnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *repeat_cnt_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.status = *repeat_cnt_ptr;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_SA_REPEAT_CNT_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_sa_repeat_cnt_repeat_cnt */

/*
 * pktgen get port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> da-repeat-cnt
 */
cparser_result_t
cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_da_repeat_cnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    int32                   ret;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.stream_idx = *stream_idx_ptr;
        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_DA_REPEAT_CNT_GET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("Port %2d stream %d DA repeat counter: %d\n",
                port, *stream_idx_ptr, cfg.status);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_get_port_ports_all_stream_stream_idx_da_repeat_cnt */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> da-repeat-cnt <UINT:repeat_cnt>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_da_repeat_cnt_repeat_cnt(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *repeat_cnt_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.status = *repeat_cnt_ptr;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_DA_REPEAT_CNT_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_da_repeat_cnt_repeat_cnt */

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field sa <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_sa_mac(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    osal_memcpy(&cfg.sa.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_SA_SET, &cfg,
                rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field da <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_da_mac(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    osal_memcpy(&cfg.da.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_DA_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field ether-type <UINT:ether_type>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_ether_type_ether_type(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *ether_type)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.etherType = (uint16)*ether_type;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_ETHTYPE_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field vlan-header <UINT:vlan_header>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_vlan_header_vlan_header(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *vlan_header)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.vlanHdr = *vlan_header;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_OTAG_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field vlan-header state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_vlan_header_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    switch(TOKEN_CHAR(9,0))
    {
        case 'd': /*disable*/
            cfg.status = DISABLED;
            break;
        case 'e': /*enable*/
            cfg.status = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_OTAG_STATE_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field payload-type ( fix-pattern | incr | repeat-pattern | zero )
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_payload_type_fix_pattern_incr_repeat_pattern_zero(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.pattern = 0;
    switch(TOKEN_CHAR(8,0))
    {
        case 'z': /*zero*/
            cfg.patternType = RTDRV_EXT_SPG_PAYLOAD_ZERO;
            break;
        case 'i': /*increment*/
            cfg.patternType = RTDRV_EXT_SPG_PAYLOAD_INCR;
            break;
        case 'f': /*fix*/
            cfg.patternType = RTDRV_EXT_SPG_PAYLOAD_FIX;
            break;
        case 'r': /*repeat*/
            cfg.patternType = RTDRV_EXT_SPG_PAYLOAD_REPEAT;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_PAYLOAD_TYPE_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) stream <UINT:stream_idx> field payload-pattern <UINT:pattern>
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_stream_stream_idx_field_payload_pattern_pattern(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *stream_idx_ptr,
    uint32_t *pattern_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*stream_idx_ptr >= DIAG_EXT_PKTGEN_STREAM_MAX)
    {
        diag_util_printf("User stream config: Error!\n");
        return CPARSER_NOT_OK;
    }

    cfg.unit = unit;
    cfg.stream_idx = *stream_idx_ptr;
    cfg.pattern = *pattern_ptr;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_PAYLOAD_PATTERN_SET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) tx
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_tx(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_TX, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen set port ( <PORT_LIST:ports> | all ) tx dying-gasp { <UINT:len> }
 */
cparser_result_t
cparser_cmd_pktgen_set_port_ports_all_tx_dying_gasp_len(
    cparser_context_t *context, char **ports_ptr, uint32_t *len)
{
    uint32                  unit;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;
    uint32                  length;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (6 == TOKEN_NUM)
        length = 1518;
    else
        length = *len;

    if (length < 64 || length > 1518)
        return CPARSER_NOT_OK;


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        cfg.len = length;
        SETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_TX_DYING_GASP, &cfg, rtdrv_ext_pktGenCfg_t, 1);
    }

    return CPARSER_OK;
}

/*
 * pktgen dump port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_pktgen_dump_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit, i;
    rtdrv_ext_pktGenCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        diag_util_mprintf("Port %2d :\n", port);

        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STATE_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("\tTX state : ");
        if (0 == cfg.enable)
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        else if (1 == cfg.enable)
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        else
            diag_util_mprintf("\n");

        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_FRAG_PKT_ACTION_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("\tFragment packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");

        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_OVERSIZE_PKT_ACTION_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("\tOver size packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");

        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_UNDERSIZE_PKT_ACTION_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("\tUnder size packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");

        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_BADCRC_PKT_ACTION_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("\tBad CRC packet action : ");
        if (0 == cfg.action)
            diag_util_mprintf("Drop\n");
        else if (1 == cfg.action)
            diag_util_mprintf("Trap\n");
        else
            diag_util_mprintf("\n");

        GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_TX_PKT_CNT_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
        diag_util_mprintf("\tTX packet counter: %d\n", cfg.status);


        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_BADCRC_STATE_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d TX bad CRC state: ", i);
            if (0 == cfg.status)
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
            else if (1 == cfg.status)
                diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
            else
                diag_util_mprintf("\n");
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_LENTYPE_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d packet length type: ", i);

            if (0 == cfg.status)
                diag_util_mprintf("Fixed\n");
            else if (1 == cfg.status)
                diag_util_mprintf("Random\n");
            else if (2 == cfg.status)
                diag_util_mprintf("Increamental\n");
            else
                diag_util_mprintf("\n");
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_CONTENT_STATE_GET, &cfg,
                    rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d random content state: ", i);

            if (0 == cfg.status)
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
            else if (1 == cfg.status)
                diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
            else
                diag_util_mprintf("\n");
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_OFFSET_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d random content offset: %d\n", i, cfg.status);
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_SA_INC_STATE_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d SA increament state: ", i);

            if (0 == cfg.status)
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
            else if (1 == cfg.status)
                diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
            else
                diag_util_mprintf("\n");
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_DA_INC_STATE_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d DA increament state: ", i);

            if (0 == cfg.status)
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
            else if (1 == cfg.status)
                diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
            else
                diag_util_mprintf("\n");
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_TX_PKT_CNT_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d TX packet counter: %d\n", i, cfg.status);
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_PKT_LEN_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d TX packet length start: %d end: %d\n", i, cfg.pktlen_start, cfg.pktlen_end);
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_SA_REPEAT_CNT_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d SA repeat counter: %d\n", i, cfg.status);
        }
        for (i = 0; i < 2; i++)
        {
            cfg.stream_idx = i;
            GETSOCKOPT(RTDRV_EXT_PKTGEN_PORT_STREAM_DA_REPEAT_CNT_GET, &cfg, rtdrv_ext_pktGenCfg_t, 1);
            diag_util_mprintf("\tstream %d DA repeat counter: %d\n", i, cfg.status);
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}

/* l2-table get aging-unit */
cparser_result_t cparser_cmd_l2_table_get_aging_unit(cparser_context_t *context)
{
    rtdrv_ext_l2Cfg_t   l2_cfg;

    DIAG_OM_GET_UNIT_ID(l2_cfg.unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("\tAging Time                                     : ");

    GETSOCKOPT(RTDRV_EXT_L2_AGING_UNIT_GET, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    diag_util_mprintf("%d (0.1 seconds).\n", l2_cfg.aging_time);

    return CPARSER_OK;
}

/* l2-table set aging-unit <UINT:time> */
cparser_result_t cparser_cmd_l2_table_set_aging_unit_time(cparser_context_t *context,
    uint32_t *time_ptr)
{
    rtdrv_ext_l2Cfg_t   l2_cfg;

    DIAG_OM_GET_UNIT_ID(l2_cfg.unit);
    DIAG_UTIL_PARAM_CHK();
    l2_cfg.aging_time = *time_ptr;

    SETSOCKOPT(RTDRV_EXT_L2_AGING_UNIT_SET, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    return CPARSER_OK;
}

/* l2-table clear table */
cparser_result_t cparser_cmd_l2_table_clear_table(cparser_context_t *context)
{
    rtdrv_ext_l2Cfg_t   l2_cfg;

    DIAG_OM_GET_UNIT_ID(l2_cfg.unit);
    DIAG_UTIL_PARAM_CHK();

    SETSOCKOPT(RTDRV_EXT_L2_TBL_CLEAR, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    return CPARSER_OK;
}

/* l2-table get cam state */
cparser_result_t cparser_cmd_l2_table_get_cam_state(cparser_context_t *context)
{
    rtdrv_ext_l2Cfg_t   l2_cfg;

    DIAG_OM_GET_UNIT_ID(l2_cfg.unit);
    DIAG_UTIL_PARAM_CHK();

    GETSOCKOPT(RTDRV_EXT_L2_CMA_ENABLE_GET, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    diag_util_mprintf("L2 table cam: %s\n", l2_cfg.enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}

/* l2-table set cam state ( enable | disable ) */
cparser_result_t cparser_cmd_l2_table_set_cam_state_enable_disable(cparser_context_t *context)
{
    rtdrv_ext_l2Cfg_t   l2_cfg;

    DIAG_OM_GET_UNIT_ID(l2_cfg.unit);
    DIAG_UTIL_PARAM_CHK();

    switch(TOKEN_CHAR(4,0))
    {
        case 'd': /*disable*/
            l2_cfg.enable = DISABLED;
            break;
        case 'e': /*enable*/
            l2_cfg.enable = ENABLED;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    SETSOCKOPT(RTDRV_EXT_L2_CMA_ENABLE_SET, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    return CPARSER_OK;
}

/* acl get meter counter */
cparser_result_t cparser_cmd_acl_get_meter_counter(cparser_context_t *context)
{
    rtdrv_ext_aclCfg_t   acl_cfg;

    DIAG_OM_GET_UNIT_ID(acl_cfg.unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    GETSOCKOPT(RTDRV_EXT_ACL_METER_COUNTER_GET, &acl_cfg, rtdrv_ext_aclCfg_t, 1);
    diag_util_printf("Counter of Meter %u unit ", acl_cfg.index);
    switch(acl_cfg.counterUnit)
    {
        case 0:
            diag_util_mprintf("1Byte\n");
            break;
        case 1:
            diag_util_mprintf("16Byte\n");
            break;
        case 2:
            diag_util_mprintf("Packet\n");
            break;
    }

    diag_util_mprintf("GreenCounter %d\n", acl_cfg.greenCounter);
    diag_util_mprintf("YellowCounter %d\n", acl_cfg.yellowCounter);
    diag_util_mprintf("RedCounter %d\n", acl_cfg.redCounter);
    diag_util_mprintf("TotalCounter %d\n", acl_cfg.totalCounter);

    return CPARSER_OK;
}

/* acl set meter counter ( 1byte | 16byte | packet ) <UINT:index> */
cparser_result_t cparser_cmd_acl_set_meter_counter_1byte_16byte_packet_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    rtdrv_ext_aclCfg_t   acl_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(acl_cfg.unit);
    switch(TOKEN_CHAR(4,1))
    {
        case 'b':
            acl_cfg.counterUnit = 0;
            break;
        case '6':
            acl_cfg.counterUnit = 1;
            break;
        case 'a':
            acl_cfg.counterUnit = 2;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    acl_cfg.index = *index_ptr;
    acl_cfg.clear = 1;

    SETSOCKOPT(RTDRV_EXT_ACL_METER_COUNTER_SET, &acl_cfg, rtdrv_ext_aclCfg_t, 1);

    return CPARSER_OK;
}

/* acl reset meter counter */
cparser_result_t cparser_cmd_acl_reset_meter_counter(cparser_context_t *context)
{
    rtdrv_ext_aclCfg_t   acl_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(acl_cfg.unit);
    acl_cfg.clear = 1;
    SETSOCKOPT(RTDRV_EXT_ACL_METER_COUNTER_RESET, &acl_cfg, rtdrv_ext_aclCfg_t, 1);

    return CPARSER_OK;
}

/* time get ptp ( <PORT_LIST:ports> | all ) timestamp ( rx-sync | rx-dreq | rx-pdrq | rx-pdrp | tx-sync | tx-dreq | tx-pdrq | tx-pdrp ) */
cparser_result_t cparser_cmd_time_get_ptp_ports_all_timestamp_rx_sync_rx_dreq_rx_pdrq_rx_pdrp_tx_sync_tx_dreq_tx_pdrq_tx_pdrp(cparser_context_t *context,
    char **ports_ptr)
{
    uint32_t            unit;
    rtk_port_t          port;
    diag_portlist_t     portlist;
    rtdrv_ext_timeCfg_t time_cfg;
    char                ts_str[8][8] = { "RX-SYNC", "RX-DREQ", "RX-PDRQ", "RX-PDRP", "TX-SYNC", "TX-DREQ", "TX-PDRQ", "TX-PDRP" };

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    time_cfg.unit = unit;
    /* Type of Timestamp */
    if ('r' == TOKEN_CHAR(5, 0))
    {   /* Rx */
        time_cfg.type = 0;
    }
    else
    {   /* Tx */
        time_cfg.type = 4;
    }

    if ('s' == TOKEN_CHAR(5, 3))
    {   /* Sync */
        time_cfg.type += 0;
    }
    else if ('d' == TOKEN_CHAR(5, 3))
    {   /* Delay Req */
        time_cfg.type += 1;
    }
    else if ('q' == TOKEN_CHAR(5, 6))
    {   /* Pdelay Req */
        time_cfg.type += 2;
    }
    else
    {   /* Pdelay Resp */
        time_cfg.type += 3;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\tPort: %2d\n", port);
        time_cfg.port = port;
        /* clear */
        time_cfg.sid = 0;
        time_cfg.sec = 0;
        time_cfg.nsec = 0;
        GETSOCKOPT(RTDRV_EXT_TIME_PORT_PTP_TIMESTAMP_GET, &time_cfg, rtdrv_ext_timeCfg_t, 1);
        diag_util_mprintf("\tPort: %2d\n", port);
        diag_util_mprintf("\t\t%s: SID = %5d, TS = %10u.%09u [0x%04X, 0x%08X, 0x%08X]\n",
            ts_str[time_cfg.type], time_cfg.sid, time_cfg.sec, time_cfg.nsec,
            time_cfg.sid, time_cfg.sec, time_cfg.nsec);
    }

    return CPARSER_OK;
}

/* nic set pkt-send src-mac <MACADDR:src_mac> dst-mac <MACADDR:dst_mac> port ( <PORT_LIST:ports> | all ) { cpu-tag } { trunk-hash }*/
cparser_result_t cparser_cmd_nic_set_pkt_send_src_mac_src_mac_dst_mac_dst_mac_port_ports_all_cpu_tag_trunk_hash(cparser_context_t *context,
    cparser_macaddr_t *src_mac_ptr,
    cparser_macaddr_t *dst_mac_ptr,
    char **ports_ptr)
{
    uint32_t    unit;
    int32  ret = RT_ERR_FAILED;
    uint32_t    i;
    diag_portlist_t     portlist;
    rtdrv_ext_nicSendCfg_t nicSendCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    nicSendCfg.unit = unit;
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&nicSendCfg, 0, sizeof(nicSendCfg));
    osal_memset(&portlist, 0, sizeof(portlist));

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    nicSendCfg.isCpuTag = FALSE;
    nicSendCfg.isTrunkHash = FALSE;

    for (i = 9; i < TOKEN_NUM; i++)
    {
        if ('c' == TOKEN_CHAR(i,0))
        {
            nicSendCfg.isCpuTag = TRUE;
        }

        if ('t' == TOKEN_CHAR(i,0))
            nicSendCfg.isTrunkHash = TRUE;
    }

    DIAG_UTIL_ERR_CHK(diag_util_str2mac(nicSendCfg.src_mac.octet, (char *)TOKEN_STR(4)), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2mac(nicSendCfg.dst_mac.octet, (char *)TOKEN_STR(6)), ret);
    osal_memcpy(&nicSendCfg.txPortmask, &portlist.portmask, sizeof(portlist.portmask));

    SETSOCKOPT(RTDRV_EXT_NIC_PKT_SEND_SET, &nicSendCfg, rtdrv_ext_nicSendCfg_t, 1);

    return CPARSER_OK;
}

/*
 *  l2-table dump mac-ucast cam
 */
cparser_result_t
cparser_cmd_l2_table_dump_mac_unicast_cam(cparser_context_t *context)
{
    uint32             scan_idx = 0;
    uint32             total_entry = 0;
    uint32             unit = 0;
    int32              ret = RT_ERR_FAILED;
    rtk_l2_ucastAddr_t l2_data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2_data, 0, sizeof(rtk_l2_ucastAddr_t));

    diag_util_mprintf("Index | MAC Address       | VID  | SPA  | SaBlock  | DaBlock  | Static  | Nexthop  | Suspend | Aggreate VID\n");
    diag_util_mprintf("------+-------------------+------+------+----------+----------+---------+----------+---------+-------------\n");


    rtdrv_ext_l2Cfg_t   l2_cfg;

    DIAG_OM_GET_UNIT_ID(l2_cfg.unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    GETSOCKOPT(RTDRV_EXT_L2_UC_SIZE_GET, &l2_cfg, rtdrv_ext_l2Cfg_t, 1);

    /* show all l2 cam table */
    scan_idx = l2_cfg.data - 1; /* get the first entry */
    while (1)
    {
        if ((ret = rtk_l2_nextValidAddr_get(unit, (int32 *)&scan_idx, 0, &l2_data)) != RT_ERR_OK)
        {
            //diag_util_mprintf("Warning:%d \n", ret);
            break;
        }

        diag_util_mprintf("%5d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4d | %8d | %8d | %7d | %8d | %7d | %4d\n",
            scan_idx,
            l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
            l2_data.vid, l2_data.port, (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
            (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
            (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0, l2_data.agg_vid);

        total_entry++;
    }

    diag_util_mprintf("\nTotal Number Of Entries :%d\n", total_entry);

    return CPARSER_OK;
}

/*
 * port get polling-phy port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_port_get_polling_phy_port_ports_all_state(
    cparser_context_t *context, char **ports_ptr)
{
    rtdrv_ext_portCfg_t portCfg;
    diag_portlist_t     portlist;
    uint32              unit, port;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    portCfg.unit = unit;

    diag_util_mprintf("Polling PHY state:\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        portCfg.port = port;
        GETSOCKOPT(RTDRV_EXT_PORT_MACPOLLINGPHYSTATE_GET, &portCfg, rtdrv_ext_portCfg_t, 1);
        diag_util_mprintf("\tPort %2d : %s\n", port, (portCfg.state == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_polling_phy_port_ports_all_state */


/*
 * port set polling-phy port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_polling_phy_port_ports_all_state_disable_enable(
    cparser_context_t *context, char **ports_ptr)
{
    rtdrv_ext_portCfg_t portCfg;
    rtk_enable_t        enable;
    diag_portlist_t     portlist;
    uint32              unit, port;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    portCfg.unit = unit;
    portCfg.state = enable;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        portCfg.port = port;
        SETSOCKOPT(RTDRV_EXT_PORT_MACPOLLINGPHYSTATE_SET, &portCfg, rtdrv_ext_portCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_polling_phy_port_ports_all_state_disable_enable */

/*
 * time get reference-time port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_time_get_reference_time_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit;
    rtdrv_ext_timeCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("The time-stamp of reference time \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;

        GETSOCKOPT(RTDRV_EXT_TIME_PORT_REF_TIME_GET, &cfg,
                rtdrv_ext_timeCfg_t, 1);

        diag_util_mprintf("\tPort %d Ref-Time : %llu.%09u\n", port, cfg.timeStamp.sec, cfg.timeStamp.nsec);
    }

    return CPARSER_OK;
}

/*
 * time set tpid port ( <PORT_LIST:ports> | all ) ( inner | outer ) <UINT:tpid_idx> <UINT:tpid>
 */
cparser_result_t
cparser_cmd_time_set_tpid_port_ports_all_inner_outer_tpid_idx_tpid(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *tpid_idx_ptr,
    uint32_t *tpid_ptr)
{
    rtdrv_ext_timeCfg_t cfg;
    diag_portlist_t     portlist;
    uint32              unit, port;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        cfg.vlanType = INNER_VLAN;
    }
    else
    {
        cfg.vlanType = OUTER_VLAN;
    }

    cfg.unit = unit;
    cfg.tpid_idx = *tpid_idx_ptr;
    cfg.tpid = *tpid_ptr;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_TIME_PORT_TPID_SET, &cfg, rtdrv_ext_timeCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_set_tpid_port_ports_all_inner_outer_tpid */

/*
 * time get tpid port ( <PORT_LIST:ports> | all ) ( inner | outer ) <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_time_get_tpid_port_ports_all_inner_outer_tpid_idx(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32                  unit;
    rtdrv_ext_timeCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        cfg.vlanType = INNER_VLAN;
        diag_util_mprintf("ITPID(%d): \n", *tpid_idx_ptr);
    }
    else
    {
        cfg.vlanType = OUTER_VLAN;
        diag_util_mprintf("OTPID(%d): \n", *tpid_idx_ptr);
    }

    cfg.tpid_idx = *tpid_idx_ptr;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;

        GETSOCKOPT(RTDRV_EXT_TIME_PORT_TPID_GET, &cfg, rtdrv_ext_timeCfg_t, 1);

        diag_util_mprintf("\tPort %d: %x\n", port, cfg.tpid);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_get_tpid_port_ports_all_inner_outer */

/*
 * time set mac-address port ( <PORT_LIST:ports> | all ) <MACADDR:mac> <UINT:range>
 */
cparser_result_t
cparser_cmd_time_set_mac_address_port_ports_all_mac_range(
    cparser_context_t *context,
    char **ports_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *range_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtdrv_ext_timeCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_ERR_CHK(diag_util_str2mac(cfg.mac.octet, TOKEN_STR(5)), ret);
    cfg.range = *range_ptr;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;

        SETSOCKOPT(RTDRV_EXT_TIME_PORT_MAC_SET, &cfg, rtdrv_ext_timeCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_set_mac_address_port_ports_all_mac_range */

/*
 * time get mac-address port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_time_get_mac_address_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtdrv_ext_timeCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;
    char   macStr[18];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;

        GETSOCKOPT(RTDRV_EXT_TIME_PORT_MAC_GET, &cfg, rtdrv_ext_timeCfg_t, 1);

        DIAG_UTIL_ERR_CHK(diag_util_mac2str(macStr, cfg.mac.octet), ret);

        diag_util_mprintf("Port %d: %s range %d\n", port, macStr, cfg.range);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_get_mac_address_port_ports_all */


/*
 * time get reference-time port ( <PORT_LIST:ports> | all ) frequency
 */
cparser_result_t
cparser_cmd_time_get_reference_time_port_ports_all_frequency(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtdrv_ext_timeCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("The frequency of reference time \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;

        GETSOCKOPT(RTDRV_EXT_TIME_PORT_REF_TIME_FREQ_GET, &cfg, rtdrv_ext_timeCfg_t, 1);

        diag_util_mprintf("Port %d: %x\n", port, cfg.freq);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_get_reference_time_frequency */


/*
 * time set reference-time port ( <PORT_LIST:ports> | all ) frequency <UINT:frequencyVal>
 */
cparser_result_t
cparser_cmd_time_set_reference_time_port_ports_all_frequency_frequencyVal(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *frequencyVal_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtdrv_ext_timeCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    cfg.freq = *frequencyVal_ptr;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;

        SETSOCKOPT(RTDRV_EXT_TIME_PORT_REF_TIME_FREQ_SET, &cfg, rtdrv_ext_timeCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_set_reference_time_frequency_frequencyVal */


/*
 * time get ptp port ( <PORT_LIST:ports> | all ) interrupt
 */
cparser_result_t
cparser_cmd_time_get_ptp_port_ports_all_interrupt(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtdrv_ext_timeCfg_t   cfg;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    cfg.unit = unit;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;

        GETSOCKOPT(RTDRV_EXT_TIME_PORT_INTERRUPT_GET, &cfg, rtdrv_ext_timeCfg_t, 1);

        diag_util_mprintf("Port %d Interrupt Enable %s Trigger %d Status 0x%x\n", port,
            cfg.interruptEbl?"Enabled":"Disabled", cfg.interruptTrigger, cfg.interruptSts);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_get_ptp_port_ports_all_interrupt */

/*
 * time set ptp port ( <PORT_LIST:ports> | all ) interrupt ( enable | disable )
 */
cparser_result_t
cparser_cmd_time_set_ptp_port_ports_all_interrupt_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    rtdrv_ext_timeCfg_t cfg;
    diag_portlist_t     portlist;
    uint32              unit, port;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(6, 0))
    {
        cfg.interruptEbl = ENABLED;
    }
    else
    {
        cfg.interruptEbl = DISABLED;
    }

    cfg.unit = unit;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.port = port;
        SETSOCKOPT(RTDRV_EXT_TIME_PORT_INTERRUPT_SET, &cfg, rtdrv_ext_timeCfg_t, 1);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_time_set_ptp_port_ports_all_interrupt_enable_disable */



