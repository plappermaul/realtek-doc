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
 * $Revision: 51563 $
 * $Date: 2014-09-23 10:56:10 +0800 (Tue, 23 Sep 2014) $
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
#include <rtk/pon_led.h>


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
int32
rtk_pon_led_SpecType_set(rtk_pon_led_spec_type_t type)
{
    rtdrv_ponLed_t pon_led;

    /* function body */
    osal_memset(&pon_led,0x0,sizeof(rtdrv_ponLed_t));
    pon_led.specType = type;
    SETSOCKOPT(RTDRV_PONLED_SPECTYPE_SET, &pon_led, rtdrv_ponLed_t, 1);

    return RT_ERR_OK;
} /* end of rtk_pon_led_SpecType_set */


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
int32
rtk_pon_led_status_set(rtk_pon_led_pon_mode_t ponMode, uint32 ponStatus)
{
    rtdrv_ponLed_t pon_led;

    /* function body */
    osal_memset(&pon_led,0x0,sizeof(rtdrv_ponLed_t));
    pon_led.ponMode = ponMode;
    pon_led.ponStatus = ponStatus;
    SETSOCKOPT(RTDRV_PONLED_STATUS_SET, &pon_led, rtdrv_ponLed_t, 1);

    return RT_ERR_OK;
} /* end of rtk_pon_led_status_set */

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
int32
rtk_pon_led_group_set(rtk_pon_led_group_t ponLedGroup, uint32 ledGroupIdx)
{
    rtdrv_ponLed_t pon_led;

    /* function body */
    osal_memset(&pon_led,0x0,sizeof(rtdrv_ponLed_t));
    pon_led.ponLedGroup = ponLedGroup;
	pon_led.ledGroupIdx = ledGroupIdx;
    SETSOCKOPT(RTDRV_PONLED_GROUP_SET, &pon_led, rtdrv_ponLed_t, 1);

    return RT_ERR_OK;
} /* end of rtk_pon_led_group_set */

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
int32
rtk_pon_led_group_get(rtk_pon_led_group_t ponLedGroup, uint32 *pLedGroupIdx)
{
    rtdrv_ponLed_t pon_led;

    /* function body */
    osal_memset(&pon_led,0x0,sizeof(rtdrv_ponLed_t));
    pon_led.ponLedGroup = ponLedGroup;
	*pLedGroupIdx = pon_led.ledGroupIdx;
    GETSOCKOPT(RTDRV_PONLED_GROUP_GET, &pon_led, rtdrv_ponLed_t, 1);

    return RT_ERR_OK;
}
