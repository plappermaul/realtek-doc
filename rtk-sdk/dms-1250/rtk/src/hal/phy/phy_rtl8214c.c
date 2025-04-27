/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
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
 * Purpose : PHY 8218B/8218FB/8214FC Driver APIs.
 *
 * Feature : PHY 8218B/8218FB/8214FC Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl8214c.h>
#include <hal/phy/phy_rtl8214c_patch.h>
#include <hal/mac/miim_common_drv.h>
#include <osal/memory.h>

/*
 * Symbol Definition
 */
#define PHY_MEDIA_LINKDOWN          (0)
#define PHY_MEDIA_LINKUP            (1)

typedef struct hal_phy_info_s
{
    uint8   auto_1000f[RTK_MAX_NUM_OF_PORTS]; /* copper, reg[9].bit[9] shadow for patch down-speed mechanism */
} hal_phy_info_t;


static hal_phy_info_t   *pPhy_info[RTK_MAX_NUM_OF_UNIT];
static uint32           phyInfo_alloc[RTK_MAX_NUM_OF_UNIT];

/*
 * Data Declaration
 */
rt_phyInfo_t phy_8214C_info =
{
    .phy_num    = PORT_NUM_IN_8214C,
    .eth_type   = HWP_GE,
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0},
    .flags      = RTK_PHYINFO_FLAG_NONE,

    .xGePhyLocalDuplexAbilityDev            = 0,
    .xGePhyLocalDuplexAbilityAddr           = 0,
    .xGePhyLocalDuplexAbilityBit            = 0,

    .xGePhyLocal1000MSpeedAbilityDev        = 0,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0,
    .xGePhyLocal1000MSpeedAbilityBit        = 0,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 0,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 0,
};


/* Function Name:
 *      phy_8214Cdrv_ge_mapperInit
 * Description:
 *      Initialize PHY 8214C driver mapper.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8214Cdrv_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8214C;
    pPhydrv->fPhydrv_init = phy_8214c_init;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8214c_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8214c_eeeEnable_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8214c_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8214c_greenEnable_set;
    pPhydrv->fPhydrv_enable_set = phy_8214c_enable_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8214c_ieeeTestMode_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8214c_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_common_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_common_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8214c_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8214c_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_common_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_common_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8214c_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8214c_speed_set;
    pPhydrv->fPhydrv_eeepEnable_get = phy_8214c_eeepEnable_get;
    pPhydrv->fPhydrv_eeepEnable_set = phy_8214c_eeepEnable_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8214c_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8214c_downSpeedEnable_set;
    pPhydrv->fPhydrv_reg_get = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set = phy_common_reg_park_set;
    pPhydrv->fPhydrv_reg_mmd_get = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_reg_mmd_portmask_set = phy_common_reg_mmd_portmask_set;
    pPhydrv->fPhydrv_reg_extParkPage_get = phy_common_reg_extParkPage_get;
    pPhydrv->fPhydrv_reg_extParkPage_set = phy_common_reg_extParkPage_set;
    pPhydrv->fPhydrv_reg_extParkPage_portmask_set = phy_common_reg_extParkPage_portmask_set;
    pPhydrv->fPhydrv_masterSlave_get = phy_common_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_common_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_common_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_common_loopback_set;
    pPhydrv->fPhydrv_patch_set = phy_8214c_patch_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_8214c_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8214c_rtct_start;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8214c_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8214c_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_8214c_crossOverStatus_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8214c_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8214c_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_gigaLiteEnable_get = phy_8214c_gigaLiteEnable_get;
    pPhydrv->fPhydrv_gigaLiteEnable_set = phy_8214c_gigaLiteEnable_set;
#endif
} /* end of phy_8214Cdrv_ge_mapperInit*/


/*
 * Function Declaration
 */
