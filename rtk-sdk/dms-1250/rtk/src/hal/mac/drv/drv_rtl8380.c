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
#if defined(CONFIG_SDK_RTL8231) && defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
#include <private/drv/rtl8231/rtl8231.h>
#endif
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/mac/drv/drv_rtl8380.h>
#include <hal/mac/drv/drv.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>
#include <hal/mac/mac_probe.h>
#include <hal/mac/led/led_rtl8380.h>
#include <soc/type.h>
#include <osal/time.h>
#include <ioal/mem32.h>
#include <hwp/hw_profile.h>
#include <dal/dal_construct.h>
#include <dal/maple/dal_maple_construct.h>

/*
 * Symbol Definition
 */
uint32 rtl8380_sds_modify_flag = 0;
/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
int32 _rtl8380_miim_write(uint32 unit, rtk_port_t port, uint32 page, uint32 phy_reg, uint32 data);

/* Function Name:
 *      rtl8380_port_probe
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
rtl8380_port_probe(uint32 unit)
{
    return RT_ERR_OK;
} /* end of rtl8380_port_probe */


/* Function Name:
 *      rtl8380_smiAddr_init
 * Description:
 *      Initialize SMI Address (PHY address) of specific port.
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
void rtl8380_smiAddr_init(uint32 unit)
{
    uint32 mac_id,phy_id;
    uint32 port_field[]={
         MAPLE_PORT0_ADDRf, MAPLE_PORT1_ADDRf, MAPLE_PORT2_ADDRf, MAPLE_PORT3_ADDRf, MAPLE_PORT4_ADDRf,
         MAPLE_PORT5_ADDRf, MAPLE_PORT6_ADDRf, MAPLE_PORT7_ADDRf, MAPLE_PORT8_ADDRf, MAPLE_PORT9_ADDRf,
        MAPLE_PORT10_ADDRf,MAPLE_PORT11_ADDRf,MAPLE_PORT12_ADDRf,MAPLE_PORT13_ADDRf,MAPLE_PORT14_ADDRf,
        MAPLE_PORT15_ADDRf,MAPLE_PORT16_ADDRf,MAPLE_PORT17_ADDRf,MAPLE_PORT18_ADDRf,MAPLE_PORT19_ADDRf,
        MAPLE_PORT20_ADDRf,MAPLE_PORT21_ADDRf,MAPLE_PORT22_ADDRf,MAPLE_PORT23_ADDRf,MAPLE_PORT24_ADDRf,
        MAPLE_PORT25_ADDRf,MAPLE_PORT26_ADDRf,MAPLE_PORT27_ADDRf,
    };

    for(mac_id=0;mac_id<=5;mac_id++)
    {
        phy_id = HWP_PHY_ADDR(unit,mac_id);
        if(phy_id==HWP_NONE)
            continue;
        reg_field_write(unit, MAPLE_SMI_PORT0_5_ADDR_CTRLr, port_field[mac_id], &phy_id);
    }

    for(mac_id=6;mac_id<=11;mac_id++)
    {
        phy_id = HWP_PHY_ADDR(unit,mac_id);
        if(phy_id==HWP_NONE)
            continue;
        reg_field_write(unit, MAPLE_SMI_PORT6_11_ADDR_CTRLr, port_field[mac_id], &phy_id);
    }

    for(mac_id=12;mac_id<=17;mac_id++)
    {
        phy_id = HWP_PHY_ADDR(unit,mac_id);
        if(phy_id==HWP_NONE)
            continue;
        reg_field_write(unit, MAPLE_SMI_PORT12_17_ADDR_CTRLr, port_field[mac_id], &phy_id);
    }

    for(mac_id=18;mac_id<=23;mac_id++)
    {
        phy_id = HWP_PHY_ADDR(unit,mac_id);
        if(phy_id==HWP_NONE)
            continue;
        reg_field_write(unit, MAPLE_SMI_PORT18_23_ADDR_CTRLr, port_field[mac_id], &phy_id);
    }

    for(mac_id=24;mac_id<=27;mac_id++)
    {
        phy_id = HWP_PHY_ADDR(unit,mac_id);
        if(phy_id==HWP_NONE)
            continue;
        reg_field_write(unit, MAPLE_SMI_PORT24_27_ADDR_CTRLr, port_field[mac_id], &phy_id);
    }
}

/* Function Name:
 *      rtl8380_reg_led_blinking_pre_init
 * Description:
 *      rtl8380 led pre init config
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
 void
 rtl8380_reg_led_blinking_pre_init(uint32 unit)
{
    uint32 val;

    val = 0x5;
    reg_write(unit, MAPLE_INT_MODE_CTRLr, &val);

    val = 0x0;
    reg_field_write(unit, MAPLE_SMI_GLB_CTRLr, MAPLE_EX_PHY_MAN_24_27f, &val);

    osal_time_udelay(100);
    val = 0x1;
    reg_field_write(unit, MAPLE_SMI_GLB_CTRLr, MAPLE_EX_PHY_MAN_24_27f, &val);
    rtl8380_sds_modify_flag = 1;

}

/* Function Name:
 *      rtl8380_regDefault_init
 * Description:
 *      Initialize default value of some registers
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
rtl8380_regDefault_init(uint32 unit)
{
    uint32 val;

    /*Protect settings to prevent access out of range,
        becasue register Range is different from 82M & 80M, 32M & 30M*/
    val = 0x3;
    reg_write(unit, MAPLE_INT_RW_CTRLr, &val);

    val = 0x1;
    reg_field_write(unit, MAPLE_EXTENDED_FEATURE_CTRL0r, MAPLE_PROTECT_CIRCUIT_ENABLEf, &val);

    val = 0x0;
    reg_write(unit, MAPLE_INT_RW_CTRLr, &val);

    rtl8380_reg_led_blinking_pre_init(unit);
    return RT_ERR_OK;
}


