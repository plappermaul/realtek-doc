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
 * Purpose : Hardware Abstraction Layer (HAL) control structure and definition in the SDK.
 *
 * Feature : HAL control structure and definition
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <osal/lib.h>
#include <hal/common/halctrl.h>
#include <hal/mac/mac_probe.h>
#include <hal/phy/phy_probe.h>
#include <dal/dal_mgmt.h>
#include <dal/dal_construct.h>
#include <hwp/hw_profile.h>
#include <hal/phy/identify.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
hal_control_t hal_ctrl[RTK_MAX_NUM_OF_UNIT];
rtk_portmask_t rtk_hal_stack_ports[RTK_MAX_NUM_OF_UNIT];
static uint32 hal_init_sys_status = INIT_NOT_COMPLETED;


/*
 * Function Declaration
 */
void hal_show_portmask(rtk_portmask_t  *portmask)
{
    int i,j;
    uint32 tmp;
    for(i=RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST-1;i>=0;i--)
    {
        osal_printf("      [%d]:",i);
        tmp = portmask->bits[i];
        for(j=BITMAP_WIDTH-1;j>=0;j--)
        {
            osal_printf("%d",(tmp&(0x1<<j))?1:0);
        }
        osal_printf("\n");
    }
}

void hal_debug_show_info(uint32 unit, uint32 sequenceNo)
{
    int i,port;

    osal_printf("+------(%u)---halctrl ctrl(unit %u)-----------------\n",sequenceNo,unit);
    osal_printf("|    chip_flags=0x%08X\n",       hal_ctrl[unit].chip_flags);
    osal_printf("|    *pChip_driver=0x%08X\n",    (uint32)(uintptr)hal_ctrl[unit].pChip_driver);
    osal_printf("|        type=%d\n",             hal_ctrl[unit].pChip_driver->type);
    osal_printf("|        driver_id=0x%08X\n",    hal_ctrl[unit].pChip_driver->driver_id);
    osal_printf("|        driver_rev_id=0x%08X\n",hal_ctrl[unit].pChip_driver->driver_rev_id);
    osal_printf("|        pReg_list=0x%08X\n",    (uint32)(uintptr)hal_ctrl[unit].pChip_driver->pReg_list);
    osal_printf("|        pPortmask_list=\n");
    hal_show_portmask(hal_ctrl[unit].pChip_driver->pPortmask_list);
    for(i=RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST-1;i>=0;i--)

    osal_printf("|    *pDev_info=0x%08X\n",    (uint32)(uintptr)hal_ctrl[unit].pDev_info);
    osal_printf("|        chip_id=0x%08X\n",hal_ctrl[unit].pDev_info->chip_id);
    osal_printf("|        chip_rev_id=0x%08X\n",hal_ctrl[unit].pDev_info->chip_rev_id);
    osal_printf("|        driver_id=0x%08X\n",hal_ctrl[unit].pDev_info->driver_id);
    osal_printf("|        driver_rev_id=0x%08X\n",hal_ctrl[unit].pDev_info->driver_rev_id);
    osal_printf("|        chip_aflags=0x%08X\n",hal_ctrl[unit].pDev_info->chip_aflags);
    osal_printf("|        chip_family_id=0x%08X\n",hal_ctrl[unit].pDev_info->chip_family_id);

    HWP_PORT_TRAVS(unit,port)
        osal_printf("    *pPhy_ctrl[%d]=0x%08X\n", port, (uint32)(uintptr)hal_ctrl[unit].pPhy_ctrl[port]);
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PHY_EXIST(unit, port))
        {
            osal_printf("    phydrv_index(%d)   %d\n", port, (uint32)hal_ctrl[unit].pPhy_ctrl[port]->pPhydrv->phydrv_index);
        }
    }
    osal_printf("+---------------------------------------------------\n");

}


/* Function Name:
 *      hal_init_sys
 * Description:
 *      Initialize the system part of hal layer. The system part will initial only once.
 *      (unit part will initial per unit)
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - General Error
 * Note:
 */
int32
hal_init_sys(void)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    if(hal_init_sys_status == INIT_COMPLETED) //init only once
        return RT_ERR_OK;

    for(unit=0;unit<RTK_MAX_NUM_OF_UNIT;unit++)
        osal_memset((uint8 *)&hal_ctrl[unit], 0, sizeof(hal_control_t));

