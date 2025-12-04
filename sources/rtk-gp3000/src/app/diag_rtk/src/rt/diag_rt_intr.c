#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/rt/rt_intr.h>

static
void string_2_intr_type(char *pStr, rt_intr_type_t *pType)
{
    if(osal_strcmp(pStr,"all") == 0)
        *pType = RT_INTR_TYPE_ALL;
    else if(osal_strcmp(pStr,"link-change") == 0)
        *pType = RT_INTR_TYPE_LINK_CHANGE;
    else if(osal_strcmp(pStr,"meter-exceed") == 0)
        *pType = RT_INTR_TYPE_METER_EXCEED;
    else if(osal_strcmp(pStr,"learn-over") == 0)
        *pType = RT_INTR_TYPE_LEARN_OVER;
    else if(osal_strcmp(pStr,"speed-change") == 0)
        *pType = RT_INTR_TYPE_SPEED_CHANGE;
    else if(osal_strcmp(pStr,"special-congestion") == 0)
        *pType = RT_INTR_TYPE_SPECIAL_CONGEST;
    else if(osal_strcmp(pStr,"loop-detection") == 0)
        *pType = RT_INTR_TYPE_LOOP_DETECTION;
    else if(osal_strcmp(pStr,"cable-diag") == 0)
        *pType = RT_INTR_TYPE_CABLE_DIAG_FIN;
    else if(osal_strcmp(pStr,"acl") == 0)
        *pType = RT_INTR_TYPE_ACL_ACTION;
    else if(osal_strcmp(pStr,"gphy") == 0)
        *pType = RT_INTR_TYPE_GPHY;
    else if(osal_strcmp(pStr,"serdes") == 0)
        *pType = RT_INTR_TYPE_SERDES;
    else if(osal_strcmp(pStr,"gpon") == 0)
        *pType = RT_INTR_TYPE_GPON;
    else if(osal_strcmp(pStr,"epon") == 0)
        *pType = RT_INTR_TYPE_EPON;
    else if(osal_strcmp(pStr,"ptp") == 0)
        *pType = RT_INTR_TYPE_PTP;
    else if(osal_strcmp(pStr,"dying-gasp") == 0)
        *pType = RT_INTR_TYPE_DYING_GASP;
    else if(osal_strcmp(pStr,"thermal") == 0)
        *pType = RT_INTR_TYPE_THERMAL;
    else if(osal_strcmp(pStr,"adc") == 0)
        *pType = RT_INTR_TYPE_ADC;
    else if(osal_strcmp(pStr,"eeprom110to118") == 0)
        *pType = RT_INTR_TYPE_EEPROM_UPDATE_110OR118;
    else if(osal_strcmp(pStr,"eeprom128to247") == 0)
        *pType = RT_INTR_TYPE_EEPROM_UPDATE_128TO247;
    else if(osal_strcmp(pStr,"pktbuffer-error") == 0)
        *pType = RT_INTR_TYPE_PKTBUFFER_ERROR;
    else if(osal_strcmp(pStr,"debug") == 0)
        *pType = RT_INTR_TYPE_DEBUG;
    else if(osal_strcmp(pStr,"smart-card") == 0)
        *pType = RT_INTR_TYPE_SMARTCARD;
    else if(osal_strcmp(pStr,"switch-error") == 0)
        *pType = RT_INTR_TYPE_SWITCH_ERROR;
    else if(osal_strcmp(pStr,"sfp") == 0)
        *pType = RT_INTR_TYPE_SFP;
    else if(osal_strcmp(pStr,"fb-event") == 0)
        *pType = RT_INTR_TYPE_FB_EVENT;
    else if(osal_strcmp(pStr,"tod") == 0)
        *pType = RT_INTR_TYPE_TOD;
    else if(osal_strcmp(pStr,"tod-update") == 0)
        *pType = RT_INTR_TYPE_TOD_UPDATE;
    else if(osal_strcmp(pStr,"los") == 0)
        *pType = RT_INTR_TYPE_LOS;
    else if(osal_strcmp(pStr,"lof") == 0)
        *pType = RT_INTR_TYPE_LOF;
    else
        *pType = RT_INTR_TYPE_END;
}

/*
 * rt_interrupt init
 */
cparser_result_t
cparser_cmd_rt_interrupt_init(
    cparser_context_t *context)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rt_intr_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_interrupt_init */

/*
 * rt_interrupt set mask ( all | link-change | meter-exceed | learn-over | speed-change | special-congestion | loop-detection | cable-diag | acl | gphy | serdes | gpon | epon | ptp | dying-gasp | thermal | adc | eeprom110to118 | eeprom128to247 | pktbuffer-error | debug | smart-card | switch-error | sfp | fb-event | tod | tod-update | los | lof ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_interrupt_set_mask_all_link_change_meter_exceed_learn_over_speed_change_special_congestion_loop_detection_cable_diag_acl_gphy_serdes_gpon_epon_ptp_dying_gasp_thermal_adc_eeprom110to118_eeprom128to247_pktbuffer_error_debug_smart_card_switch_error_sfp_fb_event_tod_tod_update_los_lof_state_disable_enable(
    cparser_context_t *context)
{
    rt_intr_type_t type;
    rt_enable_t enable = DISABLED;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(5,0))
    {
        enable = DISABLED;
    }
    else
    {
        enable = ENABLED;
    }

    string_2_intr_type(TOKEN_STR(3), &type);

    DIAG_UTIL_ERR_CHK(rt_intr_imr_set(type, enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_interrupt_set_mask_all_link_change_meter_exceed_learn_over_speed_change_special_congestion_loop_detection_cable_diag_acl_gphy_serdes_gpon_epon_ptp_dying_gasp_thermal_adc_eeprom110to118_eeprom128to247_pktbuffer_error_debug_smart_card_switch_error_sfp_fb_event_tod_tod_update_los_lof_state_disable_enable */

/*
 * rt_interrupt get mask ( all | link-change | meter-exceed | learn-over | speed-change | special-congestion | loop-detection | cable-diag | acl | gphy | serdes | gpon | epon | ptp | dying-gasp | thermal | adc | eeprom110to118 | eeprom128to247 | pktbuffer-error | debug | smart-card | switch-error | sfp | fb-event | tod | tod-update | los | lof ) state
 */
cparser_result_t
cparser_cmd_rt_interrupt_get_mask_all_link_change_meter_exceed_learn_over_speed_change_special_congestion_loop_detection_cable_diag_acl_gphy_serdes_gpon_epon_ptp_dying_gasp_thermal_adc_eeprom110to118_eeprom128to247_pktbuffer_error_debug_smart_card_switch_error_sfp_fb_event_tod_tod_update_los_lof_state(
    cparser_context_t *context)
{
    rt_intr_type_t type;
    uint32 mask;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    string_2_intr_type(TOKEN_STR(3), &type);

    DIAG_UTIL_ERR_CHK(rt_intr_imr_get(type, &mask), ret);

    if(RT_INTR_TYPE_ALL == type)
    {
        diag_util_printf("IMR: 0x%08x\n\r", mask);
    }
    else
    {
        diag_util_printf("IMR: %s\n\r", diagStr_enable[mask]);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_interrupt_get_mask_all_link_change_meter_exceed_learn_over_speed_change_special_congestion_loop_detection_cable_diag_acl_gphy_serdes_gpon_epon_ptp_dying_gasp_thermal_adc_eeprom110to118_eeprom128to247_pktbuffer_error_debug_smart_card_switch_error_sfp_fb_event_tod_tod_update_los_lof_state */