/* Function Name:
 *      rtl8380_init
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
rtl8380_init(uint32 unit)
{
#if defined(CONFIG_SDK_RTL8231) && defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    int32       ret = RT_ERR_FAILED;
#endif

    rtl8380_regDefault_init(unit);
    rtl8380_smiAddr_init(unit);
    {
        uint8 ledModeInitSkip = LEDMODEINITSKIP_NO;
#if !defined(__BOOTLOADER__)
        ioal_param_ledInitFlag_get(&ledModeInitSkip);
#endif
        if (LEDMODEINITSKIP_NO == ledModeInitSkip)
        {
            rtl8380_led_config(unit);
        }
    }


#if defined(CONFIG_SDK_RTL8231) && defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)

    /* register rtl8231 mdc semphore callback function */
    if ((ret = drv_rtl8231_mdcSem_register(unit, rtl83xx_mdcSem_callback)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }
#endif

    /* Initialize the threshold value of chip buffer if need */
    return RT_ERR_OK;
} /* end of rtl8380_init */


/* Function Name:
 *      rtl8380_miim_read
 * Description:
 *      Get PHY registers from rtl8380 family chips.
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl8380_miim_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      *pData)
{
    uint32 temp;
    uint32 val;
    uint32 port_rmk_regaddr;
    uint32 port_rmk_phyid = 0;
    uint32 parkPage;

    int32  ret = RT_ERR_FAILED;


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
            _rtl8380_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, page);
            page = HAL_MIIM_PAGE_ID_MAX(unit);
        }
    }


    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * INDATA[5:0] is the PHY address WHEN RWOP = 0b0
     */
     /*For 80 8ports mode & 16 ports mode, loader may remark phyID,
        so here get real phyID according to remark table, although 82m no need this, but here also using this mechanism*/
    if((port >= 0) && (port <= 5))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT0_5_ADDR_CTRLr;
    }
    else if((port >= 6) && (port <= 11))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT6_11_ADDR_CTRLr;
    }
    else if((port >= 12) && (port <= 17))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT12_17_ADDR_CTRLr;
    }
    else if((port >= 18) && (port <= 23))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT18_23_ADDR_CTRLr;
    }
    else if((port >= 24) && (port <= 27))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT24_27_ADDR_CTRLr;
    }
    else
    {
        PHY_SEM_UNLOCK(unit);
        return RT_ERR_PORT_ID;
    }

    if ((ret = reg_read(unit, port_rmk_regaddr, &port_rmk_phyid)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    port = (port_rmk_phyid>>(5*(port%6))) & 0x1F;

    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_2r, MAPLE_INDATA_15_0f, &port)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select register number to access */
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_REG_ADDR_4_0f, &phy_reg, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select page number to access */
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_MAIN_PAGE_11_0f, &page, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select parked page*/
    /*Always not Park, because 80 mac polling always send right page*/
    parkPage = 0x1f;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_PARK_PAGE_4_0f, &parkPage, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }


    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 0;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 0;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, 0x1, 0);

    /* get the read operation result to pData */
    if ((ret = reg_field_read(unit, MAPLE_SMI_ACCESS_PHY_CTRL_2r, MAPLE_DATA_15_0f, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    PHY_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_HAL), "pData=0x%x", *pData);

    return RT_ERR_OK;
} /* end of rtl8380_miim_read */


