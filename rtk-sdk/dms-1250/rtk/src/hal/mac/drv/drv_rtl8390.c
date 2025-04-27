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
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/mac/drv/drv.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/mac/mac_probe.h>
#include <hal/mac/drv/drv_rtl8390.h>
#include <hal/mac/led/led_rtl8390.h>
#include <soc/type.h>
#include <osal/time.h>
#include <ioal/mem32.h>
#include <hwp/hw_profile.h>
#include <dal/dal_construct.h>
#include <dal/cypress/dal_cypress_construct.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */
#define RTL8390_PHYREG_ACCESS_CTRL_FAIL_MASK            (0x1 << 1)
#define SERDES_SET(_reg, _sb, _eb, _val) drv_rtl8390_serdes_set(unit, _reg, _sb, _eb, _val)

/*
 * Function Declaration
 */
int32 _rtl8390_miim_write(uint32 unit, rtk_port_t port, uint32 page, uint32 phy_reg, uint32 data);

/* Function Name:
 *      _rtl8390_miim_accessPortmask_set
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
_rtl8390_miim_accessPortmask_set(uint32 unit, rtk_portmask_t  *pPortmask)
{
    int32       ret;
    uint32      portIdx;
    uint32      val;


    val = pPortmask->bits[0];
    portIdx = 0;
    if ((ret = reg_array_write(unit, CYPRESS_PHYREG_PORT_CTRLr, portIdx, REG_ARRAY_INDEX_NONE, &val)) != RT_ERR_OK)
    {
        return ret;
    }

    val = pPortmask->bits[1];
    portIdx = 32;
    if ((ret = reg_array_write(unit, CYPRESS_PHYREG_PORT_CTRLr, portIdx, REG_ARRAY_INDEX_NONE, &val)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl8390_miim_phyRegAccessCtrlRegFields_set
 * Description:
 *      Set all fields of CYPRESS_PHYREG_ACCESS_CTRLr register
 * Input:
 *      unit        - unit id
 *      main_page   - value of CYPRESS_MAIN_PAGEf   - page number to access
 *      phy_reg     - value of CYPRESS_REGf         - register number to access
 *      park_page   - value of CYPRESS_PARK_PAGEf   - park page
 *      broadcast   - value of CYPRESS_BROADCASTf   - 0b0: normal; 0b1: broadcast
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
_rtl8390_miim_phyRegAccessCtrlRegFields_set(uint32   unit,
                                   uint32   main_page,
                                   uint32   phy_reg,
                                   uint32   park_page,
                                   uint32   broadcast,
                                   uint32   type,
                                   uint32   rwop,
                                   uint32   cmd,
                                   uint32   *pData)
{
    int32       ret;

    if ((ret = reg_field_set(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_REGf, &phy_reg, pData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select page number to access */
    if ((ret = reg_field_set(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_MAIN_PAGEf, &main_page, pData)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_set(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_PARK_PAGEf, &park_page, pData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    if ((ret = reg_field_set(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_TYPEf, &type, pData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    if ((ret = reg_field_set(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_RWOPf, &rwop, pData)) != RT_ERR_OK)
    {
        return ret;
    }
    /* Broadcast operation
     * 0b0: normal
     * 0b1: broadcast
     */
    if ((ret = reg_field_set(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_BROADCASTf, &broadcast, pData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    if ((ret = reg_field_set(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_CMDf, &cmd, pData)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8390_miim_pollingReg_init
 * Description:
 *      Initialize the MAC Polling PHY registers.
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
static int32
rtl8390_miim_pollingReg_init(uint32 unit)
{
    int32       ret = RT_ERR_FAILED;
    int32       port;
    uint32      reg_addr;
    uint32      enable;

    /*Fill MAC polling PHY regs to Zero*/
    if ((ret = reg_idx2Addr_get(unit, CYPRESS_SMI_PORT_POLLING_CTRLr, &reg_addr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "Get MAC Polling PHY Regs address fail");
        return ret;
    }
    ioal_mem32_write(unit, reg_addr, 0);
    ioal_mem32_write(unit, (reg_addr+4), 0);

    /*Enable MAC polling PHY for existed port only*/
    enable = 1;
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        /* 96M 10G port was designed to polling internal SerDes */
        if (HWP_10GE_PORT(unit, port))
        {
            continue;
        }

        if ((ret = reg_array_field_write(unit, CYPRESS_SMI_PORT_POLLING_CTRLr, port, REG_ARRAY_INDEX_NONE,
                                         CYPRESS_SMI_POLLING_PMSKf, &enable)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HAL), "Configure MAC Polling PHY Regs fail");
            return ret;
        }
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      rtl8390_port_probe
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
rtl8390_port_probe(uint32 unit)
{
    return RT_ERR_OK;
} /* end of rtl8390_port_probe */


/* Function Name:
 *      rtl8390_regDefault_init
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
rtl8390_regDefault_init(uint32 unit)
{
    int32 port;
    uint32  val;
    rtk_portmask_t      portmask;

    /* enable special congest and set congest timer to 2 sec */
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        val = ((0x2<<0) | (0x2<<16));
        reg_array_write(unit, CYPRESS_SC_P_ENr, REG_ARRAY_INDEX_NONE, port, &val);
    }


    /* clear LED setting */
    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
    reg_write(unit, CYPRESS_LED_COPR_PMASK_CTRLr, portmask.bits);


    val = 0;
    /* Disable 48 pass 1,
       so high speed to low speed Tx experiment in half mode won't drop packet by this function */
    reg_field_write(unit, CYPRESS_MAC_GLB_CTRLr, CYPRESS_MAC_DROP_48PASS1_ENf, &val);

    reg_field_write(unit, CYPRESS_MAC_GLB_CTRLr, CYPRESS_HALF_48PASS1_ENf, &val);

    /* half duplex backpressure collision method */
    reg_field_write(unit, CYPRESS_MAC_GLB_CTRLr, CYPRESS_BKPRES_MTHD_SELf, &val);

    if (HWP_CHIP_ID(unit) == RTL8396M_CHIP_ID)
    {

        /* Table read for egress bandwidth of 10G port 24 to be 10Gbps */
        val = 0x218;
        reg_write(unit, CYPRESS_TBL_ACCESS_CTRL_2r, &val);

        /* Data correct to two registers */
        reg_array_read(unit, CYPRESS_TBL_ACCESS_DATA_2r, REG_ARRAY_INDEX_NONE, 7, &val);
        val &= ~(0xFF);
        val |= 0x98;
        reg_array_write(unit, CYPRESS_TBL_ACCESS_DATA_2r, REG_ARRAY_INDEX_NONE, 7, &val);

        reg_array_read(unit, CYPRESS_TBL_ACCESS_DATA_2r, REG_ARRAY_INDEX_NONE, 8, &val);
        val &= ~(0xFFF << 20);
        val |= (0x968 << 20);
        reg_array_write(unit, CYPRESS_TBL_ACCESS_DATA_2r, REG_ARRAY_INDEX_NONE, 8, &val);

        reg_read(unit, CYPRESS_SCHED_LB_TICK_TKN_CTRLr, &val);
        val &= ~(0xFFFF << 16);
        val |= (0x52a << 16);
        reg_write(unit, CYPRESS_SCHED_LB_TICK_TKN_CTRLr, &val);

        /* Table write */
        val = 0x318;
        reg_write(unit, CYPRESS_TBL_ACCESS_CTRL_2r, &val);

        /* Table read for egress bandwidth of 10G port 36 to be 10Gbps */
        val = 0x224;
        reg_write(unit, CYPRESS_TBL_ACCESS_CTRL_2r, &val);

        /* Data correct to two registers */
        reg_array_read(unit, CYPRESS_TBL_ACCESS_DATA_2r, REG_ARRAY_INDEX_NONE, 7, &val);
        val &= ~(0xFF);
        val |= 0x98;
        reg_array_write(unit, CYPRESS_TBL_ACCESS_DATA_2r, REG_ARRAY_INDEX_NONE, 7, &val);

        reg_array_read(unit, CYPRESS_TBL_ACCESS_DATA_2r, REG_ARRAY_INDEX_NONE, 8, &val);
        val &= ~(0xFFF << 20);
        val |= (0x968 << 20);
        reg_array_write(unit, CYPRESS_TBL_ACCESS_DATA_2r, REG_ARRAY_INDEX_NONE, 8, &val);

        reg_read(unit, CYPRESS_SCHED_LB_TICK_TKN_CTRLr, &val);
        val &= ~(0xFFFF << 16);
        val |= (0x52a << 16);
        reg_write(unit, CYPRESS_SCHED_LB_TICK_TKN_CTRLr, &val);

        /* Table write */
        val = 0x324;
        reg_write(unit, CYPRESS_TBL_ACCESS_CTRL_2r, &val);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8390_init
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
rtl8390_init(uint32 unit)
{
    int32       ret = RT_ERR_FAILED;


    rtl8390_regDefault_init(unit);
    rtl8390_smiAddr_init(unit);
    {
        uint8 ledModeInitSkip = LEDMODEINITSKIP_NO;
#if !defined(__BOOTLOADER__)
        ioal_param_ledInitFlag_get(&ledModeInitSkip);
#endif
        if (LEDMODEINITSKIP_NO == ledModeInitSkip)
        {
            if ((ret = rtl8390_led_config(unit)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_INIT), "led config failed");
            }
        }
    }

    if ((ret = rtl8390_miim_pollingReg_init(unit)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "Init MAC Polling PHY Register fail");
        return ret;
    }

    dal_cypress_construct_phy_reset(unit);

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
} /* end of rtl8390_init */


/* Function Name:
 *      rtl8390_miim_read
 * Description:
 *      Get PHY registers from rtl8390 family chips.
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
rtl8390_miim_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      *pData)
{
    uint32 temp;
    uint32 val;
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
            _rtl8390_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, page);
            page = HAL_MIIM_PAGE_ID_MAX(unit);
        }
    }


    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * INDATA[5:0] is the PHY address WHEN RWOP = 0b0
     */
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_INDATAf, &port)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    val = 0x1ff;
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_CTRLr, CYPRESS_EXT_PAGEf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl8390_miim_phyRegAccessCtrlRegFields_set(unit,
                                   page,        /* MAIN_PAGE - page number to access */
                                   phy_reg,     /* REG       - register number to access */
                                   0,           /* PARK_PAGE - park page */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   0,           /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                                   0,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, CYPRESS_PHYREG_ACCESS_CTRLr, 0x1, RTL8390_PHYREG_ACCESS_CTRL_FAIL_MASK);

    /* get the read operation result to pData */
    if ((ret = reg_field_read(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_DATAf, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    PHY_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_HAL), "pData=0x%x", *pData);

    return RT_ERR_OK;
} /* end of rtl8390_miim_read */


/* Function Name:
 *      rtl8390_miim_write
 * Description:
 *      Set PHY registers in rtl8390 family chips.
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
_rtl8390_miim_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    rtk_portmask_t  portmask;
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;


    /* Select PHY to access */
    RTK_PORTMASK_RESET(portmask);
    RTK_PORTMASK_PORT_SET(portmask, port);
    if ((ret = _rtl8390_miim_accessPortmask_set(unit, &portmask)) != RT_ERR_OK)
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
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_INDATAf, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    val = 0x1ff;
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_CTRLr, CYPRESS_EXT_PAGEf, &val)) != RT_ERR_OK)
    {
        return ret;
    }

    ret = _rtl8390_miim_phyRegAccessCtrlRegFields_set(unit,
                                   page,        /* MAIN_PAGE - page number to access */
                                   phy_reg,     /* REG       - register number to access */
                                   0,           /* PARK_PAGE - park page */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   0,           /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                                   1,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, CYPRESS_PHYREG_ACCESS_CTRLr, 0x1, RTL8390_PHYREG_ACCESS_CTRL_FAIL_MASK);

    return RT_ERR_OK;
} /* end of rtl8390_miim_write */


/* Function Name:
 *      rtl8390_miim_write
 * Description:
 *      Set PHY registers in rtl8390 family chips.
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
rtl8390_miim_write(
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
            _rtl8390_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, page);
            page = HAL_MIIM_PAGE_ID_MAX(unit);
        }
    }

    ret = _rtl8390_miim_write(unit, port, page, phy_reg, data);
    PHY_SEM_UNLOCK(unit);

    return ret;
} /* end of rtl8390_miim_write */

/* Function Name:
 *      rtl8390_miim_park_read
 * Description:
 *      Get PHY registers from rtl8390 family chips.
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl8390_miim_park_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData)
{
    uint32 temp;
    uint32 val;
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, page=0x%x, phy_reg=0x%x", unit, port, page, phy_reg);

    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((parkPage > HAL_MIIM_PAGE_ID_MAX(unit)), RT_ERR_PHY_PAGE_ID);
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
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_INDATAf, &port)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    val = 0x1ff;
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_CTRLr, CYPRESS_EXT_PAGEf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl8390_miim_phyRegAccessCtrlRegFields_set(unit,
                                   page,        /* MAIN_PAGE - page number to access */
                                   phy_reg,     /* REG       - register number to access */
                                   parkPage,    /* PARK_PAGE - park page */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   0,           /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                                   0,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, CYPRESS_PHYREG_ACCESS_CTRLr, 0x1, 0);

    /* get the read operation result to pData */
    if ((ret = reg_field_read(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_DATAf, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    PHY_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_HAL), "pData=0x%x", *pData);

    return RT_ERR_OK;
} /* end of rtl8390_miim_park_read */


/* Function Name:
 *      rtl8390_miim_park_write
 * Description:
 *      Set PHY registers in rtl8390 family chips.
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl8390_miim_park_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    rtk_portmask_t  portmask;
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, phy_reg=0x%x, data=0x%x", unit, port, page, phy_reg, data);
    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((parkPage > HAL_MIIM_PAGE_ID_MAX(unit)), RT_ERR_PHY_PAGE_ID);
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
    RTK_PORTMASK_RESET(portmask);
    RTK_PORTMASK_PORT_SET(portmask, port);
    if ((ret = _rtl8390_miim_accessPortmask_set(unit, &portmask)) != RT_ERR_OK)
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
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_INDATAf, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    val = 0x1ff;
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_CTRLr, CYPRESS_EXT_PAGEf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl8390_miim_phyRegAccessCtrlRegFields_set(unit,
                                   page,        /* MAIN_PAGE - page number to access */
                                   phy_reg,     /* REG       - register number to access */
                                   parkPage,    /* PARK_PAGE - park page */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   0,           /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                                   1,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, CYPRESS_PHYREG_ACCESS_CTRLr, 0x1, 0);

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of rtl8390_miim_park_write */

/* Function Name:
 *      rtl8390_miim_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl8390 family chips.
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
rtl8390_miim_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          page,
    uint32          phy_reg,
    uint32          data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, portmask0=0x%x, portmask1=0x%x, page=0x%x, phy_reg=0x%x, data=0x%x",
        unit,
        RTK_PORTMASK_WORD_GET(portmask, 0),
        RTK_PORTMASK_WORD_GET(portmask, 1),
        page, phy_reg, data);

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
    if ((ret = _rtl8390_miim_accessPortmask_set(unit, &portmask)) != RT_ERR_OK)
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
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_INDATAf, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    val = 0x1ff;
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_CTRLr, CYPRESS_EXT_PAGEf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl8390_miim_phyRegAccessCtrlRegFields_set(unit,
                                   page,        /* MAIN_PAGE - page number to access */
                                   phy_reg,     /* REG       - register number to access */
                                   0,           /* PARK_PAGE - park page */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   0,           /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                                   1,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, CYPRESS_PHYREG_ACCESS_CTRLr, 0x1, 0);

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of rtl8390_miim_portmask_write */

/* Function Name:
 *      rtl8390_miim_broadcast_write
 * Description:
 *      Set PHY registers in rtl8390 family chips with broadcast mechanism.
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
rtl8390_miim_broadcast_write(
    uint32      unit,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, reg=0x%x \
           data=0x%x", unit, page, phy_reg, data);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX(unit)), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    PHY_SEM_LOCK(unit);

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[5:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_INDATAf, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    val = 0x1ff;
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_CTRLr, CYPRESS_EXT_PAGEf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl8390_miim_phyRegAccessCtrlRegFields_set(unit,
                                   page,        /* MAIN_PAGE - page number to access */
                                   phy_reg,     /* REG       - register number to access */
                                   0,           /* PARK_PAGE - park page */
                                   1,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   0,           /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                                   1,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, CYPRESS_PHYREG_ACCESS_CTRLr, 0x1, 0);

    /* check the FAIL bit */
    if ((ret = reg_field_read(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_FAILf, &val)) != RT_ERR_OK)
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
} /* end of rtl8390_miim_write */


/* Function Name:
 *      rtl8390_miim_read
 * Description:
 *      Get PHY registers from rtl8390 family chips.
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl8390_miim_extParkPage_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData)
{
    uint32 temp;
    uint32 val;
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, mainPage=0x%x, extPage=0x%x, parkPage=0x%x, reg=0x%x",
           unit, port, mainPage, extPage, parkPage, phy_reg);

    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    //RT_PARAM_CHK((mainPage > HAL_MIIM_PAGE_ID_MAX(unit)), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    PHY_SEM_LOCK(unit);

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * INDATA[5:0] is the PHY address WHEN RWOP = 0b0
     */
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_INDATAf, &port)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_CTRLr, CYPRESS_EXT_PAGEf, &extPage)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl8390_miim_phyRegAccessCtrlRegFields_set(unit,
                                   mainPage,    /* MAIN_PAGE - page number to access */
                                   phy_reg,     /* REG       - register number to access */
                                   parkPage,    /* PARK_PAGE - park page */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   0,           /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                                   0,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, CYPRESS_PHYREG_ACCESS_CTRLr, 0x1, 0);

    /* get the read operation result to pData */
    if ((ret = reg_field_read(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_DATAf, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* check the FAIL bit */
    if ((ret = reg_field_read(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_FAILf, &val)) != RT_ERR_OK)
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
} /* end of rtl8390_miim_read */


/* Function Name:
 *      rtl8390_miim_write
 * Description:
 *      Set PHY registers in rtl8390 family chips.
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
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl8390_miim_extParkPage_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    rtk_portmask_t  portmask;
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, mainPage=0x%x, extPage=0x%x, parkPage=0x%x, reg=0x%x \
           data=0x%x", unit, port, mainPage, extPage, parkPage, phy_reg, data);
    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    //RT_PARAM_CHK((page >= PHY_PAGE_MAX), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    PHY_SEM_LOCK(unit);

    /* Select PHY to access */
    RTK_PORTMASK_RESET(portmask);
    RTK_PORTMASK_PORT_SET(portmask, port);
    if ((ret = _rtl8390_miim_accessPortmask_set(unit, &portmask)) != RT_ERR_OK)
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
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_INDATAf, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_CTRLr, CYPRESS_EXT_PAGEf, &extPage)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl8390_miim_phyRegAccessCtrlRegFields_set(unit,
                                   mainPage,    /* MAIN_PAGE - page number to access */
                                   phy_reg,     /* REG       - register number to access */
                                   parkPage,    /* PARK_PAGE - park page */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   0,           /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                                   1,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, CYPRESS_PHYREG_ACCESS_CTRLr, 0x1, 0);

    /* check the FAIL bit */
    if ((ret = reg_field_read(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_FAILf, &val)) != RT_ERR_OK)
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
} /* end of rtl8390_miim_write */

