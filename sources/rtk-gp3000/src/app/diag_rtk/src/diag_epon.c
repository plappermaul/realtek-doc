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

#include <hal/common/halctrl.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>
#include <rtk/epon.h>


#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <dal/apollomp/raw/apollomp_raw_epon.h>
#include <dal/apollomp/dal_apollomp_epon.h>
#endif


#ifdef CONFIG_SDK_RTL9601B
#include <hal/chipdef/rtl9601b/rtk_rtl9601b_reg_struct.h>
#include <dal/rtl9601b/dal_rtl9601b_epon.h>
#endif

#ifdef CONFIG_SDK_RTL9602C
#include <hal/chipdef/rtl9602c/rtk_rtl9602c_reg_struct.h>
#include <dal/rtl9602c/dal_rtl9602c_epon.h>
#endif

#ifdef CONFIG_SDK_RTL9607C
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_reg_struct.h>
#include <dal/rtl9607c/dal_rtl9607c_epon.h>
#endif

#ifdef CONFIG_SDK_RTL9603CVD
#include <hal/chipdef/rtl9603cvd/rtk_rtl9603cvd_reg_struct.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_epon.h>
#endif



#if defined (FPGA_DEFINED)
void _epon_ind_phyReg_write(uint8 phyId, uint8 reg, uint16 data)
{
    #define PATCH_W_OFFSET 0xC000

    io_mii_phy_reg_write(8, 1, data);

    data = (PATCH_W_OFFSET | ((phyId & 0x1F) << 5) |(reg & 0x1F));
    io_mii_phy_reg_write(8, 0, data);
}
#endif

/*
 * epon init
 */
cparser_result_t
cparser_cmd_epon_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

#if defined (FPGA_DEFINED)
    uint32 value;

    /* PHY */
    _epon_ind_phyReg_write(0x11, 0x00, 0x000a);
    _epon_ind_phyReg_write(0x11, 0x01, 0x0100);
    
    _epon_ind_phyReg_write(0x1d, 0x1a, 0x0000);
    _epon_ind_phyReg_write(0x1d, 0x02, 0x2d16);
    _epon_ind_phyReg_write(0x1d, 0x16, 0xa8b2);
    _epon_ind_phyReg_write(0x1d, 0x03, 0x6041);
    _epon_ind_phyReg_write(0x1d, 0x18, 0xdde4);

    _epon_ind_phyReg_write(0x1d, 0x06, 0xf4f0);
    _epon_ind_phyReg_write(0x1d, 0x05, 0x4003);
    _epon_ind_phyReg_write(0x1d, 0x0f, 0x4fe6);

    _epon_ind_phyReg_write(0x10, 0x01, 0x000c);

    /*FPON FEC patch*/
    _epon_ind_phyReg_write(27, 13, 0x4e6a);
    _epon_ind_phyReg_write(27, 15, 0x1562);
    _epon_ind_phyReg_write(27, 16, 0xbd2a);
    _epon_ind_phyReg_write(27, 12, 0x3);
    _epon_ind_phyReg_write(26, 7 , 0x1059);
#endif


#if defined (FPGA_DEFINED)
    /*reset EPON module*/
    io_mii_phy_reg_write(0x8, 0x7, 0xC00);
#endif

    DIAG_UTIL_ERR_CHK(rtk_epon_init(), ret); 

#if defined (FPGA_DEFINED)
    io_mii_phy_reg_write(0x8, 0x7, 0xE00);
#endif


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_init */
/*
 * epon get bypass-fec state
 */
cparser_result_t
cparser_cmd_epon_get_bypass_fec_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_bypassFecEnable_get(&enable), ret); 
            diag_util_printf("state:%s",diagStr_enable[enable]);
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif             
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_bypass_fec_state */

/*
 * epon set bypass-fec state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_bypass_fec_state_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_bypassFecEnable_set(enable), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif             
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_bypass_fec_state_disable_enable */

/*
 * epon get llid-table <UINT:index>
 */
cparser_result_t
cparser_cmd_epon_get_llid_table_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_llid_entry_t  llidEntry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    memset(&llidEntry,0x0,sizeof(rtk_epon_llid_entry_t));
    
    llidEntry.llidIdx = *index_ptr;
    
    DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
    diag_util_mprintf("idx:%d LLID:%6d valid:%d report_timer:%4d report_timeout:%d mac-address:%s\n",
                            *index_ptr,llidEntry.llid,llidEntry.valid,llidEntry.reportTimer,llidEntry.isReportTimeout,diag_util_inet_mactoa(&llidEntry.mac.octet[0]));
           

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_llid_table_index */

/*
 * epon get llid-table
 */
cparser_result_t
cparser_cmd_epon_get_llid_table(
    cparser_context_t *context)
{
    uint32 index;
    int32 ret = RT_ERR_FAILED;
    rtk_epon_llid_entry_t  llidEntry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    for(index = 0; index<HAL_MAX_LLID_ENTRY(); index++)
    {
        memset(&llidEntry,0x0,sizeof(rtk_epon_llid_entry_t));

        llidEntry.llidIdx = index;
        DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
        diag_util_mprintf("idx:%d LLID:%6d valid:%d report_timer:%4d report_timeout:%d mac-address:%s\n",
                            index,llidEntry.llid,llidEntry.valid,llidEntry.reportTimer,llidEntry.isReportTimeout,diag_util_inet_mactoa(&llidEntry.mac.octet[0]));
    }                

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_llid_table */

/*
 * epon set llid-table <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_llid_table_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    rtk_epon_llid_entry_t  llidEntry;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(5,0))
        enable = ENABLED;
    else
        enable = DISABLED;

     llidEntry.llidIdx = *index_ptr;
        
     DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
     llidEntry.valid = enable;
     DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_llid_table_index_state_disable_enable */

/*
 * epon set llid-table <UINT:index> llid <UINT:llid>
 */
cparser_result_t
cparser_cmd_epon_set_llid_table_index_llid_llid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *llid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_llid_entry_t  llidEntry;
     
    DIAG_UTIL_PARAM_CHK();

    llidEntry.llidIdx = *index_ptr;
        
    DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
    llidEntry.llid = *llid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_llid_table_index_llid_llid */

/*
 * epon set llid-table <UINT:index> report-timer <UINT:timer>
 */
cparser_result_t
cparser_cmd_epon_set_llid_table_index_report_timer_timer(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *timer_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_llid_entry_t  llidEntry;
    
    DIAG_UTIL_PARAM_CHK();


    llidEntry.llidIdx = *index_ptr;
        
    DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
    llidEntry.reportTimer = *timer_ptr;
    DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_llid_table_index_report_timer_timer */

/*
 * epon get mpcp-gate action
 */
cparser_result_t
cparser_cmd_epon_get_mpcp_gate_action(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_gatehandle_t act;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_mpcpGateHandle_get(&act), ret); 
            
            switch(act)
            {
                case APOLLOMP_EPON_GATE_ASIC_HANDLE:
                    diag_util_mprintf("ASIC Handle\n");
                    break;    
                case APOLLOMP_EPON_GATE_ASIC_HANDLE_AND_TRAP_TO_CPU:
                    diag_util_mprintf("ASIC Handle and trap\n");

                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",act);
                    break;                
            }
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            uint32 val;
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_MPCP_CTRr,RTL9601B_GATE_HANDLEf,&val), ret); 
            
            switch(val)
            {
                case 0:
                    diag_util_mprintf("ASIC Handle\n");
                    break;    
                case 1:
                    diag_util_mprintf("ASIC Handle and trap\n");

                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",val);
                    break;                
            }
            break;
        }
#endif  
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        {
            uint32 val;
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_MPCP_CTRr,RTL9602C_GATE_HANDLEf,&val), ret); 
            
            switch(val)
            {
                case 0:
                    diag_util_mprintf("ASIC Handle\n");
                    break;    
                case 1:
                    diag_util_mprintf("ASIC Handle and trap\n");

                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",val);
                    break;                
            }
            break;
        }
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        {
            uint32 val;
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_MPCP_CTRr,RTL9607C_GATE_HANDLEf,&val), ret); 
            
            switch(val)
            {
                case 0:
                    diag_util_mprintf("ASIC Handle\n");
                    break;    
                case 1:
                    diag_util_mprintf("ASIC Handle and trap\n");

                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",val);
                    break;                
            }
            break;
        }
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        {
            uint32 val;
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_MPCP_CTRr,RTL9603CVD_GATE_HANDLEf,&val), ret); 
            
            switch(val)
            {
                case 0:
                    diag_util_mprintf("ASIC Handle\n");
                    break;    
                case 1:
                    diag_util_mprintf("ASIC Handle and trap\n");

                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",val);
                    break;                
            }
            break;
        }
#endif
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_mpcp_gate_action */

/*
 * epon set mpcp-gate action ( asic-only | trap-and-asic ) 
 */
cparser_result_t
cparser_cmd_epon_set_mpcp_gate_action_asic_only_trap_and_asic(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_gatehandle_t act;

            if('a'==TOKEN_CHAR(4,0))
                act = APOLLOMP_EPON_GATE_ASIC_HANDLE;
            else
                act = APOLLOMP_EPON_GATE_ASIC_HANDLE_AND_TRAP_TO_CPU;

            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_mpcpGateHandle_set(act), ret); 
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            uint32 val;

            if('a'==TOKEN_CHAR(4,0))
                val = 0;
            else
                val = 1;

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_MPCP_CTRr,RTL9601B_GATE_HANDLEf,&val), ret); 
            break;
        }
#endif    
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        {
            uint32 val;

            if('a'==TOKEN_CHAR(4,0))
                val = 0;
            else
                val = 1;

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_MPCP_CTRr,RTL9602C_GATE_HANDLEf,&val), ret); 
            break;
        }
#endif    
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        {
            uint32 val;

            if('a'==TOKEN_CHAR(4,0))
                val = 0;
            else
                val = 1;

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_MPCP_CTRr,RTL9607C_GATE_HANDLEf,&val), ret); 
            break;
        }
#endif    
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        {
            uint32 val;

            if('a'==TOKEN_CHAR(4,0))
                val = 0;
            else
                val = 1;

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_MPCP_CTRr,RTL9603CVD_GATE_HANDLEf,&val), ret); 
            break;
        }
#endif
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_mpcp_gate_action_asic_only_trap_and_asic */

/*
 * epon get mpcp-invalid-len action
 */
cparser_result_t
cparser_cmd_epon_get_mpcp_invalid_len_action(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_mpcpHandle_t act;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_mpcpInvalidLenHandle_get(&act), ret); 
            
            switch(act)
            {
                case APOLLOMP_EPON_MPCP_DROP:
                    diag_util_mprintf("Drop\n");
                    break;    
                case APOLLOMP_EPON_MPCP_PASS:
                    diag_util_mprintf("Pass\n");
                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",act);
                    break;                
            }
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            uint32 tmp_val;
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_MPCP_CTRr,RTL9601B_INVALID_LEN_HANDLEf,&tmp_val), ret); 
            
            switch(tmp_val)
            {
                case 0:
                    diag_util_mprintf("Drop\n");
                    break;    
                case 1:
                    diag_util_mprintf("Pass\n");
                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",tmp_val);
                    break;                
            }
            break;
        }
#endif   
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        {
            uint32 tmp_val;
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_MPCP_CTRr,RTL9602C_INVALID_LEN_HANDLEf,&tmp_val), ret); 
            
            switch(tmp_val)
            {
                case 0:
                    diag_util_mprintf("Drop\n");
                    break;    
                case 1:
                    diag_util_mprintf("Pass\n");
                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",tmp_val);
                    break;                
            }
            break;
        }
#endif   

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        {
            uint32 tmp_val;
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_MPCP_CTRr,RTL9607C_INVALID_LEN_HANDLEf,&tmp_val), ret); 
            
            switch(tmp_val)
            {
                case 0:
                    diag_util_mprintf("Drop\n");
                    break;    
                case 1:
                    diag_util_mprintf("Pass\n");
                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",tmp_val);
                    break;                
            }
            break;
        }
#endif   

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        {
            uint32 tmp_val;
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_MPCP_CTRr,RTL9603CVD_INVALID_LEN_HANDLEf,&tmp_val), ret); 
            
            switch(tmp_val)
            {
                case 0:
                    diag_util_mprintf("Drop\n");
                    break;    
                case 1:
                    diag_util_mprintf("Pass\n");
                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",tmp_val);
                    break;                
            }
            break;
        }
#endif   
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_mpcp_invalid_len_action */

/*
 * epon set mpcp-invalid-len action ( drop | pass ) 
 */
cparser_result_t
cparser_cmd_epon_set_mpcp_invalid_len_action_drop_pass(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_gatehandle_t act;

            if('d'==TOKEN_CHAR(4,0))
                act = APOLLOMP_EPON_MPCP_DROP;
            else
                act = APOLLOMP_EPON_MPCP_PASS;

            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_mpcpInvalidLenHandle_set(act), ret); 
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            uint32 writeVal;
            if('d'==TOKEN_CHAR(4,0))
                writeVal = 0;
            else
                writeVal = 1;

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_MPCP_CTRr,RTL9601B_INVALID_LEN_HANDLEf,&writeVal), ret); 
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        {
            uint32 writeVal;
            if('d'==TOKEN_CHAR(4,0))
                writeVal = 0;
            else
                writeVal = 1;

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_MPCP_CTRr,RTL9602C_INVALID_LEN_HANDLEf,&writeVal), ret); 
            break;
        }
