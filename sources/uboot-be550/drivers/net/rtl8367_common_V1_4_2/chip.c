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
 * Purpose : chip symbol and data type definition in the SDK.
 *
 * Feature : chip symbol and data type definition
 *
 */

#if (!defined(CONFIG_DAL_RTL8367C) && !defined(CONFIG_DAL_RTL8367D))
#define CONFIG_DAL_ALL
#endif

#include <rtk_error.h>
#include <chip.h>
#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_ALL)
#include <dal/rtl8367c/rtl8367c_asicdrv.h>
#endif
#if defined(CONFIG_DAL_RTL8367D) || defined(CONFIG_DAL_ALL)
#include <dal/rtl8367d/rtl8367d_asicdrv.h>
#endif
#include <linux/kernel.h>

//#define FORCE_PROBE_RTL8367C
#define FORCE_PROBE_RTL8367D

#if (!defined(FORCE_PROBE_RTL8367C) && !defined(FORCE_PROBE_RTL8370B) && !defined(FORCE_PROBE_RTL8364B) && !defined(FORCE_PROBE_RTL8363SC_VB) && !defined(FORCE_PROBE_RTL8367D))
#define AUTO_PROBE 1
#else
#define AUTO_PROBE 0
#endif

#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8367C))
static rtk_switch_halCtrl_t rtl8367c_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8367C,

    /* Logical to Physical */
    {0, 1, 2, 3, 4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UTP_PORT0, UTP_PORT1, UTP_PORT2, UTP_PORT3, UTP_PORT4, UNDEFINE_PORT, EXT_PORT0, EXT_PORT1,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {1, 1, 1, 1, 1, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid CPU port mask */
    0x00,

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    7,

    /* Physical port mask */
    0xDF,

    /* Combo Logical port ID */
    4,

    /* HSG Logical portmask */
    (0x1 << EXT_PORT0),

    /* SGMII Logical portmask */
    (0x1 << EXT_PORT0),

    /* Max Meter ID */
    31,

    /* MAX LUT Address Number */
    2112,

    /* Trunk Group Mask */
    0x03,

    /* Packet buffer page number */
    1024
};
#endif

#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8370B))
static rtk_switch_halCtrl_t rtl8370b_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8370B,

    /* Logical to Physical */
    {0, 1, 2, 3, 4, 5, 6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     8, 9, 10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UTP_PORT0, UTP_PORT1, UTP_PORT2, UTP_PORT3, UTP_PORT4, UTP_PORT5, UTP_PORT6, UTP_PORT7,
     EXT_PORT0, EXT_PORT1, EXT_PORT2, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {1, 1, 1, 1, 1, 1, 1, 1,
     0, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) | (0x1 << UTP_PORT5) | (0x1 << UTP_PORT6) | (0x1 << UTP_PORT7) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) | (0x1 << EXT_PORT2) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) | (0x1 << UTP_PORT5) | (0x1 << UTP_PORT6) | (0x1 << UTP_PORT7) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) | (0x1 << EXT_PORT2) ),

    /* Valid CPU port mask */
    (0x1 << EXT_PORT2),

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    10,

    /* Physical port mask */
    0x7FF,

    /* Combo Logical port ID */
    7,

    /* HSG Logical portmask */
    (0x1 << EXT_PORT1),

    /* SGMII Logical portmask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Max Meter ID */
    63,

    /* MAX LUT Address Number 4096 + 64*/
    4160,

    /* Trunk Group Mask */
    0x07,

    /* Packet buffer page number */
    1536
};
#endif

#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8364B))
static rtk_switch_halCtrl_t rtl8364b_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8364B,

    /* Logical to Physical */
    {0xFF, 1, 0xFF, 3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UNDEFINE_PORT, UTP_PORT1, UNDEFINE_PORT, UTP_PORT3, UNDEFINE_PORT, UNDEFINE_PORT, EXT_PORT0, EXT_PORT1,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UNKNOWN_PORT, UTP_PORT, UNKNOWN_PORT, UTP_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT1) | (0x1 << UTP_PORT3) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT1) | (0x1 << UTP_PORT3) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid CPU port mask */
    0x00,

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    7,

    /* Physical port mask */
    0xCA,

    /* Combo Logical port ID */
    4,

    /* HSG Logical portmask */
    (0x1 << EXT_PORT0),

    /* SGMII Logical portmask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Max Meter ID */
    32,

    /* MAX LUT Address Number */
    2112,

    /* Trunk Group Mask */
    0x01,

    /* Packet buffer page number */
    512
};
#endif

#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8363SC_VB))
static rtk_switch_halCtrl_t rtl8363sc_vb_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8363SC_VB,

    /* Logical to Physical */
    {0xFF, 0xFF, 1, 3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UNDEFINE_PORT, UTP_PORT2, UNDEFINE_PORT, UTP_PORT3, UNDEFINE_PORT, UNDEFINE_PORT, EXT_PORT0, EXT_PORT1,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UNKNOWN_PORT, UNKNOWN_PORT, UTP_PORT, UTP_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid CPU port mask */
    0x00,

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    7,

    /* Physical port mask */
    0xCA,

    /* Combo Logical port ID */
    4,

    /* HSG Logical portmask */
    (0x1 << EXT_PORT0),

    /* SGMII Logical portmask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Max Meter ID */
    32,

    /* MAX LUT Address Number */
    2112,

    /* Trunk Group Mask */
    0x01,

    /* Packet buffer page number */
    512
};
#endif

