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
 * Purpose : mac driver service APIs in the SDK.
 *
 * Feature : mac driver service APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <common/util/rt_util_system.h>
#include <ioal/ioal_param.h>
#include <osal/memory.h>
#if defined(CONFIG_SDK_RTL8231) && defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
#include <private/drv/rtl8231/rtl8231.h>
#endif
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/mac/drv/drv.h>
#include <hal/mac/drv/drv_rtl9310.h>
#include <hal/mac/led/led_rtl9310.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/mac/mac_probe.h>
#include <hal/mac/serdes.h>
#include <hal/mac/drv/drv_rtl9310.h>
#include <soc/type.h>
#include <ioal/mem32.h>
#include <hwp/hw_profile.h>
#include <dal/dal_construct.h>
#include <dal/mango/dal_mango_construct.h>
#include <private/drv/swcore/swcore_rtl9310.h>

/*
 * Symbol Definition
 */
#define TABLE_TYPE_L3_HOST_ROUTE            (3)     /* Host Routing/OpenFlow table */
#define TABLE_TYPE_L3_PREFIX_ROUTE          (4)     /* Prefix Routing/OpenFlow table */
#define DRV_MANGO_LOOP_MAX                  0xFFFF
#define DRV_RTL9310_SMI_MAX                 4

typedef struct drv_port_txrx_en_s
{
    rtk_bitmap_t disModInfo[BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END)];
} drv_port_txrx_en_t;

/*
 * Data Declaration
 */
static uint32           drv_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t     drv_sem[RTK_MAX_NUM_OF_UNIT];

drv_port_txrx_en_t      *port_rx_en_info[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS] = {{0}};
drv_port_txrx_en_t      *port_tx_en_info[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS] = {{0}};
drv_port_frc_info_t     port_frc_info[RTK_MAX_NUM_OF_PORTS][DRV_RTL9310_FRCLINK_MODULE_END];

/*
 * Macro Declaration
 */
#define L3_ENTRY_IDX_TO_ADDR(_idx)          ((((_idx)/6)*8) + ((_idx)%6))
#define L3_ENTRY_ADDR_TO_IDX(_addr)         ((((_addr)/8)*6) + ((_addr)%8))

#define DRV_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(drv_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_RTDRV), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define DRV_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(drv_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_RTDRV), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */
int32 _rtl9310_miim_write(uint32 unit, rtk_port_t port, uint32 page, uint32 phy_reg, uint32 data);

/* Function Name:
 *      drv_rtl9310_regAddrField_write
 * Description:
 *      Write register address field configuration
 * Input:
 *      unit - unit id
 *      reg  - register address
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 *      data - configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
drv_rtl9310_regAddrField_write(uint32 unit, uint32 reg, uint32 endBit,
    uint32 startBit, uint32 data)
{
    uint32  configVal, len, mask;
    int32   ret;

    len = endBit - startBit + 1;

    if (endBit < startBit)
        return RT_ERR_INPUT;

    if (32 == len)
        configVal = data;
    else
    {
        mask = (1 << len) - 1;

        RT_ERR_CHK(ioal_mem32_read(unit, reg, &configVal), ret);

        configVal &= ~(mask << startBit);
        configVal |= (data << startBit);
    }

    RT_ERR_CHK(ioal_mem32_write(unit, reg, configVal), ret);

    return ret;
}   /* end of drv_rtl9310_regAddrField_write */

/* Function Name:
 *      drv_rtl9310_sds2AnaSds_get
 * Description:
 *      Get analog SerDes from SerDes ID
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      anaSds - analog SerDes id
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
drv_rtl9310_sds2AnaSds_get(uint32 unit, uint32 sds, uint32 *anaSds)
{
    uint32 sdsMap[] = {0, 1, 2, 3, 6, 7, 10, 11, 14, 15, 18, 19, 22, 23};

    RT_PARAM_CHK((NULL == anaSds), RT_ERR_NULL_POINTER);

    *anaSds = sdsMap[sds];

    return RT_ERR_OK;
}   /* end of drv_rtl9310_sds2AnaSds_get */

/* Function Name:
 *      drv_rtl9310_sds2XsgmSds_get
 * Description:
 *      Get XSGMII SerDes from SerDes ID
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      xsgmSds - XSGMII SerDes id
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
drv_rtl9310_sds2XsgmSds_get(uint32 unit, uint32 sds, uint32 *xsgmSds)
{
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == xsgmSds), RT_ERR_NULL_POINTER);

    if (sds < 2)
        *xsgmSds = sds;
    else
        *xsgmSds = (sds - 1) * 2;

    return RT_ERR_OK;
}   /* end of drv_rtl9310_sds2XsgmSds_get */

/* Function Name:
 *      drv_rtl9310_sdsCmuPage_get
 * Description:
 *      Get SerDes CMU page
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      page - CMU page
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
drv_rtl9310_sdsCmuPage_get(uint32 unit, uint32 sds, uint32 *page)
{
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == page), RT_ERR_NULL_POINTER);

    switch (HWP_SDS_MODE(unit, sds))
    {
        case RTK_MII_SGMII:
            *page = 0x24;
            break;
        case RTK_MII_XSMII:
            *page = 0x26;
            break;
        case RTK_MII_HISGMII:
            *page = 0x28;
            break;
        case RTK_MII_HISGMII_5G:
            *page = 0x2A;
            break;
        case RTK_MII_QSGMII:
            *page = 0x34;
            break;
        case RTK_MII_RXAUI_LITE:
            *page = 0x2C;
            break;
        case RTK_MII_XSGMII:
        case RTK_MII_10GR:
            *page = 0x2E;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}   /* end of drv_rtl9310_sdsCmuPage_get */

static int32
_drv_rtl9310_portMacForceLink_oper(uint32 unit, rtk_port_t port)
{
    drv_rtl9310_frcLink_module_t    mod;
    uint32  enSts = FALSE;
    uint32  linkDownSts = FALSE;
    uint32  enVal, linkVal;
    int32   ret;

    for (mod = 0; mod < DRV_RTL9310_FRCLINK_MODULE_END; ++mod)
    {
        if (ENABLED == port_frc_info[port][mod].operSts)
        {
            enSts = TRUE;

            if (PORT_LINKDOWN == port_frc_info[port][mod].linkSts)
                linkDownSts = TRUE;
        }
    }

    if (enSts)
    {
        enVal = 1;
        if (linkDownSts)
            linkVal = 0;
        else
            linkVal = 1;

        RT_ERR_CHK(reg_array_field_write(unit, MANGO_MAC_FORCE_MODE_CTRLr, port,
                REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINKf, &linkVal), ret);
    }
    else
        enVal = 0;

    RT_ERR_CHK(reg_array_field_write(unit, MANGO_MAC_FORCE_MODE_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINK_ENf, &enVal), ret);

    return ret;
}   /* end of _drv_rtl9310_portMacForceLink_oper */

/* Function Name:
 *      drv_rtl9310_portMacForceLink_set
 * Description:
 *      configure mac force link status
 * Input:
 *      unit    - unit id
 *      port   - port ID
 *      forceEn   - force enable
 *      linkSts    - link status
 *      module     - which module updates force link
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
drv_rtl9310_portMacForceLink_set(uint32 unit, rtk_port_t port, rtk_enable_t en,
    rtk_port_linkStatus_t sts, drv_rtl9310_frcLink_module_t module)
{
    int32   ret;

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((en != DISABLED && en != ENABLED), RT_ERR_INPUT);
    RT_PARAM_CHK((module >= DRV_RTL9310_FRCLINK_MODULE_END), RT_ERR_INPUT);

    DRV_SEM_LOCK(unit);

    port_frc_info[port][module].operSts = en;
    port_frc_info[port][module].linkSts = sts;

    ret = _drv_rtl9310_portMacForceLink_oper(unit, port);

    DRV_SEM_UNLOCK(unit);

    return ret;
}   /* end of drv_rtl9310_portMacForceLink_set */