/* Function Name:
 *      rtl8390_miim_extParkPage_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl8390 family chips.
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
rtl8390_miim_extParkPage_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mainPage,
    uint32          extPage,
    uint32          parkPage,
    uint32          phy_reg,
    uint32          data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, portmask0=0x%x, portmask1=0x%x, mainPage=0x%x, extPage=0x%x, parkPage=0x%x, reg=0x%x \
           data=0x%x", unit,
           RTK_PORTMASK_WORD_GET(portmask, 0),
           RTK_PORTMASK_WORD_GET(portmask, 1),
           mainPage, extPage, parkPage, phy_reg, data);
    //RT_PARAM_CHK((page >= PHY_PAGE_MAX), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    PHY_SEM_LOCK(unit);

    /* Select PHY to access */
    if ((ret = _rtl8390_miim_accessPortmask_set(unit, &portmask)) != RT_ERR_OK)
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
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_INDATAf, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_CTRLr, CYPRESS_EXT_PAGEf, &extPage)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl8390_miim_phyRegAccessCtrlRegFields_set(unit,
                                   mainPage,    /* MAIN_PAGE - page number to access */
                                   phy_reg,     /* REG       - register number to access */
                                   parkPage,    /* PARK_PAGE - park page */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   0,           /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                                   1,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, CYPRESS_PHYREG_ACCESS_CTRLr, 0x1, 0);

    /* check the FAIL bit */
    if ((ret = reg_field_read(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_FAILf, &val)) != RT_ERR_OK)
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
} /* end of rtl8390_miim_portmask_write */

