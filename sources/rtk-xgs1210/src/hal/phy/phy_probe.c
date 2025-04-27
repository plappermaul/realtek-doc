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
 * $Revision: 98236 $
 * $Date: 2019-06-27 13:26:44 +0800 (Thu, 27 Jun 2019) $
 *
 * Purpose : PHY probe and init service APIs in the SDK.
 *
 * Feature : PHY probe and init service APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/chipdef/driver.h>
#include <hal/chipdef/chipdef.h>
#include <hal/common/halctrl.h>
#include <hal/mac/mac_probe.h>
#include <hal/phy/identify.h>
#include <hal/phy/phy_probe.h>
#include <hal/phy/phy_common.h>
#include <hwp/hw_profile.h>
#include <hwp/hwp_util.h>
#include <hal/mac/miim_common_drv.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Function Declaration
 */


/* Static Function Body */

#if !(defined(__MODEL_USER__) || defined(CONFIG_VIRTUAL_ARRAY_ONLY))

/* Function Name:
 *      phy_driver_hook
 * Description:
 *      Probe the external PHY chip in the specified chip.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
rt_phyctrl_t *
phy_driver_hook(uint32 unit, rtk_port_t port)
{
    rt_phyctrl_t    *pPhyctrl = NULL;
    hal_control_t   *pHalCtrl;

    if ((unit >= RTK_MAX_NUM_OF_UNIT) || (port >= RTK_MAX_PORT_PER_UNIT))
    {
        return NULL;
    }

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return NULL;

    if (HWP_USEDEFHWP())
    {
        pPhyctrl = phy_identify_driver_find_blindly(unit,port);
    }
    else
    {
        if (!HWP_PHY_EXIST(unit, port) && !HWP_SERDES_PORT(unit, port))
        {
            pHalCtrl->pPhy_ctrl[port] = NULL;
            RT_LOG(LOG_TRACE, MOD_HAL, "unit %u port %d HWP does not specify PHY nor SerDes-port.", unit, port);
            return NULL;
        }

        /* find PHY control */
        if (NULL == (pPhyctrl = phy_identify_find(unit, port)))
        {
            /* check upon all phy drivers to see if we can find one driver */
            pPhyctrl = phy_identify_driver_find_blindly(unit, port);
            if (pPhyctrl == NULL)
            {
                pHalCtrl->pPhy_ctrl[port] = NULL;
                RT_LOG(LOG_TRACE, MOD_HAL, "unit %u port %u probe PHY type %d driver not found.\n",
                   unit, port, HWP_PHY_MODEL_BY_PORT(unit, port));
                return NULL;
            }
        }
    }

    pHalCtrl->pPhy_ctrl[port] = pPhyctrl;
    RT_LOG(LOG_TRACE, MOD_HAL, "PHY driver probed on unit %u port %d", unit, port);

    return pPhyctrl;
} /* end of phy_driver_hook */
#endif

/* Function Name:
 *      _phy_probe_defaultProfilePort_update
 * Description:
 *      Updates defaul hardware profile according to probed PHY type.
 * Input:
 *      defaultHwp - default hardware profile
 *      unit - unit id
 *      port - port id
 *      phyIndex - for updating phy_idx of hardware profile port description
 *      phyBasePort - for updating mac_id of hardware profile phy description
 *      phyCtrl - probed PHY control block
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
void
_phy_probe_defaultProfilePort_update(hwp_hwProfile_t *defaultHwp, uint32 local_unit_id, uint32 port, int32 phyIndex, int32 phyBasePort, rt_phyctrl_t *phyCtrl)
{
    uint8       attr, medi, eth;

    attr = HWP_ETH;
    eth  = (uint8)phyCtrl->pPhyInfo->eth_type;

    switch (phyCtrl->phyType)
    {
      case RTK_PHYTYPE_RTL8212B:
      case RTK_PHYTYPE_RTL8214FB:
      case RTK_PHYTYPE_RTL8214FC:
      case RTK_PHYTYPE_RTL8218FB:
        if (phyCtrl->pPhyInfo->isComboPhy[(port-phyBasePort)])
            medi = HWP_COMBO;
        else
            medi = HWP_COPPER;
        break;
      case RTK_PHYTYPE_RTL8295R_C22:
      case RTK_PHYTYPE_RTL8295R:
      case RTK_PHYTYPE_RTL8214QF_NC5:
      case RTK_PHYTYPE_RTL8214QF:
        medi = HWP_FIBER;
        break;
      case RTK_PHYTYPE_SERDES:
        medi = HWP_SERDES;
        break;
      case RTK_PHYTYPE_RTL8214B:
      case RTK_PHYTYPE_RTL8214C:
      case RTK_PHYTYPE_RTL8218B:
      case RTK_PHYTYPE_RTL8218D_NMP:
      case RTK_PHYTYPE_RTL8218D:
      case RTK_PHYTYPE_RTL8208D:
      case RTK_PHYTYPE_RTL8208G:
      case RTK_PHYTYPE_RTL8208L:
      case RTK_PHYTYPE_RTL8208L_INT:
      case RTK_PHYTYPE_RTL8284:
        medi = HWP_COPPER;
        break;
      default:
        return;
    }

    hwp_defaultProfilePort_update(defaultHwp, local_unit_id, (uint8)port, attr, medi, eth, phyIndex);
}


/* Public Function Body */