/* Function Name:
 *      rtl8380_miim_write
 * Description:
 *      Set PHY registers in rtl8380 family chips.
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
_rtl8380_miim_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    uint32  temp;
    uint32  val;
    uint32 parkPage;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, phy_reg=0x%x, data=0x%x", unit, port, page, phy_reg, data);


    /* Select PHY to access */
    val = 1<<port;
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_0r, MAPLE_PHY_MASKf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[5:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_2r, MAPLE_INDATA_15_0f, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select register number to access */
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_REG_ADDR_4_0f, &phy_reg, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select page number to access */
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_MAIN_PAGE_11_0f, &page, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select parked page*/
    /*Always not Park, because 80 mac polling always send right page*/
    parkPage = 0x1f;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_PARK_PAGE_4_0f, &parkPage, &temp)) != RT_ERR_OK)
    {
        return ret;
    }


    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 0;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, 0x1, 0);

    return RT_ERR_OK;
} /* end of rtl8380_miim_write */

/* Function Name:
 *      rtl8380_miim_write
 * Description:
 *      Set PHY registers in rtl8380 family chips.
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl8380_miim_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, phy_reg=0x%x, data=0x%x", unit, port, page, phy_reg, data);
    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    PHY_SEM_LOCK(unit);

    if (page == HAL_MIIM_FIX_PAGE)
    {
        page = HAL_MIIM_PAGE_ID_MAX(unit);
    }
    else
    {
        if (page > HAL_MIIM_PAGE_ID_MAX(unit))
        {
            _rtl8380_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, page);
            page = HAL_MIIM_PAGE_ID_MAX(unit);
        }
    }

    ret = _rtl8380_miim_write(unit,  port, page, phy_reg, data);
    PHY_SEM_UNLOCK(unit);

    return ret;

}


/* Function Name:
 *      rtl8380_miim_park_read
 * Description:
 *      Get PHY registers from rtl8380 family chips.
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl8380_miim_park_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData)
{
    uint32 temp;
    uint32 val;
    uint32 port_rmk_regaddr;
    uint32 port_rmk_phyid = 0;

    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, page=0x%x, phy_reg=0x%x", unit, port, page, phy_reg);

    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((parkPage > 0x1F), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if (page == HAL_MIIM_FIX_PAGE)
    {
        page = HAL_MIIM_PAGE_ID_MAX(unit);
    }
    else
    {
        RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX(unit)), RT_ERR_PHY_PAGE_ID);
    }

    PHY_SEM_LOCK(unit);

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * INDATA[5:0] is the PHY address WHEN RWOP = 0b0
     */
     /*For 80 8ports mode & 16 ports mode, loader may remark phyID,
        so here get real phyID according to remark table, although 82m no need this, but here also using this mechanism*/
    if((port >= 0) && (port <= 5))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT0_5_ADDR_CTRLr;
    }
    else if((port >= 6) && (port <= 11))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT6_11_ADDR_CTRLr;
    }
    else if((port >= 12) && (port <= 17))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT12_17_ADDR_CTRLr;
    }
    else if((port >= 18) && (port <= 23))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT18_23_ADDR_CTRLr;
    }
    else if((port >= 24) && (port <= 27))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT24_27_ADDR_CTRLr;
    }
    else
    {
        PHY_SEM_UNLOCK(unit);
        return RT_ERR_PORT_ID;
    }

    if ((ret = reg_read(unit, port_rmk_regaddr, &port_rmk_phyid)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    port = (port_rmk_phyid>>(5*(port%6))) & 0x1F;
    if(port >= 28)
    {
        PHY_SEM_UNLOCK(unit);
        return RT_ERR_PORT_ID;
    }

    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_2r, MAPLE_INDATA_15_0f, &port)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select register number to access */
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_REG_ADDR_4_0f, &phy_reg, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select page number to access */
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_MAIN_PAGE_11_0f, &page, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select parked page*/
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_PARK_PAGE_4_0f, &parkPage, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }


    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 0;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 0;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, 0x1, 0);

    /* get the read operation result to pData */
    if ((ret = reg_field_read(unit, MAPLE_SMI_ACCESS_PHY_CTRL_2r, MAPLE_DATA_15_0f, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    PHY_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_HAL), "pData=0x%x", *pData);

    return RT_ERR_OK;
} /* end of rtl8380_miim_read */