/* Function Name:
 *      rtl8390_miim_mmd_read
 * Description:
 *      Get PHY registers from rtl8390 family chips.
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
rtl8390_miim_mmd_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      *pData)
{
    uint32 temp;
    uint32 val;
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, mmdAddr=0x%x, mmdReg=0x%x, reg=0x%x",
           unit, port, mmdAddr, mmdReg);

    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    //RT_PARAM_CHK((mainPage > HAL_MIIM_PAGE_ID_MAX(unit)), RT_ERR_PHY_PAGE_ID);
    //RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    PHY_SEM_LOCK(unit);

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * INDATA[5:0] is the PHY address WHEN RWOP = 0b0
     */
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_INDATAf, &port)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd device address to access */
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_MMD_CTRLr, CYPRESS_MMD_DEVADf, &mmdAddr)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd register to access */
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_MMD_CTRLr, CYPRESS_MMD_REGf, &mmdReg)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl8390_miim_phyRegAccessCtrlRegFields_set(unit,
                                   0,           /* MAIN_PAGE - page number to access */
                                   0,           /* REG       - register number to access */
                                   0,           /* PARK_PAGE - park page */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   1,           /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                                   0,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, CYPRESS_PHYREG_ACCESS_CTRLr, 0x1, RTL8390_PHYREG_ACCESS_CTRL_FAIL_MASK);

    /* get the read operation result to pData */
    if ((ret = reg_field_read(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_DATAf, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* check the FAIL bit */
    if ((ret = reg_field_read(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_FAILf, &val)) != RT_ERR_OK)
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
} /* end of rtl8390_miim_mmd_read */


/* Function Name:
 *      rtl8390_miim_mmd_write
 * Description:
 *      Set PHY registers in rtl8390 family chips.
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
rtl8390_miim_mmd_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      data)
{
    rtk_portmask_t  portmask;
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, mmdAddr=0x%x, mmdReg=0x%x \
           data=0x%x", unit, port, mmdAddr, mmdReg, data);
    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    //RT_PARAM_CHK((page >= PHY_PAGE_MAX), RT_ERR_PHY_PAGE_ID);
    //RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    PHY_SEM_LOCK(unit);

    /* Select PHY to access */
    RTK_PORTMASK_RESET(portmask);
    RTK_PORTMASK_PORT_SET(portmask, port);
    if ((ret = _rtl8390_miim_accessPortmask_set(unit, &portmask)) != RT_ERR_OK)
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
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_INDATAf, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd device address to access */
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_MMD_CTRLr, CYPRESS_MMD_DEVADf, &mmdAddr)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd register to access */
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_MMD_CTRLr, CYPRESS_MMD_REGf, &mmdReg)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl8390_miim_phyRegAccessCtrlRegFields_set(unit,
                                   0,           /* MAIN_PAGE - page number to access */
                                   0,           /* REG       - register number to access */
                                   0,           /* PARK_PAGE - park page */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   1,           /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                                   1,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, CYPRESS_PHYREG_ACCESS_CTRLr, 0x1, RTL8390_PHYREG_ACCESS_CTRL_FAIL_MASK);

    /* check the FAIL bit */
    if ((ret = reg_field_read(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_FAILf, &val)) != RT_ERR_OK)
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
} /* end of rtl8390_miim_mmd_write */