#endif  
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        {
            uint32 writeVal;
            if('d'==TOKEN_CHAR(4,0))
                writeVal = 0;
            else
                writeVal = 1;

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_MPCP_CTRr,RTL9607C_INVALID_LEN_HANDLEf,&writeVal), ret); 
            break;
        }
#endif  
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        {
            uint32 writeVal;
            if('d'==TOKEN_CHAR(4,0))
                writeVal = 0;
            else
                writeVal = 1;

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_MPCP_CTRr,RTL9603CVD_INVALID_LEN_HANDLEf,&writeVal), ret); 
            break;
        }
#endif 
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_mpcp_invalid_len_action_drop_pass */

/*
 * epon get register mode
 */
cparser_result_t
cparser_cmd_epon_get_register_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_regMode_t mode;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regMode_get(&mode), ret); 
            
            switch(mode)
            {
                case APOLLOMP_EPON_SW_REG:
                    diag_util_mprintf("SW register\n");
                    break;    
                case APOLLOMP_EPON_HW_REG:
                    diag_util_mprintf("HW register\n");
                    break;    
                default:
                    diag_util_mprintf("unknown mode:%d\n",mode);
                    break;                
            }
            break;
        }
#endif            

        default:
            diag_util_printf("feature not support! always HW register\n");
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_mode */

/*
 * epon set register mode ( asic | sw ) 
 */
cparser_result_t
cparser_cmd_epon_set_register_mode_asic_sw(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_regMode_t mode;

            if('a'==TOKEN_CHAR(4,0))
                mode = APOLLOMP_EPON_HW_REG;
            else
                mode = APOLLOMP_EPON_SW_REG;
                
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regMode_set(mode), ret); 
            
            break;
        }
#endif            
        default:
            diag_util_printf("HW register\n");       
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_mode_asic_sw */

/*
 * epon get register llid-idx
 */
cparser_result_t
cparser_cmd_epon_get_register_llid_idx(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_regReq_t regReq;
    DIAG_UTIL_PARAM_CHK();
      
    DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret); 
    diag_util_mprintf("register llid table index:%d\n",regReq.llidIdx);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_llid_idx */

/*
 * epon set register llid-idx <UINT:index> 
 */
cparser_result_t
cparser_cmd_epon_set_register_llid_idx_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_regReq_t regReq;
    DIAG_UTIL_PARAM_CHK();
      

    DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret); 
    regReq.llidIdx = *index_ptr; 
    DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_set(&regReq), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_llid_idx_index */

/*
 * epon get register state
 */
cparser_result_t
cparser_cmd_epon_get_register_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    rtk_epon_regReq_t regReq;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret); 


    diag_util_printf("state:%s",diagStr_enable[regReq.doRequest]);

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_state */

/*
 * epon set register state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_register_state_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    rtk_epon_regReq_t regReq;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;


    DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret); 
    regReq.doRequest = enable; 
    DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_set(&regReq), ret); 


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_state_disable_enable */

/*
 * epon get register mac-address
 */
cparser_result_t
cparser_cmd_epon_get_register_mac_address(
    cparser_context_t *context)
{
    rtk_mac_t   regMac;
    int32 ret = RT_ERR_FAILED;
  
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regMac_get(&regMac), ret); 
            diag_util_mprintf("dmac data: %s\n",diag_util_inet_mactoa(&regMac.octet[0]));
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            rtk_epon_regReq_t regReq;
            rtk_epon_llid_entry_t llidEntry;

            memset(&regReq,0x0,sizeof(rtk_epon_regReq_t));
            memset(&llidEntry,0x0,sizeof(rtk_epon_llid_entry_t));

            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret);
            llidEntry.llidIdx = regReq.llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret);
            
             
            diag_util_mprintf("dmac data: %s\n",diag_util_inet_mactoa(&llidEntry.mac.octet[0]));
            break;
        }
#endif            
        default:
        {
            rtk_epon_regReq_t regReq;
            rtk_epon_llid_entry_t llidEntry;

            memset(&regReq,0x0,sizeof(rtk_epon_regReq_t));
            memset(&llidEntry,0x0,sizeof(rtk_epon_llid_entry_t));

            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret);
            llidEntry.llidIdx = regReq.llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret);
            
             
            diag_util_mprintf("dmac data: %s\n",diag_util_inet_mactoa(&llidEntry.mac.octet[0]));
            break;
        } 
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_mac_address */

/*
 * epon set register mac-address <MACADDR:mac> 
 */
cparser_result_t
cparser_cmd_epon_set_register_mac_address_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    rtk_mac_t   regMac;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    osal_memcpy(&regMac.octet, mac_ptr->octet, ETHER_ADDR_LEN);    

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regMac_set(&regMac), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            rtk_epon_regReq_t regReq;
            rtk_epon_llid_entry_t llidEntry;
            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret);
            llidEntry.llidIdx = regReq.llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret);
             memcpy(&llidEntry.mac.octet, &regMac.octet, ETHER_ADDR_LEN);
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret);
       

            break;
        }
#endif            
        default:
        {
            rtk_epon_regReq_t regReq;
            rtk_epon_llid_entry_t llidEntry;
            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret);
            llidEntry.llidIdx = regReq.llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret);
             memcpy(&llidEntry.mac.octet, &regMac.octet, ETHER_ADDR_LEN);
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret);
       

            break;
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_mac_address_mac */

/*
 * epon get register pendding-grant
 */
cparser_result_t
cparser_cmd_epon_get_register_pendding_grant(
    cparser_context_t *context)
{
    uint32   grantNum;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regPendingGrantNum_get(&grantNum), ret); 
            diag_util_mprintf("grant number:%d\n",grantNum);        
           
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_RGSTR2r,RTL9601B_REG_PENDDING_GRANTf,&grantNum), ret); 
            diag_util_mprintf("grant number:%d\n",grantNum);        
           
            break;
#endif  
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_RGSTR2r,RTL9602C_REG_PENDDING_GRANTf,&grantNum), ret); 
            diag_util_mprintf("grant number:%d\n",grantNum);        
           
            break;
#endif 
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_RGSTR2r,RTL9607C_REG_PENDDING_GRANTf,&grantNum), ret); 
            diag_util_mprintf("grant number:%d\n",grantNum);        
           
            break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_RGSTR2r,RTL9603CVD_REG_PENDDING_GRANTf,&grantNum), ret); 
            diag_util_mprintf("grant number:%d\n",grantNum);        
           
            break;
#endif 
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_pendding_grant */

/*
 * epon set register pendding-grant <UINT:number> 
 */
cparser_result_t
cparser_cmd_epon_set_register_pendding_grant_number(
    cparser_context_t *context,
    uint32_t  *number_ptr)
{
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regPendingGrantNum_set(*number_ptr), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_RGSTR2r,RTL9601B_REG_PENDDING_GRANTf,number_ptr), ret); 
            break;
#endif  
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_RGSTR2r,RTL9602C_REG_PENDDING_GRANTf,number_ptr), ret); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_RGSTR2r,RTL9607C_REG_PENDDING_GRANTf,number_ptr), ret); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_RGSTR2r,RTL9603CVD_REG_PENDDING_GRANTf,number_ptr), ret); 
            break;
#endif 
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_pendding_grant_index */




/*
 * epon set force-laser-on state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_force_laser_on_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_forceLaserOn_set(enable), ret); 
            break;
#endif       

#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_epon_forceLaserOn_set(enable), ret); 
            break;
#endif       
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_epon_forceLaserOn_set(enable), ret); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_epon_forceLaserOn_set(enable), ret); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_epon_forceLaserOn_set(enable), ret); 
            break;
#endif 

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_force_laser_on_state_disable_enable */

/*
 * epon get force-laser-on state
 */
cparser_result_t
cparser_cmd_epon_get_force_laser_on_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable=ENABLED;

        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_forceLaserOn_get(&enable), ret); 
            
            break;
#endif      

#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9601b_raw_epon_forceLaserOn_get(&enable), ret); 
            
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_epon_forceLaserOn_get(&enable), ret); 
            
            break;
#endif      
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_epon_forceLaserOn_get(&enable), ret); 
            
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_epon_forceLaserOn_get(&enable), ret); 
            
            break;
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;    
    }

    if(CPARSER_NOT_OK==ret)
    {
        return ret;
    }
    else
    {    
        diag_util_printf("state:%s\n",diagStr_enable[enable]);
        return CPARSER_OK;
    }
}    /* end of cparser_cmd_epon_get_force_laser_on_state */

/*
 * epon get local-time
 */
cparser_result_t
cparser_cmd_epon_get_local_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 localTime=0;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_localTime_get(&localTime), ret); 
            
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_LOCAL_TIMEr,RTL9601B_LOCAL_TIMEf,&localTime), ret); 
            
            break;
#endif 
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_LOCAL_TIMEr,RTL9602C_LOCAL_TIMEf,&localTime), ret); 
            
            break;
#endif 
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_LOCAL_TIMEr,RTL9607C_LOCAL_TIMEf,&localTime), ret); 
            
            break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_LOCAL_TIMEr,RTL9603CVD_LOCAL_TIMEf,&localTime), ret); 
            
            break;
#endif 
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;    
    }

    if(CPARSER_NOT_OK==ret)
    {
        return ret;    
    }
    else
    {
        diag_util_printf("Local time:0x%x\n",localTime);
        return CPARSER_OK;
    }
}    /* end of cparser_cmd_epon_get_local_time */

/*
 * epon get grant-shift
 */
cparser_result_t
cparser_cmd_epon_get_grant_shift(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 shiftTime=0;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_grantShift_get(&shiftTime), ret); 
            
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_GRANT_SHIFT_STARTr,RTL9601B_GRANT_SHIFTf,&shiftTime), ret); 
                  
            break;
#endif            
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_GRANT_SHIFT_STARTr,RTL9602C_GRANT_SHIFTf,&shiftTime), ret); 
                  
            break;
#endif 
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_GRANT_SHIFT_STARTr,RTL9607C_GRANT_SHIFTf,&shiftTime), ret); 
                  
            break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_GRANT_SHIFT_STARTr,RTL9603CVD_GRANT_SHIFTf,&shiftTime), ret); 
                  
            break;
#endif 
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;    
    }
    if(CPARSER_NOT_OK==ret)
    {
        return ret;    
    }
    else
    {
        diag_util_printf("shift TQ:0x%x\n",shiftTime);
        return CPARSER_OK;
    }
}    /* end of cparser_cmd_epon_get_grant_shift */

/*
 * epon set grant-shift <UINT:number> 
 */
cparser_result_t
cparser_cmd_epon_set_grant_shift_number(
    cparser_context_t *context,
    uint32_t  *number_ptr)
{
    int32 ret = RT_ERR_FAILED;
       
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_grantShift_set(*number_ptr), ret); 
            
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_GRANT_SHIFT_STARTr,RTL9601B_GRANT_SHIFTf,number_ptr), ret); 
                  
            break;
#endif   
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_GRANT_SHIFT_STARTr,RTL9602C_GRANT_SHIFTf,number_ptr), ret); 
                  
            break;
#endif 
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_GRANT_SHIFT_STARTr,RTL9607C_GRANT_SHIFTf,number_ptr), ret); 
                  
            break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_GRANT_SHIFT_STARTr,RTL9603CVD_GRANT_SHIFTf,number_ptr), ret); 
                  
            break;
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;    
    }
    if(CPARSER_NOT_OK==ret)
    {
        return ret;    
    }
    else
    {
        return CPARSER_OK;
    }
}    /* end of cparser_cmd_epon_set_grant_shift_number */

/*
 * epon get discovery-shift
 */
cparser_result_t
cparser_cmd_epon_get_discovery_shift(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 shiftTime;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_GRANT_SHIFTr,APOLLOMP_DIS_SHIFT_TIMEf,&shiftTime), ret); 
            diag_util_printf("shift TQ:0x%x\n",shiftTime);
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_discovery_shift */

/*
 * epon set discovery-shift <UINT:number> 
 */
cparser_result_t
cparser_cmd_epon_set_discovery_shift_number(
    cparser_context_t *context,
    uint32_t  *number_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 shiftTime;
       
    DIAG_UTIL_PARAM_CHK();
    
    shiftTime = *number_ptr;
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_GRANT_SHIFTr,APOLLOMP_DIS_SHIFT_TIMEf,&shiftTime), ret); 
            
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_discovery_shift_number */

/*
 * epon get discovery-random-delay state 
 */
cparser_result_t
cparser_cmd_epon_get_discovery_random_delay_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_DEBUG1r,APOLLOMP_DIS_RANDOM_DELAY_ENf,&enable), ret); 
            diag_util_printf("state:%s\n",diagStr_enable[enable]);
            break;