/* Function Name:
 *      phy_8214c_init
 * Description:
 *      Initialize PHY 8214C.
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
phy_8214c_init(uint32 unit, rtk_port_t port)
{
    uint32  base_port = 0;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    /* Initialize the PHY software shadow */
    if (phyInfo_alloc[unit] == 0)
    {
        pPhy_info[unit] = (hal_phy_info_t *)osal_alloc(sizeof(hal_phy_info_t));
        if (NULL == pPhy_info[unit])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "memory allocate failed");
            return RT_ERR_FAILED;
        }
        osal_memset(pPhy_info[unit], 0, sizeof(hal_phy_info_t));
        phyInfo_alloc[unit] = 1;
    }

    if ((ret = hal_miim_write(unit, base_port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
        return ret;

    /* Read the chip copper Auto-1000F ability value and initial the shadow value */
    if ((ret = hal_miim_read(unit, port, 0xa42, 9, &val)) != RT_ERR_OK)
        return ret;
    /* store to shadow */
    (*pPhy_info[unit]).auto_1000f[port] = (val >> _1000Base_TFullDuplex_OFFSET) & 0x1;

    /* Set chip flag for phy cable esd watchdog */
    if((port % 4) == 0)
    {
        if ((ret = hal_miim_write(unit, port, 0, 29, 0x1322)) != RT_ERR_OK)
            return ret;

        if ((ret = hal_miim_read(unit, port, 0, 30, &val)) != RT_ERR_OK)
            return ret;

        val |= 0x10;
        if ((ret = hal_miim_write(unit, port, 0, 30, val)) != RT_ERR_OK)
            return ret;
    }

    /* For 838x & 833x, Set PKTGEN RXPATH is from MAC TX & Enable PKTGEN RX for Phy error counter */
    if (HWP_8380_30_FAMILY(unit))
    {
        uint32 phy_data;

        ret = hal_miim_read(unit,  port,  0xc80,  16, &phy_data);
        if(ret != RT_ERR_OK)
            return ret;

        phy_data &= ~0xF;
        phy_data |= 0x1<<2;
        phy_data |= 0x2<<0;
        ret = hal_miim_write(unit,  port,  0xc80,  16, phy_data);
        if(ret != RT_ERR_OK)
            return ret;
    }

    phy_8214c_eeeEnable_set(unit, port, DISABLED);
    phy_8214c_greenEnable_set(unit, port, DISABLED);
    return RT_ERR_OK;
} /* end of phy_8214c_init */

/* Function Name:
 *      phy_8214c_eeeEnable_get
 * Description:
 *      Get enable status of EEE function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEE
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8214c_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, 0xa43, 25, &phyData)) != RT_ERR_OK)
        return ret;

    if (((phyData >> 5) & 0x1) == 0x1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
} /* end of phy_8214c_eeeEnable_get */

/* Function Name:
 *      phy_8214c_eeeEnable_set
 * Description:
 *      Set enable status of EEE function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEE
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8214c_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  phyData;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t     an_enable;

    phy_common_autoNegoEnable_get(unit, port, &an_enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, 0xa43, 25, &phyData)) != RT_ERR_OK)
            return ret;

    #if 0 /* always configure to MAC mode EEE, the mode configuration could be aparted to another API in the future */
    if (ENABLED == enable)
    {
        phyData |= (1 << 5);
        if ((ret = hal_miim_write(unit, port, 0xa43, 25, phyData)) != RT_ERR_OK)
            return ret;
    }
    else
    #else
    {
        phyData &= ~(1 << 5);
        if ((ret = hal_miim_write(unit, port, 0xa43, 25, phyData)) != RT_ERR_OK)
            return ret;
    }
    #endif

    if(ENABLED == enable)
        phyData = 0x6; /*enable 100M/1000M EEE ability*/
    else
        phyData = 0x0; /*disable 100M/1000M EEE ability*/

    ret = hal_miim_mmd_write(unit, port, 7, 60, phyData);
    if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        return ret;
    }

    if ((ret = hal_miim_read(unit, port, 0xa42, 20, &phyData)) != RT_ERR_OK)
    {
        return ret;
    }

    if(ENABLED == enable)
        phyData |= 0x1 << 7; /*enable 500M EEE ability*/
    else
        phyData &= ~(0x1 << 7); /*disable 500M EEE ability*/

    if ((ret = hal_miim_write(unit, port, 0xa42, 20, phyData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == an_enable)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
            return ret;

        phyData = phyData & ~(RestartAutoNegotiation_MASK);
        phyData = phyData | (an_enable << RestartAutoNegotiation_OFFSET);

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
} /* end of phy_8214c_eeeEnable_set */

/* Function Name:
 *      phy_8214c_greenEnable_get
 * Description:
 *      Get the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - pointer to status of link-up green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8214c is supported the per-port link-up green feature.
 */
int32
phy_8214c_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData, fixed_page;

    fixed_page = 0;

    /* get value from CHIP*/
    if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
        return ret;

    if ((phyData >> 9) & 0x1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
} /* end of phy_8214c_greenEnable_get */

/* Function Name:
 *      phy_8214c_greenEnable_set
 * Description:
 *      Set the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-up  green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8214c is supported the per-port link-up green feature.
 */
int32
phy_8214c_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  value, fixed_page;
    int32   ret = RT_ERR_FAILED;
    uint32  phyData0, restore_phyData0, restore_flag;

    fixed_page = 0;
    restore_flag = 0;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    restore_phyData0 = phyData0;

    /* If the port is link up, link down at first */
    if ((phyData0 & (PowerDown_MASK)) == 0)
    {
        restore_flag = 1;
        phyData0 |= (1 << PowerDown_OFFSET);
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_read(unit, port, fixed_page, 28, &value)) != RT_ERR_OK)
        return ret;

    if (ENABLED == enable)
    {
        value |= (1 << 9);
        if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, fixed_page, 28, value)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        value &= ~(1 << 9);
        if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, fixed_page, 28, value)) != RT_ERR_OK)
            return ret;
    }

    /* To restore Reg 0*/
    if (restore_flag == 1)
    {
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, restore_phyData0)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
} /* end of phy_8214c_greenEnable_set */