/* Function Name:
 *      rtl8380_miim_park_write
 * Description:
 *      Set PHY registers in rtl8380 family chips.
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl8380_miim_park_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, phy_reg=0x%x, data=0x%x", unit, port, page, phy_reg, data);
    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((parkPage > 0x1F), RT_ERR_PHY_PAGE_ID);
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
    val = 1<<port;
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_0r, MAPLE_PHY_MASKf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[5:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_2r, MAPLE_INDATA_15_0f, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select register number to access */
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_REG_ADDR_4_0f, &phy_reg, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select page number to access */
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_MAIN_PAGE_11_0f, &page, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select parked page*/
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_PARK_PAGE_4_0f, &parkPage, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }


    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 0;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, 0x1, 0);

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of rtl8380_miim_write */





/* Function Name:
 *      rtl8380_miim_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl8380 family chips.
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
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl8380_miim_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          page,
    uint32          phy_reg,
    uint32          data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, portmask=0x%x, page=0x%x, phy_reg=0x%x, data=0x%x", unit, portmask.bits[0], page, phy_reg, data);
    RT_PARAM_CHK((page >= PHY_PAGE_MAX), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    if (page == HAL_MIIM_FIX_PAGE)
    {
        page = HAL_MIIM_PAGE_ID_MAX(unit);
    }

    PHY_SEM_LOCK(unit);

    /* Select PHY to access */
    val = portmask.bits[0];
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_0r, MAPLE_PHY_MASKf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[5:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_2r, MAPLE_INDATA_15_0f, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select register number to access */
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_REG_ADDR_4_0f, &phy_reg, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select page number to access */
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_MAIN_PAGE_11_0f, &page, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }


    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 0;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, 0x1, 0);

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of rtl8380_miim_portmask_write */


