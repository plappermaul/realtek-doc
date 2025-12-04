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
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#include <rtk/led.h>


/*
 * led init
 */
cparser_result_t
cparser_cmd_led_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    
    /*init led module*/
    DIAG_UTIL_ERR_CHK(rtk_led_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_led_init */

/*
 * led set mode ( serial | parallel )
 */
cparser_result_t
cparser_cmd_led_set_mode_serial_parallel(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_operation_t mode=LED_OP_SERIAL;
    DIAG_UTIL_PARAM_CHK();

    if ('s' == TOKEN_CHAR(3,0))
    {
        mode = LED_OP_SERIAL;
    }
    else if ('p' == TOKEN_CHAR(3,0))
    {
        mode = LED_OP_PARALLEL;
    }
    DIAG_UTIL_ERR_CHK(rtk_led_operation_set(mode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_led_set_mode_serial_parallel */

/*
 * led get mode
 */
cparser_result_t
cparser_cmd_led_get_mode(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_operation_t mode;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_led_operation_get(&mode), ret);
    
    if(LED_OP_PARALLEL == mode)
        diag_util_mprintf("Parallel Mode\n");
    else
        diag_util_mprintf("Serial Mode\n");
        
    return CPARSER_OK;
}    /* end of cparser_cmd_led_get_mode */

/*
 * led set group <UINT:index> type utp-port <UINT:port>
 */
cparser_result_t
cparser_cmd_led_set_group_index_type_utp_port_port(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *port_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_type_t type;
    rtk_led_config_t ledConfig;

    DIAG_UTIL_PARAM_CHK();

    memset(&ledConfig,0x0,sizeof(rtk_led_config_t));    
    switch(*port_ptr)
    {
        case 0:
            type = LED_TYPE_UTP0;
            break;
        case 1:
            type = LED_TYPE_UTP1;
            break;
        case 2:
            type = LED_TYPE_UTP2;
            break;
        case 3:
            type = LED_TYPE_UTP3;
            break;
        case 4:
            type = LED_TYPE_UTP4;
            break;
        case 5:
            type = LED_TYPE_UTP5;
            break;
        default:
            return CPARSER_NOT_OK;
            break;            
    }
    
    DIAG_UTIL_ERR_CHK(rtk_led_config_set(*index_ptr,type,&ledConfig), ret);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_led_set_group_index_type_utp_port_port */

/*
 * led set group <UINT:index> type ( fiber | pon | usb0 | usb1 | sata | fiber-0 | fiber-1 | none )
 */
cparser_result_t
cparser_cmd_led_set_group_index_type_fiber_pon_usb0_usb1_sata_fiber_0_fiber_1_none(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_type_t type;
    rtk_led_config_t ledConfig;

    DIAG_UTIL_PARAM_CHK();
    memset(&ledConfig,0x0,sizeof(rtk_led_config_t));    

    if(strcmp(TOKEN_STR(5),"fiber")==0)
    {
        type = LED_TYPE_FIBER;     
    }
    else if(strcmp(TOKEN_STR(5),"pon")==0)
    {
        type = LED_TYPE_PON;     
    }
    else if(strcmp(TOKEN_STR(5),"usb0")==0)
    {
        type = LED_TYPE_USB0;     
    }    
    else if(strcmp(TOKEN_STR(5),"usb1")==0)
    {
        type = LED_TYPE_USB1;     
    }
    else if(strcmp(TOKEN_STR(5),"fiber-0")==0)
    {
        type = LED_TYPE_FIBER0;     
    }
    else if(strcmp(TOKEN_STR(5),"fiber-1")==0)
    {
        type = LED_TYPE_FIBER1;     
    }
    else if(strcmp(TOKEN_STR(5),"none")==0)
    {
        type = LED_TYPE_NONE;     
    }
    else
    {
        type = LED_TYPE_SATA;     
    }     
    DIAG_UTIL_ERR_CHK(rtk_led_config_set(*index_ptr,type,&ledConfig), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_led_set_group_index_type_fiber_pon_usb0_usb1_sata */

/*
 * led set group <UINT:index> source ( collision | duplex | tx-act | rx-act | force | pon-link | soc-link-act | pon-alarm | pon-warning ) state ( enable | disable ) 
 */
cparser_result_t
cparser_cmd_led_set_group_index_source_collision_duplex_tx_act_rx_act_force_pon_link_soc_link_act_pon_alarm_pon_warning_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_type_t type;
    rtk_led_config_t ledConfig;
    rtk_enable_t enable;
        
    DIAG_UTIL_PARAM_CHK();

    memset(&ledConfig,0x0,sizeof(rtk_led_config_t));    

    DIAG_UTIL_ERR_CHK(rtk_led_config_get(*index_ptr,&type,&ledConfig), ret);


    if('e'==TOKEN_CHAR(7,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    if(strcmp(TOKEN_STR(5),"collision")==0)
    {
        ledConfig.ledEnable[LED_CONFIG_COL] = enable;
    }
    else if(strcmp(TOKEN_STR(5),"duplex")==0)
    {
        ledConfig.ledEnable[LED_CONFIG_DUP] = enable;
    }
    else if(strcmp(TOKEN_STR(5),"tx-act")==0)
    {
        ledConfig.ledEnable[LED_CONFIG_TX_ACT] = enable;
    }
    else if(strcmp(TOKEN_STR(5),"rx-act")==0)
    {
        ledConfig.ledEnable[LED_CONFIG_RX_ACT] = enable;
    }    
    else if(strcmp(TOKEN_STR(5),"force")==0)
    {
        ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = enable;
    }
    else if(strcmp(TOKEN_STR(5),"pon-link")==0)
    {
        ledConfig.ledEnable[LED_CONFIG_PON_LINK] = enable;
    }
    else if(strcmp(TOKEN_STR(5),"pon-alarm")==0)
    {
        ledConfig.ledEnable[LED_CONFIG_PON_ALARM] = enable;
    }
    else if(strcmp(TOKEN_STR(5),"pon-warning")==0)
    {
        ledConfig.ledEnable[LED_CONFIG_PON_WARNING] = enable;
    }
    else
    {
        ledConfig.ledEnable[LED_CONFIG_SOC_LINK_ACK] = enable;
    }     

    DIAG_UTIL_ERR_CHK(rtk_led_config_set(*index_ptr,type,&ledConfig), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_led_set_group_index_source_collision_tx_act_rx_act_pon_link_soc_link_act_state_enable_disable */

/*
 * led set group <UINT:index> source speed ( 10M | 100M | 500M | 1000M ) mode ( link | act ) state ( enable | disable ) 
 */
cparser_result_t
cparser_cmd_led_set_group_index_source_speed_10M_100M_500M_1000M_mode_link_act_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_type_t type;
    rtk_led_config_t ledConfig;
    rtk_enable_t enable;
        
    DIAG_UTIL_PARAM_CHK();

    memset(&ledConfig,0x0,sizeof(rtk_led_config_t));    

    DIAG_UTIL_ERR_CHK(rtk_led_config_get(*index_ptr,&type,&ledConfig), ret);

    if('e'==TOKEN_CHAR(10,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    if(strcmp(TOKEN_STR(6),"10M")==0)
    {
        if('l'!=TOKEN_CHAR(8,0))
            ledConfig.ledEnable[LED_CONFIG_SPD10ACT] = enable;
        else
            ledConfig.ledEnable[LED_CONFIG_SPD10] = enable;        
    }    
    else if(strcmp(TOKEN_STR(6),"100M")==0)
    {
        if('l'!=TOKEN_CHAR(8,0))
            ledConfig.ledEnable[LED_CONFIG_SPD100ACT] = enable;
        else
            ledConfig.ledEnable[LED_CONFIG_SPD100] = enable;        
    }
    else if(strcmp(TOKEN_STR(6),"500M")==0)
    {
        if('l'!=TOKEN_CHAR(8,0))
            ledConfig.ledEnable[LED_CONFIG_SPD500ACT] = enable;
        else
            ledConfig.ledEnable[LED_CONFIG_SPD500] = enable;        
    }
    else
    {
        if('l'!=TOKEN_CHAR(8,0))
            ledConfig.ledEnable[LED_CONFIG_SPD1000ACT] = enable;
        else
            ledConfig.ledEnable[LED_CONFIG_SPD1000] = enable;        
    }

    DIAG_UTIL_ERR_CHK(rtk_led_config_set(*index_ptr,type,&ledConfig), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_led_set_group_index_source_speed_10m_100m_500m_1000m_mode_link_act_state_enable_disable */

/*
 * led get group <UINT:index> config
 */
cparser_result_t
cparser_cmd_led_get_group_index_config(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_type_t type;
    rtk_led_config_t ledConfig;
    rtk_enable_t enable;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    memset(&ledConfig,0x0,sizeof(rtk_led_config_t));    

    DIAG_UTIL_ERR_CHK(rtk_led_config_get(*index_ptr,&type,&ledConfig), ret);

    diag_util_mprintf("LED index %d setting\n",*index_ptr);

    diag_util_mprintf("Type:\n");
    switch(type)
    {
        case LED_TYPE_UTP0:
            diag_util_mprintf("LED_TYPE_UTP0\n");
            break;
        case LED_TYPE_UTP1:
            diag_util_mprintf("LED_TYPE_UTP1\n");
            break;
        case LED_TYPE_UTP2:
            diag_util_mprintf("LED_TYPE_UTP2\n");
            break;
        case LED_TYPE_UTP3:
            diag_util_mprintf("LED_TYPE_UTP3\n");
            break;
        case LED_TYPE_UTP4:
            diag_util_mprintf("LED_TYPE_UTP4\n");
            break;
        case LED_TYPE_UTP5:
            diag_util_mprintf("LED_TYPE_UTP5\n");
            break;
        case LED_TYPE_FIBER:
            diag_util_mprintf("LED_TYPE_FIBER\n");
            break;
        case LED_TYPE_PON:
            diag_util_mprintf("LED_TYPE_PON\n");
            break;
        case LED_TYPE_USB0:
            diag_util_mprintf("LED_TYPE_USB0\n");
            break;
        case LED_TYPE_USB1:
            diag_util_mprintf("LED_TYPE_USB1\n");
            break;        
        case LED_TYPE_SATA:
            diag_util_mprintf("LED_TYPE_SATA\n");
            break;
        case LED_TYPE_NONE:
            diag_util_mprintf("LED_TYPE_NONE\n");
            break;

        default:
            diag_util_mprintf("Known led type!\n");
            break;                
    }

    diag_util_mprintf("LED setting:\n");

    diag_util_mprintf("LED_CONFIG_COL         :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_COL]]);    
    diag_util_mprintf("LED_CONFIG_TX_ACT      :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_TX_ACT]]);    
    diag_util_mprintf("LED_CONFIG_RX_ACT      :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_RX_ACT]]);    
    diag_util_mprintf("LED_CONFIG_SPD10ACT    :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_SPD10ACT]]);    
    diag_util_mprintf("LED_CONFIG_SPD100ACT   :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_SPD100ACT]]);    
    diag_util_mprintf("LED_CONFIG_SPD500ACT   :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_SPD500ACT]]);    
    diag_util_mprintf("LED_CONFIG_SPD1000ACT  :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_SPD1000ACT]]);    
    diag_util_mprintf("LED_CONFIG_SPD10ACT    :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_SPD10ACT]]);    
    diag_util_mprintf("LED_CONFIG_DUP         :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_DUP]]);    
    diag_util_mprintf("LED_CONFIG_SPD10       :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_SPD10]]);    
    diag_util_mprintf("LED_CONFIG_SPD100      :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_SPD100]]);    
    diag_util_mprintf("LED_CONFIG_SPD500      :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_SPD500]]);    
    diag_util_mprintf("LED_CONFIG_SPD1000     :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_SPD1000]]);    
    diag_util_mprintf("LED_CONFIG_FORCE_MODE  :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_FORCE_MODE]]);    
    diag_util_mprintf("LED_CONFIG_PON_LINK    :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_PON_LINK]]);    
    diag_util_mprintf("LED_CONFIG_PON_WARNING :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_PON_WARNING]]);    
    diag_util_mprintf("LED_CONFIG_PON_ALARM   :%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_PON_ALARM]]);    
    diag_util_mprintf("LED_CONFIG_SOC_LINK_ACK:%s\n",diagStr_enable[ledConfig.ledEnable[LED_CONFIG_SOC_LINK_ACK]]);    

    return CPARSER_OK;
}    /* end of cparser_cmd_led_get_group_index_config */

/*
 * led set group <UINT:index> force ( on | off | blinking | blinking-g2 ) 
 */
cparser_result_t
cparser_cmd_led_set_group_index_force_on_off_blinking_blinking_g2(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_force_mode_t mode;
     
    DIAG_UTIL_PARAM_CHK();

    if(strcmp(TOKEN_STR(5),"on")==0)
        mode = LED_FORCE_ON;
    else if(strcmp(TOKEN_STR(5),"off")==0)
        mode = LED_FORCE_OFF;
    else if(strcmp(TOKEN_STR(5),"blinking-g2")==0)
        mode = LED_FORCE_BLINK_GROUP2;
    else
        mode = LED_FORCE_BLINK;
    
    DIAG_UTIL_ERR_CHK(rtk_led_modeForce_set(*index_ptr,mode), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_led_set_group_index_force_on_off_blinking_blinking_g2 */

/*
 * led get group <UINT:index> force
 */
cparser_result_t
cparser_cmd_led_get_group_index_force(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_force_mode_t mode;
  
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_led_modeForce_get(*index_ptr,&mode), ret);
    
    switch(mode)
    {
        case LED_FORCE_ON:
            diag_util_mprintf("force on\n");
            break;
        case LED_FORCE_OFF:
            diag_util_mprintf("force off\n");
            break;
        case LED_FORCE_BLINK:
            diag_util_mprintf("blinking\n");
            break;
        case LED_FORCE_BLINK_GROUP2:
            diag_util_mprintf("blinking group2\n");
            break;

        default:
            diag_util_mprintf("known force mode\n");
            break;
            
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_led_get_group_index_force */

/*
 * led set group <UINT:index> parallel-state ( enable | disable )
 */
cparser_result_t
cparser_cmd_led_set_group_index_parallel_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;
   
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(5,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_led_parallelEnable_set(*index_ptr,enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_led_set_group_index_parallel_state_enable_disable */

/*
 * led get group <UINT:index> parallel-state 
 */
cparser_result_t
cparser_cmd_led_get_group_index_parallel_state(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;
  
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_led_parallelEnable_get(*index_ptr,&enable), ret);

    diag_util_mprintf("parallel state:%s\n",diagStr_enable[enable]);    

    return CPARSER_OK;
}    /* end of cparser_cmd_led_get_group_index_parallel_state */

/*
 * led set group <UINT:index> parallel ( active-high | active-low )
 */
cparser_result_t
cparser_cmd_led_set_group_index_parallel_active_high_active_low(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_active_t  active;
   
    DIAG_UTIL_PARAM_CHK();

    if(strcmp(TOKEN_STR(5),"active-high")==0)
        active = LED_ACTIVE_HIGH;
    else
        active = LED_ACTIVE_LOW;

    DIAG_UTIL_ERR_CHK(rtk_led_parallelMode_set(*index_ptr, active), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_led_set_group_index_parallel_active_high_active_low */

/*
 * led get group <UINT:index> parallel
 */
cparser_result_t
cparser_cmd_led_get_group_index_parallel(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_active_t  active;
  
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_led_parallelMode_get(*index_ptr, &active), ret);

    if(LED_ACTIVE_HIGH == active)
        diag_util_mprintf("active-high\n");
    else
        diag_util_mprintf("active-low\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_led_get_group_index_parallel */

/*
 * led set blinking ( port | force | usb | sata | force-g2 ) rate ( blink-32 | blink-48 | blink-64 | blink-96 | blink-128 | blink-256 | blink-512 | blink-1024 ) 
 */
cparser_result_t
cparser_cmd_led_set_blinking_port_force_usb_sata_force_g2_rate_blink_32_blink_48_blink_64_blink_96_blink_128_blink_256_blink_512_blink_1024(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_blinkGroup_t   type;
    rtk_led_blink_rate_t   rate;
    
    DIAG_UTIL_PARAM_CHK();

    if('p'==TOKEN_CHAR(3,0))
        type = LED_BLINK_GROUP_PORT;
    else if('u'==TOKEN_CHAR(3,0))
        type = LED_BLINK_GROUP_USB;
    else if('s'==TOKEN_CHAR(3,0))
        type = LED_BLINK_GROUP_SATA;
    else if(strcmp(TOKEN_STR(3), "force-g2")==0)
        type = LED_BLINK_GROUP_FORCE_MODE2;
    else 
        type = LED_BLINK_GROUP_FORCE_MODE;


    if(strcmp(TOKEN_STR(5),"blink-32")==0)
        rate = LED_BLINKRATE_32MS;
    else if(strcmp(TOKEN_STR(5),"blink-48")==0)
        rate = LED_BLINKRATE_48MS;
    else if(strcmp(TOKEN_STR(5),"blink-64")==0)
        rate = LED_BLINKRATE_64MS;
    else if(strcmp(TOKEN_STR(5),"blink-128")==0)
        rate = LED_BLINKRATE_128MS;
    else if(strcmp(TOKEN_STR(5),"blink-256")==0)
        rate = LED_BLINKRATE_256MS;
    else if(strcmp(TOKEN_STR(5),"blink-512")==0)
        rate = LED_BLINKRATE_512MS;
    else
        rate = LED_BLINKRATE_1024MS;

    DIAG_UTIL_ERR_CHK(rtk_led_blinkRate_set(type,rate), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_led_set_blinking_port_force_usb_sata_rate_blink_32_blink_48_blink_64_blink_96_blink_128_blink_256_blink_512_blink_1024 */


static void _led_display_rate(rtk_led_blinkGroup_t type ,rtk_led_blink_rate_t rate)
{
   switch(type)
   {
        case LED_BLINK_GROUP_PORT:
            diag_util_mprintf("port led blinking rate:"); 
            break; 
        case LED_BLINK_GROUP_FORCE_MODE:
            diag_util_mprintf("force mode blinking rate:"); 
            break; 
        case LED_BLINK_GROUP_FORCE_MODE2:
            diag_util_mprintf("force mode group2 blinking rate:"); 
            break; 

        case LED_BLINK_GROUP_USB:
            diag_util_mprintf("usb led blinking rate:"); 
            break; 
        case LED_BLINK_GROUP_SATA:
            diag_util_mprintf("sata led blinking rate:"); 
            break; 
        default:
            return;
   } 
   switch(rate)
   {
        case LED_BLINKRATE_32MS:
            diag_util_mprintf("32MS\n"); 
            break; 
        case LED_BLINKRATE_48MS:
            diag_util_mprintf("48MS\n"); 
            break; 
        case LED_BLINKRATE_64MS:
            diag_util_mprintf("64MS\n"); 
            break; 
        case LED_BLINKRATE_128MS:
            diag_util_mprintf("128MS\n"); 
            break; 
        case LED_BLINKRATE_256MS:
            diag_util_mprintf("256MS\n"); 
            break; 
        case LED_BLINKRATE_512MS:
            diag_util_mprintf("512MS\n"); 
            break; 
        case LED_BLINKRATE_1024MS:
            diag_util_mprintf("1024MS\n"); 
            break; 
        default:
            return;
   } 

    return;
}
/*
 * led get blinking
 */
cparser_result_t
cparser_cmd_led_get_blinking(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_blinkGroup_t   type;
    rtk_led_blink_rate_t   rate;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    type = LED_BLINK_GROUP_PORT;
    if(rtk_led_blinkRate_get(type,&rate)==RT_ERR_OK)
    	_led_display_rate(type,rate);
    
    type = LED_BLINK_GROUP_FORCE_MODE;
    if(rtk_led_blinkRate_get(type,&rate)==RT_ERR_OK)
    	_led_display_rate(type,rate);

    type = LED_BLINK_GROUP_USB;
    if(rtk_led_blinkRate_get(type,&rate)==RT_ERR_OK);
    	_led_display_rate(type,rate);

    type = LED_BLINK_GROUP_SATA;
    if(rtk_led_blinkRate_get(type,&rate)==RT_ERR_OK);
    	_led_display_rate(type,rate);

    type = LED_BLINK_GROUP_FORCE_MODE2;
    if(rtk_led_blinkRate_get(type,&rate)==RT_ERR_OK);
    	_led_display_rate(type,rate);


    return CPARSER_OK;
}    /* end of cparser_cmd_led_get_blinking */

/*
 * led set serial ( active-high | active-low )
 */
cparser_result_t
cparser_cmd_led_set_serial_active_high_active_low(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_active_t  active;

    DIAG_UTIL_PARAM_CHK();

    if(strcmp(TOKEN_STR(3),"active-high")==0)
        active = LED_ACTIVE_HIGH;
    else
        active = LED_ACTIVE_LOW;

    DIAG_UTIL_ERR_CHK(rtk_led_serialMode_set(active), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_led_set_serial_active_high_active_low */

/*
 * led get serial */
cparser_result_t
cparser_cmd_led_get_serial(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_led_active_t  active;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_led_serialMode_get(&active), ret);

    if(LED_ACTIVE_HIGH == active)
        diag_util_mprintf("active-high\n");
    else
        diag_util_mprintf("active-low\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_led_get_serial */

/*
 * led set pon ( alarm | warning ) state ( enable | disable ) 
 */
cparser_result_t
cparser_cmd_led_set_pon_alarm_warning_state_enable_disable(
    cparser_context_t *context)
{
    rtk_enable_t state;
    int32     ret = RT_ERR_FAILED;
 
    DIAG_UTIL_PARAM_CHK();
    if('e'==TOKEN_CHAR(5,0))
        state = ENABLED;
    else
        state = DISABLED;

    
    if(strcmp(TOKEN_STR(3),"alarm")==0)
        DIAG_UTIL_ERR_CHK(rtk_led_ponAlarm_set(state), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_led_ponWarning_set(state), ret);

  
    return CPARSER_OK;
}    /* end of cparser_cmd_led_set_pon_alarm_warning_state_enable_disable */

/*
 * led get pon ( alarm | warning ) state
 */
cparser_result_t
cparser_cmd_led_get_pon_alarm_warning_state(
    cparser_context_t *context)
{
    rtk_enable_t state;
    int32     ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(strcmp(TOKEN_STR(3),"alarm")==0)
        DIAG_UTIL_ERR_CHK(rtk_led_ponAlarm_get(&state), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_led_ponWarning_get(&state), ret);

    diag_util_mprintf("state:%s\n",diagStr_enable[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_led_get_pon_alarm_warning_state */