/* Function Name:
 *      rtl8390_miim_mmd_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl8390 family chips.
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
rtl8390_miim_mmd_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mmdAddr,
    uint32          mmdReg,
    uint32          data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, portmask0=0x%x, portmask1=0x%x, mmdAddr=0x%x, mmdReg=0x%x \
           data=0x%x", unit,
           RTK_PORTMASK_WORD_GET(portmask, 0),
           RTK_PORTMASK_WORD_GET(portmask, 1),
           mmdAddr, mmdReg, data);
    //RT_PARAM_CHK((page >= PHY_PAGE_MAX), RT_ERR_PHY_PAGE_ID);
    //RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    PHY_SEM_LOCK(unit);

    /* Select PHY to access */
    if ((ret = _rtl8390_miim_accessPortmask_set(unit, &portmask)) != RT_ERR_OK)
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
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_DATA_CTRLr, CYPRESS_INDATAf, &data)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd device address to access */
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_MMD_CTRLr, CYPRESS_MMD_DEVADf, &mmdAddr)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd register to access */
    if ((ret = reg_field_write(unit, CYPRESS_PHYREG_MMD_CTRLr, CYPRESS_MMD_REGf, &mmdReg)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _rtl8390_miim_phyRegAccessCtrlRegFields_set(unit,
                                   0,           /* MAIN_PAGE - page number to access */
                                   0,           /* REG       - register number to access */
                                   0,           /* PARK_PAGE - park page */
                                   0,           /* BROADCAST - 0b0: normal; 0b1: broadcast */
                                   1,           /* TYPE      - 0b0: Normal register; 0b1: MMD register */
                                   1,           /* RWOP      - 0b0: read; 0b1: write */
                                   1,           /* CMD       - 0b0: complete access; 0b1: execute access */
                                   &temp);
    if (ret != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, CYPRESS_PHYREG_ACCESS_CTRLr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, CYPRESS_PHYREG_ACCESS_CTRLr, 0x1, 0);

    /* check the FAIL bit */
    if ((ret = reg_field_read(unit, CYPRESS_PHYREG_ACCESS_CTRLr, CYPRESS_FAILf, &val)) != RT_ERR_OK)
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
} /* end of rtl8390_miim_mmd_portmask_write */

/* Function Name:
 *      rtl8390_table_read
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
rtl8390_table_read(
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
    rtk_cypress_reg_list_t ctrlReg[] = {
        CYPRESS_TBL_ACCESS_L2_CTRLr,
        CYPRESS_TBL_ACCESS_CTRL_0r,
        CYPRESS_TBL_ACCESS_CTRL_1r,
        CYPRESS_TBL_ACCESS_CTRL_2r };
    rtk_cypress_reg_list_t dataReg[] = {
        CYPRESS_TBL_ACCESS_L2_DATAr,
        CYPRESS_TBL_ACCESS_DATA_0r,
        CYPRESS_TBL_ACCESS_DATA_1r,
        CYPRESS_TBL_ACCESS_DATA_2r };
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
    case CYPRESS_L2_UCt:
    case CYPRESS_L2_MCt:
    case CYPRESS_L2_IP_MC_SIPt:
    case CYPRESS_L2_IP_MCt:
    case CYPRESS_L2_CAM_UCt:
    case CYPRESS_L2_CAM_MCt:
    case CYPRESS_L2_CAM_IP_MC_SIPt:
    case CYPRESS_L2_CAM_IP_MCt:
    case CYPRESS_MC_PMSKt:
    case CYPRESS_L2_NEXT_HOPt:
    case CYPRESS_L2_NH_LEGACYt:
        groupId = 0;
        break;

    case CYPRESS_VLANt:
    case CYPRESS_VLAN_IGR_CNVTt:
    case CYPRESS_VLAN_IP_SUBNET_BASEDt:
    case CYPRESS_VLAN_MAC_BASEDt:
    case CYPRESS_IACLt:
    case CYPRESS_EACLt:
    case CYPRESS_METERt:
    case CYPRESS_LOGt:
    case CYPRESS_MSTIt:
        groupId = 1;
        break;

    case CYPRESS_UNTAGt:
    case CYPRESS_VLAN_EGR_CNVTt:
    case CYPRESS_ROUTINGt:
    case CYPRESS_MPLS_LIBt:
        groupId = 2;
        break;

    case CYPRESS_SCHEDt:
    case CYPRESS_SPG_PORTt:
    case CYPRESS_OUT_Qt:
        groupId = 3;
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
    if ((ret = reg_field_set(unit, ctrlReg[groupId], CYPRESS_EXECf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Table access operation
     * 0b0: read
     * 0b1: write
     */
    reg_value = 0;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], CYPRESS_CMDf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* access table type */
    reg_value = pTable->type;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], CYPRESS_TBLf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Select access address of the table */
    reg_value = addr;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], CYPRESS_ADDRf, &reg_value, &reg_data)) != RT_ERR_OK)
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
        if ((ret = reg_field_read(unit, ctrlReg[groupId], CYPRESS_EXECf, &busy)) != RT_ERR_OK)
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
} /* end of rtl8390_table_read */


