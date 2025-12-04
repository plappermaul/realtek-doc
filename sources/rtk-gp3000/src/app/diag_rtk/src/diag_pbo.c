 /*
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (?Ÿæ?äº? 03 äº”æ? 2013) $
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
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/pbo.h>
#include <ioal/mem32.h>

#if defined(CONFIG_SDK_RTL9601B)
#include <dal/rtl9601b/dal_rtl9601b_pbo.h>
#endif
#if defined(CONFIG_SDK_RTL9602C)
#include <dal/rtl9602c/dal_rtl9602c_pbo.h>
#endif
#if defined(CONFIG_SDK_RTL9607C)
#include <dal/rtl9607c/dal_rtl9607c_pbo.h>
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
#include <dal/rtl9603cvd/dal_rtl9603cvd_pbo.h>
#endif

#include <osal/time.h>

/*
 * pbo init
 */
cparser_result_t
cparser_cmd_pbo_init(
    cparser_context_t *context)
{
    int32 ret;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_init(), ret);
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_init */

/*
 * pbo init gpon
 */
cparser_result_t
cparser_cmd_pbo_init_gpon(
    cparser_context_t *context)
{
    int32 ret;
    rtk_pbo_initParam_t initParam;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            initParam.mode = PBO_MODE_GPON;
            initParam.usPageSize = PBO_PAGE_SIZE_128;
            initParam.dsPageSize = PBO_PAGE_SIZE_128;
            DIAG_UTIL_ERR_CHK(rtk_pbo_init(initParam), ret);
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        case RTL9607C_CHIP_ID:
        case RTL9603CVD_CHIP_ID:
            initParam.mode = PBO_MODE_GPON;
            initParam.usPageSize = PBO_PAGE_SIZE_128;
            initParam.dsPageSize = PBO_PAGE_SIZE_128;
            DIAG_UTIL_ERR_CHK(rtk_pbo_init(initParam), ret);
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_init_gpon */

/*
 * pbo init epon
 */
cparser_result_t
cparser_cmd_pbo_init_epon(
    cparser_context_t *context)
{
    int32 ret;
    rtk_pbo_initParam_t initParam;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            initParam.mode = PBO_MODE_EPON;
            initParam.usPageSize = PBO_PAGE_SIZE_128;
            initParam.dsPageSize = PBO_PAGE_SIZE_128;
            DIAG_UTIL_ERR_CHK(rtk_pbo_init(initParam), ret);
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        case RTL9607C_CHIP_ID:
        case RTL9603CVD_CHIP_ID:
            initParam.mode = PBO_MODE_EPON;
            initParam.usPageSize = PBO_PAGE_SIZE_128;
            initParam.dsPageSize = PBO_PAGE_SIZE_128;
            DIAG_UTIL_ERR_CHK(rtk_pbo_init(initParam), ret);
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_init_epon */

/*
 * pbo init gpon us-page-size ( 128 | 256 | 512 ) ds-page-size ( 128 | 256 | 512 )
 */
cparser_result_t
cparser_cmd_pbo_init_gpon_us_page_size_128_256_512_ds_page_size_128_256_512(
    cparser_context_t *context)
{
    int32 ret;
    rtk_pbo_initParam_t initParam;
    DIAG_UTIL_PARAM_CHK();

    if ('1' == TOKEN_CHAR(4,0))
    {
        initParam.usPageSize = PBO_PAGE_SIZE_128;
    }
    else if ('2' == TOKEN_CHAR(4,0))
    {
        initParam.usPageSize = PBO_PAGE_SIZE_256;
    }
    else if ('5' == TOKEN_CHAR(4,0))
    {
        initParam.usPageSize = PBO_PAGE_SIZE_512;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    if ('1' == TOKEN_CHAR(6,0))
    {
        initParam.dsPageSize = PBO_PAGE_SIZE_128;
    }
    else if ('2' == TOKEN_CHAR(6,0))
    {
        initParam.dsPageSize = PBO_PAGE_SIZE_256;
    }
    else if ('5' == TOKEN_CHAR(6,0))
    {
        initParam.dsPageSize = PBO_PAGE_SIZE_512;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            initParam.mode = PBO_MODE_GPON;
            DIAG_UTIL_ERR_CHK(rtk_pbo_init(initParam), ret);
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        case RTL9607C_CHIP_ID:
        case RTL9603CVD_CHIP_ID:
            initParam.mode = PBO_MODE_GPON;
            DIAG_UTIL_ERR_CHK(rtk_pbo_init(initParam), ret);
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_init_gpon_us_page_size_128_256_512_ds_page_size_128_256_512 */

/*
 * pbo init epon us-page-size ( 128 | 256 | 512 ) ds-page-size ( 128 | 256 | 512 )
 */
cparser_result_t
cparser_cmd_pbo_init_epon_us_page_size_128_256_512_ds_page_size_128_256_512(
    cparser_context_t *context)
{
    int32 ret;
    rtk_pbo_initParam_t initParam;
    DIAG_UTIL_PARAM_CHK();

    if ('1' == TOKEN_CHAR(4,0))
    {
        initParam.usPageSize = PBO_PAGE_SIZE_128;
    }
    else if ('2' == TOKEN_CHAR(4,0))
    {
        initParam.usPageSize = PBO_PAGE_SIZE_256;
    }
    else if ('5' == TOKEN_CHAR(4,0))
    {
        initParam.usPageSize = PBO_PAGE_SIZE_512;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    if ('1' == TOKEN_CHAR(6,0))
    {
        initParam.dsPageSize = PBO_PAGE_SIZE_128;
    }
    else if ('2' == TOKEN_CHAR(6,0))
    {
        initParam.dsPageSize = PBO_PAGE_SIZE_256;
    }
    else if ('5' == TOKEN_CHAR(6,0))
    {
        initParam.dsPageSize = PBO_PAGE_SIZE_512;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            initParam.mode = PBO_MODE_EPON;
            DIAG_UTIL_ERR_CHK(rtk_pbo_init(initParam), ret);
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        case RTL9607C_CHIP_ID:
        case RTL9603CVD_CHIP_ID:
            initParam.mode = PBO_MODE_EPON;
            DIAG_UTIL_ERR_CHK(rtk_pbo_init(initParam), ret);
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_init_epon_us_page_size_128_256_512_ds_page_size_128_256_512 */

/*
 * pbo set stop-all threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_pbo_set_stop_all_threshold_threshold(
    cparser_context_t *context,
    uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_stopAllThreshold_set(*threshold_ptr), ret);
            diag_util_mprintf("Set Drop All Threshold to %u\n", *threshold_ptr);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_stopAllThreshold_set(*threshold_ptr), ret);
            diag_util_mprintf("Set Drop All Threshold to %u\n", *threshold_ptr);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_stopAllThreshold_set(*threshold_ptr), ret);
            diag_util_mprintf("Set Drop All Threshold to %u\n", *threshold_ptr);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_stopAllThreshold_set(*threshold_ptr), ret);
            diag_util_mprintf("Set Drop All Threshold to %u\n", *threshold_ptr);
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_drop_all_threshold_threshold */

/*
 * pbo get stop-all threshold
 */
cparser_result_t
cparser_cmd_pbo_get_stop_all_threshold(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 threshold;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_stopAllThreshold_get(&threshold), ret);
            diag_util_mprintf("Drop All Threshold is %u\n", threshold); 
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_stopAllThreshold_get(&threshold), ret);
            diag_util_mprintf("Drop All Threshold is %u\n", threshold); 
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_stopAllThreshold_get(&threshold), ret);
            diag_util_mprintf("Drop All Threshold is %u\n", threshold); 
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_stopAllThreshold_get(&threshold), ret);
            diag_util_mprintf("Drop All Threshold is %u\n", threshold); 
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_drop_all_threshold */

/*
 * pbo set stop-all state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pbo_set_stop_all_state_enable_disable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable=DISABLED;
    DIAG_UTIL_PARAM_CHK();

    if ('e' == TOKEN_CHAR(4,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_stopAllState_set(enable), ret);
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_stop_all_state_enable_disable */

/*
 * pbo get stop-all state
 */
cparser_result_t
cparser_cmd_pbo_get_stop_all_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t enable;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_stopAllState_get(&enable), ret);

            if(enable == ENABLED){
                diag_util_printf("Stop all enabled!\n");
            }else{
                diag_util_printf("Stop all disabled!\n");
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_stop_all_state */

/*
 * pbo set dsc-runout dram <UINT:threshold> sram <UINT:threshold>
 */
cparser_result_t
cparser_cmd_pbo_set_dsc_runout_dram_dram_th_sram_sram_th(
    cparser_context_t *context,
    uint32_t  *dram_th_ptr,
    uint32_t  *sram_th_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_dscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
            diag_util_mprintf("Set dsc run out threshold to DRAM: %u   SRAM: %u\n", *dram_th_ptr, *sram_th_ptr);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usDscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsDscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
            diag_util_mprintf("Set dsc run out threshold to DRAM: %u   SRAM: %u\n", *dram_th_ptr, *sram_th_ptr);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usDscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsDscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
            diag_util_mprintf("Set dsc run out threshold to DRAM: %u   SRAM: %u\n", *dram_th_ptr, *sram_th_ptr);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usDscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsDscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
            diag_util_mprintf("Set dsc run out threshold to DRAM: %u   SRAM: %u\n", *dram_th_ptr, *sram_th_ptr);
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_dsc_runout_dram_threshold_sram_threshold */

/*
 * pbo set ( upstream | downstream ) dsc-runout dram <UINT:dram_th> sram <UINT:sram_th>
 */
cparser_result_t
cparser_cmd_pbo_set_upstream_downstream_dsc_runout_dram_dram_th_sram_sram_th(
    cparser_context_t *context,
    uint32_t  *dram_th_ptr,
    uint32_t  *sram_th_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    if ('u' == TOKEN_CHAR(2,0))
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9601B)
            case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_dscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
                diag_util_mprintf("Set upstream dsc run out threshold to DRAM: %u   SRAM: %u\n", *dram_th_ptr, *sram_th_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usDscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
                diag_util_mprintf("Set upstream dsc run out threshold to DRAM: %u   SRAM: %u\n", *dram_th_ptr, *sram_th_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usDscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
                diag_util_mprintf("Set upstream dsc run out threshold to DRAM: %u   SRAM: %u\n", *dram_th_ptr, *sram_th_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usDscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
                diag_util_mprintf("Set upstream dsc run out threshold to DRAM: %u   SRAM: %u\n", *dram_th_ptr, *sram_th_ptr);
            }
            break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
            break;
        }
    }
    else
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsDscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
                diag_util_mprintf("Set downstream dsc run out threshold to DRAM: %u   SRAM: %u\n", *dram_th_ptr, *sram_th_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsDscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
                diag_util_mprintf("Set downstream dsc run out threshold to DRAM: %u   SRAM: %u\n", *dram_th_ptr, *sram_th_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsDscRunoutThreshold_set(*dram_th_ptr, *sram_th_ptr), ret);
                diag_util_mprintf("Set downstream dsc run out threshold to DRAM: %u   SRAM: %u\n", *dram_th_ptr, *sram_th_ptr);
            }
            break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
            break;
        }
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_upstream_downstream_dsc_runout_dram_dram_th_sram_sram_th */

/*
 * pbo get dsc-runout
 */
cparser_result_t
cparser_cmd_pbo_get_dsc_runout(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 dramTh, sramTh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_dscRunoutThreshold_get(&dramTh, &sramTh), ret);
            diag_util_mprintf("Dsc run out threshold are DRAM: %u   SRAM: %u\n", dramTh, sramTh);   
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usDscRunoutThreshold_get(&dramTh, &sramTh), ret);
            diag_util_mprintf("Upstrem Dsc run out threshold are DRAM: %u   SRAM: %u\n", dramTh, sramTh);   
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsDscRunoutThreshold_get(&dramTh, &sramTh), ret);
            diag_util_mprintf("Downstrem Dsc run out threshold are DRAM: %u   SRAM: %u\n", dramTh, sramTh); 
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usDscRunoutThreshold_get(&dramTh, &sramTh), ret);
            diag_util_mprintf("Upstrem Dsc run out threshold are DRAM: %u   SRAM: %u\n", dramTh, sramTh);   
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsDscRunoutThreshold_get(&dramTh, &sramTh), ret);
            diag_util_mprintf("Downstrem Dsc run out threshold are DRAM: %u   SRAM: %u\n", dramTh, sramTh); 
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usDscRunoutThreshold_get(&dramTh, &sramTh), ret);
            diag_util_mprintf("Upstrem Dsc run out threshold are DRAM: %u   SRAM: %u\n", dramTh, sramTh);   
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsDscRunoutThreshold_get(&dramTh, &sramTh), ret);
            diag_util_mprintf("Downstrem Dsc run out threshold are DRAM: %u   SRAM: %u\n", dramTh, sramTh); 
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_dsc_runout */

/*
 * pbo set system flowctrl-threshold ( off | on ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_pbo_set_system_flowctrl_threshold_off_on_threshold_threshold(
    cparser_context_t *context,
    uint32_t  *threshold_ptr)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_globalThreshold_set(onThresh, *threshold_ptr), ret);
                diag_util_printf("Set PBO Global Off Threshold to %u\n",*threshold_ptr);
            }
            else if('n'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_globalThreshold_set(*threshold_ptr, offThresh), ret);
                diag_util_printf("Set PBO Global On Threshold to %u\n",*threshold_ptr);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_globalThreshold_set(onThresh, *threshold_ptr), ret);
                diag_util_printf("Set PBO Global Off Threshold to %u\n",*threshold_ptr);
            }
            else if('n'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_globalThreshold_set(*threshold_ptr, offThresh), ret);
                diag_util_printf("Set PBO Global On Threshold to %u\n",*threshold_ptr);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_globalThreshold_set(onThresh, *threshold_ptr), ret);
                diag_util_printf("Set PBO Global Off Threshold to %u\n",*threshold_ptr);
            }
            else if('n'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_globalThreshold_set(*threshold_ptr, offThresh), ret);
                diag_util_printf("Set PBO Global On Threshold to %u\n",*threshold_ptr);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_globalThreshold_set(onThresh, *threshold_ptr), ret);
                diag_util_printf("Set PBO Global Off Threshold to %u\n",*threshold_ptr);
            }
            else if('n'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_globalThreshold_set(*threshold_ptr, offThresh), ret);
                diag_util_printf("Set PBO Global On Threshold to %u\n",*threshold_ptr);
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_system_flowctrl_threshold_off_on_threshold_threshold */

