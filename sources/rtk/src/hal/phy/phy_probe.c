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
 *      ppPhyctrl - PHY driver pointer
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_driver_hook(uint32 unit, rtk_port_t port, rt_phyctrl_t **ppPhyctrl)
{
    rt_phyctrl_t    *pPhyctrl = NULL;
    hal_control_t   *pHalCtrl;

    *ppPhyctrl = NULL;

    if ((unit >= RTK_MAX_NUM_OF_UNIT) || (port >= RTK_MAX_PORT_PER_UNIT))
    {
        return RT_ERR_FAILED;
    }

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

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

            return RT_ERR_OK;
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
                return RT_ERR_FAILED;
            }
        }
    }

    pHalCtrl->pPhy_ctrl[port] = pPhyctrl;
    RT_LOG(LOG_TRACE, MOD_HAL, "PHY driver probed on unit %u port %d", unit, port);

    *ppPhyctrl = pPhyctrl;
    return RT_ERR_OK;
} /* end of phy_driver_hook */
#endif


#ifdef RTK_HWP_DEFAULT_PROFILE
  #ifndef PHY_ONLY
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
      case RTK_PHYTYPE_RTL8224QF:
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
      case RTK_PHYTYPE_RTL8226:
      case RTK_PHYTYPE_RTL8226B:
      case RTK_PHYTYPE_RTL826X:
        medi = HWP_COPPER;
        break;
      default:
        return;
    }

    hwp_defaultProfilePort_update(defaultHwp, local_unit_id, (uint8)port, attr, medi, eth, phyIndex);
}
  #endif /* PHY_ONLY */
#endif

/* Public Function Body */

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
    int32           ret;
    uint32          port=0;
    rtk_portmask_t  phyProbeFailPmsk;
    rtk_portmask_t  phyUpdatePmsk;
    rt_phyctrl_t    *phyCtrl;
    hal_control_t   *pHalCtrl;
#ifdef RTK_HWP_DEFAULT_PROFILE
  #ifndef PHY_ONLY
    uint32 phy_total_port=0, TBD_basePort=0, TBD_phyIdx=0;
  #endif
#endif
    uint32  update_hwp = FALSE;

    /* Probe PHY */
    RT_INIT_MSG("  PHY probe (unit %u)\n",unit);


    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    RTK_PORTMASK_RESET(phyProbeFailPmsk);
    RTK_PORTMASK_RESET(phyUpdatePmsk);
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        ret = phy_driver_hook(unit, port, &phyCtrl);
        if (ret != RT_ERR_OK)
        {
            RTK_PORTMASK_PORT_SET(phyProbeFailPmsk, port);
        }

        if (phyCtrl != NULL)
        {
#ifdef RTK_HWP_DEFAULT_PROFILE
  #ifndef PHY_ONLY
            if (HWP_USEDEFHWP())
            {
                if (phy_total_port == 0)
                {
                    TBD_basePort = port;
                }

                phy_total_port++;

                _phy_probe_defaultProfilePort_update(hwp_myHwProfile, local_unit_id, port, TBD_phyIdx, TBD_basePort, phyCtrl);
                hwp_defaultProfilePhy_build(hwp_myHwProfile, port, phyCtrl, TBD_basePort, TBD_phyIdx);
                update_hwp = TRUE;

                if (phy_total_port >= phyCtrl->pPhyInfo->phy_num)
                {
                    TBD_phyIdx++;
                    phy_total_port=0;
                }

            }
            else
  #endif /* PHY_ONLY */
#endif
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
                hwp_profilePhy_update(unit, port, pHalCtrl->pPhy_ctrl[port]->phyType);
                update_hwp = TRUE;
            }
        }
    }

    if (!RTK_PORTMASK_IS_ALL_ZERO(phyProbeFailPmsk))
    {
        RT_LOG(LOG_INFO, MOD_HAL, "unit %u port %u delete PHY to HWP", unit, port);
        hwp_profilePhy_del(unit, &phyProbeFailPmsk);
        update_hwp = TRUE;
    }

    /* do profile rebuild */
    if (update_hwp == TRUE)
    {
        hwp_parsedInfo_buildup((hwp_parsedInfo_t **)&hwp_myHwProfile->parsed_info, hwp_myHwProfile);
#ifndef PHY_ONLY
        if (HWP_UNIT_VALID_LOCAL(unit))
        {
            hwp_chipInfo_update(unit);
        }
#endif
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



