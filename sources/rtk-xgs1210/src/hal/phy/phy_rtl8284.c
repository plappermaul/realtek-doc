/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 75479 $
 * $Date: 2017-01-20 15:17:16 +0800 (Fri, 20 Jan 2017) $
 *
 * Purpose : PHY 8284 Driver APIs.
 *
 * Feature : PHY 8284 Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_std_mmd_def.h>
#include <hal/phy/phy_rtl8284.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/phy/nic_rtl8226/nic_rtl8226.h>


/*
 * Symbol Definition
 */
#define MMD_VEND1               30
#define MMD_VEND2               31

#define RTL8284_SDS_IND_ACCESS_WD_MMD                                                               (MMD_VEND1)
#define RTL8284_SDS_IND_ACCESS_WD_ADDR                                                              (0x0099)
  #define RTL8284_SDS_IND_ACCESS_WD_RD_DAT_FIELD_GET(_d)                                            REG32_FIELD_GET(_d,     (0), (0xFFFF << 0))
  #define RTL8284_SDS_IND_ACCESS_WD_RD_DAT_FIELD_SET(_d, _v)                                        REG32_FIELD_SET(_d, _v, (0), (0xFFFF << 0))


#define RTL8284_SDS_IND_ACCESS_RD_MMD                                                               (MMD_VEND1)
#define RTL8284_SDS_IND_ACCESS_RD_ADDR                                                              (0x009A)
  #define RTL8284_SDS_IND_ACCESS_RD_RD_DAT_FIELD_GET(_d)                                            REG32_FIELD_GET(_d,     (0), (0xFFFF << 0))
  #define RTL8284_SDS_IND_ACCESS_RD_RD_DAT_FIELD_SET(_d, _v)                                        REG32_FIELD_SET(_d, _v, (0), (0xFFFF << 0))

#define RTL8284_SDS_IND_ACC_CMD_MMD                                                                 (MMD_VEND1)
#define RTL8284_SDS_IND_ACC_CMD_ADDR                                                                (0x009B)
  #define RTL8284_SDS_IND_ACC_CMD_CMD_EN_FIELD_GET(_d)                                              REG32_FIELD_GET(_d,     (15), (0x1 << 15))
  #define RTL8284_SDS_IND_ACC_CMD_CMD_EN_FIELD_SET(_d, _v)                                          REG32_FIELD_SET(_d, _v, (15), (0x1 << 15))
  #define RTL8284_SDS_IND_ACC_CMD_WREN_FIELD_GET(_d)                                                REG32_FIELD_GET(_d,     (11), (0x1 << 11))
  #define RTL8284_SDS_IND_ACC_CMD_WREN_FIELD_SET(_d, _v)                                            REG32_FIELD_SET(_d, _v, (11), (0x1 << 11))
  #define RTL8284_SDS_IND_ACC_CMD_SDS_REG_ADDR_FIELD_GET(_d)                                        REG32_FIELD_GET(_d,     (6), (0x1F << 6))
  #define RTL8284_SDS_IND_ACC_CMD_SDS_REG_ADDR_FIELD_SET(_d, _v)                                    REG32_FIELD_SET(_d, _v, (6), (0x1F << 6))
  #define RTL8284_SDS_IND_ACC_CMD_SDS_PAGE_FIELD_GET(_d)                                            REG32_FIELD_GET(_d,     (0), (0x3F << 0))
  #define RTL8284_SDS_IND_ACC_CMD_SDS_PAGE_FIELD_SET(_d, _v)                                        REG32_FIELD_SET(_d, _v, (0), (0x3F << 0))


/* error code convert */
#define RTL8284_EC_CONV(fn, ret)    \
            do { \
                if ((fn) == SUCCESS) \
                    ret = RT_ERR_OK; \
                else \
                    ret = RT_ERR_FAILED; \
            } while (0)

/*
 * Data Declaration
 */
rt_phyInfo_t phy_8284_info =
{
    .phy_num    = PORT_NUM_IN_8284,
    .eth_type   = HWP_2_5GE,
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0},
};


/*
 * Macro Declaration
 */



/*
 * Function Declaration
 */


