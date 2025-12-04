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
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
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
#include <rtk/rate.h>


#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <hal/chipdef/rtl9601b/rtk_rtl9601b_reg_struct.h>
#include <hal/chipdef/rtl9602c/rtk_rtl9602c_reg_struct.h>
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_reg_struct.h>
#include <hal/chipdef/rtl9603cvd/rtk_rtl9603cvd_reg_struct.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>

/*
 * meter init
 */
cparser_result_t
cparser_cmd_meter_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    /*init rate module*/
    DIAG_UTIL_ERR_CHK(rtk_rate_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_meter_init */


/*
 * meter get entry <MASK_LIST:index>
 */
cparser_result_t
cparser_cmd_meter_get_entry_index(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 burstSize;
    uint32 rate;
    rtk_enable_t ifgInclude;
    rtk_rate_metet_mode_t meterMode;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
#ifdef CONFIG_LUNA_G3_SERIES
    index = atoi(TOKEN_STR(3));
#else
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 3), ret);
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
#endif
    DIAG_UTIL_MASK_SCAN(mask, index)
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeterBucket_get(index, &burstSize), ret);
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_get(index, &rate,&ifgInclude), ret);

        ret = rtk_rate_shareMeterMode_get(index, &meterMode);
        
        if(ret == RT_ERR_DRIVER_NOT_FOUND) {
            diag_util_mprintf("Meter idx = %-2u, meter rate = %-5u Kbps, include IFG = %-8s, burst size = %u\n",
                                index, rate,
                                ifgInclude?DIAG_STR_ENABLE:DIAG_STR_DISABLE,
                                burstSize);
        } else {
            if(METER_MODE_BIT_RATE==meterMode) {
                diag_util_mprintf("Meter idx = %-2u, meter rate = %-5u Kbps, include IFG = %-8s, burst size = %u\n",
                                    index, rate,
                                    ifgInclude?DIAG_STR_ENABLE:DIAG_STR_DISABLE,
                                    burstSize);
            } else {
                diag_util_mprintf("Meter idx = %-2u, meter rate = %-5u packet per second, include IFG = %-8s, burst size = %u\n",
                                    index, rate,
                                    ifgInclude?DIAG_STR_ENABLE:DIAG_STR_DISABLE,
                                    burstSize);
            }
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_entry_index */

/*
 * meter get entry <MASK_LIST:index> burst-size
 */
cparser_result_t
cparser_cmd_meter_get_entry_index_burst_size(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 burstSize;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
#ifdef CONFIG_LUNA_G3_SERIES
    index = atoi(TOKEN_STR(3));
#else
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 3), ret);
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
#endif
    DIAG_UTIL_MASK_SCAN(mask, index)
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeterBucket_get(index, &burstSize), ret);
        diag_util_mprintf("Meter idx = %u, burst size = %u\n", index, burstSize);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_entry_index_burst_size */

/*
 * meter set entry <MASK_LIST:index> burst-size <UINT:size>
 */
cparser_result_t
cparser_cmd_meter_set_entry_index_burst_size_size(
    cparser_context_t *context,
    char * *index_ptr,
    uint32_t  *size_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;

    DIAG_UTIL_PARAM_CHK();
#ifdef CONFIG_LUNA_G3_SERIES
    index = atoi(TOKEN_STR(3));
#else
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 3), ret);
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
#endif
    DIAG_UTIL_MASK_SCAN(mask, index)
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeterBucket_set(index, *size_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_meter_set_entry_index_burst_size_size */

/*
 * meter get entry <MASK_LIST:index> ifg
 */
cparser_result_t
cparser_cmd_meter_get_entry_index_ifg(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 rate;
    rtk_enable_t ifgInclude;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_PARAM_CHK();
#ifdef CONFIG_LUNA_G3_SERIES
    index = atoi(TOKEN_STR(3));
#else
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 3), ret);
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
#endif
    DIAG_UTIL_MASK_SCAN(mask, index)
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_get(index, &rate,&ifgInclude), ret);
        diag_util_mprintf("Meter idx = %u, include IFG = %s\n", index, diagStr_ifgState[ifgInclude]);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_entry_index_ifg */

/*
 * meter set entry <MASK_LIST:index> ifg ( exclude | include )
 */
cparser_result_t
cparser_cmd_meter_set_entry_index_ifg_exclude_include(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 rate;
    rtk_enable_t ifgInclude;

    DIAG_UTIL_PARAM_CHK();
#ifdef CONFIG_LUNA_G3_SERIES
    index = atoi(TOKEN_STR(3));
#else
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 3), ret);
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
#endif

    DIAG_UTIL_MASK_SCAN(mask, index)
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_get(index, &rate,&ifgInclude), ret);

        if ('e' == TOKEN_CHAR(5,0))
            ifgInclude = DISABLED;
        else
            ifgInclude = ENABLED;
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_set(index, rate,ifgInclude), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_set_entry_index_ifg_exclude_include */

/*
 * meter get entry <MASK_LIST:index> meter-exceed
 */