/* Function Name:
 *      phy_8214c_enable_set
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
phy_8214c_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
        return ret;

    if (DISABLED == enable)
    {
        phyData |= (1 << PowerDown_OFFSET);
    }else{
        phyData &= ~(PowerDown_MASK);
    }

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
         return ret;

    return ret;
} /* end of phy_8214c_enable_set */

void
_phy_8214c_ieeeTestModeAllPhyPort_disable(uint32 unit, rtk_port_t phyBasePort, uint32 phyPortCnt)
{
    uint32      phyPort;

    for (phyPort = 0; phyPort < phyPortCnt; phyPort++)
    {
        hal_miim_write(unit, phyBasePort+phyPort,0xA41,9,0x0E00);  // Disable Test mode
    }
}

/* Function Name:
 *      phy_8214c_ieeeTestMode_set
 * Description:
 *      Set test mode for Giga PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      pTestMode->mode  - test mode 1 ~ 4 which is defined in IEEE 40.6.1.1.2
 *      pTestMode->channel  - Channel A, B, C, D, or none
 *      pTestMode->flags -
 *          RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS -
 *              apply the test on all ports of the PHY.
 *              To use this feature, the "port" parameter shall set to the first port of the PHY.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_NOT_ALLOWED - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED - test mode is not supported
 * Note:
 *      RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS only support in mode 4
 */
