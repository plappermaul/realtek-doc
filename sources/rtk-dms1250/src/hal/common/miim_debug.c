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
 * $Revision: 91355 $
 * $Date: 2018-08-13 21:54:45 +0800 (Mon, 13 Aug 2018) $
 *
 * Purpose : MIIM service APIs in the SDK.
 *
 * Feature : MIIM service APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <soc/type.h>
#include <hal/phy/phydef.h>
#include <hal/common/miim_debug.h>

#ifdef CONFIG_SDK_RTL8218D
  #include <hal/phy/phy_rtl8218d.h>
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8224QF)
  #include <hal/phy/phy_rtl8295.h>
#endif
#if defined(CONFIG_SDK_RTL826X) || defined(CONFIG_SDK_RTL826XI) || defined(CONFIG_SDK_RTL825X) || defined(CONFIG_SDK_RTL825XI) || defined(CONFIG_SDK_RTL825XL) || defined(CONFIG_SDK_RTL8224)
  #include <hal/phy/phy_rtl826x.h>
#endif

/*
 * Symbol Definition
 */


/*
 * Macro Definition
 */
/* MIIM semaphore handling */
#define MIIM_SEM_LOCK(unit)\
do {\
    hal_control_t   *pHalCtrl;  \
    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)    \
        return RT_ERR_FAILED;   \
    if (osal_sem_mutex_take(pHalCtrl->miim_sem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "MIIM semaphore lock failed");\
        return RT_ERR_FAILED;\
    }\
} while(0)

#define MIIM_SEM_UNLOCK(unit)\
do {\
    hal_control_t   *pHalCtrl;  \
    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)    \
        return RT_ERR_FAILED;   \
    if (osal_sem_mutex_give(pHalCtrl->miim_sem) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "MIIM semaphore unlock failed");\
        return RT_ERR_FAILED;\
    }\
} while(0)


/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

char *
_phy_debug_speed_str(rtk_port_speed_t speed)
{
    switch(speed)
    {
      case PORT_SPEED_10M:
        return "10M";
      case PORT_SPEED_100M:
        return "100M";
      case PORT_SPEED_1000M:
        return "1000M";
      case PORT_SPEED_500M:
        return "1G Lite";
      case PORT_SPEED_2G:
        return "2G";
      case PORT_SPEED_2_5G:
        return "2.5G";
      case PORT_SPEED_5G:
        return "5G";
      case PORT_SPEED_10G:
        return "10G";
      case PORT_SPEED_2_5G_LITE:
        return "2.5G Lite";
      case PORT_SPEED_5G_LITE:
        return "5G Lite";
      case PORT_SPEED_10G_LITE:
        return "10G Lite";

      default:
        return "Unknown";
    }
}