/* Function Name:
 *      rtl8380_miim_mmd_read
 * Description:
 *      Get PHY registers from rtl8380 family chips.
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl8380_miim_mmd_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      *pData)
{
    uint32 temp;
    uint32 val;
    uint32 port_rmk_regaddr;
    uint32 port_rmk_phyid = 0;

    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, mmdAddr=0x%x, mmdReg=0x%x, reg=0x%x",
           unit, port, mmdAddr, mmdReg);

    RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    //RT_PARAM_CHK((mainPage > HAL_MIIM_PAGE_ID_MAX(unit)), RT_ERR_PHY_PAGE_ID);
    //RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    PHY_SEM_LOCK(unit);

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * INDATA[5:0] is the PHY address WHEN RWOP = 0b0
     */
     /*For 80 8ports mode & 16 ports mode, loader may remark phyID,
        so here get real phyID according to remark table, although 82m no need this, but here also using this mechanism*/
    if((port >= 0) && (port <= 5))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT0_5_ADDR_CTRLr;
    }
    else if((port >= 6) && (port <= 11))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT6_11_ADDR_CTRLr;
    }
    else if((port >= 12) && (port <= 17))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT12_17_ADDR_CTRLr;
    }
    else if((port >= 18) && (port <= 23))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT18_23_ADDR_CTRLr;
    }
    else if((port >= 24) && (port <= 27))
    {
        port_rmk_regaddr = MAPLE_SMI_PORT24_27_ADDR_CTRLr;
    }
    else
    {
        PHY_SEM_UNLOCK(unit);
        return RT_ERR_PORT_ID;
    }

    if ((ret = reg_read(unit, port_rmk_regaddr, &port_rmk_phyid)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    port = (port_rmk_phyid>>(5*(port%6))) & 0x1F;
    if(port >= 28)
    {
        PHY_SEM_UNLOCK(unit);
        return RT_ERR_PORT_ID;
    }

    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_2r, MAPLE_INDATA_15_0f, &port)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd device address to access */
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_3r, MAPLE_MMD_DEVAD_4_0f, &mmdAddr)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd register to access */
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_3r, MAPLE_MMD_REG_15_0f, &mmdReg)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 0;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, 0x1, 0);

    /* get the read operation result to pData */
    if ((ret = reg_field_read(unit, MAPLE_SMI_ACCESS_PHY_CTRL_2r, MAPLE_DATA_15_0f, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    PHY_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_HAL), "pData=0x%x", *pData);

    return RT_ERR_OK;
} /* end of rtl8380_miim_mmd_read */


/* Function Name:
 *      rtl8380_miim_mmd_write
 * Description:
 *      Set PHY registers in rtl8380 family chips.
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl8380_miim_mmd_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, mmdAddr=0x%x, mmdReg=0x%x \
           data=0x%x", unit, port, mmdAddr, mmdReg, data);
    RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    //RT_PARAM_CHK((page >= PHY_PAGE_MAX), RT_ERR_PHY_PAGE_ID);
    //RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    PHY_SEM_LOCK(unit);

    /* Select PHY to access */
    val = 1<<port;
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_0r, MAPLE_PHY_MASKf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[5:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_2r, MAPLE_INDATA_15_0f, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd device address to access */
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_3r, MAPLE_MMD_DEVAD_4_0f, &mmdAddr)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd register to access */
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_3r, MAPLE_MMD_REG_15_0f, &mmdReg)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, 0x1, 0);

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of rtl8380_miim_mmd_write */

/* Function Name:
 *      rtl8380_miim_mmd_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl8380 family chips.
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
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl8380_miim_mmd_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mmdAddr,
    uint32          mmdReg,
    uint32          data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, portmask=0x%x, mmdAddr=0x%x, mmdReg=0x%x \
           data=0x%x", unit, portmask.bits[0], mmdAddr, mmdReg, data);
    //RT_PARAM_CHK((page >= PHY_PAGE_MAX), RT_ERR_PHY_PAGE_ID);
    //RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    PHY_SEM_LOCK(unit);

    /* Select PHY to access */
    val = portmask.bits[0];
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_0r, MAPLE_PHY_MASKf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[5:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_2r, MAPLE_INDATA_15_0f, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd device address to access */
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_3r, MAPLE_MMD_DEVAD_4_0f, &mmdAddr)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd register to access */
    if ((ret = reg_field_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_3r, MAPLE_MMD_REG_15_0f, &mmdReg)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, MAPLE_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, MAPLE_SMI_ACCESS_PHY_CTRL_1r, 0x1, 0);

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of rtl8380_miim_mmd_portmask_write */