cparser_result_t
cparser_cmd_meter_get_entry_index_meter_exceed(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    rtk_enable_t isExceed;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
#ifdef CONFIG_LUNA_G3_SERIES
    index = atoi(TOKEN_STR(3));
#else
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 3), ret);
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
#endif

    DIAG_UTIL_MASK_SCAN(mask, index)
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeterExceed_get(index, &isExceed), ret);
        diag_util_mprintf("Meter idx = %u, meter exceed = %s\n", index, isExceed?DIAG_STR_YES:DIAG_STR_NO);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_entry_index_meter_exceed */

/*
 * meter reset entry <MASK_LIST:index> meter-exceed
 */
cparser_result_t
cparser_cmd_meter_reset_entry_index_meter_exceed(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    rtk_enable_t isExceed;

    DIAG_UTIL_PARAM_CHK();

#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 3), ret);
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
#endif

    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeterExceed_clear(index), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_reset_entry_index_meter_exceed */

/*
 * meter get tick-token
 */
cparser_result_t
cparser_cmd_meter_get_tick_token(
    cparser_context_t *context)
{
    uint32 tickPeriod = 0,tkn = 0;
    uint32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_read(METER_TB_CTRLr, TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(METER_TB_CTRLr, TKNf, &tkn)), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_read(APOLLOMP_METER_TB_CTRLr, APOLLOMP_TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(APOLLOMP_METER_TB_CTRLr, APOLLOMP_TKNf, &tkn)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9601B_METER_TB_CTRLr, RTL9601B_TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9601B_METER_TB_CTRLr, RTL9601B_TKNf, &tkn)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9602C_METER_TB_CTRLr, RTL9602C_TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9602C_METER_TB_CTRLr, RTL9602C_TKNf, &tkn)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9607C_METER_TB_CTRLr, RTL9607C_TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9607C_METER_TB_CTRLr, RTL9607C_TKNf, &tkn)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9603CVD_METER_TB_CTRLr, RTL9603CVD_TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9603CVD_METER_TB_CTRLr, RTL9603CVD_TKNf, &tkn)), ret);
            break;
#endif
       case UNKNOWN_CHIP_ID:
           break;
       default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;

    }

    diag_util_mprintf("tick period:%u, token:%u \n",tickPeriod,tkn);

    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_tick_token */

/*
 * meter set tick-token tick-period <UINT:period> token <UINT:token>
 */
cparser_result_t
cparser_cmd_meter_set_tick_token_tick_period_period_token_token(
    cparser_context_t *context,
    uint32_t  *period_ptr,
    uint32_t  *token_ptr)
{
    uint32 ret;
    uint32 period;
    uint32 token;

    period = *period_ptr;
    token  = *token_ptr;

    DIAG_UTIL_PARAM_CHK();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_write(METER_TB_CTRLr, TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(METER_TB_CTRLr, TKNf, &token)), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_write(APOLLOMP_METER_TB_CTRLr, APOLLOMP_TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(APOLLOMP_METER_TB_CTRLr, APOLLOMP_TKNf, &token)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9601B_METER_TB_CTRLr, RTL9601B_TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9601B_METER_TB_CTRLr, RTL9601B_TKNf, &token)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9602C_METER_TB_CTRLr, RTL9602C_TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9602C_METER_TB_CTRLr, RTL9602C_TKNf, &token)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9607C_METER_TB_CTRLr, RTL9607C_TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9607C_METER_TB_CTRLr, RTL9607C_TKNf, &token)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9603CVD_METER_TB_CTRLr, RTL9603CVD_TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9603CVD_METER_TB_CTRLr, RTL9603CVD_TKNf, &token)), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;

    }

    return CPARSER_OK;
}    /* end of cparser_cmd_meter_set_tick_token_tick_period_period_token_token */

/*
 * meter get pon-tick-token
 */
cparser_result_t
cparser_cmd_meter_get_pon_tick_token(
    cparser_context_t *context)
{
    uint32 tickPeriod = 0,tkn = 0;
    uint32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_read(PON_TB_CTRLr, TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(PON_TB_CTRLr, TKNf, &tkn)), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_read(APOLLOMP_PON_TB_CTRLr, APOLLOMP_TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(APOLLOMP_PON_TB_CTRLr, APOLLOMP_TKNf, &tkn)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9601B_PON_TB_CTRLr, RTL9601B_TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9601B_PON_TB_CTRLr, RTL9601B_TKNf, &tkn)), ret);
            break;
#endif

#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9602C_PON_TB_CTRLr, RTL9602C_TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9602C_PON_TB_CTRLr, RTL9602C_TKNf, &tkn)), ret);
            break;
#endif

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9607C_PON_TB_CTRLr, RTL9607C_TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9607C_PON_TB_CTRLr, RTL9607C_TKNf, &tkn)), ret);
            break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9603CVD_PON_TB_CTRLr, RTL9603CVD_TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(RTL9603CVD_PON_TB_CTRLr, RTL9603CVD_TKNf, &tkn)), ret);
            break;
