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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) switch commands.    
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <hal/mac/reg.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <dal/apollomp/raw/apollomp_raw_switch.h>
#include <dal/apollomp/raw/apollomp_raw.h>
//#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <hal/chipdef/rtl9601b/rtk_rtl9601b_reg_struct.h>
#include <dal/rtl9601b/dal_rtl9601b_switch.h>
#ifdef CONFIG_SDK_RTL9602C 
#include <dal/rtl9602c/dal_rtl9602c_switch.h>
#include <hal/chipdef/rtl9602c/rtk_rtl9602c_reg_struct.h>
#endif
#ifdef CONFIG_SDK_RTL9607C 
#include <dal/rtl9607c/dal_rtl9607c_switch.h>
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_reg_struct.h>
#endif
#ifdef CONFIG_SDK_RTL9603CVD
#include <dal/rtl9603cvd/dal_rtl9603cvd_switch.h>
#include <hal/chipdef/rtl9603cvd/rtk_rtl9603cvd_reg_struct.h>
#endif


/*
 * switch init
 */
cparser_result_t
cparser_cmd_switch_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    
    /*init switch module*/
    DIAG_UTIL_ERR_CHK(rtk_switch_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_init */


/*
 * switch get 48-pass-1 state
 */
cparser_result_t
cparser_cmd_switch_get_48_pass_1_state(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable=ENABLED;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_read(CFG_BACKPRESSUREr, EN_48_PASS_1f, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_CFG_BACKPRESSUREr, APOLLOMP_EN_48_PASS_1f, &enable),ret);

            break;
#endif    

#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_EN_48_PASS_1f, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_CFG_BACKPRESSUREr, RTL9602C_EN_48_PASS_1f, &enable),ret);

            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_CFG_BACKPRESSUREr, RTL9607C_EN_48_PASS_1f, &enable),ret);

            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_CFG_BACKPRESSUREr, RTL9603CVD_EN_48_PASS_1f, &enable),ret);

            break;
#endif

        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            break;

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    diag_util_mprintf("48 Pass 1 function: %s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_48_pass_1_state */

/*
 * switch set 48-pass-1 state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_48_pass_1_state_disable_enable(
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
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_write(CFG_BACKPRESSUREr, EN_48_PASS_1f, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_CFG_BACKPRESSUREr, APOLLOMP_EN_48_PASS_1f, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_EN_48_PASS_1f, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_CFG_BACKPRESSUREr, RTL9602C_EN_48_PASS_1f, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_CFG_BACKPRESSUREr, RTL9607C_EN_48_PASS_1f, &enable),ret);

            break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_CFG_BACKPRESSUREr, RTL9603CVD_EN_48_PASS_1f, &enable),ret);

            break;
#endif 
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_48_pass_1_state_disable_enable */

/*
 * switch set ipg-compensation state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_ipg_compensation_state_disable_enable(
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
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_write(SWITCH_CTRLr, SHORT_IPGf, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_SWITCH_CTRLr, APOLLOMP_SHORT_IPGf, &enable),ret);

            break;
#endif    

#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_SHORT_IPGf, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_SWITCH_CTRLr, RTL9602C_SHORT_IPGf, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_SWITCH_CTRLr, RTL9607C_SHORT_IPGf, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_SWITCH_CTRLr, RTL9603CVD_SHORT_IPGf, &enable),ret);

            break;
#endif    

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_ipg_compensation_state_disable_enable */

/*
 * switch set ipg-compensation ( 65ppm | 90ppm )
 */
cparser_result_t
cparser_cmd_switch_set_ipg_compensation_65ppm_90ppm(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_switch_ipgCompMode_t mode;
    
    DIAG_UTIL_PARAM_CHK();

    if ('6' == TOKEN_CHAR(3,0))
    {
        mode = IPGCOMPMODE_65PPM;
    }
    else if ('9' == TOKEN_CHAR(3,0))
    {
        mode = IPGCOMPMODE_90PPM;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_write(CFG_UNHIOLr, IPG_COMPENSATIONf, &mode),ret);

            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_CFG_UNHIOLr, APOLLOMP_IPG_COMPENSATIONf, &mode),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_IPG_COMPENSATIONf, &mode),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_CFG_UNHIOLr, RTL9602C_IPG_COMPENSATIONf, &mode),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_CFG_UNHIOLr, RTL9607C_IPG_COMPENSATIONf, &mode),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_CFG_UNHIOLr, RTL9603CVD_IPG_COMPENSATIONf, &mode),ret);

            break;
#endif 

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_ipg_compensation_65ppm_90ppm */

/*
 * switch get ipg-compensation state
 */
cparser_result_t
cparser_cmd_switch_get_ipg_compensation_state(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable=ENABLED;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_read(SWITCH_CTRLr, SHORT_IPGf, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_SWITCH_CTRLr, APOLLOMP_SHORT_IPGf, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_SHORT_IPGf, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_SWITCH_CTRLr, RTL9602C_SHORT_IPGf, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_SWITCH_CTRLr, RTL9607C_SHORT_IPGf, &enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_SWITCH_CTRLr, RTL9603CVD_SHORT_IPGf, &enable),ret);

            break;
#endif    
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            break;
            
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }


    diag_util_mprintf("Short IPG function: %s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_ipg_compensation_state */

/*
 * switch get ipg-compensation
 */
cparser_result_t
cparser_cmd_switch_get_ipg_compensation(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_switch_ipgCompMode_t mode=IPGCOMPMODE_65PPM;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_read(CFG_UNHIOLr, IPG_COMPENSATIONf, &mode),ret);

            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_CFG_UNHIOLr, APOLLOMP_IPG_COMPENSATIONf, &mode),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_IPG_COMPENSATIONf, &mode),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_CFG_UNHIOLr, RTL9602C_IPG_COMPENSATIONf, &mode),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_CFG_UNHIOLr, RTL9607C_IPG_COMPENSATIONf, &mode),ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_CFG_UNHIOLr, RTL9603CVD_IPG_COMPENSATIONf, &mode),ret);

            break;
#endif    
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            break;
            
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    diag_util_mprintf("IPG compensation: %s\n",diagStr_ipgCompensation[mode]);


    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_ipg_compensation */


/*
 * switch get rx-check-crc port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_switch_get_rx_check_crc_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable=ENABLED;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port       Status \n"); 	
    diag_util_mprintf("-----------------------------\n"); 	
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_read(P_MISCr, port, REG_ARRAY_INDEX_NONE, CRC_SKIPf,&enable),ret);
                if(0==enable)
                    enable= ENABLED;
                else
                    enable= DISABLED;
                break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_P_MISCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_CRC_SKIPf, &enable),ret);
                if(0==enable)
                    enable= ENABLED;
                else
                    enable= DISABLED;
                break;
#endif    
#ifdef CONFIG_SDK_RTL9601B 
            case RTL9601B_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9601B_CRC_SKIPf, &enable),ret);
                if(0==enable)
                    enable= ENABLED;
                else
                    enable= DISABLED;
                break;
#endif    
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9602C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9602C_CRC_SKIPf, &enable),ret);
                if(0==enable)
                    enable= ENABLED;
                else
                    enable= DISABLED;
                break;
#endif
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9607C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9607C_CRC_SKIPf, &enable),ret);
                if(0==enable)
                    enable= ENABLED;
                else
                    enable= DISABLED;
                break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9603CVD_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_CRC_SKIPf, &enable),ret);
                if(0==enable)
                    enable= ENABLED;
                else
                    enable= DISABLED;
                break;
#endif 

            case UNKNOWN_CHIP_ID:
                /*for coverity dead code issue*/
                break;
            
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;        
                break;
        }

        diag_util_mprintf("%-10u  %s\n", port, diagStr_enable[enable]);   
    }	

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_rx_check_crc_port_ports_all_state */