/* Function Name:
 *      rtl8380_table_read
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
rtl8380_table_read(
    uint32  unit,
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    uint32      reg_data, reg_value;
    uint32      busy;
    int32       ret = RT_ERR_FAILED;
    rtk_table_t *pTable = NULL;
    uint32      groupId;

    rtk_indirectCtrlGroup_t ctrlGroup[] = {
        INDIRECT_CTRL_GROUP_L2,
        INDIRECT_CTRL_GROUP_TABLE,
        INDIRECT_CTRL_GROUP_PKT_ENC,
        INDIRECT_CTRL_GROUP_EGR_CTRL };
    rtk_maple_reg_list_t ctrlReg[] = {
        MAPLE_TBL_ACCESS_L2_CTRLr,
        MAPLE_TBL_ACCESS_CTRL_0r,
        MAPLE_TBL_ACCESS_CTRL_1r
         };
    rtk_maple_reg_list_t dataReg[] = {
        MAPLE_TBL_ACCESS_L2_DATAr,
        MAPLE_TBL_ACCESS_DATA_0r,
        MAPLE_TBL_ACCESS_DATA_1r
         };
    uint32      index;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, table=%d, addr=0x%x", unit, table, addr);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(unit, table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch (table)
    {
        case MAPLE_L2_UCt:
        case MAPLE_L2_MCt:
        case MAPLE_L2_IP_MC_SIPt:
        case MAPLE_L2_IP_MCt:
        case MAPLE_L2_NEXT_HOPt:
        case MAPLE_L2_NEXT_HOP_LEGACYt:
        case MAPLE_L2_CAM_UCt:
        case MAPLE_L2_CAM_MCt:
        case MAPLE_L2_CAM_IP_MC_SIPt:
        case MAPLE_L2_CAM_IP_MCt:
        case MAPLE_MC_PMSKt:
            groupId = 0;
            break;

        case MAPLE_VLANt:
        case MAPLE_IACLt:
        case MAPLE_LOGt:
        case MAPLE_MSTIt:
            groupId = 1;
            break;

        case MAPLE_UNTAGt:
        case MAPLE_VLAN_EGR_CNVTt:
        case MAPLE_ROUTINGt:
            groupId = 2;
            break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    MEM_SEM_LOCK(unit, ctrlGroup[groupId]);

    /* initialize variable */
    reg_data = 0;
    busy = 0;

    /* Command hardware to execute indirect table access
     * 0b0: not execute
     * 0b1: execute
     * Note: This bit is common used by software and hardware.
     *       When hardware completes the table access, it will clear this bit.
     */
    reg_value = 1;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MAPLE_EXECf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Table access operation
     * 0b0: write
     * 0b1: read
     */
    reg_value = 1;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MAPLE_CMDf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* access table type */
    reg_value = pTable->type;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MAPLE_TBLf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Select access address of the table */
    reg_value = addr;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MAPLE_ADDRf, &reg_value, &reg_data)) != RT_ERR_OK)
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
        if ((ret = reg_field_read(unit, ctrlReg[groupId], MAPLE_EXECf, &busy)) != RT_ERR_OK)
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

    MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);

    return RT_ERR_OK;
} /* end of rtl8380_table_read */