#endif            

        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;    
    }
    if(CPARSER_NOT_OK==ret)
    {
        return ret;    
    }
    else
    {
        return CPARSER_OK;
    }
}    /* end of cparser_cmd_epon_get_discovery_random_delay_state */

/*
 * epon set discovery-random-delay state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_discovery_random_delay_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_DEBUG1r,APOLLOMP_DIS_RANDOM_DELAY_ENf,&enable), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_discovery_random_delay_state_disable_enable */

/*
 * epon get mpcp-gate-trap type
 */
cparser_result_t
cparser_cmd_epon_get_mpcp_gate_trap_type(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 type=3;

    char *diagStr_reportType[] = {
        "trap all gate",
        "trap discovery gate",
        "trap normal gate",
        "unknown type"
    };
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_GATE_TRAP_TYPEf,&type), ret); 
            if(type>3)
                type = 3;
            break;
#endif            

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_MPCP_CTRr,RTL9607C_GATE_TRAP_TYPEf,&type), ret); 
            if(type>3)
                type = 3;
            break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_MPCP_CTRr,RTL9603CVD_GATE_TRAP_TYPEf,&type), ret); 
            if(type>3)
                type = 3;
            break;
#endif 

        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    
    diag_util_printf("%s\n",diagStr_reportType[type]);
        
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_mpcp_gate_trap_type */

/*
 * epon set mpcp-gate-trap type ( all | discovery | normal )
 */
cparser_result_t
cparser_cmd_epon_set_mpcp_gate_trap_type_all_discovery_normal(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 type;
    DIAG_UTIL_PARAM_CHK();

    if('a'==TOKEN_CHAR(4,0))
        type = 0;
    else if('d'==TOKEN_CHAR(4,0))
        type = 1;
    else if('n'==TOKEN_CHAR(4,0))
        type = 2;
    else
        type = 2;    

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_GATE_TRAP_TYPEf,&type), ret); 
            break;
#endif            

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_MPCP_CTRr,RTL9607C_GATE_TRAP_TYPEf,&type), ret); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_MPCP_CTRr,RTL9603CVD_GATE_TRAP_TYPEf,&type), ret); 
            break;
#endif 
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;    
    }
    if(CPARSER_NOT_OK==ret)
        return ret;
    else
        return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_mpcp_gate_trap_type_all_discovery_normal */

/*
 * epon get reg-ack flag
 */
cparser_result_t
cparser_cmd_epon_get_reg_ack_flag(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_REG_ACKr,APOLLOMP_ACK_FLAGf,&value), ret); 
            diag_util_printf("ACK flag:0x%x\n",value); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_reg_ack_flag */

/*
 * epon set reg-ack flag <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_reg_ack_flag_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    value = *value_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_REG_ACKr,APOLLOMP_ACK_FLAGf,&value), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_reg_ack_flag_value */

/*
 * epon get reg-ack llid-idx
 */
cparser_result_t
cparser_cmd_epon_get_reg_ack_llid_idx(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_REG_ACKr,APOLLOMP_LLID_IDXf,&value), ret); 
            diag_util_printf("LLID idx:0x%x\n",value); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_REG_ACKr,RTL9601B_LLID_IDXf,&value), ret); 
            diag_util_printf("LLID idx:0x%x\n",value); 
            break;
#endif            

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_reg_ack_llid_idx */

/*
 * epon set reg-ack llid-idx <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_reg_ack_llid_idx_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();


    value = *value_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_REG_ACKr,APOLLOMP_LLID_IDXf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_REG_ACKr,RTL9601B_LLID_IDXf,&value), ret); 
            break;
#endif            

        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_reg_ack_llid_idx_value */

/*
 * epon set reg-ack trigger
 */
cparser_result_t
cparser_cmd_epon_set_reg_ack_trigger(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    value = 1;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_REG_ACKr,APOLLOMP_ACK_TRIGGERf,&value), ret); 
            break;
#endif            
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;
            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;    
    }
    if(CPARSER_NOT_OK==ret)
    {
        return ret;    
    }
    else
    {
        return CPARSER_OK;
    }
}    /* end of cparser_cmd_epon_set_reg_ack_trigger */



/*
 * epon get laser-on time
 */
cparser_result_t
cparser_cmd_epon_get_laser_on_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint8 laserOn,laserOff;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_epon_laserTime_get(&laserOn, &laserOff), ret); 

    diag_util_printf("laser on time(TQ):0X%x\n",laserOn); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_laser_on_time */

/*
 * epon set laser-on time <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_laser_on_time_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint8 laserOn,laserOff;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_epon_laserTime_get(&laserOn, &laserOff), ret); 
    laserOn = *value_ptr;
    DIAG_UTIL_ERR_CHK(rtk_epon_laserTime_set(laserOn, laserOff), ret); 
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_laser_on_time_value */

/*
 * epon get laser-off time 
 */
cparser_result_t
cparser_cmd_epon_get_laser_off_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint8 laserOn,laserOff;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_epon_laserTime_get(&laserOn, &laserOff), ret); 

    diag_util_printf("laser off time(TQ):0X%x\n",laserOff); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_laser_off_time */

/*
 * epon set laser-off time <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_laser_off_time_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint8 laserOn,laserOff;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_epon_laserTime_get(&laserOn, &laserOff), ret); 
    laserOff = *value_ptr;
    DIAG_UTIL_ERR_CHK(rtk_epon_laserTime_set(laserOn, laserOff), ret); 
 
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_laser_off_time_value */

/*
 * epon get laser-on shift-time
 */
cparser_result_t
cparser_cmd_epon_get_laser_on_shift_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value=0;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_ASIC_TIMING_ADJUST2r,RTL9601B_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_ASIC_TIMING_ADJUST2r,RTL9602C_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_ASIC_TIMING_ADJUST2r,RTL9607C_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_ASIC_TIMING_ADJUST2r,RTL9603CVD_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;    
    }
    if(CPARSER_NOT_OK==ret)
    {
        return ret;    
    }
    else
    {
        diag_util_printf("debug laser off shift time(TQ):0X%x\n",value); 
        return CPARSER_OK;
    }
}    /* end of cparser_cmd_epon_get_laser_on_shift_time */

/*
 * epon set laser-on shift-time <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_laser_on_shift_time_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    value = *value_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_ASIC_TIMING_ADJUST2r,RTL9601B_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_ASIC_TIMING_ADJUST2r,RTL9602C_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_ASIC_TIMING_ADJUST2r,RTL9607C_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_ASIC_TIMING_ADJUST2r,RTL9603CVD_LSR_ON_SHIFTf,&value), ret); 
            break;
#endif
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_laser_on_shift_time_value */

/*
 * epon get laser-off shift-time
 */
cparser_result_t
cparser_cmd_epon_get_laser_off_shift_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_OFF_SHIFTf,&value), ret); 
            diag_util_printf("debug laser off shift time(TQ):0X%x\n",value); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_ASIC_TIMING_ADJUST2r,RTL9601B_LSR_OFF_SHIFTf,&value), ret); 
            diag_util_printf("debug laser off shift time(TQ):0X%x\n",value); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_ASIC_TIMING_ADJUST2r,RTL9602C_LSR_OFF_SHIFTf,&value), ret); 
            diag_util_printf("debug laser off shift time(TQ):0X%x\n",value); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_ASIC_TIMING_ADJUST2r,RTL9607C_LSR_OFF_SHIFTf,&value), ret); 
            diag_util_printf("debug laser off shift time(TQ):0X%x\n",value); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_ASIC_TIMING_ADJUST2r,RTL9603CVD_LSR_OFF_SHIFTf,&value), ret); 
            diag_util_printf("debug laser off shift time(TQ):0X%x\n",value); 
            break;
#endif 
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            break;    
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_laser_off_shift_time */

/*
 * epon set laser-off shift-time <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_laser_off_shift_time_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    value = *value_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_OFF_SHIFTf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_ASIC_TIMING_ADJUST2r,RTL9601B_LSR_OFF_SHIFTf,&value), ret); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_ASIC_TIMING_ADJUST2r,RTL9602C_LSR_OFF_SHIFTf,&value), ret); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_ASIC_TIMING_ADJUST2r,RTL9607C_LSR_OFF_SHIFTf,&value), ret); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_ASIC_TIMING_ADJUST2r,RTL9603CVD_LSR_OFF_SHIFTf,&value), ret); 
            break;
#endif 
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_laser_off_shift_time_value */

/*
 * epon get sync-time
 */
cparser_result_t
cparser_cmd_epon_get_sync_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value1;
    uint32 value2;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_SYNC_TIMEr,APOLLOMP_NORMAL_SYNC_TIMEf,&value1), ret); 
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_SYNC_TIMEr,APOLLOMP_DISC_SYNC_TIMEf,&value2), ret); 

            break;
#endif   

#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            uint8 sysncTime;
            DIAG_UTIL_ERR_CHK(rtk_epon_syncTime_get(&sysncTime), ret); 
            value2 = sysncTime;
            value1 = sysncTime;
            break;
        }
#endif   
        default:
        {
            uint8 sysncTime;
            DIAG_UTIL_ERR_CHK(rtk_epon_syncTime_get(&sysncTime), ret); 
            value2 = sysncTime;
            value1 = sysncTime;
            break;    
        }
    }

    diag_util_printf("normal sync time (TQ):%d\n",value1); 
    diag_util_printf("discovery sync time (TQ):%d\n",value2); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_sync_time */

/*
 * epon set sync-time <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_sync_time_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    value = *value_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_SYNC_TIMEr,APOLLOMP_NORMAL_SYNC_TIMEf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_SYNC_TIMEr,RTL9601B_NORMAL_SYNC_TIMEf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_SYNC_TIMEr,RTL9602C_NORMAL_SYNC_TIMEf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_SYNC_TIMEr,RTL9607C_NORMAL_SYNC_TIMEf,&value), ret); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_SYNC_TIMEr,RTL9603CVD_NORMAL_SYNC_TIMEf,&value), ret); 
            break;
#endif 

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_sync_time_value */

/*
 * epon active llid-idx <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_active_llid_idx_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 llidIdx;
    DIAG_UTIL_PARAM_CHK();
    llidIdx = *value_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
       case APOLLOMP_CHIP_ID:
       {
            apollomp_raw_epon_llid_table_t   llidEntry;
            /*set llid to disable*/
            
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_get(llidIdx ,&llidEntry), ret); 
            
            llidEntry.valid = DISABLED;
            llidEntry.llid = 0x7fff;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_set(llidIdx,&llidEntry), ret); 
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regLlidIdx_set(llidIdx), ret); 
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regReguest_set(ENABLED), ret); 
            break;
      }  
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            rtk_epon_llid_entry_t llidEntry;
            rtk_epon_regReq_t  regReq;
            llidEntry.llidIdx = llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
            llidEntry.valid = DISABLED;
            llidEntry.llid = 0x7fff;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret); 

            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret); 
            regReq.doRequest = ENABLED;
            regReq.llidIdx = llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_set(&regReq), ret); 

            break;
        } 
#endif              
        default:
        {
            rtk_epon_llid_entry_t llidEntry;
            rtk_epon_regReq_t  regReq;
            llidEntry.llidIdx = llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
            llidEntry.valid = DISABLED;
            llidEntry.llid = 0x7fff;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret); 

            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_get(&regReq), ret); 
            regReq.doRequest = ENABLED;
            regReq.llidIdx = llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_registerReq_set(&regReq), ret); 
            break;
        } 
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_active_llid_idx_value */

/*
 * epon de-active llid-idx <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_de_active_llid_idx_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 llidIdx;
    DIAG_UTIL_PARAM_CHK();
    llidIdx = *value_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_llid_table_t   llidEntry;
            /*set llid to disable*/
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_get(llidIdx ,&llidEntry), ret); 
            
            llidEntry.valid = DISABLED;
            llidEntry.llid = 0x7fff;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_set(llidIdx,&llidEntry), ret); 
            break;
        }
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            rtk_epon_llid_entry_t llidEntry;
            llidEntry.llidIdx = llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
            llidEntry.valid = DISABLED;
            llidEntry.llid = 0x7fff;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret); 
            break;
        }
#endif  
        default:
        {
            rtk_epon_llid_entry_t llidEntry;
            llidEntry.llidIdx = llidIdx;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_get(&llidEntry), ret); 
            llidEntry.valid = DISABLED;
            llidEntry.llid = 0x7fff;
            DIAG_UTIL_ERR_CHK(rtk_epon_llid_entry_set(&llidEntry), ret); 
            break;
        }    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_de_active_llid_idx_value */

/*
 * epon set report-mode ( normal | report-0-f | force-0 | force-f )
 */