/*
 * switch set rx-check-crc port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_rx_check_crc_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e'==TOKEN_CHAR(6,0))
        enable = DISABLED;
    else if('d'==TOKEN_CHAR(6,0))
        enable = ENABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
     
                DIAG_UTIL_ERR_CHK(reg_array_field_write(P_MISCr, port, REG_ARRAY_INDEX_NONE, CRC_SKIPf,&enable),ret);

                break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
            case APOLLOMP_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_P_MISCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_CRC_SKIPf, &enable),ret);

                break;
#endif    
#ifdef CONFIG_SDK_RTL9601B 
            case RTL9601B_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9601B_CRC_SKIPf, &enable),ret);

                break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
            case RTL9602C_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9602C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9602C_CRC_SKIPf, &enable),ret);

                break;
#endif    
#ifdef CONFIG_SDK_RTL9607C 
            case RTL9607C_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9607C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9607C_CRC_SKIPf, &enable),ret);

                break;
#endif    
#ifdef CONFIG_SDK_RTL9603CVD 
            case RTL9603CVD_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9603CVD_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_CRC_SKIPf, &enable),ret);

                break;
#endif     
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;        
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_rx_check_crc_port_ports_all_state_disable_enable */

/*
 * switch set bypass-tx-crc state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_bypass_tx_crc_state_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else if('d'==TOKEN_CHAR(4,0))
        enable = DISABLED;
  
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_write(CFG_BACKPRESSUREr, EN_BYPASS_ERRORf,&enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_CFG_BACKPRESSUREr, APOLLOMP_EN_BYPASS_ERRORf, &enable),ret);

            break;
#endif    

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_bypass_tx_crc_state_disable_enable */

/*
 * switch get bypass-tx-crc state
 */
cparser_result_t
cparser_cmd_switch_get_bypass_tx_crc_state(
    cparser_context_t *context)
{
    rtk_enable_t enable=ENABLED;
    int32 ret = RT_ERR_FAILED;
    cparser_result_t retVal = CPARSER_OK;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
   
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_read(CFG_BACKPRESSUREr, EN_BYPASS_ERRORf,&enable),ret);

            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_CFG_BACKPRESSUREr, APOLLOMP_EN_BYPASS_ERRORf, &enable),ret);

            break;
#endif  

        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;
            
        default:

            retVal = CPARSER_NOT_OK;
            break;
    }

    if(retVal == CPARSER_OK)
    {
        diag_util_mprintf("Bypass Tx CRC: %s\n", diagStr_enable[enable]); 
        return CPARSER_OK;
    }
    else
    {
        diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
        return retVal; 
    }
        
}    /* end of cparser_cmd_switch_get_bypass_tx_crc_state */


/*
 * switch set tx-crc ( pkt-changed | disable | always )
 */
cparser_result_t
cparser_cmd_switch_set_tx_crc_pkt_changed_disable_always(
    cparser_context_t *context)
{
    dal_rtl9601b_switch_txcrc_type_t type;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    if('p'==TOKEN_CHAR(3,0))
        type = RTL9601B_TXCRC_PKTCHANGED;
    else if('d'==TOKEN_CHAR(3,0))
        type = RTL9601B_TXCRC_DISABLE;
    else if('a'==TOKEN_CHAR(3,0))
        type = RTL9601B_TXCRC_ALWAYS;
        
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_CRCRECALf,&type),ret);
            break;
#endif   
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_SWITCH_CTRLr, RTL9602C_CRCRECALf, &type),ret);
            break;
#endif      
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_SWITCH_CTRLr, RTL9607C_CRCRECALf, &type),ret);
            break;
#endif      
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_SWITCH_CTRLr, RTL9603CVD_CRCRECALf, &type),ret);
            break;
#endif       
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_tx_crc_pkt_changed_disable_always */

/*
 * switch get tx-crc
 */
cparser_result_t
cparser_cmd_switch_get_tx_crc(
    cparser_context_t *context)
{
    dal_rtl9601b_switch_txcrc_type_t type=0;
    int32 ret = RT_ERR_FAILED;
    cparser_result_t retVal = CPARSER_OK;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_CRCRECALf,&type),ret);
            break;
#endif   
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_SWITCH_CTRLr, RTL9602C_CRCRECALf,&type),ret);
            break;
#endif   
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_SWITCH_CTRLr, RTL9607C_CRCRECALf,&type),ret);
            break;
#endif   
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_SWITCH_CTRLr, RTL9603CVD_CRCRECALf,&type),ret);
            break;
#endif    
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            break;
            
        default:
            retVal = CPARSER_NOT_OK;
            break;
    }
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    if(type==0b00)
        diag_util_mprintf("TX CRC config: %s\n", DIAG_STR_PKTCHANGE);
    else if(type==0b01)
        diag_util_mprintf("TX CRC config: %s\n", DIAG_STR_DISABLE);
    else if((type&0x2)==0b10)
        diag_util_mprintf("TX CRC config: %s\n", DIAG_STR_ALWAYS);
#else
    if(retVal == CPARSER_NOT_OK)
    {
        diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
    }
    else
    {
        diag_util_mprintf("TX CRC config: %s\n", diagStr_txRrcType[type]);
    }
#endif    
    
    return retVal;
}    /* end of cparser_cmd_switch_get_tx_crc */




/*
 * switch set mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_switch_set_mac_address_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    rtk_mac_t mac;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    osal_memcpy(&mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);    
    DIAG_UTIL_ERR_CHK(rtk_switch_mgmtMacAddr_set(&mac), ret); 		

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_mac_addr_addr */

/*
 * switch get mac-address
 */
cparser_result_t
cparser_cmd_switch_get_mac_address(
    cparser_context_t *context)
{
    rtk_mac_t mac;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
   
    DIAG_UTIL_ERR_CHK(rtk_switch_mgmtMacAddr_get(&mac), ret); 		

    diag_util_mprintf("Switch MAC Address: %s\n", diag_util_inet_mactoa(&mac.octet[0]));

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_mac_addr_addr */

/*
 * switch set max-pkt-len ( fe | ge ) port ( <PORT_LIST:ports> | all ) ( rx | tx ) length <UINT:len>
 */
cparser_result_t
cparser_cmd_switch_set_max_pkt_len_fe_ge_port_ports_all_rx_tx_length_len(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *len_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    //apollo_raw_linkSpeed_t speed;
    int32 ret = RT_ERR_FAILED;
    uint32 value;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
                
                value = *len_ptr;

                if('r'==TOKEN_CHAR(6,0))
                {
                    if('g'==TOKEN_CHAR(3,0))
                    {
                        DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAX_LENGTH_GIGAf, &value), ret); 	
                    }
                    else if('f'==TOKEN_CHAR(3,0))
                    {
                        DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAX_LENGTH_10_100f, &value), ret); 	
                    }
                }
                else if('t'==TOKEN_CHAR(6,0))
                {
                    if('g'==TOKEN_CHAR(3,0))
                    {
                        DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_TX_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_TX_MAX_LENGTH_GIGAf, &value), ret); 	
                    }
                    else if('f'==TOKEN_CHAR(3,0))
                    {
                        DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_TX_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_TX_MAX_LENGTH_10_100f, &value), ret); 	
                    }                    
                }            
                break;
