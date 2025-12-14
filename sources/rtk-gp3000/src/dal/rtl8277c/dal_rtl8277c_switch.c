/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 46644 $
 * $Date: 2014-02-26 18:16:35 +0800 (?Ÿæ?ä¸? 26 äº?æ? 2014) $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */


/*
 * Include Files
 */
#include <linux/delay.h>
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/rtl8277c/dal_rtl8277c.h>
#include <dal/rtl8277c/dal_rtl8277c_switch.h>
#include <scfg.h>
#include <osal/time.h>

#include <cortina-api/include/port.h>
#include <peri.h>
#include <aal_pon.h>
#include <aal_ni_l2.h>
#include <aal_port.h>
#include <aal_sds.h>
#include <osal_spinlock.h>

#include <linux/arm-smccc.h>
#include <soc/cortina/cortina-smc.h>
#include <soc/cortina/registers.h>
#include <soc/cortina/cortina-soc.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32 switch_init = INIT_NOT_COMPLETED;
ca_uint8_t CA_PON_MODE = ONU_PON_MAC_MODE_IGNORED;

/*
 * Macro Declaration
 */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      dal_rtl8277c_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
dal_rtl8277c_switch_init(void)
{
    ca_status_t ret=CA_E_OK;
    ca_uint8_t pon_mode;
#if !defined(CONFIG_CORTINA_BOARD_FPGA)
    void __iomem *tempA_iobase = NULL;
    void __iomem *tempB_iobase = NULL;
    void __iomem *tempC_iobase = NULL;
#endif

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    pon_mode = aal_pon_mac_mode_get(0);

    CA_PON_MODE = pon_mode;
#if !defined(CONFIG_CORTINA_BOARD_FPGA)
    tempA_iobase = ioremap(GLOBAL_TMS_REG_A, 4);
    writel(0x07f80000,tempA_iobase);
    iounmap(tempA_iobase);
    tempB_iobase = ioremap(GLOBAL_TMS_REG_B, 4);
    writel(0x374000,tempB_iobase);
    iounmap(tempB_iobase);
    tempC_iobase = ioremap(GLOBAL_TMS_REG_C, 4);
    writel(0x2102011e,tempC_iobase);
    mdelay(10);
    writel(0x2102011f,tempC_iobase);
    iounmap(tempC_iobase);
#endif
    switch_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_rtl8277c_switch_init */

/* Function Name:
 *      dal_rtl8277c_switch_phyPortId_get
 * Description:
 *      Get physical port id from logical port name
 * Input:
 *      portName - logical port name
 * Output:
 *      pPortId  - pointer to the physical port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port ID must get from this API
 */
int32
dal_rtl8277c_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortId), RT_ERR_NULL_POINTER);

    /*get chip info to check port name mapping*/
    switch(portName)
    {
        case RTK_PORT_UTP0:
            *pPortId = 0;
            break;
        case RTK_PORT_UTP1:
            *pPortId = 1;
            break;
        case RTK_PORT_UTP2:
            *pPortId = 2;
            break;
        case RTK_PORT_UTP3:
            *pPortId = 3;
            break;
        case RTK_PORT_UTP4:
        case RTK_PORT_HSG0:
            *pPortId = 4;
            break;
        case RTK_PORT_UTP5:
            *pPortId = 5;
            break;
        case RTK_PORT_UTP6:
        case RTK_PORT_HSG1:
            *pPortId = 6;
            break;
        case RTK_PORT_UTP7:
        case RTK_PORT_PON:
            *pPortId = 7;
            break;
        case RTK_PORT_CPU0:
            *pPortId = 16;
            break;
        case RTK_PORT_CPU1:
            *pPortId = 17;
            break;
        case RTK_PORT_CPU2:
            *pPortId = 18;
            break;
        case RTK_PORT_CPU3:
            *pPortId = 19;
            break;
        case RTK_PORT_CPU4:
            *pPortId = 20;
            break;
        case RTK_PORT_CPU5:
            *pPortId = 21;
            break;
        case RTK_PORT_CPU6:
            *pPortId = 22;
            break;
        case RTK_PORT_CPU7:
            *pPortId = 23;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
} /* end of dal_rtl8277c_switch_phyPortId_get */

/* Function Name:
 *      dal_rtl8277c_switch_version_get
 * Description:
 *      Get chip version
 * Input:
 *      pChipId    - chip id
 *      pRev       - revision id
 *      pSubtype   - sub type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */

int32 drv_swcore_cid_get(uint32 *pChip_id, uint32 *pChip_rev_id);