int32
phy_8214c_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{
    uint32      phyBasePort = HWP_PHY_BASE_MACID(unit, port);
    uint32      phyPortCnt = 0;
    uint32      val, chn;

    osal_printf("RTL8214C Test Mode (PHYID: %u)\n", port);

    if ((pTestMode->flags & RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS) && (pTestMode->mode != 4))
    {
        osal_printf("mode %u not support all-phy-port operation.\n", pTestMode->mode);
        return RT_ERR_NOT_ALLOWED;
    }

    switch (HWP_PHY_MODEL_BY_PORT(unit, port))
    {
      case RTK_PHYTYPE_RTL8214C:
        phyPortCnt = PORT_NUM_IN_8214C;
        break;
      default:
        osal_printf("port type not support.\n");
        return RT_ERR_PORT_NOT_SUPPORTED;
    }

    switch (pTestMode->mode)
    {
      case 1:
        {
            _phy_8214c_ieeeTestModeAllPhyPort_disable(unit, phyBasePort, phyPortCnt);

            hal_miim_write(unit, port,0xA41,9,0x2E00); // Test mode 1
            return RT_ERR_OK;
        }
        break;

      case 2:
        {
            _phy_8214c_ieeeTestModeAllPhyPort_disable(unit, phyBasePort, phyPortCnt);

            hal_miim_write(unit, port,0xA41,9,0x4E00);// Test mode 2
            return RT_ERR_OK;
        }
        break;

    case 4:
        {
            uint32      phyPort;

            //0=A,1=B,2=C,3=D
            chn = pTestMode->channel - PHY_TESTMODE_CHANNEL_A;
            if (chn > 3)
            {
                osal_printf(" Error test channel: %u\n", chn);
                return RT_ERR_INPUT;
            }

            osal_printf(" test channel: %u\n", chn);

            if (!(pTestMode->flags & RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS))
            {
                /* single port test */
                for (phyPort = 0; phyPort < phyPortCnt; phyPort++)
                {
                    hal_miim_write(unit, phyBasePort+phyPort,0xBC0,18,0x0000);
                    hal_miim_write(unit, phyBasePort+phyPort,0xBC0,19,0x01C0);
                    hal_miim_write(unit, phyBasePort+phyPort,0xA41,9,0x0E00);
                    hal_miim_write(unit, phyBasePort+phyPort,0xA43,27,0x80C1);
                    hal_miim_write(unit, phyBasePort+phyPort,0xA43,28,0x7E00);// Disable Test mode
                }

                val = 0x1 << chn;
                hal_miim_write(unit, port,0xBC0,18, val);
                hal_miim_write(unit, port,0xBC0,19,0x01D0);/*Enable get power control*/
                hal_miim_write(unit, port,0xA41,9,0x8E00);/*Enable Test Mode 4*/
                hal_miim_write(unit, port,0xA43,27 ,0x80C1);/*Current adjusting*/
                hal_miim_write(unit, port,0xA43,28,0xFE00);

                return RT_ERR_OK;
            }
            else
            {
                osal_printf("Not support all-phy-port operation.\n");
                return RT_ERR_PORT_NOT_SUPPORTED;
            }
        }
        break;

    default:
        osal_printf("The mode (%u) is not suppoted.\n", pTestMode->mode);
        return RT_ERR_PORT_NOT_SUPPORTED;
    }

}

#if !defined(__BOOTLOADER__)
/* Function Name:
 *      phy_8214c_autoNegoAbility_get
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
phy_8214c_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData4;
    uint32  phyData9;
    rtk_enable_t     enable;

    phy_common_autoNegoEnable_get(unit, port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
    pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
        return ret;

    pAbility->Full_100= (phyData4 & _100Base_TX_FD_R4_MASK) >> _100Base_TX_FD_R4_OFFSET;
    pAbility->Half_100= (phyData4 & _100Base_TX_R4_MASK) >> _100Base_TX_R4_OFFSET;
    pAbility->Full_10= (phyData4 & _10Base_T_FD_R4_MASK) >> _10Base_T_FD_R4_OFFSET;
    pAbility->Half_10= (phyData4 & _10Base_T_R4_MASK) >> _10Base_T_R4_OFFSET;
    pAbility->Half_1000 = (phyData9 & _1000Base_THalfDuplex_MASK) >> _1000Base_THalfDuplex_OFFSET;
    pAbility->Full_1000 = (phyData9 & _1000Base_TFullDuplex_MASK) >> _1000Base_TFullDuplex_OFFSET;

    return ret;
} /* end of phy_8214c_autoNegoAbility_get */