/*
 * pbo get system flowctrl-threshold ( off | on ) threshold
 */
cparser_result_t
cparser_cmd_pbo_get_system_flowctrl_threshold_off_on_threshold(
    cparser_context_t *context)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                diag_util_printf("PBO Global Off Threshold is %u\n",offThresh);
            }
            else if('n'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                diag_util_printf("PBO Global On Threshold is %u\n",onThresh);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                diag_util_printf("PBO Global Off Threshold is %u\n",offThresh);
            }
            else if('n'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                diag_util_printf("PBO Global On Threshold is %u\n",onThresh);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                diag_util_printf("PBO Global Off Threshold is %u\n",offThresh);
            }
            else if('n'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                diag_util_printf("PBO Global On Threshold is %u\n",onThresh);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                diag_util_printf("PBO Global Off Threshold is %u\n",offThresh);
            }
            else if('n'==TOKEN_CHAR(4,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_globalThreshold_get(&onThresh, &offThresh), ret);
                diag_util_printf("PBO Global On Threshold is %u\n",onThresh);
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_system_flowctrl_threshold_off_on_threshold_threshold */

/*
 * pbo set sid <UINT:sid> flowctrl-threshold ( off | on ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_pbo_set_sid_sid_flowctrl_threshold_off_on_threshold_threshold(
    cparser_context_t *context,
    uint32_t  *sid_ptr,
    uint32_t  *threshold_ptr)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_sidThreshold_set(*sid_ptr, onThresh, *threshold_ptr), ret);
                diag_util_printf("Set PBO SID Off Threshold to %u\n",*threshold_ptr);
            }
            else if('n'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_sidThreshold_set(*sid_ptr, *threshold_ptr, offThresh), ret);
                diag_util_printf("Set PBO SID On Threshold to %u\n",*threshold_ptr);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_sidThreshold_set(*sid_ptr, onThresh, *threshold_ptr), ret);
                diag_util_printf("Set PBO SID Off Threshold to %u\n",*threshold_ptr);
            }
            else if('n'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_sidThreshold_set(*sid_ptr, *threshold_ptr, offThresh), ret);
                diag_util_printf("Set PBO SID On Threshold to %u\n",*threshold_ptr);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_sidThreshold_set(*sid_ptr, onThresh, *threshold_ptr), ret);
                diag_util_printf("Set PBO SID Off Threshold to %u\n",*threshold_ptr);
            }
            else if('n'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_sidThreshold_set(*sid_ptr, *threshold_ptr, offThresh), ret);
                diag_util_printf("Set PBO SID On Threshold to %u\n",*threshold_ptr);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_sidThreshold_set(*sid_ptr, onThresh, *threshold_ptr), ret);
                diag_util_printf("Set PBO SID Off Threshold to %u\n",*threshold_ptr);
            }
            else if('n'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_sidThreshold_set(*sid_ptr, *threshold_ptr, offThresh), ret);
                diag_util_printf("Set PBO SID On Threshold to %u\n",*threshold_ptr);
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_sid_flowctrl_threshold_off_on_threshold_threshold */

/*
 * pbo get sid <UINT:sid> flowctrl-threshold ( off | on ) threshold
 */
cparser_result_t
cparser_cmd_pbo_get_sid_sid_flowctrl_threshold_off_on_threshold(
    cparser_context_t *context,
    uint32_t  *sid_ptr)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                diag_util_printf("PBO SID Off Threshold is %u\n",offThresh);
            }
            else if('n'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                diag_util_printf("PBO SID On Threshold is %u\n",onThresh);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                diag_util_printf("PBO SID Off Threshold is %u\n",offThresh);
            }
            else if('n'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                diag_util_printf("PBO SID On Threshold is %u\n",onThresh);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                diag_util_printf("PBO SID Off Threshold is %u\n",offThresh);
            }
            else if('n'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                diag_util_printf("PBO SID On Threshold is %u\n",onThresh);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            if('f'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                diag_util_printf("PBO SID Off Threshold is %u\n",offThresh);
            }
            else if('n'==TOKEN_CHAR(5,1))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_sidThreshold_get(*sid_ptr, &onThresh, &offThresh), ret);
                diag_util_printf("PBO SID On Threshold is %u\n",onThresh);
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_sid_flowctrl_threshold_off_on_threshold_threshold */

/*
 * pbo set state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pbo_set_state_enable_disable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t enable=DISABLED;
    DIAG_UTIL_PARAM_CHK();

    if ('e' == TOKEN_CHAR(3,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        enable = DISABLED;
    }
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_state_set(enable), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usState_set(enable), ret);
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsState_set(enable), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usState_set(enable), ret);
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsState_set(enable), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usState_set(enable), ret);
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsState_set(enable), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }        

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_state_enable_disable */

/*
 * pbo set ( upstream | downstream ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pbo_set_upstream_downstream_state_enable_disable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t enable=DISABLED;
    DIAG_UTIL_PARAM_CHK();

    if ('e' == TOKEN_CHAR(3,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        enable = DISABLED;
    }

    if ('u' == TOKEN_CHAR(2,0))
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9601B)
            case RTL9601B_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_state_set(enable), ret);
                break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usState_set(enable), ret);
                break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usState_set(enable), ret);
                break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usState_set(enable), ret);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
        }
    }
    else
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsState_set(enable), ret);
                break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsState_set(enable), ret);
                break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsState_set(enable), ret);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_upstream_downstream_state_enable_disable */

/*
 * pbo get state
 */
cparser_result_t
cparser_cmd_pbo_get_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t enable;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_state_get(&enable), ret);

            if(enable == ENABLED){
                diag_util_printf("State enabled!\n");
            }else{
                diag_util_printf("State disabled!\n");
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usState_get(&enable), ret);
            diag_util_printf("Upstream State %s\n",(enable == ENABLED) ? "enable" : "disable");
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsState_get(&enable), ret);
            diag_util_printf("Downstream State %s\n",(enable == ENABLED) ? "enable" : "disable");
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usState_get(&enable), ret);
            diag_util_printf("Upstream State %s\n",(enable == ENABLED) ? "enable" : "disable");
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsState_get(&enable), ret);
            diag_util_printf("Downstream State %s\n",(enable == ENABLED) ? "enable" : "disable");
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usState_get(&enable), ret);
            diag_util_printf("Upstream State %s\n",(enable == ENABLED) ? "enable" : "disable");
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsState_get(&enable), ret);
            diag_util_printf("Downstream State %s\n",(enable == ENABLED) ? "enable" : "disable");
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_state */

/*
 * pbo get status
 */
cparser_result_t
cparser_cmd_pbo_get_status(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t enable;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_status_get(&enable), ret);

            if(enable == ENABLED){
                diag_util_printf("State ready!\n");
            }else{
                diag_util_printf("State not ready!\n");
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usStatus_get(&enable), ret);
            diag_util_printf("Upstream status %s\n",(enable == ENABLED) ? "ready" : "not ready");
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsStatus_get(&enable), ret);
            diag_util_printf("Downstream status %s\n",(enable == ENABLED) ? "ready" : "not ready");
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usStatus_get(&enable), ret);
            diag_util_printf("Upstream status %s\n",(enable == ENABLED) ? "ready" : "not ready");
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsStatus_get(&enable), ret);
            diag_util_printf("Downstream status %s\n",(enable == ENABLED) ? "ready" : "not ready");
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usStatus_get(&enable), ret);
            diag_util_printf("Upstream status %s\n",(enable == ENABLED) ? "ready" : "not ready");
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsStatus_get(&enable), ret);
            diag_util_printf("Downstream status %s\n",(enable == ENABLED) ? "ready" : "not ready");
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_status */

/*
 * pbo get used-page
 */
cparser_result_t
cparser_cmd_pbo_get_used_page(
    cparser_context_t *context)
{
    int32 ret;
    uint32 sram, dram;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
    case RTL9601B_CHIP_ID:
        DIAG_UTIL_PARAM_CHK();
        DIAG_UTIL_OUTPUT_INIT();

        DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_usedPageCount_get(&sram, &dram), ret);
        diag_util_printf("Current:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);
        DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_maxUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("Maximum:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
    case RTL9602C_CHIP_ID:
        DIAG_UTIL_PARAM_CHK();
        DIAG_UTIL_OUTPUT_INIT();

        DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("======== Upstream ========\n");
        diag_util_printf("Current:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);
        DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usMaxUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("Maximum:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);

        diag_util_printf("\n");

        DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("======= Downstream =======\n");
        diag_util_printf("Current:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);
        DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsMaxUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("Maximum:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
    case RTL9607C_CHIP_ID:
    {
        uint32 normal0, normal1, high;
        DIAG_UTIL_PARAM_CHK();
        DIAG_UTIL_OUTPUT_INIT();

        DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("======== Upstream ========\n");
        diag_util_printf("Current:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);
        DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usMaxUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("Maximum:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);

        diag_util_printf("\n");

        DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("======= Downstream =======\n");
        diag_util_printf("Current:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);
        DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsMaxUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("Maximum:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);

        diag_util_printf("\n");

        diag_util_printf("======= Per downstream queue =======\n");
        diag_util_printf("Current:\n");
        DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsQUsedPageCount_get(RTL9607C_PBO_DSQUEUE_TYPE_NORMAL0, &normal0), ret);
        DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsQUsedPageCount_get(RTL9607C_PBO_DSQUEUE_TYPE_NORMAL1, &normal1), ret);
        DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsQUsedPageCount_get(RTL9607C_PBO_DSQUEUE_TYPE_HIGH, &high), ret);
        diag_util_printf("NORMAL0: %u    NORMAL1: %u    HIGH: %u\n", normal0, normal1, high);
        diag_util_printf("Maximum:\n");
        DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsQMaxUsedPageCount_get(RTL9607C_PBO_DSQUEUE_TYPE_NORMAL0, &normal0), ret);
        DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsQMaxUsedPageCount_get(RTL9607C_PBO_DSQUEUE_TYPE_NORMAL1, &normal1), ret);
        DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsQMaxUsedPageCount_get(RTL9607C_PBO_DSQUEUE_TYPE_HIGH, &high), ret);
        diag_util_printf("NORMAL0: %u    NORMAL1: %u    HIGH: %u\n", normal0, normal1, high);
        break;
    }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    case RTL9603CVD_CHIP_ID:
    {
        uint32 normal0, normal1, high;
        DIAG_UTIL_PARAM_CHK();
        DIAG_UTIL_OUTPUT_INIT();

        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("======== Upstream ========\n");
        diag_util_printf("Current:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);
        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usMaxUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("Maximum:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);

        diag_util_printf("\n");

        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("======= Downstream =======\n");
        diag_util_printf("Current:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);
        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsMaxUsedPageCount_get(&sram, &dram), ret);
        diag_util_printf("Maximum:\n");
        diag_util_printf("SRAM: %u       DRAM: %u\n", sram, dram);

        diag_util_printf("\n");

        diag_util_printf("======= Per downstream queue =======\n");
        diag_util_printf("Current:\n");
        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsQUsedPageCount_get(RTL9603CVD_PBO_DSQUEUE_TYPE_NORMAL0, &normal0), ret);
        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsQUsedPageCount_get(RTL9603CVD_PBO_DSQUEUE_TYPE_NORMAL1, &normal1), ret);
        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsQUsedPageCount_get(RTL9603CVD_PBO_DSQUEUE_TYPE_HIGH, &high), ret);
        diag_util_printf("NORMAL0: %u    NORMAL1: %u    HIGH: %u\n", normal0, normal1, high);
        diag_util_printf("Maximum:\n");
        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsQMaxUsedPageCount_get(RTL9603CVD_PBO_DSQUEUE_TYPE_NORMAL0, &normal0), ret);
        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsQMaxUsedPageCount_get(RTL9603CVD_PBO_DSQUEUE_TYPE_NORMAL1, &normal1), ret);
        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsQMaxUsedPageCount_get(RTL9603CVD_PBO_DSQUEUE_TYPE_HIGH, &high), ret);
        diag_util_printf("NORMAL0: %u    NORMAL1: %u    HIGH: %u\n", normal0, normal1, high);
        break;
    }
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_used_page_sram_dram */

/*
 * pbo get flowcontrol current
 */
