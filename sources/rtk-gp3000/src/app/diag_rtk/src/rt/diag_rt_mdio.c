/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
 * Purpose : Definition those MDIO command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <rtk/rt/rt_mdio.h>

/*
 * rt_mdio init
 */
cparser_result_t
cparser_cmd_rt_mdio_init(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_mdio_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mdio_init */

/*
 * rt_mdio set c22-format bus <UINT:bus> phyid <UINT:phyid> register <UINT:register> data <UINT:data>
 */
cparser_result_t
cparser_cmd_rt_mdio_set_c22_format_bus_bus_phyid_phyid_register_register_data_data(
    cparser_context_t *context,
    uint32_t  *bus_ptr,
    uint32_t  *phyid_ptr,
    uint32_t  *register_ptr,
    uint32_t  *data_ptr)
{
    int32 ret;
    rt_mdio_info_t mdioInfo;

    DIAG_UTIL_PARAM_CHK();

    mdioInfo.format = RT_MDIO_FMT_C22;
    mdioInfo.set = *bus_ptr;
    mdioInfo.phyAddr = *phyid_ptr;
    mdioInfo.dev = 0;
    mdioInfo.reg = *register_ptr;
    DIAG_UTIL_ERR_CHK(rt_mdio_write(&mdioInfo, (uint16_t)(*data_ptr & 0xffff)), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mdio_set_c22_format_bus_bus_phyid_phyid_register_register_data_data */

/*
 * rt_mdio get c22-format bus <UINT:bus> phyid <UINT:phyid> register <UINT:register>
 */
cparser_result_t
cparser_cmd_rt_mdio_get_c22_format_bus_bus_phyid_phyid_register_register(
    cparser_context_t *context,
    uint32_t  *bus_ptr,
    uint32_t  *phyid_ptr,
    uint32_t  *register_ptr)
{
    int32 ret;
    uint16_t data;
    rt_mdio_info_t mdioInfo;

    DIAG_UTIL_PARAM_CHK();

    mdioInfo.format = RT_MDIO_FMT_C22;
    mdioInfo.set = *bus_ptr;
    mdioInfo.phyAddr = *phyid_ptr;
    mdioInfo.dev = 0;
    mdioInfo.reg = *register_ptr;
    DIAG_UTIL_ERR_CHK(rt_mdio_read(&mdioInfo, &data), ret);

    diag_util_printf("  bus: 0x%x, phyid: 0x%x, register: 0x%x data: 0x%x\n\r", *bus_ptr, *phyid_ptr, *register_ptr, data);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mdio_get_c22_format_bus_bus_phyid_phyid_register_register */

/*
 * rt_mdio set c45-format bus <UINT:bus> phyaddr <UINT:phyaddr> device <UINT:device> register <UINT:register> data <UINT:data>
 */
cparser_result_t
cparser_cmd_rt_mdio_set_c45_format_bus_bus_phyaddr_phyaddr_device_device_register_register_data_data(
    cparser_context_t *context,
    uint32_t  *bus_ptr,
    uint32_t  *phyaddr_ptr,
    uint32_t  *device_ptr,
    uint32_t  *register_ptr,
    uint32_t  *data_ptr)
{
    int32 ret;
    rt_mdio_info_t mdioInfo;

    DIAG_UTIL_PARAM_CHK();

    mdioInfo.format = RT_MDIO_FMT_C45;
    mdioInfo.set = *bus_ptr;
    mdioInfo.phyAddr = *phyaddr_ptr;
    mdioInfo.dev = *device_ptr;
    mdioInfo.reg = *register_ptr;
    DIAG_UTIL_ERR_CHK(rt_mdio_write(&mdioInfo, (uint16_t)(*data_ptr & 0xffff)), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mdio_set_c45_format_bus_bus_phyaddr_phyaddr_device_device_register_register_data_data */

/*
 * rt_mdio get c45-format bus <UINT:bus> phyaddr <UINT:phyaddr> device <UINT:device> register <UINT:register>
 */
cparser_result_t
cparser_cmd_rt_mdio_get_c45_format_bus_bus_phyaddr_phyaddr_device_device_register_register(
    cparser_context_t *context,
    uint32_t  *bus_ptr,
    uint32_t  *phyaddr_ptr,
    uint32_t  *device_ptr,
    uint32_t  *register_ptr)
{
    int32 ret;
    uint16_t data;
    rt_mdio_info_t mdioInfo;

    DIAG_UTIL_PARAM_CHK();

    mdioInfo.format = RT_MDIO_FMT_C45;
    mdioInfo.set = *bus_ptr;
    mdioInfo.phyAddr = *phyaddr_ptr;
    mdioInfo.dev = *device_ptr;
    mdioInfo.reg = *register_ptr;
    DIAG_UTIL_ERR_CHK(rt_mdio_read(&mdioInfo, &data), ret);

    diag_util_printf("  bus: 0x%x, phyaddr: 0x%x, device: 0x%0x register: 0x%x data: 0x%x\n\r", *bus_ptr, *phyaddr_ptr, *device_ptr, *register_ptr, data);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mdio_get_c45_format_bus_bus_phyaddr_phyaddr_device_device_register_register */