/* Function Name:
 *      phy_8214c_autoNegoAbility_set
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
phy_8214c_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData0;
    uint32  phyData4;
    uint32  phyData9;
    rtk_enable_t     enable;

    phy_common_autoNegoEnable_get(unit, port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
    phyData4 = phyData4
            | (pAbility->FC << Pause_R4_OFFSET)
            | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
        return ret;

    phyData4 = phyData4 &
            ~(_100Base_TX_FD_R4_MASK | _100Base_TX_R4_MASK | _10Base_T_FD_R4_MASK | _10Base_T_R4_MASK);
    phyData4 = phyData4
            | (pAbility->Full_100 << _100Base_TX_FD_R4_OFFSET)
            | (pAbility->Half_100 << _100Base_TX_R4_OFFSET)
            | (pAbility->Full_10 << _10Base_T_FD_R4_OFFSET)
            | (pAbility->Half_10 << _10Base_T_R4_OFFSET);

    phyData9 = phyData9 & ~(_1000Base_TFullDuplex_MASK | _1000Base_THalfDuplex_MASK);
    phyData9 = phyData9 | (pAbility->Full_1000 << _1000Base_TFullDuplex_OFFSET)
               | (pAbility->Half_1000 << _1000Base_THalfDuplex_OFFSET);

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
        return ret;


    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, phyData9)) != RT_ERR_OK)
        return ret;

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == enable)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;
    }

    return ret;
} /* end of phy_8214c_autoNegoAbility_set */

/* Function Name:
 *      phy_8214c_speed_get
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
phy_8214c_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    int32   ret;
    uint32  val;
    uint32  phyData0;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    *pSpeed = ((phyData0 & SpeedSelection1_MASK) >> (SpeedSelection1_OFFSET -1))
              | ((phyData0 & SpeedSelection0_MASK) >> SpeedSelection0_OFFSET);

    return RT_ERR_OK;
} /* end of phy_8214c_speed_get */

/* Function Name:
 *      phy_8214c_speed_set
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
phy_8214c_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1)) | ((speed & 1) << SpeedSelection0_OFFSET));

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8214c_speed_set */


/* Function Name:
 *      phy_8214c_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The result unit is cm
 */
int32
phy_8214c_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, fixed_page;
    uint32  speed;

    /* Check the port is link up or not? */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
        return ret;

    phy_common_speed_get(unit, port, &speed);

    if ((phyData & LinkStatus_MASK) && speed != PORT_SPEED_10M)
    {
       /* If the port is link up,
            * return cable length from green function
           */
        fixed_page = 0xa88;

        /* The Length is store in [7:0], and the unit is meter*/
        if ((ret = hal_miim_read(unit, port, fixed_page, 16, &phyData)) != RT_ERR_OK)
            return ret;

        osal_memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));
        if (speed == PORT_SPEED_1000M) /* 1000M */
        {
            pRtctResult->linkType = PORT_SPEED_1000M;
            pRtctResult->un.ge_result.channelALen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelBLen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelCLen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelDLen = (phyData & 0x00FF)*100;
        }
        else /* 100M */
        {
            pRtctResult->linkType = PORT_SPEED_100M;
            pRtctResult->un.fe_result.rxLen = (phyData & 0x00FF)*100;
            pRtctResult->un.fe_result.txLen = (phyData & 0x00FF)*100;
        }
    }
    else
    {
        /* If the port is link down,
         * return cable length from RTCT function
         */
        /* Page 0xa42, Register 17
         * bit[15]: cable test finished or not?
         *             1: Finished
         *             0: Not finished
         */
         fixed_page = 0xa42;

         if ((ret = hal_miim_read(unit, port, fixed_page, 17, &phyData)) != RT_ERR_OK)
            return ret;

         if(((phyData >> RTCT_DONE) & 0x1) != 0x1)
            return RT_ERR_PHY_RTCT_NOT_FINISH;

        pRtctResult->linkType = PORT_SPEED_1000M;
        /* Length = (Index/64)*8ns*(0.2m/ns) = Index/80 (m) = (1.25) * Index (cm) */

        if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_A)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= 0x3FFF; /*[13:0] are valid*/
        pRtctResult->un.ge_result.channelALen = (phyData)*5/4;

        if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_B)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= 0x3FFF; /*[13:0] are valid*/
        pRtctResult->un.ge_result.channelBLen = (phyData)*5/4;

        if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_C)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= 0x3FFF; /*[13:0] are valid*/
        pRtctResult->un.ge_result.channelCLen = (phyData)*5/4;

        if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_D)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
            return ret;
        phyData &= 0x3FFF; /*[13:0] are valid*/
        pRtctResult->un.ge_result.channelDLen = (phyData)*5/4;

        /* === Channel A Status ===*/
        if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_A)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
            return ret;

        if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelAShort = 1;
        if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelAOpen = 1;
        if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelAMismatch = 1;
        if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelAMismatch |= 0x2;
        if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelALinedriver = 1;

        /* === Channel B Status ===*/
        if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_B)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
            return ret;

        if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelBShort = 1;
        if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelBOpen = 1;
        if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelBMismatch = 1;
        if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelBMismatch |= 0x2;
        if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelBLinedriver = 1;

        /* === Channel C Status ===*/
        if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_C)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
            return ret;

        if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelCShort = 1;
        if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelCOpen = 1;
        if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelCMismatch = 1;
        if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelCMismatch |= 0x2;
        if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelCLinedriver = 1;

        /* === Channel D Status ===*/
        if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_D)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
            return ret;

        if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelDShort = 1;
        if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelDOpen = 1;
        if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelDMismatch = 1;
        if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelDMismatch |= 0x2;
        if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
            pRtctResult->un.ge_result.channelDLinedriver = 1;
    }

    return ret;
} /* end of phy_8214c_rtctResult_get */