cparser_result_t
cparser_cmd_epon_set_report_mode_normal_report_0_f_force_0_force_f(
    cparser_context_t *context)
{
    rtk_epon_report_mode_t mode;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();


    if('n'==TOKEN_CHAR(3,0))
        mode = RTK_EPON_REPORT_NORMAL;
    else if('r'==TOKEN_CHAR(3,0))
        mode = RTK_EPON_REPORT_0_F;
    else
    {
        if('0'==TOKEN_CHAR(3,6))        
            mode = RTK_EPON_REPORT_FORCE_0;
        else
            mode = RTK_EPON_REPORT_FORCE_F;
    }
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_apollomp_epon_reportMode_set(mode), ret); 
            break;

#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_rtl9601b_epon_reportMode_set(mode), ret); 
            break;

#endif 
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_rtl9602c_epon_reportMode_set(mode), ret); 
            break;

#endif 
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_rtl9607c_epon_reportMode_set(mode), ret); 
            break;

#endif 
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_rtl9603cvd_epon_reportMode_set(mode), ret); 
            break;

#endif 
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_report_mode_normal_report_0_f_force_0_force_f */

/*
 * epon get report-mode
 */
cparser_result_t
cparser_cmd_epon_get_report_mode(
    cparser_context_t *context)
{
    rtk_epon_report_mode_t mode=RTK_EPON_REPORT_END;
    int32 ret = RT_ERR_FAILED;

    char *diagStr_reportType[] = {
        "normal",
        "0x0 or 0xFFFF",
        "force 0x0",
        "force 0xFFFF",
        "threshold report",
        "unknown"
    };


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_apollomp_epon_reportMode_get(&mode), ret); 
            break;

#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_rtl9601b_epon_reportMode_get(&mode), ret); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_rtl9602c_epon_reportMode_get(&mode), ret); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_rtl9607c_epon_reportMode_get(&mode), ret); 
            break;
#endif 

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(dal_rtl9603cvd_epon_reportMode_get(&mode), ret); 
            break;
#endif 
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;    
    }
 
    
    if(CPARSER_NOT_OK==ret)
    {
        return CPARSER_NOT_OK;    
    }
    else
    {
        diag_util_mprintf("report mode:");
        diag_util_mprintf("%s\n",diagStr_reportType[mode]);

        return CPARSER_OK;
    }
}    /* end of cparser_cmd_epon_get_report_mode */





/*
 * epon dump mib-counter { <UINT:llididx> }
 */
cparser_result_t
cparser_cmd_epon_dump_mib_counter_llididx(
    cparser_context_t *context,
    uint32_t  *llididx_ptr)
{
    rtk_epon_counter_t counter;
    int32 ret = RT_ERR_FAILED;
    int i;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    memset(&counter,0x0,sizeof(rtk_epon_counter_t));

    if(4 == TOKEN_NUM() && (NULL != llididx_ptr))
        counter.llidIdx = *llididx_ptr;
    else
        counter.llidIdx = 0;
    
    DIAG_UTIL_ERR_CHK(rtk_epon_mibCounter_get(&counter), ret); 

    diag_util_mprintf("[Per LLID counter--LLID index :%d]\n",counter.llidIdx);
    
    for(i=0;i<10;i++)
        diag_util_mprintf("queue %d             : %8d\n",i,counter.llidIdxCnt.queueTxFrames[i]);

    diag_util_mprintf("mpcpTxReport        : %8d\n",counter.llidIdxCnt.mpcpTxReport);
    diag_util_mprintf("mpcpRxGate          : %8d\n",counter.llidIdxCnt.mpcpRxGate);
    diag_util_mprintf("onuLlidNotBcst      : %8d\n\n",counter.llidIdxCnt.onuLlidNotBcst);

    diag_util_mprintf("[Globol counter]\n");

    diag_util_mprintf("mpcpRxDiscGate      : %8d\n",counter.mpcpRxDiscGate);
    diag_util_mprintf("mpcpTxRegRequest    : %8d\n",counter.mpcpTxRegRequest);
    diag_util_mprintf("crc8Err             : %8d\n",counter.crc8Err);
    diag_util_mprintf("notBcstBitNotOnuLlid: %8d\n",counter.notBcstBitNotOnuLlid);
    diag_util_mprintf("bcstBitPlusOnuLLid  : %8d\n",counter.bcstBitPlusOnuLLid);
    diag_util_mprintf("bcstNotOnuLLid      : %8d\n",counter.bcstNotOnuLLid);
    diag_util_mprintf("[FEC]\n");
    diag_util_mprintf("fecCorrectedBlocks  : %8d\n",counter.fecCorrectedBlocks);
    diag_util_mprintf("fecUncorrectedBlocks: %8d\n",counter.fecUncorrectedBlocks);

    return CPARSER_OK;
}   /* end of cparser_cmd_epon_dump_mib_counter_llididx */

/*
 * epon reset mib-counter { <UINT:llididx> }
 */
cparser_result_t
cparser_cmd_epon_reset_mib_counter_llididx(
    cparser_context_t *context,
    uint32_t  *llididx_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(4 == TOKEN_NUM() && (NULL != llididx_ptr))
        DIAG_UTIL_ERR_CHK(rtk_epon_mibLlidIdx_reset(*llididx_ptr), ret); 
    else
        DIAG_UTIL_ERR_CHK(rtk_epon_mibGlobal_reset(), ret); 


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_reset_mib_counter_llididx */



/*
 * epon set us-fec state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_us_fec_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_epon_usFecState_set(enable), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_us_fec_state_disable_enable */

/*
 * epon get us-fec state
 */
cparser_result_t
cparser_cmd_epon_get_us_fec_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
 
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_epon_usFecState_get(&enable), ret); 
        
    diag_util_mprintf("US FEC state:%s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_us_fec_state */

/*
 * epon set ds-fec state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_ds_fec_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_epon_dsFecState_set(enable), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_ds_fec_state_disable_enable */

/*
 * epon get ds-fec state
 */
cparser_result_t
cparser_cmd_epon_get_ds_fec_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
 
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_epon_dsFecState_get(&enable), ret); 
        
    diag_util_mprintf("DS FEC state:%s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_ds_fec_state */

/*
 * epon set fec-global state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_fec_global_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_epon_fecState_set(enable), ret);
     
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_fec_global_state_disable_enable */

/*
 * epon get fec-global state
 */
cparser_result_t
cparser_cmd_epon_get_fec_global_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_epon_fecState_get(&enable), ret);

    if(enable == ENABLED || enable == DISABLED)
        diag_util_mprintf("Global FEC state:%s\n",diagStr_enable[enable]);
    else
        diag_util_mprintf("Error return state:0x%x\n",enable);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_fec_global_state */

/*
 * epon set laser-polarity mode ( low | high )
 */
cparser_result_t
cparser_cmd_epon_set_laser_polarity_mode_low_high(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_polarity_t polarity;

    DIAG_UTIL_PARAM_CHK();

    if('l'==TOKEN_CHAR(4,0))
        polarity = EPON_POLARITY_LOW;
    else
        polarity = EPON_POLARITY_HIGH;

    DIAG_UTIL_ERR_CHK(rtk_epon_opticalPolarity_set(polarity), ret);
     
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_laser_polarity_mode_low_high */

/*
 * epon get laser-polarity mode
 */
cparser_result_t
cparser_cmd_epon_get_laser_polarity_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_polarity_t polarity;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
 
    DIAG_UTIL_ERR_CHK(rtk_epon_opticalPolarity_get(&polarity), ret);
    
    if(EPON_POLARITY_HIGH==polarity) 
        diag_util_mprintf("Optical polarity: High\n");
    else
        diag_util_mprintf("Optical polarity: Low\n");
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_laser_polarity_mode */

/*
 * epon set laser-state mode ( normal | force-on | force-off )
 */
cparser_result_t
cparser_cmd_epon_set_laser_state_mode_normal_force_on_force_off(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_laser_status_t state;

    DIAG_UTIL_PARAM_CHK();

    if('n'==TOKEN_CHAR(4,0))
        state = RTK_EPON_LASER_STATUS_NORMAL;
    else
    {
        if('f'==TOKEN_CHAR(4,7))
            state = RTK_EPON_LASER_STATUS_FORCE_OFF;
        else
            state = RTK_EPON_LASER_STATUS_FORCE_ON;
    }
    DIAG_UTIL_ERR_CHK(rtk_epon_forceLaserState_set(state), ret);
     
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_laser_state_mode_normal_force_on_force_off */

/*
 * epon get laser-state mode
 */
cparser_result_t
cparser_cmd_epon_get_laser_state_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_epon_laser_status_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
 
    DIAG_UTIL_ERR_CHK(rtk_epon_forceLaserState_get(&state), ret);
    
    diag_util_mprintf("Laser status:");
    if(RTK_EPON_LASER_STATUS_NORMAL==state) 
        diag_util_mprintf("normal\n");
    else if(RTK_EPON_LASER_STATUS_FORCE_ON==state)
        diag_util_mprintf("force-on\n");
    else
        diag_util_mprintf("force-off\n");
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_laser_state_mode */

/*
 * epon set threshold-report llid-idx <UINT:index> queue-set <UINT:qnum> th1 <UINT:th1> th2 <UINT:th2>  th3 <UINT:th3>
 */
cparser_result_t
cparser_cmd_epon_set_threshold_report_llid_idx_index_queue_set_qnum_th1_th1_th2_th2_th3_th3(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *qnum_ptr,
    uint32_t  *th1_ptr,
    uint32_t  *th2_ptr,
    uint32_t  *th3_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 llidIdx;
    rtk_epon_report_threshold_t thRpt;
    
    
    llidIdx = *index_ptr;
    thRpt.levelNum = *qnum_ptr;
    thRpt.th1 = *th1_ptr;
    thRpt.th2 = *th2_ptr;
    thRpt.th3 = *th3_ptr;


    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_epon_thresholdReport_set(llidIdx,&thRpt), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_threshold_report_llid_idx_index_queue_set_qnum_th1_th1_th2_th2_th3_th3 */

/*
 * epon get threshold-report llid-idx <UINT:index>
 */
cparser_result_t
cparser_cmd_epon_get_threshold_report_llid_idx_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 llidIdx;
    rtk_epon_report_threshold_t thRpt;

    memset(&thRpt,0x0,sizeof(rtk_epon_report_threshold_t));
    
    llidIdx = *index_ptr;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_epon_thresholdReport_get(llidIdx,&thRpt), ret);

    diag_util_mprintf("LLID index:%d\n",llidIdx);
    diag_util_mprintf("queue set number:%d\n",thRpt.levelNum);
    diag_util_mprintf("threshold level 1:%d\n",thRpt.th1);
    diag_util_mprintf("threshold level 2:%d\n",thRpt.th2);
    diag_util_mprintf("threshold level 3:%d\n",thRpt.th3);


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_threshold_report_llid_idx_index */

/*
 * epon set gate-analysis start  ( all-gate | discovery-gate )
 */
cparser_result_t cparser_cmd_epon_set_gate_analysis_start_all_gate_discovery_gate(
	cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
	uint32 trigger, type;
    DIAG_UTIL_PARAM_CHK();

    if('a'==TOKEN_CHAR(4,0))
    {
		type = 0x0;
    }
	else if ('d'==TOKEN_CHAR(4,0))
	{
		type = 0x1;
	}
	else
	{
	    return CPARSER_NOT_OK;    
	}

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_RTL9601B
		case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_GATE_ANAr,RTL9601B_LATCH_FILTERf,&type), ret); 
			trigger = 0x1;
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_GATE_ANAr,RTL9601B_GRANT_LATCH_TRIGGERf,&trigger), ret); 
			break;
#endif 
#ifdef CONFIG_SDK_RTL9602C
		case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_GATE_ANAr,RTL9602C_LATCH_FILTERf,&type), ret); 
			trigger = 0x1;
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_GATE_ANAr,RTL9602C_GRANT_LATCH_TRIGGERf,&trigger), ret); 
			break;
#endif 
#ifdef CONFIG_SDK_RTL9607C
		case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_GATE_ANAr,RTL9607C_LATCH_FILTERf,&type), ret); 
			trigger = 0x1;
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_GATE_ANAr,RTL9607C_GRANT_LATCH_TRIGGERf,&trigger), ret); 
			break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
		case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_GATE_ANAr,RTL9603CVD_LATCH_FILTERf,&type), ret); 
			trigger = 0x1;
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_GATE_ANAr,RTL9603CVD_GRANT_LATCH_TRIGGERf,&trigger), ret); 
			break;
#endif 
		default:
			diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);		 
			return CPARSER_NOT_OK;
			break;	  
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_gate_analysis_start */

/* 
 * epon set gate-analysis start selected-llid <UINT:llididx>
 */
cparser_result_t cparser_cmd_epon_set_gate_analysis_start_selected_llid_llididx(
	cparser_context_t *context,
    uint32_t *llididx_ptr)
{
	int32 ret = RT_ERR_FAILED;
	uint32 trigger, type, llid;
	DIAG_UTIL_PARAM_CHK();

	type = 0x2;
	llid = *llididx_ptr;
	
	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_RTL9601B
		case RTL9601B_CHIP_ID:
			DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_GATE_ANAr,RTL9601B_LATCH_FILTERf,&type), ret); 
			DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_GATE_ANAr,RTL9601B_GRANT_LATCH_LLIDf,&llid), ret); 
			trigger = 0x1;
			DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_GATE_ANAr,RTL9601B_GRANT_LATCH_TRIGGERf,&trigger), ret); 
			break;