cparser_result_t
cparser_cmd_pbo_get_flowcontrol_current(
    cparser_context_t *context)
{
    int32 ret;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            uint32 i;
            uint32 sid;
            rtk_enable_t enable;

            DIAG_UTIL_PARAM_CHK();
            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("SID flow control current status:\n");
            for(sid = 0; sid < HAL_CLASSIFY_SID_NUM(); sid ++)
            {
                if((sid % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(i = sid; (i < HAL_CLASSIFY_SID_NUM()) && (i < sid + 16); i ++)
                    {
                        diag_util_printf("%3u ", i);
                    }
                    diag_util_printf("\nSTS | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_flowcontrolStatus_get(sid, RTL9601B_PBO_FLOWCONTROL_STATUS_CURRENT, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'O' : 'X');
                if((sid % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((sid % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            uint32 i;
            uint32 sid;
            rtk_enable_t enable;

            DIAG_UTIL_PARAM_CHK();
            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("SID flow control current status:\n");
            for(sid = 0; sid < HAL_CLASSIFY_SID_NUM(); sid ++)
            {
                if((sid % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(i = sid; (i < HAL_CLASSIFY_SID_NUM()) && (i < sid + 16); i ++)
                    {
                        diag_util_printf("%3u ", i);
                    }
                    diag_util_printf("\nSTS | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_flowcontrolStatus_get(sid, RTL9602C_PBO_FLOWCONTROL_STATUS_CURRENT, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'O' : 'X');
                if((sid % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((sid % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            uint32 i;
            uint32 sid;
            rtk_enable_t enable;

            DIAG_UTIL_PARAM_CHK();
            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("SID flow control current status:\n");
            for(sid = 0; sid < HAL_CLASSIFY_SID_NUM(); sid ++)
            {
                if((sid % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(i = sid; (i < HAL_CLASSIFY_SID_NUM()) && (i < sid + 16); i ++)
                    {
                        diag_util_printf("%3u ", i);
                    }
                    diag_util_printf("\nSTS | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_flowcontrolStatus_get(sid, RTL9607C_PBO_FLOWCONTROL_STATUS_CURRENT, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'O' : 'X');
                if((sid % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((sid % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            uint32 i;
            uint32 sid;
            rtk_enable_t enable;

            DIAG_UTIL_PARAM_CHK();
            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("SID flow control current status:\n");
            for(sid = 0; sid < HAL_CLASSIFY_SID_NUM(); sid ++)
            {
                if((sid % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(i = sid; (i < HAL_CLASSIFY_SID_NUM()) && (i < sid + 16); i ++)
                    {
                        diag_util_printf("%3u ", i);
                    }
                    diag_util_printf("\nSTS | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_flowcontrolStatus_get(sid, RTL9603CVD_PBO_FLOWCONTROL_STATUS_CURRENT, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'O' : 'X');
                if((sid % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((sid % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            
            break;
        }
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_flowcontrol_current */

/*
 * pbo get flowcontrol latch
 */
cparser_result_t
cparser_cmd_pbo_get_flowcontrol_latch(
    cparser_context_t *context)
{
    int32 ret;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            uint32 i;
            uint32 sid;
            rtk_enable_t enable;

            DIAG_UTIL_PARAM_CHK();
            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("SID flow control latched status:\n");
            for(sid = 0; sid < HAL_CLASSIFY_SID_NUM(); sid ++)
            {
                if((sid % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(i = sid; (i < HAL_CLASSIFY_SID_NUM()) && (i < sid + 16); i ++)
                    {
                        diag_util_printf("%3u ", i);
                    }
                    diag_util_printf("\nSTS | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_flowcontrolStatus_get(sid, RTL9601B_PBO_FLOWCONTROL_STATUS_LATCHED, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'O' : 'X');
                if((sid % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((sid % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            uint32 i;
            uint32 sid;
            rtk_enable_t enable;

            DIAG_UTIL_PARAM_CHK();
            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("SID flow control latched status:\n");
            for(sid = 0; sid < HAL_CLASSIFY_SID_NUM(); sid ++)
            {
                if((sid % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(i = sid; (i < HAL_CLASSIFY_SID_NUM()) && (i < sid + 16); i ++)
                    {
                        diag_util_printf("%3u ", i);
                    }
                    diag_util_printf("\nSTS | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_flowcontrolStatus_get(sid, RTL9602C_PBO_FLOWCONTROL_STATUS_LATCHED, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'O' : 'X');
                if((sid % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((sid % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            uint32 i;
            uint32 sid;
            rtk_enable_t enable;

            DIAG_UTIL_PARAM_CHK();
            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("SID flow control latched status:\n");
            for(sid = 0; sid < HAL_CLASSIFY_SID_NUM(); sid ++)
            {
                if((sid % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(i = sid; (i < HAL_CLASSIFY_SID_NUM()) && (i < sid + 16); i ++)
                    {
                        diag_util_printf("%3u ", i);
                    }
                    diag_util_printf("\nSTS | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_flowcontrolStatus_get(sid, RTL9607C_PBO_FLOWCONTROL_STATUS_LATCHED, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'O' : 'X');
                if((sid % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((sid % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            uint32 i;
            uint32 sid;
            rtk_enable_t enable;

            DIAG_UTIL_PARAM_CHK();
            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("SID flow control latched status:\n");
            for(sid = 0; sid < HAL_CLASSIFY_SID_NUM(); sid ++)
            {
                if((sid % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(i = sid; (i < HAL_CLASSIFY_SID_NUM()) && (i < sid + 16); i ++)
                    {
                        diag_util_printf("%3u ", i);
                    }
                    diag_util_printf("\nSTS | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_flowcontrolStatus_get(sid, RTL9603CVD_PBO_FLOWCONTROL_STATUS_LATCHED, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'O' : 'X');
                if((sid % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((sid % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            
            break;
        }
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_flowcontrol_latch */

/*
 * pbo clear used-page
 */
cparser_result_t
cparser_cmd_pbo_clear_used_page(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_maxUsedPageCount_clear(), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usMaxUsedPageCount_clear(), ret);
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsMaxUsedPageCount_clear(), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usMaxUsedPageCount_clear(), ret);
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsMaxUsedPageCount_clear(), ret);
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsQMaxUsedPageCount_clear(), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usMaxUsedPageCount_clear(), ret);
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsMaxUsedPageCount_clear(), ret);
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsQMaxUsedPageCount_clear(), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_clear_used_page */

/*
 * pbo clear ( upstream | downstream ) used-page
 */
cparser_result_t
cparser_cmd_pbo_clear_upstream_downstream_used_page(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if ('u' == TOKEN_CHAR(2,0))
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9601B)
            case RTL9601B_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_maxUsedPageCount_clear(), ret);
                break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usMaxUsedPageCount_clear(), ret);
                break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usMaxUsedPageCount_clear(), ret);
                break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usMaxUsedPageCount_clear(), ret);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
        }
    }
    else
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsMaxUsedPageCount_clear(), ret);
                break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsMaxUsedPageCount_clear(), ret);
                break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsMaxUsedPageCount_clear(), ret);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_clear_upstream_downstream_used_page */

/*
 * pbo clear flowcontrol latch
 */
cparser_result_t
cparser_cmd_pbo_clear_flowcontrol_latch(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            uint32 sid;

            for(sid = 0;sid < HAL_CLASSIFY_SID_NUM();sid ++)
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_flowcontrolLatchStatus_clear(sid), ret);
            }
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            uint32 sid;

            for(sid = 0;sid < HAL_CLASSIFY_SID_NUM();sid ++)
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_flowcontrolLatchStatus_clear(sid), ret);
            }
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            uint32 sid;

            for(sid = 0;sid < HAL_CLASSIFY_SID_NUM();sid ++)
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_flowcontrolLatchStatus_clear(sid), ret);
            }
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            uint32 sid;

            for(sid = 0;sid < HAL_CLASSIFY_SID_NUM();sid ++)
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_flowcontrolLatchStatus_clear(sid), ret);
            }
            break;
        }
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_clear_flowcontrol_latch */

/*
 * pbo clear counter group ( <MASK_LIST:group> | all )
 */
cparser_result_t
cparser_cmd_pbo_clear_counter_group_group_all(
    cparser_context_t *context,
    char * *group_ptr)
{
    int32 ret = CPARSER_OK;
    diag_mask_t  groupList;
    uint32 group;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(groupList, 4), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_groupCounter_clear(group), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usGroupCounter_clear(group), ret);
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsGroupCounter_clear(group), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usGroupCounter_clear(group), ret);
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsGroupCounter_clear(group), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usGroupCounter_clear(group), ret);
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsGroupCounter_clear(group), ret);
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_clear_counter_group_group_all */

/*
 * pbo clear ( upstream | downstream ) counter group ( <MASK_LIST:group> | all )
 */
cparser_result_t
cparser_cmd_pbo_clear_upstream_downstream_counter_group_group_all(
    cparser_context_t *context,
    char * *group_ptr)
{
    int32 ret = CPARSER_OK;
    diag_mask_t  groupList;
    uint32 group;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(groupList, 5), ret);

    if ('u' == TOKEN_CHAR(2,0))
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9601B)
            case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();

                DIAG_UTIL_MASK_SCAN(groupList, group)
                {
                    if(group >= 5)
                    {
                        break;
                    }

                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_groupCounter_clear(group), ret);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();

                DIAG_UTIL_MASK_SCAN(groupList, group)
                {
                    if(group >= 5)
                    {
                        break;
                    }

                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usGroupCounter_clear(group), ret);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();

                DIAG_UTIL_MASK_SCAN(groupList, group)
                {
                    if(group >= 5)
                    {
                        break;
                    }

                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usGroupCounter_clear(group), ret);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();

                DIAG_UTIL_MASK_SCAN(groupList, group)
                {
                    if(group >= 5)
                    {
                        break;
                    }

                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usGroupCounter_clear(group), ret);
                }
            }
            break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
        }
    }
    else
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();

                DIAG_UTIL_MASK_SCAN(groupList, group)
                {
                    if(group >= 5)
                    {
                        break;
                    }

                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsGroupCounter_clear(group), ret);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();

                DIAG_UTIL_MASK_SCAN(groupList, group)
                {
                    if(group >= 5)
                    {
                        break;
                    }

                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsGroupCounter_clear(group), ret);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();

                DIAG_UTIL_MASK_SCAN(groupList, group)
                {
                    if(group >= 5)
                    {
                        break;
                    }

                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsGroupCounter_clear(group), ret);
                }
            }
            break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_clear_upstream_downstream_counter_group_group_all */

/*
 * pbo clear counter pbo
 */
cparser_result_t
cparser_cmd_pbo_clear_counter_pbo(
    cparser_context_t *context)
{
    int32 ret = CPARSER_OK;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usCounter_clear(), ret);
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsCounter_clear(), ret);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usCounter_clear(), ret);
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsCounter_clear(), ret);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usCounter_clear(), ret);
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsCounter_clear(), ret);
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_clear_counter_pbo */

/*
 * pbo clear ( upstream | downstream ) counter pbo
 */
cparser_result_t
cparser_cmd_pbo_clear_upstream_downstream_counter_pbo(
    cparser_context_t *context)
{
    int32 ret = CPARSER_OK;
    diag_mask_t  groupList;
    uint32 group;

    DIAG_UTIL_PARAM_CHK();

    if ('u' == TOKEN_CHAR(2,0))
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usCounter_clear(), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usCounter_clear(), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usCounter_clear(), ret);
            }
            break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
        }
    }
    else
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsCounter_clear(), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsCounter_clear(), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsCounter_clear(), ret);
            }
            break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_clear_upstream_downstream_counter_pbo */

/*
 * pbo clear counter ponnic
 */
cparser_result_t
cparser_cmd_pbo_clear_counter_ponnic(
    cparser_context_t *context)
{
    int32 ret = CPARSER_OK;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usPonnicCounter_clear(), ret);
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsPonnicCounter_clear(), ret);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usPonnicCounter_clear(), ret);
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsPonnicCounter_clear(), ret);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usPonnicCounter_clear(), ret);
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsPonnicCounter_clear(), ret);
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_clear_counter_ponnic */

/*
 * pbo clear ( upstream | downstream ) counter ponnic
 */
cparser_result_t
cparser_cmd_pbo_clear_upstream_downstream_counter_ponnic(
    cparser_context_t *context)
{
    int32 ret = CPARSER_OK;

    DIAG_UTIL_PARAM_CHK();

    if ('u' == TOKEN_CHAR(2,0))
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usPonnicCounter_clear(), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usPonnicCounter_clear(), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usPonnicCounter_clear(), ret);
            }
            break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
        }
    }
    else
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsPonnicCounter_clear(), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsPonnicCounter_clear(), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_OUTPUT_INIT();
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsPonnicCounter_clear(), ret);
            }
            break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_clear_upstream_downstream_counter_ponnic */

/*
 * pbo set counter group <UINT:grp> sid ( <MASK_LIST:sid> | all )
 */
cparser_result_t
cparser_cmd_pbo_set_counter_group_grp_sid_sid_all(
    cparser_context_t *context,
    uint32_t  *grp_ptr,
    char * *sid_ptr)
{
    int32 ret = CPARSER_OK;
    diag_mask_t  sidList;
    uint32 sid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(sidList, 6), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_counterGroupMember_clear(*grp_ptr), ret);
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_counterGroupMember_add(*grp_ptr, sid), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usCounterGroupMember_clear(*grp_ptr), ret);
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usCounterGroupMember_add(*grp_ptr, sid), ret);
            }

            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsCounterGroupMember_clear(*grp_ptr), ret);
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsCounterGroupMember_add(*grp_ptr, sid), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usCounterGroupMember_clear(*grp_ptr), ret);
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usCounterGroupMember_add(*grp_ptr, sid), ret);
            }

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsCounterGroupMember_clear(*grp_ptr), ret);
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsCounterGroupMember_add(*grp_ptr, sid), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usCounterGroupMember_clear(*grp_ptr), ret);
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usCounterGroupMember_add(*grp_ptr, sid), ret);
            }

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsCounterGroupMember_clear(*grp_ptr), ret);
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsCounterGroupMember_add(*grp_ptr, sid), ret);
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_counter_group_grp_sid_sid_all */

/*
 * pbo set ( upstream | downstream ) counter group <UINT:grp> sid ( <MASK_LIST:sid> | all )
 */
cparser_result_t
cparser_cmd_pbo_set_upstream_downstream_counter_group_grp_sid_sid_all(
    cparser_context_t *context,
    uint32_t  *grp_ptr,
    char * *sid_ptr)
{
    int32 ret = CPARSER_OK;
    diag_mask_t  sidList;
    uint32 sid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(sidList, 7), ret);

    if ('u' == TOKEN_CHAR(2,0))
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_counterGroupMember_clear(*grp_ptr), ret);
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_counterGroupMember_add(*grp_ptr, sid), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usCounterGroupMember_clear(*grp_ptr), ret);
                DIAG_UTIL_MASK_SCAN(sidList, sid)
                {
                    if(sid >= HAL_CLASSIFY_SID_NUM())
                    {
                        break;
                    }
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usCounterGroupMember_add(*grp_ptr, sid), ret);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usCounterGroupMember_clear(*grp_ptr), ret);
                DIAG_UTIL_MASK_SCAN(sidList, sid)
                {
                    if(sid >= HAL_CLASSIFY_SID_NUM())
                    {
                        break;
                    }
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usCounterGroupMember_add(*grp_ptr, sid), ret);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usCounterGroupMember_clear(*grp_ptr), ret);
                DIAG_UTIL_MASK_SCAN(sidList, sid)
                {
                    if(sid >= HAL_CLASSIFY_SID_NUM())
                    {
                        break;
                    }
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usCounterGroupMember_add(*grp_ptr, sid), ret);
                }
            }
            break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
            break;
        }
    }
    else
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsCounterGroupMember_clear(*grp_ptr), ret);
                DIAG_UTIL_MASK_SCAN(sidList, sid)
                {
                    if(sid >= HAL_CLASSIFY_SID_NUM())
                    {
                        break;
                    }
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsCounterGroupMember_add(*grp_ptr, sid), ret);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsCounterGroupMember_clear(*grp_ptr), ret);
                DIAG_UTIL_MASK_SCAN(sidList, sid)
                {
                    if(sid >= HAL_CLASSIFY_SID_NUM())
                    {
                        break;
                    }
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsCounterGroupMember_add(*grp_ptr, sid), ret);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsCounterGroupMember_clear(*grp_ptr), ret);
                DIAG_UTIL_MASK_SCAN(sidList, sid)
                {
                    if(sid >= HAL_CLASSIFY_SID_NUM())
                    {
                        break;
                    }
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsCounterGroupMember_add(*grp_ptr, sid), ret);
                }
            }
            break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
            break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_upstream_downstream_counter_group_grp_sid_sid_all */