/* Function Name:
 *      rtl8390_table_write
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
rtl8390_table_write(
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
    rtk_cypress_reg_list_t ctrlReg[] = {
        CYPRESS_TBL_ACCESS_L2_CTRLr,
        CYPRESS_TBL_ACCESS_CTRL_0r,
        CYPRESS_TBL_ACCESS_CTRL_1r,
        CYPRESS_TBL_ACCESS_CTRL_2r };
    rtk_cypress_reg_list_t dataReg[] = {
        CYPRESS_TBL_ACCESS_L2_DATAr,
        CYPRESS_TBL_ACCESS_DATA_0r,
        CYPRESS_TBL_ACCESS_DATA_1r,
        CYPRESS_TBL_ACCESS_DATA_2r };
    uint32      index;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, table=%d, addr=0x%x", unit, table, addr);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(unit, table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);

    if (CYPRESS_LOGt == table)
    {
        RT_PARAM_CHK(((addr & 0xFFFFF3FF) >= pTable->size), RT_ERR_OUT_OF_RANGE);
    }
    else
    {
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);
    }

    switch (table)
    {
    case CYPRESS_L2_UCt:
    case CYPRESS_L2_MCt:
    case CYPRESS_L2_IP_MC_SIPt:
    case CYPRESS_L2_IP_MCt:
    case CYPRESS_L2_CAM_UCt:
    case CYPRESS_L2_CAM_MCt:
    case CYPRESS_L2_CAM_IP_MC_SIPt:
    case CYPRESS_L2_CAM_IP_MCt:
    case CYPRESS_MC_PMSKt:
    case CYPRESS_L2_NEXT_HOPt:
    case CYPRESS_L2_NH_LEGACYt:
        groupId = 0;
        break;

    case CYPRESS_VLANt:
    case CYPRESS_VLAN_IGR_CNVTt:
    case CYPRESS_VLAN_IP_SUBNET_BASEDt:
    case CYPRESS_VLAN_MAC_BASEDt:
    case CYPRESS_IACLt:
    case CYPRESS_EACLt:
    case CYPRESS_METERt:
    case CYPRESS_LOGt:
    case CYPRESS_MSTIt:
        groupId = 1;
        break;

    case CYPRESS_UNTAGt:
    case CYPRESS_VLAN_EGR_CNVTt:
    case CYPRESS_ROUTINGt:
    case CYPRESS_MPLS_LIBt:
        groupId = 2;
        break;

    case CYPRESS_SCHEDt:
    case CYPRESS_SPG_PORTt:
    case CYPRESS_OUT_Qt:
        groupId = 3;
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
    if ((ret = reg_field_set(unit, ctrlReg[groupId], CYPRESS_EXECf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Table access operation
     * 0b0: read
     * 0b1: write
     */
    reg_value = 1;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], CYPRESS_CMDf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* access table type */
    reg_value = pTable->type;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], CYPRESS_TBLf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Select access address of the table */
    reg_value = addr;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], CYPRESS_ADDRf, &reg_value, &reg_data)) != RT_ERR_OK)
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
        if ((ret = reg_field_read(unit, ctrlReg[groupId], CYPRESS_EXECf, &busy)) != RT_ERR_OK)
        {
            MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
            return ret;
        }
    } while (busy);