#ifndef __BOOTLOADER__
    for (unit = 0; unit < RTK_MAX_NUM_OF_UNIT; unit++)
    {
        int i;

        /* Create semaphores for HAL protection mechanism */
        hal_ctrl[unit].reg_sem = osal_sem_mutex_create();
        if (0 == hal_ctrl[unit].reg_sem)
        {
            RT_INIT_ERR(RT_ERR_FAILED, MOD_HAL, "register semaphore create failed!!\n");
            return RT_ERR_FAILED;
        }

        for (i = 0; i < RTK_INDIRECT_CTRL_GROUP_END; i++)
        {
            hal_ctrl[unit].tbl_sem[i] = osal_sem_mutex_create();
            if (0 == hal_ctrl[unit].tbl_sem[i])
            {
                RT_INIT_ERR(RT_ERR_FAILED, MOD_HAL, "table semaphore %d create failed!!\n", i);
                return RT_ERR_FAILED;
            }
        }

        hal_ctrl[unit].phy_sem = osal_sem_mutex_create();
        if (0 == hal_ctrl[unit].phy_sem)
        {
            RT_INIT_ERR(RT_ERR_FAILED, MOD_HAL, "PHY semaphore create failed!!\n");
            return RT_ERR_FAILED;
        }

        hal_ctrl[unit].miim_sem = osal_sem_mutex_create();
        if (0 == hal_ctrl[unit].miim_sem)
        {
            RT_INIT_ERR(RT_ERR_FAILED, MOD_HAL, "MIIM semaphore create failed!!\n");
            return RT_ERR_FAILED;
        }

        hal_ctrl[unit].sds_sem = osal_sem_mutex_create();
        if (0 == hal_ctrl[unit].sds_sem)
        {
            RT_INIT_ERR(RT_ERR_FAILED, MOD_HAL, "Serdes semaphore create failed!!\n");
            return RT_ERR_FAILED;
        }

        osal_memset(&rtk_hal_stack_ports[unit], 0, sizeof(rtk_portmask_t));
    }
#endif //__BOOTLOADER__

    /* ini identify database */
    phy_identify_init();


    /* Initialize the dal layer */
    if ((ret = dal_mgmt_init()) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_INIT, "dal_mgmt_init failed!!\n");
        return ret;
    }
    RT_LOG(LOG_EVENT, MOD_INIT, "dal_mgmt_init Completed!!\n");


    hal_init_sys_status = INIT_COMPLETED;
    return RT_ERR_OK;
}


/* Function Name:
 *      hal_chipInfo_clear
 * Description:
 *      Clear chip related info and driver
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - General Error
 * Note:
 */
int32
hal_chipInfo_clear(uint32 unit)
{
    int32   port;

    if (hal_ctrl[unit].chip_flags & HAL_CHIP_INITED)
    {
        hal_ctrl[unit].pChip_driver = NULL;
        hal_ctrl[unit].pDev_info = NULL;
        for (port = 0; port < RTK_MAX_PORT_PER_UNIT; port++)
        {
            hal_ctrl[unit].pPhy_ctrl[port] = NULL;
        }

        hal_ctrl[unit].chip_flags &= ~HAL_CHIP_INITED;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      hal_init
 * Description:
 *      Initialize the hal layer API.
 * Input:
 *      unit    - unit ID
 *      devID   - device ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - General Error
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_DRIVER_NOT_FOUND - The driver can not found
 * Note:
 *      Initialize the hal layer API, include get the chip id and chip revision
 *      id, get its driver id and driver revision id, then bind to its major
 *      driver. Also initialize its hal_ctrl structure for this specified unit.
 *      Before calling the function, bsps should already scan HW interface, like
 *      PCI device in all buses, or physical Lextra, and the total chip numbers,
 *      its chip id and chip revision id is known and store in database in lower
 *      layer.
 */
int32
hal_init(uint32 unit, uint32 devID)
{
    int32   ret = RT_ERR_FAILED;

    if ((ret = hal_init_sys()) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_HAL, "hal_init_sys failed!!\n");
        return ret;
    }

    dal_mgmt_mapper_clear(unit);
    hal_chipInfo_clear(unit);

    /* Probe MAC */
    if ((ret = mac_probe(unit)) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_HAL, "mac_probe failed!!\n");
        return ret;
    }

    /* Init MAC */
    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        if ((ret = mac_init(unit)) != RT_ERR_OK)
        {
            RT_INIT_ERR(ret, MOD_HAL, "mac_init failed!!\n");
            return ret;
        }
    }

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        /* Probe PHY protocol */
        phy_mdcProto_probe(unit);

        /* Probe PHY */
        if ((ret = phy_probe(unit)) != RT_ERR_OK)
        {
            RT_INIT_ERR(ret, MOD_HAL, "phy_probe failed!!\n");
            return ret;
        }
    }
    else
    {
        /* hook driver */
        if ((ret = phy_attach_hook_driver(unit)) != RT_ERR_OK)
        {
            RT_INIT_ERR(ret, MOD_HAL, "phy_attach_hook_driver failed!!\n");
            return ret;
        }
    }

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        /* Construct PHY/MAC-Serdes */
        if ((ret = chip_construct(unit)) != RT_ERR_OK)
        {
            RT_INIT_ERR(ret, MOD_HAL, "chip_construct() failed!!\n");
            return ret;
        }
    }

    /* Init PHY */
    if ((ret = phy_init(unit)) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_HAL, "phy_init failed!!\n");
        return ret;
    }

    rtk_unit2devID[unit]=devID;
    rtk_dev2unitID[devID]=unit;

    return RT_ERR_OK;
} /* end of hal_init */

