/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : For linux kernel mode
 *           I/O read/write APIs using MDIO interface in the SDK.
 *
 * Feature : I/O read/write APIs, by access swich register by MDIO interface
 *
 */

/*
 * Include Files
 */
#include <common/error.h>
#include <common/util.h>
#include <ioal/ioal_init.h>
#include <ioal/io_mii.h>

/*
 * Symbol Definition
 */
#define MII_ADDR  0xFF

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */



void io_mii_init(void)
{
}

void io_mii_deinit(void)
{
}

int io_mii_phy_reg_write(uint8 phy_id,uint8 reg, uint16 value)
{
    int32 ret;

    if((ret = rtusr_phy_reg_set(phy_id,reg,value)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

int io_mii_phy_reg_read(uint8 phy_id,uint8 reg, uint16 *pValue)
{
    int32 ret;

    if((ret = rtusr_phy_reg_get(phy_id,reg,pValue)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

int io_mii_memory_write(uint32 memaddr,uint32 data)
{
    int32 ret;

    if((ret = rtusr_reg_address_set(memaddr, data)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

uint32 io_mii_memory_read(uint32 memaddr)
{
    int32 ret;
    uint32 data;

    if((ret = rtusr_reg_address_get(memaddr,&data)) != RT_ERR_OK)
    {
        return ret;
    }

    return data;
}




int io_mii_socMemory_write(uint32 memaddr,uint32 data)
{
    int32 ret;

    if((ret = rtusr_soc_address_set(memaddr, data)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

uint32 io_mii_socMemory_read(uint32 memaddr)
{
    int32 ret;
    uint32 data;

    if((ret = rtusr_soc_address_get(memaddr,&data)) != RT_ERR_OK)
    {
        return ret;
    }

    return data;
}