#endif
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_max_pkt_len_fe_ge_port_ports_all_rx_tx_length_len */

/*
 * switch set max-pkt-len port ( <PORT_LIST:ports> | all ) length <UINT:len>
 */
cparser_result_t
cparser_cmd_switch_set_max_pkt_len_port_ports_all_length_len(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *len_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        DIAG_UTIL_ERR_CHK(rtk_switch_maxPktLenByPort_set(port, *len_ptr), ret);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_max_pkt_len_port_ports_all_length_len */


/*
 * switch get max-pkt-len port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_switch_get_max_pkt_len_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32 value;
            
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port Speed\n"); 	
    diag_util_mprintf("----------\n"); 	
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
    	diag_util_mprintf("%-4d ", port);

    	DIAG_UTIL_ERR_CHK(rtk_switch_maxPktLenByPort_get(port, &value), ret); 	
    	diag_util_mprintf("%-8d \n", value);             
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_max_pkt_len_port_ports_all */

/*
 * switch set max-pkt-len rx-inc-tag port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_switch_set_max_pkt_len_rx_inc_tag_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32 value;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
                if('e'==TOKEN_CHAR(7,0))
                {
                    value = 1;
                }
                else if('d'==TOKEN_CHAR(7,0))
                {
                    value = 0;
                }                
                else
                    return CPARSER_ERR_INVALID_PARAMS;

                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAXLEN_INC_TAGf, &value), ret); 	
                break;
#endif
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
                return CPARSER_NOT_OK;
                break;
        }
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_max_pkt_len_rx_inc_tag_port_ports_all_state_enable_disable */

/*
 * switch get max-pkt-len rx-inc-tag port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_switch_get_max_pkt_len_rx_inc_tag_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32 value;
            
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            diag_util_mprintf("Port rx-inc-tag\n"); 	
            diag_util_mprintf("---- ----------\n"); 	
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                diag_util_mprintf("%-4d ", port);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAXLEN_INC_TAGf, &value), ret);
                diag_util_mprintf("%s\n", diagStr_enable[value]);
            }
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }



    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_max_pkt_len_rx_inc_tag_port_ports_all_state */

/*
 * switch set max-pkt-len ( fe | ge ) port ( <PORT_LIST:ports> | all ) index <UINT:index>
 */
cparser_result_t
cparser_cmd_switch_set_max_pkt_len_fe_ge_port_ports_all_index_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    apollo_raw_linkSpeed_t speed;
    int32 ret = RT_ERR_FAILED;
    uint32 value,regField;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if('g'==TOKEN_CHAR(3,0))
     {
        speed = LINKSPEED_GIGA;
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            regField = APOLLOMP_MAX_LENGTH_GIGAf;
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
             regField = RTL9602C_MAX_LENGTH_GIGAf;
             break;
#endif
        default:
            regField = 0;
            break;
        }
     }
    else if('f'==TOKEN_CHAR(3,0))
    {
        speed = LINKSPEED_100M;
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            regField = APOLLOMP_MAX_LENGTH_10_100f;
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            regField = RTL9602C_MAX_LENGTH_10_100f;
            break;
#endif
        default:
            regField = 0;
            break;
        }
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;	
        
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
    #ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_switch_maxPktLenSpeed_set( port, speed, *index_ptr), ret); 			
                break;
    #endif
    #ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                value = *index_ptr;
                DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, regField, &value), ret); 			
                break;
    #endif
    #ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
                value = *index_ptr;
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9602C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, regField, &value), ret); 			
                break;
    #endif
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_max_pkt_len_fe_ge_port_ports_all_index_index */

/*
 * switch get max-pkt-len ( fe | ge ) port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_switch_get_max_pkt_len_fe_ge_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    apollo_raw_linkSpeed_t speed;
    uint32 index;
    int32 ret = RT_ERR_FAILED;
    uint32 regField;
            
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            diag_util_mprintf("Port       Speed       Config \n"); 	
            diag_util_mprintf("-----------------------------\n"); 	
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                if('g'==TOKEN_CHAR(3,0))
                {
                    speed = LINKSPEED_GIGA;
                }    
                else if('f'==TOKEN_CHAR(3,0))
                {
                    speed = LINKSPEED_100M;
                }
                else
                    return CPARSER_ERR_INVALID_PARAMS;
                
                DIAG_UTIL_ERR_CHK(apollo_raw_switch_maxPktLenSpeed_get(port, speed, &index), ret); 			
                diag_util_mprintf("%-10u  %s    %d\n", port, diagStr_portSpeed[speed], index);   
            }
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            diag_util_mprintf("Port       Speed       Config \n"); 	
            diag_util_mprintf("-----------------------------\n"); 	
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                if('g'==TOKEN_CHAR(3,0))
                {
                    speed = LINKSPEED_GIGA;
                    regField = APOLLOMP_MAX_LENGTH_GIGAf;
                }
                else if('f'==TOKEN_CHAR(3,0))
                {
                    speed = LINKSPEED_100M;
                    regField = APOLLOMP_MAX_LENGTH_10_100f;
                }
                else
                    return CPARSER_ERR_INVALID_PARAMS;
                
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, regField, &index), ret);
                diag_util_mprintf("%-10u  %s    %d\n", port, diagStr_portSpeed[speed], index);   
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            diag_util_mprintf("Port Speed rxLength txLength\n"); 	
            diag_util_mprintf("-----------------------------\n"); 	
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                if('g'==TOKEN_CHAR(3,0))
                {
                    speed = LINKSPEED_GIGA;
                    regField = RTL9601B_RX_MAX_LENGTH_GIGAf;
                }
                else if('f'==TOKEN_CHAR(3,0))
                {
                    speed = LINKSPEED_100M;
                    regField = RTL9601B_RX_MAX_LENGTH_10_100f;
                }
                else
                    return CPARSER_ERR_INVALID_PARAMS;
                
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, regField, &index), ret);
                diag_util_mprintf("%-4d %-5s %-8d ",  port, diagStr_portSpeed[speed], index);   

                if('g'==TOKEN_CHAR(3,0))
                {
                    regField = RTL9601B_TX_MAX_LENGTH_GIGAf;
                }
                else if('f'==TOKEN_CHAR(3,0))
                {
                    regField = RTL9601B_TX_MAX_LENGTH_10_100f;
                }
                else
                    return CPARSER_ERR_INVALID_PARAMS;

                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_TX_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, regField, &index), ret);
                diag_util_mprintf("%-8d\n", index);   
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            diag_util_mprintf("Port       Speed       Config \n");  
            diag_util_mprintf("-----------------------------\n");   
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                if('g'==TOKEN_CHAR(3,0))
                {
                    speed = LINKSPEED_GIGA;
                    regField = RTL9602C_MAX_LENGTH_GIGAf;
                }
                else if('f'==TOKEN_CHAR(3,0))
                {
                    speed = LINKSPEED_100M;
                    regField = RTL9602C_MAX_LENGTH_10_100f;
                }
                else
                    return CPARSER_ERR_INVALID_PARAMS;
                
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9602C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, regField, &index), ret);
                diag_util_mprintf("%-10u  %s    %d\n", port, diagStr_portSpeed[speed], index);   
            }
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_max_pkt_len_fe_ge_port_ports_all */