#endif

    MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);

    return RT_ERR_OK;
} /* end of rtl8390_table_write */

/* Function Name:
 *      rtl8390_miim_pollingEnable_get
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
rtl8390_miim_pollingEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    *pEnabled)
{
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d", unit, port);

    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnabled), RT_ERR_NULL_POINTER);

    if ((ret = rtl8390_miim_globalPollingEnable_get(unit, pEnabled)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl8390_miim_pollingEnable_get */

/* Function Name:
 *      rtl8390_miim_pollingEnable_set
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
rtl8390_miim_pollingEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    enabled)
{
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, enabled=%d", unit, port, enabled);

    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enabled != DISABLED && enabled != ENABLED), RT_ERR_INPUT);

    if ((ret = rtl8390_miim_globalPollingEnable_set(unit, enabled)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl8390_miim_pollingEnable_set */

/* Function Name:
 *      rtl8390_miim_globalPollingEnable_get
 * Description:
 *      Get the global mac polling PHY status.
 * Input:
 *      unit     - unit id
 * Output:
 *      pEnabled - pointer buffer of mac polling PHY status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
rtl8390_miim_globalPollingEnable_get(
    uint32          unit,
    rtk_enable_t    *pEnabled)
{
    uint32 val;
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d", unit);

    RT_PARAM_CHK((NULL == pEnabled), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(unit, CYPRESS_SMI_GLB_CTRLr, CYPRESS_MDX_POLLING_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }

    if (val == 1)
        (*pEnabled) = ENABLED;
    else
        (*pEnabled) = DISABLED;

    return RT_ERR_OK;
} /* end of rtl8390_miim_globalPollingEnable_get */

