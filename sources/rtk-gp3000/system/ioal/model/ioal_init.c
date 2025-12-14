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
 * Purpose : IOAL Layer Init Module
 *
 * Feature : IOAL Init Functions
 *
 */

/*
 * Include Files
 */
#include <ioal/ioal_init.h>

/*
 * Symbol Definition
 */
#define VIRT_SWCORE_REG_SIZE    (0x1000000 - 16*64*1024)     /*15M */
#define VIRTUAL_SWCORE_TBL_SIZE	(512*1024)                  /* 512k */

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
    switch(mem)
    {
        case IOAL_MEM_SWCORE:
            *pBaseAddr = swcore_base[unit];
            break;      

        default:
            return RT_ERR_FAILED;
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

    /* common IOAL init procedure */

    return RT_ERR_OK;
} /* end of ioal_init */