/*
 * pbo get counter group <UINT:grp>
 */
cparser_result_t
cparser_cmd_pbo_get_counter_group_grp(
    cparser_context_t *context,
    uint32_t  *grp_ptr)
{
    int32 ret;
    uint32 j, k;
    uint32 groupStart, groupEnd;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            
            if(*grp_ptr >= 4)
            {
                diag_util_printf("Group out of range\n");
                return CPARSER_NOT_OK;
            }

            diag_util_printf("Group detail:\n");
            diag_util_printf("Group %u:\n", *grp_ptr);
            for(j = 0;j < HAL_CLASSIFY_SID_NUM();j ++)
            {
                if((j % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(k = j; (k < HAL_CLASSIFY_SID_NUM()) && (k < j + 16); k ++)
                    {
                        diag_util_printf("%3u ", k);
                    }
                    diag_util_printf("\n    | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_counterGroupMember_get(*grp_ptr, j, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'V' : ' ');
                if((j % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((j % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            
            if(*grp_ptr >= 4)
            {
                diag_util_printf("Group out of range\n");
                return CPARSER_NOT_OK;
            }

            diag_util_printf("======== Upstream ========\n");
            diag_util_printf("Group detail:\n");
            diag_util_printf("Group %u:\n", *grp_ptr);
            for(j = 0;j < HAL_CLASSIFY_SID_NUM();j ++)
            {
                if((j % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(k = j; (k < HAL_CLASSIFY_SID_NUM()) && (k < j + 16); k ++)
                    {
                        diag_util_printf("%3u ", k);
                    }
                    diag_util_printf("\n    | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usCounterGroupMember_get(*grp_ptr, j, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'V' : ' ');
                if((j % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((j % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");

            diag_util_printf("======= Downstream ========\n");
            diag_util_printf("Group detail:\n");
            diag_util_printf("Group %u:\n", *grp_ptr);
            for(j = 0;j < HAL_CLASSIFY_SID_NUM();j ++)
            {
                if((j % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(k = j; (k < HAL_CLASSIFY_SID_NUM()) && (k < j + 16); k ++)
                    {
                        diag_util_printf("%3u ", k);
                    }
                    diag_util_printf("\n    | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsCounterGroupMember_get(*grp_ptr, j, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'V' : ' ');
                if((j % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((j % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            
            if(*grp_ptr >= 4)
            {
                diag_util_printf("Group out of range\n");
                return CPARSER_NOT_OK;
            }

            diag_util_printf("======== Upstream ========\n");
            diag_util_printf("Group detail:\n");
            diag_util_printf("Group %u:\n", *grp_ptr);
            for(j = 0;j < HAL_CLASSIFY_SID_NUM();j ++)
            {
                if((j % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(k = j; (k < HAL_CLASSIFY_SID_NUM()) && (k < j + 16); k ++)
                    {
                        diag_util_printf("%3u ", k);
                    }
                    diag_util_printf("\n    | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usCounterGroupMember_get(*grp_ptr, j, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'V' : ' ');
                if((j % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((j % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");

            diag_util_printf("======= Downstream ========\n");
            diag_util_printf("Group detail:\n");
            diag_util_printf("Group %u:\n", *grp_ptr);
            for(j = 0;j < HAL_CLASSIFY_SID_NUM();j ++)
            {
                if((j % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(k = j; (k < HAL_CLASSIFY_SID_NUM()) && (k < j + 16); k ++)
                    {
                        diag_util_printf("%3u ", k);
                    }
                    diag_util_printf("\n    | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsCounterGroupMember_get(*grp_ptr, j, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'V' : ' ');
                if((j % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((j % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            
            if(*grp_ptr >= 4)
            {
                diag_util_printf("Group out of range\n");
                return CPARSER_NOT_OK;
            }

            diag_util_printf("======== Upstream ========\n");
            diag_util_printf("Group detail:\n");
            diag_util_printf("Group %u:\n", *grp_ptr);
            for(j = 0;j < HAL_CLASSIFY_SID_NUM();j ++)
            {
                if((j % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(k = j; (k < HAL_CLASSIFY_SID_NUM()) && (k < j + 16); k ++)
                    {
                        diag_util_printf("%3u ", k);
                    }
                    diag_util_printf("\n    | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usCounterGroupMember_get(*grp_ptr, j, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'V' : ' ');
                if((j % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((j % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");

            diag_util_printf("======= Downstream ========\n");
            diag_util_printf("Group detail:\n");
            diag_util_printf("Group %u:\n", *grp_ptr);
            for(j = 0;j < HAL_CLASSIFY_SID_NUM();j ++)
            {
                if((j % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("SID | ");
                    for(k = j; (k < HAL_CLASSIFY_SID_NUM()) && (k < j + 16); k ++)
                    {
                        diag_util_printf("%3u ", k);
                    }
                    diag_util_printf("\n    | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsCounterGroupMember_get(*grp_ptr, j, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'V' : ' ');
                if((j % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((j % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_counter_group_grp_all */

/*
 * pbo dump counter pbo
 */
cparser_result_t
cparser_cmd_pbo_dump_counter_pbo(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            rtl9601b_pbo_cnt_t pbo_cnt;
                
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_counter_get(&pbo_cnt), ret);
            diag_util_printf("RX_SID_ERR_CNT: %u\n", pbo_cnt.rx_err_cnt);
            diag_util_printf("RX_ERR_SIDIN_CNT: %u\n", pbo_cnt.rx_in_err);
            diag_util_printf("TX_EMPTY_EOB: %u\n", pbo_cnt.tx_empty_eob_cnt);
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            rtl9602c_pbo_cnt_t pbo_cnt;
                
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usCounter_get(&pbo_cnt), ret);
            diag_util_printf("======== Upstream ========\n");
            diag_util_printf("RX_ERR_CNT: %u\n", pbo_cnt.rx_err_cnt);
            diag_util_printf("TX_EMPTY_EOB: %u\n", pbo_cnt.tx_empty_eob_cnt);

            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsCounter_get(&pbo_cnt), ret);
            diag_util_printf("======= Downstream ========\n");
            diag_util_printf("RX_ERR_CNT: %u\n", pbo_cnt.rx_err_cnt);
            diag_util_printf("RX_DROP_CNT: %u\n", pbo_cnt.rx_drop_cnt);
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            rtl9607c_pbo_cnt_t pbo_cnt;
                
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usCounter_get(&pbo_cnt), ret);
            diag_util_printf("======== Upstream ========\n");
            diag_util_printf("RX_ERR_CNT: %u\n", pbo_cnt.rx_err_cnt);
            diag_util_printf("TX_EMPTY_EOB: %u\n", pbo_cnt.tx_empty_eob_cnt);

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsCounter_get(&pbo_cnt), ret);
            diag_util_printf("======= Downstream ========\n");
            diag_util_printf("RX_ERR_CNT: %u\n", pbo_cnt.rx_err_cnt);
            diag_util_printf("RX_DROP_CNT: %u\n", pbo_cnt.rx_drop_cnt);
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            rtl9603cvd_pbo_cnt_t pbo_cnt;
                
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usCounter_get(&pbo_cnt), ret);
            diag_util_printf("======== Upstream ========\n");
            diag_util_printf("RX_ERR_CNT: %u\n", pbo_cnt.rx_err_cnt);
            diag_util_printf("TX_EMPTY_EOB: %u\n", pbo_cnt.tx_empty_eob_cnt);

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsCounter_get(&pbo_cnt), ret);
            diag_util_printf("======= Downstream ========\n");
            diag_util_printf("RX_ERR_CNT: %u\n", pbo_cnt.rx_err_cnt);
            diag_util_printf("RX_DROP_CNT: %u\n", pbo_cnt.rx_drop_cnt);
            break;
        }
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_dump_counter_pbo */

/*
 * pbo dump counter ponnic
 */
cparser_result_t
cparser_cmd_pbo_dump_counter_ponnic(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            rtl9602c_pbo_ponnic_cnt_t pbo_cnt;
                
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usPonnicCounter_get(&pbo_cnt), ret);
            diag_util_printf("======== Upstream ========\n");
            diag_util_printf("Tx PKT_OK_CNT: %u\n", pbo_cnt.pkt_ok_cnt_tx);
            diag_util_printf("Rx PKT_OK_CNT: %u\n", pbo_cnt.pkt_ok_cnt_rx);
            diag_util_printf("Tx PKT_ERR_CNT: %u\n", pbo_cnt.pkt_err_cnt_tx);
            diag_util_printf("Rx PKT_ERR_CNT: %u\n", pbo_cnt.pkt_err_cnt_rx);
            diag_util_printf("PKT_MISS_CNT: %u\n", pbo_cnt.pkt_miss_cnt);

            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsPonnicCounter_get(&pbo_cnt), ret);
            diag_util_printf("======= Downstream ========\n");
            diag_util_printf("Tx PKT_OK_CNT: %u\n", pbo_cnt.pkt_ok_cnt_tx);
            diag_util_printf("Rx PKT_OK_CNT: %u\n", pbo_cnt.pkt_ok_cnt_rx);
            diag_util_printf("Tx PKT_ERR_CNT: %u\n", pbo_cnt.pkt_err_cnt_tx);
            diag_util_printf("Rx PKT_ERR_CNT: %u\n", pbo_cnt.pkt_err_cnt_rx);
            diag_util_printf("PKT_MISS_CNT: %u\n", pbo_cnt.pkt_miss_cnt);
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            rtl9607c_pbo_ponnic_cnt_t pbo_cnt;
                
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usPonnicCounter_get(&pbo_cnt), ret);
            diag_util_printf("======== Upstream ========\n");
            diag_util_printf("Tx PKT_OK_CNT: %u\n", pbo_cnt.pkt_ok_cnt_tx);
            diag_util_printf("Rx PKT_OK_CNT: %u\n", pbo_cnt.pkt_ok_cnt_rx);
            diag_util_printf("Tx PKT_ERR_CNT: %u\n", pbo_cnt.pkt_err_cnt_tx);
            diag_util_printf("Rx PKT_ERR_CNT: %u\n", pbo_cnt.pkt_err_cnt_rx);
            diag_util_printf("PKT_MISS_CNT: %u\n", pbo_cnt.pkt_miss_cnt);

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsPonnicCounter_get(&pbo_cnt), ret);
            diag_util_printf("======= Downstream ========\n");
            diag_util_printf("Tx PKT_OK_CNT: %u\n", pbo_cnt.pkt_ok_cnt_tx);
            diag_util_printf("Rx PKT_OK_CNT: %u\n", pbo_cnt.pkt_ok_cnt_rx);
            diag_util_printf("Tx PKT_ERR_CNT: %u\n", pbo_cnt.pkt_err_cnt_tx);
            diag_util_printf("Rx PKT_ERR_CNT: %u\n", pbo_cnt.pkt_err_cnt_rx);
            diag_util_printf("PKT_MISS_CNT: %u\n", pbo_cnt.pkt_miss_cnt);
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            rtl9603cvd_pbo_ponnic_cnt_t pbo_cnt;
                
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usPonnicCounter_get(&pbo_cnt), ret);
            diag_util_printf("======== Upstream ========\n");
            diag_util_printf("Tx PKT_OK_CNT: %u\n", pbo_cnt.pkt_ok_cnt_tx);
            diag_util_printf("Rx PKT_OK_CNT: %u\n", pbo_cnt.pkt_ok_cnt_rx);
            diag_util_printf("Tx PKT_ERR_CNT: %u\n", pbo_cnt.pkt_err_cnt_tx);
            diag_util_printf("Rx PKT_ERR_CNT: %u\n", pbo_cnt.pkt_err_cnt_rx);
            diag_util_printf("PKT_MISS_CNT: %u\n", pbo_cnt.pkt_miss_cnt);

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsPonnicCounter_get(&pbo_cnt), ret);
            diag_util_printf("======= Downstream ========\n");
            diag_util_printf("Tx PKT_OK_CNT: %u\n", pbo_cnt.pkt_ok_cnt_tx);
            diag_util_printf("Rx PKT_OK_CNT: %u\n", pbo_cnt.pkt_ok_cnt_rx);
            diag_util_printf("Tx PKT_ERR_CNT: %u\n", pbo_cnt.pkt_err_cnt_tx);
            diag_util_printf("Rx PKT_ERR_CNT: %u\n", pbo_cnt.pkt_err_cnt_rx);
            diag_util_printf("PKT_MISS_CNT: %u\n", pbo_cnt.pkt_miss_cnt);
            break;
        }
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_dump_counter_ponnic */

/*
 * pbo dump counter group ( <MASK_LIST:group> | all )
 */
cparser_result_t
cparser_cmd_pbo_dump_counter_group_group_all(
    cparser_context_t *context,
    char * *group_ptr)
{
    int32 ret = CPARSER_OK;
    diag_mask_t  groupList;
    uint32 group;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(groupList, 4), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            rtl9601b_pbo_group_cnt_t group_cnt;

            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("Group counters:\n");
            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_groupCounter_get(group, &group_cnt), ret);
                diag_util_printf("Group %u:\n", group);
                diag_util_printf("   RX_SID_GOOD_CNT:%u\n", group_cnt.rx_good_cnt);
                diag_util_printf("   RX_SID_BAD_CNT:%u\n", group_cnt.rx_bad_cnt);
                diag_util_printf("   RX_SIDIN_CNT:%u\n", group_cnt.rx_in_cnt);
                diag_util_printf("   TX_SID_CNT:%u\n", group_cnt.tx_cnt);
                diag_util_printf("   TX_SID_FRAG_CNT:%u\n", group_cnt.tx_frag_cnt);
                diag_util_printf("\n");
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            rtl9602c_pbo_group_cnt_t group_cnt;

            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("======== Upstream ========\n");
            diag_util_printf("Group counters:\n");
            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usGroupCounter_get(group, &group_cnt), ret);
                diag_util_printf("Group %u:\n", group);
                diag_util_printf("   RX_SID_GOOD_CNT:%u\n", group_cnt.rx_good_cnt);
                diag_util_printf("   RX_SID_BAD_CNT:%u\n", group_cnt.rx_bad_cnt);
                diag_util_printf("   RX_SID_CNT:%u\n", group_cnt.rx_cnt);
                diag_util_printf("   TX_SID_CNT:%u\n", group_cnt.tx_cnt);
                diag_util_printf("   TX_SID_FRAG_CNT:%u\n", group_cnt.tx_frag_cnt);
                diag_util_printf("\n");
            }

            diag_util_printf("======= Downstream ========\n");
            diag_util_printf("Group counters:\n");
            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsGroupCounter_get(group, &group_cnt), ret);
                diag_util_printf("Group %u:\n", group);
                diag_util_printf("   RX_SID_GOOD_CNT:%u\n", group_cnt.rx_good_cnt);
                diag_util_printf("   RX_SID_BAD_CNT:%u\n", group_cnt.rx_bad_cnt);
                diag_util_printf("   RX_SID_FRAG_CNT:%u\n", group_cnt.rx_frag_cnt);
                diag_util_printf("   TX_SID_CNT:%u\n", group_cnt.tx_cnt);
                diag_util_printf("\n");
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            rtl9607c_pbo_group_cnt_t group_cnt;

            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("======== Upstream ========\n");
            diag_util_printf("Group counters:\n");
            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usGroupCounter_get(group, &group_cnt), ret);
                diag_util_printf("Group %u:\n", group);
                diag_util_printf("   RX_SID_GOOD_CNT:%u\n", group_cnt.rx_good_cnt);
                diag_util_printf("   RX_SID_BAD_CNT:%u\n", group_cnt.rx_bad_cnt);
                diag_util_printf("   RX_SID_CNT:%u\n", group_cnt.rx_cnt);
                diag_util_printf("   TX_SID_CNT:%u\n", group_cnt.tx_cnt);
                diag_util_printf("   TX_SID_FRAG_CNT:%u\n", group_cnt.tx_frag_cnt);
                diag_util_printf("\n");
            }

            diag_util_printf("======= Downstream ========\n");
            diag_util_printf("Group counters:\n");
            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsGroupCounter_get(group, &group_cnt), ret);
                diag_util_printf("Group %u:\n", group);
                diag_util_printf("   RX_SID_GOOD_CNT:%u\n", group_cnt.rx_good_cnt);
                diag_util_printf("   RX_SID_BAD_CNT:%u\n", group_cnt.rx_bad_cnt);
                diag_util_printf("   RX_SID_FRAG_CNT:%u\n", group_cnt.rx_frag_cnt);
                diag_util_printf("   TX_SID_CNT:%u\n", group_cnt.tx_cnt);
                diag_util_printf("\n");
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            rtl9603cvd_pbo_group_cnt_t group_cnt;

            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("======== Upstream ========\n");
            diag_util_printf("Group counters:\n");
            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usGroupCounter_get(group, &group_cnt), ret);
                diag_util_printf("Group %u:\n", group);
                diag_util_printf("   RX_SID_GOOD_CNT:%u\n", group_cnt.rx_good_cnt);
                diag_util_printf("   RX_SID_BAD_CNT:%u\n", group_cnt.rx_bad_cnt);
                diag_util_printf("   RX_SID_CNT:%u\n", group_cnt.rx_cnt);
                diag_util_printf("   TX_SID_CNT:%u\n", group_cnt.tx_cnt);
                diag_util_printf("   TX_SID_FRAG_CNT:%u\n", group_cnt.tx_frag_cnt);
                diag_util_printf("\n");
            }

            diag_util_printf("======= Downstream ========\n");
            diag_util_printf("Group counters:\n");
            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsGroupCounter_get(group, &group_cnt), ret);
                diag_util_printf("Group %u:\n", group);
                diag_util_printf("   RX_SID_GOOD_CNT:%u\n", group_cnt.rx_good_cnt);
                diag_util_printf("   RX_SID_BAD_CNT:%u\n", group_cnt.rx_bad_cnt);
                diag_util_printf("   RX_SID_FRAG_CNT:%u\n", group_cnt.rx_frag_cnt);
                diag_util_printf("   TX_SID_CNT:%u\n", group_cnt.tx_cnt);
                diag_util_printf("\n");
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_dump_counter_group_group_all */

/*
 * pbo dump config memory
 */
cparser_result_t
cparser_cmd_pbo_dump_config_memory(
    cparser_context_t *context)
{
    int32 ret = CPARSER_OK;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            uint32 value;

            diag_util_printf("US Memory Config\n");
            DIAG_UTIL_ERR_CHK(reg_read(RTL9602C_IP_MSTBASE_USr, &value), ret);
            diag_util_printf("\tBase Addr: 0x%08x\n", value);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_PON_DSC_CFG_USr, RTL9602C_CFG_PAGE_SIZEf, &value), ret);
            diag_util_printf("\tPage Size: %u\n", value == 0 ? 128 : (value == 1 ? 256 : 512));
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_PON_DSC_CFG_USr, RTL9602C_CFG_SRAM_NOf, &value), ret);
            diag_util_printf("\tSRAM DSC Cnt: %u\n", value + 1);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_PON_DSC_CFG_USr, RTL9602C_CFG_RAM_NOf, &value), ret);
            diag_util_printf("\tDRAM DSC Cnt: %u\n", value + 1);

            diag_util_printf("\n");

            diag_util_printf("DS Memory Config\n");
            DIAG_UTIL_ERR_CHK(reg_read(RTL9602C_IP_MSTBASE_DSr, &value), ret);
            diag_util_printf("\tBase Addr: 0x%08x\n", value);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_PON_DSC_CFG_DSr, RTL9602C_CFG_PAGE_SIZEf, &value), ret);
            diag_util_printf("\tPage Size: %u\n", value == 0 ? 128 : (value == 1 ? 256 : 512));
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_PON_DSC_CFG_DSr, RTL9602C_CFG_SRAM_NOf, &value), ret);
            diag_util_printf("\tSRAM DSC Cnt: %u\n", value + 1);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_PON_DSC_CFG_DSr, RTL9602C_CFG_RAM_NOf, &value), ret);
            diag_util_printf("\tRAM DSC Cnt: %u\n", value + 1);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            uint32 value;

            diag_util_printf("US Memory Config\n");
            DIAG_UTIL_ERR_CHK(reg_read(RTL9607C_IP_MSTBASE_USr, &value), ret);
            diag_util_printf("\tBase Addr: 0x%08x\n", value);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_PON_DSC_CFG_USr, RTL9607C_CFG_PAGE_SIZEf, &value), ret);
            diag_util_printf("\tPage Size: %u\n", value == 0 ? 128 : (value == 1 ? 256 : 512));
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_PON_DSC_CFG_USr, RTL9607C_CFG_SRAM_NOf, &value), ret);
            diag_util_printf("\tSRAM DSC Cnt: %u\n", value + 1);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_PON_DSC_CFG_USr, RTL9607C_CFG_RAM_NOf, &value), ret);
            diag_util_printf("\tDRAM DSC Cnt: %u\n", value + 1);

            diag_util_printf("\n");

            diag_util_printf("DS Memory Config\n");
            DIAG_UTIL_ERR_CHK(reg_read(RTL9607C_IP_MSTBASE_DSr, &value), ret);
            diag_util_printf("\tBase Addr: 0x%08x\n", value);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_PON_DSC_CFG_DSr, RTL9607C_CFG_PAGE_SIZEf, &value), ret);
            diag_util_printf("\tPage Size: %u\n", value == 0 ? 128 : (value == 1 ? 256 : 512));
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_PON_DSC_CFG_DSr, RTL9607C_CFG_SRAM_NOf, &value), ret);
            diag_util_printf("\tSRAM DSC Cnt: %u\n", value + 1);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_PON_DSC_CFG_DSr, RTL9607C_CFG_RAM_NOf, &value), ret);
            diag_util_printf("\tDRAM DSC Cnt: %u\n", value + 1);
            
            diag_util_printf("\n");

            diag_util_printf("SW Memory Config\n");
            DIAG_UTIL_ERR_CHK(reg_read(RTL9607C_SW_PBO_MSTBASEr, &value), ret);
            diag_util_printf("\tBase Addr: 0x%08x\n", value);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_SW_PBO_DSCCTRLr, RTL9607C_CFG_PAGE_SIZEf, &value), ret);
            diag_util_printf("\tPage Size: %u\n", value == 0 ? 128 : (value == 1 ? 256 : 512));
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            uint32 value;

            diag_util_printf("US Memory Config\n");
            DIAG_UTIL_ERR_CHK(reg_read(RTL9603CVD_IP_MSTBASE_USr, &value), ret);
            diag_util_printf("\tBase Addr: 0x%08x\n", value);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_PON_DSC_CFG_USr, RTL9603CVD_CFG_PAGE_SIZEf, &value), ret);
            diag_util_printf("\tPage Size: %u\n", value == 0 ? 128 : (value == 1 ? 256 : 512));
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_PON_DSC_CFG_USr, RTL9603CVD_CFG_SRAM_NOf, &value), ret);
            diag_util_printf("\tSRAM DSC Cnt: %u\n", value + 1);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_PON_DSC_CFG_USr, RTL9603CVD_CFG_RAM_NOf, &value), ret);
            diag_util_printf("\tDRAM DSC Cnt: %u\n", value + 1);

            diag_util_printf("\n");

            diag_util_printf("DS Memory Config\n");
            DIAG_UTIL_ERR_CHK(reg_read(RTL9603CVD_IP_MSTBASE_DSr, &value), ret);
            diag_util_printf("\tBase Addr: 0x%08x\n", value);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_PON_DSC_CFG_DSr, RTL9603CVD_CFG_PAGE_SIZEf, &value), ret);
            diag_util_printf("\tPage Size: %u\n", value == 0 ? 128 : (value == 1 ? 256 : 512));
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_PON_DSC_CFG_DSr, RTL9603CVD_CFG_SRAM_NOf, &value), ret);
            diag_util_printf("\tSRAM DSC Cnt: %u\n", value + 1);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_PON_DSC_CFG_DSr, RTL9603CVD_CFG_RAM_NOf, &value), ret);
            diag_util_printf("\tDRAM DSC Cnt: %u\n", value + 1);
            
            diag_util_printf("\n");

            diag_util_printf("SW Memory Config\n");
            DIAG_UTIL_ERR_CHK(reg_read(RTL9603CVD_SW_PBO_MSTBASEr, &value), ret);
            diag_util_printf("\tBase Addr: 0x%08x\n", value);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_SW_PBO_DSCCTRLr, RTL9603CVD_CFG_PAGE_SIZEf, &value), ret);
            diag_util_printf("\tPage Size: %u\n", value == 0 ? 128 : (value == 1 ? 256 : (value == 2 ? 512 : 1024)));
            diag_util_printf("\tSRAM DSC Cnt: %u\n", value == 0 ? 16 : (value == 1 ? 8 : (value == 2 ? 4 : 2)));
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_SW_PBO_DSCCTRLr, RTL9603CVD_CFG_PAGE_NUMBERf, &value), ret);
            diag_util_printf("\tDRAM DSC Cnt: %u\n", value == 0 ? 4096 : (value == 1 ? 2048 : (value == 2 ? 1024 : 512)));
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_dump_config_memory */