/* Function Name:
 *      rtl8380_table_write
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
rtl8380_table_write(
    uint32  unit,
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    uint32      reg_data, reg_value;
    uint32      busy;
    int32       ret = RT_ERR_FAILED;
    rtk_table_t *pTable = NULL;
    uint32      groupId;

    rtk_indirectCtrlGroup_t ctrlGroup[] = {
        INDIRECT_CTRL_GROUP_L2,
        INDIRECT_CTRL_GROUP_TABLE,
        INDIRECT_CTRL_GROUP_PKT_ENC,
        INDIRECT_CTRL_GROUP_EGR_CTRL };
    rtk_maple_reg_list_t ctrlReg[] = {
        MAPLE_TBL_ACCESS_L2_CTRLr,
        MAPLE_TBL_ACCESS_CTRL_0r,
        MAPLE_TBL_ACCESS_CTRL_1r,
        };
    rtk_maple_reg_list_t dataReg[] = {
        MAPLE_TBL_ACCESS_L2_DATAr,
        MAPLE_TBL_ACCESS_DATA_0r,
        MAPLE_TBL_ACCESS_DATA_1r,
        };
    uint32      index;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, table=%d, addr=0x%x", unit, table, addr);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(unit, table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    if(MAPLE_LOGt != table)
        RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);
    else
        RT_PARAM_CHK(((addr&0x7f) >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch (table)
    {
        case MAPLE_L2_UCt:
        case MAPLE_L2_MCt:
        case MAPLE_L2_IP_MC_SIPt:
        case MAPLE_L2_IP_MCt:
        case MAPLE_L2_NEXT_HOPt:
        case MAPLE_L2_NEXT_HOP_LEGACYt:
        case MAPLE_L2_CAM_UCt:
        case MAPLE_L2_CAM_MCt:
        case MAPLE_L2_CAM_IP_MC_SIPt:
        case MAPLE_L2_CAM_IP_MCt:
        case MAPLE_MC_PMSKt:
            groupId = 0;
            break;

        case MAPLE_VLANt:
        case MAPLE_IACLt:
        case MAPLE_LOGt:
        case MAPLE_MSTIt:
            groupId = 1;
            break;

        case MAPLE_UNTAGt:
        case MAPLE_VLAN_EGR_CNVTt:
        case MAPLE_ROUTINGt:
             groupId = 2;
            break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    MEM_SEM_LOCK(unit, ctrlGroup[groupId]);

    /* initialize variable */
    reg_data = 0;
    busy = 0;

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
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MAPLE_EXECf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Table access operation
     * 0b0: write
     * 0b1: read
     */
    reg_value = 0;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MAPLE_CMDf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* access table type */
    reg_value = pTable->type;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MAPLE_TBLf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Select access address of the table */
    reg_value = addr;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], MAPLE_ADDRf, &reg_value, &reg_data)) != RT_ERR_OK)
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
        if ((ret = reg_field_read(unit, ctrlReg[groupId], MAPLE_EXECf, &busy)) != RT_ERR_OK)
        {
            MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
            return ret;
        }
    } while (busy);
#endif

    MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);

    return RT_ERR_OK;
} /* end of rtl8380_table_write */

/* Function Name:
 *      rtl8380_miim_pollingEnable_get
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
rtl8380_miim_pollingEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    *pEnabled)
{
    uint32 val;
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d", unit, port);

    RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnabled), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(unit, MAPLE_SMI_POLL_CTRLr,
        MAPLE_SMI_POLL_MASK_27_0f, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }

    if (((val >> port) & 0x1) == 1)
        (*pEnabled) = ENABLED;
    else
        (*pEnabled) = DISABLED;

    return RT_ERR_OK;
} /* end of rtl8380_miim_pollingEnable_get */

/* Function Name:
 *      rtl8380_miim_pollingEnable_set
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
rtl8380_miim_pollingEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    enable)
{
    uint32 val;
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, enable=%d", unit, port, enable);

    RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    if ((ret = reg_field_read(unit, MAPLE_SMI_POLL_CTRLr,
        MAPLE_SMI_POLL_MASK_27_0f, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }

    if (enable)
        val |= (1 << port);
    else
        val &= ~(1 << port);

    if ((ret = reg_field_write(unit, MAPLE_SMI_POLL_CTRLr,
        MAPLE_SMI_POLL_MASK_27_0f, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl8380_miim_pollingEnable_set */