/* Function Name:
 *      drv_rtl9310_portMacForceLinkInfo_get
 * Description:
 *      Get port froce link information
 * Input:
 *      unit   - unit id
 *      port   - port ID
 * Output:
 *      pFrcInfo     - force link information
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - null pointer
 * Note:
 *      None
 */
int32
drv_rtl9310_portMacForceLinkInfo_get(uint32 unit, rtk_port_t port, drv_port_frc_info_t *pFrcInfo)
{
    RT_PARAM_CHK(NULL == pFrcInfo, RT_ERR_NULL_POINTER);

    osal_memcpy(pFrcInfo, port_frc_info[port], sizeof(drv_port_frc_info_t) * DRV_RTL9310_FRCLINK_MODULE_END);

    return RT_ERR_OK;
}   /* end of drv_rtl9310_portMacForceLinkInfo_get */

/* Function Name:
 *      drv_rtl9310_sds_rx_rst
 * Description:
 *      Reset Serdes Rx and original patch are kept.
 * Input:
 *      unit    - unit id
 *      sds     - user serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
int32
drv_rtl9310_sds_rx_rst(uint32  unit, uint32 sds)
{
    uint32  asds;
    int32   ret;

    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);

    if (sds < 2)
        return RT_ERR_FAILED;

    RT_ERR_CHK(drv_rtl9310_sds2AnaSds_get(unit, sds, &asds), ret);

    RT_ERR_CHK(drv_rtl9310_sds_write(unit, asds, 0x2e, 0x12, 0x2740), ret);
    RT_ERR_CHK(drv_rtl9310_sds_write(unit, asds, 0x2f, 0x0 , 0x0), ret);
    RT_ERR_CHK(drv_rtl9310_sds_write(unit, asds, 0x2f, 0x2 , 0x2010), ret);
    RT_ERR_CHK(drv_rtl9310_sds_write(unit, asds, 0x20, 0x0 , 0xc10), ret);

    RT_ERR_CHK(drv_rtl9310_sds_write(unit, asds, 0x2e, 0x12, 0x27c0), ret);
    RT_ERR_CHK(drv_rtl9310_sds_write(unit, asds, 0x2f, 0x0 , 0xc000), ret);
    RT_ERR_CHK(drv_rtl9310_sds_write(unit, asds, 0x2f, 0x2 , 0x6010), ret);
    RT_ERR_CHK(drv_rtl9310_sds_write(unit, asds, 0x20, 0x0 , 0xc30), ret);

    osal_time_mdelay(50);

    return RT_ERR_OK;
}   /* end of drv_rtl9310_sds_rx_rst */

/* Function Name:
 *      drv_rtl9310_sds_read
 * Description:
 *      Read SerDes configuration
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      page - page number
 *      reg  - register
 * Output:
 *      data - SerDes Configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
drv_rtl9310_sds_read(uint32 unit, uint32 sds, uint32 page, uint32 reg, uint32 *data)
{
    uint32  val, cnt = 0;
    int32   ret;

    //RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    RT_ERR_CHK(reg_field_write(unit, MANGO_SERDES_INDRT_ACCESS_CTRLr, MANGO_REGf, &reg), ret);
    RT_ERR_CHK(reg_field_write(unit, MANGO_SERDES_INDRT_ACCESS_CTRLr, MANGO_PAGEf, &page), ret);
    RT_ERR_CHK(reg_field_write(unit, MANGO_SERDES_INDRT_ACCESS_CTRLr, MANGO_INDEXf, &sds), ret);

    val = 0;
    RT_ERR_CHK(reg_field_write(unit, MANGO_SERDES_INDRT_ACCESS_CTRLr, MANGO_RWOPf, &val), ret);
    val = 1;
    RT_ERR_CHK(reg_field_write(unit, MANGO_SERDES_INDRT_ACCESS_CTRLr, MANGO_CMDf, &val), ret);

    while (cnt < DRV_MANGO_LOOP_MAX)
    {
        RT_ERR_CHK(reg_field_read(unit, MANGO_SERDES_INDRT_ACCESS_CTRLr, MANGO_CMDf, &val), ret);
        if (val == 0)
            break;
        ++cnt;
    }

    if (DRV_MANGO_LOOP_MAX == cnt)
    {
        RTK_DBG_PRINT(RTK_DEBUG_LEVEL, "Read sds %u page %u reg %u\n", sds, page, reg);
        return RT_ERR_TIMEOUT;
    }

    RT_ERR_CHK(reg_field_read(unit, MANGO_SERDES_INDRT_DATA_CTRLr, MANGO_DATAf, data), ret);

    return RT_ERR_OK;
}   /* end of drv_rtl9310_sds_read */

/* Function Name:
 *      drv_rtl9310_sds_write
 * Description:
 *      Write SerDes configuration
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      page - page number
 *      reg  - register
 *      data - SerDes Configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
drv_rtl9310_sds_write(uint32 unit, uint32 sds, uint32 page, uint32 reg, uint32 data)
{
    uint32  val, cnt = 0;
    int32   ret;

    //RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);

    RT_ERR_CHK(reg_field_write(unit, MANGO_SERDES_INDRT_ACCESS_CTRLr, MANGO_REGf, &reg), ret);
    RT_ERR_CHK(reg_field_write(unit, MANGO_SERDES_INDRT_ACCESS_CTRLr, MANGO_PAGEf, &page), ret);
    RT_ERR_CHK(reg_field_write(unit, MANGO_SERDES_INDRT_ACCESS_CTRLr, MANGO_INDEXf, &sds), ret);

    RT_ERR_CHK(reg_field_write(unit, MANGO_SERDES_INDRT_DATA_CTRLr, MANGO_DATAf, &data), ret);

    val = 1;
    RT_ERR_CHK(reg_field_write(unit, MANGO_SERDES_INDRT_ACCESS_CTRLr, MANGO_RWOPf, &val), ret);
    val = 1;
    RT_ERR_CHK(reg_field_write(unit, MANGO_SERDES_INDRT_ACCESS_CTRLr, MANGO_CMDf, &val), ret);

    while (cnt < DRV_MANGO_LOOP_MAX)
    {
        RT_ERR_CHK(reg_field_read(unit, MANGO_SERDES_INDRT_ACCESS_CTRLr, MANGO_CMDf, &val), ret);
        if (val == 0)
            return RT_ERR_OK;
        ++cnt;
    }

    RTK_DBG_PRINT(RTK_DEBUG_LEVEL, "Write sds %u page %u reg %u data 0x%X\n", sds, page, reg, data);

    return RT_ERR_TIMEOUT;
}   /* end of drv_rtl9310_sds_write */