/* Function Name:
 *      rtl8390_miim_globalPollingEnable_set
 * Description:
 *      Set the global mac polling PHY status.
 * Input:
 *      unit    - unit id
 *      enabled - global mac polling PHY status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
rtl8390_miim_globalPollingEnable_set(
    uint32          unit,
    rtk_enable_t    enabled)
{
    uint32 val;
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, enabled=%d", unit, enabled);

    RT_PARAM_CHK((enabled != DISABLED && enabled != ENABLED), RT_ERR_INPUT);

    if (enabled)
        val = 1;
    else
        val = 0;

    if ((ret = reg_field_write(unit, CYPRESS_SMI_GLB_CTRLr, CYPRESS_MDX_POLLING_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl8390_miim_globalPollingEnable_set */

/* Function Name:
 *      _rtl8390_smiAddr_chk
 * Description:
 *      Check whether the smi address of each port is valid.
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
int32 _rtl8390_smiAddr_chk(uint32 unit)
{
    uint32 mac_id;
    uint32 offSet = 0;
    uint32 smiMax = 0;
    uint32 phyAddrMin = HWP_NONE, macWithPhyMin = HWP_NONE;


    HWP_PORT_TRAVS_EXCEPT_CPU(unit, mac_id)
    {
        /*check whether the phy address is valid of each port.*/
        if (((mac_id <= 23) && (0 != HWP_PORT_SMI(unit, mac_id)))
            || ((HWP_CHIP_ID(unit) != RTL8396M_CHIP_ID) && (mac_id > 23) && (1 != HWP_PORT_SMI(unit, mac_id)))
            || ((HWP_PORT_PHY_IDX(unit, mac_id) == HWP_NONE) && (HWP_NONE != HWP_PORT_SMI(unit, mac_id)) && (HWP_NONE != HWP_PHY_ADDR(unit, mac_id))))
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SWITCH), "smi of mac_id[%d] is invalid.\n", mac_id);
            return RT_ERR_FAILED;
        }
        if((HWP_PORT_SMI(unit, mac_id) != HWP_NONE) && (HWP_PORT_SMI(unit, mac_id) > smiMax))
        {
            smiMax = HWP_PORT_SMI(unit, mac_id);
        }
        if(HWP_PHY_ADDR(unit, mac_id) < phyAddrMin)
        {
            phyAddrMin = HWP_PHY_ADDR(unit, mac_id);
            macWithPhyMin = mac_id;
        }
    }

    offSet = phyAddrMin - macWithPhyMin;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, mac_id)
    {
        if(HWP_PHY_ADDR(unit, mac_id) == HWP_NONE)
            break;
        /*check the min phy_addr and offset between phy_addr and mac_id are correct */
        if(((HWP_PHY_ADDR(unit, mac_id) - mac_id) * (HWP_PHY_ADDR(unit, mac_id) - mac_id) != offSet * offSet)
            && (smiMax != 0))
        {
            offSet = mac_id - HWP_PHY_ADDR(unit, mac_id);
            smiMax--;
            if(phyAddrMin != HWP_PHY_ADDR(unit, mac_id))
                return RT_ERR_FAILED;
        }
        if (((HWP_PHY_ADDR(unit, mac_id) - mac_id) * (HWP_PHY_ADDR(unit, mac_id) - mac_id) != offSet * offSet))
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8390_smiAddr_init
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
int32 rtl8390_smiAddr_init(uint32 unit)
{
    uint32 mac_id;
    uint32 offSet = 0;
    uint32 ret;
    uint32 phyAddrMin = HWP_NONE, macWithPhyMin = HWP_NONE;

    if ((ret = _rtl8390_smiAddr_chk(unit)) != RT_ERR_OK)
    {
        //osal_printf("SMI interface and phy_addr check failed!\n");
        return ret;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, mac_id)
    {
        if(HWP_PHY_ADDR(unit, mac_id) < phyAddrMin)
        {
            phyAddrMin = HWP_PHY_ADDR(unit, mac_id);
            macWithPhyMin = mac_id;
            break;
        }
    }
    /* simply return if there is no PHY in the system */
    if (phyAddrMin == HWP_NONE)
    {
        return RT_ERR_OK;
    }

    offSet = phyAddrMin - macWithPhyMin;
    reg_field_write(unit, CYPRESS_SMI_GLB_CTRLr, CYPRESS_SMI_POLLING_START_ADDRf, &offSet);

    return RT_ERR_OK;
} /* end of rtl8390_smiAddr_init */


/* Function Name:
 *      rtl8390_miim_portSmiMdxProto_get
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
rtl8390_miim_portSmiMdxProto_get(uint32 unit, rtk_port_t port, drv_smi_mdxProtoSel_t *pProto)
{
    int32   ret;
    uint32      smi_id;

    smi_id = HWP_PORT_SMI(unit, port);
    switch (smi_id)
    {
      case 0:
      case 1:
        *pProto = DRV_SMI_MDX_PROTO_C22;
        break;
      case 2:
        *pProto = DRV_SMI_MDX_PROTO_C45;
        break;
      default:
        if ((port == 25) ||  (port == 26))
        {
            *pProto = DRV_SMI_MDX_PROTO_C22;
        }
        else
        {
            ret = RT_ERR_INPUT;
            RT_ERR(ret, (MOD_HAL), "unit %u smi_id %u", unit, smi_id);
            return ret;
        }
    }
    return RT_ERR_OK;
}

void drv_rtl8390_serdes_set(uint32 unit, uint32 reg, uint32 endBit,
    uint32 startBit, uint32 val)
{
    uint32  configVal, len, mask;
    uint32  i;

    len = endBit - startBit + 1;

    if (32 == len)
        configVal = val;
    else
    {
        mask = 0;
        for (i = startBit; i <= endBit; ++i)
            mask |= (1 << i);

        ioal_mem32_read(unit, reg, &configVal);
        configVal &= ~(mask);
        configVal |= (val << startBit);
    }
    //RT_DBG(LOG_EVENT, (MOD_DAL|MOD_PORT), "reg 0x%x val 0x%x",reg, configVal);
    ioal_mem32_write(unit, reg, configVal);

    return;
}   /* end of drv_rtl8390_serdes_set */

/* Function Name:
 *      drv_rtl8390_sds_rx_rst
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
drv_rtl8390_sds_rx_rst(uint32  unit, uint32 sds_num)
{
    uint32  sdsReg[] = {0xA328, 0xA728, 0xAB28, 0xAF28, 0xB320, 0xB728, 0xBB20};
    uint32  addr_ofst = 0x400;
    uint32  ofst, sdsAddr;

    if (sds_num >= RTK_MAX_NUM_OF_SERDES)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SWITCH), "Serdes[%d] doesn't exist\n", sds_num);
        return RT_ERR_OUT_OF_RANGE;
    }

    ofst = addr_ofst * (sds_num / 2);
    sdsAddr = sdsReg[sds_num/2] + (0x80 * (sds_num % 2));

    if (sds_num < 8 || sds_num == 10 || sds_num == 11) {
        SERDES_SET(0xa3c0 + ofst,  31 , 16 , 0x0050);
        SERDES_SET(0xa3c0 + ofst,  31 , 16 , 0x00f0);
        SERDES_SET(0xa3c0 + ofst,  31 , 16 , 0x0);

        SERDES_SET(sdsAddr,  0 , 0 , 0x0);
        SERDES_SET(sdsAddr,  9 , 9 , 0x1);
        osal_time_usleep(100 * 1000);
        SERDES_SET(sdsAddr,  9 , 9 , 0x0);
    } else if (sds_num == 8 || sds_num == 9) {
        SERDES_SET(0xb320,  3 , 3 , 0x0);
        SERDES_SET(0xb340,  15 , 15 , 0x1);
        osal_time_usleep(100 * 1000);
        SERDES_SET(0xb340,  15 , 15 , 0x0);
    } else if (sds_num == 12 || sds_num == 13) {
        SERDES_SET(0xbb20,  3 , 3 , 0x0);
        SERDES_SET(0xbb40,  15 , 15 , 0x1);
        osal_time_usleep(100 * 1000);
        SERDES_SET(0xbb40,  15 , 15 , 0x0);
    } else {
        return RT_ERR_FAILED;
    }

    SERDES_SET(0xa004 + ofst,  31 , 16 , 0x7146);
    osal_time_usleep(100 * 1000);
    SERDES_SET(0xa004 + ofst,  31 , 16 , 0x7106);
    SERDES_SET(0xa004 + ofst + 0x100,  31 , 16 , 0x7146);
    osal_time_usleep(100 * 1000);
    SERDES_SET(0xa004 + ofst + 0x100,  31 , 16 , 0x7106);

    return RT_ERR_OK;
}   /* end of drv_rtl8390_sds_rx_rst */

/*
 * RTL8390 mac driver service APIs
 */
rt_macdrv_t rtl8390_macdrv =
{
    .fMdrv_init                             = rtl8390_init,
    .fMdrv_miim_read                        = rtl8390_miim_read,
    .fMdrv_miim_write                       = rtl8390_miim_write,
    .fMdrv_miim_park_read                   = rtl8390_miim_park_read,
    .fMdrv_miim_park_write                  = rtl8390_miim_park_write,
    .fMdrv_miim_broadcast_write             = rtl8390_miim_broadcast_write,
    .fMdrv_miim_extParkPage_read            = rtl8390_miim_extParkPage_read,
    .fMdrv_miim_extParkPage_write           = rtl8390_miim_extParkPage_write,
    .fMdrv_miim_extParkPage_portmask_write  = rtl8390_miim_extParkPage_portmask_write,
    .fMdrv_miim_mmd_read                    = rtl8390_miim_mmd_read,
    .fMdrv_miim_mmd_write                   = rtl8390_miim_mmd_write,
    .fMdrv_miim_mmd_portmask_write          = rtl8390_miim_mmd_portmask_write,
    .fMdrv_table_read                       = rtl8390_table_read,
    .fMdrv_table_write                      = rtl8390_table_write,
    .fMdrv_port_probe                       = rtl8390_port_probe,
    .fMdrv_miim_portmask_write              = rtl8390_miim_portmask_write,
    .fMdrv_miim_pollingEnable_get           = rtl8390_miim_pollingEnable_get,
    .fMdrv_miim_pollingEnable_set           = rtl8390_miim_pollingEnable_set,
    .fMdrv_mac_serdes_rst                   = drv_rtl8390_sds_rx_rst,
    .fMdrv_mac_serdes_read                  = NULL,
    .fMdrv_mac_serdes_write                 = NULL,
    .fMdrv_smi_read                         = NULL,
    .fMdrv_smi_write                        = NULL,
    .fMdrv_miim_portSmiMdxProto_get         = rtl8390_miim_portSmiMdxProto_get,
    .fMdrv_miim_portSmiMdxProto_set         = NULL,
}; /* end of rtl8390_macdrv */


