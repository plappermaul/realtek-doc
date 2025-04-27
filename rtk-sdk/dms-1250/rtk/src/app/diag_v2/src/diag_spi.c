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
 * Purpose : Definition those internal GPIO command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) SPI commands.
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <drv/spi/spi.h>
#include <private/drv/spi/spi_private.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_diag.h>
#endif

#ifdef CMD_SPI_SET_REG_REG_ADD_REG_DATA
/*
 * spi set cs <UINT:cs> reg <UINT:reg_add> <UINT:reg_data>
 */
cparser_result_t cparser_cmd_spi_set_reg_reg_add_reg_data(cparser_context_t *context,
    uint32_t *reg_add_ptr,
    uint32_t *reg_data_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((reg_add_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((reg_data_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(drv_spi_write(unit, *reg_add_ptr, reg_data_ptr), ret);

    diag_util_printf("    Write Register:0x%02X, VALUE:0x%02X", *reg_add_ptr, *reg_data_ptr);
    diag_util_printf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_SPI_GET_REG_REG_ADD
/*
 * spi get reg <UINT:reg_add>
 */
cparser_result_t cparser_cmd_spi_get_reg_reg_add(cparser_context_t *context,
    uint32_t *reg_add_ptr)
{
    uint32 unit,ret_data;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((reg_add_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    /* normal function */
    DIAG_UTIL_ERR_CHK(drv_spi_read(unit,*reg_add_ptr, &ret_data), ret);
    diag_util_printf("    READ Register:0x%02X, VALUE:0x%02X",*reg_add_ptr, ret_data);
    diag_util_printf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_SPI_SET_SCLK_SCLK_MOSI_MOSI_MISO_MISO_SS_SS_RESET_RESET_INT_INT
cparser_result_t cparser_cmd_spi_set_sclk_sclk_mosi_mosi_miso_miso_ss_ss_reset_reset_int_int(cparser_context_t *context,
    uint32_t *sclk_ptr,
    uint32_t *mosi_ptr,
    uint32_t *miso_ptr,
    uint32_t *ss_ptr,
    uint32_t *reset_ptr,
    uint32_t *int_ptr)
{
    uint32  unit = 0;
    spi_init_info_t init_info;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    init_info.gpioNum_int   = *int_ptr;
    init_info.gpioNum_mosi  = *mosi_ptr;
    init_info.gpioNum_miso  = *miso_ptr;
    init_info.gpioNum_reset = *reset_ptr;
    init_info.gpioNum_sclk  = *sclk_ptr;
    init_info.gpioNum_ss    = *ss_ptr;

    drv_spiPin_init(unit, &init_info);

    diag_util_printf("Init GPIO pin for SPI ... Done!\n");

    return CPARSER_OK;

}

#endif

