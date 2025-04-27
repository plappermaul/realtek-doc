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
 * Purpose : Definition those public SPI APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <private/drv/i2c/i2c_mapper.h>
#include <common/debug/rt_log.h>
#include <hwp/hw_profile.h>

#if (defined(CONFIG_SDK_DRIVER_I2C_R8390))
    #include <private/drv/i2c/i2c_rtl8390.h>
#endif
#if (defined(CONFIG_SDK_DRIVER_I2C_R8380))
    #include <private/drv/i2c/i2c_rtl8380.h>
#endif
#if (defined(CONFIG_SDK_DRIVER_I2C_R9300))
    #include <private/drv/i2c/i2c_rtl9300.h>
#endif
#if (defined(CONFIG_SDK_DRIVER_I2C_R9310))
    #include <private/drv/i2c/i2c_rtl9310.h>
#endif

/*
 * Symbol Definition
 */
#define I2C_DB_SIZE     (sizeof(i2c_db)/sizeof(cid_prefix_group_t))

/*
 * Data Declaration
 */
const static cid_prefix_group_t i2c_db[] =
{
#if defined(CONFIG_SDK_DRIVER_I2C_R8390)
        {RTL8390_FAMILY_ID, I2C_R8390},
        {RTL8350_FAMILY_ID, I2C_R8390},
#endif
#if defined(CONFIG_SDK_DRIVER_I2C_R8380)
        {RTL8380_FAMILY_ID, I2C_R8380},
        {RTL8330_FAMILY_ID, I2C_R8380},
#endif
#if (defined(CONFIG_SDK_DRIVER_I2C_R9300))
        {RTL9300_FAMILY_ID, I2C_R9300},
#endif
#if (defined(CONFIG_SDK_DRIVER_I2C_R9310))
        {RTL9310_FAMILY_ID, I2C_R9310},
#endif
};

i2c_mapper_operation_t i2c_ops[I2C_CTRL_END] =
{
#if (defined(CONFIG_SDK_DRIVER_I2C_R8390))
    {   /* I2C_R8390 */
        .init = r8390_i2c_init,
        .i2c_dev_init = r8390_i2c_dev_init,
        .i2c_write = r8390_i2c_write,
        .i2c_read = r8390_i2c_read,
    },
#endif
#if (defined(CONFIG_SDK_DRIVER_I2C_R8380))
    {   /* I2C_R8380 */
        .init = r8380_i2c_init,
        .i2c_dev_init = r8380_i2c_dev_init,
        .i2c_write = r8380_i2c_write,
        .i2c_read = r8380_i2c_read,
    },
#endif
#if (defined(CONFIG_SDK_DRIVER_I2C_R9300))
    {   /* I2C_R9300 */
        .init = r9300_i2c_init,
        .i2c_dev_init = r9300_i2c_dev_init,
        .i2c_write = r9300_i2c_write,
        .i2c_read = r9300_i2c_read,
    },
#endif
#if (defined(CONFIG_SDK_DRIVER_I2C_R9310))
    {   /* I2C_R9300 */
        .init = r9310_i2c_init,
        .i2c_dev_init = r9310_i2c_dev_init,
        .i2c_write = r9310_i2c_write,
        .i2c_read = r9310_i2c_read,
    },
#endif


};

uint32 i2c_if[RTK_MAX_NUM_OF_UNIT] = {CID_GROUP_NONE};

/*
 * Function Declaration
 */

/* Function Name:
 *      i2c_probe
 * Description:
 *      Probe i2c module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
i2c_probe(uint32 unit)
{
    uint32      i;
    uint32      cid = 0;

    RT_INIT_MSG("  I2C probe (unit %u)\n", unit);

    cid = HWP_CHIP_ID(unit);

    i2c_if[unit] = CID_GROUP_NONE;
    for (i=0; i<I2C_DB_SIZE; i++)
    {
        if (CID_PREFIX_MATCH(i2c_db[i], cid))
        {
            i2c_if[unit] = i2c_db[i].gid;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_CHIP_NOT_FOUND;

} /* end of i2c_probe */

