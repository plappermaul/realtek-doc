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


#ifndef __RTK_LED_H__
#define __RTK_LED_H__


/*
 * Include Files
 */
#include <common/rt_type.h>


/*
 * Symbol Definition
 */

typedef enum rtk_led_operation_e
{
    LED_OP_PARALLEL=0,
    LED_OP_SERIAL,
    LED_OP_END,
}rtk_led_operation_t;


typedef enum rtk_led_active_e
{
    LED_ACTIVE_HIGH=0,
    LED_ACTIVE_LOW,
    LED_ACTIVE_END,
}rtk_led_active_t;

/* led type */
typedef enum rtk_led_type_e
{
    LED_TYPE_UTP0 = 0,
    LED_TYPE_UTP1,
    LED_TYPE_UTP2,
    LED_TYPE_UTP3,
    LED_TYPE_UTP4,
    LED_TYPE_UTP5,
    LED_TYPE_FIBER,
    LED_TYPE_FIBER0,
    LED_TYPE_FIBER1,
    LED_TYPE_PON,
    LED_TYPE_USB0,
    LED_TYPE_USB1,
    LED_TYPE_SATA,
    LED_TYPE_RGMII,
    LED_TYPE_NONE,
    LED_TYPE_END
} rtk_led_type_t;

typedef enum rtk_led_blinkGroup_e
{
    LED_BLINK_GROUP_PORT=0,
    LED_BLINK_GROUP_USB,
    LED_BLINK_GROUP_SATA,
    LED_BLINK_GROUP_FORCE_MODE,
    LED_BLINK_GROUP_FORCE_MODE2,    
    LED_BLINK_GROUP_END
}rtk_led_blinkGroup_t;




typedef enum rtk_led_blink_rate_e
{
    LED_BLINKRATE_32MS=0,
    LED_BLINKRATE_64MS,
    LED_BLINKRATE_128MS,
    LED_BLINKRATE_256MS,
    LED_BLINKRATE_512MS,
    LED_BLINKRATE_1024MS,
    LED_BLINKRATE_48MS,
    LED_BLINKRATE_96MS,
    LED_BLINKRATE_END,
}rtk_led_blink_rate_t;


typedef enum rtk_led_enable_config_e
{
    LED_CONFIG_COL=0,
    LED_CONFIG_TX_ACT,
    LED_CONFIG_RX_ACT,
    LED_CONFIG_SPD10ACT,
    LED_CONFIG_SPD100ACT,
    LED_CONFIG_SPD500ACT,
    LED_CONFIG_SPD1000ACT,
    LED_CONFIG_DUP,
    LED_CONFIG_SPD10,
    LED_CONFIG_SPD100,
    LED_CONFIG_SPD500,
    LED_CONFIG_SPD1000,
    LED_CONFIG_FORCE_MODE,
    LED_CONFIG_PON_LINK,
    LED_CONFIG_SOC_LINK_ACK,
    LED_CONFIG_PON_ALARM,
    LED_CONFIG_PON_WARNING,
    LED_CONFIG_END
}rtk_led_enable_config_t;




typedef struct rtk_led_config_s
{
    rtk_enable_t  ledEnable[LED_CONFIG_END];
}rtk_led_config_t;



typedef enum rtk_led_force_mode_e
{
    LED_FORCE_BLINK = 0,
    LED_FORCE_OFF,
    LED_FORCE_ON,
    LED_FORCE_BLINK_GROUP2,
    LED_FORCE_END
}rtk_led_force_mode_t;



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
extern int32
rtk_led_init(void);

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
extern int32 
rtk_led_operation_get(rtk_led_operation_t *pMode);


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
extern int32 
rtk_led_operation_set(rtk_led_operation_t mode);


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
extern int32 
rtk_led_serialMode_get(rtk_led_active_t *pActive);

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
extern int32 
rtk_led_serialMode_set(rtk_led_active_t active);

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
extern int32 
rtk_led_blinkRate_get(rtk_led_blinkGroup_t group, rtk_led_blink_rate_t *pBlinkRate);

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
extern int32 
rtk_led_blinkRate_set(rtk_led_blinkGroup_t group, rtk_led_blink_rate_t blinkRate);


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
extern int32 
rtk_led_config_set(uint32 ledIdx, rtk_led_type_t type, rtk_led_config_t *pConfig);

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
extern int32 
rtk_led_config_get(uint32 ledIdx, rtk_led_type_t *pType, rtk_led_config_t *pConfig);

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
extern int32
rtk_led_modeForce_get(uint32 ledIdx, rtk_led_force_mode_t *pMode);

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
extern int32
rtk_led_modeForce_set(uint32 ledIdx, rtk_led_force_mode_t mode);


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
extern int32
rtk_led_parallelEnable_get(uint32 ledIdx, rtk_enable_t *pState);

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
extern int32
rtk_led_parallelEnable_set(uint32 ledIdx, rtk_enable_t state);



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
extern int32
rtk_led_ponAlarm_get(rtk_enable_t *pState);

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
extern int32
rtk_led_ponAlarm_set(rtk_enable_t state);


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
extern int32
rtk_led_ponWarning_get(rtk_enable_t *pState);

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
extern int32
rtk_led_ponWarning_set(rtk_enable_t state);

/* Function Name:
 *      rtk_led_pon_port_set
 * Description:
 *      Deprecated
 * Input:
 *      type   			- LED type
 *      forceMode  		- LED force mode
 * 		forceBlinkRate	- LED force blink rate
 *      pConfig  		- LED configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 * Note:
 */
int32 
rtk_led_pon_port_set(rtk_led_type_t type, rtk_led_force_mode_t forceMode,  rtk_led_blink_rate_t forceBlinkRate, rtk_led_config_t *pConfig);

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
extern int32
rtk_led_parallelMode_get(uint32 ledIdx, rtk_led_active_t *pActive);

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
extern int32
rtk_led_parallelMode_set(uint32 ledIdx, rtk_led_active_t active);

#endif /* __RTK_LED_H__ */