#endif
        case UNKNOWN_CHIP_ID:
            break;
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_mprintf("tick period:%u, token:%u \n",tickPeriod,tkn);

    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_pon_tick_token */

/*
 * meter set pon-tick-token tick-period <UINT:period> token <UINT:token>
 */
cparser_result_t
cparser_cmd_meter_set_pon_tick_token_tick_period_period_token_token(
    cparser_context_t *context,
    uint32_t  *period_ptr,
    uint32_t  *token_ptr)
{
    uint32 ret;
    uint32 period;
    uint32 token;

    period = *period_ptr;
    token  = *token_ptr;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_write(PON_TB_CTRLr, TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(PON_TB_CTRLr, TKNf, &token)), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_write(APOLLOMP_PON_TB_CTRLr, APOLLOMP_TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(APOLLOMP_PON_TB_CTRLr, APOLLOMP_TKNf, &token)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9601B_PON_TB_CTRLr, RTL9601B_TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9601B_PON_TB_CTRLr, RTL9601B_TKNf, &token)), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9602C_PON_TB_CTRLr, RTL9602C_TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9602C_PON_TB_CTRLr, RTL9602C_TKNf, &token)), ret);
            break;
#endif

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9607C_PON_TB_CTRLr, RTL9607C_TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9607C_PON_TB_CTRLr, RTL9607C_TKNf, &token)), ret);
            break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9603CVD_PON_TB_CTRLr, RTL9603CVD_TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(RTL9603CVD_PON_TB_CTRLr, RTL9603CVD_TKNf, &token)), ret);
            break;
#endif

       default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_meter_set_pon_tick_token_tick_period_period_token_token */

/*
 * meter get entry <MASK_LIST:index> rate
 */
cparser_result_t
cparser_cmd_meter_get_entry_index_rate(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 rate;
    rtk_enable_t ifgInclude;
    rtk_rate_metet_mode_t meterMode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_PARAM_CHK();
#ifdef CONFIG_LUNA_G3_SERIES
    index = atoi(TOKEN_STR(3));
#else
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 3), ret);
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
#endif
    DIAG_UTIL_MASK_SCAN(mask, index)
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_get(index, &rate,&ifgInclude), ret);
        ret = rtk_rate_shareMeterMode_get(index, &meterMode);
        if(ret == RT_ERR_DRIVER_NOT_FOUND) {
            diag_util_mprintf("Meter idx = %u, meter rate = %u Kbps\n", index, rate);
        } else {
            if(METER_MODE_BIT_RATE==meterMode) {
                diag_util_mprintf("Meter idx = %u, meter rate = %u Kbps\n", index, rate);
            } else {
                diag_util_mprintf("Meter idx = %u, meter rate = %u packet per second\n", index, rate);
            }
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_entry_index_rate */

/*
 * meter set entry <MASK_LIST:index> rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_meter_set_entry_index_rate_rate(
    cparser_context_t *context,
    char * *index_ptr,
    uint32_t  *rate_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 rate;
    rtk_enable_t ifgInclude;

    DIAG_UTIL_PARAM_CHK();
#ifdef CONFIG_LUNA_G3_SERIES
    index = atoi(TOKEN_STR(3));
#else
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 3), ret);
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
#endif

    DIAG_UTIL_MASK_SCAN(mask, index)
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_get(index, &rate,&ifgInclude), ret);

        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_set(index, *rate_ptr,ifgInclude), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_set_entry_index_rate_rate */

/*
 * meter get entry <MASK_LIST:index> meter-mode
 */
cparser_result_t
cparser_cmd_meter_get_entry_index_meter_mode(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    rtk_rate_metet_mode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_PARAM_CHK();
#ifdef CONFIG_LUNA_G3_SERIES
    index = atoi(TOKEN_STR(3));
#else
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 3), ret);
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
#endif
    DIAG_UTIL_MASK_SCAN(mask, index)
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeterMode_get(index, &mode), ret);
        if(mode == METER_MODE_BIT_RATE)
            diag_util_mprintf("Meter idx = %u, bit-rate\n", index);
        else
            diag_util_mprintf("Meter idx = %u, packet-rate\n", index);
            
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_entry_index_meter_mode */

/*
 * meter set entry <MASK_LIST:index> meter-mode ( bit-rate | packet-rate )
 */
cparser_result_t
cparser_cmd_meter_set_entry_index_meter_mode_bit_rate_packet_rate(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    rtk_rate_metet_mode_t mode;

    DIAG_UTIL_PARAM_CHK();
#ifdef CONFIG_LUNA_G3_SERIES
    index = atoi(TOKEN_STR(3));
#else
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 3), ret);
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
#endif
#endif
    if ('b' == TOKEN_CHAR(5,0))
        mode = METER_MODE_BIT_RATE;
    else
        mode = METER_MODE_PACKET_RATE;

#ifdef CONFIG_LUNA_G3_SERIES
    /*nothing*/
#else
    DIAG_UTIL_MASK_SCAN(mask, index)
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeterMode_set(index, mode), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_set_entry_index_meter_mode_bit_rate_packet_rate */