#endif 
#ifdef CONFIG_SDK_RTL9602C
		case RTL9602C_CHIP_ID:
			DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_GATE_ANAr,RTL9602C_LATCH_FILTERf,&type), ret); 
			DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_GATE_ANAr,RTL9602C_GRANT_LATCH_LLIDf,&llid), ret); 
			trigger = 0x1;
			DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_GATE_ANAr,RTL9602C_GRANT_LATCH_TRIGGERf,&trigger), ret); 
			break;

#endif 
#ifdef CONFIG_SDK_RTL9607C
		case RTL9607C_CHIP_ID:
			DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_GATE_ANAr,RTL9607C_LATCH_FILTERf,&type), ret); 
			DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_GATE_ANAr,RTL9607C_GRANT_LATCH_LLIDf,&llid), ret); 
			trigger = 0x1;
			DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_GATE_ANAr,RTL9607C_GRANT_LATCH_TRIGGERf,&trigger), ret); 
			break;

#endif 
#ifdef CONFIG_SDK_RTL9603CVD
		case RTL9603CVD_CHIP_ID:
			DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_GATE_ANAr,RTL9603CVD_LATCH_FILTERf,&type), ret); 
			DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_GATE_ANAr,RTL9603CVD_GRANT_LATCH_LLIDf,&llid), ret); 
			trigger = 0x1;
			DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_GATE_ANAr,RTL9603CVD_GRANT_LATCH_TRIGGERf,&trigger), ret); 
			break;

#endif 
		default:
			diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);		 
			return CPARSER_NOT_OK;
			break;	  
	}

	return CPARSER_OK;
}

/*
 * epon dump gate-analysis from <UINT:index1> to <UINT:index2>
 */
cparser_result_t cparser_cmd_epon_dump_gate_analysis_from_index1_to_index2(
	cparser_context_t *context,
	uint32_t *index1_ptr,
	uint32_t *index2_ptr)
{
	int32 ret = RT_ERR_FAILED;
	uint32 index, index1, index2;
	uint32 loopcnt=0;
	int32 busy_flag;
	uint32 data0, data1;
	uint32 firstGateStart=0,latestGateEnd=1 ,totalGateLength=0;


    index1 = *index1_ptr;
	index2 = *index2_ptr;
    
	RT_PARAM_CHK(index1>index2, RT_ERR_INPUT);        

#define GATE_ANA_BUSY_TIMEOUT  80000
	
	DIAG_UTIL_PARAM_CHK();

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_RTL9601B
		case RTL9601B_CHIP_ID:

            printf("Index    LLID    Start           Length    Discovery  Force_Report\n");
			for(index = index1; index <= index2; index++)
			{
				DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_TBL_ACCESS_STSr,RTL9601B_INDEXf,&index), ret); 
				do {
					DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_TBL_ACCESS_STSr,RTL9601B_BUSY_FLAGf,&busy_flag), ret); 
    			    loopcnt++;
    			    if(loopcnt>GATE_ANA_BUSY_TIMEOUT)
    			    {
    			        diag_util_printf("Access Timeout\n");	
    			        return CPARSER_NOT_OK;
    			    }
    			} while (busy_flag);

				DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_TBL_ACCESS_RD_DATAr,RTL9601B_DATA0f,&data0), ret); 
				DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_TBL_ACCESS_RD_DATAr,RTL9601B_DATA1f,&data1), ret); 
				
				diag_util_printf("%3d        %d     0x%-8x      %d          %d          %d\n",index, (data1>>17)&0x1, data0, data1&0xFFFF, (data1>>16)&0x1, (data1>>18)&0x1);
                if(index == index1)
                    firstGateStart = data0; 
                totalGateLength = totalGateLength + (data1&0xFFFF);

                if(index == index2)
                    latestGateEnd = data0 + (data1&0xFFFF); 
                
                       
			}
			diag_util_printf("===========================================================\n");
			diag_util_printf("bandwidth: %d(M)", (totalGateLength*1024)/(latestGateEnd-firstGateStart));
			diag_util_printf("===========================================================\n");
			
			
			break;

#endif 

#ifdef CONFIG_SDK_RTL9602C
		case RTL9602C_CHIP_ID:
            printf("Index    LLID    Start           Length    Discovery  Force_Report\n");
			for(index = index1; index <= index2; index++)
			{
				DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_TBL_ACCESS_STSr,RTL9602C_INDEXf,&index), ret); 
				do {
					DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_TBL_ACCESS_STSr,RTL9602C_BUSY_FLAGf,&busy_flag), ret); 
    			    loopcnt++;
    			    if(loopcnt>GATE_ANA_BUSY_TIMEOUT)
    			    {
    			        diag_util_printf("Access Timeout\n");	
    			        return CPARSER_NOT_OK;
    			    }
    			} while (busy_flag);

				DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_TBL_ACCESS_RD_DATAr,RTL9602C_DATA0f,&data0), ret); 
				DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_TBL_ACCESS_RD_DATAr,RTL9602C_DATA1f,&data1), ret); 
				
				diag_util_printf("%3d        %d     0x%-8x      %d          %d          %d\n",index, (data1>>17)&0x7, data0, data1&0xFFFF, (data1>>16)&0x1, (data1>>20)&0x1);
                if(index == index1)
                    firstGateStart = data0; 
                totalGateLength = totalGateLength + (data1&0xFFFF);

                if(index == index2)
                    latestGateEnd = data0 + (data1&0xFFFF); 
			}
			diag_util_printf("===========================================================\n");
			diag_util_printf("bandwidth: %d(M)", (totalGateLength*1024)/(latestGateEnd-firstGateStart));
			diag_util_printf("===========================================================\n");
			break;
#endif 

#ifdef CONFIG_SDK_RTL9607C
		case RTL9607C_CHIP_ID:
            printf("Index    LLID    Start           Length    Discovery  Force_Report\n");
			for(index = index1; index <= index2; index++)
			{
				DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_TBL_ACCESS_STSr,RTL9607C_INDEXf,&index), ret); 
				do {
					DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_TBL_ACCESS_STSr,RTL9607C_BUSY_FLAGf,&busy_flag), ret); 
    			    loopcnt++;
    			    if(loopcnt>GATE_ANA_BUSY_TIMEOUT)
    			    {
    			        diag_util_printf("Access Timeout\n");	
    			        return CPARSER_NOT_OK;
    			    }
    			} while (busy_flag);

				DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_TBL_ACCESS_RD_DATAr,RTL9607C_DATA0f,&data0), ret); 
				DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_TBL_ACCESS_RD_DATAr,RTL9607C_DATA1f,&data1), ret); 
				
				diag_util_printf("%3d        %d     0x%-8x      %d          %d          %d\n",index, (data1>>17)&0x7, data0, data1&0xFFFF, (data1>>16)&0x1, (data1>>20)&0x1);
                if(index == index1)
                    firstGateStart = data0; 
                totalGateLength = totalGateLength + (data1&0xFFFF);

                if(index == index2)
                    latestGateEnd = data0 + (data1&0xFFFF); 
			}
			diag_util_printf("===========================================================\n");
			diag_util_printf("bandwidth: %d(M)", (totalGateLength*1024)/(latestGateEnd-firstGateStart));
			diag_util_printf("===========================================================\n");
			break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
		case RTL9603CVD_CHIP_ID:
            printf("Index    LLID    Start           Length    Discovery  Force_Report\n");
			for(index = index1; index <= index2; index++)
			{
				DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_TBL_ACCESS_STSr,RTL9603CVD_INDEXf,&index), ret); 
				do {
					DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_TBL_ACCESS_STSr,RTL9603CVD_BUSY_FLAGf,&busy_flag), ret); 
    			    loopcnt++;
    			    if(loopcnt>GATE_ANA_BUSY_TIMEOUT)
    			    {
    			        diag_util_printf("Access Timeout\n");	
    			        return CPARSER_NOT_OK;
    			    }
    			} while (busy_flag);

				DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_TBL_ACCESS_RD_DATAr,RTL9603CVD_DATA0f,&data0), ret); 
				DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_TBL_ACCESS_RD_DATAr,RTL9603CVD_DATA1f,&data1), ret); 
				
				diag_util_printf("%3d        %d     0x%-8x      %d          %d          %d\n",index, (data1>>17)&0x7, data0, data1&0xFFFF, (data1>>16)&0x1, (data1>>20)&0x1);
                if(index == index1)
                    firstGateStart = data0; 
                totalGateLength = totalGateLength + (data1&0xFFFF);

                if(index == index2)
                    latestGateEnd = data0 + (data1&0xFFFF); 
			}
			diag_util_printf("===========================================================\n");
			diag_util_printf("bandwidth: %d(M)", (totalGateLength*1024)/(latestGateEnd-firstGateStart));
			diag_util_printf("===========================================================\n");
			break;
#endif 
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

		default:
			diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);		 
			ret = CPARSER_NOT_OK;
			break;	  
	}
    if(CPARSER_NOT_OK==ret)
        return ret;
    else
    	return CPARSER_OK;
}    /* end of cparser_cmd_epon_dump_gate_analysis */
/*
 * epon get priority-to-sid llid-idx <UINT:llid_idx>
 */
cparser_result_t
cparser_cmd_epon_get_priority_to_sid_llid_idx_llid_idx(
    cparser_context_t *context,
    uint32_t  *llid_idx_ptr)
{
    int32 ret = RT_ERR_FAILED;
	uint32 index=0,llid_idx=0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	llid_idx = *llid_idx_ptr;
	RT_PARAM_CHK(HAL_MAX_LLID_ENTRY() <= llid_idx, RT_ERR_INPUT);

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_RTL9601B
		case RTL9601B_CHIP_ID:
			DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_EPON_LLID_SIDMAP_CTRLr, REG_ARRAY_INDEX_NONE, llid_idx, RTL9601B_IDXf, &index), ret);

			break;
#endif 
#ifdef CONFIG_SDK_RTL9602C
		case RTL9602C_CHIP_ID:
			DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9602C_EPON_LLID_SIDMAP_CTRLr, REG_ARRAY_INDEX_NONE, llid_idx, RTL9602C_IDXf, &index), ret);

			break;
#endif 
#ifdef CONFIG_SDK_RTL9607C
		case RTL9607C_CHIP_ID:
			DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9607C_EPON_LLID_SIDMAP_CTRLr, REG_ARRAY_INDEX_NONE, llid_idx, RTL9607C_IDXf, &index), ret);

			break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
		case RTL9603CVD_CHIP_ID:
			DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9603CVD_EPON_LLID_SIDMAP_CTRLr, REG_ARRAY_INDEX_NONE, llid_idx, RTL9603CVD_IDXf, &index), ret);

			break;
#endif 
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

		default:
			diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);		 
			ret = CPARSER_NOT_OK;
			break;	  
	}
	
    if(CPARSER_NOT_OK == ret)
    {
        return ret;    
    }
    else
    {
        diag_util_mprintf("LLID index: %d mapping table index: %d\n",llid_idx, index);
        return CPARSER_OK;
    }
}	 /* end of cparser_cmd_epon_get_priority_to_sid_llid_idx_llid_idx */

/*
 * epon set priority-to-sid llid-idx <UINT:llid_idx> table <UINT:index>
 */
cparser_result_t
cparser_cmd_epon_set_priority_to_sid_llid_idx_llid_idx_table_index(
    cparser_context_t *context,
    uint32_t  *llid_idx_ptr,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    int32 ret = RT_ERR_FAILED;
	uint32 index,llid_idx;

	index = *index_ptr;
	llid_idx = *llid_idx_ptr;
	
	RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() < index), RT_ERR_ENTRY_INDEX);
	RT_PARAM_CHK(HAL_MAX_LLID_ENTRY() <= llid_idx, RT_ERR_INPUT);

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_RTL9601B
		case RTL9601B_CHIP_ID:
			DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_EPON_LLID_SIDMAP_CTRLr, REG_ARRAY_INDEX_NONE, llid_idx, RTL9601B_IDXf, &index), ret);

			break;
#endif 
#ifdef CONFIG_SDK_RTL9602C
		case RTL9602C_CHIP_ID:
			DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9602C_EPON_LLID_SIDMAP_CTRLr, REG_ARRAY_INDEX_NONE, llid_idx, RTL9602C_IDXf, &index), ret);

			break;
#endif 
#ifdef CONFIG_SDK_RTL9607C
		case RTL9607C_CHIP_ID:
			DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9607C_EPON_LLID_SIDMAP_CTRLr, REG_ARRAY_INDEX_NONE, llid_idx, RTL9607C_IDXf, &index), ret);

			break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
		case RTL9603CVD_CHIP_ID:
			DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9603CVD_EPON_LLID_SIDMAP_CTRLr, REG_ARRAY_INDEX_NONE, llid_idx, RTL9603CVD_IDXf, &index), ret);

			break;