/* Function Name:
 *      _rtl9310_miim_phyRegAccessCtrlRegFields_set
 * Description:
 *      Set all fields of CYPRESS_PHYREG_ACCESS_CTRLr register
 * Input:
 *      unit        - unit id
 *      main_page   - value of CYPRESS_MAIN_PAGEf   - page number to access
 *      phy_reg     - value of CYPRESS_REGf         - register number to access
 *      type        - value of CYPRESS_TYPEf        - 0b0: Normal register; 0b1: MMD register
 *      rwop        - value of CYPRESS_RWOPf        - 0b0: read; 0b1: write
 *      cmd         - value of CYPRESS_CMDf         - 0b0: complete access; 0b1: execute access
 * Output:
 *      pData   - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
_rtl9310_miim_phyRegAccessCtrlRegFields_set(uint32   unit,
                                   uint32   access,
                                   uint32   main_page,
                                   uint32   phy_reg,
                                   uint32   broadcast,
                                   uint32   type,
                                   uint32   rwop,
                                   uint32   cmd,
                                   uint32   *pData)
{
    int32 ret = RT_ERR_FAILED;

    /* access mode
     * 0b0  mdx
     * 0b1: inband
     */
    if ((ret = reg_field_set(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, MANGO_ACCESSf, &access, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select page number to access */
    if ((ret = reg_field_set(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, MANGO_MAIN_PAGEf, &main_page, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select register number to access */
    if ((ret = reg_field_set(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, MANGO_REGf, &phy_reg, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* BROADCAST state
     * 0b0  disable
     * 0b1: enable
     */
    if ((ret = reg_field_set(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, MANGO_BROADCASTf, &broadcast, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }


    /* Select PHY register type
     * If select 1G/10G MMD register type, registers EXT_PAGE, MAIN_PAGE and REG settings are don?™t care.
     * 0x0  Normal register
     * 0x1: 1G MMD register
     * 0x2: 10G MMD register
     */
    if ((ret = reg_field_set(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, MANGO_TYPEf, &type, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }


    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    if ((ret = reg_field_set(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, MANGO_RWOPf, &rwop, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: Normal(finish)
     * 0b1: execute
     * Note: When MAC completes access, it will clear this bit.
     */
    if ((ret = reg_field_set(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, MANGO_CMDf, &cmd, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _rtl9310_miim_c22BroadcastPortmask_get
 * Description:
 *      Get C22 SMI broadcast port mask
 * Input:
 *      unit        - unit id
 *      pPortmask   - pointer of portmask that are selected for PHY access
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
void
_rtl9310_miim_c22BroadcastPortmask_get(uint32 unit, rtk_portmask_t  *pPortmask)
{
    uint32 i;
    uint32 port;
    uint32 val;
    uint32   smiFormat_field[] = {
        MANGO_SMI_SET0_FMT_SELf,
        MANGO_SMI_SET1_FMT_SELf,
        MANGO_SMI_SET2_FMT_SELf,
        MANGO_SMI_SET3_FMT_SELf,
    };

    for(i = 0; i < sizeof(smiFormat_field)/sizeof(uint32); i++)
    {
        if ( RT_ERR_OK != reg_field_read(unit, MANGO_SMI_GLB_CTRL1r, smiFormat_field[i], &val))
        {
            return;
        }
        if (val != 1 && val != 0)
        {
            continue;
        }
        HWP_PORT_TRAVS(unit, port)
        {
            if(HWP_PORT_SMI(unit, port) == i)
            {
                RTK_PORTMASK_PORT_SET(*pPortmask, port);
                break;
            }
        }

    }

    return;
}

/* Function Name:
 *      _rtl9310_miim_accessPortmask_set
 * Description:
 *      Enable the ports for PHY register access
 * Input:
 *      unit        - unit id
 *      pPortmask   - pointer of portmask that are selected for PHY access
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
_rtl9310_miim_accessPortmask_set(uint32 unit, rtk_portmask_t  *pPortmask)
{
    int32       ret;
    uint32      portIdx;
    uint32      val;

    val = pPortmask->bits[0];
    portIdx = 0;
    if ((ret = reg_array_write(unit, MANGO_SMI_INDRT_ACCESS_CTRL_2r, portIdx, REG_ARRAY_INDEX_NONE, &val)) != RT_ERR_OK)
    {
        return ret;
    }

    val = pPortmask->bits[1];
    portIdx = 32;
    if ((ret = reg_array_write(unit, MANGO_SMI_INDRT_ACCESS_CTRL_2r, portIdx, REG_ARRAY_INDEX_NONE, &val)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _rtl9310_miim_page_set
 * Description:
 *      Set the page to park
 * Input:
 *      unit        - unit id
 *      pPortmask   - pointer of portmask that are selected for PHY access
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
_rtl9310_miim_page_set(uint32 unit, uint32 park_page)
{
    uint32 temp = 0;
    int32 ret = RT_ERR_FAILED;

    if ((ret = reg_field_write(unit, MANGO_SMI_INDRT_ACCESS_CTRL_3r, MANGO_WRTIE_DATAf, &park_page)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    ret = _rtl9310_miim_phyRegAccessCtrlRegFields_set(unit,
                0,         /* ACCESS_MOD- 0b0: mdx; 0b1: inband*/
                0,         /* MAIN_PAGE - page number to access */
                31,        /* REG       - register number to access */
                0,         /* BRODCAST  - 0b0: disable; 0b1: enable*/
                0,         /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                1,         /* RWOP      - 0b0: read; 0b1: write */
                1,         /* CMD       - 0b0: Normal(finish); 0b1: execute */
                &temp);

    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    /* write register to active the write operation */
    if ((ret = reg_write(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, RTL9310_SMI_INDRT_ACCESS_CTRL_0_CMD_MASK, RTL9310_SMI_INDRT_ACCESS_CTRL_0_FAIL_MASK);

    return RT_ERR_OK;
}

/* Function Name:
 *      _rtl9310_miim_writeAccessPortmask_get
 * Description:
 *      Get enable portmask of PHY register access
 * Input:
 *      unit        - unit id
 *      pPortmask   - pointer of portmask that are selected for PHY access
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
_rtl9310_miim_writeAccessPortmask_get(uint32 unit, rtk_portmask_t  *pPortmask)
{
    int32       ret;
    uint32      portIdx;
    uint32      val;

    HWP_PORT_TRAVS(unit, portIdx)
    {
        if ((ret = reg_array_field_read(unit,
                              MANGO_SMI_INDRT_ACCESS_CTRL_2r,
                              portIdx,
                              REG_ARRAY_INDEX_NONE,
                              MANGO_PORT_MSKf,
                              &val)) != RT_ERR_OK)
        {
            return ret;
        }

        if(val == ENABLED)
            RTK_PORTMASK_PORT_SET(*pPortmask, portIdx);
        else
            RTK_PORTMASK_PORT_CLEAR(*pPortmask, portIdx);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9310_port_probe
 * Description:
 *      Probe the select port interface settings of the chip.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
rtl9310_port_probe(uint32 unit)
{
    return RT_ERR_OK;
} /* end of rtl9310_port_probe */

/* Function Name:
 *      rtl9310_miim_portSmiMdxProto_set
 * Description:
 *      Configure MDC/MDIO protocol for an SMI interface
 * Input:
 *      unit - unit id
 *      port - port id
 *      proto  - protocol as Clause 22 or Clause 45
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_INPUT  - error smi id or proto
 * Note:
 *      None
 */
int32
rtl9310_miim_portSmiMdxProto_set(uint32 unit, rtk_port_t port, drv_smi_mdxProtoSel_t proto)
{
    int32       ret;
    uint32      field, val, rval, is_std;
    uint32      smi_id;

    smi_id = HWP_PORT_SMI(unit, port);
    switch (smi_id)
    {
      case 0:
        field = MANGO_SMI_SET0_FMT_SELf;
        break;
      case 1:
        field = MANGO_SMI_SET1_FMT_SELf;
        break;
      case 2:
        field = MANGO_SMI_SET2_FMT_SELf;
        break;
      case 3:
        field = MANGO_SMI_SET3_FMT_SELf;
        break;
      default:
        ret = RT_ERR_INPUT;
        RT_ERR(ret, (MOD_HAL), "unit %u smi_id %u", unit, smi_id);
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_SMI_GLB_CTRL1r, field, &rval)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u SMI_SET%u_FMT_SELf get fail 0x%x", unit, smi_id, ret);
        return ret;
    }

    if ((rval == 0x0) || (rval == 0x2))
        is_std = TRUE;
    else
        is_std = FALSE;

    if (proto == DRV_SMI_MDX_PROTO_C22)
    {
        if (is_std)
            val = 0x0;
        else
            val = 0x1;
    }
    else if (proto == DRV_SMI_MDX_PROTO_C45)
    {
        if (is_std)
            val = 0x2;
        else
            val = 0x3;
    }
    else
    {
        return RT_ERR_INPUT;
    }

    ret = reg_field_write(unit, MANGO_SMI_GLB_CTRL1r, field, &val);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u SMI%u_INTF_SELf set %u fail 0x%x", unit, smi_id, val, ret);
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      rtl9310_miim_portSmiMdxProto_get
 * Description:
 *      Configure MDC/MDIO protocol for an SMI interface
 * Input:
 *      unit - unit id
 *      port - port id
 *      proto  - protocol as Clause 22 or Clause 45
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_INPUT  - error smi id or proto
 * Note:
 *      None
 */
int32
rtl9310_miim_portSmiMdxProto_get(uint32 unit, rtk_port_t port, drv_smi_mdxProtoSel_t *pProto)
{
    int32       ret;
    uint32      field, val = 0;
    uint32      smi_id;

    smi_id = HWP_PORT_SMI(unit, port);
    switch (smi_id)
    {
      case 0:
        field = MANGO_SMI_SET0_FMT_SELf;
        break;
      case 1:
        field = MANGO_SMI_SET1_FMT_SELf;
        break;
      case 2:
        field = MANGO_SMI_SET2_FMT_SELf;
        break;
      case 3:
        field = MANGO_SMI_SET3_FMT_SELf;
        break;
      default:
        ret = RT_ERR_INPUT;
        RT_ERR(ret, (MOD_HAL), "unit %u smi_id %u", unit, smi_id);
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_SMI_GLB_CTRL1r, field, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u SMI%u_INTF_SEL set %u fail 0x%x", unit, smi_id, val, ret);
        return ret;
    }

    *pProto = ((val == 0x0) || (val == 0x1)) ? DRV_SMI_MDX_PROTO_C22 : DRV_SMI_MDX_PROTO_C45;

    return RT_ERR_OK;

}

/* Function Name:
 *      rtl9310_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
rtl9310_init(uint32 unit)
{
    rtk_port_t  port;
    int32 ret = RT_ERR_FAILED;

    drv_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    drv_sem[unit] = osal_sem_mutex_create();
    if (0 == drv_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_RTDRV), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    HWP_PORT_TRAVS(unit, port)
    {
        port_rx_en_info[unit][port] = osal_alloc(sizeof(drv_port_txrx_en_t));
        if (!port_rx_en_info[unit][port])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_RTDRV), "memory allocate failed");
            ret = RT_ERR_MEM_ALLOC;
            goto ERR;
        }
        osal_memset(port_rx_en_info[unit][port], 0, sizeof(drv_port_txrx_en_t));

        port_tx_en_info[unit][port] = osal_alloc(sizeof(drv_port_txrx_en_t));
        if (!port_tx_en_info[unit][port])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_RTDRV), "memory allocate failed");
            ret = RT_ERR_MEM_ALLOC;
            goto ERR;
        }
        osal_memset(port_tx_en_info[unit][port], 0, sizeof(drv_port_txrx_en_t));

        osal_memset(port_frc_info[port], 0, sizeof(drv_port_frc_info_t) * DRV_RTL9310_FRCLINK_MODULE_END);
    }

    drv_init[unit] = INIT_COMPLETED;

    rtl9310_smiAddr_init(unit);

    {
        uint8 ledModeInitSkip = LEDMODEINITSKIP_NO;
#if !defined(__BOOTLOADER__)
        ioal_param_ledInitFlag_get(&ledModeInitSkip);
#endif
        if (LEDMODEINITSKIP_NO == ledModeInitSkip)
        {
            if ((ret = rtl9310_led_config(unit)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_INIT), "led config failed");
            }
        }
    }

    dal_mango_construct_macConfig_init(unit);

    return RT_ERR_OK;

ERR:
    osal_sem_mutex_destroy(drv_sem[unit]);

    HWP_PORT_TRAVS(unit, port)
    {
        if (port_rx_en_info[unit][port])
            osal_free(port_rx_en_info[unit][port]);

        if (port_tx_en_info[unit][port])
            osal_free(port_tx_en_info[unit][port]);
    }

    return ret;
} /* end of rtl9310_init */


/* Function Name:
 *      rtl9310_miim_read
 * Description:
 *      Get PHY registers from rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 51
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9310_miim_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      *pData)
{
    uint32 temp;
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, page=0x%x, phy_reg=0x%x", unit, port, page, phy_reg);

    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    PHY_SEM_LOCK(unit);

    if (page == HAL_MIIM_FIX_PAGE)
    {
        page = HAL_MIIM_PAGE_ID_MAX(unit);
    }
    else
    {
        if (page > HAL_MIIM_PAGE_ID_MAX(unit))
        {
            _rtl9310_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, page);
            page = HAL_MIIM_PAGE_ID_MAX(unit);
        }
    }


    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * Select port id for indirect read access.
     */
    if ((ret = reg_field_write(unit, MANGO_SMI_INDRT_ACCESS_BC_PHYID_CTRLr,
            MANGO_PORT_IDf, &port)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl9310_miim_phyRegAccessCtrlRegFields_set(unit,
                0,         /* ACCESS_MOD- 0b0: mdx; 0b1: inband*/
                page,      /* MAIN_PAGE - page number to access */
                phy_reg,   /* REG       - register number to access */
                0,         /* BRODCAST  - 0b0: disable; 0b1: enable*/
                0,         /* TYPE      - 0:Normal register; 1:1G MMD register; 2:10G MMD register*/
                0,         /* RWOP      - 0b0: read; 0b1: write */
                1,         /* CMD       - 0b0: Normal(finish); 0b1: execute */
                &temp);

    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, RTL9310_SMI_INDRT_ACCESS_CTRL_0_CMD_MASK, RTL9310_SMI_INDRT_ACCESS_CTRL_0_FAIL_MASK);

    /* get the read operation result to pData */
    if ((ret = reg_field_read(unit, MANGO_SMI_INDRT_ACCESS_CTRL_3r, MANGO_READ_DATAf, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    PHY_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_HAL), "pData=0x%x", *pData);

    return RT_ERR_OK;

} /* end of rtl9310_miim_read */


/* Function Name:
 *      rtl9310_miim_write
 * Description:
 *      Set PHY registers in rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 51
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
_rtl9310_miim_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    int32   ret = RT_ERR_FAILED;
    rtk_portmask_t portMask;

    /* Select PHY to access */
    RTK_PORTMASK_RESET(portMask);
    RTK_PORTMASK_PORT_SET(portMask, port);
    if ((ret = rtl9310_miim_portmask_write(unit, portMask, page, phy_reg, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;

} /* end of rtl9310_miim_write */


/* Function Name:
 *      rtl9310_miim_write
 * Description:
 *      Set PHY registers in rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 51
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9310_miim_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, phy_reg=0x%x, data=0x%x", unit, port, page, phy_reg, data);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    if (page == HAL_MIIM_FIX_PAGE)
    {
        page = HAL_MIIM_PAGE_ID_MAX(unit);
    }
    else
    {
        if (page > HAL_MIIM_PAGE_ID_MAX(unit))
        {
            _rtl9310_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, page);
            page = HAL_MIIM_PAGE_ID_MAX(unit);
        }
    }

    ret = _rtl9310_miim_write(unit, port, page, phy_reg, data);

    return ret;

} /* end of rtl9310_miim_write */
/* Function Name:
 *      rtl9310_miim_park_read
 * Description:
 *      Get PHY registers from rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 51
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9310_miim_park_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData)
{

    return rtl9310_miim_read(unit, port, page, phy_reg, pData);
} /* end of rtl9310_miim_park_read */


/* Function Name:
 *      rtl9310_miim_park_write
 * Description:
 *      Set PHY registers in rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 51
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9310_miim_park_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    return rtl9310_miim_write(unit, port, page, phy_reg, data);
} /* end of rtl9310_miim_park_write */

/* Function Name:
 *      rtl9310_miim_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl9310 family chips.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      page     - PHY page
 *      phy_reg  - PHY register
 *      data     - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. portmask valid range is bit 0 ~ 51
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9310_miim_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          page,
    uint32          phy_reg,
    uint32          data)
{
    uint32  temp;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, page=0x%x, phy_reg=0x%x, data=0x%x", unit, page, phy_reg, data);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    if (page == HAL_MIIM_FIX_PAGE)
    {
        page = HAL_MIIM_PAGE_ID_MAX(unit);
    }
    else
    {
        RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX(unit)), RT_ERR_PHY_PAGE_ID);
    }


    PHY_SEM_LOCK(unit);

    /* Select PHY to access */
    if ((ret = _rtl9310_miim_accessPortmask_set(unit, &portmask)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Input parameters:
    * If RWOP = 0(read), then READ_DATA
    * If RWOP = 1(write), then WRTIE_DATA
    */
    if ((ret = reg_field_write(unit, MANGO_SMI_INDRT_ACCESS_CTRL_3r, MANGO_WRTIE_DATAf, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    ret = _rtl9310_miim_phyRegAccessCtrlRegFields_set(unit,
                0,         /* ACCESS_MOD- 0b0: mdx; 0b1: inband*/
                page,      /* MAIN_PAGE - page number to access */
                phy_reg,   /* REG       - register number to access */
                0,         /* BRODCAST  - 0b0: disable; 0b1: enable*/
                0,         /* TYPE      - 0:Normal register; 1:1G MMD register; 2:10G MMD register*/
                1,         /* RWOP      - 0b0: read; 0b1: write */
                1,         /* CMD       - 0b0: Normal(finish); 0b1: execute */
                &temp);

    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    /* write register to active the write operation */
    if ((ret = reg_write(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, RTL9310_SMI_INDRT_ACCESS_CTRL_0_CMD_MASK, RTL9310_SMI_INDRT_ACCESS_CTRL_0_FAIL_MASK);

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;

} /* end of rtl9310_miim_portmask_write */

/* Function Name:
 *      rtl9310_miim_broadcast_write
 * Description:
 *      Set PHY registers in rtl9310 family chips with broadcast mechanism.
 * Input:
 *      unit    - unit id
 *      page    - page id
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. page valid range is 0 ~ 31
 *      2. phy_reg valid range is 0 ~ 31
 */
int32
rtl9310_miim_broadcast_write(
    uint32      unit,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    uint32 temp;
    uint32 val;
    int32  ret = RT_ERR_FAILED;
    rtk_portmask_t portMask, orgPortMask;


    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, page=0x%x, phy_reg=0x%x, data=0x%x", unit, page, phy_reg, data);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    if (page == HAL_MIIM_FIX_PAGE)
    {
        page = HAL_MIIM_PAGE_ID_MAX(unit);
    }
    else
    {
        RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX(unit)), RT_ERR_PHY_PAGE_ID);
    }

    osal_memset(&portMask, 0, sizeof(rtk_portmask_t));
    osal_memset(&orgPortMask, 0, sizeof(rtk_portmask_t));

    PHY_SEM_LOCK(unit);

    /* Get PHY to access */
    if ((ret = _rtl9310_miim_writeAccessPortmask_get(unit, &orgPortMask)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* Select broadcast port mask to access */
    _rtl9310_miim_c22BroadcastPortmask_get(unit, &portMask);
    if ((ret = _rtl9310_miim_accessPortmask_set(unit, &portMask)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /*Select broadcast PHY ID for indirect access.*/
    val = 0x1F;    /*BC_PHYID[4:0]*/
    if ((ret = reg_field_write(unit, MANGO_SMI_INDRT_ACCESS_BC_PHYID_CTRLr, MANGO_BC_PHYIDf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Input parameters:
    * If RWOP = 0(read), then READ_DATA
    * If RWOP = 1(write), then WRTIE_DATA
    */
   if ((ret = reg_field_write(unit, MANGO_SMI_INDRT_ACCESS_CTRL_3r, MANGO_WRTIE_DATAf, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    ret = _rtl9310_miim_phyRegAccessCtrlRegFields_set(unit,
                0,         /* ACCESS_MOD- 0b0: mdx; 0b1: inband*/
                page,      /* MAIN_PAGE - page number to access */
                phy_reg,   /* REG       - register number to access */
                1,         /* BRODCAST  - 0b0: disable; 0b1: enable*/
                0,         /* TYPE      - 0:Normal register; 1:1G MMD register; 2:10G MMD register*/
                1,         /* RWOP      - 0b0: read; 0b1: write */
                1,         /* CMD       - 0b0: Normal(finish); 0b1: execute */
                &temp);

    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    /* write register to active the write operation */
    if ((ret = reg_write(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, RTL9310_SMI_INDRT_ACCESS_CTRL_0_CMD_MASK, RTL9310_SMI_INDRT_ACCESS_CTRL_0_FAIL_MASK);

    /* Select PHY to access */
    if ((ret = _rtl9310_miim_accessPortmask_set(unit, &orgPortMask)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;

} /* end of rtl9310_miim_write */


/* Function Name:
 *      rtl9310_miim_extParkPage_read
 * Description:
 *      Get PHY registers from rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mainPage    - main page id
 *      extPage     - extension page id
 *      parkPage    - parking page id
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 51
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9310_miim_extParkPage_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      rtl9310_miim_write
 * Description:
 *      Set PHY registers in rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mainPage    - main page id
 *      extPage     - extension page id
 *      parkPage    - parking page id
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 51
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9310_miim_extParkPage_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    return RT_ERR_OK;
} /* end of rtl9310_miim_write */

/* Function Name:
 *      rtl9310_miim_extParkPage_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl9310 family chips.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      page     - PHY page
 *      phy_reg  - PHY register
 *      data     - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. portmask valid range is bit 0 ~ 51
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9310_miim_extParkPage_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mainPage,
    uint32          extPage,
    uint32          parkPage,
    uint32          phy_reg,
    uint32          data)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9310_miim_mmd_read
 * Description:
 *      Get PHY registers from rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 51
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9310_miim_mmd_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      *pData)
{
    uint32 temp;
    uint32 val;
    int32  ret = RT_ERR_FAILED;
    int32  type = 0;
    drv_smi_mdxProtoSel_t   proto;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, mmdAddr=0x%x, mmdReg=0x%x, reg=0x%x",
           unit, port, mmdAddr, mmdReg);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if ((ret = rtl9310_miim_portSmiMdxProto_get(unit, port, &proto)) != RT_ERR_OK)
    {
        return ret;
    }
    if (proto == DRV_SMI_MDX_PROTO_C45)
        type = 2;
    else
        type = 1;

    PHY_SEM_LOCK(unit);

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * INDATA[5:0] is the PHY address WHEN RWOP = 0b0
     */
    if ((ret = reg_field_write(unit, MANGO_SMI_INDRT_ACCESS_BC_PHYID_CTRLr, MANGO_PORT_IDf, &port)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd device address to access */
    if ((ret = reg_field_write(unit, MANGO_SMI_INDRT_ACCESS_MMD_CTRLr, MANGO_MMD_DEVADf, &mmdAddr)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd register to access */
    if ((ret = reg_field_write(unit, MANGO_SMI_INDRT_ACCESS_MMD_CTRLr, MANGO_MMD_REGf, &mmdReg)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl9310_miim_phyRegAccessCtrlRegFields_set(unit,
                                   0,           /* ACCESS_MOD- 0b0: mdx; 0b1: inband*/
                                   0,           /* MAIN_PAGE - page number to access */
                                   0,           /* REG       - register number to access */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   type,        /* TYPE      - 0:Normal register; 1:1G MMD register; 2:10G MMD register*/
                                   0,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, RTL9310_SMI_INDRT_ACCESS_CTRL_0_CMD_MASK, RTL9310_SMI_INDRT_ACCESS_CTRL_0_FAIL_MASK);

    /* get the read operation result to pData */
    if ((ret = reg_field_read(unit, MANGO_SMI_INDRT_ACCESS_CTRL_3r, MANGO_READ_DATAf, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* check the FAIL bit */
    if ((ret = reg_field_read(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, MANGO_FAILf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    if(val == 1)
    {
        PHY_SEM_UNLOCK(unit);
        return RT_ERR_PHY_ACCESS_FAIL;
    }

    PHY_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_HAL), "pData=0x%x", *pData);

    return RT_ERR_OK;

} /* end of rtl9310_miim_mmd_read */


/* Function Name:
 *      rtl9310_miim_mmd_write
 * Description:
 *      Set PHY registers in rtl9310 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 51
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9310_miim_mmd_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      data)
{
    int32 ret = RT_ERR_FAILED;
    rtk_portmask_t portMask;
        /* Select PHY to access */
    RTK_PORTMASK_RESET(portMask);
    RTK_PORTMASK_PORT_SET(portMask, port);

    if (RT_ERR_OK != (ret = rtl9310_miim_mmd_portmask_write(unit, portMask, mmdAddr, mmdReg, data)))
        return ret;

    return RT_ERR_OK;
} /* end of rtl9310_miim_mmd_write */

/* Function Name:
 *      rtl9310_miim_mmd_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl9310 family chips.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      mmdAddr  - mmd device address
 *      mmdReg   - mmd reg id
 *      data     - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. portmask valid range is bit 0 ~ 51
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9310_miim_mmd_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mmdAddr,
    uint32          mmdReg,
    uint32          data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;
    int32   type = 0;
    int32   first_exist_port;
    drv_smi_mdxProtoSel_t proto = DRV_SMI_MDX_PROTO_END;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, portmask0=0x%x, portmask1=0x%x, mmdAddr=0x%x, mmdReg=0x%x \
           data=0x%x", unit,
           RTK_PORTMASK_WORD_GET(portmask, 0),
           RTK_PORTMASK_WORD_GET(portmask, 1),
           mmdAddr, mmdReg, data);


    first_exist_port = RTK_PORTMASK_GET_FIRST_PORT(portmask);

    if(first_exist_port < 0 || !HWP_PORT_EXIST(unit, first_exist_port))
        return RT_ERR_PORT_ID;

    if ((ret = rtl9310_miim_portSmiMdxProto_get(unit, first_exist_port, &proto)) != RT_ERR_OK)
    {
        return ret;
    }
    if (proto == DRV_SMI_MDX_PROTO_C45)
        type = 2;
    else
        type = 1;

    PHY_SEM_LOCK(unit);

    if ((ret = _rtl9310_miim_accessPortmask_set(unit, &portmask)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Input parameters*/
    if ((ret = reg_field_write(unit, MANGO_SMI_INDRT_ACCESS_CTRL_3r, MANGO_WRTIE_DATAf, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd device address to access */
    if ((ret = reg_field_write(unit, MANGO_SMI_INDRT_ACCESS_MMD_CTRLr, MANGO_MMD_DEVADf, &mmdAddr)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd register to access */
    if ((ret = reg_field_write(unit, MANGO_SMI_INDRT_ACCESS_MMD_CTRLr, MANGO_MMD_REGf, &mmdReg)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl9310_miim_phyRegAccessCtrlRegFields_set(unit,
                                   0,           /* ACCESS_MOD- 0b0: mdx; 0b1: inband*/
                                   0,           /* MAIN_PAGE - page number to access */
                                   0,           /* REG       - register number to access */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   type,        /* TYPE      - 0:Normal register; 1:C22 access C45 MMD register; 2:C45 MMD register */
                                   1,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    /* write register to active the write operation */
    if ((ret = reg_write(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, RTL9310_SMI_INDRT_ACCESS_CTRL_0_CMD_MASK, RTL9310_SMI_INDRT_ACCESS_CTRL_0_FAIL_MASK);

    /* check the FAIL bit */
    if ((ret = reg_field_read(unit, MANGO_SMI_INDRT_ACCESS_CTRL_0r, MANGO_FAILf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    if(val == 1)
    {
        PHY_SEM_UNLOCK(unit);
        return RT_ERR_PHY_ACCESS_FAIL;
    }

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;

} /* end of rtl9310_miim_mmd_portmask_write */

/* Function Name:
 *      rtl9310_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      unit  - unit id
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      None
 */
int32
rtl9310_table_read(
    uint32  unit,
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    uint32      reg_data, reg_value;
  #if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
    uint32      busy;
  #endif
    int32       ret = RT_ERR_FAILED;
    rtk_table_t *pTable = NULL;
    uint32      groupId;
    rtk_indirectCtrlGroup_t ctrlGroup[] = {
        INDIRECT_CTRL_GROUP_SET0,
        INDIRECT_CTRL_GROUP_SET1,
        INDIRECT_CTRL_GROUP_SET2,
        INDIRECT_CTRL_GROUP_SET3,
        INDIRECT_CTRL_GROUP_SET4,
        INDIRECT_CTRL_GROUP_SET5 };
    rtk_mango_reg_list_t ctrlReg[] = {
        MANGO_TBL_ACCESS_CTRL_0r,
        MANGO_TBL_ACCESS_CTRL_1r,
        MANGO_TBL_ACCESS_CTRL_2r,
        MANGO_TBL_ACCESS_CTRL_3r,
        MANGO_TBL_ACCESS_CTRL_4r,
        MANGO_TBL_ACCESS_CTRL_5r };
    rtk_mango_reg_list_t dataReg[] = {
        MANGO_TBL_ACCESS_DATA_0r,
        MANGO_TBL_ACCESS_DATA_1r,
        MANGO_TBL_ACCESS_DATA_2r,
        MANGO_TBL_ACCESS_DATA_3r,
        MANGO_TBL_ACCESS_DATA_4r,
        MANGO_TBL_ACCESS_DATA_5r };
    uint32      index;
    uint32      hit = 0, l2GetNext = 0;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, table=%d, addr=0x%x", unit, table, addr);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX(unit) && (table != TBL_L2_SRAM_FIND_NEXT && table != TBL_L2_BCAM_FIND_NEXT)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);


    if (table == TBL_L2_SRAM_FIND_NEXT)
    {
        l2GetNext = 1;
        table = MANGO_L2_UCt;
    }
    else if (table == TBL_L2_BCAM_FIND_NEXT)
    {
        l2GetNext = 1;
        table = MANGO_L2_CAM_UCt;
    }

    pTable = table_find(unit, table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    groupId = pTable->set;

    MEM_SEM_LOCK(unit, ctrlGroup[groupId]);

    /* initialize variable */
    reg_data = 0;
  #if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
    busy = 0;
  #endif

    /* Command hardware to execute indirect table access
     * 0b0: not execute
     * 0b1: execute
     * Note: This bit is common used by software and hardware.
     *       When hardware completes the table access, it will clear this bit.
     */
    reg_value = 1;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MANGO_EXECf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Table access operation
     * 0b0: read
     * 0b1: write
     */
    reg_value = 0;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MANGO_CMDf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* access table type */
    reg_value = pTable->type;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MANGO_TBLf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Select access address of the table */
    if((ctrlGroup[groupId] == INDIRECT_CTRL_GROUP_SET2) && (pTable->type == 3 || pTable->type == 4))
    {
        /* In L3 HOST/PREFIX table, the 6th/7th entry of a row does NOT exist,
         * have to do the 6:8 mapping here.
         */
        reg_value = L3_ENTRY_IDX_TO_ADDR(addr);
    }
    else
    {
        reg_value = addr;
    }
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MANGO_ADDRf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(unit, ctrlReg[groupId], &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

#if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(unit, ctrlReg[groupId], MANGO_EXECf, &busy)) != RT_ERR_OK)
        {
            MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
            return ret;
        }
    } while (busy);
#endif

    /* Read table data from indirect data register */
    for (index=0; index<(pTable->datareg_num); index++)
    {
        if ((ret = reg_array_read(unit, dataReg[groupId], REG_ARRAY_INDEX_NONE, index, pData + index)) != RT_ERR_OK)
        {
            MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
            return ret;
        }
    }

    if (l2GetNext)
    {
        if ((ret = reg_field_read(unit, MANGO_TBL_ACCESS_L2_METHOD_CTRLr, MANGO_HITf, &hit)) != RT_ERR_OK)
        {
            MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
            return ret;
        }
        if ((ret = reg_field_read(unit, MANGO_TBL_ACCESS_L2_METHOD_CTRLr, MANGO_HIT_ADDRf, &pData[3])) != RT_ERR_OK)
        {
            MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
            return ret;
        }
        pData[3] |= hit << 15;
    }

    MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);

    return RT_ERR_OK;
} /* end of rtl9310_table_read */

/* Function Name:
 *      rtl9310_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
 *      unit  - unit id
 *      table - table index
 *      addr  - entry address of the table
 *      pData - pointer buffer of table entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      None
 */
int32
rtl9310_table_write(
    uint32  unit,
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    uint32      reg_data, reg_value;
  #if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
    uint32      busy;
  #endif
    int32       ret = RT_ERR_FAILED;
    rtk_table_t *pTable = NULL;
    uint32      groupId;
    rtk_indirectCtrlGroup_t ctrlGroup[] = {
        INDIRECT_CTRL_GROUP_SET0,
        INDIRECT_CTRL_GROUP_SET1,
        INDIRECT_CTRL_GROUP_SET2,
        INDIRECT_CTRL_GROUP_SET3,
        INDIRECT_CTRL_GROUP_SET4,
        INDIRECT_CTRL_GROUP_SET5 };
    rtk_mango_reg_list_t ctrlReg[] = {
        MANGO_TBL_ACCESS_CTRL_0r,
        MANGO_TBL_ACCESS_CTRL_1r,
        MANGO_TBL_ACCESS_CTRL_2r,
        MANGO_TBL_ACCESS_CTRL_3r,
        MANGO_TBL_ACCESS_CTRL_4r,
        MANGO_TBL_ACCESS_CTRL_5r };
    rtk_mango_reg_list_t dataReg[] = {
        MANGO_TBL_ACCESS_DATA_0r,
        MANGO_TBL_ACCESS_DATA_1r,
        MANGO_TBL_ACCESS_DATA_2r,
        MANGO_TBL_ACCESS_DATA_3r,
        MANGO_TBL_ACCESS_DATA_4r,
        MANGO_TBL_ACCESS_DATA_5r };
    uint32      index;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, table=%d, addr=0x%x", unit, table, addr);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(unit, table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);

    if (MANGO_FLOW_CNTRt == table)
    {
        RT_PARAM_CHK(((addr & 0xFFF) >= pTable->size), RT_ERR_OUT_OF_RANGE);
    }
    else
    {
        RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);
    }

    groupId = pTable->set;

    MEM_SEM_LOCK(unit, ctrlGroup[groupId]);

    /* initialize variable */
    reg_data = 0;
  #if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
    busy = 0;
  #endif

    /* Write pre-configure table data to indirect data register */
    for (index=0; index<(pTable->datareg_num); index++)
    {
        if ((ret = reg_array_write(unit, dataReg[groupId], REG_ARRAY_INDEX_NONE, index, pData + index)) != RT_ERR_OK)
        {
            MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
            return ret;
        }
    }

    /* Command hardware to execute indirect table access
     * 0b0: not execute
     * 0b1: execute
     * Note: This bit is common used by software and hardware.
     *       When hardware completes the table access, it will clear this bit.
     */
    reg_value = 1;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MANGO_EXECf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Table access operation
     * 0b0: read
     * 0b1: write
     */
    reg_value = 1;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MANGO_CMDf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* access table type */
    reg_value = pTable->type;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MANGO_TBLf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Select access address of the table */
    if((ctrlGroup[groupId] == INDIRECT_CTRL_GROUP_SET2) && (pTable->type == 3 || pTable->type == 4))
    {
        /* In L3 HOST/PREFIX table, the 6th/7th entry of a row does NOT exist,
         * have to do the 6:8 mapping here.
         */
        reg_value = L3_ENTRY_IDX_TO_ADDR(addr);
    }
    else
    {
        reg_value = addr;
    }
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MANGO_ADDRf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(unit, ctrlReg[groupId], &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

#if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(unit, ctrlReg[groupId], MANGO_EXECf, &busy)) != RT_ERR_OK)
        {
            MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
            return ret;
        }
    } while (busy);
#endif

    MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);

    return RT_ERR_OK;
} /* end of rtl9310_table_write */

/* Function Name:
 *      rtl9310_miim_pollingEnable_get
 * Description:
 *      Get the mac polling PHY status of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pEnabled - pointer buffer of mac polling PHY status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
rtl9310_miim_pollingEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    *pEnabled)
{
    uint32 val;
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d", unit, port);

    RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnabled), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(unit, MANGO_SMI_PORT_POLLING_CTRLr,
        port, REG_ARRAY_INDEX_NONE, MANGO_SMI_POLLING_PMSKf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }

    if (val == 1)
        (*pEnabled) = ENABLED;
    else
        (*pEnabled) = DISABLED;

    return RT_ERR_OK;
} /* end of rtl9310_miim_pollingEnable_get */

/* Function Name:
 *      rtl9310_miim_pollingEnable_set
 * Description:
 *      Set the mac polling PHY status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enabled - mac polling PHY status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
rtl9310_miim_pollingEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    enabled)
{
    uint32 val;
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, enabled=%d", unit, port, enabled);

    RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enabled != DISABLED && enabled != ENABLED), RT_ERR_INPUT);

    if (enabled)
        val = 1;
    else
        val = 0;

    if ((ret = reg_array_field_write(unit, MANGO_SMI_PORT_POLLING_CTRLr,
        port, REG_ARRAY_INDEX_NONE, MANGO_SMI_POLLING_PMSKf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9310_miim_pollingEnable_set */


/* Function Name:
 *      rtl9310_smiAddr_init
 * Description:
 *      Initialize SMI Address (PHY address).
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32 rtl9310_smiAddr_init(uint32 unit)
{
    int32       ret;
    uint32      mac_id, smi_id, phy_addr, val;
    uint32      mdc_en[DRV_RTL9310_SMI_MAX] = { 0 };
    uint32      mdc_en_f[DRV_RTL9310_SMI_MAX] = { MANGO_SET0_MDC_ENf, MANGO_SET1_MDC_ENf, MANGO_SET2_MDC_ENf, MANGO_SET3_MDC_ENf };

    /* disable MAC-poll-PHY */
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, mac_id)
    {
        val = 0;
        if ((ret = reg_array_field_write(unit, MANGO_SMI_PORT_POLLING_CTRLr,
                            mac_id, REG_ARRAY_INDEX_NONE, MANGO_SMI_POLLING_PMSKf, &val)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u port %u disable polling faile 0x%x", unit, mac_id, ret);
        }
    }
    osal_time_mdelay(50);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, mac_id)
    {
        if ((phy_addr = HWP_PHY_ADDR(unit, mac_id)) == HWP_NONE)
            continue;

        ret = reg_array_field_write(unit, MANGO_SMI_PORT_ADDR_CTRLr, mac_id, REG_ARRAY_INDEX_NONE, MANGO_PORT_ADDRf, &phy_addr);
        if (ret != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u set phy addr %u fail %d", unit, mac_id, phy_addr, ret);
        }

        if ((smi_id = (uint32)HWP_PORT_SMI(unit, mac_id)) != HWP_NONE)
        {
            if (smi_id >= DRV_RTL9310_SMI_MAX)
            {
                RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u smi_id %u out of range", unit, mac_id, smi_id);
                continue;
            }

            ret = reg_array_field_write(unit, MANGO_SMI_PORT_POLLING_SELr, mac_id, REG_ARRAY_INDEX_NONE, MANGO_PORT_POLLING_SELf, &smi_id);
            if (ret != RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %d macId %d SMI_PORTx_y_POLLING_SEL set %u fail 0x%x", unit, mac_id, smi_id, ret);
            }

            mdc_en[smi_id] = 1;

        }
    }/* end for */

    for (smi_id = 0; smi_id < DRV_RTL9310_SMI_MAX; smi_id++)
    {
        if (mdc_en[smi_id] != 1)
            continue;

        ret = reg_field_write(unit, MANGO_MAC_L2_GLOBAL_CTRL2r, mdc_en_f[smi_id], &mdc_en[smi_id]);
        if (ret != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %d SET%u_MDC_ENf set fail 0x%x", unit, smi_id, ret);
        }

    }/* end for */

    return RT_ERR_OK;
} /* end of rtl9310_smiAddr_init */

/* Function Name:
 *      drv_port_txEnable_get
 * Description:
 *      Get the TX enable status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port TX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The TX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
drv_port_txEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(drv_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    DRV_SEM_LOCK(unit);

    RT_ERR_HDL(reg_array_field_read(unit, MANGO_MAC_L2_PORT_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_TX_ENf, &val), ERR, ret);

    DRV_SEM_UNLOCK(unit);

    if (1 == val)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;

ERR:
    DRV_SEM_UNLOCK(unit);
    return ret;
}   /* end of drv_port_txEnable_get */

/* Function Name:
 *      drv_port_txEnable_set
 * Description:
 *      Set the TX enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      enable         - enable status of TX
 *      module         - which module set the function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The TX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
drv_port_txEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable, drv_rtl9310_port_txrx_en_mod_t module)
{
    rtk_bitmap_t    emptyMod[BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END)];
    rtk_bitmap_t    oriModInfo[BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END)];
    uint32          val;
    int32           ret = RT_ERR_OK;

    /* check Init status */
    RT_INIT_CHK(drv_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((module >= DRV_RTL9310_PORT_TXRX_EN_MOD_END), RT_ERR_INPUT);

    RT_LOG(LOG_DEBUG, (MOD_RTDRV), "TX_EN Port %d module %d sts %d\n", port, module, enable);

    BITMAP_ASSIGN(oriModInfo, port_tx_en_info[unit][port]->disModInfo,
            BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END));

    if (ENABLED == enable)
        BITMAP_CLEAR(port_tx_en_info[unit][port]->disModInfo, module);
    else
        BITMAP_SET(port_tx_en_info[unit][port]->disModInfo, module);

    BITMAP_RESET(emptyMod, BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END));

    if (0 == BITMAP_COMPARE(port_tx_en_info[unit][port]->disModInfo, emptyMod,
            BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END)))
        val = 1;
    else
        val = 0;

    DRV_SEM_LOCK(unit);

    RT_ERR_HDL(reg_array_field_write(unit, MANGO_MAC_L2_PORT_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_TX_ENf, &val), ERR, ret);

    DRV_SEM_UNLOCK(unit);

    return RT_ERR_OK;

ERR:
    BITMAP_ASSIGN(port_tx_en_info[unit][port]->disModInfo, oriModInfo,
            BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END));

    DRV_SEM_UNLOCK(unit);

    return ret;
}   /* end of drv_port_txEnable_set */

/* Function Name:
 *      drv_port_rxEnable_get
 * Description:
 *      Get the RX enable status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port RX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The TX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
drv_port_rxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  val;
    int32   ret;

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    DRV_SEM_LOCK(unit);

    RT_ERR_HDL(reg_array_field_read(unit, MANGO_MAC_L2_PORT_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_RX_ENf, &val), ERR, ret);

    DRV_SEM_UNLOCK(unit);

    if (1 == val)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;

ERR:
    DRV_SEM_UNLOCK(unit);
    return ret;
}   /* end of drv_port_rxEnable_get */

/* Function Name:
 *      drv_port_moduleRxEnable_get
 * Description:
 *      Get the module RX enable status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      module  - module id
 * Output:
 *      pEnable - pointer to the port RX status of module
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The TX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
drv_port_moduleRxEnable_get(uint32 unit, rtk_port_t port, drv_rtl9310_port_txrx_en_mod_t module, rtk_enable_t *pEnable)
{
    /* check Init status */
    RT_INIT_CHK(drv_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((module >= DRV_RTL9310_PORT_TXRX_EN_MOD_END), RT_ERR_INPUT);

    if (BITMAP_IS_CLEAR(port_rx_en_info[unit][port]->disModInfo, module))
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}   /* end of drv_port_moduleRxEnable_get */

/* Function Name:
 *      drv_port_rxEnable_set
 * Description:
 *      Set the RX enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      enable         - enable status of RX
 *      module         - which module set the function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The RX enable status of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
int32
drv_port_rxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable, drv_rtl9310_port_txrx_en_mod_t module)
{
    rtk_bitmap_t    emptyMod[BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END)];
    rtk_bitmap_t    oriModInfo[BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END)];
    uint32          val;
    int32           ret = RT_ERR_OK;

    /* check Init status */
    RT_INIT_CHK(drv_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((module >= DRV_RTL9310_PORT_TXRX_EN_MOD_END), RT_ERR_INPUT);

    RT_LOG(LOG_DEBUG, (MOD_RTDRV), "RX_EN Port %d module %d sts %d\n", port, module, enable);

    BITMAP_ASSIGN(oriModInfo, port_rx_en_info[unit][port]->disModInfo,
            BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END));

    if (ENABLED == enable)
        BITMAP_CLEAR(port_rx_en_info[unit][port]->disModInfo, module);
    else
        BITMAP_SET(port_rx_en_info[unit][port]->disModInfo, module);

    BITMAP_RESET(emptyMod, BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END));

    if (0 == BITMAP_COMPARE(port_rx_en_info[unit][port]->disModInfo, emptyMod,
            BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END)))
        val = 1;
    else
        val = 0;

    DRV_SEM_LOCK(unit);

    RT_ERR_HDL(reg_array_field_write(unit, MANGO_MAC_L2_PORT_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_RX_ENf, &val), ERR, ret);

    DRV_SEM_UNLOCK(unit);

    return RT_ERR_OK;

ERR:
    BITMAP_ASSIGN(port_rx_en_info[unit][port]->disModInfo, oriModInfo,
            BITMAP_ARRAY_CNT(DRV_RTL9310_PORT_TXRX_EN_MOD_END));

    DRV_SEM_UNLOCK(unit);

    return ret;
}   /* end of drv_port_rxEnable_set */

/* RTL9310 mac driver service APIs */
rt_macdrv_t rtl9310_macdrv =
{
    .fMdrv_init                             = rtl9310_init,
    .fMdrv_miim_read                        = rtl9310_miim_read,
    .fMdrv_miim_write                       = rtl9310_miim_write,
    .fMdrv_miim_park_read                   = rtl9310_miim_park_read,
    .fMdrv_miim_park_write                  = rtl9310_miim_park_write,
    .fMdrv_miim_broadcast_write             = rtl9310_miim_broadcast_write,
    .fMdrv_miim_extParkPage_read            = rtl9310_miim_extParkPage_read,
    .fMdrv_miim_extParkPage_write           = rtl9310_miim_extParkPage_write,
    .fMdrv_miim_extParkPage_portmask_write  = rtl9310_miim_extParkPage_portmask_write,
    .fMdrv_miim_mmd_read                    = rtl9310_miim_mmd_read,
    .fMdrv_miim_mmd_write                   = rtl9310_miim_mmd_write,
    .fMdrv_miim_mmd_portmask_write          = rtl9310_miim_mmd_portmask_write,
    .fMdrv_table_read                       = rtl9310_table_read,
    .fMdrv_table_write                      = rtl9310_table_write,
    .fMdrv_port_probe                       = rtl9310_port_probe,
    .fMdrv_miim_portmask_write              = rtl9310_miim_portmask_write,
    .fMdrv_miim_pollingEnable_get           = rtl9310_miim_pollingEnable_get,
    .fMdrv_miim_pollingEnable_set           = rtl9310_miim_pollingEnable_set,
    .fMdrv_mac_serdes_rst                   = drv_rtl9310_sds_rx_rst,
    .fMdrv_mac_serdes_read                  = drv_rtl9310_sds_read,
    .fMdrv_mac_serdes_write                 = drv_rtl9310_sds_write,
    .fMdrv_smi_read                         = NULL,
    .fMdrv_smi_write                        = NULL,
    .fMdrv_miim_portSmiMdxProto_set         = rtl9310_miim_portSmiMdxProto_set,
    .fMdrv_miim_portSmiMdxProto_get         = rtl9310_miim_portSmiMdxProto_get,
}; /* end of rtl9310_macdrv */

