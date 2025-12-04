/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
#include <hal/chipdef/chip.h>
#include <hal/common/halctrl.h>
#include <hal/mac/mac_probe.h>
#include <hal/phy/identify.h>
#include <hal/phy/phy_probe.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Function Declaration
 */
static int32 phy_probe_ext(rtk_port_t port);
static int32 phy_probe_int(rtk_port_t port);

/* Static Function Body */

/* Function Name:
 *      phy_probe_ext
 * Description:
 *      Probe the external PHY chip in the specified chip.
 * Input:
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
static int32
phy_probe_ext(rtk_port_t port)
{
    rt_phyctrl_t    *pPhyctrl = NULL;

    if (!HAL_IS_PORT_EXIST(port) || !HAL_IS_RGMII_PORT(port))
    {
        if(port < RTK_MAX_NUM_OF_PORTS)
            hal_ctrl.pPhy_ctrl[port] = NULL;

        RT_DBG(LOG_TRACE, MOD_HAL, "PHY external driver not probed (port %d)", port);
        return RT_ERR_OK;
    }

    /* Start phy_ctrl struct finding process */
    if (NULL == (pPhyctrl = phy_identify_find(port)))
    {
        hal_ctrl.pPhy_ctrl[port] = NULL;
        RT_DBG(LOG_TRACE, MOD_HAL, "phy_identify_find(port %d) failed!!", port);
        return RT_ERR_OK;
    }

    hal_ctrl.pPhy_ctrl[port] = pPhyctrl;
    RT_DBG(LOG_TRACE, MOD_HAL, "PHY driver probed (port %d)", port);

    return RT_ERR_OK;
} /* end of phy_probe_ext */

/* Function Name:
 *      phy_probe_int
 * Description:
 *      Probe the internal PHY chip in the specified chip.
 * Input:
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
static int32
phy_probe_int(rtk_port_t port)
{
    rt_phyctrl_t    *pPhyctrl;

    pPhyctrl = hal_ctrl.pPhy_ctrl[port];
    if (NULL != pPhyctrl)
        return RT_ERR_OK; /* exist external PHY already */

    if (!HAL_IS_PORT_EXIST(port) || HAL_IS_CPU_PORT(port) || HAL_IS_RGMII_PORT(port) || (HAL_IS_SERDES_PORT(port) && !HAL_IS_ETHER_PORT(port)))
    {
        if(port < RTK_MAX_NUM_OF_PORTS)
            hal_ctrl.pPhy_ctrl[port] = NULL;

        RT_DBG(LOG_TRACE, MOD_HAL, "PHY internal driver not probed (port %d)", port);
        return RT_ERR_OK;
    }

    if (NULL == (pPhyctrl = phy_identify_int_find(port)))
    {
        hal_ctrl.pPhy_ctrl[port] = NULL;
        RT_DBG(LOG_TRACE, MOD_HAL, "phy_identify_int_find(port %d) failed!!", port);
        return RT_ERR_OK;
    }

    hal_ctrl.pPhy_ctrl[port] = pPhyctrl;
    RT_DBG(LOG_TRACE, MOD_HAL, "PHY driver probed (port %d)", port);


    return RT_ERR_OK;
} /* end of phy_probe_int */

/* Public Function Body */

/* Function Name:
 *      phy_probe
 * Description:
 *      Probe the PHY chip in the specified chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_probe(void)
{
    uint32  i, min_port, max_port;
    int32   ret = RT_ERR_FAILED;

    /* Probe PHY */
    min_port = HAL_GET_MIN_PORT();
    max_port = HAL_GET_MAX_PORT();

    for (i = min_port; i < max_port; i++)
    {
        if ((ret = phy_probe_ext(i)) != RT_ERR_OK)
        {
            RT_LOG(LOG_TRACE, MOD_HAL, "PHY external probed (port %d)", i);
        }

        if ((ret = phy_probe_int(i)) != RT_ERR_OK)
        {
            RT_LOG(LOG_TRACE, MOD_HAL, "PHY internal probed (port %d)", i);
        }
    }

    return RT_ERR_OK;
} /* end of phy_probe */

/* Function Name:
 *      phy_init
 * Description:
 *      Init the PHY chip in the specified chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_init(void)
{
    uint32  i;

    for (i = HAL_GET_MIN_PORT(); i <= HAL_GET_MAX_PORT(); i++)
    {
        if ((NULL != hal_ctrl.pPhy_ctrl[i]) &&
            (NULL != hal_ctrl.pPhy_ctrl[i]->pPhydrv->fPhydrv_init))
        {
            hal_ctrl.pPhy_ctrl[i]->pPhydrv->fPhydrv_init(i);
            RT_LOG(LOG_INFO, MOD_HAL, "PHY driver init (port %d)", i);
        }
    }

    return RT_ERR_OK;
} /* end of phy_init */
