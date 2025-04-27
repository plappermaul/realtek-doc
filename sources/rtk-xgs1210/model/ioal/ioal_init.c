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
 * $Revision: 92261 $
 * $Date: 2018-09-12 14:32:00 +0800 (Wed, 12 Sep 2018) $
 *
 * Purpose : IOAL Layer Init Module
 *
 * Feature : IOAL Init Functions
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <virtualmac/vmac_target.h>
#include <private/drv/swcore/chip_probe.h>


/*
 * Symbol Definition
 */
#define VIRT_SWCORE_REG_SIZE    (0x1000000 - 16*64*1024)     /*15M */
#define VIRTUAL_SWCORE_TBL_SIZE	(4 * 1024*1024)             /* 4M */

/*
 * Data Declaration
 */
int8 virtualSwReg[VIRT_SWCORE_REG_SIZE];
int8 virtualSwTable[VIRTUAL_SWCORE_TBL_SIZE];
int8 *pVirtualSWReg = (int8 *)0;
int8 *pVirtualSWTable = (int8 *)0;
uint32 swcore_base[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      ioal_init_memBase_get
 * Description:
 *      Get memory base address
 * Input:
 *      unit      - unit id
 *      mem       - memory region
 * Output:
 *      pBaseAddr - pointer to the base address of memory region
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
ioal_init_memRegion_get(uint32 unit, ioal_memRegion_t mem, uint32 *pBaseAddr)
{
    enum IC_TYPE ictype;

    vmac_getTarget(&ictype);

    switch (ictype)
    {
        case IC_TYPE_MODEL:
            if (IOAL_MEM_SWCORE == mem)
                *pBaseAddr = swcore_base[unit];
            else
                return RT_ERR_FAILED;
            break;
        case IC_TYPE_REAL:
            if (IOAL_MEM_SWCORE == mem)
                *pBaseAddr = SWCORE_VIRT_BASE;
            else if (IOAL_MEM_SOC == mem)
                *pBaseAddr = SOC_VIRT_BASE;
            else
                return RT_ERR_FAILED;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
} /* end of ioal_init_memBase_get */

/* Function Name:
 *      ioal_init
 * Description:
 *      Init SDK IOAL Layer
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
ioal_init(uint32 unit)
{
    pVirtualSWReg = virtualSwReg;
    swcore_base[unit] = (uint32)pVirtualSWReg;
    pVirtualSWTable = virtualSwTable;

    drv_swcore_ioalCB_register(ioal_mem32_read,ioal_mem32_write);

    return RT_ERR_OK;
} /* end of ioal_init */

/* Function Name:
 *      ioal_init_unitID_change
 * Description:
 *      Change a unit ID from fromID to toID, and delete fromID.
 * Input:
 *      fromID        - original unit ID
 *      toID          - new unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
ioal_init_unitID_change(uint32 fromID, uint32 toID)
{

    return RT_ERR_OK;
}

int32
ioal_debug_show(void)
{
    return RT_ERR_OK;
}


