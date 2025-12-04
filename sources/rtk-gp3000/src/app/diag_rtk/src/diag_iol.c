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
 * Purpose : Definition those Extension command and APIs in the SDK diagnostic shell.
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
#include <parser/cparser_priv.h>
#include <diag_str.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>

#ifdef CONFIG_SDK_APOLLO
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#endif

#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#endif

#ifdef CONFIG_SDK_RTL9601B
#include <hal/chipdef/rtl9601b/rtk_rtl9601b_reg_struct.h>
#endif

#if defined(CONFIG_SDK_RTL9602C)
#include <hal/chipdef/rtl9602c/rtk_rtl9602c_reg_struct.h>
#endif

#if defined(CONFIG_SDK_RTL9607C)
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_reg_struct.h>
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
#include <hal/chipdef/rtl9603cvd/rtk_rtl9603cvd_reg_struct.h>
#endif


/*
 * iol set max-length state ( disable | enable )
 */
cparser_result_t
cparser_cmd_iol_set_max_length_state_disable_enable(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        enable = ENABLED;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(IOL_RXDROP_CFGr, RX_IOL_MAX_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_IOL_RXDROP_CFGr, APOLLOMP_RX_IOL_MAX_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if 0
        case RTL9601B_CHIP_ID:
            if ((ret = reg_field_write(RTL9601B_IOL_RXDROP_CFGr, RTL9601B_RX_IOL_MAX_LENGTH_CFGf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return ret;
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_CFG_MAC_MISCr, RTL9602C_RX_IOL_MAX_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_CFG_MAC_MISCr, RTL9607C_RX_IOL_MAX_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_CFG_MAC_MISCr, RTL9603CVD_RX_IOL_MAX_LENGTH_CFGf, &enable), ret);

            break;
#endif 

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_iol_set_max_length_state_disable_enable */

/*
 * iol get max-length
 */
cparser_result_t
cparser_cmd_iol_get_max_length(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable=ENABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(IOL_RXDROP_CFGr, RX_IOL_MAX_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_IOL_RXDROP_CFGr, APOLLOMP_RX_IOL_MAX_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if 0
        case RTL9601B_CHIP_ID:
            if ((ret = reg_field_read(RTL9601B_IOL_RXDROP_CFGr, RTL9601B_RX_IOL_MAX_LENGTH_CFGf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return ret;
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_CFG_MAC_MISCr, RTL9602C_RX_IOL_MAX_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_CFG_MAC_MISCr, RTL9607C_RX_IOL_MAX_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_CFG_MAC_MISCr, RTL9603CVD_RX_IOL_MAX_LENGTH_CFGf, &enable), ret);

            break;
#endif 
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            break;
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_mprintf("IOL max length checking: %s\n", diagStr_enable[enable]);


    return CPARSER_OK;
}    /* end of cparser_cmd_iol_get_max_length */

/*
 * iol set error-length state ( disable | enable )
 */
cparser_result_t
cparser_cmd_iol_set_error_length_state_disable_enable(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        enable = ENABLED;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(IOL_RXDROP_CFGr, RX_IOL_ERROR_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_IOL_RXDROP_CFGr, APOLLOMP_RX_IOL_ERROR_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_RX_IOL_ERROR_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_CFG_MAC_MISCr, RTL9602C_RX_IOL_ERROR_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_CFG_MAC_MISCr, RTL9607C_RX_IOL_ERROR_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_CFG_MAC_MISCr, RTL9603CVD_RX_IOL_ERROR_LENGTH_CFGf, &enable), ret);

            break;
#endif 
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_iol_set_error_length_state_disable_enable */

/*
 * iol get error-length
 */
cparser_result_t
cparser_cmd_iol_get_error_length(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable=ENABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(IOL_RXDROP_CFGr, RX_IOL_ERROR_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_IOL_RXDROP_CFGr, APOLLOMP_RX_IOL_ERROR_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_RX_IOL_ERROR_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_CFG_MAC_MISCr, RTL9602C_RX_IOL_ERROR_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_CFG_MAC_MISCr, RTL9607C_RX_IOL_ERROR_LENGTH_CFGf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_CFG_MAC_MISCr, RTL9603CVD_RX_IOL_ERROR_LENGTH_CFGf, &enable), ret);

            break;
#endif 
        case UNKNOWN_CHIP_ID:
            break;
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_mprintf("IOL error length checking: %s\n", diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_iol_get_error_length */

/*
 * iol set 16-collision state ( disable | enable )
 */
cparser_result_t
cparser_cmd_iol_set_16_collision_state_disable_enable(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else
    {
        enable = ENABLED;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(CFG_UNHIOLr, IOL_16DROPf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_CFG_UNHIOLr, APOLLOMP_IOL_16DROPf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_IOL_16DROPf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_CFG_UNHIOLr, RTL9602C_IOL_16DROPf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_CFG_UNHIOLr, RTL9607C_IOL_16DROPf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_CFG_UNHIOLr, RTL9603CVD_IOL_16DROPf, &enable), ret);

            break;
#endif 
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_iol_set_16_collision_state_disable_enable */

/*
 * iol get 16-collision
 */
cparser_result_t
cparser_cmd_iol_get_16_collision(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable = ENABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(CFG_UNHIOLr, IOL_16DROPf, &enable), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_CFG_UNHIOLr, APOLLOMP_IOL_16DROPf, &enable), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_IOL_16DROPf, &enable), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_CFG_UNHIOLr, RTL9602C_IOL_16DROPf, &enable), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_CFG_UNHIOLr, RTL9607C_IOL_16DROPf, &enable), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_CFG_UNHIOLr, RTL9603CVD_IOL_16DROPf, &enable), ret);
            break;
#endif 
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            break;
            
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_mprintf("IOL drop after 16 collisions: %s\n", diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_iol_get_16_collision */