#endif 
		default:
			diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);		 
			return CPARSER_NOT_OK;
			break;	  
	}

    return CPARSER_OK;
}	 /* end of cparser_cmd_epon_set_priority_to_sid_llid_idx_llid_idx_table_index */

/*
 * epon set interrupt ( los | register-ok | mpcp-timeout | time-drift | 1pps ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_interrupt_los_register_ok_mpcp_timeout_time_drift_1pps_state_disable_enable(
    cparser_context_t *context)
{
    rtk_epon_intrType_t type=EPON_INTR_LOS;
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable=ENABLED;
    
    DIAG_UTIL_PARAM_CHK();


    if('l'==TOKEN_CHAR(3,0))
    {
		type = EPON_INTR_LOS;
    }
	else if ('r'==TOKEN_CHAR(3,0))
	{
		type = EPON_INTR_REG_FIN;
	}
	else if ('m'==TOKEN_CHAR(3,0))
	{
		type = EPON_INTR_MPCPTIMEOUT;
	}
	else if ('t'==TOKEN_CHAR(3,0))
	{
		type = EPON_INTR_TIMEDRIFT;
	}
	else if ('1'==TOKEN_CHAR(3,0))
	{
		type = EPON_INTR_1PPS;
	}
	
    if('e'==TOKEN_CHAR(5,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_epon_intrMask_set(type,enable), ret);
 
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_interrupt_los_register_ok_mpcp_timeout_time_drift_1pps_state_disable_enable */

/*
 * epon dump debug timer
 */
cparser_result_t
cparser_cmd_epon_dump_debug_timer(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
	uint32 data;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_ADJ_BCf,&data), ret); 
            diag_util_printf("adj-bc:%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_ASIC_TIMING_ADJUST1r,APOLLOMP_RPT_TMGf,&data), ret); 
            diag_util_printf("rpt-tmg:%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_ASIC_TIMING_ADJUST1r,APOLLOMP_REG_TMGf,&data), ret); 
            diag_util_printf("reg-tmg:%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_ASIC_TIMING_ADJUST1r,APOLLOMP_QU_TMGf,&data), ret); 
            diag_util_printf("qu-tmg :%d\n",data); 
            break;
#endif            
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_ASIC_TIMING_ADJUST2r,RTL9601B_ADJ_BCf,&data), ret); 
            diag_util_printf("adj-bc       :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_ASIC_TIMING_ADJUST1r,RTL9601B_RPT_TMGf,&data), ret); 
            diag_util_printf("rpt-tmg      :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_ASIC_TIMING_ADJUST1r,RTL9601B_REG_TMGf,&data), ret); 
            diag_util_printf("reg-tmg      :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_ASIC_TIMING_ADJUST1r,RTL9601B_QU_TMGf,&data), ret); 
            diag_util_printf("qu-tmg       :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_SCH_TIMINGr,RTL9601B_CFG_EPON_SCH_LATENCYf,&data), ret); 
            diag_util_printf("sche latency :%d\n",data); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_ASIC_TIMING_ADJUST2r,RTL9602C_ADJ_BCf,&data), ret); 
            diag_util_printf("adj-bc       :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_ASIC_TIMING_ADJUST1r,RTL9602C_RPT_TMGf,&data), ret); 
            diag_util_printf("rpt-tmg      :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_ASIC_TIMING_ADJUST1r,RTL9602C_REG_TMGf,&data), ret); 
            diag_util_printf("reg-tmg      :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_ASIC_TIMING_ADJUST1r,RTL9602C_QU_TMGf,&data), ret); 
            diag_util_printf("qu-tmg       :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_SCH_TIMINGr,RTL9602C_CFG_EPON_SCH_LATENCYf,&data), ret); 
            diag_util_printf("sche latency :%d\n",data); 

            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_ASIC_TIMING_ADJUST2r,RTL9602C_DY_ADJ_BCf,&data), ret); 
            diag_util_printf("adj-bc       :%d, %d(TQ)\n",data,(data==0?0:2^(data-1))); 

            
            break;
#endif 

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_ASIC_TIMING_ADJUST2r,RTL9607C_ADJ_BCf,&data), ret); 
            diag_util_printf("adj-bc       :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_ASIC_TIMING_ADJUST1r,RTL9607C_RPT_TMGf,&data), ret); 
            diag_util_printf("rpt-tmg      :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_ASIC_TIMING_ADJUST1r,RTL9607C_REG_TMGf,&data), ret); 
            diag_util_printf("reg-tmg      :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_ASIC_TIMING_ADJUST1r,RTL9607C_QU_TMGf,&data), ret); 
            diag_util_printf("qu-tmg       :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_SCH_TIMINGr,RTL9607C_CFG_EPON_SCH_LATENCYf,&data), ret); 
            diag_util_printf("sche latency :%d\n",data); 

            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_ASIC_TIMING_ADJUST2r,RTL9607C_DY_ADJ_BCf,&data), ret); 
            diag_util_printf("adj-bc       :%d, %d(TQ)\n",data,(data==0?0:2^(data-1))); 
            break;
#endif 
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_ASIC_TIMING_ADJUST2r,RTL9603CVD_ADJ_BCf,&data), ret); 
            diag_util_printf("adj-bc       :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_ASIC_TIMING_ADJUST1r,RTL9603CVD_RPT_TMGf,&data), ret); 
            diag_util_printf("rpt-tmg      :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_ASIC_TIMING_ADJUST1r,RTL9603CVD_REG_TMGf,&data), ret); 
            diag_util_printf("reg-tmg      :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_ASIC_TIMING_ADJUST1r,RTL9603CVD_QU_TMGf,&data), ret); 
            diag_util_printf("qu-tmg       :%d\n",data); 
            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_SCH_TIMINGr,RTL9603CVD_CFG_EPON_SCH_LATENCYf,&data), ret); 
            diag_util_printf("sche latency :%d\n",data); 

            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_ASIC_TIMING_ADJUST2r,RTL9603CVD_DY_ADJ_BCf,&data), ret); 
            diag_util_printf("adj-bc       :%d, %d(TQ)\n",data,(data==0?0:2^(data-1))); 
            break;
#endif 
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_dump_debug_timer */

/*
 * epon dump debug counter
 */
cparser_result_t
cparser_cmd_epon_dump_debug_counter(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
	uint32 data,i;
    rtk_epon_dbgCnt_t dbgCnt;
    memset(&dbgCnt,0x0,sizeof(rtk_epon_dbgCnt_t));

    DIAG_UTIL_ERR_CHK(rtk_epon_dbgInfo_get(&dbgCnt), ret);

    diag_util_printf("[interrupt status]\n"); 
    diag_util_printf("register-fin-int : %d\n",dbgCnt.regFinIntCnt); 
    diag_util_printf("drift-int        : %d\n",dbgCnt.timeDriftIntCnt); 
    diag_util_printf("los-int          : %d\n",dbgCnt.losIntCnt); 
    diag_util_printf("mpcp-timeout-int : %d\n",dbgCnt.mpcpIntCnt); 
    diag_util_printf("  llid-index-int : "); 

    for(i=0;i<HAL_MAX_LLID_ENTRY();i++)
    {
        diag_util_printf("%d ",dbgCnt.mpcpTimeoutCnt[i]); 
    }
    diag_util_printf("\n"); 
    
    diag_util_printf("register-success : %d\n",dbgCnt.regSuccessCnt); 
    diag_util_printf("register-fail    : %d\n",dbgCnt.regFailCnt); 
    diag_util_printf("fec-int          : %d\n",dbgCnt.fecIntCnt); 
    diag_util_printf("1pps-int         : %d\n",dbgCnt.tod1ppsIntCnt); 


    diag_util_printf("[dbg cnt]\n"); 
    diag_util_printf("draint-out-fail  : %d\n",dbgCnt.queueDraintFailCnt); 
    
    diag_util_printf("[special gate cnt]\n"); 
    diag_util_printf("hidden           : %d\n",dbgCnt.gateHidden); 
    diag_util_printf("back-to-back     : %d\n",dbgCnt.gateBackToBack); 
    diag_util_printf("non-force-report : %d\n",dbgCnt.gatenoForceReport); 
    

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_dump_debug_counter */

/*
 * epon set debug-timer ( adj-bc | dy-adjbc | rpt-tmg | reg-tmg | qu-tmg | sch-lantency ) <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_debug_timer_adj_bc_dy_adjbc_rpt_tmg_reg_tmg_qu_tmg_sch_lantency_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;

	uint32 reg=0,field=0,data;
    DIAG_UTIL_PARAM_CHK();

    data = *value_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:    
            if('a'==TOKEN_CHAR(3,0))
            {
        		reg   = APOLLOMP_EPON_ASIC_TIMING_ADJUST2r;
        		field = APOLLOMP_ADJ_BCf;
            }
        	else if ('q'==TOKEN_CHAR(3,0))
        	{
        		reg   = APOLLOMP_EPON_ASIC_TIMING_ADJUST1r;
        		field = APOLLOMP_QU_TMGf;
        	}
        	else if ('g'==TOKEN_CHAR(3,0))
        	{
                diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
                return CPARSER_NOT_OK;
        	}
        	else if ('s'==TOKEN_CHAR(3,0))
        	{
                diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
                return CPARSER_NOT_OK;        	}
        	else if ('r'==TOKEN_CHAR(3,0))
        	{
                if ('p'==TOKEN_CHAR(3,1))
                {/*rpt_tmg*/
            		reg   = APOLLOMP_EPON_ASIC_TIMING_ADJUST1r;
            		field = APOLLOMP_RPT_TMGf;                    
                }
                else
                {
             		reg   = APOLLOMP_EPON_ASIC_TIMING_ADJUST1r;
            		field = APOLLOMP_REG_TMGf;                   
                }
        	}
         	DIAG_UTIL_ERR_CHK(reg_field_write(reg,field,&data), ret); 
            return CPARSER_OK;

        break;
#endif        
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            if('a'==TOKEN_CHAR(3,0))
            {
        		reg   = RTL9601B_EPON_ASIC_TIMING_ADJUST2r;
        		field = RTL9601B_ADJ_BCf;
            }
        	else if ('q'==TOKEN_CHAR(3,0))
        	{
        		reg   = RTL9601B_EPON_ASIC_TIMING_ADJUST1r;
        		field = RTL9601B_QU_TMGf;
        	}
        	else if ('s'==TOKEN_CHAR(3,0))
        	{
        		reg   = RTL9601B_EPON_SCH_TIMINGr;
        		field = RTL9601B_CFG_EPON_SCH_LATENCYf;
            }
        	else if ('r'==TOKEN_CHAR(3,0))
        	{
                if ('p'==TOKEN_CHAR(3,1))
                {/*rpt_tmg*/
            		reg   = RTL9601B_EPON_ASIC_TIMING_ADJUST1r;
            		field = RTL9601B_RPT_TMGf;                    
                }
                else
                {
             		reg   = RTL9601B_EPON_ASIC_TIMING_ADJUST1r;
            		field = RTL9601B_REG_TMGf;                   
                }
        	}
         	DIAG_UTIL_ERR_CHK(reg_field_write(reg,field,&data), ret); 
            return CPARSER_OK;

        break;        
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            if('a'==TOKEN_CHAR(3,0))
            {
        		reg   = RTL9602C_EPON_ASIC_TIMING_ADJUST2r;
        		field = RTL9602C_ADJ_BCf;
            }
        	else if ('q'==TOKEN_CHAR(3,0))
        	{
        		reg   = RTL9602C_EPON_ASIC_TIMING_ADJUST1r;
        		field = RTL9602C_QU_TMGf;
        	}
        	else if ('s'==TOKEN_CHAR(3,0))
        	{
        		reg   = RTL9602C_EPON_SCH_TIMINGr;
        		field = RTL9602C_CFG_EPON_SCH_LATENCYf;
            }
        	else if ('r'==TOKEN_CHAR(3,0))
        	{
                if ('p'==TOKEN_CHAR(3,1))
                {/*rpt_tmg*/
            		reg   = RTL9602C_EPON_ASIC_TIMING_ADJUST1r;
            		field = RTL9602C_RPT_TMGf;                    
                }
                else
                {
             		reg   = RTL9602C_EPON_ASIC_TIMING_ADJUST1r;
            		field = RTL9602C_REG_TMGf;                   
                }
        	}
        	else if ('d'==TOKEN_CHAR(3,0))
        	{
        		reg   = RTL9602C_EPON_ASIC_TIMING_ADJUST2r;
        		field = RTL9602C_DY_ADJ_BCf;
            }

         	DIAG_UTIL_ERR_CHK(reg_field_write(reg,field,&data), ret); 
            return CPARSER_OK;

        break;        
#endif

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            if('a'==TOKEN_CHAR(3,0))
            {
        		reg   = RTL9607C_EPON_ASIC_TIMING_ADJUST2r;
        		field = RTL9607C_ADJ_BCf;
            }
        	else if ('q'==TOKEN_CHAR(3,0))
        	{
        		reg   = RTL9607C_EPON_ASIC_TIMING_ADJUST1r;
        		field = RTL9607C_QU_TMGf;
        	}
        	else if ('s'==TOKEN_CHAR(3,0))
        	{
        		reg   = RTL9607C_EPON_SCH_TIMINGr;
        		field = RTL9607C_CFG_EPON_SCH_LATENCYf;
            }
        	else if ('r'==TOKEN_CHAR(3,0))
        	{
                if ('p'==TOKEN_CHAR(3,1))
                {/*rpt_tmg*/
            		reg   = RTL9607C_EPON_ASIC_TIMING_ADJUST1r;
            		field = RTL9607C_RPT_TMGf;                    
                }
                else
                {
             		reg   = RTL9607C_EPON_ASIC_TIMING_ADJUST1r;
            		field = RTL9607C_REG_TMGf;                   
                }
        	}
        	else if ('d'==TOKEN_CHAR(3,0))
        	{
        		reg   = RTL9607C_EPON_ASIC_TIMING_ADJUST2r;
        		field = RTL9607C_DY_ADJ_BCf;
            }
         	DIAG_UTIL_ERR_CHK(reg_field_write(reg,field,&data), ret); 
            return CPARSER_OK;
        break;        
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            if('a'==TOKEN_CHAR(3,0))
            {
        		reg   = RTL9603CVD_EPON_ASIC_TIMING_ADJUST2r;
        		field = RTL9603CVD_ADJ_BCf;
            }
        	else if ('q'==TOKEN_CHAR(3,0))
        	{
        		reg   = RTL9603CVD_EPON_ASIC_TIMING_ADJUST1r;
        		field = RTL9603CVD_QU_TMGf;
        	}
        	else if ('s'==TOKEN_CHAR(3,0))
        	{
        		reg   = RTL9603CVD_EPON_SCH_TIMINGr;
        		field = RTL9603CVD_CFG_EPON_SCH_LATENCYf;
            }
        	else if ('r'==TOKEN_CHAR(3,0))
        	{
                if ('p'==TOKEN_CHAR(3,1))
                {/*rpt_tmg*/
            		reg   = RTL9603CVD_EPON_ASIC_TIMING_ADJUST1r;
            		field = RTL9603CVD_RPT_TMGf;                    
                }
                else
                {
             		reg   = RTL9603CVD_EPON_ASIC_TIMING_ADJUST1r;
            		field = RTL9603CVD_REG_TMGf;                   
                }
        	}
        	else if ('d'==TOKEN_CHAR(3,0))
        	{
        		reg   = RTL9603CVD_EPON_ASIC_TIMING_ADJUST2r;
        		field = RTL9603CVD_DY_ADJ_BCf;
            }

         	DIAG_UTIL_ERR_CHK(reg_field_write(reg,field,&data), ret); 
            return CPARSER_OK;
        break;        

#endif
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return  CPARSER_NOT_OK; 
        break;        

    }
    
}    /* end of cparser_cmd_epon_set_debug_timer_adj_bc_dy_adjbc_rpt_tmg_reg_tmg_qu_tmg_sch_lantency_value */