/*
 * switch set max-pkt-len index <UINT:index> length <UINT:len>
 */
cparser_result_t
cparser_cmd_switch_set_max_pkt_len_index_index_length_len(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *len_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 length;
    uint32 index;
    
    DIAG_UTIL_PARAM_CHK();
    length = *len_ptr;
    index = *index_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:

            switch(index)
            {
                case 0:
                    DIAG_UTIL_ERR_CHK(reg_field_write(MAX_LENGTH_CFG0r, ACCEPT_MAX_LENTH_CFG0f, &length), ret);

                    break;
                case 1:
                    DIAG_UTIL_ERR_CHK(reg_field_write(MAX_LENGTH_CFG1r, ACCEPT_MAX_LENTH_CFG1f, &length), ret);

                    break;
                default:

                    return RT_ERR_CHIP_NOT_SUPPORTED;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
       case APOLLOMP_CHIP_ID:

            switch(index)
            {
                case 0:
                    DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_MAX_LENGTH_CFG0r, APOLLOMP_ACCEPT_MAX_LENTH_CFG0f, &length), ret);

                    break;
                case 1:
                    DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_MAX_LENGTH_CFG1r, APOLLOMP_ACCEPT_MAX_LENTH_CFG1f, &length), ret);

                    break;
                default:

                    return RT_ERR_CHIP_NOT_SUPPORTED;
            }
			break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
       case RTL9602C_CHIP_ID:

            switch(index)
            {
                case 0:
                    DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_MAX_LENGTH_CFG0r, RTL9602C_ACCEPT_MAX_LENTH_CFG0f, &length), ret);

                    break;
                case 1:
                    DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_MAX_LENGTH_CFG1r, RTL9602C_ACCEPT_MAX_LENTH_CFG1f, &length), ret);

                    break;
                default:

                    return RT_ERR_CHIP_NOT_SUPPORTED;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_max_pkt_len_index_index_length_len */

/*
 * switch get max-pkt-len index <UINT:index>
 */
cparser_result_t
cparser_cmd_switch_get_max_pkt_len_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 length=0;
    uint32 index=0;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    index = *index_ptr;
    cparser_result_t retVal = CPARSER_OK;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:

            switch(index)
            {
                case 0:
                    DIAG_UTIL_ERR_CHK(reg_field_read(MAX_LENGTH_CFG0r, ACCEPT_MAX_LENTH_CFG0f, &length), ret);

                    break;
                case 1:
                    DIAG_UTIL_ERR_CHK(reg_field_read(MAX_LENGTH_CFG1r, ACCEPT_MAX_LENTH_CFG1f, &length), ret);

                    break;
                default:

                    retVal = RT_ERR_CHIP_NOT_SUPPORTED;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:

            switch(index)
            {
                case 0:
                    DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_MAX_LENGTH_CFG0r, APOLLOMP_ACCEPT_MAX_LENTH_CFG0f, &length), ret);

                    break;
                case 1:
                    DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_MAX_LENGTH_CFG1r, APOLLOMP_ACCEPT_MAX_LENTH_CFG1f, &length), ret);

                    break;
                default:

                    retVal = RT_ERR_CHIP_NOT_SUPPORTED;
            }    
			break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:

            switch(index)
            {
                case 0:
                    DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_MAX_LENGTH_CFG0r, RTL9602C_ACCEPT_MAX_LENTH_CFG0f, &length), ret);

                    break;
                case 1:
                    DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_MAX_LENGTH_CFG1r, RTL9602C_ACCEPT_MAX_LENTH_CFG1f, &length), ret);

                    break;
                default:

                    retVal = RT_ERR_CHIP_NOT_SUPPORTED;
            }    
            break;
#endif    
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            retVal = CPARSER_NOT_OK;
            break;
    }

    if(retVal == CPARSER_OK)
    {
        diag_util_mprintf("Max-Length Index %u is Length %u bytes.\n", index, length);   
        return CPARSER_OK;
    }
    else
    {
        return retVal;
    }

}    /* end of cparser_cmd_switch_get_max_pkt_len_index_index */

/*
 * switch set limit-pause state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_limit_pause_state_disable_enable(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = ENABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_write(SWITCH_CTRLr, PAUSE_MAX128f,&enable), ret);
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_SWITCH_CTRLr, APOLLOMP_PAUSE_MAX128f, &enable), ret);
            break;
#endif    

#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_PAUSE_MAX128f, &enable), ret);
            break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_SWITCH_CTRLr, RTL9602C_PAUSE_MAX128f, &enable), ret);
            break;
#endif   
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_SWITCH_CTRLr, RTL9607C_PAUSE_MAX128f, &enable), ret);
            break;
#endif  
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_SWITCH_CTRLr, RTL9603CVD_PAUSE_MAX128f, &enable), ret);
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }
	
    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_limit_pause_state_disable_enable */

/*
 * switch get limit-pause state
 */
cparser_result_t
cparser_cmd_switch_get_limit_pause_state(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable=ENABLED;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_read(SWITCH_CTRLr, PAUSE_MAX128f,&enable), ret); 	
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_SWITCH_CTRLr, APOLLOMP_PAUSE_MAX128f, &enable), ret); 
            if(0==enable)
                enable = ENABLED;
            else
                enable = DISABLED;
            break;
#endif    
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_PAUSE_MAX128f, &enable), ret); 	
            if(0==enable)
                enable = ENABLED;
            else
                enable = DISABLED;

            break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_SWITCH_CTRLr, RTL9602C_PAUSE_MAX128f, &enable), ret); 	
            if(0==enable)
                enable = ENABLED;
            else
                enable = DISABLED;

            break;
#endif  
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_SWITCH_CTRLr, RTL9607C_PAUSE_MAX128f, &enable), ret); 	
            if(0==enable)
                enable = ENABLED;
            else
                enable = DISABLED;

            break;
#endif  
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_SWITCH_CTRLr, RTL9603CVD_PAUSE_MAX128f, &enable), ret); 	
            if(0==enable)
                enable = ENABLED;
            else
                enable = DISABLED;

            break;