/* Function Name:
 *      miim_debug_sdsRxCali_start
 * Description:
 *      Manually start rx-cali
 * Input:
 *      unit   - unit id
 *      port   - mac ID of PHY port 0 (PHY's base MAC id)
 *      sdsId  - PHY's serdes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
miim_debug_sdsRxCali_start(uint32 unit, uint32 port, uint32 sdsId)
{
#if defined(CONFIG_SDK_RTL8218D)
    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8218D)
    {
        int32   ret;
        if ((ret = phy_8218d_macIntfSdsRxCali_start(unit, port)) != RT_ERR_OK)
        {
            osal_printf("  Unit:%u Port:%u SDS:%u ERR:0x%x\n", unit, port, sdsId, ret);
        }
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8214QF)
    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8214QF)
    {
        int32   ret;
        if ((ret = phy_8214qf_macIntfSdsRxCali_start(unit, port, sdsId)) != RT_ERR_OK)
        {
            osal_printf("  Unit:%u Port:%u SDS:%u ERR:0x%x\n", unit, port, sdsId, ret);
        }
    }
    else
#endif
    {
        osal_printf("  Unit:%u Port:%u SDS:%u Not support\n", unit, port, sdsId);
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      miim_debug_diag_dump
 * Description:
 *      Dump PHY registers
 * Input:
 *      unit   - unit id
 *      port   - mac ID of PHY port 0 (PHY's base MAC id)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
miim_debug_diag_dump(uint32 unit, uint32 port)
{
#if defined(CONFIG_SDK_RTL8218D)
    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8218D)
    {
        _phy_8218d_diag_dump(unit, port);
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
    if ((HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8214QF) || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8224QF))
    {
        _phy_8214qf_diag_dump(unit, port);
    }
    else
#endif
    {
        osal_printf("  Unit:%u Port:%u command not support\n", unit, port);
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      _phy_debug_cmd
 * Description:
 *      PHY debug API.
 * Input:
 *      unit   - unit id
 *      cmd_str - command string
 *      portmask - configure port
 *      param1 - parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_debug_cmd(uint32 unit, char *cmd_str, rtk_portmask_t *portmask, uint32 param1, uint32 param2, uint32 param3, uint32 param4, uint32 param5)
{
    int32       ret = RT_ERR_OK;
    uint32      port = 0;

    if (ret); /* prevent compile warning */
    if (port); /* prevent compile warning */

    RT_LOG(LOG_TRACE, (MOD_HAL|MOD_PHY), "cmd:%s unit=%u portmask=%08X %08X", cmd_str, unit, RTK_PORTMASK_WORD_GET(*portmask,0), RTK_PORTMASK_WORD_GET(*portmask,1));

    if (!osal_strcmp((char *)cmd_str, "speed"))
    {
        rtk_port_speed_t    speed;
        osal_printf("Speed:\n");
        RTK_PORTMASK_SCAN(*portmask, port)
        {
            if ((ret = phy_speed_get(unit, port, &speed)) == RT_ERR_OK)
            {
                osal_printf("  Unit:%u Port:%u speed_enum:%u (%s)\n", unit, port, speed, _phy_debug_speed_str(speed));
            }
            else
            {
                osal_printf("  Unit:%u Port:%u (ERR:0x%X)\n", unit, port, ret);
            }
        }
    }
    else
    if (!osal_strcmp((char *)cmd_str, "speed_sts"))
    {
        rtk_port_speed_t    speed;
        osal_printf("Link speed status:\n");
        RTK_PORTMASK_SCAN(*portmask, port)
        {
            if ((ret = phy_speedStatus_get(unit, port, &speed)) == RT_ERR_OK)
            {
                osal_printf("  Unit:%u Port:%u speed_enum:%u (%s)\n", unit, port, speed, _phy_debug_speed_str(speed));
            }
            else
            {
                osal_printf("  Unit:%u Port:%u (ERR:0x%X)\n", unit, port, ret);
            }
        }
    }
    else
    if (!osal_strcmp((char *)cmd_str, "diag_dump"))
    {
        MIIM_SEM_LOCK(unit);
        RTK_PORTMASK_SCAN(*portmask, port)
        {
            miim_debug_diag_dump(unit, port);
        }
        MIIM_SEM_UNLOCK(unit);
    }
    else
    if (!osal_strcmp((char *)cmd_str, "rx_cali_start"))
    {
        uint32      sdsId = param1;
        MIIM_SEM_LOCK(unit);
        RTK_PORTMASK_SCAN(*portmask, port)
        {
            osal_printf("  Unit:%u Port:%u sdsId:%u rx cali start\n", unit, port, sdsId);
            miim_debug_sdsRxCali_start(unit, port, sdsId);
        }
        MIIM_SEM_UNLOCK(unit);
    }
    else