/*
 * epon set rtt-adj ( increase | decrease ) <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_rtt_adj_increase_decrease_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 data;
    int16 tempData,setValue;
    setValue = (int16)*value_ptr; 
    DIAG_UTIL_PARAM_CHK();

    if('i'==TOKEN_CHAR(3,0))
    {
        tempData = 0-setValue;    
    }
    else
    {
        tempData = setValue;
    }
    data = (uint32)tempData;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:    
        DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EPON_TIME_CTRLr,APOLLOMP_RTT_ADJf,&data), ret);
        break;
#endif        
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_TIME_CTRLr,RTL9601B_RTT_ADJf,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_TIME_CTRLr,RTL9602C_RTT_ADJf,&data), ret);

        break;        
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_TIME_CTRLr,RTL9607C_RTT_ADJf,&data), ret);

        break;        
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_TIME_CTRLr,RTL9603CVD_RTT_ADJf,&data), ret);

        break;        
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;
    }

    if(CPARSER_NOT_OK==ret)
    {
        return ret;    
    }
    else
    {
        return CPARSER_OK;
    }
}    /* end of cparser_cmd_epon_set_rtt_adj_increase_decrease_value */

/*
 * epon get rtt-adj
 */
cparser_result_t
cparser_cmd_epon_get_rtt_adj(
    cparser_context_t *context)
{
    uint32 data=0;
    int32 ret = RT_ERR_FAILED;
     
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:    
        DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EPON_TIME_CTRLr,APOLLOMP_RTT_ADJf,&data), ret);
        ret = CPARSER_OK;
        break;
#endif        
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_TIME_CTRLr,RTL9601B_RTT_ADJf,&data), ret);
        ret = CPARSER_OK;
        break;        
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_TIME_CTRLr,RTL9602C_RTT_ADJf,&data), ret);

        ret = CPARSER_OK;
        break;        
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_TIME_CTRLr,RTL9607C_RTT_ADJf,&data), ret);

        ret = CPARSER_OK;
        break;        
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_TIME_CTRLr,RTL9603CVD_RTT_ADJf,&data), ret);
        ret = CPARSER_OK;
        break;        
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            data = 0xffff;
            ret = CPARSER_OK;
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;
    }

    diag_util_printf("RTT adj:%d  (0x%4.4x) %s\n",(int16)data,data,(((int16)data)<0)?"increase":"decrease");        
    return ret;    
}    /* end of cparser_cmd_epon_get_rtt_adj */

/*
 * epon set program-grant llid-idx <UINT:index> <UINT:length>
 */
cparser_result_t
cparser_cmd_epon_set_program_grant_llid_idx_index_length(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *length_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_program_grant_llid_idx_index_length */

/*
 * epon set stop-local-time state ( enable | disable ) 
 */
cparser_result_t
cparser_cmd_epon_set_stop_local_time_state_enable_disable(
    cparser_context_t *context)
{
    uint32 data;
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
    {
        data=1;    
    }
    else
    {
        data=0;
    }

    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:    
        DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_EP_MISCr,APOLLOMP_STOP_LOCAL_TIMEf,&data), ret);

        break;
#endif        
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EP_MISCr,RTL9601B_STOP_LOCAL_TIMEf,&data), ret);

        break;        
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EP_MISCr,RTL9602C_STOP_LOCAL_TIMEf,&data), ret);

        break;        
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EP_MISCr,RTL9607C_STOP_LOCAL_TIMEf,&data), ret);

        break;        
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EP_MISCr,RTL9603CVD_STOP_LOCAL_TIMEf,&data), ret);

        break;        
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;
    }

    if(CPARSER_NOT_OK==ret)
    {
        return ret;    
    }
    else
    {
        return CPARSER_OK;
    }
}    /* end of cparser_cmd_epon_set_stop_local_time_state_enable_disable */

/*
 * epon get stop-local-time state
 */
cparser_result_t
cparser_cmd_epon_get_stop_local_time_state(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    uint32 data=ENABLED;
    int32 ret = RT_ERR_FAILED;
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:    
        DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_EP_MISCr,APOLLOMP_STOP_LOCAL_TIMEf,&data), ret);

        break;
#endif        
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EP_MISCr,RTL9601B_STOP_LOCAL_TIMEf,&data), ret);

        break;        
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EP_MISCr,RTL9602C_STOP_LOCAL_TIMEf,&data), ret);

        break;        
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EP_MISCr,RTL9607C_STOP_LOCAL_TIMEf,&data), ret);

        break;        
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EP_MISCr,RTL9603CVD_STOP_LOCAL_TIMEf,&data), ret);

        break;        
#endif

        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }
    
    diag_util_printf("\nstop local time state:%s\n",diagStr_enable[data]);  
    
        
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_stop_local_time_state */

/*
 * epon set stop-sync-olt-localtime state ( enable | disable ) 
 */
cparser_result_t
cparser_cmd_epon_set_stop_sync_olt_localtime_state_enable_disable(
    cparser_context_t *context)
{
    uint32 data;
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
    {
        data=1;    
    }
    else
    {
        data=0;
    }
    switch(DIAG_UTIL_CHIP_TYPE)
    {
   
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        {
            uint32 tmpData;
            DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_RSVD_EPON_CTRLr, REG_ARRAY_INDEX_NONE, 0, RTL9601B_RSVD_MEMf, &tmpData), ret);
         
            if(data == 0)
            {
                tmpData = tmpData & (~(1 << 5));    
            }
            else
            {
                tmpData = tmpData | (1 << 5);    
            }
            
            DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_RSVD_EPON_CTRLr, REG_ARRAY_INDEX_NONE, 0, RTL9601B_RSVD_MEMf, &tmpData), ret);

        }
        break;        
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_ASIC_OPTI1r,RTL9602C_STOP_SYNCf,&data), ret);

        break;        
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_ASIC_OPTI1r,RTL9607C_STOP_SYNCf,&data), ret);

        break;        
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_ASIC_OPTI1r,RTL9603CVD_STOP_SYNCf,&data), ret);

        break;        
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_stop_sync_olt_localtime_state_enable_disable */

/*
 * epon get stop-sync-olt-localtime state
 */
cparser_result_t
cparser_cmd_epon_get_stop_sync_olt_localtime_state(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    uint32 data=0;
    int32 ret = RT_ERR_FAILED;
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_DBG_LCTM_DRFr,RTL9601B_UPD_LCL_TYPEf,&data), ret);
        ret = CPARSER_OK;
        break;        
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_DBG_LCTM_DRFr,RTL9602C_UPD_LCL_TYPEf,&data), ret);
        ret = CPARSER_OK;

        break;        
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_DBG_LCTM_DRFr,RTL9607C_UPD_LCL_TYPEf,&data), ret);
        ret = CPARSER_OK;

        break;        
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_DBG_LCTM_DRFr,RTL9603CVD_UPD_LCL_TYPEf,&data), ret);
        ret = CPARSER_OK;

        break;        
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            data=2;
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            ret = CPARSER_NOT_OK;
            break;
            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;
    }
    
    diag_util_printf("\nstop local time state:%s\n",(data==2)?"enabled":"disabled");  
    return ret;    
}    /* end of cparser_cmd_epon_get_stop_sync_olt_localtime_state */


/*
 * epon set report-val-adj <UINT:length>  
 */
cparser_result_t
cparser_cmd_epon_set_report_val_adj_length(
    cparser_context_t *context,
    uint32_t  *length_ptr)
{
    uint32 data;
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();

    data=*length_ptr;    

    switch(DIAG_UTIL_CHIP_TYPE)
    {
   
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_REPORT_CNT_ADJf,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_REPORT_MODEr,RTL9602C_REPORT_CNT_ADJf,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_REPORT_MODEr,RTL9607C_REPORT_CNT_ADJf,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_REPORT_MODEr,RTL9603CVD_REPORT_CNT_ADJf,&data), ret);
        break;        
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_report_val_adj_length */

/*
 * epon get report-val-adj
 */
cparser_result_t
cparser_cmd_epon_get_report_val_adj(
    cparser_context_t *context)
{
    uint32 data=0;
    int32 ret = RT_ERR_FAILED;
 
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_EPON_REPORT_MODEr,RTL9601B_REPORT_CNT_ADJf,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_EPON_REPORT_MODEr,RTL9602C_REPORT_CNT_ADJf,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_EPON_REPORT_MODEr,RTL9607C_REPORT_CNT_ADJf,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_EPON_REPORT_MODEr,RTL9603CVD_REPORT_CNT_ADJf,&data), ret);
        break;        
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;
    }
    
    if(CPARSER_NOT_OK==ret)
    {
        return ret;    
    }
    else
    {
        diag_util_printf("\nreport adjust value:%d\n",data);  
        return CPARSER_OK;
    }
}    /* end of cparser_cmd_epon_get_report_val_adj */

/*
 * epon set grant-shift ( add | del ) <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_grant_shift_add_del_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 data;
    int32 tempData,setValue;
    setValue = (int32)*value_ptr; 
    DIAG_UTIL_PARAM_CHK();

    if('d'==TOKEN_CHAR(3,0))
    {
        tempData = 0-setValue;    
    }
    else
    {
        tempData = setValue;
    }
    data = (uint32)tempData;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
   
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_EPON_GRANT_SHIFT_STARTr,RTL9601B_GRANT_SHIFTf,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_EPON_GRANT_SHIFT_STARTr,RTL9602C_GRANT_SHIFTf,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_EPON_GRANT_SHIFT_STARTr,RTL9607C_GRANT_SHIFTf,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_EPON_GRANT_SHIFT_STARTr,RTL9603CVD_GRANT_SHIFTf,&data), ret);
        break;        
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;
    }
    if(CPARSER_NOT_OK==ret)
        return ret;
    else    
        return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_grant_shift_add_del_value */

