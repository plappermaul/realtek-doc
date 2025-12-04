/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 51563 $
 * $Date: 2014-09-23 10:56:10 +0800 (Tue, 23 Sep 2014) $
 *
 * Purpose : Definition those public PON LED APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) PON LED settings
 */


#ifndef __RTK_PON_LED_H__
#define __RTK_PON_LED_H__


/*
 * Include Files
 */
#include <common/rt_type.h>


/*
 * Symbol Definition
 */
typedef enum rtk_pon_led_spec_type_e
{
    PON_LED_SPEC_TYPE_00 = 0, /* RTK Generic */
    PON_LED_SPEC_TYPE_01,
    PON_LED_SPEC_TYPE_02,
    PON_LED_SPEC_TYPE_03,
    PON_LED_SPEC_TYPE_04,
    PON_LED_SPEC_TYPE_05,
    PON_LED_SPEC_TYPE_06,
    PON_LED_SPEC_TYPE_07,
    PON_LED_SPEC_TYPE_08,
    PON_LED_SPEC_TYPE_09,
    PON_LED_SPEC_TYPE_10,
    PON_LED_SPEC_TYPE_END
}rtk_pon_led_spec_type_t;

typedef enum rtk_pon_led_pon_mode_e
{
    PON_LED_PON_MODE_GPON = 0,
    PON_LED_PON_MODE_EPON,
    PON_LED_PON_MODE_OTHERS,
    PON_LED_PON_MODE_END
}rtk_pon_led_pon_mode_t;

typedef enum rtk_pon_led_group_e
{
	PON_LED_GROUP_PON_STATUS_IDX,
	PON_LED_GROUP_SYSTEM_IDX,
	PON_LED_GROUP_LOS_IDX,
	PON_LED_GROUP_END
}rtk_pon_led_group_t;

typedef enum rtk_pon_led_pon_state_e
{
    PON_LED_STATE_UNKNOWN = 0,      /* Unknown State: Get Status fail */
    PON_LED_STATE_O1,               /* Initial State */
    PON_LED_STATE_O2,               /* Standby State */
    PON_LED_STATE_O3,               /* Serial Number State */
    PON_LED_STATE_O4,               /* Ranging State */
    PON_LED_STATE_O5,               /* Operation State */
    PON_LED_STATE_O6,               /* POPUP State */
    PON_LED_STATE_O7,               /* Emergency Stop State */
    PON_LED_STATE_AUTH_OK,          /* LOID authentication OK */
    PON_LED_STATE_AUTH_NG,          /* LOID authentication NG */
    PON_LED_STATE_POWER_NORMAL,     /* RX optic power normal */
    PON_LED_STATE_POWER_HIGH,       /* RX optic power too high */
    PON_LED_STATE_POWER_LOW,        /* RX optic power too low */
    PON_LED_STATE_TIMER_50MS,       /* Timer every 50 ms */
    PON_LED_STATE_EPONMAC_UP,       /* EPON link up */
    PON_LED_STATE_EPONMAC_DOWN,     /* EPON link down */
    PON_LED_STATE_EPONOAM_UP,       /* EPON OAM discovery compete */
    PON_LED_STATE_EPONOAM_TRYING,   /* EPON OAM discovery on-going */
    PON_LED_STATE_EPONOAM_DOWN,     /* EPON OAM disconnected */
    PON_LED_STATE_LOS_OCCURRED,     /* LOS event occurred */
    PON_LED_STATE_LOS_CLEARED,      /* LOS event cleared */

    PON_LED_FORCE_OFF,              //cxy 2016-8-22: set pon led off
    PON_LED_FORCE_ON,               //cxy 2016-8-22: set pon led on
    PON_LED_STATE_RESTORE,          //cxy 2016-8-22: restore pon led state

    PON_LED_STATE_END
}rtk_pon_led_pon_state_t;

typedef enum rtk_pon_led_force_mode_e
{
	PON_LED_FORCE_MODE_NONE,
	PON_LED_FORCE_MODE_ON,
	PON_LED_FORCE_MODE_OFF,
	PON_LED_FORCE_MODE_END
}rtk_pon_led_force_mode_t;

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_pon_led_SpecType_set
 * Description:
 *      Set pon port spec type
 * Input:
 *      type   			- PON port LED spec type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
extern int32
rtk_pon_led_SpecType_set(rtk_pon_led_spec_type_t type);

/* Function Name:
 *      rtk_pon_led_status_set
 * Description:
 *      Set GPON/EPON pon port LED by pon status
 * Input:
 *      ponMode   			- GPON or EPON
 *      ponStatus           - GPON/EPON PON status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
extern int32
rtk_pon_led_status_set(rtk_pon_led_pon_mode_t ponMode, rtk_pon_led_pon_state_t ponStatus);

/* Function Name:
 *      rtk_pon_led_group_set
 * Description:
 *      Set PON LED group using which LED group
 * Input:
 *      ponLedGroup         - PON LED group
 *      ledGroupIdx         - LED group index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
extern int32
rtk_pon_led_group_set(rtk_pon_led_group_t ponLedGroup, uint32 ledGroupIdx);

/* Function Name:
 *      rtk_pon_led_group_get
 * Description:
 *      Get PON LED group using which LED group
 * Input:
 *      ponLedGroup         - PON LED group
 *      pLedGroupIdx        - LED group index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
extern int32
rtk_pon_led_group_get(rtk_pon_led_group_t ponLedGroup, uint32 *pLedGroupIdx);

#endif /* __RTK_PON_LED_H__ */