#endif   
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            break;
            
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }


    diag_util_mprintf("Limit Pause Frame: %s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_limit_pause_state */

/*
 * switch set small-ipg-tag port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_switch_set_small_ipg_tag_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e'==TOKEN_CHAR(6,0))
        enable = ENABLED;
    else if('d'==TOKEN_CHAR(6,0))
        enable = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
    #ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_switch_smallTagIpg_set(port, enable), ret); 			
                break;
    #endif
    #ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_P_MISCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_SMALL_TAG_IPGf, &enable), ret); 			
                break;
    #endif
    #ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9602C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9602C_SMALL_TAG_IPGf, &enable), ret); 			
                break;
    #endif
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9607C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9607C_SMALL_TAG_IPGf, &enable), ret); 			
                break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9603CVD_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_SMALL_TAG_IPGf, &enable), ret);          
                break;
#endif

            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_small_ipg_tag_port_ports_all_state_enable_disable */

/*
 * switch get small-ipg-tag port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_switch_get_small_ipg_tag_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            diag_util_mprintf("Port       Status \n"); 	
            diag_util_mprintf("-----------------------------\n"); 	
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(apollo_raw_switch_smallTagIpg_get(port, &enable), ret); 			
                diag_util_mprintf("%-10u  %s\n", port, diagStr_enable[enable]);   
            }	
            break;
#endif

#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            diag_util_mprintf("Port       Status \n"); 	
            diag_util_mprintf("-----------------------------\n"); 	
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_P_MISCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_SMALL_TAG_IPGf, &enable), ret); 			
                diag_util_mprintf("%-10u  %s\n", port, diagStr_enable[enable]);   
            }	
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            diag_util_mprintf("Port       Status \n");  
            diag_util_mprintf("-----------------------------\n");   
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9602C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9602C_SMALL_TAG_IPGf, &enable), ret);           
                diag_util_mprintf("%-10u  %s\n", port, diagStr_enable[enable]);   
            }   
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            diag_util_mprintf("Port       Status \n");  
            diag_util_mprintf("-----------------------------\n");   
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9607C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9607C_SMALL_TAG_IPGf, &enable), ret);           
                diag_util_mprintf("%-10u  %s\n", port, diagStr_enable[enable]);   
            }   
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            diag_util_mprintf("Port       Status \n");  
            diag_util_mprintf("-----------------------------\n");   
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9603CVD_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_SMALL_TAG_IPGf, &enable), ret);           
                diag_util_mprintf("%-10u  %s\n", port, diagStr_enable[enable]);   
            }   
            break;
#endif 
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }



    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_small_ipg_tag_port_ports_all_state */


/*
 * switch get back-pressure
 */
cparser_result_t
cparser_cmd_switch_get_back_pressure(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 state=ENABLED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_read(CFG_BACKPRESSUREr, LONGTXEf,&state), ret);
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_CFG_BACKPRESSUREr, APOLLOMP_LONGTXEf, &state), ret);
            break;
#endif    
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_BKPRES_METHOD_SELf, &state), ret);
            break;
#endif   
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_CFG_BACKPRESSUREr, RTL9602C_LONGTXEf, &state), ret);
            break;
#endif    
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_CFG_BACKPRESSUREr, RTL9607C_LONGTXEf, &state), ret);
            break;
#endif    
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_CFG_BACKPRESSUREr, RTL9603CVD_LONGTXEf, &state), ret);
            break;
#endif     

        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            break;
            
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }


    diag_util_mprintf("Back-pressure: %s\n",  diagStr_backPressure[state]);   
	
    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_back_pressure */

/*
 * switch set back-pressure ( jam | defer )
 */
cparser_result_t
cparser_cmd_switch_set_back_pressure_jam_defer(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 state;
        
    DIAG_UTIL_PARAM_CHK();

    if('j'==TOKEN_CHAR(3,0))
        state = 0;
    else if('d'==TOKEN_CHAR(3,0))
        state = 1;
    else
        return CPARSER_ERR_INVALID_PARAMS;
        
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            DIAG_UTIL_ERR_CHK(reg_field_write(CFG_BACKPRESSUREr, LONGTXEf,&state), ret);

            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_CFG_BACKPRESSUREr, APOLLOMP_LONGTXEf, &state), ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_BKPRES_METHOD_SELf, &state), ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_CFG_BACKPRESSUREr, RTL9602C_LONGTXEf, &state), ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_CFG_BACKPRESSUREr, RTL9607C_LONGTXEf, &state), ret);

            break;
#endif    
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_CFG_BACKPRESSUREr, RTL9603CVD_LONGTXEf, &state), ret);

            break;
#endif     
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_back_pressure_jam_defer */

/*
 * switch set small-pkt port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_switch_set_small_pkt_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e'==TOKEN_CHAR(6,0))
        enable = ENABLED;
    else if('d'==TOKEN_CHAR(6,0))
        enable = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
     
                DIAG_UTIL_ERR_CHK(reg_array_field_write(P_MISCr, port, REG_ARRAY_INDEX_NONE, RX_SPCf,&enable), ret);
                break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
            case APOLLOMP_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_P_MISCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RX_SPCf, &enable), ret);
                break;
#endif    

#ifdef CONFIG_SDK_RTL9601B 
            case RTL9601B_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9601B_RX_SPCf, &enable), ret);
                break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
            case RTL9602C_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9602C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9602C_RX_SPCf, &enable), ret);
                break;
#endif   
#ifdef CONFIG_SDK_RTL9607C 
            case RTL9607C_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9607C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9607C_RX_SPCf, &enable), ret);
                break;
#endif   
#ifdef CONFIG_SDK_RTL9603CVD 
            case RTL9603CVD_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9603CVD_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_RX_SPCf, &enable), ret);
                break;
#endif    
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;        
                break;
        }

    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_small_pkt_port_ports_all_state_enable_disable */

/*
 * switch get small-pkt port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_switch_get_small_pkt_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable=ENABLED;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port       Status \n"); 	
    diag_util_mprintf("-----------------------------\n"); 	
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
     
                DIAG_UTIL_ERR_CHK(reg_array_field_read(P_MISCr, port, REG_ARRAY_INDEX_NONE, RX_SPCf,&enable), ret);
                break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
            case APOLLOMP_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_P_MISCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RX_SPCf, &enable), ret);
                break;
#endif    
#ifdef CONFIG_SDK_RTL9601B 
            case RTL9601B_CHIP_ID:
                
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9601B_RX_SPCf, &enable), ret);
                break;
#endif 
#ifdef CONFIG_SDK_RTL9602C 
           case RTL9602C_CHIP_ID:
               
               DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9602C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9602C_RX_SPCf, &enable), ret);
               break;
#endif    
#ifdef CONFIG_SDK_RTL9607C 
            case RTL9607C_CHIP_ID:
              
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9607C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9607C_RX_SPCf, &enable), ret);
                break;
#endif    
#ifdef CONFIG_SDK_RTL9603CVD 
            case RTL9603CVD_CHIP_ID:
              
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9603CVD_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_RX_SPCf, &enable), ret);
                break;
#endif     

            case UNKNOWN_CHIP_ID:
                /*for coverity dead code issue*/
                break;
            
           default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;        
                break;
        }
        diag_util_mprintf("%-10u  %s\n", port, diagStr_enable[enable]);   
    }	

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_small_pkt_port_ports_all_state */

/*
 * switch reset ( global | chip | config | queue | pon-mac | serdes | switch-core | gphy )
 */