/* Function Name:
 *      _phy_probe_unit
 * Description:
 *      Probe the PHY chip ports in the specified chip.
 * Input:
 *      unit            - unit ID
 *      hwp             - hardware profile of that board
 *      local_unit_id   - local unit ID of this hwp
 * Output:
 *      need_update_hwp - TRUE: hardware profile is updated due to PHY on the board does not match the profile;
 *                        FALSE: hardware profile is not changed.
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
static int32
_phy_probe_unit(uint32 unit, hwp_hwProfile_t *hwp, uint32 local_unit_id, uint32 *need_update_hwp)
{
    uint32          port=0;
    uint32          phyHwpUpCnt = 0;
    rtk_portmask_t  phyProbeFailPmsk;
    rtk_portmask_t  phyUpdatePmsk;
    rt_phyctrl_t    *phyCtrl;
    hal_control_t   *pHalCtrl;
    uint32 phy_total_port=0, TBD_basePort=0, TBD_phyIdx=0;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    RTK_PORTMASK_RESET(phyProbeFailPmsk);
    RTK_PORTMASK_RESET(phyUpdatePmsk);
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((phyCtrl =  phy_driver_hook(unit, port)) == NULL)
        {
            if (!HWP_USEDEFHWP())
            {
                RTK_PORTMASK_PORT_SET(phyProbeFailPmsk, port);
            }
        }
        else
        {
            if (HWP_USEDEFHWP())
            {
                if (phy_total_port == 0)
                {
                    TBD_basePort = port;
                }

                phy_total_port++;

                _phy_probe_defaultProfilePort_update(hwp, local_unit_id, port, TBD_phyIdx, TBD_basePort, phyCtrl);
                hwp_defaultProfilePhy_build(hwp, port, phyCtrl, TBD_basePort, TBD_phyIdx);

                if (phy_total_port >= phyCtrl->pPhyInfo->phy_num)
                {
                    TBD_phyIdx++;
                    phy_total_port=0;
                }
            }
            else
            {
                if ((HWP_PHY_MODEL_BY_PORT(unit, port) != phyCtrl->phyType) && (HWP_PHY_BASE_MACID(unit, port) == port))
                {
                    RTK_PORTMASK_PORT_SET(phyUpdatePmsk, port);
                }
            }
        }
    }/* end HWP_PORT_TRAVS_EXCEPT_CPU */


    if (!RTK_PORTMASK_IS_ALL_ZERO(phyUpdatePmsk))
    {
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
        {
            if (RTK_PORTMASK_IS_PORT_SET(phyUpdatePmsk, port))
            {
                RT_LOG(LOG_INFO, MOD_HAL, "unit %u port %u update PHY type to HWP", unit, port);
                phyHwpUpCnt += hwp_profilePhy_update(unit, port, pHalCtrl->pPhy_ctrl[port]->phyType);
            }
        }

    }

    if (!RTK_PORTMASK_IS_ALL_ZERO(phyProbeFailPmsk))
    {
        RT_LOG(LOG_INFO, MOD_HAL, "unit %u port %u delete PHY to HWP", unit, port);
        phyHwpUpCnt += hwp_profilePhy_del(unit, &phyProbeFailPmsk);
    }

    if (phyHwpUpCnt == 0)
    {
        *need_update_hwp = FALSE;
    }
    else
    {
        *need_update_hwp = TRUE;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_probe
 * Description:
 *      Probe the PHY chip in the specified chip.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_probe(uint32 unit)
{
    uint32  need_hwp_update = FALSE;

    /* Probe PHY */
    RT_INIT_MSG("  PHY probe (unit %u)\n",unit);

    _phy_probe_unit(unit, hwp_myHwProfile, unit, &need_hwp_update);

    if (HWP_USEDEFHWP() || (need_hwp_update > 0))
    {
        hwp_parsedInfo_buildup((hwp_parsedInfo_t **)&hwp_myHwProfile->parsed_info, hwp_myHwProfile);
        if (HWP_UNIT_VALID_LOCAL(unit))
        {
            hwp_chipInfo_update(unit);
        }
    }

    return RT_ERR_OK;

} /* end of phy_probe */


/* Function Name:
 *      phy_attach_hook_driver
 * Description:
 *      Hook PHY driver according to PHY type
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_attach_hook_driver(uint32 unit)
{
    rt_phyctrl_t    *pPhyctrl = NULL;
    hal_control_t   *pHalCtrl;
    uint32          port;
    phy_type_t      phyType;

    if (unit >= RTK_MAX_NUM_OF_UNIT)
    {
        return RT_ERR_UNIT_ID;
    }
    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_NULL_POINTER;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        pHalCtrl->pPhy_ctrl[port] = NULL;
        if ((phyType = HWP_PHY_MODEL_BY_PORT(unit, port)) >= RTK_PHYTYPE_END)
            continue;

        if (phyType != RTK_PHYTYPE_SERDES)
            pPhyctrl = phy_identify_driver_find_by_phyType(phyType);
        else
            pPhyctrl = phy_identify_serdesDrvCtrl_get(unit, port);

        pHalCtrl->pPhy_ctrl[port] = pPhyctrl;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_init
 * Description:
 *      Init the PHY chip in the specified chip.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_init(uint32 unit)
{
    uint32          port;
    rt_phyctrl_t    *pPhyctrl;
    hal_control_t   *pHalCtrl;

    RT_INIT_MSG("  PHY init (unit %u)\n",unit);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    HWP_PORT_TRAVS(unit,port)
    {
        if ((pPhyctrl = pHalCtrl->pPhy_ctrl[port]) == NULL)
            continue;

        /* init */
        if (NULL != pPhyctrl->pPhydrv->fPhydrv_init)
        {
            pPhyctrl->pPhydrv->fPhydrv_init(unit, port);
            RT_LOG(LOG_INFO, MOD_HAL, "PHY driver init on unit %u port %u", unit, port);
        }
    }

    return RT_ERR_OK;
} /* end of phy_init */