int32
dal_rtl8277c_switch_version_get(uint32 *pChipId, uint32 *pRev, uint32 *pSubtype)
{
    int32  ret;
    static uint8 is_init = 0;
    static uint32 chipId;
    static uint32 chipRev;
    static uint32 chipSubtype;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pChipId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRev), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSubtype), RT_ERR_NULL_POINTER);

    if(0 == is_init)
    {
        if (drv_swcore_cid_get(&chipId, &chipRev) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

#if defined(CONFIG_SDK_KERNEL_LINUX)
        {
            struct ca_soc_data data;
            uint8 chip_id_buf[2];
            extern int plat_soc_get_chipmode(u8 *buf, int buflen);

            ca_soc_data_get(&data);
            /* data.chip.revision is A/B/C/...
             * Translate it to CHIP_REV_ID_A/CHIP_REV_ID_B/CHIP_REV_ID_C/...
             */
            chipRev = data.chip_revision - 'A' + 1;

            plat_soc_get_chipmode(chip_id_buf, sizeof(chip_id_buf));
            chipSubtype = chip_id_buf[1]<<8 | chip_id_buf[0];
            chipSubtype &= ~(0xf);
        }
#else
        chipSubtype = 1234;
#endif

        is_init = 1;
    }

    /* function body */
    *pChipId = chipId;
    *pRev = chipRev;
    *pSubtype = chipSubtype;

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_switch_version_get */

/* Function Name:
  *      dal_rtl8277c_switch_maxPktLenByPort_get
  * Description:
  *      Get the max packet length setting of specific port
  * Input:
  *      port - speed type
  * Output:
  *      pLen - pointer to the max packet length
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  */
int32
dal_rtl8277c_switch_maxPktLenByPort_get(rtk_port_t port, uint32 *pLen)
{
    ca_status_t ret=CA_E_OK;
    ca_uint32_t ppid = 0;
    ca_port_id_t port_id;
    ca_uint32_t size=0;
    ca_uint32_t xge_idx = 0;
    aal_ni_pkt_len_config_t ge_cfg;
    aal_ni_xaui_mac_rx_cfg_t xaui_rx_cfg;
    aal_ni_xge_mac_frame_len_config_t xge_cfg;
    aal_sds_mode_t sdsMode = AAL_SDS_MODE_NONE;
    aal_sds_speed_t sdsSpeed;
#if defined(CONFIG_NE_PON)
    ca_uint32_t rx_size, tx_size, mtu_tag_en = 0;
#endif

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    /* function body */
    ppid = PORT_ID(port_id);    /* for ethernet ports, the ppid is the same as lpid */
    switch(ppid)
    {
    case CA_PORT_ID_NI5:
        if((ret = aal_sds_mode_get(0, port, &sdsMode, &sdsSpeed)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        if(sdsMode == AAL_SDS_MODE_RXAUI)
        {
            memset(&xaui_rx_cfg, 0, sizeof(aal_ni_xaui_mac_rx_cfg_t));
            if((ret = aal_ni_xaui_mac_rx_config_get(0, &xaui_rx_cfg)) != CA_E_OK)
            {
        return ret;
            }
                size = xaui_rx_cfg.max_size;
            break;
            }
        /* 
         * Fall through
         * For RGMII mode, use the same settings as other LAN ports
         */
    case CA_PORT_ID_NI0:
    case CA_PORT_ID_NI1:
    case CA_PORT_ID_NI2:
    case CA_PORT_ID_NI3:
    case CA_PORT_ID_NI4:
        memset(&ge_cfg, 0, sizeof(aal_ni_pkt_len_config_t));
        ret = aal_ni_pkt_len_get(0, &ge_cfg);

        if(ret != CA_E_OK){
            return ret;
        }
        size = ge_cfg.max_pkt_size;
        break;
    case CA_PORT_ID_NI6:
        xge_idx = ppid - AAL_NI_XE_START;
        memset(&xge_cfg, 0, sizeof(aal_ni_xge_mac_frame_len_config_t));
        if((ret = aal_ni_xge_mac_frame_len_get(0, xge_idx, &xge_cfg)) != CA_E_OK)
        {
            return ret;
        }
            size = xge_cfg.max_len;
        break;
#if defined(CONFIG_NE_PON)
    case CA_PORT_ID_NI7:
        __PON_LOCK();
        ret = aal_pon_mac_max_frame_size_get(0, port_id, &tx_size, &rx_size, &mtu_tag_en);
        __PON_UNLOCK();
            size = rx_size;
        break;
#endif
    default:
        return RT_ERR_PORT_ID;
}

    *pLen = size;

    return RT_ERR_OK;
} /*end of dal_rtl8277c_switch_maxPktLenByPort_get*/

/* Function Name:
  *      dal_rtl8277c_switch_maxPktLenByPort_set
  * Description:
  *      Set the max packet length of specific port
  * Input:
  *      port  - port
  *      len   - max packet length
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  */
int32
dal_rtl8277c_switch_maxPktLenByPort_set(rtk_port_t port, uint32 len)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_uint32_t ppid = 0;
    ca_uint32_t xge_idx = 0;
    aal_ni_pkt_len_config_mask_t ge_msk;
    aal_ni_pkt_len_config_t ge_cfg;
    aal_ni_xaui_mac_rx_cfg_mask_t xaui_rx_msk;
    aal_ni_xaui_mac_rx_cfg_t xaui_rx_cfg;
    aal_ni_xge_mac_frame_len_config_mask_t xge_msk;
    aal_ni_xge_mac_frame_len_config_t xge_cfg;
    aal_sds_mode_t sdsMode = AAL_SDS_MODE_NONE;
    aal_sds_speed_t sdsSpeed;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    CA_PORT_PHYSICAL_CHECK(port_id);
    
    /* function body */
    ppid = PORT_ID(port_id);    /* for ethernet ports, the ppid is the same as lpid */
    switch(ppid)
    {
    case CA_PORT_ID_NI5:
        if((ret = aal_sds_mode_get(0, port, &sdsMode, &sdsSpeed)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        if(sdsMode == AAL_SDS_MODE_RXAUI)
        {
            memset(&xaui_rx_msk, 0, sizeof(aal_ni_xaui_mac_rx_cfg_mask_t));
            memset(&xaui_rx_cfg, 0, sizeof(aal_ni_xaui_mac_rx_cfg_t));

            xaui_rx_msk.s.max_size = 1;
            xaui_rx_cfg.max_size = len;
            ca_printf("aal_ni_xaui_mac_rx_config_set size %d \r\n", len);
            ret = aal_ni_xaui_mac_rx_config_set(0, xaui_rx_msk, &xaui_rx_cfg);
            break;
        }
        /* 
         * Fall through
         * For RGMII mode, use the same settings as other LAN ports
         */
    case CA_PORT_ID_NI0:
    case CA_PORT_ID_NI1:
    case CA_PORT_ID_NI2:
    case CA_PORT_ID_NI3:
    case CA_PORT_ID_NI4:
        memset(&ge_msk, 0, sizeof(aal_ni_pkt_len_config_mask_t));
        memset(&ge_cfg, 0, sizeof(aal_ni_pkt_len_config_t));

        ge_msk.s.max_pkt_size = 1;
        ge_cfg.max_pkt_size = len;
        ca_printf("aal_ni_pkt_len_set size %d \r\n", len);
        ret = aal_ni_pkt_len_set(0, ge_msk, &ge_cfg);
        break;
    case CA_PORT_ID_NI6:
        xge_idx = ppid - AAL_NI_XE_START;
        memset(&xge_msk, 0, sizeof(aal_ni_xge_mac_frame_len_config_mask_t));
        memset(&xge_cfg, 0, sizeof(aal_ni_xge_mac_frame_len_config_t));

        xge_msk.s.max_len = 1;
        xge_msk.s.maxlen_mode = 1;

        xge_cfg.max_len = len;
        xge_cfg.maxlen_mode = 0; /* include_tag (full length) */

        ca_printf("aal_ni_xge_mac_frame_len_set xge_idx %d size %d \r\n", xge_idx, len);
        ret = aal_ni_xge_mac_frame_len_set(0, xge_idx, xge_msk, &xge_cfg);
        break;
#if defined(CONFIG_NE_PON)
    case CA_PORT_ID_NI7:
        ca_printf("aal_pon_mac_max_frame_size_set size %d \r\n", len);
        __PON_LOCK();
        ret = aal_pon_mac_max_frame_size_set(0, port_id, len, len, 0);
        __PON_UNLOCK();
        break;
#endif
    default:
        ret = RT_ERR_PORT_ID;
        break;
    }

        return ret;
}/*end of dal_rtl8277c_switch_maxPktLenByPort_set*/

/* Function Name:
 *      dal_rtl8277c_switch_chip_reset
 * Description:
 *      Reset switch chip
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 */
int32
dal_rtl8277c_switch_chip_reset(void)
{
    cap_watchdog_config_t wdt_cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    wdt_cfg.enable = 1;
    wdt_cfg.threshold = 1;
    wdt_cfg.action = CAP_WATCHDOG_ACTION_REBOOT;
    if(CA_E_OK != cap_watchdog_set(0, &wdt_cfg))
	return RT_ERR_FAILED;

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_switch_chip_reset */

/* Function Name:
  *      dal_rtl8277c_switch_thermal_get
  * Description:
  *      Get soc thermal value
  * Input:
  *      none
  * Output:
  *       thermalIntger  - pointer to thermal value for integer part
  *       thermalDecimal - pointer to thermal value for Decimal part
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  */
int32
dal_rtl8277c_switch_thermal_get(int32 *thermalIntger, int32 *thermalDecimal)
{
    int32   ret;
    uint32 value,tmp;
    void __iomem *temp_iobase = NULL;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == thermalIntger), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == thermalDecimal), RT_ERR_NULL_POINTER);

    /* function body */
    temp_iobase = ioremap(GLOBAL_TMS_TEMP_DATA, 4);
    value = readl(temp_iobase);
    iounmap(temp_iobase);

    *thermalIntger = (value / 1024);
    if(*thermalIntger > 5)
        *thermalIntger -= 5;
    *thermalDecimal = value % 1024;
    *thermalDecimal = ((1000 *(*thermalDecimal))/1024)*1000;

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_switch_thermal_get */