cparser_result_t
cparser_cmd_switch_reset_global_chip_config_queue_pon_mac_serdes_switch_core_gphy(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    uint32 field,reg,resetVal;
    DIAG_UTIL_PARAM_CHK();

	
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            apollo_raw_chipReset_t reset=RAW_CHIPRESET_END;
            if (!osal_strcmp(TOKEN_STR(2),"global"))
            {
                reset = RAW_SW_GLOBAL_RST;
            }  
            else if (!osal_strcmp(TOKEN_STR(2),"chip"))
            {
                reset = RAW_SW_CHIP_RST;
            }
            else
                return CPARSER_NOT_OK;
        
            DIAG_UTIL_ERR_CHK(apollo_raw_switch_chipReset_set(reset), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            resetVal = 1;
            if (!osal_strcmp(TOKEN_STR(2),"global"))
            {
                field = APOLLOMP_SW_RSTf;
                reg   = APOLLOMP_SOFTWARE_RSTr;
            }  
            else if (!osal_strcmp(TOKEN_STR(2),"chip"))
            {
                field = APOLLOMP_CMD_CHIP_RST_PSf;
                reg   = APOLLOMP_SOFTWARE_RSTr;
            }
            else
                return CPARSER_NOT_OK;
            
            DIAG_UTIL_ERR_CHK(reg_field_write(reg, field, &resetVal), ret);
            
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            resetVal = 1;
            if (!osal_strcmp(TOKEN_STR(2),"global"))
            {
                field = RTL9601B_CMD_SWSYS_RST_PSf;
                reg   = RTL9601B_SOFTWARE_RSTr;
            }  
            else if (!osal_strcmp(TOKEN_STR(2),"chip"))
            {
                field = RTL9601B_CMD_CHIP_RST_PSf;
                reg   = RTL9601B_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"config"))
            {
                field = RTL9601B_CMD_CFG_RSTf;
                reg   = RTL9601B_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"queue"))
            {
                field = RTL9601B_CMD_QUEUE_RST_PSf;
                reg   = RTL9601B_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"pon-mac"))
            {
                field = RTL9601B_CMD_PONMAC_RSTf;
                reg   = RTL9601B_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"serdes"))
            {
                field = RTL9601B_CMD_PON_RSTf;
                reg   = RTL9601B_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"gphy"))
            {
                field = RTL9601B_CMD_GPHY_RST_PSf;
                reg   = RTL9601B_SOFTWARE_RSTr;
            }            
            else
                return CPARSER_NOT_OK;
            
            DIAG_UTIL_ERR_CHK(reg_field_write(reg, field, &resetVal), ret);
            
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        {
            uint32 uartPinMux;

            /* Backup UART pin mux configuration */
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_IO_MODE_ENr, RTL9602C_UART0_ENf, &uartPinMux), ret);

            resetVal = 1;
            if (!osal_strcmp(TOKEN_STR(2),"global"))
            {
                field = RTL9602C_CMD_SWSYS_RST_PSf;
                reg   = RTL9602C_SOFTWARE_RSTr;
            }  
            else if (!osal_strcmp(TOKEN_STR(2),"chip"))
            {
                field = RTL9602C_CMD_CHIP_RST_PSf;
                reg   = RTL9602C_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"config"))
            {
                field = RTL9602C_CMD_CFG_RST_PSf;
                reg   = RTL9602C_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"pon-mac"))
            {
                field = RTL9602C_PONMAC_RSTf;
                reg   = RTL9602C_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"serdes"))
            {
                field = RTL9602C_CMD_SDS_RST_PSf;
                reg   = RTL9602C_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"switch-core"))
            {
                field = RTL9602C_SW_RSTf;
                reg   = RTL9602C_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"gphy"))
            {
                field = RTL9602C_CMD_GPHY_RST_PSf;
                reg   = RTL9602C_SOFTWARE_RSTr;
            }            
            else
                return CPARSER_NOT_OK;
            
            DIAG_UTIL_ERR_CHK(reg_field_write(reg, field, &resetVal), ret);

            /* Restore UART configure if any */
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_IO_MODE_ENr, RTL9602C_UART0_ENf, &uartPinMux), ret);
            break;
        }
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        {
            uint32 uartPinMux;

            /* Backup UART pin mux configuration */
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_IO_MODE_ENr, RTL9607C_UART0_ENf, &uartPinMux), ret);

            resetVal = 1;
            if (!osal_strcmp(TOKEN_STR(2),"global"))
            {
                field = RTL9607C_CMD_SWSYS_RST_PSf;
                reg   = RTL9607C_SOFTWARE_RSTr;
            }  
            else if (!osal_strcmp(TOKEN_STR(2),"chip"))
            {
                field = RTL9607C_CMD_CHIP_RST_PSf;
                reg   = RTL9607C_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"config"))
            {
                field = RTL9607C_CMD_CFG_RST_PSf;
                reg   = RTL9607C_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"pon-mac"))
            {
                field = RTL9607C_PONMAC_RSTf;
                reg   = RTL9607C_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"serdes"))
            {
                field = RTL9607C_CMD_SDS_RST_PSf;
                reg   = RTL9607C_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"switch-core"))
            {
                field = RTL9607C_SW_RSTf;
                reg   = RTL9607C_SOFTWARE_RSTr;
            }
            else if (!osal_strcmp(TOKEN_STR(2),"gphy"))
            {
                field = RTL9607C_CMD_GPHY_RST_PSf;
                reg   = RTL9607C_SOFTWARE_RSTr;
            }            
            else
                return CPARSER_NOT_OK;
            
            DIAG_UTIL_ERR_CHK(reg_field_write(reg, field, &resetVal), ret);
            /* Restore UART configure if any */
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_IO_MODE_ENr, RTL9607C_UART0_ENf, &uartPinMux), ret);
            break;
        }
#endif
#ifdef CONFIG_SDK_RTL9603CVD
                case RTL9603CVD_CHIP_ID:
                {
                    uint32 uartPinMux;
        
                    /* Backup UART pin mux configuration */
                    DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_IO_MODE_ENr, RTL9603CVD_UART0_ENf, &uartPinMux), ret);
        
                    resetVal = 1;
                    if (!osal_strcmp(TOKEN_STR(2),"global"))
                    {
                        field = RTL9603CVD_CMD_SWSYS_RST_PSf;
                        reg   = RTL9603CVD_SOFTWARE_RSTr;
                    }  
                    else if (!osal_strcmp(TOKEN_STR(2),"chip"))
                    {
                        field = RTL9603CVD_CMD_CHIP_RST_PSf;
                        reg   = RTL9603CVD_SOFTWARE_RSTr;
                    }
                    else if (!osal_strcmp(TOKEN_STR(2),"config"))
                    {
                        field = RTL9603CVD_CMD_CFG_RST_PSf;
                        reg   = RTL9603CVD_SOFTWARE_RSTr;
                    }
                    else if (!osal_strcmp(TOKEN_STR(2),"pon-mac"))
                    {
                        field = RTL9603CVD_PONMAC_RSTf;
                        reg   = RTL9603CVD_SOFTWARE_RSTr;
                    }
                    else if (!osal_strcmp(TOKEN_STR(2),"serdes"))
                    {
                        field = RTL9603CVD_CMD_SDS_RST_PSf;
                        reg   = RTL9603CVD_SOFTWARE_RSTr;
                    }
                    else if (!osal_strcmp(TOKEN_STR(2),"switch-core"))
                    {
                        field = RTL9603CVD_SW_RSTf;
                        reg   = RTL9603CVD_SOFTWARE_RSTr;
                    }
                    else if (!osal_strcmp(TOKEN_STR(2),"gphy"))
                    {
                        field = RTL9603CVD_CMD_GPHY_RST_PSf;
                        reg   = RTL9603CVD_SOFTWARE_RSTr;
                    }            
                    else
                        return CPARSER_NOT_OK;
                    
                    DIAG_UTIL_ERR_CHK(reg_field_write(reg, field, &resetVal), ret);
                    /* Restore UART configure if any */
                    DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_IO_MODE_ENr, RTL9603CVD_UART0_ENf, &uartPinMux), ret);
                    break;
                }
#endif
 
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    
/* end of cparser_cmd_switch_reset_global_chip_config_queue_pon_mac_serdes_switch_core_gphy */