/* Function Name:
 *      hal_ctrlInfo_get
 * Description:
 *      Find the hal control information structure for this specified unit.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of hal control information structure that found
 * Note:
 *      The function have found the exactly hal control information structure.
 */
hal_control_t *
hal_ctrlInfo_get(uint32 unit)
{
    if (hal_ctrl[unit].chip_flags & HAL_CHIP_ATTACHED)
    {
        return &hal_ctrl[unit];
    }
    else
    {
        return NULL;
    }
} /* end of hal_ctrlInfo_get */

/* Function Name:
 *      hal_portMaxBandwidth_ret
 * Description:
 *      Get the max bandwith of port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      max bandwidth value
 * Note:
 *      The return value is different in FE/GE/10GE port.
 */
uint32
hal_portMaxBandwidth_ret(uint32 unit, rtk_port_t port)
{
    if (HWP_FE_PORT(unit, port))
        return (hal_ctrl[unit].pDev_info->pCapacityInfo->rate_of_bandwidth_max_fe_port);
    else if (HWP_GE_PORT(unit, port))
        return (hal_ctrl[unit].pDev_info->pCapacityInfo->rate_of_bandwidth_max_ge_port);
    else if (HWP_10GE_PORT(unit, port))
        return (hal_ctrl[unit].pDev_info->pCapacityInfo->rate_of_bandwidth_max_10ge_port);
    else
        return (hal_ctrl[unit].pDev_info->pCapacityInfo->rate_of_bandwidth_max);
} /* end of hal_portMaxBandwidth_ret */


/* Function Name:
 *      hal_stack_portmask_set
 * Description:
 *      Set stack port database
 * Input:
 *      unit - unit id
 *      pStkPorts - stack ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK - OK
 *      RT_ERR_FAILED - set fail
 * Note:
 *      None
 */
int32
hal_stack_portmask_set(uint32 unit, rtk_portmask_t *pStkPorts)
{
    if (unit >= RTK_MAX_NUM_OF_UNIT)
    {
        return RT_ERR_FAILED;
    }

    RTK_PORTMASK_ASSIGN(rtk_hal_stack_ports[unit], *pStkPorts);
    return RT_ERR_OK;
}

/* Function Name:
 *      hal_stack_portmask_get
 * Description:
 *      Set stack port database
 * Input:
 *      unit - unit id
 * Output:
 *      pPortmask - stack ports
 * Return:
 *      RT_ERR_OK - OK
 *      RT_ERR_FAILED - set fail
 * Note:
 *      None
 */
int32
hal_stack_portmask_get(uint32 unit, rtk_portmask_t *pPortmask)
{
    if (unit >= RTK_MAX_NUM_OF_UNIT)
    {
        return RT_ERR_FAILED;
    }

    RTK_PORTMASK_ASSIGN(*pPortmask, rtk_hal_stack_ports[unit]);
    return RT_ERR_OK;
}

/* Function Name:
 *      hal_stack_portmask_check
 * Description:
 *      Check portmask is all stack port
 * Input:
 *      unit - unit id
 *      pPortmask - portmask to check
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - ports in portmask are all stack ports
 *      RT_ERR_OK  - there are non-stacking port in the portmask
 * Note:
 *      None
 */
uint32
hal_stack_portmask_check(uint32 unit, rtk_portmask_t *pPortmask)
{
    rtk_portmask_t      pmskTmp;

    if (unit >= RTK_MAX_NUM_OF_UNIT)
    {
        return RT_ERR_FAILED;
    }

    RTK_PORTMASK_ASSIGN(pmskTmp, rtk_hal_stack_ports[unit]);
    RTK_PORTMASK_REVERT(pmskTmp);
    RTK_PORTMASK_AND(pmskTmp, *pPortmask);
    if (RTK_PORTMASK_IS_ALL_ZERO(pmskTmp))
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_FAILED;
    }

}