int32
_phy_8284_topMmd_read(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 *pData)
{
    int32       ret;
    uint32      dummy;
    uint32      tmp_port;
    rtk_enable_t    poll_status;
    rtk_portmask_t  poll_status_portmask;

    /* disable MAC polling PHY */
    RTK_PORTMASK_RESET(poll_status_portmask);
    HWP_COPPER_PORT_TRAVS(unit, tmp_port)
    {
        if (hal_miim_pollingEnable_get(unit, tmp_port, &poll_status) == RT_ERR_OK)
        {
            if (poll_status == ENABLED)
            {
                RTK_PORTMASK_PORT_SET(poll_status_portmask, tmp_port);
                hal_miim_pollingEnable_set(unit, tmp_port, DISABLED);
            }
        }
    }

    hal_miim_mmd_read(unit, port, 30, 146, &dummy);

    ret = hal_miim_mmd_read(unit, port, mmdAddr, mmdReg, pData);

    /* recover MAC polling PHY */
    HWP_COPPER_PORT_TRAVS(unit, tmp_port)
    {
        if (RTK_PORTMASK_IS_PORT_SET(poll_status_portmask, tmp_port))
        {
            hal_miim_pollingEnable_set(unit, tmp_port, ENABLED);
        }
    }

    return ret;
}

int32
_phy_8284_topMmd_write(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data)
{

    int32       ret;
    uint32      dummy;
    rtk_enable_t    poll_status = ENABLED;

    hal_miim_pollingEnable_get(unit, port, &poll_status);
    hal_miim_pollingEnable_set(unit, port, DISABLED);

    hal_miim_mmd_read(unit, port, 30, 146, &dummy);

    ret = hal_miim_mmd_write(unit, port, mmdAddr, mmdReg, data);
    hal_miim_pollingEnable_set(unit, port, poll_status);

    return ret;
}


int32
_phy_8284_topMmdField_write(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 offset, uint32 mask, uint32 val)
{
    int32       ret;
    uint32      dummy;
    uint32      phydata;
    rtk_enable_t    poll_status = ENABLED;

    hal_miim_pollingEnable_get(unit, port, &poll_status);
    hal_miim_pollingEnable_set(unit, port, DISABLED);
    hal_miim_mmd_read(unit, port, 30, 146, &dummy);

    if ((ret = hal_miim_mmd_read(unit, port, mmdAddr, mmdReg, &phydata)) != RT_ERR_OK)
    {
        goto EXIT;
    }

    phydata = REG32_FIELD_SET(phydata, val, offset, mask);
    if ((ret = hal_miim_mmd_write(unit, port, mmdAddr, mmdReg, phydata)) != RT_ERR_OK)
    {
        goto EXIT;
    }

  EXIT:
    hal_miim_pollingEnable_set(unit, port, poll_status);
    return ret;
}

int32
_phy_8284_mmd_read(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 *pData)
{
    if (mmdAddr == MMD_VEND1)
    {
        return _phy_8284_topMmd_read(unit, port, mmdAddr, mmdReg, pData);
    }
    else
    {
        return hal_miim_mmd_read(unit, port, mmdAddr, mmdReg, pData);
    }
}


int32
_phy_8284_mmd_write(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data)
{
    RT_LOG(LOG_TRACE, (MOD_HAL|MOD_PHY), "unit=%u port=%u MMD=0x%X REG=0x%X DATA=0x%X", unit, port, mmdAddr, mmdReg, data);
    if (mmdAddr == MMD_VEND1)
    {
        return _phy_8284_topMmd_write(unit, port, mmdAddr, mmdReg, data);
    }
    else
    {
        return hal_miim_mmd_write(unit, port, mmdAddr, mmdReg, data);
    }
}

int32
_phy_8284_mmdField_read(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 offset, uint32 mask, uint32 *pVal)
{
    int32   ret;
    uint32  phydata;

    if (mmdAddr == MMD_VEND1)
    {
        ret = _phy_8284_topMmd_read(unit, port, mmdAddr, mmdReg, &phydata);
    }
    else
    {
        ret = hal_miim_mmd_read(unit, port, mmdAddr, mmdReg, &phydata);
    }

    if (ret != RT_ERR_OK)
        return ret;

    *pVal = REG32_FIELD_GET(phydata, offset, mask);
    return RT_ERR_OK;
}