/*
 * pbo set downstream sid2q sid ( <MASK_LIST:sid> | all ) queue ( normal0 | normal1 | high )
 */
cparser_result_t
cparser_cmd_pbo_set_downstream_sid2q_sid_sid_all_queue_normal0_normal1_high(
    cparser_context_t *context,
    char * *sid_ptr)
{
    int32 ret = CPARSER_OK;
    diag_mask_t  sidList;
    uint32 sid;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            rtl9602c_pbo_dsQueue_type_t qType;
            DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(sidList, 5), ret);
            if(!strcmp("normal0", TOKEN_STR(7)))
            {
                qType = RTL9602C_PBO_DSQUEUE_TYPE_NORMAL0;
            }
            else if(!strcmp("normal1", TOKEN_STR(7)))
            {
                qType = RTL9602C_PBO_DSQUEUE_TYPE_NORMAL1;
            }
            else
            {
                qType = RTL9602C_PBO_DSQUEUE_TYPE_HIGH;
            }

            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsSidToQueueMap_set(sid, qType), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            rtl9607c_pbo_dsQueue_type_t qType;
            DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(sidList, 5), ret);
            if(!strcmp("normal0", TOKEN_STR(7)))
            {
                qType = RTL9607C_PBO_DSQUEUE_TYPE_NORMAL0;
            }
            else if(!strcmp("normal1", TOKEN_STR(7)))
            {
                qType = RTL9607C_PBO_DSQUEUE_TYPE_NORMAL1;
            }
            else
            {
                qType = RTL9607C_PBO_DSQUEUE_TYPE_HIGH;
            }

            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsSidToQueueMap_set(sid, qType), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            rtl9603cvd_pbo_dsQueue_type_t qType;
            DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(sidList, 5), ret);
            if(!strcmp("normal0", TOKEN_STR(7)))
            {
                qType = RTL9603CVD_PBO_DSQUEUE_TYPE_NORMAL0;
            }
            else if(!strcmp("normal1", TOKEN_STR(7)))
            {
                qType = RTL9603CVD_PBO_DSQUEUE_TYPE_NORMAL1;
            }
            else
            {
                qType = RTL9603CVD_PBO_DSQUEUE_TYPE_HIGH;
            }

            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsSidToQueueMap_set(sid, qType), ret);
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_downstream_sid2q_sid_sid_all_queue_normal0_normal1_high */

/*
 * pbo get downstream sid2q sid ( <MASK_LIST:sid> | all )
 */
cparser_result_t
cparser_cmd_pbo_get_downstream_sid2q_sid_sid_all(
    cparser_context_t *context,
    char * *sid_ptr)
{
    int32 ret = CPARSER_OK;
    diag_mask_t  sidList;
    uint32 sid;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            rtl9602c_pbo_dsQueue_type_t qType;

            DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(sidList, 5), ret);
            diag_util_printf("SID Queue\n");
            diag_util_printf("--- ----------\n");
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                qType = 0;
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsSidToQueueMap_get(sid, &qType), ret);
                diag_util_printf("%3d %s\n", sid, diagStr_dsQueueType[qType]);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            rtl9607c_pbo_dsQueue_type_t qType;

            DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(sidList, 5), ret);
            diag_util_printf("SID Queue\n");
            diag_util_printf("--- ----------\n");
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                qType = 0;
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsSidToQueueMap_get(sid, &qType), ret);
                diag_util_printf("%3d %s\n", sid, diagStr_dsQueueType[qType]);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            rtl9603cvd_pbo_dsQueue_type_t qType;

            DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(sidList, 5), ret);
            diag_util_printf("SID Queue\n");
            diag_util_printf("--- ----------\n");
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= HAL_CLASSIFY_SID_NUM())
                {
                    break;
                }
                qType = 0;
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsSidToQueueMap_get(sid, &qType), ret);
                diag_util_printf("%3d %s\n", sid, diagStr_dsQueueType[qType]);
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_downstream_sid2q_sid_sid_all */