#if defined(CONFIG_SDK_RTL826X) || defined(CONFIG_SDK_RTL826XI) || defined(CONFIG_SDK_RTL825X) || defined(CONFIG_SDK_RTL825XI) || defined(CONFIG_SDK_RTL825XL)
    if (!osal_strcmp((char *)cmd_str, "826x_sdsr"))
    {
        uint32 sdsPage=param1, sdsReg=param2;
        uint32 data = 0;
        MIIM_SEM_LOCK(unit);
        RTK_PORTMASK_SCAN(*portmask, port)
        {
            if ((ret = _phy_826x_serdes_reg_read(unit, port, sdsPage, sdsReg, &data)) == RT_ERR_OK)
            {
                osal_printf("  unit:%u port:%u page:0x%X reg:0x%X data:0x%X\n", unit, port, sdsPage, sdsReg, data);
            }
            else
            {
                osal_printf("  unit:%u port:%u page:0x%X reg:0x%X data:(ERR:0x%X)\n", unit, port, sdsPage, sdsReg, (uint32)ret);
            }
        }
        MIIM_SEM_UNLOCK(unit);
    }
    else if (!osal_strcmp((char *)cmd_str, "826x_sdsw"))
    {
        uint32 sdsPage = param1, sdsReg = param2;
        uint32 data = param3;
        MIIM_SEM_LOCK(unit);
        RTK_PORTMASK_SCAN(*portmask, port)
        {
            if ((ret = _phy_826x_serdes_reg_write(unit, port, sdsPage, sdsReg, data)) == RT_ERR_OK)
            {
                //osal_printf("unit:%u port:%u page:0x%X reg:0x%X data:0x%X\n", unit, port, sdsPage, sdsReg, data);
            }
            else
            {
                osal_printf("  unit:%u port:%u page:0x%X reg:0x%X data:(ERR:0x%X)\n", unit, port, sdsPage, sdsReg, (uint32)ret);
            }
        }
        MIIM_SEM_UNLOCK(unit);
    }
    else if (!osal_strcmp((char *)cmd_str, "826x_sdspw"))
    {
        uint32 sdsPage = param1, sdsReg = param2;
        uint32 msb = param3, lsb = param4;
        uint32 data = param5;
        uint32 phyData = 0;
        uint32 mask = 0, i = 0;

        for (i = lsb; i <= msb; i++)
        {
            mask |= (1UL << i);
        }

        MIIM_SEM_LOCK(unit);
        RTK_PORTMASK_SCAN(*portmask, port)
        {
            if ((ret = _phy_826x_serdes_reg_read(unit, port, sdsPage, sdsReg, &phyData)) != RT_ERR_OK)
            {
                osal_printf("  unit:%u port:%u page:0x%X reg:0x%X data:(r ERR:0x%X)\n", unit, port, sdsPage, sdsReg, (uint32)ret);
            }
            osal_printf("p:%u r:%u m:%u l:%u d:0x%04X\n",sdsPage, sdsReg, msb, lsb, data);
            osal_printf("phyData:0x%04X\n", phyData);
            phyData = REG32_FIELD_SET(phyData, data, lsb, mask);
            osal_printf("phyData:0x%04X\n", phyData);

            if ((ret = _phy_826x_serdes_reg_write(unit, port, sdsPage, sdsReg, phyData)) == RT_ERR_OK)
            {
                //osal_printf("unit:%u port:%u page:0x%X reg:0x%X data:0x%X\n", unit, port, sdsPage, sdsReg, data);
            }
            else
            {
                osal_printf("  unit:%u port:%u page:0x%X reg:0x%X data:(w ERR:0x%X)\n", unit, port, sdsPage, sdsReg, (uint32)ret);
            }
        }
        MIIM_SEM_UNLOCK(unit);
    }
    else
#endif/* CONFIG_SDK_RTL826X */
    {

        osal_printf("[Common]\n");
        osal_printf("  speed port <ports>                                                - PHY port speed\n");
        osal_printf("  speed_sts port <ports>                                            - PHY port operational link speed status\n");
        osal_printf("  rx_cali_start port <ports> <sdsId>                                - Manually start rx-cali\n");
#if defined(CONFIG_SDK_RTL826X) || defined(CONFIG_SDK_RTL826XI) || defined(CONFIG_SDK_RTL825X) || defined(CONFIG_SDK_RTL825XI) || defined(CONFIG_SDK_RTL825XL) || defined(CONFIG_SDK_RTL8224)
        osal_printf("[RTL826X]\n");
        osal_printf("  826x_sdsr port <ports> <sds_page> <sds_reg>                       - RTL8261/8264 serdes register read\n");
        osal_printf("  826x_sdsw port <ports> <sds_page> <sds_reg> <data>                - RTL8261/8264 serdes register write\n");
        osal_printf("  826x_sdspw port <ports> <sds_page> <sds_reg> <msb> <lsb> <data>   - RTL8261/8264 serdes register write field\n");
#endif

        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;

}