/* Function Name:
 *      phy_8214c_rtct_start
 * Description:
 *      Start PHY interface RTCT test of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not supported
 * Note:
 *      None
 */
int32
phy_8214c_rtct_start(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, fixed_page;
    uint32  speed;

    /* Check the port is link up or not? */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
        return ret;

    phy_common_speed_get(unit, port, &speed);


    if ((phyData & LinkStatus_MASK) && speed != PORT_SPEED_10M)
    {
        fixed_page = 0;

        /* get value from CHIP*/
        if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
            return ret;
        /* enable green feature */
        phyData |= (1 << 9);
        if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, fixed_page, 28, phyData)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        if ((ret = hal_miim_read(unit, port, 0xa42, 17, &phyData)) != RT_ERR_OK)
            return ret;

        phyData |= (1 << RTCT_ENABLE);
        phyData |= (1 << RTCT_CH_A);
        phyData |= (1 << RTCT_CH_B);
        phyData |= (1 << RTCT_CH_C);
        phyData |= (1 << RTCT_CH_D);

        if ((ret = hal_miim_write(unit, port, 0xa42, 17, phyData)) != RT_ERR_OK)
            return ret;
    }

    return ret;

} /* end of phy_8214c_rtct_start */

/* Function Name:
 *      phy_8214c_crossOverMode_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8214c_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, force_mode, mdi;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, 0xa43, 24, &phyData)) != RT_ERR_OK)
        return ret;

    if((phyData & (1 << 9)) != 0)
    {
        force_mode = 1;
        if((phyData & (1 << 8)) != 0)
            mdi = 1;
        else
            mdi = 0;
    }else{
        force_mode = 0;
    }

    if (force_mode)
    {
        if (mdi)
            *pMode = PORT_CROSSOVER_MODE_MDI;
        else
            *pMode = PORT_CROSSOVER_MODE_MDIX;
    }
    else
        *pMode = PORT_CROSSOVER_MODE_AUTO;

    return RT_ERR_OK;
} /* end of phy_8214c_crossOverMode_get */

/* Function Name:
 *      phy_8214c_crossOverMode_set
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
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8214c_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, 0xa43, 24, &phyData)) != RT_ERR_OK)
        return ret;

    switch (mode)
    {
        case PORT_CROSSOVER_MODE_AUTO:
            phyData &= ~(1 << 9);
            break;
        case PORT_CROSSOVER_MODE_MDI:
            phyData |= (1 << 9);
            phyData |= (1 << 8);
            break;
        case PORT_CROSSOVER_MODE_MDIX:
            phyData |= (1 << 9);
            phyData &= ~(1 << 8);
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = hal_miim_write(unit, port, 0xa43, 24, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;

} /* end of phy_8214c_crossOverMode_set */

/* Function Name:
 *      phy_8214c_crossOverStatus_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PHY_FIBER_LINKUP - This feature is not supported in this mode
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
int32
phy_8214c_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, 0xa43, 26, &phyData)) != RT_ERR_OK)
        return ret;

    if((phyData & (1 << 1)) != 0)
    {
        *pStatus = PORT_CROSSOVER_STATUS_MDI;
    }else{
        *pStatus = PORT_CROSSOVER_STATUS_MDIX;
    }

    return RT_ERR_OK;
} /* end of phy_8214c_crossOverStatus_get */

