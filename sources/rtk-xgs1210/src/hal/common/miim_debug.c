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
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <soc/type.h>
#include <hal/phy/phydef.h>
#include <hal/common/miim_debug.h>

#ifdef CONFIG_SDK_RTL8284
  #include <hal/phy/phy_rtl8284.h>
#endif

#ifdef CONFIG_SDK_RTL8226
  #include <hal/phy/nic_rtl8226/nic_rtl8226.h>
#endif

#ifdef CONFIG_SDK_RTL8218D
  #include <hal/phy/phy_rtl8218d.h>
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
    if (phy_osal_sem_mutex_take(pHalCtrl->miim_sem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
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
    if (phy_osal_sem_mutex_give(pHalCtrl->miim_sem) != RT_ERR_OK)\
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
        return "500M";
      case PORT_SPEED_2G:
        return "2G";
      case PORT_SPEED_2_5G:
        return "2.5G";
      case PORT_SPEED_5G:
        return "5G";
      case PORT_SPEED_10G:
        return "10G";
      default:
        return "Unknown";
    }
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
        RTK_PORTMASK_SCAN(*portmask, port)
        {
            if ((ret = phy_speed_get(unit, port, &speed)) == RT_ERR_OK)
            {
                phy_osal_printf("  Unit:%u Port:%u speed_enum:%u (%s)\n", unit, port, speed, _phy_debug_speed_str(speed));
            }
            else
            {
                phy_osal_printf("  Unit:%u Port:%u (ERR:0x%X)\n", unit, port, ret);
            }
        }
    }
    else
    if (!osal_strcmp((char *)cmd_str, "diag_dump"))
    {
        MIIM_SEM_LOCK(unit);
        RTK_PORTMASK_SCAN(*portmask, port)
        {
    #if defined(CONFIG_SDK_RTL8218D)
            if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8218D)
            {
                _phy_8218d_diag_dump(unit, port);
            }
            else
    #endif
            {
                phy_osal_printf("  Unit:%u Port:%u command not support\n", unit, port);
            }
        }
        MIIM_SEM_UNLOCK(unit);
    }
    else
#ifdef CONFIG_SDK_RTL8284
    if (!osal_strcmp((char *)cmd_str, "8284_sdsr"))
    {
        uint32 sdsPage=param1, sdsReg=param2;
        uint32 data = 0;

        RTK_PORTMASK_SCAN(*portmask, port)
        {
            if ((ret = _phy_8284_sdsReg_read(unit, port, sdsPage, sdsReg, &data)) == RT_ERR_OK)
            {
                phy_osal_printf("  unit:%u port:%u page:0x%X reg:0x%X data:0x%X\n", unit, port, sdsPage, sdsReg, data);
            }
            else
            {
                phy_osal_printf("  unit:%u port:%u page:0x%X reg:0x%X data:(ERR:0x%X)\n", unit, port, sdsPage, sdsReg, (uint32)ret);
            }
        }
        return RT_ERR_OK;
    }
    else
    if (!osal_strcmp((char *)cmd_str, "8284_sdswf"))
    {
        uint32 sdsPage = param1, sdsReg = param2;
        uint32 msb = param3, lsb = param4, val = param5;

        RTK_PORTMASK_SCAN(*portmask, port)
        {
            if ((ret = _phy_8284_sdsRegField_write(unit, port, sdsPage, sdsReg, msb, lsb, val)) == RT_ERR_OK)
            {
                //phy_osal_printf("unit:%u port:%u page:0x%X reg:0x%X msb:%u lsb:%u value:0x%X\n", unit, port, sdsPage, sdsReg, msb, lsb, val);
            }
            else
            {
                phy_osal_printf("  unit:%u port:%u page:0x%X reg:0x%X msb:%u lsb:%u value:(ERR:0x%X)\n", unit, port, sdsPage, sdsReg, msb, lsb, (uint32)ret);
            }
        }
        return RT_ERR_OK;
    }
    else
    if (!osal_strcmp((char *)cmd_str, "8284_sdsw"))
    {
        uint32 sdsPage = param1, sdsReg = param2;
        uint32 data = param3;

        RTK_PORTMASK_SCAN(*portmask, port)
        {
            if ((ret = _phy_8284_sdsReg_write(unit, port, sdsPage, sdsReg, data)) == RT_ERR_OK)
            {
                //phy_osal_printf("unit:%u port:%u page:0x%X reg:0x%X data:0x%X\n", unit, port, sdsPage, sdsReg, data);
            }
            else
            {
                phy_osal_printf("  unit:%u port:%u page:0x%X reg:0x%X data:(ERR:0x%X)\n", unit, port, sdsPage, sdsReg, (uint32)ret);
            }
        }
        return RT_ERR_OK;
    }
    else
#endif/* CONFIG_SDK_RTL8284 */
#ifdef CONFIG_SDK_RTL8226
    if (!osal_strcmp((char *)cmd_str, "8226_sds_link_get"))
    {
        HANDLE  hDevice;
        BOOL    drv_sdsLink = 0;
        PHY_SERDES_MODE     drv_sdsMode = PHY_SERDES_MODE_OTHER;
        BOOLEAN ret;

        RTK_PORTMASK_SCAN(*portmask, port)
        {
            hDevice.unit = unit;
            hDevice.port = port;
            if ((ret = Rtl8226_serdes_link_get(hDevice, &drv_sdsLink, &drv_sdsMode)) == SUCCESS)
            {
                phy_osal_printf("  Unit:%u Port:%u serdes link=%u mode=%u\n", unit, port, drv_sdsLink, drv_sdsMode);
            }
            else
            {
                phy_osal_printf("  Unit:%u Port:%u serdes link get fail: %u\n", unit, port, ret);
            }
        }
    }
    else
#endif/* CONFIG_SDK_RTL8226 */
    {

        phy_osal_printf("[Common]\n");
        phy_osal_printf("  speed port <ports>                                                - PHY port speed\n");
        phy_osal_printf("  diag_dump                                                         - dump specific PHY registers for diag\n");
#ifdef CONFIG_SDK_RTL8226
        phy_osal_printf("[RTL8226]\n");
        phy_osal_printf("  8226_sds_link_get port <ports>                                    - RTL8284 serdes link and mode\n");
#endif
#ifdef CONFIG_SDK_RTL8284
        phy_osal_printf("[RTL8284]\n");
        phy_osal_printf("  8284_sdsr port <ports> <sds_page> <sds_reg>                       - RTL8284 serdes register read\n");
        phy_osal_printf("  8284_sdsw port <ports> <sds_page> <sds_reg> <data>                - RTL8284 serdes register write\n");
        phy_osal_printf("  8284_sdswf port <ports> <sds_page> <sds_reg> <value> <msb> <lsb>  - RTL8284 serdes register write field\n");
#endif
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;

}