/* Function Name:
 *      _phy_c22_check
 * Description:
 *      Use Clause 22 MDC/MDIO protocol to read from PHY checking if PHY supports C22
 * Input:
 *      unit - unit id
 *      port - port number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
_phy_c22_check(uint32 unit, uint32 port)
{
    uint32  data = 0;
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);
    RT_PARAM_CHK((NULL == MACDRV(pHalCtrl)->fMdrv_miim_read), RT_ERR_FAILED);

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, 0, PHY_IDENTIFIER_2_REG, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    if (data != 0)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_FAILED;
    }
}

/* Function Name:
 *      _phy_c45_check
 * Description:
 *      Use Clause 45 MDC/MDIO protocol to read from PHY checking if PHY supports C45
 * Input:
 *      unit - unit id
 *      port - port number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
_phy_c45_check(uint32 unit, uint32 port)
{
    uint32      data;
    int32       ret;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);
    RT_PARAM_CHK((NULL == MACDRV(pHalCtrl)->fMdrv_miim_read), RT_ERR_FAILED);

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 1, 2, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    if (data != 0)
    {
        return RT_ERR_OK;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 3, 2, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    if (data != 0)
    {
        return RT_ERR_OK;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 7, 2, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    if (data != 0)
    {
        return RT_ERR_OK;
    }

#if defined(CONFIG_SDK_RTL8226)
    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8226)
    {
        /* Workaround of 8226 NIC Card: set reg for mmd 1/3/7 reg 2,3. */
        if ((!osal_strcmp(HWP_IDENTIFIER_NAME(), "RTL9301_3x8218D_2x8226CARD_2XGE")) \
            || (!osal_strcmp(HWP_IDENTIFIER_NAME(), "RTL9303_8X8226")))
        {
            osal_printf("    unit=%u port=%u RTL8226 workaround for MMD 1/3/1 reg 2,3\n", unit, port);
            data = 0x001C;
            ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_write(unit, port, 31, 0xA6BC, 0x001C);
            data = 0xC838;
            ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_write(unit, port, 31, 0xA6BE, 0xC838);
            return RT_ERR_OK;
        }
    }