/*
 * switch set output-drop port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_output_drop_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
	
    if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
		    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
		    {    
				DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &enable), ret); 			
		   	}
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
		    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
		    {    
				DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &enable), ret); 			
		   	}
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9602C_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &enable), ret);             
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9607C_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &enable), ret);             
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9603CVD_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_ENf, &enable), ret);             
            }
            break;
#endif 
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_output_drop_port_ports_all_state_disable_enable */

/*
 * switch get output-drop port ( <PORT_LIST:ports> | all ) state 
 */
cparser_result_t
cparser_cmd_switch_get_output_drop_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
			diag_util_mprintf("Port Status\n");
		    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
		    {    
				DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &enable), ret); 			
				diag_util_mprintf("%-4d %s\n", port, diagStr_enable[enable]);  
			}
            break;
#endif    
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
			diag_util_mprintf("Port Status\n");
		    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
		    {    
				DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9601B_ENf, &enable), ret); 			
				diag_util_mprintf("%-4d %s\n", port, diagStr_enable[enable]);  
			}
            break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            diag_util_mprintf("Port Status\n");
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9602C_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &enable), ret);          
                diag_util_mprintf("%-4d %s\n", port, diagStr_enable[enable]);  
            }
            break;
#endif  
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            diag_util_mprintf("Port Status\n");
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9607C_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &enable), ret);          
                diag_util_mprintf("%-4d %s\n", port, diagStr_enable[enable]);  
            }
            break;
#endif  
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            diag_util_mprintf("Port Status\n");
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9603CVD_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_ENf, &enable), ret);          
                diag_util_mprintf("%-4d %s\n", port, diagStr_enable[enable]);  
            }
            break;
#endif   
       default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_output_drop_port_ports_all_state */

/*
 * switch set output-drop ( broadcast | unknown-unicast | multicast ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_output_drop_broadcast_unknown_unicast_multicast_state_disable_enable(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
	
    if ('d' == TOKEN_CHAR(5,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(5,0))
    {
        enable = ENABLED;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_OUTPUT_DROP_CFGr, APOLLOMP_OD_BC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_OUTPUT_DROP_CFGr, APOLLOMP_OD_MC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_OUTPUT_DROP_CFGr, APOLLOMP_OD_UC_SELf, &enable), ret);

            }
            else
                return CPARSER_ERR_INVALID_PARAMS;
            break;
#endif    
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_OUTPUT_DROP_CFGr, RTL9601B_OD_BC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_OUTPUT_DROP_CFGr, RTL9601B_OD_MC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_OUTPUT_DROP_CFGr, RTL9601B_OD_UC_SELf, &enable), ret);

            }
            else
                return CPARSER_ERR_INVALID_PARAMS;
            
            break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_OUTPUT_DROP_CFGr, RTL9602C_OD_BC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_OUTPUT_DROP_CFGr, RTL9602C_OD_MC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_OUTPUT_DROP_CFGr, RTL9602C_OD_UC_SELf, &enable), ret);

            }
            else
                return CPARSER_ERR_INVALID_PARAMS;
            break;
#endif    
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_OUTPUT_DROP_CFGr, RTL9607C_OD_BC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
            {
               DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_OUTPUT_DROP_CFGr, RTL9607C_OD_MC_SELf, &enable), ret);
 
            }
            else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
            {
               DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_OUTPUT_DROP_CFGr, RTL9607C_OD_UC_SELf, &enable), ret);

            }
            else
               return CPARSER_ERR_INVALID_PARAMS;
           break;
#endif    
#ifdef CONFIG_SDK_RTL9603CVD 
               case RTL9603CVD_CHIP_ID:
                   if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
                   {
                       DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_OUTPUT_DROP_CFGr, RTL9603CVD_OD_BC_SELf, &enable), ret);
       
                   }
                   else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
                   {
                      DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_OUTPUT_DROP_CFGr, RTL9603CVD_OD_MC_SELf, &enable), ret);
        
                   }
                   else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
                   {
                      DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_OUTPUT_DROP_CFGr, RTL9603CVD_OD_UC_SELf, &enable), ret);
       
                   }
                   else
                      return CPARSER_ERR_INVALID_PARAMS;
                  break;
#endif    
 
       default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_output_drop_broadcast_unknown_unicast_multicast_state_disable_enable */

/*
 * switch get output-drop ( broadcast | unknown-unicast | multicast ) state
 */
cparser_result_t
cparser_cmd_switch_get_output_drop_broadcast_unknown_unicast_multicast_state(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9601B 
        case RTL9601B_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_OUTPUT_DROP_CFGr, RTL9601B_OD_BC_SELf, &enable), ret);
  
            }
            else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_OUTPUT_DROP_CFGr, RTL9601B_OD_MC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_OUTPUT_DROP_CFGr, RTL9601B_OD_UC_SELf, &enable), ret);

            }
            else
                return CPARSER_ERR_INVALID_PARAMS;
        
        	diag_util_mprintf("%s\n", diagStr_enable[enable]);  
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_OUTPUT_DROP_CFGr, APOLLOMP_OD_BC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_OUTPUT_DROP_CFGr, APOLLOMP_OD_MC_SELf, &enable), ret);
 
            }
            else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_OUTPUT_DROP_CFGr, APOLLOMP_OD_UC_SELf, &enable), ret);

            }
            else
                return CPARSER_ERR_INVALID_PARAMS;
        
        	diag_util_mprintf("%s\n", diagStr_enable[enable]);  
            break;
#endif    
#ifdef CONFIG_SDK_RTL9602C 
        case RTL9602C_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_OUTPUT_DROP_CFGr, RTL9602C_OD_BC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_OUTPUT_DROP_CFGr, RTL9602C_OD_MC_SELf, &enable), ret);
  
            }
            else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_OUTPUT_DROP_CFGr, RTL9602C_OD_UC_SELf, &enable), ret);

            }
            else
                return CPARSER_ERR_INVALID_PARAMS;

            diag_util_mprintf("%s\n", diagStr_enable[enable]);  
            break;
#endif    
#ifdef CONFIG_SDK_RTL9607C 
        case RTL9607C_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_OUTPUT_DROP_CFGr, RTL9607C_OD_BC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_OUTPUT_DROP_CFGr, RTL9607C_OD_MC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_OUTPUT_DROP_CFGr, RTL9607C_OD_UC_SELf, &enable), ret);

            }
            else
                return CPARSER_ERR_INVALID_PARAMS;

            diag_util_mprintf("%s\n", diagStr_enable[enable]);  
            break;
#endif     
#ifdef CONFIG_SDK_RTL9603CVD 
        case RTL9603CVD_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_OUTPUT_DROP_CFGr, RTL9603CVD_OD_BC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_OUTPUT_DROP_CFGr, RTL9603CVD_OD_MC_SELf, &enable), ret);

            }
            else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_OUTPUT_DROP_CFGr, RTL9603CVD_OD_UC_SELf, &enable), ret);

            }
            else
                return CPARSER_ERR_INVALID_PARAMS;

            diag_util_mprintf("%s\n", diagStr_enable[enable]);  
            break;
#endif      
       default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

	
	
    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_output_drop_broadcast_unknown_unicast_multicast_state */

/*
 * switch set change-duplex state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_change_duplex_state_disable_enable(
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
    else
        return CPARSER_ERR_INVALID_PARAMS;


    DIAG_UTIL_ERR_CHK(rtk_switch_changeDuplex_set(enable), ret); 	

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_change_duplex_state_disable_enable */

/*
 * switch get change-duplex state
 */
