/*
 * Copyright (C) 2011Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
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
#include <ioal/ioal.h>

#if defined(CONFIG_SDK_ASICDRV_TEST)
    uint32 DrvVirtualReg[DRV_VIRTUAL_REG_SIZE];
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
uint32 swcore_base;

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
ioal_init_memRegion_get(ioal_memRegion_t mem, uint32 *pBaseAddr)
{
    switch(mem)
    {
        case IOAL_MEM_SWCORE:
            *pBaseAddr = swcore_base;
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
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
ioal_init(void)
{
#if defined(CONFIG_SDK_ASICDRV_TEST)
    swcore_base = (uint32)DrvVirtualReg;

#else

  #if defined(RTL_RLX_IO)
    io_rsp_init();
  #endif

  #if defined(CYGWIN_MDIO_IO) || defined(LINUX_KERNEL_MDIO_IO)
    io_mii_init();
  #endif

    swcore_base = SWCORE_VIRT_BASE;
#endif


    /* common IOAL init procedure */

    return RT_ERR_OK;
} /* end of ioal_init */



