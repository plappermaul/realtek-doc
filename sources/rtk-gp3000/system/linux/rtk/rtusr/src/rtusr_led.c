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
 * Purpose : Definition those public LED APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) LED parameter settings
 */




/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_led_init
 * Description:
 *      Initialize led module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize led module before calling any led APIs.
 */
int32
rtk_led_init(void)
{
    rtdrv_ledCfg_t led_cfg;

    /* function body */
    osal_memset(&led_cfg,0x0,sizeof(rtdrv_ledCfg_t));
    SETSOCKOPT(RTDRV_LED_INIT, &led_cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_led_init */

/* Function Name:
 *      rtk_led_operation_set
 * Description:
 *      Get Led operation mode
 * Input:
 *      None
 * Output:
 *      pMode - LED operation mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can set Led operation mode.
 *      The modes that can be set are as following:
 *      - LED_OP_PARALLEL 17 led
 *      - LED_OP_SERIAL   32 led 
 */
int32 
rtk_led_operation_get(rtk_led_operation_t *pOpMode)
{
    rtdrv_ledCfg_t led_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pOpMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&led_cfg.opMode, pOpMode, sizeof(rtk_led_operation_t));
    GETSOCKOPT(RTDRV_LED_OPERATION_GET, &led_cfg, rtdrv_ledCfg_t, 1);
    osal_memcpy(pOpMode, &led_cfg.opMode, sizeof(rtk_led_operation_t));

    return RT_ERR_OK;
}   /* end of rtk_led_operation_get */


/* Function Name:
 *      rtk_led_operation_set
 * Description:
 *      Set Led operation mode
 * Input:
 *      mode - LED operation mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can set Led operation mode.
 *      The modes that can be set are as following:
 *      - LED_OP_PARALLEL 17 led
 *      - LED_OP_SERIAL   32 led 
 */
int32 
rtk_led_operation_set(rtk_led_operation_t opMode)
{
    rtdrv_ledCfg_t led_cfg;

    /* function body */
    osal_memset(&led_cfg,0x0,sizeof(rtdrv_ledCfg_t));
    osal_memcpy(&led_cfg.opMode, &opMode, sizeof(rtk_led_operation_t));
    SETSOCKOPT(RTDRV_LED_OPERATION_SET, &led_cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_led_operation_set */


/* Function Name:
 *      rtk_led_serialMode_set
 * Description:
 *      Set Led serial mode active congiuration
 * Input:
 *      None
 * Output:
 *      pActive - high low active mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can set LED serial mode active congiuration.
 */
int32 
rtk_led_serialMode_get(rtk_led_active_t *pActive)
{
    rtdrv_ledCfg_t led_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pActive), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&led_cfg.active, pActive, sizeof(rtk_led_active_t));
    GETSOCKOPT(RTDRV_LED_SERIALMODE_GET, &led_cfg, rtdrv_ledCfg_t, 1);
    osal_memcpy(pActive, &led_cfg.active, sizeof(rtk_led_active_t));

    return RT_ERR_OK;
}   /* end of rtk_led_serialMode_get */

/* Function Name:
 *      rtk_led_serialMode_set
 * Description:
 *      Set Led serial mode active congiuration
 * Input:
 *      active - high low active mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can set LED serial mode active congiuration.
 */
int32 
rtk_led_serialMode_set(rtk_led_active_t active)
{
    rtdrv_ledCfg_t led_cfg;

    /* function body */
    osal_memset(&led_cfg,0x0,sizeof(rtdrv_ledCfg_t));
    osal_memcpy(&led_cfg.active, &active, sizeof(rtk_led_active_t));
    SETSOCKOPT(RTDRV_LED_SERIALMODE_SET, &led_cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_led_serialMode_set */

/* Function Name:
 *      rtk_led_blinkRate_get
 * Description:
 *      Get LED blinking rate at mode 0 to mode 3
 * Input:
 *      group      - led blinking group
 * Output:
 *      pBlinkRate - blinking rate.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      There are 8 types of LED blinking rates at 32ms, 48ms, 64ms, 96ms, 128ms, 256ms, 512ms, and 1024ms.
 */
int32 
rtk_led_blinkRate_get(rtk_led_blinkGroup_t group, rtk_led_blink_rate_t *pBlinkRate)
{
    rtdrv_ledCfg_t led_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pBlinkRate), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&led_cfg.group, &group, sizeof(rtk_led_blinkGroup_t));
    GETSOCKOPT(RTDRV_LED_BLINKRATE_GET, &led_cfg, rtdrv_ledCfg_t, 1);
    osal_memcpy(pBlinkRate, &led_cfg.blinkRate, sizeof(rtk_led_blink_rate_t));

    return RT_ERR_OK;
}   /* end of rtk_led_blinkRate_get */

/* Function Name:
 *      rtk_led_blinkRate_set
 * Description:
 *      Get LED blinking rate
 * Input:
 *      group      - led blinking group
 *      blinkRate  - blinking rate.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      There are 8 types of LED blinking rates at 32ms, 48ms, 64ms, 96ms, 128ms, 256ms, 512ms, and 1024ms.
 */
int32 
rtk_led_blinkRate_set(rtk_led_blinkGroup_t group, rtk_led_blink_rate_t blinkRate)
{
    rtdrv_ledCfg_t led_cfg;

    /* function body */
    osal_memset(&led_cfg,0x0,sizeof(rtdrv_ledCfg_t));
    osal_memcpy(&led_cfg.group, &group, sizeof(rtk_led_blinkGroup_t));
    osal_memcpy(&led_cfg.blinkRate, &blinkRate, sizeof(rtk_led_blink_rate_t));
    SETSOCKOPT(RTDRV_LED_BLINKRATE_SET, &led_cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_led_blinkRate_set */


/* Function Name:
 *      rtk_led_Config_set
 * Description:
 *      Set per group Led to congiuration mode
 * Input:
 *      ledIdx  - LED index id.
 *      type    - LED type
 *      pConfig  - LED configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
int32 
rtk_led_config_set(uint32 ledIdx, rtk_led_type_t type, rtk_led_config_t *pConfig)
{
    rtdrv_ledCfg_t led_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&led_cfg, 0, sizeof(rtdrv_ledCfg_t));
    osal_memcpy(&led_cfg.ledIdx, &ledIdx, sizeof(uint32));
    osal_memcpy(&led_cfg.type, &type, sizeof(rtk_led_type_t));
    osal_memcpy(&led_cfg.config, pConfig, sizeof(rtk_led_config_t));
    SETSOCKOPT(RTDRV_LED_CONFIG_SET, &led_cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_led_config_set */

/* Function Name:
 *      rtk_led_groupConfig_set
 * Description:
 *      Set per group Led to congiuration mode
 * Input:
 *      ledIdx  - LED index id.
 * Output:
 *      pType    - LED type
 *      pConfig  - LED configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
int32 
rtk_led_config_get(uint32 ledIdx, rtk_led_type_t *pType, rtk_led_config_t *pConfig)
{
    rtdrv_ledCfg_t led_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&led_cfg.ledIdx, &ledIdx, sizeof(uint32));
    GETSOCKOPT(RTDRV_LED_CONFIG_GET, &led_cfg, rtdrv_ledCfg_t, 1);
    osal_memcpy(pType, &led_cfg.type, sizeof(rtk_led_type_t));
    osal_memcpy(pConfig, &led_cfg.config, sizeof(rtk_led_config_t));

    return RT_ERR_OK;
}   /* end of rtk_led_config_get */

/* Function Name:
 *      rtk_led_modeForce_get
 * Description:
 *      Get Led group to congiuration force mode
 * Input:
 *      ledIdx - LED index id.
 * Output:
 *      pMode  - LED force mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can get forced Led group mode.
 *      The force modes that can be set are as following:
 *      - LED_FORCE_BLINK,
 *      - LED_FORCE_OFF,
 *      - LED_FORCE_ON.
 *      For LED_OP_SERIAL the max led index is 31  
 *      For LED_OP_PARALLEL the max led index is 16  
 */
int32
rtk_led_modeForce_get(uint32 ledIdx, rtk_led_force_mode_t *pMode)
{
    rtdrv_ledCfg_t led_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&led_cfg.ledIdx, &ledIdx, sizeof(uint32));
    GETSOCKOPT(RTDRV_LED_MODEFORCE_GET, &led_cfg, rtdrv_ledCfg_t, 1);
    osal_memcpy(pMode, &led_cfg.mode, sizeof(rtk_led_force_mode_t));

    return RT_ERR_OK;
}   /* end of rtk_led_modeForce_get */

/* Function Name:
 *      rtk_led_modeForce_set
 * Description:
 *      Set Led group to congiuration force mode
 * Input:
 *      ledIdx - LED index id.
 *      mode  - LED force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can get forced Led group mode.
 *      The force modes that can be set are as following:
 *      - LED_FORCE_BLINK,
 *      - LED_FORCE_OFF,
 *      - LED_FORCE_ON.
 *      For LED_OP_SERIAL the max led index is 31  
 *      For LED_OP_PARALLEL the max led index is 16  
 */
int32
rtk_led_modeForce_set(uint32 ledIdx, rtk_led_force_mode_t mode)
{
    rtdrv_ledCfg_t led_cfg;

    /* function body */
    osal_memset(&led_cfg, 0, sizeof(rtdrv_ledCfg_t));
    osal_memcpy(&led_cfg.ledIdx, &ledIdx, sizeof(uint32));
    osal_memcpy(&led_cfg.mode, &mode, sizeof(rtk_led_force_mode_t));
    SETSOCKOPT(RTDRV_LED_MODEFORCE_SET, &led_cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_led_modeForce_set */


/* Function Name:
 *      rtk_led_parallelEnable_get
 * Description:
 *      Get Led group enable status for parallel mode
 * Input:
 *      ledIdx - LED index id.
 * Output:
 *      pState  - LED parallel enable status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
int32
rtk_led_parallelEnable_get(uint32 ledIdx, rtk_enable_t *pState)
{
    rtdrv_ledCfg_t led_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&led_cfg.ledIdx, &ledIdx, sizeof(uint32));
    GETSOCKOPT(RTDRV_LED_PARALLELENABLE_GET, &led_cfg, rtdrv_ledCfg_t, 1);
    osal_memcpy(pState, &led_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_led_parallelEnable_get */

/* Function Name:
 *      rtk_led_parallelEnable_set
 * Description:
 *      Set Led group enable status for parallel mode
 * Input:
 *      ledIdx - LED index id.
 *      state  - LED parallel enable status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
int32
rtk_led_parallelEnable_set(uint32 ledIdx, rtk_enable_t state)
{
    rtdrv_ledCfg_t led_cfg;

    /* function body */
    osal_memset(&led_cfg, 0, sizeof(rtdrv_ledCfg_t));
    osal_memcpy(&led_cfg.ledIdx, &ledIdx, sizeof(uint32));
    osal_memcpy(&led_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_LED_PARALLELENABLE_SET, &led_cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_led_parallelEnable_set */

/* Function Name:
 *      rtk_led_ponAlarm_get
 * Description:
 *      Get PON alarm state
 * Input:
 *      None
 * Output:
 *      pState     - PON alarm state
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 */
int32
rtk_led_ponAlarm_get(rtk_enable_t *pState)
{
    rtdrv_ledCfg_t led_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&led_cfg.state, pState, sizeof(rtk_enable_t));
    GETSOCKOPT(RTDRV_LED_PONALARM_GET, &led_cfg, rtdrv_ledCfg_t, 1);
    osal_memcpy(pState, &led_cfg.state, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_led_ponAlarm_get */

/* Function Name:
 *      rtk_led_ponAlarm_set
 * Description:
 *      Get PON alarm state
 * Input:
 *      state     - PON alarm state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 */
int32
rtk_led_ponAlarm_set(rtk_enable_t state)
{
    rtdrv_ledCfg_t led_cfg;

    /* function body */
    osal_memset(&led_cfg,0x0,sizeof(rtdrv_ledCfg_t));
    osal_memcpy(&led_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_LED_PONALARM_SET, &led_cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_led_ponAlarm_set */


/* Function Name:
 *      rtk_led_ponWarning_get
 * Description:
 *      Get PON alarm state
 * Input:
 *      None
 * Output:
 *      pState     - PON alarm state
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 */
int32
rtk_led_ponWarning_get(rtk_enable_t *pState)
{
    rtdrv_ledCfg_t led_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&led_cfg.state, pState, sizeof(rtk_enable_t));
    GETSOCKOPT(RTDRV_LED_PONWARNING_GET, &led_cfg, rtdrv_ledCfg_t, 1);
    osal_memcpy(pState, &led_cfg.state, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_led_ponWarning_get */

/* Function Name:
 *      rtk_led_ponWarning_set
 * Description:
 *      Get PON warning state
 * Input:
 *      state     - PON alarm state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 */
int32
rtk_led_ponWarning_set(rtk_enable_t state)
{
    rtdrv_ledCfg_t led_cfg;

    /* function body */
    osal_memset(&led_cfg,0x0,sizeof(rtdrv_ledCfg_t));
    osal_memcpy(&led_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_LED_PONWARNING_SET, &led_cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_led_ponWarning_set */

/* Function Name:
 *      rtk_led_pon_port_set
 * Description:
 *      Set pon port with whihc type, force mode, and force blink rate 
 * Input:
 *      type   			- LED type
 *      forceMode  		- LED force mode
 * 		forceBlinkRate	- LED force blink rate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
int32 
rtk_led_pon_port_set(rtk_led_type_t type, rtk_led_force_mode_t forceMode,  rtk_led_blink_rate_t forceBlinkRate, rtk_led_config_t *pConfig)
{
    rtdrv_ledCfg_t led_cfg;
	
    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&led_cfg,0x0,sizeof(rtdrv_ledCfg_t));
    osal_memcpy(&led_cfg.type, &type, sizeof(rtk_led_type_t));
	osal_memcpy(&led_cfg.mode, &forceMode, sizeof(rtk_led_force_mode_t));
	osal_memcpy(&led_cfg.blinkRate, &forceBlinkRate, sizeof(rtk_led_blink_rate_t));
    osal_memcpy(&led_cfg.config, pConfig, sizeof(rtk_led_config_t));
    SETSOCKOPT(RTDRV_LED_PONPORT_SET, &led_cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_led_pon_port_set */

/* Function Name:
 *      rtk_led_parallelMode_get
 * Description:
 *      Get Led group active polarity for parallel mode
 * Input:
 *      ledIdx - LED index id.
 * Output:
 *      pActive - LED parallel polarity.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
int32
rtk_led_parallelMode_get(uint32 ledIdx, rtk_led_active_t *pActive)
{
    rtdrv_ledCfg_t led_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pActive), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&led_cfg.ledIdx, &ledIdx, sizeof(uint32));
    osal_memcpy(&led_cfg.active, pActive, sizeof(rtk_led_active_t));
    GETSOCKOPT(RTDRV_LED_PARALLELMODE_GET, &led_cfg, rtdrv_ledCfg_t, 1);
    osal_memcpy(pActive, &led_cfg.active, sizeof(rtk_led_active_t));

    return RT_ERR_OK;
}   /* end of rtk_led_parallelMode_get */

/* Function Name:
 *      rtk_led_parallelMode_set
 * Description:
 *      Set Led group active polarity for parallel mode
 * Input:
 *      ledIdx - LED index id.
 *      active - LED parallel enable status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
int32
rtk_led_parallelMode_set(uint32 ledIdx, rtk_led_active_t active)
{
    rtdrv_ledCfg_t led_cfg;

    /* function body */
    osal_memset(&led_cfg, 0, sizeof(rtdrv_ledCfg_t));
    osal_memcpy(&led_cfg.ledIdx, &ledIdx, sizeof(uint32));
    osal_memcpy(&led_cfg.active, &active, sizeof(rtk_led_active_t));
    SETSOCKOPT(RTDRV_LED_PARALLELMODE_SET, &led_cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_led_parallelMode_set */