/*
 * pbo set downstream queue-scheduling ( round-robin | strict-priority )
 */
cparser_result_t
cparser_cmd_pbo_set_downstream_queue_scheduling_round_robin_strict_priority(
    cparser_context_t *context)
{
    int32 ret = CPARSER_OK;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            rtl9602c_pbo_dsQueueQos_type_t qosType;

            DIAG_UTIL_PARAM_CHK();

            if('r' == TOKEN_CHAR(4,0))
            {
                qosType = RTL9602C_PBO_DSQUEUEQOS_TYPE_ROUNDROBIN;
            }
            else
            {
                qosType = RTL9602C_PBO_DSQUEUEQOS_TYPE_STRICTPRIORITY;
            }

            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsQueueQos_set(qosType), ret);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            rtl9607c_pbo_dsQueueQos_type_t qosType;

            DIAG_UTIL_PARAM_CHK();

            if('r' == TOKEN_CHAR(4,0))
            {
                qosType = RTL9607C_PBO_DSQUEUEQOS_TYPE_ROUNDROBIN;
            }
            else
            {
                qosType = RTL9607C_PBO_DSQUEUEQOS_TYPE_STRICTPRIORITY;
            }

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsQueueQos_set(qosType), ret);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            rtl9603cvd_pbo_dsQueueQos_type_t qosType;

            DIAG_UTIL_PARAM_CHK();

            if('r' == TOKEN_CHAR(4,0))
            {
                qosType = RTL9603CVD_PBO_DSQUEUEQOS_TYPE_ROUNDROBIN;
            }
            else
            {
                qosType = RTL9603CVD_PBO_DSQUEUEQOS_TYPE_STRICTPRIORITY;
            }

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsQueueQos_set(qosType), ret);
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_downstream_queue_scheduling_round_robin_strict_priority */

/*
 * pbo get downstream queue-scheduling
 */