/* Function Name:
 *      phy_8214c_linkDownPowerSavingEnable_get
 * Description:
 *      Get the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of link-down power saving
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8214c is supported the per-port link-down power saving
 */
int32
phy_8214c_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;

    /* For Link-Down Power Saving (per-port) */
    if ((ret = hal_miim_read(unit, port, 0xa43, 24, &value)) != RT_ERR_OK)
        return ret;

    if (((value >> 2) & 0x1) == 0x1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
} /* end of phy_8214c_linkDownPowerSavingEnable_get */

/* Function Name:
 *      phy_8214c_linkDownPowerSavingEnable_set
 * Description:
 *      Set the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-down power saving
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8214c is supported the per-port link-down power saving
 */
int32
phy_8214c_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;

    /* For Link-Down Power Saving (per-port) */
    if ((ret = hal_miim_read(unit, port, 0xa43, 24, &value)) != RT_ERR_OK)
        return ret;

    value &= ~(0x1 << 2);
    if (ENABLED == enable)
    {
        value |= (0x1 << 2);
    }
    if ((ret = hal_miim_write(unit, port, 0xa43, 24, value)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;

} /* end of phy_8214c_linkDownPowerSavingEnable_set */

/* Function Name:
 *      phy_8214c_gigaLiteEnable_get
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
 *      1. The RTL8214c is supported the per-port Giga Lite feature.
 */
int32
phy_8214c_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;

    /* For Giga Lite (per-port) */
    if ((ret = hal_miim_read(unit, port, GIGA_LITE_CTRL_REG, 20, &value)) != RT_ERR_OK)
        return ret;

    if (((value >> 9) & 0x1) == 0x1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;

} /* end of phy_8214c_gigaLiteEnable_get */

/* Function Name:
 *      phy_8214c_gigaLiteEnable_set
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
 *      1. The RTL8214c is supported the per-port Giga Lite feature.
 */
int32
phy_8214c_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;


    /* Configure the GPHY page to copper */
    if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 29, 0x0001)) != RT_ERR_OK)
        return ret;

    /* bit[2]: configure 2-pair auto-downspeed */
    /* bit[9]: configure retry speed down to 500M */
    if ((ret = hal_miim_read(unit, port, 0xa44, 17, &value)) != RT_ERR_OK)
        return ret;

    value &= ~(0x1 << 9);
    value &= ~(0x1 << 2);
    if (ENABLED == enable)
    {
        value |= (0x1 << 9);
        value |= (0x1 << 2);
    }
    if ((ret = hal_miim_write(unit, port, 0xa44, 17, value)) != RT_ERR_OK)
        return ret;


    /* For Giga Lite (per-port) */
    if ((ret = hal_miim_read(unit, port, GIGA_LITE_CTRL_REG, 20, &value)) != RT_ERR_OK)
        return ret;

    value &= ~(0x1 << 9);
    if (ENABLED == enable)
    {
        value |= (0x1 << 9);
    }

    if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 20, value)) != RT_ERR_OK)
        return ret;

    /* Do the restart N-WAY */
    if ((ret = hal_miim_read(unit, port, 0, 0, &value)) != RT_ERR_OK)
        return ret;
    value |= (0x1UL<<9);
    if ((ret = hal_miim_write(unit, port, 0, 0, value)) != RT_ERR_OK)
        return ret;


    /* Configure the GPHY page to auto */
    if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 29, 0x0000)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of phy_8214c_gigaLiteEnable_set */


/* Function Name:
 *      phy_8214c_media_get
 * Description:
 *      Get PHY 8214C media type.
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
 *      The media type is return PORT_MEDIA_COPPER.
 */
int32
phy_8214c_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    *pMedia = PORT_MEDIA_COPPER;
    return RT_ERR_OK;
} /* end of phy_8214c_media_get */