cparser_result_t
cparser_cmd_switch_get_change_duplex_state(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    DIAG_UTIL_ERR_CHK(rtk_switch_changeDuplex_get(&enable), ret); 		


	diag_util_mprintf("%s\n", diagStr_enable[enable]);  
	
    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_change_duplex_state */

/*
 * switch set system-init ( default | igmp | storm | 8696 )
 */
cparser_result_t
cparser_cmd_switch_set_system_init_default_igmp_storm_8696(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
	rtk_switch_system_mode_t mode;

    DIAG_UTIL_PARAM_CHK();

	memset(&mode, 0x00, sizeof(rtk_switch_system_mode_t));
	
    if ('d' == TOKEN_CHAR(3,0))
    {
        mode.initDefault = ENABLED; 
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        mode.initIgmpSnooping = ENABLED;
    }
    else if ('s' == TOKEN_CHAR(3,0))
    {
        mode.initStorm = ENABLED;
    }
    else if ('r' == TOKEN_CHAR(3,0))
    {
        mode.initRldp = ENABLED;
    }
    else if ('8' == TOKEN_CHAR(3,0))
    {
        mode.init8696InterCtrl = ENABLED;
    }
	
	
    DIAG_UTIL_ERR_CHK(rtk_switch_system_init(&mode), ret); 		

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_system_init_default_igmp_storm_8696 */

/*
 * switch set system-init ( lan | wan ) mac <MACADDR:mac> ip <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_switch_set_system_init_lan_wan_mac_mac_ip_ip(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *ip_ptr)
{
    int32     ret = RT_ERR_FAILED;
	rtk_switch_system_mode_t mode;

    DIAG_UTIL_PARAM_CHK();

	memset(&mode, 0x00, sizeof(rtk_switch_system_mode_t));
	
    if ('l' == TOKEN_CHAR(3,0))
    {
        mode.initLan = ENABLED; 
		osal_memcpy(&mode.macLan.octet, mac_ptr->octet, ETHER_ADDR_LEN);    		
		mode.ipLan = *ip_ptr;
    }
    else if ('w' == TOKEN_CHAR(3,0))
    {
        mode.initWan = ENABLED;
		osal_memcpy(&mode.macWan.octet, mac_ptr->octet, ETHER_ADDR_LEN);    		
		mode.ipWan = *ip_ptr;
    }
	
    DIAG_UTIL_ERR_CHK(rtk_switch_system_init(&mode), ret); 		

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_system_init_lan_wan_mac_mac_ip_ip */

/*
 * switch set system-init rldp mac <MACADDR:mac> 
 */
cparser_result_t
cparser_cmd_switch_set_system_init_rldp_mac_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    int32     ret = RT_ERR_FAILED;
	rtk_switch_system_mode_t mode;

    DIAG_UTIL_PARAM_CHK();

	memset(&mode, 0x00, sizeof(rtk_switch_system_mode_t));

	mode.initRldp = ENABLED;
	osal_memcpy(&mode.macLan.octet, mac_ptr->octet, ETHER_ADDR_LEN);   		

    DIAG_UTIL_ERR_CHK(rtk_switch_system_init(&mode), ret); 		

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_system_init_rldp_mac_mac */

/*
 * switch set change-duplex threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_switch_set_change_duplex_threshold_threshold(
    cparser_context_t *context,
    uint32_t  *threshold_ptr)
{
    int32     ret = RT_ERR_FAILED;
    uint32_t value;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    value = *threshold_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9607C 
    case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_CHANGE_DUPLEX_CTRLr, RTL9607C_CFG_CHG_DUP_THRf, &value),ret); 
        break;
#endif    
#ifdef CONFIG_SDK_RTL9603CVD 
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_CHANGE_DUPLEX_CTRLr, RTL9603CVD_CFG_CHG_DUP_THRf, &value),ret); 
        break;
#endif    
    default:
        diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;        
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_change_duplex_threshold_threshold */

/*
 * switch get change-duplex threshold
 */
cparser_result_t
cparser_cmd_switch_get_change_duplex_threshold(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    uint32_t value;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9607C 
    case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_CHANGE_DUPLEX_CTRLr, RTL9607C_CFG_CHG_DUP_THRf, &value),ret); 
        diag_util_mprintf("threshold : %d\n",value);
        break;
#endif     
#ifdef CONFIG_SDK_RTL9603CVD 
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_CHANGE_DUPLEX_CTRLr, RTL9603CVD_CFG_CHG_DUP_THRf, &value),ret); 
        diag_util_mprintf("threshold : %d\n",value);
        break;
#endif      
    default:
        diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;        
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_change_duplex_threshold */

/*
 * switch set change-duplex frccol ( include | exclude )
 */
cparser_result_t
cparser_cmd_switch_set_change_duplex_frccol_include_exclude(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    uint32_t value;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9607C 
    case RTL9607C_CHIP_ID:
        if ('i' == TOKEN_CHAR(4,0))
        {
            value=1;
        }
        else if ('e' == TOKEN_CHAR(4,0))
        {
            value=0;
        }
        else
        {
            break;            
        }

        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_CHANGE_DUPLEX_CTRLr, RTL9607C_CFG_CHG_DUP_CONGESTf, &value),ret); 

        break;
#endif     
#ifdef CONFIG_SDK_RTL9603CVD 
    case RTL9603CVD_CHIP_ID:
        if ('i' == TOKEN_CHAR(4,0))
        {
            value=1;
        }
        else if ('e' == TOKEN_CHAR(4,0))
        {
            value=0;
        }
        else
        {
            break;            
        }

        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_CHANGE_DUPLEX_CTRLr, RTL9603CVD_CFG_CHG_DUP_CONGESTf, &value),ret); 

        break;
#endif      
    default:
        diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_change_duplex_frccol_include_exclude */

/*
 * switch get change-duplex frccol
 */
cparser_result_t
cparser_cmd_switch_get_change_duplex_frccol(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    uint32_t value;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9607C 
    case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_CHANGE_DUPLEX_CTRLr, RTL9607C_CFG_CHG_DUP_CONGESTf, &value),ret); 

        diag_util_mprintf("frccol : %s\n",(value==1)?"include":"exclude");
        break;
#endif     
#ifdef CONFIG_SDK_RTL9603CVD 
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_CHANGE_DUPLEX_CTRLr, RTL9603CVD_CFG_CHG_DUP_CONGESTf, &value),ret); 

        diag_util_mprintf("frccol : %s\n",(value==1)?"include":"exclude");
        break;
#endif     
    default:
        diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;        
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_change_duplex_frccol */

/*
 * switch get thermal 
 */
cparser_result_t
cparser_cmd_switch_get_thermal(
    cparser_context_t *context)
{
    int32    thermalIntger=0;
    int32 thermalDecimal=0;
    float thermalValue=0;
    int32     ret = RT_ERR_FAILED; 
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    DIAG_UTIL_ERR_CHK(rtk_switch_thermal_get(&thermalIntger,&thermalDecimal), ret); 
    #if 1
    diag_util_mprintf("=============\n");
    diag_util_mprintf("%d   %d\n",thermalIntger,thermalDecimal);
    diag_util_mprintf("=============\n");
    #endif
    
    		
    thermalValue = (float)thermalIntger + (float)thermalDecimal/1000000;
        
    diag_util_mprintf("Thermal value :%2.1f\n",thermalValue);

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_thermal */