cparser_result_t
cparser_cmd_pbo_get_downstream_queue_scheduling(
    cparser_context_t *context)
{
    int32 ret = CPARSER_OK;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            rtl9602c_pbo_dsQueueQos_type_t qosType;

            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsQueueQos_get(&qosType), ret);
            diag_util_printf("DS queue scheduling: ");
            if(RTL9602C_PBO_DSQUEUEQOS_TYPE_ROUNDROBIN == qosType)
            {
                diag_util_printf("round-robin\n");
            }
            else
            {
                diag_util_printf("strict priority\n");
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            rtl9607c_pbo_dsQueueQos_type_t qosType;

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsQueueQos_get(&qosType), ret);
            diag_util_printf("DS queue scheduling: ");
            if(RTL9607C_PBO_DSQUEUEQOS_TYPE_ROUNDROBIN == qosType)
            {
                diag_util_printf("round-robin\n");
            }
            else
            {
                diag_util_printf("strict priority\n");
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            rtl9603cvd_pbo_dsQueueQos_type_t qosType;

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsQueueQos_get(&qosType), ret);
            diag_util_printf("DS queue scheduling: ");
            if(RTL9603CVD_PBO_DSQUEUEQOS_TYPE_ROUNDROBIN == qosType)
            {
                diag_util_printf("round-robin\n");
            }
            else
            {
                diag_util_printf("strict priority\n");
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_downstream_queue_scheduling */

/*
 * pbo set downstream fc-ignore ( start | stop ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_pbo_set_downstream_fc_ignore_start_stop_threshold_threshold(
    cparser_context_t *context,
    uint32_t  *threshold_ptr)
{
    int32 ret = CPARSER_OK;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            uint32 startTh, stopTh;

            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsFcIgnoreThreshold_get(&stopTh, &startTh), ret);
            if('a' == TOKEN_CHAR(4,2))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsFcIgnoreThreshold_set(stopTh, *threshold_ptr), ret);
            }
            else
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsFcIgnoreThreshold_set(*threshold_ptr, startTh), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            uint32 startTh, stopTh;

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsFcIgnoreThreshold_get(&stopTh, &startTh), ret);
            if('a' == TOKEN_CHAR(4,2))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsFcIgnoreThreshold_set(stopTh, *threshold_ptr), ret);
            }
            else
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsFcIgnoreThreshold_set(*threshold_ptr, startTh), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            uint32 startTh, stopTh;

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsFcIgnoreThreshold_get(&stopTh, &startTh), ret);
            if('a' == TOKEN_CHAR(4,2))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsFcIgnoreThreshold_set(stopTh, *threshold_ptr), ret);
            }
            else
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsFcIgnoreThreshold_set(*threshold_ptr, startTh), ret);
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_set_downstream_fc_ignore_start_stop_threshold_threshold */

/*
 * pbo get downstream fc-ignore ( start | stop ) threshold
 */
cparser_result_t
cparser_cmd_pbo_get_downstream_fc_ignore_start_stop_threshold(
    cparser_context_t *context)
{
    int32 ret = CPARSER_OK;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            uint32 startTh, stopTh;

            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsFcIgnoreThreshold_get(&stopTh, &startTh), ret);
            if('a' == TOKEN_CHAR(4,2))
            {
                diag_util_printf("fc-ignore start threshold: %d\n", startTh);
            }
            else
            {
                diag_util_printf("fc-ignore stop threshold: %d\n", stopTh);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            uint32 startTh, stopTh;

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsFcIgnoreThreshold_get(&stopTh, &startTh), ret);
            if('a' == TOKEN_CHAR(4,2))
            {
                diag_util_printf("fc-ignore start threshold: %d\n", startTh);
            }
            else
            {
                diag_util_printf("fc-ignore stop threshold: %d\n", stopTh);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            uint32 startTh, stopTh;

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsFcIgnoreThreshold_get(&stopTh, &startTh), ret);
            if('a' == TOKEN_CHAR(4,2))
            {
                diag_util_printf("fc-ignore start threshold: %d\n", startTh);
            }
            else
            {
                diag_util_printf("fc-ignore stop threshold: %d\n", stopTh);
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_get_downstream_fc_ignore_start_stop_threshold */

/*
 * pbo reset pbo
 */
cparser_result_t
cparser_cmd_pbo_reset_pbo(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
        {
            int i = 100;
            int32 ret;
            uint32 value;

            /* Reset PBO */
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_state_set(DISABLED), ret);

            /* Reset PON NIC */
            DIAG_UTIL_ERR_CHK(ioal_ponNic_write(0x18014038, 0x01), ret);
            
            /* Wait until its done or timeout*/
            do
            {
                DIAG_UTIL_ERR_CHK(ioal_ponNic_read(0x18014038, &value), ret);
            } while((value == 0x01) && i-- >= 0);
            if(i <= 0)
            {
                diag_util_printf("PON NIC reset wait timeout!\n");
            }

            /* Release reset PBO */
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_state_set(ENABLED), ret);
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            int32 ret;
            uint32 value;

            /* Reset PON NIC */
            value = 0;
            if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_GMII_RX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 0;
            if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_GMII_TX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 0;
            if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_GMII_RX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 0;
            if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_GMII_TX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }

            /* Reset PBO */
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usState_set(DISABLED), ret);
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsState_set(DISABLED), ret);

            /* Release reset PBO */
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usState_set(ENABLED), ret);
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsState_set(ENABLED), ret);

            /* Release reset PON NIC */
            value = 1;
            if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_GMII_RX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 1;
            if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_GMII_TX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 1;
            if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_GMII_RX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 1;
            if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_GMII_TX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            int32 ret;
            uint32 value;

            /* Reset PON NIC */
            value = 0;
            if ((ret = reg_field_write(RTL9607C_IO_CMD_0_USr, RTL9607C_GMII_RX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 0;
            if ((ret = reg_field_write(RTL9607C_IO_CMD_0_USr, RTL9607C_GMII_TX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 0;
            if ((ret = reg_field_write(RTL9607C_IO_CMD_0_DSr, RTL9607C_GMII_RX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 0;
            if ((ret = reg_field_write(RTL9607C_IO_CMD_0_DSr, RTL9607C_GMII_TX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }

            /* Reset PBO */
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usState_set(DISABLED), ret);
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsState_set(DISABLED), ret);

            /* Release reset PBO */
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usState_set(ENABLED), ret);
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsState_set(ENABLED), ret);

            /* Release reset PON NIC */
            value = 1;
            if ((ret = reg_field_write(RTL9607C_IO_CMD_0_USr, RTL9607C_GMII_RX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 1;
            if ((ret = reg_field_write(RTL9607C_IO_CMD_0_USr, RTL9607C_GMII_TX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 1;
            if ((ret = reg_field_write(RTL9607C_IO_CMD_0_DSr, RTL9607C_GMII_RX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 1;
            if ((ret = reg_field_write(RTL9607C_IO_CMD_0_DSr, RTL9607C_GMII_TX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            int32 ret;
            uint32 value;

            /* Reset PON NIC */
            value = 0;
            if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_USr, RTL9603CVD_GMII_RX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 0;
            if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_USr, RTL9603CVD_GMII_TX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 0;
            if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_DSr, RTL9603CVD_GMII_RX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 0;
            if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_DSr, RTL9603CVD_GMII_TX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }

            /* Reset PBO */
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usState_set(DISABLED), ret);
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsState_set(DISABLED), ret);

            /* Release reset PBO */
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usState_set(ENABLED), ret);
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsState_set(ENABLED), ret);

            /* Release reset PON NIC */
            value = 1;
            if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_USr, RTL9603CVD_GMII_RX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 1;
            if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_USr, RTL9603CVD_GMII_TX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 1;
            if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_DSr, RTL9603CVD_GMII_RX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            value = 1;
            if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_DSr, RTL9603CVD_GMII_TX_ENf, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                return ret;
            }
            break;
        }
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_reset_pbo */

/*
 * pbo reset ( upstream | downstream ) pbo
 */
cparser_result_t
cparser_cmd_pbo_reset_upstream_downstream_pbo(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if ('u' == TOKEN_CHAR(2,0))
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9601B)
            case RTL9601B_CHIP_ID:
            {
                int i = 100;
                int32 ret;
                uint32 value;

                /* Reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_state_set(DISABLED), ret);

                /* Reset PON NIC */
                DIAG_UTIL_ERR_CHK(ioal_ponNic_write(0x18014038, 0x01), ret);
                
                /* Wait until its done or timeout*/
                do
                {
                    DIAG_UTIL_ERR_CHK(ioal_ponNic_read(0x18014038, &value), ret);
                } while((value == 0x01) && i-- >= 0);
                if(i <= 0)
                {
                    diag_util_printf("PON NIC reset wait timeout!\n");
                }

                /* Release reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_state_set(ENABLED), ret);
                break;
            }
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
            {
                int32 ret;
                uint32 value;

                /* Reset PON NIC */
                value = 0;
                if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_GMII_RX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                value = 0;
                if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_GMII_TX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }

                /* Reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usState_set(DISABLED), ret);

                /* Release reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usState_set(ENABLED), ret);

                /* Reease reset PON NIC */
                value = 1;
                if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_GMII_RX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                value = 1;
                if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_GMII_TX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                break;
            }
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
            {
                int32 ret;
                uint32 value;

                /* Reset PON NIC */
                value = 0;
                if ((ret = reg_field_write(RTL9607C_IO_CMD_0_USr, RTL9607C_GMII_RX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                value = 0;
                if ((ret = reg_field_write(RTL9607C_IO_CMD_0_USr, RTL9607C_GMII_TX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }

                value = 1;
                if ((ret = reg_field_write(RTL9607C_IO_CMD_0_USr, RTL9607C_GMII_TX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }

                /* Reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usState_set(DISABLED), ret);

                /* Release reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_usState_set(ENABLED), ret);

                /* Reease reset PON NIC */
                value = 1;
                if ((ret = reg_field_write(RTL9607C_IO_CMD_0_USr, RTL9607C_GMII_RX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                break;
            }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
            {
                int32 ret;
                uint32 value;

                /* Reset PON NIC */
                value = 0;
                if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_USr, RTL9603CVD_GMII_RX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                value = 0;
                if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_USr, RTL9603CVD_GMII_TX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }

                /* Reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usState_set(DISABLED), ret);

                /* Release reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_usState_set(ENABLED), ret);

                /* Reease reset PON NIC */
                value = 1;
                if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_USr, RTL9603CVD_GMII_RX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                value = 1;
                if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_USr, RTL9603CVD_GMII_TX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                break;
            }
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
        }
    }
    else
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
            {
                int32 ret;
                uint32 value;

                /* Reset PON NIC */
                value = 0;
                if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_GMII_RX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                value = 0;
                if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_GMII_TX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }

                /* Reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsState_set(DISABLED), ret);

                /* Release reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsState_set(ENABLED), ret);

                /* Reease reset PON NIC */
                value = 1;
                if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_GMII_RX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                value = 1;
                if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_GMII_TX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                break;
            }
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
            {
                int32 ret;
                uint32 value;

                /* Reset PON NIC */
                value = 0;
                if ((ret = reg_field_write(RTL9607C_IO_CMD_0_DSr, RTL9607C_GMII_RX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                value = 0;
                if ((ret = reg_field_write(RTL9607C_IO_CMD_0_DSr, RTL9607C_GMII_TX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }

                /* Reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsState_set(DISABLED), ret);

                /* Release reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_pbo_dsState_set(ENABLED), ret);

                /* Reease reset PON NIC */
                value = 1;
                if ((ret = reg_field_write(RTL9607C_IO_CMD_0_DSr, RTL9607C_GMII_RX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                value = 1;
                if ((ret = reg_field_write(RTL9607C_IO_CMD_0_DSr, RTL9607C_GMII_TX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                break;
            }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
            {
                int32 ret;
                uint32 value;

                /* Reset PON NIC */
                value = 0;
                if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_DSr, RTL9603CVD_GMII_RX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                value = 0;
                if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_DSr, RTL9603CVD_GMII_TX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }

                /* Reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsState_set(DISABLED), ret);

                /* Release reset PBO */
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_pbo_dsState_set(ENABLED), ret);

                /* Reease reset PON NIC */
                value = 1;
                if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_DSr, RTL9603CVD_GMII_RX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                value = 1;
                if ((ret = reg_field_write(RTL9603CVD_IO_CMD_0_DSr, RTL9603CVD_GMII_TX_ENf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
                    return ret;
                }
                break;
            }
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_pbo_reset_upstream_downstream_pbo */

/* 
 * swpbo private functions
 */
typedef struct bist_table_s 
{
    char sramName[32];
    uint32 reg;
    uint32 control;
    uint32 result;
} bist_table_t;

#if defined(CONFIG_SDK_RTL9607C)
static bist_table_t bist_table_rtl9607c[] =
{
    { "PONNIC_TXFIFO_A", RTL9607C_PBOES_BIST_0r },
    { "PONNIC_RXFIFO_A", RTL9607C_PBOES_BIST_1r },
    { "PONNIC_TXFIFO_B", RTL9607C_PBOES_BIST_2r },
    { "PONNIC_RXFIFO_B", RTL9607C_PBOES_BIST_3r },
    { "FASTPATH_MEM_0", RTL9607C_PBOES_BIST_4r },
    { "FASTPATH_MEM_1", RTL9607C_PBOES_BIST_5r },
    { "SCH_OUTQ_0", RTL9607C_PBOES_BIST_6r },
    { "SCH_OUTQ_1", RTL9607C_PBOES_BIST_7r },
    { "SCH_OUTQ_2", RTL9607C_PBOES_BIST_8r },
    { "SCH_OUTQ_3", RTL9607C_PBOES_BIST_9r },
    { "SCH_OUTQ_4", RTL9607C_PBOES_BIST_10r },
    { "DESC_0", RTL9607C_PBOES_BIST_11r },
    { "DESC_1", RTL9607C_PBOES_BIST_12r },
    { "DESC_2", RTL9607C_PBOES_BIST_13r },
    { "DESC_3", RTL9607C_PBOES_BIST_14r },
    { "DESC_4", RTL9607C_PBOES_BIST_15r }
};

static int _rtl9607c_swpbo_sram_bist(void)
{
    int i, ret;
    uint32 value;

    for(i = 0 ; i < sizeof(bist_table_rtl9607c)/sizeof(bist_table_t) ; i ++)
    {
        value = 0x2;
        if ((ret = reg_write(bist_table_rtl9607c[i].reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
    }
    for(i = 0 ; i < sizeof(bist_table_rtl9607c)/sizeof(bist_table_t) ; i ++)
    {
        value = 0x3;
        if ((ret = reg_write(bist_table_rtl9607c[i].reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
    }
    osal_time_mdelay(300);
    for(i = 0 ; i < sizeof(bist_table_rtl9607c)/sizeof(bist_table_t) ; i ++)
    {
        if ((ret = reg_read(bist_table_rtl9607c[i].reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
        /* bist done */
        if(value != 0x403)
        {
            diag_util_printf("bist failed at %s read 0x%x\n", bist_table_rtl9607c[i].sramName, value);
            return ret;
        }
    }
    diag_util_printf("bist passed\n");

    return RT_ERR_OK;
}
static int _rtl9607c_swpbo_sram_drf_bist(void)
{
    int i, ret;
    uint32 value;

    for(i = 0 ; i < sizeof(bist_table_rtl9607c)/sizeof(bist_table_t) ; i ++)
    {
        value = 0x4;
        if ((ret = reg_write(bist_table_rtl9607c[i].reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
    }
    for(i = 0 ; i < sizeof(bist_table_rtl9607c)/sizeof(bist_table_t) ; i ++)
    {
        value = 0x5;
        if ((ret = reg_write(bist_table_rtl9607c[i].reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
    }    
    osal_time_mdelay(300);
    for(i = 0 ; i < sizeof(bist_table_rtl9607c)/sizeof(bist_table_t) ; i ++)
    {
        if ((ret = reg_read(bist_table_rtl9607c[i].reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
        /* 1st pause */
        if(value != 0x1005)
        {
            diag_util_printf("1st pause failed at %s read 0x%x\n", bist_table_rtl9607c[i].sramName, value);
            return ret;
        }
    }
    for(i = 0 ; i < sizeof(bist_table_rtl9607c)/sizeof(bist_table_t) ; i ++)
    {
        value = 0xd;
        if ((ret = reg_write(bist_table_rtl9607c[i].reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
    }
    for(i = 0 ; i < sizeof(bist_table_rtl9607c)/sizeof(bist_table_t) ; i ++)
    {
        value = 0x5;
        if ((ret = reg_write(bist_table_rtl9607c[i].reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
    }        
    osal_time_mdelay(300);
    for(i = 0 ; i < sizeof(bist_table_rtl9607c)/sizeof(bist_table_t) ; i ++)
    {
        if ((ret = reg_read(bist_table_rtl9607c[i].reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
        /* 2nd pause */
        if(value != 0x1005)
        {
            diag_util_printf("2nd pause failed at %s read 0x%x\n", bist_table_rtl9607c[i].sramName, value);
            return ret;
        }
    }
    for(i = 0 ; i < sizeof(bist_table_rtl9607c)/sizeof(bist_table_t) ; i ++)
    {
        value = 0xd;
        if ((ret = reg_write(bist_table_rtl9607c[i].reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
    }
    for(i = 0 ; i < sizeof(bist_table_rtl9607c)/sizeof(bist_table_t) ; i ++)
    {
        value = 0x5;
        if ((ret = reg_write(bist_table_rtl9607c[i].reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
    }
    osal_time_mdelay(300);
    for(i = 0 ; i < sizeof(bist_table_rtl9607c)/sizeof(bist_table_t) ; i ++)
    {
        if ((ret = reg_read(bist_table_rtl9607c[i].reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
        /* drf done */
        if(value != 0x805)
        {
            diag_util_printf("drf-bist failed at %s read 0x%x\n", bist_table_rtl9607c[i].sramName, value);
            return ret;
        }
    }
    diag_util_printf("drf-bist passed\n");

    return RT_ERR_OK;
}
#endif

static void _swpbo_sram_bist(void)
{
    int ret;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
    case RTL9607C_CHIP_ID:
        if((ret = _rtl9607c_swpbo_sram_bist()) != RT_ERR_OK)
        {
            return;
        }
        break;
#endif
    default:
        return;
    }
}

static void _swpbo_sram_drf_bist(void)
{
    int ret;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
    case RTL9607C_CHIP_ID:
        if((ret = _rtl9607c_swpbo_sram_drf_bist()) != RT_ERR_OK)
        {
            return;
        }
        break;
#endif
    default:
        return;
    }
}


/*
 * swpbo init
 */
cparser_result_t
cparser_cmd_swpbo_init(
    cparser_context_t *context)
{
    int32 ret;
    rtk_swPbo_initParam_t initParam;
    
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
    case RTL9607C_CHIP_ID:
        initParam.pageSize = PBO_PAGE_SIZE_128;
        initParam.pageCount = 0; /* Not used in RTL9607C series */
        DIAG_UTIL_ERR_CHK(rtk_swPbo_init(initParam), ret);
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_init */

/*
 * swpbo init page-size ( 128 | 256 | 512 | 1024 ) page-count ( 512 | 1024 | 2048 | 4096 )
 */
cparser_result_t
cparser_cmd_swpbo_init_page_size_128_256_512_1024_page_count_512_1024_2048_4096(
    cparser_context_t *context)
{
    int32 ret;
    rtk_swPbo_initParam_t initParam;
    
    DIAG_UTIL_PARAM_CHK();

    if(!strcmp(TOKEN_STR(3), "128"))
    {
        initParam.pageSize = PBO_PAGE_SIZE_128;
    }
    else if(!strcmp(TOKEN_STR(3), "256"))
    {
        initParam.pageSize = PBO_PAGE_SIZE_256;
    }
    else if(!strcmp(TOKEN_STR(3), "512"))
    {
        initParam.pageSize = PBO_PAGE_SIZE_512;
    }
    else if(!strcmp(TOKEN_STR(3), "1024"))
    {
        initParam.pageSize = PBO_PAGE_SIZE_1024;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    if(!strcmp(TOKEN_STR(5), "512"))
    {
        initParam.pageCount = PBO_PAGE_COUNT_512;
    }
    else if(!strcmp(TOKEN_STR(5), "1024"))
    {
        initParam.pageCount = PBO_PAGE_COUNT_1024;
    }
    else if(!strcmp(TOKEN_STR(5), "2048"))
    {
        initParam.pageCount = PBO_PAGE_COUNT_2048;
    }
    else if(!strcmp(TOKEN_STR(5), "4096"))
    {
        initParam.pageCount = PBO_PAGE_COUNT_4096;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_swPbo_init(initParam), ret);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_init_page_size_128_256_512_1024_page_count_512_1024_2048_4096 */

/*
 * swpbo set port ( <PORT_LIST:ports> ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_swpbo_set_port_ports_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;
    rtk_enable_t enable=DISABLED;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    if ('e' == TOKEN_CHAR(5,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(5,0))
    {
        enable = DISABLED;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_swPbo_portState_set(port, enable), ret);
        }        
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_set_port_ports_state_enable_disable */

/*
 * swpbo get port ( <PORT_LIST:ports> ) state
 */
cparser_result_t
cparser_cmd_swpbo_get_port_ports_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;
    rtk_enable_t enable;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {

#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        diag_util_printf("SW PBO port state\n");
        diag_util_printf("Port State\n");
        diag_util_printf("---- -------\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_swPbo_portState_get(port, &enable), ret);
            diag_util_printf("%4u %s\n", port, diagStr_enable[enable]);
        }        
        diag_util_printf("---- --------\n");
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_get_port_ports_state */

/*
 * swpbo set port ( <PORT_LIST:ports> ) auto-config ( enable | disable )
 */
cparser_result_t
cparser_cmd_swpbo_set_port_ports_auto_conf_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;
    rtk_enable_t enable=DISABLED;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    if ('e' == TOKEN_CHAR(5,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(5,0))
    {
        enable = DISABLED;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_swPbo_portAutoConf_set(port, enable), ret);
        }        
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_set_port_ports_auto_conf_enable_disable */

/*
 * swpbo get port ( <PORT_LIST:ports> ) auto-config
 */
cparser_result_t
cparser_cmd_swpbo_get_port_ports_auto_conf(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;
    rtk_enable_t enable;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        diag_util_printf("SW PBO port auto-conf\n");
        diag_util_printf("Port State\n");
        diag_util_printf("---- -------\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_swPbo_portAutoConf_get(port, &enable), ret);
            diag_util_printf("%4u %s\n", port, diagStr_enable[enable]);
        }        
        diag_util_printf("---- --------\n");
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_get_port_ports_auto_conf */
/*
 * swpbo set port <PORT_LIST:ports> dsc-runout sram <UINT:sram_th> dram <UINT:dram_th>
 */
cparser_result_t
cparser_cmd_swpbo_set_port_ports_dsc_runout_sram_sram_th_dram_dram_th(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *sram_th_ptr,
    uint32_t  *dram_th_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
    case RTL9607C_CHIP_ID:
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_swPbo_dscRunoutThreshold_set(port, *sram_th_ptr, *dram_th_ptr), ret);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_swPbo_dscRunoutThreshold_set(port, *sram_th_ptr, *dram_th_ptr), ret);
        }
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_set_port_ports_dsc_runout_sram_sram_th_dram_dram_th */

/*
 * swpbo get port <PORT_LIST:ports> dsc-runout
 */
cparser_result_t
cparser_cmd_swpbo_get_port_ports_dsc_runout(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 sramTh, dramTh;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
    case RTL9607C_CHIP_ID:
        diag_util_printf("SW PBO port dsc run out\n");
        diag_util_printf("Port SRAM DRAM\n");
        diag_util_printf("---- ---- ----\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_swPbo_dscRunoutThreshold_get(port, &sramTh, &dramTh), ret);
            diag_util_mprintf("%4u %4u %4u\n", port, sramTh, dramTh);
        }
        diag_util_printf("---- ---- ----\n");
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    case RTL9603CVD_CHIP_ID:
        diag_util_printf("SW PBO port dsc run out\n");
        diag_util_printf("Port SRAM DRAM\n");
        diag_util_printf("---- ---- ----\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_swPbo_dscRunoutThreshold_get(port, &sramTh, &dramTh), ret);
            diag_util_mprintf("%4u %4u %4u\n", port, sramTh, dramTh);
        }
        diag_util_printf("---- ---- ----\n");
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_get_port_ports_dsc_runout */

/*
 * swpbo set port <PORT_LIST:ports> threshold <UINT:port_th>
 */
cparser_result_t
cparser_cmd_swpbo_set_port_ports_threshold_port_th(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *port_th_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_swPbo_portThreshold_set(port, *port_th_ptr), ret);
        }
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_set_port_ports_threshold_port_th */

/*
 * swpbo get port <PORT_LIST:ports> threshold
 */
cparser_result_t
cparser_cmd_swpbo_get_port_ports_threshold(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 threshold;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        diag_util_printf("SW PBO port threshold\n");
        diag_util_printf("Port Threshold\n");
        diag_util_printf("---- ---------\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_swPbo_portThreshold_get(port, &threshold), ret);
            diag_util_mprintf("%4u %9u\n", port, threshold);
        }
        diag_util_printf("---- ---------\n");
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_get_port_ports_threshold */

/*
 * swpbo get port <PORT_LIST:ports> used-page
 */
cparser_result_t
cparser_cmd_swpbo_get_port_ports_used_page(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 curr, max;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        diag_util_printf("SW PBO port used-page\n");
        diag_util_printf("Port Curr  Max\n");
        diag_util_printf("---- ---- ----\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_swPbo_portUsedPageCount_get(port, &curr, &max), ret);
            diag_util_mprintf("%4u %4u %4u\n", port, curr, max);
        }
        diag_util_printf("---- ---- ----\n");
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_get_port_ports_used_page */

/*
 * swpbo clear port <PORT_LIST:ports> used-page
 */
cparser_result_t
cparser_cmd_swpbo_clear_port_ports_used_page(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 page;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_swPbo_portUsedPageCount_clear(port), ret);
        }
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_clear_port_ports_used_page */

/*
 * swpbo set port <PORT_LIST:ports> queue ( <MASK_LIST:qid> | all ) threshold <UINT:queue_th>
 */
cparser_result_t
cparser_cmd_swpbo_set_port_ports_queue_qid_all_threshold_queue_th(
    cparser_context_t *context,
    char * *ports_ptr,
    char * *qid_ptr,
    uint32_t  *queue_th_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    diag_mask_t  queuelist;
    rtk_port_t port;
    rtk_qid_t queue;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(rtk_swPbo_queueThreshold_set(port, queue, *queue_th_ptr), ret);
            }
        }
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_set_port_ports_queue_qid_all_threshold_queue_th */

/*
 * swpbo get port <PORT_LIST:ports> queue ( <MASK_LIST:qid> | all ) threshold
 */
cparser_result_t
cparser_cmd_swpbo_get_port_ports_queue_qid_all_threshold(
    cparser_context_t *context,
    char * *ports_ptr,
    char * *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    diag_mask_t  queuelist;
    rtk_port_t port;
    rtk_qid_t queue;
    uint32 threshold;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        diag_util_printf("SW PBO queue threshold\n");
        diag_util_printf("Port  Q Threshold\n");
        diag_util_printf("---- -- ---------\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(rtk_swPbo_queueThreshold_get(port, queue, &threshold), ret);
                diag_util_mprintf("%4u %2u %9u\n", port, queue, threshold);
            }
            diag_util_printf("---- -- ---------\n");
        }
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_get_port_ports_queue_qid_all_threshold */

/*
 * swpbo get port <PORT_LIST:ports> queue ( <MASK_LIST:qid> | all ) used-page
 */
cparser_result_t
cparser_cmd_swpbo_get_port_ports_queue_qid_all_used_page(
    cparser_context_t *context,
    char * *ports_ptr,
    char * *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    diag_mask_t  queuelist;
    rtk_port_t port;
    rtk_qid_t queue;
    uint32 curr, max;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        diag_util_printf("SW PBO queue used-page\n");
        diag_util_printf("Port  Q Curr  Max\n");
        diag_util_printf("---- -- ---- ----\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(rtk_swPbo_queueUsedPageCount_get(port, queue, &curr, &max), ret);
                diag_util_mprintf("%4u %2u %4u %4u\n", port, queue, curr, max);
            }
            diag_util_printf("---- -- ---- ----\n");
        }
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_get_port_ports_queue_qid_all_used_page */

/*
 * swpbo clear port <PORT_LIST:ports> queue ( <MASK_LIST:qid> | all ) used-page
 */
cparser_result_t
cparser_cmd_swpbo_clear_port_ports_queue_qid_all_used_page(
    cparser_context_t *context,
    char * *ports_ptr,
    char * *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    diag_mask_t  queuelist;
    rtk_port_t port;
    rtk_qid_t queue;
    uint32 threshold;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(rtk_swPbo_queueUsedPageCount_clear(port, queue), ret);
            }
        }
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_clear_port_ports_queue_qid_all_used_page */

/*
 * swpbo set link-state ( link-down | link-up )
 */
cparser_result_t
cparser_cmd_swpbo_set_link_state_link_down_link_up(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;

    rtk_port_macAbility_t portAbility;
    rtk_port_linkStatus_t   linkStatus;

    if ('d' == TOKEN_CHAR(3,5))
    {
        linkStatus = PORT_LINKDOWN;
    }
    else 
    {
        linkStatus = PORT_LINKUP;
    }

    port = HAL_GET_SWPBOLB_PORT();
    
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbility_get(port, &portAbility), ret);

    portAbility.linkStatus = linkStatus;

    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbility_set(port, portAbility), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_set_link_state_link_down_link_up */

/*
 * swpbo get link-state
 */
cparser_result_t
cparser_cmd_swpbo_get_link_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;

    rtk_port_macAbility_t portAbility;

    DIAG_UTIL_PARAM_CHK();

    port = HAL_GET_SWPBOLB_PORT();

    
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbility_get(port, &portAbility), ret);

    diag_util_mprintf("%s\n",diagStr_portLinkStatus[portAbility.linkStatus]);

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_get_link_state */

/*
 * swpbo get port <PORT_LIST:ports> dsc
 */
cparser_result_t
cparser_cmd_swpbo_get_port_ports_dsc(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 currSram, currDram, maxSram, maxDram;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        diag_util_printf("SW PBO port used-page\n");
        diag_util_printf("Port CurrSram CurrDram MaxSram MaxDram\n");
        diag_util_printf("---- -------- -------- ------- -------\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_swPbo_portDscCount_get(port, &currSram, &currDram, &maxSram, &maxDram), ret);
            diag_util_mprintf("%4u %8u %8u %7u %7u\n", port, currSram, currDram, maxSram, maxDram);
        }
        diag_util_printf("---- -------- -------- ------- -------\n");
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_get_port_ports_dsc */

/*
 * swpbo clear port <PORT_LIST:ports> dsc
 */
cparser_result_t
cparser_cmd_swpbo_clear_port_ports_dsc(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 page;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_swPbo_portDscCount_clear(port), ret);
        }
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_clear_port_ports_dsc */

/*
 * swpbo set sram ( bist | drf-bist )
 */
cparser_result_t
cparser_cmd_swpbo_set_sram_bist_drf_bist(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
    case RTL9607C_CHIP_ID:
        if ('b' == TOKEN_CHAR(3,0))
        {
            /* BIST */
            _swpbo_sram_bist();
        }
        else if ('d' == TOKEN_CHAR(3,0))
        {
            /* DRF-BIST */
            _swpbo_sram_drf_bist();
        }
        else
        {
            return CPARSER_NOT_OK;
        }
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_set_sram_bist_drf_bist */

/*
 * swpbo dump counter ponnic
 */
cparser_result_t
cparser_cmd_swpbo_dump_counter_ponnic(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            rtl9607c_pbo_ponnic_cnt_t swpbo_cnt;
                
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_swPbo_ponnicCounter_get(&swpbo_cnt), ret);
            diag_util_printf("Tx PKT_OK_CNT: %u\n", swpbo_cnt.pkt_ok_cnt_tx);
            diag_util_printf("Rx PKT_OK_CNT: %u\n", swpbo_cnt.pkt_ok_cnt_rx);
            diag_util_printf("Tx PKT_ERR_CNT: %u\n", swpbo_cnt.pkt_err_cnt_tx);
            diag_util_printf("Rx PKT_ERR_CNT: %u\n", swpbo_cnt.pkt_err_cnt_rx);
            diag_util_printf("PKT_MISS_CNT: %u\n", swpbo_cnt.pkt_miss_cnt);
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            rtl9603cvd_pbo_ponnic_cnt_t swpbo_cnt;
                
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_swPbo_ponnicCounter_get(&swpbo_cnt), ret);
            diag_util_printf("Tx PKT_OK_CNT: %u\n", swpbo_cnt.pkt_ok_cnt_tx);
            diag_util_printf("Rx PKT_OK_CNT: %u\n", swpbo_cnt.pkt_ok_cnt_rx);
            diag_util_printf("Tx PKT_ERR_CNT: %u\n", swpbo_cnt.pkt_err_cnt_tx);
            diag_util_printf("Rx PKT_ERR_CNT: %u\n", swpbo_cnt.pkt_err_cnt_rx);
            diag_util_printf("PKT_MISS_CNT: %u\n", swpbo_cnt.pkt_miss_cnt);
            break;
        }
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_dump_counter_ponnic */

/*
 * swpbo set counter group <UINT:grp> qid ( <MASK_LIST:qid> | all )
 */
cparser_result_t
cparser_cmd_swpbo_set_counter_group_grp_qid_qid_all(
    cparser_context_t *context,
    uint32_t  *grp_ptr,
    char * *sid_ptr)
{
    int32 ret = CPARSER_OK;
    diag_mask_t  sidList;
    uint32 sid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(sidList, 6), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_swPbo_counterGroupMember_clear(*grp_ptr), ret);
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= 40)
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_swPbo_counterGroupMember_add(*grp_ptr, sid), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_swPbo_counterGroupMember_clear(*grp_ptr), ret);
            DIAG_UTIL_MASK_SCAN(sidList, sid)
            {
                if(sid >= 40)
                {
                    break;
                }
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_swPbo_counterGroupMember_add(*grp_ptr, sid), ret);
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_set_counter_group_grp_qid_qid_all */

/*
 * swpbo get counter group <UINT:grp>
 */
cparser_result_t
cparser_cmd_swpbo_get_counter_group_grp(
    cparser_context_t *context,
    uint32_t  *grp_ptr)
{
    int32 ret;
    uint32 j, k;
    uint32 groupStart, groupEnd;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            
            if(*grp_ptr >= 5)
            {
                diag_util_printf("Group out of range\n");
                return CPARSER_NOT_OK;
            }

            diag_util_printf("Group detail:\n");
            diag_util_printf("Group %u:\n", *grp_ptr);
            for(j = 0;j < 40;j ++)
            {
                if((j % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("QID | ");
                    for(k = j; (k < 40) && (k < j + 16); k ++)
                    {
                        diag_util_printf("%3u ", k);
                    }
                    diag_util_printf("\n    | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_swPbo_counterGroupMember_get(*grp_ptr, j, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'V' : ' ');
                if((j % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((j % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            
            if(*grp_ptr >= 5)
            {
                diag_util_printf("Group out of range\n");
                return CPARSER_NOT_OK;
            }

            diag_util_printf("Group detail:\n");
            diag_util_printf("Group %u:\n", *grp_ptr);
            for(j = 0;j < 40;j ++)
            {
                if((j % 16) == 0)
                {
                    diag_util_printf("----+------------------------------------------------------------------\n");
                    diag_util_printf("QID | ");
                    for(k = j; (k < 40) && (k < j + 16); k ++)
                    {
                        diag_util_printf("%3u ", k);
                    }
                    diag_util_printf("\n    | ");
                }
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_swPbo_counterGroupMember_get(*grp_ptr, j, &enable), ret);
                diag_util_printf("%3c ", enable == ENABLED ? 'V' : ' ');
                if((j % 16) == 15)
                {
                    diag_util_printf("\n");
                }
            }
            if((j % 16) != 15)
            {
                diag_util_printf("\n");
            }
            diag_util_printf("----+------------------------------------------------------------------\n");
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_get_counter_group_grp */
/*
 * swpbo dump counter group ( <MASK_LIST:group> | all )
 */
cparser_result_t
cparser_cmd_swpbo_dump_counter_group_group_all(
    cparser_context_t *context,
    char * *group_ptr)
{
    int32 ret = CPARSER_OK;
    diag_mask_t  groupList;
    uint32 group;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(groupList, 4), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            rtl9607c_swPbo_group_cnt_t group_cnt;

            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("Group counters:\n");
            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9607c_raw_swPbo_groupCounter_get(group, &group_cnt), ret);
                diag_util_printf("Group %u:\n", group);
                diag_util_printf("   RX_SID_GOOD_CNT:%u\n", group_cnt.rx_good_cnt);
                diag_util_printf("   RX_SID_BAD_CNT:%u\n", group_cnt.rx_bad_cnt);
                diag_util_printf("   TX_SID_CNT:%u\n", group_cnt.tx_cnt);
                diag_util_printf("\n");
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            rtl9603cvd_swPbo_group_cnt_t group_cnt;

            DIAG_UTIL_OUTPUT_INIT();

            diag_util_printf("Group counters:\n");
            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_swPbo_groupCounter_get(group, &group_cnt), ret);
                diag_util_printf("Group %u:\n", group);
                diag_util_printf("   RX_SID_GOOD_CNT:%u\n", group_cnt.rx_good_cnt);
                diag_util_printf("   RX_SID_BAD_CNT:%u\n", group_cnt.rx_bad_cnt);
                diag_util_printf("   TX_SID_CNT:%u\n", group_cnt.tx_cnt);
                diag_util_printf("\n");
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_dump_counter_group_group_all */

/*
 * swpbo clear counter group ( <MASK_LIST:group> | all )
 */
cparser_result_t
cparser_cmd_swpbo_clear_counter_group_group_all(
    cparser_context_t *context,
    char * *group_ptr)
{
    int32 ret = CPARSER_OK;
    diag_mask_t  groupList;
    uint32 group;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(groupList, 4), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9607c_raw_swPbo_groupCounter_clear(group), ret);
            }
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_MASK_SCAN(groupList, group)
            {
                if(group >= 5)
                {
                    break;
                }

                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_swPbo_groupCounter_clear(group), ret);
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_clear_counter_group_group_all */


/*
 * swpbo dump counter swpbo
 */
cparser_result_t
cparser_cmd_swpbo_dump_counter_swpbo(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            rtl9607c_swPbo_cnt_t swpbo_cnt;
                
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_swPbo_counter_get(&swpbo_cnt), ret);
            diag_util_printf("RX_DROP_CNT: %u\n", swpbo_cnt.rx_drop_cnt);
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            rtl9603cvd_swPbo_cnt_t swpbo_cnt;
                
            DIAG_UTIL_OUTPUT_INIT();

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_swPbo_counter_get(&swpbo_cnt), ret);
            diag_util_printf("RX_DROP_CNT: %u\n", swpbo_cnt.rx_drop_cnt);
            break;
        }
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_dump_counter_swpbo */

/*
 * swpbo clear counter swpbo
 */
cparser_result_t
cparser_cmd_swpbo_clear_counter_swpbo(
    cparser_context_t *context)
{
    int32 ret = CPARSER_OK;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_swPbo_counter_clear(), ret);
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            DIAG_UTIL_OUTPUT_INIT();
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_swPbo_counter_clear(), ret);
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_clear_counter_swpbo */

/*
 * swpbo dump config memory
 */
cparser_result_t
cparser_cmd_swpbo_dump_config_memory(
    cparser_context_t *context)
{
    int32 ret = CPARSER_OK;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            uint32 value;

            diag_util_printf("SW Memory Config\n");
            DIAG_UTIL_ERR_CHK(reg_read(RTL9607C_SW_PBO_MSTBASEr, &value), ret);
            diag_util_printf("\tBase Addr: 0x%08x\n", value);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_SW_PBO_DSCCTRLr, RTL9607C_CFG_PAGE_SIZEf, &value), ret);
            diag_util_printf("\tPage Size: %u\n", value == 0 ? 128 : (value == 1 ? 256 : 512));
        }
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            uint32 value;

            diag_util_printf("SW Memory Config\n");
            DIAG_UTIL_ERR_CHK(reg_read(RTL9603CVD_SW_PBO_MSTBASEr, &value), ret);
            diag_util_printf("\tBase Addr: 0x%08x\n", value);
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_SW_PBO_DSCCTRLr, RTL9603CVD_CFG_PAGE_SIZEf, &value), ret);
            diag_util_printf("\tPage Size: %u\n", value == 0 ? 128 : (value == 1 ? 256 : (value == 2 ? 512 : 1024)));
            diag_util_printf("\tSRAM DSC Cnt: %u\n", value == 0 ? 16 : (value == 1 ? 8 : (value == 2 ? 4 : 2)));
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_SW_PBO_DSCCTRLr, RTL9603CVD_CFG_PAGE_NUMBERf, &value), ret);
            diag_util_printf("\tDRAM DSC Cnt: %u\n", value == 0 ? 4096 : (value == 1 ? 2048 : (value == 2 ? 1024 : 512)));
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_swpbo_dump_config_memory */