/* Function Name:
 *      phy_8214c_media_set
 * Description:
 *      Set PHY 8214C media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media type
 * Output:
 *      None
 * Return:
 *      RT_ERR_CHIP_NOT_SUPPORTED - RTL8214C doesn't support media set
 * Note:
 *      (1) media type is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 */
int32
phy_8214c_media_set(uint32 unit, rtk_port_t port, rtk_port_media_t media)
{

    return RT_ERR_CHIP_NOT_SUPPORTED;

} /* end of phy_8214c_media_set */

/* Function Name:
 *      phy_8214c_auto_1000f_get
 * Description:
 *      Get PHY 8214C copper 1000f ability from shadow.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pAbility - pointer to copper 1000f ability
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. copper 1000f ability value is as following:
 *      - 0: not the ability
 *      - 1: have the ability
 */
int32
phy_8214c_auto_1000f_get(uint32 unit, rtk_port_t port, uint32 *pAbility)
{
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPhy_info[unit]), RT_ERR_NULL_POINTER);

    *pAbility = (*pPhy_info[unit]).auto_1000f[port];
    return RT_ERR_OK;
} /* end of phy_8214c_auto_1000f_get */

/* Function Name:
 *      phy_8214c_auto_1000f_set
 * Description:
 *      Set PHY 8214C copper 1000f ability to shadow.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      ability  - copper 1000f ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. copper 1000f ability value is as following:
 *      - 0: not the ability
 *      - 1: have the ability
 */
int32
phy_8214c_auto_1000f_set(uint32 unit, rtk_port_t port, uint32 ability)
{
    RT_PARAM_CHK((ability != 0) && (ability != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPhy_info[unit]), RT_ERR_NULL_POINTER);

    (*pPhy_info[unit]).auto_1000f[port] = ability;
    return RT_ERR_OK;
} /* end of phy_8214c_auto_1000f_set */

/* Function Name:
 *      phy_8214c_eeepEnable_get
 * Description:
 *      Get enable status of EEEP function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEEP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8214c_eeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    /* Need to Implement */
    return RT_ERR_FAILED;
} /* end of phy_8214c_eeepEnable_get */

/* Function Name:
 *      phy_8214c_eeepEnable_set
 * Description:
 *      Set enable status of EEEP function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEEP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8214c_eeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    /* Need to Implement */
    return RT_ERR_FAILED;
} /* end of phy_8214c_eeepEnable_set */

/* Function Name:
 *      phy_8214c_utpDownSpeedEnable_get
 * Description:
 *      Get UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of UTP down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8214c_utpDownSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_read(unit, port, 0xa44, 17, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & (1 << 3))
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;

}/* phy_8214c_utpDownSpeedEnable_get */

/* Function Name:
 *      phy_8214c_utpDownSpeedEnable_set
 * Description:
 *      Set UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of UTP down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8214c_utpDownSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_read(unit, port, 0xa44, 17, &phyData)) != RT_ERR_OK)
        return ret;

    /* DownSpeed to 100M*/
    phyData &= ~(1 << 5);

    if (ENABLED == enable)
        phyData |= (1 << 3);
    else
        phyData &= ~(1 << 3);

    if ((ret = hal_miim_write(unit, port, 0xa44, 17, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* phy_8214c_utpDownSpeedEnable_set */

/* Function Name:
 *      phy_8214c_downSpeedEnable_get
 * Description:
 *      Get UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8214c_downSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    int32   ret;

    ret = phy_8214c_utpDownSpeedEnable_get(unit, port, pEnable);

    return ret;
} /* phy_8214c_downSpeedEnable_get */

/* Function Name:
 *      phy_8214c_downSpeedEnable_set
 * Description:
 *      Set UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8214c_downSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    int32   ret;

    ret = phy_8214c_utpDownSpeedEnable_set(unit, port, enable);

    return ret;
} /* phy_8214c_downSpeedEnable_set */

/* Function Name:
 *      phy_8214c_patch_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8214c_patch_set(uint32 unit, rtk_port_t port)
{
    int32 ret;
    ret = sub_phy_8214c_patch_set(unit, port);
    return ret;
} /* end of phy_8214fc_mp_patch_set */

#endif /* !defined(__BOOTLOADER__) */