/*
 * epon set test-reg ( PRG_EPON_GN0 | PRG_EPON_GN1 | PRG_EPON_GN2 ) <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_test_reg_PRG_EPON_GN0_PRG_EPON_GN1_PRG_EPON_GN2_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    uint32 data;
    int32 ret = RT_ERR_FAILED;
    uint32 reg;  
    DIAG_UTIL_PARAM_CHK();
    
    
    data = *value_ptr;
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            if('0'==TOKEN_CHAR(3,11))
            {
        		reg   = RTL9602C_PRG_EPON_GN0r;
            }
        	else if ('1'==TOKEN_CHAR(3,11))
        	{
        		reg   = RTL9602C_PRG_EPON_GN1r;
        	}
        	else if ('2'==TOKEN_CHAR(3,11))
        	{
        		reg   = RTL9602C_PRG_EPON_GN2r;
            }
            else
            {
                ret = CPARSER_NOT_OK;
                break;    
            }
            DIAG_UTIL_ERR_CHK(reg_write(reg,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            if('0'==TOKEN_CHAR(3,11))
            {
        		reg   = RTL9607C_PRG_EPON_GN0r;
            }
        	else if ('1'==TOKEN_CHAR(3,11))
        	{
        		reg   = RTL9607C_PRG_EPON_GN1r;
        	}
        	else if ('2'==TOKEN_CHAR(3,11))
        	{
        		reg   = RTL9607C_PRG_EPON_GN2r;
            }
            else
            {
                ret = CPARSER_NOT_OK;
                break;    
            }

            DIAG_UTIL_ERR_CHK(reg_write(reg,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            if('0'==TOKEN_CHAR(3,11))
            {
        		reg   = RTL9603CVD_PRG_EPON_GN0r;
            }
        	else if ('1'==TOKEN_CHAR(3,11))
        	{
        		reg   = RTL9603CVD_PRG_EPON_GN1r;
        	}
        	else if ('2'==TOKEN_CHAR(3,11))
        	{
        		reg   = RTL9603CVD_PRG_EPON_GN2r;
            }
            else
            {
                ret = CPARSER_NOT_OK;
                break;    
            }

            DIAG_UTIL_ERR_CHK(reg_write(reg,&data), ret);
        break;
#endif                
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;
            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;
    }
    if(CPARSER_NOT_OK==ret)
        return ret;
    else        
        return CPARSER_OK;    
}    /* end of cparser_cmd_epon_set_test_reg_prg_epon_gn0_prg_epon_gn1_prg_epon_gn2_value */

/*
 * epon get test-reg ( PRG_EPON_GN0 | PRG_EPON_GN1 | PRG_EPON_GN2 ) 
 */
cparser_result_t
cparser_cmd_epon_get_test_reg_PRG_EPON_GN0_PRG_EPON_GN1_PRG_EPON_GN2(
    cparser_context_t *context)
{
    uint32 data=0;
    int32 ret = RT_ERR_FAILED;
    uint32 reg;  

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            if('0'==TOKEN_CHAR(3,11))
            {
        		reg   = RTL9602C_PRG_EPON_GN0r;
            }
        	else if ('1'==TOKEN_CHAR(3,11))
        	{
        		reg   = RTL9602C_PRG_EPON_GN1r;
        	}
        	else if ('2'==TOKEN_CHAR(3,11))
        	{
        		reg   = RTL9602C_PRG_EPON_GN2r;
            }
            else
            {
                ret = CPARSER_NOT_OK;
                break;   
            }
            DIAG_UTIL_ERR_CHK(reg_read(reg,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            if('0'==TOKEN_CHAR(3,11))
            {
        		reg   = RTL9607C_PRG_EPON_GN0r;
            }
        	else if ('1'==TOKEN_CHAR(3,11))
        	{
        		reg   = RTL9607C_PRG_EPON_GN1r;
        	}
        	else if ('2'==TOKEN_CHAR(3,11))
        	{
        		reg   = RTL9607C_PRG_EPON_GN2r;
            }
            else
            {
                ret = CPARSER_NOT_OK;
                break;   
            }

            DIAG_UTIL_ERR_CHK(reg_read(reg,&data), ret);
        break;        
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            if('0'==TOKEN_CHAR(3,11))
            {
        		reg   = RTL9603CVD_PRG_EPON_GN0r;
            }
        	else if ('1'==TOKEN_CHAR(3,11))
        	{
        		reg   = RTL9603CVD_PRG_EPON_GN1r;
        	}
        	else if ('2'==TOKEN_CHAR(3,11))
        	{
        		reg   = RTL9603CVD_PRG_EPON_GN2r;
            }
            else
            {
                ret = CPARSER_NOT_OK;
                break;   
            }

            DIAG_UTIL_ERR_CHK(reg_read(reg,&data), ret);
        break;        
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            ret = CPARSER_NOT_OK;
            break;
    }
  
    if(CPARSER_NOT_OK==ret)
    {
        return ret;   
    }
    else
    {
        diag_util_mprintf("data: 0x%x\n",data);
        return CPARSER_OK;
    }
}    /* end of cparser_cmd_epon_get_test_reg_prg_epon_gn0_prg_epon_gn1_prg_epon_gn2 */

/*
 * epon set force-prbs ( off | prbs31 | prbs23 | prbs15 | prbs07 | prbs03 )
 */
cparser_result_t
cparser_cmd_epon_set_force_prbs_off_prbs31_prbs23_prbs15_prbs11_prbs09_prbs07_prbs03(
    cparser_context_t *context)
{
    rtk_epon_prbs_t		prbsCfg;
    int32				ret = RT_ERR_FAILED;

	DIAG_UTIL_PARAM_CHK();

	if (0 == strcmp("off",TOKEN_STR(3)))
		prbsCfg = RTK_EPON_PRBS_OFF;
	else if (0 == strcmp("prbs31",TOKEN_STR(3)))
		prbsCfg = RTK_EPON_PRBS_31;
	else if (0 == strcmp("prbs23",TOKEN_STR(3)))
		prbsCfg = RTK_EPON_PRBS_23;
	else if (0 == strcmp("prbs15",TOKEN_STR(3)))
		prbsCfg = RTK_EPON_PRBS_15;
	else if (0 == strcmp("prbs11", TOKEN_STR(3)))
		prbsCfg = RTK_EPON_PRBS_11;
	else if (0 == strcmp("prbs09", TOKEN_STR(3)))
		prbsCfg = RTK_EPON_PRBS_9;
	else if (0 == strcmp("prbs07",TOKEN_STR(3)))
		prbsCfg = RTK_EPON_PRBS_7;
	else if (0 == strcmp("prbs03",TOKEN_STR(3)))
		prbsCfg = RTK_EPON_PRBS_3;
	else
	    return CPARSER_ERR_INVALID_PARAMS;	


	DIAG_UTIL_ERR_CHK(rtk_epon_forcePRBS_set(prbsCfg), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_force_prbs_off_prbs31_prbs23_prbs15_prbs11_prbs09_prbs07_prbs03 */

/*
 * epon get force-prbs
 */
cparser_result_t
cparser_cmd_epon_get_force_prbs (
	cparser_context_t *context
)
{
    int32   ret = RT_ERR_FAILED;
    rtk_epon_prbs_t		prbsCfg;
	char* prbsStr [] = {
		"OFF",
		"prbs31",
		"prbs23",
		"prbs15",
		"prbs11",
		"prbs9",
		"prbs7",
		"prbs3"
	};

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	memset (&prbsCfg, 0x00, sizeof (rtk_epon_prbs_t));

	DIAG_UTIL_ERR_CHK(rtk_epon_forcePRBS_get(&prbsCfg), ret);

    diag_util_printf("PRBS MODE:%s\n", prbsStr[prbsCfg]);

    return CPARSER_OK;
}

/*
 * epon get multi-llid-mode 
 */
cparser_result_t
cparser_cmd_epon_get_multi_llid_mode(
    cparser_context_t *context)
{
    rtk_epon_multiLlidMode_t mode;
    int32   ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

     DIAG_UTIL_ERR_CHK(rtk_epon_multiLlidMode_get(&mode), ret);
     switch(mode)
     {
         case RTK_EPON_MULIT_1_LLID_MODE:
             diag_util_printf("1 LLID MODE\n"); 
             break;
         case RTK_EPON_MULIT_4_LLID_PER_LLID_8_QUEUE:
             diag_util_printf("4 LLID per LLID 8 Queue MODE\n"); 
             break;
         case RTK_EPON_MULIT_8_LLID_PER_LLID_4_QUEUE:
             diag_util_printf("8 LLID per LLID 4 Queue MODE\n"); 
             break;
                         
         default:
             diag_util_printf("unknown mode\n"); 
             break;    
     }            

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_multi_llid_mode */

/*
 * epon set multi-llid-mode ( 8-LLID-4-queue | 4-LLID-8-queue )
 */
cparser_result_t
cparser_cmd_epon_set_multi_llid_mode_8_LLID_4_queue_4_LLID_8_queue(
    cparser_context_t *context)
{
    rtk_epon_multiLlidMode_t mode;
    int32   ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();

	if (0 == strcmp("8-LLID-4-queue",TOKEN_STR(3)))
		mode = RTK_EPON_MULIT_8_LLID_PER_LLID_4_QUEUE;
	else if (0 == strcmp("4-LLID-8-queue",TOKEN_STR(3)))
		mode = RTK_EPON_MULIT_4_LLID_PER_LLID_8_QUEUE;
	else
	    return CPARSER_ERR_INVALID_PARAMS;	

    DIAG_UTIL_ERR_CHK(rtk_epon_multiLlidMode_set(mode), ret);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_multi_llid_mode_8_llid_4_queue_4_llid_8_queue */


/*
 * epon set auto-powersaving state ( enable | disable ) 
 */
cparser_result_t
cparser_cmd_epon_set_auto_powersaving_state_enable_disable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_epon_autoPowerSavingState_set(enable), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_auto_powersaving_state_enabled_disable */

/*
 * epon get auto-powersaving state 
 */
cparser_result_t
cparser_cmd_epon_get_auto_powersaving_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32   ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_epon_autoPowerSavingState_get(&enable), ret); 

    diag_util_printf("\nauto power saving mode:%s\n",diagStr_enable[enable]);  

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_auto_powersaving_state */

/*
 * epon set auto-powersaving-oe-recover-time <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_auto_powersaving_oe_recover_time_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    uint32 data;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    
    data = *value_ptr;
    DIAG_UTIL_ERR_CHK(rtk_epon_autoPowerSavingOeRecoverTime_set((int)data), ret); 


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_auto_powersaving_oe_recover_time_value */

/*
 * epon get auto-powersaving-oe-recover-time
 */
cparser_result_t
cparser_cmd_epon_get_auto_powersaving_oe_recover_time(
    cparser_context_t *context)
{
    uint32 data;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_epon_autoPowerSavingOeRecoverTime_get((int *)&data), ret); 

    diag_util_printf("\nOE recover time:%d\n",data);  

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_auto_powersaving_oe_recover_time */

/*
 * epon set report-queue-set <UINT:value>
 */
cparser_result_t
cparser_cmd_epon_set_report_queue_set_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    uint32 data;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    
    data = *value_ptr;
    DIAG_UTIL_ERR_CHK(rtk_epon_reportQueueSet_set((uint8)data), ret); 

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_report_queue_set_value */

/*
 * epon get report-queue-set
 */
cparser_result_t
cparser_cmd_epon_get_report_queue_set(
    cparser_context_t *context)
{
    uint32 data;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_epon_reportQueueSet_get((uint8 *)&data), ret); 

    diag_util_printf("\nreport queueset number:%d\n",data);  

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_report_queue_set */

/*
 * epon set threshold-report queue <UINT:index> th1 <UINT:th1> th2 <UINT:th2>  th3 <UINT:th3>
 */
cparser_result_t
cparser_cmd_epon_set_threshold_report_queue_index_th1_th1_th2_th2_th3_th3(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *th1_ptr,
    uint32_t  *th2_ptr,
    uint32_t  *th3_ptr)
{
    uint32 queue;
    int32 ret = RT_ERR_FAILED;
    rtk_epon_report_threshold_t  thrReport;

    DIAG_UTIL_PARAM_CHK();
    
    
    memset(&thrReport,0x0,sizeof(rtk_epon_report_threshold_t));    
    
    queue = *index_ptr;
    thrReport.th1=*th1_ptr;
    thrReport.th2=*th2_ptr;
    thrReport.th3=*th3_ptr;

    DIAG_UTIL_ERR_CHK(rtk_epon_queueThresholdReport_set(queue,&thrReport), ret); 


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_threshold_report_queue_index_th1_th1_th2_th2_th3_th3 */

/*
 * epon get threshold-report queue <UINT:index>
 */
cparser_result_t
cparser_cmd_epon_get_threshold_report_queue_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32 queue;
    int32 ret = RT_ERR_FAILED;
    rtk_epon_report_threshold_t  thrReport;

    memset(&thrReport,0x0,sizeof(rtk_epon_report_threshold_t));
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    queue = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_epon_queueThresholdReport_get(queue,&thrReport), ret); 

    diag_util_mprintf("report threshold:\n");
    diag_util_mprintf("threshold level1:%d\n",thrReport.th1);
    diag_util_mprintf("threshold level2:%d\n",thrReport.th2);
    diag_util_mprintf("threshold level3:%d\n",thrReport.th3);

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_threshold_report_queue_index */