#endif

    return RT_ERR_FAILED;
}


/* Function Name:
 *      phy_mdcProto_probe
 * Description:
 *      Probe PHY's MDC/MDIO protocol and set MAC SMI bus to match
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_mdcProto_probe(uint32 unit)
{
    uint32  port, smi_id;
    uint32  is_c22, is_c45;
    drv_smi_mdxProtoSel_t   org_mdxProto, mdxProto;

#if 1 /* Temp skip 9310 for 8218D probe fail problem. SST-664, FIXME */
    if (HWP_9310_FAMILY_ID(unit))
    {
        return RT_ERR_OK;
    }
#endif

    RT_INIT_MSG("  SMI protocol probe (unit %u)\n",unit);
    HWP_PORT_TRAVS(unit, port)
    {
        if (port != HWP_PHY_BASE_MACID(unit, port))
        {
            continue;
        }

        if ((smi_id = HWP_PORT_SMI(unit, port)) == HWP_NONE)
        {
            continue;
        }

        is_c22 = FALSE;
        is_c45 = FALSE;
        if (hal_miim_portSmiMdxProto_get(unit, port, &org_mdxProto) != RT_ERR_OK)
        {
            continue;
        }

        if (org_mdxProto == DRV_SMI_MDX_PROTO_C22)
        {
            if (_phy_c22_check(unit, port) == RT_ERR_OK)
            {
                is_c22 = TRUE;
            }
        }
        else
        {
            if (_phy_c45_check(unit, port) == RT_ERR_OK)
            {
                is_c45 = TRUE;
            }
        }

        mdxProto = (org_mdxProto == DRV_SMI_MDX_PROTO_C22) ? DRV_SMI_MDX_PROTO_C45 : DRV_SMI_MDX_PROTO_C22;
        if (hal_miim_portSmiMdxProto_set(unit, port, mdxProto) != RT_ERR_OK)
        {
            /* skip process if SMI is not able to change the protocol */
            continue;
        }

        if (mdxProto == DRV_SMI_MDX_PROTO_C22)
        {
            if (_phy_c22_check(unit, port) == RT_ERR_OK)
            {
                is_c22 = TRUE;
            }
        }
        else
        {
            if (_phy_c45_check(unit, port) == RT_ERR_OK)
            {
                is_c45 = TRUE;
            }
        }

        RT_LOG(LOG_DEBUG, (MOD_PHY), "unit=%u port=%u smi=%u C45=%u C22=%u", unit, port, smi_id, is_c45, is_c22);
        if (is_c45 == TRUE)
        {
            hal_miim_portSmiMdxProto_set(unit, port, DRV_SMI_MDX_PROTO_C45);
            /* C45 has higher priority to use */
            continue;
        }
        else if (is_c22 == TRUE)
        {
            hal_miim_portSmiMdxProto_set(unit, port, DRV_SMI_MDX_PROTO_C22);
            continue;
        }
        else
        {
            /* recover back to origin mode */
            hal_miim_portSmiMdxProto_set(unit, port, org_mdxProto);
        }

    } /* end HWP_PORT_TRAVS */

    return RT_ERR_OK;
}