int32
_phy_8284_mmdField_write(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 offset, uint32 mask, uint32 val)
{
    RT_LOG(LOG_TRACE, (MOD_HAL|MOD_PHY), "unit=%u port=%u MMD=0x%X REG=0x%X offset=%u mask=0x%x val=0x%x", unit, port, mmdAddr, mmdReg, offset, mask, val);
    if (mmdAddr == MMD_VEND1)
    {
        return _phy_8284_topMmdField_write(unit, port, mmdAddr, mmdReg, offset, mask, val);
    }
    else
    {
        int32   ret;
        uint32  phydata;
        if ((ret = hal_miim_mmd_read(unit, port, mmdAddr, mmdReg, &phydata)) != RT_ERR_OK)
        {
            return ret;
        }
        phydata = REG32_FIELD_SET(phydata, val, offset, mask);
        return hal_miim_mmd_write(unit, port, mmdAddr, mmdReg, phydata);
    }
}


/* Function Name:
 *      _phy_8284_sdsIndAccCmdReady_wait
 * Description:
 *      wait command ready
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - read fail
 *      RT_ERR_BUSYWAIT_TIMEOUT - timeout
 * Note:
 *      None
 */
int32
_phy_8284_sdsIndAccCmdReady_wait(uint32 unit, rtk_port_t port)
{
    uint32      cmd;
    int32       ret;
    int32       i, loop_cnt=1000; /* shall use time to wait */

    for (i = 0; i < loop_cnt; i++)
    {
        if ((ret = _phy_8284_mmd_read(unit, port, RTL8284_SDS_IND_ACC_CMD_MMD, RTL8284_SDS_IND_ACC_CMD_ADDR, &cmd)) != RT_ERR_OK)
        {
            return ret;
        }

        if (RTL8284_SDS_IND_ACC_CMD_CMD_EN_FIELD_GET(cmd) == 0)
        {
            return RT_ERR_OK;
        }

        loop_cnt--;
    }
    if (i >= loop_cnt)
    {
        return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8284_sdsReg_read
 * Description:
 *      read serdes register through indirect access
 * Input:
 *      unit - unit id
 *      port - port id
 *      sdsPage - serdes page
 *      sdsRegAddr - serdes register address
 * Output:
 *      pData - serdes register data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - read fail
 * Note:
 *      None
 */
int32
_phy_8284_sdsReg_read(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsRegAddr, uint32 *pData)
{
    int32       ret;
    uint32      cmd = 0;

    cmd = RTL8284_SDS_IND_ACC_CMD_SDS_PAGE_FIELD_SET(cmd, sdsPage);
    cmd = RTL8284_SDS_IND_ACC_CMD_SDS_REG_ADDR_FIELD_SET(cmd, sdsRegAddr);
    cmd = RTL8284_SDS_IND_ACC_CMD_WREN_FIELD_SET(cmd, 0);
    cmd = RTL8284_SDS_IND_ACC_CMD_CMD_EN_FIELD_SET(cmd, 1);

    if ((ret = _phy_8284_mmd_write(unit, port, RTL8284_SDS_IND_ACC_CMD_MMD, RTL8284_SDS_IND_ACC_CMD_ADDR, cmd)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = _phy_8284_sdsIndAccCmdReady_wait(unit, port)) != RT_ERR_OK)
    {
        return ret;
    }

    ret = _phy_8284_mmd_read(unit, port, RTL8284_SDS_IND_ACCESS_RD_MMD, RTL8284_SDS_IND_ACCESS_RD_ADDR, pData);
    return ret;
}

/* Function Name:
 *      _phy_8284_sdsReg_write
 * Description:
 *      Write serdes register through indirect access
 * Input:
 *      unit - unit id
 *      port - base mac ID of the PHY
 *      sdsPage - serdes page
 *      sdsRegAddr - serdes register address
 *      data - register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - write fail
 * Note:
 *      None
 */
int32
_phy_8284_sdsReg_write(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsRegAddr, uint32 data)
{
    int32       ret;
    uint32      cmd = 0;

    RT_LOG(LOG_TRACE, (MOD_HAL|MOD_PHY), "unit=%u port=%u sdsPage=0x%X sdsReg=0x%X data=0x%x", unit, port, sdsPage, sdsRegAddr, data);
    cmd = RTL8284_SDS_IND_ACC_CMD_SDS_PAGE_FIELD_SET(cmd, sdsPage);
    cmd = RTL8284_SDS_IND_ACC_CMD_SDS_REG_ADDR_FIELD_SET(cmd, sdsRegAddr);
    cmd = RTL8284_SDS_IND_ACC_CMD_WREN_FIELD_SET(cmd, 1);
    cmd = RTL8284_SDS_IND_ACC_CMD_CMD_EN_FIELD_SET(cmd, 1);

    if ((ret = _phy_8284_mmd_write(unit, port, RTL8284_SDS_IND_ACCESS_WD_MMD, RTL8284_SDS_IND_ACCESS_WD_ADDR, data)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = _phy_8284_mmd_write(unit, port, RTL8284_SDS_IND_ACC_CMD_MMD, RTL8284_SDS_IND_ACC_CMD_ADDR, cmd)) != RT_ERR_OK)
    {
        return ret;
    }

    ret = _phy_8284_sdsIndAccCmdReady_wait(unit, port);
    return ret;
}


/* Function Name:
 *      _phy_8284_sdsRegField_write
 * Description:
 *      Write field of serdes register through indirect access
 * Input:
 *      unit - unit id
 *      port - port id
 *      sdsPage - serdes page
 *      sdsRegAddr - serdes register address
 *      msb - MSB of the field
 *      lsb - LSB of the field
 *      value - field value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - write fail
 * Note:
 *      None
 */
int32
_phy_8284_sdsRegField_write(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsRegAddr, uint32 msb, uint32 lsb, uint32 value)
{
    uint32      phydata, mask;
    int32       ret;

    if ((ret = _phy_8284_sdsReg_read(unit, port, sdsPage, sdsRegAddr, &phydata)) != RT_ERR_OK)
    {
        return ret;
    }

    mask = UINT32_BITS_MASK(msb, lsb);
    phydata = REG32_FIELD_SET(phydata, value, lsb, mask);

    ret = _phy_8284_sdsReg_write(unit, port, sdsPage, sdsRegAddr, phydata);

    return ret;
}


/* Function Name:
 *      phy_8284_init
 * Description:
 *      Initialize PHY.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8284_init(uint32 unit, rtk_port_t port)
{
    uint32  base_port;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_init", unit, port);
    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "unit=%u,port=%u get base port ID fail", unit, port);
        return RT_ERR_PORT_ID;
    }

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_init, base_port=%u", unit, port, base_port);
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8284_media_get
 * Description:
 *      Get PHY media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
int32
phy_8284_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    *pMedia = PORT_MEDIA_COPPER;
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8284_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8284_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_enable_set enable=%u\n", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8284_EC_CONV(Rtl8226_enable_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u fail=%X\n", unit, port, ret);
    }

    return ret;
}


/* Function Name:
 *      phy_8284_autoNegoEnable_get
 * Description:
 *      Get autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8284_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8284_EC_CONV(Rtl8226_autoNegoEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_autoNegoEnable_get fail=%X\n", unit, port, ret);
    }

    return ret;
}

/* Function Name:
 *      phy_8284_autoNegoEnable_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8284_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_autoNegoEnable_set enable=%u\n", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8284_EC_CONV(Rtl8226_autoNegoEnable_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_autoNegoEnable_set fail=%X\n", unit, port, ret);
    }

    return ret;
}

/* Function Name:
 *      phy_8284_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8284_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    HANDLE  hDevice;
    PHY_LINK_ABILITY    drv_phyAbility;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    osal_memset(&drv_phyAbility, 0, sizeof(PHY_LINK_ABILITY));

    RTL8284_EC_CONV(Rtl8226_autoNegoAbility_get(hDevice, &drv_phyAbility), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_autoNegoEnable_set fail=%X\n", unit, port, ret);
        return ret;
    }

    pAbility->Half_10   = drv_phyAbility.Half_10;
    pAbility->Full_10   = drv_phyAbility.Full_10;
    pAbility->Half_100  = drv_phyAbility.Half_100;
    pAbility->Full_100  = drv_phyAbility.Full_100;
    pAbility->Full_1000 = drv_phyAbility.Full_1000;
    pAbility->adv_2_5G  = drv_phyAbility.adv_2_5G;
    pAbility->FC        = drv_phyAbility.FC;
    pAbility->AsyFC     = drv_phyAbility.AsyFC;

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8284_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8284_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    HANDLE  hDevice;
    PHY_LINK_ABILITY    drv_phyAbility;
    int32   ret;


    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 10h=%u 10f=%u 100h=%u 100f=%u 1000f=%u 2.5g=%u FC=%u AsyFC=%u", unit, port,
        pAbility->Half_10, pAbility->Full_10, pAbility->Half_100, pAbility->Full_100, pAbility->Full_1000, pAbility->adv_2_5G, pAbility->FC, pAbility->AsyFC);

    hDevice.unit = unit;
    hDevice.port = port;

    osal_memset(&drv_phyAbility, 0, sizeof(PHY_LINK_ABILITY));

    drv_phyAbility.Half_10   = pAbility->Half_10;
    drv_phyAbility.Full_10   = pAbility->Full_10;
    drv_phyAbility.Half_100  = pAbility->Half_100;
    drv_phyAbility.Full_100  = pAbility->Full_100;
    drv_phyAbility.Full_1000 = pAbility->Full_1000;
    drv_phyAbility.adv_2_5G  = pAbility->adv_2_5G;
    drv_phyAbility.FC        = pAbility->FC;
    drv_phyAbility.AsyFC     = pAbility->AsyFC;


    RTL8284_EC_CONV(Rtl8226_autoNegoAbility_set(hDevice, &drv_phyAbility), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_autoNegoAbility_set fail=%X\n", unit, port, ret);
        return ret;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      phy_8284_duplex_get
 * Description:
 *      Get duplex mode status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pDuplex - pointer to PHY duplex mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8284_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8284_EC_CONV(Rtl8226_duplex_get(hDevice, &drv_enable), ret);
    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    *pDuplex = drv_enable;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8284_duplex_set
 * Description:
 *      Set duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8284_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{
    uint32      phydata;
    int32       ret;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_duplex_set duplex=%u\n", unit, port, duplex);

    if ((ret = _phy_8284_mmd_read(unit, port, MMD_VEND2, 0xA400, &phydata)) != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_duplex_set fail=%X\n", unit, port, ret);
        return ret;
    }

    if (duplex == 1)
        phydata |= BIT_8;
    else
        phydata &= ~BIT_8;

    if ((ret = _phy_8284_mmd_write(unit, port, MMD_VEND2, 0xA400, phydata)) != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_duplex_set fail=%X\n", unit, port, ret);
        return ret;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      phy_8284_speed_get
 * Description:
 *      Get link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8284_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    HANDLE  hDevice;
    UINT16  drv_speed;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8284_EC_CONV(Rtl8226_speed_get(hDevice, &drv_speed), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_speed_get fail=%X\n", unit, port, ret);
        return ret;
    }

    switch(drv_speed)
    {
      case LINK_SPEED_10M:
        *pSpeed = PORT_SPEED_10M;
        break;
      case LINK_SPEED_100M:
        *pSpeed = PORT_SPEED_100M;
        break;
      case LINK_SPEED_1G:
        *pSpeed = PORT_SPEED_1000M;
        break;
      case LINK_SPEED_500M:
        *pSpeed = PORT_SPEED_500M;
        break;
	  case LINK_SPEED_2P5G:
        *pSpeed = PORT_SPEED_2_5G;
        break;
      case NO_LINK:
        *pSpeed = PORT_SPEED_10M;
        break;
      default:
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8284_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
int32
phy_8284_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    HANDLE  hDevice;
    UINT16  drv_speed = 0;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_speed_set speed=%u\n", unit, port, speed);
    switch(speed)
    {
      case PORT_SPEED_10M:
        drv_speed = 10;
        break;
      case PORT_SPEED_100M:
        drv_speed = 100;
        break;
      default:
        {
            uint32  phydata = 0;
            _phy_8284_mmd_read(unit, port, MMD_PMAPMD, 0, &phydata);
            switch(speed)
            {
              case PORT_SPEED_1000M:
                phydata &= ~(BIT_13);
                phydata |= (BIT_6);
                break;
              case PORT_SPEED_2_5G:
                phydata &= ~(BIT_5 | BIT_2);
                phydata |= (BIT_13 | BIT_6 | BIT_4 | BIT_3);
                break;
              default:
                return RT_ERR_INPUT;
            }
            ret = _phy_8284_mmd_write(unit, port, MMD_PMAPMD, 0, phydata);
            if (ret != RT_ERR_OK)
            {
                RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_speed_set fail=%X\n", unit, port, ret);
            }
            return ret;
        }
    } /* end switch */

    RTL8284_EC_CONV(Rtl8226_force_speed_set(hDevice, drv_speed), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_speed_set fail=%X\n", unit, port, ret);
    }

    return ret;
}


/* Function Name:
 *      phy_8284_crossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8284_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    HANDLE  hDevice;
    PHY_CROSSPVER_MODE  drv_crossOverMode;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8284_EC_CONV(Rtl8226_crossOverMode_get(hDevice, &drv_crossOverMode), ret);
    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    switch (drv_crossOverMode)
    {
      case PHY_CROSSPVER_MODE_MDI:
        *pMode = PORT_CROSSOVER_MODE_MDI;
        break;
      case PHY_CROSSPVER_MODE_MDIX:
        *pMode = PORT_CROSSOVER_MODE_MDIX;
        break;
      case PHY_CROSSPVER_MODE_AUTO:
        *pMode = PORT_CROSSOVER_MODE_AUTO;
        break;
      default:
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8284_crossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8284_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    HANDLE  hDevice;
    PHY_CROSSPVER_MODE  drv_crossOverMode;
    int32   ret;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_crossOverMode_set mode=%u\n", unit, port, mode);

    hDevice.unit = unit;
    hDevice.port = port;

    switch (mode)
    {
      case PORT_CROSSOVER_MODE_MDI:
        drv_crossOverMode = PHY_CROSSPVER_MODE_MDI;
        break;
      case PORT_CROSSOVER_MODE_MDIX:
        drv_crossOverMode = PHY_CROSSPVER_MODE_MDIX;
        break;
      case PORT_CROSSOVER_MODE_AUTO:
        drv_crossOverMode = PHY_CROSSPVER_MODE_AUTO;
        break;
      default:
        return RT_ERR_INPUT;
    }


    RTL8284_EC_CONV(Rtl8226_crossOverMode_set(hDevice, drv_crossOverMode), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u fail=%X\n", unit, port, ret);
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8284_crossOverStatus_get
 * Description:
 *      Get cross over status in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
int32
phy_8284_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    HANDLE  hDevice;
    PHY_CROSSPVER_STATUS  drv_crossOverStatus;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8284_EC_CONV(Rtl8226_crossOverStatus_get(hDevice, &drv_crossOverStatus), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u fail=%X\n", unit, port, ret);
        return ret;
    }

    switch (drv_crossOverStatus)
    {
      case PHY_CROSSPVER_STATUS_MDI:
        *pStatus = PORT_CROSSOVER_STATUS_MDI;
        break;
      case PHY_CROSSPVER_STATUS_MDIX:
        *pStatus = PORT_CROSSOVER_STATUS_MDIX;
        break;
      default:
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8284_masterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pMasterSlaveCfg     - pointer to the PHY master slave configuration
 *      pMasterSlaveActual  - pointer to the PHY master slave actual link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
int32
phy_8284_masterSlave_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlaveCfg,
    rtk_port_masterSlave_t   *pMasterSlaveActual)
{
    HANDLE  hDevice;
    PHY_MASTERSLAVE_MODE  drv_masterSlaveMode;
    int32   ret;
    uint32  phydata;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8284_EC_CONV(Rtl8226_masterSlave_get(hDevice, &drv_masterSlaveMode), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u fail=%X\n", unit, port, ret);
        return ret;
    }

    switch (drv_masterSlaveMode)
    {
      case PHY_AUTO_MODE:
        *pMasterSlaveActual = PORT_AUTO_MODE;
        break;
      case PHY_MASTER_MODE:
        *pMasterSlaveActual = PORT_MASTER_MODE;
        break;
      case PHY_SLAVE_MODE:
        *pMasterSlaveActual = PORT_SLAVE_MODE;
        break;
      default:
        return RT_ERR_FAILED;
    }

    /* config status get */
    if ((ret = _phy_8284_mmd_read(unit, port, MMD_AN, 32, &phydata)) != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u fail=%X\n", unit, port, ret);
        return ret;
    }

    if (phydata & BIT_15)
    {
        if (phydata & BIT_14)
        {
            *pMasterSlaveCfg = PORT_MASTER_MODE;
        }
        else
        {
            *pMasterSlaveCfg = PORT_SLAVE_MODE;
        }
    }
    else
    {
        *pMasterSlaveCfg = PORT_AUTO_MODE;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8284_masterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      masterSlave         - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8284_masterSlave_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    HANDLE  hDevice;
    PHY_MASTERSLAVE_MODE  drv_masterSlaveMode;
    int32   ret;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_masterSlave_set mode=%u\n", unit, port, masterSlave);

    hDevice.unit = unit;
    hDevice.port = port;

    switch (masterSlave)
    {
      case PORT_AUTO_MODE:
        drv_masterSlaveMode = PHY_AUTO_MODE;
        break;
      case PORT_MASTER_MODE:
        drv_masterSlaveMode = PHY_MASTER_MODE;
        break;
      case PORT_SLAVE_MODE:
        drv_masterSlaveMode = PHY_SLAVE_MODE;
        break;
      default:
        return RT_ERR_FAILED;
    }

    RTL8284_EC_CONV(Rtl8226_masterSlave_set(hDevice, drv_masterSlaveMode), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u ret=%u\n", unit, port, ret);
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8284_loopback_get
 * Description:
 *      Get PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      pEnable           -loopback mode status;
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8284_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8284_EC_CONV(Rtl8226_loopback_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u ret=%u\n", unit, port, ret);
    }

    return ret;
}


/* Function Name:
 *      phy_8284_loopback_set
 * Description:
 *      Set PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - ENABLED: Enable loopback;
 *                            DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8284_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_loopback_set enable=%u\n", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8284_EC_CONV(Rtl8226_loopback_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u ret=%u\n", unit, port, ret);
    }

    return ret;

}

/* Function Name:
 *      phy_8284_gigaLiteEnable_get
 * Description:
 *      Get the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of Giga Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8284_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable = FALSE;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8284_EC_CONV(Rtl8226_gigaLiteEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u ret=%u\n", unit, port, ret);
    }

    return ret;
}



/* Function Name:
 *      phy_8284_gigaLiteEnable_set
 * Description:
 *      Set the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of Giga Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8284_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_gigaLiteEnable_set enable=%u\n", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8284_EC_CONV(Rtl8226_gigaLiteEnable_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u ret=%u\n", unit, port, ret);
    }

    return ret;
}



/* Function Name:
 *      phy_8284_2point5gLiteEnable_get
 * Description:
 *      Get the status of 2.5G Lite of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of Giga Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8284_2pt5gLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8284_EC_CONV(Rtl8226_2p5gLiteEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u ret=%u\n", unit, port, ret);
    }

    return ret;
}



/* Function Name:
 *      phy_8284_2point5gLiteEnable_set
 * Description:
 *      Set the status of 2.5G Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of Giga Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8284_2pt5gLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u 8284_2pt5gLiteEnable_set enable=%u\n", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8284_EC_CONV(Rtl8226_2p5gLiteEnable_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%u port=%u ret=%u\n", unit, port, ret);
    }

    return ret;
}


/* Function Name:
 *      phy_8284_chipRevId_get
 * Description:
 *      Get chip revision ID
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pChip_rev_id - chip revision ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8284_chipRevId_get(uint32 unit, rtk_port_t port, uint16 *chip_rev_id)
{
    *chip_rev_id = 0;

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8284drv_mapperInit
 * Description:
 *      Initialize PHY 8284 driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_8284drv_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8284;
    pPhydrv->fPhydrv_init = phy_8284_init;
    pPhydrv->fPhydrv_media_get = phy_8284_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8284_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8284_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8284_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8284_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8284_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8284_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8284_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8284_speed_set;
    pPhydrv->fPhydrv_enable_set = phy_8284_enable_set;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8284_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8284_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_8284_crossOverStatus_get;
    pPhydrv->fPhydrv_masterSlave_get = phy_8284_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_8284_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_8284_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8284_loopback_set;
    pPhydrv->fPhydrv_reg_mmd_get = _phy_8284_mmd_read;
    pPhydrv->fPhydrv_reg_mmd_set = _phy_8284_mmd_write;
    pPhydrv->fPhydrv_chipRevId_get = phy_8284_chipRevId_get;

    pPhydrv->fPhydrv_gigaLiteEnable_get = phy_8284_gigaLiteEnable_get;
    pPhydrv->fPhydrv_gigaLiteEnable_set = phy_8284_gigaLiteEnable_set;

    pPhydrv->fPhydrv_2pt5gLiteEnable_get = phy_8284_2pt5gLiteEnable_get;
    pPhydrv->fPhydrv_2pt5gLiteEnable_set = phy_8284_2pt5gLiteEnable_set;

} /* end of phy_8284drv_ge_mapperInit*/