#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8367D))
static rtk_switch_halCtrl_t rtl8367d_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8367D,

    /* Logical to Physical */
    {0, 1, 2, 3, 4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UTP_PORT0, UTP_PORT1, UTP_PORT2, UTP_PORT3, UTP_PORT4, UNDEFINE_PORT, EXT_PORT0, EXT_PORT1,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid CPU port mask */
    0x00,

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    7,

    /* Physical port mask */
    0xFF,

    /* Combo Logical port ID */
    4,

    /* HSG Logical portmask */
    ((0x1 << EXT_PORT0) |  (0x1 << EXT_PORT1)),

    /* SGMII Logical portmask */
    ((0x1 << EXT_PORT0) |  (0x1 << EXT_PORT1)),

    /* Max Meter ID */
    39,

    /* MAX LUT Address Number */
    2056,

    /* Trunk Group Mask */
    0x03,

    /* Packet buffer page number */
    2048
};
#endif

/* Function Name:
 *      switch_probe
 * Description:
 *      Probe switch
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Switch probed
 *      RT_ERR_FAILED   - Switch Unprobed.
 * Note:
 *
 */
rtk_api_ret_t switch_probe(switch_chip_t *pSwitchChip)
{
#if defined(FORCE_PROBE_RTL8367C)

    *pSwitchChip = CHIP_RTL8367C;
    return RT_ERR_OK;

#elif defined(FORCE_PROBE_RTL8370B)

    *pSwitchChip = CHIP_RTL8370B;
    return RT_ERR_OK;

#elif defined(FORCE_PROBE_RTL8364B)

    *pSwitchChip = CHIP_RTL8364B;
    return RT_ERR_OK;

#elif defined(FORCE_PROBE_RTL8363SC_VB)

    *pSwitchChip = CHIP_RTL8363SC_VB;
    return RT_ERR_OK;

#elif defined(FORCE_PROBE_RTL8367D)

    *pSwitchChip = CHIP_RTL8367D;
    return RT_ERR_OK;

#else
    rtk_uint32 retVal;
    rtk_uint32 data;
    rtk_uint32 regValue;

#if defined(CONFIG_DAL_RTL8367C) || defined(CONFIG_DAL_ALL)

    if((retVal = rtl8367c_setAsicReg(0x13C2, 0x0249)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367c_getAsicReg(0x1300, &data)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367c_getAsicReg(0x1301, &regValue)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367c_setAsicReg(0x13C2, 0x0000)) != RT_ERR_OK)
        return retVal;

    switch (data)
    {
        case 0x0276:
        case 0x0597:
        case 0x6367:
            *pSwitchChip = CHIP_RTL8367C;
            return RT_ERR_OK;
        case 0x0652:
        case 0x6368:
            *pSwitchChip = CHIP_RTL8370B;
            return RT_ERR_OK;
        case 0x0801:
        case 0x6511:
            if( (regValue & 0x00F0) == 0x0080)
            {
                *pSwitchChip = CHIP_RTL8363SC_VB;
                return RT_ERR_OK;
            }
            else
            {
                *pSwitchChip = CHIP_RTL8364B;
                return RT_ERR_OK;
            }
        default:
            break;
    }
#endif

#if defined(CONFIG_DAL_RTL8367D) || defined(CONFIG_DAL_ALL)

    if((retVal = rtl8367d_setAsicReg(0x13C2, 0x0249)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_getAsicReg(0x1300, &data)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_getAsicReg(0x1301, &regValue)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_setAsicReg(0x13C2, 0x0000)) != RT_ERR_OK)
        return retVal;

    switch (data)
    {
        case 0x6642:
            *pSwitchChip = CHIP_RTL8367D;
            return RT_ERR_OK;
        default:
            break;
     }
#endif
#endif

    return RT_ERR_CHIP_NOT_FOUND;
}

/* Function Name:
 *      hal_find_device
 * Description:
 *      Find the mac chip from SDK supported mac device lists.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      NULL        - Not found
 *      Otherwise   - Pointer of mac chip HAL structure that found
 * Note:
 *
 */
 
extern switch_chip_t g_switch_chip;
rtk_switch_halCtrl_t *hal_find_device(void)
{
    switch_chip_t switchChip;
    rtk_uint32  retVal;

    /* probe switch */
    if((retVal = switch_probe(&switchChip)) != RT_ERR_OK)
        return NULL;

    g_switch_chip = switchChip;

    switch (switchChip)
    {
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8367C))
        case CHIP_RTL8367C:
            return &rtl8367c_hal_Ctrl;
#endif
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8370B))
        case CHIP_RTL8370B:
            return &rtl8370b_hal_Ctrl;
#endif
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8364B))
        case CHIP_RTL8364B:
            return &rtl8364b_hal_Ctrl;
#endif
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8363SC_VB))
        case CHIP_RTL8363SC_VB:
            return &rtl8363sc_vb_hal_Ctrl;
#endif
#if (AUTO_PROBE || defined(FORCE_PROBE_RTL8367D))
        case CHIP_RTL8367D:
            return &rtl8367d_hal_Ctrl;
#endif
        default:
            return NULL;
    }
	
    /* Not found */
    return NULL;
}