/* Function Name:
 *      rtl8380_serdes_rst
 * Description:
 *      Reset Serdes and original patch are kept.
 * Input:
 *      unit    - unit id
 *      sds_num    - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
int32
rtl8380_serdes_rst(
    uint32  unit,
    uint32 sds_num)
{
    int32 ret;
    uint32 val;

    /*rx reset*/
    ioal_mem32_read(unit, 0xF3A4+sds_num*0x100, &val);
    val |= 1UL<<9;
    ioal_mem32_write(unit, 0xF3A4+sds_num*0x100, val);
    val &= ~(1UL<<9);
    ioal_mem32_write(unit, 0xF3A4+sds_num*0x100, val);

    /*cmu reset*/
    val = 0x4040;
    ioal_mem32_write(unit, 0xF380+sds_num*0x100, val);
    val = 0x4740;
    ioal_mem32_write(unit, 0xF380+sds_num*0x100, val);
    val = 0x47c0;
    ioal_mem32_write(unit, 0xF380+sds_num*0x100, val);
    val = 0x4000;
    ioal_mem32_write(unit, 0xF380+sds_num*0x100, val);

    /*software reset*/
    val = 0x7146;
    ioal_mem32_write(unit, 0xE78C+sds_num*0x200, val);
    val = 0x7106;
    ioal_mem32_write(unit, 0xE78C+sds_num*0x200, val);

    /*tx & rx reset*/
    val = 0x0c00;
    ioal_mem32_write(unit, 0xE780+sds_num*0x200, val);
    val = 0x0c03;
    ioal_mem32_write(unit, 0xE780+sds_num*0x200, val);

    ret = RT_ERR_OK;
    return ret;
}   /* end of rtl8380_serdes_rst */




/* RTL8380 mac driver service APIs */
rt_macdrv_t rtl8380_macdrv =
{
    .fMdrv_init                             = rtl8380_init,
    .fMdrv_miim_read                        = rtl8380_miim_read,
    .fMdrv_miim_write                       = rtl8380_miim_write,
    .fMdrv_miim_park_read                   = rtl8380_miim_park_read,
    .fMdrv_miim_park_write                  = rtl8380_miim_park_write,
    .fMdrv_miim_broadcast_write             = NULL,
    .fMdrv_miim_extParkPage_read            = NULL,
    .fMdrv_miim_extParkPage_write           = NULL,
    .fMdrv_miim_extParkPage_portmask_write  = NULL,
    .fMdrv_miim_mmd_read                    = rtl8380_miim_mmd_read,
    .fMdrv_miim_mmd_write                   = rtl8380_miim_mmd_write,
    .fMdrv_miim_mmd_portmask_write          = rtl8380_miim_mmd_portmask_write,
    .fMdrv_table_read                       = rtl8380_table_read,
    .fMdrv_table_write                      = rtl8380_table_write,
    .fMdrv_port_probe                       = rtl8380_port_probe,
    .fMdrv_miim_portmask_write              = rtl8380_miim_portmask_write,
    .fMdrv_miim_pollingEnable_get           = rtl8380_miim_pollingEnable_get,
    .fMdrv_miim_pollingEnable_set           = rtl8380_miim_pollingEnable_set,
    .fMdrv_mac_serdes_rst                   = rtl8380_serdes_rst,
    .fMdrv_mac_serdes_read                  = NULL,
    .fMdrv_mac_serdes_write                 = NULL,
    .fMdrv_smi_read                         = NULL,
    .fMdrv_smi_write                        = NULL,
    .fMdrv_miim_portSmiMdxProto_set         = NULL,
    .fMdrv_miim_portSmiMdxProto_get         = NULL,
}; /* end of rtl8380_macdrv */



